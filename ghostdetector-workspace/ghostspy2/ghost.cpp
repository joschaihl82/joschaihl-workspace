#include <QApplication>
#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QtMultimedia>      // QCamera, QMediaCaptureSession
#include <QImageCapture>     // Für die Bildaufnahme
#include <QMediaDevices>     // Für die Kamera-Geräteerkennung
#include <QVideoWidget>      // Für die Live-Anzeige (Qt5)

#include <vector>
#include <cmath>
#include <numeric>
#include <stdexcept>
#include <algorithm>

using namespace std;

// Definiert den Edge-Detection Filter Kernel K
const vector<vector<double>> KERNEL_K = {
    { 0.0, -1.0, 0.0 },
    { -1.0, 0.0, 1.0 },
    { 0.0, 1.0, 0.0 }
};
const int KERNEL_SIZE = 3;

// --- Algorithmus-Kern (Basis-processed CGI) ---

/**
 * @brief Führt eine 2D-Faltung (Konvolution) eines 2D-Musters mit einem 3x3-Kernel durch.
 */
vector<double> convolve2D(const vector<double>& pattern, int N, const vector<vector<double>>& kernel)
{
    vector<double> output(N * N, 0.0);
    int center = KERNEL_SIZE / 2;

    for (int y = 0; y < N; ++y) {
        for (int x = 0; x < N; ++x) {
            double sum = 0.0;
            for (int ky = 0; ky < KERNEL_SIZE; ++ky) {
                for (int kx = 0; kx < KERNEL_SIZE; ++kx) {
                    // Zyklische Randbedingungen (cyclic boundary conditions)
                    int py = (y + ky - center + N) % N;
                    int px = (x + kx - center + N) % N;
                    
                    sum += pattern[py * N + px] * kernel[ky][kx];
                }
            }
            output[y * N + x] = sum;
        }
    }
    return output;
}

/**
 * @brief Implementiert den Basis-processed Computational Ghost Imaging Algorithmus.
 */
class GhostImager
{
public:
    GhostImager(int size) : N(size), M(size * size)
    {
        // 1. Initialisierung der Rekonstruktionsbasis Psi (Canonical/Raster Basis)
        Psi.resize(M, vector<double>(M, 0.0));
        for (int j = 0; j < M; ++j) {
            Psi[j][j] = 1.0; 
        }
        applyFilterToBasis(KERNEL_K); // Generiere die Modifizierte Basis Phi beim Start
    }
    
    const vector<vector<double>>& getPhi() const { return Phi; }

    void applyFilterToBasis(const vector<vector<double>>& K)
    {
        Phi.clear();
        Phi.reserve(M);
        
        for (const auto& psi_j : Psi) {
            Phi.push_back(convolve2D(psi_j, N, K));
        }
        qDebug() << "Modifizierte Basis (Phi) generiert. Größe:" << Phi.size() << "x" << Phi[0].size();
    }

    /**
     * @brief Führt die Basis-processed Rekonstruktion durch: $|I\rangle = \sum_{j} S_j \cdot |\psi_{j}\rangle$.
     */
    vector<double> reconstructImage(const vector<double>& bucketSignals)
    {
        if (bucketSignals.size() != M) {
            throw runtime_error("Ungültige Eingabe: Bucket-Signale passen nicht zur Basisgröße.");
        }

        vector<double> reconstructedImage(M, 0.0);
        
        for (int j = 0; j < M; ++j) {
            double Sj = bucketSignals[j];
            
            // $|I\rangle$ ist die gewichtete Summe der Rekonstruktionsbasis $|\psi_{j}\rangle$
            for (int i = 0; i < M; ++i) {
                reconstructedImage[i] += Sj * Psi[j][i]; 
            }
        }
        return reconstructedImage;
    }

private:
    int N; // Seitenlänge (z.B. 64)
    int M; // Gesamtpixelzahl (N * N)
    vector<vector<double>> Psi; // Originale Rekonstruktionsbasis
    vector<vector<double>> Phi; // Modifizierte Beleuchtungsbasis
};

// --- GUI und Kamera-Logik ---

class GhostApp : public QMainWindow
{
    Q_OBJECT

public:
    GhostApp(QWidget *parent = nullptr) : QMainWindow(parent), N_RES(64), imager(N_RES)
    {
        setWindowTitle("CGI Simulation mit QCamera (Edge Detection)");
        QWidget *centralWidget = new QWidget;
        QGridLayout *layout = new QGridLayout(centralWidget);
        setCentralWidget(centralWidget);

        // --- 1. Kamera-Setup (Live-View) ---
        QCameraDevice defaultDevice = QMediaDevices::defaultVideoInput();
        if (defaultDevice.isNull()) {
            QMessageBox::critical(this, "Kamerafehler", "Keine Kamera gefunden!");
            return;
        }

        camera = new QCamera(defaultDevice, this);
        capture = new QImageCapture(camera, this);
        viewfinder = new QVideoWidget(centralWidget);
        
        // Qt5-Syntax für Live-View
        camera->setViewfinder(viewfinder); 

        liveLabel = new QLabel("Kamera-Livebild", centralWidget);
        liveLabel->setFixedSize(320, 240);
        liveLabel->setAlignment(Qt::AlignCenter);
        
        // Starten der Kamera-Vorschau
        camera->start(); 
        
        // --- 2. GUI-Elemente ---
        recoLabel = new QLabel("CGI Rekonstruktion (Gefiltert)", centralWidget);
        recoLabel->setAlignment(Qt::AlignCenter);
        recoLabel->setFixedSize(320, 240);
        
        startButton = new QPushButton("BILD AUFNEHMEN & CGI SIMULIEREN", centralWidget);
        
        // --- 3. Verbindungen ---
        connect(startButton, &QPushButton::clicked, this, &GhostApp::captureImage);
        connect(capture, &QImageCapture::imageCaptured, this, &GhostApp::processCapturedImage);
        
        // --- 4. Layout ---
        layout->addWidget(viewfinder, 0, 0); // Live-View
        layout->addWidget(recoLabel, 0, 1);
        layout->addWidget(startButton, 1, 0, 1, 2);
    }
    
    ~GhostApp() {
        if (camera && camera->isActive()) {
            camera->stop();
        }
    }

private slots:
    /**
     * @brief Löst die Aufnahme eines Einzelbildes von der Kamera aus.
     */
    void captureImage()
    {
        if (!camera->isActive() || camera->availability() != QCamera::Available) {
            QMessageBox::warning(this, "Kamera nicht bereit", "Bitte warten Sie, bis die Kamera bereit ist.");
            return;
        }
        startButton->setEnabled(false);
        startButton->setText("Bild wird aufgenommen...");
        
        // Hier wird das eigentliche Bild ausgelöst. Das Ergebnis kommt in processCapturedImage().
        capture->capture(); 
    }
    
    /**
     * @brief Verarbeitet das aufgenommene QImage als Objekt O und startet die Simulation.
     */
    void processCapturedImage(int id, const QImage& preview)
    {
        Q_UNUSED(id);
        
        startButton->setText("Verarbeite Bild...");
        QElapsedTimer timer;
        timer.start();
        
        // 1. Das aufgenommene QImage (preview) ist unser Objekt O. 
        // Skaliere und konvertiere das Bild zu einem Graustufen-Vektor der Größe M
        vector<double> objectVector = convertImageToObjectVector(preview);

        // 2. Simuliere Messung (Single-Pixel-Detektor)
        vector<double> bucketSignals = simulateBucketMeasurement(objectVector);
        
        // 3. Rekonstruktion des gefilterten Bildes
        vector<double> reconstructedImage = imager.reconstructImage(bucketSignals);

        // 4. Ergebnis anzeigen
        updateReconstructionDisplay(reconstructedImage);
        
        qDebug() << "CGI Simulation abgeschlossen in:" << timer.elapsed() << " ms.";
        
        startButton->setEnabled(true);
        startButton->setText("BILD AUFNEHMEN & CGI SIMULIEREN");
    }


    /**
     * @brief Konvertiert das aufgenommene QImage in einen Graustufen-Vektor O.
     */
    vector<double> convertImageToObjectVector(const QImage& image)
    {
        QImage resizedImage = image.convertToFormat(QImage::Format_Grayscale8).scaled(
            N_RES, N_RES, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            
        vector<double> objectVector(N_RES * N_RES);
        
        for (int y = 0; y < N_RES; ++y) {
            for (int x = 0; x < N_RES; ++x) {
                // Wert zwischen 0 (schwarz) und 1 (weiß)
                objectVector[y * N_RES + x] = qGray(resizedImage.pixel(x, y)) / 255.0;
            }
        }
        return objectVector;
    }
    
    /**
     * @brief Simuliert die Single-Pixel-Messung S_j = <phi_j | O>.
     */
    vector<double> simulateBucketMeasurement(const vector<double>& object)
    {
        int M = object.size();
        vector<double> S(M, 0.0);
        const auto& Phi = imager.getPhi();

        // Messung S_j ist das Skalarprodukt des Musters $\phi_{j}$ mit dem Objekt $O$
        for (int j = 0; j < M; ++j) {
            double sum = 0.0;
            const auto& phi_j = Phi[j];
            for (int i = 0; i < M; ++i) {
                sum += phi_j[i] * object[i]; // Skalarprodukt (Matrixmultiplikation)
            }
            S[j] = sum;
        }
        return S;
    }

    /**
     * @brief Zeigt das rekonstruierte und gefilterte Bild im GUI an.
     */
    void updateReconstructionDisplay(const vector<double>& recoImage)
    {
        double minVal = *min_element(recoImage.begin(), recoImage.end());
        double maxVal = *max_element(recoImage.begin(), recoImage.end());
        double range = max(abs(minVal), abs(maxVal));

        QImage recoImageQ(N_RES, N_RES, QImage::Format_RGB32);
        
        for (int y = 0; y < N_RES; ++y) {
            for (int x = 0; x < N_RES; ++x) {
                double val = recoImage[y * N_RES + x];
                
                // Visualisierung: Positive Kanten (Rot), Negative Kanten (Blau), Null (Schwarz)
                int r = 0, g = 0, b = 0;
                
                if (range > 1e-6) {
                    if (val > 0) {
                        r = static_cast<int>((val / range) * 255);
                    } else if (val < 0) {
                        b = static_cast<int>((abs(val) / range) * 255);
                    }
                }
                recoImageQ.setPixel(x, y, qRgb(r, g, b));
            }
        }
        recoLabel->setPixmap(QPixmap::fromImage(recoImageQ.scaled(recoLabel->size())));
    }


private:
    const int N_RES; // CGI-Auflösung (z.B. 64x64)
    GhostImager imager;
    
    // Qt Multimedia Komponenten
    QCamera *camera = nullptr;
    QImageCapture *capture = nullptr;
    QVideoWidget *viewfinder = nullptr;
    
    // GUI Komponenten
    QLabel *liveLabel; // Not used as viewfinder is used
    QLabel *recoLabel;
    QPushButton *startButton;
};

// --- Makro für Qt's Meta-Object-System ---
#include "ghost.moc"

// --- Hauptfunktion ---
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GhostApp w;
    w.setFixedSize(650, 300); 
    w.show();
    return a.exec();
}
