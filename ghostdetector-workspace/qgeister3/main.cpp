/*
 * qgeister1 - 3-Graph Edition
 * V4L2 Native Capture mit 3-fach Analyse (Dauer, Mittel, Kurzzeit)
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
#include <QVector>
#include <QSocketNotifier>
#include <QTimer>
#include <QDateTime>

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
#define Y_HOT_THRESH 200 
#define GRAPH_POINTS 100 // Anzahl der Punkte pro Graph

// --- Datenstrukturen ---
struct GeigerStats {
    double currentRad = 0.0;      // Momentaner Wert
    double avgRad = 0.0;          // Mittelwert
    double totalDose = 0.0;       // Gesamtdosis
    bool alertState = false;
    
    QDateTime startTime;
    QDateTime currentTime;

    // Drei separate Historien
    QVector<double> shortHistory; // Raw/Peaks
    QVector<double> avgHistory;   // Geglättet
    QVector<double> longHistory;  // Zeitraffer
};

// --- V4L2 Capture Klasse (Low-Level) ---
struct Buffer { void *start; size_t length; };

class V4L2Capture : public QObject {
    Q_OBJECT
public:
    explicit V4L2Capture(QObject *parent = nullptr) : QObject(parent) {
        fd = open(DEVICE_NODE, O_RDWR | O_NONBLOCK, 0);
        if (fd == -1) { qWarning() << "Fehler bei" << DEVICE_NODE; return; }

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
    ~V4L2Capture() { close(fd); }

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
    int fd; Buffer *buffers; unsigned int n_buffers; QSocketNotifier *notifier;
};

// --- Logik-Klasse ---
class GeigerLogic : public QObject {
    Q_OBJECT
public:
    explicit GeigerLogic(QObject *parent = nullptr) : QObject(parent) {
        startTime = QDateTime::currentDateTime();
        // Puffer initialisieren
        shortHistory.resize(GRAPH_POINTS); shortHistory.fill(0);
        avgHistory.resize(GRAPH_POINTS); avgHistory.fill(0);
        longHistory.resize(GRAPH_POINTS); longHistory.fill(0);
    }

public slots:
    void processFrame(const QImage &img) {
        int hotPixels = 0;
        // Sub-Sampling Scan
        for (int y = 0; y < img.height(); y += 4) {
            const QRgb *line = (const QRgb*)img.constScanLine(y);
            for (int x = 0; x < img.width(); x += 4) {
                if (qGray(line[x]) > Y_HOT_THRESH) hotPixels++;
            }
        }

        // Messwerte
        double rawVal = (double)hotPixels / 50.0; 
        currentRadSmooth = (currentRadSmooth * 0.9) + (rawVal * 0.1);
        totalDoseAcc += currentRadSmooth * 0.001;

        // 1. Kurzzeit (Raw Peaks)
        rotateBuffer(shortHistory, rawVal);

        // 2. Mittelwert (EMA)
        rotateBuffer(avgHistory, currentRadSmooth);

        // 3. Langzeit (Dauerverlauf)
        // Nur jeden 10. Frame speichern (Time-Lapse Effekt)
        longTermCounter++;
        if (longTermCounter >= 10) {
            rotateBuffer(longHistory, currentRadSmooth);
            longTermCounter = 0;
        }

        // Stats packen
        GeigerStats stats;
        stats.currentRad = rawVal;
        stats.avgRad = currentRadSmooth;
        stats.totalDose = totalDoseAcc;
        stats.alertState = currentRadSmooth > 3.0;
        stats.startTime = startTime;
        stats.currentTime = QDateTime::currentDateTime();
        
        stats.shortHistory = shortHistory;
        stats.avgHistory = avgHistory;
        stats.longHistory = longHistory;

        emit statsUpdated(stats);
        
        if (rawVal > 0.5) emit requestClick(440.0 + (rawVal * 100.0));
    }

signals:
    void statsUpdated(GeigerStats stats);
    void requestClick(double freq);

private:
    void rotateBuffer(QVector<double> &buf, double val) {
        for(int i = 0; i < buf.size()-1; ++i) buf[i] = buf[i+1];
        buf[buf.size()-1] = val;
    }

    double currentRadSmooth = 0.0;
    double totalDoseAcc = 0.0;
    QDateTime startTime;
    int longTermCounter = 0;

    QVector<double> shortHistory;
    QVector<double> avgHistory;
    QVector<double> longHistory;
};

// --- Audio Engine ---
class AudioEngine : public QObject {
    Q_OBJECT
public:
    explicit AudioEngine(QObject *parent = nullptr) : QObject(parent) {
        QAudioFormat format;
        format.setSampleRate(44100); format.setChannelCount(1); format.setSampleSize(16);
        format.setCodec("audio/pcm"); format.setByteOrder(QAudioFormat::LittleEndian); format.setSampleType(QAudioFormat::SignedInt);
        QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
        if (!info.isFormatSupported(format)) format = info.nearestFormat(format);
        audioOutput = new QAudioOutput(format, this); audioDevice = audioOutput->start();
    }
public slots:
    void playClick(double freq) {
        if (!audioDevice) return;
        int sampleCount = 1500; // kurz
        QByteArray data; data.resize(sampleCount * 2);
        qint16 *ptr = reinterpret_cast<qint16*>(data.data());
        for (int i = 0; i < sampleCount; ++i) {
            double t = (double)i / 44100.0;
            ptr[i] = static_cast<qint16>(qSin(2 * M_PI * freq * t) * qExp(-t * 30) * 10000);
        }
        audioDevice->write(data);
    }
private:
    QAudioOutput *audioOutput; QIODevice *audioDevice;
};

// --- UI Widget ---
class GeigerWidget : public QWidget {
    Q_OBJECT
public:
    GeigerWidget(GeigerLogic *logic, QWidget *parent = nullptr) : QWidget(parent), m_logic(logic) {
        setWindowTitle("qrad 3-Graph");
        resize(800, 600);
        setStyleSheet("background-color: #000; color: #fff;");
    }

public slots:
    void updateStats(GeigerStats stats) { m_stats = stats; update(); }
    void updateImage(const QImage &img) { m_lastFrame = img; update(); }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        
        // 1. Video Hintergrund
        if (!m_lastFrame.isNull()) {
            p.drawImage(rect(), m_lastFrame);
        }
        
        // 2. Hauptwerte (Oben Links)
        p.setFont(QFont("Monospace", 18, QFont::Bold));
        p.setPen(Qt::green);
        p.drawText(20, 40, QString("AKTUELL: %1 µSv/h").arg(m_stats.currentRad, 0, 'f', 2));
        p.setPen(Qt::white);
        p.drawText(20, 70, QString("DOSIS:   %1 µSv").arg(m_stats.totalDose, 0, 'f', 4));

        // 3. Die 3 transparenten Graphen (Unten)
        int graphH = 100;
        int margin = 10;
        int totalW = width() - (4 * margin);
        int singleGraphW = totalW / 3;
        int yPos = height() - graphH - margin;

        // --- Graph 1: Dauerverlauf (Langzeit) ---
        QRect r1(margin, yPos, singleGraphW, graphH);
        QString tStart = m_stats.startTime.toString("HH:mm:ss");
        QString tEnd = m_stats.currentTime.toString("HH:mm:ss");
        drawGraph(p, r1, m_stats.longHistory, QColor(0, 100, 255, 120), "Dauerverlauf", tStart, tEnd);

        // --- Graph 2: Mittelwertsverlauf ---
        QRect r2(margin*2 + singleGraphW, yPos, singleGraphW, graphH);
        QString avgLabel = QString("Ø %1").arg(m_stats.avgRad, 0, 'f', 2);
        drawGraph(p, r2, m_stats.avgHistory, QColor(255, 200, 0, 120), "Mittelwert", avgLabel, "");

        // --- Graph 3: Kurzzeitsverlauf (Peaks) ---
        QRect r3(margin*3 + singleGraphW*2, yPos, singleGraphW, graphH);
        QColor peakCol = m_stats.alertState ? QColor(255, 0, 0, 150) : QColor(0, 255, 0, 120);
        drawGraph(p, r3, m_stats.shortHistory, peakCol, "Kurzzeit (Peaks)", "Live", "");
    }

private:
    void drawGraph(QPainter &p, QRect r, const QVector<double> &data, QColor col, QString title, QString labelL, QString labelR) {
        // Hintergrund
        p.fillRect(r, QColor(0, 0, 0, 160)); // Halbtransparentes Schwarz
        p.setPen(QColor(100, 100, 100));
        p.drawRect(r);

        // Titel
        p.setPen(Qt::white);
        p.setFont(QFont("Sans", 9, QFont::Bold));
        p.drawText(r.adjusted(5, 5, -5, 0), Qt::AlignTop | Qt::AlignLeft, title);

        // Labels unten
        p.setFont(QFont("Sans", 7));
        if (!labelL.isEmpty()) p.drawText(r.adjusted(5, 0, 0, -2), Qt::AlignBottom | Qt::AlignLeft, labelL);
        if (!labelR.isEmpty()) p.drawText(r.adjusted(0, 0, -5, -2), Qt::AlignBottom | Qt::AlignRight, labelR);

        if (data.isEmpty()) return;

        // Autoscaling
        double maxVal = 0.001;
        for(double v : data) if(v > maxVal) maxVal = v;
        maxVal *= 1.2; // Headroom

        // Graph zeichnen
        QPainterPath path;
        double stepX = (double)r.width() / (data.size() - 1);
        
        // Bodenlinie für Fill
        path.moveTo(r.left(), r.bottom());

        for (int i = 0; i < data.size(); ++i) {
            double valNorm = data[i] / maxVal;
            double x = r.left() + (i * stepX);
            double y = r.bottom() - (valNorm * r.height() * 0.8); // 80% Höhe nutzen
            path.lineTo(x, y);
        }
        // Abschluss für Fill
        path.lineTo(r.right(), r.bottom());
        path.closeSubpath();

        p.fillPath(path, col);
        p.setPen(QColor(255, 255, 255, 150));
        p.drawPath(path);
    }

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
        QPixmap pix(24, 24); pix.fill(Qt::transparent);
        QPainter p(&pix);
        p.setRenderHint(QPainter::Antialiasing, false); // Sharp pixels

        // 1. Layer: Langzeit (Hintergrund, Blau)
        drawMiniGraph(p, stats.longHistory, QColor(0, 0, 255, 200), true);

        // 2. Layer: Mittelwert (Linie, Gelb, etwas dicker)
        QPen penAvg(QColor(255, 200, 0)); penAvg.setWidth(2); p.setPen(penAvg);
        drawMiniLine(p, stats.avgHistory);

        // 3. Layer: Kurzzeit (Feine Peaks, Grün/Rot)
        p.setPen(stats.alertState ? Qt::red : Qt::green);
        drawMiniLine(p, stats.shortHistory);

        // Rahmen
        p.setPen(QColor(128,128,128,100)); p.drawRect(0,0,23,23);
        tray->setIcon(QIcon(pix));
        tray->setToolTip(QString("Rad: %1 | Avg: %2").arg(stats.currentRad).arg(stats.avgRad));
    }

private:
    void drawMiniGraph(QPainter &p, const QVector<double> &data, QColor col, bool fill) {
        if (data.isEmpty()) return;
        double maxV = 0.001; for(double v : data) if(v>maxV) maxV=v;
        
        QPainterPath path; path.moveTo(0, 24);
        for(int i=0; i<data.size(); ++i) {
            double x = (double)i / data.size() * 24.0;
            double y = 24.0 - ((data[i]/maxV) * 24.0);
            path.lineTo(x, y);
        }
        path.lineTo(24, 24);
        if(fill) p.fillPath(path, col);
        else p.drawPath(path);
    }

    void drawMiniLine(QPainter &p, const QVector<double> &data) {
        if (data.isEmpty()) return;
        double maxV = 0.5; // Fixer scale für bessere Vergleichbarkeit
        for(double v : data) if(v>maxV) maxV=v;

        for(int i=1; i<data.size(); ++i) {
            double x1 = (double)(i-1) / data.size() * 24.0;
            double x2 = (double)i / data.size() * 24.0;
            double y1 = 24.0 - ((data[i-1]/maxV) * 24.0);
            double y2 = 24.0 - ((data[i]/maxV) * 24.0);
            p.drawLine(QPointF(x1, y1), QPointF(x2, y2));
        }
    }

    QSystemTrayIcon *tray; GeigerLogic *m_logic; QWidget *m_win;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    V4L2Capture capture;
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
