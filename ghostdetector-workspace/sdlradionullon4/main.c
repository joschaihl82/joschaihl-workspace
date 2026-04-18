#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <time.h> 

// Linux V4L2 Includes
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

// --- Statische Konfiguration (bleibt konstant) ---
#define DEVICE_NODE "/dev/video0"
#define IMG_W 640
#define IMG_H 480
#define PIXEL_SIZE (IMG_W * IMG_H)

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// =================================================================
// 0. GLOBALE, VARIABLE KONSTANTEN ZUR LAUFZEITEVALUIERUNG (g_XXX)
// =================================================================

// Allgemeine Filter-Schwellenwerte
float g_RESIDUE_THRESH = 15.0f;
int g_HEATMAP_ALPHA = 155;
int g_RADIONULLON_THRESH_MIN = 30; 

// Dynamische Schwellenwert-Konstanten
int g_THRESH_UPDATE_FRAMES = 1800; 
float g_BASELINE_EMA_ALPHA = 0.01f;   
float g_THRESH_OFFSET_FACTOR = 1.5f;  

// Morphing- und Geschwindigkeits-Parameter (60s+)
float g_BRIGHTNESS_LIMIT_HIGH = 200.0f;
float g_MINIMUM_BG_LEARN_RATE = 0.00005f;   
float g_BG_RATE_SLOW = 0.00005f;           
float g_BG_RATE_FAST = 0.0001f;            

float g_INITIAL_MAX_LEARN_RATE = 0.005f;    
int g_DAMPING_DURATION_FRAMES = 1800;     

// Flicker-Parameter
int g_FLICKER_DECAY_RATE = 10;       
int g_INITIAL_FLICKER_BOOST = 25;    

// Extreme Bildverarbeitung (Blur/Boost)
int g_BLUR_KERNEL_RADIUS = 2;        
float g_GAUSSIAN_SIGMA = 5.0f;         
float g_BRIGHTNESS_BOOST_GAIN = 2.5f;  
float g_CONTRAST_BOOST_FACTOR = 1.5f;  
float g_EDGE_GAIN_MIN = 1.0f;           
float g_EDGE_GAIN_MAX = 2.0f;           

// Audit-Kreis Parameter
int g_AUDIT_CIRCLE_RADIUS = 60; 
int g_AUDIT_CIRCLE_CENTER_X; 
int g_AUDIT_CIRCLE_CENTER_Y; 

// Audit-Werte
int g_AUDIT_SWITCH_INTERVAL = 150;    
float g_AUDIT_LOW_VAL = 20.0f;          
float g_AUDIT_HIGH_VAL = 200.0f;        

// NEUE FARBWERTE FÜR HELL-ZU-DUNKEL VERLAUF (werden für Skin-Contour genutzt):
// innen (hell, hautähnlich)
int g_SKIN_CENTER_R = 245;
int g_SKIN_CENTER_G = 220;
int g_SKIN_CENTER_B = 192;

// außen (dunkler, konturartig)
int g_SKIN_CONTOUR_R = 120;
int g_SKIN_CONTOUR_G = 70;
int g_SKIN_CONTOUR_B = 40;

// Parameter für Rosa Rauschen
float g_NOISE_HUE_MIN = 300.0f; 
float g_NOISE_HUE_MAX = 330.0f; 
float g_NOISE_SATURATION_MIN = 0.5f; 
float g_NOISE_SATURATION_MAX = 1.0f;
float g_NOISE_VALUE_MIN = 0.9f;     
float g_NOISE_VALUE_MAX = 1.0f;
int g_NOISE_ALPHA = 120;            

// Stärke der Konturblendung (0-255)
int g_SKIN_CONTOUR_ALPHA = 200;

// --- Globale Zustände für Radionullon-Filter (Persistent) ---
float bg[PIXEL_SIZE];
unsigned char flicker[PIXEL_SIZE];
float prevGlobalAvg = 128.0f;
double currentRadSmooth = 0.0;
double totalDoseAcc = 0.0;
int flicker_decay_counter = 0;

// Dynamische Parameter und Zähler
int dynamic_radionullon_thresh = 30; 
double baseline_score_ema = 0.0;
int frame_counter = 0;
unsigned int frame_counter_total = 0; 
float audit_target_val = 200.0f; 
int audit_frame_counter = 0;             

// V4L2 Strukturen & Zustand
struct Buffer { void *start; size_t length; };
struct Buffer *buffers;
int n_buffers = 0;
int fd_v4l2 = -1;

// =================================================================
// 1. V4L2 CAPTURE & SETUP
// =================================================================

int v4l2_init() {
    fd_v4l2 = open(DEVICE_NODE, O_RDWR | O_NONBLOCK, 0);
    if (fd_v4l2 == -1) {
        perror("V4L2: Fehler beim Oeffnen des Geraets");
        return -1;
    }

    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = IMG_W;
    fmt.fmt.pix.height = IMG_H;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; 
    ioctl(fd_v4l2, VIDIOC_S_FMT, &fmt);

    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = 2;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    ioctl(fd_v4l2, VIDIOC_REQBUFS, &req);

    buffers = (struct Buffer*)calloc(req.count, sizeof(*buffers));
    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;
        ioctl(fd_v4l2, VIDIOC_QUERYBUF, &buf);
        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_v4l2, buf.m.offset);
    }
    
    for (int i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        ioctl(fd_v4l2, VIDIOC_QBUF, &buf);
    }
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd_v4l2, VIDIOC_STREAMON, &type);

    memset(bg, 0, sizeof(bg));
    memset(flicker, 0, sizeof(flicker));

    return 0;
}

unsigned char* v4l2_read_frame() {
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd_v4l2, VIDIOC_DQBUF, &buf) == -1) {
        return NULL;
    }

    unsigned char* frame_data = (unsigned char*)buffers[buf.index].start;
    
    ioctl(fd_v4l2, VIDIOC_QBUF, &buf);
    
    return frame_data;
}

void v4l2_cleanup() {
    if (fd_v4l2 != -1) {
        enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ioctl(fd_v4l2, VIDIOC_STREAMOFF, &type);
        for (int i = 0; i < n_buffers; ++i) {
            munmap(buffers[i].start, buffers[i].length);
        }
        free(buffers);
        close(fd_v4l2);
    }
}

// =================================================================
// 2. YUYV ZU RGB/GRAY KONVERTIERUNG
// =================================================================

void yuyv_to_rgb32_and_gray(const unsigned char* src_yuyv, unsigned int* dst_rgb32, unsigned char* dst_gray, float* calculated_globalAvg) {
    float temp_globalAvg = 0.0f;
    for (int y = 0; y < IMG_H; y++) {
        const unsigned char* srcLine = src_yuyv + (y * IMG_W * 2);
        unsigned int* dstRgbLine = dst_rgb32 + (y * IMG_W);
        unsigned char* dstGrayLine = dst_gray + (y * IMG_W);

        for (int x = 0; x < IMG_W; x += 2) {
            int Y1 = srcLine[0]; int U = srcLine[1]; int Y2 = srcLine[2]; int V = srcLine[3]; srcLine += 4;
            
            int C1 = Y1 - 16; int D = U - 128; int E = V - 128;
            int R1 = (298 * C1 + 409 * E + 128) >> 8;
            int G1 = (298 * C1 - 100 * D - 208 * E + 128) >> 8;
            int B1 = (298 * C1 + 516 * D + 128) >> 8;
            
            int C2 = Y2 - 16;
            int R2 = (298 * C2 + 409 * E + 128) >> 8;
            int G2 = (298 * C2 - 100 * D - 208 * E + 128) >> 8;
            int B2 = (298 * C2 + 516 * D + 128) >> 8;

            #define CLAMP(v) ((unsigned char)((v) > 255 ? 255 : ((v) < 0 ? 0 : (v))))

            unsigned char r1_c = CLAMP(R1); unsigned char g1_c = CLAMP(G1); unsigned char b1_c = CLAMP(B1);
            dstRgbLine[0] = (0xFF << 24) | (r1_c << 16) | (g1_c << 8) | b1_c;
            
            unsigned char r2_c = CLAMP(R2); unsigned char g2_c = CLAMP(G2); unsigned char b2_c = CLAMP(B2);
            dstRgbLine[1] = (0xFF << 24) | (r2_c << 16) | (g2_c << 8) | b2_c;
            
            dstRgbLine += 2;

            dstGrayLine[0] = CLAMP(Y1);
            dstGrayLine[1] = CLAMP(Y2);
            dstGrayLine += 2;
            
            temp_globalAvg += Y1;
            temp_globalAvg += Y2;
            #undef CLAMP
        }
    }
    *calculated_globalAvg = temp_globalAvg / PIXEL_SIZE;
}

// =================================================================
// 3. EXTREME BILDVERARBEITUNGS-HILFSFUNKTIONEN
// =================================================================

// Berechnet den Gauß-Kernel (Radius ist g_BLUR_KERNEL_RADIUS, 2r+1 x 2r+1)
void calculate_gaussian_kernel(float** kernel_ptr, int *kernel_size) {
    int r = g_BLUR_KERNEL_RADIUS;
    *kernel_size = 2 * r + 1;
    
    float* kernel = (float*)malloc(*kernel_size * *kernel_size * sizeof(float));
    if (!kernel) return;
    *kernel_ptr = kernel;

    float sigma = g_GAUSSIAN_SIGMA;
    float sum = 0.0f;
    
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            int idx = (y + r) * (*kernel_size) + (x + r);
            kernel[idx] = expf(-(float)(x * x + y * y) / (2.0f * sigma * sigma));
            sum += kernel[idx];
        }
    }

    for (int i = 0; i < (*kernel_size) * (*kernel_size); i++) {
        kernel[i] /= sum;
    }
}


// Führt extremen Blur, Boost und Kantenkontrast-Rekonstruktion durch
void apply_extreme_blur_and_boost(const unsigned char* src_gray, unsigned char* dst_soft_boosted, unsigned char* dst_sobel_edge) {
    int w = IMG_W;
    int h = IMG_H;
    
    // 1. Gauß-Weichzeichnung
    float* kernel_data = NULL;
    int kernel_size;
    calculate_gaussian_kernel(&kernel_data, &kernel_size);
    if (!kernel_data) return;

    float blurred_float[PIXEL_SIZE] = {0.0f};
    int r = g_BLUR_KERNEL_RADIUS; 

    for (int y = r; y < h - r; y++) {
        for (int x = r; x < w - r; x++) {
            float sum = 0.0f;
            for (int ky = -r; ky <= r; ky++) {
                for (int kx = -r; kx <= r; kx++) {
                    int kernel_idx = (ky + r) * kernel_size + (kx + r);
                    sum += (float)src_gray[(y + ky) * w + (x + kx)] * kernel_data[kernel_idx];
                }
            }
            blurred_float[y * w + x] = sum;
        }
    }
    
    free(kernel_data);


    // 2. Sobel Edge Detection (auf dem Originalbild)
    unsigned char sobel_mag[PIXEL_SIZE] = {0};
    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            int i = y * w + x;
            int gx = -src_gray[i - w - 1] - 2 * src_gray[i - 1] - src_gray[i + w - 1] +
                      src_gray[i - w + 1] + 2 * src_gray[i + 1] + src_gray[i + w + 1];
            
            int gy = -src_gray[i - w - 1] - 2 * src_gray[i - w] - src_gray[i - w + 1] +
                      src_gray[i + w - 1] + 2 * src_gray[i + w] + src_gray[i + w + 1];
                      
            int mag = (int)sqrt((double)gx * gx + (double)gy * gy);
            sobel_mag[i] = (unsigned char)(mag > 255 ? 255 : mag);
        }
    }

    // 3. Extremes Hochziehen und Kontrast-Rekonstruktion
    for (int i = 0; i < PIXEL_SIZE; i++) {
        float boosted = blurred_float[i] * g_BRIGHTNESS_BOOST_GAIN; 
        float edge_correction = (float)sobel_mag[i] * g_CONTRAST_BOOST_FACTOR;
        float final_value = boosted - edge_correction;

        dst_soft_boosted[i] = (unsigned char)(fmaxf(0.0f, fminf(255.0f, final_value)));
        
        dst_sobel_edge[i] = sobel_mag[i];
    }
}


// =================================================================
// 4. RADIONULLON FILTER (ULTRA-SLOW MORPHING MIT AUDITING)
// =================================================================

unsigned int blend_color(unsigned int original_rgb, unsigned char r_heat, unsigned char g_heat, unsigned char b_heat, int alpha) {
    unsigned char R0, G0, B0;
    
    R0 = (original_rgb >> 16) & 0xFF;
    G0 = (original_rgb >> 8) & 0xFF;
    B0 = original_rgb & 0xFF;

    unsigned char R_blend = (R0 * (255 - alpha) + r_heat * alpha) / 255;
    unsigned char G_blend = (G0 * (255 - alpha) + g_heat * alpha) / 255;
    unsigned char B_blend = (B0 * (255 - alpha) + b_heat * alpha) / 255;

    return (0xFF << 24) | (R_blend << 16) | (G_blend << 8) | B_blend;
}


// HSV zu RGB Konvertierung
void hsv_to_rgb(float h, float s, float v, unsigned char *r, unsigned char *g, unsigned char *b) {
    int i;
    float f, p, q, t;
    if (s == 0) {
        *r = *g = *b = (unsigned char)(v * 255.0f);
        return;
    }
    h /= 60.0f; 
    i = floor(h);
    f = h - i; 
    p = v * (1.0f - s);
    q = v * (1.0f - s * f);
    t = v * (1.0f - s * (1.0f - f));
    
    float rf = 0, gf = 0, bf = 0;

    switch (i) {
        case 0: rf = v; gf = t; bf = p; break;
        case 1: rf = q; gf = v; bf = p; break;
        case 2: rf = p; gf = v; bf = t; break;
        case 3: rf = p; gf = q; bf = v; break;
        case 4: rf = t; gf = p; bf = v; break;
        default: rf = v; gf = p; bf = q; break;
    }

    *r = (unsigned char)(rf * 255.0f);
    *g = (unsigned char)(gf * 255.0f);
    *b = (unsigned char)(bf * 255.0f);
}


void process_radionullon(unsigned int *output_pixels, const unsigned char* gray, float globalAvg) {
    int size = PIXEL_SIZE;
    int i; 
    double totalScore = 0.0;
    
    unsigned char soft_boosted_gray[PIXEL_SIZE];
    unsigned char sobel_edge_output[PIXEL_SIZE]; 

    // --- 4.1 FILTERSTUFE: EXTREMER BLUR, BOOST, KANTENKONTRAST ---
    apply_extreme_blur_and_boost(gray, soft_boosted_gray, sobel_edge_output);
    
    // --- 4.2 AUDITING-ALGORITHMUS: Zielwert setzen und auf den Kreis anwenden ---
    audit_frame_counter++;
    if (audit_frame_counter >= g_AUDIT_SWITCH_INTERVAL) {
        if (audit_target_val == g_AUDIT_HIGH_VAL) {
            audit_target_val = g_AUDIT_LOW_VAL;
        } else {
            audit_target_val = g_AUDIT_HIGH_VAL;
        }
        audit_frame_counter = 0;
    }
    
    // Audit-Signal auf den Kreis anwenden (grau)
    for (int y_offset = -g_AUDIT_CIRCLE_RADIUS; y_offset <= g_AUDIT_CIRCLE_RADIUS; y_offset++) {
        for (int x_offset = -g_AUDIT_CIRCLE_RADIUS; x_offset <= g_AUDIT_CIRCLE_RADIUS; x_offset++) {
            float dist_sq = (float)(x_offset * x_offset + y_offset * y_offset);
            if (dist_sq <= (g_AUDIT_CIRCLE_RADIUS * g_AUDIT_CIRCLE_RADIUS)) {
                int current_x = g_AUDIT_CIRCLE_CENTER_X + x_offset;
                int current_y = g_AUDIT_CIRCLE_CENTER_Y + y_offset;

                if (current_x >= 0 && current_x < IMG_W && current_y >= 0 && current_y < IMG_H) {
                    int current_audit_index = current_y * IMG_W + current_x;
                    soft_boosted_gray[current_audit_index] = (unsigned char)audit_target_val;
                }
            }
        }
    }
    const unsigned char* processed_gray = soft_boosted_gray;


    // --- 4.3 Berechnung dynamischer Parameter (MAXIMALE VERZÖGERUNG) ---
    float normalized_avg = fminf(globalAvg / g_BRIGHTNESS_LIMIT_HIGH, 1.0f); 

    frame_counter_total++;
    
    float dynamic_bg_learn_rate_base;

    dynamic_bg_learn_rate_base = g_BG_RATE_SLOW + (g_BG_RATE_FAST - g_BG_RATE_SLOW) * normalized_avg;
    
    float damping_factor = 0.0f;
    
    if (frame_counter_total < g_DAMPING_DURATION_FRAMES) { 
        
        float t_norm = ((float)frame_counter_total / g_DAMPING_DURATION_FRAMES) * (M_PI / 2.0f); 
        damping_factor = fmaxf(0.0f, cosf(t_norm));
    }
    
    float initial_boost_rate = g_INITIAL_MAX_LEARN_RATE * damping_factor;
    float dynamic_bg_learn_rate = dynamic_bg_learn_rate_base + initial_boost_rate;
                                  
    if (dynamic_bg_learn_rate < g_MINIMUM_BG_LEARN_RATE) {
        dynamic_bg_learn_rate = g_MINIMUM_BG_LEARN_RATE;
    }
    
    int dynamic_flicker_thresh = (int)(10.0 + (globalAvg / 15.0));
    if (dynamic_flicker_thresh > 50) dynamic_flicker_thresh = 50;

    float dynamic_edge_gain = g_EDGE_GAIN_MIN + (g_EDGE_GAIN_MAX - g_EDGE_GAIN_MIN) * normalized_avg;

    // --- 4.4 Filter-Pipeline ---
    
    unsigned char residue[size];
    unsigned char edge[size];
    
    // --- Hintergrundmodell (EMA) und Residue ---
    for (i = 0; i < size; i++) {
        bg[i] = (1.0f - dynamic_bg_learn_rate) * bg[i] + dynamic_bg_learn_rate * (float)processed_gray[i];
        
        int diff = abs((int)processed_gray[i] - (int)bg[i]);
        residue[i] = (diff > g_RESIDUE_THRESH) ? (unsigned char)(diff > 255 ? 255 : diff) : 0;
    }

    // --- Flicker-Maß ---
    unsigned char currentAvg = (unsigned char)(globalAvg > 255.0f ? 255 : (globalAvg < 0.0f ? 0 : globalAvg));
    int delta = abs((int)currentAvg - (int)prevGlobalAvg);
    prevGlobalAvg = currentAvg;
    
    unsigned char boost = (delta > dynamic_flicker_thresh) ? g_INITIAL_FLICKER_BOOST : 1; 

    for (i = 0; i < size; i++) {
        int new_flicker = (int)flicker[i] + boost;
        flicker[i] = (unsigned char)(new_flicker > 255 ? 255 : new_flicker);
    }
    
    // ULTRA-LANGSAMER ZERFALL
    flicker_decay_counter++;
    if (flicker_decay_counter >= g_FLICKER_DECAY_RATE) {
        for (i = 0; i < size; i++) {
            if (flicker[i] > 0) flicker[i] = (unsigned char)(flicker[i] - 1);
        }
        flicker_decay_counter = 0;
    }

    // --- Edge (Sobel) Zuweisung ---
    for (i = 0; i < size; i++) {
        int mag = (int)(dynamic_edge_gain * (float)sobel_edge_output[i]);
        edge[i] = (unsigned char)(mag > 255 ? 255 : mag);
    }

    // --- Heatmap Overlay und Radionullon-Analyse (außerhalb des Audit-Kreises) ---
    for (i = 0; i < size; i++) {
        int score = (int)residue[i] + (int)(edge[i] / 2) + (int)(flicker[i] / 3);
        totalScore += score;
        
        int px = i % IMG_W;
        int py = i / IMG_W;
        
        float dist_from_center_sq = (float)((px - g_AUDIT_CIRCLE_CENTER_X) * (px - g_AUDIT_CIRCLE_CENTER_X) +
                                             (py - g_AUDIT_CIRCLE_CENTER_Y) * (py - g_AUDIT_CIRCLE_CENTER_Y));
                                             
        if (dist_from_center_sq <= (g_AUDIT_CIRCLE_RADIUS * g_AUDIT_CIRCLE_RADIUS)) {
            continue; 
        }

        if (score < dynamic_radionullon_thresh) {
            continue;
        }

        int s = score > 255 ? 255 : score; 
        unsigned char r, g, b;
        
        if (s < 64) { b = 255; g = s * 4; r = 0; }
        else if (s < 128) { b = 255 - (s - 64) * 4; g = 255; r = 0; }
        else if (s < 192) { b = 0; g = 255; r = (s - 128) * 4; }
        else { b = 0; g = 255 - (s - 192) * 4; r = 255; } 
        
        output_pixels[i] = blend_color(output_pixels[i], r, g, b, g_HEATMAP_ALPHA);
    }
    
    // --- 4.5 ANZEIGE DES MORPHING-MINIMUMS und HAUTFARB-KONTUREN im (VIERTEL) KREIS ---
    
    int center_pixel_index = g_AUDIT_CIRCLE_CENTER_Y * IMG_W + g_AUDIT_CIRCLE_CENTER_X;
    
    if (center_pixel_index < 0 || center_pixel_index >= PIXEL_SIZE) {
        center_pixel_index = PIXEL_SIZE / 2; 
    }
    
    float current_bg_val = bg[center_pixel_index]; 
    float min_morphing_residue = fabsf(audit_target_val - current_bg_val);
    
    // Erzeuge skin-feedback basierend auf min_morphing_residue (wird nur für Logging genutzt)
    // unsigned int circle_feedback_color kept for compatibility if needed later

    // Zeichne das Viertelkreis-Tortenstück (z.B. oberes rechtes Viertel)
    // Wir definieren: Winkel  -90..0 (oben rechts). Das kann nach Wunsch angepasst werden.
    float start_angle = -M_PI/2.0f; // -90°
    float end_angle = 0.0f;         // 0° (rechts)

    for (int y_offset = -g_AUDIT_CIRCLE_RADIUS; y_offset <= g_AUDIT_CIRCLE_RADIUS; y_offset++) {
        for (int x_offset = -g_AUDIT_CIRCLE_RADIUS; x_offset <= g_AUDIT_CIRCLE_RADIUS; x_offset++) {
            float dist = sqrtf((float)(x_offset * x_offset + y_offset * y_offset));
            
            if (dist <= g_AUDIT_CIRCLE_RADIUS) {
                // berechne Winkel des Vektors (x_offset,y_offset)
                float ang = atan2f((float)y_offset, (float)x_offset); // -PI..PI

                // Normiere auf Bereich -PI..PI und prüfe ob Winkel im Viertel liegt
                // Für oberes rechtes Viertel: ang in [-PI/2, 0]
                if (!(ang >= start_angle && ang <= end_angle)) {
                    continue; // außerhalb des gewünschten Viertels -> überspringen
                }

                int current_x = g_AUDIT_CIRCLE_CENTER_X + x_offset;
                int current_y = g_AUDIT_CIRCLE_CENTER_Y + y_offset;

                if (current_x >= 0 && current_x < IMG_W && current_y >= 0 && current_y < IMG_H) {
                    int current_pixel_index = current_y * IMG_W + current_x;

                    // 1. Hell-zu-Dunkel Basis (Verlauf) - jetzt mit hautfarbenem Verlauf
                    float normalized_dist = dist / g_AUDIT_CIRCLE_RADIUS; 
                    
                    // Interpoliere zwischen skin-center und skin-contour
                    unsigned char r_base = (unsigned char)(g_SKIN_CENTER_R * (1.0f - normalized_dist) + g_SKIN_CONTOUR_R * normalized_dist);
                    unsigned char g_base = (unsigned char)(g_SKIN_CENTER_G * (1.0f - normalized_dist) + g_SKIN_CONTOUR_G * normalized_dist);
                    unsigned char b_base = (unsigned char)(g_SKIN_CENTER_B * (1.0f - normalized_dist) + g_SKIN_CONTOUR_B * normalized_dist);
                    
                    unsigned int base_color = (0xFF << 24) | (r_base << 16) | (g_base << 8) | b_base;

                    // 2. Rosa Rauschen (HSV) - bleibt optional/ästhetisch
                    float h_rand = g_NOISE_HUE_MIN + (float)rand() / RAND_MAX * (g_NOISE_HUE_MAX - g_NOISE_HUE_MIN);
                    float s_rand = g_NOISE_SATURATION_MIN + (float)rand() / RAND_MAX * (g_NOISE_SATURATION_MAX - g_NOISE_SATURATION_MIN);
                    float v_rand = g_NOISE_VALUE_MIN + (float)rand() / RAND_MAX * (g_NOISE_VALUE_MAX - g_NOISE_VALUE_MIN);
                    
                    unsigned char r_noise, g_noise, b_noise;
                    hsv_to_rgb(h_rand, s_rand, v_rand, &r_noise, &g_noise, &b_noise);
                    
                    // 3. Rauschen über Basis blenden (moderate Alpha)
                    unsigned int combined_color = blend_color(base_color, r_noise, g_noise, b_noise, g_NOISE_ALPHA);

                    // 4. Hautkontur-Feedback: je näher am Rand, desto stärker die dunkle Kontur
                    // contour_factor: 0 (Mitte) .. 1 (Rand)
                    float contour_factor = fminf(1.0f, normalized_dist * 1.2f); // leicht stärker am Rand
                    int contour_alpha = (int)(g_SKIN_CONTOUR_ALPHA * contour_factor);

                    // dunklere Konturfarbe (kann leicht variieren mit morphing-Residuum)
                    unsigned char contour_r = (unsigned char)g_SKIN_CONTOUR_R;
                    unsigned char contour_g = (unsigned char)g_SKIN_CONTOUR_G;
                    unsigned char contour_b = (unsigned char)g_SKIN_CONTOUR_B;

                    // Weiches inneres Aufhellen: innen noch etwas heller als base (leicht)
                    if (normalized_dist < 0.5f) {
                        float brighten = (0.5f - normalized_dist) * 0.4f; // bis ~40% Helligkeitsboost im Zentrum
                        unsigned char r_br = (unsigned char)fminf(255.0f, r_base + 255.0f * brighten);
                        unsigned char g_br = (unsigned char)fminf(255.0f, g_base + 255.0f * brighten);
                        unsigned char b_br = (unsigned char)fminf(255.0f, b_base + 255.0f * brighten);
                        combined_color = blend_color(combined_color, r_br, g_br, b_br, (int)(g_NOISE_ALPHA * 0.6f));
                    }

                    // 5. Kontur über die Kombination blenden
                    output_pixels[current_pixel_index] = blend_color(combined_color, contour_r, contour_g, contour_b, contour_alpha);
                }
            }
        }
    }

    if (audit_frame_counter == 1) { 
        SDL_Log("AUDIT: Min. Morphing Restdiff: %.2f (BG: %.2f | Ziel: %.0f | Rate: %f | Damping: %.4f)", 
                min_morphing_residue, current_bg_val, audit_target_val, dynamic_bg_learn_rate, damping_factor);
    }
    
    // --- 4.6 Geiger-Werte ableiten und Dynamischen Threshold evaluieren (Alle 60s) ---
    double rawVal = totalScore / 10000.0;
    currentRadSmooth = (currentRadSmooth * 0.9) + (rawVal * 0.1);
    totalDoseAcc += currentRadSmooth * 0.001;
    
    double avgFrameScore = totalScore / (double)PIXEL_SIZE;
    
    baseline_score_ema = (1.0 - g_BASELINE_EMA_ALPHA) * baseline_score_ema + g_BASELINE_EMA_ALPHA * avgFrameScore;
    
    frame_counter++;
    
    if (frame_counter >= g_THRESH_UPDATE_FRAMES) { 
        int new_thresh = (int)(baseline_score_ema * g_THRESH_OFFSET_FACTOR);
        
        if (new_thresh < g_RADIONULLON_THRESH_MIN) {
            new_thresh = g_RADIONULLON_THRESH_MIN;
        }

        dynamic_radionullon_thresh = new_thresh;
        SDL_Log("Dynamischer Threshold aktualisiert: %d (Basis-EMA: %.2f | Helligkeit: %.1f)", 
                dynamic_radionullon_thresh, baseline_score_ema, globalAvg);
        
        frame_counter = 0;
    }
}

// =================================================================
// 5. SDL2 HAUPTPROGRAMM
// =================================================================

int main(int argc, char *argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* texture = NULL;
    int running = 1;
    SDL_Event event;

    // --- INITIALISIERUNG DER GLOBALEN VARIABLEN ---
    g_AUDIT_CIRCLE_CENTER_X = IMG_W - g_AUDIT_CIRCLE_RADIUS - 40; 
    g_AUDIT_CIRCLE_CENTER_Y = IMG_H - g_AUDIT_CIRCLE_RADIUS - 40; 
    dynamic_radionullon_thresh = g_RADIONULLON_THRESH_MIN;
    audit_target_val = g_AUDIT_HIGH_VAL;
    srand((unsigned int)time(NULL)); 
    // ----------------------------------------------


    unsigned int *rgb_pixels = malloc(PIXEL_SIZE * sizeof(unsigned int));
    unsigned char *gray_pixels = malloc(PIXEL_SIZE * sizeof(unsigned char));
    float current_globalAvg = 128.0f; 

    if (!rgb_pixels || !gray_pixels) {
        fprintf(stderr, "Speicherzuweisung fehlgeschlagen!\n");
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL konnte nicht initialisiert werden: %s\n", SDL_GetError());
        free(rgb_pixels); free(gray_pixels);
        return 1;
    }
    if (v4l2_init() != 0) {
        fprintf(stderr, "V4L2-Initialisierung fehlgeschlagen. Beende.\n");
        SDL_Quit();
        free(rgb_pixels); free(gray_pixels);
        return 1;
    }

    window = SDL_CreateWindow("Radionullon Ghost Filter (SDL2) - Viertelkreis Audit + Hautkontur",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              IMG_W, IMG_H,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    SDL_MaximizeWindow(window); 

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 
                                SDL_TEXTUREACCESS_STREAMING, IMG_W, IMG_H);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = 0;
            }
        }

        unsigned char *yuyv_data = v4l2_read_frame();
        if (yuyv_data) {
            yuyv_to_rgb32_and_gray(yuyv_data, rgb_pixels, gray_pixels, &current_globalAvg);

            process_radionullon(rgb_pixels, gray_pixels, current_globalAvg);
            
            SDL_UpdateTexture(texture, NULL, rgb_pixels, IMG_W * sizeof(unsigned int));
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    // 6. Cleanup
    free(rgb_pixels);
    free(gray_pixels);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    v4l2_cleanup();
    SDL_Quit();

    return 0;
}

