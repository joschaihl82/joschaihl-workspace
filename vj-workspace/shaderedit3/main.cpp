// main.cpp
// Single-threaded GLSL editor + preview (Qt6, C++20)
// - Left: editor exactly left half; Right: preview exactly right half
// - Cube generated entirely in vertex shader (gl_VertexID)
// - Cube scaled to occupy 50% of the smaller screen dimension (Option C)
// - Single-threaded ALSA polling via QTimer (no QThread)
// - Shader compile/link errors are mapped to editor lines and underlined (wavy red)
// - Press 'f' in the preview to toggle fullscreen, Esc to exit fullscreen
//
// Build with a CMakeLists.txt that enables AUTOMOC and links Qt6::Widgets Qt6::OpenGLWidgets and ALSA (optional).

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
#include <QObject>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>
#include <QTextBlock>
#include <QTextCursor>
#include <alsa/asoundlib.h>
#include <cmath>
#include <vector>
#include <random>
#include <cstddef>
#include <string>
#include <cstdarg>
#include <cstdlib>

// portable PI
static constexpr double PI = std::acos(-1.0);

// --- Config ---
constexpr int BUFFERSIZE = 512;
constexpr int FFT_SIZE = 2048;
constexpr unsigned DEFAULT_SR = 48000;
constexpr float TARGET_DB = 48.0f;
constexpr float MAX_GAIN = 4.0f;
constexpr float ATTACK_FAST = 0.6f;
constexpr float RELEASE_SLOW = 0.02f;
constexpr int KICK_DEBOUNCE_MS = 200;

// --- Fatal error helper (abort for debugger) ---
static inline void fatalError(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    std::vfprintf(stderr, fmt, ap);
    std::fprintf(stderr, "\n");
    va_end(ap);
    qCritical().noquote() << QString("FATAL: ") + QString::fromUtf8(fmt);
    std::abort();
}

// --- ALSA mixer globals (optional) ---
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
        double angle = -2.0 * PI * k / (double)FFT_SIZE;
        tw_re[k] = std::cos(angle);
        tw_im[k] = std::sin(angle);
    }
    for (int n = 0; n < FFT_SIZE; ++n) {
        window_hann[n] = 0.5 * (1.0 - std::cos(2.0 * PI * n / (FFT_SIZE - 1)));
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
    if (g_ring_cap < (size_t)FFT_SIZE) return 0.0;
    std::vector<double> local(FFT_SIZE);
    {
        QMutexLocker locker(&g_ring_mutex);
        if (g_ring_cap == 0 || g_ring.empty()) return 0.0;
        size_t start = (g_ring_pos + g_ring_cap - FFT_SIZE) % g_ring_cap;
        for (int n = 0; n < FFT_SIZE; ++n) local[n] = g_ring[(start + n) % g_ring_cap];
    }
    for (int n = 0; n < FFT_SIZE; ++n) {
        fft_re[n] = local[n] * window_hann[n];
        fft_im[n] = 0.0;
    }
    fft_execute(fft_re, fft_im);
    double bin_width = (double)sample_rate / (double)FFT_SIZE;
    int bin_lo = (int)std::floor(f_lo / bin_width);
    int bin_hi = (int)std::ceil(f_hi / bin_width);
    if (bin_lo < 0) bin_lo = 0;
    if (bin_hi > FFT_SIZE/2 - 1) bin_hi = FFT_SIZE/2 - 1;
    double sum = 0.0;
    for (int b = bin_lo; b <= bin_hi; ++b) {
        double re = fft_re[b];
        double im = fft_im[b];
        double mag = std::sqrt(re*re + im*im);
        sum += mag;
    }
    return sum;
}

static void push_to_ring_from_s16(const int16_t *in, size_t n) {
    QMutexLocker locker(&g_ring_mutex);
    if (g_ring_cap == 0 || g_ring.empty()) return;
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
    return (float)std::sqrt(s / (double)n);
}

static float compute_rms_from_ring(size_t n) {
    if (n == 0) return 0.0f;
    if (g_ring_cap == 0 || g_ring.empty()) return 0.0f;
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
    return (float)std::sqrt(s / (double)n);
}

// --- GLSL Syntax Highlighter ---
class GLSLHighlighter : public QSyntaxHighlighter {
public:
    GLSLHighlighter(QTextDocument *parent = nullptr) : QSyntaxHighlighter(parent) {
        QTextCharFormat kw; kw.setForeground(QColor("#569CD6")); kw.setFontWeight(QFont::Bold);
        QStringList keywords = {
            "attribute","const","uniform","varying","break","continue","do","for","while","if","else",
            "in","out","inout","float","int","void","bool","true","false","lowp","mediump","highp",
            "precision","return","mat2","mat3","mat4","vec2","vec3","vec4","sampler1D","sampler2D",
            "sampler3D","samplerCube","struct"
        };
        for (const QString &k : keywords) rules.append({QRegularExpression("\\b" + QRegularExpression::escape(k) + "\\b"), kw});

        QTextCharFormat fn; fn.setForeground(QColor("#DCDCAA"));
        QStringList funcs = {"texture1D","texture2D","texture","mix","mod","sin","cos","tan","pow","exp","log","sqrt","abs","min","max","clamp","smoothstep","length","normalize","dot","cross"};
        for (const QString &f : funcs) rules.append({QRegularExpression("\\b" + QRegularExpression::escape(f) + "\\b"), fn});

        QTextCharFormat num; num.setForeground(QColor("#B5CEA8"));
        rules.append({QRegularExpression("\\b[0-9]+(\\.[0-9]+)?([eE][-+]?[0-9]+)?\\b"), num});

        QTextCharFormat pre; pre.setForeground(QColor("#C586C0")); pre.setFontWeight(QFont::Bold);
        rules.append({QRegularExpression("^\\s*#.*"), pre});

        QTextCharFormat com; com.setForeground(QColor("#6A9955"));
        rules.append({QRegularExpression("//[^\\n]*"), com});

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
        setCurrentBlockState(0);
        int startIndex = 0;
        if (previousBlockState() != 1) {
            QRegularExpressionMatch m = commentStart.match(text);
            if (m.hasMatch()) startIndex = m.capturedStart();
            else startIndex = -1;
        } else startIndex = 0;
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
    QList<QTextEdit::ExtraSelection> extras;
    QRegularExpression re1("ERROR:\\s*0:(\\d+):");
    QRegularExpression re2("ERROR:\\s*(\\d+):");
    QRegularExpression re3("0\\((\\d+)\\)");
    QRegularExpression re4("\\((\\d+)\\)\\s*:\\s*error");
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
    if (lines.isEmpty() && !log.trimmed().isEmpty()) lines.insert(1);
    QTextCharFormat fmt; fmt.setUnderlineStyle(QTextCharFormat::WaveUnderline); fmt.setUnderlineColor(Qt::red); fmt.setForeground(Qt::red);
    for (int ln : lines) {
        int blockNumber = ln - 1;
        if (blockNumber < 0) continue;
        QTextBlock block = editor->document()->findBlockByNumber(blockNumber);
        if (!block.isValid()) continue;
        QTextCursor cursor(block);
        QTextEdit::ExtraSelection sel;
        sel.cursor = cursor;
        sel.format = fmt;
        sel.cursor.select(QTextCursor::LineUnderCursor);
        extras.append(sel);
    }
    editor->setExtraSelections(extras);
}
static void clearShaderErrorHighlights(QPlainTextEdit *editor) { editor->setExtraSelections({}); }

// --- Editor parsing utilities ---
static bool splitEditorShaders(const QString &combined, QString &outVertex, QString &outFragment, int &vertexStartLine, int &fragmentStartLine, QString &err) {
    QString lower = combined.toLower();
    int idxV = lower.indexOf("#shader vertex");
    int idxF = lower.indexOf("#shader fragment");
    if (idxV == -1 || idxF == -1) {
        err = "Editor must contain both '#shader vertex' and '#shader fragment' markers.";
        return false;
    }
    if (idxF < idxV) {
        err = "'#shader vertex' must appear before '#shader fragment'.";
        return false;
    }
    vertexStartLine = combined.left(idxV).count('\n') + 1;
    fragmentStartLine = combined.left(idxF).count('\n') + 1;
    int vLineStartPos = idxV + QString("#shader vertex").length();
    int fLineStartPos = idxF + QString("#shader fragment").length();
    QString vsrc = combined.mid(vLineStartPos, idxF - vLineStartPos);
    QString fsrc = combined.mid(fLineStartPos);
    if (vsrc.startsWith('\n')) vsrc = vsrc.mid(1);
    if (fsrc.startsWith('\n')) fsrc = fsrc.mid(1);
    outVertex = vsrc;
    outFragment = fsrc;
    return true;
}

// Map GL compiler log line numbers to editor line numbers by adding baseLine-1
static QString mapLogLinesToEditor(const QString &log, int baseLine) {
    QString out = log;
    QRegularExpression re1("0:(\\d+):");
    QRegularExpressionMatchIterator it1 = re1.globalMatch(out);
    QString result;
    int lastPos = 0;
    while (it1.hasNext()) {
        QRegularExpressionMatch m = it1.next();
        int pos = m.capturedStart();
        int len = m.capturedLength();
        result += out.mid(lastPos, pos - lastPos);
        bool ok = false;
        int num = m.captured(1).toInt(&ok);
        if (ok) {
            int mapped = num + baseLine - 1;
            result += QString("0:%1:").arg(mapped);
        } else {
            result += m.captured(0);
        }
        lastPos = pos + len;
    }
    result += out.mid(lastPos);
    out = result;

    QRegularExpression re2("\\((\\d+)\\)");
    QRegularExpressionMatchIterator it2 = re2.globalMatch(out);
    result.clear();
    lastPos = 0;
    while (it2.hasNext()) {
        QRegularExpressionMatch m = it2.next();
        int pos = m.capturedStart();
        int len = m.capturedLength();
        result += out.mid(lastPos, pos - lastPos);
        bool ok = false;
        int num = m.captured(1).toInt(&ok);
        if (ok) {
            int mapped = num + baseLine - 1;
            result += QString("(%1)").arg(mapped);
        } else {
            result += m.captured(0);
        }
        lastPos = pos + len;
    }
    result += out.mid(lastPos);
    out = result;
    return out;
}

// --- GL Widget: shaders come from editor; vertex shader must generate cube via gl_VertexID ---
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
        beatCount = 0;
        randSeed = float(std::uniform_real_distribution<float>(0.0f, 1.0f)(rng));
    }

    // update texture samples from ring buffer (thread-safe)
    void updateTextureFromRing() {
        QMutexLocker locker(&g_ring_mutex);
        if (g_ring_cap == 0 || g_ring.empty()) return;
        size_t start = (g_ring_pos + g_ring_cap - (size_t)tex_size) % g_ring_cap;
        for (int i = 0; i < tex_size; ++i) tex_samples[i] = g_ring[(start + i) % g_ring_cap];
    }

    // compile and set fragment+vertex shader source at runtime
    // returns true only if compilation+link succeeded and program replaced
    // errorOut will contain mapped log (line numbers mapped to editor lines)
    bool setShadersFromEditor(const QString &combinedEditorText, QString &errorOut) {
        // Guard: ensure GL context is ready before calling makeCurrent()/GL calls
        if (!isValid() || !context() || !context()->isValid()) {
            errorOut = "GL context not ready yet; try again after the widget is shown.";
            return false;
        }

        makeCurrent();

        QString vsrc, fsrc;
        int vMarkerLine = 0, fMarkerLine = 0;
        QString splitErr;
        if (!splitEditorShaders(combinedEditorText, vsrc, fsrc, vMarkerLine, fMarkerLine, splitErr)) {
            errorOut = splitErr;
            doneCurrent();
            return false;
        }

        // compile vertex shader
        const QByteArray vUtf8 = vsrc.toUtf8();
        const char *vptr = vUtf8.constData();
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vptr, nullptr);
        glCompileShader(vs);
        GLint ok = 0; glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            GLint len = 0; glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &len);
            std::string log(len > 0 ? len : 1, '\0');
            glGetShaderInfoLog(vs, len, nullptr, &log[0]);
            QString mapped = mapLogLinesToEditor(QString::fromStdString(log), vMarkerLine + 1);
            errorOut = QString("VERTEX SHADER COMPILE ERROR:\n%1").arg(mapped);
            glDeleteShader(vs);
            doneCurrent();
            return false;
        }

        // compile fragment shader
        const QByteArray fUtf8 = fsrc.toUtf8();
        const char *fptr = fUtf8.constData();
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fptr, nullptr);
        glCompileShader(fs);
        glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            GLint len = 0; glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &len);
            std::string log(len > 0 ? len : 1, '\0');
            glGetShaderInfoLog(fs, len, nullptr, &log[0]);
            QString mapped = mapLogLinesToEditor(QString::fromStdString(log), fMarkerLine + 1);
            errorOut = QString("FRAGMENT SHADER COMPILE ERROR:\n%1").arg(mapped);
            glDeleteShader(vs);
            glDeleteShader(fs);
            doneCurrent();
            return false;
        }

        // link program
        GLuint newProg = glCreateProgram();
        glAttachShader(newProg, vs);
        glAttachShader(newProg, fs);
        glLinkProgram(newProg);
        glGetProgramiv(newProg, GL_LINK_STATUS, &ok);
        if (!ok) {
            GLint len = 0; glGetProgramiv(newProg, GL_INFO_LOG_LENGTH, &len);
            std::string log(len > 0 ? len : 1, '\0');
            glGetProgramInfoLog(newProg, len, nullptr, &log[0]);
            QString raw = QString::fromStdString(log);
            QString mappedV = mapLogLinesToEditor(raw, vMarkerLine + 1);
            QString mappedVF = mapLogLinesToEditor(mappedV, fMarkerLine + 1);
            errorOut = QString("LINK ERROR:\n%1").arg(mappedVF);
            glDeleteProgram(newProg);
            glDeleteShader(vs);
            glDeleteShader(fs);
            doneCurrent();
            return false;
        }

        // success: replace program
        if (prog) glDeleteProgram(prog);
        prog = newProg;
        glDeleteShader(vs);
        glDeleteShader(fs);

        // cache uniform locations used by both shaders
        loc_u_time = glGetUniformLocation(prog, "u_time");
        loc_u_resolution = glGetUniformLocation(prog, "u_resolution");
        loc_u_beat = glGetUniformLocation(prog, "u_beat");
        loc_u_beatCount = glGetUniformLocation(prog, "u_beatCount");
        loc_u_randSeed = glGetUniformLocation(prog, "u_randSeed");
        loc_u_scale = glGetUniformLocation(prog, "u_scale");

        doneCurrent();
        return true;
    }

signals:
    void requestToggleFullscreen();
    void requestExitFullscreen();
    void glInitialized();

protected:
    void initializeGL() override {
        initializeOpenGLFunctions();
        fft_prepare();
        // emit signal so main can trigger initial compile safely
        emit glInitialized();
    }

    void paintGL() override {
        glClearColor(0.06f, 0.06f, 0.06f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        if (prog) {
            glUseProgram(prog);
            float t = float(QDateTime::currentMSecsSinceEpoch() % 0x7fffffff) * 0.001f;
            if (loc_u_time >= 0) glUniform1f(loc_u_time, t);
            if (loc_u_resolution >= 0) glUniform2f(loc_u_resolution, float(width()), float(height()));
            if (loc_u_beat >= 0) glUniform1i(loc_u_beat, kick ? 1 : 0);
            if (loc_u_beatCount >= 0) glUniform1i(loc_u_beatCount, beatCount);
            if (loc_u_randSeed >= 0) glUniform1f(loc_u_randSeed, randSeed);

            // Compute scale so cube occupies 50% of the smaller screen dimension (Option C)
            // u_resolution passed above; compute scale in C++ to avoid precision differences
            float w = float(width());
            float h = float(height());
            float minDim = std::min(w, h);
            // scale factor: world-space multiplier applied to cube coordinates so that
            // projected size corresponds to half of the smaller screen dimension.
            // We use a simple proportional factor: 0.5 * (minDim / h)
            // This keeps cube size relative to viewport and aspect; it's stable and responsive.
            float scale = 0.5f * (minDim / h);
            if (loc_u_scale >= 0) glUniform1f(loc_u_scale, scale);

            // draw cube procedurally (36 vertices)
            glDrawArrays(GL_TRIANGLES, 0, 36);

            glUseProgram(0);
        } else {
            glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
    }

    void resizeGL(int w, int h) override { glViewport(0, 0, w, h); }

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

public:
    // public state updated from audio poller
    float limiter_gain;
    bool kick;
    float hue;
    int beatCount;
    float randSeed;

private:
    unsigned rate;
    int tex_size;
    std::vector<float> tex_samples;
    GLuint prog = 0;
    GLint loc_u_time = -1;
    GLint loc_u_resolution = -1;
    GLint loc_u_beat = -1;
    GLint loc_u_beatCount = -1;
    GLint loc_u_randSeed = -1;
    GLint loc_u_scale = -1;
    std::mt19937 rng{ std::random_device{}() };
};

// ----------------------
// Single-threaded ALSA poller (runs in GUI thread via QTimer)
// ----------------------
class AlsaPoller : public QObject {
    Q_OBJECT
public:
    AlsaPoller(unsigned sampleRate, QObject *parent = nullptr)
        : QObject(parent), rate(sampleRate), pcm(nullptr) {}

    ~AlsaPoller() {
        if (pcm) {
            snd_pcm_close(pcm);
            pcm = nullptr;
        }
    }

    bool openDevice() {
        // open non-blocking
        int err = snd_pcm_open(&pcm, "default", SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK);
        if (err < 0) {
            qWarning() << "snd_pcm_open failed:" << snd_strerror(err);
            pcm = nullptr;
            return false;
        }
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
            pcm = nullptr;
            return false;
        }
        snd_pcm_hw_params_free(hw);
        snd_pcm_prepare(pcm);
        return true;
    }

    // Called periodically from GUI thread
    void pollOnce(GLWaveWidget *glw) {
        if (!pcm) return;
        std::vector<int16_t> read_buf(BUFFERSIZE);
        snd_pcm_sframes_t frames = snd_pcm_readi(pcm, read_buf.data(), BUFFERSIZE);
        if (frames == -EAGAIN || frames == -EWOULDBLOCK) {
            // no data right now, try later
            return;
        } else if (frames == -EPIPE) {
            // overrun
            snd_pcm_prepare(pcm);
            return;
        } else if (frames < 0) {
            // recoverable error?
            frames = snd_pcm_recover(pcm, frames, 0);
            if (frames < 0) {
                qWarning() << "snd_pcm_recover failed:" << snd_strerror(frames);
                return;
            }
        } else if (frames == 0) {
            return;
        }

        // process frames
        push_to_ring_from_s16(read_buf.data(), (size_t)frames);

        // limiter/gain logic (simple)
        float chunk_rms = compute_rms_s16(read_buf.data(), (size_t)frames);
        float desired_gain = std::pow(10.0f, -TARGET_DB / 20.0f) / std::max(chunk_rms, 1e-12f);
        static float g_limiter_gain = 1.0f;
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

        double energy = fft_compute_bass_energy(rate, 20.0, 150.0);
        static double running_avg_local = 1e-8;
        const double alpha_avg = 0.05;
        running_avg_local = alpha_avg * energy + (1.0 - alpha_avg) * running_avg_local;
        bool kick_local = false;
        static quint32 last_kick_ms_local = 0;
        if (energy > running_avg_local * 3.0 && energy > 1e-6) {
            quint32 now = (quint32)(QDateTime::currentMSecsSinceEpoch() & 0xffffffff);
            if ((now - last_kick_ms_local) > KICK_DEBOUNCE_MS) {
                kick_local = true;
                last_kick_ms_local = now;
            }
        }

        float rms500 = compute_rms_from_ring((size_t)((rate * 500) / 1000));
        float hue_local = std::min(0.7f, rms500 * 5.0f);

        // update GL widget state directly (single-threaded)
        if (glw) {
            glw->limiter_gain = g_limiter_gain;
            glw->kick = kick_local;
            glw->hue = hue_local;
            if (kick_local) glw->beatCount++;
            glw->updateTextureFromRing();
            glw->update();
        }
    }

private:
    unsigned rate;
    snd_pcm_t *pcm;
};

// --- Main application ---
int main(int argc, char **argv) {
    // Use compatibility profile so glDrawArrays works without explicit VAO on many drivers.
    QSurfaceFormat fmt;
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CompatibilityProfile);
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication a(argc, argv);

    // initialize ring buffer
    {
        QMutexLocker locker(&g_ring_mutex);
        g_ring_cap = DEFAULT_SR * 4; // 4 seconds buffer
        g_ring.assign(g_ring_cap, 0.0f);
        g_ring_pos = 0;
    }

    // try to find ALSA capture control (optional)
    if (!alsa_find_capture_control("Capture")) {
        if (!alsa_find_capture_control("Mic")) {
            qWarning() << "Warning: Could not find ALSA capture control 'Capture' or 'Mic'. Capture volume control will be disabled.";
        }
    }

    // Create a horizontal splitter and force exact left/right halves
    QSplitter *split = new QSplitter;
    split->setOrientation(Qt::Horizontal);

    // Left: editor
    QPlainTextEdit *editor = new QPlainTextEdit;

    // Right: preview container
    QWidget *rightContainer = new QWidget;
    QVBoxLayout *vlay = new QVBoxLayout(rightContainer);
    vlay->setContentsMargins(0,0,0,0);

    // GL widget (preview)
    GLWaveWidget *glw = new GLWaveWidget(DEFAULT_SR);
    vlay->addWidget(glw);

    // Add widgets to splitter in correct order: editor left, preview right
    split->addWidget(editor);
    split->addWidget(rightContainer);

    // Ensure equal stretch so halves are equal
    split->setStretchFactor(0, 1);
    split->setStretchFactor(1, 1);

    // Editor template: both vertex and fragment shaders must be present here.
    // Note: vertex shader now expects uniform float u_scale and applies it to worldPos.
    editor->setPlainText(QString(
R"(#shader vertex
#version 330 core
uniform float u_time;
uniform vec2 u_resolution;
uniform int u_beat;
uniform int u_beatCount;
uniform float u_randSeed;
uniform float u_scale;
out vec3 v_normal;
out vec3 v_worldPos;
out vec2 v_uv;
out vec3 v_color;

// Procedural cube using gl_VertexID (36 vertices)
vec3 faceNormal(int face) {
  if(face==0) return vec3(1,0,0);
  if(face==1) return vec3(-1,0,0);
  if(face==2) return vec3(0,1,0);
  if(face==3) return vec3(0,-1,0);
  if(face==4) return vec3(0,0,1);
  return vec3(0,0,-1);
}
vec3 faceU(int face) {
  if(face==0) return vec3(0,0,1);
  if(face==1) return vec3(0,0,-1);
  if(face==2) return vec3(1,0,0);
  if(face==3) return vec3(1,0,0);
  if(face==4) return vec3(1,0,0);
  return vec3(-1,0,0);
}
vec3 faceV(int face) {
  if(face==0) return vec3(0,1,0);
  if(face==1) return vec3(0,1,0);
  if(face==2) return vec3(0,0,-1);
  if(face==3) return vec3(0,0,1);
  if(face==4) return vec3(0,1,0);
  return vec3(0,1,0);
}

void main() {
  int vid = gl_VertexID; // 0..35
  int face = vid / 6;
  int tri = (vid % 6) / 3;
  int v = vid % 3;
  int corner = 0;
  if(tri==0) {
    if(v==0) corner = 0; else if(v==1) corner = 1; else corner = 2;
  } else {
    if(v==0) corner = 0; else if(v==1) corner = 2; else corner = 3;
  }
  vec2 uv;
  if(corner==0) uv = vec2(0.0,0.0);
  else if(corner==1) uv = vec2(1.0,0.0);
  else if(corner==2) uv = vec2(1.0,1.0);
  else uv = vec2(0.0,1.0);
  vec3 N = faceNormal(face);
  vec3 U = faceU(face);
  vec3 V = faceV(face);
  vec2 p = uv - 0.5;
  vec3 pos = N * 0.5 + U * p.x + V * p.y;

  // rotate slowly
  float slowX = u_time * 0.35;
  float slowY = u_time * 0.45;
  mat3 rx = mat3(1,0,0, 0,cos(slowX),-sin(slowX), 0,sin(slowX),cos(slowX));
  mat3 ry = mat3(cos(slowY),0,sin(slowY), 0,1,0, -sin(slowY),0,cos(slowY));
  vec3 worldPos = ry * rx * pos;

  // Apply uniform scale so cube occupies 50% of the smaller screen dimension (Option C)
  worldPos *= u_scale;

  vec3 worldNormal = normalize(ry * rx * N);
  v_normal = worldNormal;
  v_worldPos = worldPos;
  v_uv = uv;
  vec3 faceColor;
  if(abs(worldNormal.x) > 0.5) faceColor = (worldNormal.x>0.0)?vec3(0.9,0.3,0.2):vec3(0.2,0.6,0.9);
  else if(abs(worldNormal.y) > 0.5) faceColor = (worldNormal.y>0.0)?vec3(0.9,0.8,0.2):vec3(0.4,0.2,0.9);
  else faceColor = (worldNormal.z>0.0)?vec3(0.2,0.9,0.4):vec3(0.9,0.4,0.7);
  v_color = faceColor;

  float fov = radians(45.0);
  float aspect = u_resolution.x / max(1.0, u_resolution.y);
  float f = 1.0 / tan(fov * 0.5);
  mat4 proj = mat4(f/aspect,0,0,0, 0,f,0,0, 0,0,-(100.0+0.1)/(100.0-0.1),-1.0, 0,0,-(2.0*100.0*0.1)/(100.0-0.1),0);
  mat4 view = mat4(1.0); view[3].z = -3.0;
  gl_Position = proj * view * vec4(worldPos,1.0);
}

#shader fragment
#version 330 core
in vec3 v_normal;
in vec3 v_worldPos;
in vec2 v_uv;
in vec3 v_color;
out vec4 fragColor;
uniform float u_time;
uniform vec2 u_resolution;
uniform int u_beat;
uniform int u_beatCount;
uniform float u_randSeed;

vec3 phong(vec3 pos, vec3 normal, vec3 baseColor) {
    vec3 lightCenter = vec3(2.0 * sin(u_time * 0.7), 1.2 * cos(u_time * 0.9), 2.0 + sin(u_time * 0.5));
    vec3 L = normalize(lightCenter - pos);
    vec3 V = normalize(vec3(0.0,0.0,3.0) - pos);
    vec3 N = normalize(normal);
    vec3 ambient = 0.12 * baseColor;
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * baseColor;
    vec3 H = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), 64.0);
    vec3 specular = vec3(1.0) * spec * 0.6;
    float rim = pow(1.0 - max(dot(N, V), 0.0), 2.0);
    float beatPulse = (u_beat == 1) ? 1.0 : 0.0;
    float bc = float(u_beatCount);
    float smoothPulse = 0.0;
    if (bc > 0.0) smoothPulse = 0.2 * abs(sin(u_time * 10.0 + fract(bc * 0.1234)));
    vec3 rimColor = vec3(1.0, 0.9, 0.6) * (0.6 * rim * (beatPulse + smoothPulse));
    return ambient + diffuse + specular + rimColor;
}

void main() {
    vec3 base = v_color;
    vec3 n = normalize(v_normal);
    float g = 0.5 + 0.5 * n.y;
    base *= mix(vec3(0.9,0.9,1.0), vec3(1.0,0.9,0.8), g);
    float hueShift = 0.2 * sin(u_time * 1.5 + length(v_worldPos.xy));
    base = base * (1.0 - 0.3*hueShift) + vec3(base.z, base.x, base.y) * (0.3*hueShift);
    vec3 lit = phong(v_worldPos, n, base);
    vec2 uv = (gl_FragCoord.xy / u_resolution.xy) * 2.0 - 1.0;
    float vign = smoothstep(1.6, 0.8, length(uv));
    lit *= vign;
    vec3 color = pow(lit, vec3(1.0/2.2));
    fragColor = vec4(color, 1.0);
}
)"));

    GLSLHighlighter *hl = new GLSLHighlighter(editor->document());
    Q_UNUSED(hl);

    split->setWindowTitle("GLSL Editor + Preview (exact left/right halves)");

    // create poller and open device
    AlsaPoller poller(DEFAULT_SR);
    bool alsaOpened = poller.openDevice();
    if (!alsaOpened) qWarning() << "ALSA device not opened; audio capture disabled.";

    // compile shader on text change (debounced)
    QTimer *debounce = new QTimer(&a);
    debounce->setSingleShot(true);
    debounce->setInterval(300);

    // Debounce lambda: checks GL readiness and retries if necessary
    auto compileNow = [&]() {
        QString src = editor->toPlainText();
        QString log;
        if (!glw->setShadersFromEditor(src, log)) {
            highlightShaderErrorsInEditor(editor, log);
            qDebug() << "Shader compile/link error:\n" << log;
        } else {
            clearShaderErrorHighlights(editor);
            qDebug() << "Shaders compiled and linked successfully.";
        }
        glw->update();
    };

    QObject::connect(editor, &QPlainTextEdit::textChanged, [&](){ debounce->start(); });

    QObject::connect(debounce, &QTimer::timeout, [&](){
        // If GL widget not ready yet, retry shortly instead of calling setShadersFromEditor now
        if (!glw->isValid() || !glw->context() || !glw->context()->isValid()) {
            QTimer::singleShot(150, [&](){ debounce->start(); });
            return;
        }
        compileNow();
    });

    // Ensure initial compile happens only after GL is initialized
    QObject::connect(glw, &GLWaveWidget::glInitialized, [&](){
        // compile initial content once GL is ready
        debounce->start();
    });

    // fullscreen toggles
    QObject::connect(glw, &GLWaveWidget::requestToggleFullscreen, [&](){
        if (rightContainer->isFullScreen()) rightContainer->showNormal();
        else rightContainer->showFullScreen();
    });
    QObject::connect(glw, &GLWaveWidget::requestExitFullscreen, [&](){
        if (rightContainer->isFullScreen()) rightContainer->showNormal();
    });

    // Poll timer: single-threaded ALSA polling
    QTimer pollTimer;
    pollTimer.setInterval(10); // 10 ms poll interval (tunable)
    QObject::connect(&pollTimer, &QTimer::timeout, [&](){
        if (alsaOpened) poller.pollOnce(glw);
    });
    pollTimer.start();

    // Show window and enforce equal halves after show
    split->resize(1200, 720);
    split->show();

    // Force equal halves: compute sizes and set them explicitly so left is exactly half and right exactly half.
    QTimer::singleShot(0, [&](){
        QSize s = split->size();
        int half = s.width() / 2;
        QList<int> sizes;
        sizes << half << (s.width() - half);
        split->setSizes(sizes);
    });

    int ret = a.exec();

    // cleanup
    pollTimer.stop();
    if (g_mixer) {
        snd_mixer_close(g_mixer);
        g_mixer = nullptr;
        g_selem = nullptr;
    }

    return ret;
}

#include "main.moc"
