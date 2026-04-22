// main.cpp
// Qt6-only Eye Tracker using OpenCV4 + dlib
// - All UI with Qt Widgets (video in QLabel + overlay + log box)
// - Worker thread runs tight loop (no sleep), adaptive yielding, automatic calibration
// - Compact, minimal unused temporaries, public control methods for safe access
// Build with qmake6 eyetracker.pro (provided earlier)

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPlainTextEdit>
#include <QHBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QScreen>
#include <QGuiApplication>
#include <QCursor>
#include <QMetaObject>
#include <QMutex>
#include <QScrollBar>
#include <QThread>
#include <QKeyEvent>
#include <QPainter>

#include <opencv2/opencv.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>

#include <iostream>
#include <streambuf>
#include <vector>
#include <atomic>
#include <chrono>
#include <thread>
#include <cmath>

    // ---------------------- Stream Redirector ----------------------
    class QtStreamBuf : public std::streambuf {
public:
    explicit QtStreamBuf(QPlainTextEdit *edit) : m_edit(edit) {}
protected:
    int_type overflow(int_type v) override {
        if (v == traits_type::eof()) return traits_type::not_eof(v);
        append(QString(QChar(static_cast<char>(v))));
        return v;
    }
    std::streamsize xsputn(const char *p, std::streamsize n) override {
        append(QString::fromUtf8(p, static_cast<int>(n)));
        return n;
    }
private:
    QPlainTextEdit *m_edit;
    void append(const QString &text) {
        if (!m_edit) return;
        QString copy = text;
        QMetaObject::invokeMethod(m_edit, "appendPlainText", Qt::QueuedConnection, Q_ARG(QString, copy));
        QMetaObject::invokeMethod(m_edit->verticalScrollBar(), "setValue", Qt::QueuedConnection,
                                  Q_ARG(int, m_edit->verticalScrollBar()->maximum()));
    }
};

class StreamRedirector {
public:
    StreamRedirector(std::ostream &s, QtStreamBuf *b) : stream(s), oldbuf(stream.rdbuf()), buf(b) { stream.rdbuf(buf); }
    ~StreamRedirector() { stream.rdbuf(oldbuf); }
private:
    std::ostream &stream;
    std::streambuf *oldbuf;
    QtStreamBuf *buf;
};

// ---------------------- OverlayLabel ----------------------
class OverlayLabel : public QLabel {
    Q_OBJECT
public:
    explicit OverlayLabel(QWidget *parent = nullptr) : QLabel(parent), showOverlay(false) {
        setScaledContents(false);
    }

    void setFrame(const QImage &img) {
        QMutexLocker locker(&m_mutex);
        m_frame = img;
        update();
    }

    void setCalibrationTarget(const QPointF &pt, int idx, int total, bool visible) {
        QMutexLocker locker(&m_mutex);
        overlayPoint = pt;
        overlayIndex = idx;
        overlayTotal = total;
        showOverlay = visible;
        update();
    }

protected:
    void paintEvent(QPaintEvent *ev) override {
        Q_UNUSED(ev);
        QPainter p(this);
        p.setRenderHint(QPainter::SmoothPixmapTransform, true);

        QMutexLocker locker(&m_mutex);
        if (!m_frame.isNull()) {
            QPixmap pm = QPixmap::fromImage(m_frame);
            QSize labelSize = size();
            pm = pm.scaled(labelSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            QRect srcRect((pm.width() - labelSize.width())/2, (pm.height() - labelSize.height())/2,
                          labelSize.width(), labelSize.height());
            p.drawPixmap(0, 0, pm, srcRect.x(), srcRect.y(), srcRect.width(), srcRect.height());
        } else {
            p.fillRect(rect(), Qt::black);
        }

        if (showOverlay) {
            QScreen *screen = QGuiApplication::primaryScreen();
            QRect screenGeo = screen->geometry();
            if (!m_frame.isNull()) {
                QSize labelSize = size();
                double frameW = m_frame.width();
                double frameH = m_frame.height();
                double scale = std::max(labelSize.width() / frameW, labelSize.height() / frameH);
                double dispW = frameW * scale;
                double dispH = frameH * scale;
                double offsetX = (labelSize.width() - dispW) / 2.0;
                double offsetY = (labelSize.height() - dispH) / 2.0;

                double nx = (overlayPoint.x() - screenGeo.left()) / double(screenGeo.width());
                double ny = (overlayPoint.y() - screenGeo.top())  / double(screenGeo.height());
                nx = std::clamp(nx, 0.0, 1.0);
                ny = std::clamp(ny, 0.0, 1.0);

                double lx = offsetX + nx * dispW;
                double ly = offsetY + ny * dispH;

                QColor col(255, 200, 0, 200);
                p.setPen(Qt::NoPen);
                p.setBrush(col);
                int r = 18;
                p.drawEllipse(QPointF(lx, ly), r, r);

                p.setPen(Qt::white);
                p.setFont(QFont("Sans", 10, QFont::Bold));
                p.drawText(QRectF(lx - 30, ly + r + 4, 60, 18), Qt::AlignCenter,
                           QString("%1/%2").arg(overlayIndex).arg(overlayTotal));
            }
        }
    }

private:
    QImage m_frame;
    QMutex m_mutex;
    QPointF overlayPoint;
    int overlayIndex = 0;
    int overlayTotal = 0;
    bool showOverlay = false;
};

// ---------------------- Processor (Worker) ----------------------
// Calibration state enum
enum class CalState { IDLE = 0, RUNNING = 1, CAPTURING = 2, DONE = 3 };

class Processor : public QObject {
    Q_OBJECT
public:
    explicit Processor(int camIndex = 0)
        : camIndex(camIndex),
        stopFlag(false),
        mouseEnabled(true),
        targetFps(60.0),
        rtc(0), ltc(0), dr(0), dl(0), fv(25),
        rcx(0), rcy(0), lcx(0), lcy(0),
        calState(CalState::IDLE), calIndex(0),
        calSampleDurationMs(300), calSamplesPerPointMin(8),
        maxFailedSamplesPerPoint(30)
    {
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect geo = screen->geometry();
        screenTargets = {
            QPointF(geo.center()),
            QPointF(geo.left() + geo.width()*0.15, geo.top() + geo.height()*0.15),
            QPointF(geo.right() - geo.width()*0.15, geo.top() + geo.height()*0.15),
            QPointF(geo.left() + geo.width()*0.15, geo.bottom() - geo.height()*0.15),
            QPointF(geo.right() - geo.width()*0.15, geo.bottom() - geo.height()*0.15)
        };
    }

    void requestStop() { stopFlag.store(true); }

    // Public control methods (safe accessors/mutators)
    void resetThresholds() { rtc = 0; ltc = 0; dr = 0; dl = 0; }
    void setTargetFps(double fps) { targetFps = fps; emit logMessage(QString("Target FPS: %1").arg(fps)); }
    double getTargetFps() const { return targetFps; }
    void toggleMouse() { mouseEnabled = !mouseEnabled; emit logMessage(QString("Mouse control %1").arg(mouseEnabled ? "enabled" : "disabled")); }
    void startCalibration() {
        if (calState == CalState::IDLE || calState == CalState::DONE) {
            measuredEyes.clear(); calIndex = 0; calibrationReady = false; affineMat.release(); calState = CalState::RUNNING;
            emit logMessage("Calibration started.");
        } else emit logMessage("Calibration already running.");
    }

signals:
    void frameReady(const QImage &img);
    void logMessage(const QString &msg);
    void overlayUpdate(const QPointF &screenPoint, int idx, int total, bool visible);
    void calibrationFinished(bool ok);

public slots:
    void process() {
        try {
            emit logMessage("Worker: starting camera and models...");
            cv::VideoCapture cap(camIndex);
            if (!cap.isOpened()) { emit logMessage("Error: cannot open camera"); return; }
            emit logMessage("Camera opened.");

            dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
            dlib::shape_predictor predictor;
            try {
#ifdef SHAPE_MODEL_PATH
                std::string modelPath = SHAPE_MODEL_PATH;
#else
                std::string modelPath = "./shape_predictor_68_face_landmarks.dat";
#endif
                dlib::deserialize(modelPath) >> predictor;
                emit logMessage("Shape predictor loaded.");
            } catch (std::exception &e) {
                emit logMessage(QString("Failed to load shape predictor: %1").arg(e.what()));
                return;
            }

            const int RIGHT_EYE_START = 36, RIGHT_EYE_END = 42;
            const int LEFT_EYE_START = 42, LEFT_EYE_END = 48;
            const double CURSOR_ALPHA = 0.18;

            QPointF smoothedCursor(QCursor::pos());
            using clock = std::chrono::high_resolution_clock;
            auto lastTime = clock::now();
            double measuredFps = 0.0;
            const double fpsHysteresis = 2.0;

            std::vector<QPointF> currentSamples;
            int failedSamples = 0;

            while (!stopFlag.load()) {
                cv::Mat frame;
                if (!cap.read(frame) || frame.empty()) { emit logMessage("Warning: empty frame"); std::this_thread::yield(); continue; }

                cv::resize(frame, frame, cv::Size(), 550.0 / frame.cols, 550.0 / frame.cols, cv::INTER_CUBIC);
                cv::Mat gray; cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
                dlib::cv_image<unsigned char> dlibGray(gray);
                auto rects = detector(dlibGray);

                int rows = 1, cols = 1;

                if (!rects.empty()) {
                    dlib::full_object_detection shape = predictor(dlibGray, rects[0]);
                    std::vector<cv::Point> pts(shape.num_parts());
                    for (unsigned i = 0; i < shape.num_parts(); ++i) pts[i] = { shape.part(i).x(), shape.part(i).y() };

                    auto extract_roi_direct = [&](int i, int j) -> cv::Mat {
                        if (pts.size() < (size_t)j) return {};
                        cv::Rect r = cv::boundingRect(std::vector<cv::Point>(pts.begin() + i, pts.begin() + j));
                        int xm = 5, ym = 5;
                        int x = std::max(0, r.x - xm);
                        int y = std::max(0, r.y - ym);
                        int w = std::min(frame.cols - x, r.width + 2*xm);
                        int h = std::min(frame.rows - y, r.height + 2*ym);
                        if (w <= 0 || h <= 0) return {};
                        cv::Mat roi = frame(cv::Rect(x, y, w, h)).clone();
                        cv::Mat out;
                        double scale = 250.0 / std::max(1, roi.cols);
                        cv::resize(roi, out, cv::Size(), scale, scale, cv::INTER_CUBIC);
                        return out;
                    };

                    cv::Mat rightROI = extract_roi_direct(RIGHT_EYE_START, RIGHT_EYE_END);
                    rows = rightROI.empty() ? 1 : rightROI.rows;
                    cols = rightROI.empty() ? 1 : rightROI.cols;

                    auto process_eye = [&](int start, int end, int &out_cx, int &out_cy, int threshold) {
                        cv::Mat roi = extract_roi_direct(start, end);
                        if (roi.empty()) { out_cx = out_cy = 0; return; }
                        cv::Mat g; cv::cvtColor(roi, g, cv::COLOR_BGR2GRAY);
                        cv::medianBlur(g, g, fv);
                        cv::GaussianBlur(g, g, cv::Size(fv, fv), 0);
                        cv::Mat th; cv::threshold(g, th, threshold, 255, cv::THRESH_BINARY_INV);
                        std::vector<std::vector<cv::Point>> contours;
                        cv::findContours(th, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
                        if (contours.empty()) { out_cx = out_cy = 0; return; }
                        std::sort(contours.begin(), contours.end(), [](auto &a, auto &b){ return cv::contourArea(a) > cv::contourArea(b); });
                        cv::Rect r = cv::boundingRect(contours.front());
                        out_cx = r.x + r.width/2;
                        out_cy = r.y + r.height/2;
                    };

                    process_eye(RIGHT_EYE_START, RIGHT_EYE_END, rcx, rcy, rtc);
                    process_eye(LEFT_EYE_START, LEFT_EYE_END, lcx, lcy, ltc);

                    int count = 0; double eyeX = 0.0, eyeY = 0.0;
                    if (rcx > 0 && rcy > 0) { eyeX += rcx; eyeY += rcy; ++count; }
                    if (lcx > 0 && lcy > 0) { eyeX += lcx; eyeY += lcy; ++count; }
                    if (count > 0) { eyeX /= count; eyeY /= count; }

                    // calibration state machine
                    if (calState == CalState::RUNNING) {
                        currentSamples.clear(); failedSamples = 0;
                        captureStart = std::chrono::high_resolution_clock::now();
                        calState = CalState::CAPTURING;
                        emit overlayUpdate(screenTargets[calIndex], calIndex+1, (int)screenTargets.size(), true);
                        emit logMessage(QString("Calibration: target %1/%2").arg(calIndex+1).arg(screenTargets.size()));
                    } else if (calState == CalState::CAPTURING) {
                        auto now = std::chrono::high_resolution_clock::now();
                        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - captureStart).count();
                        if (count > 0) currentSamples.emplace_back(eyeX, eyeY); else ++failedSamples;
                        if (elapsed >= calSampleDurationMs) {
                            if ((int)currentSamples.size() >= calSamplesPerPointMin) {
                                double sx=0, sy=0;
                                for (auto &p : currentSamples) { sx += p.x(); sy += p.y(); }
                                measuredEyes.emplace_back(sx/currentSamples.size(), sy/currentSamples.size());
                                emit logMessage(QString("Captured %1 (%2 samples)").arg(calIndex+1).arg((int)currentSamples.size()));
                                ++calIndex;
                                emit overlayUpdate(screenTargets[calIndex-1], calIndex, (int)screenTargets.size(), false);
                                if (calIndex >= (int)screenTargets.size()) {
                                    std::vector<cv::Point2d> src, dst;
                                    for (int i=0;i<(int)measuredEyes.size();++i) { src.emplace_back(measuredEyes[i].x(), measuredEyes[i].y()); dst.emplace_back(screenTargets[i].x(), screenTargets[i].y()); }
                                    cv::Mat A = cv::estimateAffine2D(src, dst);
                                    if (!A.empty()) { affineMat = A.clone(); calibrationReady = true; calState = CalState::DONE; emit logMessage("Calibration finished."); emit calibrationFinished(true); }
                                    else { emit logMessage("Calibration failed (degenerate)."); calibrationReady = false; calState = CalState::IDLE; emit calibrationFinished(false); }
                                } else calState = CalState::RUNNING;
                            } else {
                                if (failedSamples > maxFailedSamplesPerPoint) { measuredEyes.emplace_back(0,0); ++calIndex; calState = (calIndex >= (int)screenTargets.size()) ? CalState::DONE : CalState::RUNNING; }
                                else calState = CalState::RUNNING;
                            }
                        }
                    }

                    // cursor movement
                    if (mouseEnabled && count > 0) {
                        double tx, ty;
                        if (calibrationReady && !affineMat.empty()) {
                            cv::Mat dst = affineMat * (cv::Mat_<double>(3,1) << eyeX, eyeY, 1.0);
                            tx = dst.at<double>(0,0); ty = dst.at<double>(1,0);
                        } else {
                            double nx = (cols > 0) ? (eyeX / double(cols)) : 0.5;
                            double ny = (rows > 0) ? (eyeY / double(rows)) : 0.5;
                            nx = std::clamp(nx, 0.0, 1.0); ny = std::clamp(ny, 0.0, 1.0);
                            QRect geo = QGuiApplication::primaryScreen()->geometry();
                            tx = geo.left() + nx * geo.width(); ty = geo.top() + ny * geo.height();
                        }
                        smoothedCursor.setX(smoothedCursor.x() * (1.0 - CURSOR_ALPHA) + tx * CURSOR_ALPHA);
                        smoothedCursor.setY(smoothedCursor.y() * (1.0 - CURSOR_ALPHA) + ty * CURSOR_ALPHA);
                        QCursor::setPos(QPoint(int(std::round(smoothedCursor.x())), int(std::round(smoothedCursor.y()))));
                    }
                } // end if faces

                cv::putText(frame, "EyeTracker Qt UI", cv::Point(10,30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255,255,255), 2);

                QImage qimg;
                if (frame.channels() == 3) { cv::Mat rgb; cv::cvtColor(frame, rgb, cv::COLOR_BGR2RGB); qimg = QImage((const uchar*)rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy(); }
                else if (frame.channels() == 1) qimg = QImage((const uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_Grayscale8).copy();
                if (!qimg.isNull()) emit frameReady(qimg);

                auto loopEnd = clock::now();
                double loopMs = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(loopEnd - lastTime).count();
                if (loopMs > 1e-6) measuredFps = 1000.0 / loopMs;
                lastTime = loopEnd;
                if (measuredFps > targetFps + fpsHysteresis) std::this_thread::yield();
            } // end while

            cap.release();
            emit logMessage("Worker: stopped.");
        } catch (std::exception &e) {
            emit logMessage(QString("Worker exception: %1").arg(e.what()));
        }
    }

private:
    // ordering matches initializer list
    int camIndex;
    std::atomic<bool> stopFlag;
    std::atomic<bool> mouseEnabled;
    double targetFps;

    int rtc, ltc, dr, dl, fv;
    int rcx, rcy, lcx, lcy;
    int tcr[2] = {0,0}, tcl[2] = {0,0};

    CalState calState;
    int calIndex;
    std::vector<QPointF> screenTargets;
    std::vector<QPointF> measuredEyes;
    cv::Mat affineMat;
    bool calibrationReady = false;
    std::chrono::high_resolution_clock::time_point captureStart;
    int calSampleDurationMs;
    int calSamplesPerPointMin;
    int maxFailedSamplesPerPoint;
};

// ---------------------- MainWindow ----------------------
class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(OverlayLabel *videoLabel, QPlainTextEdit *logBox) : QWidget(nullptr), m_videoLabel(videoLabel), m_logBox(logBox) {
        setWindowTitle("Eye Tracker Qt Only");
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->addWidget(m_videoLabel, 3);
        layout->addWidget(m_logBox, 1);
        setLayout(layout);
        setFocusPolicy(Qt::StrongFocus);
    }
signals:
    void keyPressed(int key);
protected:
    void keyPressEvent(QKeyEvent *event) override { emit keyPressed(event->key()); }
private:
    OverlayLabel *m_videoLabel;
    QPlainTextEdit *m_logBox;
};

// ---------------------- main ----------------------
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    OverlayLabel *videoLabel = new OverlayLabel();
    videoLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    videoLabel->setAlignment(Qt::AlignCenter);
    videoLabel->setStyleSheet("background-color: black;");

    QPlainTextEdit *logBox = new QPlainTextEdit();
    logBox->setReadOnly(true);
    logBox->setMaximumWidth(420);
    logBox->setStyleSheet("QPlainTextEdit { background: rgba(0,0,0,160); color: #e6e6e6; font-family: Consolas, monospace; font-size: 12px; }");

    MainWindow mainWindow(videoLabel, logBox);
    mainWindow.resize(1280, 800);
    mainWindow.show();

    QtStreamBuf *qtBuf = new QtStreamBuf(logBox);
    StreamRedirector redirectCout(std::cout, qtBuf);
    StreamRedirector redirectCerr(std::cerr, qtBuf);

    std::cout << "Program started. Launching worker thread..." << std::endl;

    Processor *worker = new Processor(0);
    QThread *thread = new QThread();
    worker->moveToThread(thread);

    QObject::connect(thread, &QThread::started, worker, &Processor::process);
    QObject::connect(&app, &QApplication::aboutToQuit, [&]() {
        worker->requestStop();
        thread->quit();
        thread->wait();
    });

    QObject::connect(worker, &Processor::frameReady, [&](const QImage &img) { videoLabel->setFrame(img); });
    QObject::connect(worker, &Processor::logMessage, [&](const QString &msg) { std::cout << msg.toStdString() << std::endl; });
    QObject::connect(worker, &Processor::overlayUpdate, [&](const QPointF &pt, int idx, int total, bool visible) { videoLabel->setCalibrationTarget(pt, idx, total, visible); });
    QObject::connect(worker, &Processor::calibrationFinished, [&](bool ok) { std::cout << "Calibration finished: " << (ok ? "OK" : "FAIL") << std::endl; });

    QObject::connect(&mainWindow, &MainWindow::keyPressed, [&](int key) {
        if (key == Qt::Key_Escape) QApplication::quit();
        else if (key == Qt::Key_M) worker->toggleMouse();
        else if (key == Qt::Key_R) { worker->resetThresholds(); std::cout << "Reset thresholds\n"; }
        else if (key == Qt::Key_C) worker->startCalibration();
        else if (key == Qt::Key_Plus || key == Qt::Key_Equal) worker->setTargetFps(worker->getTargetFps() + 10.0);
        else if (key == Qt::Key_Minus) worker->setTargetFps(std::max(10.0, worker->getTargetFps() - 10.0));
    });

    thread->start();

    int ret = app.exec();

    worker->requestStop();
    thread->quit();
    thread->wait();
    delete worker;
    delete thread;
    delete qtBuf;
    return ret;
}

#include "main.moc"
