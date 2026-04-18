// ghost.cpp
// Qt5 realtime ghost filter without OpenCV
// (Fixed: Numerische Stabilität für weißen Bildschirm, Resize-Loop-Bug und boxBlur-Bug)
//
// qmake project: QT += core gui widgets multimedia
// Compile: qmake && make

#include <QApplication>
#include <QCamera>
#include <QCameraInfo>
#include <QAbstractVideoSurface>
#include <QVideoFrame>
#include <QVideoSurfaceFormat>
#include <QLabel>
#include <QImage>
#include <QVBoxLayout>
#include <QWidget>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>
#include <cmath>
#include <vector>
#include <algorithm>

static inline float clampf(float v, float a = 0.0f, float b = 1.0f) {
    return v < a ? a : (v > b ? b : v);
}

// Convert YUYV (YUY2) buffer to QImage RGB888
static QImage yuyvToImage(const uchar* src, int w, int h, int bytesPerLine) {
    QImage img(w, h, QImage::Format_RGB888);
    for (int y = 0; y < h; ++y) {
        const uchar* row = src + y * bytesPerLine;
        uchar* out = img.scanLine(y);
        int outIdx = 0;
        for (int x = 0; x < w; x += 2) {
            int Y0 = row[0];
            int U  = row[1];
            int Y1 = row[2];
            int V  = row[3];
            auto conv = [&](int Y, int Uv, int Vv) {
                int C = Y - 16;
                int D = Uv - 128;
                int E = Vv - 128;
                int R = (298 * C + 409 * E + 128) >> 8;
                int G = (298 * C - 100 * D - 208 * E + 128) >> 8;
                int B = (298 * C + 516 * D + 128) >> 8;
                R = std::min(255, std::max(0, R));
                G = std::min(255, std::max(0, G));
                B = std::min(255, std::max(0, B));
                out[outIdx++] = static_cast<uchar>(R);
                out[outIdx++] = static_cast<uchar>(G);
                out[outIdx++] = static_cast<uchar>(B);
            };
            conv(Y0, U, V);
            conv(Y1, U, V);
            row += 4;
        }
    }
    return img;
}

// Video surface for Qt5 that emits QImage frames
class VideoSurface : public QAbstractVideoSurface {
    Q_OBJECT
public:
    explicit VideoSurface(QObject* parent = nullptr) : QAbstractVideoSurface(parent) {}

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType) const override {
        return {
            QVideoFrame::Format_RGB32,
            QVideoFrame::Format_ARGB32,
            QVideoFrame::Format_RGB24,
            QVideoFrame::Format_YUYV
        };
    }

signals:
    void frameAvailable(const QImage& img);

public:
    bool present(const QVideoFrame &frame) override {
        if (!frame.isValid()) return false;
        QVideoFrame f(frame);
        if (!f.map(QAbstractVideoBuffer::ReadOnly)) return false;

        QImage img;
        switch (f.pixelFormat()) {
            case QVideoFrame::Format_RGB32:
                img = QImage(f.bits(), f.width(), f.height(), f.bytesPerLine(), QImage::Format_RGB32).copy();
                break;
            case QVideoFrame::Format_ARGB32:
                img = QImage(f.bits(), f.width(), f.height(), f.bytesPerLine(), QImage::Format_ARGB32).copy();
                break;
            case QVideoFrame::Format_RGB24:
                img = QImage(f.bits(), f.width(), f.height(), f.bytesPerLine(), QImage::Format_RGB888).copy();
                break;
            case QVideoFrame::Format_YUYV:
                img = yuyvToImage(reinterpret_cast<const uchar*>(f.bits()), f.width(), f.height(), f.bytesPerLine());
                break;
            default:
                f.unmap();
                return false;
        }
        f.unmap();
        emit frameAvailable(img);
        return true;
    }
};

// GhostWidget implements the DSP ghost filter and displays result
class GhostWidget : public QWidget {
    Q_OBJECT
public:
    explicit GhostWidget(QWidget* parent = nullptr)
        : QWidget(parent), label(new QLabel(this)), W(0), H(0)
    {
        label->setAlignment(Qt::AlignCenter);

        // FIX: Hinzufügen von setScaledContents und setSizePolicy zur Behebung des Resize-Bugs
        label->setScaledContents(true);
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        QVBoxLayout* l = new QVBoxLayout(this);
        l->addWidget(label);
        setLayout(l);
        initBuffers(640, 480);
    }

public slots:
    void onFrame(const QImage& img) {
        QImage rgb = img.convertToFormat(QImage::Format_RGB888);
        QImage scaled = rgb.scaled(targetW, targetH, Qt::KeepAspectRatioByExpanding);
        processFrame(scaled);
    }

private:
    QLabel* label;
    QMutex mutex;

    // parameters
    const int targetW = 640;
    const int targetH = 480;
    const float BG_LEARNING = 0.02f;
    const float MOTION_THRESH = 0.06f;
    const float TRAIL_DECAY = 0.092f;
    const float TRAIL_GAIN = 0.9f;
    const int POSTERIZE_LEVELS = 8;
    const float BLOOM_WEIGHT = 0.6f;
    const int BLOOM_RADIUS = 7;
    const float EDGE_WEIGHT = 0.9f;

    int W, H;
    std::vector<float> background; // W*H*3
    std::vector<float> trail;      // W*H*3
    std::vector<float> tmpBuf;     // W*H*3

    void initBuffers(int w, int h) {
        W = w; H = h;
        background.assign(W * H * 3, 0.0f);
        trail.assign(W * H * 3, 0.0f);
        tmpBuf.assign(W * H * 3, 0.0f);
    }

    // separable box blur for float RGB buffer (Fixed for stability/index errors)
    void boxBlur(const std::vector<float>& src, std::vector<float>& dst, int radius) {
        if (radius <= 0) { dst = src; return; }
        int k = radius;
        int w = W, h = H;
        if (tmpBuf.size() != w*h*3) tmpBuf.assign(w*h*3, 0.0f);
        
        int kernel = 2*k + 1;
        float invKernel = 1.0f / kernel;

        // Horizontal Pass
        for (int y = 0; y < h; ++y) {
            for (int c = 0; c < 3; ++c) {
                float sum = 0.0f;
                // Initial sum for the first pixel (x=0)
                for (int x_init = -k; x_init <= k; ++x_init) {
                    int xx = std::min(w - 1, std::max(0, x_init));
                    sum += src[(y * w + xx) * 3 + c];
                }

                for (int x = 0; x < w; ++x) {
                    tmpBuf[(y * w + x) * 3 + c] = sum * invKernel;
                    
                    int x_out = x - k;
                    int x_in = x + k + 1;

                    // Clamped indices for sliding window update
                    int x_out_clamped = std::min(w - 1, std::max(0, x_out));
                    int x_in_clamped = std::min(w - 1, x_in);

                    sum -= src[(y * w + x_out_clamped) * 3 + c];
                    sum += src[(y * w + x_in_clamped) * 3 + c];
                }
            }
        }
        
        // Vertical Pass
        for (int x = 0; x < w; ++x) {
            for (int c = 0; c < 3; ++c) {
                float sum = 0.0f;
                // Initial sum for the first pixel (y=0)
                for (int y_init = -k; y_init <= k; ++y_init) {
                    int yy = std::min(h - 1, std::max(0, y_init));
                    sum += tmpBuf[(yy * w + x) * 3 + c];
                }

                for (int y = 0; y < h; ++y) {
                    dst[(y * w + x) * 3 + c] = sum * invKernel;
                    int y_out = y - k;
                    int y_in = y + k + 1;

                    int y_out_clamped = std::min(h - 1, std::max(0, y_out));
                    int y_in_clamped = std::min(h - 1, y_in);
                    
                    sum -= tmpBuf[(y_out_clamped * w + x) * 3 + c];
                    sum += tmpBuf[(y_in_clamped * w + x) * 3 + c];
                }
            }
        }
    }

    // Sobel edges on grayscale
    void sobelEdges(const std::vector<float>& gray, std::vector<float>& edges) {
        edges.assign(W*H, 0.0f);
        for (int y=1;y<H-1;++y) {
            for (int x=1;x<W-1;++x) {
                float gx = -gray[(y-1)*W + (x-1)] - 2.0f*gray[y*W + (x-1)] - gray[(y+1)*W + (x-1)]
                           + gray[(y-1)*W + (x+1)] + 2.0f*gray[y*W + (x+1)] + gray[(y+1)*W + (x+1)];
                float gy = -gray[(y-1)*W + (x-1)] - 2.0f*gray[(y-1)*W + x] - gray[(y-1)*W + (x+1)]
                           + gray[(y+1)*W + (x-1)] + 2.0f*gray[(y+1)*W + x] + gray[(y+1)*W + (x+1)];
                float mag = std::sqrt(gx*gx + gy*gy);
                edges[y*W + x] = clampf(mag * 0.25f);
            }
        }
    }

    void processFrame(const QImage& img) {
        QMutexLocker lock(&mutex);
        if (img.width() != W || img.height() != H) {
            initBuffers(img.width(), img.height());
        }

        // Convert to float RGB [0..1]
        std::vector<float> frame(W*H*3);
        for (int y=0;y<H;++y) {
            const uchar* scan = img.scanLine(y);
            for (int x=0;x<W;++x) {
                int idx = (y*W + x)*3;
                frame[idx+0] = scan[x*3 + 0] / 255.0f; // R
                frame[idx+1] = scan[x*3 + 1] / 255.0f; // G
                frame[idx+2] = scan[x*3 + 2] / 255.0f; // B
            }
        }

        // Background running average
        if (background.empty()) background = frame;
        else {
            for (size_t i=0;i<background.size();++i) {
                background[i] = background[i] * (1.0f - BG_LEARNING) + frame[i] * BG_LEARNING;
                // FIX: Numerische Stabilität für den Hintergrund
                background[i] = clampf(background[i], 0.0f, 1.0f);
            }
        }

        // Motion mask (grayscale diff)
        // ... (Logik unverändert) ...
        std::vector<float> diffGray(W*H);
        for (int i=0;i<W*H;++i) {
            float r = frame[i*3+0], g = frame[i*3+1], b = frame[i*3+2];
            float br = background[i*3+0], bgc = background[i*3+1], bb = background[i*3+2];
            float d = std::fabs(r - br) + std::fabs(g - bgc) + std::fabs(b - bb);
            diffGray[i] = clampf(d / 3.0f);
        }

        // simple blur on diffGray to reduce noise (radius 1)
        // ... (Logik unverändert) ...
        std::vector<float> diffBlur(W*H, 0.0f);
        std::vector<float> diffH(W*H, 0.0f); 
        int kr = 1;
        float invKernel = 1.0f / (2*kr+1);
        
        // Horizontal
        for (int y=0;y<H;++y) {
            float sum = 0.0f;
            for (int x=-kr;x<=kr;++x) sum += diffGray[y*W + std::min(W-1, std::max(0, x))];
            for (int x=0;x<W;++x) {
                diffH[y*W + x] = sum * invKernel;
                int x_out = x - kr;
                int x_in  = x + kr + 1;
                if (x_out >= 0) sum -= diffGray[y*W + x_out];
                if (x_in < W) sum += diffGray[y*W + x_in];
            }
        }
        
        // Vertical
        for (int x=0;x<W;++x) {
            float sum = 0.0f;
            for (int y=-kr;y<=kr;++y) sum += diffH[std::min(H-1, std::max(0, y))*W + x];
            for (int y=0;y<H;++y) {
                diffBlur[y*W + x] = sum * invKernel;
                int y_out = y - kr;
                int y_in  = y + kr + 1;
                if (y_out >= 0) sum -= diffH[y_out*W + x];
                if (y_in < H) sum += diffH[y_in*W + x];
            }
        }

        // Motion mask binary
        std::vector<float> motionMask(W*H, 0.0f);
        for(int i=0; i<W*H; ++i) {
             motionMask[i] = diffBlur[i] > MOTION_THRESH ? 1.0f : 0.0f;
        }

        // Update trail buffer
        for (int i=0;i<W*H;++i) {
            float m = motionMask[i];
            for (int c=0;c<3;++c) {
                int idx = i*3 + c;
                trail[idx] = trail[idx] * TRAIL_DECAY + frame[idx] * (m * TRAIL_GAIN);
                // FIX: Numerische Stabilität für die Spur (Trail)
                // Dies verhindert das Aufblähen der Float-Werte, was den weißen Bildschirm verursacht.
                trail[idx] = clampf(trail[idx], 0.0f, 1.0f); 
            }
        }

        // Bloom: blur trail
        std::vector<float> bloomBuf(W*H*3, 0.0f);
        boxBlur(trail, bloomBuf, BLOOM_RADIUS);

        // Posterize base
        std::vector<float> base = frame;
        for (int i=0;i<W*H*3;++i) base[i] = std::floor(base[i] * POSTERIZE_LEVELS) / float(POSTERIZE_LEVELS);

        // Edges
        std::vector<float> gray(W*H);
        for (int i=0;i<W*H;++i) gray[i] = 0.299f*frame[i*3+0] + 0.587f*frame[i*3+1] + 0.114f*frame[i*3+2];
        std::vector<float> edges;
        sobelEdges(gray, edges);

        // Compose final
        std::vector<float> outBuf(W*H*3, 0.0f);
        for (int i=0;i<W*H;++i) {
            float e = edges[i];
            for (int c=0;c<3;++c) {
                float b = base[i*3 + c];
                float bl = bloomBuf[i*3 + c];
                float tr = trail[i*3 + c];
                float val = b * (1.0f - BLOOM_WEIGHT) + bl * BLOOM_WEIGHT + tr * 0.6f;
                float tintR = 0.7f, tintG = 0.2f, tintB = 0.8f;
                float tint = (c==0? tintR : (c==1? tintG : tintB));
                val = val * 0.85f + tint * 0.15f;
                val += e * EDGE_WEIGHT;
                outBuf[i*3 + c] = clampf(val);
            }
        }

        // Convert to QImage and display
        QImage outImg(W, H, QImage::Format_RGB888);
        for (int y=0;y<H;++y) {
            uchar* scan = outImg.scanLine(y);
            for (int x=0;x<W;++x) {
                int idx = (y*W + x)*3;
                float r = outBuf[idx+0], g = outBuf[idx+1], b = outBuf[idx+2];
                scan[x*3 + 0] = static_cast<uchar>(clampf(r) * 255.0f);
                scan[x*3 + 1] = static_cast<uchar>(clampf(g) * 255.0f);
                scan[x*3 + 2] = static_cast<uchar>(clampf(b) * 255.0f);
            }
        }

        // FIX: Entfernen der manuellen Skalierung zur Behebung des Resize-Bugs
        label->setPixmap(QPixmap::fromImage(outImg));
    }
};

#include "main.moc"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    // choose first available camera
    QList<QCameraInfo> cams = QCameraInfo::availableCameras();
    QCamera* camera = nullptr;
    if (!cams.isEmpty()) {
        camera = new QCamera(cams.first());
    } else {
        qWarning() << "Keine Kamera gefunden! Verwende Standardkamera.";
        camera = new QCamera; // default
    }

    VideoSurface* surface = new VideoSurface;
    GhostWidget* widget = new GhostWidget;
    widget->resize(800, 600);
    widget->show();

    QObject::connect(surface, &VideoSurface::frameAvailable, widget, &GhostWidget::onFrame);

    camera->setViewfinder(surface);
    camera->start();

    int ret = app.exec();

    camera->stop();
    delete camera;
    delete surface;
    delete widget;
    return ret;
}
