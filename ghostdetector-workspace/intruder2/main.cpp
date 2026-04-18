// intruder.cpp
// Single-file Qt5 + OpenCV intruder detector
// - captures first detected face as owner.jpg
// - then detects intruders and saves images to intruders/
// - applies a neon-green "Radiation Blur" filter to simulate X-rays, Gamma-rays, and Delta-rays.
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
#include <QElapsedTimer> // Needed for the 10-second delay logic

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
    if (mat.type() == CV_8UC3) {
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, COLOR_BGR2RGB);
        return QImage((const uchar*)rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
    } else if (mat.type() == CV_8UC1) {
        return QImage((const uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
    } else if (mat.type() == CV_8UC4) {
        return QImage((const uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32).copy();
    }
    return QImage();
}

class IntruderDetector : public QMainWindow {
    Q_OBJECT
public:
    IntruderDetector(QWidget *parent = nullptr) : QMainWindow(parent),
        timer(new QTimer(this)), ownerCaptured(false), thresholdDistance(60.0),
        intruderDetected(false), // New initialization
        savingIntruder(false) // New initialization
    {
        setWindowTitle("Intruder Detector");
        resize(1000, 700);

        // UI: left = live preview, right = gallery
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

        // Ensure intruders directory exists
        QDir().mkpath("intruders");

        // Load Haar cascade
        QString cascadePath = "haarcascade_frontalface_default.xml";
        if (!QFile::exists(cascadePath)) {
            cascadePath = "/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml";
        }

        if (!faceCascade.load(cascadePath.toStdString())) {
            QMessageBox::critical(this, "Error", "Failed to load Haar cascade. Please ensure 'haarcascade_frontalface_default.xml' is available.");
        }

        // Open webcam (Using CAP_V4L2 for better Linux compatibility)
        cap.open(0, CAP_V4L2);
        if (!cap.isOpened()) {
            QMessageBox::critical(this, "Error", "Cannot open webcam. Check device index (0) and permissions.");
            qApp->exit(1);
            return;
        }

        // set reasonable capture size
        cap.set(CAP_PROP_FRAME_WIDTH, 640);
        cap.set(CAP_PROP_FRAME_HEIGHT, 480);
        cap.set(CAP_PROP_FPS, 25);

        // Prepare recognizer
        recognizer = LBPHFaceRecognizer::create();

        // Initial check for 'owner.jpg'
        if (QFileInfo::exists("owner.jpg")) {
            Mat owner = imread("owner.jpg", IMREAD_GRAYSCALE);
            if (!owner.empty()) {
                cv::resize(owner, owner, Size(200,200));
                try {
                    recognizer->train(vector<Mat>{owner}, vector<int>{0});
                    ownerCaptured = true;
                    statusBar()->showMessage("Owner loaded from owner.jpg");
                    status->setText("Owner loaded. Monitoring for intruders.");
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

    // Fix: Destructor to ensure the webcam resource is released gracefully
    ~IntruderDetector() override {
        if (cap.isOpened()) {
            cap.release();
        }
    }

private:
    // New utility function to apply the specific blur filter
    Mat applyRadiationFilter(const Mat& inputFrame) {

        // 1. Create Grayscale Base (General ambiance)
        Mat gray;
        cvtColor(inputFrame, gray, COLOR_BGR2GRAY);
        Mat outputFrame;
        cvtColor(gray, outputFrame, COLOR_GRAY2BGR); // Base is monochrome

        // 2. High-Pass Filter Simulation (Highlighting energetic traces like Gamma/Delta rays)
        Mat laplacian;
        Laplacian(gray, laplacian, CV_16S, 3);
        Mat absLaplacian;
        convertScaleAbs(laplacian, absLaplacian);

        // 3. Thresholding for Ray/Trace Detection (Isolation)
        Mat traceMask;
        threshold(absLaplacian, traceMask, traceThreshold, 255, THRESH_BINARY);

        // 4. Blur the Traces (The "Neon Blur" effect)
        Mat blurredTraces;
        int blurSize = 5;
        GaussianBlur(traceMask, blurredTraces, Size(blurSize, blurSize), 0);

        // 5. Color and Combine the Blur (The Neon-Green effect)
        Mat neonGreenTrace(inputFrame.size(), CV_8UC3, Scalar(0, 255, 0));
        Mat coloredTraces;
        neonGreenTrace.copyTo(coloredTraces, blurredTraces);

        // 6. Blend the Neon Blur onto the Grayscale Base
        double alpha = 0.5;
        addWeighted(outputFrame, 1.0, coloredTraces, alpha, 0.0, outputFrame);

        // 7. Simulating X-ray/Gamma-ray Density (A lighter, diffused glow)
        Mat diffusedGlow;
        GaussianBlur(gray, diffusedGlow, Size(5, 5), 0);
        cvtColor(diffusedGlow, diffusedGlow, COLOR_GRAY2BGR);

        // Blend a subtle green wash over the entire frame (representing overall radiation)
        Mat greenWash(inputFrame.size(), CV_8UC3, Scalar(0, 100, 0));
        greenWash.copyTo(greenWash, diffusedGlow);
        addWeighted(outputFrame, 1.0, greenWash, 0.1, 0.0, outputFrame);

        return outputFrame;
    }

private slots:
    void processFrame() {
        Mat frame;
        cap >> frame;

        if (frame.empty() || frame.cols == 0 || frame.rows == 0) {
            // ... (error handling remains unchanged) ...
            if (cap.isOpened()) {
                statusBar()->showMessage("Warning: Frame empty, camera may be disconnected.");
            } else {
                statusBar()->showMessage("Error: Camera is not open!");
            }
            return;
        }

        Mat filteredDisplay = applyRadiationFilter(frame);
        Mat gray;
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        Mat display = filteredDisplay.clone();

        vector<Rect> faces;
        faceCascade.detectMultiScale(gray, faces, 1.1, 4, CASCADE_SCALE_IMAGE, Size(80,80));

        // Owner Capture Logic (Unchanged)
        if (!ownerCaptured && !faces.empty()) {
            Rect f = faces[0];
            if (f.x >= 0 && f.y >= 0 && f.x + f.width <= gray.cols && f.y + f.height <= gray.rows) {
                Mat faceROI = gray(f).clone();
                cv::resize(faceROI, faceROI, Size(200,200));
                if (imwrite("owner.jpg", faceROI)) {
                    recognizer->train(vector<Mat>{faceROI}, vector<int>{0});
                    ownerCaptured = true;
                    statusBar()->showMessage("Owner captured and saved to owner.jpg");
                    status->setText("Owner captured. Monitoring for intruders.");
                } else {
                    statusBar()->showMessage("Failed to save owner.jpg. Check write permissions.");
                }
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

                    if (recognizer->empty()) {
                        putText(display, "Training error", Point(r.x, r.y - 6), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,255), 1);
                        continue;
                    }

                    recognizer->predict(faceROI, label, confidence);
                    if (confidence > thresholdDistance) {
                        intruderCurrentlyPresent = true;
                        string text = format("INTRUDER (%.1f)", confidence);
                        putText(display, text, Point(r.x, r.y - 10), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0,0,255), 2);
                        rectangle(display, r, Scalar(0,0,255), 3); // Red box for intruder

                        // Capture the current filtered frame to save later
                        currentIntruderFrame = filteredDisplay.clone();
                        // Annotate the captured frame with the final red box before saving
                        rectangle(currentIntruderFrame, r, Scalar(0,0,255), 3);
                        putText(currentIntruderFrame, text, Point(r.x, r.y - 10), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0,0,255), 2);

                        break; // Stop checking faces, we found an intruder
                    } else {
                        // owner recognized
                        string txt = format("owner (%.1f)", confidence);
                        putText(display, txt, Point(r.x, r.y - 6), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,255,0), 1);
                    }
                }
            } else {
                putText(display, "candidate owner", Point(r.x, r.y - 6), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,255,0), 1);
            }
        }

        // Intruder State Machine: Save only one photo after 10s absence
        if (intruderCurrentlyPresent) {
            if (!intruderDetected) {
                // Intruder just appeared
                intruderDetected = true;
                intruderDisappearedTimer.restart(); // Start timer, but only use it for tracking absence
                statusBar()->showMessage("Intruder detected! Stand by for photo...");
            }
            // If present, reset the disappearance timeout implicitly by not entering the 'else' block
        } else if (intruderDetected) {
            // Intruder was detected, but is now gone
            if (!savingIntruder) {
                // If it's the first frame they're gone, start the 10s countdown
                if (intruderDisappearedTimer.elapsed() < 0) { // First time in this block after detection
                    intruderDisappearedTimer.restart();
                }

                long long elapsed = intruderDisappearedTimer.elapsed();
                long long remaining = 10000 - elapsed; // 10 seconds in milliseconds

                if (elapsed >= 10000) {
                    // 10 seconds have passed since they left, save the frame.
                    saveIntruderPhoto();
                    intruderDetected = false; // Reset state machine
                    savingIntruder = false;
                } else {
                    // Display countdown
                    statusBar()->showMessage(QString("Intruder gone. Photo saving in: %1 s...").arg(remaining / 1000.0, 0, 'f', 1));
                }
            }
        } else {
            // Normal monitoring state
            if (ownerCaptured) {
                status->setText("Owner loaded. Monitoring for intruders.");
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

    void saveIntruderPhoto() {
        if (currentIntruderFrame.empty()) return;

        savingIntruder = true;

        string ts = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss").toStdString();
        char fname[128];
        snprintf(fname, sizeof(fname), "intruders/intruder_%s.jpg", ts.c_str());

        if (imwrite(fname, currentIntruderFrame)) {
            qDebug() << "Saved delayed intruder photo:" << fname;
            statusBar()->showMessage(QString("Intruder photo saved after 10s delay: %1").arg(fname), 5000);
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
        // Sort by time in descending order (newest first)
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
        const int cols = 1; // CHANGE: Only 1 column for maximum size
        const int maxThumbs = 5; // Limiting to 5 for practical display
        const int thumbWidth = 300; // CHANGE: Max thumbnail width
        const int thumbHeight = 225; // CHANGE: Max thumbnail height (4:3 ratio)

        for (const QString &f : files) {
            if (row * cols + col >= maxThumbs) break;

            QLabel *thumb = new QLabel;
            thumb->setParent(galleryWidget);

            // Set thumbnail size to be as big as possible (300x225)
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

    // Intensity threshold for detecting sharp features (high-energy traces)
    int traceThreshold = 180;

    // --- NEW MEMBERS FOR DELAYED SAVE LOGIC ---
    QElapsedTimer intruderDisappearedTimer; // Tracks time since last intrusion
    bool intruderDetected;                  // Tracks if an intrusion has occurred (waiting for save)
    bool savingIntruder;                    // Prevents re-entering save logic multiple times
    Mat currentIntruderFrame;               // Holds the frame to be saved after the delay
};

#include "main.moc"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("IntruderDetector");

    IntruderDetector w;
    w.show();
    return a.exec();
}
