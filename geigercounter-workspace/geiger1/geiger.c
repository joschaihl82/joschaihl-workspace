/*
 geiger.c
 Einfacher "Geiger‑Zähler" (audio+video triggers -> klick sound)
 Plain C, pthreads, ALSA capture/playback, V4L2 capture.
 Compile: gcc -O2 -pthread -lasound -o geiger geiger.c
 Run: ./geiger
 Note: This does NOT detect radiation. It produces clicks when audio spikes or bright pixels occur.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <alsa/asoundlib.h>
#include <signal.h>
#include <stdint.h>
#include <time.h>

static volatile int running = 1;
static void sigint(int s){ (void)s; running = 0; fprintf(stderr,"\nSIGINT -> stopping\n"); }

// ---- Config ----
#define SAMPLE_RATE 48000            // playback/capture rate
#define AUDIO_CH 1
#define AUDIO_FRAMES 256
#define PLAYBACK_DEVICE "default"
#define CAPTURE_DEVICE "default"
#define VIDEO_DEVICE "/dev/video0"
#define CLICK_DURATION_MS 60         // click length
#define CLICK_FREQ 2000.0f           // click tone freq
#define CLICK_DECAY 0.003f           // decay time constant (seconds)

// detection thresholds (tune these)
#define AUDIO_PEAK_THRESH 0.20f      // relative to full scale (float)
#define AUDIO_RMS_THRESH 0.02f
#define VIDEO_BRIGHT_PIXEL_THRESH 200 // brightness 0..255
#define VIDEO_BRIGHT_COUNT_THRESH 50  // number of bright pixels to trigger

// ---- Shared event queue (simple fixed ring) ----
#define EVT_Q_LEN 256
typedef struct { int head, tail, count; pthread_mutex_t m; pthread_cond_t c; } evtq_t;
static evtq_t evtq;

static void evtq_init(evtq_t *q){ memset(q,0,sizeof(*q)); pthread_mutex_init(&q->m,NULL); pthread_cond_init(&q->c,NULL); }
static void evtq_push(evtq_t *q){
    pthread_mutex_lock(&q->m);
    if(q->count < EVT_Q_LEN){
        q->tail = (q->tail + 1) % EVT_Q_LEN;
        q->count++;
        pthread_cond_signal(&q->c);
    }
    pthread_mutex_unlock(&q->m);
}
static int evtq_pop(evtq_t *q){
    pthread_mutex_lock(&q->m);
    while(q->count == 0 && running) pthread_cond_wait(&q->c, &q->m);
    if(q->count == 0 && !running){ pthread_mutex_unlock(&q->m); return 0; }
    q->head = (q->head + 1) % EVT_Q_LEN;
    q->count--;
    pthread_mutex_unlock(&q->m);
    return 1;
}

// ---- Click generator (synth) ----
static void fill_click(float *buf, int frames, int sr){
    // simple sine burst with exponential decay
    float A = 1.0f;
    float env_tc = CLICK_DECAY; // seconds
    for(int i=0;i<frames;i++){
        float t = (float)i / sr;
        float env = A * expf(-t / env_tc);
        float s = env * sinf(2.0f * M_PI * CLICK_FREQ * t);
        buf[i] = s;
    }
}

// ---- ALSA playback (thread) ----
typedef struct { snd_pcm_t *handle; } play_t;
static play_t player;

static int playback_init(play_t *p){
    int err;
    snd_pcm_t *h;
    snd_pcm_hw_params_t *params;
    unsigned int rate = SAMPLE_RATE;
    if((err = snd_pcm_open(&h, PLAYBACK_DEVICE, SND_PCM_STREAM_PLAYBACK, 0)) < 0){
        fprintf(stderr,"ALSA open playback error: %s\n", snd_strerror(err)); return -1;
    }
    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(h, params);
    snd_pcm_hw_params_set_access(h, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(h, params, SND_PCM_FORMAT_FLOAT_LE);
    snd_pcm_hw_params_set_rate_near(h, params, &rate, 0);
    snd_pcm_hw_params_set_channels(h, params, AUDIO_CH);
    snd_pcm_hw_params_set_period_size(h, params, AUDIO_FRAMES, 0);
    if((err = snd_pcm_hw_params(h, params)) < 0){ fprintf(stderr,"ALSA hw params error: %s\n", snd_strerror(err)); snd_pcm_hw_params_free(params); snd_pcm_close(h); return -1;}
    snd_pcm_hw_params_free(params);
    p->handle = h;
    return 0;
}

// play one click (blocking): generate envelope and write to ALSA
static void playback_play_click(play_t *p){
    int frames = (CLICK_DURATION_MS * SAMPLE_RATE) / 1000;
    float *buf = malloc(sizeof(float) * frames);
    if(!buf) return;
    fill_click(buf, frames, SAMPLE_RATE);
    // write frames
    int written = 0;
    while(written < frames && running){
        int w = snd_pcm_writei(p->handle, buf + written, frames - written);
        if(w == -EPIPE){ snd_pcm_prepare(p->handle); continue; }
        if(w < 0){ fprintf(stderr,"ALSA write error: %s\n", snd_strerror(w)); break; }
        written += w;
    }
    free(buf);
}

// ---- Audio capture & detection thread ----
typedef struct { snd_pcm_t *handle; } cap_t;
static void *audio_thread(void *arg){
    (void)arg;
    snd_pcm_t *h;
    snd_pcm_hw_params_t *params;
    unsigned int rate = SAMPLE_RATE;
    int err;
    if((err = snd_pcm_open(&h, CAPTURE_DEVICE, SND_PCM_STREAM_CAPTURE, 0)) < 0){
        fprintf(stderr,"ALSA open capture error: %s\n", snd_strerror(err)); return NULL;
    }
    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(h, params);
    snd_pcm_hw_params_set_access(h, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(h, params, SND_PCM_FORMAT_FLOAT_LE);
    snd_pcm_hw_params_set_rate_near(h, params, &rate, 0);
    snd_pcm_hw_params_set_channels(h, params, AUDIO_CH);
    snd_pcm_hw_params_set_period_size(h, params, AUDIO_FRAMES, 0);
    if((err = snd_pcm_hw_params(h, params)) < 0){ fprintf(stderr,"ALSA capture hw params error: %s\n", snd_strerror(err)); snd_pcm_hw_params_free(params); snd_pcm_close(h); return NULL; }
    snd_pcm_hw_params_free(params);

    float *buf = malloc(sizeof(float) * AUDIO_FRAMES);
    if(!buf){ snd_pcm_close(h); return NULL; }

    while(running){
        int r = snd_pcm_readi(h, buf, AUDIO_FRAMES);
        if(r == -EPIPE){ snd_pcm_prepare(h); continue; }
        if(r < 0){ fprintf(stderr,"ALSA capture read error: %s\n", snd_strerror(r)); usleep(10000); continue; }
        // analyze buffer: peak and RMS
        float peak = 0.0f, sum = 0.0f;
        for(int i=0;i<r;i++){
            float v = buf[i];
            if(v < 0) v = -v;
            if(v > peak) peak = v;
            sum += v*v;
        }
        float rms = sqrtf(sum / r);
        if(peak > AUDIO_PEAK_THRESH || rms > AUDIO_RMS_THRESH){
            // push event
            evtq_push(&evtq);
            // simple debouncing: skip a short amount to avoid flood
            usleep(30000); // 30 ms
        }
    }

    free(buf);
    snd_pcm_close(h);
    return NULL;
}

// ---- V4L2 video capture & detection thread ----
static void *video_thread(void *arg){
    (void)arg;
    int fd = open(VIDEO_DEVICE, O_RDWR);
    if(fd < 0){ fprintf(stderr,"V4L2 open %s failed: %s\n", VIDEO_DEVICE, strerror(errno)); return NULL; }

    // query caps
    struct v4l2_capability cap;
    if(ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0){ fprintf(stderr,"VIDIOC_QUERYCAP failed\n"); close(fd); return NULL; }

    // enumerate formats and choose a common one (try YUYV)
    struct v4l2_format fmt;
    memset(&fmt,0,sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 640;
    fmt.fmt.pix.height = 480;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if(ioctl(fd, VIDIOC_S_FMT, &fmt) < 0){
        fprintf(stderr,"VIDIOC_S_FMT failed: %s\n", strerror(errno));
        close(fd); return NULL;
    }

    // request buffers (mmap)
    struct v4l2_requestbuffers req;
    memset(&req,0,sizeof(req));
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if(ioctl(fd, VIDIOC_REQBUFS, &req) < 0){ fprintf(stderr,"VIDIOC_REQBUFS failed\n"); close(fd); return NULL; }
    if(req.count < 2){ fprintf(stderr,"Insufficient buffer memory\n"); close(fd); return NULL; }

    void **buffers = calloc(req.count, sizeof(void*));
    size_t *buflen = calloc(req.count, sizeof(size_t));
    for(unsigned i=0;i<req.count;i++){
        struct v4l2_buffer bufq;
        memset(&bufq,0,sizeof(bufq));
        bufq.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        bufq.memory = V4L2_MEMORY_MMAP;
        bufq.index = i;
        if(ioctl(fd, VIDIOC_QUERYBUF, &bufq) < 0){ fprintf(stderr,"VIDIOC_QUERYBUF failed\n"); close(fd); return NULL; }
        buflen[i] = bufq.length;
        buffers[i] = mmap(NULL, bufq.length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, bufq.m.offset);
        if(buffers[i] == MAP_FAILED){ fprintf(stderr,"mmap failed\n"); close(fd); return NULL; }
    }
    // queue buffers
    for(unsigned i=0;i<req.count;i++){
        struct v4l2_buffer bufq;
        memset(&bufq,0,sizeof(bufq));
        bufq.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        bufq.memory = V4L2_MEMORY_MMAP;
        bufq.index = i;
        if(ioctl(fd, VIDIOC_QBUF, &bufq) < 0){ fprintf(stderr,"VIDIOC_QBUF failed\n"); }
    }
    // start stream
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(fd, VIDIOC_STREAMON, &type) < 0){ fprintf(stderr,"VIDIOC_STREAMON failed\n"); close(fd); return NULL; }

    // detection loop: read frames and evaluate Y component brightness (YUYV layout)
    while(running){
        struct v4l2_buffer bufq;
        memset(&bufq,0,sizeof(bufq));
        bufq.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        bufq.memory = V4L2_MEMORY_MMAP;
        if(ioctl(fd, VIDIOC_DQBUF, &bufq) < 0){ if(errno==EAGAIN){ usleep(1000); continue;} break; }
        uint8_t *data = (uint8_t*)buffers[bufq.index];
        size_t len = buflen[bufq.index];

        // YUYV: bytes are Y0 U Y1 V ...
        int bright = 0;
        for(size_t i=0;i+1<len;i+=2){
            uint8_t y = data[i];
            if(y >= VIDEO_BRIGHT_PIXEL_THRESH) bright++;
        }
        if(bright >= VIDEO_BRIGHT_COUNT_THRESH){
            evtq_push(&evtq);
            // small debounce
            usleep(30000);
        }

        if(ioctl(fd, VIDIOC_QBUF, &bufq) < 0){ fprintf(stderr,"VIDIOC_QBUF requeue failed\n"); break; }
    }

    // cleanup
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd, VIDIOC_STREAMOFF, &type);
    for(unsigned i=0;i<req.count;i++){
        if(buffers[i]) munmap(buffers[i], buflen[i]);
    }
    free(buffers); free(buflen);
    close(fd);
    return NULL;
}

// ---- Main: start threads and consume events ----
int main(int argc, char **argv){
    (void)argc; (void)argv;
    signal(SIGINT, sigint);

    evtq_init(&evtq);

    if(playback_init(&player) != 0) { fprintf(stderr,"Playback init failed\n"); return 1; }
    fprintf(stderr,"Playback opened at %d Hz\n", SAMPLE_RATE);

    pthread_t at, vt;
    if(pthread_create(&at, NULL, audio_thread, NULL) != 0){ fprintf(stderr,"audio thread create failed\n"); return 1; }
    if(pthread_create(&vt, NULL, video_thread, NULL) != 0){ fprintf(stderr,"video thread create failed\n"); /* continue audio only */ }

    // main loop: pop events and play clicks
    while(running){
        int has = evtq_pop(&evtq);
        if(!has) break;
        // play click (blocking)
        playback_play_click(&player);
        // small random jitter to simulate Geiger spacing
        struct timespec ts = {0, (rand() % 30000) * 1000}; // 0..30ms jitter
        nanosleep(&ts, NULL);
    }

    // shutdown: join threads and cleanup
    running = 0;
    pthread_join(at, NULL);
    pthread_join(vt, NULL);

    // drain ALSA and close
    snd_pcm_drain(player.handle);
    snd_pcm_close(player.handle);

    fprintf(stderr,"Exiting\n");
    return 0;
}

