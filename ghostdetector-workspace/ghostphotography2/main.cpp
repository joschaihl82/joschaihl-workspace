// live_ghost_filter.cpp
// Ghost Filter (Gradient Separator) applied to a live video stream via Qt5/OpenCV.

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <QStatusBar>
#include <QDebug>

#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>

using namespace std;
using namespace cv;

// --- CONFIGURATION ---
// These values define the 'ghost cue' based on glass thickness and camera angle.
const int GHOST_DISPLACEMENT_X = 3;
const int GHOST_DISPLACEMENT_Y = 3;
// Cosine similarity threshold for classification. Higher = stricter.
const float ALIGNMENT_THRESHOLD = 0.95f;

// --- Utility: Mat to QImage Conversion (for display) ---
static QImage matToQImage(const cv::Mat &mat) {
    if (mat.empty()) return QImage();
    if (mat.type() == CV_8UC3) {
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, COLOR_BGR2RGB);
        return QImage((const uchar*)rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
    }
    if (mat.type() == CV_8UC1) {
        return QImage((const uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
    }
    return QImage();
}

// ====================================================================
// GHOST FILTER: GRADIENT SEPARATOR (STAGES I & II)
// ====================================================================

/**
 * @brief Isolates the Scene Gradient by suppressing Reflection Gradients based on the Ghost Cue.
 * @param inputImage Input frame.
 * @param sceneGradient Output: Purified scene gradient map (CV_32FC2).
 * @param weightMap Output: Confidence weight map (CV_32FC1).
 */
void separateGradients(const Mat& inputImage,
                       Mat& sceneGradient,
                       Mat& weightMap)
{
    // --- 1. Compute Image Gradients G ---
    Mat gray;
    cvtColor(inputImage, gray, COLOR_BGR2GRAY);

    Mat grad_x, grad_y;
    Sobel(gray, grad_x, CV_32F, 1, 0, 3);
    Sobel(gray, grad_y, CV_32F, 0, 1, 3);

    vector<Mat> channels = {grad_x, grad_y};
    Mat G;
    merge(channels, G); // G is CV_32FC2

    sceneGradient.create(G.size(), CV_32FC2);
    weightMap.create(G.size(), CV_32FC1);
    sceneGradient.setTo(0);
    weightMap.setTo(0);

    // --- 2. Iterate and Classify Edges ---
    const int rows = G.rows;
    const int cols = G.cols;
    const float MIN_MAGNITUDE_THRESHOLD = 1.5f;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {

            Point p(j, i);
            const Vec2f current_G = G.at<Vec2f>(p);
            float current_mag = norm(current_G);

            if (current_mag < MIN_MAGNITUDE_THRESHOLD) continue;

            // Ghost neighbors
            Point p_minus(j - GHOST_DISPLACEMENT_X, i - GHOST_DISPLACEMENT_Y);
            Point p_plus(j + GHOST_DISPLACEMENT_X, i + GHOST_DISPLACEMENT_Y);

            bool valid_minus = (p_minus.x >= 0 && p_minus.y >= 0);
            bool valid_plus = (p_plus.x < cols && p_plus.y < rows);

            float A_Ref_Minus = 0.0f;
            float A_Ref_Plus = 0.0f;
            float mag_minus = 0.0f;

            // Check alignment with p-
            if (valid_minus) {
                const Vec2f G_minus = G.at<Vec2f>(p_minus);
                mag_minus = norm(G_minus);
                if (mag_minus > MIN_MAGNITUDE_THRESHOLD) {
                    A_Ref_Minus = (current_G.dot(G_minus)) / (current_mag * mag_minus);
                }
            }
            // Check alignment with p+
            if (valid_plus) {
                const Vec2f G_plus = G.at<Vec2f>(p_plus);
                float mag_plus = norm(G_plus);
                if (mag_plus > MIN_MAGNITUDE_THRESHOLD) {
                    A_Ref_Plus = (current_G.dot(G_plus)) / (current_mag * mag_minus);
                }
            }

            // CLASSIFICATION
            bool is_reflection = (A_Ref_Minus > ALIGNMENT_THRESHOLD || A_Ref_Plus > ALIGNMENT_THRESHOLD);

            if (!is_reflection) {
                // Scene Edge: Keep gradient, set weight to 1.0
                sceneGradient.at<Vec2f>(p) = current_G;
                weightMap.at<float>(p) = 1.0f;
            } else {
                // Reflection Edge: Suppress gradient, set weight to 0.0
                sceneGradient.at<Vec2f>(p) = Vec2f(0.0f, 0.0f);
                weightMap.at<float>(p) = 0.0f;
            }
        }
    }
}

// STAGE III PLACEHOLDER: Visualize the output of the filter (Scene Gradient)
Mat visualizeSceneGradient(const Mat& sceneGradient) {
    Mat Gx_scene;
    Mat channels[2];
    split(sceneGradient, channels);

    // Convert the float X-gradient to an 8-bit image for display
    channels[0].convertTo(Gx_scene, CV_8UC1, 0.5, 127);

    putText(Gx_scene, "GHOST FILTER OUTPUT (Gradient Map)", Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255), 2);

    return Gx_scene;
}


class LiveGhostFilterApp : public QMainWindow {
    Q_OBJECT
public:
    LiveGhostFilterApp(QWidget *parent = nullptr) : QMainWindow(parent),
        timer(new QTimer(this))
    {
        setWindowTitle("Live Ghost Filter: Reflection Gradient Separator");

        // --- UI Setup ---
        QWidget *central = new QWidget;
        QVBoxLayout *mainLayout = new QVBoxLayout(central);

        inputLabel = new QLabel("Live Video Input");
        inputLabel->setAlignment(Qt::AlignCenter);
        inputLabel->setMinimumSize(320, 240);

        outputLabel = new QLabel("Ghost Filter Output (Scene Gradient)");
        outputLabel->setAlignment(Qt::AlignCenter);
        outputLabel->setMinimumSize(320, 240);

        QHBoxLayout *videoLayout = new QHBoxLayout;
        videoLayout->addWidget(inputLabel);
        videoLayout->addWidget(outputLabel);
        mainLayout->addLayout(videoLayout);

        setCentralWidget(central);

        // --- OpenCV Video Setup ---
        cap.open("/dev/video0");
        if (!cap.isOpened()) {
            QMessageBox::critical(this, "Fatal Error", "Cannot open webcam at /dev/video0. Check permissions or device path.");
            // If running in Qt Creator, you may need to run this command in the terminal:
            // LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libv4l/v4l2convert.so ./YOUR_APP_NAME
            qApp->exit(1);
            return;
        }

        cap.set(CAP_PROP_FRAME_WIDTH, 640);
        cap.set(CAP_PROP_FRAME_HEIGHT, 480);
        cap.set(CAP_PROP_FPS, 30);

        statusBar()->showMessage("Camera connected. Processing live feed...");

        // Start processing loop (33ms interval = ~30 FPS)
        connect(timer, &QTimer::timeout, this, &LiveGhostFilterApp::processFrame);
        timer->start(33);
    }

    ~LiveGhostFilterApp() override {
        if (cap.isOpened()) {
            cap.release();
        }
    }

private slots:
    void processFrame() {
        Mat inputFrame;
        cap >> inputFrame;

        if (inputFrame.empty()) {
            statusBar()->showMessage("Warning: Empty frame received.", 100);
            return;
        }

        // --- 1. Display Input ---
        QSize labelSize = inputLabel->size();
        QImage qimg_input = matToQImage(inputFrame);
        if (!qimg_input.isNull()) {
            inputLabel->setPixmap(QPixmap::fromImage(qimg_input).scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }

        // --- 2. RUN GHOST FILTER (Stages I & II) ---
        Mat sceneGradient;
        Mat weightMap;

        // The ghost filter function populates sceneGradient and weightMap
        separateGradients(inputFrame, sceneGradient, weightMap);

        // --- 3. Display Filter Output (Visualization of G_Scene) ---
        Mat filterOutputVis = visualizeSceneGradient(sceneGradient);

        QImage qimg_out = matToQImage(filterOutputVis);
        if (!qimg_out.isNull()) {
            outputLabel->setPixmap(QPixmap::fromImage(qimg_out).scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

private:
    cv::VideoCapture cap;
    QTimer *timer;
    QLabel *inputLabel;
    QLabel *outputLabel;
};

#include "main.moc"
c
// =========================================================================
// CRITICAL: MOC include for Qt slots/signals
// This file must be configured in your build system to generate this file.
// =========================================================================
// #include "live_ghost_filter.moc" // Uncomment this line if you are using QMake/CMake

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    LiveGhostFilterApp w;
    w.show();
    return a.exec();
}
