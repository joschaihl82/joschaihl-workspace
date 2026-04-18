#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/select.h> // Fuer select() in der Hauptschleife
#include <math.h> 
#include <X11/Xutil.h>
// X11 und V4L2 Header
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <linux/videodev2.h>

// --- KONFIGURATION (MUSS ANGEPASST WERDEN) ---
#define VIDEO_DEVICE "/dev/video0"
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define NUM_BUFFERS 4 // Anzahl der Puffer für V4L2-Streaming
#define BYTES_PER_PIXEL 3 // 24-bit RGB fuer XImage

// --- DYNAMISCHE OPTIMIERUNG ---
#define TARGET_AVG_Y 60.0 
#define SMOOTHING_FACTOR 0.05
#define GAMMA_MIN 0.2
#define GAMMA_MAX 1.0

#define CLAMP(x) ((x) < 0 ? 0 : ((x) > 255 ? 255 : (x)))

// Globale X11/V4L2/Dynamik Variablen
Display *g_dpy;
Window g_win;
GC g_gc;
XImage *g_ximage;
XShmSegmentInfo g_shminfo;
static double g_current_gamma = 1.0; 
static int g_fd;

// Struktur zur Verwaltung der V4L2-Puffer
struct buffer {
    void *start;
    size_t length;
};
static struct buffer *g_buffers;


// =========================================================================
//                   FARBKONVERTIERUNG UND FILTER-LOGIK (64-bit)
// =========================================================================

/**
 * Konvertiert RGB [0.0, 1.0] in HSV [H:0-360, S/V:0-1.0]. Verwendet 64-bit double.
 */
void rgb_to_hsv(double r, double g, double b, double *h, double *s, double *v) {
    double max = fmax(fmax(r, g), b);
    double min = fmin(fmin(r, g), b);
    double delta = max - min;

    *v = max;
    *s = (max == 0.0) ? 0.0 : (delta / max);

    if (delta == 0.0) {
        *h = 0.0;
    } else {
        if (r == max) *h = 60.0 * fmod(((g - b) / delta), 6.0);
        else if (g == max) *h = 60.0 * (((b - r) / delta) + 2.0);
        else *h = 60.0 * (((r - g) / delta) + 4.0);
    }
    if (*h < 0.0) *h += 360.0;
}

/**
 * Konvertiert HSV zurueck in RGB [0.0, 1.0]. Verwendet 64-bit double.
 */
void hsv_to_rgb(double h, double s, double v, double *r, double *g, double *b) {
    int i;
    double f, p, q, t;

    if (s == 0.0) { *r = *g = *b = v; return; }

    h = fmod(h, 360.0) / 60.0;
    i = (int)floor(h);
    f = h - (double)i;

    p = v * (1.0 - s);
    q = v * (1.0 - s * f);
    t = v * (1.0 - s * (1.0 - f));

    switch (i) {
        case 0: *r = v; *g = t; *b = p; break;
        case 1: *r = q; *g = v; *b = p; break;
        case 2: *r = p; *g = v; *b = t; break;
        case 3: *r = p; *g = q; *b = v; break;
        case 4: *r = t; *g = p; *b = v; break;
        default: *r = v; *g = p; *b = q; break;
    }
}

/**
 * Berechnet einen dynamisch angepassten Gamma-Wert (64-bit).
 */
double calculate_dynamic_gamma(double avg_y) {
    double deviation = TARGET_AVG_Y - avg_y;
    double adjustment = deviation / 255.0; 
    double new_gamma = g_current_gamma - adjustment;

    if (new_gamma < GAMMA_MIN) new_gamma = GAMMA_MIN;
    if (new_gamma > GAMMA_MAX) new_gamma = GAMMA_MAX;

    g_current_gamma = g_current_gamma * (1.0 - SMOOTHING_FACTOR) + new_gamma * SMOOTHING_FACTOR;
    return g_current_gamma;
}

/**
 * Wendet die Nachtsicht-Transformation mit dynamischem Gamma (64-bit) an.
 */
void apply_night_vision_filter_dynamic(double *h, double *s, double *v, double current_gamma) {
    *h = 120.0; // Gruen-Ton
    *s = 1.0;   // Maximale Saettigung
    
    // Dynamische Gamma-Korrektur auf V (Helligkeit)
    *v = pow(*v, current_gamma);
}

/**
 * Konvertiert YUYV zu RGB (24-bit) und berechnet die durchschnittliche Helligkeit.
 */
void yuyv_to_rgb(const unsigned char *yuyv_in, unsigned char *rgb_out, int width, int height, double *avg_y_out) {
    long yuyv_size = width * height * 2;
    long rgb_index = 0;
    long total_y = 0;
    int y_count = 0;

    for (long i = 0; i < yuyv_size; i += 4) {
        int y1 = yuyv_in[i];
        int u  = yuyv_in[i + 1];
        int y2 = yuyv_in[i + 2];
        int v_y  = yuyv_in[i + 3];

        total_y += y1 + y2;
        y_count += 2;

        int c1 = y1 - 16;
        int c2 = y2 - 16;
        int d = u - 128;
        int e = v_y - 128;

        // --- Pixel 1 (Y1) ---
        int r1 = (int)(1.164 * c1 + 1.596 * e);
        int g1 = (int)(1.164 * c1 - 0.813 * e - 0.391 * d);
        int b1 = (int)(1.164 * c1 + 2.018 * d);

        rgb_out[rgb_index++] = CLAMP(r1);
        rgb_out[rgb_index++] = CLAMP(g1);
        rgb_out[rgb_index++] = CLAMP(b1);

        // --- Pixel 2 (Y2) ---
        int r2 = (int)(1.164 * c2 + 1.596 * e);
        int g2 = (int)(1.164 * c2 - 0.813 * e - 0.391 * d);
        int b2 = (int)(1.164 * c2 + 2.018 * d);

        rgb_out[rgb_index++] = CLAMP(r2);
        rgb_out[rgb_index++] = CLAMP(g2);
        rgb_out[rgb_index++] = CLAMP(b2);
    }
    
    *avg_y_out = (double)total_y / y_count;
}


// =========================================================================
//                         V4L2 UND X11 INIT/CLEANUP
// =========================================================================

// Makro für einfaches Error-Handling
#define X_IOCTL(fd, request, data) \
    do { \
        if (ioctl(fd, request, data) < 0) { \
            perror(#request); \
            return -1; \
        } \
    } while (0)

/**
 * Initialisiert V4L2: oeffnet Geraet, setzt Format YUYV, mappt Puffer, startet Streaming.
 */
int v4l2_init(void) {
    g_fd = open(VIDEO_DEVICE, O_RDWR | O_NONBLOCK, 0);
    if (g_fd < 0) {
        perror("Fehler beim Oeffnen von /dev/video0. Ist die Webcam angeschlossen?");
        return -1;
    }

    // 1. Format setzen (YUYV)
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = FRAME_WIDTH;
    fmt.fmt.pix.height = FRAME_HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    X_IOCTL(g_fd, VIDIOC_S_FMT, &fmt);
    
    // Überprüfung, ob das Format gesetzt wurde (Optional, aber gut)
    if (fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV) {
        fprintf(stderr, "Fehler: Geraet unterstuetzt YUYV nicht. Gefunden: %c%c%c%c\n", 
                (fmt.fmt.pix.pixelformat & 0xFF), (fmt.fmt.pix.pixelformat >> 8) & 0xFF,
                (fmt.fmt.pix.pixelformat >> 16) & 0xFF, (fmt.fmt.pix.pixelformat >> 24) & 0xFF);
        return -1;
    }
    
    // 2. Puffer anfordern
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = NUM_BUFFERS;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    X_IOCTL(g_fd, VIDIOC_REQBUFS, &req);

    g_buffers = calloc(req.count, sizeof(*g_buffers));
    
    // 3. Puffer mappen und in die Queue stellen
    for (unsigned int n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = n_buffers;
        X_IOCTL(g_fd, VIDIOC_QUERYBUF, &buf);

        g_buffers[n_buffers].length = buf.length;
        g_buffers[n_buffers].start = mmap(NULL /* start anywhere */,
                                          buf.length,
                                          PROT_READ | PROT_WRITE /* required */,
                                          MAP_SHARED /* recommended */,
                                          g_fd, buf.m.offset);

        if (g_buffers[n_buffers].start == MAP_FAILED) {
            perror("mmap");
            return -1;
        }

        // Alle Puffer in die Queue stellen
        X_IOCTL(g_fd, VIDIOC_QBUF, &buf);
    }
    
    // 4. Streaming starten
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    X_IOCTL(g_fd, VIDIOC_STREAMON, &type);
    
    printf("V4L2 Initialisiert. Streaming gestartet (%dx%d).\n", FRAME_WIDTH, FRAME_HEIGHT);
    return 0;
}

/**
 * Initialisiert X11, erstellt Fenster und Shared Memory (XShm).
 */
int x11_init(void) {
    g_dpy = XOpenDisplay(NULL);
    if (!g_dpy) {
        fprintf(stderr, "Kann X-Display nicht oeffnen.\n");
        return -1;
    }

    // Fenster erstellen
    int screen = DefaultScreen(g_dpy);
    unsigned long black = BlackPixel(g_dpy, screen);
    unsigned long white = WhitePixel(g_dpy, screen);

    g_win = XCreateWindow(g_dpy, DefaultRootWindow(g_dpy), 0, 0, FRAME_WIDTH, FRAME_HEIGHT, 0, 
                          DefaultDepth(g_dpy, screen), InputOutput, DefaultVisual(g_dpy, screen), 0, NULL);
    XStoreName(g_dpy, g_win, "Ghost Night Vision (HSV 64-bit)");
    XMapWindow(g_dpy, g_win);
    
    g_gc = XCreateGC(g_dpy, g_win, 0, NULL);
    XSetForeground(g_dpy, g_gc, white);

    // XShm initialisieren
    g_ximage = XShmCreateImage(g_dpy, DefaultVisual(g_dpy, screen), DefaultDepth(g_dpy, screen), 
                             ZPixmap, NULL, &g_shminfo, FRAME_WIDTH, FRAME_HEIGHT);

    // Shared Memory Segment anfordern
    g_shminfo.shmid = shmget(IPC_PRIVATE, g_ximage->bytes_per_line * g_ximage->height, IPC_CREAT | 0777);
    if (g_shminfo.shmid < 0) {
        perror("shmget");
        return -1;
    }
    
    // Segment in Prozessraum mappen und XImage verbinden
    g_shminfo.shmaddr = g_ximage->data = shmat(g_shminfo.shmid, 0, 0);
    g_shminfo.readOnly = False;

    // Segment fuer den X-Server freigeben und zur Löschung markieren
    XShmAttach(g_dpy, &g_shminfo);
    shmctl(g_shminfo.shmid, IPC_RMID, 0); 

    printf("X11/XShm Initialisiert. Fenster erstellt.\n");
    return 0;
}

/**
 * Raeumt alle Ressourcen auf.
 */
void cleanup(unsigned char *rgb_temp) {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    // Streaming stoppen
    ioctl(g_fd, VIDIOC_STREAMOFF, &type);

    // mmap Puffer freigeben
    for (unsigned int i = 0; i < NUM_BUFFERS; ++i)
        munmap(g_buffers[i].start, g_buffers[i].length);
    
    close(g_fd);
    free(g_buffers);
    free(rgb_temp);

    // X11/XShm aufraeumen
    XShmDetach(g_dpy, &g_shminfo);
    XDestroyImage(g_ximage);
    shmdt(g_shminfo.shmaddr);
    XDestroyWindow(g_dpy, g_win);
    XCloseDisplay(g_dpy);
    
    printf("Ressourcen freigegeben.\n");
}


// =========================================================================
//                             HAUPTPROGRAMM
// =========================================================================

int main(void) {
    if (v4l2_init() != 0) return 1;
    if (x11_init() != 0) return 1;

    size_t rgb_size = FRAME_WIDTH * FRAME_HEIGHT * BYTES_PER_PIXEL;
    unsigned char *rgb_temp = malloc(rgb_size);

    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    while (1) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(g_fd, &fds);
        
        // Timeout-Struktur (z.B. 2 Sekunden)
        struct timeval tv;
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        // Warte auf einen verfügbaren Frame
        int r = select(g_fd + 1, &fds, NULL, NULL, &tv);
        if (r == -1) {
            if (errno == EINTR) continue;
            perror("select");
            break;
        }
        if (r == 0) {
            fprintf(stderr, "Kamera-Timeout.\n");
            continue;
        }

        // --- 1. V4L2 Frame erfassen (Puffer aus der Queue nehmen) ---
        if (ioctl(g_fd, VIDIOC_DQBUF, &buf) < 0) {
            perror("VIDIOC_DQBUF");
            break;
        }
        const unsigned char *yuyv_data = g_buffers[buf.index].start; 
        
        double avg_y;
        
        // --- 2. YUYV -> RGB und Helligkeit messen ---
        yuyv_to_rgb(yuyv_data, rgb_temp, FRAME_WIDTH, FRAME_HEIGHT, &avg_y);
        
        // --- 3. Dynamisches Gamma berechnen ---
        double dynamic_gamma = calculate_dynamic_gamma(avg_y);

        // --- 4. HSV-Verarbeitung (Pixel fuer Pixel) ---
        unsigned char *output_rgb_x11 = (unsigned char *)g_ximage->data; 

        for (int i = 0; i < FRAME_WIDTH * FRAME_HEIGHT; i++) {
            // Normalisierung von [0, 255] auf [0.0, 1.0]
            double r = rgb_temp[i*3] / 255.0;
            double g = rgb_temp[i*3+1] / 255.0;
            double b = rgb_temp[i*3+2] / 255.0;

            double h, s, v;

            // RGB -> HSV (64-bit)
            rgb_to_hsv(r, g, b, &h, &s, &v); 

            // Filter anwenden (Nachtsicht & dynamische Helligkeit)
            apply_night_vision_filter_dynamic(&h, &s, &v, dynamic_gamma);

            // HSV -> RGB zurueck (64-bit)
            double r_new, g_new, b_new;
            hsv_to_rgb(h, s, v, &r_new, &g_new, &b_new); 

            // Denormalisieren und in XImage Puffer schreiben (24-bit RGB)
            // Die Reihenfolge kann je nach X-Visual BGR oder RGB sein. Hier als RGB angenommen.
            output_rgb_x11[i * BYTES_PER_PIXEL + 0] = CLAMP((int)(r_new * 255.0)); // R
            output_rgb_x11[i * BYTES_PER_PIXEL + 1] = CLAMP((int)(g_new * 255.0)); // G
            output_rgb_x11[i * BYTES_PER_PIXEL + 2] = CLAMP((int)(b_new * 255.0)); // B
        }

        // --- 5. Anzeige und Pufferfreigabe ---
        XShmPutImage(g_dpy, g_win, g_gc, g_ximage, 0, 0, 0, 0, FRAME_WIDTH, FRAME_HEIGHT, False);
        XFlush(g_dpy);

        // Puffer wieder in die Queue stellen
        if (ioctl(g_fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF");
            break;
        }

        // X-Events verarbeiten (z.B. Fenster schließen)
        XEvent ev;
        while (XPending(g_dpy)) {
            XNextEvent(g_dpy, &ev);
            if (ev.type == DestroyNotify) {
                goto end_loop; // Beende die Hauptschleife
            }
        }
    }

end_loop:
    cleanup(rgb_temp);
    return 0;
}


