// main.cpp
//
// qgeister1 - Radionullon Ghost Filter Edition (Dual-Display)
// Gleiche Funktionalität wie vorher, aber ohne Präprozessor-Makros zur Konfiguration
// und ohne Verwendung von Qt-Metaobjekt-Makros (kein Q_OBJECT, keine Qt-Signale/Slots).
// Kommunikation zwischen Komponenten erfolgt über std::function-Callbacks.
// Hinweis: Dieser Entwurf ersetzt Qt-Signale/Slots bewusst durch Callbacks.
// Für Produktivcode sollte man sorgfältig auf Thread-Safety und Lebenszeit achten.

#include <QApplication>
#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QImage>
#include <QAudioOutput>
#include <QIODevice>
#include <QtMath>
#include <QDebug>
#include <QVector>
#include <QSocketNotifier>
#include <QDateTime>
#include <QTimer>
#include <cstdio>
#include <functional>
#include <memory>

// Linux V4L2 Includes
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

// -----------------------------
// Konfiguration (keine Makros)
// -----------------------------
static const char DEVICE_NODE[] = "/dev/video0";
static const int IMG_W = 640;
static const int IMG_H = 480;
static const int GRAPH_POINTS = 200;

// Radionullon-Tunables
static const float BG_LEARN_RATE = 0.005f;
static const int RESIDUE_THRESH = 12;
static const int FLICKER_THRESH = 15;
static const float EDGE_GAIN = 1.2f;
static const int HEATMAP_ALPHA = 180;
static const int RADIONULLON_THRESH = 25;

// Geiger-Tunables
static const int HOT_PIXEL_THRESH = 220;
static const double GEIGER_SMOOTH_FACTOR = 0.25;
static const double GEIGER_NORM_FACTOR = 120.0;
static const double GEIGER_ALERT_THRESH = 3.0;

// Raw Recording
static const bool RAW_RECORD_ENABLED = true;
static const char RAW_RECORD_FILENAME[] = "video.raw";

// -----------------------------
// Datenstrukturen
// -----------------------------
struct GeigerStats {
    double currentRad = 0.0;
    double avgRad = 0.0;
    double totalDose = 0.0;
    bool alertState = false;

    QDateTime startTime;
    QDateTime currentTime;
    QVector<double> shortHistory;
    QVector<double> avgHistory;
    QVector<double> longHistory;
};

// -----------------------------
// V4L2 Capture (ohne QObject/Q_OBJECT)
// -----------------------------
struct Buffer { void *start; size_t length; };

class V4L2Capture {
public:
    // Callback: void(const QImage&)
    std::function<void(const QImage&)> onImageCaptured;

    V4L2Capture() : fd(-1), buffers(nullptr), n_buffers(0), notifier(nullptr), rawFile(nullptr) {
        fd = ::open(DEVICE_NODE, O_RDWR | O_NONBLOCK, 0);
        if (fd == -1) {
            qWarning() << "Fehler beim Öffnen von" << DEVICE_NODE;
            return;
        }

        struct v4l2_format fmt; memset(&fmt, 0, sizeof(fmt));
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width = IMG_W; fmt.fmt.pix.height = IMG_H;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) qWarning() << "VIDIOC_S_FMT failed";

        struct v4l2_requestbuffers req; memset(&req, 0, sizeof(req));
        req.count = 2; req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; req.memory = V4L2_MEMORY_MMAP;
        if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) qWarning() << "VIDIOC_REQBUFS failed";

        buffers = (Buffer*)calloc(req.count, sizeof(*buffers));
        for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
            struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf));
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = n_buffers;
            if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) qWarning() << "VIDIOC_QUERYBUF failed";
            buffers[n_buffers].length = buf.length;
            buffers[n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        }

        for (unsigned int i = 0; i < n_buffers; ++i) {
            struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf));
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
            if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) qWarning() << "VIDIOC_QBUF failed";
        }

        enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) qWarning() << "VIDIOC_STREAMON failed";

        // QSocketNotifier benötigt ein QObject. Wir erstellen hier einen QTimer poller als Vereinfachung.
        pollTimer = new QTimer();
        QObject::connect(pollTimer, &QTimer::timeout, [this]() { this->tryReadFrame(); });
        pollTimer->start(10); // poll 100 Hz

        if (RAW_RECORD_ENABLED) {
            rawFile = std::fopen(RAW_RECORD_FILENAME, "ab");
            if (!rawFile) qWarning() << "Konnte Raw-File nicht öffnen:" << RAW_RECORD_FILENAME;
            else qDebug() << "Raw-Recording enabled ->" << RAW_RECORD_FILENAME;
        }
    }

    ~V4L2Capture() {
        if (pollTimer) { pollTimer->stop(); delete pollTimer; pollTimer = nullptr; }
        if (rawFile) { std::fflush(rawFile); std::fclose(rawFile); rawFile = nullptr; }
        if (fd != -1) {
            if (buffers) {
                for (unsigned int i = 0; i < n_buffers; ++i) {
                    if (buffers[i].start) munmap(buffers[i].start, buffers[i].length);
                }
                free(buffers);
                buffers = nullptr;
            }
            ::close(fd);
            fd = -1;
        }
    }

    // Polling reading
    void tryReadFrame() {
        if (fd == -1) return;

        struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1) {
            return;
        }

        unsigned char* src = (unsigned char*)buffers[buf.index].start;

        // Raw recorder
        if (RAW_RECORD_ENABLED && rawFile && buffers[buf.index].length > 0) {
            size_t written = std::fwrite(buffers[buf.index].start, 1, buffers[buf.index].length, rawFile);
            if (written != buffers[buf.index].length) qWarning() << "Raw write incomplete";
        }

        // Convert YUYV -> RGB32 into QImage
        QImage img(IMG_W, IMG_H, QImage::Format_RGB32);
        for (int y = 0; y < IMG_H; ++y) {
            unsigned char* srcLine = src + (y * IMG_W * 2);
            QRgb* dstLine = reinterpret_cast<QRgb*>(img.scanLine(y));
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

        // Requeue buffer
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) qWarning() << "VIDIOC_QBUF failed (requeue)";

        // Fire callback if set
        if (onImageCaptured) onImageCaptured(img);
    }

private:
    int fd;
    Buffer *buffers;
    unsigned int n_buffers;
    QTimer *pollTimer;
    FILE *rawFile;
};

// -----------------------------
// GeigerLogic (keine Q_OBJECT, callbacks)
// -----------------------------
class GeigerLogic {
public:
    // Callbacks:
    std::function<void(const GeigerStats&)> onStatsUpdated;
    std::function<void(const QImage&)> onProcessedImage;
    std::function<void(double)> onRequestClick;

    GeigerLogic() {
        startTime = QDateTime::currentDateTime();
        bg.resize(IMG_W * IMG_H); bg.fill(0.0f);
        flicker.resize(IMG_W * IMG_H); flicker.fill(0);
        shortHistory.resize(GRAPH_POINTS); shortHistory.fill(0);
        avgHistory.resize(GRAPH_POINTS); avgHistory.fill(0);
        longHistory.resize(GRAPH_POINTS); longHistory.fill(0);
    }

    void processFrame(const QImage &img) {
        if (img.isNull()) return;
        QImage processed = processRadionullon(img);
        if (onProcessedImage) onProcessedImage(processed);
    }

private:
    QImage processRadionullon(const QImage &img) {
        QImage outputImg = img;
        const int size = IMG_W * IMG_H;
        QVector<quint8> gray(size);
        QVector<quint8> residue(size);
        QVector<quint8> edge(size);
        int hotPixels = 0;

        // 1) grayscale + hot pixel counting
        for (int y = 0; y < IMG_H; ++y) {
            const QRgb* srcLine = reinterpret_cast<const QRgb*>(img.constScanLine(y));
            for (int x = 0; x < IMG_W; ++x) {
                quint8 g = static_cast<quint8>(qGray(srcLine[x]));
                gray[y * IMG_W + x] = g;
                if (g > HOT_PIXEL_THRESH) ++hotPixels;
            }
        }

        // 2) background EMA + residue
        float globalAvg = 0.0f;
        for (int i = 0; i < size; ++i) {
            bg[i] = (1.0f - BG_LEARN_RATE) * bg[i] + BG_LEARN_RATE * static_cast<float>(gray[i]);
            int diff = qAbs(static_cast<int>(gray[i]) - static_cast<int>(bg[i]));
            residue[i] = (diff > RESIDUE_THRESH) ? static_cast<quint8>(qMin(diff, 255)) : 0;
            globalAvg += static_cast<float>(gray[i]);
        }
        globalAvg /= size;

        // 3) flicker
        quint8 currentAvg = static_cast<quint8>(qBound(0.0f, globalAvg, 255.0f));
        int delta = qAbs(static_cast<int>(currentAvg) - static_cast<int>(prevGlobalAvg));
        prevGlobalAvg = currentAvg;
        quint8 boost = (delta > FLICKER_THRESH) ? 20 : 1;
        for (int i = 0; i < size; ++i) {
            flicker[i] = static_cast<quint8>(qBound(0, static_cast<int>(flicker[i]) + boost, 255));
            if (flicker[i] > 0) flicker[i] = static_cast<quint8>(qMax(0, static_cast<int>(flicker[i]) - 1));
        }

        // 4) Sobel edges
        for (int y = 1; y < IMG_H - 1; ++y) {
            for (int x = 1; x < IMG_W - 1; ++x) {
                int i = y * IMG_W + x;
                int gx = -gray[i - IMG_W - 1] - 2*gray[i - 1] - gray[i + IMG_W - 1]
                         + gray[i - IMG_W + 1] + 2*gray[i + 1] + gray[i + IMG_W + 1];
                int gy = -gray[i - IMG_W - 1] - 2*gray[i - IMG_W] - gray[i - IMG_W + 1]
                         + gray[i + IMG_W - 1] + 2*gray[i + IMG_W] + gray[i + IMG_W + 1];
                int mag = static_cast<int>(EDGE_GAIN * qSqrt(gx*gx + gy*gy));
                edge[i] = static_cast<quint8>(qBound(0, mag, 255));
            }
        }

        // 5) heatmap overlay
        double totalScore = 0.0;
        for (int y = 0; y < IMG_H; ++y) {
            QRgb* dstLine = reinterpret_cast<QRgb*>(outputImg.scanLine(y));
            for (int x = 0; x < IMG_W; ++x) {
                int i = y * IMG_W + x;
                int score = static_cast<int>(residue[i]) + static_cast<int>(edge[i] / 2) + static_cast<int>(flicker[i] / 3);
                totalScore += score;
                if (score < RADIONULLON_THRESH) {
                    dstLine[x] = qRgb(gray[i], gray[i], gray[i]);
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
                quint8 R_blend = static_cast<quint8>((R0*(255-alpha) + r*alpha)/255);
                quint8 G_blend = static_cast<quint8>((G0*(255-alpha) + g*alpha)/255);
                quint8 B_blend = static_cast<quint8>((B0*(255-alpha) + b*alpha)/255);
                dstLine[x] = qRgb(R_blend, G_blend, B_blend);
            }
        }

        // 6) Geiger values
        double rawVal = static_cast<double>(hotPixels) / GEIGER_NORM_FACTOR;
        currentRadSmooth = (currentRadSmooth * (1.0 - GEIGER_SMOOTH_FACTOR)) + (rawVal * GEIGER_SMOOTH_FACTOR);
        totalDoseAcc += currentRadSmooth * 0.001;

        rotateBuffer(shortHistory, rawVal);
        rotateBuffer(avgHistory, currentRadSmooth);
        ++longTermCounter;
        if (longTermCounter >= 10) {
            rotateBuffer(longHistory, currentRadSmooth);
            longTermCounter = 0;
        }

        GeigerStats stats;
        stats.currentRad = rawVal;
        stats.avgRad = currentRadSmooth;
        stats.totalDose = totalDoseAcc;
        stats.alertState = currentRadSmooth > GEIGER_ALERT_THRESH;
        stats.startTime = startTime;
        stats.currentTime = QDateTime::currentDateTime();
        stats.shortHistory = shortHistory;
        stats.avgHistory = avgHistory;
        stats.longHistory = longHistory;

        if (onStatsUpdated) onStatsUpdated(stats);
        if (rawVal > 2.0 && onRequestClick) onRequestClick(440.0 + (rawVal * 100.0));

        return outputImg;
    }

    void rotateBuffer(QVector<double> &buf, double val) {
        if (buf.isEmpty()) return;
        for (int i = 0; i < buf.size() - 1; ++i) buf[i] = buf[i+1];
        buf[buf.size()-1] = val;
    }

public:
    std::function<void(const GeigerStats&)> onStatsUpdated; // set by caller

private:
    double currentRadSmooth = 0.0;
    double totalDoseAcc = 0.0;
    QDateTime startTime;
    int longTermCounter = 0;

    QVector<float> bg;
    QVector<quint8> flicker;
    float prevGlobalAvg = 128.0f;

    QVector<double> shortHistory;
    QVector<double> avgHistory;
    QVector<double> longHistory;

    // local callback holders used inside process
    std::function<void(const GeigerStats&)> onStatsUpdated_local = nullptr;
    std::function<void(double)> onRequestClick = nullptr;

    // Expose callbacks expected by outer code
public:
    void setOnStatsUpdated(const std::function<void(const GeigerStats&)>& cb) { onStatsUpdated = cb; }
    void setOnProcessedImageCallback(const std::function<void(const QImage&)>& cb) { onProcessedImage = cb; }
    void setOnRequestClick(const std::function<void(double)>& cb) { onRequestClick = cb; }
    std::function<void(const QImage&)> onProcessedImage;
};

// -----------------------------
// Audio engine (keine QObject-Signale)
// -----------------------------
class AudioEngine {
public:
    AudioEngine() {
        QAudioFormat format;
        format.setSampleRate(44100); format.setChannelCount(1); format.setSampleSize(16);
        format.setCodec("audio/pcm"); format.setByteOrder(QAudioFormat::LittleEndian); format.setSampleType(QAudioFormat::SignedInt);
        QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
        if (!info.isFormatSupported(format)) format = info.nearestFormat(format);
        audioOutput = new QAudioOutput(format);
        audioDevice = audioOutput->start();
    }

    ~AudioEngine() {
        if (audioOutput) {
            audioOutput->stop();
            delete audioOutput;
            audioOutput = nullptr;
        }
    }

    void playClick(double freq) {
        if (!audioDevice) return;
        const int sampleCount = 1500;
        QByteArray data; data.resize(sampleCount * 2);
        qint16 *ptr = reinterpret_cast<qint16*>(data.data());
        for (int i = 0; i < sampleCount; ++i) {
            double t = static_cast<double>(i) / 44100.0;
            ptr[i] = static_cast<qint16>(qSin(2 * M_PI * freq * t) * qExp(-t * 30) * 10000);
        }
        audioDevice->write(data);
    }

private:
    QAudioOutput *audioOutput = nullptr;
    QIODevice *audioDevice = nullptr;
};

// -----------------------------
// UI Widget (wie vorher, aber ohne QObject macro)
// -----------------------------
class GeigerWidget : public QWidget {
public:
    GeigerWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("qrad Radionullon Viewer | Original vs. Heatmap");
        setMinimumSize(640, 480);
        setStyleSheet("background-color: #000; color: #fff;");
    }

    // Callbacks to update state (replaces Qt slot connections)
    void updateStats(const GeigerStats &stats) { m_stats = stats; update(); }
    void updateOriginalImage(const QImage &img) { m_lastFrame_original = img; update(); }
    void updateProcessedImage(const QImage &img) { m_lastFrame_processed = img; update(); }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.fillRect(rect(), Qt::black);

        const int gap = 8;
        const int bottomArea = 120;
        int contentW = width() - gap * 3;
        int contentH = height() - gap * 3 - bottomArea;

        int maxFrameW = contentW / 2;
        int maxFrameH = contentH;

        double frameAspect = static_cast<double>(IMG_W) / static_cast<double>(IMG_H);
        QSize frameSize;
        if (static_cast<double>(maxFrameW) / frameAspect <= maxFrameH) {
            frameSize.setWidth(maxFrameW);
            frameSize.setHeight(qRound(maxFrameW / frameAspect));
        } else {
            frameSize.setHeight(maxFrameH);
            frameSize.setWidth(qRound(maxFrameH * frameAspect));
        }

        int yOffset = gap + (contentH - frameSize.height()) / 2;
        int leftX = gap;
        int rightX = gap * 2 + frameSize.width();

        if (!m_lastFrame_original.isNull()) {
            QImage scaled = m_lastFrame_original.scaled(frameSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            p.drawImage(QRect(leftX, yOffset, scaled.width(), scaled.height()), scaled);
        } else {
            p.setPen(Qt::white);
            p.drawText(QRect(leftX, yOffset, frameSize.width(), frameSize.height()), Qt::AlignCenter, "Original (Warte auf Stream)");
        }
        p.setPen(Qt::white);
        p.drawText(QRect(leftX, yOffset + frameSize.height() + 6, frameSize.width(), 18), Qt::AlignCenter, "UNVERÄNDERTES ORIGINAL");

        if (!m_lastFrame_processed.isNull()) {
            QImage scaled = m_lastFrame_processed.scaled(frameSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            p.drawImage(QRect(rightX, yOffset, scaled.width(), scaled.height()), scaled);
        } else {
            p.setPen(Qt::white);
            p.drawText(QRect(rightX, yOffset, frameSize.width(), frameSize.height()), Qt::AlignCenter, "Radionullon (Warte auf Stream)");
        }
        p.setPen(QColor(255, 150, 0));
        p.drawText(QRect(rightX, yOffset + frameSize.height() + 6, frameSize.width(), 18), Qt::AlignCenter, "RADIONULLON HEATMAP (ALGORITHMUS)");

        int textX = gap;
        int textY = height() - bottomArea + 20;

        p.setFont(QFont("Monospace", 16, QFont::Bold));
        p.setPen(m_stats.alertState ? Qt::red : Qt::green);
        p.drawText(textX, textY, QString("AKTUELL: %1 Sv/h").arg(m_stats.currentRad, 0, 'f', 2));

        p.setFont(QFont("Monospace", 12));
        p.setPen(Qt::white);
        p.drawText(textX, textY + 26, QString("Ø MITTEL: %1 Sv/h").arg(m_stats.avgRad, 0, 'f', 2));
        p.drawText(textX, textY + 46, QString("GESAMT DOSE: %1 Sv").arg(m_stats.totalDose, 0, 'f', 4));

        // Graphs
        int graphH = 80;
        int margin = 10;
        int graphsAreaW = width() - (4 * margin);
        int singleGraphW = qMax(100, graphsAreaW / 3);
        int yPos = height() - graphH - margin;

        QRect r1(margin, yPos, singleGraphW, graphH);
        QRect r2(margin*2 + singleGraphW, yPos, singleGraphW, graphH);
        QRect r3(margin*3 + singleGraphW*2, yPos, singleGraphW, graphH);

        drawGraph(p, r1, m_stats.longHistory, QColor(0,100,255,120), "Langzeit", m_stats.startTime.toString("HH:mm:ss"), m_stats.currentTime.toString("HH:mm:ss"));
        drawGraph(p, r2, m_stats.avgHistory, QColor(255,200,0,120), "Mittelwert", QString("Ø %1").arg(m_stats.avgRad,0,'f',2), "");
        drawGraph(p, r3, m_stats.shortHistory, m_stats.alertState ? QColor(255,0,0,150) : QColor(0,255,0,120), "Kurzzeit (Peaks)", "Live", "");
    }

private:
    void drawGraph(QPainter &p, QRect r, const QVector<double>& data, QColor col, QString title, QString labelL, QString labelR) {
        p.fillRect(r, QColor(0,0,0,160));
        p.setPen(QColor(100,100,100));
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
        double stepX = static_cast<double>(r.width()) / (data.size() - 1);
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

// -----------------------------
// Tray manager (verwendet standard Qt API, ohne Q_OBJECT)
// -----------------------------
class TrayGraphManager {
public:
    TrayGraphManager(QWidget *win) : m_win(win) {
        tray = new QSystemTrayIcon();
        QMenu *m = new QMenu();
        m->addAction("Toggle Window", [this](){ m_win->isVisible() ? m_win->hide() : m_win->show(); });
        m->addAction("Exit", qApp, &QCoreApplication::quit);
        tray->setContextMenu(m);
        tray->show();
    }

    ~TrayGraphManager() { delete tray; }

    void updateIcon(const GeigerStats &stats) {
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
        tray->setToolTip(QString("Rad: %1 | Avg: %2").arg(stats.currentRad,0,'f',2).arg(stats.avgRad,0,'f',2));
    }

private:
    void drawMiniGraph(QPainter &p, const QVector<double>& data, QColor col, bool fill, double maxScale) {
        if (data.isEmpty()) return;
        QPainterPath path; path.moveTo(0,24);
        for (int i=0;i<data.size();++i) {
            double x = static_cast<double>(i) / data.size() * 24.0;
            double y = 24.0 - (qMin(data[i] / maxScale, 1.0) * 24.0);
            path.lineTo(x,y);
        }
        path.lineTo(24,24);
        if (fill) p.fillPath(path, col);
    }

    void drawMiniLine(QPainter &p, const QVector<double>& data, double maxScale) {
        if (data.isEmpty()) return;
        for (int i=1;i<data.size();++i) {
            double x1 = static_cast<double>(i-1) / data.size() * 24.0;
            double x2 = static_cast<double>(i) / data.size() * 24.0;
            double y1 = 24.0 - (qMin(data[i-1] / maxScale, 1.0) * 24.0);
            double y2 = 24.0 - (qMin(data[i] / maxScale, 1.0) * 24.0);
            p.drawLine(QPointF(x1,y1), QPointF(x2,y2));
        }
    }

    QSystemTrayIcon *tray = nullptr;
    QWidget *m_win = nullptr;
};

// -----------------------------
// main
// -----------------------------
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    // Komponenten
    auto capture = std::make_shared<V4L2Capture>();
    auto logic = std::make_shared<GeigerLogic>();
    auto audio = std::make_shared<AudioEngine>();
    auto widget = std::make_shared<GeigerWidget>();
    auto tray = std::make_shared<TrayGraphManager>(widget.get());

    // Wire callbacks manually (anstatt Qt signals/slots)
    capture->onImageCaptured = [logic, widget](const QImage &img){
        // feed logic and original UI
        logic->processFrame(img);
        widget->updateOriginalImage(img);
    };

    // logic -> UI / tray / audio via callbacks
    logic->onProcessedImage = [widget](const QImage &proc){
        widget->updateProcessedImage(proc);
    };
    logic->onStatsUpdated = [widget, tray, audio](const GeigerStats &stats){
        widget->updateStats(stats);
        tray->updateIcon(stats);
    };
    // request click is implemented inside GeigerLogic via onRequestClick; connect to audio
    logic->setOnRequestClick([audio](double f){
        audio->playClick(f);
    });

    widget->show();
    return app.exec();
}

