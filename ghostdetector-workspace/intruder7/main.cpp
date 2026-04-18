// intruder.cpp or main.cpp
// Single-file Qt5 + OpenCV intruder detector (MAXIMUM CPU PERFORMANCE, FULLSCREEN)
// FIX: Using the manual #include "filename.moc" to resolve vtable linking errors.

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
#include <QThread>

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <vector>
#include <iostream>

using namespace std;
using namespace cv;
using namespace cv::face;

// Utility function to convert cv::Mat to QImage
static QImage matToQImage(const cv::Mat &mat) {
    if (mat.empty()) return QImage();
    if (mat.depth() != CV_8U) {
        Mat convertedMat;
        mat.convertTo(convertedMat, CV_8U);
        if (convertedMat.channels() == 3) {
            cv::Mat rgb;
            cv::cvtColor(convertedMat, rgb, COLOR_BGR2RGB);
            return QImage((const uchar*)rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
        } else if (convertedMat.channels() == 1) {
            return QImage((const uchar*)convertedMat.data, convertedMat.cols, convertedMat.rows, convertedMat.step, QImage::Format_Grayscale8).copy();
        }
    } else {
        if (mat.type() == CV_8UC3) {
            cv::Mat rgb;
            cv::cvtColor(mat, rgb, COLOR_BGR2RGB);
            return QImage((const uchar*)rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
        } else if (mat.type() == CV_8UC1) {
            return QImage((const uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
        } else if (mat.type() == CV_8UC4) {
            return QImage((const uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32).copy();
        }
    }
    return QImage();
}


class IntruderDetector : public QMainWindow {
    Q_OBJECT

public:
    // **Constructor body follows the initializer list**
    IntruderDetector(QWidget *parent = nullptr) : QMainWindow(parent),
        timer(new QTimer(this)), ownerCaptured(false), thresholdDistance(60.0),
        intruderDetected(false),
        savingIntruder(false),
        minMotionArea(500),
        motionDetected(false),
        frameCounter(0)
    {
        setWindowTitle("Intruder Detector: EXTREME PERFORMANCE DEVIL WATCH");

        // Vollbildansicht beim Start
        showFullScreen();

        // UI setup
        QWidget *central = new QWidget;
        QVBoxLayout *mainLayout = new QVBoxLayout(central);

        previewLabel = new QLabel;
        previewLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        previewLabel->setStyleSheet("background: #222; color: #ddd;");
        previewLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(previewLabel);

        status = new QLabel("Initializing...");
        mainLayout->addWidget(status);

        // Layout-Ränder entfernen
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        setCentralWidget(central);
        statusBar()->showMessage("Starting...");

        QDir().mkpath("intruders");

        QString cascadePath = "haarcascade_frontalface_default.xml";
        if (!QFile::exists(cascadePath)) {
            cascadePath = "/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml";
        }

        if (!faceCascade.load(cascadePath.toStdString())) {
            QMessageBox::critical(this, "Error", "Failed to load Haar cascade.");
        }

        cap.open(0, CAP_V4L2);
        if (!cap.isOpened()) {
            QMessageBox::critical(this, "Error", "Cannot open webcam.");
            qApp->exit(1);
            return;
        }

        cap.set(CAP_PROP_FRAME_WIDTH, 640);
        cap.set(CAP_PROP_FRAME_HEIGHT, 480);
        cap.set(CAP_PROP_FPS, 30);

        recognizer = LBPHFaceRecognizer::create();

        // Owner/Model Loading Logic
        if (QFileInfo::exists("recognizer.yml")) {
            try {
                recognizer->read("recognizer.yml");
                ownerCaptured = true;
                statusBar()->showMessage("Owner profile loaded from recognizer.yml. Monitoring initiated.");
                status->setText("Owner loaded. Monitoring for radioactive devils.");
            } catch (const cv::Exception& e) {
                qDebug() << "OpenCV Model Load Exception:" << e.what();
                statusBar()->showMessage("Error loading recognizer.yml. Attempting new capture...");
                ownerCaptured = false;
            }
        }

        if (!ownerCaptured && QFileInfo::exists("owner.jpg")) {
            Mat owner = imread("owner.jpg", IMREAD_GRAYSCALE);
            if (!owner.empty()) {
                vector<Mat> trainingFaces;
                vector<int> labels;
                Mat resizedOwner;
                cv::resize(owner, resizedOwner, Size(200, 200));
                trainingFaces.push_back(resizedOwner);
                labels.push_back(0);
                for (float gamma = 0.5f; gamma <= 1.5f; gamma += 0.5f) {
                    Mat augmented;
                    resizedOwner.convertTo(augmented, -1, 1.0, gamma * 50 - 50);
                    trainingFaces.push_back(augmented);
                    labels.push_back(0);
                }

                try {
                    recognizer->train(trainingFaces, labels);
                    recognizer->write("recognizer.yml");
                    ownerCaptured = true;
                    statusBar()->showMessage("Owner loaded from owner.jpg, model trained & saved.");
                    status->setText("Owner loaded. Monitoring for radioactive devils.");
                } catch (const cv::Exception& e) {
                    statusBar()->showMessage("Error training model from owner.jpg. Owner profile needed.");
                    ownerCaptured = false;
                }
            } else {
                statusBar()->showMessage("owner.jpg exists but could not be read. Owner profile needed.");
                status->setText("Waiting to capture owner face...");
            }
        } else if (!ownerCaptured) {
            statusBar()->showMessage("No owner profile found. Waiting to capture owner face...");
            status->setText("Waiting to capture owner face...");
        }

        // Timer auf 33ms (ca. 30 FPS) gesetzt
        connect(timer, &QTimer::timeout, this, &IntruderDetector::processFrame);
        timer->start(33);
    }

    ~IntruderDetector() override {
        if (cap.isOpened()) {
            cap.release();
        }
    }

private:
    // **MAX PERFORMANCE** Radiation Filter
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

        // 4. Final Blend: Optimized blending using addWeighted
        Mat finalBlendedOutput;
        addWeighted(outputFrame, 0.7, transparentHullLayer, 0.5, 0.0, finalBlendedOutput);

        return finalBlendedOutput;
    }

private slots:
    void processFrame() {
        frameCounter++;
        Mat frame;
        cap >> frame;

        if (frame.empty()) {
            statusBar()->showMessage(cap.isOpened() ? "Warning: Frame empty." : "Error: Camera not open!");
            return;
        }

        Mat currentGray;
        cvtColor(frame, currentGray, COLOR_BGR2GRAY);

        Mat blurredGray;
        GaussianBlur(currentGray, blurredGray, Size(21, 21), 0);

        if (previousGray.empty()) {
            previousGray = blurredGray.clone();
            return;
        }

        // --- Motion Detection Tripwire ---
        Mat diff;
        absdiff(previousGray, blurredGray, diff);
        threshold(diff, diff, 25, 255, THRESH_BINARY);

        vector<vector<Point>> motionContours;
        findContours(diff, motionContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        motionDetected = false;
        for (const auto& c : motionContours) {
            if (contourArea(c) > minMotionArea) {
                motionDetected = true;
                break;
            }
        }
        blurredGray.copyTo(previousGray);
        // --- ENDE Motion Detection ---

        Mat display = frame;
        Mat filteredDisplay;

        // Filter nur auf 10% der Frames anwenden, oder wenn Bewegung erkannt wurde.
        bool needsFilter = motionDetected || (frameCounter % 10 == 0);

        if (needsFilter) {
            filteredDisplay = applyRadiationFilter(frame);
            display = filteredDisplay;
        } else {
            Mat tempGray;
            cvtColor(frame, tempGray, COLOR_BGR2GRAY);
            cvtColor(tempGray, display, COLOR_GRAY2BGR);
        }

        vector<Rect> faces;

        if (motionDetected || intruderDetected) {
            faceCascade.detectMultiScale(currentGray, faces, 1.2, 3, CASCADE_SCALE_IMAGE, Size(100,100));
        }

        // Owner Capture Logic
        if (!ownerCaptured && !faces.empty()) {
            Rect f = faces[0];
            Mat faceROI = currentGray(f).clone();
            cv::resize(faceROI, faceROI, Size(200,200));

            if (imwrite("owner.jpg", faceROI)) {
                vector<Mat> trainingFaces;
                vector<int> labels;
                trainingFaces.push_back(faceROI);
                labels.push_back(0);
                for (float gamma = 0.5f; gamma <= 1.5f; gamma += 0.5f) {
                    Mat augmented;
                    faceROI.convertTo(augmented, -1, 1.0, gamma * 50 - 50);
                    trainingFaces.push_back(augmented);
                    labels.push_back(0);
                }
                try {
                    recognizer->train(trainingFaces, labels);
                    recognizer->write("recognizer.yml");
                    ownerCaptured = true;
                    statusBar()->showMessage("Owner captured, model trained & saved.");
                    status->setText("Owner loaded. Monitoring for radioactive devils.");
                } catch (const cv::Exception& e) {
                    statusBar()->showMessage("Error during training, capturing new owner...");
                }
            } else {
                statusBar()->showMessage("Failed to save owner.jpg.");
            }
        }

        bool intruderCurrentlyPresent = false;

        // Intruder Detection
        for (const Rect &r : faces) {
            rectangle(display, r, Scalar(0,255,0), 2);

            if (ownerCaptured) {
                if (r.x >= 0 && r.y >= 0 && r.x + r.width <= currentGray.cols && r.y + r.height <= currentGray.rows) {
                    Mat faceROI = currentGray(r).clone();
                    cv::resize(faceROI, faceROI, Size(200,200));
                    int label = -1;
                    double confidence = 0.0;

                    if (recognizer->empty()) continue;

                    recognizer->predict(faceROI, label, confidence);
                    if (confidence > thresholdDistance) {
                        intruderCurrentlyPresent = true;
                        string text = format("NUCLEAR DEVIL DETECTED (%.1f)", confidence);
                        putText(display, text, Point(r.x, r.y - 10), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0,0,255), 2);
                        rectangle(display, r, Scalar(0,0,255), 3);

                        if (filteredDisplay.empty()) {
                            filteredDisplay = applyRadiationFilter(frame);
                        }

                        currentIntruderFrame = filteredDisplay.clone();
                        rectangle(currentIntruderFrame, r, Scalar(0,0,255), 3);
                        putText(currentIntruderFrame, text, Point(r.x, r.y - 10), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0,0,255), 2);
                        break;
                    } else {
                        string txt = format("Owner (%.1f)", confidence);
                        putText(display, txt, Point(r.x, r.y - 6), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,255,0), 1);
                    }
                }
            } else {
                putText(display, "Candidate Owner", Point(r.x, r.y - 6), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,255,0), 1);
            }
        }

        // Intruder State Machine & Status Update
        QString currentStatus;
        if (motionDetected) {
            currentStatus = "🚨 **MOTION DETECTED**";
        } else {
            currentStatus = "✅ **System Idle**";
        }

        if (intruderCurrentlyPresent) {
            currentStatus = "☢️ **NUCLEAR DEVIL DETECTED!**";
            if (!intruderDetected) {
                intruderDetected = true;
                intruderDisappearedTimer.restart();
                statusBar()->showMessage("NUCLEAR DEVIL DETECTED! Stand by for photo...");
            }
        } else if (intruderDetected) {
            if (!savingIntruder) {
                if (intruderDisappearedTimer.elapsed() < 0) { intruderDisappearedTimer.restart(); }

                long long elapsed = intruderDisappearedTimer.elapsed();
                long long remaining = 10000 - elapsed;

                if (elapsed >= 10000) {
                    saveIntruderPhoto();
                    intruderDetected = false;
                    savingIntruder = false;
                } else {
                    statusBar()->showMessage(QString("Devil gone. Photo saving in: %1 s...").arg(remaining / 1000.0, 0, 'f', 1));
                    currentStatus = QString("⏳ Devil Disappeared. Saving in %1s...").arg(remaining / 1000.0, 0, 'f', 1);
                }
            }
        } else {
            if (ownerCaptured) {
                status->setText(currentStatus + " | Monitoring for radioactive devils.");
            } else {
                status->setText(currentStatus + " | Waiting to capture owner face...");
            }
        }

        // update preview
        QImage qimg = matToQImage(display);
        if (!qimg.isNull()) {
            previewLabel->setPixmap(QPixmap::fromImage(qimg).scaled(previewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

    void saveIntruderPhoto() {
        if (currentIntruderFrame.empty()) return;

        savingIntruder = true;

        string ts = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss").toStdString();
        char fname[128];
        snprintf(fname, sizeof(fname), "intruders/intruder_%s.jpg", ts.c_str());

        if (imwrite(fname, currentIntruderFrame)) {
            qDebug() << "Saved delayed intruder photo:" << fname;
            statusBar()->showMessage(QString("Nuclear Devil photo saved after 10s delay: %1").arg(fname), 5000);
            currentIntruderFrame = Mat();
        } else {
            qDebug() << "Failed to save delayed intruder image";
            statusBar()->showMessage("Failed to save delayed intruder image.", 5000);
        }
        savingIntruder = false;
    }


private:
    QTimer *timer;
    QLabel *previewLabel;
    QLabel *status;
    CascadeClassifier faceCascade;
    Ptr<LBPHFaceRecognizer> recognizer;
    VideoCapture cap;
    bool ownerCaptured;
    double thresholdDistance;

    Mat previousGray;
    int minMotionArea;
    bool motionDetected;
    int frameCounter;

    QElapsedTimer intruderDisappearedTimer;
    bool intruderDetected;
    bool savingIntruder;
    Mat currentIntruderFrame;
};

// =========================================================================
// CRITICAL FIX: The MOC include MUST be here to define vtable and staticMetaObject.
// Replace "intruder" with the actual filename if different (e.g., "main.moc").
// Your log suggests the filename is main.cpp, so I'll use intruder.moc as the log output
// suggests the final binary is named 'intruder'.
// You may need to manually change this to 'main.moc' in your IDE settings if needed.
// =========================================================================

#include "main.moc"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("IntruderDetector");

    IntruderDetector w;
    w.show();
    return a.exec();
}
