// intruder.cpp
// Single-file Qt5 + OpenCV intruder detector (Optimized & Dynamic Color Balance)
// ... (Description and includes remain the same) ...

#include <QApplication>
#include <QMainWindow>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QTimer>
#include <QDir>
#include <QPixmap>
#include <QStatusBar>
#include <QMessageBox>
#include <QDateTime>
#include <QFileInfo>
#include <QDebug>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QFile>
#include <QElapsedTimer>

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <vector>
#include <iostream>

using namespace std;
using namespace cv;
using namespace cv::face;

// Utility function to convert cv::Mat to QImage (Unchanged)
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
    IntruderDetector(QWidget *parent = nullptr) : QMainWindow(parent),
        timer(new QTimer(this)), ownerCaptured(false), thresholdDistance(60.0),
        intruderDetected(false),
        savingIntruder(false),
        minMotionArea(500),
        motionDetected(false)
    {
        setWindowTitle("Intruder Detector: Elite Nuclear Devil Surveillance (Optimiert)");
        // ... (UI setup remains the same) ...
        QWidget *central = new QWidget;
        QHBoxLayout *mainLayout = new QHBoxLayout(central);

        QWidget *left = new QWidget;
        QVBoxLayout *leftLayout = new QVBoxLayout(left);
        previewLabel = new QLabel;
        previewLabel->setFixedSize(640, 480);
        previewLabel->setStyleSheet("background: #222; color: #ddd;");
        previewLabel->setAlignment(Qt::AlignCenter);
        leftLayout->addWidget(previewLabel);

        status = new QLabel("Initializing...");
        leftLayout->addWidget(status);

        mainLayout->addWidget(left, 0);

        QWidget *right = new QWidget;
        QVBoxLayout *rightLayout = new QVBoxLayout(right);
        QLabel *gtitle = new QLabel("<b>Intruder Gallery</b>");
        rightLayout->addWidget(gtitle);

        galleryWidget = new QWidget;
        galleryLayout = new QGridLayout(galleryWidget);
        galleryLayout->setSpacing(8);
        galleryLayout->setContentsMargins(0, 0, 0, 0);

        QScrollArea *scroll = new QScrollArea;
        scroll->setWidget(galleryWidget);
        scroll->setWidgetResizable(true);
        scroll->setMinimumWidth(320);
        rightLayout->addWidget(scroll);

        mainLayout->addWidget(right, 1);

        setCentralWidget(central);
        statusBar()->showMessage("Starting...");

        QDir().mkpath("intruders");

        QString cascadePath = "haarcascade_frontalface_default.xml";
        if (!QFile::exists(cascadePath)) {
            cascadePath = "/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml";
        }

        if (!faceCascade.load(cascadePath.toStdString())) {
            QMessageBox::critical(this, "Error", "Failed to load Haar cascade. Ensure 'haarcascade_frontalface_default.xml' is available.");
        }

        cap.open(0, CAP_V4L2);
        if (!cap.isOpened()) {
            QMessageBox::critical(this, "Error", "Cannot open webcam.");
            qApp->exit(1);
            return;
        }

        cap.set(CAP_PROP_FRAME_WIDTH, 640);
        cap.set(CAP_PROP_FRAME_HEIGHT, 480);
        cap.set(CAP_PROP_FPS, 25);

        recognizer = LBPHFaceRecognizer::create();

        // Owner/Model Loading Logic (Remains the same for persistence)
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
                    statusBar()->showMessage("Owner loaded from owner.jpg, model trained & saved to recognizer.yml.");
                    status->setText("Owner loaded. Monitoring for radioactive devils.");
                } catch (const cv::Exception& e) {
                    qDebug() << "OpenCV Training Exception:" << e.what();
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

        connect(timer, &QTimer::timeout, this, &IntruderDetector::processFrame);
        timer->start(50);
        loadGallery();
    }

    ~IntruderDetector() override {
        if (cap.isOpened()) {
            cap.release();
        }
    }

private:
    // **UPGRADED** Multiscale Radiation Filter with Dynamic Color Balance
    Mat applyRadiationFilter(const Mat& inputFrame) {
        Mat gray;
        // 1. Dynamic Color Balance: Adjust brightness/contrast of the base image
        // to stabilize the radiation ghosts regardless of current lighting.
        // We use simple histogram equalization for quick normalization.
        cvtColor(inputFrame, gray, COLOR_BGR2GRAY);
        equalizeHist(gray, gray); // <--- DYNAMIC COLOR BALANCE

        // Base frame is derived from the dynamically adjusted gray image
        Mat outputFrame;
        cvtColor(gray, outputFrame, COLOR_GRAY2BGR);

        // 2. High-Pass Filter (Laplacian)
        Mat laplacian;
        Laplacian(gray, laplacian, CV_16S, 3);
        Mat absLaplacian;
        convertScaleAbs(laplacian, absLaplacian);

        // Dynamic Thresholding (based on normalized image)
        double meanIntensity = mean(absLaplacian)[0];
        int dynamicTraceThreshold = static_cast<int>(min(250.0, max(50.0, meanIntensity * 1.8))); // Increased multiplier for normalized image

        Mat traceMask;
        threshold(absLaplacian, traceMask, dynamicTraceThreshold, 255, THRESH_BINARY);

        // Gamma-Ray Dilation
        Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
        dilate(traceMask, traceMask, kernel, Point(-1, -1), 1);

        // 3. Multiscale Contours & Hull Drawing
        vector<vector<Point>> contours;
        Canny(traceMask, traceMask, 50, 150);
        findContours(traceMask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        Mat transparentHullLayer = Mat::zeros(inputFrame.size(), CV_8UC4);
        Scalar neonGreenWithAlpha = Scalar(0, 255, 0, 255 * 0.95);

        for (size_t i = 0; i < contours.size(); i++) {
            if (contours[i].size() > 5) {
                vector<Point> hull;
                convexHull(contours[i], hull);

                vector<vector<Point>> hull_vec = {hull};
                fillPoly(transparentHullLayer, hull_vec, neonGreenWithAlpha);

                // Secondary diffusion layer
                Mat hullMask = Mat::zeros(inputFrame.size(), CV_8UC4);
                fillPoly(hullMask, hull_vec, Scalar(0, 255, 0, 100));
                GaussianBlur(hullMask, hullMask, Size(15, 15), 0);

                // Optimized blend of primary and secondary layer
                for (int y = 0; y < inputFrame.rows; ++y) {
                    uchar* ptrHull = transparentHullLayer.ptr<uchar>(y);
                    uchar* ptrDiff = hullMask.ptr<uchar>(y);
                    for (int x = 0; x < inputFrame.cols; ++x) {
                        if (ptrHull[x*4 + 3] < 255) { // Check alpha channel
                            for (int c = 0; c < 4; ++c) {
                                ptrHull[x*4 + c] = saturate_cast<uchar>(ptrHull[x*4 + c] + ptrDiff[x*4 + c] * 0.5);
                            }
                        }
                    }
                }
            }
        }

        // 4. Final Blend
        Mat blendedOutput = Mat::zeros(inputFrame.size(), CV_8UC3);

        // Optimized blend of foreground (hull) and background (grayscale base)
        for (int y = 0; y < outputFrame.rows; ++y) {
            uchar* ptrBlend = blendedOutput.ptr<uchar>(y);
            uchar* ptrBG = outputFrame.ptr<uchar>(y);
            uchar* ptrFG = transparentHullLayer.ptr<uchar>(y);

            for (int x = 0; x < outputFrame.cols; ++x) {
                double alpha_fg = ptrFG[x*4 + 3] / 255.0;
                double alpha_bg = 1.0 - alpha_fg;

                for (int c = 0; c < 3; ++c) {
                    ptrBlend[x*3 + c] =
                        static_cast<uchar>(ptrBG[x*3 + c] * alpha_bg + ptrFG[x*4 + c] * alpha_fg);
                }
            }
        }

        return blendedOutput;
    }

private slots:
    void processFrame() {
        Mat frame;
        cap >> frame;

        if (frame.empty()) {
            statusBar()->showMessage(cap.isOpened() ? "Warning: Frame empty, camera may be disconnected." : "Error: Camera is not open!");
            return;
        }

        Mat currentGray;
        cvtColor(frame, currentGray, COLOR_BGR2GRAY);
        GaussianBlur(currentGray, currentGray, Size(21, 21), 0);

        if (previousGray.empty()) {
            previousGray = currentGray.clone();
            return;
        }

        // --- Motion Detection Tripwire ---
        Mat diff;
        absdiff(previousGray, currentGray, diff);
        threshold(diff, diff, 25, 255, THRESH_BINARY);

        vector<vector<Point>> motionContours;
        // ACHTUNG: Verwende `diff` direkt, vermeide `diff.clone()`
        findContours(diff, motionContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        motionDetected = false;
        for (const auto& c : motionContours) {
            if (contourArea(c) > minMotionArea) {
                motionDetected = true;
                break;
            }
        }
        // Vermeide unnötiges Klonen: previousGray = currentGray.clone();
        // Stattdessen nur die Pointer der vorherigen Graustufenmatrix austauschen oder bei Bedarf kopieren
        if (previousGray.size() == currentGray.size()) {
            // In-place copy (performanter als Mat::clone() in manchen Fällen)
            currentGray.copyTo(previousGray);
        } else {
            previousGray = currentGray.clone(); // Fallback
        }
        // --- ENDE Motion Detection ---

        Mat filteredDisplay = applyRadiationFilter(frame); // <--- DYNAMIC FILTER
        Mat display = filteredDisplay; // Vermeide .clone(), da filteredDisplay das Endprodukt ist.

        vector<Rect> faces;

        // Face detection only on motion
        if (motionDetected) {
            // Face detection läuft auf der NICHT-GEGLÄTTETEN Graustufenversion der aktuellen Frame
            Mat detectionGray;
            cvtColor(frame, detectionGray, COLOR_BGR2GRAY);
            faceCascade.detectMultiScale(detectionGray, faces, 1.2, 3, CASCADE_SCALE_IMAGE, Size(100,100));
        }

        // Owner Capture Logic (Unchanged, uses un-smoothed gray frame)
        if (!ownerCaptured && !faces.empty()) {
            Rect f = faces[0];
            Mat faceROI = currentGray(f).clone();
            cv::resize(faceROI, faceROI, Size(200,200));

            // ... (Augmentation and model saving remains the same) ...
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
                    statusBar()->showMessage("Owner captured, model trained & saved to recognizer.yml. Monitoring initiated.");
                    status->setText("Owner loaded. Monitoring for radioactive devils.");
                } catch (const cv::Exception& e) {
                    statusBar()->showMessage("Error during training, capturing new owner...");
                }
            } else {
                statusBar()->showMessage("Failed to save owner.jpg. Check write permissions.");
            }
        }

        bool intruderCurrentlyPresent = false;

        // Intruder Detection and Status Update
        for (const Rect &r : faces) {
            // ... (Detection logic remains the same) ...
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

                        // Speichern der bereits gefilterten Frame
                        currentIntruderFrame = filteredDisplay.clone(); // HIER MUSS GEKLONT WERDEN, da `display` weiter modifiziert wird
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

        // Intruder State Machine & Status Update (Unchanged)
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

    // ... (saveIntruderPhoto and loadGallery remain unchanged) ...
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
            loadGallery();
        } else {
            qDebug() << "Failed to save delayed intruder image";
            statusBar()->showMessage("Failed to save delayed intruder image.", 5000);
        }
        savingIntruder = false;
    }

    void loadGallery() {
        QDir dir("intruders");
        QStringList files = dir.entryList(QStringList() << "*.jpg" << "*.png", QDir::Files, QDir::Time | QDir::Reversed);

        QLayoutItem *child;
        while ((child = galleryLayout->takeAt(0)) != nullptr) {
            if (child->widget()) {
                child->widget()->hide();
                child->widget()->deleteLater();
            }
            delete child;
        }

        int row = 0, col = 0;
        const int cols = 1;
        const int maxThumbs = 5;
        const int thumbWidth = 300;
        const int thumbHeight = 225;

        for (const QString &f : files) {
            if (row * cols + col >= maxThumbs) break;

            QLabel *thumb = new QLabel;
            thumb->setParent(galleryWidget);

            thumb->setFixedSize(thumbWidth, thumbHeight);
            thumb->setAlignment(Qt::AlignCenter);

            QPixmap pm(dir.filePath(f));

            if (!pm.isNull()) {
                thumb->setPixmap(pm.scaled(thumb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            } else {
                thumb->setText(f);
            }

            galleryLayout->addWidget(thumb, row, col);
            col++;
            if (col >= cols) { col = 0; ++row; }
        }

        galleryWidget->adjustSize();
        galleryLayout->update();
    }


private:
    QTimer *timer;
    QLabel *previewLabel;
    QLabel *status;
    QWidget *galleryWidget;
    QGridLayout *galleryLayout;
    CascadeClassifier faceCascade;
    Ptr<LBPHFaceRecognizer> recognizer;
    VideoCapture cap;
    bool ownerCaptured;
    double thresholdDistance;

    // Motion Detection Variables
    Mat previousGray;
    int minMotionArea;
    bool motionDetected;

    QElapsedTimer intruderDisappearedTimer;
    bool intruderDetected;
    bool savingIntruder;
    Mat currentIntruderFrame;
};

#include "main.moc"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("IntruderDetector");

    IntruderDetector w;
    w.show();
    return a.exec();
}

