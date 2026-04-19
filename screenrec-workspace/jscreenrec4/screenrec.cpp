// screenrec.cpp
// Qt6 screen recorder with ALSA microphone capture and libav encoding/muxing.
// - Webcam preview in top-right (3x larger than previous default).
// - Output video width set to 1024 (height preserves screen aspect ratio).
// - Lower quality: reduced bitrate and 25 fps target to save disk space.
// Build: link Qt6 Widgets/Multimedia, libavformat, libavcodec, libavutil, libswscale, libswresample, libasound.

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QScreen>
#include <QGuiApplication>
#include <QTimer>
#include <QWidget>
#include <QVBoxLayout>
#include <QVideoWidget>
#include <QMediaDevices>
#include <QCamera>
#include <QCameraDevice>
#include <QImage>
#include <QPixmap>
#include <QDateTime>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QDebug>
#include <QMediaCaptureSession>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

#include <alsa/asoundlib.h>
#include <atomic>
#include <vector>
#include <memory>
#include <cstring>
#include <cmath>

#define LOG qDebug() << "[" << QDateTime::currentDateTime().toString("hh:mm:ss") << "]"

// ---------------------------
// ALSA capture thread
// ---------------------------
class AlsaCapture : public QThread {
    Q_OBJECT
public:
    AlsaCapture(const QString &deviceName = "default",
                unsigned int sampleRate = 48000,
                int channels = 2,
                int framesPerPeriod = 1024)
        : m_device(deviceName),
          m_sampleRate(sampleRate),
          m_channels(channels),
          m_framesPerPeriod(framesPerPeriod) {}

    ~AlsaCapture() { stop(); }

    bool startCapture() {
        if (isRunning()) return false;
        m_stop = false;
        start();
        return true;
    }

    void stop() {
        m_stop = true;
        if (isRunning()) wait();
    }

    bool popAudio(std::vector<uint8_t> &out) {
        QMutexLocker lk(&m_mutex);
        if (m_buffer.empty()) return false;
        out.swap(m_buffer.front());
        m_buffer.erase(m_buffer.begin());
        return true;
    }

    int sampleRate() const { return m_sampleRate; }
    int channels() const { return m_channels; }

signals:
    void errorOccurred(const QString &msg);

protected:
    void run() override {
        snd_pcm_t *handle = nullptr;
        snd_pcm_hw_params_t *params = nullptr;
        int rc = snd_pcm_open(&handle, m_device.toUtf8().constData(), SND_PCM_STREAM_CAPTURE, 0);
        if (rc < 0) {
            emit errorOccurred(QString("Unable to open ALSA device %1: %2").arg(m_device).arg(snd_strerror(rc)));
            return;
        }

        snd_pcm_hw_params_alloca(&params);
        snd_pcm_hw_params_any(handle, params);
        snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
        snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
        snd_pcm_hw_params_set_channels(handle, params, m_channels);
        unsigned int rate = m_sampleRate;
        snd_pcm_hw_params_set_rate_near(handle, params, &rate, 0);
        snd_pcm_hw_params_set_period_size_near(handle, params, (snd_pcm_uframes_t*)&m_framesPerPeriod, 0);

        rc = snd_pcm_hw_params(handle, params);
        if (rc < 0) {
            emit errorOccurred(QString("Unable to set ALSA hw params: %1").arg(snd_strerror(rc)));
            snd_pcm_close(handle);
            return;
        }

        int bytesPerFrame = m_channels * 2; // S16_LE
        int bufferSize = m_framesPerPeriod * bytesPerFrame;
        std::vector<uint8_t> buffer(bufferSize);

        while (!m_stop) {
            rc = snd_pcm_readi(handle, buffer.data(), m_framesPerPeriod);
            if (rc == -EPIPE) {
                snd_pcm_prepare(handle);
                continue;
            } else if (rc < 0) {
                emit errorOccurred(QString("ALSA read error: %1").arg(snd_strerror(rc)));
                break;
            }
            {
                QMutexLocker lk(&m_mutex);
                m_buffer.emplace_back(buffer.begin(), buffer.begin() + bufferSize);
                while (m_buffer.size() > 50) m_buffer.erase(m_buffer.begin());
            }
            QThread::msleep(1);
        }

        snd_pcm_close(handle);
    }

private:
    QString m_device;
    unsigned int m_sampleRate;
    int m_channels;
    int m_framesPerPeriod;
    std::atomic<bool> m_stop{false};
    QMutex m_mutex;
    std::vector<std::vector<uint8_t>> m_buffer;
};

// ---------------------------
// Video frame queue
// ---------------------------
struct VideoFrame { QImage image; qint64 pts; };

class FrameQueue {
public:
    void push(VideoFrame &&f) {
        QMutexLocker lk(&m_mutex);
        m_queue.push_back(std::move(f));
        if (m_queue.size() > 200) m_queue.erase(m_queue.begin());
        m_cond.wakeOne();
    }

    bool pop(VideoFrame &out, int timeoutMs = 100) {
        QMutexLocker lk(&m_mutex);
        if (m_queue.empty()) {
            if (!m_cond.wait(&m_mutex, timeoutMs)) return false;
        }
        if (m_queue.empty()) return false;
        out = std::move(m_queue.front());
        m_queue.erase(m_queue.begin());
        return true;
    }

    void clear() { QMutexLocker lk(&m_mutex); m_queue.clear(); }

private:
    QMutex m_mutex;
    QWaitCondition m_cond;
    std::vector<VideoFrame> m_queue;
};

// ---------------------------
// Recorder thread
// ---------------------------
class Recorder : public QThread {
    Q_OBJECT
public:
    Recorder(FrameQueue *videoQueue, AlsaCapture *alsa, int screenW, int screenH, int overlayW)
        : m_videoQueue(videoQueue), m_alsa(alsa), m_screenW(screenW), m_screenH(screenH), m_overlayW(overlayW) {}

    ~Recorder() { stopRecording(); }

    bool startRecording(const QString &outFile) {
        if (isRunning()) return false;
        m_outFile = outFile;
        m_stop = false;
        start();
        return true;
    }

    void stopRecording() {
        m_stop = true;
        if (isRunning()) wait();
    }

signals:
    void errorOccurred(const QString &msg);
    void finishedRecording();

protected:
    void run() override {
        AVFormatContext *fmt_ctx = nullptr;
        if (avformat_alloc_output_context2(&fmt_ctx, nullptr, nullptr, m_outFile.toUtf8().constData()) < 0 || !fmt_ctx) {
            emit errorOccurred("Could not allocate output context");
            return;
        }

        // Video encoder
        const AVCodec *videoCodec = avcodec_find_encoder_by_name("libx264");
        if (!videoCodec) videoCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
        if (!videoCodec) { emit errorOccurred("H.264 encoder not found"); avformat_free_context(fmt_ctx); return; }

        AVStream *video_st = avformat_new_stream(fmt_ctx, nullptr);
        if (!video_st) { emit errorOccurred("Failed to create video stream"); avformat_free_context(fmt_ctx); return; }

        AVCodecContext *video_enc_ctx = avcodec_alloc_context3(videoCodec);

        // Target width = 1024, preserve aspect ratio
        const int targetWidth = 1024;
        int targetHeight = static_cast<int>(std::round((double)targetWidth * m_screenH / (double)m_screenW));
        const int targetFps = 25;

        video_enc_ctx->codec_id = videoCodec->id;
        video_enc_ctx->width = targetWidth;
        video_enc_ctx->height = targetHeight;
        video_enc_ctx->time_base = AVRational{1, targetFps};
        video_st->time_base = video_enc_ctx->time_base;
        video_enc_ctx->pix_fmt = AV_PIX_FMT_YUV420P;

        // Lower quality to save disk: reduce bitrate
        video_enc_ctx->bit_rate = 800 * 1000; // 800 kbps

        // Use faster preset for lower CPU; quality controlled by bitrate above
        if (video_enc_ctx->priv_data) {
            av_opt_set(video_enc_ctx->priv_data, "preset", "veryfast", 0);
            av_opt_set(video_enc_ctx->priv_data, "tune", "zerolatency", 0);
        }

        if (fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) video_enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        if (avcodec_open2(video_enc_ctx, videoCodec, nullptr) < 0) { emit errorOccurred("Could not open video encoder"); avcodec_free_context(&video_enc_ctx); avformat_free_context(fmt_ctx); return; }
        if (avcodec_parameters_from_context(video_st->codecpar, video_enc_ctx) < 0) { emit errorOccurred("Failed to copy video codec parameters"); avcodec_free_context(&video_enc_ctx); avformat_free_context(fmt_ctx); return; }

        // Audio encoder
        const AVCodec *audioCodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
        if (!audioCodec) { emit errorOccurred("AAC encoder not found"); avcodec_free_context(&video_enc_ctx); avformat_free_context(fmt_ctx); return; }

        AVStream *audio_st = avformat_new_stream(fmt_ctx, nullptr);
        if (!audio_st) { emit errorOccurred("Failed to create audio stream"); avcodec_free_context(&video_enc_ctx); avformat_free_context(fmt_ctx); return; }

        AVCodecContext *audio_enc_ctx = avcodec_alloc_context3(audioCodec);
        audio_enc_ctx->sample_fmt = AV_SAMPLE_FMT_FLTP;
        audio_enc_ctx->bit_rate = 96000;
        audio_enc_ctx->sample_rate = m_alsa->sampleRate();
        av_channel_layout_default(&audio_enc_ctx->ch_layout, m_alsa->channels());
        audio_enc_ctx->time_base = AVRational{1, audio_enc_ctx->sample_rate};
        if (fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) audio_enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        if (avcodec_open2(audio_enc_ctx, audioCodec, nullptr) < 0) { emit errorOccurred("Could not open audio encoder"); avcodec_free_context(&audio_enc_ctx); avcodec_free_context(&video_enc_ctx); avformat_free_context(fmt_ctx); return; }
        if (avcodec_parameters_from_context(audio_st->codecpar, audio_enc_ctx) < 0) { emit errorOccurred("Failed to copy audio codec parameters"); avcodec_free_context(&audio_enc_ctx); avcodec_free_context(&video_enc_ctx); avformat_free_context(fmt_ctx); return; }

        // Open output
        if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
            if (avio_open(&fmt_ctx->pb, m_outFile.toUtf8().constData(), AVIO_FLAG_WRITE) < 0) { emit errorOccurred("Could not open output file"); avcodec_free_context(&audio_enc_ctx); avcodec_free_context(&video_enc_ctx); avformat_free_context(fmt_ctx); return; }
        }
        if (avformat_write_header(fmt_ctx, nullptr) < 0) { emit errorOccurred("Error writing header to output file"); avio_closep(&fmt_ctx->pb); avcodec_free_context(&audio_enc_ctx); avcodec_free_context(&video_enc_ctx); avformat_free_context(fmt_ctx); return; }

        // Converters
        SwsContext *sws_ctx = sws_getContext(m_screenW, m_screenH, AV_PIX_FMT_BGRA, video_enc_ctx->width, video_enc_ctx->height, video_enc_ctx->pix_fmt, SWS_BILINEAR, nullptr, nullptr, nullptr);
        if (!sws_ctx) { emit errorOccurred("Failed to create SwsContext"); avio_closep(&fmt_ctx->pb); avcodec_free_context(&audio_enc_ctx); avcodec_free_context(&video_enc_ctx); avformat_free_context(fmt_ctx); return; }

        // Configure resampler using swr_alloc_set_opts2 (modern API)
        SwrContext *swr = nullptr;
        AVChannelLayout in_ch_layout;
        av_channel_layout_default(&in_ch_layout, m_alsa->channels());
        AVChannelLayout out_ch_layout = audio_enc_ctx->ch_layout;
        enum AVSampleFormat in_sample_fmt = AV_SAMPLE_FMT_S16;
        enum AVSampleFormat out_sample_fmt = audio_enc_ctx->sample_fmt;

        // swr_alloc_set_opts2 signature:
        // int swr_alloc_set_opts2(SwrContext **s, const AVChannelLayout *out_ch_layout, enum AVSampleFormat out_sample_fmt, int out_sample_rate, const AVChannelLayout *in_ch_layout, enum AVSampleFormat in_sample_fmt, int in_sample_rate, int log_offset, void *log_ctx);
        int rc = swr_alloc_set_opts2(&swr,
                                     &out_ch_layout, out_sample_fmt, audio_enc_ctx->sample_rate,
                                     &in_ch_layout,  in_sample_fmt,  m_alsa->sampleRate(),
                                     0, nullptr);
        if (rc < 0 || !swr) {
            char errbuf[256]; av_strerror(rc, errbuf, sizeof(errbuf));
            emit errorOccurred(QString("swr_alloc_set_opts2 failed: %1").arg(errbuf));
            sws_freeContext(sws_ctx);
            avio_closep(&fmt_ctx->pb);
            avcodec_free_context(&audio_enc_ctx);
            avcodec_free_context(&video_enc_ctx);
            avformat_free_context(fmt_ctx);
            return;
        }

        rc = swr_init(swr);
        if (rc < 0) {
            char errbuf[256]; av_strerror(rc, errbuf, sizeof(errbuf));
            emit errorOccurred(QString("swr_init failed: %1").arg(errbuf));
            swr_free(&swr);
            sws_freeContext(sws_ctx);
            avio_closep(&fmt_ctx->pb);
            avcodec_free_context(&audio_enc_ctx);
            avcodec_free_context(&video_enc_ctx);
            avformat_free_context(fmt_ctx);
            return;
        }

        // Prepare frames/packets
        AVFrame *vframe = av_frame_alloc();
        vframe->format = video_enc_ctx->pix_fmt;
        vframe->width = video_enc_ctx->width;
        vframe->height = video_enc_ctx->height;
        av_frame_get_buffer(vframe, 32);

        AVPacket *pkt = av_packet_alloc();

        AVFrame *aframe = av_frame_alloc();
        aframe->nb_samples = audio_enc_ctx->frame_size ? audio_enc_ctx->frame_size : 1024;
        aframe->format = audio_enc_ctx->sample_fmt;
        aframe->sample_rate = audio_enc_ctx->sample_rate;
        aframe->ch_layout = audio_enc_ctx->ch_layout;
        if (av_frame_get_buffer(aframe, 0) < 0) { emit errorOccurred("Could not allocate audio frame buffer"); av_packet_free(&pkt); av_frame_free(&vframe); av_frame_free(&aframe); swr_free(&swr); sws_freeContext(sws_ctx); avio_closep(&fmt_ctx->pb); avcodec_free_context(&audio_enc_ctx); avcodec_free_context(&video_enc_ctx); avformat_free_context(fmt_ctx); return; }

        if (!m_alsa->isRunning()) m_alsa->startCapture();

        int64_t video_pts = 0;
        int64_t audio_pts = 0;

        // Main loop
        bool running = true;
        while (!m_stop && running) {
            // Video
            VideoFrame vf;
            bool haveVideo = m_videoQueue->pop(vf, 50);
            if (haveVideo) {
                QImage img = vf.image.convertToFormat(QImage::Format_ARGB32);
                const uint8_t *srcData[4] = { img.bits(), nullptr, nullptr, nullptr };
                int srcLinesize[4] = { static_cast<int>(img.bytesPerLine()), 0, 0, 0 };
                sws_scale(sws_ctx, srcData, srcLinesize, 0, m_screenH, vframe->data, vframe->linesize);
                vframe->pts = video_pts++;
                if (avcodec_send_frame(video_enc_ctx, vframe) == 0) {
                    while (avcodec_receive_packet(video_enc_ctx, pkt) == 0) {
                        pkt->stream_index = video_st->index;
                        av_packet_rescale_ts(pkt, video_enc_ctx->time_base, video_st->time_base);
                        av_interleaved_write_frame(fmt_ctx, pkt);
                        av_packet_unref(pkt);
                    }
                }
            }

            // Audio
            std::vector<uint8_t> audioBuf;
            while (m_alsa->popAudio(audioBuf)) {
                const uint8_t *inData[1] = { audioBuf.data() };
                int inSamples = audioBuf.size() / (m_alsa->channels() * 2);

                int outSamples = av_rescale_rnd(swr_get_delay(swr, m_alsa->sampleRate()) + inSamples,
                                                audio_enc_ctx->sample_rate, m_alsa->sampleRate(), AV_ROUND_UP);

                if (outSamples > aframe->nb_samples) {
                    av_frame_unref(aframe);
                    aframe->nb_samples = outSamples;
                    if (av_frame_get_buffer(aframe, 0) < 0) { emit errorOccurred("Failed to reallocate audio frame buffer"); running = false; break; }
                }

                const uint8_t **in = inData;
                int converted = swr_convert(swr, aframe->data, aframe->nb_samples, in, inSamples);
                if (converted < 0) { char errbuf[256]; av_strerror(converted, errbuf, sizeof(errbuf)); emit errorOccurred(QString("swr_convert failed: %1").arg(errbuf)); running = false; break; }

                aframe->pts = audio_pts;
                audio_pts += aframe->nb_samples;

                if (avcodec_send_frame(audio_enc_ctx, aframe) == 0) {
                    while (avcodec_receive_packet(audio_enc_ctx, pkt) == 0) {
                        pkt->stream_index = audio_st->index;
                        av_packet_rescale_ts(pkt, audio_enc_ctx->time_base, audio_st->time_base);
                        av_interleaved_write_frame(fmt_ctx, pkt);
                        av_packet_unref(pkt);
                    }
                }
            }

            QThread::msleep(5);
        }

        // Flush encoders
        avcodec_send_frame(video_enc_ctx, nullptr);
        while (avcodec_receive_packet(video_enc_ctx, pkt) == 0) {
            pkt->stream_index = video_st->index;
            av_packet_rescale_ts(pkt, video_enc_ctx->time_base, video_st->time_base);
            av_interleaved_write_frame(fmt_ctx, pkt);
            av_packet_unref(pkt);
        }

        avcodec_send_frame(audio_enc_ctx, nullptr);
        while (avcodec_receive_packet(audio_enc_ctx, pkt) == 0) {
            pkt->stream_index = audio_st->index;
            av_packet_rescale_ts(pkt, audio_enc_ctx->time_base, audio_st->time_base);
            av_interleaved_write_frame(fmt_ctx, pkt);
            av_packet_unref(pkt);
        }

        av_write_trailer(fmt_ctx);

        // Cleanup
        av_packet_free(&pkt);
        av_frame_free(&vframe);
        av_frame_free(&aframe);
        sws_freeContext(sws_ctx);
        swr_free(&swr);
        avcodec_free_context(&video_enc_ctx);
        avcodec_free_context(&audio_enc_ctx);
        if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE)) avio_closep(&fmt_ctx->pb);
        avformat_free_context(fmt_ctx);

        emit finishedRecording();
    }

private:
    FrameQueue *m_videoQueue;
    AlsaCapture *m_alsa;
    QString m_outFile;
    int m_screenW;
    int m_screenH;
    int m_overlayW;
    std::atomic<bool> m_stop{false};
};

// ---------------------------
// Webcam preview (top-right, 3x larger)
// ---------------------------
class WebcamPreview : public QWidget {
    Q_OBJECT
public:
    explicit WebcamPreview(int widthPx, QWidget *parent = nullptr)
        : QWidget(parent), m_width(widthPx) {
        setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_NoSystemBackground, false);

        m_videoWidget = new QVideoWidget(this);
        auto layout = new QVBoxLayout(this);
        layout->setContentsMargins(0,0,0,0);
        layout->addWidget(m_videoWidget);

        QList<QCameraDevice> cams = QMediaDevices::videoInputs();
        QCameraDevice chosen;
        for (const auto &d : cams) {
            QString id = d.id();
            if (id.contains("/dev/video0") || id.contains("video0")) { chosen = d; break; }
        }
        if (!chosen.isNull()) m_camera = new QCamera(chosen, this);
        else if (!cams.isEmpty()) m_camera = new QCamera(cams.first(), this);
        else m_camera = nullptr;

        m_session = new QMediaCaptureSession(this);
        if (m_camera) {
            m_session->setCamera(m_camera);
            m_session->setVideoOutput(m_videoWidget);
            m_camera->start();
        }

        adjustSizeAndPosition();
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) connect(screen, &QScreen::geometryChanged, this, [this](const QRect&){ adjustSizeAndPosition(); });
    }

    ~WebcamPreview() { if (m_camera) m_camera->stop(); }

    void adjustSizeAndPosition() {
        QScreen *screen = QGuiApplication::primaryScreen();
        if (!screen) return;
        QSize screenSize = screen->size();
        int w = m_width;
        int h = static_cast<int>(std::round(w * 3.0 / 4.0));
        setFixedSize(w, h);
        int x = screenSize.width() - width() - 10;
        int y = 10;
        move(x, y);
        show();
    }

private:
    QVideoWidget *m_videoWidget = nullptr;
    QCamera *m_camera = nullptr;
    QMediaCaptureSession *m_session = nullptr;
    int m_width;
};

// ---------------------------
// RecorderApp
// ---------------------------
class RecorderApp : public QObject {
    Q_OBJECT
public:
    RecorderApp(QApplication &app) : QObject(nullptr), m_app(app) {
        m_tray = new QSystemTrayIcon(QIcon::fromTheme("camera-video"), this);
        m_menu = new QMenu();
        m_actionRecord = m_menu->addAction("Record");
        m_actionPause = m_menu->addAction("Pause");
        m_actionStop = m_menu->addAction("Stop");
        m_menu->addSeparator();
        m_actionQuit = m_menu->addAction("Quit");

        m_actionPause->setEnabled(false);
        m_actionStop->setEnabled(false);

        connect(m_actionRecord, &QAction::triggered, this, &RecorderApp::startRecording);
        connect(m_actionPause, &QAction::triggered, this, &RecorderApp::togglePause);
        connect(m_actionStop, &QAction::triggered, this, &RecorderApp::stopRecording);
        connect(m_actionQuit, &QAction::triggered, &m_app, &QApplication::quit);

        m_tray->setContextMenu(m_menu);
        m_tray->show();

        QScreen *screen = QGuiApplication::primaryScreen();
        if (!screen) qFatal("No primary screen found");
        QSize screenSize = screen->size();
        m_screenW = screenSize.width();
        m_screenH = screenSize.height();

        // previous overlay width calculation multiplied by 3 to make webcam preview 3x larger
        int baseOverlay = qMax(64, (int)std::round(m_screenW * 0.075));
        m_overlayW = baseOverlay * 3;

        m_preview = new WebcamPreview(m_overlayW);

        m_alsa = new AlsaCapture("default", 48000, 2, 1024);
        connect(m_alsa, &AlsaCapture::errorOccurred, this, &RecorderApp::onError);

        m_recorder = new Recorder(&m_frameQueue, m_alsa, m_screenW, m_screenH, m_overlayW);
        connect(m_recorder, &Recorder::errorOccurred, this, &RecorderApp::onError);
        connect(m_recorder, &Recorder::finishedRecording, this, &RecorderApp::onFinished);

        m_captureTimer = new QTimer(this);
        connect(m_captureTimer, &QTimer::timeout, this, &RecorderApp::captureScreenFrame);
    }

    ~RecorderApp() {
        stopRecording();
        if (m_alsa) { m_alsa->stop(); delete m_alsa; }
        delete m_recorder;
    }

public slots:
    void startRecording() {
        if (m_isRecording) return;
        m_outFile = "out.mp4";
        m_frameQueue.clear();
        if (!m_alsa->isRunning()) m_alsa->startCapture();
        if (!m_recorder->startRecording(m_outFile)) { onError("Failed to start recorder thread"); return; }
        m_captureTimer->start(40); // ~25 fps
        m_isRecording = true;
        m_actionRecord->setEnabled(false);
        m_actionPause->setEnabled(true);
        m_actionStop->setEnabled(true);
        m_tray->showMessage("Screen Recorder", "Recording started");
    }

    void togglePause() {
        if (!m_isRecording) return;
        if (!m_isPaused) { m_captureTimer->stop(); m_isPaused = true; m_actionPause->setText("Resume"); m_tray->showMessage("Screen Recorder", "Paused"); }
        else { m_captureTimer->start(40); m_isPaused = false; m_actionPause->setText("Pause"); m_tray->showMessage("Screen Recorder", "Resumed"); }
    }

    void stopRecording() {
        if (!m_isRecording) return;
        m_captureTimer->stop();
        m_recorder->stopRecording();
        m_alsa->stop();
        m_isRecording = false;
        m_actionRecord->setEnabled(true);
        m_actionPause->setEnabled(false);
        m_actionStop->setEnabled(false);
        m_actionPause->setText("Pause");
        m_tray->showMessage("Screen Recorder", "Stopped");
    }

private slots:
    void captureScreenFrame() {
        QScreen *screen = QGuiApplication::primaryScreen();
        if (!screen) return;
        QPixmap pix = screen->grabWindow(0);
        QImage img = pix.toImage();
        VideoFrame vf; vf.image = img; vf.pts = QDateTime::currentMSecsSinceEpoch();
        m_frameQueue.push(std::move(vf));
    }

    void onError(const QString &msg) { LOG << "Error:" << msg; m_tray->showMessage("Screen Recorder Error", msg); }
    void onFinished() { LOG << "Recording finished"; m_tray->showMessage("Screen Recorder", "Recording finished"); }

private:
    QApplication &m_app;
    QSystemTrayIcon *m_tray = nullptr;
    QMenu *m_menu = nullptr;
    QAction *m_actionRecord = nullptr;
    QAction *m_actionPause = nullptr;
    QAction *m_actionStop = nullptr;
    QAction *m_actionQuit = nullptr;

    WebcamPreview *m_preview = nullptr;
    AlsaCapture *m_alsa = nullptr;
    Recorder *m_recorder = nullptr;
    FrameQueue m_frameQueue;
    QTimer *m_captureTimer = nullptr;

    int m_screenW = 1280;
    int m_screenH = 720;
    int m_overlayW = 160;
    bool m_isRecording = false;
    bool m_isPaused = false;
    QString m_outFile;
};

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    avformat_network_init();
    RecorderApp recorder(app);
    return app.exec();
}

#include "screenrec.moc"

