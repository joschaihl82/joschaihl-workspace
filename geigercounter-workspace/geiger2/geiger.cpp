#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QThread>
#include <QMutex>
#include <QTimer>
#include <QDebug>
#include <QLCDNumber>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QAudioOutput> // Qt Multimedia für Audio-Feedback
#include <QMediaPlayer>
#include <QBuffer>
#include <QDateTime>

// Standard C/System Header
#include <cmath>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <alsa/asoundlib.h> // Direkte ALSA-Nutzung als Fallback für Klicks

// Qt Charts Namespace
QT_CHARTS_USE_NAMESPACE

// =======================================================================
// KONFIGURATION
// =======================================================================
namespace Config {
    const QString VIDEO_DEVICE = "/dev/video0";
    const int FRAME_WIDTH = 640;
    const int FRAME_HEIGHT = 480;
    const quint8 PIXEL_THRESHOLD = 200; // Helligkeitsschwelle für "Hot Pixel"
    const double RAD_SCALING_FACTOR = 0.01;
    const double ALERT_THRESHOLD = 3.0; // Rad/m²
    const int HISTORY_POINTS = 150;
    const double ALPHA_MIN = 0.05;
    const double ALPHA_MAX = 0.50;
    const double AEMA_SCALE_FACTOR = 1.0;
    const int CAMERA_UPDATE_MS = 50; // 20 FPS
}

// =======================================================================
// KLASSENDEKLARATIONEN (Innere Klassen)
// =======================================================================

// --- 1. Audio ---
class AudioAlert : public QObject {
    Q_OBJECT
public:
    explicit AudioAlert(QObject *parent = nullptr) : QObject(parent) {
        // Generiere einen kurzen Klick-Sound im RAM
        generateClickSound();
    }

signals:
    void logMessage(const QString& msg);

public slots:
    // Slot für Klicks (jede Messung)
    void playClick() {
        if (m_clickBuffer.size() > 0) {
            m_clickPlayer.setMedia(QMediaContent(), &m_clickBuffer);
            m_clickPlayer.play();
        }
    }

    // Slot für Alarme (über Schwellwert)
    void playAlarm(double intensity) {
        // Implementierung des Alarms (z.B. längerer Ton oder Frequenzmodulation)
        // Hier wird nur ein Log-Eintrag gemacht, da komplexere Audio-Logik
        // den Code unnötig aufblähen würde. Echte Alarmtöne müssten hier generiert werden.
        emit logMessage(QString("☢️ ALARM! Strahlung %.2f rad/m² überschreitet Schwellwert!").arg(intensity));
    }

private:
    QMediaPlayer m_clickPlayer;
    QBuffer m_clickBuffer;

    void generateClickSound() {
        // Vereinfachte Generierung eines 10ms 10kHz Klicks (Raw 16-bit PCM)
        const int SAMPLE_RATE = 44100;
        const double DURATION = 0.01; // 10 ms
        const double FREQUENCY = 10000.0;
        const int AMPLITUDE = 32000;
        
        QByteArray pcmData;
        int numSamples = SAMPLE_RATE * DURATION;
        pcmData.reserve(numSamples * sizeof(qint16));

        for (int i = 0; i < numSamples; ++i) {
            double t = (double)i / SAMPLE_RATE;
            qint16 sample = (qint16)(AMPLITUDE * std::sin(2.0 * M_PI * FREQUENCY * t) * std::exp(-t * 500.0));
            pcmData.append((char*)&sample, sizeof(qint16));
        }

        m_clickBuffer.setData(pcmData);
        m_clickBuffer.open(QIODevice::ReadOnly);
        m_clickPlayer.setVolume(80); 
    }
};

// --- 2. V4L2 Kamera-Erfassung (läuft in eigenem Thread) ---
class CameraCapturer : public QThread {
    Q_OBJECT
public:
    explicit CameraCapturer(QObject *parent = nullptr) : QThread(parent), m_fd(-1), m_mem(nullptr) {}
    ~CameraCapturer() override {
        requestInterruption();
        quit();
        wait();
        cleanupV4L2();
    }

signals:
    void logMessage(const QString& msg);
    void frameReady(QByteArray rawFrameData); // Rohdaten (YUYV)
    void captureError(const QString& error);

public slots:
    void startCapture() {
        if (m_fd < 0) {
            if (initV4L2() < 0) {
                emit captureError("V4L2 Initialisierung fehlgeschlagen.");
                return;
            }
        }
        if (!isRunning()) start();
    }

protected:
    void run() override {
        emit logMessage("Kamera-Thread gestartet.");
        while (!isInterruptionRequested()) {
            if (captureFrame()) {
                // Sende die rohen Frame-Daten (Kopie)
                emit frameReady(m_currentFrame);
            }
            // Verzögerung basierend auf der gewünschten FPS
            QThread::msleep(Config::CAMERA_UPDATE_MS);
        }
        emit logMessage("Kamera-Thread beendet.");
    }

private:
    int m_fd;
    void *m_mem;
    struct v4l2_buffer m_buf;
    QByteArray m_currentFrame;

    int initV4L2() {
        // ... V4L2 Initialisierungslogik (wie in C-Version) ...
        m_fd = open(Config::VIDEO_DEVICE.toLocal8Bit().constData(), O_RDWR | O_NONBLOCK, 0);
        if (m_fd == -1) {
            emit logMessage(QString("Fehler beim Öffnen von %1: %2").arg(Config::VIDEO_DEVICE).arg(strerror(errno)));
            return -1;
        }
        // [V4L2 format, requestbuffers, querybuf, mmap, qbuf Logik]
        struct v4l2_format fmt = {0};
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; fmt.fmt.pix.width = Config::FRAME_WIDTH;
        fmt.fmt.pix.height = Config::FRAME_HEIGHT; fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        if (ioctl(m_fd, VIDIOC_S_FMT, &fmt) == -1) {
             emit logMessage("VIDIOC_S_FMT Fehler"); cleanupV4L2(); return -1;
        }
        struct v4l2_requestbuffers req = {0}; req.count = 1; req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_MMAP;
        if (ioctl(m_fd, VIDIOC_REQBUFS, &req) == -1) { emit logMessage("VIDIOC_REQBUFS Fehler"); cleanupV4L2(); return -1; }
        m_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; m_buf.memory = V4L2_MEMORY_MMAP; m_buf.index = 0;
        if (ioctl(m_fd, VIDIOC_QUERYBUF, &m_buf) == -1) { emit logMessage("VIDIOC_QUERYBUF Fehler"); cleanupV4L2(); return -1; }
        m_mem = mmap(NULL, m_buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, m_buf.m.offset);
        if (m_mem == MAP_FAILED) { emit logMessage("mmap Fehler"); cleanupV4L2(); return -1; }
        if (ioctl(m_fd, VIDIOC_QBUF, &m_buf) == -1) { emit logMessage("VIDIOC_QBUF Fehler"); cleanupV4L2(); return -1; }
        enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl(m_fd, VIDIOC_STREAMON, &type) == -1) { emit logMessage("VIDIOC_STREAMON Fehler"); cleanupV4L2(); return -1; }

        emit logMessage("V4L2 Initialisierung erfolgreich.");
        return 0;
    }

    void cleanupV4L2() {
        if (m_fd > 0) {
            enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            ioctl(m_fd, VIDIOC_STREAMOFF, &type);
            if (m_mem != MAP_FAILED && m_mem != nullptr) {
                munmap(m_mem, m_buf.length);
            }
            close(m_fd);
            m_fd = -1;
            m_mem = nullptr;
            emit logMessage("V4L2 aufgeräumt.");
        }
    }

    bool captureFrame() {
        if (ioctl(m_fd, VIDIOC_DQBUF, &m_buf) == -1) {
            if (errno != EAGAIN) {
                emit logMessage(QString("VIDIOC_DQBUF Fehler: %1").arg(strerror(errno)));
            }
            return false;
        }

        m_currentFrame = QByteArray((const char*)m_mem, m_buf.length);
        
        ioctl(m_fd, VIDIOC_QBUF, &m_buf);
        return true;
    }
};

// --- 3. Bildverarbeitung ---
class ImageProcessor : public QObject {
    Q_OBJECT
public:
    explicit ImageProcessor(QObject *parent = nullptr) : QObject(parent) {}

signals:
    void imageReady(const QImage& image, int hotPixelCount);
    void logMessage(const QString& msg);

public slots:
    void processFrame(QByteArray rawFrameData) {
        int hotPixelCount = 0;
        QImage image(Config::FRAME_WIDTH, Config::FRAME_HEIGHT, QImage::Format_RGB32);
        
        const quint8* yuyv = reinterpret_cast<const quint8*>(rawFrameData.constData());
        
        // YUYV -> RGB32 Konvertierung mit Hot Pixel Hervorhebung
        for (int y = 0; y < Config::FRAME_HEIGHT; ++y) {
            for (int x = 0; x < Config::FRAME_WIDTH; x += 2) {
                // YUYV-Daten sind Y0 U Y1 V
                int yuyv_index = (y * Config::FRAME_WIDTH + x) * 2;
                quint8 Y0 = yuyv[yuyv_index];
                quint8 Y1 = yuyv[yuyv_index + 2];
                
                // Pixel 0
                if (Y0 >= Config::PIXEL_THRESHOLD) {
                    image.setPixel(x, y, qRgb(255, 0, 0)); // ROT Hervorhebung
                    hotPixelCount++;
                } else {
                    image.setPixel(x, y, qRgb(Y0, Y0, Y0)); // Graustufe (nur Y-Komponente)
                }

                // Pixel 1
                if (Y1 >= Config::PIXEL_THRESHOLD) {
                    image.setPixel(x + 1, y, qRgb(255, 0, 0)); // ROT Hervorhebung
                    hotPixelCount++;
                } else {
                    image.setPixel(x + 1, y, qRgb(Y1, Y1, Y1)); // Graustufe
                }
            }
        }
        
        emit imageReady(image, hotPixelCount);
    }
};

// --- 4. Dosimeter Logik ---
class DosimeterLogic : public QObject {
    Q_OBJECT
public:
    explicit DosimeterLogic(QObject *parent = nullptr) : QObject(parent) {
        m_startTime = QDateTime::currentMSecsSinceEpoch();
    }

signals:
    void radValueChanged(double rad);
    void dosageUpdated(double totalDosage);
    void alphaUpdated(double dynamicAlpha);
    void historyPoint(double rad);
    void logMessage(const QString& message);
    void alertSignal(double intensity);
    void playClick();

public slots:
    void processCameraData(int hotPixelCount) {
        if (!m_isRunning) return;
        
        // 1. Messwertberechnung
        double measuredRadValue = (double)hotPixelCount * Config::RAD_SCALING_FACTOR;
        if (measuredRadValue < 0.001) measuredRadValue = 0.001; 

        // 2. Adaptive EMA Filterung
        double radValue;
        if (m_filteredRadValue == 0.0) {
            radValue = measuredRadValue;
            m_dynamicAlpha = Config::ALPHA_MIN;
        } else {
            double diff = std::fabs(measuredRadValue - m_filteredRadValue);
            double alphaRange = Config::ALPHA_MAX - Config::ALPHA_MIN;
            double changeRate = diff / Config::AEMA_SCALE_FACTOR;
            if (changeRate > 1.0) changeRate = 1.0; 

            m_dynamicAlpha = Config::ALPHA_MIN + (alphaRange * changeRate);

            radValue = (measuredRadValue * m_dynamicAlpha) + (m_filteredRadValue * (1.0 - m_dynamicAlpha));
        }

        // 3. Zustand aktualisieren
        m_filteredRadValue = radValue;
        m_accumulatedDosage += radValue * (Config::CAMERA_UPDATE_MS / 1000.0); // Rad * Zeit = Dosis
        
        // 4. Signale senden
        emit radValueChanged(radValue);
        emit dosageUpdated(m_accumulatedDosage);
        emit alphaUpdated(m_dynamicAlpha);
        emit historyPoint(radValue);
        emit playClick();

        // 5. Alarmprüfung
        if (radValue > Config::ALERT_THRESHOLD) {
            emit alertSignal(radValue);
        }
    }
    
    void startMeasurement() { m_isRunning = true; emit logMessage("Messung gestartet."); }
    void stopMeasurement() { m_isRunning = false; emit logMessage("Messung gestoppt."); }

    bool isRunning() const { return m_isRunning; }

private:
    bool m_isRunning = false;
    double m_accumulatedDosage = 0.0;
    double m_filteredRadValue = 0.0;
    double m_dynamicAlpha = Config::ALPHA_MIN;
    qint64 m_startTime;
};

// --- 5. Hauptfenster ---
class GeigerMainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit GeigerMainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("☢️ Qt Dosimeter Konsole ☢️");
        setupUI();
        setupClassesAndConnections();
    }

private slots:
    void onStartStopClicked() {
        if (m_logic->isRunning()) {
            m_logic->stopMeasurement();
            m_camera->requestInterruption();
            m_startStopButton->setText("Starten");
            m_statusLabel->setText("Status: Gestoppt");
        } else {
            m_logic->startMeasurement();
            m_camera->startCapture();
            m_startStopButton->setText("Stoppen");
            m_statusLabel->setText("Status: Messung aktiv");
        }
    }

    void updateMainRadDisplay(double rad) {
        m_radValueDisplay->display(rad);
    }

    void updateDosageDisplay(double dosage) {
        m_dosageLabel->setText(QString("Gesamtdosis: <b>%.3f rad*s</b>").arg(dosage));
    }

    void updateAlphaDisplay(double alpha) {
        m_alphaLabel->setText(QString("Filter-Alpha: <b>%.2f</b>").arg(alpha));
    }

    void updateVideoAndCount(const QImage& image, int hotPixelCount) {
        // Skaliere Bild auf Label-Größe für bessere Ansicht
        QPixmap pixmap = QPixmap::fromImage(image.scaled(m_videoLabel->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
        m_videoLabel->setPixmap(pixmap);
        m_hotPixelLabel->setText(QString("Hot Pixels: <b>%d</b>").arg(hotPixelCount));
    }

    void addHistoryPoint(double rad) {
        qreal x = m_series->points().count();
        m_series->append(x, rad);
        
        // Schiebe das Diagramm, wenn die maximale Punktanzahl erreicht ist
        if (m_series->points().count() > Config::HISTORY_POINTS) {
            m_series->remove(0);
            m_chart->scroll(m_axisX->tickInterval(), 0); 
            // Workaround für die Verschiebung der X-Achse
            m_axisX->setMin(m_series->points().first().x());
            m_axisX->setMax(m_series->points().last().x() + 1);
        }
    }

private:
    // Core Klassen
    DosimeterLogic *m_logic;
    CameraCapturer *m_camera;
    ImageProcessor *m_processor;
    AudioAlert *m_audioAlert;
    
    // UI Komponenten
    QLCDNumber *m_radValueDisplay;
    QLabel *m_hotPixelLabel;
    QLabel *m_dosageLabel;
    QLabel *m_alphaLabel;
    QLabel *m_statusLabel;
    QPushButton *m_startStopButton;
    QTextEdit *m_logConsole;
    QLabel *m_videoLabel; // Für Video-Vorschau
    
    // Chart Komponenten
    QChartView *m_chartView;
    QSplineSeries *m_series;
    QChart *m_chart;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY;

    void setupUI() {
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        // --- Haupt-Layout ---
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        QHBoxLayout *topLayout = new QHBoxLayout;
        
        // 1. (Links) Hauptanzeige (Rad Value)
        m_radValueDisplay = new QLCDNumber;
        m_radValueDisplay->setSegmentStyle(QLCDNumber::Flat);
        m_radValueDisplay->setStyleSheet("color: darkgreen; background-color: #f0f0f0;");
        m_radValueDisplay->setDigitCount(5);
        m_radValueDisplay->setMinimumHeight(150);
        
        QVBoxLayout *infoPanel = new QVBoxLayout;
        infoPanel->addWidget(new QLabel("<h2>Aktuelle Strahlung (rad/m²)</h2>"));
        infoPanel->addWidget(m_radValueDisplay);
        
        m_hotPixelLabel = new QLabel("Hot Pixels: <b>0</b>");
        m_dosageLabel = new QLabel("Gesamtdosis: <b>0.000 rad*s</b>");
        m_alphaLabel = new QLabel("Filter-Alpha: <b>0.05</b>");
        
        infoPanel->addWidget(m_hotPixelLabel);
        infoPanel->addWidget(m_dosageLabel);
        infoPanel->addWidget(m_alphaLabel);
        infoPanel->addStretch(1);
        
        topLayout->addLayout(infoPanel);
        
        // 2. (Rechts) Video-Vorschau
        m_videoLabel = new QLabel("Video-Vorschau (640x480)");
        m_videoLabel->setAlignment(Qt::AlignCenter);
        m_videoLabel->setFixedSize(640, 480);
        m_videoLabel->setStyleSheet("border: 1px solid gray; background-color: black; color: lightgray;");
        topLayout->addWidget(m_videoLabel);
        
        mainLayout->addLayout(topLayout);

        // --- Historie (Diagramm) ---
        m_series = new QSplineSeries();
        m_series->setName("Rad/m²");

        m_chart = new QChart();
        m_chart->legend()->hide();
        m_chart->addSeries(m_series);
        m_chart->setTitle("Strahlungsverlauf (Letzte 150 Messungen)");

        m_axisX = new QValueAxis;
        m_axisX->setRange(0, Config::HISTORY_POINTS);
        m_axisX->setLabelFormat("%d");
        m_axisX->setTitleText("Zeitpunkt (Frames)");
        m_chart->addAxis(m_axisX, Qt::AlignBottom);
        m_series->attachAxis(m_axisX);

        m_axisY = new QValueAxis;
        m_axisY->setRange(0, 5.0); // Max Rad/m²
        m_axisY->setTitleText("Rad/m²");
        m_chart->addAxis(m_axisY, Qt::AlignLeft);
        m_series->attachAxis(m_axisY);

        m_chartView = new QChartView(m_chart);
        m_chartView->setRenderHint(QPainter::Antialiasing);
        m_chartView->setMinimumHeight(250);
        mainLayout->addWidget(m_chartView);

        // --- Steuerung und Log ---
        QHBoxLayout *bottomLayout = new QHBoxLayout;
        
        m_startStopButton = new QPushButton("Starten");
        m_startStopButton->setCheckable(true);
        m_startStopButton->setMinimumHeight(40);
        m_statusLabel = new QLabel("Status: Bereit");
        
        bottomLayout->addWidget(m_startStopButton);
        bottomLayout->addWidget(m_statusLabel);
        
        mainLayout->addLayout(bottomLayout);
        
        m_logConsole = new QTextEdit;
        m_logConsole->setReadOnly(true);
        m_logConsole->setMaximumHeight(100);
        mainLayout->addWidget(new QLabel("System Log:"));
        mainLayout->addWidget(m_logConsole);
    }

    void setupClassesAndConnections() {
        // Instanziierung aller Klassen
        m_logic = new DosimeterLogic(this);
        m_processor = new ImageProcessor(this);
        m_audioAlert = new AudioAlert(this);
        // CameraCapturer muss in seinen eigenen Thread verschoben werden,
        // da er eine QThread-Basisklasse ist.
        m_camera = new CameraCapturer();
        m_camera->moveToThread(m_camera);
        m_camera->setParent(nullptr); // QObject Parent-Hierarchie trennen, da es im eigenen Thread läuft

        // 1. Steuerung (UI -> Logic)
        connect(m_startStopButton, &QPushButton::clicked, this, &GeigerMainWindow::onStartStopClicked);

        // 2. Logik-Kette (Camera -> Processor -> Logic)
        connect(m_camera, &CameraCapturer::frameReady, m_processor, &ImageProcessor::processFrame);
        connect(m_processor, &ImageProcessor::imageReady, [this](const QImage& img, int count) {
            updateVideoAndCount(img, count);
            m_logic->processCameraData(count);
        });

        // 3. UI-Updates (Logic -> UI)
        connect(m_logic, &DosimeterLogic::radValueChanged, this, &GeigerMainWindow::updateMainRadDisplay);
        connect(m_logic, &DosimeterLogic::dosageUpdated, this, &GeigerMainWindow::updateDosageDisplay);
        connect(m_logic, &DosimeterLogic::alphaUpdated, this, &GeigerMainWindow::updateAlphaDisplay);
        connect(m_logic, &DosimeterLogic::historyPoint, this, &GeigerMainWindow::addHistoryPoint);
        
        // 4. Audio & Logging
        connect(m_logic, &DosimeterLogic::playClick, m_audioAlert, &AudioAlert::playClick);
        connect(m_logic, &DosimeterLogic::alertSignal, m_audioAlert, &AudioAlert::playAlarm);
        
        connect(m_audioAlert, &AudioAlert::logMessage, m_logConsole, &QTextEdit::append);
        connect(m_camera, &CameraCapturer::logMessage, m_logConsole, &QTextEdit::append);
        connect(m_camera, &CameraCapturer::captureError, m_logConsole, &QTextEdit::append);
    }
};

// =======================================================================
// MAIN-FUNKTION
// =======================================================================

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    // Qt-Charts müssen zuerst initialisiert werden, bevor sie in Widgets verwendet werden
    // (normalerweise in einer separaten Header-Datei, hier direkt eingebunden)

    GeigerMainWindow w;
    w.show();

    return a.exec();
}

#include "geiger.moc" // Notwendig für den MOC-Prozess


