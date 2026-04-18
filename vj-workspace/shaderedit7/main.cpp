// main.cpp
// Qt6 single-file GLSL editor + preview
// - Linke Hälfte: Editor (Vertex+Fragment shader, default: procedural cube)
// - Rechte Hälfte: Preview (renders cube procedurally via gl_VertexID)
// - Komplett ohne FFT: einfache, robuste Beat‑Erkennung per RMS (kein FFT)
// - Visualisierung bei Beat: kurzer weißer Blitz (ganzer Preview wird kurz komplett weiß)
// - Single-threaded ALSA polling via QTimer (keine QThread)
// - Qt6, C++20

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
#include <QTextCursor>
#include <alsa/asoundlib.h>
#include <cmath>
#include <vector>
#include <random>
#include <string>
#include <cstdarg>
#include <cstdlib>

// --------------------------- Konfiguration ---------------------------
constexpr unsigned DEFAULT_SR = 48000;
constexpr int BUFFERSIZE = 512;            // ALSA read frames per poll
constexpr int RING_SECONDS = 4;            // ring buffer seconds
constexpr int POLL_MS = 10;                // Poll-Intervall in ms
constexpr int FLASH_MS = 120;              // Dauer des weißen Blitzes in ms
constexpr int KICK_DEBOUNCE_MS = 200;      // minimale Zeit zwischen Kicks
constexpr float BEAT_THRESHOLD = 2.5f;     // beat wenn RMS > running_avg * threshold
constexpr float RMS_ALPHA = 0.08f;         // Glättung für laufender Mittelwert

// --------------------------- Hilfsfunktionen -------------------------
static inline void fatalError(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    std::vfprintf(stderr, fmt, ap);
    std::fprintf(stderr, "\n");
    va_end(ap);
    qCritical().noquote() << QString("FATAL: ") + QString::fromUtf8(fmt);
    std::abort();
}

// --------------------------- ALSA Capture Control --------------------
static snd_mixer_t *g_mixer = nullptr;
static snd_mixer_elem_t *g_selem = nullptr;
static long g_vol_min = 0, g_vol_max = 0;

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
    if (v < g_vol_min) v = g_vol_min;
    if (v > g_vol_max) v = g_vol_max;
    snd_mixer_selem_set_capture_volume_all(g_selem, v);
}

// --------------------------- Ring Buffer (mono float) -----------------
static std::vector<float> g_ring;
static size_t g_ring_cap = 0;
static size_t g_ring_pos = 0;
static QMutex g_ring_mutex;

static void push_to_ring_from_s16(const int16_t *in, size_t n) {
    QMutexLocker locker(&g_ring_mutex);
    if (g_ring_cap == 0) return;
    for (size_t i = 0; i < n; ++i) {
        g_ring[g_ring_pos] = in[i] / 32768.0f;
        g_ring_pos = (g_ring_pos + 1) % g_ring_cap;
    }
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

// --------------------------- GLSL Highlighter (klein) ------------------
class GLSLHighlighter : public QSyntaxHighlighter {
public:
    GLSLHighlighter(QTextDocument *parent = nullptr) : QSyntaxHighlighter(parent) {
        QTextCharFormat kw; kw.setForeground(QColor("#569CD6")); kw.setFontWeight(QFont::Bold);
        QStringList keywords = {"uniform","varying","attribute","void","float","int","vec2","vec3","vec4","mat3","mat4","if","else","return","for","while"};
        for (const QString &k : keywords) rules.append({QRegularExpression("\\b" + QRegularExpression::escape(k) + "\\b"), kw});
        QTextCharFormat com; com.setForeground(QColor("#6A9955"));
        rules.append({QRegularExpression("//[^\\n]*"), com});
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

// --------------------------- GL Widget (Cube + Flash) ------------------
class GLWaveWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    explicit GLWaveWidget(unsigned sampleRate, QWidget *parent = nullptr)
        : QOpenGLWidget(parent), rate(sampleRate) {
        setFocusPolicy(Qt::StrongFocus);
        prog = 0;
        loc_u_time = loc_u_resolution = loc_u_beat = loc_u_beatCount = -1;
        flashActive = false;
        beatCount = 0;
        randSeed = float(std::uniform_real_distribution<float>(0.0f,1.0f)(rng));
    }

    // set shaders from combined editor text (#shader vertex / #shader fragment)
    bool setShadersFromEditor(const QString &combinedEditorText, QString &errorOut) {
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
            errorOut = "Editor must contain '#shader vertex' then '#shader fragment'.";
            doneCurrent();
            return false;
        }
        int vStart = idxV + QString("#shader vertex").length();
        int fStart = idxF + QString("#shader fragment").length();
        QString vsrc = combinedEditorText.mid(vStart, idxF - vStart);
        QString fsrc = combinedEditorText.mid(fStart);
        if (vsrc.startsWith('\n')) vsrc = vsrc.mid(1);
        if (fsrc.startsWith('\n')) fsrc = fsrc.mid(1);

        // compile vertex
        QByteArray vba = vsrc.toUtf8();
        const char *vptr = vba.constData();
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vptr, nullptr);
        glCompileShader(vs);
        GLint ok = 0; glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            GLint len = 0; glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &len);
            std::string log(len>0?len:1, '\0');
            glGetShaderInfoLog(vs, len, nullptr, &log[0]);
            errorOut = QString("Vertex compile error:\n%1").arg(QString::fromStdString(log));
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
            GLint len = 0; glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &len);
            std::string log(len>0?len:1, '\0');
            glGetShaderInfoLog(fs, len, nullptr, &log[0]);
            errorOut = QString("Fragment compile error:\n%1").arg(QString::fromStdString(log));
            glDeleteShader(vs); glDeleteShader(fs);
            doneCurrent();
            return false;
        }

        // link
        GLuint newProg = glCreateProgram();
        glAttachShader(newProg, vs);
        glAttachShader(newProg, fs);
        glLinkProgram(newProg);
        glGetProgramiv(newProg, GL_LINK_STATUS, &ok);
        if (!ok) {
            GLint len = 0; glGetProgramiv(newProg, GL_INFO_LOG_LENGTH, &len);
            std::string log(len>0?len:1, '\0');
            glGetProgramInfoLog(newProg, len, nullptr, &log[0]);
            errorOut = QString("Link error:\n%1").arg(QString::fromStdString(log));
            glDeleteProgram(newProg); glDeleteShader(vs); glDeleteShader(fs);
            doneCurrent();
            return false;
        }

        // replace program
        if (prog) glDeleteProgram(prog);
        prog = newProg;
        glDeleteShader(vs); glDeleteShader(fs);

        // cache uniforms
        loc_u_time = glGetUniformLocation(prog, "u_time");
        loc_u_resolution = glGetUniformLocation(prog, "u_resolution");
        loc_u_beat = glGetUniformLocation(prog, "u_beat");
        loc_u_beatCount = glGetUniformLocation(prog, "u_beatCount");
        loc_u_randSeed = glGetUniformLocation(prog, "u_randSeed");

        doneCurrent();
        return true;
    }

    // trigger white flash for FLASH_MS
    void triggerFlash() {
        flashActive = true;
        flashTimer.start(FLASH_MS);
        update();
    }

protected:
    void initializeGL() override {
        initializeOpenGLFunctions();
        // timer to clear flash
        flashTimer.setSingleShot(true);
        connect(&flashTimer, &QTimer::timeout, this, [this](){ flashActive = false; update(); });
    }

    void paintGL() override {
        if (flashActive) {
            // kompletter weißer Blitz
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            return;
        }

        glClearColor(0.06f, 0.06f, 0.06f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        if (!prog) {
            // magenta als Hinweis auf fehlendes Shader-Programm
            glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            return;
        }

        glUseProgram(prog);
        float t = float(QDateTime::currentMSecsSinceEpoch() % 0x7fffffff) * 0.001f;
        if (loc_u_time >= 0) glUniform1f(loc_u_time, t);
        if (loc_u_resolution >= 0) glUniform2f(loc_u_resolution, float(width()), float(height()));
        if (loc_u_beat >= 0) glUniform1i(loc_u_beat, lastBeat ? 1 : 0);
        if (loc_u_beatCount >= 0) glUniform1i(loc_u_beatCount, beatCount);
        if (loc_u_randSeed >= 0) glUniform1f(loc_u_randSeed, randSeed);

        // draw cube procedurally (vertex shader must generate geometry via gl_VertexID)
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glUseProgram(0);
    }

    void resizeGL(int w, int h) override { glViewport(0, 0, w, h); }

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
    GLuint prog;
    GLint loc_u_time, loc_u_resolution, loc_u_beat, loc_u_beatCount, loc_u_randSeed;
    bool flashActive;
    QTimer flashTimer;
    std::mt19937 rng{ std::random_device{}() };
    float randSeed;
};

// --------------------------- ALSA Poller (single-threaded) -----------------
class AlsaPoller : public QObject {
    Q_OBJECT
public:
    explicit AlsaPoller(unsigned sr, QObject *parent = nullptr) : QObject(parent), rate(sr), pcm(nullptr) {}

    ~AlsaPoller() {
        if (pcm) { snd_pcm_close(pcm); pcm = nullptr; }
    }

    bool openDevice() {
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

    // poll once from GUI thread
    void pollOnce(GLWaveWidget *glw) {
        if (!pcm) return;
        std::vector<int16_t> buf(BUFFERSIZE);
        snd_pcm_sframes_t frames = snd_pcm_readi(pcm, buf.data(), BUFFERSIZE);
        if (frames == -EAGAIN || frames == -EWOULDBLOCK) return;
        if (frames == -EPIPE) { snd_pcm_prepare(pcm); return; }
        if (frames < 0) {
            frames = snd_pcm_recover(pcm, frames, 0);
            if (frames < 0) { qWarning() << "snd_pcm_recover failed"; return; }
        }
        if (frames == 0) return;

        // push to ring
        push_to_ring_from_s16(buf.data(), static_cast<size_t>(frames));

        // simple RMS-based beat detection
        float rms = compute_rms_s16(buf.data(), static_cast<size_t>(frames));
        // running average (exponential)
        if (!runningAvgInitialized) { runningAvg = rms; runningAvgInitialized = true; }
        runningAvg = RMS_ALPHA * rms + (1.0f - RMS_ALPHA) * runningAvg;

        bool isBeat = false;
        quint32 now = static_cast<quint32>(QDateTime::currentMSecsSinceEpoch() & 0xffffffff);
        if (rms > runningAvg * BEAT_THRESHOLD && (now - lastKickMs) > KICK_DEBOUNCE_MS) {
            isBeat = true;
            lastKickMs = now;
        }

        // update GL widget state and trigger flash if beat
        if (glw) {
            glw->lastBeat = isBeat;
            if (isBeat) {
                glw->beatCount++;
                glw->triggerFlash();
            }
            glw->update();
        }
    }

private:
    unsigned rate;
    snd_pcm_t *pcm;
    float runningAvg = 0.0f;
    bool runningAvgInitialized = false;
    quint32 lastKickMs = 0;
};

// --------------------------- Editor default shaders (cube) ------------------
static const char *DEFAULT_EDITOR =
R"(#shader vertex
#version 330 core
uniform float u_time;
uniform vec2 u_resolution;
uniform int u_beat;
uniform int u_beatCount;
uniform float u_randSeed;
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
  float slowX = u_time * 0.35;
  float slowY = u_time * 0.45;
  mat3 rx = mat3(1,0,0, 0,cos(slowX),-sin(slowX), 0,sin(slowX),cos(slowX));
  mat3 ry = mat3(cos(slowY),0,sin(slowY), 0,1,0, -sin(slowY),0,cos(slowY));
  vec3 worldPos = ry * rx * pos;
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
)";

// --------------------------- Main ---------------------------------------
int main(int argc, char **argv) {
    QApplication a(argc, argv);

    // prepare ring buffer
    g_ring_cap = static_cast<size_t>(DEFAULT_SR) * RING_SECONDS;
    g_ring.assign(g_ring_cap, 0.0f);
    g_ring_pos = 0;

    // try to find ALSA capture control (optional)
    if (!alsa_find_capture_control("Capture") && !alsa_find_capture_control("Mic")) {
        qWarning() << "Warning: Could not find ALSA capture control 'Capture' or 'Mic'. Capture volume control will be disabled.";
    }

    // UI: splitter with exact left/right halves
    QSplitter *split = new QSplitter(Qt::Horizontal);
    QPlainTextEdit *editor = new QPlainTextEdit(QString::fromUtf8(DEFAULT_EDITOR));
    GLSLHighlighter *hl = new GLSLHighlighter(editor->document());
    Q_UNUSED(hl);

    QWidget *rightContainer = new QWidget;
    QVBoxLayout *vlay = new QVBoxLayout(rightContainer);
    vlay->setContentsMargins(0,0,0,0);
    GLWaveWidget *glw = new GLWaveWidget(DEFAULT_SR);
    vlay->addWidget(glw);

    split->addWidget(editor);
    split->addWidget(rightContainer);
    split->setStretchFactor(0, 1);
    split->setStretchFactor(1, 1);

    // ALSA poller (single-threaded)
    AlsaPoller poller(DEFAULT_SR);
    bool alsaOpened = poller.openDevice();
    if (!alsaOpened) qWarning() << "ALSA device not opened; audio capture disabled.";

    // Debounced shader compile (only after GL initialized)
    QTimer *debounce = new QTimer(&a);
    debounce->setSingleShot(true);
    debounce->setInterval(250);

    auto compileNow = [&](){
        QString src = editor->toPlainText();
        QString log;
        if (!glw->setShadersFromEditor(src, log)) {
            qWarning() << "Shader compile/link error:\n" << log;
        } else {
            qDebug() << "Shaders compiled successfully.";
        }
        glw->update();
    };

    QObject::connect(editor, &QPlainTextEdit::textChanged, [&](){ debounce->start(); });
    QObject::connect(debounce, &QTimer::timeout, [&](){
        if (!glw->isValid() || !glw->context() || !glw->context()->isValid()) {
            QTimer::singleShot(150, [&](){ debounce->start(); });
            return;
        }
        compileNow();
    });

    QObject::connect(glw, &GLWaveWidget::requestToggleFullscreen, [&](){
        if (rightContainer->isFullScreen()) rightContainer->showNormal();
        else rightContainer->showFullScreen();
    });
    QObject::connect(glw, &GLWaveWidget::requestExitFullscreen, [&](){
        if (rightContainer->isFullScreen()) rightContainer->showNormal();
    });

    // Poll timer: single-threaded ALSA polling and beat detection
    QTimer pollTimer;
    pollTimer.setInterval(POLL_MS);
    QObject::connect(&pollTimer, &QTimer::timeout, [&](){
        if (alsaOpened) poller.pollOnce(glw);
    });
    pollTimer.start();

    // show window and force equal halves
    split->resize(1200, 720);
    split->show();
    QTimer::singleShot(0, [&](){
        QSize s = split->size();
        int half = s.width() / 2;
        QList<int> sizes; sizes << half << (s.width() - half);
        split->setSizes(sizes);
    });

    // initial compile after GL ready (simple single-shot)
    QTimer::singleShot(300, [&](){ debounce->start(); });

    int ret = a.exec();

    // cleanup
    pollTimer.stop();
    if (g_mixer) { snd_mixer_close(g_mixer); g_mixer = nullptr; g_selem = nullptr; }

    return ret;
}

#include "main.moc"
