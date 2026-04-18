// ghost_remover.cpp
// Live Video Implementation of Reflection Removal by Ghost Effect (Shih, 2012)
// Captures video from /dev/video0 and applies gradient separation in real-time.
// NOTE: Stage III (Poisson Reconstruction) remains a placeholder for demonstration.

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <QStatusBar>
#include <QDebug>
#include <QPushButton>

#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>

using namespace std;
using namespace cv;

// --- CONFIGURATION ---
// Set the estimated ghost displacement (d) here. This is CRITICAL for the algorithm.
// Adjust these values to match the expected ghost shift from your glass/camera setup.
const int GHOST_DISPLACEMENT_X = 2;
const int GHOST_DISPLACEMENT_Y = 2;

// Threshold for gradient alignment (cosine similarity). Higher = stricter alignment required for reflection.
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
        // Use a standard grayscale palette for single channel images
        return QImage((const uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
    }
    return QImage();
}

// --- CORE ALGORITHM FUNCTIONS ---

// Stage III Placeholder: Requires a separate Poisson Solver implementation.
Mat reconstructSceneFromGradient(const Mat& sceneGradient, const Mat& weightMap) {
    // --- POISSON RECONSTRUCTION PLACEHOLDER ---
    // This function should solve the Poisson Equation to create the final image.

    // For visualization in this live demo, we convert the scene gradient magnitude to an 8-bit image.
    Mat gx, gy;
    Mat channels[2];
    split(sceneGradient, channels);

    // Convert the X-gradient to 8-bit visualization (centered at 127)
    channels[0].convertTo(gx, CV_8UC1, 0.5, 127);

    // Set text overlay for clarity
    putText(gx, "RECONSTRUCTION PLACEHOLDER", Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255), 2);

    return gx;
}


// Stage I & II: Separates the gradients G into Scene (G_S) and Reflection (G_R).
Mat separateGradients(const Mat& inputImage, Mat& sceneGradient, Mat& weightMap) {
    // 1. Compute Gradients G (Sobel on grayscale)
    Mat gray;
    cvtColor(inputImage, gray, COLOR_BGR2GRAY);

    Mat grad_x, grad_y;
    Sobel(gray, grad_x, CV_32F, 1, 0, 3);
    Sobel(gray, grad_y, CV_32F, 0, 1, 3);

    vector<Mat> channels = {grad_x, grad_y};
    Mat G;
    merge(channels, G); // G is CV_32FC2

    // Initialize output maps
    sceneGradient.create(G.size(), CV_32FC2);
    weightMap.create(G.size(), CV_32FC1);
    sceneGradient.setTo(0);
    weightMap.setTo(0);

    // 2. Iterate and Classify Edges (using Ghost Cue)
    const int rows = G.rows;
    const int cols = G.cols;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {

            Point p(j, i);
            const Vec2f current_G = G.at<Vec2f>(p);
            float current_mag = norm(current_G);

            if (current_mag < 1.5) { // Ignore small gradients (noise suppression)
                continue;
            }

            Point p_minus(j - GHOST_DISPLACEMENT_X, i - GHOST_DISPLACEMENT_Y);
            Point p_plus(j + GHOST_DISPLACEMENT_X, i + GHOST_DISPLACEMENT_Y);

            bool valid_minus = (p_minus.x >= 0 && p_minus.y >= 0);
            bool valid_plus = (p_plus.x < cols && p_plus.y < rows);

            float A_Ref_Minus = 0.0f;
            float A_Ref_Plus = 0.0f;

            if (valid_minus) {
                const Vec2f G_minus = G.at<Vec2f>(p_minus);
                float mag_minus = norm(G_minus);
                if (mag_minus > 1.5f) {
                    A_Ref_Minus = (current_G.dot(G_minus)) / (current_mag * mag_minus);
                }
            }

            if (valid_plus) {
                const Vec2f G_plus = G.at<Vec2f>(p_plus);
                float mag_plus = norm(G_plus);
                if (mag_plus > 1.5f) {
                    A_Ref_Plus = (current_G.dot(G_plus)) / (current_mag * mag_plus);
                }
            }

            // Classification: If highly aligned with a ghosted neighbor, it's reflection.
            bool is_reflection = false;
            if (A_Ref_Minus > ALIGNMENT_THRESHOLD || A_Ref_Plus > ALIGNMENT_THRESHOLD) {
                is_reflection = true;
            }

            // Assignment and Weight Update
            if (!is_reflection) {
                // Scene Edge: Keep gradient and set high confidence weight
                sceneGradient.at<Vec2f>(p) = current_G;
                weightMap.at<float>(p) = 1.0f;
            } else {
                // Reflection Edge: Suppress gradient and set low confidence weight
                sceneGradient.at<Vec2f>(p) = Vec2f(0.0f, 0.0f);
                weightMap.at<float>(p) = 0.0f;
            }
        }
    }

    // Return the original input frame for side-by-side comparison
    return inputImage;
}


class LiveReflectionRemover : public QMainWindow {
    Q_OBJECT
public:
    LiveReflectionRemover(QWidget *parent = nullptr) : QMainWindow(parent),
        timer(new QTimer(this))
    {
        setWindowTitle("Live Reflection Remover (Shih 2012)");

        // --- UI Setup ---
        QWidget *central = new QWidget;
        QVBoxLayout *mainLayout = new QVBoxLayout(central);

        // Labels for live feed
        inputLabel = new QLabel("Live Video (/dev/video0)");
        inputLabel->setAlignment(Qt::AlignCenter);

        outputLabel = new QLabel("Reflection-Removed Output");
        outputLabel->setAlignment(Qt::AlignCenter);

        QHBoxLayout *videoLayout = new QHBoxLayout;
        videoLayout->addWidget(inputLabel);
        videoLayout->addWidget(outputLabel);
        mainLayout->addLayout(videoLayout);

        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(5);
        setCentralWidget(central);

        // --- OpenCV Video Setup ---
        // Open the specified video device
        cap.open("/dev/video0");
        if (!cap.isOpened()) {
            QMessageBox::critical(this, "Fatal Error", "Cannot open webcam at /dev/video0. Check permissions or device path.");
            qApp->exit(1);
            return;
        }

        // Set properties for better performance/standard size
        cap.set(CAP_PROP_FRAME_WIDTH, 640);
        cap.set(CAP_PROP_FRAME_HEIGHT, 480);
        cap.set(CAP_PROP_FPS, 30); // Request 30 FPS

        statusBar()->showMessage("Camera connected. Processing live feed...");

        // Start processing loop (aim for 30 FPS, 33ms interval)
        connect(timer, &QTimer::timeout, this, &LiveReflectionRemover::processFrame);
        timer->start(33);
    }

    ~LiveReflectionRemover() override {
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

        // Ensure labels have size for scaling
        QSize labelSize = inputLabel->size();

        // Display raw input frame
        QImage qimg_input = matToQImage(inputFrame);
        if (!qimg_input.isNull()) {
            inputLabel->setPixmap(QPixmap::fromImage(qimg_input).scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }

        // --- RUN REFLECTION REMOVAL ALGORITHM ---
        Mat sceneGradient;
        Mat weightMap;

        // This function returns the original frame (inputFrame) for display in this structure,
        // but its main action is filling sceneGradient and weightMap.
        Mat rawOutputVis = separateGradients(inputFrame, sceneGradient, weightMap);

        // --- STAGE III: RECONSTRUCTION (Placeholder) ---
        Mat reconstructedMat = reconstructSceneFromGradient(sceneGradient, weightMap);

        // Display Reconstruction Placeholder
        QImage qimg_out = matToQImage(reconstructedMat);
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

// =========================================================================
// CRITICAL FIX: The MOC include MUST be here
// =========================================================================

#include "main.moc"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    LiveReflectionRemover w;
    w.show();
    return a.exec();
}
