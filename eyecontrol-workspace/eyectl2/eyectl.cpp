// main.cpp
// Qt6 + OpenCV4 Eye Tracker mit 4-Punkt-Kalibrierung, Kalman-Filter + Median-Glättung
// Kalibrierung: 5s Countdown + 5s Sammeln pro Punkt.
// Hinweis per Ton (ALSA) und weißem Bildschirmblitz beim Übergang Countdown->Kalibrieren.
// Keine QRC, Haarcascades hartkodiert aus /usr/share/opencv4/haarcascades/
//
// Kompilieren (Beispiel):
// qmake6 && make
// ggf. in .pro: PKGCONFIG += opencv4
// und LIBS += -lasound (wenn nicht automatisch vorhanden)

#include <QApplication>
#include <QLabel>
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
#include <QWindow>
#include <QDebug>
#include <thread>
#include <deque>
#include <algorithm>
#include <cmath>

#include <opencv2/opencv.hpp>

#include <alsa/asoundlib.h>

    using namespace cv;

// --- ALSA beep helper (simple sine tone) ---
static bool playBeepALSA(double freqHz = 880.0, int ms = 200, int sampleRate = 44100, double amplitude = 0.25) {
    snd_pcm_t *pcm = nullptr;
    snd_pcm_hw_params_t *params = nullptr;
    int rc = snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        qWarning() << "ALSA: cannot open PCM device:" << snd_strerror(rc);
        return false;
    }
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm, params);
    snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm, params, 1);
    unsigned int rate = sampleRate;
    snd_pcm_hw_params_set_rate_near(pcm, params, &rate, 0);
    snd_pcm_uframes_t frames = 1024;
    snd_pcm_hw_params_set_period_size_near(pcm, params, &frames, 0);
    rc = snd_pcm_hw_params(pcm, params);
    if (rc < 0) {
        qWarning() << "ALSA: cannot set HW params:" << snd_strerror(rc);
        snd_pcm_close(pcm);
        return false;
    }

    int totalSamples = (int)((ms / 1000.0) * sampleRate);
    std::vector<int16_t> buffer(totalSamples);
    double twoPiF = 2.0 * M_PI * freqHz;
    for (int i = 0; i < totalSamples; ++i) {
        double t = double(i) / sampleRate;
        double s = amplitude * sin(twoPiF * t);
        // scale to int16
        buffer[i] = (int16_t)std::round(s * 32767.0);
    }

    // write in chunks
    int offset = 0;
    while (offset < totalSamples) {
        int toWrite = std::min<int>(frames, totalSamples - offset);
        rc = snd_pcm_writei(pcm, buffer.data() + offset, toWrite);
        if (rc == -EPIPE) {
            snd_pcm_prepare(pcm);
        } else if (rc < 0) {
            qWarning() << "ALSA write error:" << snd_strerror(rc);
            break;
        } else {
            offset += rc;
        }
    }

    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    return true;
}

// --- White flash overlay widget ---
class FlashWidget : public QWidget {
public:
    FlashWidget() {
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_TranslucentBackground, false);
        setAttribute(Qt::WA_ShowWithoutActivating);
        setStyleSheet("background: white;");
    }
    void showFullScreenFor(int ms) {
        showFullScreen();
        QTimer::singleShot(ms, this, [this]() { hide(); });
    }
};

// Hilfsfunktion: mappt Punkt mit 2x3 Affine-Matrix (CV_64F)
static Point2f mapPointAffine(const Mat &aff, const Point2f &p) {
    if (aff.empty()) return Point2f(-1,-1);
    double a00 = aff.at<double>(0,0), a01 = aff.at<double>(0,1), a02 = aff.at<double>(0,2);
    double a10 = aff.at<double>(1,0), a11 = aff.at<double>(1,1), a12 = aff.at<double>(1,2);
    double x = a00 * p.x + a01 * p.y + a02;
    double y = a10 * p.x + a11 * p.y + a12;
    return Point2f((float)x, (float)y);
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QLabel view;
    view.setWindowTitle("Eye Tracker - Kalibrierung + Kalman + Flash/Beep");
    view.resize(900, 700);
    view.show();

    // Kamera öffnen
    VideoCapture cap(0, cv::CAP_ANY);
    if (!cap.isOpened()) {
        qFatal("Cannot open camera (index 0).");
        return -1;
    }
    cap.set(CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CAP_PROP_FRAME_HEIGHT, 480);

    // Hartkodierte Pfade zu Haarcascades
    const std::string cascadesBase = "/usr/share/opencv4/haarcascades/";
    const std::string faceCascadePath = cascadesBase + "haarcascade_frontalface_default.xml";
    const std::string eyeCascadePath  = cascadesBase + "haarcascade_eye.xml";

    CascadeClassifier faceCascade;
    CascadeClassifier eyeCascade;
    if (!faceCascade.load(faceCascadePath)) {
        qFatal("Failed to load face cascade from %s", faceCascadePath.c_str());
        return -1;
    }
    if (!eyeCascade.load(eyeCascadePath)) {
        qFatal("Failed to load eye cascade from %s", eyeCascadePath.c_str());
        return -1;
    }

    // Flash widget (full-screen white)
    FlashWidget flash;
    // Kalibrierungszustand
    enum Mode { CALIBRATE, TRACK };
    Mode mode = CALIBRATE;

    // Bildschirm-Targets (4 Ecken mit Margin)
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeom = screen->geometry();
    const int margin = 120;
    std::vector<QPoint> screenTargets = {
        QPoint(screenGeom.x() + margin, screenGeom.y() + margin),                                 // oben links
        QPoint(screenGeom.x() + screenGeom.width() - margin, screenGeom.y() + margin),            // oben rechts
        QPoint(screenGeom.x() + margin, screenGeom.y() + screenGeom.height() - margin),           // unten links
        QPoint(screenGeom.x() + screenGeom.width() - margin, screenGeom.y() + screenGeom.height() - margin) // unten rechts
    };

    // Anzeigeprojektion: Screen->View (nur für Visualisierung)
    auto screenToView = [&](const QPoint &sp) -> QPoint {
        int vw = view.width(), vh = view.height();
        int vx = int((double)(sp.x() - screenGeom.x()) / screenGeom.width() * vw);
        int vy = int((double)(sp.y() - screenGeom.y()) / screenGeom.height() * vh);
        return QPoint(vx, vy);
    };

    // Kalibrierungsdaten
    const int numTargets = 4;
    std::vector<std::vector<Point2f>> samples(numTargets);

    // Zeiten: 5s Countdown + 5s Sammeln
    const int countdownMs = 5000;
    const int collectMs = 5000;
    int currentTarget = 0;
    QElapsedTimer phaseTimer;
    bool inCountdown = true;
    phaseTimer.start();

    // Affine Transform (pupil -> screen)
    Mat affine; // 2x3 CV_64F

    // Kalman-Filter initialisieren (state: x,y,vx,vy ; meas: x,y)
    KalmanFilter kf(4, 2, 0, CV_64F);
    double dtInit = 1.0;
    Mat A = (Mat_<double>(4,4) << 1,0,dtInit,0,  0,1,0,dtInit,  0,0,1,0,  0,0,0,1);
    kf.transitionMatrix = A;
    kf.measurementMatrix = Mat::zeros(2,4,CV_64F);
    kf.measurementMatrix.at<double>(0,0) = 1.0;
    kf.measurementMatrix.at<double>(1,1) = 1.0;
    setIdentity(kf.processNoiseCov, Scalar::all(1e-3));
    setIdentity(kf.measurementNoiseCov, Scalar::all(1e-1));
    setIdentity(kf.errorCovPost, Scalar::all(1.0));
    kf.statePost = Mat::zeros(4,1,CV_64F);

    // Median-Puffer
    const int MEDIAN_WINDOW = 5;
    std::deque<Point2f> medianBuf;

    // Cursor smoothing / limits
    QPoint lastCursorPos = QCursor::pos();
    const double MAX_STEP = 80.0; // max Pixel pro Frame

    // Timing für Kalman dt
    qint64 lastTs = QDateTime::currentMSecsSinceEpoch();

    QTimer timer;
    timer.setInterval(30);
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        Mat frame;
        cap >> frame;
        if (frame.empty()) return;

        Mat gray;
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        equalizeHist(gray, gray);

        // Gesichtserkennung
        std::vector<Rect> faces;
        faceCascade.detectMultiScale(gray, faces, 1.1, 3, 0, Size(100,100));

        // Maske: nur Augen sichtbar lassen
        Mat masked = Mat::zeros(frame.size(), frame.type());
        Mat mask = Mat::zeros(frame.size(), CV_8UC1);

        Point2f bestPupil(-1,-1);
        double bestArea = 0.0;

        for (const Rect &f : faces) {
            Mat faceROIgray = gray(f);
            Rect upperFaceRegion(0, 0, f.width, int(f.height * 0.6));
            Mat searchRegion = faceROIgray(upperFaceRegion);

            std::vector<Rect> eyes;
            eyeCascade.detectMultiScale(searchRegion, eyes, 1.1, 3, 0, Size(20,20));

            for (const Rect &eRel : eyes) {
                Rect e(eRel.x, eRel.y, eRel.width, eRel.height);
                Point center(f.x + e.x + e.width/2, f.y + e.y + e.height/2);
                Size axes(e.width/2, e.height/2);
                ellipse(mask, center, axes, 0, 0, 360, Scalar(255), -1);
            }
        }

        frame.copyTo(masked, mask);
        Mat grayMasked;
        cvtColor(masked, grayMasked, COLOR_BGR2GRAY);

        // Pupillenerkennung
        if (countNonZero(grayMasked) > 50) {
            Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8,8));
            Mat claheOut;
            clahe->apply(grayMasked, claheOut);
            GaussianBlur(claheOut, claheOut, Size(7,7), 0);

            Mat th;
            adaptiveThreshold(claheOut, th, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 15, 7);
            Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3,3));
            morphologyEx(th, th, MORPH_OPEN, kernel);
            morphologyEx(th, th, MORPH_CLOSE, kernel);

            Mat edges;
            Canny(th, edges, 50, 150);
            std::vector<std::vector<Point>> contours;
            findContours(edges, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

            for (const auto &c : contours) {
                double area = contourArea(c);
                if (area < 30) continue;
                if (c.size() < 6) continue;
                RotatedRect rr = fitEllipse(c);
                double ratio = rr.size.width / rr.size.height;
                if (ratio < 0.4 || ratio > 2.5) continue;
                if (area > bestArea) {
                    bestArea = area;
                    bestPupil = rr.center;
                }
            }
        }

        // --- Kalibrierungslogik mit Flash+Beep beim Übergang Countdown->Collect ---
        if (mode == CALIBRATE) {
            qint64 elapsed = phaseTimer.elapsed();
            if (inCountdown) {
                if (elapsed >= countdownMs) {
                    // Übergang: mache kurzen weißen Blitz und Ton, dann starte Sammeln
                    flash.showFullScreenFor(200); // 200 ms weißer Bildschirm
                    // Play beep asynchronously to avoid blocking UI thread: use a detached Qt thread via QtConcurrent or std::thread
                    std::thread([](){ playBeepALSA(880.0, 200); }).detach();
                    inCountdown = false;
                    phaseTimer.restart();
                }
            } else {
                if (bestPupil.x >= 0) {
                    samples[currentTarget].push_back(bestPupil);
                }
                if (phaseTimer.elapsed() >= collectMs) {
                    currentTarget++;
                    if (currentTarget >= numTargets) {
                        // Berechne affine Transform
                        std::vector<Point2f> srcPts;
                        std::vector<Point2f> dstPts;
                        bool ok = true;
                        for (int i = 0; i < numTargets; ++i) {
                            const auto &vec = samples[i];
                            if (vec.empty()) { ok = false; break; }
                            Point2f avg(0,0);
                            for (const auto &p : vec) avg += p;
                            avg.x /= (float)vec.size();
                            avg.y /= (float)vec.size();
                            srcPts.push_back(avg);
                            QPoint sp = screenTargets[i];
                            dstPts.push_back(Point2f((float)sp.x(), (float)sp.y()));
                        }
                        if (!ok) {
                            qWarning() << "Kalibrierung unvollständig, starte neu.";
                            for (auto &v : samples) v.clear();
                            currentTarget = 0;
                            inCountdown = true;
                            phaseTimer.restart();
                        } else {
                            Mat srcMat(srcPts), dstMat(dstPts);
                            srcMat.convertTo(srcMat, CV_64F);
                            dstMat.convertTo(dstMat, CV_64F);
                            Mat inliers;
                            affine = estimateAffine2D(srcMat, dstMat, inliers, RANSAC, 3.0);
                            if (affine.empty()) {
                                qWarning() << "Affine Transform konnte nicht berechnet werden.";
                                for (auto &v : samples) v.clear();
                                currentTarget = 0;
                                inCountdown = true;
                                phaseTimer.restart();
                            } else {
                                if (affine.type() != CV_64F) affine.convertTo(affine, CV_64F);
                                mode = TRACK;
                                qDebug() << "Kalibrierung abgeschlossen. Wechsel zu Tracking.";
                                // kurzer Bestätigungsblitz + Ton
                                flash.showFullScreenFor(200);
                                std::thread([](){ playBeepALSA(1200.0, 250); }).detach();
                            }
                        }
                    } else {
                        // Wechsel zum nächsten Target: Countdown wieder starten
                        inCountdown = true;
                        phaseTimer.restart();
                    }
                }
            }
        }

        // --- Kalman + Glättung + Cursorbewegung ---
        qint64 nowTs = QDateTime::currentMSecsSinceEpoch();
        double dtSec = std::max<double>(1.0, double(nowTs - lastTs)) / 1000.0;
        lastTs = nowTs;
        kf.transitionMatrix.at<double>(0,2) = dtSec;
        kf.transitionMatrix.at<double>(1,3) = dtSec;

        Mat pred = kf.predict();
        Point2f predPt((float)pred.at<double>(0), (float)pred.at<double>(1));

        bool haveMeasurement = (bestPupil.x >= 0);
        const double GATE_PIX = 80.0;
        if (haveMeasurement) {
            double dx = bestPupil.x - predPt.x;
            double dy = bestPupil.y - predPt.y;
            double dist = std::sqrt(dx*dx + dy*dy);
            if (dist > GATE_PIX) haveMeasurement = false;
        }

        if (haveMeasurement) {
            Mat meas = Mat::zeros(2,1,CV_64F);
            meas.at<double>(0) = bestPupil.x;
            meas.at<double>(1) = bestPupil.y;
            kf.correct(meas);
        }

        Mat state = kf.statePost;
        Point2f filtPt((float)state.at<double>(0), (float)state.at<double>(1));

        // Median-Puffer
        medianBuf.push_back(filtPt);
        if ((int)medianBuf.size() > MEDIAN_WINDOW) medianBuf.pop_front();
        Point2f medianPt(0,0);
        if (!medianBuf.empty()) {
            std::vector<float> xs, ys;
            for (auto &p : medianBuf) { xs.push_back(p.x); ys.push_back(p.y); }
            std::sort(xs.begin(), xs.end()); std::sort(ys.begin(), ys.end());
            medianPt.x = xs[xs.size()/2];
            medianPt.y = ys[ys.size()/2];
        } else medianPt = filtPt;

        // Mapping auf Bildschirm
        Point2f screenPt;
        if (!affine.empty()) {
            screenPt = mapPointAffine(affine, medianPt);
        } else {
            double nx = medianPt.x / double(frame.cols);
            double ny = medianPt.y / double(frame.rows);
            screenPt.x = float(screenGeom.x() + int(nx * screenGeom.width()));
            screenPt.y = float(screenGeom.y() + int(ny * screenGeom.height()));
        }

        // Begrenze Schrittweite
        QPoint last = lastCursorPos;
        double stepDx = screenPt.x - last.x();
        double stepDy = screenPt.y - last.y();
        double stepDist = std::sqrt(stepDx*stepDx + stepDy*stepDy);
        if (stepDist > MAX_STEP) {
            double scale = MAX_STEP / stepDist;
            stepDx *= scale; stepDy *= scale;
        }
        QPoint newPos(int(last.x() + stepDx), int(last.y() + stepDy));
        if (mode == TRACK) {
            QCursor::setPos(newPos);
            lastCursorPos = newPos;
        }

        // --- Anzeige ---
        Mat display;
        addWeighted(frame, 0.85, masked, 0.15, 0, display);
        cvtColor(display, display, COLOR_BGR2RGB);
        QImage img((uchar*)display.data, display.cols, display.rows, display.step, QImage::Format_RGB888);

        QPixmap pix = QPixmap::fromImage(img).scaled(view.size(), Qt::KeepAspectRatio);
        QPainter painter(&pix);
        painter.setRenderHint(QPainter::Antialiasing);
        QFont font = painter.font();
        font.setPointSize(18);
        font.setBold(true);
        painter.setFont(font);

        if (mode == CALIBRATE) {
            painter.setPen(Qt::yellow);
            QString msg;
            int remainingSec = 0;
            if (inCountdown) {
                qint64 rem = std::max<qint64>(0, countdownMs - phaseTimer.elapsed());
                remainingSec = int((rem + 999) / 1000);
                switch (currentTarget) {
                case 0: msg = "Bitte oben links hinschauen"; break;
                case 1: msg = "Bitte oben rechts hinschauen"; break;
                case 2: msg = "Bitte unten links hinschauen"; break;
                case 3: msg = "Bitte unten rechts hinschauen"; break;
                default: msg = "Kalibrierung"; break;
                }
                painter.drawText(pix.rect().adjusted(10,10,-10,-10), Qt::AlignTop | Qt::AlignLeft, msg);
                painter.drawText(pix.rect(), Qt::AlignCenter, QString("Start in %1 s").arg(remainingSec));
            } else {
                qint64 rem = std::max<qint64>(0, collectMs - phaseTimer.elapsed());
                remainingSec = int((rem + 999) / 1000);
                switch (currentTarget) {
                case 0: msg = "Kalibriere oben links"; break;
                case 1: msg = "Kalibriere oben rechts"; break;
                case 2: msg = "Kalibriere unten links"; break;
                case 3: msg = "Kalibriere unten rechts"; break;
                default: msg = "Kalibrierung"; break;
                }
                painter.drawText(pix.rect().adjusted(10,10,-10,-10), Qt::AlignTop | Qt::AlignLeft, msg);
                painter.drawText(pix.rect(), Qt::AlignCenter, QString("Kalibrierung: %1 s").arg(remainingSec));
            }
            if (currentTarget >= 0 && currentTarget < numTargets) {
                QPoint vp = screenToView(screenTargets[currentTarget]);
                painter.setPen(Qt::red);
                painter.setBrush(Qt::NoBrush);
                int r = 20;
                painter.drawEllipse(vp, r, r);
                painter.setPen(Qt::white);
                painter.drawText(vp + QPoint(-r, -r-10), "Ziel");
            }
        } else {
            painter.setPen(Qt::green);
            painter.drawText(pix.rect().adjusted(10,10,-10,-10), Qt::AlignTop | Qt::AlignLeft, "Tracking aktiv");
        }

        // Markiere gefundene Pupille (falls vorhanden)
        if (bestPupil.x >= 0) {
            double scale = std::min((double)view.width() / display.cols, (double)view.height() / display.rows);
            int dispW = int(display.cols * scale);
            int dispH = int(display.rows * scale);
            int offsetX = (view.width() - dispW) / 2;
            int offsetY = (view.height() - dispH) / 2;
            int px = int(bestPupil.x * scale) + offsetX;
            int py = int(bestPupil.y * scale) + offsetY;
            painter.setPen(Qt::red);
            painter.setBrush(Qt::red);
            painter.drawEllipse(QPoint(px, py), 5, 5);
        }

        painter.end();
        view.setPixmap(pix);
    });

    timer.start();
    return app.exec();
}
