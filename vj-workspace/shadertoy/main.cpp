// main.cpp
// Qt6 + ALSA + OpenGL - Amalgamierte Single-File Version (gepatcht)
// Enthält: ALSA S16_LE Capture -> float, Limiter, GPU-FFT (Compute Shader), Energy Reduction,
// SSBO Ping-Pong, Readback, Onset/Kick-Detection, Live GLSL-Editor + Preview.
// Build: cmake + Qt6 (Widgets, OpenGL) + alsa

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
#include <atomic>
#include <mutex>

// Metatype-Registrierungen für queued connections
Q_DECLARE_METATYPE(std::vector<float>)
Q_DECLARE_METATYPE(std::set<int>)

// -----------------------------
// Konfiguration / Konstanten
// -----------------------------
constexpr int BUFFERSIZE = 128;
constexpr int FFT_SIZE = 2048;
constexpr unsigned DEFAULT_SR = 44100;
constexpr int MAX_ENERGY_GROUPS = 64;
constexpr int ENERGY_GROUPS_FOR_DISPATCH = 8; // Anzahl Workgroups für Energy-Shader (CPU readback)
constexpr int LOCAL_SIZE_X = 256; // muss mit compute shader shared array übereinstimmen
constexpr int AUDIO_UNIFORM_SIZE = 128; // für fragment preview uniform

// ============================================================
// AudioHardware: sichere Headroom-Setter (nur Integer volume API)
// ============================================================
class AudioHardware {
public:
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

                if (snd_mixer_selem_has_capture_volume(elem)) {
                    long minVol = 0, maxVol = 0;
                    if (snd_mixer_selem_get_capture_volume_range(elem, &minVol, &maxVol) == 0 && maxVol > minVol) {
                        double fraction = 0.20;
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
// Limiter: Portierung des C-Limiters in C++
// ============================================================
class Limiter {
public:
    void init(unsigned sample_rate) {
        float attack_s = 5.0f * 0.001f;
        float release_s = 80.0f * 0.001f;
        attack_coeff = std::exp(-1.0f / (attack_s * (float)sample_rate + 1e-9f));
        release_coeff = std::exp(-1.0f / (release_s * (float)sample_rate + 1e-9f));
        env = 0.0f;
        gain_smooth = 1.0f;
        threshold_lin = std::pow(10.0f, -6.0f / 20.0f); // default threshold -6 dB
        ratio = 8.0f;
        makeup_lin = 1.0f;
    }

    void process(float *buf, size_t n) {
        for (size_t i = 0; i < n; ++i) {
            float x = buf[i];
            float absx = std::fabs(x);
            if (absx > env) env = attack_coeff * env + (1.0f - attack_coeff) * absx;
            else env = release_coeff * env + (1.0f - release_coeff) * absx;
            float envv = std::fmax(env, 1e-9f);
            float env_db = 20.0f * std::log10(envv);
            float thr_db = -6.0f;
            float gain_db = 0.0f;
            if (env_db > thr_db) {
                float over_db = env_db - thr_db;
                float reduced_db = over_db * (1.0f - 1.0f / ratio);
                gain_db = -reduced_db;
            } else gain_db = 0.0f;
            float desired_gain = std::pow(10.0f, gain_db / 20.0f) * makeup_lin;
            if (desired_gain < gain_smooth) gain_smooth = attack_coeff * gain_smooth + (1.0f - attack_coeff) * desired_gain;
            else gain_smooth = release_coeff * gain_smooth + (1.0f - release_coeff) * desired_gain;
            buf[i] = x * gain_smooth;
        }
    }

private:
    float env = 0.0f;
    float gain_smooth = 1.0f;
    float attack_coeff = 0.0f;
    float release_coeff = 0.0f;
    float threshold_lin = 1.0f;
    float ratio = 8.0f;
    float makeup_lin = 1.0f;
};

// ============================================================
// AudioWorker: ALSA Capture -> int16 samples -> normalize to float
// Limiter wird hier angewendet, dann emit bufferReady
// ============================================================
class AudioWorker : public QThread {
    Q_OBJECT
public:
    bool running = true;
    const int bufferSize = BUFFERSIZE;

    AudioWorker(QObject* parent = nullptr) : QThread(parent) {
        limiter.init(DEFAULT_SR);
    }
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
        if (snd_pcm_open(&capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0) < 0) {
            qWarning() << "snd_pcm_open failed";
            return;
        }

        snd_pcm_hw_params_t *hw_params = nullptr;
        snd_pcm_hw_params_alloca(&hw_params);
        if (snd_pcm_hw_params_any(capture_handle, hw_params) < 0) {
            qWarning() << "snd_pcm_hw_params_any failed";
            snd_pcm_close(capture_handle);
            return;
        }

        if (snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
            qWarning() << "snd_pcm_hw_params_set_access failed";
            snd_pcm_close(capture_handle);
            return;
        }

        if (snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_S16_LE) < 0) {
            qWarning() << "snd_pcm_hw_params_set_format(S16_LE) failed - device may not support S16_LE";
        }

        unsigned int rate = DEFAULT_SR;
        if (snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &rate, 0) < 0) {
            qWarning() << "snd_pcm_hw_params_set_rate_near failed";
            snd_pcm_close(capture_handle);
            return;
        }

        snd_pcm_uframes_t frames = static_cast<snd_pcm_uframes_t>(bufferSize);
        if (snd_pcm_hw_params_set_period_size_near(capture_handle, hw_params, &frames, 0) < 0) {
            qWarning() << "snd_pcm_hw_params_set_period_size_near failed";
            snd_pcm_close(capture_handle);
            return;
        }

        if (snd_pcm_hw_params(capture_handle, hw_params) < 0) {
            qWarning() << "snd_pcm_hw_params failed";
            snd_pcm_close(capture_handle);
            return;
        }

        snd_pcm_prepare(capture_handle);

        std::vector<short> raw(bufferSize);
        std::vector<float> processed(bufferSize);

        // Kick detection envelope variables
        double prevLowEnergy = 0.0;
        const double attack = 0.9;
        const double release = 0.995;
        double lowEnv = 0.0;
        const int lowBandSamples = 20;
        const double kickThreshold = 0.12;

        while (running && !isInterruptionRequested()) {
            int err = snd_pcm_readi(capture_handle, raw.data(), bufferSize);
            if (err == -EPIPE) {
                snd_pcm_prepare(capture_handle);
                continue;
            } else if (err < 0) {
                qWarning() << "snd_pcm_readi error:" << snd_strerror(err);
                break;
            }

            int readFrames = (err > 0) ? err : bufferSize;
            for (int i = 0; i < readFrames; ++i) {
                float f = static_cast<float>(raw[i]) / 32768.0f;
                if (f > 1.0f) f = 1.0f;
                if (f < -1.0f) f = -1.0f;
                processed[i] = f;
            }
            for (int i = readFrames; i < bufferSize; ++i) processed[i] = 0.0f;

            // Limiter applied here
            limiter.process(processed.data(), processed.size());

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

            emit bufferReady(processed);

            QThread::msleep(1);
        }

        snd_pcm_close(capture_handle);
    }

private:
    Limiter limiter;
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
// ShaderView: QOpenGLWidget mit Compute-FFT Pipeline + Preview
// - Kompiliert Fragment-Shader für Live-Preview
// - Nimmt std::vector<float> Audio-Buffers entgegen
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
    // Wird per queued connection aufgerufen (AudioWorker -> ShaderView)
    void updateAudio(const std::vector<float>& data) {
        // store latest audio buffer (thread-safe via queued connection)
        audioData = data;

        // Run GPU FFT pipeline here (we must make context current)
        if (!isValid()) return;

        makeCurrent();
        // Ensure compute programs and SSBOs exist
        if (!fftProgram || !energyProgram || ssboA == 0) {
            doneCurrent();
            update(); // schedule paint
            return;
        }

        // Prepare texSamples (FFT_SIZE) from ring of incoming audio
        // We'll create a simple sliding window: copy last FFT_SIZE samples from audioData repeated if needed
        // For live preview we fill with zeros if not enough data
        std::vector<float> texSamples(FFT_SIZE, 0.0f);
        int copyN = std::min<int>(static_cast<int>(audioData.size()), FFT_SIZE);
        for (int i = 0; i < copyN; ++i) texSamples[i] = audioData[i];
        // If you want a proper ring buffer across frames, integrate a shared ring buffer; for simplicity we use latest chunk.

        // Upload SSBO input (complex array: real, imag)
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboA);
        void* ptr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * 2 * FFT_SIZE, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        if (ptr) {
            float* fp = reinterpret_cast<float*>(ptr);
            for (int i = 0; i < FFT_SIZE; ++i) {
                fp[2*i + 0] = texSamples[i];
                fp[2*i + 1] = 0.0f;
            }
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        }

        // Zero energy buffer for groups we'll use
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboEnergy);
        std::vector<float> zeros(ENERGY_GROUPS_FOR_DISPATCH, 0.0f);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * ENERGY_GROUPS_FOR_DISPATCH, zeros.data());

        // FFT compute dispatch (ping-pong)
        int stages = static_cast<int>(std::log2(FFT_SIZE));
        GLuint cur_in = ssboA;
        GLuint cur_out = ssboB;
        glUseProgram(fftProgram->programId());
        for (int stage = 0; stage < stages; ++stage) {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, cur_in);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, cur_out);
            // set uniforms
            GLint loc_stage = glGetUniformLocation(fftProgram->programId(), "u_stage");
            GLint loc_N = glGetUniformLocation(fftProgram->programId(), "u_N");
            if (loc_stage >= 0) glUniform1i(loc_stage, stage);
            if (loc_N >= 0) glUniform1i(loc_N, FFT_SIZE);

            int pairs = FFT_SIZE / 2;
            int local = LOCAL_SIZE_X;
            int groups = (pairs + local - 1) / local;
            if (groups < 1) groups = 1;
            glDispatchCompute(groups, 1, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            std::swap(cur_in, cur_out);
        }

        // Energy compute dispatch
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, cur_in);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboEnergy);
        glUseProgram(energyProgram->programId());
        // compute bin range for 20..150 Hz
        double bin_width = static_cast<double>(DEFAULT_SR) / static_cast<double>(FFT_SIZE);
        int bin_lo = static_cast<int>(std::floor(20.0 / bin_width));
        int bin_hi = static_cast<int>(std::ceil(150.0 / bin_width));
        if (bin_lo < 0) bin_lo = 0;
        if (bin_hi > FFT_SIZE/2 - 1) bin_hi = FFT_SIZE/2 - 1;
        GLint loc_lo = glGetUniformLocation(energyProgram->programId(), "u_bin_lo");
        GLint loc_hi = glGetUniformLocation(energyProgram->programId(), "u_bin_hi");
        GLint loc_N2 = glGetUniformLocation(energyProgram->programId(), "u_N");
        if (loc_lo >= 0) glUniform1i(loc_lo, bin_lo);
        if (loc_hi >= 0) glUniform1i(loc_hi, bin_hi);
        if (loc_N2 >= 0) glUniform1i(loc_N2, FFT_SIZE);

        int groups2 = ENERGY_GROUPS_FOR_DISPATCH;
        if (groups2 > MAX_ENERGY_GROUPS) groups2 = MAX_ENERGY_GROUPS;
        glDispatchCompute(groups2, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

        // Read energy buffer and sum on CPU
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboEnergy);
        void* emap = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * groups2, GL_MAP_READ_BIT);
        float energy_val = 0.0f;
        if (emap) {
            float* ev = reinterpret_cast<float*>(emap);
            for (int i = 0; i < groups2; ++i) energy_val += ev[i];
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        }

        // Onset detection (simple running average)
        static double running_avg = 1e-8;
        running_avg = 0.05 * energy_val + (1.0 - 0.05) * running_avg;
        int kick = 0;
        static uint32_t last_kick_ms = 0;
        uint32_t now_ms = static_cast<uint32_t>(QDateTime::currentMSecsSinceEpoch() & 0xffffffff);
        double onset_factor = 3.0;
        if (energy_val > running_avg * onset_factor && energy_val > 1e-8) {
            if ((now_ms - last_kick_ms) > 200) { kick = 1; last_kick_ms = now_ms; }
        }
        // update kickLevel for visual
        if (kick) kickLevel = 1.0f;
        // copy small audio buffer into uniform array for fragment shader
        float fBuffer[AUDIO_UNIFORM_SIZE];
        for (int i = 0; i < AUDIO_UNIFORM_SIZE; ++i) fBuffer[i] = 0.0f;
        for (int i = 0; i < static_cast<int>(audioData.size()) && i < AUDIO_UNIFORM_SIZE; ++i) fBuffer[i] = audioData[i];

        // set uniforms on fragment program (if linked)
        if (program && program->isLinked()) {
            program->bind();
            program->setUniformValueArray("audio", fBuffer, AUDIO_UNIFORM_SIZE, 1);
            program->setUniformValue("kick", kickLevel);
            program->release();
        }

        doneCurrent();
        update(); // schedule paintGL
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

        // Compile fragment shader (lastGoodFragment)
        QPair<bool, QString> res = tryCompileFragment(lastGoodFragment);
        if (!res.first) qWarning() << "Initial shader compile failed:" << res.second;

        // Compile compute shaders and create SSBOs
        compileComputeShadersAndBuffers();

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

        // audio uniform already set in updateAudio; draw fullscreen quad
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
    void compileComputeShadersAndBuffers() {
        // Compute shader sources (from original C)
        const char *compute_fft_src =
        "#version 430\n"
        "layout(local_size_x = 256) in;\n"
        "layout(std430, binding = 0) buffer BufIn { vec2 data[]; } inBuf;\n"
        "layout(std430, binding = 1) buffer BufOut { vec2 data[]; } outBuf;\n"
        "uniform int u_stage;\n"
        "uniform int u_N;\n"
        "void main() {\n"
        "  uint gid = gl_GlobalInvocationID.x;\n"
        "  int N = u_N;\n"
        "  int stage = u_stage;\n"
        "  int m = 1 << (stage + 1);\n"
        "  int hlf = m >> 1;\n"
        "  int pairIndex = int(gid);\n"
        "  int block = pairIndex / hlf;\n"
        "  int j = pairIndex % hlf;\n"
        "  int k = block * m + j;\n"
        "  int idx1 = k;\n"
        "  int idx2 = k + hlf;\n"
        "  if (idx2 >= N) return;\n"
        "  vec2 a = inBuf.data[idx1];\n"
        "  vec2 b = inBuf.data[idx2];\n"
        "  float angle = -2.0 * 3.14159265358979323846 * float(j) / float(m);\n"
        "  float wr = cos(angle);\n"
        "  float wi = sin(angle);\n"
        "  vec2 t;\n"
        "  t.x = b.x * wr - b.y * wi;\n"
        "  t.y = b.x * wi + b.y * wr;\n"
        "  outBuf.data[idx1] = a + t;\n"
        "  outBuf.data[idx2] = a - t;\n"
        "}\n";

        const char *compute_energy_src =
        "#version 430\n"
        "layout(local_size_x = 256) in;\n"
        "layout(std430, binding = 0) buffer FFTBuf { vec2 data[]; } fftBuf;\n"
        "layout(std430, binding = 2) buffer EnergyBuf { float energy[]; } energyBuf;\n"
        "uniform int u_bin_lo;\n"
        "uniform int u_bin_hi;\n"
        "uniform int u_N;\n"
        "shared float ssum[256];\n"
        "void main() {\n"
        "  uint gid = gl_GlobalInvocationID.x;\n"
        "  uint lid = gl_LocalInvocationID.x;\n"
        "  uint localSize = gl_WorkGroupSize.x;\n"
        "  float sum = 0.0;\n"
        "  for (int b = int(gid); b <= u_bin_hi; b += int(gl_NumWorkGroups.x * localSize)) {\n"
        "    if (b >= u_bin_lo && b <= u_bin_hi) {\n"
        "      float re = fftBuf.data[b].x;\n"
        "      float im = fftBuf.data[b].y;\n"
        "      sum += sqrt(re*re + im*im);\n"
        "    }\n"
        "  }\n"
        "  ssum[lid] = sum;\n"
        "  memoryBarrierShared();\n"
        "  barrier();\n"
        "  for (uint offset = localSize/2; offset > 0; offset >>= 1) {\n"
        "    if (lid < offset) ssum[lid] += ssum[lid + offset];\n"
        "    memoryBarrierShared();\n"
        "    barrier();\n"
        "  }\n"
        "  if (lid == 0) {\n"
        "    energyBuf.energy[gl_WorkGroupID.x] = ssum[0];\n"
        "  }\n"
        "}\n";

        // Compile compute shaders using QOpenGLShaderProgram (Compute)
        if (fftProgram) { delete fftProgram; fftProgram = nullptr; }
        fftProgram = new QOpenGLShaderProgram();
        if (!fftProgram->addShaderFromSourceCode(QOpenGLShader::Compute, compute_fft_src)) {
            qWarning() << "FFT compute shader compile failed:" << fftProgram->log();
            delete fftProgram; fftProgram = nullptr;
        } else {
            if (!fftProgram->link()) {
                qWarning() << "FFT compute shader link failed:" << fftProgram->log();
                delete fftProgram; fftProgram = nullptr;
            }
        }

        if (energyProgram) { delete energyProgram; energyProgram = nullptr; }
        energyProgram = new QOpenGLShaderProgram();
        if (!energyProgram->addShaderFromSourceCode(QOpenGLShader::Compute, compute_energy_src)) {
            qWarning() << "Energy compute shader compile failed:" << energyProgram->log();
            delete energyProgram; energyProgram = nullptr;
        } else {
            if (!energyProgram->link()) {
                qWarning() << "Energy compute shader link failed:" << energyProgram->log();
                delete energyProgram; energyProgram = nullptr;
            }
        }

        // Create SSBOs
        glGenBuffers(1, &ssboA);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboA);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 2 * FFT_SIZE, nullptr, GL_DYNAMIC_DRAW);

        glGenBuffers(1, &ssboB);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboB);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 2 * FFT_SIZE, nullptr, GL_DYNAMIC_DRAW);

        glGenBuffers(1, &ssboEnergy);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboEnergy);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * MAX_ENERGY_GROUPS, nullptr, GL_DYNAMIC_DRAW);

        // Unbind
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

private:
    QOpenGLShaderProgram *program = nullptr;
    QOpenGLShaderProgram *fftProgram = nullptr;
    QOpenGLShaderProgram *energyProgram = nullptr;
    QOpenGLBuffer vbo;
    QOpenGLVertexArrayObject vao;
    std::vector<float> audioData;
    float kickLevel = 0.0f;
    QString lastGoodFragment;
    QString pendingSource;

    // GL objects for FFT
    GLuint ssboA = 0, ssboB = 0, ssboEnergy = 0;

    // helper for program id access
    GLuint programId() const { return program ? program->programId() : 0; }
};

// ============================================================
// ShaderIDE: main window
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

#include "main.moc"
