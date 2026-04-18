#include <QApplication>
#include <QMainWindow>
#include <QDockWidget>
#include <QPlainTextEdit>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QAudioSource>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QProgressBar>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QTimer>
#include <QMutex>
#include <QSettings>
#include <QStyle>
#include <QPointer>
#include <QElapsedTimer>
#include <QMargins>

#include <alsa/asoundlib.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>

// ============================================================================
// 1. FlowLayout (Qt6 kompatibel)
// ============================================================================
class FlowLayout: public QLayout {
public:
	explicit FlowLayout(QWidget *parent = nullptr, int margin = -1,
			int hSpacing = -1, int vSpacing = -1) :
			QLayout(parent), m_hSpace(hSpacing), m_vSpace(vSpacing) {
		setContentsMargins(margin, margin, margin, margin);
	}
	~FlowLayout() {
		QLayoutItem *item;
		while ((item = takeAt(0)))
			delete item;
	}

	void addItem(QLayoutItem *item) override {
		itemList.append(item);
	}

	int horizontalSpacing() const {
		return m_hSpace >= 0 ?
				m_hSpace : smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
	}

	int verticalSpacing() const {
		return m_vSpace >= 0 ?
				m_vSpace : smartSpacing(QStyle::PM_LayoutVerticalSpacing);
	}

	int count() const override {
		return itemList.size();
	}
	QLayoutItem* itemAt(int index) const override {
		return itemList.value(index);
	}
	QLayoutItem* takeAt(int index) override {
		return index >= 0 && index < itemList.size() ?
				itemList.takeAt(index) : nullptr;
	}
	Qt::Orientations expandingDirections() const override {
		return {};
	}
	bool hasHeightForWidth() const override {
		return true;
	}
	int heightForWidth(int width) const override {
		return doLayout(QRect(0, 0, width, 0), true);
	}
	void setGeometry(const QRect &rect) override {
		QLayout::setGeometry(rect);
		doLayout(rect, false);
	}
	QSize sizeHint() const override {
		return minimumSize();
	}

	QSize minimumSize() const override {
		QSize size;
		for (QLayoutItem *item : std::as_const(itemList))
			size = size.expandedTo(item->minimumSize());
		QMargins m = contentsMargins();
		size += QSize(m.left() + m.right(), m.top() + m.bottom());
		return size;
	}

private:
	int doLayout(const QRect &rect, bool testOnly) const {
		int left, top, right, bottom;
		getContentsMargins(&left, &top, &right, &bottom);
		QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
		int x = effectiveRect.x();
		int y = effectiveRect.y();
		int lineHeight = 0;
		for (QLayoutItem *item : std::as_const(itemList)) {
			QWidget *wid = item->widget();
			int spaceX = horizontalSpacing();
			if (spaceX == -1)
				spaceX = wid->style()->layoutSpacing(QSizePolicy::PushButton,
						QSizePolicy::PushButton, Qt::Horizontal);
			int spaceY = verticalSpacing();
			if (spaceY == -1)
				spaceY = wid->style()->layoutSpacing(QSizePolicy::PushButton,
						QSizePolicy::PushButton, Qt::Vertical);
			int nextX = x + item->sizeHint().width() + spaceX;
			if (nextX - spaceX > effectiveRect.right() && lineHeight > 0) {
				x = effectiveRect.x();
				y = y + lineHeight + spaceY;
				nextX = x + item->sizeHint().width() + spaceX;
				lineHeight = 0;
			}
			if (!testOnly)
				item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));
			x = nextX;
			lineHeight = qMax(lineHeight, item->sizeHint().height());
		}
		return y + lineHeight - rect.y() + bottom;
	}
	int smartSpacing(QStyle::PixelMetric pm) const {
		if (!parent())
			return -1;
		else if (parent()->isWidgetType())
			return static_cast<QWidget*>(parent())->style()->pixelMetric(pm,
					nullptr, static_cast<QWidget*>(parent()));
		else
			return static_cast<QLayout*>(parent())->spacing();
	}
	QList<QLayoutItem*> itemList;
	int m_hSpace, m_vSpace;
};

// ============================================================================
// 2. ALSA Hardware Steuerung
// ============================================================================
namespace ALSAControl {
static const char *MIXER_NAME = "default";
static const char *SELEM_NAME = "Capture";

long getHardwareMicVolumePercent() {
	snd_mixer_t *handle;
	snd_mixer_selem_id_t *sid;
	if (snd_mixer_open(&handle, 0) < 0)
		return -1;
	if (snd_mixer_attach(handle, MIXER_NAME) < 0
			|| snd_mixer_selem_register(handle, NULL, NULL) < 0
			|| snd_mixer_load(handle) < 0) {
		snd_mixer_close(handle);
		return -1;
	}
	snd_mixer_selem_id_alloca(&sid);
	snd_mixer_selem_id_set_index(sid, 0);
	snd_mixer_selem_id_set_name(sid, SELEM_NAME);
	snd_mixer_elem_t *elem = snd_mixer_find_selem(handle, sid);

	long percent = -1;
	if (elem && snd_mixer_selem_has_capture_volume(elem)) {
		long min, max, vol;
		snd_mixer_selem_get_capture_volume_range(elem, &min, &max);
		snd_mixer_selem_get_capture_volume(elem, SND_MIXER_SCHN_FRONT_LEFT,
				&vol);
		if (max != min)
			percent = (long) (((double) (vol - min) / (double) (max - min))
					* 100.0);
	}
	snd_mixer_close(handle);
	return percent;
}

void setHardwareMicVolumePercent(long percent) {
	percent = std::clamp(percent, 0L, 100L);
	snd_mixer_t *handle;
	snd_mixer_selem_id_t *sid;
	if (snd_mixer_open(&handle, 0) < 0)
		return;
	if (snd_mixer_attach(handle, MIXER_NAME) >= 0
			&& snd_mixer_selem_register(handle, NULL, NULL) >= 0
			&& snd_mixer_load(handle) >= 0) {
		snd_mixer_selem_id_alloca(&sid);
		snd_mixer_selem_id_set_index(sid, 0);
		snd_mixer_selem_id_set_name(sid, SELEM_NAME);
		snd_mixer_elem_t *elem = snd_mixer_find_selem(handle, sid);
		if (elem && snd_mixer_selem_has_capture_volume(elem)) {
			long min, max;
			snd_mixer_selem_get_capture_volume_range(elem, &min, &max);
			long vol = min + (long) (((double) percent / 100.0) * (max - min));
			snd_mixer_selem_set_capture_volume_all(elem, vol);
		}
	}
	snd_mixer_close(handle);
}
}

// ============================================================================
// 3. Audio Prozessor & Ringpuffer
// ============================================================================
class AudioProcessor: public QObject {
Q_OBJECT
public:
	struct Params {
		bool limiterEnabled = false;
		double ceilingDb = -3.0;
		double hystOn = 1.0;
		double hystOff = 0.5;
		double headroomLinear = 1.0;
		int texSize = 1024;
	};

	AudioProcessor() {
		QAudioFormat format;
		format.setSampleRate(48000);
		format.setChannelCount(1);
		format.setSampleFormat(QAudioFormat::Int16);

		QAudioDevice info = QMediaDevices::defaultAudioInput();
		audioSource = new QAudioSource(info, format, this);
		ringBuffer.resize(48000 * 5, 0.0f);
	}

	void start() {
		device = audioSource->start();
		if (device)
			connect(device, &QIODevice::readyRead, this,
					&AudioProcessor::processAudioData);
	}

	void setParams(const Params &p) {
		QMutexLocker locker(&mutex);
		params = p;
	}

	std::vector<float> getLatestSamples() {
		QMutexLocker locker(&mutex);
		std::vector<float> result(params.texSize, 0.0f);
		int ringSize = (int) ringBuffer.size();
		if (ringSize == 0)
			return result;

		int startIdx = (writePos - params.texSize + ringSize) % ringSize;
		for (int i = 0; i < params.texSize; ++i) {
			result[i] = ringBuffer[(startIdx + i) % ringSize]
					* (float) params.headroomLinear;
		}
		return result;
	}

signals:
	void levelsUpdated(double micRms, double micPeak, int hwPercent, bool kick);

private slots:
	void processAudioData() {
		QByteArray data = device->readAll();
		if (data.isEmpty())
			return;

		const int16_t *samples =
				reinterpret_cast<const int16_t*>(data.constData());
		int sampleCount = data.size() / sizeof(int16_t);

		double peakAmp = 0.0;
		double sumSquares = 0.0;

		mutex.lock();
		for (int i = 0; i < sampleCount; ++i) {
			float normSample = samples[i] / 32768.0f;
			ringBuffer[writePos] = normSample;
			writePos = (writePos + 1) % ringBuffer.size();
			double absSample = std::abs((double) normSample);
			if (absSample > peakAmp)
				peakAmp = absSample;
			sumSquares += absSample * absSample;
		}

		double rms = std::sqrt(sumSquares / sampleCount);
		bool kick = (rms > rmsHistoryAvg * 1.8) && (rms > 0.02);
		rmsHistoryAvg = 0.95 * rmsHistoryAvg + 0.05 * rms;

		int currentHwPercent = ALSAControl::getHardwareMicVolumePercent();
		if (params.limiterEnabled && peakAmp > 1e-5) {
			double peakDb = 20.0 * std::log10(peakAmp);
			if (peakDb > params.ceilingDb + params.hystOn) {
				int newPercent = std::max(0, currentHwPercent - 2);
				ALSAControl::setHardwareMicVolumePercent(newPercent);
				currentHwPercent = newPercent;
			}
		}
		mutex.unlock();
		emit levelsUpdated(rms, peakAmp, currentHwPercent, kick);
	}

private:
	QAudioSource *audioSource = nullptr;
	QIODevice *device = nullptr;
	QMutex mutex;
	std::vector<float> ringBuffer;
	int writePos = 0;
	Params params;
	double rmsHistoryAvg = 0.0;
};

// ============================================================================
// 4. OpenGL Visualisierer
// ============================================================================
class GLWaveWidget: public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
Q_OBJECT
public:
	explicit GLWaveWidget(QWidget *parent = nullptr) :
			QOpenGLWidget(parent) {
	}

	void updateAudioData(const std::vector<float> &samples, bool kick) {
		audioSamples = samples;
		if (kick)
			flashTime = 1.0f;
		update();
	}

	void setShaderCode(const QString &vertex, const QString &fragment) {
		makeCurrent();
		QOpenGLShaderProgram *newProgram = new QOpenGLShaderProgram(this);
		if (newProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertex)
				&& newProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
						fragment) && newProgram->link()) {
			if (program)
				delete program;
			program = newProgram;
			emit shaderCompiled("Shader OK");
		} else {
			emit shaderCompiled(newProgram->log());
			delete newProgram;
		}
		doneCurrent();
	}

signals:
	void shaderCompiled(const QString &log);

protected:
	void initializeGL() override {
		initializeOpenGLFunctions();
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		float vertices[] = { -1, -1, 1, -1, -1, 1, 1, -1, 1, 1, -1, 1 };
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
				GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
				nullptr);
		glEnableVertexAttribArray(0);
		glGenTextures(1, &textureId);
		timer.start();
	}

	void paintGL() override {
		glClear(GL_COLOR_BUFFER_BIT);
		if (!program || audioSamples.empty())
			return;
		glBindTexture(GL_TEXTURE_1D, textureId);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, (int) audioSamples.size(), 0,
				GL_RED, GL_FLOAT, audioSamples.data());
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		program->bind();
		program->setUniformValue("u_time", (float) timer.elapsed() / 1000.0f);
		program->setUniformValue("u_kick", flashTime);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		flashTime = std::max(0.0f, flashTime - 0.05f);
	}

private:
	QOpenGLShaderProgram *program = nullptr;
	GLuint vao { 0 }, vbo { 0 }, textureId { 0 };
	std::vector<float> audioSamples;
	QElapsedTimer timer;
	float flashTime = 0.0f;
};

// ============================================================================
// 5. Hauptfenster
// ============================================================================
class MainWindow: public QMainWindow {
Q_OBJECT
public:
	MainWindow() {
		setupUI();
		loadSettings();
		audioProcessor = new AudioProcessor();
		connect(audioProcessor, &AudioProcessor::levelsUpdated, this,
				&MainWindow::updateLevels);

		QTimer *vTimer = new QTimer(this);
		connect(vTimer, &QTimer::timeout, this,
				[this]() {
					glWidget->updateAudioData(
							audioProcessor->getLatestSamples(), currentKick);
					currentKick = false;
				});
		vTimer->start(16);
		updateAudioParams();
		audioProcessor->start();
	}

private slots:
	void updateLevels(double rms, double peak, int hwPercent, bool kick) {
		if (kick)
			currentKick = true;
		micProgressBar->setValue((int) (peak * 100));
		hwLabel->setText(QString("HW: %1%").arg(hwPercent));
		double outVal = peak * std::pow(10, headroomSlider->value() / 500.0);
		vizProgressBar->setValue(std::clamp((int) (outVal * 100), 0, 100));
	}

	void updateAudioParams() {
		AudioProcessor::Params p;
		p.limiterEnabled = limiterCheck->isChecked();
		p.ceilingDb = -(double) limiterCeiling->value();
		p.hystOn = hystOnSpin->value();
		p.hystOff = hystOffSpin->value();
		p.headroomLinear = std::pow(10.0, headroomSlider->value() / 1000.0);
		p.texSize = texSizeSpin->value();
		audioProcessor->setParams(p);
	}

	void compileShader() {
		QString code = shaderEditor->toPlainText();
		int vIdx = code.indexOf("#shader vertex");
		int fIdx = code.indexOf("#shader fragment");
		if (vIdx != -1 && fIdx != -1) {
			glWidget->setShaderCode(
					code.mid(vIdx + 14, fIdx - (vIdx + 14)).trimmed(),
					code.mid(fIdx + 16).trimmed());
		}
	}

private:
	void setupUI() {
		QWidget *central = new QWidget();
		QVBoxLayout *layout = new QVBoxLayout(central);

		QWidget *toolContainer = new QWidget();
		FlowLayout *flow = new FlowLayout(toolContainer);

		micProgressBar = new QProgressBar();
		micProgressBar->setFixedWidth(100);
		hwLabel = new QLabel("HW: --");
		headroomSlider = new QSlider(Qt::Horizontal);
		headroomSlider->setRange(0, 2000);
		limiterCheck = new QCheckBox("Limit");
		limiterCeiling = new QSlider(Qt::Horizontal);
		limiterCeiling->setRange(0, 60);
		hystOnSpin = new QDoubleSpinBox();
		hystOffSpin = new QDoubleSpinBox();
		texSizeSpin = new QSpinBox();
		texSizeSpin->setRange(128, 2048);
		texSizeSpin->setValue(1024);
		vizProgressBar = new QProgressBar();
		vizProgressBar->setFixedWidth(100);

		flow->addWidget(new QLabel("Mic:"));
		flow->addWidget(micProgressBar);
		flow->addWidget(hwLabel);
		flow->addWidget(new QLabel("Gain:"));
		flow->addWidget(headroomSlider);
		flow->addWidget(limiterCheck);
		flow->addWidget(new QLabel("Ceil -dB:"));
		flow->addWidget(limiterCeiling);
		flow->addWidget(new QLabel("Hyst:"));
		flow->addWidget(hystOnSpin);
		flow->addWidget(hystOffSpin);
		flow->addWidget(new QLabel("Tex:"));
		flow->addWidget(texSizeSpin);
		flow->addWidget(vizProgressBar);

		shaderEditor = new QPlainTextEdit();
		shaderEditor->setPlainText(
				"#shader vertex\n#version 330 core\nlayout(location=0) in vec2 aPos; out vec2 uv; void main(){ uv=aPos*0.5+0.5; gl_Position=vec4(aPos,0,1); }\n\n#shader fragment\n#version 330 core\nin vec2 uv; out vec4 f; uniform sampler1D u_samples; uniform float u_time; uniform float u_kick; void main(){ float v = texture(u_samples, uv.x).r; vec3 c = mix(vec3(0.1,0.4,0.8), vec3(1,0.2,0.5), abs(v)*2.0); if(abs(uv.y-0.5) > abs(v)) c*=0.2; f=vec4(c+u_kick*0.2,1); }");

		layout->addWidget(toolContainer);
		layout->addWidget(shaderEditor);
		setCentralWidget(central);

		QDockWidget *d = new QDockWidget("Preview");
		glWidget = new GLWaveWidget();
		d->setWidget(glWidget);
		addDockWidget(Qt::RightDockWidgetArea, d);

		QDockWidget *l = new QDockWidget("Log");
		logText = new QPlainTextEdit();
		logText->setReadOnly(true);
		l->setWidget(logText);
		addDockWidget(Qt::BottomDockWidgetArea, l);

		connect(shaderEditor, &QPlainTextEdit::textChanged, this, [this]() {
			QTimer::singleShot(400, this, &MainWindow::compileShader);
		});
		connect(glWidget, &GLWaveWidget::shaderCompiled, logText,
				&QPlainTextEdit::setPlainText);

		// Update connections
		connect(headroomSlider, &QSlider::valueChanged, this,
				&MainWindow::updateAudioParams);
		connect(limiterCheck, &QCheckBox::toggled, this,
				&MainWindow::updateAudioParams);
		connect(limiterCeiling, &QSlider::valueChanged, this,
				&MainWindow::updateAudioParams);
	}

	void loadSettings() {
		QSettings s("VJ", "Visualizer");
		headroomSlider->setValue(s.value("gain", 1000).toInt());
	}

	AudioProcessor *audioProcessor;
	GLWaveWidget *glWidget;
	QPlainTextEdit *shaderEditor, *logText;
	QProgressBar *micProgressBar, *vizProgressBar;
	QLabel *hwLabel;
	QSlider *headroomSlider, *limiterCeiling;
	QCheckBox *limiterCheck;
	QDoubleSpinBox *hystOnSpin, *hystOffSpin;
	QSpinBox *texSizeSpin;
	bool currentKick = false;
};

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}

#include "main.moc"
