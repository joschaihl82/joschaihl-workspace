// intruder.cpp
// Single-file Qt5 + OpenCV intruder detector
// - captures first detected face as owner.jpg
// - then detects intruders and saves images to intruders/
// - applies a neon-green "Radiation Blur" filter to simulate X-rays, Gamma-rays, and Delta-rays.
// - Features: Adaptive Thresholding, Gamma-Ray Dilation, and Filter-Robust Owner Training.
// - saves only ONE photo after a 10-second delay when the intruder is gone.
// - shows live preview and large gallery thumbnails.

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

// Utility function to convert cv::Mat to QImage
static QImage matToQImage(const cv::Mat &mat) {
    if (mat.empty()) return QImage();
    // Ensure the input Mat is 8-bit unsigned
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
    } else { // Already 8-bit unsigned
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
        savingIntruder(false)
    {
        setWindowTitle("Intruder Detector: Nuclear Devil Surveillance");
        resize(1000, 700);

        // UI Initialization (Standard Qt setup)
        // ... (UI setup code omitted for brevity, remains unchanged) ...
        QWidget *central = new QWidget;
        QHBoxLayout *mainLayout = new QHBoxLayout(central);

        // Live preview area
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

        // Gallery area
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

        // Ensure directories exist
        QDir().mkpath("intruders");

        // Load Haar cascade
        QString cascadePath = "haarcascade_frontalface_default.xml";
        if (!QFile::exists(cascadePath)) {
            cascadePath = "/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml";
        }

        if (!faceCascade.load(cascadePath.toStdString())) {
            QMessageBox::critical(this, "Error", "Failed to load Haar cascade. Please ensure 'haarcascade_frontalface_default.xml' is available.");
        }

        // Open webcam
        cap.open(0, CAP_V4L2);
        if (!cap.isOpened()) {
            QMessageBox::critical(this, "Error", "Cannot open webcam. Check device index (0) and permissions.");
            qApp->exit(1);
            return;
        }

        cap.set(CAP_PROP_FRAME_WIDTH, 640);
        cap.set(CAP_PROP_FRAME_HEIGHT, 480);
        cap.set(CAP_PROP_FPS, 25);

        // Prepare recognizer
        recognizer = LBPHFaceRecognizer::create();

        // Initial check for 'owner.jpg'
        if (QFileInfo::exists("owner.jpg")) {
            Mat owner = imread("owner.jpg", IMREAD_GRAYSCALE);
            if (!owner.empty()) {

                // NEW: Load the original owner face and augment training data
                vector<Mat> trainingFaces;
                vector<int> labels;

                // 1. Original Face
                Mat resizedOwner;
                cv::resize(owner, resizedOwner, Size(200, 200));
                trainingFaces.push_back(resizedOwner);
                labels.push_back(0);

                // 2. Filtered/Augmented Faces (to handle the radiation ghost effect)
                // Generate several versions with different contrast/brightness for robustness
                for (float gamma = 0.5f; gamma <= 1.5f; gamma += 0.5f) {
                    Mat augmented;
                    resizedOwner.convertTo(augmented, -1, 1.0, gamma * 50 - 50); // Simple exposure/contrast change
                    trainingFaces.push_back(augmented);
                    labels.push_back(0);
                }

                try {
                    recognizer->train(trainingFaces, labels);
                    ownerCaptured = true;
                    statusBar()->showMessage(QString("Owner loaded and augmented with %1 radiation profiles.").arg(trainingFaces.size()));
                    status->setText("Owner loaded. Monitoring for radioactive devils.");
                } catch (const cv::Exception& e) {
                    qDebug() << "OpenCV Training Exception:" << e.what();
                    statusBar()->showMessage("Error during training, capturing new owner...");
                    ownerCaptured = false;
                }
            } else {
                statusBar()->showMessage("owner.jpg exists but could not be read");
                status->setText("Waiting to capture owner face...");
            }
        } else {
            statusBar()->showMessage("No owner.jpg found. Waiting to capture owner face...");
            status->setText("Waiting to capture owner face...");
        }

        // Timer to process frames
        connect(timer, &QTimer::timeout, this, &IntruderDetector::processFrame);
        timer->start(100); // 10 fps processing

        // initial gallery load
        loadGallery();
    }

    // Destructor
    ~IntruderDetector() override {
        if (cap.isOpened()) {
            cap.release();
        }
    }

private:
    // Utility function to apply the specific blur filter with convex hull
    Mat applyRadiationFilter(const Mat& inputFrame) {
        // 1. Create Grayscale Base
        Mat gray;
        cvtColor(inputFrame, gray, COLOR_BGR2GRAY);
        Mat outputFrame;
        cvtColor(gray, outputFrame, COLOR_GRAY2BGR); // Base is monochrome

        // 2. High-Pass Filter (Laplacian) for traces
        Mat laplacian;
        Laplacian(gray, laplacian, CV_16S, 3);
        Mat absLaplacian;
        convertScaleAbs(laplacian, absLaplacian);

        // NEW: Adaptive Thresholding (more reliable for varying light)
        // Adjust threshold based on the average intensity of the radiation traces
        double meanIntensity = mean(absLaplacian)[0];
        int dynamicTraceThreshold = static_cast<int>(min(250.0, max(50.0, meanIntensity * 1.5))); // Ensure bounds

        // 3. Thresholding for Ray/Trace Detection (Isolation)
        Mat traceMask;
        threshold(absLaplacian, traceMask, dynamicTraceThreshold, 255, THRESH_BINARY);

        // NEW: Gamma-Ray Dilation to connect fragmented pixels
        Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
        dilate(traceMask, traceMask, kernel, Point(-1, -1), 1);

        // --- Convex Hull and Alpha Blending ---
        vector<vector<Point>> contours;
        // Use Canny edges on the trace mask to ensure contours are well-defined
        Canny(traceMask, traceMask, 50, 150);
        findContours(traceMask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        Mat transparentHullLayer = Mat::zeros(inputFrame.size(), CV_8UC4); // 4 channels for alpha

        // Define EXTREME neon green with 100% alpha (to dominate the blend)
        Scalar neonGreenWithAlpha = Scalar(0, 255, 0, 255 * 0.95); // BGR + Alpha (95% opaque)

        for (size_t i = 0; i < contours.size(); i++) {
            if (contours[i].size() > 5) {
                vector<Point> hull;
                convexHull(contours[i], hull);

                // Draw and fill the convex hull on the transparent layer
                vector<vector<Point>> hull_vec = {hull};
                fillPoly(transparentHullLayer, hull_vec, neonGreenWithAlpha);
            }
        }

        // Blend the transparent hull layer onto the output frame
        Mat blendedOutput = Mat::zeros(inputFrame.size(), CV_8UC3);

        for (int y = 0; y < outputFrame.rows; ++y) {
            for (int x = 0; x < outputFrame.cols; ++x) {
                Vec3b backgroundPx = outputFrame.at<Vec3b>(y, x);
                Vec4b foregroundPx = transparentHullLayer.at<Vec4b>(y, x);

                double alpha_fg = foregroundPx[3] / 255.0; // Alpha of the hull (approx 0.95)
                double alpha_bg = 1.0 - alpha_fg;

                for (int c = 0; c < 3; ++c) {
                    blendedOutput.at<Vec3b>(y, x)[c] =
                        static_cast<uchar>(backgroundPx[c] * alpha_bg + foregroundPx[c] * alpha_fg);
                }
            }
        }
        outputFrame = blendedOutput;

        return outputFrame;
    }

private slots:
    void processFrame() {
        Mat frame;
        cap >> frame;

        if (frame.empty()) {
            statusBar()->showMessage(cap.isOpened() ? "Warning: Frame empty, camera may be disconnected." : "Error: Camera is not open!");
            return;
        }

        Mat filteredDisplay = applyRadiationFilter(frame);
        Mat gray;
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        Mat display = filteredDisplay.clone();

        vector<Rect> faces;
        // Face detection parameters adjusted for better general detection
        faceCascade.detectMultiScale(gray, faces, 1.2, 3, CASCADE_SCALE_IMAGE, Size(100,100)); // Min size increased to 100x100

        // Owner Capture Logic
        if (!ownerCaptured && !faces.empty()) {
            Rect f = faces[0];
            Mat faceROI = gray(f).clone();
            cv::resize(faceROI, faceROI, Size(200,200));

            // NEW: Save the owner and train the augmented model immediately
            if (imwrite("owner.jpg", faceROI)) {
                // Re-train the model with augmented data (same logic as in constructor)
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
                    ownerCaptured = true;
                    statusBar()->showMessage("Owner captured and augmented. Monitoring for radioactive devils.");
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
            rectangle(display, r, Scalar(0,255,0), 2);

            if (ownerCaptured) {
                if (r.x >= 0 && r.y >= 0 && r.x + r.width <= gray.cols && r.y + r.height <= gray.rows) {
                    Mat faceROI = gray(r).clone();
                    cv::resize(faceROI, faceROI, Size(200,200));
                    int label = -1;
                    double confidence = 0.0;

                    if (recognizer->empty()) continue;

                    recognizer->predict(faceROI, label, confidence);
                    if (confidence > thresholdDistance) {
                        intruderCurrentlyPresent = true;
                        string text = format("NUCLEAR DEVIL DETECTED (%.1f)", confidence);
                        putText(display, text, Point(r.x, r.y - 10), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0,0,255), 2);
                        rectangle(display, r, Scalar(0,0,255), 3); // Red box for intruder

                        // Capture the current filtered frame to save later
                        currentIntruderFrame = filteredDisplay.clone();
                        // Annotate the captured frame
                        rectangle(currentIntruderFrame, r, Scalar(0,0,255), 3);
                        putText(currentIntruderFrame, text, Point(r.x, r.y - 10), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0,0,255), 2);

                        break;
                    } else {
                        // owner recognized
                        string txt = format("Owner (%.1f)", confidence);
                        putText(display, txt, Point(r.x, r.y - 6), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,255,0), 1);
                    }
                }
            } else {
                putText(display, "Candidate Owner", Point(r.x, r.y - 6), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,255,0), 1);
            }
        }

        // Intruder State Machine: Save only one photo after 10s absence
        if (intruderCurrentlyPresent) {
            if (!intruderDetected) {
                intruderDetected = true;
                intruderDisappearedTimer.restart();
                statusBar()->showMessage("NUCLEAR DEVIL DETECTED! Stand by for photo...");
            }
        } else if (intruderDetected) {
            if (!savingIntruder) {
                if (intruderDisappearedTimer.elapsed() < 0) {
                    intruderDisappearedTimer.restart();
                }

                long long elapsed = intruderDisappearedTimer.elapsed();
                long long remaining = 10000 - elapsed;

                if (elapsed >= 10000) {
                    saveIntruderPhoto();
                    intruderDetected = false;
                    savingIntruder = false;
                } else {
                    statusBar()->showMessage(QString("Devil gone. Photo saving in: %1 s...").arg(remaining / 1000.0, 0, 'f', 1));
                }
            }
        } else {
            if (ownerCaptured) {
                status->setText("Owner loaded. Monitoring for radioactive devils.");
            } else {
                status->setText("Waiting to capture owner face...");
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
            currentIntruderFrame = Mat(); // Clear frame buffer
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

    // Trace threshold removed, now dynamically calculated
    // int traceThreshold = 180;

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

