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
#include <QDateTime>

// Standard C/System Header
#include <cmath>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <alsa/asoundlib.h> 
#include <alloca.h>      
#include <cstring>       

// Qt Charts Namespace
QT_CHARTS_USE_NAMESPACE

// =======================================================================
// KONFIGURATION
// =======================================================================
namespace Config {
    const QString VIDEO_DEVICE = "/dev/video0";
    const int FRAME_WIDTH = 640;
    const int FRAME_HEIGHT = 480;
    const quint8 PIXEL_THRESHOLD = 200; 
    // Skalierungsfaktor für realistische Werte
    const double RAD_SCALING_FACTOR = 0.00005; 
    const double ALERT_THRESHOLD = 0.05; 
    const int HISTORY_POINTS = 150; 
    const double ALPHA_MIN = 0.05;
    const double ALPHA_MAX = 0.50;
    const double AEMA_SCALE_FACTOR = 1.0;
    const int CAMERA_UPDATE_MS = 50; 
    const int ALARM_COOLDOWN_MS = 1000; // Alarm max. 1x pro Sekunde
    const int LCD_DECIMALS = 3; // Dezimalstellen für Haupt-LCD
    const int DOSAGE_DECIMALS = 6; // Dezimalstellen für Gesamtdosis

    // ALSA-Konfiguration
    const int SAMPLE_RATE = 44100;
    const int CHANNELS = 1;
    const double ALERT_VOLUME = 32000.0;
    const char* ALERT_PCM_DEVICE = "default";
}

// =======================================================================
// KLASSENDEKLARATIONEN (Innere Klassen)
// =======================================================================

// --- Hilfsfunktion zur Wellenform-Generierung ---
static short* generate_click(double duration_sec, int rate, double amplitude, size_t *frames_out) {
    size_t n = (size_t)(duration_sec * rate);
    *frames_out = n;
    short *s = (short*)malloc(n * sizeof(short));
    if (!s) return nullptr;
    double freq = 10000.0;
    for (size_t i = 0; i < n; ++i) {
        double t = (double)i / rate;
        s[i] = (short)(amplitude * std::sin(2.0 * M_PI * freq * t) * std::exp(-t * 500.0));
    }
    return s;
}


// --- 1. Audio (mit direkter ALSA-Klick-Logik) ---
class AudioAlert : public QObject {
    Q_OBJECT
public:
    explicit AudioAlert(QObject *parent = nullptr) : QObject(parent) {}

signals:
    void logMessage(const QString& msg);

public slots:
    void playClick() {
        playAlsaClickSound();
    }

    void playAlarm(double intensity) {
        emit logMessage(QString("☢️ ALARM! Strahlung %1 rad/m² überschreitet Schwellwert!").arg(intensity, 0, 'f', Config::LCD_DECIMALS));
    }

private:
    void playAlsaClickSound() {
        snd_pcm_t *pcm = nullptr;
        snd_pcm_hw_params_t *params = (snd_pcm_hw_params_t *)alloca(snd_pcm_hw_params_sizeof());
        unsigned int rate = Config::SAMPLE_RATE, chans = Config::CHANNELS;
        double duration = 0.01; 
        size_t frames;

        short *buf = generate_click(duration, rate, Config::ALERT_VOLUME, &frames);
        if (!buf) return; 

        if (snd_pcm_open(&pcm, Config::ALERT_PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0) < 0) {
            free(buf); 
            return;
        }

        snd_pcm_hw_params_any(pcm, params);
        snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
        snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_S16_LE);
        snd_pcm_hw_params_set_channels(pcm, params, chans);
        snd_pcm_hw_params_set_rate_near(pcm, params, &rate, nullptr);
        snd_pcm_hw_params_set_period_size_near(pcm, params, (snd_pcm_uframes_t *)&frames, nullptr);

        if (snd_pcm_hw_params(pcm, params) < 0) {
            snd_pcm_close(pcm); free(buf); return;
        }

        snd_pcm_writei(pcm, buf, frames);
        snd_pcm_drain(pcm);
        
        snd_pcm_close(pcm);
        free(buf);
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
    void frameReady(QByteArray rawFrameData); 
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
                emit frameReady(m_currentFrame);
            }
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
        m_fd = open(Config::VIDEO_DEVICE.toLocal8Bit().constData(), O_RDWR | O_NONBLOCK, 0);
        if (m_fd == -1) {
            emit logMessage(QString("Fehler beim Öffnen von %1: %2").arg(Config::VIDEO_DEVICE).arg(strerror(errno)));
            return -1;
        }
        
        struct v4l2_format fmt = {};
        memset(&fmt, 0, sizeof(fmt));
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
        fmt.fmt.pix.width = Config::FRAME_WIDTH;
        fmt.fmt.pix.height = Config::FRAME_HEIGHT; 
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        if (ioctl(m_fd, VIDIOC_S_FMT, &fmt) == -1) {
             emit logMessage("VIDIOC_S_FMT Fehler"); cleanupV4L2(); return -1;
        }
        
        struct v4l2_requestbuffers req = {}; 
        memset(&req, 0, sizeof(req));
        req.count = 1; 
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_MMAP;
        if (ioctl(m_fd, VIDIOC_REQBUFS, &req) == -1) { emit logMessage("VIDIOC_REQBUFS Fehler"); cleanupV4L2(); return -1; }
        
        m_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; m_buf.memory = V4L2_MEMORY_MMAP; m_buf.index = 0;
        if (ioctl(m_fd, VIDIOC_QUERYBUF, &m_buf) == -1) { emit logMessage("VIDIOC_QUERYBUF Fehler"); cleanupV4L2(); return -1; }
        m_mem = mmap(nullptr, m_buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, m_buf.m.offset);
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
        }
    }

    bool captureFrame() {
        if (ioctl(m_fd, VIDIOC_DQBUF, &m_buf) == -1) {
            if (errno != EAGAIN) {
                emit logMessage(QString("VIDIOC_DQBUF Fehler: %1").arg(strerror(errno)));
            }
            return false;
        }
        m_currentFrame = QByteArray(reinterpret_cast<const char*>(m_mem), static_cast<int>(m_buf.length));
        
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
        
        for (int y = 0; y < Config::FRAME_HEIGHT; ++y) {
            for (int x = 0; x < Config::FRAME_WIDTH; x += 2) {
                int yuyv_index = (y * Config::FRAME_WIDTH + x) * 2;
                quint8 Y0 = yuyv[yuyv_index];
                quint8 Y1 = yuyv[yuyv_index + 2];
                
                // Pixel 0
                if (Y0 >= Config::PIXEL_THRESHOLD) {
                    image.setPixel(x, y, qRgb(255, 0, 0)); 
                    hotPixelCount++;
                } else {
                    image.setPixel(x, y, qRgb(Y0, Y0, Y0)); 
                }

                // Pixel 1
                if (Y1 >= Config::PIXEL_THRESHOLD) {
                    image.setPixel(x + 1, y, qRgb(255, 0, 0)); 
                    hotPixelCount++;
                } else {
                    image.setPixel(x + 1, y, qRgb(Y1, Y1, Y1)); 
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
        m_lastAlertTime = 0; 
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
        
        double measuredRadValue = (double)hotPixelCount * Config::RAD_SCALING_FACTOR;
        if (measuredRadValue < 0.00001) measuredRadValue = 0.00001; 
        
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

        m_filteredRadValue = radValue;
        m_accumulatedDosage += radValue * (Config::CAMERA_UPDATE_MS / 1000.0);
        
        emit radValueChanged(radValue);
        emit dosageUpdated(m_accumulatedDosage);
        emit alphaUpdated(m_dynamicAlpha);
        emit historyPoint(radValue);
        emit playClick();

        // Bugfix: Alarm-Cooldown Logik korrigiert
        if (radValue > Config::ALERT_THRESHOLD) {
            qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
            if (currentTime - m_lastAlertTime >= Config::ALARM_COOLDOWN_MS) { // >= ist wichtig
                emit alertSignal(radValue);
                m_lastAlertTime = currentTime;
            }
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
    qint64 m_lastAlertTime; 
};

// --- 5. Hauptfenster ---
class GeigerMainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit GeigerMainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("☢️ Qt Dosimeter Konsole (ALSA-Klick) ☢️");
        setupUI();
        setupClassesAndConnections();
        
        onStartStopClicked(); 
    }

private slots:
    void onStartStopClicked() {
        if (m_logic->isRunning()) {
            m_logic->stopMeasurement();
            m_camera->requestInterruption();
            m_startStopButton->setText("Starten");
            m_statusLabel->setText("Status: Gestoppt");
        } else {
            m_startStopButton->setChecked(true); 
            m_logic->startMeasurement();
            m_camera->startCapture();
            m_startStopButton->setText("Stoppen");
            m_statusLabel->setText("Status: Messung aktiv");
        }
    }

    // Bugfix: Begrenzung auf konfigurierte Dezimalstellen
    void updateMainRadDisplay(double rad) { 
        m_radValueDisplay->display(QString::number(rad, 'f', Config::LCD_DECIMALS)); 
    }
    
    // Bugfix: Korrekte Formatierung mit konfigurierter Dezimalstellenanzahl
    void updateDosageDisplay(double dosage) { 
        // Schönheitsfix: Einheit rechtsbündig
        m_dosageLabel->setText(
            QString("Gesamtdosis: <b>%1 rad*s</b>").arg(
                QString("%1").arg(dosage, 0, 'f', Config::DOSAGE_DECIMALS)
            )
        ); 
    }
    
    // Bugfix: Korrekte Formatierung 
    void updateAlphaDisplay(double alpha) { 
         // Schönheitsfix: Einheit rechtsbündig
        m_alphaLabel->setText(
            QString("Filter-Alpha: <b>%1</b>").arg(
                QString("%1").arg(alpha, 0, 'f', 2)
            )
        ); 
    }
    
    void updateVideoAndCount(const QImage& image, int hotPixelCount) {
        QPixmap pixmap = QPixmap::fromImage(image); 
        QPixmap scaledPixmap = pixmap.scaled(m_videoLabel->size(), Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);
        
        // Schönheitsfix: Zentriert das Bild, falls es kleiner ist als das Label
        m_videoLabel->setAlignment(Qt::AlignCenter);
        m_videoLabel->setPixmap(scaledPixmap);
        
        m_hotPixelLabel->setText(QString("Hot Pixels: <b>%1</b>").arg(hotPixelCount));
    }

    void addHistoryPoint(double rad) {
        qreal x = m_series->points().count();
        m_series->append(x, rad);
        
        if (m_series->points().count() > Config::HISTORY_POINTS) {
            m_axisX->setMin(qMax(0.0, x - Config::HISTORY_POINTS + 1));
            m_axisX->setMax(x + 1);
        } else {
            m_axisX->setMax(Config::HISTORY_POINTS);
        }

        if (m_series->points().count() > 1) {
            double minY = m_series->points().constFirst().y();
            double maxY = m_series->points().constFirst().y();
            
            int startIndex = qMax(0, m_series->points().count() - Config::HISTORY_POINTS);
            for (int i = startIndex; i < m_series->points().count(); ++i) {
                const QPointF &point = m_series->points().at(i);
                if (point.y() < minY) minY = point.y();
                if (point.y() > maxY) maxY = point.y();
            }

            double range = maxY - minY;
            if (range < 0.0001) range = 0.0001; 

            m_axisY->setMin(qMax(0.0, minY - range * 0.2)); 
            m_axisY->setMax(maxY + range * 0.2);           
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
    QLabel *m_videoLabel; 
    
    // Chart Komponenten
    QChartView *m_chartView; 
    QSplineSeries *m_series;
    QChart *m_chart;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY;

    void setupUI() {
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        QHBoxLayout *topLayout = new QHBoxLayout;
        
        m_radValueDisplay = new QLCDNumber; m_radValueDisplay->setSegmentStyle(QLCDNumber::Flat);
        m_radValueDisplay->setStyleSheet("color: darkgreen; background-color: #f0f0f0;");
        m_radValueDisplay->setDigitCount(2 + 1 + Config::LCD_DECIMALS); // 2 Vorkommastellen + Punkt + Dezimalstellen
        m_radValueDisplay->setMinimumHeight(150);
        
        QVBoxLayout *infoPanel = new QVBoxLayout;
        QLabel *titleLabel = new QLabel("<h2>Aktuelle Strahlung (rad/m²)</h2>");
        titleLabel->adjustSize(); 
        infoPanel->addWidget(titleLabel);
        infoPanel->addWidget(m_radValueDisplay);
        
        // Schönheitsfix: Setzt die Textausrichtung auf links, um die Werte zu trennen
        m_hotPixelLabel = new QLabel(QString("Hot Pixels: <b>%1</b>").arg(0));
        m_dosageLabel = new QLabel(QString("Gesamtdosis: <b>%1 rad*s</b>").arg(0.0, 0, 'f', Config::DOSAGE_DECIMALS)); 
        m_alphaLabel = new QLabel(QString("Filter-Alpha: <b>%1</b>").arg(0.05, 0, 'f', 2));

        infoPanel->addWidget(m_hotPixelLabel); infoPanel->addWidget(m_dosageLabel); infoPanel->addWidget(m_alphaLabel);
        infoPanel->addStretch(1); topLayout->addLayout(infoPanel);
        
        m_videoLabel = new QLabel("Video-Vorschau (640x480)"); 
        m_videoLabel->setAlignment(Qt::AlignCenter);
        // Schönheitsfix: Größeres Seitenverhältnis, um den Platz besser zu nutzen
        m_videoLabel->setFixedSize(400, 300); 
        m_videoLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_videoLabel->setStyleSheet("border: 1px solid gray; background-color: black; color: lightgray;");
        topLayout->addWidget(m_videoLabel); mainLayout->addLayout(topLayout);

        m_series = new QSplineSeries(); m_series->setName("Rad/m²");
        m_chart = new QChart(); m_chart->legend()->hide(); m_chart->addSeries(m_series);
        m_chart->setTitle("Strahlungsverlauf (Alle Messungen gespeichert)");

        m_axisX = new QValueAxis; 
        m_axisX->setRange(0, Config::HISTORY_POINTS); 
        m_axisX->setLabelFormat("%d"); // Bugfix: Sorgt dafür, dass die X-Achse Dezimalzahlen (Frames) ausgibt
        m_axisX->setTitleText("Zeitpunkt (Frames)"); m_chart->addAxis(m_axisX, Qt::AlignBottom);
        m_series->attachAxis(m_axisX);

        m_axisY = new QValueAxis; 
        m_axisY->setRange(0, Config::ALERT_THRESHOLD + 0.01); 
        m_axisY->setLabelFormat("%.4f"); // Bugfix: Erzwingt die Ausgabe als saubere Dezimalzahl
        m_axisY->setTitleText("Rad/m²"); // Schönheitsfix: Setzt den Achsentitel vertikal
        m_chart->addAxis(m_axisY, Qt::AlignLeft); m_series->attachAxis(m_axisY);

        m_chartView = new QChartView(m_chart); 
        m_chartView->setRenderHint(QPainter::Antialiasing);
        m_chartView->setMinimumHeight(250); mainLayout->addWidget(m_chartView);

        QHBoxLayout *bottomLayout = new QHBoxLayout;
        m_startStopButton = new QPushButton("Starten"); m_startStopButton->setCheckable(true);
        m_startStopButton->setMinimumHeight(40);
        m_statusLabel = new QLabel("Status: Bereit");
        
        bottomLayout->addWidget(m_startStopButton); bottomLayout->addWidget(m_statusLabel);
        mainLayout->addLayout(bottomLayout);
        
        m_logConsole = new QTextEdit; m_logConsole->setReadOnly(true); m_logConsole->setMaximumHeight(100);
        mainLayout->addWidget(new QLabel("System Log:")); mainLayout->addWidget(m_logConsole);
    }

    void setupClassesAndConnections() {
        m_logic = new DosimeterLogic(this);
        m_processor = new ImageProcessor(this);
        m_audioAlert = new AudioAlert(this);
        m_camera = new CameraCapturer();
        m_camera->moveToThread(m_camera);
        m_camera->setParent(nullptr); 

        connect(m_startStopButton, &QPushButton::clicked, this, &GeigerMainWindow::onStartStopClicked);

        connect(m_camera, &CameraCapturer::frameReady, m_processor, &ImageProcessor::processFrame);
        connect(m_processor, &ImageProcessor::imageReady, [this](const QImage& img, int count) {
            updateVideoAndCount(img, count);
            m_logic->processCameraData(count);
        });

        connect(m_logic, &DosimeterLogic::radValueChanged, this, &GeigerMainWindow::updateMainRadDisplay);
        connect(m_logic, &DosimeterLogic::dosageUpdated, this, &GeigerMainWindow::updateDosageDisplay);
        connect(m_logic, &DosimeterLogic::alphaUpdated, this, &GeigerMainWindow::updateAlphaDisplay);
        connect(m_logic, &DosimeterLogic::historyPoint, this, &GeigerMainWindow::addHistoryPoint);
        
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
    GeigerMainWindow w;
    w.show();

    return a.exec();
}

#include "geiger.moc"


