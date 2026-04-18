/*
 * qgeister1 - V4L2 Native Version
 * Nutzt direkt /dev/video0 und visualisiert "Strahlung" (Helligkeit/Rauschen)
 */

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
#include <QQueue>
#include <QSocketNotifier>
#include <QTimer>

// Linux V4L2 Includes
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

// --- Konfiguration ---
#define DEVICE_NODE "/dev/video0"
#define IMG_W 640
#define IMG_H 480
#define HISTORY_SIZE 24
#define Y_HOT_THRESH 200 // Helligkeitsschwelle (0-255) für "Hot Pixel"

// --- Datenstrukturen ---
struct GeigerStats {
    double currentRad = 0.0;
    double totalDose = 0.0;
    bool alertState = false;
};

// --- V4L2 Capture Klasse (Der Low-Level Treiber) ---
struct Buffer {
    void   *start;
    size_t  length;
};

class V4L2Capture : public QObject {
    Q_OBJECT
public:
    explicit V4L2Capture(QObject *parent = nullptr) : QObject(parent) {
        fd = open(DEVICE_NODE, O_RDWR | O_NONBLOCK, 0);
        if (fd == -1) {
            qWarning() << "Kann" << DEVICE_NODE << "nicht öffnen!";
            return;
        }

        // Format setzen (YUYV 640x480)
        struct v4l2_format fmt;
        memset(&fmt, 0, sizeof(fmt));
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width = IMG_W;
        fmt.fmt.pix.height = IMG_H;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; 
        fmt.fmt.pix.field = V4L2_FIELD_NONE;
        if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) qWarning() << "S_FMT fehlgeschlagen";

        // Buffer anfordern
        struct v4l2_requestbuffers req;
        memset(&req, 0, sizeof(req));
        req.count = 2;
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_MMAP;
        ioctl(fd, VIDIOC_REQBUFS, &req);

        buffers = (Buffer*)calloc(req.count, sizeof(*buffers));

        for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
            struct v4l2_buffer buf;
            memset(&buf, 0, sizeof(buf));
            buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory      = V4L2_MEMORY_MMAP;
            buf.index       = n_buffers;
            ioctl(fd, VIDIOC_QUERYBUF, &buf);
            buffers[n_buffers].length = buf.length;
            buffers[n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        }

        for (unsigned int i = 0; i < n_buffers; ++i) {
            struct v4l2_buffer buf;
            memset(&buf, 0, sizeof(buf));
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = i;
            ioctl(fd, VIDIOC_QBUF, &buf);
        }

        enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ioctl(fd, VIDIOC_STREAMON, &type);

        // Qt Event Loop Integration via SocketNotifier
        notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
        connect(notifier, &QSocketNotifier::activated, this, &V4L2Capture::readFrame);
    }

    ~V4L2Capture() {
        enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ioctl(fd, VIDIOC_STREAMOFF, &type);
        close(fd);
    }

signals:
    void imageCaptured(const QImage &img);

private slots:
    void readFrame() {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1) return;

        // YUYV zu RGB Konvertierung (Simpel)
        unsigned char* src = (unsigned char*)buffers[buf.index].start;
        QImage img(IMG_W, IMG_H, QImage::Format_RGB32);
        
        for (int y = 0; y < IMG_H; y++) {
            unsigned char* srcLine = src + (y * IMG_W * 2);
            QRgb* dstLine = (QRgb*)img.scanLine(y);
            for (int x = 0; x < IMG_W; x += 2) {
                int Y1 = srcLine[0];
                int U  = srcLine[1];
                int Y2 = srcLine[2];
                int V  = srcLine[3];
                srcLine += 4;

                // YUV zu RGB Formel (Integer approx)
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
    int fd;
    struct Buffer *buffers;
    unsigned int n_buffers;
    QSocketNotifier *notifier;
};

// --- Logik-Klasse (Analysiert das V4L2 Bild) ---
class GeigerLogic : public QObject {
    Q_OBJECT
public:
    explicit GeigerLogic(QObject *parent = nullptr) : QObject(parent) {
        for(int i=0; i<HISTORY_SIZE; ++i) { radHistory.enqueue(0.0); doseHistory.enqueue(0.0); }
    }

    QQueue<double> getRadHistory() const { return radHistory; }
    QQueue<double> getDoseHistory() const { return doseHistory; }

public slots:
    void processFrame(const QImage &img) {
        int hotPixels = 0;
        
        // Einfache Analyse: Zähle helle Pixel ("Strahlung")
        // Wir scannen nur jeden 4. Pixel für Performance
        for (int y = 0; y < img.height(); y += 4) {
            const QRgb *line = (const QRgb*)img.constScanLine(y);
            for (int x = 0; x < img.width(); x += 4) {
                if (qGray(line[x]) > Y_HOT_THRESH) hotPixels++;
            }
        }

        // Normierung (Willkürlich kalibriert auf Webcam-Rauschen)
        double rawVal = (double)hotPixels / 100.0; 
        
        // EMA Filter
        currentRadSmooth = (currentRadSmooth * 0.7) + (rawVal * 0.3);
        totalDoseAcc += currentRadSmooth * 0.001;

        // History Ringbuffer
        if (radHistory.size() >= HISTORY_SIZE) radHistory.dequeue();
        if (doseHistory.size() >= HISTORY_SIZE) doseHistory.dequeue();
        radHistory.enqueue(currentRadSmooth);
        doseHistory.enqueue(totalDoseAcc);

        GeigerStats stats;
        stats.currentRad = currentRadSmooth;
        stats.totalDose = totalDoseAcc;
        stats.alertState = currentRadSmooth > 5.0; // Schwelle anpassen

        emit statsUpdated(stats);
        
        // Klick bei starkem Anstieg
        if (rawVal > 0.5) emit requestClick(440.0 + (rawVal * 100.0));
    }

signals:
    void statsUpdated(GeigerStats stats);
    void requestClick(double freq);

private:
    double currentRadSmooth = 0.0;
    double totalDoseAcc = 0.0;
    QQueue<double> radHistory;
    QQueue<double> doseHistory;
};

// --- Audio Engine (Qt5) ---
class AudioEngine : public QObject {
    Q_OBJECT
public:
    explicit AudioEngine(QObject *parent = nullptr) : QObject(parent) {
        QAudioFormat format;
        format.setSampleRate(44100);
        format.setChannelCount(1);
        format.setSampleSize(16);
        format.setCodec("audio/pcm");
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::SignedInt);
        
        QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
        if (!info.isFormatSupported(format)) format = info.nearestFormat(format);
        
        audioOutput = new QAudioOutput(format, this);
        audioDevice = audioOutput->start();
    }

public slots:
    void playClick(double freq) {
        if (!audioDevice) return;
        int sampleCount = 2200; // ~50ms
        QByteArray data; data.resize(sampleCount * 2);
        qint16 *ptr = reinterpret_cast<qint16*>(data.data());
        for (int i = 0; i < sampleCount; ++i) {
            double t = (double)i / 44100.0;
            ptr[i] = static_cast<qint16>(qSin(2 * M_PI * freq * t) * qExp(-t * 20) * 10000);
        }
        audioDevice->write(data);
    }
private:
    QAudioOutput *audioOutput;
    QIODevice *audioDevice;
};

// --- UI Widget (Mit Video Hintergrund) ---
class GeigerWidget : public QWidget {
    Q_OBJECT
public:
    GeigerWidget(GeigerLogic *logic, QWidget *parent = nullptr) : QWidget(parent), m_logic(logic) {
        setWindowTitle("qrad V4L2");
        resize(640, 480);
        setStyleSheet("background-color: #000; color: #0f0; font-family: Monospace;");
    }

public slots:
    void updateStats(GeigerStats stats) { m_stats = stats; update(); }
    void updateImage(const QImage &img) { m_lastFrame = img; update(); }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        
        // 1. Hintergrund Video
        if (!m_lastFrame.isNull()) {
            p.drawImage(rect(), m_lastFrame);
            // Abdunkeln für bessere Lesbarkeit
            p.fillRect(rect(), QColor(0, 0, 0, 100));
        }

        // 2. Overlay Text
        p.setPen(Qt::green);
        p.setFont(QFont("Monospace", 24, QFont::Bold));
        p.drawText(20, 50, QString("RAD: %1 µSv/h").arg(m_stats.currentRad, 0, 'f', 2));
        
        p.setFont(QFont("Monospace", 14));
        p.setPen(Qt::white);
        p.drawText(20, 90, QString("DOSE: %1 µSv").arg(m_stats.totalDose, 0, 'f', 4));

        if (m_stats.alertState) {
            p.setPen(Qt::red);
            p.drawText(rect().center(), "WARNUNG: STRAHLUNG HOCH");
        }
    }

private:
    GeigerLogic *m_logic;
    GeigerStats m_stats;
    QImage m_lastFrame;
};

// --- Tray Manager ---
class TrayGraphManager : public QObject {
    Q_OBJECT
public:
    TrayGraphManager(GeigerLogic *logic, QWidget *win, QObject *p = nullptr) : QObject(p), m_logic(logic), m_win(win) {
        tray = new QSystemTrayIcon(this);
        QMenu *m = new QMenu();
        m->addAction("Show/Hide", [this](){ m_win->isVisible() ? m_win->hide() : m_win->show(); });
        m->addAction("Exit", qApp, &QCoreApplication::quit);
        tray->setContextMenu(m);
        tray->show();
        
        connect(tray, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason r){
            if (r == QSystemTrayIcon::DoubleClick) m_win->isVisible() ? m_win->hide() : m_win->show();
        });
    }

public slots:
    void updateIcon(GeigerStats stats) {
        QPixmap pix(24, 24); pix.fill(Qt::transparent);
        QPainter p(&pix);
        auto rHist = m_logic->getRadHistory();
        auto dHist = m_logic->getDoseHistory();
        if(rHist.isEmpty()) return;

        // Graph A (Dosis, Blau, Hintergrund)
        double minD = 1e9, maxD = -1e9;
        for(double v : dHist) { if(v<minD) minD=v; if(v>maxD) maxD=v; }
        double range = maxD - minD; if(range < 0.001) range=1.0;
        
        QPainterPath path; path.moveTo(0, 24);
        for(int i=0; i<dHist.size(); ++i) 
            path.lineTo(i, 24 - ((dHist[i]-minD)/range * 12.0));
        path.lineTo(23, 24);
        p.fillPath(path, QColor(0, 0, 255, 120));

        // Graph B (Aktuell, Grün/Rot)
        p.setPen(stats.alertState ? Qt::red : Qt::green);
        for(int i=1; i<rHist.size(); ++i) {
            double h1 = qBound(0.0, rHist[i-1] * 2.0, 24.0);
            double h2 = qBound(0.0, rHist[i] * 2.0, 24.0);
            p.drawLine(i-1, 24-h1, i, 24-h2);
        }
        
        p.setPen(Qt::gray); p.drawRect(0,0,23,23);
        tray->setIcon(QIcon(pix));
    }
private:
    QSystemTrayIcon *tray; GeigerLogic *m_logic; QWidget *m_win;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    V4L2Capture capture; // Startet Kamera sofort
    GeigerLogic logic;
    AudioEngine audio;
    GeigerWidget w(&logic);
    TrayGraphManager tray(&logic, &w);

    QObject::connect(&capture, &V4L2Capture::imageCaptured, &logic, &GeigerLogic::processFrame);
    QObject::connect(&capture, &V4L2Capture::imageCaptured, &w, &GeigerWidget::updateImage);
    QObject::connect(&logic, &GeigerLogic::statsUpdated, &w, &GeigerWidget::updateStats);
    QObject::connect(&logic, &GeigerLogic::statsUpdated, &tray, &TrayGraphManager::updateIcon);
    QObject::connect(&logic, &GeigerLogic::requestClick, &audio, &AudioEngine::playClick);

    w.show();
    return app.exec();
}

#include "main.moc"
