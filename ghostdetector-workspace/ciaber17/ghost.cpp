// ghost.cpp
#define _GNU_SOURCE
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <ctime>
#include <vector>
#include <atomic>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>

#include <linux/videodev2.h>

// ALSA
#include <alsa/asoundlib.h>

// Qt5
#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QElapsedTimer>
#include <QTimer>
#include <QFont>
#include <QDebug>

// ----------------- Konfiguration -----------------
#define DEVICE_NODE "/dev/video0"
#define IMG_W 640
#define IMG_H 480

#define SAMPLE_RATE 44100
#define CLICK_DURATION 0.05
#define CLICK_SAMPLES (int)(SAMPLE_RATE * CLICK_DURATION)
#define NUM_SCALE_NOTES 16
#define BASE_FREQ_A4 440.0

#define BG_LEARN_RATE 0.01f
#define RESIDUE_THRESH 15
#define FLICKER_THRESH 20
#define EDGE_GAIN 1.6f
#define HEATMAP_ALPHA 155
#define RADIONULLON_THRESH 30
#define HOT_PIXEL_THRESH 200

#define GEIGER_SMOOTH_FACTOR 0.3
#define GEIGER_ALERT_THRESH 5.0

#define FUSION_WEIGHT_VIDEO 0.97
#define FUSION_WEIGHT_AUDIO 0.03

// ----------------- Physikalische Konstanten (klar benannt) -----------------
static const double SV_TO_USV = 1e6;        // 1 Sv = 1e6 µSv
static const double SEC_TO_HOUR = 1.0 / 3600.0; // Sekunden -> Stunden
// Heuristischer Faktor: Sv pro Zerfall (muss ggf. kalibriert werden)
static const double DECAY_TO_SV = 1e-8; // Sv per decay (placeholder, dokumentieren / anpassen)

// ----------------- Isotope (gekürzt, gleiche Struktur) -----------------
typedef struct {
    const char *name;
    double T_HALF_MS;
    double ENERGY_MEV;
    double RBE_FACTOR;
    double ACTIVITY_CPS; // counts per second baseline activity (used as absolute rate)
} Isotope;

static const Isotope ISOTOPES[16] = {
    {"Pu-239 (Plutonium)", 7.59e+15, 5.15, 20.0, 0.001},
    {"Am-241 (Americium)", 1.38e+13, 5.48, 20.0, 0.005},
    {"Rn-222 (Radon)", 3.29e+5, 5.49, 20.0, 0.005},
    {"Ra-226 (Radium)", 5.06e+10, 4.78, 20.0, 0.008},
    {"Cs-137 (Caesium)", 9.48e+11, 0.662, 1.0, 0.2},
    {"Co-60 (Kobalt)", 1.66e+11, 1.25, 1.0, 0.1},
    {"I-131 (Iod)", 6.95e+8, 0.364, 1.0, 0.05},
    {"Sr-90 (Strontium)", 8.78e+11, 0.546, 1.0, 0.15},
    {"U-238 (Uran)", 1.41e+17, 4.20, 20.0, 0.0001},
    {"Tc-99 (Technetium)", 1.33e+14, 0.294, 1.0, 0.002},
    {"C-14 (Kohlenstoff)", 1.794e+14, 0.0495, 1.0, 5.0e-4},
    {"K-40 (Kalium)", 4.053e+19, 1.460, 1.0, 2.0e-2},
    {"Kr-85 (Krypton)", 3.32e+10, 0.514, 1.0, 0.01},
    {"Xe-133 (Xenon)", 4.54e+7, 0.081, 1.0, 0.05},
    {"Au-198 (Gold)", 2.30e+5, 0.96, 1.0, 0.08},
    {"H-3 (Tritium)", 3.892e+11, 0.0057, 1.0, 1.0e-3}
};

// ----------------- Hilfsfunktionen -----------------
static inline uint8_t rgb32_to_grayscale(uint32_t rgb) {
    uint8_t r = (uint8_t)((rgb >> 16) & 0xFF);
    uint8_t g = (uint8_t)((rgb >> 8) & 0xFF);
    uint8_t b = (uint8_t)(rgb & 0xFF);
    return (uint8_t)((299 * r + 587 * g + 114 * b) / 1000);
}
void hsv_to_rgb(float H, float S, float V, float *R, float *G, float *B) {
    if (S == 0.0f) { *R = *G = *B = V; return; }
    float h = H / 60.0f; int i = (int)floorf(h); float f = h - i;
    float p = V * (1.0f - S); float q = V * (1.0f - S * f);
    float t = V * (1.0f - S * (1.0f - f));
    switch (i % 6) {
        case 0: *R = V; *G = t; *B = p; break; case 1: *R = q; *G = V; *B = p; break;
        case 2: *R = p; *G = V; *B = t; break; case 3: *R = p; *G = q; *B = V; break;
        case 4: *R = t; *G = p; *B = V; break; case 5: *R = V; *G = p; *B = q; break;
    }
}

// ----------------- Geiger State -----------------
struct GeigerStats {
    double currentRad;
    double avgRad;
    double totalDoseSv;
    double currentAudioUsvS;
    double currentFusedUsvS;
    int alertState;
    std::vector<double> shortHistory;
    std::vector<double> avgHistory;
    std::vector<double> longHistory;
    long long startTimeSec;
    GeigerStats(): currentRad(0), avgRad(0), totalDoseSv(0), currentAudioUsvS(0),
        currentFusedUsvS(0), alertState(0), shortHistory(100), avgHistory(100), longHistory(100), startTimeSec(0) {}
};

struct GeigerLogicState {
    std::vector<float> bg;
    std::vector<uint8_t> flicker;
    float prevGlobalAvg;
    double currentRadSmooth;
    double totalDoseAcc;
    int longTermCounter;
    long long isotope_count[16];
    double isotope_total_dose_usvh[16];
    double total_usv_h;
    GeigerStats stats;
    GeigerLogicState(): bg(IMG_W*IMG_H), flicker(IMG_W*IMG_H), prevGlobalAvg(128.0f),
        currentRadSmooth(0), totalDoseAcc(0), longTermCounter(0), total_usv_h(0) {
        memset(isotope_count,0,sizeof(isotope_count));
        memset(isotope_total_dose_usvh,0,sizeof(isotope_total_dose_usvh));
        struct timeval tv; gettimeofday(&tv, NULL); stats.startTimeSec = tv.tv_sec;
    }
};

// ----------------- V4L2 Capture Thread -----------------
class CaptureThread : public QThread {
    Q_OBJECT
public:
    CaptureThread(QObject *parent = nullptr): QThread(parent), running(true) {}
    ~CaptureThread() { stop(); }

    void stop() {
        running = false;
        requestInterruption();
        wait(2000);
    }

signals:
    void frameReady(const QImage &img);

protected:
    void run() override {
        if (v4l2_init() != 0) return;
        while (!isInterruptionRequested() && running) {
            uint32_t rgbbuf[IMG_W * IMG_H];
            int r = v4l2_read_frame(rgbbuf);
            if (r == 0) {
                QImage img((uchar*)rgbbuf, IMG_W, IMG_H, QImage::Format_RGB32);
                emit frameReady(img.copy());
            } else {
                msleep(10);
            }
        }
        v4l2_close();
    }

private:
    bool running;

    struct Buffer { void *start; size_t length; };
    Buffer *v4l2_buffers = nullptr;
    unsigned int v4l2_n_buffers = 0;
    int v4l2_fd = -1;

    static int xioctl(int fh, int request, void *arg) {
        int r;
        do { r = ioctl(fh, request, arg); } while (-1 == r && EINTR == errno);
        return r;
    }

    int v4l2_init() {
        struct v4l2_format fmt; struct v4l2_requestbuffers req;
        v4l2_fd = open(DEVICE_NODE, O_RDWR | O_NONBLOCK, 0);
        if (v4l2_fd == -1) { qWarning("V4L2: open failed"); return -1; }
        memset(&fmt,0,sizeof(fmt)); fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width = IMG_W; fmt.fmt.pix.height = IMG_H; fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        if (xioctl(v4l2_fd, VIDIOC_S_FMT, &fmt) == -1) { qWarning("VIDIOC_S_FMT failed"); return -1; }
        memset(&req,0,sizeof(req)); req.count = 2; req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; req.memory = V4L2_MEMORY_MMAP;
        if (xioctl(v4l2_fd, VIDIOC_REQBUFS, &req) == -1) { qWarning("VIDIOC_REQBUFS failed"); return -1; }
        v4l2_buffers = (Buffer*)calloc(req.count, sizeof(*v4l2_buffers));
        for (v4l2_n_buffers = 0; v4l2_n_buffers < req.count; ++v4l2_n_buffers) {
            struct v4l2_buffer buf; memset(&buf,0,sizeof(buf)); buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = v4l2_n_buffers;
            if (xioctl(v4l2_fd, VIDIOC_QUERYBUF, &buf) == -1) { qWarning("VIDIOC_QUERYBUF failed"); return -1; }
            v4l2_buffers[v4l2_n_buffers].length = buf.length;
            v4l2_buffers[v4l2_n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, v4l2_fd, buf.m.offset);
            if (MAP_FAILED == v4l2_buffers[v4l2_n_buffers].start) { qWarning("mmap failed"); return -1; }
        }
        for (unsigned int i = 0; i < v4l2_n_buffers; ++i) {
            struct v4l2_buffer buf; memset(&buf,0,sizeof(buf)); buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
            if (xioctl(v4l2_fd, VIDIOC_QBUF, &buf) == -1) { qWarning("VIDIOC_QBUF failed"); return -1; }
        }
        enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (xioctl(v4l2_fd, VIDIOC_STREAMON, &type) == -1) { qWarning("VIDIOC_STREAMON failed"); return -1; }
        return 0;
    }

    int v4l2_read_frame(uint32_t *rgb32_buffer) {
        struct v4l2_buffer buf; memset(&buf,0,sizeof(buf)); buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP;
        if (xioctl(v4l2_fd, VIDIOC_DQBUF, &buf) == -1) {
            if (errno == EAGAIN) return 1;
            qWarning("VIDIOC_DQBUF failed");
            return -1;
        }
        uint8_t *src = (uint8_t *)v4l2_buffers[buf.index].start;
        yuyv_to_rgb32(src, rgb32_buffer, IMG_W, IMG_H);
        if (xioctl(v4l2_fd, VIDIOC_QBUF, &buf) == -1) { qWarning("VIDIOC_QBUF failed"); return -1; }
        return 0;
    }

    void v4l2_close() {
        if (v4l2_fd != -1) {
            enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            xioctl(v4l2_fd, VIDIOC_STREAMOFF, &type);
        }
        if (v4l2_buffers) {
            for (unsigned int i = 0; i < v4l2_n_buffers; ++i) {
                if (v4l2_buffers[i].start) munmap(v4l2_buffers[i].start, v4l2_buffers[i].length);
            }
            free(v4l2_buffers); v4l2_buffers = nullptr;
        }
        if (v4l2_fd != -1) close(v4l2_fd);
        v4l2_fd = -1;
    }

    void yuyv_to_rgb32(const uint8_t *src, uint32_t *dst, int width, int height) {
        for (int y = 0; y < height; y++) {
            const uint8_t *srcLine = src + (y * width * 2);
            uint32_t *dstLine = dst + (y * width);
            for (int x = 0; x < width; x += 2) {
                int Y1 = srcLine[0]; int U = srcLine[1]; int Y2 = srcLine[2]; int V = srcLine[3]; srcLine += 4;
                int C = Y1 - 16; int D = U - 128; int E = V - 128;
                int R = (298 * C + 409 * E + 128) >> 8; int G = (298 * C - 100 * D - 208 * E + 128) >> 8; int B = (298 * C + 516 * D + 128) >> 8;
                dstLine[0] = (uint32_t)(((R>255?255:(R<0?0:R)) & 0xFF) << 16 | ((G>255?255:(G<0?0:G)) & 0xFF) << 8 | ((B>255?255:(B<0?0:B)) & 0xFF));
                C = Y2 - 16;
                R = (298 * C + 409 * E + 128) >> 8; G = (298 * C - 100 * D - 208 * E + 128) >> 8; B = (298 * C + 516 * D + 128) >> 8;
                dstLine[1] = (uint32_t)(((R>255?255:(R<0?0:R)) & 0xFF) << 16 | ((G>255?255:(G<0?0:G)) & 0xFF) << 8 | ((B>255?255:(B<0?0:B)) & 0xFF));
                dstLine += 2;
            }
        }
    }
};

// ----------------- ALSA Audio (libasound) -----------------
static short click_buffers[NUM_SCALE_NOTES][CLICK_SAMPLES];
static snd_pcm_t *global_pcm_handle = nullptr;

void generate_fm_click(double duration, short* buffer, double amplitude, double carrier_freq, double decay_rate) {
    int samples = (int)(duration * SAMPLE_RATE);
    double fm_ratio = 1.414;
    double modulator_freq = carrier_freq * fm_ratio;
    double modulation_index = 10.0 / pow(carrier_freq / BASE_FREQ_A4, 0.2);
    if (modulation_index < 0.5) modulation_index = 0.5;
    for(int i = 0; i < samples; i++) {
        double t = (double)i / SAMPLE_RATE;
        double decay = exp(-t * decay_rate);
        double phase_modulation = modulation_index * sin(2.0 * M_PI * modulator_freq * t);
        double signal = sin(2.0 * M_PI * carrier_freq * t + phase_modulation);
        buffer[i] = (short)(amplitude * signal * decay);
    }
}
void init_click_buffers() {
    srand((unsigned)time(NULL));
    const int SEMITONES[NUM_SCALE_NOTES] = {0,2,3,5,7,8,10,12,14,15,17,19,20,22,24,25};
    for (int i = 0; i < NUM_SCALE_NOTES; i++) {
        double carrier_freq = BASE_FREQ_A4 * pow(2.0, (double)SEMITONES[i] / 12.0);
        double amplitude = 30000.0 - 500.0 * (i % 7);
        generate_fm_click(CLICK_DURATION, click_buffers[i], amplitude, carrier_freq, 1200.0);
    }
}

int init_alsa() {
    int err;
    snd_pcm_hw_params_t *params;
    unsigned int rate = SAMPLE_RATE;
    snd_pcm_uframes_t frames = 512;

    if ((err = snd_pcm_open(&global_pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        qWarning("ALSA: snd_pcm_open error: %s", snd_strerror(err));
        global_pcm_handle = nullptr;
        return -1;
    }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(global_pcm_handle, params);
    snd_pcm_hw_params_set_access(global_pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(global_pcm_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(global_pcm_handle, params, 1);
    snd_pcm_hw_params_set_rate_near(global_pcm_handle, params, &rate, 0);
    snd_pcm_hw_params_set_period_size_near(global_pcm_handle, params, &frames, 0);

    if ((err = snd_pcm_hw_params(global_pcm_handle, params)) < 0) {
        qWarning("ALSA: snd_pcm_hw_params error: %s", snd_strerror(err));
        snd_pcm_close(global_pcm_handle);
        global_pcm_handle = nullptr;
        return -1;
    }

    snd_pcm_prepare(global_pcm_handle);
    return 0;
}

void alsa_cleanup() {
    if (global_pcm_handle) {
        snd_pcm_drain(global_pcm_handle);
        snd_pcm_close(global_pcm_handle);
        global_pcm_handle = nullptr;
    }
}

void play_alsa_click(int click_index) {
    if (!global_pcm_handle || click_index < 0 || click_index >= NUM_SCALE_NOTES) return;
    int samples = CLICK_SAMPLES;
    short *buf = click_buffers[click_index];
    int frames_written = 0;
    while (frames_written < samples) {
        snd_pcm_sframes_t r = snd_pcm_writei(global_pcm_handle, buf + frames_written, samples - frames_written);
        if (r == -EPIPE) {
            // underrun
            snd_pcm_prepare(global_pcm_handle);
            continue;
        } else if (r < 0) {
            qWarning("ALSA write error: %s", snd_strerror((int)r));
            snd_pcm_prepare(global_pcm_handle);
            break;
        } else {
            frames_written += (int)r;
        }
    }
}

void play_startup_clicks() {
    if (!global_pcm_handle) return;
    for (int i = 0; i < NUM_SCALE_NOTES; ++i) {
        play_alsa_click(i);
        usleep(5000);
    }
}

// ----------------- Processing (portiert, Option 1: ACTIVITY_CPS as absolute rate) -----------------
double poisson_decay_simulation_unused(int event_count, long long *isotope_count) {
    // kept for reference but not used in the new model
    double total_activity = 0.0;
    for (int i = 0; i < 16; ++i) total_activity += ISOTOPES[i].ACTIVITY_CPS;
    if (total_activity == 0.0) return 0.0;
    for (int i = 0; i < 16; ++i) {
        double relative_activity = ISOTOPES[i].ACTIVITY_CPS / total_activity;
        int decays = (int)round(event_count * relative_activity);
        if (decays > 0) { decays += (rand() % 3) - 1; if (decays < 0) decays = 0; }
        isotope_count[i] += decays;
    }
    return total_activity;
}

double update_isotopic_dose(GeigerLogicState *state, double frame_delta_sec) {
    double total_usv_h = 0.0;
    for (int i = 0; i < 16; ++i) {
        // current_cps is derived from counts observed in this frame
        double current_cps = (frame_delta_sec > 0.0) ? (double)state->isotope_count[i] / frame_delta_sec : 0.0;
        // Sv per second from decays: decays * DECAY_TO_SV => Sv/s if decays is per second
        double sv_per_s = current_cps * DECAY_TO_SV * ISOTOPES[i].RBE_FACTOR;
        // convert Sv/s to µSv/h: Sv/s * 1e6 (µSv/Sv) * 3600 (s->h)
        double usv_per_h = sv_per_s * SV_TO_USV / SEC_TO_HOUR;
        state->isotope_total_dose_usvh[i] = usv_per_h;
        total_usv_h += usv_per_h;
        // reset per-frame counts (we used them to compute current rate)
        state->isotope_count[i] = 0;
    }
    state->total_usv_h = total_usv_h;
    double frame_time_h = frame_delta_sec / 3600.0;
    state->stats.totalDoseSv += (total_usv_h / SV_TO_USV) * frame_time_h;
    return total_usv_h;
}

void process_radionullon(GeigerLogicState *state, const QImage &img_in, QImage &img_out, double frame_delta_sec) {
    int size = IMG_W * IMG_H;
    if (img_in.format() != QImage::Format_RGB32) return;
    img_out = img_in.copy();

    std::vector<uint8_t> gray(size);
    std::vector<uint8_t> residue(size);
    std::vector<uint8_t> edge(size);
    memset(edge.data(), 0, size);

    int hotPixels = 0;
    float globalAvg = 0.0f;

    const uint32_t *inPixels = reinterpret_cast<const uint32_t*>(img_in.constBits());
    uint32_t *outPixels = reinterpret_cast<uint32_t*>(img_out.bits());

    for (int i = 0; i < size; ++i) {
        uint8_t g = rgb32_to_grayscale(inPixels[i]);
        gray[i] = g;
        if (g > HOT_PIXEL_THRESH) hotPixels++;
        state->bg[i] = (1.0f - BG_LEARN_RATE) * state->bg[i] + BG_LEARN_RATE * (float)gray[i];
        int diff = (int)fabsf((float)gray[i] - state->bg[i]);
        const int RESIDUE_THRESH_SQR = RESIDUE_THRESH * RESIDUE_THRESH;
        residue[i] = (uint8_t)((diff * diff > RESIDUE_THRESH_SQR) ? (diff > 255 ? 255 : diff) : 0);
        globalAvg += (float)gray[i];
    }
    globalAvg /= size;
    uint8_t currentAvg = (uint8_t)globalAvg;
    int delta = (int)fabsf((float)currentAvg - state->prevGlobalAvg);
    state->prevGlobalAvg = currentAvg;
    uint8_t boost = (delta > FLICKER_THRESH) ? 20 : 1;
    for (int i = 0; i < size; i++) {
        state->flicker[i] = (uint8_t)(((int)state->flicker[i] + boost) > 255 ? 255 : (state->flicker[i] + boost));
        if (state->flicker[i] > 0) state->flicker[i] = (uint8_t)(state->flicker[i] - 1);
    }

    for (int y = 1; y < IMG_H - 1; y++) {
        for (int x = 1; x < IMG_W - 1; x++) {
            int i = y * IMG_W + x;
            int gx = -gray[i - IMG_W - 1] - 2 * gray[i - 1] - gray[i + IMG_W - 1] + gray[i - IMG_W + 1] + 2 * gray[i + 1] + gray[i + IMG_W + 1];
            int gy = -gray[i - IMG_W - 1] - 2 * gray[i - IMG_W] - gray[i - IMG_W + 1] + gray[i + IMG_W - 1] + 2 * gray[i + IMG_W] + gray[i + IMG_W + 1];
            int mag = (int)(EDGE_GAIN * sqrtf((float)(gx * gx + gy * gy)));
            edge[i] = (uint8_t)(mag > 255 ? 255 : mag);
        }
    }

    for (int i = 0; i < size; ++i) {
        int score = (int)residue[i] + (int)(edge[i] / 2) + (int)(state->flicker[i] / 3);
        if (score < RADIONULLON_THRESH) {
            uint8_t g = gray[i];
            outPixels[i] = (uint32_t)(g << 16 | g << 8 | g);
            continue;
        }
        int s_clamped = (score > 255) ? 255 : score;
        float H_norm = 1.0f - ((float)s_clamped / 255.0f);
        float Rf, Gf, Bf;
        hsv_to_rgb(H_norm * 240.0f, 1.0f, 1.0f, &Rf, &Gf, &Bf);
        uint8_t R_blend = (uint8_t)((((inPixels[i] >> 16) & 0xFF) * (255 - HEATMAP_ALPHA) + (uint8_t)(Rf * 255.0f) * HEATMAP_ALPHA) / 255);
        uint8_t G_blend = (uint8_t)((((inPixels[i] >> 8) & 0xFF) * (255 - HEATMAP_ALPHA) + (uint8_t)(Gf * 255.0f) * HEATMAP_ALPHA) / 255);
        uint8_t B_blend = (uint8_t)(((inPixels[i] & 0xFF) * (255 - HEATMAP_ALPHA) + (uint8_t)(Bf * 255.0f) * HEATMAP_ALPHA) / 255);
        outPixels[i] = (uint32_t)(R_blend << 16 | G_blend << 8 | B_blend);
    }

    // --- NEW: Use ACTIVITY_CPS as absolute expected decays per frame
    // Add expected decays from each isotope's ACTIVITY_CPS
    for (int i = 0; i < 16; ++i) {
        double expected = ISOTOPES[i].ACTIVITY_CPS * frame_delta_sec; // expected decays in this frame
        int decays = (int)round(expected + ((rand() % 3) - 1)); // small stochastic term
        if (decays < 0) decays = 0;
        state->isotope_count[i] += decays;
    }

    // Compute dose from isotope_count and reset counts per frame
    double total_usv_h_new = update_isotopic_dose(state, frame_delta_sec);
    double video_usv_s = total_usv_h_new / 3600.0;
    double rawVal = total_usv_h_new / (SV_TO_USV / SEC_TO_HOUR); // not used directly but kept for smoothing

    // EMA smoothing
    state->currentRadSmooth = (state->currentRadSmooth * (1.0 - GEIGER_SMOOTH_FACTOR)) + (rawVal * GEIGER_SMOOTH_FACTOR);

    // Fusion (audio currently static)
    double audio_usv_s = 0.0;
    double fused_usv_s = (video_usv_s * FUSION_WEIGHT_VIDEO) + (audio_usv_s * FUSION_WEIGHT_AUDIO);

    state->stats.currentAudioUsvS = audio_usv_s;
    state->stats.currentFusedUsvS = fused_usv_s;
    state->stats.currentRad = fused_usv_s * (SV_TO_USV / 3600.0);
    state->stats.avgRad = state->currentRadSmooth;
    state->stats.alertState = state->stats.currentRad > GEIGER_ALERT_THRESH;
}

// ----------------- Main Widget (nur gefilterter Videooutput, Overlays mit 50% Transparenz) -----------------
class MainWidget : public QWidget {
    Q_OBJECT
public:
    MainWidget(QWidget *parent = nullptr): QWidget(parent), audioThread(nullptr) {
        setWindowTitle("ghost (Qt5) | Radionullon Fusion (ALSA)");
        // Make window resizable; processed video will fill entire client area
        resize(IMG_W * 2, IMG_H * 2 / 1); // initial size; user can resize
        init_click_buffers();
        state = new GeigerLogicState();
        capture = new CaptureThread(this);
        connect(capture, &CaptureThread::frameReady, this, &MainWidget::onFrame);
        capture->start();

        // ALSA init
        if (init_alsa() == 0) {
            audioThread = QThread::create([=]() {
                play_startup_clicks();
            });
            audioThread->start();
        } else {
            qWarning("ALSA init failed; no audio.");
        }

        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &MainWidget::onTimeout);
        timer->start(33);
        elapsed.start();
    }

    ~MainWidget() {
        capture->stop();
        if (audioThread) {
            audioThread->quit();
            audioThread->wait();
            delete audioThread;
            audioThread = nullptr;
        }
        alsa_cleanup();
        delete state;
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::SmoothPixmapTransform, true);

        // Draw processed video to fill entire widget (maintain aspect by scaling)
        if (!processed.isNull()) {
            QImage scaled = processed.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            // center crop if necessary
            QRect targetRect(0, 0, width(), height());
            p.drawImage(targetRect, scaled);
        } else {
            p.fillRect(rect(), Qt::black);
        }

        // Semi-transparent overlay background for text (50% alpha)
        QColor bgColor(0, 0, 0, 128); // 50% transparent black
        int overlayW = width();
        int overlayH = 110;
        QRect overlayRect(0, height() - overlayH, overlayW, overlayH);
        p.fillRect(overlayRect, bgColor);

        // Draw text (white) on top of semi-transparent background
        p.setPen(Qt::white);
        QFont f("Monospace", 12);
        p.setFont(f);
        int x = 12;
        int y = height() - overlayH + 24;
        char buf[256];
        sprintf(buf, "FUSION (97%%/3%%): %.6f µSv/s (%.3f µSv/h)", state->stats.currentFusedUsvS, state->stats.currentFusedUsvS * 3600.0);
        p.drawText(x, y, buf); y += 22;
        sprintf(buf, "VIDEO (Roh): %.6f µSv/s | AUDIO: %.6f µSv/s", state->total_usv_h/3600.0, state->stats.currentAudioUsvS);
        p.drawText(x, y, buf); y += 22;
        sprintf(buf, "TOTALDOSIS (Akku): %.6f µSv", state->stats.totalDoseSv * SV_TO_USV);
        p.drawText(x, y, buf);

        // Right-side isotope list with semi-transparent background (50% alpha)
        int isoW = 320;
        QRect isoRect(width() - isoW, 0, isoW, height());
        QColor isoBg(0, 0, 0, 128);
        p.fillRect(isoRect, isoBg);

        p.setPen(Qt::white);
        int isoX = width() - isoW + 8;
        int isoY = 20;
        p.drawText(isoX, isoY, "Top 16 Isotope (µSv/h):"); isoY += 20;
        for (int i=0;i<16;i++) {
            QColor col = (state->isotope_total_dose_usvh[i] > 1.0) ? Qt::red : Qt::lightGray;
            p.setPen(col);
            sprintf(buf, "%-20s: %.6f", ISOTOPES[i].name, state->isotope_total_dose_usvh[i]);
            p.drawText(isoX, isoY, buf); isoY += 16;
            if (isoY > height() - 20) break;
        }
    }

    void resizeEvent(QResizeEvent *ev) override {
        QWidget::resizeEvent(ev);
        update();
    }

private slots:
    void onFrame(const QImage &img) {
        QMutexLocker lk(&m);
        // Only show filtered (processed) video in the entire window
        double dt = elapsed.elapsed() / 1000.0;
        if (dt <= 0.0) dt = 1.0/30.0;
        process_radionullon(state, img, processed, dt);
        elapsed.restart();
        update();
    }
    void onTimeout() {
        // periodic tasks if needed
    }

private:
    CaptureThread *capture;
    QImage processed;
    QMutex m;
    GeigerLogicState *state;
    QTimer *timer;
    QElapsedTimer elapsed;
    QThread *audioThread;
};

// ----------------- main -----------------
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWidget w;
    w.show();
    return a.exec();
}

#include "ghost.moc"
