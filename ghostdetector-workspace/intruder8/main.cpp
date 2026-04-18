// intruder.cpp
// Simple, Fast, Single-Threaded Intruder Detector (Motion + Face Detection Only)
// Focus: HIGH FRAME RATE & SIMPLICITY

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
        // Faster QImage creation without copy()
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
        setWindowTitle("Intruder Detector: Simple & Fast Monitoring");

        // Vollbildansicht beim Start beibehalten
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

        // Layout-Ränder entfernen
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        setCentralWidget(central);
        statusBar()->showMessage("Starting...");

        QDir().mkpath("intruders");

        QString cascadePath = "haarcascade_frontalface_default.xml";
        if (!QFile::exists(cascadePath)) {
            // Versuche den Standardpfad auf Linux-Systemen
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
        cap.set(CAP_PROP_FPS, 60); // Versuch, höhere FPS zu setzen

        // Timer auf 16ms (ca. 60 FPS) gesetzt
        connect(timer, &QTimer::timeout, this, &IntruderDetector::processFrame);
        timer->start(16);
    }

    ~IntruderDetector() override {
        if (cap.isOpened()) {
            cap.release();
        }
    }

private slots:
    void processFrame() {
        Mat frame;
        cap >> frame;

        if (frame.empty()) {
            statusBar()->showMessage(cap.isOpened() ? "Warning: Frame empty." : "Error: Camera not open!");
            return;
        }

        // --- 1. Vorverarbeitung ---
        Mat currentGray;
        cvtColor(frame, currentGray, COLOR_BGR2GRAY);

        Mat blurredGray;
        // Leichte Unschärfe für die Bewegungserkennung
        GaussianBlur(currentGray, blurredGray, Size(21, 21), 0);

        if (previousGray.empty()) {
            previousGray = blurredGray.clone();
            return;
        }

        // --- 2. Bewegungserkennung (Schnell!) ---
        Mat diff;
        absdiff(previousGray, blurredGray, diff);
        threshold(diff, diff, 25, 255, THRESH_BINARY);

        vector<vector<Point>> motionContours;
        // Konturerkennung ist der langsamste Teil der Bewegungserkennung,
        // wird aber für die Flächenberechnung benötigt.
        findContours(diff, motionContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        motionDetected = false;
        bool largeMotion = false;
        for (const auto& c : motionContours) {
            if (contourArea(c) > 5000) { // Größere Bewegung
                largeMotion = true;
                break;
            } else if (contourArea(c) > minMotionArea) { // Normale Bewegung
                motionDetected = true;
            }
        }
        blurredGray.copyTo(previousGray);

        Mat display = frame.clone();
        bool intruderPresent = false;

        // --- 3. Gesichtsdetektion (Nur bei Bewegung oder großer Bewegung) ---
        vector<Rect> faces;
        if (motionDetected || largeMotion) {
            // Nur alle 5 Frames bei kleiner Bewegung, bei großer Bewegung immer
            if (largeMotion || (frameCounter % 5 == 0)) {
                faceCascade.detectMultiScale(currentGray, faces, 1.1, 5, 0, Size(80,80));
            }
        }
        frameCounter++;

        // --- 4. Markierung und Status ---
        for (const Rect &r : faces) {
            // Wenn ein Gesicht gefunden wird, ist es ein Eindringling (da kein Besitzer trainiert wurde)
            rectangle(display, r, Scalar(0,0,255), 3); // ROT
            putText(display, "INTRUDER DETECTED!", Point(r.x, r.y - 10), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,0,255), 2);
            intruderPresent = true;
            // Foto speichern (optional)
            if (largeMotion && !savingIntruder) {
                savingIntruder = true;
                saveIntruderPhoto(frame, r);
            }
        }

        // Status-Update
        QString currentStatus;
        if (intruderPresent) {
            currentStatus = "🚨 **INTRUDER DETECTED!** (Saving Photo)";
            statusBar()->showMessage("Intruder face captured!", 5000);
        } else if (motionDetected) {
            currentStatus = "⚠️ **MOTION DETECTED** (Monitoring for face)";
        } else {
            currentStatus = "✅ **System Idle**";
        }
        status->setText(currentStatus);

        // --- 5. Display Update (sehr schnell) ---
        QImage qimg = matToQImage(display);
        if (!qimg.isNull()) {
            previewLabel->setPixmap(QPixmap::fromImage(qimg).scaled(previewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

    void saveIntruderPhoto(const Mat& frame, const Rect& faceRect) {
        string ts = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss").toStdString();
        char fname[128];
        snprintf(fname, sizeof(fname), "intruders/intruder_%s.jpg", ts.c_str());

        Mat markedFrame = frame.clone();
        rectangle(markedFrame, faceRect, Scalar(0,0,255), 3);

        if (imwrite(fname, markedFrame)) {
            qDebug() << "Saved intruder photo:" << fname;
        } else {
            qDebug() << "Failed to save intruder image";
        }
        savingIntruder = false;
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
    bool savingIntruder = false;
    int frameCounter = 0;
};

// =========================================================================
// CRITICAL FIX: The MOC include MUST be here to define vtable and staticMetaObject.
// Passen Sie den Namen an Ihre Haupt-Quelldatei an (z.B. main.moc)
// =========================================================================

#include "main.moc"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("SimpleIntruderDetector");

    IntruderDetector w;
    w.show();
    return a.exec();
}
