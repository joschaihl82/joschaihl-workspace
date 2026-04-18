// main.cpp
// GLSL Audio Visualizer - Complete amalgamated main.cpp
// - Features:
//   * Click-drag horizontal sliders with logarithmic mapping for headroom and limiter ceiling
//   * Hardware (ALSA) microphone percent control and hard limiter with hysteresis
//   * Ring buffer (resizable) for audio texture and visualization
//   * OpenGL preview with shader editor and compile/log dock
//   * Toolbar with FlowLayout that wraps to new rows when full
//   * Progress bars for mic input, limiter output, and visualizer output (100 px each)
//   * Persistent settings via QSettings
// Build: link Qt6::Widgets Qt6::OpenGLWidgets Qt6::Multimedia and libasound
// Note: This is a single-file amalgam; compile with -std=gnu++20 and link ALSA and Qt6 modules.

#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QLabel>
#include <QStatusBar>
#include <QDockWidget>
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
#include <QSettings>
#include <QTextEdit>
#include <QSizePolicy>
#include <QSlider>
#include <QHBoxLayout>
#include <QWidget>
#include <QMouseEvent>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QProgressBar>
#include <QStyleOption>
#include <QPainter>

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

#include <alsa/asoundlib.h>

// --------------------------- Adjustable defaults (persisted via QSettings) ---------------------------
static unsigned g_sampleRate = 48000u;
static int g_ringSeconds = 4;
static int g_sampleBufferFrames = 128;
static int g_audioTexSize = 2048;
static float g_targetHeadroomDbAbs = 10.0f;
static bool g_limiterEnabled = true;
static float g_limiterCeilingDb = 6.0f;
static float g_limiterHysteresisOnDb = 1.0f;
static float g_limiterHysteresisOffDb = 0.5f;
static int g_hwPercentDefault = 80;
static int g_hwPercentMin = 0;
static int g_hwPercentMax = 150;
static int g_hwAdjustIntervalMs = 200;
static int g_limiterAdjustMinMs = 50;
static float g_displayGainRefDb = 30.0f;

constexpr int POLL_MS = 10;
constexpr int FLASH_MS = 120;

// Forward declarations
static inline float computeDisplayGainFromHeadroom(float headroomDb);
static void allocateRingBuffer(int seconds);
static void read_ring_into_float_array(float *out, int outSize);
static int compute_recent_output_percent(int sampleWindow = 128);

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

// --------------------------- FlowLayout (wraps children to new rows) -------------------------
class FlowLayout : public QLayout {
public:
    explicit FlowLayout(QWidget *parent = nullptr, int margin = -1, int hSpacing = -1, int vSpacing = -1)
        : QLayout(parent), m_hSpace(hSpacing), m_vSpace(vSpacing) {
        if (margin >= 0) setContentsMargins(margin, margin, margin, margin);
    }
    ~FlowLayout() override {
        QLayoutItem *item;
        while ((item = takeAt(0))) delete item;
    }
    void addItem(QLayoutItem *item) override { itemList.append(item); }
    int horizontalSpacing() const {
        if (m_hSpace >= 0) return m_hSpace;
        return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
    }
    int verticalSpacing() const {
        if (m_vSpace >= 0) return m_vSpace;
        return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
    }
    int count() const override { return itemList.size(); }
    QLayoutItem *itemAt(int index) const override { return itemList.value(index); }
    QLayoutItem *takeAt(int index) override {
        if (index >= 0 && index < itemList.size()) return itemList.takeAt(index);
        return nullptr;
    }
    Qt::Orientations expandingDirections() const override { return {}; }
    bool hasHeightForWidth() const override { return true; }
    int heightForWidth(int width) const override {
        int height = doLayout(QRect(0, 0, width, 0), true);
        return height;
    }
    void setGeometry(const QRect &rect) override {
        QLayout::setGeometry(rect);
        doLayout(rect, false);
    }
    QSize sizeHint() const override { return minimumSize(); }
    QSize minimumSize() const override {
        QSize size;
        for (QLayoutItem *item : itemList) size = size.expandedTo(item->minimumSize());
        int left, top, right, bottom;
        getContentsMargins(&left, &top, &right, &bottom);
        size += QSize(left + right, top + bottom);
        return size;
    }
    void getAllContentsMargins(int &left, int &top, int &right, int &bottom) const {
        getContentsMargins(&left, &top, &right, &bottom);
    }
    int contentsMarginTotalHorizontal() const {
        int left, top, right, bottom; getContentsMargins(&left, &top, &right, &bottom); return left + right;
    }
    int contentsMarginTotalVertical() const {
        int left, top, right, bottom; getContentsMargins(&left, &top, &right, &bottom); return top + bottom;
    }
    int contentsMarginLeft() const { int left, top, right, bottom; getContentsMargins(&left, &top, &right, &bottom); return left; }
    int contentsMarginTop() const { int left, top, right, bottom; getContentsMargins(&left, &top, &right, &bottom); return top; }

private:
    int doLayout(const QRect &rect, bool testOnly) const {
        int left, top, right, bottom;
        getContentsMargins(&left, &top, &right, &bottom);
        QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
        int x = effectiveRect.x();
        int y = effectiveRect.y();
        int lineHeight = 0;
        int spaceX = horizontalSpacing();
        int spaceY = verticalSpacing();
        if (spaceX == -1) spaceX = 6;
        if (spaceY == -1) spaceY = 6;

        for (QLayoutItem *item : itemList) {
            QSize itemSize = item->sizeHint();
            int nextX = x + itemSize.width() + spaceX;
            if (nextX - spaceX > effectiveRect.right() && lineHeight > 0) {
                x = effectiveRect.x();
                y += lineHeight + spaceY;
                nextX = x + itemSize.width() + spaceX;
                lineHeight = 0;
            }
            if (!testOnly) item->setGeometry(QRect(QPoint(x, y), itemSize));
            x = nextX;
            lineHeight = std::max(lineHeight, itemSize.height());
        }
        return y + lineHeight - rect.y() + bottom;
    }
    int smartSpacing(QStyle::PixelMetric pm) const {
        QObject *parent = this->parent();
        if (!parent) return -1;
        if (parent->isWidgetType()) {
            QWidget *pw = static_cast<QWidget *>(parent);
            return pw->style()->pixelMetric(pm, nullptr, pw);
        }
        return -1;
    }
    QList<QLayoutItem*> itemList;
    int m_hSpace;
    int m_vSpace;
};

// --------------------------- ALSA helpers (set + get) -------------------------
static const std::vector<std::string> kCandidateCaptureNames = { "Capture", "Mic", "Mic Capture", "Capture Volume", "Input Source" };

bool setHardwareMicVolumePercent(int percent) {
    if (percent < g_hwPercentMin) percent = g_hwPercentMin;
    if (percent > g_hwPercentMax) percent = g_hwPercentMax;
    int err = 0;
    snd_mixer_t *handle = nullptr;
    if ((err = snd_mixer_open(&handle, 0)) < 0) {
        fprintf(stderr, "ALSA: snd_mixer_open error: %s\n", snd_strerror(err));
        return false;
    }
    if ((err = snd_mixer_attach(handle, "default")) < 0) {
        snd_mixer_close(handle);
        if ((err = snd_mixer_open(&handle, 0)) < 0) {
            fprintf(stderr, "ALSA: snd_mixer_open fallback error: %s\n", snd_strerror(err));
            return false;
        }
        if ((err = snd_mixer_attach(handle, "hw:0")) < 0) {
            fprintf(stderr, "ALSA: snd_mixer_attach error: %s\n", snd_strerror(err));
            snd_mixer_close(handle);
            return false;
        }
    }
    if ((err = snd_mixer_selem_register(handle, nullptr, nullptr)) < 0) {
        fprintf(stderr, "ALSA: snd_mixer_selem_register error: %s\n", snd_strerror(err));
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
    for (elem = snd_mixer_first_elem(handle); elem; elem = snd_mixer_elem_next(elem)) {
        if (!snd_mixer_selem_is_active(elem)) continue;
        const char *ename = snd_mixer_selem_get_name(elem);
        if (!ename) continue;
        std::string name(ename);
        if (!snd_mixer_selem_has_capture_volume(elem)) continue;
        std::string lname = name;
        std::transform(lname.begin(), lname.end(), lname.begin(), ::tolower);
        bool matched = false;
        for (const auto &cand : kCandidateCaptureNames) {
            std::string lcand = cand;
            std::transform(lcand.begin(), lcand.end(), lcand.begin(), ::tolower);
            if (lname.find(lcand) != std::string::npos) { matched = true; break; }
        }
        if (!matched) continue;
        long minv = 0, maxv = 0;
        if (snd_mixer_selem_get_capture_volume_range(elem, &minv, &maxv) < 0) continue;
        double linear = percent / 100.0;
        if (linear < 0.0) linear = 0.0;
        if (linear > 1.5) linear = 1.5;
        long target = static_cast<long>(minv + linear * (maxv - minv) + 0.5);
        bool set_ok = true;
        for (int c = 0; c <= SND_MIXER_SCHN_LAST; ++c) {
            snd_mixer_selem_channel_id_t ch_id = static_cast<snd_mixer_selem_channel_id_t>(c);
            if (snd_mixer_selem_has_capture_channel(elem, ch_id)) {
                if ((err = snd_mixer_selem_set_capture_volume(elem, ch_id, target)) < 0) {
                    set_ok = false;
                    break;
                }
            }
        }
        if (!set_ok) {
            if ((err = snd_mixer_selem_set_capture_volume_all(elem, target)) < 0) {
                continue;
            }
        }
        success = true;
        break;
    }
    snd_mixer_close(handle);
    if (!success) fprintf(stderr, "ALSA: No suitable capture control found or set failed.\n");
    return success;
}

int getHardwareMicVolumePercent() {
    int err = 0;
    snd_mixer_t *handle = nullptr;
    if ((err = snd_mixer_open(&handle, 0)) < 0) return -1;
    if ((err = snd_mixer_attach(handle, "default")) < 0) {
        snd_mixer_close(handle);
        if ((err = snd_mixer_open(&handle, 0)) < 0) return -1;
        if ((err = snd_mixer_attach(handle, "hw:0")) < 0) { snd_mixer_close(handle); return -1; }
    }
    if ((err = snd_mixer_selem_register(handle, nullptr, nullptr)) < 0) { snd_mixer_close(handle); return -1; }
    if ((err = snd_mixer_load(handle)) < 0) { snd_mixer_close(handle); return -1; }
    snd_mixer_elem_t *elem = nullptr;
    int outPercent = -1;
    for (elem = snd_mixer_first_elem(handle); elem; elem = snd_mixer_elem_next(elem)) {
        if (!snd_mixer_selem_is_active(elem)) continue;
        const char *ename = snd_mixer_selem_get_name(elem);
        if (!ename) continue;
        std::string name(ename);
        if (!snd_mixer_selem_has_capture_volume(elem)) continue;
        std::string lname = name;
        std::transform(lname.begin(), lname.end(), lname.begin(), ::tolower);
        bool matched = false;
        for (const auto &cand : kCandidateCaptureNames) {
            std::string lcand = cand;
            std::transform(lcand.begin(), lcand.end(), lcand.begin(), ::tolower);
            if (lname.find(lcand) != std::string::npos) { matched = true; break; }
        }
        if (!matched) continue;
        long minv = 0, maxv = 0;
        if (snd_mixer_selem_get_capture_volume_range(elem, &minv, &maxv) < 0) continue;
        long sum = 0; int count = 0;
        for (int c = 0; c <= SND_MIXER_SCHN_LAST; ++c) {
            snd_mixer_selem_channel_id_t ch_id = static_cast<snd_mixer_selem_channel_id_t>(c);
            if (snd_mixer_selem_has_capture_channel(elem, ch_id)) {
                long val = 0;
                if (snd_mixer_selem_get_capture_volume(elem, ch_id, &val) == 0) { sum += val; ++count; }
            }
        }
        if (count == 0) continue;
        double avg = static_cast<double>(sum) / static_cast<double>(count);
        double linear = (avg - minv) / static_cast<double>(maxv - minv);
        int percent = static_cast<int>(std::round(linear * 100.0));
        outPercent = percent;
        break;
    }
    snd_mixer_close(handle);
    return outPercent;
}

// --------------------------- Ring buffer -------------------------
static std::vector<float> g_ring;
static size_t g_ring_cap = 0;
static size_t g_ring_pos = 0;
static QMutex g_ring_mutex;

void allocateRingBuffer(int seconds) {
    QMutexLocker locker(&g_ring_mutex);
    if (seconds < 1) seconds = 1;
    g_ring_cap = static_cast<size_t>(g_sampleRate) * static_cast<size_t>(seconds);
    g_ring.assign(g_ring_cap, 0.0f);
    g_ring_pos = 0;
}

static void push_to_ring_from_s16(const int16_t *in, size_t n) {
    QMutexLocker locker(&g_ring_mutex);
    if (g_ring_cap == 0) return;
    for (size_t i = 0; i < n; ++i) {
        g_ring[g_ring_pos] = in[i] / 32768.0f;
        g_ring_pos = (g_ring_pos + 1) % g_ring_cap;
    }
}

void read_ring_into_float_array(float *out, int outSize) {
    QMutexLocker locker(&g_ring_mutex);
    if (g_ring_cap == 0) {
        for (int i = 0; i < outSize; ++i) out[i] = 0.0f;
        return;
    }
    size_t start = (g_ring_pos + g_ring_cap - (size_t) outSize) % g_ring_cap;
    for (int i = 0; i < outSize; ++i) out[i] = g_ring[(start + i) % g_ring_cap];
}

static float compute_rms_s16(const int16_t *buf, size_t n) {
    if (n == 0) return 0.0f;
    double s = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double v = buf[i] / 32768.0;
        s += v * v;
    }
    return static_cast<float>(std::sqrt(s / static_cast<double>(n)));
}

int compute_recent_output_percent(int sampleWindow) {
    QMutexLocker locker(&g_ring_mutex);
    if (g_ring_cap == 0) return 0;
    size_t n = std::min<size_t>(sampleWindow, g_ring_cap);
    size_t start = (g_ring_pos + g_ring_cap - n) % g_ring_cap;
    float peak = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        float v = std::abs(g_ring[(start + i) % g_ring_cap]);
        if (v > peak) peak = v;
    }
    int p = static_cast<int>(std::round(std::clamp(peak, 0.0f, 1.0f) * 100.0f));
    return p;
}

// --------------------------- GLSL Highlighter ------------------
class GLSLHighlighter: public QSyntaxHighlighter {
public:
    GLSLHighlighter(QTextDocument *parent = nullptr) : QSyntaxHighlighter(parent) {
        QTextCharFormat kw; kw.setForeground(QColor("#569CD6")); kw.setFontWeight(QFont::Bold);
        QStringList keywords = { "uniform","in","out","varying","attribute","void","float","int","vec2","vec3","vec4","mat3","mat4","if","else","return","for","while","sampler1D","texture" };
        for (const QString &k : keywords) rules.append({ QRegularExpression("\\b" + QRegularExpression::escape(k) + "\\b"), kw });
        QTextCharFormat com; com.setForeground(QColor("#6A9955"));
        rules.append({ QRegularExpression("//[^\\n]*"), com });
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
    struct Rule { QRegularExpression pattern; QTextCharFormat format; };
    QVector<Rule> rules;
};

// --------------------------- ClickDragSlider ------------------
class ClickDragSlider : public QSlider {
public:
    explicit ClickDragSlider(Qt::Orientation o, QWidget *parent = nullptr) : QSlider(o, parent) { setMouseTracking(true); }
protected:
    void mousePressEvent(QMouseEvent *ev) override {
        if (ev->button() == Qt::LeftButton) {
            int minv = minimum(), maxv = maximum();
            if (orientation() == Qt::Horizontal) {
                int w = width();
                if (w > 0) {
                    float px = static_cast<float>(ev->position().x()) / static_cast<float>(w);
                    px = std::clamp(px, 0.0f, 1.0f);
                    int val = static_cast<int>(std::round(minv + px * (maxv - minv)));
                    setValue(val);
                    setSliderDown(true);
                }
            }
            ev->accept();
            emit sliderMoved(value());
            return;
        }
        QSlider::mousePressEvent(ev);
    }
    void mouseMoveEvent(QMouseEvent *ev) override {
        if (isSliderDown() && (ev->buttons() & Qt::LeftButton)) {
            int minv = minimum(), maxv = maximum();
            if (orientation() == Qt::Horizontal) {
                int w = width();
                if (w > 0) {
                    float px = static_cast<float>(ev->position().x()) / static_cast<float>(w);
                    px = std::clamp(px, 0.0f, 1.0f);
                    int val = static_cast<int>(std::round(minv + px * (maxv - minv)));
                    setValue(val);
                    emit sliderMoved(value());
                }
            }
            ev->accept();
            return;
        }
        QSlider::mouseMoveEvent(ev);
    }
    void mouseReleaseEvent(QMouseEvent *ev) override {
        if (isSliderDown() && ev->button() == Qt::LeftButton) {
            setSliderDown(false);
            emit sliderReleased();
            ev->accept();
            return;
        }
        QSlider::mouseReleaseEvent(ev);
    }
};

// --------------------------- GLWaveWidget ------------------
class GLWaveWidget: public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    explicit GLWaveWidget(unsigned sampleRate, QWidget *parent = nullptr) : QOpenGLWidget(parent), rate(sampleRate) {
        setFocusPolicy(Qt::StrongFocus);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        prog = 0; vbo = texSamples = 0;
        loc_u_time = loc_u_resolution = loc_u_vscale = loc_u_gain_display = loc_u_hue = loc_u_kick = loc_u_samples = -1;
        flashActive = false; beatCount = 0; headroomDb = g_targetHeadroomDbAbs;
        flashTimer.setSingleShot(true);
        connect(&flashTimer, &QTimer::timeout, this, [this]() { flashActive = false; update(); });
        setAttribute(Qt::WA_OpaquePaintEvent, true);
    }
    bool setShadersFromEditor(const QString &combinedEditorText, QString &errorOut) {
        if (!isValid() || !context() || !context()->isValid()) { errorOut = "GL context not ready yet."; return false; }
        makeCurrent();
        QString lower = combinedEditorText.toLower();
        int idxV = lower.indexOf("#shader vertex");
        int idxF = lower.indexOf("#shader fragment");
        if (idxV == -1 || idxF == -1 || idxF < idxV) { errorOut = "Editor must contain '#shader vertex' then '#shader fragment'."; doneCurrent(); return false; }
        int vStart = idxV + QString("#shader vertex").length();
        int fStart = idxF + QString("#shader fragment").length();
        QString vsrc = combinedEditorText.mid(vStart, idxF - vStart);
        QString fsrc = combinedEditorText.mid(fStart);
        if (vsrc.startsWith('\n')) vsrc = vsrc.mid(1);
        if (fsrc.startsWith('\n')) fsrc = fsrc.mid(1);
        QByteArray vba = vsrc.toUtf8(); const char *vptr = vba.constData();
        GLuint vs = glCreateShader(GL_VERTEX_SHADER); glShaderSource(vs, 1, &vptr, nullptr); glCompileShader(vs);
        GLint ok = 0; glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
        if (!ok) { GLint len=0; glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &len); std::string log(len>0?len:1,'\0'); glGetShaderInfoLog(vs,len,nullptr,&log[0]); errorOut = QString("Vertex compile error:\n%1").arg(QString::fromStdString(log)); glDeleteShader(vs); doneCurrent(); return false; }
        QByteArray fba = fsrc.toUtf8(); const char *fptr = fba.constData();
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(fs,1,&fptr,nullptr); glCompileShader(fs);
        glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
        if (!ok) { GLint len=0; glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &len); std::string log(len>0?len:1,'\0'); glGetShaderInfoLog(fs,len,nullptr,&log[0]); errorOut = QString("Fragment compile error:\n%1").arg(QString::fromStdString(log)); glDeleteShader(vs); glDeleteShader(fs); doneCurrent(); return false; }
        GLuint newProg = glCreateProgram(); glAttachShader(newProg, vs); glAttachShader(newProg, fs); glBindAttribLocation(newProg, 0, "a_pos"); glLinkProgram(newProg);
        glGetProgramiv(newProg, GL_LINK_STATUS, &ok);
        if (!ok) { GLint len=0; glGetProgramiv(newProg, GL_INFO_LOG_LENGTH, &len); std::string log(len>0?len:1,'\0'); glGetProgramInfoLog(newProg,len,nullptr,&log[0]); errorOut = QString("Link error:\n%1").arg(QString::fromStdString(log)); glDeleteProgram(newProg); glDeleteShader(vs); glDeleteShader(fs); doneCurrent(); return false; }

        if (prog) { glDeleteProgram(prog); }
        prog = newProg;
        glDeleteShader(vs);
        glDeleteShader(fs);

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
    void updateAudioTexture(const float *data, int n) {
        if (!isValid() || !context() || !context()->isValid()) return;
        makeCurrent();
        if (texSamples == 0) {
            glGenTextures(1, &texSamples);
            glBindTexture(GL_TEXTURE_1D, texSamples);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            std::vector<float> zeros(g_audioTexSize, 0.0f);
            glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, g_audioTexSize, 0, GL_RED, GL_FLOAT, zeros.data());
        } else {
            glBindTexture(GL_TEXTURE_1D, texSamples);
        }
        if (n == g_audioTexSize) {
            glTexSubImage1D(GL_TEXTURE_1D, 0, 0, g_audioTexSize, GL_RED, GL_FLOAT, data);
        } else {
            std::vector<float> tmp(g_audioTexSize);
            for (int i = 0; i < g_audioTexSize; ++i) tmp[i] = data[i % n];
            glTexSubImage1D(GL_TEXTURE_1D, 0, 0, g_audioTexSize, GL_RED, GL_FLOAT, tmp.data());
        }
        doneCurrent();
    }
    void triggerFlash() {
        const float HR_MIN = 0.0f, HR_MAX = 60.0f;
        float clamped = std::clamp(headroomDb, HR_MIN, HR_MAX);
        float t = (clamped - HR_MIN) / (HR_MAX - HR_MIN);
        float intensity = 1.0f - 0.6f * t;
        flashIntensity = intensity;
        float durationScale = 1.0f + (1.0f - t) * 0.8f;
        int dur = static_cast<int>(std::round(FLASH_MS * durationScale));
        flashActive = true;
        flashTimer.start(dur);
        update();
    }
    void setHeadroomDb(float db) { headroomDb = db; }
protected:
    void initializeGL() override {
        initializeOpenGLFunctions();
        float quad[] = { -1.f,-1.f, 1.f,-1.f, -1.f,1.f, 1.f,1.f };
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
        glGenTextures(1, &texSamples);
        glBindTexture(GL_TEXTURE_1D, texSamples);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        std::vector<float> zeros(g_audioTexSize, 0.0f);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, g_audioTexSize, 0, GL_RED, GL_FLOAT, zeros.data());
    }
    void paintGL() override {
        if (flashActive) {
            float c = std::clamp(flashIntensity, 0.0f, 1.0f);
            glClearColor(c,c,c,1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            return;
        }
        glClearColor(0.06f,0.06f,0.06f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        if (!prog) { glClearColor(1.0f,0.0f,1.0f,1.0f); glClear(GL_COLOR_BUFFER_BIT); return; }
        glUseProgram(prog);
        float t = float(QDateTime::currentMSecsSinceEpoch() % 0x7fffffff) * 0.001f;
        if (loc_u_time >= 0) glUniform1f(loc_u_time, t);
        if (loc_u_resolution >= 0) glUniform2f(loc_u_resolution, float(width()), float(height()));
        if (loc_u_vscale >= 0) glUniform1f(loc_u_vscale, 0.4f);
        if (loc_u_gain_display >= 0) glUniform1f(loc_u_gain_display, 0.8f);
        if (loc_u_hue >= 0) glUniform1f(loc_u_hue, 0.12f);
        if (loc_u_kick >= 0) glUniform1i(loc_u_kick, lastBeat ? 1 : 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_1D, texSamples);
        if (loc_u_samples >= 0) glUniform1i(loc_u_samples, 0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,nullptr);
        glDrawArrays(GL_TRIANGLE_STRIP,0,4);
        glDisableVertexAttribArray(0);
        glUseProgram(0);
    }
    void resizeGL(int w,int h) override { glViewport(0,0,w,h); update(); }
    void keyPressEvent(QKeyEvent *ev) override {
        if (ev->key() == Qt::Key_F) { emit requestToggleFullscreen(); ev->accept(); return; }
        if (ev->key() == Qt::Key_Escape) { emit requestExitFullscreen(); ev->accept(); return; }
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
    GLuint prog, vbo, texSamples;
    GLint loc_u_time, loc_u_resolution, loc_u_vscale, loc_u_gain_display, loc_u_hue, loc_u_kick, loc_u_samples;
    bool flashActive;
    float flashIntensity = 1.0f;
    QTimer flashTimer;
    std::mt19937 rng{ std::random_device{}() };
    float headroomDb;
};

// --------------------------- QtAudioInput (with limiter/hysteresis) -----------------
class QtAudioInput: public QObject {
    Q_OBJECT
public:
    explicit QtAudioInput(unsigned sr, QObject *parent = nullptr) : QObject(parent), rate(sr), audioSource(nullptr), audioIO(nullptr),
        runningAvg(0.0f), runningAvgInit(false), lastKickMs(0), glw(nullptr), lastHwPercent(g_hwPercentDefault),
        lastHwAdjustMs(0), lastLimiterAdjustMs(0), limiterEngaged(false), preLimiterPercent(g_hwPercentDefault) {}

    ~QtAudioInput() { stop(); }

    bool start() {
        QAudioFormat fmt; fmt.setSampleRate(static_cast<int>(rate)); fmt.setChannelCount(1); fmt.setSampleFormat(QAudioFormat::Int16);
        QAudioDevice device = QMediaDevices::defaultAudioInput();
        if (!device.isFormatSupported(fmt)) { fmt = device.preferredFormat(); qWarning() << "Requested audio format not supported; using device preferred format."; }
        audioSource = new QAudioSource(device, fmt, this);
        audioIO = audioSource->start();
        if (!audioIO) { qWarning() << "QAudioSource::start() failed"; delete audioSource; audioSource=nullptr; return false; }
        connect(audioIO, &QIODevice::readyRead, this, &QtAudioInput::onReadyRead);
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

    void setTargetWidget(GLWaveWidget *w) { glw = w; }

    bool isLimiterEngaged() const { return limiterEngaged; }
    int currentHardwarePercent() const { return lastHwPercent; }

private slots:
    void onReadyRead() {
        if (!audioIO) return;
        const int bytesPerFrame = 2;
        while (audioIO->bytesAvailable() >= g_sampleBufferFrames * bytesPerFrame) {
            QByteArray chunk = audioIO->read(g_sampleBufferFrames * bytesPerFrame);
            if (chunk.size() != g_sampleBufferFrames * bytesPerFrame) break;
            const int16_t *samples = reinterpret_cast<const int16_t*>(chunk.constData());
            size_t frames = g_sampleBufferFrames;

            push_to_ring_from_s16(samples, frames);

            float rms = compute_rms_s16(samples, frames);
            if (!runningAvgInit) { runningAvg = rms; runningAvgInit = true; }
            runningAvg = 0.08f * rms + (1.0f - 0.08f) * runningAvg;

            bool isBeat = false;
            quint32 now = static_cast<quint32>(QDateTime::currentMSecsSinceEpoch() & 0xffffffff);
            if (rms > runningAvg * 2.5f && (now - lastKickMs) > 200) { isBeat = true; lastKickMs = now; }

            float peak = 0.0f;
            for (size_t i = 0; i < frames; ++i) {
                float v = std::abs(samples[i]) / 32768.0f;
                if (v > peak) peak = v;
            }
            float peakDb = 20.0f * std::log10(std::max(peak, 1e-9f));

            if (g_limiterEnabled) enforceHardwareLimiterWithHysteresis(peakDb);
            else if (limiterEngaged) {
                limiterEngaged = false;
                if (preLimiterPercent >= g_hwPercentMin && preLimiterPercent <= g_hwPercentMax) {
                    if (setHardwareMicVolumePercent(preLimiterPercent)) lastHwPercent = preLimiterPercent;
                }
            }

            adjustHardwareGainIfNeeded(rms);

            if (glw) {
                glw->lastBeat = isBeat;
                if (isBeat) { glw->beatCount++; glw->setHeadroomDb(g_targetHeadroomDbAbs); glw->triggerFlash(); }
                std::vector<float> tex(g_audioTexSize);
                read_ring_into_float_array(tex.data(), g_audioTexSize);
                float displayGain = computeDisplayGainFromHeadroom(g_targetHeadroomDbAbs);
                if (displayGain != 1.0f) for (int i = 0; i < g_audioTexSize; ++i) tex[i] *= displayGain;
                glw->updateAudioTexture(tex.data(), g_audioTexSize);
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
    qint64 lastLimiterAdjustMs;

    bool limiterEngaged;
    int preLimiterPercent;

    static inline float rmsToDbFS(float rms) { const float eps = 1e-9f; return 20.0f * std::log10(std::max(rms, eps)); }

    void enforceHardwareLimiterWithHysteresis(float peakDb) {
        float ceilingDbFS = -g_limiterCeilingDb;
        float engageThreshold = ceilingDbFS + g_limiterHysteresisOnDb;
        float releaseThreshold = ceilingDbFS - g_limiterHysteresisOffDb;
        qint64 nowMs = QDateTime::currentMSecsSinceEpoch();
        if (limiterEngaged) {
            if (peakDb < releaseThreshold) {
                if ((nowMs - lastLimiterAdjustMs) >= g_limiterAdjustMinMs) {
                    if (preLimiterPercent >= g_hwPercentMin && preLimiterPercent <= g_hwPercentMax) {
                        if (setHardwareMicVolumePercent(preLimiterPercent)) {
                            lastHwPercent = preLimiterPercent;
                            limiterEngaged = false;
                            lastLimiterAdjustMs = nowMs;
                        }
                    } else limiterEngaged = false;
                }
            } else {
                if ((nowMs - lastLimiterAdjustMs) >= g_limiterAdjustMinMs) {
                    float neededDb = peakDb - ceilingDbFS;
                    if (neededDb > 0.1f) {
                        float factor = std::pow(10.0f, -neededDb / 20.0f);
                        float newPercentF = static_cast<float>(lastHwPercent) * factor;
                        int newPercent = static_cast<int>(std::floor(newPercentF));
                        if (newPercent < g_hwPercentMin) newPercent = g_hwPercentMin;
                        if (newPercent > g_hwPercentMax) newPercent = g_hwPercentMax;
                        if (std::abs(newPercent - lastHwPercent) >= 1) {
                            if (setHardwareMicVolumePercent(newPercent)) {
                                lastHwPercent = newPercent;
                                lastLimiterAdjustMs = nowMs;
                            }
                        }
                    }
                }
            }
        } else {
            if (peakDb > engageThreshold) {
                if ((nowMs - lastLimiterAdjustMs) < g_limiterAdjustMinMs) return;
                float neededDb = peakDb - ceilingDbFS;
                float factor = std::pow(10.0f, -neededDb / 20.0f);
                float newPercentF = static_cast<float>(lastHwPercent) * factor;
                int newPercent = static_cast<int>(std::floor(newPercentF));
                if (newPercent < g_hwPercentMin) newPercent = g_hwPercentMin;
                if (newPercent > g_hwPercentMax) newPercent = g_hwPercentMax;
                if (std::abs(newPercent - lastHwPercent) < 1) {
                    limiterEngaged = true;
                    preLimiterPercent = lastHwPercent;
                    lastLimiterAdjustMs = nowMs;
                    return;
                }
                preLimiterPercent = lastHwPercent;
                if (setHardwareMicVolumePercent(newPercent)) {
                    lastHwPercent = newPercent;
                    limiterEngaged = true;
                    lastLimiterAdjustMs = nowMs;
                }
            }
        }
    }

    void adjustHardwareGainIfNeeded(float rms) {
        qint64 nowMs = QDateTime::currentMSecsSinceEpoch();
        if ((nowMs - lastHwAdjustMs) < g_hwAdjustIntervalMs) return;
        float targetHeadroomDb = -g_targetHeadroomDbAbs;
        float currentDb = rmsToDbFS(rms);
        float deltaDb = currentDb - targetHeadroomDb;
        if (std::abs(deltaDb) < 0.1f) return;
        float factor = std::pow(10.0f, -deltaDb / 20.0f);
        float newPercentF = static_cast<float>(lastHwPercent) * factor;
        int newPercent = static_cast<int>(std::round(newPercentF));
        if (newPercent < g_hwPercentMin) newPercent = g_hwPercentMin;
        if (newPercent > g_hwPercentMax) newPercent = g_hwPercentMax;
        if (std::abs(newPercent - lastHwPercent) < 1) return;
        if (setHardwareMicVolumePercent(newPercent)) {
            lastHwPercent = newPercent;
            lastHwAdjustMs = nowMs;
        }
    }
};

// --------------------------- Default editor content ------------------
static const char *DEFAULT_EDITOR = R"(#shader vertex
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

// --------------------------- Utility: compute display gain ------------------
static inline float computeDisplayGainFromHeadroom(float headroomDb) {
    float diff = (g_displayGainRefDb - headroomDb);
    const float MAX_DIFF = 36.0f;
    if (diff > MAX_DIFF) diff = MAX_DIFF;
    if (diff < -MAX_DIFF) diff = -MAX_DIFF;
    return std::pow(10.0f, diff / 20.0f);
}

// --------------------------- Main ---------------------------------------
int main(int argc, char **argv) {
    QApplication a(argc, argv);

    QSettings settings("glsl-visualizer", "glsl-visualizer");
    g_sampleRate = settings.value("sampleRate", (int)g_sampleRate).toUInt();
    g_ringSeconds = settings.value("ringSeconds", g_ringSeconds).toInt();
    g_sampleBufferFrames = settings.value("sampleBufferFrames", g_sampleBufferFrames).toInt();
    g_audioTexSize = settings.value("audioTexSize", g_audioTexSize).toInt();
    g_targetHeadroomDbAbs = settings.value("headroomDb", g_targetHeadroomDbAbs).toFloat();
    g_limiterEnabled = settings.value("limiterEnabled", g_limiterEnabled).toBool();
    g_limiterCeilingDb = settings.value("limiterCeilingDb", g_limiterCeilingDb).toFloat();
    g_limiterHysteresisOnDb = settings.value("limiterHysteresisOnDb", g_limiterHysteresisOnDb).toFloat();
    g_limiterHysteresisOffDb = settings.value("limiterHysteresisOffDb", g_limiterHysteresisOffDb).toFloat();
    g_hwPercentDefault = settings.value("hwPercentDefault", g_hwPercentDefault).toInt();
    g_hwPercentMin = settings.value("hwPercentMin", g_hwPercentMin).toInt();
    g_hwPercentMax = settings.value("hwPercentMax", g_hwPercentMax).toInt();
    g_hwAdjustIntervalMs = settings.value("hwAdjustIntervalMs", g_hwAdjustIntervalMs).toInt();
    g_limiterAdjustMinMs = settings.value("limiterAdjustMinMs", g_limiterAdjustMinMs).toInt();
    g_displayGainRefDb = settings.value("displayGainRefDb", g_displayGainRefDb).toFloat();

    allocateRingBuffer(g_ringSeconds);

    QMainWindow *mainWin = new QMainWindow;
    mainWin->setWindowTitle(QStringLiteral("GLSL Audio Visualizer - UI with progress bars"));

    QPlainTextEdit *editor = new QPlainTextEdit(QString::fromUtf8(DEFAULT_EDITOR));
    GLSLHighlighter *hl = new GLSLHighlighter(editor->document());
    Q_UNUSED(hl);
    editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    editor->setMinimumWidth(320);
    mainWin->setCentralWidget(editor);

    GLWaveWidget *glw = new GLWaveWidget(g_sampleRate);
    glw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    glw->setMinimumSize(0,0);

    QDockWidget *dockPreview = new QDockWidget(QStringLiteral("Preview"), mainWin);
    dockPreview->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    dockPreview->setWidget(glw);
    dockPreview->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    mainWin->addDockWidget(Qt::RightDockWidgetArea, dockPreview);

    QTextEdit *logWidget = new QTextEdit; logWidget->setReadOnly(true);
    QDockWidget *dockLog = new QDockWidget(QStringLiteral("Shader Log"), mainWin);
    dockLog->setWidget(logWidget);
    mainWin->addDockWidget(Qt::BottomDockWidgetArea, dockLog);

    QToolBar *tb = mainWin->addToolBar(QStringLiteral("Tools"));
    tb->setMovable(false);

    QAction *actCompile = tb->addAction(QStringLiteral("Compile"));
    QAction *actTogglePreview = tb->addAction(QStringLiteral("Toggle Preview"));
    QAction *actToggleLog = tb->addAction(QStringLiteral("Toggle Log"));
    QAction *actFullscreen = tb->addAction(QStringLiteral("Fullscreen Preview"));

    // Flow widget inside toolbar
    QWidget *flowWidget = new QWidget;
    FlowLayout *flow = new FlowLayout(flowWidget, 2, 6, 6);
    flowWidget->setLayout(flow);

    // Microphone (leftmost) controls
    QLabel *lblMic = new QLabel("Mic"); lblMic->setMinimumWidth(40); flow->addWidget(lblMic);
    QProgressBar *pbMic = new QProgressBar; pbMic->setFixedSize(100, 14); pbMic->setRange(0, 100); pbMic->setValue(0); pbMic->setTextVisible(false); flow->addWidget(pbMic);
    QLabel *lblMicHw = new QLabel(QString("HW %: --")); lblMicHw->setMinimumWidth(60); flow->addWidget(lblMicHw);

    QLabel *lblHead = new QLabel("Headroom"); lblHead->setMinimumWidth(70); flow->addWidget(lblHead);
    ClickDragSlider *headroomSlider = new ClickDragSlider(Qt::Horizontal); headroomSlider->setRange(0, 1000); headroomSlider->setFixedWidth(220); flow->addWidget(headroomSlider);
    QLabel *lblHeadVal = new QLabel; lblHeadVal->setMinimumWidth(80); lblHeadVal->setAlignment(Qt::AlignRight | Qt::AlignVCenter); flow->addWidget(lblHeadVal);

    // Limiter controls (center)
    QCheckBox *chkLimiter = new QCheckBox("Limiter"); chkLimiter->setChecked(g_limiterEnabled); flow->addWidget(chkLimiter);
    QLabel *lblLim = new QLabel("Ceiling"); lblLim->setMinimumWidth(50); flow->addWidget(lblLim);
    ClickDragSlider *limiterSlider = new ClickDragSlider(Qt::Horizontal); limiterSlider->setRange(0, 2000); limiterSlider->setFixedWidth(160); flow->addWidget(limiterSlider);
    QLabel *lblLimVal = new QLabel; lblLimVal->setMinimumWidth(70); lblLimVal->setAlignment(Qt::AlignRight | Qt::AlignVCenter); flow->addWidget(lblLimVal);
    QProgressBar *pbLimiterOut = new QProgressBar; pbLimiterOut->setFixedSize(100, 14); pbLimiterOut->setRange(0, 100); pbLimiterOut->setValue(0); pbLimiterOut->setTextVisible(false); flow->addWidget(pbLimiterOut);

    QLabel *lblHystOn = new QLabel("Hyst On"); lblHystOn->setMinimumWidth(60); flow->addWidget(lblHystOn);
    QDoubleSpinBox *spinHystOn = new QDoubleSpinBox; spinHystOn->setRange(0.0, 10.0); spinHystOn->setDecimals(2); spinHystOn->setValue(g_limiterHysteresisOnDb); spinHystOn->setSingleStep(0.1); spinHystOn->setFixedWidth(80); flow->addWidget(spinHystOn);

    QLabel *lblHystOff = new QLabel("Hyst Off"); lblHystOff->setMinimumWidth(60); flow->addWidget(lblHystOff);
    QDoubleSpinBox *spinHystOff = new QDoubleSpinBox; spinHystOff->setRange(0.0, 10.0); spinHystOff->setDecimals(2); spinHystOff->setValue(g_limiterHysteresisOffDb); spinHystOff->setSingleStep(0.1); spinHystOff->setFixedWidth(80); flow->addWidget(spinHystOff);

    // Ring buffer and frames controls (towards right)
    QLabel *lblRing = new QLabel("Ring s"); lblRing->setMinimumWidth(50); flow->addWidget(lblRing);
    QSpinBox *spinRing = new QSpinBox; spinRing->setRange(1, 60); spinRing->setValue(g_ringSeconds); spinRing->setFixedWidth(70); flow->addWidget(spinRing);

    QLabel *lblFrames = new QLabel("Frames"); lblFrames->setMinimumWidth(60); flow->addWidget(lblFrames);
    QSpinBox *spinFrames = new QSpinBox; spinFrames->setRange(32, 2048); spinFrames->setValue(g_sampleBufferFrames); spinFrames->setFixedWidth(80); flow->addWidget(spinFrames);

    QLabel *lblTex = new QLabel("Tex"); lblTex->setMinimumWidth(30); flow->addWidget(lblTex);
    QSpinBox *spinTex = new QSpinBox; spinTex->setRange(256, 8192); spinTex->setValue(g_audioTexSize); spinTex->setFixedWidth(80); flow->addWidget(spinTex);

    // Output (rightmost) controls
    QLabel *lblOut = new QLabel("Visualizer Out"); lblOut->setMinimumWidth(100); flow->addWidget(lblOut);
    QProgressBar *pbOut = new QProgressBar; pbOut->setFixedSize(100, 14); pbOut->setRange(0, 100); pbOut->setValue(0); pbOut->setTextVisible(false); flow->addWidget(pbOut);

    tb->addWidget(flowWidget);

    // Status bar
    QStatusBar *sb = mainWin->statusBar();
    QLabel *lblBeat = new QLabel("Beats: 0");
    QLabel *lblAudio = new QLabel("Audio: init");
    sb->addPermanentWidget(lblAudio);
    sb->addPermanentWidget(lblBeat);

    // Mapping helpers for headroom slider (log-like)
    auto linearToLog = [&](float linear01)->float {
        const float HR_MIN = 0.0f, HR_MAX = 60.0f;
        if (linear01 <= 0.0f) return HR_MIN;
        if (linear01 >= 1.0f) return HR_MAX;
        float lmin = std::log(HR_MIN + 1.0f), lmax = std::log(HR_MAX + 1.0f);
        return std::exp(lmin + linear01 * (lmax - lmin)) - 1.0f;
    };
    auto logToLinear = [&](float val)->float {
        const float HR_MIN = 0.0f, HR_MAX = 60.0f;
        if (val <= HR_MIN) return 0.0f;
        if (val >= HR_MAX) return 1.0f;
        float lmin = std::log(HR_MIN + 1.0f), lmax = std::log(HR_MAX + 1.0f);
        return (std::log(val + 1.0f) - lmin) / (lmax - lmin);
    };

    // Initialize UI values
    {
        float cur = std::clamp(g_targetHeadroomDbAbs, 0.0f, 60.0f);
        float lin = logToLinear(cur);
        headroomSlider->setValue(static_cast<int>(std::round(lin * 1000.0f)));
        lblHeadVal->setText(QString::asprintf("%.2f dB", cur));
        limiterSlider->setValue(static_cast<int>(std::round(g_limiterCeilingDb * 100.0f)));
        lblLimVal->setText(QString::asprintf("%.2f dB", g_limiterCeilingDb));
    }

    // Restore geometry/state
    if (settings.contains("geometry")) mainWin->restoreGeometry(settings.value("geometry").toByteArray());
    if (settings.contains("windowState")) mainWin->restoreState(settings.value("windowState").toByteArray());

    // Audio input
    QtAudioInput audioIn(g_sampleRate);
    audioIn.setTargetWidget(glw);
    bool audioStarted = audioIn.start();
    if (!audioStarted) {
        qWarning() << "Audio input could not be started via Qt Multimedia.";
        lblAudio->setText("Audio: failed");
    } else {
        lblAudio->setText("Audio: running");
    }

    // initial hardware percent
    if (!setHardwareMicVolumePercent(g_hwPercentDefault)) {
        qWarning() << "Initial hardware mic volume set failed (ALSA).";
    }

    // Debounced shader compile
    QTimer *debounce = new QTimer(&a); debounce->setSingleShot(true); debounce->setInterval(250);
    auto compileNow = [&](bool showLog=true){
        QString src = editor->toPlainText(); QString log;
        if (!glw->setShadersFromEditor(src, log)) {
            if (showLog) logWidget->append("=== Compile Error ===\n" + log);
        } else {
            if (showLog) logWidget->append("Shaders compiled successfully.");
        }
        glw->update();
    };
    QObject::connect(editor, &QPlainTextEdit::textChanged, [&](){ debounce->start(); });
    QObject::connect(debounce, &QTimer::timeout, [&](){ if (!glw->isValid()||!glw->context()||!glw->context()->isValid()){ QTimer::singleShot(150, [&](){ debounce->start(); }); return; } compileNow(true); });
    QObject::connect(actCompile, &QAction::triggered, [&](){ compileNow(true); });
    QObject::connect(actTogglePreview, &QAction::triggered, [&](){ dockPreview->setVisible(!dockPreview->isVisible()); });
    QObject::connect(actToggleLog, &QAction::triggered, [&](){ dockLog->setVisible(!dockLog->isVisible()); });
    QObject::connect(actFullscreen, &QAction::triggered, [&](){ if (dockPreview->isFloating()) dockPreview->showMaximized(); else { dockPreview->setFloating(true); dockPreview->showMaximized(); } });

    // UI interactions
    QObject::connect(headroomSlider, &QSlider::valueChanged, [&](int v){
        float linear01 = float(v) / 1000.0f;
        float valDb = linearToLog(linear01);
        valDb = std::clamp(valDb, 0.0f, 60.0f);
        g_targetHeadroomDbAbs = valDb;
        lblHeadVal->setText(QString::asprintf("%.2f dB", valDb));
        float displayGain = computeDisplayGainFromHeadroom(g_targetHeadroomDbAbs);
        std::vector<float> tex(g_audioTexSize); read_ring_into_float_array(tex.data(), g_audioTexSize);
        if (displayGain != 1.0f) for (int i=0;i<g_audioTexSize;++i) tex[i] *= displayGain;
        glw->updateAudioTexture(tex.data(), g_audioTexSize);
        glw->setHeadroomDb(g_targetHeadroomDbAbs);
        settings.setValue("headroomDb", g_targetHeadroomDbAbs);
    });

    QObject::connect(chkLimiter, &QCheckBox::toggled, [&](bool on){
        g_limiterEnabled = on;
        settings.setValue("limiterEnabled", g_limiterEnabled);
    });

    QObject::connect(limiterSlider, &QSlider::valueChanged, [&](int v){
        float val = float(v) / 100.0f;
        g_limiterCeilingDb = val;
        lblLimVal->setText(QString::asprintf("%.2f dB", val));
        settings.setValue("limiterCeilingDb", g_limiterCeilingDb);
    });

    QObject::connect(spinHystOn, qOverload<double>(&QDoubleSpinBox::valueChanged), [&](double v){
        g_limiterHysteresisOnDb = static_cast<float>(v);
        settings.setValue("limiterHysteresisOnDb", g_limiterHysteresisOnDb);
    });
    QObject::connect(spinHystOff, qOverload<double>(&QDoubleSpinBox::valueChanged), [&](double v){
        g_limiterHysteresisOffDb = static_cast<float>(v);
        settings.setValue("limiterHysteresisOffDb", g_limiterHysteresisOffDb);
    });

    QObject::connect(spinRing, qOverload<int>(&QSpinBox::valueChanged), [&](int v){
        g_ringSeconds = v;
        allocateRingBuffer(g_ringSeconds);
        settings.setValue("ringSeconds", g_ringSeconds);
    });

    QObject::connect(spinFrames, qOverload<int>(&QSpinBox::valueChanged), [&](int v){
        g_sampleBufferFrames = v;
        settings.setValue("sampleBufferFrames", g_sampleBufferFrames);
    });

    QObject::connect(spinTex, qOverload<int>(&QSpinBox::valueChanged), [&](int v){
        g_audioTexSize = v;
        settings.setValue("audioTexSize", g_audioTexSize);
    });

    // Periodic UI update: progress bars and ALSA percent
    QTimer uiTimer;
    uiTimer.setInterval(100);
    QObject::connect(&uiTimer, &QTimer::timeout, [&](){
        int hwPercent = getHardwareMicVolumePercent();
        if (hwPercent >= 0) lblMicHw->setText(QString("HW %: %1").arg(hwPercent));
        else lblMicHw->setText(QString("HW %: --"));

        int micPercent = compute_recent_output_percent(128);
        pbMic->setValue(micPercent);

        int limiterOutPercent = micPercent;
        if (audioIn.isLimiterEngaged()) {
            int curHw = audioIn.currentHardwarePercent();
            if (curHw >= 0) limiterOutPercent = static_cast<int>(std::round(micPercent * (curHw / 100.0)));
        }
        pbLimiterOut->setValue(limiterOutPercent);

        int outPercent = compute_recent_output_percent(128);
        float dg = computeDisplayGainFromHeadroom(g_targetHeadroomDbAbs);
        float scaled = std::clamp<float>(outPercent * dg, 0.0f, 100.0f);
        pbOut->setValue(static_cast<int>(std::round(scaled)));

        lblBeat->setText(QString("Beats: %1").arg(glw->beatCount));
        glw->update();
    });
    uiTimer.start();

    QTimer::singleShot(300, [&](){ debounce->start(); });

    mainWin->resize(1200,720);
    mainWin->show();

    int ret = a.exec();

    settings.setValue("geometry", mainWin->saveGeometry());
    settings.setValue("windowState", mainWin->saveState());
    settings.setValue("sampleRate", (int)g_sampleRate);
    settings.setValue("hwPercentDefault", g_hwPercentDefault);
    settings.setValue("hwPercentMin", g_hwPercentMin);
    settings.setValue("hwPercentMax", g_hwPercentMax);
    settings.setValue("hwAdjustIntervalMs", g_hwAdjustIntervalMs);
    settings.setValue("limiterAdjustMinMs", g_limiterAdjustMinMs);
    settings.setValue("displayGainRefDb", g_displayGainRefDb);

    audioIn.stop();
    return ret;
}

#include "main.moc"
