// dosi.c
// Dosimeter-Konsole mit V4L2 (Hot-Pixel-Zählung), Adaptive EMA-Filter und X11-Visualisierung.
// Die Hot-Pixel-Ereignisse (simulierte Radioaktivität) werden im X11-Fenster Rot hervorgehoben.
//
// Kompilieren (ACHTUNG: Benötigt libx11-dev und libxext-dev):
// gcc -std=c99 -Wall -Wextra -O3 -march=native dosi.c -o dosi -lpthread -lasound -lm -lX11 -lXext

// =======================================================================
// HEADERS (Behebt Timespec/Timeval Konflikte zwischen ALSA und System-Headern)
// =======================================================================
#define _POSIX_C_SOURCE 200809L 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <errno.h>

// POSIX/System-Header (MÜSSEN VOR ALSA UND X11 kommen)
#include <time.h>        // Für timespec, nanosleep
#include <unistd.h>      // Für usleep
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/mman.h>    // Für mmap()
#include <sys/ioctl.h>   // Für ioctl()
#include <sys/shm.h>     // Für shmget, shmdt (XShm)
#include <fcntl.h>       // Für open()

// Kernel-Header
#include <linux/videodev2.h> 

// X11 Header
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h> 
#include <X11/Xatom.h>

// ALSA und Hilfs-Header
#include <alsa/asoundlib.h>
#include <alloca.h>      

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// =======================================================================
// Konfiguration & Strukturen
// =======================================================================
#define SAMPLE_RATE 44100
#define CHANNELS 1
#define ALERT_VOLUME 32000.0
#define ALERT_PCM_DEVICE "default"
#define UPDATE_INTERVAL_MS 50 

#define VIDEO_DEVICE "/dev/video0"
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define FRAME_FORMAT V4L2_PIX_FMT_YUYV 
#define PIXEL_THRESHOLD 200            
#define RAD_SCALING_FACTOR 0.01        

#define ALPHA_MIN 0.05 
#define ALPHA_MAX 0.50 
#define AEMA_SCALE_FACTOR 1.0 

typedef struct {
    int fd;
    struct v4l2_buffer buf;
    void *mem;
    size_t length;
    int hot_pixel_count;
} v4l2_context_t;

typedef struct {
    Display *display;
    int screen;
    Window window;
    GC gc;
    XImage *image;
    XShmSegmentInfo shm_info;
    bool shm_attached;
    Atom wm_delete_window; 
} x11_context_t;

typedef struct {
    volatile bool running;
    pthread_mutex_t mutex;
    pthread_cond_t cv;
    volatile bool alert_requested;
    volatile bool alert_stop_requested;
    volatile bool data_ready;
    double current_rad_value;
    double accumulated_dosage;
    double filtered_rad_value;
    double dynamic_alpha;
    double history[150];
    int history_count;
    time_t start_time;
    snd_pcm_t *alert_pcm_handle;
} dosimeter_state_t;

static dosimeter_state_t state;
static v4l2_context_t v4l2_ctx;
static x11_context_t x11_ctx;
static pthread_t alert_thread_id;
static pthread_t update_thread_id;

// =======================================================================
// Funktions-Prototypen (Löst alle "implicit declaration" Fehler)
// =======================================================================
static void play_alsa_click_sound();
static void play_alert();

// V4L2-Funktionen
static int v4l2_init(v4l2_context_t *ctx, int width, int height);
static void v4l2_cleanup(v4l2_context_t *ctx);
static int v4l2_capture_and_count(v4l2_context_t *ctx);
static int v4l2_capture_and_count_and_draw(v4l2_context_t *ctx, x11_context_t *x11_ctx);

// X11-Funktionen
static int x11_init(x11_context_t *ctx, int width, int height);
static void x11_cleanup(x11_context_t *ctx);

// Dosimeter-Funktionen
static double translate_noise_to_rad(int noise_type);
static void update_display_labels();
static void update_data();

// =======================================================================
// V4L2 Funktionen
// =======================================================================

static int v4l2_init(v4l2_context_t *ctx, int width, int height) {
    ctx->fd = open(VIDEO_DEVICE, O_RDWR | O_NONBLOCK, 0);
    if (ctx->fd == -1) {
        perror("Fehler beim Öffnen von /dev/video0");
        return -1;
    }

    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = width;
    fmt.fmt.pix.height = height;
    fmt.fmt.pix.pixelformat = FRAME_FORMAT;
    if (ioctl(ctx->fd, VIDIOC_S_FMT, &fmt) == -1) {
        perror("VIDIOC_S_FMT Fehler");
        close(ctx->fd);
        return -1;
    }

    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(ctx->fd, VIDIOC_REQBUFS, &req) == -1) {
        perror("VIDIOC_REQBUFS Fehler");
        close(ctx->fd);
        return -1;
    }

    ctx->buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ctx->buf.memory = V4L2_MEMORY_MMAP;
    ctx->buf.index = 0;
    if (ioctl(ctx->fd, VIDIOC_QUERYBUF, &ctx->buf) == -1) {
        perror("VIDIOC_QUERYBUF Fehler");
        close(ctx->fd);
        return -1;
    }

    ctx->length = ctx->buf.length;
    ctx->mem = mmap(NULL, ctx->length, PROT_READ | PROT_WRITE, MAP_SHARED, ctx->fd, ctx->buf.m.offset);
    if (ctx->mem == MAP_FAILED) {
        perror("mmap Fehler");
        close(ctx->fd);
        return -1;
    }

    if (ioctl(ctx->fd, VIDIOC_QBUF, &ctx->buf) == -1) {
        perror("VIDIOC_QBUF Fehler");
        munmap(ctx->mem, ctx->length);
        close(ctx->fd);
        return -1;
    }
    
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(ctx->fd, VIDIOC_STREAMON, &type) == -1) {
        perror("VIDIOC_STREAMON Fehler");
        munmap(ctx->mem, ctx->length);
        close(ctx->fd);
        return -1;
    }

    fprintf(stderr, "V4L2: Video-Stream auf %s gestartet. W: %d, H: %d, Buffer-Größe: %zu\n", 
            VIDEO_DEVICE, width, height, ctx->length);
    return 0;
}

static void v4l2_cleanup(v4l2_context_t *ctx) {
    if (ctx->fd > 0) {
        enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ioctl(ctx->fd, VIDIOC_STREAMOFF, &type);
        if (ctx->mem != MAP_FAILED && ctx->mem != NULL) {
            munmap(ctx->mem, ctx->length);
        }
        close(ctx->fd);
    }
}

// Fallback-Funktion (neu hinzugefügt, um Fehler zu beheben)
static int v4l2_capture_and_count(v4l2_context_t *ctx) {
    if (ioctl(ctx->fd, VIDIOC_DQBUF, &ctx->buf) == -1) {
        if (errno == EAGAIN) return ctx->hot_pixel_count;
        perror("VIDIOC_DQBUF Fehler");
        return -1;
    }
    
    int count = 0;
    unsigned char *data = (unsigned char *)ctx->mem;
    size_t num_pixels = FRAME_WIDTH * FRAME_HEIGHT;
    
    for (size_t i = 0; i < ctx->length && i < num_pixels * 2; i += 2) {
        if (data[i] >= PIXEL_THRESHOLD) {
            count++;
        }
    }
    
    ctx->hot_pixel_count = count;
    
    if (ioctl(ctx->fd, VIDIOC_QBUF, &ctx->buf) == -1) {
        perror("VIDIOC_QBUF Fehler");
        return -1;
    }
    
    return count;
}


// Erfasst Frame, zählt Hot Pixels UND zeichnet in X11-Puffer
static int v4l2_capture_and_count_and_draw(v4l2_context_t *ctx, x11_context_t *x11_ctx) {
    if (ioctl(ctx->fd, VIDIOC_DQBUF, &ctx->buf) == -1) {
        if (errno == EAGAIN) return ctx->hot_pixel_count;
        perror("VIDIOC_DQBUF Fehler");
        return -1;
    }
    
    int count = 0;
    unsigned char *v4l2_data = (unsigned char *)ctx->mem;
    char *x11_data = x11_ctx->image->data;
    
    // Iteriere über alle YUYV-Bytes (2 Bytes pro Pixel)
    for (size_t i = 0, x11_offset = 0; i < ctx->length && x11_offset < FRAME_WIDTH * FRAME_HEIGHT * 4; i += 2, x11_offset += 4) {
        
        unsigned char Y = v4l2_data[i];
        
        if (Y >= PIXEL_THRESHOLD) {
            count++;
            
            // Hervorhebung: Setze das Pixel im X11-Puffer auf ROT (0xFF0000)
            x11_data[x11_offset + 0] = 0;   // B
            x11_data[x11_offset + 1] = 0;   // G
            x11_data[x11_offset + 2] = 255; // R
            x11_data[x11_offset + 3] = 0;   // Alpha

        } else {
            // Konvertierung (Y -> Grauwert)
            x11_data[x11_offset + 0] = Y;     // B (Grau)
            x11_data[x11_offset + 1] = Y;     // G (Grau)
            x11_data[x11_offset + 2] = Y;     // R (Grau)
            x11_data[x11_offset + 3] = 0;
        }
    }
    
    ctx->hot_pixel_count = count;

    if (x11_ctx->display) {
        if (x11_ctx->shm_attached) {
            XShmPutImage(x11_ctx->display, x11_ctx->window, x11_ctx->gc, x11_ctx->image, 
                         0, 0, 0, 0, FRAME_WIDTH, FRAME_HEIGHT, False);
        } else {
            XPutImage(x11_ctx->display, x11_ctx->window, x11_ctx->gc, x11_ctx->image, 
                      0, 0, 0, 0, FRAME_WIDTH, FRAME_HEIGHT);
        }
        XFlush(x11_ctx->display);
    }

    if (ioctl(ctx->fd, VIDIOC_QBUF, &ctx->buf) == -1) {
        perror("VIDIOC_QBUF Fehler");
        return -1;
    }
    
    return count;
}

// =======================================================================
// X11 Funktionen
// =======================================================================

static void x11_cleanup(x11_context_t *ctx) {
    if (ctx->display) {
        if (ctx->shm_attached) {
            XShmDetach(ctx->display, &ctx->shm_info);
            shmdt(ctx->shm_info.shmaddr);
        }
        if (ctx->image) {
            if (!ctx->shm_attached && ctx->image->data) {
                free(ctx->image->data);
            }
            XDestroyImage(ctx->image);
        }
        if (ctx->gc) XFreeGC(ctx->display, ctx->gc);
        if (ctx->window) XDestroyWindow(ctx->display, ctx->window);
        XCloseDisplay(ctx->display);
    }
}

static int x11_init(x11_context_t *ctx, int width, int height) {
    ctx->display = XOpenDisplay(NULL);
    if (!ctx->display) {
        fprintf(stderr, "X11 Fehler: Konnte Display nicht öffnen. Visualisierung deaktiviert.\n");
        return -1;
    }

    ctx->screen = DefaultScreen(ctx->display);
    
    ctx->window = XCreateSimpleWindow(ctx->display, 
                                      RootWindow(ctx->display, ctx->screen),
                                      10, 10, width, height, 1,
                                      BlackPixel(ctx->display, ctx->screen),
                                      WhitePixel(ctx->display, ctx->screen));

    XStoreName(ctx->display, ctx->window, "☢️ Dosimeter Video Analyse (X11)");
    XSelectInput(ctx->display, ctx->window, ExposureMask | KeyPressMask);
    ctx->wm_delete_window = XInternAtom(ctx->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(ctx->display, ctx->window, &ctx->wm_delete_window, 1);

    XMapWindow(ctx->display, ctx->window);
    ctx->gc = XCreateGC(ctx->display, ctx->window, 0, NULL);
    
    if (XShmQueryExtension(ctx->display)) {
        int depth = DefaultDepth(ctx->display, ctx->screen);
        
        ctx->image = XShmCreateImage(ctx->display, 
                                      DefaultVisual(ctx->display, ctx->screen),
                                      depth, ZPixmap, NULL, 
                                      &ctx->shm_info, width, height);

        if (ctx->image) {
            ctx->shm_info.shmid = shmget(IPC_PRIVATE, ctx->image->bytes_per_line * height, IPC_CREAT | 0777);
            if (ctx->shm_info.shmid >= 0) {
                ctx->image->data = ctx->shm_info.shmaddr = shmat(ctx->shm_info.shmid, 0, 0);
                if (ctx->image->data != (char*)-1) {
                    ctx->shm_info.readOnly = False;
                    XShmAttach(ctx->display, &ctx->shm_info);
                    XSync(ctx->display, False);
                    shmctl(ctx->shm_info.shmid, IPC_RMID, 0); 
                    ctx->shm_attached = true;
                    fprintf(stderr, "X11: XShm (Shared Memory) ist aktiv.\n");
                }
            }
        }
    } 
    
    if (!ctx->shm_attached) {
        fprintf(stderr, "X11: XShm nicht aktiv/verfügbar. Verwende langsamen XImage-Modus.\n");
        int depth = DefaultDepth(ctx->display, ctx->screen);
        // Fallback: Erstelle Puffer im Heap
        ctx->image = XCreateImage(ctx->display, 
                                  DefaultVisual(ctx->display, ctx->screen),
                                  depth, ZPixmap, 0, (char*)malloc(width * height * 4), 
                                  width, height, 32, 0);
    }
    
    if (!ctx->image || ctx->image->data == NULL) {
        fprintf(stderr, "X11 Fehler: Konnte XImage Puffer nicht erstellen.\n");
        x11_cleanup(ctx);
        return -1;
    }

    return 0;
}


// =======================================================================
// Dosimeter Logik und Audio (gekürzt für Lesbarkeit)
// =======================================================================
static short* generate_sine_wave(double freq, double duration_sec, int rate, double amplitude, size_t *frames_out) {
    size_t n = (size_t)(duration_sec * rate); *frames_out = n;
    short *s = (short*)malloc(n * sizeof(short));
    for (size_t i = 0; i < n; ++i) { double t = (double)i / rate; s[i] = (short)(amplitude * sin(2.0 * M_PI * freq * t)); }
    return s;
}
static short* generate_square_wave(double freq, double duration_sec, int rate, double amplitude, size_t *frames_out) {
    size_t n = (size_t)(duration_sec * rate); *frames_out = n;
    short *s = (short*)malloc(n * sizeof(short));
    for (size_t i = 0; i < n; ++i) { double t = (double)i / rate; s[i] = (short)(amplitude * ((sin(2.0 * M_PI * freq * t) >= 0) ? 1.0 : -1.0)); }
    return s;
}
static short* generate_click(double duration_sec, int rate, double amplitude, size_t *frames_out) {
    size_t n = (size_t)(duration_sec * rate); *frames_out = n;
    short *s = (short*)malloc(n * sizeof(short));
    double freq = 10000.0;
    for (size_t i = 0; i < n; ++i) { double t = (double)i / rate; s[i] = (short)(amplitude * sin(2.0 * M_PI * freq * t) * exp(-t * 500.0)); }
    return s;
}
static void play_alsa_click_sound() {
    snd_pcm_t *pcm = NULL; snd_pcm_hw_params_t *params = (snd_pcm_hw_params_t *)alloca(snd_pcm_hw_params_sizeof());
    unsigned int rate = SAMPLE_RATE, chans = CHANNELS; double duration = 0.01; size_t frames;
    short *buf = generate_click(duration, rate, ALERT_VOLUME, &frames); if (!buf) { return; }
    if (snd_pcm_open(&pcm, ALERT_PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0) < 0) { free(buf); return; }
    snd_pcm_hw_params_any(pcm, params); snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_S16_LE); snd_pcm_hw_params_set_channels(pcm, params, chans);
    snd_pcm_hw_params_set_rate_near(pcm, params, &rate, NULL); snd_pcm_hw_params_set_period_size_near(pcm, params, (snd_pcm_uframes_t *)&frames, NULL);
    if (snd_pcm_hw_params(pcm, params) < 0) { snd_pcm_close(pcm); free(buf); return; }
    snd_pcm_writei(pcm, buf, frames); snd_pcm_drain(pcm); snd_pcm_close(pcm); free(buf);
}
static void play_alert() { pthread_mutex_lock(&state.mutex); state.alert_requested = true; pthread_mutex_unlock(&state.mutex); pthread_cond_signal(&state.cv); }
static double translate_noise_to_rad(int noise_type) {
    switch (noise_type) { case 0: return 0.1; case 1: return 0.5; case 2: return 1.0; 
        case 3: return 2.0; case 4: return 5.0; default: return 0.0; }
}

static void update_display_labels() {
    time_t now = time(NULL); long duration_sec = (long)(now - state.start_time);
    printf("\033[2J\033[H"); 
    printf("======================================\n"); printf("☢️ Dosimeter Console Monitor ☢️\n"); printf("======================================\n");
    printf("Current Time: %s", ctime(&now)); printf("Duration:     %ld seconds\n", duration_sec);
    printf("Accum. Dosage: %.3lf rad/m²\n", state.accumulated_dosage); printf("Last Reading:  %.3lf rad/m²\n", state.current_rad_value);
    printf("Filter Alpha:  %.2lf (Dynamic)\n", state.dynamic_alpha);
    if (v4l2_ctx.fd > 0) { printf("Hot Pixels:   %d (from %s)\n", v4l2_ctx.hot_pixel_count, VIDEO_DEVICE); } else { printf("Hot Pixels:   V4L2 ERROR (Using fallback)\n"); }
    printf("--------------------------------------\n"); printf("Press Ctrl+C or close X11 window to stop.\n"); fflush(stdout);
}

static void update_data() {
    double measured_rad_value; int hit_count = 0;
    if (v4l2_ctx.fd > 0 && x11_ctx.display) {
        hit_count = v4l2_capture_and_count_and_draw(&v4l2_ctx, &x11_ctx);
        if (hit_count < 0) hit_count = 0; 
        measured_rad_value = (double)hit_count * RAD_SCALING_FACTOR;
        if (measured_rad_value < 0.001) measured_rad_value = 0.001; 
    } else if (v4l2_ctx.fd > 0) { // V4L2 geht, aber X11 nicht
        hit_count = v4l2_capture_and_count(&v4l2_ctx);
        measured_rad_value = (double)hit_count * RAD_SCALING_FACTOR;
    } else {
        measured_rad_value = translate_noise_to_rad(rand() % 5);
    }
    
    double rad_value; pthread_mutex_lock(&state.mutex);
    if (state.filtered_rad_value == 0.0) {
        rad_value = measured_rad_value; state.dynamic_alpha = ALPHA_MIN;
    } else {
        double diff = fabs(measured_rad_value - state.filtered_rad_value);
        double alpha_range = ALPHA_MAX - ALPHA_MIN; double change_rate = diff / AEMA_SCALE_FACTOR;
        if (change_rate > 1.0) change_rate = 1.0; 
        state.dynamic_alpha = ALPHA_MIN + (alpha_range * change_rate);
        rad_value = (measured_rad_value * state.dynamic_alpha) + (state.filtered_rad_value * (1.0 - state.dynamic_alpha));
    }
    
    state.filtered_rad_value = rad_value; state.accumulated_dosage += rad_value; state.current_rad_value = rad_value; 
    if (state.history_count >= 150) {
        memmove(state.history, state.history + 1, (150 - 1) * sizeof(double)); state.history[149] = rad_value;
    } else { state.history[state.history_count++] = rad_value; }
    pthread_mutex_unlock(&state.mutex);

    play_alsa_click_sound();
    if (rad_value > 3.0) { play_alert(); }
    pthread_mutex_lock(&state.mutex); state.data_ready = true; pthread_mutex_unlock(&state.mutex);
    update_display_labels();
}

// =======================================================================
// Threads und Steuerung
// =======================================================================

static void* update_thread(void *arg) {
    (void)arg;
    while (state.running) {
        update_data();
        usleep(UPDATE_INTERVAL_MS * 1000); 
    }
    return NULL;
}

static void* alert_thread_function(void *arg) {
    (void)arg;
    while (true) {
        struct timespec ts; bool should_wait;
        pthread_mutex_lock(&state.mutex);
        should_wait = !(state.alert_requested || state.alert_stop_requested);
        while(should_wait) { pthread_cond_wait(&state.cv, &state.mutex); should_wait = !(state.alert_requested || state.alert_stop_requested); }
        if (state.alert_stop_requested) { pthread_mutex_unlock(&state.mutex); break; }
        if (state.alert_requested) {
            state.alert_requested = false;
            if (!state.alert_pcm_handle) {
                unsigned int rate = SAMPLE_RATE;
                if (snd_pcm_open(&state.alert_pcm_handle, ALERT_PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0) < 0) { pthread_mutex_unlock(&state.mutex); continue; }
                snd_pcm_hw_params_t *params = (snd_pcm_hw_params_t *)alloca(snd_pcm_hw_params_sizeof());
                snd_pcm_hw_params_any(state.alert_pcm_handle, params);
                snd_pcm_hw_params_set_access(state.alert_pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
                snd_pcm_hw_params_set_format(state.alert_pcm_handle, params, SND_PCM_FORMAT_S16_LE);
                snd_pcm_hw_params_set_channels(state.alert_pcm_handle, params, CHANNELS);
                snd_pcm_hw_params_set_rate_near(state.alert_pcm_handle, params, &rate, NULL);
                snd_pcm_hw_params(state.alert_pcm_handle, params);
            }
            double rad = state.current_rad_value; double interval = 1.0 / (1.0 + log(1.0 + rad));
            if (interval < 0.1) interval = 0.1; double dur = 0.2; double base_freq = 440.0;
            pthread_mutex_unlock(&state.mutex); 
            size_t base_frames, mod_frames;
            short *base = generate_square_wave(base_freq, dur, SAMPLE_RATE, ALERT_VOLUME, &base_frames);
            short *mod = generate_sine_wave(base_freq * 1.5, dur, SAMPLE_RATE, ALERT_VOLUME * 0.5, &mod_frames);
            if (base && mod) { snd_pcm_writei(state.alert_pcm_handle, base, base_frames); }
            free(base); free(mod); 
            ts.tv_sec = (time_t)interval; ts.tv_nsec = (long)((interval - (double)ts.tv_sec) * 1e9);
            nanosleep(&ts, NULL);
        } else { pthread_mutex_unlock(&state.mutex); }
    }
    return NULL;
}


static void sigint_handler(int sig) {
    (void)sig; state.running = false;
    pthread_mutex_lock(&state.mutex); state.alert_stop_requested = true; pthread_mutex_unlock(&state.mutex);
    pthread_cond_signal(&state.cv); fprintf(stderr, "\nShutting down...\n");
}

static void initialize_dosimeter() {
    memset(&state, 0, sizeof(state)); state.running = true; state.start_time = time(NULL);
    state.filtered_rad_value = 0.0; state.dynamic_alpha = ALPHA_MIN; srand(time(NULL));
    pthread_mutex_init(&state.mutex, NULL); pthread_cond_init(&state.cv, NULL);
}

static void cleanup_dosimeter() {
    if (update_thread_id) pthread_join(update_thread_id, NULL);
    if (alert_thread_id) pthread_join(alert_thread_id, NULL);
    v4l2_cleanup(&v4l2_ctx);
    x11_cleanup(&x11_ctx);
    if (state.alert_pcm_handle) { snd_pcm_drain(state.alert_pcm_handle); snd_pcm_close(state.alert_pcm_handle); }
    pthread_mutex_destroy(&state.mutex); pthread_cond_destroy(&state.cv);
}

int main() {
    initialize_dosimeter();
    signal(SIGINT, sigint_handler);

    if (v4l2_init(&v4l2_ctx, FRAME_WIDTH, FRAME_HEIGHT) == 0) {
        if (x11_init(&x11_ctx, FRAME_WIDTH, FRAME_HEIGHT) == 0) {
            fprintf(stderr, "Video und X11 sind aktiv.\n");
        }
    } else {
        fprintf(stderr, "V4L2 konnte nicht initialisiert werden. Simuliere weiterhin Zufallswerte.\n");
    }

    if (pthread_create(&alert_thread_id, NULL, alert_thread_function, NULL) != 0 ||
        pthread_create(&update_thread_id, NULL, update_thread, NULL) != 0) {
        fprintf(stderr, "Failed to create threads.\n");
        pthread_mutex_lock(&state.mutex); state.alert_stop_requested = true; pthread_mutex_unlock(&state.mutex);
        pthread_cond_signal(&state.cv); cleanup_dosimeter();
        return 1;
    }

    while (state.running) {
        if (x11_ctx.display) {
            XEvent ev;
            while (XPending(x11_ctx.display)) {
                XNextEvent(x11_ctx.display, &ev);
                if (ev.type == KeyPress) {
                    state.running = false; 
                } else if (ev.type == ClientMessage) {
                    if ((Atom)ev.xclient.data.l[0] == x11_ctx.wm_delete_window) {
                        state.running = false;
                    }
                }
            }
        }
        sleep(0);
    }

    cleanup_dosimeter();
    return 0;
}

