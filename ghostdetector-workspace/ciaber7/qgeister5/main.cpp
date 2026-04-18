/*
 * qgeister1 - Radionullon Ghost Filter Edition (Dual-Display)
 * Zeigt das unveränderte Originalbild und das Radionullon-Heatmap-Bild nebeneinander.
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
#include <QElapsedTimer> 

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
#define GRAPH_POINTS 100 // Anzahl der Punkte pro Graph

// --- Radionullon-Tunables ---
#define BG_LEARN_RATE 0.01f     // Hintergrund-EMA-Faktor
#define RESIDUE_THRESH 15       // Intensitätsdifferenz für Bewegungsmaske
#define FLICKER_THRESH 20       // Temporäre Flicker-Empfindlichkeit (Global)
#define EDGE_GAIN 1.6f          // Sobel-Kantenverstärkung
#define HEATMAP_ALPHA 155       // Overlay-Transparenz (0..255)
#define RADIONULLON_THRESH 30   // Mindest-Score für Heatmap-Anzeige

// --- NEUE GEIGER-TUNABLES (aus geiger_cli.c abgeleitet) ---
#define HOT_PIXEL_THRESH 200     // Y_HOT_THRESH aus geiger_cli.c (auf RGB32 Graustufen angewandt)
#define GEIGER_SMOOTH_FACTOR 0.3 // EMA-Faktor aus geiger_cli.c (0.3)
#define GEIGER_NORM_FACTOR 100.0 // Normalisierungsfaktor aus geiger_cli.c (100.0)
#define GEIGER_ALERT_THRESH 5.0  // Alert-Schwelle aus geiger_cli.c (5.0)

// --- Datenstrukturen ---
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

// --- V4L2 Capture Klasse (Liefert RGB32 QImage) ---
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
        // Output format ist RGB32 (entspricht 4 Bytes pro Pixel)
        QImage img(IMG_W, IMG_H, QImage::Format_RGB32);
        
        for (int y = 0; y < IMG_H; y++) {
            unsigned char* srcLine = src + (y * IMG_W * 2);
            QRgb* dstLine = (QRgb*)img.scanLine(y);
            for (int x = 0; x < IMG_W; x += 2) {
                int Y1 = srcLine[0]; int U = srcLine[1]; int Y2 = srcLine[2]; int V = srcLine[3]; srcLine += 4;
                // YUV zu RGB Konvertierung
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
        emit imageCaptured(img); // Originalbild senden
    }
private:
    int fd; Buffer *buffers; unsigned int n_buffers; QSocketNotifier *notifier;
};

// --- Logik-Klasse (enthält den Radionullon-Filter und die NEUE Geiger-Logik) ---
class GeigerLogic : public QObject {
    Q_OBJECT
public:
    explicit GeigerLogic(QObject *parent = nullptr) : QObject(parent) {
        startTime = QDateTime::currentDateTime();
        
        // Speicher für den Radionullon-Algorithmus
        bg.resize(IMG_W * IMG_H); bg.fill(0.0f);
        flicker.resize(IMG_W * IMG_H); flicker.fill(0);

        // Historien
        shortHistory.resize(GRAPH_POINTS); shortHistory.fill(0);
        avgHistory.resize(GRAPH_POINTS); avgHistory.fill(0);
        longHistory.resize(GRAPH_POINTS); longHistory.fill(0);
    }

    // Qt-konforme Version des Radionullon-Filters (mit integrierter Hot-Pixel-Geiger-Logik)
    QImage processRadionullon(const QImage &img) {
        if (img.isNull()) return img;

        QImage outputImg = img;
        int size = IMG_W * IMG_H;
        
        QVector<quint8> gray(size);
        QVector<quint8> residue(size);
        QVector<quint8> edge(size);

        // NEUE GEIGER-LOGIK: Hot Pixel Zähler
        int hotPixels = 0;

        // 1. RGB32 zu Graustufen konvertieren und Hot Pixels zählen
        for (int y = 0; y < IMG_H; ++y) {
            const QRgb* srcLine = (const QRgb*)img.constScanLine(y);
            for (int x = 0; x < IMG_W; ++x) {
                quint8 g = (quint8)qGray(srcLine[x]);
                gray[y * IMG_W + x] = g;
                
                // Hot Pixel Zählung (basierend auf Y_HOT_THRESH aus geiger_cli.c)
                if (g > HOT_PIXEL_THRESH) {
                    hotPixels++;
                }
            }
        }

        // 2. Hintergrundmodell (EMA) und Residue (Bewegungsmaske)
        float globalAvg = 0.0f;
        for (int i = 0; i < size; i++) {
            // Update BG (EMA)
            bg[i] = (1.0f - BG_LEARN_RATE) * bg[i] + BG_LEARN_RATE * (float)gray[i];
            
            // Residue
            int diff = qAbs((int)gray[i] - (int)bg[i]);
            residue[i] = (diff > RESIDUE_THRESH) ? (quint8)qMin(diff, 255) : 0;
            globalAvg += (float)gray[i];
        }
        globalAvg /= size;


        // 3. Flicker-Maß (Temporale Helligkeitsschwankung)
        quint8 currentAvg = (quint8)qBound(0.0f, globalAvg, 255.0f);
        int delta = qAbs((int)currentAvg - (int)prevGlobalAvg);
        prevGlobalAvg = currentAvg;
        quint8 boost = (delta > FLICKER_THRESH) ? 20 : 1;

        for (int i = 0; i < size; i++) {
            // Flicker-Akkumulation und Decay
            flicker[i] = qBound(0, (int)flicker[i] + boost, 255);
            if (flicker[i] > 0) flicker[i] = qMax(0, (int)flicker[i] - 1);
        }

        // 4. Sobel Edge Detection
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


        // 5. Heatmap Overlay und Radionullon-Analyse (VISUELL)
        // totalScore wird weiterhin für die VISUELLE Heatmap benötigt
        double totalScore = 0.0;
        for (int y = 0; y < IMG_H; ++y) {
            QRgb* dstLine = (QRgb*)outputImg.scanLine(y);
            for (int x = 0; x < IMG_W; ++x) {
                int i = y*IMG_W + x;
                // Anomaly Score (Radionullon)
                int score = (int)residue[i] + (int)(edge[i]/2) + (int)(flicker[i]/3);
                totalScore += score;
                
                if (score < RADIONULLON_THRESH) {
                    // Bild leicht graustufen/entsättigen, wenn Score niedrig ist
                    dstLine[x] = qRgb(gray[i], gray[i], gray[i]); 
                    continue; 
                }

                // Map Score to Heat Color (Blue -> Green -> Yellow -> Red)
                int s = qMin(score, 255);
                quint8 r, g, b;
                if (s < 64) { b = 255; g = s*4; r = 0; }
                else if (s < 128) { b = 255 - (s-64)*4; g = 255; r = 0; }
                else if (s < 192) { b = 0; g = 255; r = (s-128)*4; }
                else { b = 0; g = 255 - (s-192)*4; r = 255; }

                // Alpha Blend
                quint8 R0 = qRed(dstLine[x]), G0 = qGreen(dstLine[x]), B0 = qBlue(dstLine[x]);
                int alpha = HEATMAP_ALPHA;
                quint8 R_blend = (R0*(255-alpha) + r*alpha)/255;
                quint8 G_blend = (G0*(255-alpha) + g*alpha)/255;
                quint8 B_blend = (B0*(255-alpha) + b*alpha)/255;
                dstLine[x] = qRgb(R_blend, G_blend, B_blend);
            }
        }

        // 6. Geiger-Werte aus Hot-Pixel-Zählung ableiten (NEUE LOGIK)
        // rawVal = (double)hotPixels / 100.0;
        double rawVal = (double)hotPixels / GEIGER_NORM_FACTOR; 
        
        // currentRadSmooth = (currentRadSmooth * 0.7) + (rawVal * 0.3);
        currentRadSmooth = (currentRadSmooth * (1.0 - GEIGER_SMOOTH_FACTOR)) + (rawVal * GEIGER_SMOOTH_FACTOR);
        
        // totalDoseAcc += currentRadSmooth * 0.001; (unverändert)
        totalDoseAcc += currentRadSmooth * 0.001;

        // Historien
        rotateBuffer(shortHistory, rawVal);
        rotateBuffer(avgHistory, currentRadSmooth);
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
        // Neue Alert-Schwelle: alertState = currentRadSmooth > 5.0;
        stats.alertState = currentRadSmooth > GEIGER_ALERT_THRESH;
        stats.startTime = startTime;
        stats.currentTime = QDateTime::currentDateTime();
        stats.shortHistory = shortHistory;
        stats.avgHistory = avgHistory;
        stats.longHistory = longHistory;

        emit statsUpdated(stats);
        if (rawVal > 2.0) emit requestClick(440.0 + (rawVal * 100.0));
        
        return outputImg;
    }

signals:
    void statsUpdated(GeigerStats stats);
    void requestClick(double freq);
    void processedImage(const QImage &img); // Signal für das verarbeitete Bild

public slots:
    void processFrame(const QImage &img) {
        // Die Logik erhält das Originalbild von V4L2Capture
        QImage processed = processRadionullon(img);
        
        // Wir senden nur das verarbeitete Bild weiter.
        emit processedImage(processed);
    }

private:
    void rotateBuffer(QVector<double> &buf, double val) {
        for(int i = 0; i < buf.size()-1; ++i) buf[i] = buf[i+1];
        buf[buf.size()-1] = val;
    }

    double currentRadSmooth = 0.0;
    double totalDoseAcc = 0.0;
    QDateTime startTime;
    int longTermCounter = 0;

    // Radionullon States
    QVector<float> bg;
    QVector<quint8> flicker;
    float prevGlobalAvg = 128.0f; 

    // Historien
    QVector<double> shortHistory;
    QVector<double> avgHistory;
    QVector<double> longHistory;
};

// --- Audio Engine (unverändert) ---
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
        int sampleCount = 1500;
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


// --- UI Widget (Resizable, Dual-Display) ---
class GeigerWidget : public QWidget {
    Q_OBJECT
public:
    GeigerWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("qrad Radionullon Viewer | Original vs. Heatmap");
        setMinimumSize(640, 480); // sinnvolle Mindestgröße
        // Kein fixiertes resize mehr — das Fenster ist jetzt frei skalierbar
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

        const int gap = 8;
        const int bottomArea = 120; // Platz für Text + Graphen
        int contentW = width() - gap * 3;
        int contentH = height() - gap * 3 - bottomArea;

        // Zwei Bildbereiche nebeneinander, jeweils max half der contentW
        int maxFrameW = contentW / 2;
        int maxFrameH = contentH;

        // Behalte Video-Frame-Seitenverhältnis (IMG_W:IMG_H)
        double frameAspect = (double)IMG_W / (double)IMG_H;
        QSize frameSize;
        // Berechne bestmögliche Größe, die in (maxFrameW, maxFrameH) passt
        if (maxFrameW / frameAspect <= maxFrameH) {
            frameSize.setWidth(maxFrameW);
            frameSize.setHeight(qRound(maxFrameW / frameAspect));
        } else {
            frameSize.setHeight(maxFrameH);
            frameSize.setWidth(qRound(maxFrameH * frameAspect));
        }

        // Zentriert vertikal innerhalb des content-Bereichs
        int yOffset = gap + (contentH - frameSize.height()) / 2;
        int leftX = gap;
        int rightX = gap * 2 + frameSize.width();

        // --- 1. Linkes Bild: Original ---
        if (!m_lastFrame_original.isNull()) {
            QImage scaled = m_lastFrame_original.scaled(frameSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            p.drawImage(QRect(leftX, yOffset, scaled.width(), scaled.height()), scaled);
        } else {
            p.setPen(Qt::white);
            p.drawText(QRect(leftX, yOffset, frameSize.width(), frameSize.height()), Qt::AlignCenter, "Original (Warte auf Stream)");
        }
        p.setPen(Qt::white);
        p.drawText(QRect(leftX, yOffset + frameSize.height() + 6, frameSize.width(), 18), Qt::AlignCenter, "UNVERÄNDERTES ORIGINAL");

        // --- 2. Rechtes Bild: Processed ---
        if (!m_lastFrame_processed.isNull()) {
            QImage scaled = m_lastFrame_processed.scaled(frameSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            p.drawImage(QRect(rightX, yOffset, scaled.width(), scaled.height()), scaled);
        } else {
            p.setPen(Qt::white);
            p.drawText(QRect(rightX, yOffset, frameSize.width(), frameSize.height()), Qt::AlignCenter, "Radionullon (Warte auf Stream)");
        }
        p.setPen(QColor(255, 150, 0));
        p.drawText(QRect(rightX, yOffset + frameSize.height() + 6, frameSize.width(), 18), Qt::AlignCenter, "RADIONULLON HEATMAP (ALGORITHMUS)");

        // --- 3. Overlay Text und Geiger-Werte (unten, links) ---
        int textX = gap;
        int textY = height() - bottomArea + 20;

        p.setFont(QFont("Monospace", 16, QFont::Bold));
        p.setPen(m_stats.alertState ? Qt::red : Qt::green);
        p.drawText(textX, textY, QString("AKTUELL: %1 Sv/h").arg(m_stats.currentRad, 0, 'f', 2));

        p.setFont(QFont("Monospace", 12));
        p.setPen(Qt::white);
        p.drawText(textX, textY + 26, QString("Ø MITTEL: %1 Sv/h").arg(m_stats.avgRad, 0, 'f', 2));
        p.drawText(textX, textY + 46, QString("GESAMT DOSE: %1 Sv").arg(m_stats.totalDose, 0, 'f', 4));

        // --- 4. Drei Graphen (unten) ---
        int graphH = 80;
        int margin = 10;
        int graphsAreaW = width() - (4 * margin);
        int singleGraphW = qMax(100, graphsAreaW / 3); // min width
        int yPos = height() - graphH - margin;

        QRect r1(margin, yPos, singleGraphW, graphH);
        QRect r2(margin*2 + singleGraphW, yPos, singleGraphW, graphH);
        QRect r3(margin*3 + singleGraphW*2, yPos, singleGraphW, graphH);

        QString tStart = m_stats.startTime.toString("HH:mm:ss");
        QString tEnd = m_stats.currentTime.toString("HH:mm:ss");
        drawGraph(p, r1, m_stats.longHistory, QColor(0, 100, 255, 120), "Langzeit", tStart, tEnd);

        QString avgLabel = QString("Ø %1").arg(m_stats.avgRad, 0, 'f', 2);
        drawGraph(p, r2, m_stats.avgHistory, QColor(255, 200, 0, 120), "Mittelwert", avgLabel, "");

        QColor peakCol = m_stats.alertState ? QColor(255, 0, 0, 150) : QColor(0, 255, 0, 120);
        drawGraph(p, r3, m_stats.shortHistory, peakCol, "Kurzzeit (Peaks)", "Live", "");
    }

private:
    void drawGraph(QPainter &p, QRect r, const QVector<double> &data, QColor col, QString title, QString labelL, QString labelR) {
        p.fillRect(r, QColor(0, 0, 0, 160));
        p.setPen(QColor(100, 100, 100));
        p.drawRect(r);

        p.setPen(Qt::white);
        p.setFont(QFont("Sans", 9, QFont::Bold));
        p.drawText(r.adjusted(5, 5, -5, 0), Qt::AlignTop | Qt::AlignLeft, title);

        p.setFont(QFont("Sans", 7));
        if (!labelL.isEmpty()) p.drawText(r.adjusted(5, 0, 0, -2), Qt::AlignBottom | Qt::AlignLeft, labelL);
        if (!labelR.isEmpty()) p.drawText(r.adjusted(0, 0, -5, -2), Qt::AlignBottom | Qt::AlignRight, labelR);

        if (data.isEmpty()) return;

        double maxVal = 0.001;
        for(double v : data) if(v > maxVal) maxVal = v;
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
        p.setPen(QColor(255, 255, 255, 150));
        p.drawPath(path);
    }

    GeigerStats m_stats;
    QImage m_lastFrame_original;
    QImage m_lastFrame_processed;
};

// --- Tray Manager (unverändert) ---
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
        p.setRenderHint(QPainter::Antialiasing, false);

        double maxScale = 5.0; // Fester Max-Wert für Tray-Skalierung

        // 1. Langzeit (Hintergrund, Blau)
        drawMiniGraph(p, stats.longHistory, QColor(0, 0, 255, 150), true, maxScale);
        // 2. Mittelwert (Linie, Gelb)
        QPen penAvg(QColor(255, 200, 0)); penAvg.setWidth(2); p.setPen(penAvg);
        drawMiniLine(p, stats.avgHistory, maxScale);
        // 3. Kurzzeit (Feine Peaks, Grün/Rot)
        QPen penShort(stats.alertState ? Qt::red : Qt::green); penShort.setWidth(1); p.setPen(penShort);
        drawMiniLine(p, stats.shortHistory, maxScale);

        // Rahmen
        p.setPen(QColor(128,128,128,100)); p.drawRect(0,0,23,23);
        tray->setIcon(QIcon(pix));
        tray->setToolTip(QString("Rad: %1 | Avg: %2").arg(stats.currentRad, 0, 'f', 2).arg(stats.avgRad, 0, 'f', 2));
    }

private:
    void drawMiniGraph(QPainter &p, const QVector<double> &data, QColor col, bool fill, double maxScale) {
        if (data.isEmpty()) return;
        QPainterPath path; path.moveTo(0, 24);
        for(int i=0; i<data.size(); ++i) {
            double x = (double)i / data.size() * 24.0;
            double y = 24.0 - (qMin(data[i] / maxScale, 1.0) * 24.0);
            path.lineTo(x, y);
        }
        path.lineTo(24, 24);
        if(fill) p.fillPath(path, col);
    }

    void drawMiniLine(QPainter &p, const QVector<double> &data, double maxScale) {
        if (data.isEmpty()) return;
        for(int i=1; i<data.size(); ++i) {
            double x1 = (double)(i-1) / data.size() * 24.0;
            double x2 = (double)i / data.size() * 24.0;
            double y1 = 24.0 - (qMin(data[i-1] / maxScale, 1.0) * 24.0);
            double y2 = 24.0 - (qMin(data[i] / maxScale, 1.0) * 24.0);
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
    GeigerWidget w; 
    TrayGraphManager tray(&logic, &w);

    // 1. Kamera -> Logik (Radionullon-Verarbeitung)
    QObject::connect(&capture, &V4L2Capture::imageCaptured, &logic, &GeigerLogic::processFrame);
    // 2. Kamera -> Widget (Originalbild, linke Seite)
    QObject::connect(&capture, &V4L2Capture::imageCaptured, &w, &GeigerWidget::updateOriginalImage);
    // 3. Logik -> Widget (Processed Image / Heatmap, rechte Seite)
    QObject::connect(&logic, &GeigerLogic::processedImage, &w, &GeigerWidget::updateProcessedImage);
    // 4. Logik -> UI Updates
    QObject::connect(&logic, &GeigerLogic::statsUpdated, &w, &GeigerWidget::updateStats);
    QObject::connect(&logic, &GeigerLogic::statsUpdated, &tray, &TrayGraphManager::updateIcon);
    QObject::connect(&logic, &GeigerLogic::requestClick, &audio, &AudioEngine::playClick);

    w.show();
    return app.exec();
}

#include "main.moc"

