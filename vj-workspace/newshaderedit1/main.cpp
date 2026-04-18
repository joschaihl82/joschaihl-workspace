// main.cpp
// Realtime Audio Visualizer mit vollständig automatischem Limiter‑Optimizer
// Keine Persistenz, keine Toolbar/Parametersteuerung im Fenster.
// Optimizer‑Parameter werden auf stdout ausgegeben, jeweils mit '\n' getrennt.
// Build:
// g++ -std=gnu++20 main.cpp -o audioviz_auto `pkg-config --cflags --libs Qt6Widgets Qt6Gui Qt6Core Qt6Multimedia Qt6OpenGL` -lasound

#include <QtWidgets>
#include <QtGui>
#include <QtCore>
#include <QtMultimedia>
#include <QtOpenGL>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QAudioFormat>
#include <QAudioSource>
#include <QIODevice>
#include <cmath>
#include <atomic>
#include <vector>
#include <mutex>
#include <chrono>
#include <deque>
#include <algorithm>
#include <random>
#include <alsa/asoundlib.h>

// ----------------------------- Konfiguration -------------------------------------
static constexpr int DEFAULT_SAMPLE_RATE = 48000;
static constexpr int DEFAULT_CHANNELS = 1;
static constexpr int DEFAULT_RING_SECONDS = 4;
static constexpr int DEFAULT_TEXTURE_SIZE = 4096;
static constexpr int DEFAULT_FRAMES_PER_READ = 512;
static constexpr int PROGRESSBAR_WIDTH = 300;
static constexpr int g_hwAdjustIntervalMs = 200;

// ----------------------------- Hilfsfunktionen -----------------------------------
static inline float s16_to_float(qint16 v) {
	return std::max(-1.0f, std::min(1.0f, float(v) / 32768.0f));
}
static inline float dbfs_from_linear(float x) {
	return 20.0f * std::log10(std::max(1e-12f, x));
}
static inline float linear_from_dbfs(float db) {
	return std::pow(10.0f, db / 20.0f);
}
static float computeDisplayGainFromHeadroom(float headroom) {
	float db = (1.0f - headroom) * 36.0f - 24.0f;
	return linear_from_dbfs(db);
}
static inline float expAlphaFromMs(float tcMs, float dtMs) {
	if (tcMs <= 0.0f)
		return 1.0f;
	return 1.0f - std::exp(-dtMs / tcMs);
}

// ----------------------------- RingBuffer ----------------------------------------
class RingBuffer {
public:
	RingBuffer() :
			sampleRate(DEFAULT_SAMPLE_RATE), channels(DEFAULT_CHANNELS) {
		resize(DEFAULT_RING_SECONDS);
	}
	void configure(int sr, int ch, int seconds) {
		QMutexLocker lk(&mutex);
		sampleRate = sr;
		channels = ch;
		resize(seconds);
		writePos = 0;
		filled = false;
	}
	void resize(int seconds) {
		QMutexLocker lk(&mutex);
		capacity = std::max(1, sampleRate * seconds * channels);
		buffer.assign(capacity, 0.0f);
		writePos = 0;
		filled = false;
	}
	void pushSamples(const float *data, int frames) {
		QMutexLocker lk(&mutex);
		int samples = frames * channels;
		for (int i = 0; i < samples; ++i) {
			buffer[writePos] = data[i];
			writePos = (writePos + 1) % capacity;
			if (writePos == 0)
				filled = true;
		}
	}
	void copyLastNSamples(float *out, int outSamples) {
		QMutexLocker lk(&mutex);
		if (capacity == 0)
			return;
		if (channels == 1) {
			int start = (writePos - outSamples + capacity) % capacity;
			for (int i = 0; i < outSamples; ++i)
				out[i] = buffer[(start + i) % capacity];
		} else {
			int start = (writePos - outSamples * channels + capacity)
					% capacity;
			for (int i = 0; i < outSamples; ++i)
				out[i] = buffer[(start + i * channels) % capacity];
		}
	}
	int getCapacity() const {
		return capacity;
	}
private:
	mutable QMutex mutex;
	std::vector<float> buffer;
	int capacity = 0;
	int writePos = 0;
	bool filled = false;
	int sampleRate;
	int channels;
};

// ----------------------------- ALSA Mixer helper ----------------------------------
class AlsaMixer {
public:
	AlsaMixer() :
			handle(nullptr) {
		if (snd_ctl_open(&handle, "default", 0) < 0) {
			handle = nullptr;
		}
	}
	~AlsaMixer() {
		if (handle)
			snd_ctl_close(handle);
	}
	bool setCapturePercent(long percent) {
		if (!handle)
			return false;
		snd_mixer_t *mixer = nullptr;
		if (snd_mixer_open(&mixer, 0) < 0)
			return false;
		if (snd_mixer_attach(mixer, "default") < 0) {
			snd_mixer_close(mixer);
			return false;
		}
		if (snd_mixer_selem_register(mixer, nullptr, nullptr) < 0) {
			snd_mixer_close(mixer);
			return false;
		}
		if (snd_mixer_load(mixer) < 0) {
			snd_mixer_close(mixer);
			return false;
		}
		bool success = false;
		snd_mixer_elem_t *elem;
		for (elem = snd_mixer_first_elem(mixer); elem; elem =
				snd_mixer_elem_next(elem)) {
			if (!snd_mixer_selem_is_active(elem))
				continue;
			long minv, maxv;
			if (snd_mixer_selem_has_capture_volume(elem)) {
				snd_mixer_selem_get_capture_volume_range(elem, &minv, &maxv);
				long val = minv + (percent * (maxv - minv)) / 100;
				if (snd_mixer_selem_set_capture_volume_all(elem, val) == 0)
					success = true;
			}
		}
		snd_mixer_close(mixer);
		return success;
	}
private:
	snd_ctl_t *handle;
};

// ----------------------------- AudioIODevice -------------------------------------
class AudioIODevice: public QIODevice {
Q_OBJECT
public:
	AudioIODevice(RingBuffer *rb, QObject *parent = nullptr) :
			QIODevice(parent), ring(rb) {
	}
	bool open(OpenMode mode) override {
		return QIODevice::open(mode);
	}
	void close() override {
		QIODevice::close();
	}
	qint64 readData(char *data, qint64 maxlen) override {
		Q_UNUSED(data);
		Q_UNUSED(maxlen);
		return 0;
	}
	qint64 writeData(const char *data, qint64 len) override {
		int16_t *samples = (int16_t*) data;
		int frames = len / (sizeof(int16_t) * DEFAULT_CHANNELS);
		std::vector<float> tmp(frames * DEFAULT_CHANNELS);
		for (int i = 0; i < frames * DEFAULT_CHANNELS; ++i)
			tmp[i] = s16_to_float(samples[i]);
		ring->pushSamples(tmp.data(), frames);
		float sumSq = 0.0f;
		float peak = 0.0f;
		for (int i = 0; i < frames * DEFAULT_CHANNELS; ++i) {
			float v = tmp[i];
			sumSq += v * v;
			peak = std::max(peak, std::abs(v));
		}
		float rms = std::sqrt(sumSq / (frames * DEFAULT_CHANNELS));
		emit blockAnalyzed(rms, peak);
		return len;
	}
signals:
	void blockAnalyzed(float rms, float peak);
private:
	RingBuffer *ring;
};

// ----------------------------- ShaderEditor (minimal) -----------------------------
class ShaderEditor: public QWidget {
Q_OBJECT
public:
	ShaderEditor(QWidget *parent = nullptr) :
			QWidget(parent) {
		QVBoxLayout *l = new QVBoxLayout(this);
		editor = new QTextEdit(this);
		editor->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
		l->addWidget(editor);
		compileTimer = new QTimer(this);
		compileTimer->setSingleShot(true);
		compileTimer->setInterval(400);
		connect(compileTimer, &QTimer::timeout, this,
				&ShaderEditor::compileRequested);
		connect(editor, &QTextEdit::textChanged, this, [this]() {
			compileTimer->start();
		});
		editor->setPlainText(defaultShader());
	}
	QString vertexSource() const {
		return extractShader(editor->toPlainText(), "vertex");
	}
	QString fragmentSource() const {
		return extractShader(editor->toPlainText(), "fragment");
	}
signals:
	void compileRequested();
private:
	QTextEdit *editor;
	QTimer *compileTimer;
	QString extractShader(const QString &src, const QString &type) const {
		QStringList lines = src.split('\n');
		bool in = false;
		QStringList out;
		for (const QString &ln : lines) {
			if (ln.trimmed().startsWith("#shader")) {
				in = ln.contains(type);
				continue;
			}
			if (in)
				out << ln;
		}
		if (out.isEmpty())
			return (type == "vertex") ? defaultVertex() : defaultFragment();
		return out.join('\n');
	}
	QString defaultShader() const {
		return QString("#shader vertex\n") + defaultVertex()
				+ "\n#shader fragment\n" + defaultFragment();
	}
	QString defaultVertex() const {
		return R"(#version 330 core
layout(location = 0) in vec2 aPos;
out vec2 vUV;
void main() { vUV = (aPos + 1.0) * 0.5; gl_Position = vec4(aPos, 0.0, 1.0); })";
	}
	QString defaultFragment() const {
		return R"(#version 330 core
in vec2 vUV; out vec4 FragColor;
uniform sampler1D u_samples; uniform vec2 u_resolution; uniform float u_time;
uniform float u_vscale; uniform float u_gain_display; uniform float u_hue; uniform float u_kick;
void main() {
    float x = vUV.x;
    float s = texture(u_samples, x).r * u_gain_display;
    float val = smoothstep(0.0, 1.0, s * u_vscale - (1.0 - vUV.y));
    vec3 col = vec3(0.5 + 0.5 * sin(u_hue + s * 6.2831), 0.3, 0.7);
    vec3 outc = mix(vec3(0.0), col, val);
    outc += u_kick * vec3(1.0);
    FragColor = vec4(outc, 1.0);
})";
	}
};

// ----------------------------- GLWaveWidget --------------------------------------
class GLWaveWidget: public QOpenGLWidget, protected QOpenGLFunctions {
Q_OBJECT
public:
	GLWaveWidget(QWidget *parent = nullptr) :
			QOpenGLWidget(parent), textureSize(DEFAULT_TEXTURE_SIZE) {
		setMinimumSize(320, 200);
		startTimer(16);
	}
	void setShaderSources(const QString &vert, const QString &frag) {
		QMutexLocker lk(&shaderMutex);
		vertexSrc = vert;
		fragmentSrc = frag;
		needsCompile = true;
	}
	void setSamples(const std::vector<float> &s, int texSize) {
		QMutexLocker lk(&dataMutex);
		samples = s;
		textureSize = texSize;
		needUpload = true;
	}
	void setUniforms(float vscale, float gain_display, float hue, float kick) {
		vscale_ = vscale;
		gain_display_ = gain_display;
		hue_ = hue;
		kick_ = kick;
	}
protected:
	void initializeGL() override {
		initializeOpenGLFunctions();
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		float verts[] = { -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_1D, tex);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_1D, 0);
		program = new QOpenGLShaderProgram();
		QMutexLocker lk(&shaderMutex);
		compileProgramLocked();
	}
	void resizeGL(int w, int h) override {
		glViewport(0, 0, w, h);
	}
	void paintGL() override {
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		{
			QMutexLocker lk(&shaderMutex);
			if (needsCompile)
				compileProgramLocked();
		}
		if (!program->isLinked())
			return;
		program->bind();
		{
			QMutexLocker lk(&dataMutex);
			if (needUpload && !samples.empty()) {
				glBindTexture(GL_TEXTURE_1D, tex);
				glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, textureSize, 0, GL_RED,
						GL_FLOAT, samples.data());
				glBindTexture(GL_TEXTURE_1D, 0);
				needUpload = false;
			}
		}
		program->setUniformValue("u_time",
				float(QTime::currentTime().msecsSinceStartOfDay() / 1000.0));
		program->setUniformValue("u_resolution", QVector2D(width(), height()));
		program->setUniformValue("u_vscale", vscale_);
		program->setUniformValue("u_gain_display", gain_display_);
		program->setUniformValue("u_hue", hue_);
		program->setUniformValue("u_kick", kick_);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_1D, tex);
		program->setUniformValue("u_samples", 0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		int posLoc = program->attributeLocation("aPos");
		program->enableAttributeArray(posLoc);
		glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		program->disableAttributeArray(posLoc);
		program->release();
	}
	void timerEvent(QTimerEvent *ev) override {
		Q_UNUSED(ev);
		kick_ *= 0.92f;
		update();
	}
private:
	QOpenGLShaderProgram *program = nullptr;
	GLuint vbo = 0, tex = 0;
	int textureSize = DEFAULT_TEXTURE_SIZE;
	std::vector<float> samples;
	QMutex dataMutex;
	QString vertexSrc, fragmentSrc;
	QMutex shaderMutex;
	bool needsCompile = false, needUpload = false;
	float vscale_ = 1.0f, gain_display_ = 1.0f, hue_ = 0.0f, kick_ = 0.0f;
	void compileProgramLocked() {
		if (!program)
			return;
		program->removeAllShaders();
		program->release();
		program->removeAllShaders();
		program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexSrc);
		if (!program->addShaderFromSourceCode(QOpenGLShader::Fragment,
				fragmentSrc))
			qWarning() << "Frag compile:" << program->log();
		if (!program->link())
			qWarning() << "Link error:" << program->log();
		needsCompile = false;
		needUpload = true;
	}
};

// ----------------------------- LimiterVisualizer ---------------------------------
class LimiterVisualizer: public QWidget {
Q_OBJECT
public:
	LimiterVisualizer(QWidget *parent = nullptr) :
			QWidget(parent) {
		setFixedSize(220, 40);
	}
	void pushValue(float gainDb) {
		if (history.size() >= maxSamples)
			history.pop_front();
		history.push_back(gainDb);
		update();
	}
	void setEngaged(bool e) {
		engaged = e;
		update();
	}
protected:
	void paintEvent(QPaintEvent*) override {
		QPainter p(this);
		p.fillRect(rect(), Qt::transparent);
		p.setPen(Qt::NoPen);
		p.setBrush(QColor(30, 30, 30, 220));
		p.drawRoundedRect(rect(), 4, 4);
		QRect ledRect(4, 4, 10, 10);
		p.setBrush(engaged ? Qt::red : Qt::green);
		p.drawEllipse(ledRect);
		QRect graphRect(18, 4, width() - 22, height() - 8);
		p.setPen(QPen(QColor(200, 200, 200, 180), 1));
		p.drawRect(graphRect);
		if (history.empty())
			return;
		float minDb = -24.0f, maxDb = 0.0f;
		int n = history.size();
		QPolygon poly;
		for (int i = 0; i < n; ++i) {
			float v = std::clamp(history[i], minDb, maxDb);
			float x = graphRect.left()
					+ (float(i) / float(std::max(1, n - 1)))
							* graphRect.width();
			float t = (v - minDb) / (maxDb - minDb);
			float y = graphRect.bottom() - t * graphRect.height();
			poly << QPoint(int(x), int(y));
		}
		p.setPen(QPen(QColor(100, 220, 255), 2));
		p.drawPolyline(poly);
	}
private:
	std::deque<float> history;
	const int maxSamples = 64;
	bool engaged = false;
};

// ----------------------------- Optimizer (Adaptive SPSA) --------------------------
class LimiterOptimizer: public QObject {
Q_OBJECT
public:
	LimiterOptimizer(RingBuffer *rb, QObject *parent = nullptr) :
			QObject(parent), ring(rb), rng(std::random_device { }()), uni(0.0f,
					1.0f) {
		P_target_db = -1.0f;
		R_target_db = -18.0f;
		w_p = 1.0f;
		w_r = 0.2f;
		w_c = 120.0f;
		w_s = 0.05f;
		a_base = 0.8f;
		c_base = 0.6f;
		A = 50.0f;
		alpha = 0.602f;
		gamma = 0.101f;
		iter = 1;
		theta_headroom = 0.5f;
		theta_attackMs = 30.0f;
		theta_releaseMs = 300.0f;
		theta_hystOn = 0.5f;
		theta_hystOff = 0.3f;
		smoothed_headroom = theta_headroom;
		smoothed_attackMs = theta_attackMs;
		smoothed_releaseMs = theta_releaseMs;
		smoothed_hystOn = theta_hystOn;
		smoothed_hystOff = theta_hystOff;
		optTimer = new QTimer(this);
		optTimer->setInterval(180);
		connect(optTimer, &QTimer::timeout, this, &LimiterOptimizer::step);
		lastLoss = 1e9f;
		noImprovementCount = 0;
	}
	void start() {
		optTimer->start();
	}
	void stop() {
		optTimer->stop();
	}
	float headroom() const {
		return smoothed_headroom;
	}
	int attackMs() const {
		return int(smoothed_attackMs);
	}
	int releaseMs() const {
		return int(smoothed_releaseMs);
	}
	float hystOn() const {
		return smoothed_hystOn;
	}
	float hystOff() const {
		return smoothed_hystOff;
	}
signals:
	void paramsUpdated();
private slots:
	void step() {
		int N = std::min(4096, ring->getCapacity());
		if (N <= 128)
			return;
		std::vector<float> buf(N);
		ring->copyLastNSamples(buf.data(), N);
		float peak = 0.0f, sumSq = 0.0f;
		int clipCount = 0;
		for (int i = 0; i < N; ++i) {
			float v = std::abs(buf[i]);
			peak = std::max(peak, v);
			sumSq += buf[i] * buf[i];
			if (v > 0.999f)
				++clipCount;
		}
		float rms = std::sqrt(sumSq / float(N));
		float peakDb = dbfs_from_linear(peak);
		float rmsDb = dbfs_from_linear(rms);
		float ck = c_base / std::pow(float(iter + A), gamma);
		auto rndSign = [&]() {
			return (uni(rng) > 0.5f) ? 1.0f : -1.0f;
		};
		float d1 = rndSign(), d2 = rndSign(), d3 = rndSign(), d4 = rndSign(),
				d5 = rndSign();
		float scaleA = 10.0f, scaleR = 50.0f;
		float h_p = theta_headroom + ck * d1;
		float a_p = theta_attackMs + ck * d2 * scaleA;
		float r_p = theta_releaseMs + ck * d3 * scaleR;
		float ho_p = theta_hystOn + ck * d4;
		float hf_p = theta_hystOff + ck * d5;
		float h_m = theta_headroom - ck * d1;
		float a_m = theta_attackMs - ck * d2 * scaleA;
		float r_m = theta_releaseMs - ck * d3 * scaleR;
		float ho_m = theta_hystOn - ck * d4;
		float hf_m = theta_hystOff - ck * d5;
		auto clamp01 = [](float x) {
			return std::clamp(x, 0.0f, 1.0f);
		};
		h_p = clamp01(h_p);
		h_m = clamp01(h_m);
		ho_p = std::clamp(ho_p, 0.0f, 12.0f);
		ho_m = std::clamp(ho_m, 0.0f, 12.0f);
		hf_p = std::clamp(hf_p, 0.0f, 12.0f);
		hf_m = std::clamp(hf_m, 0.0f, 12.0f);
		a_p = std::clamp(a_p, 1.0f, 1000.0f);
		a_m = std::clamp(a_m, 1.0f, 1000.0f);
		r_p = std::clamp(r_p, 10.0f, 10000.0f);
		r_m = std::clamp(r_m, 10.0f, 10000.0f);
		float loss_p = simulateLoss(buf.data(), N, h_p, a_p, r_p, ho_p, hf_p);
		float loss_m = simulateLoss(buf.data(), N, h_m, a_m, r_m, ho_m, hf_m);
		if (loss_p < lastLoss || loss_m < lastLoss) {
			noImprovementCount = 0;
			c_base = std::max(0.02f, c_base * 0.995f);
			a_base = std::max(0.02f, a_base * 0.995f);
		} else {
			noImprovementCount++;
			if (noImprovementCount > 10) {
				c_base = std::min(2.0f, c_base * 1.02f);
				a_base = std::min(2.0f, a_base * 1.02f);
				noImprovementCount = 0;
			}
		}
		float ak = a_base / std::pow(float(iter + A), alpha);
		float g1 = (loss_p - loss_m) / (2.0f * ck * d1);
		float g2 = (loss_p - loss_m) / (2.0f * ck * d2 * scaleA);
		float g3 = (loss_p - loss_m) / (2.0f * ck * d3 * scaleR);
		float g4 = (loss_p - loss_m) / (2.0f * ck * d4);
		float g5 = (loss_p - loss_m) / (2.0f * ck * d5);
		theta_headroom -= ak * g1;
		theta_attackMs -= ak * g2;
		theta_releaseMs -= ak * g3;
		theta_hystOn -= ak * g4;
		theta_hystOff -= ak * g5;
		theta_headroom = clamp01(theta_headroom);
		theta_attackMs = std::clamp(theta_attackMs, 1.0f, 1000.0f);
		theta_releaseMs = std::clamp(theta_releaseMs, 10.0f, 10000.0f);
		theta_hystOn = std::clamp(theta_hystOn, 0.0f, 12.0f);
		theta_hystOff = std::clamp(theta_hystOff, 0.0f, 12.0f);
		float smoothAlpha = 0.08f;
		smoothed_headroom = smoothed_headroom
				+ smoothAlpha * (theta_headroom - smoothed_headroom);
		smoothed_attackMs = smoothed_attackMs
				+ smoothAlpha * (theta_attackMs - smoothed_attackMs);
		smoothed_releaseMs = smoothed_releaseMs
				+ smoothAlpha * (theta_releaseMs - smoothed_releaseMs);
		smoothed_hystOn = smoothed_hystOn
				+ smoothAlpha * (theta_hystOn - smoothed_hystOn);
		smoothed_hystOff = smoothed_hystOff
				+ smoothAlpha * (theta_hystOff - smoothed_hystOff);
		float bestLoss = std::min(loss_p, loss_m);
		lastLoss =
				(lastLoss < 1e8f) ?
						(0.995f * lastLoss + 0.005f * bestLoss) : bestLoss;
		++iter;
		emit paramsUpdated();
	}
private:
	RingBuffer *ring;
	QTimer *optTimer;
	float P_target_db, R_target_db, w_p, w_r, w_c, w_s;
	float a_base, c_base, A, alpha, gamma;
	int iter;
	float theta_headroom, theta_attackMs, theta_releaseMs, theta_hystOn,
			theta_hystOff;
	float smoothed_headroom, smoothed_attackMs, smoothed_releaseMs,
			smoothed_hystOn, smoothed_hystOff;
	float lastLoss;
	int noImprovementCount;
	std::mt19937 rng;
	std::uniform_real_distribution<float> uni;
	float simulateLoss(const float *samples, int N, float headroom,
			float attackMs, float releaseMs, float hystOn, float hystOff) {
		float peak = 0.0f, sumSq = 0.0f;
		int clipCount = 0;
		for (int i = 0; i < N; ++i) {
			float v = std::abs(samples[i]);
			peak = std::max(peak, v);
			sumSq += samples[i] * samples[i];
			if (v > 0.999f)
				++clipCount;
		}
		float rms = std::sqrt(sumSq / float(N));
		float peakDb = dbfs_from_linear(peak);
		float rmsDb = dbfs_from_linear(rms);
		float ceilingDb = -6.0f;
		float desiredAttenDb =
				(peakDb > ceilingDb) ? (ceilingDb - peakDb) : 0.0f;
		float clipPenalty = float(clipCount) / float(N);
		float stabilityPenalty = 0.0f;
		int win = std::max(16, N / 16);
		float prevRms = 0.0f;
		int count = 0;
		for (int i = 0; i + win < N; i += win) {
			float s = 0.0f;
			for (int j = 0; j < win; ++j)
				s += samples[i + j] * samples[i + j];
			float r = std::sqrt(s / float(win));
			if (count > 0)
				stabilityPenalty += std::abs(r - prevRms);
			prevRms = r;
			++count;
		}
		if (count > 0)
			stabilityPenalty /= float(count);
		float peakDiff = peakDb - P_target_db;
		float rmsDiff = rmsDb - R_target_db;
		float loss = w_p * peakDiff * peakDiff + w_r * rmsDiff * rmsDiff
				+ w_c * clipPenalty * 100.0f + w_s * stabilityPenalty * 10.0f;
		float reg = 0.001f
				* (std::abs(headroom - 0.5f) + attackMs / 1000.0f
						+ releaseMs / 1000.0f + hystOn + hystOff);
		loss += reg;
		return loss;
	}
};

// ----------------------------- MainWindow ----------------------------------------
class MainWindow: public QMainWindow {
Q_OBJECT
public:
	MainWindow() {
		setWindowTitle("Auto-Optimizing Audio Visualizer (no UI params)");
		QWidget *central = new QWidget(this);
		QVBoxLayout *mainLayout = new QVBoxLayout(central);

		// Split: shader editor | preview
		QSplitter *split = new QSplitter(Qt::Horizontal, this);
		shaderEditor = new ShaderEditor(this);
		split->addWidget(shaderEditor);
		glWidget = new GLWaveWidget(this);
		split->addWidget(glWidget);
		split->setStretchFactor(0, 1);
		split->setStretchFactor(1, 1);
		mainLayout->addWidget(split, 3);

		// Signals area (top input, intermediate, output)
		QWidget *signalsWidget = new QWidget(this);
		QVBoxLayout *signalsLayout = new QVBoxLayout(signalsWidget);
		signalsLayout->setContentsMargins(6, 6, 6, 6);

		addSignalRow(signalsLayout, "Eingangssignal (Peak)", &inputProgress,
				&inputLabel);
		addSignalRow(signalsLayout, "RMS (Eingang)", &rmsProgress, &rmsLabel);
		addSignalRow(signalsLayout, "Peak dBFS (Eingang)", &peakDbProgress,
				&peakDbLabel);
		addSignalRow(signalsLayout, "Optimizer Desired Attenuation (dB)",
				&desiredAttenProgress, &desiredAttenLabel);
		addSignalRow(signalsLayout, "Limiter Engaged (0/1)", &engagedProgress,
				&engagedLabel);
		addSignalRow(signalsLayout, "Hardware Capture Percent (HW %)",
				&hwProgress, &hwLabel);
		addSignalRow(signalsLayout, "Ausgangssignal (geschätzt Peak)",
				&outputProgress, &outputLabel);

		mainLayout->addWidget(signalsWidget, 1);

		logView = new QTextEdit(this);
		logView->setReadOnly(true);
		mainLayout->addWidget(logView, 1);

		setCentralWidget(central);

		// ring buffer & audio
		ring = new RingBuffer();
		ring->configure(DEFAULT_SAMPLE_RATE, DEFAULT_CHANNELS,
				DEFAULT_RING_SECONDS);
		audioDevice = new AudioIODevice(ring, this);
		connect(audioDevice, &AudioIODevice::blockAnalyzed, this,
				&MainWindow::onBlockAnalyzed);
		QAudioFormat fmt;
		fmt.setSampleRate(DEFAULT_SAMPLE_RATE);
		fmt.setChannelCount(DEFAULT_CHANNELS);
		fmt.setSampleFormat(QAudioFormat::Int16);
		audioSource = new QAudioSource(fmt, this);

		mixer = new AlsaMixer();

		// timers
		textureTimer = new QTimer(this);
		textureTimer->setInterval(30);
		connect(textureTimer, &QTimer::timeout, this,
				&MainWindow::uploadTextureFromRing);
		textureTimer->start();
		limiterTimer = new QTimer(this);
		limiterTimer->setInterval(30);
		connect(limiterTimer, &QTimer::timeout, this,
				&MainWindow::limiterPeriodic);
		limiterTimer->start();
		uiTimer = new QTimer(this);
		uiTimer->setInterval(80);
		connect(uiTimer, &QTimer::timeout, this, &MainWindow::updateUI);
		uiTimer->start();

		// optimizer
		optimizer = new LimiterOptimizer(ring, this);
		connect(optimizer, &LimiterOptimizer::paramsUpdated, this,
				&MainWindow::onOptimizerParamsUpdated);
		optimizer->start();

		connect(shaderEditor, &ShaderEditor::compileRequested, this,
				&MainWindow::compileShaderDebounced);
		compileShaderDebounced();

		startAudio();

		currentHwPercent = 100;
		smoothedHwPercent = 100.0f;
		preLimiterPercent = -1;
	}

	~MainWindow() {
		stopAudio();
	}

private:
	void addSignalRow(QVBoxLayout *layout, const QString &name,
			QProgressBar **barOut, QLabel **labelOut) {
		QWidget *row = new QWidget(this);
		QHBoxLayout *h = new QHBoxLayout(row);
		QLabel *nameLabel = new QLabel(name, this);
		nameLabel->setMinimumWidth(260);
		h->addWidget(nameLabel);
		QProgressBar *pb = new QProgressBar(this);
		pb->setRange(0, 1000000);
		pb->setFixedWidth(PROGRESSBAR_WIDTH);
		h->addWidget(pb);
		QLabel *val = new QLabel("0.0000", this);
		val->setMinimumWidth(90);
		h->addWidget(val);
		layout->addWidget(row);
		*barOut = pb;
		*labelOut = val;
	}

	// members
	ShaderEditor *shaderEditor;
	GLWaveWidget *glWidget;
	QTextEdit *logView;

	QProgressBar *inputProgress, *rmsProgress, *peakDbProgress,
			*desiredAttenProgress, *engagedProgress, *hwProgress,
			*outputProgress;
	QLabel *inputLabel, *rmsLabel, *peakDbLabel, *desiredAttenLabel,
			*engagedLabel, *hwLabel, *outputLabel;

	QAudioSource *audioSource;
	AudioIODevice *audioDevice;
	RingBuffer *ring;
	AlsaMixer *mixer;

	QTimer *textureTimer, *limiterTimer, *uiTimer;

	LimiterOptimizer *optimizer;

	std::atomic<float> lastRms { 0.0f };
	std::atomic<float> lastPeak { 0.0f };
	QMutex limiterMutex;
	float measuredPeak = 0.0f;
	float measuredPeakDb = -120.0f;
	qint64 lastMeasuredMs = 0;
	bool limiterEngaged = false;
	qint64 engageTimeMs = 0;
	qint64 releaseStartMs = 0;
	int currentHwPercent = 100;
	int preLimiterPercent = -1;
	qint64 lastLimiterAdjustMs = 0;
	float smoothedHwPercent = 100.0f;

	// optimizer-applied params
	float headroomValue = 0.5f;
	int attackMs = 30;
	int releaseMs = 300;
	float hystOn = 0.5f;
	float hystOff = 0.3f;

	int textureSize = DEFAULT_TEXTURE_SIZE;

	void startAudio() {
		if (audioSource->state() == QAudio::ActiveState)
			return;
		audioDevice->open(QIODevice::WriteOnly);
		audioSource->start(audioDevice);
	}
	void stopAudio() {
		if (audioSource->state() != QAudio::StoppedState)
			audioSource->stop();
		audioDevice->close();
	}

private slots:
	void onBlockAnalyzed(float rms, float peak) {
		lastRms.store(rms);
		lastPeak.store(peak);
		if (peak > 0.999f) {
			qint64 now = QDateTime::currentMSecsSinceEpoch();
			if (now - lastLimiterAdjustMs > g_hwAdjustIntervalMs) {
				int safeVal = std::max(10, currentHwPercent / 2);
				if (mixer->setCapturePercent(safeVal)) {
					currentHwPercent = safeVal;
					smoothedHwPercent = safeVal;
					lastLimiterAdjustMs = now;
					logView->append(
							QString(
									"[%1] Failsafe: clipping detected, HW%% -> %2").arg(
									QTime::currentTime().toString()).arg(
									currentHwPercent));
				}
			}
		}
		QMutexLocker lk(&limiterMutex);
		measuredPeak = peak;
		measuredPeakDb = dbfs_from_linear(peak);
		lastMeasuredMs = QDateTime::currentMSecsSinceEpoch();
	}

	void uploadTextureFromRing() {
		int texSize = textureSize;
		std::vector<float> tmp(texSize, 0.0f);
		ring->copyLastNSamples(tmp.data(), texSize);
		float gain = computeDisplayGainFromHeadroom(headroomValue);
		for (int i = 0; i < texSize; ++i)
			tmp[i] *= gain;
		glWidget->setSamples(tmp, texSize);
		glWidget->setUniforms(1.0f, gain, 0.0f, 0.0f);
	}

	void compileShaderDebounced() {
		QString vert = shaderEditor->vertexSource();
		QString frag = shaderEditor->fragmentSource();
		glWidget->setShaderSources(vert, frag);
		logView->append(
				QString("[%1] Shader compile requested").arg(
						QTime::currentTime().toString()));
	}

	void onOptimizerParamsUpdated() {
		headroomValue = optimizer->headroom();
		attackMs = optimizer->attackMs();
		releaseMs = optimizer->releaseMs();
		hystOn = optimizer->hystOn();
		hystOff = optimizer->hystOff();

		// Ausgabe der Parameter auf stdout, jeweils mit '\n' getrennt (nur Werte, eine Zeile pro Parameter)
		// Reihenfolge: headroom, attackMs, releaseMs, hystOn, hystOff
		// Format: numeric, headroom with 4 decimals, attack/release ints, hyst with 4 decimals
		printf("%.4f\n%d\n%d\n%.4f\n%.4f\n", headroomValue, attackMs, releaseMs,
				hystOn, hystOff);
		fflush(stdout);

		// Log im UI (sparsam)
		static float lastHead = -1.0f;
		if (std::abs(lastHead - headroomValue) > 0.005f) {
			logView->append(
					QString(
							"[%1] Optimizer updated (headroom=%2 attack=%3ms release=%4ms hystOn=%5 hystOff=%6)").arg(
							QTime::currentTime().toString()).arg(headroomValue,
							0, 'f', 4).arg(attackMs).arg(releaseMs).arg(hystOn,
							0, 'f', 4).arg(hystOff, 0, 'f', 4));
			lastHead = headroomValue;
		}
	}

	void limiterPeriodic() {
		qint64 now = QDateTime::currentMSecsSinceEpoch();
		float peakDbLocal;
		{
			QMutexLocker lk(&limiterMutex);
			if (now - lastMeasuredMs > 500)
				return;
			peakDbLocal = measuredPeakDb;
		}

		float engageThreshold = -6.0f + hystOn;
		float releaseThreshold = -6.0f - hystOff;

		if (!limiterEngaged) {
			if (peakDbLocal > engageThreshold) {
				limiterEngaged = true;
				engageTimeMs = now;
				if (preLimiterPercent < 0)
					preLimiterPercent = currentHwPercent;
				logView->append(
						QString("[%1] Limiter ENGAGE (peak %2 dB > %3 dB)").arg(
								QTime::currentTime().toString()).arg(
								peakDbLocal, 0, 'f', 2).arg(engageThreshold, 0,
								'f', 2));
			}
		} else {
			if (peakDbLocal < releaseThreshold) {
				if (releaseStartMs == 0)
					releaseStartMs = now;
				if (now - releaseStartMs > 150 + int(hystOff * 50)) {
					limiterEngaged = false;
					releaseStartMs = 0;
					logView->append(
							QString("[%1] Limiter RELEASE (peak %2 dB < %3 dB)").arg(
									QTime::currentTime().toString()).arg(
									peakDbLocal, 0, 'f', 2).arg(
									releaseThreshold, 0, 'f', 2));
				}
			} else
				releaseStartMs = 0;
		}

		float desiredAttenDb = 0.0f;
		if (limiterEngaged) {
			if (peakDbLocal > -6.0f)
				desiredAttenDb = -6.0f - peakDbLocal;
			else
				desiredAttenDb = 0.0f;
		}

		float desiredLinear = linear_from_dbfs(desiredAttenDb);
		int basePercent =
				(preLimiterPercent >= 0) ? preLimiterPercent : currentHwPercent;
		int targetPercent = int(
				std::clamp(float(basePercent) * desiredLinear, 0.0f, 100.0f));

		float overshootDb = std::max(0.0f, peakDbLocal + 6.0f);
		float adaptiveAttack = attackMs;
		float adaptiveRelease = releaseMs;
		if (overshootDb > 6.0f)
			adaptiveAttack = std::max(5, attackMs / 4);
		else if (overshootDb > 3.0f)
			adaptiveAttack = std::max(8, attackMs / 2);
		if (limiterEngaged) {
			qint64 engagedDuration = now - engageTimeMs;
			adaptiveRelease = std::min(5000,
					releaseMs + int(std::min(4000.0, engagedDuration / 2.0)));
		}

		float dtMs = limiterTimer->interval();
		float alpha =
				(targetPercent < smoothedHwPercent) ?
						expAlphaFromMs(float(adaptiveAttack), dtMs) :
						expAlphaFromMs(float(adaptiveRelease), dtMs);
		smoothedHwPercent = smoothedHwPercent
				+ alpha * (targetPercent - smoothedHwPercent);

		if (now - lastLimiterAdjustMs > g_hwAdjustIntervalMs
				&& std::abs(smoothedHwPercent - currentHwPercent) > 0.5f) {
			int setVal = int(std::round(smoothedHwPercent));
			if (mixer->setCapturePercent(setVal)) {
				currentHwPercent = setVal;
				lastLimiterAdjustMs = now;
				logView->append(
						QString(
								"[%1] ALSA set to %2% (target %3%, adaptA %4ms adaptR %5ms)").arg(
								QTime::currentTime().toString()).arg(
								currentHwPercent).arg(targetPercent).arg(
								int(adaptiveAttack)).arg(int(adaptiveRelease)));
			} else {
				logView->append(
						QString("[%1] ALSA setCapturePercent failed").arg(
								QTime::currentTime().toString()));
			}
		}
	}

	void updateUI() {
		float peak = lastPeak.load();
		float rms = lastRms.load();
		inputProgress->setValue(int(std::min(1.0f, peak) * 1000000.0f));
		inputLabel->setText(QString::number(peak, 'f', 4));
		rmsProgress->setValue(int(std::min(1.0f, rms) * 1000000.0f));
		rmsLabel->setText(QString::number(rms, 'f', 4));
		float peakDb = dbfs_from_linear(std::max(1e-12f, peak));
		float peakDbNorm = std::clamp((peakDb + 120.0f) / 120.0f, 0.0f, 1.0f);
		peakDbProgress->setValue(int(peakDbNorm * 1000000.0f));
		peakDbLabel->setText(QString::number(peakDb, 'f', 4));
		float desiredAttenDb =
				limiterEngaged ? std::min(0.0f, -6.0f - peakDb) : 0.0f;
		float attNorm = std::clamp((desiredAttenDb + 24.0f) / 24.0f, 0.0f,
				1.0f);
		desiredAttenProgress->setValue(int(attNorm * 1000000.0f));
		desiredAttenLabel->setText(QString::number(desiredAttenDb, 'f', 4));
		engagedProgress->setValue(limiterEngaged ? 1000000 : 0);
		engagedLabel->setText(limiterEngaged ? "1.0000" : "0.0000");
		hwProgress->setValue(
				int(
						std::clamp(smoothedHwPercent / 100.0f, 0.0f, 1.0f)
								* 1000000.0f));
		hwLabel->setText(QString::number(smoothedHwPercent, 'f', 4));
		float outPeak = peak * computeDisplayGainFromHeadroom(headroomValue)
				* (smoothedHwPercent / 100.0f);
		outputProgress->setValue(int(std::min(1.0f, outPeak) * 1000000.0f));
		outputLabel->setText(QString::number(outPeak, 'f', 4));
	}
};

// ----------------------------- main -----------------------------------------------
int main(int argc, char **argv) {
	QApplication app(argc, argv);
	QApplication::setApplicationName("audioviz_auto");
	QApplication::setOrganizationName("audioviz");

	MainWindow w;
	w.resize(1200, 800);
	w.show();

	return app.exec();
}

#include "main.moc"
