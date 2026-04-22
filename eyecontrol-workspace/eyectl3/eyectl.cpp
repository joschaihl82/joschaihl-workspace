// eyectl.cpp
// Integrated, fixed version for compilation:
// - All required headers included
// - BiquadNotch declared before use
// - createEnlargedEyePatch forward-declared before worker class
// - QPushButton included and used correctly
// - SmoothCursor::update accepts QPointF (GUI-friendly)
// - Minor API mismatches fixed (mappingMat.empty() checks, conversions)
// - No pyramid blending, no CUDA, no ONNX
//
// Build notes:
// - Qt6, OpenCV4, FFTW3
// - Link with: -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -lopencv_objdetect -lopencv_calib3d -lfftw3 -lpthread
// - qmake && make

#include <QApplication>
#include <QLabel>
#include <QThread>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QCursor>
#include <QScreen>
#include <QPainter>
#include <QFont>
#include <QElapsedTimer>
#include <QDateTime>
#include <QWidget>
#include <QDebug>
#include <QMetaType>
#include <QMouseEvent>
#include <QWindow>
#include <QPushButton>
#include <QMutex>
#include <QMutexLocker>

#include <vector>
#include <deque>
#include <algorithm>
#include <cmath>
#include <random>
#include <sstream>
#include <chrono>
#include <mutex>

#include <opencv2/opencv.hpp>
#include <fftw3.h>

    using namespace cv;

// -----------------------------
// Parameters
// -----------------------------
static const double INTERNAL_SR_SCALE = 2.0;
static const int TARGET_EYE_W = 320;
static const int TARGET_EYE_H = 240;

static const double KALMAN_PROCESS_NOISE = 1e-2;
static const double KALMAN_MEASUREMENT_NOISE = 1e-1;
static const double EXP_SMOOTH_ALPHA = 0.25;
static const int MEDIAN_FILTER_WINDOW = 5;

static const int DWELL_MS = 800;
static const int BLINK_CLICK_MIN_MS = 80;
static const int BLINK_CLICK_MAX_MS = 400;

static const std::string CASCADE_BASE = "/usr/share/opencv4/haarcascades/";
static const std::string FACE_CASCADE_FILE = CASCADE_BASE + "haarcascade_frontalface_default.xml";
static const std::string EYE_CASCADE_FILE  = CASCADE_BASE + "haarcascade_eye.xml";
static const std::string LEFTEYE_FILE      = CASCADE_BASE + "haarcascade_lefteye_2splits.xml";
static const std::string RIGHTEYE_FILE     = CASCADE_BASE + "haarcascade_righteye_2splits.xml";
static const std::string EYE_GLASSES_FILE  = CASCADE_BASE + "haarcascade_eye_tree_eyeglasses.xml";

// -----------------------------
// Small types
struct SpectralInfo {
    double fs;
    int N;
    std::vector<double> freqs;
    std::vector<double> magPupilX;
    std::vector<double> magPupilY;
    double peakFreqPupilX;
    double peakFreqPupilY;
    double bandEnergyLowPupilX;
    double bandEnergyHighPupilX;
};
Q_DECLARE_METATYPE(SpectralInfo)

// -----------------------------
// Forward declarations
static Mat createEnlargedEyePatch(const Mat &frame, const Rect &eyeBoxInitial, double internalScale = INTERNAL_SR_SCALE, int targetW = TARGET_EYE_W, int targetH = TARGET_EYE_H);

// -----------------------------
// Utility
template<typename T>
static T clampT(T v, T lo, T hi) { return (v < lo) ? lo : (v > hi) ? hi : v; }

// -----------------------------
// BiquadNotch (must be declared before worker)
class BiquadNotch {
public:
    BiquadNotch() { reset(); }
    void setup(double fs, double f0, double Q) {
        if (fs <= 0 || f0 <= 0 || Q <= 0) { bypass = true; return; }
        bypass = false;
        double w0 = 2.0 * M_PI * f0 / fs;
        double alpha = sin(w0) / (2.0 * Q);
        double cosw0 = cos(w0);

        double b0 = 1.0;
        double b1 = -2.0 * cosw0;
        double b2 = 1.0;
        double a0 = 1.0 + alpha;
        double a1 = -2.0 * cosw0;
        double a2 = 1.0 - alpha;

        this->b0 = b0 / a0;
        this->b1 = b1 / a0;
        this->b2 = b2 / a0;
        this->a1 = a1 / a0;
        this->a2 = a2 / a0;
    }
    double process(double x) {
        if (bypass) return x;
        double y = b0 * x + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
        x2 = x1; x1 = x;
        y2 = y1; y1 = y;
        return y;
    }
    void reset() { x1 = x2 = y1 = y2 = 0.0; bypass = true; b0=b1=b2=a1=a2=0.0; }

private:
    double b0=0,b1=0,b2=0,a1=0,a2=0;
    double x1=0,x2=0,y1=0,y2=0;
    bool bypass=true;
};

// -----------------------------
// Eye refinement
static Rect refineEyeBox(const Mat &frame, const Rect &eyeBox) {
    if (frame.empty() || eyeBox.width <= 8 || eyeBox.height <= 8) return eyeBox;
    Rect r = eyeBox & Rect(0,0,frame.cols, frame.rows);
    Mat crop = frame(r).clone();
    Mat gray; cvtColor(crop, gray, COLOR_BGR2GRAY);
    equalizeHist(gray, gray);
    Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8,8));
    Mat cl; clahe->apply(gray, cl);
    GaussianBlur(cl, cl, Size(5,5), 0);
    Mat th; adaptiveThreshold(cl, th, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 15, 7);
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3,3));
    morphologyEx(th, th, MORPH_OPEN, kernel);
    morphologyEx(th, th, MORPH_CLOSE, kernel);
    std::vector<std::vector<Point>> contours;
    findContours(th, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    Point2f cropCenter(crop.cols*0.5f, crop.rows*0.5f);
    double bestScore = 0.0;
    Rect bestRect;
    for (const auto &c : contours) {
        double area = contourArea(c);
        if (area < 20) continue;
        Rect br = boundingRect(c);
        Point2f ccenter(br.x + br.width*0.5f, br.y + br.height*0.5f);
        double dist = norm(ccenter - cropCenter);
        double score = area / (1.0 + dist);
        if (score > bestScore) { bestScore = score; bestRect = br; }
    }
    if (bestScore > 0.0) {
        int ex = std::max(2, int(bestRect.width * 0.6));
        int ey = std::max(2, int(bestRect.height * 0.8));
        Rect refined(bestRect.x - ex, bestRect.y - ey, bestRect.width + 2*ex, bestRect.height + 2*ey);
        refined &= Rect(0,0,crop.cols,crop.rows);
        Rect globalRefined(refined.x + r.x, refined.y + r.y, refined.width, refined.height);
        globalRefined &= Rect(0,0,frame.cols, frame.rows);
        if (globalRefined.area() >= 16) return globalRefined;
    }
    int padX = std::max(2, int(r.width * 0.2));
    int padY = std::max(2, int(r.height * 0.2));
    Rect fallback(r.x - padX, r.y - padY, r.width + 2*padX, r.height + 2*padY);
    fallback &= Rect(0,0,frame.cols, frame.rows);
    return fallback;
}

// -----------------------------
// Pupil estimation
static bool estimatePupilCenter(const Mat &eyeCropBGR, Point2f &outCenter, double &outArea) {
    outCenter = Point2f(-1,-1); outArea = 0.0;
    if (eyeCropBGR.empty()) return false;
    Mat gray; cvtColor(eyeCropBGR, gray, COLOR_BGR2GRAY);
    Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8,8));
    Mat cl; clahe->apply(gray, cl);
    GaussianBlur(cl, cl, Size(5,5), 0);
    Mat th; adaptiveThreshold(cl, th, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 15, 7);
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3,3));
    morphologyEx(th, th, MORPH_OPEN, kernel);
    morphologyEx(th, th, MORPH_CLOSE, kernel);
    std::vector<std::vector<Point>> contours;
    findContours(th, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    double bestScore = 0.0;
    Point2f bestCenter(-1,-1);
    double bestArea = 0.0;
    Point2f cropCenter(eyeCropBGR.cols*0.5f, eyeCropBGR.rows*0.5f);
    for (const auto &c : contours) {
        double area = contourArea(c);
        if (area < 10) continue;
        Moments m = moments(c);
        if (m.m00 == 0) continue;
        Point2f ccenter((float)(m.m10/m.m00), (float)(m.m01/m.m00));
        double dist = norm(ccenter - cropCenter);
        double score = area / (1.0 + dist);
        if (score > bestScore) { bestScore = score; bestCenter = ccenter; bestArea = area; }
    }
    if (bestScore > 0.0) {
        outCenter = bestCenter;
        outArea = bestArea;
        return true;
    }
    Mat floatGray; cl.convertTo(floatGray, CV_32F, 1.0/255.0);
    double sumI = 0.0; double mx = 0.0, my = 0.0;
    for (int y=0;y<floatGray.rows;++y) {
        const float* row = floatGray.ptr<float>(y);
        for (int x=0;x<floatGray.cols;++x) {
            float v = 1.0f - row[x];
            if (v < 0.01f) continue;
            sumI += v;
            mx += v * x;
            my += v * y;
        }
    }
    if (sumI > 1e-6) {
        outCenter = Point2f((float)(mx/sumI), (float)(my/sumI));
        outArea = 0.0;
        return true;
    }
    return false;
}

// -----------------------------
// Super-resolution (moderate)
static bool superResolveClassical(const Mat &in, Mat &out, double scale = INTERNAL_SR_SCALE) {
    if (in.empty()) return false;
    if (scale <= 1.0) { out = in.clone(); return true; }
    Mat up;
    Size upSize(int(std::round(in.cols * scale)), int(std::round(in.rows * scale)));
    try { resize(in, up, upSize, 0, 0, INTER_LANCZOS4); } catch (...) { return false; }
    Mat den;
    try {
        bilateralFilter(up, den, 9, 75.0, 75.0);
    } catch (...) { den = up.clone(); }
    int k = 3; if (k % 2 == 0) k += 1;
    Mat blurred; GaussianBlur(den, blurred, Size(k,k), 0);
    Mat den32, blurred32; den.convertTo(den32, CV_32F); blurred.convertTo(blurred32, CV_32F);
    Mat detail = den32 - blurred32;
    Mat sharpen32 = den32 + float(1.0) * detail;
    Mat sharpen; sharpen32.convertTo(sharpen, den.type());
    threshold(sharpen, sharpen, 255, 255, THRESH_TRUNC);
    threshold(sharpen, sharpen, 0, 0, THRESH_TOZERO);
    Mat lab; cvtColor(sharpen, lab, COLOR_BGR2Lab);
    std::vector<Mat> labPlanes; split(lab, labPlanes);
    try {
        Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8,8));
        Mat lclahe; clahe->apply(labPlanes[0], lclahe);
        labPlanes[0] = lclahe; merge(labPlanes, lab);
        Mat finalBGR; cvtColor(lab, finalBGR, COLOR_Lab2BGR); out = finalBGR;
    } catch (...) { out = sharpen; }
    return true;
}

// -----------------------------
// Resize to fixed target without distortion
static Mat resizeToFixedTarget(const Mat &src, int targetW, int targetH) {
    if (src.empty()) return Mat();
    double scaleW = double(targetW) / double(src.cols);
    double scaleH = double(targetH) / double(src.rows);
    double scale = std::max(scaleW, scaleH);
    Size scaledSize(int(std::round(src.cols * scale)), int(std::round(src.rows * scale)));
    Mat scaled;
    resize(src, scaled, scaledSize, 0, 0, INTER_LANCZOS4);
    int x = std::max(0, (scaled.cols - targetW) / 2);
    int y = std::max(0, (scaled.rows - targetH) / 2);
    Rect cropRect(x, y, std::min(targetW, scaled.cols - x), std::min(targetH, scaled.rows - y));
    Mat cropped = scaled(cropRect).clone();
    if (cropped.cols != targetW || cropped.rows != targetH) {
        Mat out(targetH, targetW, src.type(), Scalar(0,0,0));
        int ox = (targetW - cropped.cols) / 2;
        int oy = (targetH - cropped.rows) / 2;
        cropped.copyTo(out(Rect(ox, oy, cropped.cols, cropped.rows)));
        return out;
    }
    return cropped;
}

// -----------------------------
// Calibrator (simple least-squares mapping)
class Calibrator {
public:
    Calibrator() : collecting(false), mappingReady(false) {}
    void startCalibration(const QSize &screenSize, int points = 9) {
        std::lock_guard<std::mutex> lk(mutex);
        samples.clear(); targets.clear();
        screenW = screenSize.width(); screenH = screenSize.height();
        collecting = true;
        calibPoints.clear();
        int grid = (int)std::round(std::sqrt(points));
        if (grid < 2) grid = 2;
        for (int y=0;y<grid;++y) for (int x=0;x<grid;++x) {
                double sx = (x + 0.5) / grid;
                double sy = (y + 0.5) / grid;
                calibPoints.emplace_back(Point2f((float)(sx*screenW), (float)(sy*screenH)));
            }
        currentIndex = 0;
    }
    void stopCalibration() {
        std::lock_guard<std::mutex> lk(mutex);
        collecting = false;
    }
    bool isCollecting() {
        std::lock_guard<std::mutex> lk(mutex);
        return collecting;
    }
    Point2f currentTarget() {
        std::lock_guard<std::mutex> lk(mutex);
        if (currentIndex < (int)calibPoints.size()) return calibPoints[currentIndex];
        return Point2f(-1,-1);
    }
    void advanceTarget() {
        std::lock_guard<std::mutex> lk(mutex);
        if (currentIndex < (int)calibPoints.size()) ++currentIndex;
    }
    void addSample(const Point2f &eyeNorm, const Point2f &headNorm) {
        std::lock_guard<std::mutex> lk(mutex);
        if (!collecting) return;
        if (currentIndex >= (int)calibPoints.size()) return;
        samples.push_back({eyeNorm.x, eyeNorm.y, headNorm.x, headNorm.y});
        targets.push_back(calibPoints[currentIndex]);
    }
    bool computeMapping(Mat &affineOut) {
        std::lock_guard<std::mutex> lk(mutex);
        if (samples.size() < 6 || targets.size() < 6) return false;
        Mat src((int)samples.size(), 4, CV_64F);
        Mat dst((int)targets.size(), 2, CV_64F);
        for (size_t i=0;i<samples.size();++i) {
            src.at<double>((int)i,0) = samples[i][0];
            src.at<double>((int)i,1) = samples[i][1];
            src.at<double>((int)i,2) = samples[i][2];
            src.at<double>((int)i,3) = samples[i][3];
            dst.at<double>((int)i,0) = targets[i].x;
            dst.at<double>((int)i,1) = targets[i].y;
        }
        Mat srcAug((int)samples.size(), 5, CV_64F);
        for (int i=0;i<src.rows;++i) {
            for (int j=0;j<4;++j) srcAug.at<double>(i,j) = src.at<double>(i,j);
            srcAug.at<double>(i,4) = 1.0;
        }
        Mat A;
        bool ok = solve(srcAug, dst, A, DECOMP_SVD);
        if (!ok) return false;
        affineOut = A.clone();
        mappingReady = true;
        return true;
    }
    bool hasMapping() { std::lock_guard<std::mutex> lk(mutex); return mappingReady; }
private:
    std::mutex mutex;
    bool collecting;
    int screenW=0, screenH=0;
    std::vector<Point2f> calibPoints;
    int currentIndex = 0;
    std::vector<std::array<double,4>> samples;
    std::vector<Point2f> targets;
    bool mappingReady;
};

// -----------------------------
// SmoothCursor (Kalman + smoothing) - GUI-friendly (QPointF)
class SmoothCursor {
public:
    SmoothCursor() {
        kf = cv::KalmanFilter(4,2,0,CV_64F);
        double dt = 1.0;
        Mat trans = (Mat_<double>(4,4) << 1,0,dt,0, 0,1,0,dt, 0,0,1,0, 0,0,0,1);
        kf.transitionMatrix = trans;
        kf.measurementMatrix = Mat::zeros(2,4,CV_64F);
        kf.measurementMatrix.at<double>(0,0) = 1.0;
        kf.measurementMatrix.at<double>(1,1) = 1.0;
        setIdentity(kf.processNoiseCov, Scalar::all(KALMAN_PROCESS_NOISE));
        setIdentity(kf.measurementNoiseCov, Scalar::all(KALMAN_MEASUREMENT_NOISE));
        setIdentity(kf.errorCovPost, Scalar::all(1.0));
        kf.statePost = Mat::zeros(4,1,CV_64F);
        expX = expY = 0.0;
    }
    QPointF update(const QPointF &meas) {
        Mat measurement = Mat::zeros(2,1,CV_64F);
        measurement.at<double>(0,0) = meas.x();
        measurement.at<double>(1,0) = meas.y();
        Mat pred = kf.predict();
        Mat estimated = kf.correct(measurement);
        double ex = estimated.at<double>(0,0);
        double ey = estimated.at<double>(1,0);
        if (!initialized) { expX = ex; expY = ey; initialized = true; }
        expX = EXP_SMOOTH_ALPHA * ex + (1.0 - EXP_SMOOTH_ALPHA) * expX;
        expY = EXP_SMOOTH_ALPHA * ey + (1.0 - EXP_SMOOTH_ALPHA) * expY;
        pushMedian(expX, expY);
        QPointF out((float)medianX(), (float)medianY());
        return out;
    }
    void reset() { initialized = false; medX.clear(); medY.clear(); }
private:
    KalmanFilter kf;
    bool initialized = false;
    double expX, expY;
    std::deque<double> medX, medY;
    void pushMedian(double x, double y) {
        medX.push_back(x); medY.push_back(y);
        if ((int)medX.size() > MEDIAN_FILTER_WINDOW) { medX.pop_front(); medY.pop_front(); }
    }
    double medianX() {
        std::vector<double> v(medX.begin(), medX.end()); if (v.empty()) return expX;
        std::sort(v.begin(), v.end()); return v[v.size()/2];
    }
    double medianY() {
        std::vector<double> v(medY.begin(), medY.end()); if (v.empty()) return expY;
        std::sort(v.begin(), v.end()); return v[v.size()/2];
    }
};

// -----------------------------
// createEnlargedEyePatch implementation
static Mat createEnlargedEyePatch(const Mat &frame, const Rect &eyeBoxInitial, double internalScale, int targetW, int targetH) {
    if (frame.empty() || eyeBoxInitial.width <= 8 || eyeBoxInitial.height <= 8) return Mat();
    Rect refined = refineEyeBox(frame, eyeBoxInitial);
    Mat crop = frame(refined).clone();
    Mat up;
    bool ok = superResolveClassical(crop, up, internalScale);
    if (!ok) up = crop.clone();
    Mat finalPatch = resizeToFixedTarget(up, targetW, targetH);
    Mat lab; cvtColor(finalPatch, lab, COLOR_BGR2Lab);
    std::vector<Mat> planes; split(lab, planes);
    try {
        Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8,8));
        Mat lclahe; clahe->apply(planes[0], lclahe);
        planes[0] = lclahe; merge(planes, lab);
        Mat final; cvtColor(lab, final, COLOR_Lab2BGR);
        return final;
    } catch (...) {
        return finalPatch;
    }
}

// -----------------------------
// CaptureWorkerFFT: capture, detect, compute features, emit frames & eye patches & spectral info
class CaptureWorkerFFT : public QObject {
    Q_OBJECT
public:
    CaptureWorkerFFT(int camIndex = 0, QObject *parent = nullptr)
        : QObject(parent), camIndex(camIndex), running(false)
    {
        fs = 30.0;
        N = 256;
        window.resize(N);
        for (int i = 0; i < N; ++i) window[i] = 0.5 * (1.0 - cos(2.0*M_PI*i/(N-1)));
        bufPupilX.assign(N, 0.0);
        bufPupilY.assign(N, 0.0);
        bufFaceX.assign(N, 0.0);
        bufFaceY.assign(N, 0.0);
        writePos = 0;
        samplesCollected = 0;
        in = (double*)fftw_malloc(sizeof(double) * N);
        out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (N/2 + 1));
        plan = fftw_plan_dft_r2c_1d(N, in, out, FFTW_MEASURE);
        notchEnabled = false;
        if (!faceCascade.load(FACE_CASCADE_FILE)) qWarning() << "Failed to load face cascade:" << QString::fromStdString(FACE_CASCADE_FILE);
        if (!eyeCascade.load(EYE_CASCADE_FILE)) qWarning() << "Failed to load eye cascade:" << QString::fromStdString(EYE_CASCADE_FILE);
        if (!leftEyeCascade.load(LEFTEYE_FILE)) qWarning() << "Failed to load left eye cascade:" << QString::fromStdString(LEFTEYE_FILE);
        if (!rightEyeCascade.load(RIGHTEYE_FILE)) qWarning() << "Failed to load right eye cascade:" << QString::fromStdString(RIGHTEYE_FILE);
        if (!eyeGlassCascade.load(EYE_GLASSES_FILE)) qWarning() << "Failed to load eye glasses cascade:" << QString::fromStdString(EYE_GLASSES_FILE);
    }
    ~CaptureWorkerFFT() {
        stop();
        fftw_destroy_plan(plan);
        fftw_free(in);
        fftw_free(out);
    }

public slots:
    void start() {
        if (running) return;
        running = true;
        cap.open(camIndex, cv::CAP_ANY);
        if (!cap.isOpened()) {
            emit errorOccurred(QStringLiteral("Cannot open camera index %1").arg(camIndex));
            running = false;
            return;
        }
        double camfps = cap.get(CAP_PROP_FPS);
        if (camfps > 1.0) fs = camfps;
        cap.set(CAP_PROP_FRAME_WIDTH, 640);
        cap.set(CAP_PROP_FRAME_HEIGHT, 480);

        Mat frame;
        QElapsedTimer t; t.start();
        qint64 lastFFTTime = 0;
        const int fftIntervalMs = 500;

        while (running) {
            if (!cap.grab()) { QThread::msleep(5); continue; }
            if (!cap.retrieve(frame)) { QThread::msleep(5); continue; }
            if (frame.empty()) { QThread::msleep(5); continue; }

            Mat gray; cvtColor(frame, gray, COLOR_BGR2GRAY); equalizeHist(gray, gray);
            std::vector<Rect> faces; faceCascade.detectMultiScale(gray, faces, 1.1, 3, 0, Size(80,80));
            Rect faceRect;
            if (!faces.empty()) faceRect = *std::max_element(faces.begin(), faces.end(), [](const Rect&a,const Rect&b){return a.area()<b.area();});

            std::vector<Rect> eyesRectsGlobal;
            if (faceRect.area() > 0) {
                Mat faceROIgray = gray(faceRect);
                Rect upperFaceRegion(0,0,faceRect.width,int(faceRect.height*0.6));
                Mat searchRegion = faceROIgray(upperFaceRegion);
                std::vector<Rect> eyesRects;
                eyeCascade.detectMultiScale(searchRegion, eyesRects, 1.1, 3, 0, Size(20,20));
                if (eyesRects.empty()) leftEyeCascade.detectMultiScale(searchRegion, eyesRects, 1.1, 3, 0, Size(20,20));
                if (eyesRects.empty()) rightEyeCascade.detectMultiScale(searchRegion, eyesRects, 1.1, 3, 0, Size(20,20));
                if (eyesRects.empty()) eyeGlassCascade.detectMultiScale(searchRegion, eyesRects, 1.1, 3, 0, Size(20,20));
                for (const Rect &er : eyesRects) {
                    Rect global(er.x + faceRect.x, er.y + faceRect.y, er.width, er.height);
                    Rect refined = refineEyeBox(frame, global);
                    eyesRectsGlobal.push_back(refined);
                }
            }

            std::sort(eyesRectsGlobal.begin(), eyesRectsGlobal.end(), [](const Rect&a,const Rect&b){ return a.x < b.x; });

            Point2f leftPupil(-1,-1), rightPupil(-1,-1);
            double leftArea=0.0, rightArea=0.0;
            Mat leftPatch, rightPatch;
            if (!eyesRectsGlobal.empty()) {
                Rect leftBox = eyesRectsGlobal[0];
                Mat leftCrop = frame(leftBox).clone();
                if (estimatePupilCenter(leftCrop, leftPupil, leftArea)) {
                    leftPupil.x += leftBox.x; leftPupil.y += leftBox.y;
                } else leftPupil = Point2f(-1,-1);
                leftPatch = createEnlargedEyePatch(frame, leftBox, INTERNAL_SR_SCALE, TARGET_EYE_W, TARGET_EYE_H);
            }
            if (eyesRectsGlobal.size() >= 2) {
                Rect rightBox = eyesRectsGlobal[1];
                Mat rightCrop = frame(rightBox).clone();
                if (estimatePupilCenter(rightCrop, rightPupil, rightArea)) {
                    rightPupil.x += rightBox.x; rightPupil.y += rightBox.y;
                } else rightPupil = Point2f(-1,-1);
                rightPatch = createEnlargedEyePatch(frame, rightBox, INTERNAL_SR_SCALE, TARGET_EYE_W, TARGET_EYE_H);
            }

            Point2f faceCenter(-1,-1);
            double faceSize = 0.0;
            if (faceRect.area() > 0) {
                faceCenter = Point2f(faceRect.x + faceRect.width*0.5f, faceRect.y + faceRect.height*0.5f);
                faceSize = std::max(faceRect.width, faceRect.height);
            }

            Point2f eyeFeature(-1,-1), headFeature(-1,-1);
            if (leftPupil.x >= 0 && faceCenter.x >= 0 && faceSize > 1e-3) {
                eyeFeature.x = (leftPupil.x - faceCenter.x) / faceSize;
                eyeFeature.y = (leftPupil.y - faceCenter.y) / faceSize;
            } else if (rightPupil.x >= 0 && faceCenter.x >= 0 && faceSize > 1e-3) {
                eyeFeature.x = (rightPupil.x - faceCenter.x) / faceSize;
                eyeFeature.y = (rightPupil.y - faceCenter.y) / faceSize;
            } else {
                eyeFeature = Point2f(0,0);
            }
            if (faceCenter.x >= 0 && faceSize > 1e-3) {
                headFeature.x = faceCenter.x / (double)cap.get(CAP_PROP_FRAME_WIDTH);
                headFeature.y = faceCenter.y / (double)cap.get(CAP_PROP_FRAME_HEIGHT);
            } else headFeature = Point2f(0.5f, 0.5f);

            QImage leftQ, rightQ;
            if (!leftPatch.empty()) {
                Mat lrgb; cvtColor(leftPatch, lrgb, COLOR_BGR2RGB);
                leftQ = QImage((uchar*)lrgb.data, lrgb.cols, lrgb.rows, lrgb.step, QImage::Format_RGB888).copy();
            }
            if (!rightPatch.empty()) {
                Mat rrgb; cvtColor(rightPatch, rrgb, COLOR_BGR2RGB);
                rightQ = QImage((uchar*)rrgb.data, rrgb.cols, rrgb.rows, rrgb.step, QImage::Format_RGB888).copy();
            }
            emit eyePatchesReady(leftQ, rightQ);

            if (calib.isCollecting()) {
                Point2f eyeNorm = eyeFeature;
                Point2f headNorm = headFeature;
                calib.addSample(eyeNorm, headNorm);
            }

            Mat rgb; cvtColor(frame, rgb, COLOR_BGR2RGB);
            QImage img((uchar*)rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
            emit frameReady(img.copy(),
                            QPointF((leftPupil.x>=0)?leftPupil.x:((rightPupil.x>=0)?rightPupil.x:-1),
                                    (leftPupil.y>=0)?leftPupil.y:((rightPupil.y>=0)?rightPupil.y:-1)),
                            QRect(faceRect.x, faceRect.y, faceRect.width, faceRect.height));

            double px = (leftPupil.x >= 0) ? leftPupil.x : ((rightPupil.x >= 0) ? rightPupil.x : 0.0);
            double py = (leftPupil.y >= 0) ? leftPupil.y : ((rightPupil.y >= 0) ? rightPupil.y : 0.0);
            bufPupilX[writePos] = px;
            bufPupilY[writePos] = py;
            bufFaceX[writePos]  = faceCenter.x;
            bufFaceY[writePos]  = faceCenter.y;
            writePos = (writePos + 1) % N;
            samplesCollected = std::min(samplesCollected + 1, N);

            qint64 now = t.elapsed();
            if (now - lastFFTTime >= fftIntervalMs && samplesCollected == N) {
                lastFFTTime = now;
                for (int i = 0; i < N; ++i) {
                    int idx = (writePos + i) % N;
                    in[i] = bufPupilX[idx] * window[i];
                }
                fftw_execute(plan);
                std::vector<double> magX(N/2 + 1);
                for (int k = 0; k <= N/2; ++k) magX[k] = sqrt(out[k][0]*out[k][0] + out[k][1]*out[k][1]);
                for (int i = 0; i < N; ++i) {
                    int idx = (writePos + i) % N;
                    in[i] = bufPupilY[idx] * window[i];
                }
                fftw_execute(plan);
                std::vector<double> magY(N/2 + 1);
                for (int k = 0; k <= N/2; ++k) magY[k] = sqrt(out[k][0]*out[k][0] + out[k][1]*out[k][1]);
                std::vector<double> freqs(N/2 + 1);
                for (int k = 0; k <= N/2; ++k) freqs[k] = (double)k * fs / (double)N;
                int peakIdxX = 1; double peakValX = magX[1];
                for (int k = 2; k <= N/2; ++k) if (magX[k] > peakValX) { peakValX = magX[k]; peakIdxX = k; }
                int peakIdxY = 1; double peakValY = magY[1];
                for (int k = 2; k <= N/2; ++k) if (magY[k] > peakValY) { peakValY = magY[k]; peakIdxY = k; }
                auto bandEnergy = [&](const std::vector<double> &mag, double f0, double f1)->double {
                    int k0 = std::max(0, (int)std::floor(f0 * N / fs));
                    int k1 = std::min(N/2, (int)std::ceil(f1 * N / fs));
                    double s = 0.0;
                    for (int k = k0; k <= k1; ++k) s += mag[k]*mag[k];
                    return s;
                };
                SpectralInfo info;
                info.fs = fs; info.N = N; info.freqs = freqs;
                info.magPupilX = magX; info.magPupilY = magY;
                info.peakFreqPupilX = freqs[peakIdxX];
                info.peakFreqPupilY = freqs[peakIdxY];
                info.bandEnergyLowPupilX = bandEnergy(magX, 0.1, 1.5);
                info.bandEnergyHighPupilX = bandEnergy(magX, 2.0, 8.0);
                emit spectralInfo(info);
            }

            QThread::msleep(5);
        } // while
        cap.release();
    } // start

    void stop() { running = false; }

    bool computeCalibrationMapping(Mat &Aout) {
        return calib.computeMapping(Aout);
    }
    void startCalibration(const QSize &screenSize, int points = 9) { calib.startCalibration(screenSize, points); }
    void stopCalibration() { calib.stopCalibration(); }
    bool isCalibrating() { return calib.isCollecting(); }
    Point2f currentCalibrationTarget() { return calib.currentTarget(); }

signals:
    void frameReady(const QImage &img, const QPointF &pupil, const QRect &faceRect);
    void spectralInfo(const SpectralInfo &info);
    void eyePatchesReady(const QImage &leftEye, const QImage &rightEye);
    void cursorTarget(const QPointF &screenPoint);
    void errorOccurred(const QString &msg);

private:
    int camIndex;
    bool running;
    VideoCapture cap;
    CascadeClassifier faceCascade, eyeCascade, leftEyeCascade, rightEyeCascade, eyeGlassCascade;

    double fs;
    int N;
    std::vector<double> window;
    std::vector<double> bufPupilX, bufPupilY, bufFaceX, bufFaceY;
    int writePos;
    int samplesCollected;

    double *in;
    fftw_complex *out;
    fftw_plan plan;

    BiquadNotch notchX, notchY;
    bool notchEnabled;

    Calibrator calib;
};

// -----------------------------
// GUI + Integration
class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("EyeControl - Cursor via Eyes & Head");
        resize(1320, 800);
        viewLabel.setParent(this);
        viewLabel.setGeometry(10,10,960,540);
        viewLabel.show();

        startCalibBtn.setParent(this); startCalibBtn.setText("Start Calibration"); startCalibBtn.setGeometry(980, 20, 160, 32);
        stopCalibBtn.setParent(this); stopCalibBtn.setText("Stop Calibration"); stopCalibBtn.setGeometry(980, 60, 160, 32);
        enableDwellBtn.setParent(this); enableDwellBtn.setText("Enable Dwell Click"); enableDwellBtn.setGeometry(980, 100, 160, 32);
        enableDwellBtn.setCheckable(true);
        startCalibBtn.show(); stopCalibBtn.show(); enableDwellBtn.show();

        connect(&timer, &QTimer::timeout, this, &MainWindow::onGUITick);
        timer.start(30);

        leftEyeLabel.setParent(this); leftEyeLabel.setGeometry(980, 160, 320, 240); leftEyeLabel.show();
        rightEyeLabel.setParent(this); rightEyeLabel.setGeometry(980, 420, 320, 240); rightEyeLabel.show();

        connect(&startCalibBtn, &QPushButton::clicked, this, &MainWindow::onStartCalibration);
        connect(&stopCalibBtn, &QPushButton::clicked, this, &MainWindow::onStopCalibration);
        connect(&enableDwellBtn, &QPushButton::toggled, this, &MainWindow::onToggleDwell);

        dwellTimer.setSingleShot(true);
        connect(&dwellTimer, &QTimer::timeout, this, &MainWindow::onDwellClick);

        blinkClosed = false;
        blinkStartTime = 0;
    }

    void setWorker(CaptureWorkerFFT *w) {
        worker = w;
        connect(worker, &CaptureWorkerFFT::frameReady, this, &MainWindow::onFrameReady);
        connect(worker, &CaptureWorkerFFT::eyePatchesReady, this, &MainWindow::onEyePatchesReady);
        connect(worker, &CaptureWorkerFFT::spectralInfo, this, &MainWindow::onSpectralInfo);
    }

private slots:
    void onFrameReady(const QImage &img, const QPointF &pupil, const QRect &faceRect) {
        QMutexLocker lk(&imgMutex);
        latestFrame = img;
        latestPupil = pupil;
        latestFaceRect = faceRect;
    }
    void onEyePatchesReady(const QImage &left, const QImage &right) {
        QMutexLocker lk(&imgMutex);
        leftEyeImg = left; rightEyeImg = right;
        leftEyeLabel.setPixmap(QPixmap::fromImage(leftEyeImg).scaled(leftEyeLabel.size(), Qt::KeepAspectRatio));
        rightEyeLabel.setPixmap(QPixmap::fromImage(rightEyeImg).scaled(rightEyeLabel.size(), Qt::KeepAspectRatio));
    }
    void onSpectralInfo(const SpectralInfo &info) {
        Q_UNUSED(info);
    }
    void onGUITick() {
        QImage frameCopy;
        QPointF pupil;
        QRect faceRect;
        {
            QMutexLocker lk(&imgMutex);
            frameCopy = latestFrame;
            pupil = latestPupil;
            faceRect = latestFaceRect;
        }
        if (!frameCopy.isNull()) {
            QPixmap pix = QPixmap::fromImage(frameCopy).scaled(viewLabel.size(), Qt::KeepAspectRatio);
            QPainter p(&pix);
            p.setPen(Qt::green);
            p.setFont(QFont("Sans", 10, QFont::Bold));
            p.drawText(8, 16, "Live (head untouched). Fixed-size eye patches shown right.");
            if (pupil.x() >= 0) {
                double sx = double(pix.width()) / double(frameCopy.width());
                double sy = double(pix.height()) / double(frameCopy.height());
                int px = int(pupil.x() * sx);
                int py = int(pupil.y() * sy);
                p.setPen(Qt::yellow);
                p.drawEllipse(QPoint(px, py), 6, 6);
            }
            p.end();
            viewLabel.setPixmap(pix);
        }

        if (!latestFrame.isNull() && !mappingMat.empty()) {
            Mat frameMat(latestFrame.height(), latestFrame.width(), CV_8UC3, (void*)latestFrame.bits(), latestFrame.bytesPerLine());
            Mat bgr; cvtColor(frameMat, bgr, COLOR_RGB2BGR);
            if (latestPupil.x() >= 0 && latestFaceRect.width() > 0) {
                Point2f faceCenter(latestFaceRect.x() + latestFaceRect.width()*0.5f, latestFaceRect.y() + latestFaceRect.height()*0.5f);
                double faceSize = std::max(latestFaceRect.width(), latestFaceRect.height());
                Point2f eyeFeat((float)((latestPupil.x() - faceCenter.x)/faceSize), (float)((latestPupil.y() - faceCenter.y)/faceSize));
                Point2f headFeat((float)(faceCenter.x / (double)frameMat.cols), (float)(faceCenter.y / (double)frameMat.rows));
                Mat f = (Mat_<double>(1,5) << eyeFeat.x, eyeFeat.y, headFeat.x, headFeat.y, 1.0);
                Mat res = f * mappingMat;
                double sx = res.at<double>(0,0);
                double sy = res.at<double>(0,1);
                QScreen *screen = QGuiApplication::primaryScreen();
                QRect scr = screen->geometry();
                sx = clampT(sx, (double)scr.left(), (double)scr.right());
                sy = clampT(sy, (double)scr.top(), (double)scr.bottom());
                QPointF rawTarget((float)sx, (float)sy);
                QPointF smoothed = smoothCursor.update(rawTarget);
                QCursor::setPos(QPoint((int)smoothed.x(), (int)smoothed.y()));
                if (enableDwell) {
                    if (!dwellActive) {
                        dwellActive = true;
                        dwellStart = QDateTime::currentMSecsSinceEpoch();
                        dwellTimer.start(DWELL_MS);
                        dwellPos = smoothed;
                    } else {
                        double dist = std::hypot(smoothed.x() - dwellPos.x(), smoothed.y() - dwellPos.y());
                        if (dist > 10.0) { dwellTimer.stop(); dwellActive = true; dwellTimer.start(DWELL_MS); dwellPos = smoothed; }
                    }
                }
            }
        }
    }

    void onStartCalibration() {
        if (!worker) return;
        QScreen *screen = QGuiApplication::primaryScreen();
        QSize s = screen->size();
        worker->startCalibration(s, 9);
        qInfo() << "Calibration started. Please look at the points on screen in sequence.";
    }
    void onStopCalibration() {
        if (!worker) return;
        worker->stopCalibration();
        Mat A;
        if (worker->computeCalibrationMapping(A)) {
            mappingMat = A.clone();
            qInfo() << "Calibration mapping computed.";
        } else {
            qWarning() << "Calibration mapping failed (not enough samples).";
        }
    }
    void onToggleDwell(bool on) {
        enableDwell = on;
        if (!on) { dwellTimer.stop(); dwellActive = false; }
    }
    void onDwellClick() {
        QPoint pos = QCursor::pos();
        QWindow *w = QGuiApplication::topLevelAt(pos);
        if (w) {
            QPoint local = w->mapFromGlobal(pos);
            QMouseEvent press(QEvent::MouseButtonPress, local, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
            QMouseEvent release(QEvent::MouseButtonRelease, local, pos, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
            QCoreApplication::sendEvent(w, &press);
            QCoreApplication::sendEvent(w, &release);
            qInfo() << "Dwell click emitted at" << pos;
        } else {
            qInfo() << "Dwell click: no window under cursor";
        }
        dwellActive = false;
    }

private:
    QLabel viewLabel;
    QLabel leftEyeLabel;
    QLabel rightEyeLabel;
    QPushButton startCalibBtn;
    QPushButton stopCalibBtn;
    QPushButton enableDwellBtn;
    QTimer timer;
    QTimer dwellTimer;
    QMutex imgMutex;
    QImage latestFrame;
    QPointF latestPupil;
    QRect latestFaceRect;
    QImage leftEyeImg, rightEyeImg;
    CaptureWorkerFFT *worker = nullptr;

    SmoothCursor smoothCursor;
    Mat mappingMat;
    bool enableDwell = false;
    bool dwellActive = false;
    qint64 dwellStart = 0;
    QPointF dwellPos;

    bool blinkClosed;
    qint64 blinkStartTime;
};

// -----------------------------
// main
int main(int argc, char *argv[]) {
    qRegisterMetaType<SpectralInfo>("SpectralInfo");
    QApplication app(argc, argv);

    CaptureWorkerFFT *worker = new CaptureWorkerFFT(0);
    QThread *workerThread = new QThread;
    worker->moveToThread(workerThread);
    QObject::connect(workerThread, &QThread::started, worker, &CaptureWorkerFFT::start);
    QObject::connect(workerThread, &QThread::finished, worker, &CaptureWorkerFFT::stop);
    QObject::connect(worker, &CaptureWorkerFFT::errorOccurred, [&](const QString &msg){ qWarning() << "Worker error:" << msg; });
    workerThread->start();

    MainWindow w;
    w.setWorker(worker);
    w.show();

    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&](){
        worker->stop();
        workerThread->quit();
        workerThread->wait();
        delete worker;
        delete workerThread;
    });

    return app.exec();
}

#include "eyectl.moc"
