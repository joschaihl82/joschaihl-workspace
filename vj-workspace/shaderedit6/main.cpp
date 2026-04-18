// main.cpp
// Beat Visualizer (single-file, Qt6 + ALSA)
// Left: GLSL editor, Right: OpenGL preview
// - Cleaned, modernized and documented for readability and maintainability
// - Keep build: Qt6 Widgets, OpenGL, OpenGLWidgets; link with asound (ALSA)

#include <QApplication>
#include <QSplitter>
#include <QPlainTextEdit>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QDateTime>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QDialog>
#include <QDebug>

#include <alsa/asoundlib.h>

#include <cmath>
#include <vector>
#include <algorithm>
#include <string>
#include <memory>

// -----------------------------------------------------------------------------
// Configuration constants
// -----------------------------------------------------------------------------
constexpr int BUFFERSIZE = 512;
constexpr int FFT_SIZE = 2048;
constexpr int KICK_DEBOUNCE_MS = 200;
constexpr unsigned DEFAULT_SR = 48000;
constexpr float TARGET_DB = 48.0f;
constexpr float MAX_GAIN = 4.0f;
constexpr float ATTACK_FAST = 0.6f;
constexpr float RELEASE_SLOW = 0.02f;

// -----------------------------------------------------------------------------
// Global audio/mixer state (kept minimal and explicit)
// -----------------------------------------------------------------------------
static snd_mixer_t *g_mixer = nullptr;
static snd_mixer_elem_t *g_selem = nullptr;
static long g_vol_min = 0, g_vol_max = 0;

// ring buffer for audio samples (mono, normalized floats)
static std::vector<float> g_ring;
static size_t g_ring_cap = 0;
static size_t g_ring_pos = 0;
static QMutex g_ring_mutex;

// FFT scratch buffers (fixed-size arrays)
static double fft_re[FFT_SIZE];
static double fft_im[FFT_SIZE];
static double tw_re[FFT_SIZE / 2];
static double tw_im[FFT_SIZE / 2];
static double window_hann[FFT_SIZE];

// -----------------------------------------------------------------------------
// Default fragment shader (simple visualizer example)
// -----------------------------------------------------------------------------
static const char *DEFAULT_SHADER = R"(// Fragment shader example
#version 120
uniform sampler1D u_samples;
uniform float u_vscale;
uniform float u_gain_display;
uniform float u_hue;
uniform int u_kick;
varying vec2 v_uv;

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
    if (u_kick == 1) {
        gl_FragColor = vec4(1.0);
        return;
    }
    float s = texture1D(u_samples, v_uv.x).r;
    float dist = abs(v_uv.y - (0.5 - s * u_vscale));
    float alpha = smoothstep(1.0/600.0, 0.0, dist);
    float c_alpha = smoothstep(0.002, 0.0, abs(v_uv.y - 0.5));
    vec3 wcol = hsv2rgb(u_hue, 1.0, 1.0);
    vec3 col = mix(wcol * 0.5, wcol, alpha);
    float f_alpha = max(alpha, c_alpha);
    if (v_uv.x < 0.15 && v_uv.y < 0.03 && (v_uv.x / 0.15) < u_gain_display) {
        col = vec3(0.0, 1.0, 0.0);
        f_alpha = 1.0;
    }
    gl_FragColor = vec4(col, f_alpha);
}
)";

// -----------------------------------------------------------------------------
// ALSA mixer helpers
// -----------------------------------------------------------------------------
static bool alsa_find_capture_control(const char *name) {
    snd_mixer_selem_id_t *sid;
    if (snd_mixer_open(&g_mixer, 0) < 0) return false;
    if (snd_mixer_attach(g_mixer, "default") < 0
        || snd_mixer_selem_register(g_mixer, nullptr, nullptr) < 0
        || snd_mixer_load(g_mixer) < 0) {
        snd_mixer_close(g_mixer);
        g_mixer = nullptr;
        return false;
    }
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, name);
    g_selem = snd_mixer_find_selem(g_mixer, sid);
    if (!g_selem) return false;
    if (snd_mixer_selem_has_capture_volume(g_selem)) {
        snd_mixer_selem_get_capture_volume_range(g_selem, &g_vol_min, &g_vol_max);
        return true;
    }
    return false;
}

static void alsa_set_capture_all(long v) {
    if (!g_selem) return;
    long clamped = std::max(g_vol_min, std::min(g_vol_max, v));
    snd_mixer_selem_set_capture_volume_all(g_selem, clamped);
}

// -----------------------------------------------------------------------------
// FFT utilities (simple radix-2 iterative FFT)
// -----------------------------------------------------------------------------
static void fft_prepare() {
    for (int k = 0; k < FFT_SIZE / 2; ++k) {
        double a = -2.0 * M_PI * k / FFT_SIZE;
        tw_re[k] = std::cos(a);
        tw_im[k] = std::sin(a);
    }
    for (int n = 0; n < FFT_SIZE; ++n) {
        window_hann[n] = 0.5 * (1.0 - std::cos(2.0 * M_PI * n / (FFT_SIZE - 1)));
    }
}

static void bit_reverse_permute(double *re, double *im) {
    unsigned j = 0;
    for (unsigned i = 0; i < FFT_SIZE; ++i) {
        if (i < j) {
            std::swap(re[i], re[j]);
            std::swap(im[i], im[j]);
        }
        unsigned m = FFT_SIZE >> 1;
        while (m >= 1 && j >= m) { j -= m; m >>= 1; }
        j += m;
    }
}

static void fft_execute(double *re, double *im) {
    bit_reverse_permute(re, im);
    for (int len = 2; len <= FFT_SIZE; len <<= 1) {
        int half = len >> 1;
        int step = FFT_SIZE / len;
        for (int i = 0; i < FFT_SIZE; i += len) {
            for (int j = 0; j < half; ++j) {
                int tw = j * step;
                int k = i + j;
                double ur = re[k], ui = im[k];
                double tre = tw_re[tw] * re[k + half] - tw_im[tw] * im[k + half];
                double tim = tw_re[tw] * im[k + half] + tw_im[tw] * re[k + half];
                re[k] = ur + tre;
                im[k] = ui + tim;
                re[k + half] = ur - tre;
                im[k + half] = ui - tim;
            }
        }
    }
}

static double fft_compute_bass_energy(unsigned sr, double f_lo, double f_hi) {
    std::vector<double> loc(FFT_SIZE);
    {
        QMutexLocker l(&g_ring_mutex);
        size_t st = (g_ring_pos + g_ring_cap - FFT_SIZE) % g_ring_cap;
        for (int n = 0; n < FFT_SIZE; ++n) loc[n] = g_ring[(st + n) % g_ring_cap];
    }
    for (int n = 0; n < FFT_SIZE; ++n) {
        fft_re[n] = loc[n] * window_hann[n];
        fft_im[n] = 0.0;
    }
    fft_execute(fft_re, fft_im);

    double binWidth = static_cast<double>(sr) / FFT_SIZE;
    int b_lo = std::max(0, static_cast<int>(std::floor(f_lo / binWidth)));
    int b_hi = std::min(FFT_SIZE / 2 - 1, static_cast<int>(std::ceil(f_hi / binWidth)));

    double sum = 0.0;
    for (int b = b_lo; b <= b_hi; ++b) {
        sum += std::sqrt(fft_re[b] * fft_re[b] + fft_im[b] * fft_im[b]);
    }
    return sum;
}

// -----------------------------------------------------------------------------
// Ring buffer helpers
// -----------------------------------------------------------------------------
static void push_to_ring_from_s16(const int16_t *in, size_t n) {
    QMutexLocker l(&g_ring_mutex);
    for (size_t i = 0; i < n; ++i) {
        g_ring[g_ring_pos] = in[i] / 32768.0f;
        g_ring_pos = (g_ring_pos + 1) % g_ring_cap;
    }
}

static float compute_rms_s16(const int16_t *buf, size_t n) {
    double s = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double v = buf[i] / 32768.0;
        s += v * v;
    }
    return static_cast<float>(std::sqrt(s / static_cast<double>(n)));
}

static float compute_rms_from_ring(size_t n) {
    if (g_ring_cap == 0) return 0.0f;
    n = static_cast<size_t>(std::min(n, g_ring_cap));
    size_t st;
    {
        QMutexLocker l(&g_ring_mutex);
        st = (g_ring_pos + g_ring_cap - n) % g_ring_cap;
    }
    double s = 0.0;
    for (size_t i = 0; i < n; ++i) {
        QMutexLocker l(&g_ring_mutex);
        double v = g_ring[(st + i) % g_ring_cap];
        s += v * v;
    }
    return static_cast<float>(std::sqrt(s / static_cast<double>(n)));
}

// -----------------------------------------------------------------------------
// GL widget: renders a fullscreen quad and runs fragment shader from editor
// -----------------------------------------------------------------------------
class GLWaveWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    explicit GLWaveWidget(unsigned sr, QWidget *parent = nullptr)
        : QOpenGLWidget(parent), rate(sr), tex_size(FFT_SIZE), limiter_gain(1.f), kick(false), hue(0.f) {
        setFocusPolicy(Qt::StrongFocus);
        tex_samples.assign(tex_size, 0.0f);
    }

    // copy latest ring samples into the texture buffer
    void updateTextureFromRing() {
        QMutexLocker l(&g_ring_mutex);
        size_t st = (g_ring_pos + g_ring_cap - tex_size) % g_ring_cap;
        for (int i = 0; i < tex_size; ++i) tex_samples[i] = g_ring[(st + i) % g_ring_cap];
    }

    // compile and set fragment shader; returns mapped error string on failure
    bool setFragmentShaderSource(const QString &fsrc, QString &errOut) {
        // Ensure GL context is current
        makeCurrent();

        GLint ok = 0;
        char log[4096];

        // simple passthrough vertex shader (legacy GLSL 1.20)
        const char *vsrc =
            "#version 120\n"
            "attribute vec2 a_pos;\n"
            "varying vec2 v_uv;\n"
            "void main(){ v_uv = (a_pos + 1.0) * 0.5; gl_Position = vec4(a_pos, 0.0, 1.0); }\n";

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vsrc, nullptr);
        glCompileShader(vs);
        glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            glGetShaderInfoLog(vs, sizeof(log), nullptr, log);
            errOut = QString("Vertex shader compile error: %1").arg(log);
            glDeleteShader(vs);
            doneCurrent();
            return false;
        }

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        QByteArray fba = fsrc.toUtf8();
        const char *fsrc_c = fba.constData();
        glShaderSource(fs, 1, &fsrc_c, nullptr);
        glCompileShader(fs);
        glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            glGetShaderInfoLog(fs, sizeof(log), nullptr, log);
            errOut = QString("Fragment shader compile error: %1").arg(log);
            glDeleteShader(vs);
            glDeleteShader(fs);
            doneCurrent();
            return false;
        }

        GLuint newProg = glCreateProgram();
        glAttachShader(newProg, vs);
        glAttachShader(newProg, fs);
        glBindAttribLocation(newProg, 0, "a_pos");
        glLinkProgram(newProg);
        glGetProgramiv(newProg, GL_LINK_STATUS, &ok);
        if (!ok) {
            glGetProgramInfoLog(newProg, sizeof(log), nullptr, log);
            errOut = QString("Program link error: %1").arg(log);
            glDeleteProgram(newProg);
            glDeleteShader(vs);
            glDeleteShader(fs);
            doneCurrent();
            return false;
        }

        // swap program
        if (prog) glDeleteProgram(prog);
        prog = newProg;
        glDeleteShader(vs);
        glDeleteShader(fs);

        // cache uniform locations
        loc_u_samples = glGetUniformLocation(prog, "u_samples");
        loc_u_vscale = glGetUniformLocation(prog, "u_vscale");
        loc_u_gain_display = glGetUniformLocation(prog, "u_gain_display");
        loc_u_hue = glGetUniformLocation(prog, "u_hue");
        loc_u_kick = glGetUniformLocation(prog, "u_kick");

        doneCurrent();
        return true;
    }

protected:
    void initializeGL() override {
        initializeOpenGLFunctions();

        // create 1D texture for audio samples
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_1D, tex);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, tex_size, 0, GL_RED, GL_FLOAT, tex_samples.data());

        // fullscreen quad VBO
        float quad[] = { -1.f, -1.f,  1.f, -1.f,  -1.f, 1.f,  1.f, 1.f };
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

        // compile default shader
        QString err;
        if (!setFragmentShaderSource(DEFAULT_SHADER, err)) {
            qWarning() << "Default shader compile failed:" << err;
        }
    }

    void paintGL() override {
        glClearColor(1.f, 0.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (!prog) return;

        glUseProgram(prog);

        // upload audio texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_1D, tex);
        glTexSubImage1D(GL_TEXTURE_1D, 0, 0, tex_size, GL_RED, GL_FLOAT, tex_samples.data());
        if (loc_u_samples >= 0) glUniform1i(loc_u_samples, 0);

        // uniforms
        if (loc_u_vscale >= 0) glUniform1f(loc_u_vscale, 0.4f);
        if (loc_u_gain_display >= 0) glUniform1f(loc_u_gain_display, std::min(1.0f, limiter_gain));
        if (loc_u_hue >= 0) glUniform1f(loc_u_hue, hue);
        if (loc_u_kick >= 0) glUniform1i(loc_u_kick, kick ? 1 : 0);

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
        } else if (ev->key() == Qt::Key_Escape) {
            emit requestExitFullscreen();
            ev->accept();
        } else {
            QOpenGLWidget::keyPressEvent(ev);
        }
    }

signals:
    void requestToggleFullscreen();
    void requestExitFullscreen();

public:
    float limiter_gain;
    bool kick;
    float hue;

private:
    unsigned rate;
    int tex_size;
    std::vector<float> tex_samples;

    GLuint tex = 0;
    GLuint vbo = 0;
    GLuint prog = 0;

    GLint loc_u_samples = -1;
    GLint loc_u_vscale = -1;
    GLint loc_u_gain_display = -1;
    GLint loc_u_hue = -1;
    GLint loc_u_kick = -1;
};

// -----------------------------------------------------------------------------
// ALSA capture thread (keeps previous behavior but structured and commented)
// -----------------------------------------------------------------------------
class AlsaCaptureThread : public QThread {
    Q_OBJECT
public:
    explicit AlsaCaptureThread(unsigned sr, QObject *parent = nullptr)
        : QThread(parent), rate(sr) {}

    void run() override {
        snd_pcm_t *pcm = nullptr;
        if (snd_pcm_open(&pcm, "default", SND_PCM_STREAM_CAPTURE, 0) < 0) {
            qWarning() << "snd_pcm_open failed";
            return;
        }

        snd_pcm_hw_params_t *hw = nullptr;
        snd_pcm_hw_params_malloc(&hw);
        snd_pcm_hw_params_any(pcm, hw);
        snd_pcm_hw_params_set_access(pcm, hw, SND_PCM_ACCESS_RW_INTERLEAVED);
        snd_pcm_hw_params_set_format(pcm, hw, SND_PCM_FORMAT_S16_LE);
        unsigned r = rate;
        snd_pcm_hw_params_set_rate_near(pcm, hw, &r, 0);
        snd_pcm_hw_params_set_channels(pcm, hw, 1);
        snd_pcm_uframes_t period = BUFFERSIZE;
        snd_pcm_uframes_t buffer = period * 4;
        snd_pcm_hw_params_set_period_size_near(pcm, hw, &period, 0);
        snd_pcm_hw_params_set_buffer_size_near(pcm, hw, &buffer);

        if (snd_pcm_hw_params(pcm, hw) < 0) {
            snd_pcm_hw_params_free(hw);
            snd_pcm_close(pcm);
            qWarning() << "snd_pcm_hw_params failed";
            return;
        }
        snd_pcm_hw_params_free(hw);
        snd_pcm_prepare(pcm);

        std::vector<int16_t> read_buf(BUFFERSIZE);
        float g_limit = 1.0f;
        const float TGT_LVL = std::pow(10.0f, -TARGET_DB / 20.0f);
        alsa_set_capture_all(g_vol_min + (g_vol_max - g_vol_min) / 2);
        double run_avg = 1e-8;
        quint32 last_kick = 0;

        while (!isInterruptionRequested()) {
            snd_pcm_sframes_t f = snd_pcm_readi(pcm, read_buf.data(), BUFFERSIZE);
            if (f == -EPIPE) {
                snd_pcm_prepare(pcm);
                continue;
            } else if (f < 0) {
                f = snd_pcm_recover(pcm, f, 0);
                if (f < 0) break;
                continue;
            } else if (f == 0) {
                msleep(1);
                continue;
            }

            float chunk_rms = compute_rms_s16(read_buf.data(), static_cast<size_t>(f));
            float desired = std::min(MAX_GAIN, TGT_LVL / std::max(chunk_rms, 1e-12f));
            float factor = (desired > g_limit) ? ATTACK_FAST : RELEASE_SLOW;
            g_limit = factor * desired + (1.0f - factor) * g_limit;

            long setv = (g_limit >= 1.f)
                ? g_vol_max
                : g_vol_min + static_cast<long>((g_vol_max - g_vol_min) * std::max(0.f, g_limit) + 0.5f);
            alsa_set_capture_all(setv);

            push_to_ring_from_s16(read_buf.data(), static_cast<size_t>(f));

            double energy = fft_compute_bass_energy(rate, 20.0, 150.0);
            run_avg = 0.05 * energy + 0.95 * run_avg;
            bool is_kick = false;
            quint32 now = static_cast<quint32>(QDateTime::currentMSecsSinceEpoch() & 0xffffffff);
            if (energy > run_avg * 3.0 && energy > 1e-6 && (now - last_kick) > KICK_DEBOUNCE_MS) {
                is_kick = true;
                last_kick = now;
            }

            emit audioFrameAvailable(g_limit, is_kick, std::min(0.7f, compute_rms_from_ring((rate * 500) / 1000)));
            msleep(1);
        }

        snd_pcm_close(pcm);
    }

signals:
    void audioFrameAvailable(float limiterGain, bool kick, float hue);

private:
    unsigned rate;
};

// -----------------------------------------------------------------------------
// Main application
// -----------------------------------------------------------------------------
int main(int argc, char **argv) {
    QApplication app(argc, argv);

    // Try to find ALSA capture control; if not found, warn but continue (user may still want to edit shaders)
    if (!alsa_find_capture_control("Capture") && !alsa_find_capture_control("Mic")) {
        qWarning() << "No ALSA capture control found. Capture volume control will be disabled.";
    }

    // initialize ring buffer and FFT
    g_ring_cap = static_cast<size_t>(DEFAULT_SR) * 2; // 2 seconds
    g_ring.assign(g_ring_cap, 0.0f);
    fft_prepare();

    // main window layout: left editor, right preview (splitter)
    QWidget mainWin;
    mainWin.setWindowTitle("Beat Visualizer (Qt6)");
    mainWin.resize(1280, 720);

    QSplitter *split = new QSplitter(Qt::Horizontal, &mainWin);
    QPlainTextEdit *editor = new QPlainTextEdit(QString::fromUtf8(DEFAULT_SHADER));
    GLWaveWidget *glw = new GLWaveWidget(DEFAULT_SR);

    // Add widgets and set equal stretch so halves are visually balanced
    split->addWidget(editor);
    split->addWidget(glw);
    split->setStretchFactor(0, 1);
    split->setStretchFactor(1, 1);

    QVBoxLayout *layout = new QVBoxLayout(&mainWin);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(split);
    mainWin.show();

    // fullscreen container (frameless dialog) used when toggling fullscreen
    QDialog fsDialog(&mainWin, Qt::Window);
    fsDialog.setWindowFlag(Qt::FramelessWindowHint);
    fsDialog.setModal(false);
    QVBoxLayout fsLayout(&fsDialog);
    fsLayout.setContentsMargins(0, 0, 0, 0);

    // start ALSA capture thread
    AlsaCaptureThread *cap = new AlsaCaptureThread(DEFAULT_SR);
    QObject::connect(cap, &AlsaCaptureThread::audioFrameAvailable, [&](float lg, bool k, float h) {
        glw->limiter_gain = lg;
        glw->kick = k;
        glw->hue = h;
        glw->updateTextureFromRing();
        glw->update();
    });
    cap->start();

    // fullscreen toggle handling
    bool isFs = false;
    QObject::connect(glw, &GLWaveWidget::requestToggleFullscreen, [&]() {
        if (!isFs) {
            glw->setParent(&fsDialog);
            fsLayout.addWidget(glw);
            fsDialog.showFullScreen();
            glw->show();
        } else {
            glw->setParent(split);
            split->insertWidget(1, glw);
            fsDialog.hide();
        }
        isFs = !isFs;
    });

    QObject::connect(glw, &GLWaveWidget::requestExitFullscreen, [&]() {
        if (isFs) {
            glw->setParent(split);
            split->insertWidget(1, glw);
            fsDialog.hide();
            isFs = false;
        }
    });

    // compile shader on editor change (immediate feedback)
    QObject::connect(editor, &QPlainTextEdit::textChanged, [&]() {
        QString err;
        if (!glw->setFragmentShaderSource(editor->toPlainText(), err)) {
            qWarning() << "Shader error:" << err;
        }
        glw->updateTextureFromRing();
        glw->update();
    });

    // initial compile
    QString initialErr;
    glw->setFragmentShaderSource(editor->toPlainText(), initialErr);
    if (!initialErr.isEmpty()) qWarning() << "Initial shader error:" << initialErr;

    // run event loop
    int result = app.exec();

    // cleanup
    cap->requestInterruption();
    cap->wait();
    delete cap;

    if (g_mixer) snd_mixer_close(g_mixer);

    return result;
}

#include "main.moc"
