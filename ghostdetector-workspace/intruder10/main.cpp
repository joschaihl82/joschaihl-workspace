// intruder.cpp
// Single-file Qt5 + OpenCV intruder detector (3x STRONGER GHOSTS, NO PHOTO SAVING)
// Focus: HIGH FRAME RATE & SIMPLICITY, Pure Visualization

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QDir>
#include <QPixmap>
#include <QStatusBar>
#include <QMessageBox>
#include <QDateTime>
#include <QFileInfo>
#include <QDebug>
#include <QCoreApplication>
#include <QFile>
#include <QElapsedTimer>

#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>

using namespace std;
using namespace cv;

// Utility function to convert cv::Mat to QImage
static QImage matToQImage(const cv::Mat &mat) {
    if (mat.empty()) return QImage();
    if (mat.type() == CV_8UC3) {
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, COLOR_BGR2RGB);
        return QImage((const uchar*)rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
    } else if (mat.type() == CV_8UC1) {
        return QImage((const uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
    }
    return QImage();
}


class IntruderDetector : public QMainWindow {
    Q_OBJECT
public:
    IntruderDetector(QWidget *parent = nullptr) : QMainWindow(parent),
        timer(new QTimer(this)),
        minMotionArea(500),
        motionDetected(false)
    {
        setWindowTitle("Intruder Detector: 3x GHOST POWER - Live Monitor");

        showFullScreen();

        QWidget *central = new QWidget;
        QVBoxLayout *mainLayout = new QVBoxLayout(central);

        previewLabel = new QLabel;
        previewLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        previewLabel->setStyleSheet("background: #000; color: #fff;");
        previewLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(previewLabel);

        status = new QLabel("Initializing...");
        mainLayout->addWidget(status);

        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        setCentralWidget(central);
        statusBar()->showMessage("Starting...");

        QString cascadePath = "haarcascade_frontalface_default.xml";
        if (!QFile::exists(cascadePath)) {
            cascadePath = "/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml";
        }

        if (!faceCascade.load(cascadePath.toStdString())) {
            QMessageBox::warning(this, "Warning", "Failed to load Haar cascade. Only Motion Detection will work.");
        }

        cap.open(0, CAP_V4L2);
        if (!cap.isOpened()) {
            QMessageBox::critical(this, "Error", "Cannot open webcam.");
            qApp->exit(1);
            return;
        }

        cap.set(CAP_PROP_FRAME_WIDTH, 640);
        cap.set(CAP_PROP_FRAME_HEIGHT, 480);
        cap.set(CAP_PROP_FPS, 60);

        connect(timer, &QTimer::timeout, this, &IntruderDetector::processFrame);
        timer->start(16);
    }

    ~IntruderDetector() override {
        if (cap.isOpened()) {
            cap.release();
        }
    }

private:
    // --- Aggressive Radiation Filter ---
    Mat applyRadiationFilter(const Mat& inputFrame) {
        Mat gray;
        // 1. Dynamic Color Balance (Histogram Equalization)
        cvtColor(inputFrame, gray, COLOR_BGR2GRAY);
        equalizeHist(gray, gray);

        Mat outputFrame;
        cvtColor(gray, outputFrame, COLOR_GRAY2BGR);

        // 2. High-Pass Filter (Laplacian)
        Mat laplacian;
        Laplacian(gray, laplacian, CV_16S, 3);
        Mat absLaplacian;
        convertScaleAbs(laplacian, absLaplacian);

        double meanIntensity = mean(absLaplacian)[0];
        int dynamicTraceThreshold = static_cast<int>(min(250.0, max(50.0, meanIntensity * 1.8)));

        Mat traceMask;
        threshold(absLaplacian, traceMask, dynamicTraceThreshold, 255, THRESH_BINARY);

        Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
        dilate(traceMask, traceMask, kernel, Point(-1, -1), 1);

        // 3. Contours & Hull Drawing
        vector<vector<Point>> contours;
        Canny(traceMask, traceMask, 50, 150);
        findContours(traceMask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        Mat transparentHullLayer = Mat::zeros(inputFrame.size(), CV_8UC3);
        // Brightest Green
        Scalar neonGreen = Scalar(0, 255, 0);

        for (size_t i = 0; i < contours.size(); i++) {
            if (contours[i].size() > 5) {
                vector<Point> hull;
                convexHull(contours[i], hull);

                vector<vector<Point>> hull_vec = {hull};
                fillPoly(transparentHullLayer, hull_vec, neonGreen);

                Mat hullMask = Mat::zeros(inputFrame.size(), CV_8UC3);
                fillPoly(hullMask, hull_vec, neonGreen);
                GaussianBlur(hullMask, hullMask, Size(15, 15), 0);

                bitwise_or(transparentHullLayer, hullMask, transparentHullLayer);
            }
        }

        // 4. Final Blend: 3x Ghost Power
        Mat finalBlendedOutput;
        // **ÄNDERUNG: Ghost Layer Weight auf 0.75 gesetzt**
        // Background (outputFrame) 0.5 + Ghost Layer (transparentHullLayer) 0.75
        // Dies ist 3x stärker als der ursprüngliche 0.25 (angenommen) oder deutlich stärker als 0.5.
        addWeighted(outputFrame, 0.5, transparentHullLayer, 0.75, 0.0, finalBlendedOutput);

        return finalBlendedOutput;
    }

private slots:
    void processFrame() {
        Mat frame;
        cap >> frame;

        if (frame.empty()) {
            statusBar()->showMessage(cap.isOpened() ? "Warning: Frame empty." : "Error: Camera not open!");
            return;
        }

        // --- 1. Preprocessing ---
        Mat currentGray;
        cvtColor(frame, currentGray, COLOR_BGR2GRAY);

        Mat blurredGray;
        GaussianBlur(currentGray, blurredGray, Size(21, 21), 0);

        if (previousGray.empty()) {
            previousGray = blurredGray.clone();
            return;
        }

        // --- 2. Motion Detection ---
        Mat diff;
        absdiff(previousGray, blurredGray, diff);
        threshold(diff, diff, 25, 255, THRESH_BINARY);

        vector<vector<Point>> motionContours;
        findContours(diff, motionContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        motionDetected = false;
        bool largeMotion = false;
        for (const auto& c : motionContours) {
            if (contourArea(c) > 5000) {
                largeMotion = true;
                break;
            } else if (contourArea(c) > minMotionArea) {
                motionDetected = true;
            }
        }
        blurredGray.copyTo(previousGray);

        Mat display;
        // --- 3. Apply Filter (Only when needed) ---
        if (motionDetected || largeMotion) {
            display = applyRadiationFilter(frame);
        } else {
            cvtColor(currentGray, display, COLOR_GRAY2BGR);
        }

        bool intruderPresent = false;

        // --- 4. Face Detection ---
        vector<Rect> faces;
        if (motionDetected || largeMotion) {
            if (!faceCascade.empty()) {
                faceCascade.detectMultiScale(currentGray, faces, 1.1, 5, 0, Size(80,80));
            }
        }
        frameCounter++;

        // --- 5. Marking and Status ---
        for (const Rect &r : faces) {
            rectangle(display, r, Scalar(0,0,255), 3);
            putText(display, "NUCLEAR DEVIL ALERT!", Point(r.x, r.y - 10), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,0,255), 2);
            intruderPresent = true;
        }

        // Status Update
        QString currentStatus;
        if (intruderPresent) {
            currentStatus = "🚨 **INTRUDER DETECTED!** (Live Tracking)";
            statusBar()->showMessage("Intruder in sight!", 100);
        } else if (motionDetected) {
            currentStatus = "⚠️ **MOTION DETECTED** (Filter Active)";
        } else {
            currentStatus = "✅ **System Idle**";
        }
        status->setText(currentStatus);

        // --- 6. Display Update ---
        QImage qimg = matToQImage(display);
        if (!qimg.isNull()) {
            previewLabel->setPixmap(QPixmap::fromImage(qimg).scaled(previewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

private:
    QTimer *timer;
    QLabel *previewLabel;
    QLabel *status;
    CascadeClassifier faceCascade;
    VideoCapture cap;

    Mat previousGray;
    int minMotionArea;
    bool motionDetected;
    int frameCounter = 0;
};

// =========================================================================
// CRITICAL FIX: The MOC include MUST be here to define vtable and staticMetaObject.
// =========================================================================

#include "main.moc"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("LiveGhostDetector");

    IntruderDetector w;
    w.show();
    return a.exec();
}
