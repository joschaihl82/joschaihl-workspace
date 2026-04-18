/*
 * main.cpp - qgeister1 vollständiges Amalgamat (Sv/s + CIA-like ALSA Clicks)
 *
 * Integriert:
 * - V4L2 Video Capture (YUYV -> RGB32)
 * - Bildverarbeitung: Background EMA, Long-exposure EMA, Sobel, Residue, Heatmap
 * - UI: Full-window dual video, overlay graphs, tray minigraph (Labels in Sv/s)
 * - Echtzeit ALSA Audio Engine (event-driven, low-latency, "CIA-like" clicks)
 *
 * Build on Linux: link with -lasound and Qt Widgets
 * qgeister1.pro: QT += widgets ; unix: LIBS += -lasound
 */

#include <QApplication>
#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QImage>
#include <QIODevice>
#include <QtMath>
#include <QDebug>
#include <QVector>
#include <QSocketNotifier>
#include <QTimer>
#include <QDateTime>
#include <QElapsedTimer>
#include <QResizeEvent>

#include <alsa/asoundlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <limits.h>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <vector>
#include <cstring>
#include <cstdlib>

// --- Konfiguration ---
#define DEVICE_NODE "/dev/video0"
#define IMG_W 640
#define IMG_H 480
#define GRAPH_POINTS 100

// --- Tunables ---
#define BG_LEARN_RATE 0.01f
#define RESIDUE_THRESH 15
#define FLICKER_THRESH 20
#define EDGE_GAIN 1.6f
#define HEATMAP_ALPHA 155
#define RADIONULLON_THRESH 30

#define HOT_PIXEL_THRESH 200
#define GEIGER_SMOOTH_FACTOR 0.3
#define GEIGER_NORM_FACTOR 100.0
#define GEIGER_ALERT_THRESH 5.0

#define LONG_EXP_FRAMES 60
#define LONG_EXP_LEARN (1.0f / LONG_EXP_FRAMES)

// --- Datenstrukturen ---
struct GeigerStats {
    double currentRad = 0.0; // Sv/s
    double avgRad = 0.0;     // Sv/s (smoothed)
    double totalDose = 0.0;  // Sv (accumulated)
    bool alertState = false;

    QDateTime startTime;
    QDateTime currentTime;
    QVector<double> shortHistory;
    QVector<double> avgHistory;
    QVector<double> longHistory;
};

// --- V4L2 Capture ---
struct Buffer { void *start; size_t length; };

class V4L2Capture : public QObject {
    Q_OBJECT
public:
    explicit V4L2Capture(QObject *parent = nullptr) : QObject(parent) {
        fd = open(DEVICE_NODE, O_RDWR | O_NONBLOCK, 0);
        if (fd == -1) { qWarning() << "Fehler beim Öffnen" << DEVICE_NODE; return; }

        struct v4l2_format fmt; memset(&fmt, 0, sizeof(fmt));
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width = IMG_W; fmt.fmt.pix.height = IMG_H;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        ioctl(fd, VIDIOC_S_FMT, &fmt);

        struct v4l2_requestbuffers req; memset(&req, 0, sizeof(req));
        req.count = 2; req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; req.memory = V4L2_MEMORY_MMAP;
        ioctl(fd, VIDIOC_REQBUFS, &req);

        buffers = (Buffer*)calloc(req.count, sizeof(*buffers));
        for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
            struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf));
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = n_buffers;
            ioctl(fd, VIDIOC_QUERYBUF, &buf);
            buffers[n_buffers].length = buf.length;
            buffers[n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        }

        for (unsigned int i = 0; i < n_buffers; ++i) {
            struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf));
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
            ioctl(fd, VIDIOC_QBUF, &buf);
        }
        enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE; ioctl(fd, VIDIOC_STREAMON, &type);
        notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
        connect(notifier, &QSocketNotifier::activated, this, &V4L2Capture::readFrame);
    }
    ~V4L2Capture() { if (fd != -1) close(fd); if (buffers) free(buffers); }

signals:
    void imageCaptured(const QImage &img);

private slots:
    void readFrame() {
        struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP;
        if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1) return;

        unsigned char* src = (unsigned char*)buffers[buf.index].start;
        QImage img(IMG_W, IMG_H, QImage::Format_RGB32);

        for (int y = 0; y < IMG_H; y++) {
            unsigned char* srcLine = src + (y * IMG_W * 2);
            QRgb* dstLine = (QRgb*)img.scanLine(y);
            for (int x = 0; x < IMG_W; x += 2) {
                int Y1 = srcLine[0]; int U = srcLine[1]; int Y2 = srcLine[2]; int V = srcLine[3]; srcLine += 4;
                int C = Y1 - 16; int D = U - 128; int E = V - 128;
                int R = (298 * C + 409 * E + 128) >> 8;
                int G = (298 * C - 100 * D - 208 * E + 128) >> 8;
                int B = (298 * C + 516 * D + 128) >> 8;
                dstLine[0] = qRgb(qBound(0, R, 255), qBound(0, G, 255), qBound(0, B, 255));
                C = Y2 - 16;
                R = (298 * C + 409 * E + 128) >> 8;
                G = (298 * C - 100 * D - 208 * E + 128) >> 8;
                B = (298 * C + 516 * D + 128) >> 8;
                dstLine[1] = qRgb(qBound(0, R, 255), qBound(0, G, 255), qBound(0, B, 255));
                dstLine += 2;
            }
        }
        ioctl(fd, VIDIOC_QBUF, &buf);
        emit imageCaptured(img);
    }
private:
    int fd{ -1 }; Buffer *buffers{ nullptr }; unsigned int n_buffers{0}; QSocketNotifier *notifier{ nullptr };
};

// --- GeigerLogic ---
class GeigerLogic : public QObject {
    Q_OBJECT
public:
    explicit GeigerLogic(QObject *parent = nullptr) : QObject(parent) {
        startTime = QDateTime::currentDateTime();
        bg.resize(IMG_W * IMG_H); bg.fill(0.0f);
        flicker.resize(IMG_W * IMG_H); flicker.fill(0);
        longExp.resize(IMG_W * IMG_H); longExp.fill(0.0f);
        shortHistory.resize(GRAPH_POINTS); shortHistory.fill(0);
        avgHistory.resize(GRAPH_POINTS); avgHistory.fill(0);
        longHistory.resize(GRAPH_POINTS); longHistory.fill(0);
        frameTimer.start();
    }

    QImage processRadionullon(const QImage &img) {
        if (img.isNull()) return img;
        QImage outputImg = img;
        int size = IMG_W * IMG_H;
        QVector<quint8> gray(size);
        QVector<quint8> residue(size);
        QVector<quint8> edge(size);
        int hotPixels = 0;

        for (int y = 0; y < IMG_H; ++y) {
            const QRgb* srcLine = (const QRgb*)img.constScanLine(y);
            for (int x = 0; x < IMG_W; ++x) {
                quint8 g = (quint8)qGray(srcLine[x]);
                gray[y * IMG_W + x] = g;
                if (g > HOT_PIXEL_THRESH) hotPixels++;
            }
        }

        for (int i = 0; i < size; ++i) {
            longExp[i] = (1.0f - LONG_EXP_LEARN) * longExp[i] + LONG_EXP_LEARN * (float)gray[i];
        }

        float globalAvg = 0.0f;
        for (int i = 0; i < size; ++i) {
            bg[i] = (1.0f - BG_LEARN_RATE) * bg[i] + BG_LEARN_RATE * (float)gray[i];
            float combinedBg = 0.7f * bg[i] + 0.3f * longExp[i];
            int diff = qAbs((int)gray[i] - (int)qRound(combinedBg));
            residue[i] = (diff > RESIDUE_THRESH) ? (quint8)qMin(diff, 255) : 0;
            globalAvg += (float)gray[i];
        }
        globalAvg /= size;

        quint8 currentAvg = (quint8)qBound(0.0f, globalAvg, 255.0f);
        int delta = qAbs((int)currentAvg - (int)prevGlobalAvg);
        prevGlobalAvg = currentAvg;
        quint8 boost = (delta > FLICKER_THRESH) ? 20 : 1;
        for (int i = 0; i < size; i++) {
            flicker[i] = qBound(0, (int)flicker[i] + boost, 255);
            if (flicker[i] > 0) flicker[i] = qMax(0, (int)flicker[i] - 1);
        }

        for (int y = 1; y < IMG_H-1; y++) {
            for (int x = 1; x < IMG_W-1; x++) {
                int i = y*IMG_W + x;
                int gx = -gray[i - IMG_W - 1] - 2*gray[i - 1] - gray[i + IMG_W - 1] +
                         gray[i - IMG_W + 1] + 2*gray[i + 1] + gray[i + IMG_W + 1];
                int gy = -gray[i - IMG_W - 1] - 2*gray[i - IMG_W] - gray[i - IMG_W + 1] +
                         gray[i + IMG_W - 1] + 2*gray[i + IMG_W] + gray[i + IMG_W + 1];
                int mag = (int)(EDGE_GAIN * qSqrt(gx*gx + gy*gy));
                edge[i] = (quint8)qBound(0, mag, 255);
            }
        }

        for (int y = 0; y < IMG_H; ++y) {
            QRgb* dstLine = (QRgb*)outputImg.scanLine(y);
            for (int x = 0; x < IMG_W; ++x) {
                int i = y*IMG_W + x;
                int score = (int)residue[i] + (int)(edge[i]/2) + (int)(flicker[i]/3);

                if (score < RADIONULLON_THRESH) {
                    int le = qBound(0, (int)qRound(longExp[i]), 255);
                    int mixed = (int)((0.7f * gray[i]) + (0.3f * le));
                    dstLine[x] = qRgb(mixed, mixed, mixed);
                    continue;
                }

                int s = qMin(score, 255);
                quint8 r,g,b;
                if (s < 64) { b = 255; g = s*4; r = 0; }
                else if (s < 128) { b = 255 - (s-64)*4; g = 255; r = 0; }
                else if (s < 192) { b = 0; g = 255; r = (s-128)*4; }
                else { b = 0; g = 255 - (s-192)*4; r = 255; }

                quint8 R0 = qRed(dstLine[x]), G0 = qGreen(dstLine[x]), B0 = qBlue(dstLine[x]);
                int alpha = HEATMAP_ALPHA;
                quint8 R_blend = (R0*(255-alpha) + r*alpha)/255;
                quint8 G_blend = (G0*(255-alpha) + g*alpha)/255;
                quint8 B_blend = (B0*(255-alpha) + b*alpha)/255;
                dstLine[x] = qRgb(R_blend, G_blend, B_blend);
            }
        }

        // --- Geiger values: convert to Sv/s and smooth/accumulate ---
        // rawVal_per_h approximates Sv/h-equivalent from hot pixel count
        double rawVal_per_h = (double)hotPixels / GEIGER_NORM_FACTOR;
        double rawVal = rawVal_per_h / 3600.0; // Sv/s

        // smooth in Sv/s
        currentRadSmooth = (currentRadSmooth * (1.0 - GEIGER_SMOOTH_FACTOR)) + (rawVal * GEIGER_SMOOTH_FACTOR);

        // accumulate total dose in Sv. Use measured frame interval if available
        double frameSec = 0.1; // default fallback
        qint64 nowMs = frameTimer.elapsed();
        if (lastFrameMs > 0) {
            qint64 diff = nowMs - lastFrameMs;
            if (diff > 0) frameSec = double(diff) / 1000.0;
        }
        lastFrameMs = nowMs;

        totalDoseAcc += currentRadSmooth * frameSec; // Sv

        // history buffers (Sv/s)
        rotateBuffer(shortHistory, rawVal);
        rotateBuffer(avgHistory, currentRadSmooth);
        longTermCounter++;
        if (longTermCounter >= 10) {
            rotateBuffer(longHistory, currentRadSmooth);
            longTermCounter = 0;
        }

        GeigerStats stats;
        stats.currentRad = rawVal;
        stats.avgRad = currentRadSmooth;
        stats.totalDose = totalDoseAcc;
        stats.alertState = currentRadSmooth > (GEIGER_ALERT_THRESH / 3600.0); // convert threshold to Sv/s
        stats.startTime = startTime;
        stats.currentTime = QDateTime::currentDateTime();
        stats.shortHistory = shortHistory;
        stats.avgHistory = avgHistory;
        stats.longHistory = longHistory;

        emit statsUpdated(stats);

        // send activity (Sv/s) to audio engine for CIA-like clicks if any activity detected
        if (rawVal > 0.0) emit requestClick(rawVal);

        return outputImg;
    }

signals:
    void statsUpdated(GeigerStats stats);
    void requestClick(double activity); // activity in Sv/s
    void processedImage(const QImage &img);

public slots:
    void processFrame(const QImage &img) {
        QImage processed = processRadionullon(img);
        emit processedImage(processed);
    }

private:
    void rotateBuffer(QVector<double> &buf, double val) {
        for(int i = 0; i < buf.size()-1; ++i) buf[i] = buf[i+1];
        buf[buf.size()-1] = val;
    }

    double currentRadSmooth = 0.0; // Sv/s
    double totalDoseAcc = 0.0;     // Sv
    QDateTime startTime;
    int longTermCounter = 0;

    QVector<float> bg;
    QVector<quint8> flicker;
    float prevGlobalAvg = 128.0f;
    QVector<float> longExp;

    QVector<double> shortHistory;
    QVector<double> avgHistory;
    QVector<double> longHistory;

    QElapsedTimer frameTimer;
    qint64 lastFrameMs { -1 };
};

// --- Echtzeit ALSA Audio Engine ---
class AudioEngine : public QObject {
    Q_OBJECT
public:
    explicit AudioEngine(QObject *parent = nullptr) : QObject(parent) {
        pcm = nullptr;
        running.store(false);
        if (!initALSA()) {
            qWarning() << "ALSA init failed";
            return;
        }
        running.store(true);
        audioThread = std::thread(&AudioEngine::audioLoop, this);
    }

    ~AudioEngine() {
        running.store(false);
        cv.notify_all();
        if (audioThread.joinable()) audioThread.join();
        if (pcm) {
            snd_pcm_drain(pcm);
            snd_pcm_close(pcm);
            pcm = nullptr;
        }
    }

public slots:
    // Enqueue a click event; activity passed in Sv/s
    void playClick(double activity) {
        ClickEvent ev;
        ev.activity = activity;
        ev.timeMs = QDateTime::currentMSecsSinceEpoch();
        {
            std::lock_guard<std::mutex> g(queueMtx);
            if (clickQueue.size() < 8192) clickQueue.push_back(ev);
        }
        cv.notify_one();
    }

private:
    struct ClickEvent { double activity; qint64 timeMs; };

    bool initALSA() {
        const char* device = "default";
        int rc = snd_pcm_open(&pcm, device, SND_PCM_STREAM_PLAYBACK, 0);
        if (rc < 0) { qWarning() << "snd_pcm_open:" << snd_strerror(rc); return false; }

        snd_pcm_hw_params_t *hw;
        snd_pcm_hw_params_malloc(&hw);
        snd_pcm_hw_params_any(pcm, hw);
        snd_pcm_hw_params_set_access(pcm, hw, SND_PCM_ACCESS_RW_INTERLEAVED);
        snd_pcm_hw_params_set_format(pcm, hw, SND_PCM_FORMAT_S16_LE);
        unsigned int rate = sampleRate;
        snd_pcm_hw_params_set_rate_near(pcm, hw, &rate, 0);
        snd_pcm_hw_params_set_channels(pcm, hw, 1);
        snd_pcm_uframes_t frames = (snd_pcm_uframes_t)periodFrames;
        snd_pcm_hw_params_set_period_size_near(pcm, hw, &frames, 0);
        int err = snd_pcm_hw_params(pcm, hw);
        snd_pcm_hw_params_free(hw);
        if (err < 0) { qWarning() << "snd_pcm_hw_params:" << snd_strerror(err); snd_pcm_close(pcm); pcm = nullptr; return false; }
        snd_pcm_prepare(pcm);
        return true;
    }

    void audioLoop() {
        double lpState = 0.0;
        double prevOut = 0.0;
        const double rcLp = 1.0 / (2.0 * M_PI * 9000.0);
        const double dt = 1.0 / sampleRate;
        const double alphaLp = dt / (rcLp + dt);
        const double hpCoef = 0.995;

        struct ActiveClick { int pos; int len; double ampVar; double pitchVar; int id; };
        std::deque<ActiveClick> activeClicks;
        int nextClickId = 1;
        std::vector<int16_t> outBuf(periodFrames);

        while (running.load()) {
            // Move queued ClickEvents into activeClicks (1 click per event)
            {
                std::unique_lock<std::mutex> lk(queueMtx);
                if (clickQueue.empty()) {
                    cv.wait_for(lk, std::chrono::milliseconds(5));
                }
                while (!clickQueue.empty()) {
                    ClickEvent ev = clickQueue.front();
                    clickQueue.pop_front();

                    ActiveClick ac;
                    ac.pos = 0;
                    ac.len = std::max(1, (int)(clickLenMs * 0.001 * sampleRate));

                    // Map activity (Sv/s) to activityPerHour for compatibility with previous mapping
                    double activityPerHour = ev.activity * 3600.0;
                    double clicksPerSecond_est = qMin(200.0, 0.9 * activityPerHour + 0.5);
                    if (clicksPerSecond_est < 1.0) clicksPerSecond_est = 1.0;

                    uint32_t seed = (uint32_t)((ev.timeMs & 0xffffffff) ^ (nextClickId * 1664525u));
                    ac.ampVar = 0.85 + double(seed & 0xFF) / 255.0 * 0.4;
                    // bias pitch slightly by activity so high activity can subtly change timbre
                    ac.pitchVar = 1.0 + (((double)((seed >> 8) & 0xFF) / 255.0) - 0.5) * 0.03 + (qMin(clicksPerSecond_est, 80.0) / 8000.0);
                    ac.id = nextClickId++;
                    activeClicks.push_back(ac);

                    // Optionally: if clicksPerSecond_est >> 1, generate extra clicks immediately to reflect rate
                    int extra = (int)qFloor(qMin(clicksPerSecond_est, 200.0) * 0.002); // small multiplier to add density
                    for (int e = 0; e < extra; ++e) {
                        ActiveClick ac2 = ac;
                        ac2.id = nextClickId++;
                        // stagger start slightly by a few samples
                        ac2.pos = qMin(ac2.len-1, e * (ac2.len/4));
                        activeClicks.push_back(ac2);
                    }
                }
            }

            // Synthesize a small block
            for (int n = 0; n < periodFrames; ++n) {
                double s = 0.0;

                for (auto it = activeClicks.begin(); it != activeClicks.end(); ) {
                    ActiveClick &ac = *it;
                    if (ac.pos >= ac.len) {
                        it = activeClicks.erase(it);
                        continue;
                    }
                    double tt = (double)ac.pos / sampleRate;
                    double env = exp(-tt / envTau);
                    env *= (1.0 - exp(-40.0 * tt)); // fast attack

                    double c1 = sin(2.0 * M_PI * carrierA * ac.pitchVar * tt);
                    double c2 = 0.55 * sin(2.0 * M_PI * carrierB * (1.0 + (ac.id & 7) * 0.0018) * tt);

                    double pseudoNoise = (((ac.id * 1103515245u + 12345u) >> 16) & 0x7FFF) / (double)0x7FFF * 2.0 - 1.0;
                    s += baseAmp * ac.ampVar * env * (0.88 * c1 + 0.48 * c2 + noiseLevel * pseudoNoise);

                    ac.pos++;
                    ++it;
                }

                double bg = (((double)rand() / RAND_MAX) * 2.0 - 1.0) * 0.0009;
                s += bg;

                lpState = lpState + alphaLp * (s - lpState);
                double banded = lpState - hpCoef * prevOut;
                prevOut = lpState;
                double sample = banded;

                if (sample > 1.0) sample = 1.0;
                if (sample < -1.0) sample = -1.0;

                outBuf[n] = (int16_t)(sample * 30000.0);
            }

            // Write to ALSA (handle XRUN)
            int frames = periodFrames;
            int written = 0;
            while (written < frames) {
                int rc = snd_pcm_writei(pcm, outBuf.data() + written, frames - written);
                if (rc == -EPIPE) {
                    snd_pcm_prepare(pcm);
                    continue;
                } else if (rc < 0) {
                    qWarning() << "ALSA write error:" << snd_strerror(rc);
                    break;
                } else {
                    written += rc;
                }
            }
        }
    }

    const int sampleRate = 44100;
    const int periodFrames = 256;
    const double carrierA = 1400.0;
    const double carrierB = 3200.0;
    const double clickLenMs = 3.8;
    const double envTau = 0.0022;
    const double noiseLevel = 0.16;
    const double baseAmp = 0.95;

    snd_pcm_t *pcm = nullptr;
    std::thread audioThread;
    std::atomic<bool> running;
    std::mutex queueMtx;
    std::condition_variable cv;
    std::deque<ClickEvent> clickQueue;
};

// --- UI Widget ---
class GeigerWidget : public QWidget {
    Q_OBJECT
public:
    GeigerWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("qrad Radionullon Viewer | Original vs. Heatmap");
        setStyleSheet("background-color: #000; color: #fff;");
    }

public slots:
    void updateStats(GeigerStats stats) { m_stats = stats; update(); }
    void updateOriginalImage(const QImage &img) { m_lastFrame_original = img; update(); }
    void updateProcessedImage(const QImage &img) { m_lastFrame_processed = img; update(); }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.fillRect(rect(), Qt::black);

        int gap = 5;
        int panelW = (width() - 3*gap) / 2;
        int panelH = height() - gap;

        QRect leftImgRect(gap, gap, panelW, panelH);
        if (!m_lastFrame_original.isNull()) p.drawImage(leftImgRect, m_lastFrame_original);
        else { p.setPen(Qt::white); p.drawText(leftImgRect, Qt::AlignCenter, "Original (Warte auf Stream)"); }
        p.setPen(Qt::white);
        p.setFont(QFont("Sans", 10));
        p.drawText(QRect(gap, 5, panelW, 20), Qt::AlignLeft | Qt::AlignTop, "UNVERÄNDERTES ORIGINAL");

        QRect rightImgRect(2*gap + panelW, gap, panelW, panelH);
        if (!m_lastFrame_processed.isNull()) p.drawImage(rightImgRect, m_lastFrame_processed);
        else { p.setPen(Qt::white); p.drawText(rightImgRect, Qt::AlignCenter, "Radionullon (Warte auf Stream)"); }
        p.setPen(QColor(255, 150, 0));
        p.drawText(QRect(2*gap + panelW, 5, panelW, 20), Qt::AlignLeft | Qt::AlignTop, "RADIONULLON HEATMAP (ALGORITHMUS)");

        int textY = 28; int textX = 20;
        p.setFont(QFont("Monospace", 18, QFont::Bold));
        p.setPen(m_stats.alertState ? Qt::red : Qt::green);
        // Sv/s labels (scientific notation because values can be tiny)
        p.drawText(textX, textY, QString("AKTUELL: %1 Sv/s").arg(m_stats.currentRad, 0, 'e', 2));
        p.setFont(QFont("Monospace", 12));
        p.setPen(Qt::white);
        p.drawText(textX, textY + 22, QString("Ø MITTEL: %1 Sv/s").arg(m_stats.avgRad, 0, 'e', 2));
        p.drawText(textX, textY + 42, QString("GESAMT DOSE: %1 Sv").arg(m_stats.totalDose, 0, 'e', 4));

        int graphH = qMax(60, height() / 8);
        int margin = 10;
        int totalW = width() - (4 * margin);
        int singleGraphW = (totalW - 20) / 3;
        int yPos = height() - graphH - margin;

        QRect r1(margin, yPos, singleGraphW, graphH);
        QString tStart = m_stats.startTime.toString("HH:mm:ss");
        QString tEnd = m_stats.currentTime.toString("HH:mm:ss");
        drawGraph(p, r1, m_stats.longHistory, QColor(0, 100, 255, 140), "Langzeit", tStart, tEnd);

        QRect r2(margin*2 + singleGraphW, yPos, singleGraphW, graphH);
        QString avgLabel = QString("Ø %1").arg(m_stats.avgRad, 0, 'e', 2);
        drawGraph(p, r2, m_stats.avgHistory, QColor(255, 200, 0, 140), "Mittelwert", avgLabel, "");

        QRect r3(margin*3 + singleGraphW*2, yPos, singleGraphW, graphH);
        QColor peakCol = m_stats.alertState ? QColor(255, 0, 0, 170) : QColor(0, 255, 0, 140);
        drawGraph(p, r3, m_stats.shortHistory, peakCol, "Kurzzeit (Peaks)", "Live", "");
    }

    void resizeEvent(QResizeEvent *event) override { Q_UNUSED(event); update(); }

private:
    void drawGraph(QPainter &p, QRect r, const QVector<double> &data, QColor col, QString title, QString labelL, QString labelR) {
        QColor bgCol(0, 0, 0, 80);
        p.fillRect(r, bgCol);
        p.setPen(QColor(120,120,120,160));
        p.drawRect(r);
        p.setPen(Qt::white);
        p.setFont(QFont("Sans", 9, QFont::Bold));
        p.drawText(r.adjusted(5,5,-5,0), Qt::AlignTop | Qt::AlignLeft, title);
        p.setFont(QFont("Sans",7));
        if (!labelL.isEmpty()) p.drawText(r.adjusted(5,0,0,-2), Qt::AlignBottom | Qt::AlignLeft, labelL);
        if (!labelR.isEmpty()) p.drawText(r.adjusted(0,0,-5,-2), Qt::AlignBottom | Qt::AlignRight, labelR);
        if (data.isEmpty()) return;
        double maxVal = 0.001;
        for (double v : data) if (v > maxVal) maxVal = v;
        maxVal = qMax(maxVal, 3.0);
        QPainterPath path;
        double stepX = (double)r.width() / (data.size() - 1);
        path.moveTo(r.left(), r.bottom());
        for (int i = 0; i < data.size(); ++i) {
            double valNorm = qMin(data[i] / maxVal, 1.0);
            double x = r.left() + (i * stepX);
            double y = r.bottom() - (valNorm * r.height() * 0.8);
            path.lineTo(x, y);
        }
        path.lineTo(r.right(), r.bottom());
        path.closeSubpath();
        p.fillPath(path, col);
        p.setPen(QColor(255,255,255,150));
        p.drawPath(path);
    }

    GeigerStats m_stats;
    QImage m_lastFrame_original;
    QImage m_lastFrame_processed;
};

// --- Tray Manager ---
class TrayGraphManager : public QObject {
    Q_OBJECT
public:
    TrayGraphManager(GeigerLogic *logic, QWidget *win, QObject *p = nullptr) : QObject(p), m_logic(logic), m_win(win) {
        tray = new QSystemTrayIcon(this);
        QMenu *m = new QMenu();
        m->addAction("Toggle Window", [this](){ m_win->isVisible() ? m_win->hide() : m_win->show(); });
        m->addAction("Exit", qApp, &QCoreApplication::quit);
        tray->setContextMenu(m);
        tray->show();
        connect(tray, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason r){
            if (r == QSystemTrayIcon::DoubleClick) m_win->isVisible() ? m_win->hide() : m_win->show();
        });
    }

public slots:
    void updateIcon(GeigerStats stats) {
        QPixmap pix(24,24); pix.fill(Qt::transparent);
        QPainter p(&pix);
        p.setRenderHint(QPainter::Antialiasing, false);
        double maxScale = 5.0;
        drawMiniGraph(p, stats.longHistory, QColor(0,0,255,150), true, maxScale);
        QPen penAvg(QColor(255,200,0)); penAvg.setWidth(2); p.setPen(penAvg);
        drawMiniLine(p, stats.avgHistory, maxScale);
        QPen penShort(stats.alertState ? Qt::red : Qt::green); penShort.setWidth(1); p.setPen(penShort);
        drawMiniLine(p, stats.shortHistory, maxScale);
        p.setPen(QColor(128,128,128,100)); p.drawRect(0,0,23,23);
        tray->setIcon(QIcon(pix));
        tray->setToolTip(QString("Rad: %1 Sv/s | Avg: %2 Sv/s").arg(stats.currentRad, 0, 'e', 2).arg(stats.avgRad, 0, 'e', 2));
    }

private:
    void drawMiniGraph(QPainter &p, const QVector<double> &data, QColor col, bool fill, double maxScale) {
        if (data.isEmpty()) return;
        QPainterPath path; path.moveTo(0,24);
        for (int i=0;i<data.size();++i) {
            double x = (double)i / data.size() * 24.0;
            double y = 24.0 - (qMin(data[i] / maxScale, 1.0) * 24.0);
            path.lineTo(x,y);
        }
        path.lineTo(24,24);
        if (fill) p.fillPath(path, col);
    }
    void drawMiniLine(QPainter &p, const QVector<double> &data, double maxScale) {
        if (data.isEmpty()) return;
        for (int i=1;i<data.size();++i) {
            double x1 = (double)(i-1) / data.size() * 24.0;
            double x2 = (double)i / data.size() * 24.0;
            double y1 = 24.0 - (qMin(data[i-1] / maxScale, 1.0) * 24.0);
            double y2 = 24.0 - (qMin(data[i] / maxScale, 1.0) * 24.0);
            p.drawLine(QPointF(x1,y1), QPointF(x2,y2));
        }
    }

    QSystemTrayIcon *tray; GeigerLogic *m_logic; QWidget *m_win;
};

// --- main ---
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    V4L2Capture capture;
    GeigerLogic logic;
    AudioEngine audio;
    GeigerWidget w;
    TrayGraphManager tray(&logic, &w);

    QObject::connect(&capture, &V4L2Capture::imageCaptured, &logic, &GeigerLogic::processFrame);
    QObject::connect(&capture, &V4L2Capture::imageCaptured, &w, &GeigerWidget::updateOriginalImage);
    QObject::connect(&logic, &GeigerLogic::processedImage, &w, &GeigerWidget::updateProcessedImage);
    QObject::connect(&logic, &GeigerLogic::statsUpdated, &w, &GeigerWidget::updateStats);
    QObject::connect(&logic, &GeigerLogic::statsUpdated, &tray, &TrayGraphManager::updateIcon);
    QObject::connect(&logic, &GeigerLogic::requestClick, &audio, &AudioEngine::playClick);

    w.showMaximized();
    return app.exec();
}

#include "main.moc"

