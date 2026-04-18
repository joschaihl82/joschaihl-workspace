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
#include <QDateTime>
#include <QPalette> 
#include <QColor>   
#include <QResizeEvent> 
#include <QPen> 
#include <QListWidget> 
#include <QListWidgetItem> 

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
#include <cstdlib>
#include <ctime> 
#include <algorithm> 

// =======================================================================
// KONFIGURATION
// =======================================================================
namespace Config {
    const QString VIDEO_DEVICE = "/dev/video0";
    const int FRAME_WIDTH = 640;
    const int FRAME_HEIGHT = 480;
    const quint8 PIXEL_THRESHOLD = 200; 
    const double RAD_SCALING_FACTOR = 0.00005; 
    const double ALERT_THRESHOLD = 0.05; 
    const double ALPHA_MIN = 0.05;
    const double ALPHA_MAX = 0.50;
    const double AEMA_SCALE_FACTOR = 1.0;
    const int CAMERA_UPDATE_MS = 50; 
    const int ALARM_COOLDOWN_MS = 1000; 
    const int LCD_DECIMALS = 3; 
    const int DOSAGE_DECIMALS = 3; 
    
    // --- Akustik-Konfiguration für Eindeutigkeit und Gefahr ---
    const double MAX_INTENSITY_FOR_PITCH = 0.1;       
    const double MIN_PITCH_FREQ = 2500.0;             
    const double MAX_PITCH_FREQ = 15000.0;            
    
    // Basis-Pitch für eindeutige Isotope
    const double BASE_PITCH_HZ = 150.0;         
    const double ISOTOPE_FREQ_STEP_HZ = 12.0;   
    
    // ALARM KLICK KONFIGURATION 
    const double ALARM_FREQ = 1000.0; 
    const double ALARM_DURATION = 0.05; 

    // ALSA-Konfiguration
    const int SAMPLE_RATE = 44100;
    const int CHANNELS = 1;
    const double ALERT_VOLUME = 32000.0;
    const char* ALERT_PCM_DEVICE = "default";
    
    // Isotopen-Konfiguration (Wahrscheinlichkeit in %)
    const int HIGH_RISK_CHANCE_TOTAL = 5;  
    const int MEDIUM_RISK_CHANCE_TOTAL = 15; 
}

// =======================================================================
// KLASSENDEKLARATIONEN 
// =======================================================================

// --- Hilfsfunktion zur Wellenform-Generierung ---
static short* generate_click(double duration_sec, int rate, double amplitude, double freq, size_t *frames_out) {
    size_t n = (size_t)(duration_sec * rate);
    *frames_out = n;
    short *s = (short*)malloc(n * sizeof(short));
    if (!s) return nullptr;
    
    for (size_t i = 0; i < n; ++i) {
        double t = (double)i / rate;
        // Exponentialer Zerfall der Sinuswelle, um einen "Klick"-Sound zu simulieren
        s[i] = (short)(amplitude * std::sin(2.0 * M_PI * freq * t) * std::exp(-t * 500.0));
    }
    return s;
}


// --- 1. Audio ---
class AudioAlert : public QObject {
    Q_OBJECT
public:
    explicit AudioAlert(QObject *parent = nullptr) : QObject(parent) {}

signals:
    void logMessage(const QString& msg);

public slots:
    void playClick(double intensity, int isotopeId) {
        double normalizedIntensity = intensity / Config::MAX_INTENSITY_FOR_PITCH;
        if (normalizedIntensity > 1.0) normalizedIntensity = 1.0;
        
        double clickDuration;
        double riskFactor;
        double dangerPitchShift;
        
        // 1. Risikobasierte Einstellungen (Faktor und Dauer)
        // Je höher die Gefahr (ID), desto kürzer der Klick und höher der Basis-Shift
        if (isotopeId >= 14) { 
            riskFactor = 1.0; 
            clickDuration = 0.003; 
            dangerPitchShift = 300.0; // Hohe Gefahr: Basis-Ton + 300Hz
        } else if (isotopeId >= 7) { 
            riskFactor = 0.7; 
            clickDuration = 0.008;
            dangerPitchShift = 100.0; // Mittlere Gefahr: Basis-Ton + 100Hz
        } else { 
            riskFactor = 0.5;
            clickDuration = 0.01;
            dangerPitchShift = 0.0;   // Niedrige Gefahr: Kein Shift
        }
        
        // 2. Eindeutiger Basis-Ton (unterscheidet die 20 Isotope voneinander)
        // Jeder Isotop ID hat einen eindeutigen Grundton (150Hz + ID * 12Hz)
        double uniqueBaseFreq = Config::BASE_PITCH_HZ + (isotopeId * Config::ISOTOPE_FREQ_STEP_HZ);
        
        // 3. Intensitäts- und Risiko-Skalierung 
        // Die Frequenz wird hochmoduliert, basierend auf der Strahlungsintensität.
        double finalBaseFreq = uniqueBaseFreq + dangerPitchShift;
        double intensityRange = Config::MAX_PITCH_FREQ - finalBaseFreq; 
        if (intensityRange < 0) intensityRange = 1000.0; 

        // Die Endfrequenz kombiniert den eindeutigen Basiston, den Gefahren-Shift und die Intensitätsmodulation.
        double finalFreq = finalBaseFreq + (intensityRange * normalizedIntensity * riskFactor);
        
        playAlsaClickSound(finalFreq, clickDuration); 
    }

    void playAlarm(double intensity) {
        qDebug() << QString("☢️ ALARM! Strahlung %1 rad/m² überschreitet Schwellwert!").arg(intensity, 0, 'f', Config::LCD_DECIMALS);
        playAlsaClickSound(Config::ALARM_FREQ, Config::ALARM_DURATION); 
    }

private:
    void playAlsaClickSound(double freq, double duration) {
        snd_pcm_t *pcm = nullptr;
        snd_pcm_hw_params_t *params = (snd_pcm_hw_params_t *)alloca(snd_pcm_hw_params_sizeof());
        unsigned int rate = Config::SAMPLE_RATE, chans = Config::CHANNELS;
        size_t frames;

        short *buf = generate_click(duration, rate, Config::ALERT_VOLUME, freq, &frames);
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

// --- 2. V4L2 Kamera-Erfassung ---
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


// --- 3. Bildverarbeitung (Neon-Farben) ---
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
        
        const double MAX_BRIGHTNESS_RANGE = 255.0 - Config::PIXEL_THRESHOLD;

        for (int y = 0; y < Config::FRAME_HEIGHT; ++y) {
            for (int x = 0; x < Config::FRAME_WIDTH; x += 2) {
                int yuyv_index = (y * Config::FRAME_WIDTH + x) * 2;
                quint8 Y0 = yuyv[yuyv_index];
                quint8 Y1 = yuyv[yuyv_index + 2];
                
                // --- Pixel 0 ---
                if (Y0 >= Config::PIXEL_THRESHOLD) {
                    hotPixelCount++;
                    
                    double t = (Y0 - Config::PIXEL_THRESHOLD) / MAX_BRIGHTNESS_RANGE;
                    
                    int r = qMin(255, (int)(255 * t)); 
                    int g = qMin(100, (int)(100 * t)); 
                    int b = qMax(0, 255 - (int)(155 * t)); 
                    
                    image.setPixel(x, y, qRgb(r, g, b)); 
                } else {
                    image.setPixel(x, y, qRgb(Y0, Y0, Y0)); 
                }

                // --- Pixel 1 ---
                if (Y1 >= Config::PIXEL_THRESHOLD) {
                    hotPixelCount++;
                    
                    double t = (Y1 - Config::PIXEL_THRESHOLD) / MAX_BRIGHTNESS_RANGE;
                    
                    int r = qMin(255, (int)(255 * t)); 
                    int g = qMin(100, (int)(100 * t)); 
                    int b = qMax(0, 255 - (int)(155 * t)); 
                    
                    image.setPixel(x + 1, y, qRgb(r, g, b)); 
                } else {
                    image.setPixel(x + 1, y, qRgb(Y1, Y1, Y1)); 
                }
            }
        }
        emit imageReady(image, hotPixelCount);
    }
};

// --- 4. Dosimeter Logik (Isotopen-Simulation und Zählung) ---
class DosimeterLogic : public QObject {
    Q_OBJECT
public:
    explicit DosimeterLogic(QObject *parent = nullptr) : QObject(parent) {
        m_startTime = QDateTime::currentMSecsSinceEpoch();
        m_lastAlertTime = 0; 
        m_isRunning = false;
    }
signals:
    void radValueChanged(double rad);
    void dosageUpdated(double totalDosage);
    void alphaUpdated(double dynamicAlpha);
    void timeElapsedUpdated(qint64 ms); 
    void logMessage(const QString& message); 
    void alertSignal(double intensity);
    void playClick(double intensity, int isotopeId); 
    
    // Signal mit allen zwanzig Zählern
    void isotopeCountsUpdated(int k40, int c14, int h3, int ra226, int rn222, int u238, int po210, 
                              int cs137, int sr90, int i131, int tc99m, int ba137m, int cl36, int na22,
                              int pu239, int am241, int co60, int u235, int th232, int i129); 
    
public slots:
    void processCameraData(int hotPixelCount) {
        if (!m_isRunning || hotPixelCount == 0) return;
        
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
        
        // Vergangene Zeit aktualisieren
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        qint64 timeElapsed = currentTime - m_startTime;
        emit timeElapsedUpdated(timeElapsed);
        
        // --- Mehrstufige Isotopen-Simulation und Zählung (20 Typen) ---
        int isotopeId = 0; 
        int chance = std::rand() % 100;

        if (chance < Config::HIGH_RISK_CHANCE_TOTAL) {
            // Hohe Gefahr: 5% Chance (IDs 14-19, 6 Isotope)
            isotopeId = 14 + (std::rand() % 6); 
        } else if (chance < (Config::HIGH_RISK_CHANCE_TOTAL + Config::MEDIUM_RISK_CHANCE_TOTAL)) {
            // Mittlere Gefahr: 15% Chance (IDs 7-13, 7 Isotope)
            isotopeId = 7 + (std::rand() % 7); 
        } else {
            // Niedrige Gefahr: 80% Chance (IDs 0-6, 7 Isotope)
            isotopeId = std::rand() % 7; 
        }
        
        // Inkrementieren des entsprechenden Zählers
        switch (isotopeId) {
            case 0: m_k40Count++; break;
            case 1: m_c14Count++; break;
            case 2: m_h3Count++; break;
            case 3: m_ra226Count++; break;
            case 4: m_rn222Count++; break;
            case 5: m_u238Count++; break;   
            case 6: m_po210Count++; break;  
            case 7: m_cs137Count++; break;
            case 8: m_sr90Count++; break;
            case 9: m_i131Count++; break;
            case 10: m_tc99mCount++; break;
            case 11: m_ba137mCount++; break;
            case 12: m_cl36Count++; break;  
            case 13: m_na22Count++; break;  
            case 14: m_pu239Count++; break;
            case 15: m_am241Count++; break;
            case 16: m_co60Count++; break;
            case 17: m_u235Count++; break;
            case 18: m_th232Count++; break;
            case 19: m_i129Count++; break;  
        }

        emit isotopeCountsUpdated(m_k40Count, m_c14Count, m_h3Count, m_ra226Count, m_rn222Count, m_u238Count, m_po210Count,
                                  m_cs137Count, m_sr90Count, m_i131Count, m_tc99mCount, m_ba137mCount, m_cl36Count, m_na22Count,
                                  m_pu239Count, m_am241Count, m_co60Count, m_u235Count, m_th232Count, m_i129Count);
                                  
        emit playClick(radValue, isotopeId);
        // ------------------------------------

        if (radValue > Config::ALERT_THRESHOLD) {
            if (currentTime - m_lastAlertTime >= Config::ALARM_COOLDOWN_MS) { 
                emit alertSignal(radValue); 
                m_lastAlertTime = currentTime; 
            }
        }
    }
    
    void startMeasurement() { m_isRunning = true; emit logMessage("Messung gestartet."); }
    void stopMeasurement() { m_isRunning = false; emit logMessage("Messung gestoppt."); }
    bool isRunning() const { return m_isRunning; }
private:
    bool m_isRunning; 
    double m_accumulatedDosage = 0.0;
    double m_filteredRadValue = 0.0;
    double m_dynamicAlpha = Config::ALPHA_MIN;
    qint64 m_startTime;
    qint64 m_lastAlertTime; 
    
    // Zähler für ALLE zwanzig Isotope
    int m_k40Count = 0;
    int m_c14Count = 0;
    int m_h3Count = 0;
    int m_ra226Count = 0;
    int m_rn222Count = 0;
    int m_u238Count = 0;
    int m_po210Count = 0;
    int m_cs137Count = 0;
    int m_sr90Count = 0;
    int m_i131Count = 0;
    int m_tc99mCount = 0;
    int m_ba137mCount = 0;
    int m_cl36Count = 0;
    int m_na22Count = 0;
    int m_pu239Count = 0;
    int m_am241Count = 0;
    int m_co60Count = 0;
    int m_u235Count = 0;
    int m_th232Count = 0;
    int m_i129Count = 0;
};

// --- 5. Hauptfenster ---
class GeigerMainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit GeigerMainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("☢️ Qt Dosimeter Konsole (Vollbild Overlay) ☢️");
        
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);
        showFullScreen(); 
        
        setupUI();
        setupClassesAndConnections();
        
        m_logic->startMeasurement(); 
        m_camera->startCapture(); 
    }

private slots:
    void updateMainRadDisplay(double rad) { 
        m_radValueDisplay->display(QString::number(rad, 'f', Config::LCD_DECIMALS)); 
    }
    
    void updateDosageDisplay(double dosage) { 
        // ANPASSUNG: Schriftgröße im b-Tag etwas kleiner halten, um Platz zu sparen
        m_dosageLabel->setText(
            QString("Gesamtdosis: <b style='float:right; font-size: 16pt;'>%1 rad*s</b>").arg(
                QString("%1").arg(dosage, 0, 'f', Config::DOSAGE_DECIMALS)
            )
        ); 
    }
    
    void updateAlphaDisplay(double alpha) { 
        m_alphaLabel->setText(
            QString("Filter-Alpha: <b style='float:right;'>%1</b>").arg(
                QString("%1").arg(alpha, 0, 'f', 2)
            )
        ); 
    }
    
    // Slot zur Anzeige der vergangenen Zeit
    void updateTimeElapsedDisplay(qint64 ms) {
        qint64 totalSeconds = ms / 1000;
        qint64 totalMinutes = totalSeconds / 60;
        qint64 totalHours = totalMinutes / 60;
        qint64 totalDays = totalHours / 24;

        qint64 currentMS = ms % 1000;
        qint64 currentSeconds = totalSeconds % 60;
        qint64 currentMinutes = totalMinutes % 60;
        qint64 currentHours = totalHours % 24;
        
        QString timeString = QString("%1:%2:%3:%4:%5")
            .arg(totalDays, 2, 10, QChar('0'))
            .arg(currentHours, 2, 10, QChar('0'))
            .arg(currentMinutes, 2, 10, QChar('0'))
            .arg(currentSeconds, 2, 10, QChar('0'))
            .arg(currentMS, 3, 10, QChar('0'));
            
        m_timeElapsedLabel->setText(
            QString("Verstrichen: <b style='float:right;'>%1</b>").arg(timeString)
        );
    }
    
    void updateVideoAndCount(const QImage& image, int hotPixelCount) {
        QPixmap pixmap = QPixmap::fromImage(image); 
        QPixmap scaledPixmap = pixmap.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);
        
        m_videoLabel->setAlignment(Qt::AlignCenter);
        m_videoLabel->setPixmap(scaledPixmap);
        
        m_hotPixelLabel->setText(QString("Hot Pixels: <b style='float:right;'>%1</b>").arg(hotPixelCount));
    }

    // Slot zur Aktualisierung der Isotopen-Zähler
    void updateIsotopeCounters(int k40, int c14, int h3, int ra226, int rn222, int u238, int po210, 
                               int cs137, int sr90, int i131, int tc99m, int ba137m, int cl36, int na22,
                               int pu239, int am241, int co60, int u235, int th232, int i129) {
        
        // Hohe Gefahr (6 Isotope) - Oben
        m_pu239CountLabel->setText(
            QString("<span style='color: #FF00FF;'>☢️ Pu-239 Hits:</span> <b style='float:right;'>%1</b>").arg(pu239));
        m_am241CountLabel->setText(
            QString("<span style='color: #FF00FF;'>☢️ Am-241 Hits:</span> <b style='float:right;'>%1</b>").arg(am241));
        m_co60CountLabel->setText(
            QString("<span style='color: #FF00FF;'>☢️ Co-60 Hits:</span> <b style='float:right;'>%1</b>").arg(co60));
        m_u235CountLabel->setText(
            QString("<span style='color: #FF00FF;'>☢️ U-235 Hits:</span> <b style='float:right;'>%1</b>").arg(u235));
        m_th232CountLabel->setText(
            QString("<span style='color: #FF00FF;'>☢️ Th-232 Hits:</span> <b style='float:right;'>%1</b>").arg(th232));
        m_i129CountLabel->setText(
            QString("<span style='color: #FF00FF;'>☢️ I-129 Hits:</span> <b style='float:right;'>%1</b>").arg(i129));

        // Mittlere Gefahr (7 Isotope) - Mitte
        m_cs137CountLabel->setText(
            QString("<span style='color: #FF9900;'>⚠️ Cs-137 Hits:</span> <b style='float:right;'>%1</b>").arg(cs137));
        m_sr90CountLabel->setText(
            QString("<span style='color: #FF9900;'>⚠️ Sr-90 Hits:</span> <b style='float:right;'>%1</b>").arg(sr90));
        m_i131CountLabel->setText(
            QString("<span style='color: #FF9900;'>⚠️ I-131 Hits:</span> <b style='float:right;'>%1</b>").arg(i131));
        m_tc99mCountLabel->setText(
            QString("<span style='color: #FF9900;'>⚠️ Tc-99m Hits:</span> <b style='float:right;'>%1</b>").arg(tc99m));
        m_ba137mCountLabel->setText(
            QString("<span style='color: #FF9900;'>⚠️ Ba-137m Hits:</span> <b style='float:right;'>%1</b>").arg(ba137m));
        m_cl36CountLabel->setText(
            QString("<span style='color: #FF9900;'>⚠️ Cl-36 Hits:</span> <b style='float:right;'>%1</b>").arg(cl36));
        m_na22CountLabel->setText(
            QString("<span style='color: #FF9900;'>⚠️ Na-22 Hits:</span> <b style='float:right;'>%1</b>").arg(na22));

        // Niedrige Gefahr (7 Isotope) - Unten
        m_k40CountLabel->setText(
            QString("<span style='color: #CCCC00;'>🟢 K-40 Hits:</span> <b style='float:right;'>%1</b>").arg(k40));
        m_c14CountLabel->setText(
            QString("<span style='color: #CCCC00;'>🟢 C-14 Hits:</span> <b style='float:right;'>%1</b>").arg(c14));
        m_h3CountLabel->setText(
            QString("<span style='color: #CCCC00;'>🟢 H-3 Hits:</span> <b style='float:right;'>%1</b>").arg(h3));
        m_ra226CountLabel->setText(
            QString("<span style='color: #CCCC00;'>🟢 Ra-226 Hits:</span> <b style='float:right;'>%1</b>").arg(ra226));
        m_rn222CountLabel->setText(
            QString("<span style='color: #CCCC00;'>🟢 Rn-222 Hits:</span> <b style='float:right;'>%1</b>").arg(rn222));
        m_u238CountLabel->setText(
            QString("<span style='color: #CCCC00;'>🟢 U-238 Hits:</span> <b style='float:right;'>%1</b>").arg(u238));
        m_po210CountLabel->setText(
            QString("<span style='color: #CCCC00;'>🟢 Po-210 Hits:</span> <b style='float:right;'>%1</b>").arg(po210));

        // Debug-Ausgabe der Hochrisiko-Zähler
        if (pu239 + am241 + co60 + u235 + th232 + i129 > 0) {
            qDebug() << QString("☢️ High Risk HITS (Gesamt: %1)").arg(pu239 + am241 + co60 + u235 + th232 + i129);
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
    QLabel *m_timeElapsedLabel; 
    QLabel *m_videoLabel; 
    QWidget *m_overlayWidget = nullptr; 
    
    // Labels für die 20 Isotopen-Zählung
    QLabel *m_k40CountLabel;
    QLabel *m_c14CountLabel; 
    QLabel *m_h3CountLabel;
    QLabel *m_ra226CountLabel;
    QLabel *m_rn222CountLabel;
    QLabel *m_u238CountLabel;
    QLabel *m_po210CountLabel;
    QLabel *m_cs137CountLabel;
    QLabel *m_sr90CountLabel; 
    QLabel *m_i131CountLabel;
    QLabel *m_tc99mCountLabel;
    QLabel *m_ba137mCountLabel;
    QLabel *m_cl36CountLabel;
    QLabel *m_na22CountLabel;
    QLabel *m_pu239CountLabel;
    QLabel *m_am241CountLabel; 
    QLabel *m_co60CountLabel;
    QLabel *m_u235CountLabel;
    QLabel *m_th232CountLabel;
    QLabel *m_i129CountLabel;

    void setupUI() {
        m_videoLabel = new QLabel; 
        m_videoLabel->setAlignment(Qt::AlignCenter);
        m_videoLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        m_videoLabel->setScaledContents(true); 
        setCentralWidget(m_videoLabel); 

        m_overlayWidget = new QWidget(this); 
        m_overlayWidget->setAutoFillBackground(true);
        QPalette overlayPalette = m_overlayWidget->palette();
        overlayPalette.setColor(QPalette::Window, QColor(0, 0, 0, 0)); 
        m_overlayWidget->setPalette(overlayPalette);

        // Haupt-Layout ist nun ausschließlich VERTICAL
        QVBoxLayout *mainLayout = new QVBoxLayout(m_overlayWidget);
        mainLayout->setContentsMargins(10, 10, 10, 10);
        
        // --- TOP LAYOUT (LCD, Info und Rechtes Panel) ---
        QHBoxLayout *topLayout = new QHBoxLayout;
        
        // Linkes Panel (LCD und Info)
        QWidget *infoPanelWidget = new QWidget;
        // ANPASSUNG: Schriftgröße auf 14pt (statt 20pt)
        infoPanelWidget->setStyleSheet("background-color: rgba(30, 30, 30, 100); color: white; padding: 10px; border-radius: 5px; font-size: 14pt;");
        QVBoxLayout *infoPanel = new QVBoxLayout(infoPanelWidget);
        
        // ANPASSUNG: Überschrift auf <h4> statt <h3>, um Platz zu sparen
        QLabel *titleLabel = new QLabel("<h4>Aktuelle Strahlung (rad/m²)</h4>");
        titleLabel->setStyleSheet("color: white;");
        
        m_radValueDisplay = new QLCDNumber; m_radValueDisplay->setSegmentStyle(QLCDNumber::Flat);
        // LCD-Anzeige behält die Größe, wird aber im Layout-Stretch gestützt
        m_radValueDisplay->setStyleSheet("color: lime; background-color: rgba(0, 0, 0, 0); border: none; font-size: 20pt;"); 
        m_radValueDisplay->setDigitCount(2 + 1 + Config::LCD_DECIMALS); 
        
        // Die Labels behalten 14pt, was im Stylesheet des infoPanelWidget definiert ist.
        m_hotPixelLabel = new QLabel(QString("Hot Pixels: <b style='float:right;'>%1</b>").arg(0));
        m_dosageLabel = new QLabel(QString("Gesamtdosis: <b style='float:right; font-size: 16pt;'>%1 rad*s</b>").arg(0.0, 0, 'f', Config::DOSAGE_DECIMALS)); 
        m_alphaLabel = new QLabel(QString("Filter-Alpha: <b style='float:right;'>%1</b>").arg(0.05, 0, 'f', 2));
        m_timeElapsedLabel = new QLabel(QString("Verstrichen: <b style='float:right;'>00:00:00:00:000</b>")); 

        infoPanel->addWidget(titleLabel);
        // ANPASSUNG: Setzen des LCDs mit einem Stretch-Faktor, um es vertikal zu komprimieren, falls nötig
        infoPanel->addWidget(m_radValueDisplay); 
        infoPanel->addWidget(m_hotPixelLabel); 
        infoPanel->addWidget(m_dosageLabel); 
        infoPanel->addWidget(m_alphaLabel); 
        infoPanel->addWidget(m_timeElapsedLabel); 
        infoPanel->addStretch(1); 
        
        topLayout->addWidget(infoPanelWidget, 1);
        topLayout->addStretch(1); 
        
        // Rechtes Panel (Isotopen Zähler)
        QWidget *isotopePanelWidget = new QWidget;
        // ANPASSUNG: Schriftgröße auf 10pt (statt 14pt)
        isotopePanelWidget->setStyleSheet("background-color: rgba(30, 30, 30, 100); color: white; padding: 5px; border-radius: 5px; font-size: 10pt;");
        QVBoxLayout *isotopePanel = new QVBoxLayout(isotopePanelWidget);
        
        QLabel *isotopeTitle = new QLabel("<h4>Isotopen-Zählrate (Gesamt)</h4>");
        isotopeTitle->setStyleSheet("color: white;");
        
        // Initialisierung der Zähler-Labels (20 Labels)
        m_k40CountLabel = new QLabel;
        m_c14CountLabel = new QLabel; 
        m_h3CountLabel = new QLabel;
        m_ra226CountLabel = new QLabel;
        m_rn222CountLabel = new QLabel;
        m_u238CountLabel = new QLabel;
        m_po210CountLabel = new QLabel;
        m_cs137CountLabel = new QLabel;
        m_sr90CountLabel = new QLabel; 
        m_i131CountLabel = new QLabel;
        m_tc99mCountLabel = new QLabel;
        m_ba137mCountLabel = new QLabel;
        m_cl36CountLabel = new QLabel;
        m_na22CountLabel = new QLabel;
        m_pu239CountLabel = new QLabel;
        m_am241CountLabel = new QLabel;
        m_co60CountLabel = new QLabel;
        m_u235CountLabel = new QLabel;
        m_th232CountLabel = new QLabel;
        m_i129CountLabel = new QLabel;
        
        // Setze initiale Werte 
        updateIsotopeCounters(0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,0,0); 
        
        isotopePanel->addWidget(isotopeTitle);
        
        // Hohe Gefahr (6 Isotope) - Oben
        isotopePanel->addWidget(m_pu239CountLabel);
        isotopePanel->addWidget(m_am241CountLabel);
        isotopePanel->addWidget(m_co60CountLabel);
        isotopePanel->addWidget(m_u235CountLabel);
        isotopePanel->addWidget(m_th232CountLabel);
        isotopePanel->addWidget(m_i129CountLabel);
        isotopePanel->addWidget(new QWidget); // Trennlinie / Spacer
        
        // Mittlere Gefahr (7 Isotope) - Mitte
        isotopePanel->addWidget(m_cs137CountLabel);
        isotopePanel->addWidget(m_sr90CountLabel);
        isotopePanel->addWidget(m_i131CountLabel);
        isotopePanel->addWidget(m_tc99mCountLabel);
        isotopePanel->addWidget(m_ba137mCountLabel);
        isotopePanel->addWidget(m_cl36CountLabel);
        isotopePanel->addWidget(m_na22CountLabel);
        isotopePanel->addWidget(new QWidget); // Trennlinie / Spacer
        
        // Niedrige Gefahr (7 Isotope) - Unten
        isotopePanel->addWidget(m_k40CountLabel);
        isotopePanel->addWidget(m_c14CountLabel);
        isotopePanel->addWidget(m_h3CountLabel);
        isotopePanel->addWidget(m_ra226CountLabel);
        isotopePanel->addWidget(m_rn222CountLabel);
        isotopePanel->addWidget(m_u238CountLabel);
        isotopePanel->addWidget(m_po210CountLabel);
        
        
        topLayout->addWidget(isotopePanelWidget, 1); 
        
        mainLayout->addLayout(topLayout);
        mainLayout->addStretch(1); 

        m_overlayWidget->setGeometry(0, 0, width(), height()); 
        m_overlayWidget->show();
    }

    void setupClassesAndConnections() {
        m_logic = new DosimeterLogic(this);
        m_processor = new ImageProcessor(this);
        m_audioAlert = new AudioAlert(this);
        m_camera = new CameraCapturer();
        m_camera->moveToThread(m_camera);
        m_camera->setParent(nullptr); 

        connect(m_camera, &CameraCapturer::frameReady, m_processor, &ImageProcessor::processFrame);
        connect(m_processor, &ImageProcessor::imageReady, [this](const QImage& img, int count) {
            updateVideoAndCount(img, count);
            m_logic->processCameraData(count);
        });

        connect(m_logic, &DosimeterLogic::radValueChanged, this, &GeigerMainWindow::updateMainRadDisplay);
        connect(m_logic, &DosimeterLogic::dosageUpdated, this, &GeigerMainWindow::updateDosageDisplay);
        connect(m_logic, &DosimeterLogic::alphaUpdated, this, &GeigerMainWindow::updateAlphaDisplay);
        connect(m_logic, &DosimeterLogic::timeElapsedUpdated, this, &GeigerMainWindow::updateTimeElapsedDisplay);
        
        // Verbindung für das Audio-Feedback mit Isotopen-ID
        connect(m_logic, QOverload<double, int>::of(&DosimeterLogic::playClick), 
                m_audioAlert, QOverload<double, int>::of(&AudioAlert::playClick));
        connect(m_logic, &DosimeterLogic::alertSignal, m_audioAlert, &AudioAlert::playAlarm);
        
        // Verbindung für die Zähler-Aktualisierung (mit 20 Parametern)
        connect(m_logic, &DosimeterLogic::isotopeCountsUpdated, this, 
                &GeigerMainWindow::updateIsotopeCounters);
        
        // Debug-Ausgaben (qDebug)
        connect(m_audioAlert, &AudioAlert::logMessage, [](const QString& msg){ qDebug() << "Audio Log:" << msg; });
        connect(m_camera, &CameraCapturer::logMessage, [](const QString& msg){ qDebug() << "Camera Log:" << msg; });
        connect(m_camera, &CameraCapturer::captureError, [](const QString& msg){ qDebug() << "Camera Error:" << msg; });
        connect(m_logic, &DosimeterLogic::logMessage, [](const QString& msg){ qDebug() << "Logic Log:" << msg; });
    }
    
    void resizeEvent(QResizeEvent* event) override {
        QMainWindow::resizeEvent(event);
        if (m_overlayWidget) {
            m_overlayWidget->setGeometry(0, 0, event->size().width(), event->size().height());
        }
    }
};

// =======================================================================
// MAIN-FUNKTION
// =======================================================================

int main(int argc, char *argv[]) {
    std::srand(std::time(nullptr)); 
    
    QApplication a(argc, argv);
    GeigerMainWindow w;
    return a.exec();
}

#include "geiger.moc"
