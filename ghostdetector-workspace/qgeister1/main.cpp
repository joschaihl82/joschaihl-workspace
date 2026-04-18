/*
 * qrad.cpp - Qt 5 Version
 * Angepasst für Debian/Qt5 Kompatibilität
 */

#include <QApplication>
#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>
#include <QPainter>
#include <QPainterPath>
#include <QImage>
#include <QVideoWidget>
#include <QCamera>
#include <QCameraInfo> // Qt5 statt QMediaDevices
#include <QAudioOutput> // Qt5 statt QAudioSink
#include <QIODevice>
#include <QBuffer>
#include <QtMath>
#include <QDebug>
#include <QDateTime>
#include <QQueue>
#include <QResizeEvent>
#include <QRandomGenerator>

// --- Konfiguration ---
#define HISTORY_SIZE 24
#define UPDATE_INTERVAL 100
#define BASE_FREQ 44100

// --- Datenstrukturen ---
struct GeigerStats {
    double currentRad = 0.0;
    double totalDose = 0.0;
    bool alertState = false;
};

// --- Logik-Klasse (Backend) ---
class GeigerLogic : public QObject {
    Q_OBJECT
public:
    explicit GeigerLogic(QObject *parent = nullptr) : QObject(parent) {
        for(int i=0; i<HISTORY_SIZE; ++i) {
            radHistory.enqueue(0.0);
            doseHistory.enqueue(0.0);
        }
    }

    void processFrame(const QImage &img) {
        (void)img; // Unused in demo
        // Simulation
        double noise = (double)(QRandomGenerator::global()->generate() % 100) / 1000.0;
        double spike = (QRandomGenerator::global()->generate() % 100 < 5) ? 0.5 : 0.0;
        
        double newRad = noise + spike;
        
        currentRadSmooth = (currentRadSmooth * 0.8) + (newRad * 0.2);
        totalDoseAcc += currentRadSmooth * 0.01;

        if (radHistory.size() >= HISTORY_SIZE) radHistory.dequeue();
        if (doseHistory.size() >= HISTORY_SIZE) doseHistory.dequeue();
        
        radHistory.enqueue(currentRadSmooth);
        doseHistory.enqueue(totalDoseAcc);

        GeigerStats stats;
        stats.currentRad = currentRadSmooth;
        stats.totalDose = totalDoseAcc;
        stats.alertState = currentRadSmooth > 0.4;

        emit statsUpdated(stats);
        
        if (spike > 0.1) {
            emit requestClick(440.0 + (spike * 1000.0));
        }
    }

    QQueue<double> getRadHistory() const { return radHistory; }
    QQueue<double> getDoseHistory() const { return doseHistory; }

signals:
    void statsUpdated(GeigerStats stats);
    void requestClick(double freq);

private:
    double currentRadSmooth = 0.0;
    double totalDoseAcc = 0.0;
    QQueue<double> radHistory;
    QQueue<double> doseHistory;
};

// --- Audio Engine (Qt 5 Version) ---
class AudioEngine : public QObject {
    Q_OBJECT
public:
    explicit AudioEngine(QObject *parent = nullptr) : QObject(parent) {
        QAudioFormat format;
        format.setSampleRate(BASE_FREQ);
        format.setChannelCount(1);
        format.setSampleSize(16);       // Qt5 spezifisch
        format.setCodec("audio/pcm");   // Qt5 spezifisch
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::SignedInt);

        QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
        if (!info.isFormatSupported(format)) {
            format = info.nearestFormat(format);
        }

        // In Qt5 nutzt man QAudioOutput statt QAudioSink
        audioOutput = new QAudioOutput(format, this);
        audioDevice = audioOutput->start();
    }

public slots:
    void playClick(double freq) {
        if (!audioDevice) return;
        int durationMs = 50;
        int sampleCount = (BASE_FREQ * durationMs) / 1000;
        QByteArray data;
        data.resize(sampleCount * 2);
        qint16 *ptr = reinterpret_cast<qint16*>(data.data());
        
        for (int i = 0; i < sampleCount; ++i) {
            double t = (double)i / BASE_FREQ;
            double val = qSin(2 * M_PI * freq * t);
            val *= qExp(-t * 20); 
            ptr[i] = static_cast<qint16>(val * 10000);
        }
        audioDevice->write(data);
    }

private:
    QAudioOutput *audioOutput; // Qt5
    QIODevice *audioDevice;
};

// --- UI Widget ---
class GeigerWidget : public QWidget {
    Q_OBJECT
public:
    GeigerWidget(GeigerLogic *logic, QWidget *parent = nullptr) 
        : QWidget(parent), m_logic(logic) 
    {
        setWindowTitle("qrad Visualizer");
        resize(800, 600);
        setStyleSheet("background-color: #111; color: #0f0; font-family: Monospace;");
    }

public slots:
    void updateStats(GeigerStats stats) {
        m_stats = stats;
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        p.setPen(Qt::white);
        p.setFont(QFont("Monospace", 20));
        p.drawText(20, 40, QString("RAD: %1 Sv/h").arg(m_stats.currentRad, 0, 'f', 4));
        p.drawText(20, 80, QString("DOSE: %1 Sv").arg(m_stats.totalDose, 0, 'f', 4));

        if (m_stats.alertState) {
            p.fillRect(rect().adjusted(0, 100, 0, 0), QColor(255, 0, 0, 50));
            p.setPen(Qt::red);
            p.drawText(rect().center(), "WARNING: HIGH RADIATION");
        }
    }

private:
    GeigerLogic *m_logic;
    GeigerStats m_stats;
};

// --- Tray Icon Manager ---
class TrayGraphManager : public QObject {
    Q_OBJECT
public:
    TrayGraphManager(GeigerLogic *logic, QWidget *mainWindow, QObject *parent = nullptr)
        : QObject(parent), m_logic(logic), m_mainWindow(mainWindow) 
    {
        trayIcon = new QSystemTrayIcon(this);
        QMenu *menu = new QMenu();
        menu->addAction("Zeigen/Verstecken", [this]() {
            if (m_mainWindow->isVisible()) m_mainWindow->hide();
            else { m_mainWindow->showNormal(); m_mainWindow->activateWindow(); }
        });
        menu->addAction("Beenden", qApp, &QCoreApplication::quit);
        trayIcon->setContextMenu(menu);
        
        connect(trayIcon, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason) {
            if (reason == QSystemTrayIcon::DoubleClick) {
                if (m_mainWindow->isVisible()) m_mainWindow->hide();
                else m_mainWindow->showNormal();
            }
        });
        trayIcon->show();
    }

public slots:
    void updateIcon(GeigerStats stats) {
        (void)stats;
        int w = 24; int h = 24;
        QPixmap pix(w, h);
        pix.fill(Qt::transparent);
        
        QPainter p(&pix);
        p.setRenderHint(QPainter::Antialiasing, false);

        auto radHist = m_logic->getRadHistory();
        auto doseHist = m_logic->getDoseHistory();

        if (radHist.isEmpty()) return;

        // Graph A: Dose (Blau, Area)
        double minDose = 1e9, maxDose = -1e9;
        for(double d : doseHist) {
            if(d < minDose) minDose = d;
            if(d > maxDose) maxDose = d;
        }
        double rangeDose = maxDose - minDose;
        if (rangeDose < 0.00001) rangeDose = 1.0;

        QPainterPath dosePath;
        dosePath.moveTo(0, h);
        for (int i = 0; i < doseHist.size(); ++i) {
            double val = (doseHist[i] - minDose) / rangeDose;
            double y = h - (val * (h * 0.5));
            double x = (double)i / (HISTORY_SIZE - 1) * w;
            dosePath.lineTo(x, y);
        }
        dosePath.lineTo(w, h);
        dosePath.closeSubpath();
        p.fillPath(dosePath, QColor(0, 100, 255, 150));

        // Graph B: Rad (Grün/Rot, Line)
        double maxRad = 0.8;
        p.setPen(QPen(stats.alertState ? Qt::red : Qt::green, 2));
        QPainterPath radPath;
        bool first = true;
        for (int i = 0; i < radHist.size(); ++i) {
            double val = radHist[i] / maxRad;
            if (val > 1.0) val = 1.0;
            double y = h - (val * h);
            double x = (double)i / (HISTORY_SIZE - 1) * w;
            if (first) { radPath.moveTo(x, y); first = false; }
            else { radPath.lineTo(x, y); }
        }
        p.drawPath(radPath);
        
        // Rahmen
        p.setPen(QColor(100, 100, 100, 100));
        p.drawRect(0, 0, w-1, h-1);

        trayIcon->setIcon(QIcon(pix));
        trayIcon->setToolTip(QString("Rad: %1 | Dose: %2").arg(stats.currentRad).arg(stats.totalDose));
    }

private:
    QSystemTrayIcon *trayIcon;
    GeigerLogic *m_logic;
    QWidget *m_mainWindow;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    GeigerLogic logic;
    AudioEngine audio;
    
    GeigerWidget window(&logic);
    TrayGraphManager trayManager(&logic, &window);

    // --- Qt5 Kamera Initialisierung ---
    QCamera *camera = nullptr;
    // Nutze QCameraInfo statt QMediaDevices
    const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    if (!cameras.isEmpty()) {
        camera = new QCamera(cameras.first());
        camera->start();
        // Da wir hier keinen VideoOutput haben (headless logic),
        // muss man in Qt5 evtl. tricksen oder einen Dummy-Surface nutzen,
        // aber für die Demo reicht der Timer unten.
    }

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&logic]() {
        QImage dummy; 
        logic.processFrame(dummy); 
    });
    timer.start(UPDATE_INTERVAL);

    QObject::connect(&logic, &GeigerLogic::statsUpdated, &window, &GeigerWidget::updateStats);
    QObject::connect(&logic, &GeigerLogic::statsUpdated, &trayManager, &TrayGraphManager::updateIcon);
    QObject::connect(&logic, &GeigerLogic::requestClick, &audio, &AudioEngine::playClick);

    window.show();
    return app.exec();
}

#include "main.moc"
