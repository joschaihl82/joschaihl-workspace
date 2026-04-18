// main.cpp
// Qt6 + ALSA + OpenGL - Amalgamierte Single-File Version
// Änderungen: Erzwinge S16_LE als PCM-Format (snd_pcm_hw_params_set_format(..., SND_PCM_FORMAT_S16_LE))
// und konvertiere die Integer-Samples softwareseitig in float für die DSP-Pipeline.
// AudioWorker liefert std::vector<float> (normalisiert in [-1,1]).
// Mixer wird ausschließlich per Integer-Volume API angepasst (kein dB-API).
// Thread-safe Highlighter, deferred init, Audio immer an, Live GLSL-Editor + Preview.

#include <QApplication>
#include <QMainWindow>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QSplitter>
#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QObject>
#include <QMetaType>
#include <QToolBar>
#include <QAction>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QFont>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QFileDialog>
#include <QMessageBox>
#include <QStyleFactory>
#include <QKeyEvent>
#include <QPalette>
#include <QTextBlock>
#include <QFile>
#include <QTextStream>
#include <QStyle>
#include <QSplitterHandle>
#include <QPointer>
#include <alsa/asoundlib.h>
#include <vector>
#include <cmath>
#include <set>

// Metatype-Registrierungen für queued connections
Q_DECLARE_METATYPE(std::vector<float>)
Q_DECLARE_METATYPE(std::set<int>)

// ============================================================
// AudioHardware: sichere Headroom-Setter (nur Integer volume API)
// ============================================================
class AudioHardware {
public:
    // Versucht, für ein geeignetes Capture-Element die Capture-Volume (integer) zu setzen.
    // dbValue wird hier nur als Hinweis verwendet; die API arbeitet mit integer volumes.
    // Rückgabe: true wenn erfolgreich.
    static bool setHardwareHeadroomSafe(long dbValue) {
        snd_mixer_t *handle = nullptr;
        snd_mixer_selem_id_t *sid = nullptr;
        const char *card = "default";

        if (snd_mixer_open(&handle, 0) < 0) {
            qWarning() << "snd_mixer_open failed";
            return false;
        }
        if (snd_mixer_attach(handle, card) < 0) {
            qWarning() << "snd_mixer_attach failed for card" << card;
            snd_mixer_close(handle);
            return false;
        }
        snd_mixer_selem_register(handle, NULL, NULL);
        if (snd_mixer_load(handle) < 0) {
            qWarning() << "snd_mixer_load failed";
            snd_mixer_close(handle);
            return false;
        }

        bool setOk = false;
        snd_mixer_elem_t *elem = snd_mixer_first_elem(handle);
        while (elem) {
            if (snd_mixer_selem_is_active(elem)) {
                snd_mixer_selem_id_alloca(&sid);
                snd_mixer_selem_get_id(elem, sid);
                const char *name = snd_mixer_selem_id_get_name(sid);

                // Only use integer volume API: check if element supports capture volume
                if (snd_mixer_selem_has_capture_volume(elem)) {
                    long minVol = 0, maxVol = 0;
                    if (snd_mixer_selem_get_capture_volume_range(elem, &minVol, &maxVol) == 0 && maxVol > minVol) {
                        // Map requested dbValue to a conservative integer fraction.
                        double fraction = 0.20; // conservative default (20% of range)
                        if (dbValue > -48) {
                            double t = (dbValue + 48.0) / 48.0;
                            fraction = 0.2 + t * 0.4;
                            if (fraction < 0.05) fraction = 0.05;
                            if (fraction > 1.0) fraction = 1.0;
                        }
                        long vol = static_cast<long>(minVol + fraction * (maxVol - minVol));
                        if (vol < minVol) vol = minVol;
                        if (vol > maxVol) vol = maxVol;
                        int rc = snd_mixer_selem_set_capture_volume_all(elem, vol);
                        if (rc == 0) {
                            qDebug() << "Capture volume set to" << vol << "for element" << name << "(range" << minVol << "-" << maxVol << ")";
                            setOk = true;
                            break;
                        } else {
                            qWarning() << "set_capture_volume_all failed for" << name << "rc=" << rc << snd_strerror(rc);
                        }
                    } else {
                        qWarning() << "Could not get capture volume range for" << name;
                    }
                }
            }
            elem = snd_mixer_elem_next(elem);
        }

        if (!setOk) {
            qWarning() << "No suitable capture element found or setting volume failed. Continuing without HW headroom change.";
        }

        snd_mixer_close(handle);
        return setOk;
    }
};

// ============================================================
// AudioWorker: ALSA Capture -> int16 samples -> normalize to float
// Emits std::vector<float> via bufferReady
// ============================================================
class AudioWorker : public QThread {
    Q_OBJECT
public:
    bool running = true;
    const int bufferSize = 128;

    AudioWorker(QObject* parent = nullptr) : QThread(parent) {}
    ~AudioWorker() override {
        running = false;
        requestInterruption();
        wait(500);
    }

signals:
    void bufferReady(const std::vector<float>& data);
    void kickDetected(float strength);

protected:
    void run() override {
        snd_pcm_t *capture_handle = nullptr;
        // Open default capture device
        if (snd_pcm_open(&capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0) < 0) {
            qWarning() << "snd_pcm_open failed";
            return;
        }

        // Allocate hw params
        snd_pcm_hw_params_t *hw_params = nullptr;
        snd_pcm_hw_params_alloca(&hw_params);
        if (snd_pcm_hw_params_any(capture_handle, hw_params) < 0) {
            qWarning() << "snd_pcm_hw_params_any failed";
            snd_pcm_close(capture_handle);
            return;
        }

        // Interleaved access
        if (snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
            qWarning() << "snd_pcm_hw_params_set_access failed";
            snd_pcm_close(capture_handle);
            return;
        }

        // **Enforce 16-bit signed little-endian PCM** as requested:
        // This ensures we always receive S16_LE samples and then convert them in software to float.
        if (snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_S16_LE) < 0) {
            qWarning() << "snd_pcm_hw_params_set_format(S16_LE) failed - device may not support S16_LE";
            // We continue but the device may return a different format; still attempt to set params.
        }

        // Set sample rate (try 44100)
        unsigned int rate = 44100;
        if (snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &rate, 0) < 0) {
            qWarning() << "snd_pcm_hw_params_set_rate_near failed";
            snd_pcm_close(capture_handle);
            return;
        }

        // Set period size (frames)
        snd_pcm_uframes_t frames = static_cast<snd_pcm_uframes_t>(bufferSize);
        if (snd_pcm_hw_params_set_period_size_near(capture_handle, hw_params, &frames, 0) < 0) {
            qWarning() << "snd_pcm_hw_params_set_period_size_near failed";
            snd_pcm_close(capture_handle);
            return;
        }

        // Apply hw params
        if (snd_pcm_hw_params(capture_handle, hw_params) < 0) {
            qWarning() << "snd_pcm_hw_params failed";
            snd_pcm_close(capture_handle);
            return;
        }

        // Prepare buffers
        std::vector<short> raw(bufferSize);
        std::vector<float> processed(bufferSize);

        // Kick detection envelope variables
        double prevLowEnergy = 0.0;
        const double attack = 0.9;
        const double release = 0.995;
        double lowEnv = 0.0;
        const int lowBandSamples = 20;
        const double kickThreshold = 0.12;

        // Main capture loop
        while (running && !isInterruptionRequested()) {
            int err = snd_pcm_readi(capture_handle, raw.data(), bufferSize);
            if (err == -EPIPE) {
                // Overrun
                snd_pcm_prepare(capture_handle);
                continue;
            } else if (err < 0) {
                qWarning() << "snd_pcm_readi error:" << snd_strerror(err);
                break;
            }

            int readFrames = (err > 0) ? err : bufferSize;
            // Convert int16 -> float in software
            for (int i = 0; i < readFrames; ++i) {
                // Normalize to [-1.0, 1.0)
                float f = static_cast<float>(raw[i]) / 32768.0f;
                if (f > 1.0f) f = 1.0f;
                if (f < -1.0f) f = -1.0f;
                processed[i] = f;
            }
            for (int i = readFrames; i < bufferSize; ++i) processed[i] = 0.0f;

            // Low-frequency energy for kick detection
            double lowSum = 0.0;
            for (int i = 0; i < std::min(bufferSize, lowBandSamples); ++i)
                lowSum += std::fabs(processed[i]);
            double lowAvg = lowSum / static_cast<double>(std::min(bufferSize, lowBandSamples));

            if (lowAvg > lowEnv)
                lowEnv = attack * lowEnv + (1.0 - attack) * lowAvg;
            else
                lowEnv = release * lowEnv + (1.0 - release) * lowAvg;

            double diff = lowAvg - prevLowEnergy;
            prevLowEnergy = lowAvg;

            if (diff > kickThreshold && lowEnv > 0.01) {
                float strength = static_cast<float>(std::min(1.0, diff * 8.0));
                emit kickDetected(strength);
            }

            // Emit normalized float buffer
            emit bufferReady(processed);

            // Small sleep to avoid busy loop; ALSA period timing will usually block appropriately
            QThread::msleep(1);
        }

        snd_pcm_close(capture_handle);
    }
};

// ============================================================
// GLSLHighlighter: Syntax + Error Underline (thread-safe)
// ============================================================
class GLSLHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    GLSLHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent) {
        QTextCharFormat keywordFormat;
        keywordFormat.setForeground(Qt::cyan);
        keywordFormat.setFontWeight(QFont::Bold);
        const QStringList keywords = {
            "\\bvoid\\b", "\\bfloat\\b", "\\bint\\b", "\\buniform\\b",
            "\\bvec2\\b", "\\bvec3\\b", "\\bvec4\\b", "\\bout\\b", "\\bin\\b",
            "\\bif\\b", "\\bfor\\b", "\\breturn\\b", "\\bconst\\b"
        };
        for (const QString &kw : keywords) rules.append({QRegularExpression(kw), keywordFormat});

        QTextCharFormat numberFormat;
        numberFormat.setForeground(Qt::yellow);
        rules.append({QRegularExpression("\\b[0-9]+(\\.[0-9]+)?\\b"), numberFormat});

        QTextCharFormat commentFormat;
        commentFormat.setForeground(Qt::green);
        rules.append({QRegularExpression("//[^\n]*"), commentFormat});
    }

    void setErrorLines(const std::set<int>& lines, const QString& message = QString()) {
        if (QThread::currentThread() != qApp->thread()) {
            QPointer<GLSLHighlighter> guard(this);
            QTimer::singleShot(0, qApp, [guard, lines, message]() {
                if (guard) guard->setErrorLinesImpl(lines, message);
            });
            return;
        }
        setErrorLinesImpl(lines, message);
    }

    QString currentErrorMessage() const { return errorMessage; }

protected:
    void highlightBlock(const QString &text) override {
        for (const auto &r : rules) {
            QRegularExpressionMatchIterator it = r.pattern.globalMatch(text);
            while (it.hasNext()) {
                QRegularExpressionMatch m = it.next();
                setFormat(m.capturedStart(), m.capturedLength(), r.format);
            }
        }

        int lineNo = currentBlock().blockNumber() + 1;
        if (errorLines.find(lineNo) != errorLines.end()) {
            QTextCharFormat errFmt;
            errFmt.setUnderlineColor(Qt::red);
            errFmt.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
            setFormat(0, text.length(), errFmt);
        }
    }

private:
    struct Rule { QRegularExpression pattern; QTextCharFormat format; };
    QVector<Rule> rules;
    std::set<int> errorLines;
    QString errorMessage;

    void setErrorLinesImpl(const std::set<int>& lines, const QString& message) {
        errorLines = lines;
        errorMessage = message;
        rehighlight();
    }
};

// ============================================================
// ShaderView: compiles fragment shader, keeps last good shader
// Accepts std::vector<float> audio buffers
// ============================================================
class ShaderView : public QOpenGLWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    ShaderView(QWidget* parent = nullptr)
        : QOpenGLWidget(parent), vbo(QOpenGLBuffer::VertexBuffer)
    {
        lastGoodFragment = R"(
            #version 330 core
            uniform float audio[128];
            uniform float kick;
            out vec4 fragColor;
            in vec2 uv;
            void mainImage(out vec4 fragColor, in vec2 fragCoord) {
                int idx = int(fragCoord.x * 128.0) % 128;
                float a = audio[idx];
                vec3 base = vec3(0.0, abs(a) * 6.0, abs(a) * 2.0);
                fragColor = vec4(mix(base, vec3(1.0), clamp(kick, 0.0, 1.0)), 1.0);
            }
            void main() {
                vec2 fragCoord = uv * vec2(800.0, 600.0);
                mainImage(fragColor, fragCoord);
            }
        )";
        setMinimumSize(320, 240);
    }

    QPair<bool, QString> tryCompileFragment(const QString& fragSource) {
        if (!isValid()) return {false, QString("GL context not ready")};

        makeCurrent();

        QOpenGLShaderProgram testProg;
        const char *vsrc = R"(
            #version 330 core
            layout(location = 0) in vec2 position;
            out vec2 uv;
            void main() {
                uv = (position + 1.0) * 0.5;
                gl_Position = vec4(position, 0.0, 1.0);
            }
        )";
        if (!testProg.addShaderFromSourceCode(QOpenGLShader::Vertex, vsrc)) {
            QString log = testProg.log();
            doneCurrent();
            return {false, QString("Vertex shader compile failed:\n%1").arg(log)};
        }

        if (!testProg.addShaderFromSourceCode(QOpenGLShader::Fragment, fragSource.toUtf8().constData())) {
            QString log = testProg.log();
            doneCurrent();
            return {false, QString("Fragment shader compile failed:\n%1").arg(log)};
        }

        if (!testProg.link()) {
            QString log = testProg.log();
            doneCurrent();
            return {false, QString("Link failed:\n%1").arg(log)};
        }

        if (program) { delete program; program = nullptr; }
        program = new QOpenGLShaderProgram(this);
        program->addShaderFromSourceCode(QOpenGLShader::Vertex, vsrc);
        program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragSource.toUtf8().constData());
        if (!program->link()) {
            QString log = program->log();
            delete program;
            program = nullptr;
            doneCurrent();
            return {false, QString("Link after install failed:\n%1").arg(log)};
        }

        lastGoodFragment = fragSource;
        doneCurrent();
        return {true, QString("OK")};
    }

    QPair<bool, QString> compileAndInstall(const QString& fragSource) {
        if (!isValid()) {
            pendingSource = fragSource;
            return {false, QString("GL context not ready")};
        }
        return tryCompileFragment(fragSource);
    }

public slots:
    void updateAudio(const std::vector<float>& data) {
        audioData = data;
        update();
    }

    void onKick(float strength) {
        kickLevel = std::max(kickLevel, strength);
        update();
    }

protected:
    void initializeGL() override {
        initializeOpenGLFunctions();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        initGeometry();

        QPair<bool, QString> res = tryCompileFragment(lastGoodFragment);
        if (!res.first) qWarning() << "Initial shader compile failed:" << res.second;

        if (!pendingSource.isEmpty()) {
            compileAndInstall(pendingSource);
            pendingSource.clear();
        }
    }

    void resizeGL(int w, int h) override { glViewport(0, 0, w, h); }

    void paintGL() override {
        kickLevel *= 0.85f;
        if (kickLevel < 0.001f) kickLevel = 0.0f;

        glClear(GL_COLOR_BUFFER_BIT);

        if (!program || !program->isLinked()) {
            float b = std::min(1.0f, kickLevel);
            glClearColor(b, b, b, 1.0f);
            return;
        }

        program->bind();

        float fBuffer[128];
        for (int i = 0; i < 128; ++i) fBuffer[i] = 0.0f;
        for (int i = 0; i < static_cast<int>(audioData.size()) && i < 128; ++i)
            fBuffer[i] = audioData[i];

        program->setUniformValueArray("audio", fBuffer, 128, 1);
        program->setUniformValue("kick", kickLevel);

        vao.bind();
        vbo.bind();
        program->enableAttributeArray(0);
        program->setAttributeBuffer(0, GL_FLOAT, 0, 2, 2 * sizeof(float));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        vbo.release();
        vao.release();

        program->release();
    }

    void initGeometry() {
        GLfloat verts[] = {
            -1.0f, -1.0f,
             1.0f, -1.0f,
            -1.0f,  1.0f,
             1.0f,  1.0f
        };

        vao.create();
        vao.bind();

        vbo.create();
        vbo.bind();
        vbo.allocate(verts, sizeof(verts));

        vao.release();
        vbo.release();
    }

private:
    QOpenGLShaderProgram *program = nullptr;
    QOpenGLBuffer vbo;
    QOpenGLVertexArrayObject vao;
    std::vector<float> audioData;
    float kickLevel = 0.0f;
    QString lastGoodFragment;
    QString pendingSource;
};

// ============================================================
// ShaderIDE: main window
// - audio starts automatically (non-disableable)
// - deferred init to avoid QTextEngine races
// ============================================================
class ShaderIDE : public QMainWindow {
    Q_OBJECT
public:
    ShaderIDE(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Qt6 GLSL Live IDE (S16_LE -> float pipeline)");
        resize(1200, 800);

        auto *central = new QWidget();
        auto *layout = new QVBoxLayout(central);
        layout->setContentsMargins(6,6,6,6);
        layout->setSpacing(6);

        splitter = new QSplitter(Qt::Horizontal);
        editor = new QPlainTextEdit();
        QFont mono("Courier New");
        mono.setStyleHint(QFont::Monospace);
        mono.setPointSize(12);
        editor->setFont(mono);

        editor->setPlainText(R"(#version 330 core
uniform float audio[128];
uniform float kick;
out vec4 fragColor;
in vec2 uv;

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    int idx = int(fragCoord.x * 128.0) % 128;
    float a = audio[idx];
    vec3 base = vec3(0.0, abs(a) * 6.0, abs(a) * 2.0);
    fragColor = vec4(mix(base, vec3(1.0), clamp(kick, 0.0, 1.0)), 1.0);
}

void main() {
    vec2 fragCoord = uv * vec2(800.0, 600.0);
    mainImage(fragColor, fragCoord);
}
)");

        preview = new ShaderView();

        splitter->addWidget(editor);
        splitter->addWidget(preview);
        splitter->setStretchFactor(0, 1);
        splitter->setStretchFactor(1, 1);
        splitter->setSizes({520, 680});

        layout->addWidget(splitter);
        setCentralWidget(central);

        statusLabel = new QLabel("Ready");
        statusBar()->addWidget(statusLabel, 1);

        createActions();
        createToolBar();
        createMenus();

        worker = nullptr;

        // Use integer volume API only; dbValue is a hint for fraction mapping
        bool hwOk = AudioHardware::setHardwareHeadroomSafe(-48);
        if (!hwOk) statusBar()->showMessage("HW headroom not set (no suitable control or permission).", 8000);

        compileTimer = new QTimer(this);
        compileTimer->setSingleShot(true);
        compileTimer->setInterval(180);
        connect(compileTimer, &QTimer::timeout, this, &ShaderIDE::attemptCompileFromEditor);

        connect(editor, &QPlainTextEdit::textChanged, this, &ShaderIDE::onEditorTextChanged);

        QTimer::singleShot(0, this, &ShaderIDE::deferredInit);
    }

    ~ShaderIDE() override {
        closePreviewIfOpen();
        if (worker) {
            worker->running = false;
            worker->requestInterruption();
            worker->wait(500);
            delete worker;
            worker = nullptr;
        }
    }

protected:
    void keyPressEvent(QKeyEvent* ev) override {
        if (ev->key() == Qt::Key_F) {
            togglePreviewFullscreen();
            ev->accept();
            return;
        }
        QMainWindow::keyPressEvent(ev);
    }

private slots:
    void deferredInit() {
        highlighter = new GLSLHighlighter(editor->document());
        connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &ShaderIDE::updateCursorPosition);

        if (!worker) {
            worker = new AudioWorker(this);
            connect(worker, &AudioWorker::bufferReady, preview, &ShaderView::updateAudio, Qt::QueuedConnection);
            connect(worker, &AudioWorker::kickDetected, preview, &ShaderView::onKick, Qt::QueuedConnection);
            connect(worker, &AudioWorker::bufferReady, this, &ShaderIDE::onAudioBufferFloat, Qt::QueuedConnection);
            worker->start();
            statusBar()->showMessage("Audio capture started (always enabled).", 3000);
        }

        QTimer::singleShot(300, this, &ShaderIDE::attemptCompileFromEditor);
    }

    void onEditorTextChanged() {
        if (highlighter) highlighter->setErrorLines({}, QString());
        statusLabel->setText("Editing...");
        compileTimer->start();
    }

    void attemptCompileFromEditor() {
        if (!highlighter) {
            QTimer::singleShot(200, this, &ShaderIDE::attemptCompileFromEditor);
            return;
        }

        QString src = editor->toPlainText();
        QPair<bool, QString> res = preview->compileAndInstall(src);

        if (res.first) {
            highlighter->setErrorLines({}, QString());
            statusLabel->setText("Shader compiled OK");
        } else {
            std::set<int> lines = parseErrorLines(res.second);
            highlighter->setErrorLines(lines, res.second);
            statusLabel->setText("Compile error: see editor (red underline)");
            qDebug() << "Compile log:" << res.second;
        }
    }

    void onAudioBufferFloat(const std::vector<float>& data) {
        double sum = 0.0;
        for (float v : data) sum += static_cast<double>(v) * static_cast<double>(v);
        double rms = std::sqrt(sum / std::max<size_t>(1, data.size()));
        int pct = static_cast<int>(std::min(100.0, rms * 200.0));
        statusLabel->setText(QString("Audio RMS: %1").arg(pct));
    }

    void togglePreviewFullscreen() {
        if (previewWindow) closePreviewIfOpen(); else openPreviewFullscreen();
    }

    void openPreviewFullscreen() {
        if (previewWindow) return;
        previewWindow = new QMainWindow();
        previewWindow->setWindowTitle("Shader Preview (Fullscreen) - Press F to exit");
        previewViewer = new ShaderView();
        previewWindow->setCentralWidget(previewViewer);

        if (worker) {
            connect(worker, &AudioWorker::bufferReady, previewViewer, &ShaderView::updateAudio, Qt::QueuedConnection);
            connect(worker, &AudioWorker::kickDetected, previewViewer, &ShaderView::onKick, Qt::QueuedConnection);
        }

        previewViewer->compileAndInstall(editor->toPlainText());
        previewWindow->showFullScreen();
    }

    void closePreviewIfOpen() {
        if (!previewWindow) return;
        if (worker && previewViewer) {
            disconnect(worker, &AudioWorker::bufferReady, previewViewer, &ShaderView::updateAudio);
            disconnect(worker, &AudioWorker::kickDetected, previewViewer, &ShaderView::onKick);
        }
        previewWindow->close();
        delete previewWindow;
        previewWindow = nullptr;
        previewViewer = nullptr;
    }

    void updateCursorPosition() {
        QTextCursor c = editor->textCursor();
        int line = c.blockNumber() + 1;
        int col = c.columnNumber() + 1;
        statusBar()->showMessage(QString("Ln %1, Col %2").arg(line).arg(col), 2000);
    }

    void openFile() {
        QString fn = QFileDialog::getOpenFileName(this, "Open Shader", QString(), "GLSL Files (*.glsl *.frag *.txt);;All Files (*)");
        if (!fn.isEmpty()) {
            QFile f(fn);
            if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                editor->setPlainText(QString::fromUtf8(f.readAll()));
            } else {
                QMessageBox::warning(this, "Open", "Could not open file");
            }
        }
    }

    void saveFile() {
        QString fn = QFileDialog::getSaveFileName(this, "Save Shader", QString(), "GLSL Files (*.glsl *.frag *.txt);;All Files (*)");
        if (!fn.isEmpty()) {
            QFile f(fn);
            if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
                f.write(editor->toPlainText().toUtf8());
            } else {
                QMessageBox::warning(this, "Save", "Could not save file");
            }
        }
    }

private:
    void createActions() {
        actionOpen = new QAction("Open", this);
        actionSave = new QAction("Save", this);
        actionExit = new QAction("Exit", this);
        actionPreview = new QAction("Fullscreen Preview", this);
        actionPreview->setShortcut(Qt::Key_F);

        connect(actionOpen, &QAction::triggered, this, &ShaderIDE::openFile);
        connect(actionSave, &QAction::triggered, this, &ShaderIDE::saveFile);
        connect(actionExit, &QAction::triggered, this, &QWidget::close);
        connect(actionPreview, &QAction::triggered, this, &ShaderIDE::togglePreviewFullscreen);
    }

    void createToolBar() {
        QToolBar *tb = addToolBar("Main");
        tb->setMovable(false);
        tb->addAction(actionOpen);
        tb->addAction(actionSave);
        tb->addSeparator();
        tb->addAction(actionPreview);
    }

    void createMenus() {
        QMenu *fileMenu = menuBar()->addMenu("&File");
        fileMenu->addAction(actionOpen);
        fileMenu->addAction(actionSave);
        fileMenu->addSeparator();
        fileMenu->addAction(actionExit);

        QMenu *viewMenu = menuBar()->addMenu("&View");
        viewMenu->addAction(actionPreview);
    }

    std::set<int> parseErrorLines(const QString& log) {
        std::set<int> result;
        QRegularExpression re1(R"((?:\(|:)(\d{1,4})(?:\)|:))");
        QRegularExpressionMatchIterator it = re1.globalMatch(log);
        while (it.hasNext()) {
            QRegularExpressionMatch m = it.next();
            bool ok = false;
            int ln = m.captured(1).toInt(&ok);
            if (ok && ln > 0) result.insert(ln);
        }
        return result;
    }

    QSplitter *splitter = nullptr;
    QPlainTextEdit *editor = nullptr;
    ShaderView *preview = nullptr;
    GLSLHighlighter *highlighter = nullptr;
    AudioWorker *worker = nullptr;
    QTimer *compileTimer = nullptr;
    QLabel *statusLabel = nullptr;

    QMainWindow *previewWindow = nullptr;
    ShaderView *previewViewer = nullptr;

    QAction *actionOpen = nullptr;
    QAction *actionSave = nullptr;
    QAction *actionExit = nullptr;
    QAction *actionPreview = nullptr;
};

// ============================================================
// MAIN
// ============================================================
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // Register metatypes for queued connections
    qRegisterMetaType<std::vector<float>>("std::vector<float>");
    qRegisterMetaType<std::set<int>>("std::set<int>");

    qApp->setStyle(QStyleFactory::create("Fusion"));
    QPalette dark;
    dark.setColor(QPalette::Window, QColor(53,53,53));
    dark.setColor(QPalette::WindowText, Qt::white);
    dark.setColor(QPalette::Base, QColor(25,25,25));
    dark.setColor(QPalette::AlternateBase, QColor(53,53,53));
    dark.setColor(QPalette::ToolTipBase, Qt::white);
    dark.setColor(QPalette::ToolTipText, Qt::white);
    dark.setColor(QPalette::Text, Qt::white);
    dark.setColor(QPalette::Button, QColor(53,53,53));
    dark.setColor(QPalette::ButtonText, Qt::white);
    dark.setColor(QPalette::BrightText, Qt::red);
    qApp->setPalette(dark);

    ShaderIDE w;
    w.show();

    return a.exec();
}

// moc include for single-file
#include "main.moc"
