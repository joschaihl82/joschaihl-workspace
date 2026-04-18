// live_ghost_filter_modern.cpp
// Zusammengefasster Code: Weitwinkel-Entzerrung, Gradienten-Amalgamation, Motion-Reverb und Qt-Interface
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
#include <chrono>

#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <vector>
#include <cmath>

using namespace std;
using namespace cv;

// --- KONFIGURATION ---
const int GHOST_DISPLACEMENT_X = 32;
const int GHOST_DISPLACEMENT_Y = 32;
const float ALIGNMENT_THRESHOLD = 0.025;
const string HAARCASCADE_PATH = "/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml";
const int FROZEN_IMAGE_DISPLAY_TIME_MS = 5000;
const int MOTION_THRESHOLD = 100;
const float FADE_FACTOR = 0.90f;
const Scalar MOTION_OVERLAY_COLOR = Scalar(255, 100, 0); // BGR (Cyan-Blau)
const double MOTION_OVERLAY_ALPHA = 0.4;

// ACHTUNG: WEITWINKEL-KALIBRIERUNGSPARAMETER (Platzhalter - MÜSSEN ERSETZT WERDEN)
const Mat CAMERA_MATRIX = (Mat_<double>(3, 3) <<
                               620.0, 0.0, 320.0,
                           0.0, 620.0, 240.0,
                           0.0, 0.0, 1.0);

const Mat DISTORTION_COEFFICIENTS = (Mat_<double>(1, 5) <<
                                         -0.25, 0.08, 0.001, -0.002, 0.0);

// NEU: Datenstruktur zur Speicherung der Anomalie-Daten
struct AnomalyData {
    Rect rect;
    float FPValue;
    Scalar AvgColor;
};

// --- UTILITY: Mat zu QImage Konvertierung ---
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
// A: GRADIENTEN-SEPERATOR (LUMOS CORE LOGIC)
// ====================================================================

void separateGradients(const Mat& inputImage, Mat& sceneGradient, Mat& weightMap)
{
    Mat gray;
    cv::cvtColor(inputImage, gray, COLOR_BGR2GRAY);

    Mat grad_x, grad_y;
    Sobel(gray, grad_x, CV_32F, 1, 0, 3);
    Sobel(gray, grad_y, CV_32F, 0, 1, 3);

    vector<Mat> channels = {grad_x, grad_y};
    Mat G;
    merge(channels, G);

    sceneGradient.create(G.size(), CV_32FC2);
    weightMap.create(G.size(), CV_32FC1);
    sceneGradient.setTo(0);
    weightMap.setTo(0);

    const int rows = G.rows;
    const int cols = G.cols;
    const float MIN_MAGNITUDE_THRESHOLD = 1.5f / 8.0;

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
    // Kehre die WeightMap um: 1.0 bedeutet Ghost (Reflexion), 0.0 bedeutet Szene.
    cv::subtract(cv::Scalar(1.0f), weightMap, weightMap);
}

// ====================================================================
// B: REKONSTRUKTIONS-PROXY & VISUALISIERUNG (JET)
// ====================================================================

Mat visualizeSceneGradient(const Mat& sceneGradient) {
    Mat channels[2];
    split(sceneGradient, channels);

    Mat magnitude;
    magnitude = channels[0].mul(channels[0]) + channels[1].mul(channels[1]);
    cv::sqrt(magnitude, magnitude);

    Mat magnitude_8bit;
    magnitude.convertTo(magnitude_8bit, CV_8UC1, 10.0);

    Mat colored_output;
    applyColorMap(magnitude_8bit, colored_output, COLORMAP_JET);

    return colored_output;
}

// ====================================================================
// C: AMALGAMATION (Mischen von Szene-Visualisierung und Originalfarbe)
// ====================================================================

Mat amalgamateOutput(const Mat& originalFrame, const Mat& sceneGradientVis, const Mat& weightMap) {

    Mat sceneVisFloat;
    sceneGradientVis.convertTo(sceneVisFloat, CV_32FC3);

    Mat originalFloat;
    originalFrame.convertTo(originalFloat, CV_32FC3);

    // Die WeightMap (Ghost-Gewichtung) ist 1.0 (Ghost) / 0.0 (Szene)
    Mat alphaMap;
    cv::cvtColor(weightMap, alphaMap, COLOR_GRAY2BGR);

    // Die Scene-Gewichtskarte (1.0 = Szene)
    Mat sceneWeightMap;
    cv::subtract(cv::Scalar(1.0f), alphaMap, sceneWeightMap);

    Mat outputFloat = Mat::zeros(originalFloat.size(), CV_32FC3);

    // Scene Part: Originalfarbe an Szene-Regionen
    Mat scenePart;
    multiply(originalFloat, sceneWeightMap, scenePart);

    // Ghost Part: Gradienten-Visualisierung an Ghost-Regionen
    Mat ghostPart;
    multiply(sceneVisFloat, alphaMap, ghostPart);

    // Amalgamiere: Addiere die beiden Teile
    add(scenePart, ghostPart, outputFloat);

    Mat finalAmalgamatedOutput;
    outputFloat.convertTo(finalAmalgamatedOutput, CV_8UC3, 1.0);

    return finalAmalgamatedOutput;
}


// ====================================================================
// D: ANOMALIE BERECHNUNG & E: ZEICHNEN
// ====================================================================

AnomalyData calculateAnomalyColorAndFingerprint(const Mat& originalFrame, const Mat& weightMap, const Rect& anomalyBox) {
    AnomalyData data;
    data.rect = anomalyBox;
    data.FPValue = 0.0f;
    data.AvgColor = Scalar(0, 0, 0);

    Rect clampedBox = anomalyBox & Rect(0, 0, weightMap.cols, weightMap.rows);

    if (clampedBox.empty()) return data;

    Mat roi_weight = weightMap(clampedBox);
    Scalar sumScalar = cv::sum(roi_weight);
    data.FPValue = (float)round(sumScalar[0] / 1000.0f);

    Mat roi_color = originalFrame(clampedBox);
    data.AvgColor = cv::mean(roi_color);

    return data;
}

void drawAnomaliesAndHighlight(Mat& outputFrame, const std::vector<AnomalyData>& anomaliesData) {
    int anomalyCount = 1;
    const double ALPHA_BG_RECT = 0.33;
    const double ALPHA_FG_RECT = 0.67;

    for (const auto& data : anomaliesData) {
        const Rect& anomaly = data.rect;
        float fpValue = data.FPValue;
        const Scalar& avgColor = data.AvgColor;

        Mat colorLayer = Mat::zeros(outputFrame.size(), outputFrame.type());
        colorLayer(anomaly).setTo(avgColor);

        cv::addWeighted(outputFrame, ALPHA_BG_RECT, colorLayer, ALPHA_FG_RECT, 0.0, outputFrame);

        rectangle(outputFrame, anomaly, Scalar(0, 255, 0), 3);

        std::string label = "ANOMALIE #" + std::to_string(anomalyCount++);
        cv::putText(outputFrame, label, Point(anomaly.x, anomaly.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);

        std::stringstream ss;
        ss << "FP: " << std::fixed << std::setprecision(1) << fpValue << "K";
        std::string fp_label = ss.str();

        cv::putText(outputFrame, fp_label, Point(anomaly.x, anomaly.y + anomaly.height + 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2);
    }
}


class LiveGhostFilterApp : public QMainWindow {
    Q_OBJECT
public:
    LiveGhostFilterApp(QWidget *parent = nullptr) : QMainWindow(parent),
        timer(new QTimer(this)), frozenAnomalyTimer(new QTimer(this)), fileCounter(0)
    {
        // ... (UI, Haar Cascade Laden, Video Setup) ...
        setWindowTitle("Live Anomaliefilter: Gradienten-Analyse (Entzerrung + Amalgamation)");

        if (!faceCascade.load(HAARCASCADE_PATH)) {
            QMessageBox::critical(this, "Fatal Error", "Konnte die Haar Cascade Datei nicht laden.");
            qApp->exit(1);
            return;
        }

        lastDetectionTime = std::chrono::steady_clock::now() - std::chrono::milliseconds(2001);

        QWidget *central = new QWidget;
        QVBoxLayout *mainLayout = new QVBoxLayout(central);
        inputLabel = new QLabel("Live Video Input (Rohbild)");
        inputLabel->setAlignment(Qt::AlignCenter);
        inputLabel->setMinimumSize(320, 240);
        outputLabel = new QLabel("Restaurierte Szene (Entzerrt + Amalgamiert + Motion-Reverb)");
        outputLabel->setAlignment(Qt::AlignCenter);
        outputLabel->setMinimumSize(320, 240);
        QHBoxLayout *videoLayout = new QHBoxLayout;
        videoLayout->addWidget(inputLabel);
        videoLayout->addWidget(outputLabel);
        mainLayout->addLayout(videoLayout);

        frozenAnomalyLabel = new QLabel("Eingefrorene Anomalie");
        frozenAnomalyLabel->setAlignment(Qt::AlignCenter);
        frozenAnomalyLabel->setStyleSheet("QLabel { background-color : black; color : white; border: 2px solid red; }");
        frozenAnomalyLabel->setFixedSize(320, 240);
        frozenAnomalyLabel->hide();
        mainLayout->addWidget(frozenAnomalyLabel);
        setCentralWidget(central);

        cap.open("/dev/video0");
        if (!cap.isOpened()) {
            QMessageBox::critical(this, "Fatal Error", "Kann Webcam unter /dev/video0 nicht öffnen.");
            qApp->exit(1);
            return;
        }

        cap.set(CAP_PROP_FRAME_WIDTH, 640);
        cap.set(CAP_PROP_FRAME_HEIGHT, 480);
        cap.set(CAP_PROP_FPS, 30);

        motionAccumulationMask = Mat::zeros(cap.get(CAP_PROP_FRAME_HEIGHT), cap.get(CAP_PROP_FRAME_WIDTH), CV_8UC1);
        prevGrayFrame = Mat();

        statusBar()->showMessage("Kamera verbunden. Starte Anomaliefilter...", 100);

        connect(timer, &QTimer::timeout, this, &LiveGhostFilterApp::processFrame);
        timer->start(33);

        connect(frozenAnomalyTimer, &QTimer::timeout, this, &LiveGhostFilterApp::clearFrozenAnomaly);
    }

    ~LiveGhostFilterApp() override {
        if (cap.isOpened()) {
            cap.release();
        }
    }

private slots:
    void processFrame() {
        using namespace std::chrono;
        auto now = steady_clock::now();

        Mat rawInputFrame;
        cap >> rawInputFrame;
        if (rawInputFrame.empty()) return;

        // 0. WEITWINKEL-ENTZERRUNG
        Mat inputFrame;
        undistortFrame(rawInputFrame, inputFrame);

        Mat displayFrame;
        rawInputFrame.copyTo(displayFrame);
        QSize labelSize = inputLabel->size();

        // BEWEGUNGSERKENNUNG (Läuft auf entzerrtem Bild)
        Mat currentGrayFrame;
        cv::cvtColor(inputFrame, currentGrayFrame, COLOR_BGR2GRAY);
        if (!prevGrayFrame.empty()) {
            Mat diff;
            cv::absdiff(currentGrayFrame, prevGrayFrame, diff);
            Mat thresholdedDiff;
            cv::threshold(diff, thresholdedDiff, MOTION_THRESHOLD, 255, THRESH_BINARY);
            cv::max(motionAccumulationMask, thresholdedDiff, motionAccumulationMask);
            motionAccumulationMask = motionAccumulationMask * FADE_FACTOR;
        }
        prevGrayFrame = currentGrayFrame.clone();

        // 1. GRADIENTEN-SEPARATION
        Mat sceneGradient;
        Mat weightMap;
        separateGradients(inputFrame, sceneGradient, weightMap);

        // 2. REKONSTRUKTIONS-PROXY & VISUALISIERUNG
        Mat filterOutputVis = visualizeSceneGradient(sceneGradient);

        // 2.5. AMALGAMATION
        Mat amalgamatedFrame = amalgamateOutput(inputFrame, filterOutputVis, weightMap);
        filterOutputVis = amalgamatedFrame;

        // BEWEGUNGSSCHLEIER-ÜBERLAGERUNG
        if (!motionAccumulationMask.empty()) {
            Mat motionColorOverlay;
            Mat motionMask3C;
            cv::cvtColor(motionAccumulationMask, motionMask3C, COLOR_GRAY2BGR);
            motionColorOverlay = motionMask3C.mul(MOTION_OVERLAY_COLOR / 255.0);
            cv::addWeighted(filterOutputVis, 1.0 - MOTION_OVERLAY_ALPHA, motionColorOverlay, MOTION_OVERLAY_ALPHA, 0.0, filterOutputVis);
        }

        // 3. ANOMALIE-DETEKTION (auf entzerrtem Bild)
        std::vector<Rect> currentDetectedAnomalyRects;
        Mat frame_gray;
        cv::cvtColor(inputFrame, frame_gray, COLOR_BGR2GRAY);
        equalizeHist(frame_gray, frame_gray);
        faceCascade.detectMultiScale(frame_gray, currentDetectedAnomalyRects, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

        std::vector<AnomalyData> currentAnomaliesData;
        for (const auto& anomalyRect : currentDetectedAnomalyRects) {
            AnomalyData data = calculateAnomalyColorAndFingerprint(inputFrame, weightMap, anomalyRect);
            currentAnomaliesData.push_back(data);
        }

        // VERZÖGERUNGS- & EINFRIERLOGIK
        std::vector<AnomalyData>* anomaliesToDraw = &currentAnomaliesData;
        int currentAnomalyCount = currentAnomaliesData.size();
        auto elapsed = duration_cast<milliseconds>(now - lastDetectionTime);

        if (!currentAnomaliesData.empty()) {
            lastDetectedAnomaliesData = currentAnomaliesData;
            lastDetectionTime = now;
            if (frozenAnomalyLabel->isVisible()) clearFrozenAnomaly();
        } else if (elapsed.count() < 2000) {
            anomaliesToDraw = &lastDetectedAnomaliesData;
        } else if (!lastDetectedAnomaliesData.empty() && !frozenAnomalyLabel->isVisible()) {
            AnomalyData firstVanishedAnomalyData = lastDetectedAnomaliesData[0];
            Rect firstVanishedAnomaly = firstVanishedAnomalyData.rect;
            Rect clampedBox = firstVanishedAnomaly & Rect(0, 0, inputFrame.cols, inputFrame.rows);

            if (!clampedBox.empty()) {
                frozenAnomalyImage = inputFrame(clampedBox).clone();
                Mat scaledFrozenImage;
                cv::resize(frozenAnomalyImage, scaledFrozenImage, cv::Size(), 2.0, 2.0, cv::INTER_LINEAR);
                QImage qimgFrozen = matToQImage(scaledFrozenImage);
                if (!qimgFrozen.isNull()) {
                    frozenAnomalyLabel->setPixmap(QPixmap::fromImage(qimgFrozen).scaled(frozenAnomalyLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    frozenAnomalyLabel->setText("");
                    frozenAnomalyLabel->show();
                    frozenAnomalyTimer->start(FROZEN_IMAGE_DISPLAY_TIME_MS);
                    statusBar()->showMessage("Anomalie verschwunden. Eingefrorenes Bild wird angezeigt.", 5000);
                } else { qWarning() << "Fehler beim Konvertieren des eingefrorenen Bildes zu QImage."; }
            }
            anomaliesToDraw = &currentAnomaliesData;
        } else {
            anomaliesToDraw = &currentAnomaliesData;
        }

        // ANOMALIEN ZEICHNEN
        drawAnomaliesAndHighlight(filterOutputVis, *anomaliesToDraw);

        // SPEICHERUNG UND STATUS BAR LOGIK
        // ... (Statusbar-Logik bleibt unverändert) ...
        if (currentAnomalyCount > 0) {
            // ... (Speicherlogik) ...
        } else if (!lastDetectedAnomaliesData.empty() && duration_cast<milliseconds>(now - lastDetectionTime).count() < 2000) {
            statusBar()->showMessage("Anomalie verschwunden. Halte letzte Detektion für 2 Sekunden fest.", 100);
        } else if (frozenAnomalyLabel->isVisible()) {
            statusBar()->showMessage("Eingefrorene Anomalie wird angezeigt. Warte auf neue Detektion.", 100);
        } else {
            statusBar()->showMessage("Kamera verbunden. Warte auf Anomaliesignal...", 100);
        }

        // ANZEIGE
        QImage qimg_input = matToQImage(displayFrame);
        QImage qimg_out = matToQImage(filterOutputVis);

        if (!qimg_input.isNull() && !qimg_out.isNull()) {
            inputLabel->setPixmap(QPixmap::fromImage(qimg_input).scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            outputLabel->setPixmap(QPixmap::fromImage(qimg_out).scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

    void clearFrozenAnomaly() {
        frozenAnomalyLabel->hide();
        frozenAnomalyLabel->clear();
        frozenAnomalyLabel->setText("Eingefrorene Anomalie");
        frozenAnomalyTimer->stop();
        frozenAnomalyImage.release();
        qDebug() << "Eingefrorenes Bild ausgeblendet.";
    }

private:
    void undistortFrame(const Mat& src, Mat& dst) {
        cv::undistort(src, dst, CAMERA_MATRIX, DISTORTION_COEFFICIENTS);
    }

    cv::VideoCapture cap;
    CascadeClassifier faceCascade;
    QTimer *timer;
    QLabel *inputLabel;
    QLabel *outputLabel;
    QLabel *frozenAnomalyLabel;
    QTimer *frozenAnomalyTimer;
    Mat frozenAnomalyImage;
    int fileCounter;
    std::vector<AnomalyData> lastDetectedAnomaliesData;
    std::chrono::steady_clock::time_point lastDetectionTime;
    Mat motionAccumulationMask;
    Mat prevGrayFrame;
};

#include "main.moc"

// =========================================================================
// HAUPTPROGRAMM
// =========================================================================

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    LiveGhostFilterApp w;
    w.show();
    return a.exec();
}
