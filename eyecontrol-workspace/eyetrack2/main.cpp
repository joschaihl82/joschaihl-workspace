// eyetracking.cpp
// EyeTracker with automatic model/device, pupil-based cursor control and 5-point calibration
// Build with qmake (eyetracker.pro) and Qt6, OpenCV, dlib, X11/XTest

#include <opencv2/opencv.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>

#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QVBoxLayout>
#include <QScreen>
#include <QCursor>
#include <QPainter>
#include <QWidget>
#include <QElapsedTimer>
#include <QDateTime>

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

#include <vector>
#include <array>
#include <cmath>
#include <optional>

// fixed paths
static const std::string MODEL_PATH = "/home/joscha/joschaihl-workspace/eyecontrol-workspace/eyetrack2/shapepredictor_68_facelandmarks.dat";
static const std::string CAMERA_DEVICE = "/dev/video0";

// simulate left click via XTest (X11)
static void singleClickMouseX11() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) return;
    XTestFakeButtonEvent(display, 1, True, CurrentTime);
    XTestFakeButtonEvent(display, 1, False, CurrentTime);
    XFlush(display);
    XCloseDisplay(display);
}

// EAR helpers
static double calcEAR(const dlib::full_object_detection& shape, int start) {
    cv::Point2f p0(shape.part(start+0).x(), shape.part(start+0).y());
    cv::Point2f p1(shape.part(start+1).x(), shape.part(start+1).y());
    cv::Point2f p2(shape.part(start+2).x(), shape.part(start+2).y());
    cv::Point2f p3(shape.part(start+3).x(), shape.part(start+3).y());
    cv::Point2f p4(shape.part(start+4).x(), shape.part(start+4).y());
    cv::Point2f p5(shape.part(start+5).x(), shape.part(start+5).y());
    double A = cv::norm(p1 - p5);
    double B = cv::norm(p2 - p4);
    double C = cv::norm(p0 - p3);
    if (C < 1e-6) return 0.0;
    return (A + B) / (2.0 * C);
}

static cv::Point2f avgEye(const dlib::full_object_detection& shape, int start) {
    cv::Point2f s(0,0);
    for (int i=0;i<6;i++) s += cv::Point2f(shape.part(start+i).x(), shape.part(start+i).y());
    s *= (1.0f/6.0f);
    return s;
}

// find pupil (darkest point) in eye ROI
static bool findPupil(const cv::Mat& frameGray, const std::vector<cv::Point>& eyePoly, cv::Point2f& pupil) {
    cv::Rect bbox = cv::boundingRect(eyePoly);
    bbox &= cv::Rect(0,0,frameGray.cols, frameGray.rows);
    if (bbox.width < 6 || bbox.height < 6) return false;
    cv::Mat roi = frameGray(bbox);
    cv::Mat eq;
    cv::equalizeHist(roi, eq);
    cv::GaussianBlur(eq, eq, cv::Size(7,7), 0);
    cv::Mat inv = 255 - eq;
    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(inv, &minVal, &maxVal, &minLoc, &maxLoc);
    if (maxVal < 40) return false;
    pupil = cv::Point2f(bbox.x + maxLoc.x, bbox.y + maxLoc.y);
    return true;
}

// small 3x3 matrix inverse (for normal equations)
static bool invert3x3(const double m[3][3], double out[3][3]) {
    double a = m[0][0], b = m[0][1], c = m[0][2];
    double d = m[1][0], e = m[1][1], f = m[1][2];
    double g = m[2][0], h = m[2][1], i = m[2][2];
    double A = e*i - f*h;
    double B = -(d*i - f*g);
    double C = d*h - e*g;
    double D = -(b*i - c*h);
    double E = a*i - c*g;
    double F = -(a*h - b*g);
    double G = b*f - c*e;
    double H = -(a*f - c*d);
    double I = a*e - b*d;
    double det = a*A + b*B + c*C;
    if (fabs(det) < 1e-12) return false;
    out[0][0] = A/det; out[0][1] = D/det; out[0][2] = G/det;
    out[1][0] = B/det; out[1][1] = E/det; out[1][2] = H/det;
    out[2][0] = C/det; out[2][1] = F/det; out[2][2] = I/det;
    return true;
}

// compute affine mapping params from samples: target = M * [px py 1]
// returns optional pair of two arrays: paramsX[3], paramsY[3]
static std::optional<std::pair<std::array<double,3>, std::array<double,3>>> computeAffine(
    const std::vector<cv::Point2f>& src, const std::vector<QPointF>& dst)
{
    int N = (int)src.size();
    if (N < 3) return std::nullopt;
    // build normal equations: (A^T A) x = A^T b
    double ATA[3][3] = {{0,0,0},{0,0,0},{0,0,0}};
    double ATbx = {0}, ATby = {0}; // placeholders
    double ATbX[3] = {0,0,0};
    double ATbY[3] = {0,0,0};
    for (int k=0;k<N;k++) {
        double px = src[k].x;
        double py = src[k].y;
        double row[3] = {px, py, 1.0};
        for (int r=0;r<3;r++) for (int c=0;c<3;c++) ATA[r][c] += row[r]*row[c];
        ATbX[0] += row[0]*dst[k].x();
        ATbX[1] += row[1]*dst[k].x();
        ATbX[2] += row[2]*dst[k].x();
        ATbY[0] += row[0]*dst[k].y();
        ATbY[1] += row[1]*dst[k].y();
        ATbY[2] += row[2]*dst[k].y();
    }
    double ATAinv[3][3];
    if (!invert3x3(ATA, ATAinv)) return std::nullopt;
    std::array<double,3> pxParams{0,0,0}, pyParams{0,0,0};
    for (int r=0;r<3;r++) {
        pxParams[r] = ATAinv[r][0]*ATbX[0] + ATAinv[r][1]*ATbX[1] + ATAinv[r][2]*ATbX[2];
        pyParams[r] = ATAinv[r][0]*ATbY[0] + ATAinv[r][1]*ATbY[1] + ATAinv[r][2]*ATbY[2];
    }
    return std::make_pair(pxParams, pyParams);
}

// overlay widget for calibration points (full-screen)
class CalibOverlay : public QWidget {
    Q_OBJECT
public:
    CalibOverlay(const std::vector<QPoint>& points, QWidget* parent = nullptr)
        : QWidget(parent), pts(points), currentIndex(0)
    {
        setWindowFlag(Qt::WindowStaysOnTopHint);
        setWindowFlag(Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_TranslucentBackground);
        showFullScreen();
    }
    void next() { if (currentIndex+1 < (int)pts.size()) ++currentIndex; update(); }
    void prev() { if (currentIndex>0) --currentIndex; update(); }
    int index() const { return currentIndex; }
    QPoint currentPoint() const { return pts[currentIndex]; }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        QColor bg(0,0,0,120);
        p.fillRect(rect(), bg);
        // draw all points faint
        for (size_t i=0;i<pts.size();++i) {
            QColor col = (i==currentIndex) ? QColor(255,0,0) : QColor(255,255,255,80);
            p.setBrush(col);
            p.setPen(Qt::NoPen);
            QPoint c = pts[i];
            p.drawEllipse(c, (i==currentIndex)?18:8, (i==currentIndex)?18:8);
        }
        // instruction
        p.setPen(Qt::white);
        p.setFont(QFont("Sans", 18));
        p.drawText(20,40, QString("Calibration: Look at the red dot. Hold steady..."));
    }
private:
    std::vector<QPoint> pts;
    int currentIndex;
};

// main window: shows camera and runs calibration then tracking
class EyeTrackerWindow : public QMainWindow {
    Q_OBJECT
public:
    EyeTrackerWindow(QWidget* parent = nullptr)
        : QMainWindow(parent),
          label(new QLabel(this)),
          timer(new QTimer(this)),
          cap(CAMERA_DEVICE, cv::CAP_V4L2),
          detector(dlib::get_frontal_face_detector())
    {
        setWindowTitle("EyeTracker (calibrating)");
        resize(900,600);
        label->setAlignment(Qt::AlignCenter);
        setCentralWidget(label);

        // load predictor
        try {
            dlib::deserialize(MODEL_PATH) >> predictor;
            predictorLoaded = true;
        } catch (...) {
            predictorLoaded = false;
        }

        if (!cap.isOpened()) {
            // try open by index 0 fallback
            cap.open(0);
        }
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
        cap.set(cv::CAP_PROP_FPS, 30);

        // parameters
        earThreshold = 0.20;
        blinkFramesThreshold = 2;
        smoothing = 0.18;
        gazeGain = 1.1;

        smoothedCursor = QPointF(QApplication::primaryScreen()->geometry().center());

        // prepare calibration points (5-point: center + 4 corners-ish)
        QRect s = QApplication::primaryScreen()->geometry();
        int w = s.width(), h = s.height();
        calibPoints = {
            QPoint(w/2, h/2),
            QPoint(w/6, h/6),
            QPoint(5*w/6, h/6),
            QPoint(w/6, 5*h/6),
            QPoint(5*w/6, 5*h/6)
        };

        overlay = new CalibOverlay(calibPoints);
        overlay->show();

        // start calibration sequence
        calibIndex = 0;
        samplesPerPoint = 30;
        sampleIntervalMs = 30;
        collectedSrc.clear();
        collectedDst.clear();

        // small delay before starting to allow overlay to appear
        QTimer::singleShot(800, this, &EyeTrackerWindow::startCalibrationStep);
    }

    ~EyeTrackerWindow() override {
        timer->stop();
        if (cap.isOpened()) cap.release();
        if (overlay) { overlay->close(); delete overlay; overlay = nullptr; }
    }

private slots:
    void startCalibrationStep() {
        if (calibIndex >= (int)calibPoints.size()) {
            finishCalibration();
            return;
        }
        // show current point (overlay already shows it)
        currentSamples.clear();
        sampleTimer = new QTimer(this);
        connect(sampleTimer, &QTimer::timeout, this, &EyeTrackerWindow::collectCalibrationSample);
        sampleCount = 0;
        sampleTimer->start(sampleIntervalMs);
    }

    void collectCalibrationSample() {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) return;
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        dlib::cv_image<dlib::bgr_pixel> dlibImg(frame);
        std::vector<dlib::rectangle> faces = detector(dlibImg);
        cv::Point2f pupilPt;
        bool pupilOk = false;
        if (!faces.empty() && predictorLoaded) {
            dlib::full_object_detection shape = predictor(dlibImg, faces[0]);
            std::vector<cv::Point> leftEyePts, rightEyePts;
            for (int i=36;i<42;i++) leftEyePts.emplace_back(shape.part(i).x(), shape.part(i).y());
            for (int i=42;i<48;i++) rightEyePts.emplace_back(shape.part(i).x(), shape.part(i).y());
            cv::Point2f pL, pR;
            bool okL = findPupil(gray, leftEyePts, pL);
            bool okR = findPupil(gray, rightEyePts, pR);
            if (okL && okR) { pupilPt = (pL + pR) * 0.5f; pupilOk = true; }
            else if (okL) { pupilPt = pL; pupilOk = true; }
            else if (okR) { pupilPt = pR; pupilOk = true; }
            else {
                // fallback to eye centers
                cv::Point2f leftC = avgEye(shape,36);
                cv::Point2f rightC = avgEye(shape,42);
                pupilPt = (leftC + rightC) * 0.5f;
                pupilOk = true;
            }
        } else {
            // no face/predictor: skip sample
            pupilOk = false;
        }

        if (pupilOk) {
            currentSamples.push_back(pupilPt);
        }
        sampleCount++;
        // display live frame during calibration
        QImage qimg = matToQImage(frame);
        label->setPixmap(QPixmap::fromImage(qimg).scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

        if (sampleCount >= samplesPerPoint) {
            sampleTimer->stop();
            sampleTimer->deleteLater();
            sampleTimer = nullptr;
            // average collected samples for this point
            if (!currentSamples.empty()) {
                cv::Point2f avg(0,0);
                for (auto &p : currentSamples) avg += p;
                avg *= (1.0f / currentSamples.size());
                collectedSrc.push_back(avg);
                collectedDst.push_back(QPointF(calibPoints[calibIndex]));
            }
            // advance overlay and next step
            calibIndex++;
            if (overlay) overlay->next();
            QTimer::singleShot(300, this, &EyeTrackerWindow::startCalibrationStep);
        }
    }

    void finishCalibration() {
        if (overlay) { overlay->close(); delete overlay; overlay = nullptr; }
        // compute affine mapping
        auto maybe = computeAffine(collectedSrc, collectedDst);
        if (maybe) {
            auto pr = *maybe;
            paramsX = pr.first;
            paramsY = pr.second;
            calibrated = true;
            setWindowTitle("EyeTracker (calibrated)");
        } else {
            calibrated = false;
            setWindowTitle("EyeTracker (calibration failed)");
        }
        // start main tracking timer
        connect(timer, &QTimer::timeout, this, &EyeTrackerWindow::processFrame);
        timer->start(30);
    }

    void processFrame() {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) return;
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        dlib::cv_image<dlib::bgr_pixel> dlibImg(frame);
        std::vector<dlib::rectangle> faces = detector(dlibImg);

        if (!faces.empty() && predictorLoaded) {
            dlib::full_object_detection shape = predictor(dlibImg, faces[0]);

            double leftEAR = calcEAR(shape, 36);
            double rightEAR = calcEAR(shape, 42);
            double ear = std::min(leftEAR, rightEAR);

            if (ear < earThreshold) ++blinkCounter;
            else {
                if (blinkCounter >= blinkFramesThreshold) singleClickMouseX11();
                blinkCounter = 0;
            }

            std::vector<cv::Point> leftEyePts, rightEyePts;
            for (int i=36;i<42;i++) leftEyePts.emplace_back(shape.part(i).x(), shape.part(i).y());
            for (int i=42;i<48;i++) rightEyePts.emplace_back(shape.part(i).x(), shape.part(i).y());

            cv::Point2f pL, pR;
            bool okL = findPupil(gray, leftEyePts, pL);
            bool okR = findPupil(gray, rightEyePts, pR);
            cv::Point2f pupil;
            if (okL && okR) pupil = (pL + pR) * 0.5f;
            else if (okL) pupil = pL;
            else if (okR) pupil = pR;
            else pupil = (avgEye(shape,36) + avgEye(shape,42)) * 0.5f;

            QPointF target;
            if (calibrated) {
                double px = pupil.x, py = pupil.y;
                double tx = paramsX[0]*px + paramsX[1]*py + paramsX[2];
                double ty = paramsY[0]*px + paramsY[1]*py + paramsY[2];
                target = QPointF(tx, ty);
            } else {
                // fallback mapping: relative to face box
                dlib::rectangle f = faces[0];
                float fx = f.left(), fy = f.top(), fw = f.width(), fh = f.height();
                float nx = (pupil.x - fx) / std::max(1.0f, fw);
                float ny = (pupil.y - fy) / std::max(1.0f, fh);
                QRect screen = QApplication::primaryScreen()->geometry();
                float cx = (nx - 0.5f) * gazeGain;
                float cy = (ny - 0.5f) * gazeGain;
                target = QPointF(screen.center().x() + cx * screen.width(), screen.center().y() + cy * screen.height());
            }

            // smooth
            smoothedCursor.setX(smoothedCursor.x() * (1.0 - smoothing) + target.x() * smoothing);
            smoothedCursor.setY(smoothedCursor.y() * (1.0 - smoothing) + target.y() * smoothing);
            QCursor::setPos(smoothedCursor.toPoint());

            // overlay visualization
            for (auto &p : leftEyePts) cv::circle(frame, p, 1, cv::Scalar(0,255,0), -1);
            for (auto &p : rightEyePts) cv::circle(frame, p, 1, cv::Scalar(0,255,0), -1);
            cv::circle(frame, cv::Point((int)pupil.x, (int)pupil.y), 3, cv::Scalar(0,0,255), -1);
            cv::putText(frame, ("EAR:" + std::to_string(ear)).c_str(), cv::Point(10,30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0,255,0), 2);
        }

        QImage qimg = matToQImage(frame);
        label->setPixmap(QPixmap::fromImage(qimg).scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

private:
    QLabel* label;
    QTimer* timer;
    cv::VideoCapture cap;
    dlib::frontal_face_detector detector;
    dlib::shape_predictor predictor;
    bool predictorLoaded = false;

    // calibration
    CalibOverlay* overlay = nullptr;
    std::vector<QPoint> calibPoints;
    int calibIndex = 0;
    int samplesPerPoint = 30;
    int sampleIntervalMs = 30;
    QTimer* sampleTimer = nullptr;
    int sampleCount = 0;
    std::vector<cv::Point2f> currentSamples;
    std::vector<cv::Point2f> collectedSrc;
    std::vector<QPointF> collectedDst;
    std::vector<cv::Point2f> currentSamplesAll;

    // mapping params
    std::array<double,3> paramsX{0,0,0}, paramsY{0,0,0};
    bool calibrated = false;

    // blink/click
    int blinkCounter = 0;
    double earThreshold;
    int blinkFramesThreshold;

    // gaze mapping
    double smoothing;
    double gazeGain;
    QPointF smoothedCursor;

    QImage matToQImage(const cv::Mat& mat) {
        cv::Mat rgb;
        if (mat.channels() == 3) {
            cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
            return QImage((const uchar*)rgb.data, rgb.cols, rgb.rows, static_cast<int>(rgb.step), QImage::Format_RGB888).copy();
        } else if (mat.channels() == 1) {
            return QImage((const uchar*)mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_Grayscale8).copy();
        } else {
            return QImage();
        }
    }
};

#include "main.moc"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    EyeTrackerWindow w;
    w.show();
    return app.exec();
}

