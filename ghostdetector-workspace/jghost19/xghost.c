// gcc -O2 jghost.c -o jghost -lX11 -lm -lpthread
#define _GNU_SOURCE
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- Globale Konstanten und Variablen ---

#define NUM_THREADS 8
#define two_pi 6.283185307179586

static int current_win_W = 640;
static int current_win_H = 480;

// --- Strukturen ---

typedef struct { void *start; size_t length; } Buffer;

typedef struct {
    unsigned short h; // 0..65535 -> 0..360°
    unsigned short s; // 0..65535 -> 0..1
    unsigned short v; // 0..65535 -> 0..1
    unsigned short a; // 0..65535 alpha
} HSV64;

typedef struct {
    // Basis-Gains (werden optimiert)
    double hue_base;
    double sat_base;
    double val_base;
    double tint_base;

    // Dither-Amplituden und -Frequenzen (sinusförmig)
    double hue_amp, hue_freq;
    double sat_amp, sat_freq;
    double val_amp, val_freq;
    double tint_amp, tint_freq;

    // Zielwerte für mittlere Sättigung und Helligkeit
    double target_sat;  // 0..65535
    double target_val;  // 0..65535

    // Laufzeit-Metriken aus dem Rendering (Main-Thread schreibt)
    double avg_sat;     // Frame-Mittelwert (0..65535)
    double avg_val;     // Frame-Mittelwert (0..65535)

    // Kostenfunktion (Optimizer-Thread liest/schreibt)
    double J;           // w_sat*(avg_sat-target_sat)^2 + w_val*(avg_val-target_val)^2

    // Laufstatus & Synchronisation
    int running;
    pthread_mutex_t mtx;
} OptShared;

// Struktur für die Übergabe von Daten an jeden Filter-Worker-Thread
typedef struct {
    const unsigned char *srcRGB; // Quelle (V4L2 RGB)
    int srcW, srcH;
    unsigned int *dstARGB;       // Ziel (Fenster ARGB)
    int dstW, dstH;
    double t;                    // Aktuelle Zeit
    OptShared *os;               // Optimierungsparameter
    int layer_g;                 // Layer-Index (0 bis layers-1)
    int start_y;                 // Startzeile für diesen Thread
    int end_y;                   // Endzeile (exklusiv) für diesen Thread
    HSV64 *acc_chunk;            // Akkumulator-Speicher
    double thread_sum_sat;       // Ergebnis: Sättigungs-Summe
    double thread_sum_val;       // Ergebnis: Helligkeits-Summe
    // Für Kantendetektor:
    unsigned char *gray_image_chunk; // Graustufenbild-Teil
    unsigned char *edge_image_chunk; // Kantenbild-Teil
    unsigned char *prev_edge_image_chunk; // Vorheriges Kantenbild-Teil
    unsigned int *dst_argb_chunk;       // ARGB-Zielpuffer für Kantendetektor
} ThreadData; // Umbenannt, da es jetzt auch für Kantendetektion verwendet wird

// --- Funktionen: Hilfsfunktionen & Initialisierung ---

static int clampi(int v, int lo, int hi){ return v<lo?lo:(v>hi?hi:v); }
static unsigned int pack_argb(unsigned char a, unsigned char r, unsigned char g, unsigned char b){
    return ((unsigned int)a<<24)|((unsigned int)r<<16)|((unsigned int)g<<8)|((unsigned int)b);
}

static void optshared_init(OptShared *os) {
    // STARTWERTE UND ZIELWERTE FÜR KONTROLLIERTEN VISUELLEN EFFEKT ÜBERARBEITET
    
    // Basis-Gains (werden optimiert)
    os->hue_base  = 8.0;
    os->sat_base  = 1000.0;    // Gedämpft
    os->val_base  = 35000.0;   // Stark gedämpfte Basishelligkeit
    os->tint_base = 0.2;       // Gedämpfte Tint-Transparenz

    // Dither: kleine sinusförmige Anregung mit verschiedenen Frequenzen
    os->hue_amp  = 2.0;    os->hue_freq  = 0.9;    // Hz
    os->sat_amp  = 500.0; os->sat_freq  = 0.7;
    os->val_amp  = 500.0; os->val_freq  = 0.5;
    os->tint_amp = 0.2;  os->tint_freq = 0.75;

    // Zielwerte (Target Values)
    os->target_sat = 12000.0; // Niedrigeres Sättigungsziel
    os->target_val = 45000.0; // Niedrigeres Helligkeitsziel
    
    // Mess- und Zustandsgrößen
    os->avg_sat = 0.0;
    os->avg_val = 0.0;
    os->J = 0.0;

    os->running = 1;
    pthread_mutex_init(&os->mtx, NULL);
}

static void yuyv_to_rgb_row(unsigned char *yuyv, int width, unsigned char *rgb) {
    for (int x = 0; x < width; x += 2) {
        int y0 = yuyv[0], u = yuyv[1], y1 = yuyv[2], v = yuyv[3];
        yuyv += 4;
        int uu = u - 128, vv = v - 128;

        int r0 = clampi((int)(y0 + 1.402 * vv), 0, 255);
        int g0 = clampi((int)(y0 - 0.344136 * uu - 0.714136 * vv), 0, 255);
        int b0 = clampi((int)(y0 + 1.772 * uu), 0, 255);

        int r1 = clampi((int)(y1 + 1.402 * vv), 0, 255);
        int g1 = clampi((int)(y1 - 0.344136 * uu - 0.714136 * vv), 0, 255);
        int b1 = clampi((int)(y1 + 1.772 * uu), 0, 255);

        rgb[0]=r0; rgb[1]=g0; rgb[2]=b0; rgb+=3;
        rgb[0]=r1; rgb[1]=g1; rgb[2]=b1; rgb+=3;
    }
}

static HSV64 rgb8_to_hsv64(unsigned char r8, unsigned char g8, unsigned char b8) {
    double r = r8/255.0, g = g8/255.0, b = b8/255.0;
    double max = fmax(r, fmax(g, b)), min = fmin(r, fmin(g, b));
    double delta = max - min;
    double h = 0.0, s = 0.0, v = max;
    if (delta > 1e-12) {
        s = (max <= 0.0) ? 0.0 : delta / max;
        if (max == r) h = fmod(((g - b) / delta), 6.0);
        else if (max == g) h = ((b - r) / delta) + 2.0;
        else h = ((r - g) / delta) + 4.0;
        h *= 60.0;
        if (h < 0.0) h += 360.0;
    } else { s = 0.0; h = 0.0; }
    HSV64 out;
    out.h = (unsigned short)clampi((int)llround(h * (65535.0/360.0)), 0, 65535);
    out.s = (unsigned short)clampi((int)llround(s * 65535.0), 0, 65535);
    out.v = (unsigned short)clampi((int)llround(v * 65535.0), 0, 65535);
    out.a = 65535;
    return out;
}

static void hsv64_to_rgb8(HSV64 hsv, unsigned char *r8, unsigned char *g8, unsigned char *b8) {
    double h = (double)hsv.h * (360.0/65535.0);
    double s = (double)hsv.s / 65535.0;
    double v = (double)hsv.v / 65535.0;

    if (s <= 1e-12) {
        unsigned char val = (unsigned char)clampi((int)llround(v * 255.0), 0, 255);
        *r8 = val; *g8 = val; *b8 = val; return;
    }
    double c = v * s, hh = h / 60.0;
    double x = c * (1.0 - fabs(fmod(hh, 2.0) - 1.0));
    double m = v - c;
    double r=0,g=0,b=0;
    int region = (int)floor(hh);
    switch (region) {
        case 0: r=c; g=x; b=0; break;
        case 1: r=x; g=c; b=0; break;
        case 2: r=0; g=c; b=x; break;
        case 3: r=0; g=x; b=c; break;
        case 4: r=x; g=0; b=c; break;
        default: r=c; g=0; b=x; break;
    }
    *r8 = (unsigned char)clampi((int)llround((r+m) * 255.0), 0, 255);
    *g8 = (unsigned char)clampi((int)llround((g+m) * 255.0), 0, 255);
    *b8 = (unsigned char)clampi((int)llround((b+m) * 255.0), 0, 255);
}

static HSV64 sample_rgb_to_hsv64_bilinear_scaled(const unsigned char *src, int srcW, int srcH, int dstW, int dstH, int dst_x, int dst_y) {
    // Berechne die entsprechenden Koordinaten im Quellbild (src)
    double xf = (double)dst_x * (double)srcW / (double)dstW;
    double yf = (double)dst_y * (double)srcH / (double)dstH;

    if (xf < 0) xf = 0; if (yf < 0) yf = 0;
    if (xf > srcW-1) xf = srcW-1; if (yf > srcH-1) yf = srcH-1;
    int x0 = (int)floor(xf), y0 = (int)floor(yf);
    int x1 = clampi(x0+1, 0, srcW-1), y1 = clampi(y0+1, 0, srcH-1);
    double tx = xf - x0, ty = yf - y0;

    const unsigned char *p00 = src + (y0*srcW + x0)*3;
    const unsigned char *p10 = src + (y0*srcW + x1)*3;
    const unsigned char *p01 = src + (y1*srcW + x0)*3;
    const unsigned char *p11 = src + (y1*srcW + x1)*3;

    // Bilineare Interpolation der RGB-Kanäle
    double r = (1-tx)*(1-ty)*p00[0] + tx*(1-ty)*p10[0] + (1-tx)*ty*p01[0] + tx*ty*p11[0];
    double g = (1-tx)*(1-ty)*p00[1] + tx*(1-ty)*p10[1] + (1-tx)*ty*p01[1] + tx*ty*p11[1];
    double b = (1-tx)*(1-ty)*p00[2] + tx*(1-ty)*p10[2] + (1-tx)*ty*p01[2] + tx*ty*p11[2];

    return rgb8_to_hsv64((unsigned char)r, (unsigned char)g, (unsigned char)b);
}

static HSV64 hsv64_blend_add(HSV64 dst, HSV64 src, unsigned short alpha /*0..65535*/) {
    int dh = (int)src.h - (int)dst.h;
    if (dh > 32768) dh -= 65536;
    if (dh < -32768) dh += 65536;
    int h = dst.h + (int)((alpha / 65535.0) * dh);
    if (h < 0) h += 65536; if (h > 65535) h -= 65536;

    int s = clampi((int)dst.s + (int)((alpha / 65535.0) * src.s), 0, 65535);
    int v = clampi((int)dst.v + (int)((alpha / 65535.0) * src.v), 0, 65535);
    HSV64 out = { (unsigned short)h, (unsigned short)s, (unsigned short)v, 65535 };
    return out;
}

// Funktion zum Mischen einer Farbe mit Alpha in den ARGB-Puffer
static void blend_argb_with_alpha(unsigned int *dst, int index, unsigned char r_src, unsigned char g_src, unsigned char b_src, unsigned char alpha_src) {
    unsigned int current_pixel = dst[index];
    unsigned char a_dst = (current_pixel >> 24) & 0xFF;
    unsigned char r_dst = (current_pixel >> 16) & 0xFF;
    unsigned char g_dst = (current_pixel >> 8) & 0xFF;
    unsigned char b_dst = current_pixel & 0xFF;

    if (alpha_src == 0) { // Nichts zu tun
        return;
    } else if (alpha_src == 255) { // Volle Deckkraft
        dst[index] = pack_argb(a_dst, r_src, g_src, b_src);
    } else { // Alpha-Mischung
        double alpha_norm = alpha_src / 255.0;
        double inv_alpha_norm = 1.0 - alpha_norm;

        r_dst = (unsigned char)clampi((int)(r_src * alpha_norm + r_dst * inv_alpha_norm), 0, 255);
        g_dst = (unsigned char)clampi((int)(g_src * alpha_norm + g_dst * inv_alpha_norm), 0, 255);
        b_dst = (unsigned char)clampi((int)(b_src * alpha_norm + b_dst * inv_alpha_norm), 0, 255);
        
        // Die Alpha des Zielpixels bleibt erhalten oder wird angepasst, je nach gewünschtem Verhalten.
        // Hier lassen wir die Alpha des Zielpixels unverändert, wenn wir nur RGB-Werte mischen.
        dst[index] = pack_argb(a_dst, r_dst, g_dst, b_dst);
    }
}


// --- Funktionen: Thread-Worker ---

// Worker für die Konvertierung des finalen HSV-Akkumulators in ARGB
static void* hsv_to_argb_converter_thread(void *arg) {
    ThreadData *data = (ThreadData*)arg;
    int dstW = data->dstW;
    
    for (int y = data->start_y; y < data->end_y; ++y) {
        for (int x = 0; x < dstW; ++x) {
            int i = y * dstW + x;
            unsigned char r, g, b; 
            hsv64_to_rgb8(data->acc_chunk[i], &r, &g, &b);
            data->dstARGB[i] = pack_argb(0xFF, r, g, b);
        }
    }
    return NULL;
}

// Worker für die parallele Bearbeitung eines Bildstreifens (Ghost Sine Filter)
static void* filter_worker_thread(void *arg) {
    ThreadData *data = (ThreadData*)arg;

    const unsigned char *srcRGB = data->srcRGB;
    int srcW = data->srcW, srcH = data->srcH;
    int dstW = data->dstW, dstH = data->dstH;
    double t = data->t;
    OptShared *os = data->os;
    
    const double V_SHIFT = 15000.0;
    int g = data->layer_g; // Layer-Index 

    data->thread_sum_sat = 0.0;
    data->thread_sum_val = 0.0;

    // Hole ESC-Gains (Thread-Safe Kopie)
    pthread_mutex_lock(&os->mtx);
    double hue_gain  = os->hue_base  + os->hue_amp  * sin(two_pi * os->hue_freq  * t);
    double sat_gain  = os->sat_base  + os->sat_amp  * sin(two_pi * os->sat_freq  * t);
    double val_gain  = os->val_base  + os->val_amp  * sin(two_pi * os->val_freq  * t);
    double tint_alpha= os->tint_base + os->tint_amp * sin(two_pi * os->tint_freq * t);
    pthread_mutex_unlock(&os->mtx);
    
    // Layer-spezifische Parameter
    double phase = t * (0.6 + 0.2 * sin(t + g));
    double freqx = 1.2 + 0.2 * g;
    double freqy = 0.8 + 0.2 * g;

    double h_drift_deg = 12.0 * sin(0.7 * t + 0.5 * g);
    unsigned short h_tint = (unsigned short)(( (220.0 + h_drift_deg) * (65535.0/360.0) ));
    unsigned short s_tint = (unsigned short)(8000 + 3000 * fabs(sin(0.4 * t + g)));
    unsigned short v_tint = (unsigned short)(3000 + 3000 * fabs(sin(0.5 * t + 0.7 * g)));

    // Alpha skaliert (weniger aggressiv als zuvor)
    double alpha_scale = tint_alpha; if (alpha_scale < 0.1) alpha_scale = 0.1; if (alpha_scale > 0.8) alpha_scale = 0.8;
    unsigned short alpha = (unsigned short)clampi((int)llround((18000.0 + 12000.0 * fabs(sin(0.3 * t + g))) * alpha_scale), 0, 65535);

    // Iteration über die zugewiesenen Zeilen
    for (int y = data->start_y; y < data->end_y; ++y) {
        for (int x = 0; x < dstW; ++x) {
            double nx = (double)x / (double)dstW; // Normalisierte Koordinaten
            double ny = (double)y / (double)dstH;

            double s1 = sin(two_pi * (freqx * nx + phase));
            double s2 = sin(two_pi * (freqy * ny + 0.7 * phase + 0.3 * g));
            double s3 = sin(two_pi * (1.7 * freqx * nx + 1.0 * phase + 0.2 * g));
            double comb = s1 + 0.6 * s2 + 0.4 * s3;

            // Bild-Sample
            HSV64 sample = sample_rgb_to_hsv64_bilinear_scaled(srcRGB, srcW, srcH, dstW, dstH, x, y);

            // Modulation
            int h_mod = (int)sample.h + (int)llround((65535.0/360.0) * (hue_gain * comb));
            if (h_mod < 0) h_mod += 65536; if (h_mod > 65535) h_mod -= 65536;

            int s_mod = clampi((int)sample.s + (int)llround(sat_gain * comb), 0, 65535);
            int v_mod = clampi((int)sample.v + (int)llround(V_SHIFT + val_gain * sin(0.5 * t + ny * two_pi)), 0, 65535); 

            HSV64 mod = { (unsigned short)h_mod, (unsigned short)s_mod, (unsigned short)v_mod, 65535 };
            HSV64 tint = { h_tint, s_tint, v_tint, 65535 };

            // Akkumulation des Layer-Ergebnisses
            int acc_index = y*dstW + x;
            data->acc_chunk[acc_index] = hsv64_blend_add(data->acc_chunk[acc_index], mod, alpha);
            data->acc_chunk[acc_index] = hsv64_blend_add(data->acc_chunk[acc_index], tint, (unsigned short)llround(alpha * 0.4));

            data->thread_sum_sat += data->acc_chunk[acc_index].s;
            data->thread_sum_val += data->acc_chunk[acc_index].v;
        }
    }

    return NULL;
}

// --- Funktionen: Kantendetektion & Bewegungserkennung (NEU) ---

// Konvertiert RGB zu Graustufen (mittels Luminanz)
static unsigned char rgb_to_gray(unsigned char r, unsigned char g, unsigned char b) {
    return (unsigned char)clampi((int)(0.299 * r + 0.587 * g + 0.114 * b), 0, 255);
}

// Worker für die Graustufenkonvertierung (parallel)
static void* gray_converter_thread(void *arg) {
    ThreadData *data = (ThreadData*)arg;
    const unsigned char *srcRGB = data->srcRGB;
    unsigned char *gray_image = data->gray_image_chunk;
    int srcW = data->srcW;

    for (int y = data->start_y; y < data->end_y; ++y) {
        for (int x = 0; x < srcW; ++x) {
            int rgb_idx = (y * srcW + x) * 3;
            int gray_idx = y * srcW + x;
            gray_image[gray_idx] = rgb_to_gray(srcRGB[rgb_idx], srcRGB[rgb_idx+1], srcRGB[rgb_idx+2]);
        }
    }
    return NULL;
}


// Worker für den Sobel-Kantendetektor (parallel)
static void* sobel_worker_thread(void *arg) {
    ThreadData *data = (ThreadData*)arg;
    const unsigned char *gray_image = data->gray_image_chunk;
    unsigned char *edge_image = data->edge_image_chunk;
    int srcW = data->srcW, srcH = data->srcH;
    int start_y = data->start_y, end_y = data->end_y;

    // Sobel-Operatoren
    int sobel_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int sobel_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    
    // Für jeden Pixel im zugewiesenen Bereich (ohne Ränder)
    for (int y = start_y; y < end_y; ++y) {
        for (int x = 0; x < srcW; ++x) {
            // Randbehandlung: Kanten am Rand ignorieren
            if (x == 0 || x == srcW - 1 || y == 0 || y == srcH - 1) {
                edge_image[y * srcW + x] = 0;
                continue;
            }

            int Gx = 0;
            int Gy = 0;

            // Faltung mit Sobel-Operatoren
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    unsigned char pixel = gray_image[clampi(y + i, 0, srcH-1) * srcW + clampi(x + j, 0, srcW-1)];
                    Gx += pixel * sobel_x[i+1][j+1];
                    Gy += pixel * sobel_y[i+1][j+1];
                }
            }
            
            int magnitude = (int)sqrt((double)(Gx * Gx + Gy * Gy));
            edge_image[y * srcW + x] = (unsigned char)clampi(magnitude, 0, 255);
        }
    }
    return NULL;
}


// Worker für Bewegungserkennung und Zeichnen der Neon-Boxen (parallel)
static void* motion_and_neon_worker_thread(void *arg) {
    ThreadData *data = (ThreadData*)arg;
    unsigned char *edge_image = data->edge_image_chunk;
    unsigned char *prev_edge_image = data->prev_edge_image_chunk;
    unsigned int *dstARGB = data->dst_argb_chunk; // Direkt in den ARGB-Puffer zeichnen
    int srcW = data->srcW, srcH = data->srcH;
    int dstW = data->dstW, dstH = data->dstH;
    int start_y = data->start_y, end_y = data->end_y;

    const int edge_threshold = 40; // Schwelle für Kantendetektion
    const int motion_threshold = 30; // Schwelle für Bewegung (Differenz zwischen Kantenbildern)
    const int box_size = 5; // 5x5 Pixel Box
    const unsigned char neon_r = 0; // Neon Dark Blue
    const unsigned char neon_g = 100;
    const unsigned char neon_b = 255;
    const unsigned char neon_alpha = 25; // Alpha = 10% von 255

    for (int y = start_y; y < end_y; ++y) {
        for (int x = 0; x < srcW; ++x) {
            int current_idx = y * srcW + x;
            
            // 1. Kante vorhanden?
            if (edge_image[current_idx] > edge_threshold) {
                
                // 2. Bewegung vorhanden? (Nur vergleichen, wenn das vorherige Kantenbild existiert)
                if (prev_edge_image != NULL) {
                    // Absolute Differenz zwischen aktueller Kante und vorheriger Kante
                    int diff = abs((int)edge_image[current_idx] - (int)prev_edge_image[current_idx]);
                    
                    if (diff > motion_threshold) {
                        // "Gespenst" erkannt: Bewegte Kante -> Zeichne 5x5 Box
                        
                        // Konvertiere die V4L2-Koordinaten (srcW, srcH) auf die Fenster-Koordinaten (dstW, dstH)
                        // Vereinfachte Skalierung (Nearest Neighbor für Box-Position)
                        int dst_x = (int)round((double)x * dstW / srcW);
                        int dst_y = (int)round((double)y * dstH / srcH);
                        
                        // Zeichne 5x5 Box um dst_x, dst_y (zentriert)
                        int box_half = box_size / 2;
                        for (int by = -box_half; by <= box_half; ++by) {
                            for (int bx = -box_half; bx <= box_half; ++bx) {
                                int draw_x = clampi(dst_x + bx, 0, dstW - 1);
                                int draw_y = clampi(dst_y + by, 0, dstH - 1);
                                
                                int dst_idx = draw_y * dstW + draw_x;
                                
                                blend_argb_with_alpha(dstARGB, dst_idx, neon_r, neon_g, neon_b, neon_alpha);
                            }
                        }
                    }
                }
            }
        }
    }
    return NULL;
}



// --- Funktionen: Hauptlogik ---

static void ghost_sine_filter_hsv64(const unsigned char *srcRGB, int srcW, int srcH, unsigned int *dstARGB, int dstW, int dstH, double t, OptShared *os) {
    const int layers = 4;
    
    // Hintergrund
    HSV64 base = { (unsigned short)llround(220.0 * (65535.0/360.0)),
                   (unsigned short)(7000),
                   (unsigned short)(4000),
                   65535 };

    // Akkumulator-Speicher (HSV64)
    HSV64 *acc = (HSV64*)malloc(sizeof(HSV64) * dstW * dstH);
    if (!acc) { perror("malloc acc"); return; }
    for (int i = 0; i < dstW*dstH; ++i) acc[i] = base;

    double total_sum_sat = 0.0, total_sum_val = 0.0;
    
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    
    int rows_per_thread = dstH / NUM_THREADS;
    int remaining_rows = dstH % NUM_THREADS;

    int current_y; 

    // 1. Layer-Schleife (seriell)
    for (int g = 0; g < layers; ++g) {
        
        current_y = 0; 
        for (int i = 0; i < NUM_THREADS; ++i) {
            int block_size = rows_per_thread + (i < remaining_rows ? 1 : 0);
            
            // Verwende dieselbe ThreadData-Struktur, ignoriere Kanten-Felder
            thread_data[i] = (ThreadData){ 
                .srcRGB = srcRGB, .srcW = srcW, .srcH = srcH,
                .dstARGB = dstARGB, .dstW = dstW, .dstH = dstH,
                .t = t, .os = os, .layer_g = g,
                .start_y = current_y,
                .end_y = current_y + block_size,
                .acc_chunk = acc, 
                .thread_sum_sat = 0.0, .thread_sum_val = 0.0
            };
            
            pthread_create(&threads[i], NULL, filter_worker_thread, &thread_data[i]);
            current_y += block_size;
        }
        
        for (int i = 0; i < NUM_THREADS; ++i) {
            pthread_join(threads[i], NULL);
        }
    }
    
    // Summen-Akkumulation (nach allen Layern)
    for (int i = 0; i < NUM_THREADS; ++i) {
        total_sum_sat += thread_data[i].thread_sum_sat;
        total_sum_val += thread_data[i].thread_sum_val;
    }


    // 2. Konvertierung in ARGB (parallel)
    pthread_t convert_threads[NUM_THREADS];
    ThreadData convert_data[NUM_THREADS];
    
    current_y = 0;
    
    for (int i = 0; i < NUM_THREADS; ++i) {
        int block_size = rows_per_thread + (i < remaining_rows ? 1 : 0);
        
        convert_data[i] = (ThreadData){
            .dstARGB = dstARGB, .dstW = dstW, .dstH = dstH,
            .start_y = current_y,
            .end_y = current_y + block_size,
            .acc_chunk = acc,
        };
        
        pthread_create(&convert_threads[i], NULL, hsv_to_argb_converter_thread, &convert_data[i]);
        current_y += block_size;
    }
    
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(convert_threads[i], NULL);
    }
    
    // Metriken an Optimizer übergeben
    pthread_mutex_lock(&os->mtx);
    os->avg_sat = total_sum_sat / (double)(dstW * dstH * layers);
    os->avg_val = total_sum_val / (double)(dstW * dstH * layers);
    pthread_mutex_unlock(&os->mtx);

    free(acc);
}

// ESC-Optimizer-Thread
static void* optimizer_thread(void *arg) {
    OptShared *os = (OptShared*)arg;

    // Gewichte der Kosten
    const double w_sat = 1.0, w_val = 1.0;

    // Lock-in-Demodulation (Gradientenschätzung) mit Low-Pass Filter (EMA)
    double g_hue = 0.0, g_sat = 0.0, g_val = 0.0, g_tint = 0.0;
    const double ema_alpha = 0.0002; // LPF-Koeffizient für Demodulation

    // Lernraten
    const double lr_hue  = 5e-3;
    const double lr_sat  = 5e-4;
    const double lr_val  = 5e-4;
    const double lr_tint = 1e-3;

    // Grenzen (ANPASSUNG AN NEUE BASISWERTE)
    const double hue_min = 40.0,  hue_max = 80.0;
    const double sat_min = 500.0,  sat_max = 2000.0; // Enge Grenzen um den neuen Basiswert
    const double val_min = 30000.0, val_max = 40000.0; // Enge Grenzen um den neuen Basiswert
    const double tint_min= 0.1,    tint_max= 0.4; // Enge Grenzen um den neuen Basiswert

    // Zeitbasis
    struct timespec ts = {0, 10 * 1000 * 1000}; // 100 Hz Update
    double t = 0.0;
    const double dt = 0.01; 

    while (1) {
        // Hole Metriken & Ziel
        pthread_mutex_lock(&os->mtx);
        int running = os->running;
        double avg_sat = os->avg_sat;
        double avg_val = os->avg_val;
        double target_sat = os->target_sat;
        double target_val = os->target_val;

        // Kostenfunktion
        double e_sat = avg_sat - target_sat;
        double e_val = avg_val - target_val;
        os->J = w_sat * e_sat * e_sat + w_val * e_val * e_val;

        // Aktuelle Dither-Signale
        double s_hue  = sin(two_pi * os->hue_freq  * t);
        double s_sat  = sin(two_pi * os->sat_freq  * t);
        double s_val  = sin(two_pi * os->val_freq  * t);
        double s_tint = sin(two_pi * os->tint_freq * t);

        // Lock-in Demodulation: J * sin(...) -> Gradient-Schätzung (EMA Low-Pass)
        g_hue  = (1.0 - ema_alpha) * g_hue  + ema_alpha * (os->J * s_hue);
        g_sat  = (1.0 - ema_alpha) * g_sat  + ema_alpha * (os->J * s_sat);
        g_val  = (1.0 - ema_alpha) * g_val  + ema_alpha * (os->J * s_val);
        g_tint = (1.0 - ema_alpha) * g_tint + ema_alpha * (os->J * s_tint);

        // Gradient-Abstieg auf Basis-Gains
        os->hue_base  -= lr_hue  * g_hue;
        os->sat_base  -= lr_sat  * g_sat;
        os->val_base  -= lr_val  * g_val;
        os->tint_base -= lr_tint * g_tint;

        // Projektions-Grenzen
        os->hue_base  = clampi((int)os->hue_base,  (int)hue_min,  (int)hue_max);
        os->sat_base  = clampi((int)os->sat_base,  (int)sat_min,  (int)sat_max);
        os->val_base  = clampi((int)os->val_base,  (int)val_min,  (int)val_max);
        os->tint_base = clampi((int)llround(os->tint_base*100), (int)llround(tint_min*100), (int)llround(tint_max*100))/100.0;
        
        pthread_mutex_unlock(&os->mtx);

        if (!running) break;
        nanosleep(&ts, NULL);
        t += dt;
    }
    return NULL;
}

// Hilfsfunktion zum Maximieren des Fensters (X11 spezifisch)
static void maximize_window(Display *dpy, Window win) {
    Atom wm_state = XInternAtom(dpy, "_NET_WM_STATE", False);
    Atom wm_state_maximized_horz = XInternAtom(dpy, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
    Atom wm_state_maximized_vert = XInternAtom(dpy, "_NET_WM_STATE_MAXIMIZED_VERT", False);

    if (wm_state != None && wm_state_maximized_horz != None && wm_state_maximized_vert != None) {
        XClientMessageEvent xclient;
        memset(&xclient, 0, sizeof(xclient));
        xclient.type = ClientMessage;
        xclient.window = win;
        xclient.message_type = wm_state;
        xclient.format = 32;
        xclient.data.l[0] = 1; // 1 = _NET_WM_STATE_ADD
        xclient.data.l[1] = wm_state_maximized_horz;
        xclient.data.l[2] = wm_state_maximized_vert;
        xclient.data.l[3] = 0;
        xclient.data.l[4] = 0;
        XSendEvent(dpy, DefaultRootWindow(dpy), False, SubstructureNotifyMask | SubstructureRedirectMask, (XEvent *)&xclient);
    }
}

int main(void) {
    // Ursprüngliche V4L2-Parameter (fest)
    const int V4L2_W = 640, V4L2_H = 480;

    // --------- V4L2-Initialisierung ----------
    const char *dev = "/dev/video0";
    int fd = open(dev, O_RDWR | O_NONBLOCK, 0);
    if (fd < 0) { perror("open /dev/video0"); return 1; }

    struct v4l2_capability cap;
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) { perror("VIDIOC_QUERYCAP"); return 1; }

    struct v4l2_format fmt; memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = V4L2_W;
    fmt.fmt.pix.height = V4L2_H;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) { perror("VIDIOC_S_FMT"); return 1; }

    struct v4l2_requestbuffers req; memset(&req, 0, sizeof(req));
    req.count = 4; req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) { perror("VIDIOC_REQBUFS"); return 1; }
    if (req.count < 2) { fprintf(stderr, "Insufficient buffer memory\n"); return 1; }

    Buffer *buffers = calloc(req.count, sizeof(Buffer));
    for (unsigned int i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) { perror("VIDIOC_QUERYBUF"); return 1; }
        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[i].start == MAP_FAILED) { perror("mmap"); return 1; }
    }
    for (unsigned int i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); return 1; }
    }
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) { perror("VIDIOC_STREAMON"); return 1; }

    // --------- X11-Initialisierung ----------
    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) { fprintf(stderr, "XOpenDisplay failed\n"); return 1; }
    int scr = DefaultScreen(dpy);
    current_win_W = V4L2_W;
    current_win_H = V4L2_H;
    Window win = XCreateSimpleWindow(dpy, RootWindow(dpy, scr), 100, 100, current_win_W, current_win_H, 1,
                                     BlackPixel(dpy, scr), WhitePixel(dpy, scr));
    XStoreName(dpy, win, "Live /dev/video0 + Ghost Sine + Neon Ghost Boxes");
    XSelectInput(dpy, win, ExposureMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(dpy, win);
    maximize_window(dpy, win);

    GC gc = XCreateGC(dpy, win, 0, NULL);

    XImage *xi = XCreateImage(dpy, DefaultVisual(dpy, scr), 24, ZPixmap, 0,
                              malloc(current_win_W*current_win_H*4), current_win_W, current_win_H, 32, current_win_W*4);
    if (!xi || !xi->data) { fprintf(stderr, "XCreateImage failed\n"); return 1; }

    unsigned char *rgb = malloc(V4L2_W*V4L2_H*3);
    unsigned char *prev_rgb = malloc(V4L2_W*V4L2_H*3); // Buffer für den vorherigen Frame
    unsigned char *gray = malloc(V4L2_W*V4L2_H); // Graustufenbild
    unsigned char *edges = malloc(V4L2_W*V4L2_H); // Kantenbild
    unsigned char *prev_edges = calloc(V4L2_W*V4L2_H, 1); // Vorheriges Kantenbild (mit 0 initialisiert)
    unsigned int *argb = (unsigned int*)xi->data;
    
    // Initialisiere prev_rgb mit 0, um am Anfang einen großen Unterschied zu vermeiden
    memset(prev_rgb, 0, V4L2_W*V4L2_H*3); 

    struct timespec ts = {0, 16 * 1000 * 1000}; // ~60 FPS
    double t = 0.0, dt = 0.016;
    int running = 1;
    
    int rows_per_thread = V4L2_H / NUM_THREADS;
    int remaining_rows = V4L2_H % NUM_THREADS;
    
    pthread_t thread_pool[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    // --------- ESC-Optimierungsthread starten ----------
    OptShared os; optshared_init(&os);
    pthread_t opt_th;
    if (pthread_create(&opt_th, NULL, optimizer_thread, &os) != 0) {
        fprintf(stderr, "pthread_create failed\n"); return 1;
    }

    // --------- Haupt-Loop ----------
    while (running) {
        while (XPending(dpy)) {
            XEvent e; XNextEvent(dpy, &e);
            if (e.type == KeyPress) {
                if (XLookupKeysym(&e.xkey, 0) == XK_Escape) {
                    running = 0;
                }
            } else if (e.type == ConfigureNotify) {
                XConfigureEvent *c_e = &e.xconfigure;
                if (current_win_W != c_e->width || current_win_H != c_e->height) {
                    current_win_W = c_e->width;
                    current_win_H = c_e->height;

                    // XImage neu erstellen und alten Speicher freigeben (Double Free Bug Fix)
                    if (xi && xi->data) {
                        free(xi->data); 
                        xi->data = NULL; // Verhindert Double Free durch XDestroyImage
                    }
                    if (xi) XDestroyImage(xi);
                    xi = NULL; 

                    xi = XCreateImage(dpy, DefaultVisual(dpy, scr), 24, ZPixmap, 0,
                                      malloc(current_win_W*current_win_H*4), current_win_W, current_win_H, 32, current_win_W*4);
                    argb = (unsigned int*)xi->data; 

                    if (!xi || !xi->data) { fprintf(stderr, "XCreateImage failed (resize)\n"); running = 0; }
                }
            }
        }

        struct v4l2_buffer buf; memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP;
        if (ioctl(fd, VIDIOC_DQBUF, &buf) == 0) {
            unsigned char *yuyv = (unsigned char *)buffers[buf.index].start;
            
            // YUYV -> RGB (seriell) in den aktuellen Puffer (rgb)
            for (int y = 0; y < V4L2_H; ++y)
                yuyv_to_rgb_row(yuyv + y * (V4L2_W*2), V4L2_W, rgb + y * (V4L2_W*3));

            // --- 1. KANTEN & BEWEGUNGSDETEKTION (Paralleler Prozess) ---
            int current_y = 0;
            // 1a. RGB zu Graustufen
            for (int i = 0; i < NUM_THREADS; ++i) {
                int block_size = rows_per_thread + (i < remaining_rows ? 1 : 0);
                thread_data[i] = (ThreadData){
                    .srcRGB = rgb, .srcW = V4L2_W, .srcH = V4L2_H,
                    .start_y = current_y, .end_y = current_y + block_size,
                    .gray_image_chunk = gray // Ziel-Graustufenpuffer
                };
                pthread_create(&thread_pool[i], NULL, gray_converter_thread, &thread_data[i]);
                current_y += block_size;
            }
            for (int i = 0; i < NUM_THREADS; ++i) pthread_join(thread_pool[i], NULL);
            
            current_y = 0;
            // 1b. Sobel-Kantendetektion
            for (int i = 0; i < NUM_THREADS; ++i) {
                int block_size = rows_per_thread + (i < remaining_rows ? 1 : 0);
                thread_data[i] = (ThreadData){
                    .srcW = V4L2_W, .srcH = V4L2_H,
                    .start_y = current_y, .end_y = current_y + block_size,
                    .gray_image_chunk = gray, // Quelle: Graustufen
                    .edge_image_chunk = edges // Ziel: Kantenbild
                };
                pthread_create(&thread_pool[i], NULL, sobel_worker_thread, &thread_data[i]);
                current_y += block_size;
            }
            for (int i = 0; i < NUM_THREADS; ++i) pthread_join(thread_pool[i], NULL);

            // NOTE: Hier rufen wir ZUERST den motion_and_neon_worker auf, um die Boxen zu zeichnen.
            // Dann rufen wir den ghost_sine_filter auf, der das gesamte Bild mit dem Ghost-Effekt überblendet.
            // Dies erzeugt den Effekt, dass die Neon-Boxen Teil des Hintergrunds/der Leinwand sind.
            
            current_y = 0;
            // 1c. Bewegung erkennen und Neon-Boxen zeichnen (direkt in ARGB)
            for (int i = 0; i < NUM_THREADS; ++i) {
                int block_size = rows_per_thread + (i < remaining_rows ? 1 : 0);
                thread_data[i] = (ThreadData){
                    .srcW = V4L2_W, .srcH = V4L2_H, .dstW = current_win_W, .dstH = current_win_H,
                    .start_y = current_y, .end_y = current_y + block_size,
                    .edge_image_chunk = edges,          // Aktuelles Kantenbild
                    .prev_edge_image_chunk = prev_edges, // Vorheriges Kantenbild
                    .dst_argb_chunk = argb            // Ziel: ARGB Puffer
                };
                pthread_create(&thread_pool[i], NULL, motion_and_neon_worker_thread, &thread_data[i]);
                current_y += block_size;
            }
            for (int i = 0; i < NUM_THREADS; ++i) pthread_join(thread_pool[i], NULL);
            
            // --- 2. GHOST SINE FILTER ---
            // Dieser Filter verwendet das *aktuelle* RGB-Signal und *überblendet* den ARGB-Puffer.
            ghost_sine_filter_hsv64(rgb, V4L2_W, V4L2_H, argb, current_win_W, current_win_H, t, &os);
            
            // --- 3. AUFRÄUMEN FÜR NÄCHSTEN FRAME ---
            // Kopiere den aktuellen Kantenpuffer in den vorherigen Kantenpuffer
            memcpy(prev_edges, edges, V4L2_W * V4L2_H);
            // Kopiere das aktuelle RGB-Bild in den vorherigen RGB-Puffer
            memcpy(prev_rgb, rgb, V4L2_W * V4L2_H * 3);


            // XPutImage
            XPutImage(dpy, win, gc, xi, 0, 0, 0, 0, current_win_W, current_win_H);
            XFlush(dpy);

            if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF (requeue)"); break; }

            t += dt; nanosleep(&ts, NULL);
        } else {
            struct timespec ts2 = {0, 5 * 1000 * 1000};
            nanosleep(&ts2, NULL);
        }
    }

    // --------- Aufräumen und Thread stoppen ----------
    pthread_mutex_lock(&os.mtx);
    os.running = 0;
    pthread_mutex_unlock(&os.mtx);
    pthread_join(opt_th, NULL);

    // Saubere Freigabe des XImage-Speichers
    if (xi && xi->data) { free(xi->data); xi->data = NULL; }
    if (xi) XDestroyImage(xi);
    
    XFreeGC(dpy, gc);
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd, VIDIOC_STREAMOFF, &type);
    for (unsigned int i = 0; i < req.count; ++i) {
        if (buffers[i].start && buffers[i].start != MAP_FAILED)
            munmap(buffers[i].start, buffers[i].length);
    }
    free(buffers); 
    free(rgb); free(prev_rgb); 
    free(gray); free(edges); free(prev_edges);
    close(fd);
    return 0;
}
