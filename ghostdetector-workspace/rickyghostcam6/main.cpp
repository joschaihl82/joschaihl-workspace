// live_ghost_filter_best_cascade.cpp
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
#include <sstream>
#include <iomanip>

#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>

using namespace std;
using namespace cv;

// --- KONFIGURATION ZUSAMMENFASSUNG ---
const struct Config {
    const int DISPLACEMENT_X = 32, DISPLACEMENT_Y = 32;
    const float ALIGN_THRESHOLD = 0.025f;
    // BESTER FILTER: haarcascade_frontalface_default.xml
    const string CASCADE_PATH = "/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml";
    const int MOTION_THRESHOLD = 100;
    const float FADE_FACTOR = 0.90f;
    const Scalar OVERLAY_COLOR = Scalar(255, 100, 0);
    const double OVERLAY_ALPHA = 0.4;
} CFG;

// Datenstruktur
struct AnomalyData { Rect rect; float FPValue; Scalar AvgColor; string source; };

// --- UTILITY: Mat zu QImage Konvertierung ---
static QImage matToQImage(const Mat &mat) {
    if (mat.empty()) return {};
    Mat rgb;
    if (mat.type() == CV_8UC3) cvtColor(mat, rgb, COLOR_BGR2RGB);
    else if (mat.type() == CV_8UC1) cvtColor(mat, rgb, COLOR_GRAY2RGB);
    else rgb = mat;
    return QImage(rgb.data, rgb.cols, mat.rows, mat.step, QImage::Format_RGB888).copy();
}

// ====================================================================
// A: GRADIENTEN-SEPERATOR
// ====================================================================
void separateGradients(const Mat& inputImage, Mat& sceneGradient, Mat& weightMap) {
    Mat gray, grad_x, grad_y, G;
    cvtColor(inputImage, gray, COLOR_BGR2GRAY);
    Sobel(gray, grad_x, CV_32F, 1, 0, 3);
    Sobel(gray, grad_y, CV_32F, 0, 1, 3);
    merge(vector<Mat>{grad_x, grad_y}, G);

    sceneGradient.create(G.size(), CV_32FC2); sceneGradient.setTo(0);
    weightMap.create(G.size(), CV_32FC1); weightMap.setTo(0);

    const int R = G.rows, C = G.cols;
    const float MIN_MAG_THRESH = 1.5f / 8.0f;

    for (int i = 0; i < R; ++i) {
        for (int j = 0; j < C; ++j) {
            Point p(j, i);
            const Vec2f G_curr = G.at<Vec2f>(p);
            float mag_curr = norm(G_curr);
            if (mag_curr < MIN_MAG_THRESH) continue;
            float A_Ref = 0.0f;

            for (int sign : {-1, 1}) {
                Point p_shift(j + sign * CFG.DISPLACEMENT_X, i + sign * CFG.DISPLACEMENT_Y);
                if (p_shift.x >= 0 && p_shift.x < C && p_shift.y >= 0 && p_shift.y < R) {
                    const Vec2f G_shift = G.at<Vec2f>(p_shift);
                    float mag_shift = norm(G_shift);
                    if (mag_shift > MIN_MAG_THRESH)
                        A_Ref = max(A_Ref, (G_curr.dot(G_shift)) / (mag_curr * mag_shift));
                }
            }

            if (A_Ref < CFG.ALIGN_THRESHOLD) {
                sceneGradient.at<Vec2f>(p) = G_curr;
                weightMap.at<float>(p) = 1.0f;
            }
        }
    }
    subtract(Scalar(1.0f), weightMap, weightMap);
}

// ====================================================================
// B: REINE GRADIENTEN-VISUALISIERUNG
// ====================================================================
Mat visualizeSceneGradient(const Mat& sceneGradient) {
    Mat channels[2], magnitude, mag_8bit, colored_output;
    split(sceneGradient, channels);
    magnitude = channels[0].mul(channels[0]) + channels[1].mul(channels[1]);
    cv::sqrt(magnitude, magnitude);
    magnitude.convertTo(mag_8bit, CV_8UC1, 10.0);
    applyColorMap(mag_8bit, colored_output, COLORMAP_JET);
    return colored_output;
}

// ====================================================================
// D & E: ANOMALIE BERECHNUNG & ZEICHNEN
// ====================================================================

AnomalyData calculateAnomalyData(const Mat& originalFrame, const Mat& weightMap, const Rect& anomalyBox, const string& source) {
    AnomalyData data = {anomalyBox, 0.0f, Scalar(0, 0, 0), source};
    Rect clampedBox = anomalyBox & Rect(0, 0, weightMap.cols, weightMap.rows);
    if (clampedBox.empty()) return data;

    data.FPValue = (float)round(cv::sum(weightMap(clampedBox))[0] / 1000.0f);
    data.AvgColor = cv::mean(originalFrame(clampedBox));
    return data;
}

void drawAnomalies(Mat& outputFrame, const vector<AnomalyData>& anomaliesData) {
    int anomalyCount = 1;
    for (const auto& d : anomaliesData) {
        // Highlight-Effekt
        Mat colorLayer = Mat::zeros(outputFrame.size(), outputFrame.type());
        colorLayer(d.rect).setTo(d.AvgColor);
        addWeighted(outputFrame, 0.33, colorLayer, 0.67, 0.0, outputFrame);

        // Rechteck und Text
        rectangle(outputFrame, d.rect, Scalar(0, 255, 0), 3);
        stringstream ss;
        ss << "FP: " << fixed << setprecision(1) << d.FPValue << "K";

        string label_text = "A#" + to_string(anomalyCount++) + " (" + d.source + ")";

        putText(outputFrame, label_text, Point(d.rect.x, d.rect.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
        putText(outputFrame, ss.str(), Point(d.rect.x, d.rect.height + d.rect.y + 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2);
    }
}

// ====================================================================
// HAUPTKLASSE
// ====================================================================

class LiveGhostFilterApp : public QMainWindow {
    Q_OBJECT
public:
    LiveGhostFilterApp(QWidget *parent = nullptr) : QMainWindow(parent), timer(new QTimer(this)) {
        setWindowTitle("Live Anomaliefilter: Haar Frontalgesicht-Fokus");

        // Lade die standardmäßige Haar Cascade
        if (!faceCascade.load(CFG.CASCADE_PATH)) {
            QMessageBox::critical(this, "Fatal Error", QString("Konnte Cascade nicht laden: %1").arg(QString::fromStdString(CFG.CASCADE_PATH)));
            qApp->exit(1); return;
        }

        setupUI();

        if (!cap.open("/dev/video0")) {
            QMessageBox::critical(this, "Fatal Error", "Kann Webcam nicht öffnen."); qApp->exit(1); return;
        }
        cap.set(CAP_PROP_FRAME_WIDTH, 640);
        cap.set(CAP_PROP_FRAME_HEIGHT, 480);
        motionAccumulationMask = Mat::zeros(cap.get(CAP_PROP_FRAME_HEIGHT), cap.get(CAP_PROP_FRAME_WIDTH), CV_8UC1);

        statusBar()->showMessage("Kamera verbunden. Starte Filter...", 100);

        connect(timer, &QTimer::timeout, this, &LiveGhostFilterApp::processFrame);
        timer->start(33);
    }

private slots:
    void processFrame() {
        Mat inputFrame;
        cap >> inputFrame;
        if (inputFrame.empty()) return;

        // --- PIPELINE: VORVERARBEITUNG (Bewegung) ---
        Mat currentGray; cvtColor(inputFrame, currentGray, COLOR_BGR2GRAY);

        if (!prevGrayFrame.empty()) {
            Mat diff, thresh;
            absdiff(currentGray, prevGrayFrame, diff);
            threshold(diff, thresh, CFG.MOTION_THRESHOLD, 255, THRESH_BINARY);
            max(motionAccumulationMask, thresh, motionAccumulationMask);
            motionAccumulationMask *= CFG.FADE_FACTOR;
        }
        prevGrayFrame = currentGray.clone();

        // --- PIPELINE: KERN-ANALYSE ---
        Mat sceneGradient, weightMap;
        separateGradients(inputFrame, sceneGradient, weightMap);
        Mat filterOutputVis = visualizeSceneGradient(sceneGradient);

        // --- PIPELINE: POST-VERARBEITUNG (Motion Reverb) ---
        if (!motionAccumulationMask.empty()) {
            Mat mask3C; cvtColor(motionAccumulationMask, mask3C, COLOR_GRAY2BGR);
            Mat colorOverlay = mask3C.mul(CFG.OVERLAY_COLOR / 255.0);
            addWeighted(filterOutputVis, 1.0 - CFG.OVERLAY_ALPHA, colorOverlay, CFG.OVERLAY_ALPHA, 0.0, filterOutputVis);
        }

        // --- PIPELINE: DETEKTION MIT HAAR CASCADE ---
        vector<AnomalyData> currentAnomaliesData;
        vector<Rect> detectedRects;

        Mat histEqualizedGray; equalizeHist(currentGray, histEqualizedGray);

        // Führt Detektion aus
        faceCascade.detectMultiScale(histEqualizedGray, detectedRects, 1.1, 3, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

        // Source-Name festlegen
        string source_name = "Frontal_HAAR";

        for (const auto& r : detectedRects) {
            currentAnomaliesData.push_back(calculateAnomalyData(inputFrame, weightMap, r, source_name));
        }

        // --- ZEICHNEN & ANZEIGE ---
        drawAnomalies(filterOutputVis, currentAnomaliesData);

        // Statusbar
        QString statusMsg = (!currentAnomaliesData.empty()) ?
                                QString("Live Detektion: %1 Anomalie(n) erkannt (Frontal-HAAR).").arg(currentAnomaliesData.size()) :
                                "Kamera verbunden. Warte auf Anomaliesignal...";
        statusBar()->showMessage(statusMsg, 100);

        // Anzeige
        QSize s = inputLabel->size();
        QImage qimg_input = matToQImage(inputFrame);
        QImage qimg_out = matToQImage(filterOutputVis);

        if (!qimg_input.isNull() && !qimg_out.isNull()) {
            inputLabel->setPixmap(QPixmap::fromImage(qimg_input).scaled(s, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            outputLabel->setPixmap(QPixmap::fromImage(qimg_out).scaled(s, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

    void setupUI() {
        QWidget *central = new QWidget;
        QVBoxLayout *mainLayout = new QVBoxLayout(central);

        auto createVideoLabel = [&](const QString& text) {
            QLabel* label = new QLabel(text);
            label->setAlignment(Qt::AlignCenter);
            label->setMinimumSize(320, 240);
            return label;
        };

        inputLabel = createVideoLabel("1. Rohbild");
        outputLabel = createVideoLabel("2. Szene-Gradienten (Frontal Haar Detektion)");

        QHBoxLayout *videoLayout = new QHBoxLayout;
        videoLayout->addWidget(inputLabel);
        videoLayout->addWidget(outputLabel);
        mainLayout->addLayout(videoLayout);

        setCentralWidget(central);
    }

private:
    cv::VideoCapture cap;
    CascadeClassifier faceCascade; // Nur ein Klassifikator
    QTimer *timer;
    QLabel *inputLabel, *outputLabel;
    Mat motionAccumulationMask, prevGrayFrame;
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
