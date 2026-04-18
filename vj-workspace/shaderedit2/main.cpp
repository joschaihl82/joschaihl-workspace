// main.cpp
// Qt6 single-file application with:
// - Left: GLSL fragment shader editor with syntax highlighting
// - Right: OpenGL preview showing the last successfully compiled shader
// - Underline GLSL compiler errors in red (wavy) in the editor
// - Press 'f' in the preview to toggle fullscreen, Esc to exit fullscreen
//
// Build with CMakeLists.txt (Qt6 Widgets, OpenGL, OpenGLWidgets) and link with asound (ALSA).

#include <QApplication>
#include <QSplitter>
#include <QPlainTextEdit>
#include <QSurfaceFormat>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QDateTime>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QDialog>
#include <QDebug>
#include <QObject>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>
#include <QTextBlock>
#include <QTextCursor>
#include <alsa/asoundlib.h>
#include <cmath>
#include <vector>
#include <atomic>
#include <cstring>

// --- Config (wie im Original) ---
constexpr int BUFFERSIZE = 512;
constexpr int FFT_SIZE = 2048;
constexpr unsigned DEFAULT_SR = 48000;
constexpr float TARGET_DB = 48.0f;
constexpr float MAX_GAIN = 4.0f;
constexpr float ATTACK_FAST = 0.6f;
constexpr float RELEASE_SLOW = 0.02f;
constexpr int KICK_DEBOUNCE_MS = 200;

// --- ALSA mixer globals ---
static snd_mixer_t *g_mixer = nullptr;
static snd_mixer_elem_t *g_selem = nullptr;
static long g_vol_min = 0, g_vol_max = 0;

// --- ring buffer ---
static std::vector<float> g_ring;
static size_t g_ring_cap = 0;
static size_t g_ring_pos = 0;
static QMutex g_ring_mutex;

// --- FFT buffers ---
static double fft_re[FFT_SIZE];
static double fft_im[FFT_SIZE];
static double tw_re[FFT_SIZE/2];
static double tw_im[FFT_SIZE/2];
static double window_hann[FFT_SIZE];

// --- ALSA helper functions ---
static bool alsa_find_capture_control(const char *name) {
    snd_mixer_selem_id_t *sid;
    if (snd_mixer_open(&g_mixer, 0) < 0) return false;
    if (snd_mixer_attach(g_mixer, "default") < 0) { snd_mixer_close(g_mixer); g_mixer = nullptr; return false; }
    snd_mixer_selem_register(g_mixer, NULL, NULL);
    if (snd_mixer_load(g_mixer) < 0) { snd_mixer_close(g_mixer); g_mixer = nullptr; return false; }
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
    if (v < g_vol_min) v = g_vol_min;
    if (v > g_vol_max) v = g_vol_max;
    snd_mixer_selem_set_capture_volume_all(g_selem, v);
}

// --- FFT helpers ---
static void fft_prepare() {
    for (int k = 0; k < FFT_SIZE/2; ++k) {
        double angle = -2.0 * M_PI * k / (double)FFT_SIZE;
        tw_re[k] = cos(angle);
        tw_im[k] = sin(angle);
    }
    for (int n = 0; n < FFT_SIZE; ++n) {
        window_hann[n] = 0.5 * (1.0 - cos(2.0 * M_PI * n / (FFT_SIZE - 1)));
    }
}

static void bit_reverse_permute(double *re, double *im) {
    unsigned int j = 0;
    for (unsigned int i = 0; i < FFT_SIZE; ++i) {
        if (i < j) {
            std::swap(re[i], re[j]);
            std::swap(im[i], im[j]);
        }
        unsigned int m = FFT_SIZE >> 1;
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
                double tre = tw_re[tw] * re[i + j + half] - tw_im[tw] * im[i + j + half];
                double tim = tw_re[tw] * im[i + j + half] + tw_im[tw] * re[i + j + half];
                double ur = re[i + j];
                double ui = im[i + j];
                re[i + j] = ur + tre;
                im[i + j] = ui + tim;
                re[i + j + half] = ur - tre;
                im[i + j + half] = ui - tim;
            }
        }
    }
}

static double fft_compute_bass_energy(unsigned sample_rate, double f_lo, double f_hi) {
    std::vector<double> local(FFT_SIZE);
    {
        QMutexLocker locker(&g_ring_mutex);
        size_t start = (g_ring_pos + g_ring_cap - FFT_SIZE) % g_ring_cap;
        for (int n = 0; n < FFT_SIZE; ++n) local[n] = g_ring[(start + n) % g_ring_cap];
    }
    for (int n = 0; n < FFT_SIZE; ++n) {
        fft_re[n] = local[n] * window_hann[n];
        fft_im[n] = 0.0;
    }
    fft_execute(fft_re, fft_im);
    double bin_width = (double)sample_rate / (double)FFT_SIZE;
    int bin_lo = (int)floor(f_lo / bin_width);
    int bin_hi = (int)ceil(f_hi / bin_width);
    if (bin_lo < 0) bin_lo = 0;
    if (bin_hi > FFT_SIZE/2 - 1) bin_hi = FFT_SIZE/2 - 1;
    double sum = 0.0;
    for (int b = bin_lo; b <= bin_hi; ++b) {
        double re = fft_re[b];
        double im = fft_im[b];
        double mag = sqrt(re*re + im*im);
        sum += mag;
    }
    return sum;
}

static void push_to_ring_from_s16(const int16_t *in, size_t n) {
    QMutexLocker locker(&g_ring_mutex);
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
    return (float)sqrt(s / (double)n);
}

static float compute_rms_from_ring(size_t n) {
    if (n == 0) return 0.0f;
    if (n > g_ring_cap) n = g_ring_cap;
    size_t start;
    {
        QMutexLocker locker(&g_ring_mutex);
        start = (g_ring_pos + g_ring_cap - n) % g_ring_cap;
    }
    double s = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double v;
        {
            QMutexLocker locker(&g_ring_mutex);
            v = g_ring[(start + i) % g_ring_cap];
        }
        s += v * v;
    }
    return (float)sqrt(s / (double)n);
}

// --- GLSL Syntax Highlighter ---
class GLSLHighlighter : public QSyntaxHighlighter {
public:
    GLSLHighlighter(QTextDocument *parent = nullptr) : QSyntaxHighlighter(parent) {
        // keyword format
        QTextCharFormat kw;
        kw.setForeground(QColor("#569CD6"));
        kw.setFontWeight(QFont::Bold);
        QStringList keywords = {
            "attribute","const","uniform","varying","break","continue","do","for","while","if","else",
            "in","out","inout","float","int","void","bool","true","false","lowp","mediump","highp",
            "precision","return","mat2","mat3","mat4","vec2","vec3","vec4","sampler1D","sampler2D",
            "sampler3D","samplerCube","struct"
        };
        for (const QString &k : keywords) {
            rules.append({QRegularExpression("\\b" + QRegularExpression::escape(k) + "\\b"), kw});
        }

        // builtins / functions
        QTextCharFormat fn;
        fn.setForeground(QColor("#DCDCAA"));
        QStringList funcs = {"texture1D","texture2D","texture","mix","mod","sin","cos","tan","pow","exp","log","sqrt","abs","min","max","clamp","smoothstep","length","normalize","dot","cross"};
        for (const QString &f : funcs) rules.append({QRegularExpression("\\b" + QRegularExpression::escape(f) + "\\b"), fn});

        // numbers
        QTextCharFormat num;
        num.setForeground(QColor("#B5CEA8"));
        rules.append({QRegularExpression("\\b[0-9]+(\\.[0-9]+)?([eE][-+]?[0-9]+)?\\b"), num});

        // preprocessor
        QTextCharFormat pre;
        pre.setForeground(QColor("#C586C0"));
        pre.setFontWeight(QFont::Bold);
        rules.append({QRegularExpression("^\\s*#.*"), pre});

        // single-line comments
        QTextCharFormat com;
        com.setForeground(QColor("#6A9955"));
        rules.append({QRegularExpression("//[^\n]*"), com});

        // multi-line comments handled in highlightBlock
        commentStart = QRegularExpression("/\\*");
        commentEnd = QRegularExpression("\\*/");
        commentFormat.setForeground(QColor("#6A9955"));
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
        // multi-line comments
        setCurrentBlockState(0);
        int startIndex = 0;
        if (previousBlockState() != 1) {
            QRegularExpressionMatch m = commentStart.match(text);
            if (m.hasMatch()) startIndex = m.capturedStart();
            else startIndex = -1;
        } else {
            startIndex = 0;
        }
        while (startIndex >= 0) {
            QRegularExpressionMatch endMatch = commentEnd.match(text, startIndex);
            int endIndex = endMatch.hasMatch() ? endMatch.capturedEnd() : -1;
            if (endIndex == -1) {
                setFormat(startIndex, text.length() - startIndex, commentFormat);
                setCurrentBlockState(1);
                break;
            } else {
                setFormat(startIndex, endIndex - startIndex, commentFormat);
                startIndex = commentStart.match(text, endIndex).hasMatch() ? commentStart.match(text, endIndex).capturedStart() : -1;
            }
        }
    }

private:
    struct Rule { QRegularExpression pattern; QTextCharFormat format; };
    QVector<Rule> rules;
    QRegularExpression commentStart, commentEnd;
    QTextCharFormat commentFormat;
};

// Helper: underline error lines in the QPlainTextEdit (wavy red)
static void highlightShaderErrorsInEditor(QPlainTextEdit *editor, const QString &log) {
    // Clear previous extra selections
    QList<QTextEdit::ExtraSelection> extras;

    // Try to extract line numbers from common GLSL compiler messages.
    // Patterns supported:
    // "ERROR: 0:12: '...' : syntax error"  -> line 12
    // "0(12) : error C0000: ..."            -> line 12
    // "ERROR: 12: '...' : ..."              -> line 12
    QRegularExpression re1("ERROR:\\s*0:(\\d+):");
    QRegularExpression re2("ERROR:\\s*(\\d+):");
    QRegularExpression re3("0\\((\\d+)\\)");
    QRegularExpression re4("\\((\\d+)\\)\\s*:\\s*error"); // e.g. "(12) : error"
    QSet<int> lines;

    auto addMatches = [&](const QRegularExpression &re) {
        QRegularExpressionMatchIterator it = re.globalMatch(log);
        while (it.hasNext()) {
            QRegularExpressionMatch m = it.next();
            bool ok = false;
            int ln = m.captured(1).toInt(&ok);
            if (ok) lines.insert(ln);
        }
    };

    addMatches(re1); addMatches(re2); addMatches(re3); addMatches(re4);

    // If no lines found, mark first line as error (fallback)
    if (lines.isEmpty() && !log.trimmed().isEmpty()) lines.insert(1);

    QTextCharFormat fmt;
    fmt.setUnderlineStyle(QTextCharFormat::WaveUnderline);
    fmt.setUnderlineColor(Qt::red);
    fmt.setForeground(Qt::red);

    for (int ln : lines) {
        int blockNumber = ln - 1; // user lines are 1-based
        if (blockNumber < 0) continue;
        QTextBlock block = editor->document()->findBlockByNumber(blockNumber);
        if (!block.isValid()) continue;
        QTextCursor cursor(block);
        QTextEdit::ExtraSelection sel;
        sel.cursor = cursor;
        sel.format = fmt;
        // select the whole block to make underline visible
        sel.cursor.select(QTextCursor::LineUnderCursor);
        extras.append(sel);
    }

    editor->setExtraSelections(extras);
}

// Clear error highlights
static void clearShaderErrorHighlights(QPlainTextEdit *editor) {
    editor->setExtraSelections({});
}

// --- GL Widget with runtime shader compilation and fullscreen toggle ---
class GLWaveWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    explicit GLWaveWidget(unsigned sampleRate, QWidget *parent = nullptr)
        : QOpenGLWidget(parent), rate(sampleRate) {
        setFocusPolicy(Qt::StrongFocus);
        tex_size = FFT_SIZE;
        tex_samples.assign(tex_size, 0.0f);
        limiter_gain = 1.0f;
        kick = false;
        hue = 0.0f;
    }

    // update texture samples from ring buffer (thread-safe)
    void updateTextureFromRing() {
        QMutexLocker locker(&g_ring_mutex);
        size_t start = (g_ring_pos + g_ring_cap - (size_t)tex_size) % g_ring_cap;
        for (int i = 0; i < tex_size; ++i) tex_samples[i] = g_ring[(start + i) % g_ring_cap];
    }

    // compile and set fragment shader source at runtime
    // returns true only if compilation+link succeeded and program replaced
    bool setFragmentShaderSource(const QString &fsrc, QString &errorOut) {
        // Ensure we have a current GL context
        makeCurrent();

        // Vertex shader (fixed)
        const char *vsrc =
            "#version 120\n"
            "attribute vec2 a_pos;\n"
            "varying vec2 v_uv;\n"
            "void main() { v_uv = (a_pos + 1.0) * 0.5; gl_Position = vec4(a_pos, 0.0, 1.0); }\n";

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vsrc, nullptr);
        glCompileShader(vs);
        GLint ok = 0; glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            char log[1024]; glGetShaderInfoLog(vs, sizeof(log), nullptr, log);
            errorOut = QString("Vertex shader compile error: %1").arg(log);
            glDeleteShader(vs);
            doneCurrent();
            return false;
        }

        // Fragment shader from editor
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        QByteArray fsrcUtf8 = fsrc.toUtf8();
        const char *fsrc_c = fsrcUtf8.constData();
        glShaderSource(fs, 1, &fsrc_c, nullptr);
        glCompileShader(fs);
        glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            char log[8192]; glGetShaderInfoLog(fs, sizeof(log), nullptr, log);
            errorOut = QString::fromUtf8(log);
            glDeleteShader(vs);
            glDeleteShader(fs);
            doneCurrent();
            return false;
        }

        // Link program
        GLuint newProg = glCreateProgram();
        glAttachShader(newProg, vs);
        glAttachShader(newProg, fs);
        glBindAttribLocation(newProg, 0, "a_pos");
        glLinkProgram(newProg);
        glGetProgramiv(newProg, GL_LINK_STATUS, &ok);
        if (!ok) {
            char log[8192]; glGetProgramInfoLog(newProg, sizeof(log), nullptr, log);
            errorOut = QString::fromUtf8(log);
            glDeleteProgram(newProg);
            glDeleteShader(vs);
            glDeleteShader(fs);
            doneCurrent();
            return false;
        }

        // Success: replace program atomically
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
        // create texture and vbo
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_1D, tex);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, tex_size, 0, GL_RED, GL_FLOAT, tex_samples.data());

        float quad[] = {
            -1.0f, -1.0f,
             1.0f, -1.0f,
            -1.0f,  1.0f,
             1.0f,  1.0f
        };
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

        // compile a default fragment shader (same as original)
        QString defaultFS = R"(
#version 120
uniform sampler1D u_samples;
uniform int u_samples_count;
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
  if (u_kick == 1) { gl_FragColor = vec4(1.0); return; }
  float x = v_uv.x;
  float s = texture1D(u_samples, x).r;
  float center = 0.5;
  float y_wave = center - s * u_vscale;
  float dist = abs(v_uv.y - y_wave);
  float line_thickness = 1.0 / 600.0;
  float alpha = smoothstep(line_thickness, 0.0, dist);
  vec3 wavecol = hsv2rgb(u_hue, 1.0, 1.0);
  vec3 centercol = wavecol * 0.5;
  float center_dist = abs(v_uv.y - center);
  float center_alpha = smoothstep(0.002, 0.0, center_dist);
  vec3 col = mix(centercol, wavecol, alpha);
  float final_alpha = max(alpha, center_alpha);
  vec2 frag = v_uv;
  vec3 gaincol = vec3(0.0, 1.0, 0.0);
  if (frag.x < 0.15 && frag.y < 0.03) {
    float gx = frag.x / 0.15;
    if (gx < u_gain_display) { col = gaincol; final_alpha = 1.0; }
  }
  gl_FragColor = vec4(col, final_alpha);
}
)";
        QString err;
        if (!setFragmentShaderSource(defaultFS, err)) {
            qWarning() << "Default shader compile error:" << err;
        }
    }

    void paintGL() override {
        // update texture
        glClear(GL_COLOR_BUFFER_BIT);
        if (prog) {
            glUseProgram(prog);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_1D, tex);
            glTexSubImage1D(GL_TEXTURE_1D, 0, 0, tex_size, GL_RED, GL_FLOAT, tex_samples.data());
            glUniform1i(loc_u_samples, 0);
            glUniform1f(loc_u_vscale, 0.4f);
            glUniform1f(loc_u_gain_display, std::min(1.0f, limiter_gain));
            glUniform1f(loc_u_hue, hue);
            glUniform1i(loc_u_kick, kick ? 1 : 0);

            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glDisableVertexAttribArray(0);
            glUseProgram(0);
        } else {
            // clear to magenta to indicate no shader (shouldn't happen because default compiled)
            glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        }
    }

    void resizeGL(int w, int h) override { glViewport(0, 0, w, h); }

    // key handling for fullscreen toggle
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
    // public state updated from audio thread
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

// --- ALSA capture thread ---
class AlsaCaptureThread : public QThread {
    Q_OBJECT
public:
    AlsaCaptureThread(unsigned sampleRate, QObject *parent = nullptr)
        : QThread(parent), rate(sampleRate) {}

    void run() override {
        snd_pcm_t *pcm = nullptr;
        int err = snd_pcm_open(&pcm, "default", SND_PCM_STREAM_CAPTURE, 0);
        if (err < 0) { qWarning() << "snd_pcm_open:" << snd_strerror(err); return; }
        snd_pcm_hw_params_t *hw;
        snd_pcm_hw_params_malloc(&hw);
        snd_pcm_hw_params_any(pcm, hw);
        snd_pcm_hw_params_set_access(pcm, hw, SND_PCM_ACCESS_RW_INTERLEAVED);
        snd_pcm_hw_params_set_format(pcm, hw, SND_PCM_FORMAT_S16_LE);
        unsigned int r = rate;
        snd_pcm_hw_params_set_rate_near(pcm, hw, &r, 0);
        snd_pcm_hw_params_set_channels(pcm, hw, 1);
        snd_pcm_uframes_t period = BUFFERSIZE;
        snd_pcm_hw_params_set_period_size_near(pcm, hw, &period, 0);
        snd_pcm_uframes_t buffer = period * 4;
        snd_pcm_hw_params_set_buffer_size_near(pcm, hw, &buffer);
        if ((err = snd_pcm_hw_params(pcm, hw)) < 0) {
            qWarning() << "snd_pcm_hw_params:" << snd_strerror(err);
            snd_pcm_hw_params_free(hw);
            snd_pcm_close(pcm);
            return;
        }
        snd_pcm_hw_params_free(hw);
        snd_pcm_prepare(pcm);

        std::vector<int16_t> read_buf(BUFFERSIZE);
        float g_limiter_gain = 1.0f;
        const float TARGET_LEVEL = powf(10.0f, -TARGET_DB / 20.0f);
        long initial = g_vol_min + (g_vol_max - g_vol_min) / 2;
        alsa_set_capture_all(initial);

        double running_avg_local = 1e-8;
        const double alpha_avg = 0.05;
        quint32 last_kick_ms_local = 0;

        while (!isInterruptionRequested()) {
            snd_pcm_sframes_t frames = snd_pcm_readi(pcm, read_buf.data(), BUFFERSIZE);
            if (frames == -EPIPE) { snd_pcm_prepare(pcm); continue; }
            else if (frames < 0) { frames = snd_pcm_recover(pcm, frames, 0); if (frames < 0) break; continue; }
            else if (frames == 0) { msleep(1); continue; }

            float chunk_rms = compute_rms_s16(read_buf.data(), (size_t)frames);
            float desired_gain = TARGET_LEVEL / std::max(chunk_rms, 1e-12f);
            if (desired_gain > MAX_GAIN) desired_gain = MAX_GAIN;
            if (desired_gain > g_limiter_gain) g_limiter_gain = ATTACK_FAST * desired_gain + (1.0f - ATTACK_FAST) * g_limiter_gain;
            else g_limiter_gain = RELEASE_SLOW * desired_gain + (1.0f - RELEASE_SLOW) * g_limiter_gain;

            long setv;
            if (g_limiter_gain >= 1.0f) setv = g_vol_max;
            else {
                float gmap = std::max(0.0f, g_limiter_gain);
                setv = g_vol_min + (long)((g_vol_max - g_vol_min) * gmap + 0.5f);
            }
            alsa_set_capture_all(setv);

            push_to_ring_from_s16(read_buf.data(), (size_t)frames);

            double energy = fft_compute_bass_energy(rate, 20.0, 150.0);
            running_avg_local = alpha_avg * energy + (1.0 - alpha_avg) * running_avg_local;
            bool kick_local = false;
            if (energy > running_avg_local * 3.0 && energy > 1e-6) {
                quint32 now = (quint32)(QDateTime::currentMSecsSinceEpoch() & 0xffffffff);
                if ((now - last_kick_ms_local) > KICK_DEBOUNCE_MS) {
                    kick_local = true;
                    last_kick_ms_local = now;
                }
            }

            float rms500 = compute_rms_from_ring((size_t)((rate * 500) / 1000));
            float hue_local = std::min(0.7f, rms500 * 5.0f);

            emit audioFrameAvailable(g_limiter_gain, kick_local, hue_local);
            msleep(1);
        }

        snd_pcm_close(pcm);
    }

signals:
    void audioFrameAvailable(float limiterGain, bool kick, float hue);

private:
    unsigned rate;
};

// --- Main application ---
int main(int argc, char **argv) {
    QApplication a(argc, argv);

    if (!alsa_find_capture_control("Capture")) {
        if (!alsa_find_capture_control("Mic")) {
            qWarning() << "No ALSA capture control found. Exiting.";
            return 2;
        }
    }

    unsigned rate = DEFAULT_SR;
    g_ring_cap = (size_t)rate * 2;
    g_ring.assign(g_ring_cap, 0.0f);
    g_ring_pos = 0;

    fft_prepare();

    // UI
    QWidget mainWin;
    mainWin.setWindowTitle("Beat Visualizer Qt6 - Shader Editor and Preview");
    mainWin.resize(1280, 720);

    QSplitter *split = new QSplitter(&mainWin);
    split->setOrientation(Qt::Horizontal);

    // Editor with syntax highlighting
    QPlainTextEdit *editor = new QPlainTextEdit();
    editor->setPlainText(QString::fromUtf8(R"(
// Fragment shader hier bearbeiten
// Beispiel: Standard-Shader
#version 120
uniform sampler1D u_samples;
uniform int u_samples_count;
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
  if (u_kick == 1) { gl_FragColor = vec4(1.0); return; }
  float x = v_uv.x;
  float s = texture1D(u_samples, x).r;
  float center = 0.5;
  float y_wave = center - s * u_vscale;
  float dist = abs(v_uv.y - y_wave);
  float line_thickness = 1.0 / 600.0;
  float alpha = smoothstep(line_thickness, 0.0, dist);
  vec3 wavecol = hsv2rgb(u_hue, 1.0, 1.0);
  vec3 centercol = wavecol * 0.5;
  float center_dist = abs(v_uv.y - center);
  float center_alpha = smoothstep(0.002, 0.0, center_dist);
  vec3 col = mix(centercol, wavecol, alpha);
  float final_alpha = max(alpha, center_alpha);
  vec2 frag = v_uv;
  vec3 gaincol = vec3(0.0, 1.0, 0.0);
  if (frag.x < 0.15 && frag.y < 0.03) {
    float gx = frag.x / 0.15;
    if (gx < u_gain_display) { col = gaincol; final_alpha = 1.0; }
  }
  gl_FragColor = vec4(col, final_alpha);
}
)"));

    // Apply GLSL syntax highlighter
    GLSLHighlighter *highlighter = new GLSLHighlighter(editor->document());

    GLWaveWidget *glw = new GLWaveWidget(rate);
    split->addWidget(editor);
    split->addWidget(glw);
    split->setStretchFactor(0, 0);
    split->setStretchFactor(1, 1);

    QVBoxLayout *lay = new QVBoxLayout(&mainWin);
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(split);
    mainWin.show();

    // fullscreen dialog that will host the GL widget when toggled
    QDialog *fsDialog = new QDialog(&mainWin, Qt::Window);
    fsDialog->setWindowFlag(Qt::FramelessWindowHint);
    fsDialog->setModal(false);

    // ALSA capture thread
    AlsaCaptureThread *cap = new AlsaCaptureThread(rate);
    QObject::connect(cap, &AlsaCaptureThread::audioFrameAvailable, [&](float limiterGain, bool kick, float hue){
        glw->limiter_gain = limiterGain;
        glw->kick = kick;
        glw->hue = hue;
        glw->updateTextureFromRing();
        glw->update();
    });
    cap->start();

    // fullscreen toggle handling: reparent GL widget into dialog and back
    bool isFullscreen = false;
    int splitterIndex = 1;

    auto enterFullscreen = [&](){
        if (isFullscreen) return;
        // reparent glw into dialog
        glw->setParent(fsDialog);
        QVBoxLayout *dlay = new QVBoxLayout(fsDialog);
        dlay->setContentsMargins(0,0,0,0);
        dlay->addWidget(glw);
        fsDialog->showFullScreen();
        glw->show();
        isFullscreen = true;
    };

    auto exitFullscreen = [&](){
        if (!isFullscreen) return;
        // reparent back to splitter
        glw->setParent(split);
        split->insertWidget(splitterIndex, glw);
        fsDialog->hide();
        isFullscreen = false;
    };

    QObject::connect(glw, &GLWaveWidget::requestToggleFullscreen, [&](){
        if (!isFullscreen) enterFullscreen(); else exitFullscreen();
    });
    QObject::connect(glw, &GLWaveWidget::requestExitFullscreen, [&](){
        if (isFullscreen) exitFullscreen();
    });

    // compile on every keystroke: immediate compile on each text change
    QObject::connect(editor, &QPlainTextEdit::textChanged, [&](){
        QString err;
        // try to compile the current editor content immediately
        if (!glw->setFragmentShaderSource(editor->toPlainText(), err)) {
            // compilation failed -> keep last good program active
            qWarning() << "Shader compile error:" << err;
            // underline error lines in editor
            highlightShaderErrorsInEditor(editor, err);
        } else {
            // compilation succeeded -> new program is active
            qDebug() << "Shader compiled successfully";
            clearShaderErrorHighlights(editor);
        }
        // always refresh texture and request repaint (last-good program will render if compile failed)
        glw->updateTextureFromRing();
        glw->update();
    });

    // initial compile of editor content
    {
        QString err;
        if (!glw->setFragmentShaderSource(editor->toPlainText(), err)) {
            qWarning() << "Initial shader compile error:" << err;
            highlightShaderErrorsInEditor(editor, err);
        }
    }

    int ret = a.exec();

    cap->requestInterruption();
    cap->wait();
    delete cap;

    if (g_mixer) snd_mixer_close(g_mixer);

    return ret;
}

#include "main.moc"
