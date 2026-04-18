// live_ghost_filter.cpp
// Komplette Anwendung: Live-Video, Korrigierter Geisterfilter, JET-Farbraum, Gesichtserkennung & Speicherung

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <QStatusBar>
#include <QDebug>
#include <QString>
#include <sstream>
#include <iomanip>

#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <vector>
#include <cmath>

using namespace std;
using namespace cv;

// --- KONFIGURATION ---
const int GHOST_DISPLACEMENT_X = 3;
const int GHOST_DISPLACEMENT_Y = 3;
const float ALIGNMENT_THRESHOLD = 0.95f;
// Stellen Sie sicher, dass diese Datei im Build-Verzeichnis liegt!
const string HAARCASCADE_PATH = "/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml";


// --- Utility: Mat zu QImage Konvertierung ---
static QImage matToQImage(const cv::Mat &mat) {
    if (mat.empty()) return QImage();
    if (mat.type() == CV_8UC3) {
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, COLOR_BGR2RGB);
        return QImage((const uchar*)rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
    }
    if (mat.type() == CV_8UC1) {
        return QImage((const uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
    }
    return QImage();
}

// ====================================================================
// GHOST FILTER: GRADIENTEN-SEPERATOR (KORRIGIERT)
// ====================================================================

void separateGradients(const Mat& inputImage, Mat& sceneGradient, Mat& weightMap)
{
    Mat gray;
    cvtColor(inputImage, gray, COLOR_BGR2GRAY);

    Mat grad_x, grad_y;
    Sobel(gray, grad_x, CV_32F, 1, 0, 3);
    Sobel(gray, grad_y, CV_32F, 0, 1, 3);

    vector<Mat> channels = {grad_x, grad_y};
    Mat G;
    merge(channels, G); // G ist CV_32FC2

    sceneGradient.create(G.size(), CV_32FC2);
    weightMap.create(G.size(), CV_32FC1);
    sceneGradient.setTo(0);
    weightMap.setTo(0);

    const int rows = G.rows;
    const int cols = G.cols;
    const float MIN_MAGNITUDE_THRESHOLD = 1.5f;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {

            Point p(j, i);
            const Vec2f current_G = G.at<Vec2f>(p);
            float current_mag = norm(current_G);

            if (current_mag < MIN_MAGNITUDE_THRESHOLD) continue;

            Point p_minus(j - GHOST_DISPLACEMENT_X, i - GHOST_DISPLACEMENT_Y);
            Point p_plus(j + GHOST_DISPLACEMENT_X, i + GHOST_DISPLACEMENT_Y);

            bool valid_minus = (p_minus.x >= 0 && p_minus.y >= 0);
            bool valid_plus = (p_plus.x < cols && p_plus.y < rows);

            float A_Ref_Minus = 0.0f;
            float A_Ref_Plus = 0.0f;

            if (valid_minus) {
                const Vec2f G_minus = G.at<Vec2f>(p_minus);
                float mag_minus = norm(G_minus);
                if (mag_minus > MIN_MAGNITUDE_THRESHOLD) {
                    A_Ref_Minus = (current_G.dot(G_minus)) / (current_mag * mag_minus);
                }
            }
            if (valid_plus) {
                const Vec2f G_plus = G.at<Vec2f>(p_plus);
                float mag_plus = norm(G_plus);
                if (mag_plus > MIN_MAGNITUDE_THRESHOLD) {
                    // KORREKTUR: Verwendung von mag_plus, um den Compiler-Fehler zu beheben
                    A_Ref_Plus = (current_G.dot(G_plus)) / (current_mag * mag_plus);
                }
            }

            bool is_reflection = (A_Ref_Minus > ALIGNMENT_THRESHOLD || A_Ref_Plus > ALIGNMENT_THRESHOLD);

            if (!is_reflection) {
                sceneGradient.at<Vec2f>(p) = current_G;
                weightMap.at<float>(p) = 1.0f;
            } else {
                sceneGradient.at<Vec2f>(p) = Vec2f(0.0f, 0.0f);
                weightMap.at<float>(p) = 0.0f;
            }
        }
    }
}

// ====================================================================
// VISUALISIERUNG: RADIOAKTIVITÄTS-FARBRAUM
// ====================================================================

Mat visualizeSceneGradient(const Mat& sceneGradient) {
    Mat Gx_scene_float;
    Mat channels[2];
    split(sceneGradient, channels);

    channels[0].convertTo(Gx_scene_float, CV_8UC1, 0.5, 127);

    Mat colored_output;
    applyColorMap(Gx_scene_float, colored_output, COLORMAP_JET);

    return colored_output;
}

// ====================================================================
// INTERPRETATION: GESICHTSERKENNUNG (MIT AGENT-LOGIK)
// ====================================================================

void detectFacesAndHighlight(const Mat& inputFrame, Mat& outputFrame, CascadeClassifier& face_cascade, std::vector<Rect>& detectedFaces) {
    Mat frame_gray;

    cvtColor(inputFrame, frame_gray, COLOR_BGR2GRAY);
    equalizeHist(frame_gray, frame_gray);

    face_cascade.detectMultiScale(frame_gray, detectedFaces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

    for (size_t i = 0; i < detectedFaces.size(); ++i) {
        const auto& face = detectedFaces[i];

        if (i == 0) {
            // Das erste erkannte Gesicht ist der "Agent"
            rectangle(outputFrame, face, Scalar(0, 165, 255), 3); // BGR: Orange/Gold
            // Label es als "Agent (Kein Gespenst)"
            putText(outputFrame, "AGENT (KEIN GESPENST) - Speichern als JPG", Point(face.x, face.y - 10), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 165, 255), 2);
        } else {
            // Andere Gesichter als Sekundär-Signal
            rectangle(outputFrame, face, Scalar(255, 255, 0), 2); // BGR: Cyan
            putText(outputFrame, "Sekundar-Signal", Point(face.x, face.y + face.height + 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 0), 1);
        }
    }
}


class LiveGhostFilterApp : public QMainWindow {
    Q_OBJECT // MUSS FÜR QT-FUNKTIONEN VORHANDEN SEIN
public:
    LiveGhostFilterApp(QWidget *parent = nullptr) : QMainWindow(parent),
        timer(new QTimer(this)), fileCounter(0)
    {
        setWindowTitle("Live Geisterfilter: Agent-Erkennung & automatische Speicherung (.jpg)");

        // --- Gesichtserkennung laden ---
        if (!faceCascade.load(HAARCASCADE_PATH)) {
            QMessageBox::critical(this, "Fatal Error", "Konnte die Haar Cascade Datei nicht laden: " + QString::fromStdString(HAARCASCADE_PATH));
            qApp->exit(1);
            return;
        }

        // --- UI Setup ---
        QWidget *central = new QWidget;
        QVBoxLayout *mainLayout = new QVBoxLayout(central);

        inputLabel = new QLabel("Live Video Input (/dev/video0)");
        inputLabel->setAlignment(Qt::AlignCenter);

        outputLabel = new QLabel("Geisterfilter (JET) + Agent-Erkennung");
        outputLabel->setAlignment(Qt::AlignCenter);

        QHBoxLayout *videoLayout = new QHBoxLayout;
        videoLayout->addWidget(inputLabel);
        videoLayout->addWidget(outputLabel);
        mainLayout->addLayout(videoLayout);

        setCentralWidget(central);

        // --- OpenCV Video Setup ---
        cap.open("/dev/video0");
        if (!cap.isOpened()) {
            QMessageBox::critical(this, "Fatal Error", "Kann Webcam unter /dev/video0 nicht öffnen. Überprüfen Sie Berechtigungen oder Pfad.");
            qApp->exit(1);
            return;
        }

        cap.set(CAP_PROP_FRAME_WIDTH, 640);
        cap.set(CAP_PROP_FRAME_HEIGHT, 480);
        cap.set(CAP_PROP_FPS, 30);

        statusBar()->showMessage("Kamera verbunden. Warte auf Agent-Signal (Gesicht)...");

        connect(timer, &QTimer::timeout, this, &LiveGhostFilterApp::processFrame);
        timer->start(33);
    }

    ~LiveGhostFilterApp() override {
        if (cap.isOpened()) {
            cap.release();
        }
    }

private slots:
    void processFrame() {
        Mat inputFrame, displayFrame;
        cap >> inputFrame;

        if (inputFrame.empty()) return;

        inputFrame.copyTo(displayFrame);

        QSize labelSize = inputLabel->size();

        // 1. RUN GHOST FILTER
        Mat sceneGradient;
        Mat weightMap;
        separateGradients(inputFrame, sceneGradient, weightMap);

        // 2. VISUALISIERUNG (JET Farbraum)
        Mat filterOutputVis = visualizeSceneGradient(sceneGradient);

        // 3. INTERPRETATION & ERKENNUNG
        std::vector<Rect> detectedFaces;
        detectFacesAndHighlight(inputFrame, filterOutputVis, faceCascade, detectedFaces);

        // 4. SPEICHERUNG: Wenn Gesichter gefunden wurden
        if (!detectedFaces.empty()) {
            std::ostringstream filename_stream;
            filename_stream << "Agent_Aufnahme_" << std::setw(6) << std::setfill('0') << fileCounter++ << ".jpg";
            std::string filename = filename_stream.str();

            bool success = cv::imwrite(filename, filterOutputVis);

            if (success) {
                // Statusmeldung entsprechend der "Agent"- und ".jpg"-Anforderung aktualisiert
                statusBar()->showMessage(QString("AGENT ERKANNT (KEIN GESPENST)! Aufnahme als JPG gespeichert: %1").arg(QString::fromStdString(filename)), 5000);
            } else {
                statusBar()->showMessage("FEHLER: Aufnahme konnte nicht gespeichert werden. Speicherberechtigung prüfen.", 5000);
            }
        } else {
            statusBar()->showMessage("Kamera verbunden. Warte auf Agent-Signal (Gesicht)...", 100);
        }

        // 5. Anzeige
        QImage qimg_input = matToQImage(displayFrame);
        QImage qimg_out = matToQImage(filterOutputVis);

        if (!qimg_input.isNull() && !qimg_out.isNull()) {
            inputLabel->setPixmap(QPixmap::fromImage(qimg_input).scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            outputLabel->setPixmap(QPixmap::fromImage(qimg_out).scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

private:
    cv::VideoCapture cap;
    CascadeClassifier faceCascade;
    QTimer *timer;
    QLabel *inputLabel;
    QLabel *outputLabel;

    int fileCounter; // Zähler für eindeutige Dateinamen
};

// =========================================================================
// WICHTIG: MOC-Include
// Diese Zeile MUSS von qmake/Qt Creator in der erzeugten Makefile
// oder manuell (indem Sie den Kommentar entfernen) eingebunden werden,
// damit Qt die Metainformationen korrekt verlinkt!
// =========================================================================
#include "main.moc"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    LiveGhostFilterApp w;
    w.show();
    return a.exec();
}
