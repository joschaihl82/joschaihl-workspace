// ghost_qt6.cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QGroupBox>
#include <QSlider>
#include <QMutex>
#include <QMediaDevices>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoSink>
#include <QVideoFrame>
#include <random>
#include <numeric>
#include <cmath>

// --- Konstanten ---
const int IMAGE_SIZE = 128;
const int LIVE_PATTERNS_PER_FRAME = 50; // reduziert für Live-Performance

// Matrix als 1D-Array row-major
using MatF = QVector<float>;

inline float matAt(const MatF &m, int w, int x, int y) { return m[y * w + x]; }
inline void matSet(MatF &m, int w, int x, int y, float v) { m[y * w + x] = v; }

// Führe 3x3 Faltung (Laplacian) mit Border-Replicate aus
static void convolve3x3_rep(const MatF &src, MatF &dst, int w, int h, const float kernel[3][3]) {
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float acc = 0.0f;
            for (int ky = -1; ky <= 1; ++ky) {
                int yy = qBound(0, y + ky, h - 1);
                for (int kx = -1; kx <= 1; ++kx) {
                    int xx = qBound(0, x + kx, w - 1);
                    acc += matAt(src, w, xx, yy) * kernel[ky + 1][kx + 1];
                }
            }
            matSet(dst, w, x, y, acc);
        }
    }
}

// Erzeuge zufälliges binäres Muster (0 oder 1)
static void randomBinaryPattern(MatF &out, int w, int h, std::mt19937 &rng) {
    std::uniform_real_distribution<float> d(0.0f, 1.0f);
    int n = w * h;
    out.resize(n);
    for (int i = 0; i < n; ++i) out[i] = (d(rng) > 0.5f) ? 1.0f : 0.0f;
}

// Konvertiere QImage (RGB) -> grayscale MatF (float 0..255)
static void imageToGrayMatF(const QImage &img, MatF &out, int targetW, int targetH) {
    if (img.isNull()) { out.fill(0.0f); return; }
    QImage scaled = img.scaled(targetW, targetH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).convertToFormat(QImage::Format_RGB888);
    out.resize(targetW * targetH);
    const uchar *bits = scaled.constBits();
    int stride = scaled.bytesPerLine();
    for (int y = 0; y < targetH; ++y) {
        const uchar *row = bits + y * stride;
        for (int x = 0; x < targetW; ++x) {
            int idx = x * 3;
            float r = row[idx + 0];
            float g = row[idx + 1];
            float b = row[idx + 2];
            out[y * targetW + x] = 0.299f * r + 0.587f * g + 0.114f * b;
        }
    }
}

// Konvertiere MatF (float 0..255) zu QImage Format_Grayscale8
static QImage matFToGrayImage(const MatF &m, int w, int h) {
    QImage img(w, h, QImage::Format_Grayscale8);
    for (int y = 0; y < h; ++y) {
        uchar *scan = img.scanLine(y);
        for (int x = 0; x < w; ++x) {
            float v = m[y * w + x];
            int vi = qBound(0, (int)std::round(v), 255);
            scan[x] = static_cast<uchar>(vi);
        }
    }
    return img;
}

// Hauptklasse
class GhostImagingSimulator : public QWidget {
    Q_OBJECT
public:
    GhostImagingSimulator(QWidget *parent = nullptr) : QWidget(parent),
        camera(nullptr),
        captureSession(new QMediaCaptureSession(this)),
        videoSink(new QVideoSink(this)),
        timer(new QTimer(this)),
        m_reconstructionGainDb(0.0f),
        rng(std::random_device{}())
    {
        setWindowTitle("Implicit Ghost Imaging (Qt6 only) mit V2K Gain");
        setupUI();
        createLaplacianKernel();

        // Kamera initialisieren: erste verfügbare Kamera
        const auto cams = QMediaDevices::videoInputs();
        if (!cams.isEmpty()) {
            camera = new QCamera(cams.first(), this);
            captureSession->setCamera(camera);
            captureSession->setVideoSink(videoSink);
            connect(videoSink, &QVideoSink::videoFrameChanged, this, &GhostImagingSimulator::onVideoFrame);
            camera->start();
            statusLabel->setText("Kamera gestartet.");
        } else {
            statusLabel->setText("FEHLER: Keine Kamera gefunden.");
        }

        connect(timer, &QTimer::timeout, this, &GhostImagingSimulator::updateFrameAndReconstruct);
    }

    ~GhostImagingSimulator() {
        if (camera) {
            camera->stop();
            delete camera;
        }
    }

private:
    QCamera *camera;
    QMediaCaptureSession *captureSession;
    QVideoSink *videoSink;
    QTimer *timer;
    QMutex frameMutex;
    QImage lastFrame;

    MatF objectFloat;
    MatF laplacianKernel;

    QLabel *videoLabel;
    QLabel *objectLabel;
    QLabel *resultLabel;
    QLabel *statusLabel;

    float m_reconstructionGainDb;
    QSlider *m_v2kAmpSlider;
    QLabel *m_v2kAmpValueLabel;

    std::mt19937 rng;

    void setupUI() {
        QVBoxLayout *layout = new QVBoxLayout(this);

        videoLabel = new QLabel("Videoquelle");
        videoLabel->setAlignment(Qt::AlignCenter);
        videoLabel->setMinimumSize(256, 180);
        videoLabel->setStyleSheet("border: 1px solid #AAA; background-color: black; color: white;");
        layout->addWidget(videoLabel);

        objectLabel = new QLabel("GI-Objekt (T) - Skalierter Graustufen-Input");
        objectLabel->setAlignment(Qt::AlignCenter);
        objectLabel->setMinimumSize(256, 256);
        layout->addWidget(objectLabel);

        resultLabel = new QLabel("Rekonstruiertes Bild (Implicit Filtering)");
        resultLabel->setAlignment(Qt::AlignCenter);
        resultLabel->setMinimumSize(256, 256);
        layout->addWidget(resultLabel);

        m_v2kAmpSlider = new QSlider(Qt::Horizontal);
        m_v2kAmpSlider->setRange(-300, 300);
        m_v2kAmpSlider->setValue(0);
        m_v2kAmpValueLabel = new QLabel("0.0 dB");

        QGroupBox *ampBox = new QGroupBox("V2K GI Verstärker (Rekonstruktions-Gain)");
        QVBoxLayout *ampLayout = new QVBoxLayout;
        QHBoxLayout *h = new QHBoxLayout;
        h->addWidget(new QLabel("Bildverstärkung (dB):"));
        h->addWidget(m_v2kAmpSlider);
        m_v2kAmpValueLabel->setFixedWidth(60);
        h->addWidget(m_v2kAmpValueLabel);
        ampLayout->addLayout(h);
        ampBox->setLayout(ampLayout);
        layout->addWidget(ampBox);

        statusLabel = new QLabel("Initialisiere...");
        layout->addWidget(statusLabel);

        QPushButton *runButton = new QPushButton("Starte Live Ghost Imaging (ca. 50 Muster/Frame)");
        connect(runButton, &QPushButton::clicked, this, &GhostImagingSimulator::startStopLive);
        layout->addWidget(runButton);

        connect(m_v2kAmpSlider, &QSlider::valueChanged, this, &GhostImagingSimulator::v2kAmpChanged);

        setLayout(layout);
        resize(500, 950);
    }

    void createLaplacianKernel() {
        laplacianKernel.resize(9);
        laplacianKernel[0] = 0.0f; laplacianKernel[1] = 1.0f; laplacianKernel[2] = 0.0f;
        laplacianKernel[3] = 1.0f; laplacianKernel[4] = -4.0f; laplacianKernel[5] = 1.0f;
        laplacianKernel[6] = 0.0f; laplacianKernel[7] = 1.0f; laplacianKernel[8] = 0.0f;
    }

private slots:
    void onVideoFrame(const QVideoFrame &frame) {
        // QVideoFrame::toImage() ist in Qt6 vorhanden (kann leer zurückgeben)
        QVideoFrame f(frame);
        QImage img = f.toImage();
        if (!img.isNull()) {
            QMutexLocker locker(&frameMutex);
            lastFrame = img.copy();
        }
    }

    void startStopLive() {
        QPushButton *button = qobject_cast<QPushButton*>(sender());
        if (!button) return;

        if (timer->isActive()) {
            timer->stop();
            statusLabel->setText("Live-Verarbeitung gestoppt.");
            button->setText("Starte Live Ghost Imaging (ca. 50 Muster/Frame)");
        } else {
            timer->start(33); // ~30 FPS
            button->setText("Stoppe Live Ghost Imaging");
            statusLabel->setText("Live-Verarbeitung gestartet...");
        }
    }

    void v2kAmpChanged(int v) {
        float db = v / 10.0f;
        m_reconstructionGainDb = db;
        m_v2kAmpValueLabel->setText(QString::number(db, 'f', 1) + " dB");
    }

    void updateFrameAndReconstruct() {
        QImage frameImg;
        {
            QMutexLocker locker(&frameMutex);
            frameImg = lastFrame.copy();
        }

        if (frameImg.isNull()) {
            statusLabel->setText("Warte auf Kameraframes...");
            return;
        }

        // Zeige rohen Input-Frame an (skaliert)
        videoLabel->setPixmap(QPixmap::fromImage(frameImg.scaled(256, 256, Qt::KeepAspectRatio)));

        // Konvertiere zu Graustufen und skaliere auf IMAGE_SIZE
        imageToGrayMatF(frameImg, objectFloat, IMAGE_SIZE, IMAGE_SIZE);

        // Zeige GI-Objekt (skaliere für Ansicht)
        QImage objImg = matFToGrayImage(objectFloat, IMAGE_SIZE, IMAGE_SIZE);
        objectLabel->setPixmap(QPixmap::fromImage(objImg.scaled(256, 256, Qt::KeepAspectRatio)));

        // --- Rekonstruktion ---
        MatF reconstructed(IMAGE_SIZE * IMAGE_SIZE);
        std::fill(reconstructed.begin(), reconstructed.end(), 0.0f);
        std::vector<double> bucketSignals;
        bucketSignals.reserve(LIVE_PATTERNS_PER_FRAME);

        MatF P_i, P_prime_i;
        float kernel3[3][3] = {
            {0.0f, 1.0f, 0.0f},
            {1.0f, -4.0f, 1.0f},
            {0.0f, 1.0f, 0.0f}
        };

        int n = IMAGE_SIZE * IMAGE_SIZE;
        for (int i = 0; i < LIVE_PATTERNS_PER_FRAME; ++i) {
            randomBinaryPattern(P_i, IMAGE_SIZE, IMAGE_SIZE, rng);

            double B_i = 0.0;
            for (int p = 0; p < n; ++p) B_i += static_cast<double>(objectFloat[p]) * static_cast<double>(P_i[p]);
            bucketSignals.push_back(B_i);

            P_prime_i = MatF(n);
            convolve3x3_rep(P_i, P_prime_i, IMAGE_SIZE, IMAGE_SIZE, kernel3);

            double meanPprime = 0.0;
            for (int p = 0; p < n; ++p) meanPprime += P_prime_i[p];
            meanPprime /= n;
            for (int p = 0; p < n; ++p) P_prime_i[p] -= static_cast<float>(meanPprime);

            double meanB = std::accumulate(bucketSignals.begin(), bucketSignals.end(), 0.0) / bucketSignals.size();
            float scale = static_cast<float>(B_i - meanB);
            for (int p = 0; p < n; ++p) reconstructed[p] += P_prime_i[p] * scale;
        }

        // V2K gain
        float linearGain = std::pow(10.0f, m_reconstructionGainDb / 20.0f);
        for (float &v : reconstructed) v *= linearGain;

        // min/max
        float minVal = reconstructed.empty() ? 0.0f : reconstructed[0];
        float maxVal = reconstructed.empty() ? 0.0f : reconstructed[0];
        for (float v : reconstructed) { if (v < minVal) minVal = v; if (v > maxVal) maxVal = v; }

        MatF resultScaled(n);
        if (maxVal - minVal > 1e-6f) {
            float m = 255.0f / (maxVal - minVal);
            for (int p = 0; p < n; ++p) {
                float val = (reconstructed[p] - minVal) * m;
                if (val < 0.0f) val = 0.0f;
                if (val > 255.0f) val = 255.0f;
                resultScaled[p] = val;
            }
        } else {
            std::fill(resultScaled.begin(), resultScaled.end(), 0.0f);
        }

        QImage resultImg = matFToGrayImage(resultScaled, IMAGE_SIZE, IMAGE_SIZE);
        resultLabel->setPixmap(QPixmap::fromImage(resultImg.scaled(256, 256, Qt::KeepAspectRatio)));

        statusLabel->setText(QString("Live-Rekonstruktion | Gain: %1 dB | Muster: %2 | Aktualisiert: %3")
                             .arg(m_reconstructionGainDb, 0, 'f', 1)
                             .arg(LIVE_PATTERNS_PER_FRAME)
                             .arg(QTime::currentTime().toString("hh:mm:ss")));
    }
};

#include "ghost.moc"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    GhostImagingSimulator w;
    w.show();
    return a.exec();
}

