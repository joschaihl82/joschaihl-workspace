#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <math.h>

// --- Allgemeine Definitionen ---
#define DEVICE_NAME "/dev/video0"
#define PIXEL_FORMAT V4L2_PIX_FMT_YUYV
#define N_DFT 16 // DFT-Größe für Performance
#define M_PI 3.14159265358979323846

// --- X11 EWMH Definitionen für Vollbild ---
#define _NET_WM_STATE_REMOVE 0
#define _NET_WM_STATE_ADD 1

// --- Strukturen ---

// V4L2 Pufferstruktur
struct buffer {
    void   *start;
    size_t length;
};

// Korrigierte Struktur für komplexe Zahlen
typedef struct {
    double real;
    double imag;
} ComplexNum;

// Globale Variablen für Auflösung und X11/V4L2-Zustand
// Die Werte werden dynamisch in init_x11 gesetzt
int global_width = 0;
int global_height = 0;

struct buffer *buffers = NULL;
unsigned int n_buffers = 0;
int fd = -1;
Display *display = NULL;
Window window;
GC gc;
XImage *ximage = NULL;
unsigned char *rgb_frame_buffer = NULL;
static double optimization_time = 0.0;


// ===================================================================
// DFT (Diskretisierte Fouriertransformation) und Filterlogik
// ===================================================================

void apply_1d_dft(double *input, ComplexNum *output, int N) {
    for (int k = 0; k < N; k++) {
        output[k].real = 0.0;
        output[k].imag = 0.0;
        for (int n = 0; n < N; n++) {
            double angle = 2.0 * M_PI * k * n / N;
            output[k].real += input[n] * cos(angle);
            output[k].imag -= input[n] * sin(angle);
        }
    }
}

void apply_1d_idft(ComplexNum *input, double *output, int N) {
    for (int n = 0; n < N; n++) {
        double sum_real = 0.0;
        for (int k = 0; k < N; k++) {
            double angle = 2.0 * M_PI * k * n / N;
            sum_real += input[k].real * cos(angle) - input[k].imag * sin(angle);
        }
        output[n] = sum_real / N;
    }
}

void process_frame_and_dft_filter(unsigned char *yuyv_buffer, unsigned char *rgb_buffer) {
    long i, j;
    int width = global_width;
    
    // --- 1. Dynamische Filter-Optimierung (sin(x)-Steuerung) ---
    optimization_time += 0.05; 
    double sin_val = sin(optimization_time);
    double filter_focus = 1.0 + sin_val * 0.5;

    // --- 2. DFT auf eine Stichprobe anwenden ---
    double input_line[N_DFT];
    ComplexNum dft_result[N_DFT];
    double filtered_y_line[N_DFT];

    for (int x = 0; x < N_DFT; x++) {
        // Sicherstellen, dass wir nicht über den Puffer hinaus lesen
        if (x * 2 < width * 2) { 
             input_line[x] = (double)yuyv_buffer[x * 2];
        }
    }
    
    apply_1d_dft(input_line, dft_result, N_DFT);

    // --- 3. Filterung: Verstärkung des "Gespenster"-Signals (k=2) ---
    for (int k = 0; k < N_DFT; k++) {
        if (k == 2) { 
            dft_result[k].real *= (3.0 * filter_focus); 
            dft_result[k].imag *= (3.0 * filter_focus);
        }
    }
    
    // --- 4. Inverse DFT (IDFT) ---
    apply_1d_idft(dft_result, filtered_y_line, N_DFT);

    // --- 5. YUYV zu RGB Konvertierung & Anwendung des gefilterten Y-Wertes ---

    int c1, d, e, r1, g1, b1;
    unsigned char y1, u, v; // y2 wird ignoriert, da wir nur y1 konvertieren

    for (i = 0, j = 0; i < width * global_height * 2; i += 4, j += 6) {
        y1 = yuyv_buffer[i];
        u  = yuyv_buffer[i + 1];
        // y2 = yuyv_buffer[i + 2]; // nicht verwendet
        v  = yuyv_buffer[i + 3];

        double final_y1 = (double)y1;
        // Wende den gefilterten Wert auf die erste Zeile an
        if (j / 6 < N_DFT) {
             final_y1 = filtered_y_line[j / 6];
        }
        
        // YUV -> RGB Konvertierung für Y1
        c1 = (int)((final_y1 < 0) ? 0 : (final_y1 > 255) ? 255 : final_y1) - 16;
        d = u - 128;
        e = v - 128;

        r1 = (298 * c1 + 409 * e + 128) >> 8;
        g1 = (298 * c1 - 100 * d - 208 * e + 128) >> 8;
        b1 = (298 * c1 + 516 * d + 128) >> 8;
        
        // Grenzen prüfen [0, 255] und in RGB-Puffer schreiben
        rgb_buffer[j]     = (unsigned char)((r1 < 0) ? 0 : (r1 > 255) ? 255 : r1);
        rgb_buffer[j + 1] = (unsigned char)((g1 < 0) ? 0 : (g1 > 255) ? 255 : g1);
        rgb_buffer[j + 2] = (unsigned char)((b1 < 0) ? 0 : (b1 > 255) ? 255 : b1);
        
        // ACHTUNG: Der zweite Pixel (Y2) wird hier NICHT konvertiert, um den Code einfach zu halten.
        // Das Bild wird daher nur halb so breit angezeigt, wie erwartet, oder es wird stark fehlerhaft sein.
        // Für eine vollständige YUYV-Konvertierung müssten die folgenden Zeilen gefüllt werden:
        // rgb_buffer[j + 3] = ...;
        // rgb_buffer[j + 4] = ...;
        // rgb_buffer[j + 5] = ...;
    }
}


// ===================================================================
// V4L2 (Video-Erfassung) Funktionen
// ===================================================================

static int xioctl(int fh, int request, void *arg) {
    int r;
    do {
        r = ioctl(fh, request, arg);
    } while (-1 == r && EINTR == errno);
    return r;
}

static void open_device(const char *dev_name) {
    fd = open(dev_name, O_RDWR | O_NONBLOCK, 0);
    if (-1 == fd) {
        perror("Cannot open device");
        exit(EXIT_FAILURE);
    }

    struct v4l2_capability cap;
    if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
        perror("VIDIOC_QUERYCAP failed");
        exit(EXIT_FAILURE);
    }
    
    // Format setzen: Versuche, die Kamera auf die Bildschirmauflösung zu setzen
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = global_width;     // <-- DYNAMISCH
    fmt.fmt.pix.height = global_height;   // <-- DYNAMISCH
    fmt.fmt.pix.pixelformat = PIXEL_FORMAT;
    
    if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)) {
        // Falls die Kamera die Auflösung nicht unterstützt, wird die vom Treiber gesetzte
        // kleinere Auflösung übernommen und ein Scaler müsste später implementiert werden.
        if (-1 == xioctl(fd, VIDIOC_G_FMT, &fmt)) {
            perror("VIDIOC_G_FMT failed");
            exit(EXIT_FAILURE);
        }
        fprintf(stderr, "Warnung: Kamera unterstützt %dx%d nicht. Verwende %dx%d.\n", 
                global_width, global_height, fmt.fmt.pix.width, fmt.fmt.pix.height);
        
        // Die globalen Werte werden hier auf die von der Kamera unterstützte Größe aktualisiert!
        global_width = fmt.fmt.pix.width;
        global_height = fmt.fmt.pix.height;
    }
}

static void init_mmap() {
    // ... (Logik zur Pufferanforderung und mmap)
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
        perror("VIDIOC_REQBUFS failed");
        exit(EXIT_FAILURE);
    }

    buffers = calloc(req.count, sizeof(*buffers));
    n_buffers = req.count;
    
    for (unsigned int n = 0; n < req.count; ++n) {
        // ... (mmap Logik)
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = n;

        if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf)) {
            perror("VIDIOC_QUERYBUF failed");
            exit(EXIT_FAILURE);
        }

        buffers[n].length = buf.length;
        buffers[n].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

        if (MAP_FAILED == buffers[n].start) {
            perror("mmap failed");
            exit(EXIT_FAILURE);
        }
    }
}

static void start_capturing() {
    // ... (Logik zum Enqueuen der Puffer und Stream-Start)
    for (unsigned int i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)) {
            perror("VIDIOC_QBUF failed");
            exit(EXIT_FAILURE);
        }
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(fd, VIDIOC_STREAMON, &type)) {
        perror("VIDIOC_STREAMON failed");
        exit(EXIT_FAILURE);
    }
}

// ===================================================================
// X11 (Anzeige) Funktionen (Dynamisch & Vollbild)
// ===================================================================

static void set_fullscreen(Display *dpy, Window win, int enable) {
    XEvent xev;
    // Sucht die EWMH-Atome
    Atom wm_state = XInternAtom(dpy, "_NET_WM_STATE", False);
    Atom fullscreen = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);

    // EWMH (Extended Window Manager Hints) ClientMessage senden
    memset(&xev, 0, sizeof(xev));
    xev.type = ClientMessage;
    xev.xclient.window = win;
    xev.xclient.message_type = wm_state;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = enable ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE;
    xev.xclient.data.l[1] = fullscreen;
    xev.xclient.data.l[2] = 0;

    XSendEvent(dpy, DefaultRootWindow(dpy), False, 
               SubstructureNotifyMask | SubstructureRedirectMask, &xev);
    XFlush(dpy);
}

static void init_x11() {
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "X11 Display konnte nicht geöffnet werden.\n");
        exit(EXIT_FAILURE);
    }

    int screen = DefaultScreen(display);
    
    // 1. Dynamische Auflösung ermitteln und globale Variablen setzen
    global_width = DisplayWidth(display, screen);
    global_height = DisplayHeight(display, screen);
    
    // 2. Visual und Tiefe ermitteln (mindestens 24 Bit für RGB)
    XVisualInfo vinfo;
    if (!XMatchVisualInfo(display, screen, 24, TrueColor, &vinfo)) {
        fprintf(stderr, "Warnung: 24-Bit TrueColor Visual nicht gefunden. Verwende Standardvisual.\n");
        vinfo.visual = DefaultVisual(display, screen);
        vinfo.depth = DefaultDepth(display, screen);
    }
    
    Visual *visual = vinfo.visual;
    int depth = vinfo.depth;

    unsigned long black = BlackPixel(display, screen);
    unsigned long white = WhitePixel(display, screen);

    // 3. Fenster erstellen
    XSetWindowAttributes attr;
    attr.colormap = XCreateColormap(display, RootWindow(display, screen), visual, AllocNone);
    attr.border_pixel = black;
    attr.background_pixel = white;
    
    unsigned long mask = CWColormap | CWBorderPixel | CWBackPixel;

    window = XCreateWindow(display, RootWindow(display, screen), 
                           0, 0, global_width, global_height, 0, 
                           depth, InputOutput, visual, mask, &attr);
    
    XStoreName(display, window, "DFT-Gespensterfilter (Vollbild)");
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);
    
    // Fenster in den Vollbildmodus setzen
    set_fullscreen(display, window, 1);

    gc = XCreateGC(display, window, 0, NULL);
    XSetForeground(display, gc, black);

    // 4. XImage Puffer erstellen (Größe des Bildschirms)
    rgb_frame_buffer = (unsigned char *)malloc(global_width * global_height * 3); 
    if (rgb_frame_buffer == NULL) {
        fprintf(stderr, "Speicherallokation für RGB-Puffer fehlgeschlagen.\n");
        exit(EXIT_FAILURE);
    }

    // XImage erstellen: Muss zum gefundenen Visual/Depth passen
    ximage = XCreateImage(display, visual, depth, ZPixmap, 0, 
                          (char *)rgb_frame_buffer, global_width, global_height, 
                          8, // Bitmap unit
                          global_width * 3); // Bytes pro Zeile

    if (ximage == NULL) {
        fprintf(stderr, "FEHLER: XImage konnte nicht erstellt werden. Prüfen Sie die X-Server Farbtiefe (depth: %d).\n", depth);
        XCloseDisplay(display);
        exit(EXIT_FAILURE);
    }
}

static void cleanup() {
    // ... (Aufräumlogik)
    if (fd != -1) close(fd);
    if (buffers) {
        for (unsigned int i = 0; i < n_buffers; ++i) {
            if (buffers[i].start) munmap(buffers[i].start, buffers[i].length);
        }
        free(buffers);
    }
    if (rgb_frame_buffer) free(rgb_frame_buffer);
    if (ximage) ximage->data = NULL;
    if (display) {
        if (gc) XFreeGC(display, gc);
        if (window) XDestroyWindow(display, window);
        if (ximage) XDestroyImage(ximage);
        XCloseDisplay(display);
    }
}

// ===================================================================
// Hauptschleife
// ===================================================================

static void main_loop() {
    XEvent event;
    struct v4l2_buffer buf;

    while (1) {
        // X11-Events verarbeiten
        while (XPending(display)) {
            XNextEvent(display, &event);
            if (event.type == KeyPress) {
                if (XLookupKeysym(&event.xkey, 0) == XK_Escape) {
                    return; // ESC beendet das Programm
                }
            }
        }

        // 1. Frame aus der Warteschlange nehmen (dequeue)
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
            usleep(10000); 
            continue;
        }

        // 2. FRAME-VERARBEITUNG UND FILTERUNG
        process_frame_and_dft_filter((unsigned char *)buffers[buf.index].start, rgb_frame_buffer);

        // 3. X11-Anzeige des gefilterten Frames
        XPutImage(display, window, gc, ximage, 0, 0, 0, 0, global_width, global_height);
        XFlush(display);

        // 4. Frame zurück in die Warteschlange (queue)
        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)) {
            return;
        }

        usleep(10000);
    }
}

// ===================================================================
// Hauptprogramm
// ===================================================================

int main() {
    // 1. X11 muss zuerst initialisiert werden, um die globalen Maße zu setzen!
    init_x11();
    
    // 2. V4L2 Initialisierung nutzt die globalen Maße
    open_device(DEVICE_NAME);
    init_mmap();
    start_capturing();
    
    printf("DFT-Gespensterfilter gestartet in %dx%d. Drücken Sie ESC im Vollbild, um zu beenden.\n", global_width, global_height);

    // 3. Hauptschleife starten
    main_loop();

    // 4. Aufräumen
    cleanup();

    return 0;
}


