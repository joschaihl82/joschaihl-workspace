// main.cpp
// Qt6 single-file fullscreen visualizer + editor
// - Left: GLSL editor (vertex + fragment)
// - Right: Preview: fullscreen quad visualizer (no cube)
// - Audio input via Qt6 QAudioSource / QMediaDevices
// - Hardware mic gain control via ALSA helper (setHardwareMicVolumePercent)
// - Fixed sample buffer size (never changes)
// - Simple RMS beat detection -> white flash on beat (u_kick)
// - 1D audio texture updated from ring buffer and sampled in fragment shader
// Build: link Qt6::Widgets Qt6::OpenGLWidgets Qt6::Multimedia and libasound

#include <QApplication>
#include <QSplitter>
#include <QPlainTextEdit>
#include <QSurfaceFormat>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>
#include <QDateTime>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QDebug>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>

// Qt6 Multimedia
#include <QMediaDevices>
#include <QAudioDevice>
#include <QAudioSource>
#include <QAudioFormat>
#include <QIODevice>

#include <cmath>
#include <vector>
#include <random>
#include <cstdint>
#include <cstdarg>
#include <cstdlib>
#include <string>
#include <algorithm>

// ALSA for hardware mic control (only used by setHardwareMicVolumePercent)
#include <alsa/asoundlib.h>

// --------------------------- Configuration ---------------------------
constexpr unsigned DEFAULT_SR = 48000u;
constexpr int SAMPLE_BUFFER_FRAMES = 128;      // fixed frames per read
constexpr int SAMPLE_BYTES_PER_FRAME = 2;    // S16_LE mono -> 2 bytes per frame
constexpr int SAMPLE_BUFFER_BYTES = SAMPLE_BUFFER_FRAMES
		* SAMPLE_BYTES_PER_FRAME;
constexpr int RING_SECONDS = 4;                // ring buffer seconds
constexpr int POLL_MS = 10;                    // GUI refresh interval
constexpr int FLASH_MS = 120;                  // white flash duration (ms)
constexpr int KICK_DEBOUNCE_MS = 200;          // min time between kicks
constexpr float BEAT_THRESHOLD = 2.5f;  // beat if RMS > running_avg * threshold
constexpr float RMS_ALPHA = 0.08f;             // smoothing for running average
constexpr int AUDIO_TEX_SIZE = 2048;        // 1D texture size for audio samples

// Hardware gain control parameters
constexpr float TARGET_HEADROOM_DB = -9.0f;   // target headroom (dBFS)
constexpr int HW_ADJUST_INTERVAL_MS = 200; // minimum ms between hardware adjustments
constexpr int HW_PERCENT_MIN = 0;
constexpr int HW_PERCENT_MAX = 150;

// --------------------------- Helpers -------------------------
static inline void fatalError(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	std::vfprintf(stderr, fmt, ap);
	std::fprintf(stderr, "\n");
	va_end(ap);
	qCritical().noquote() << QString("FATAL: ") + QString::fromUtf8(fmt);
	std::abort();
}

// --------------------------- ALSA helper: set hardware microphone capture volume (Linux) ---
// Candidate control names to prefer
static const std::vector<std::string> kCandidateCaptureNames = { "Capture",
		"Mic", "Mic Capture", "Capture Volume", "Input Source" };

// Sets hardware capture volume (percent). Uses ALSA mixer simple elements.
// Returns true on success, false otherwise.
bool setHardwareMicVolumePercent(int percent) {
	if (percent < HW_PERCENT_MIN)
		percent = HW_PERCENT_MIN;
	if (percent > HW_PERCENT_MAX)
		percent = HW_PERCENT_MAX;

	int err = 0;
	snd_mixer_t *handle = nullptr;

	// open mixer
	if ((err = snd_mixer_open(&handle, 0)) < 0) {
		fprintf(stderr, "ALSA: snd_mixer_open error: %s\n", snd_strerror(err));
		return false;
	}

	// attach to default card; fallback to hw:0 if needed
	if ((err = snd_mixer_attach(handle, "default")) < 0) {
		snd_mixer_close(handle);
		if ((err = snd_mixer_open(&handle, 0)) < 0) {
			fprintf(stderr, "ALSA: snd_mixer_open fallback error: %s\n",
					snd_strerror(err));
			return false;
		}
		if ((err = snd_mixer_attach(handle, "hw:0")) < 0) {
			fprintf(stderr, "ALSA: snd_mixer_attach error: %s\n",
					snd_strerror(err));
			snd_mixer_close(handle);
			return false;
		}
	}

	if ((err = snd_mixer_selem_register(handle, nullptr, nullptr)) < 0) {
		fprintf(stderr, "ALSA: snd_mixer_selem_register error: %s\n",
				snd_strerror(err));
		snd_mixer_close(handle);
		return false;
	}

	if ((err = snd_mixer_load(handle)) < 0) {
		fprintf(stderr, "ALSA: snd_mixer_load error: %s\n", snd_strerror(err));
		snd_mixer_close(handle);
		return false;
	}

	snd_mixer_elem_t *elem = nullptr;
	bool success = false;

	// iterate over simple elements and try to find a capture control
	for (elem = snd_mixer_first_elem(handle); elem;
			elem = snd_mixer_elem_next(elem)) {
		if (!snd_mixer_selem_is_active(elem))
			continue;

		const char *ename = snd_mixer_selem_get_name(elem);
		if (!ename)
			continue;
		std::string name(ename);

		// check if element supports capture
		if (!snd_mixer_selem_has_capture_volume(elem))
			continue;

		// match candidate names (case-insensitive)
		std::string lname = name;
		std::transform(lname.begin(), lname.end(), lname.begin(), ::tolower);
		bool matched = false;
		for (const auto &cand : kCandidateCaptureNames) {
			std::string lcand = cand;
			std::transform(lcand.begin(), lcand.end(), lcand.begin(),
					::tolower);
			if (lname.find(lcand) != std::string::npos) {
				matched = true;
				break;
			}
		}
		// Use only matched controls to avoid touching unrelated mixer elements
		if (!matched)
			continue;

		// get capture volume range
		long minv = 0, maxv = 0;
		if (snd_mixer_selem_get_capture_volume_range(elem, &minv, &maxv) < 0) {
			continue;
		}

		// convert percent to ALSA volume scale (linear mapping)
		double linear = percent / 100.0;
		if (linear < 0.0)
			linear = 0.0;
		if (linear > 1.5)
			linear = 1.5;
		long target = static_cast<long>(minv + linear * (maxv - minv) + 0.5);

		// set for all channels; try per-channel then fallback to all
		bool set_ok = true;
		for (int c = 0; c <= SND_MIXER_SCHN_LAST; ++c) {
			snd_mixer_selem_channel_id_t ch_id =
					static_cast<snd_mixer_selem_channel_id_t>(c);
			if (snd_mixer_selem_has_capture_channel(elem, ch_id)) {
				if ((err = snd_mixer_selem_set_capture_volume(elem, ch_id,
						target)) < 0) {
					set_ok = false;
					break;
				}
			}
		}
		if (!set_ok) {
			if ((err = snd_mixer_selem_set_capture_volume_all(elem, target))
					< 0) {
				// failed to set this element
				continue;
			}
		}

		// success for this element
		success = true;
		break;
	}

	snd_mixer_close(handle);

	if (!success) {
		fprintf(stderr,
				"ALSA: No suitable capture control found or set failed.\n");
	}
	return success;
}

// --------------------------- Ring Buffer (mono float) -----------------
static std::vector<float> g_ring;
static size_t g_ring_cap = 0;
static size_t g_ring_pos = 0;
static QMutex g_ring_mutex;

static void push_to_ring_from_s16(const int16_t *in, size_t n) {
	QMutexLocker locker(&g_ring_mutex);
	if (g_ring_cap == 0)
		return;
	for (size_t i = 0; i < n; ++i) {
		g_ring[g_ring_pos] = in[i] / 32768.0f;
		g_ring_pos = (g_ring_pos + 1) % g_ring_cap;
	}
}

static void read_ring_into_float_array(float *out, int outSize) {
	QMutexLocker locker(&g_ring_mutex);
	if (g_ring_cap == 0) {
		for (int i = 0; i < outSize; ++i)
			out[i] = 0.0f;
		return;
	}
	// read most recent outSize samples (or repeat if ring smaller)
	size_t start = (g_ring_pos + g_ring_cap - (size_t) outSize) % g_ring_cap;
	for (int i = 0; i < outSize; ++i)
		out[i] = g_ring[(start + i) % g_ring_cap];
}

static float compute_rms_s16(const int16_t *buf, size_t n) {
	if (n == 0)
		return 0.0f;
	double s = 0.0;
	for (size_t i = 0; i < n; ++i) {
		double v = buf[i] / 32768.0;
		s += v * v;
	}
	return static_cast<float>(std::sqrt(s / static_cast<double>(n)));
}

// --------------------------- GLSL Highlighter (small) ------------------
class GLSLHighlighter: public QSyntaxHighlighter {
public:
	GLSLHighlighter(QTextDocument *parent = nullptr) :
			QSyntaxHighlighter(parent) {
		QTextCharFormat kw;
		kw.setForeground(QColor("#569CD6"));
		kw.setFontWeight(QFont::Bold);
		QStringList keywords = { "uniform", "in", "out", "varying", "attribute",
				"void", "float", "int", "vec2", "vec3", "vec4", "mat3", "mat4",
				"if", "else", "return", "for", "while", "sampler1D", "texture" };
		for (const QString &k : keywords)
			rules.append(
					{ QRegularExpression(
							"\\b" + QRegularExpression::escape(k) + "\\b"), kw });
		QTextCharFormat com;
		com.setForeground(QColor("#6A9955"));
		rules.append( { QRegularExpression("//[^\\n]*"), com });
	}
protected:
	void highlightBlock(const QString &text) override {
		for (const auto &r : rules) {
			QRegularExpressionMatchIterator it = r.pattern.globalMatch(text);
			while (it.hasNext()) {
				QRegularExpressionMatch m = it.next();
				setFormat(m.capturedStart(), m.capturedLength(), r.format);
			}
		}
	}
private:
	struct Rule {
		QRegularExpression pattern;
		QTextCharFormat format;
	};
	QVector<Rule> rules;
};

// --------------------------- GL Widget: Fullscreen Quad Visualizer ------------------
class GLWaveWidget: public QOpenGLWidget, protected QOpenGLFunctions {
Q_OBJECT
public:
	explicit GLWaveWidget(unsigned sampleRate, QWidget *parent = nullptr) :
			QOpenGLWidget(parent), rate(sampleRate) {
		setFocusPolicy(Qt::StrongFocus);
		prog = 0;
		loc_u_time = loc_u_resolution = loc_u_vscale = loc_u_gain_display =
				loc_u_hue = loc_u_kick = -1;
		flashActive = false;
		beatCount = 0;
		randSeed = float(
				std::uniform_real_distribution<float>(0.0f, 1.0f)(rng));
		flashTimer.setSingleShot(true);
		connect(&flashTimer, &QTimer::timeout, this, [this]() {
			flashActive = false;
			update();
		});
	}

	// compile vertex+fragment from editor combined text
	bool setShadersFromEditor(const QString &combinedEditorText,
			QString &errorOut) {
		if (!isValid() || !context() || !context()->isValid()) {
			errorOut = "GL context not ready yet.";
			return false;
		}
		makeCurrent();

		// split editor into vertex/fragment
		QString lower = combinedEditorText.toLower();
		int idxV = lower.indexOf("#shader vertex");
		int idxF = lower.indexOf("#shader fragment");
		if (idxV == -1 || idxF == -1 || idxF < idxV) {
			errorOut =
					"Editor must contain '#shader vertex' then '#shader fragment'.";
			doneCurrent();
			return false;
		}
		int vStart = idxV + QString("#shader vertex").length();
		int fStart = idxF + QString("#shader fragment").length();
		QString vsrc = combinedEditorText.mid(vStart, idxF - vStart);
		QString fsrc = combinedEditorText.mid(fStart);
		if (vsrc.startsWith('\n'))
			vsrc = vsrc.mid(1);
		if (fsrc.startsWith('\n'))
			fsrc = fsrc.mid(1);

		// compile vertex
		QByteArray vba = vsrc.toUtf8();
		const char *vptr = vba.constData();
		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &vptr, nullptr);
		glCompileShader(vs);
		GLint ok = 0;
		glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
		if (!ok) {
			GLint len = 0;
			glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &len);
			std::string log(len > 0 ? len : 1, '\0');
			glGetShaderInfoLog(vs, len, nullptr, &log[0]);
			errorOut = QString("Vertex compile error:\n%1").arg(
					QString::fromStdString(log));
			glDeleteShader(vs);
			doneCurrent();
			return false;
		}

		// compile fragment
		QByteArray fba = fsrc.toUtf8();
		const char *fptr = fba.constData();
		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &fptr, nullptr);
		glCompileShader(fs);
		glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
		if (!ok) {
			GLint len = 0;
			glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &len);
			std::string log(len > 0 ? len : 1, '\0');
			glGetShaderInfoLog(fs, len, nullptr, &log[0]);
			errorOut = QString("Fragment compile error:\n%1").arg(
					QString::fromStdString(log));
			glDeleteShader(vs);
			glDeleteShader(fs);
			doneCurrent();
			return false;
		}

		// link
		GLuint newProg = glCreateProgram();
		glAttachShader(newProg, vs);
		glAttachShader(newProg, fs);
		glBindAttribLocation(newProg, 0, "a_pos");
		glLinkProgram(newProg);
		glGetProgramiv(newProg, GL_LINK_STATUS, &ok);
		if (!ok) {
			GLint len = 0;
			glGetProgramiv(newProg, GL_INFO_LOG_LENGTH, &len);
			std::string log(len > 0 ? len : 1, '\0');
			glGetProgramInfoLog(newProg, len, nullptr, &log[0]);
			errorOut = QString("Link error:\n%1").arg(
					QString::fromStdString(log));
			glDeleteProgram(newProg);
			glDeleteShader(vs);
			glDeleteShader(fs);
			doneCurrent();
			return false;
		}

		// swap program
		if (prog)
			glDeleteProgram(prog);
		prog = newProg;
		glDeleteShader(vs);
		glDeleteShader(fs);

		// cache uniform locations
		loc_u_time = glGetUniformLocation(prog, "u_time");
		loc_u_resolution = glGetUniformLocation(prog, "u_resolution");
		loc_u_vscale = glGetUniformLocation(prog, "u_vscale");
		loc_u_gain_display = glGetUniformLocation(prog, "u_gain_display");
		loc_u_hue = glGetUniformLocation(prog, "u_hue");
		loc_u_kick = glGetUniformLocation(prog, "u_kick");
		loc_u_samples = glGetUniformLocation(prog, "u_samples");

		doneCurrent();
		return true;
	}

	// update 1D audio texture from float array (must be called from GUI thread)
	void updateAudioTexture(const float *data, int n) {
		if (!isValid() || !context() || !context()->isValid())
			return;
		makeCurrent();
		if (texSamples == 0) {
			// create texture if missing
			glGenTextures(1, &texSamples);
			glBindTexture(GL_TEXTURE_1D, texSamples);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, AUDIO_TEX_SIZE, 0, GL_RED,
					GL_FLOAT, nullptr);
		} else {
			glBindTexture(GL_TEXTURE_1D, texSamples);
		}
		// upload (if n != AUDIO_TEX_SIZE, we resample/fit by simple copy or wrap)
		if (n == AUDIO_TEX_SIZE) {
			glTexSubImage1D(GL_TEXTURE_1D, 0, 0, AUDIO_TEX_SIZE, GL_RED,
					GL_FLOAT, data);
		} else {
			// simple fill: copy as many as possible, wrap remainder
			std::vector<float> tmp(AUDIO_TEX_SIZE);
			for (int i = 0; i < AUDIO_TEX_SIZE; ++i)
				tmp[i] = data[i % n];
			glTexSubImage1D(GL_TEXTURE_1D, 0, 0, AUDIO_TEX_SIZE, GL_RED,
					GL_FLOAT, tmp.data());
		}
		doneCurrent();
	}

	// trigger white flash
	void triggerFlash() {
		flashActive = true;
		flashTimer.start(FLASH_MS);
		update();
	}

protected:
	void initializeGL() override {
		initializeOpenGLFunctions();
		// create fullscreen quad VBO
		float quad[] = { -1.f, -1.f, 1.f, -1.f, -1.f, 1.f, 1.f, 1.f };
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

		// create audio texture placeholder
		glGenTextures(1, &texSamples);
		glBindTexture(GL_TEXTURE_1D, texSamples);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		std::vector<float> zeros(AUDIO_TEX_SIZE, 0.0f);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, AUDIO_TEX_SIZE, 0, GL_RED,
				GL_FLOAT, zeros.data());
	}

	void paintGL() override {
		if (flashActive) {
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			return;
		}

		glClearColor(0.06f, 0.06f, 0.06f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		if (!prog) {
			// magenta fallback
			glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			return;
		}

		glUseProgram(prog);

		float t = float(QDateTime::currentMSecsSinceEpoch() % 0x7fffffff)
				* 0.001f;
		if (loc_u_time >= 0)
			glUniform1f(loc_u_time, t);
		if (loc_u_resolution >= 0)
			glUniform2f(loc_u_resolution, float(width()), float(height()));
		if (loc_u_vscale >= 0)
			glUniform1f(loc_u_vscale, 0.4f);
		if (loc_u_gain_display >= 0)
			glUniform1f(loc_u_gain_display, 0.8f);
		if (loc_u_hue >= 0)
			glUniform1f(loc_u_hue, 0.12f);
		if (loc_u_kick >= 0)
			glUniform1i(loc_u_kick, lastBeat ? 1 : 0);

		// bind audio texture to unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_1D, texSamples);
		if (loc_u_samples >= 0)
			glUniform1i(loc_u_samples, 0);

		// draw fullscreen quad
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glDisableVertexAttribArray(0);

		glUseProgram(0);
	}

	void resizeGL(int w, int h) override {
		glViewport(0, 0, w, h);
	}

	void keyPressEvent(QKeyEvent *ev) override {
		if (ev->key() == Qt::Key_F) {
			emit requestToggleFullscreen();
			ev->accept();
			return;
		}
		if (ev->key() == Qt::Key_Escape) {
			emit requestExitFullscreen();
			ev->accept();
			return;
		}
		QOpenGLWidget::keyPressEvent(ev);
	}

signals:
	void requestToggleFullscreen();
	void requestExitFullscreen();

public:
	bool lastBeat = false;
	int beatCount = 0;

private:
	unsigned rate;
	GLuint prog = 0;
	GLuint vbo = 0;
	GLuint texSamples = 0;
	GLint loc_u_time = -1;
	GLint loc_u_resolution = -1;
	GLint loc_u_vscale = -1;
	GLint loc_u_gain_display = -1;
	GLint loc_u_hue = -1;
	GLint loc_u_kick = -1;
	GLint loc_u_samples = -1;
	bool flashActive;
	QTimer flashTimer;
	std::mt19937 rng { std::random_device { }() };
	float randSeed;
};

// --------------------------- Qt Audio input (QAudioSource / QMediaDevices) -----------------
class QtAudioInput: public QObject {
Q_OBJECT
public:
	// Constructor initializer order matches member declaration order below
	explicit QtAudioInput(unsigned sr, QObject *parent = nullptr) :
			QObject(parent),
			rate(sr),
			audioSource(nullptr),
			audioIO(nullptr),
			runningAvg(0.0f),
			runningAvgInit(false),
			lastKickMs(0),
			glw(nullptr),
			lastHwPercent(80),
			lastHwAdjustMs(0) {
		// initialize lastHwPercent to a reasonable default (80%)
	}

	~QtAudioInput() {
		stop();
	}

	bool start() {
		QAudioFormat fmt;
		fmt.setSampleRate(static_cast<int>(rate));
		fmt.setChannelCount(1);
		fmt.setSampleFormat(QAudioFormat::Int16);

		QAudioDevice device = QMediaDevices::defaultAudioInput();

		if (!device.isFormatSupported(fmt)) {
			fmt = device.preferredFormat();
			qWarning()
					<< "Requested audio format not supported; using device preferred format.";
		}

		audioSource = new QAudioSource(device, fmt, this);
		audioIO = audioSource->start();
		if (!audioIO) {
			qWarning() << "QAudioSource::start() failed";
			delete audioSource;
			audioSource = nullptr;
			return false;
		}

		connect(audioIO, &QIODevice::readyRead, this,
				&QtAudioInput::onReadyRead);
		return true;
	}

	void stop() {
		if (audioSource) {
			audioSource->stop();
			audioIO = nullptr;
			delete audioSource;
			audioSource = nullptr;
		}
	}

	void setTargetWidget(GLWaveWidget *w) {
		glw = w;
	}

private slots:
	void onReadyRead() {
		if (!audioIO)
			return;
		while (audioIO->bytesAvailable() >= SAMPLE_BUFFER_BYTES) {
			QByteArray chunk = audioIO->read(SAMPLE_BUFFER_BYTES);
			if (chunk.size() != SAMPLE_BUFFER_BYTES)
				break;
			const int16_t *samples =
					reinterpret_cast<const int16_t*>(chunk.constData());
			size_t frames = SAMPLE_BUFFER_FRAMES;

			// push into ring
			push_to_ring_from_s16(samples, frames);

			// compute RMS and beat detection
			float rms = compute_rms_s16(samples, frames);
			if (!runningAvgInit) {
				runningAvg = rms;
				runningAvgInit = true;
			}
			runningAvg = RMS_ALPHA * rms + (1.0f - RMS_ALPHA) * runningAvg;

			bool isBeat = false;
			quint32 now =
					static_cast<quint32>(QDateTime::currentMSecsSinceEpoch()
							& 0xffffffff);
			if (rms > runningAvg * BEAT_THRESHOLD
					&& (now - lastKickMs) > KICK_DEBOUNCE_MS) {
				isBeat = true;
				lastKickMs = now;
			}

			// hardware gain control: adjust ALSA capture volume to maintain target headroom
			adjustHardwareGainIfNeeded(rms);

			// update GL widget state and audio texture
			if (glw) {
				glw->lastBeat = isBeat;
				if (isBeat) {
					glw->beatCount++;
					glw->triggerFlash();
				}
				// prepare audio texture data from ring and upload
				std::vector<float> tex(AUDIO_TEX_SIZE);
				read_ring_into_float_array(tex.data(), AUDIO_TEX_SIZE);
				// update texture (safe: both in GUI thread)
				glw->updateAudioTexture(tex.data(), AUDIO_TEX_SIZE);
			}
		}
	}

private:
	unsigned rate;
	QAudioSource *audioSource;
	QIODevice *audioIO;
	float runningAvg;
	bool runningAvgInit;
	quint32 lastKickMs;
	GLWaveWidget *glw;
	int lastHwPercent;
	qint64 lastHwAdjustMs;

	// Convert RMS (linear) to dBFS
	static inline float rmsToDbFS(float rms) {
		const float eps = 1e-9f;
		return 20.0f * std::log10(std::max(rms, eps));
	}

	// Adjust hardware gain if needed (simple multiplicative control)
	void adjustHardwareGainIfNeeded(float rms) {
		qint64 nowMs = QDateTime::currentMSecsSinceEpoch();
		if ((nowMs - lastHwAdjustMs) < HW_ADJUST_INTERVAL_MS)
			return; // rate limit

		float currentDb = rmsToDbFS(rms); // negative values for typical audio
		float deltaDb = currentDb - TARGET_HEADROOM_DB; // positive => too loud

		// If within +/-1 dB, do nothing
		if (std::abs(deltaDb) < 1.0f) {
			return;
		}

		// multiplicative factor to apply to hardware gain (linear)
		// we want new_gain = old_gain * 10^(-deltaDb/20)
		float factor = std::pow(10.0f, -deltaDb / 20.0f);

		// compute new percent
		float newPercentF = static_cast<float>(lastHwPercent) * factor;
		int newPercent = static_cast<int>(std::round(newPercentF));

		// clamp
		if (newPercent < HW_PERCENT_MIN)
			newPercent = HW_PERCENT_MIN;
		if (newPercent > HW_PERCENT_MAX)
			newPercent = HW_PERCENT_MAX;

		// avoid tiny changes
		if (std::abs(newPercent - lastHwPercent) < 2) {
			// change too small, skip
			return;
		}

		// attempt to set hardware volume via ALSA helper
		bool ok = setHardwareMicVolumePercent(newPercent);
		if (ok) {
			lastHwPercent = newPercent;
			lastHwAdjustMs = nowMs;
			qDebug() << "Hardware mic volume adjusted to" << newPercent
					<< "% (rms dBFS =" << currentDb << "deltaDb=" << deltaDb
					<< ")";
		} else {
			qWarning() << "Failed to set hardware mic volume via ALSA";
		}
	}
};

// --------------------------- Default editor content (fullscreen quad + fragment) ------------------
// Vertex shader: fullscreen quad that provides v_uv
// Fragment shader: your provided shader adapted to GLSL 330 core (out vec4 fragColor)
static const char *DEFAULT_EDITOR =
		R"(#shader vertex
#version 330 core
layout(location = 0) in vec2 a_pos;
out vec2 v_uv;
void main() {
    v_uv = (a_pos + 1.0) * 0.5;
    gl_Position = vec4(a_pos, 0.0, 1.0);
}

#shader fragment
#version 330 core
in vec2 v_uv;
uniform sampler1D u_samples;
uniform float u_vscale;
uniform float u_gain_display;
uniform float u_hue;
uniform int u_kick;
out vec4 fragColor;

vec3 hsv2rgb(float h, float s, float v) {
  float c = v * s;
  float hh = h * 6.0;
  float x = c * (1.0 - abs(mod(hh, 2.0) - 1.0));
  vec3 col;
  if (hh < 1.0) col = vec3(c, x, 0.0);
  else if (hh < 2.0) col = vec3(x, c, 0.0);
  else if (hh < 3.0) col = vec3(0.0, c, x);
  else if (hh < 4.0) col = vec3(0.0, x, c);
  else if (hh < 5.0) col = vec3(x, 0.0, c);
  else col = vec3(c, 0.0, x);
  return col + vec3(v - c);
}

void main() {
  if (u_kick == 1) { fragColor = vec4(1.0); return; }
  float s = texture(u_samples, v_uv.x).r;
  float dist = abs(v_uv.y - (0.5 - s * u_vscale));
  float alpha = smoothstep(4.0/600.0, 0.0, dist);
  float c_alpha = smoothstep(0.002, 0.0, abs(v_uv.y - 0.5));
  vec3 wcol = hsv2rgb(u_hue, 1.0, 1.0);
  vec3 col = mix(wcol * 0.5, wcol, alpha);
  float f_alpha = max(alpha, c_alpha);
  if (v_uv.x < 0.15 && v_uv.y < 0.03 && (v_uv.x / 0.15) < u_gain_display) {
    col = vec3(0.0, 1.0, 0.0);
    f_alpha = 1.0;
  }
  fragColor = vec4(col, f_alpha);
}
)";

// --------------------------- Main ---------------------------------------
int main(int argc, char **argv) {
	QApplication a(argc, argv);

	// prepare ring buffer
	g_ring_cap = static_cast<size_t>(DEFAULT_SR) * RING_SECONDS;
	g_ring.assign(g_ring_cap, 0.0f);
	g_ring_pos = 0;

	// UI: splitter left editor / right preview
	QSplitter *split = new QSplitter(Qt::Horizontal);
	QPlainTextEdit *editor = new QPlainTextEdit(
			QString::fromUtf8(DEFAULT_EDITOR));
	GLSLHighlighter *hl = new GLSLHighlighter(editor->document());
	Q_UNUSED(hl);

	QWidget *rightContainer = new QWidget;
	QVBoxLayout *vlay = new QVBoxLayout(rightContainer);
	vlay->setContentsMargins(0, 0, 0, 0);
	GLWaveWidget *glw = new GLWaveWidget(DEFAULT_SR);
	vlay->addWidget(glw);

	split->addWidget(editor);
	split->addWidget(rightContainer);
	split->setStretchFactor(0, 1);
	split->setStretchFactor(1, 1);

	// Qt audio input
	QtAudioInput audioIn(DEFAULT_SR);
	audioIn.setTargetWidget(glw);
	bool audioStarted = audioIn.start();
	if (!audioStarted)
		qWarning() << "Audio input could not be started via Qt Multimedia.";

	// Optionally set an initial hardware mic volume (safe default)
	if (!setHardwareMicVolumePercent(80)) {
		qWarning() << "Initial hardware mic volume set failed (ALSA).";
	}

	// Debounced shader compile
	QTimer *debounce = new QTimer(&a);
	debounce->setSingleShot(true);
	debounce->setInterval(250);

	auto compileNow = [&]() {
		QString src = editor->toPlainText();
		QString log;
		if (!glw->setShadersFromEditor(src, log)) {
			qWarning() << "Shader compile/link error:\n" << log;
		} else {
			qDebug() << "Shaders compiled successfully.";
		}
		glw->update();
	};

	QObject::connect(editor, &QPlainTextEdit::textChanged, [&]() {
		debounce->start();
	});
	QObject::connect(debounce, &QTimer::timeout, [&]() {
		if (!glw->isValid() || !glw->context() || !glw->context()->isValid()) {
			QTimer::singleShot(150, [&]() {
				debounce->start();
			});
			return;
		}
		compileNow();
	});

	QObject::connect(glw, &GLWaveWidget::requestToggleFullscreen, [&]() {
		if (rightContainer->isFullScreen())
			rightContainer->showNormal();
		else
			rightContainer->showFullScreen();
	});
	QObject::connect(glw, &GLWaveWidget::requestExitFullscreen, [&]() {
		if (rightContainer->isFullScreen())
			rightContainer->showNormal();
	});

	// GUI update timer
	QTimer guiTimer;
	guiTimer.setInterval(POLL_MS);
	QObject::connect(&guiTimer, &QTimer::timeout, [&]() {
		glw->update();
	});
	guiTimer.start();

	// show window and force equal halves
	split->resize(1200, 720);
	split->show();
	QTimer::singleShot(0, [&]() {
		QSize s = split->size();
		int half = s.width() / 2;
		QList<int> sizes;
		sizes << half << (s.width() - half);
		split->setSizes(sizes);
	});

	// initial compile after GL ready
	QTimer::singleShot(300, [&]() {
		debounce->start();
	});

	int ret = a.exec();

	audioIn.stop();
	return ret;
}

#include "main.moc"
