#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>

// Linux V4L2 Includes
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

// --- Konfiguration & Voreinstellungen ---
#define DEVICE_NODE "/dev/video0"
#define IMG_W 640
#define IMG_H 480
#define PIXEL_SIZE (IMG_W * IMG_H)

// Basiskonstanten
#define RESIDUE_THRESH 15           
#define HEATMAP_ALPHA 155
#define RADIONULLON_THRESH_MIN 30 

// Dynamische Schwellenwert-Konstanten
#define THRESH_UPDATE_FRAMES 1800 
#define BASELINE_EMA_ALPHA 0.01   
#define THRESH_OFFSET_FACTOR 1.5  

// Parameter-Grenzwerte für ULTRA-SLOW MORPHING (20+ Sekunden Persistenz)
#define BRIGHTNESS_LIMIT_HIGH 200.0f
#define MINIMUM_BG_LEARN_RATE 0.0001f 
#define BG_RATE_SLOW 0.0001f        
#define BG_RATE_FAST 0.005f         

#define INITIAL_MAX_LEARN_RATE 0.05f  
#define DAMPING_DURATION_FRAMES 900   

#define FLICKER_DECAY_RATE 10       
#define INITIAL_FLICKER_BOOST 25    

// PARAMETER FÜR EXTREMEN BLUR/BOOST
#define BLUR_KERNEL_RADIUS 2        
#define GAUSSIAN_SIGMA 5.0f         
#define BRIGHTNESS_BOOST_GAIN 2.5f  
#define CONTRAST_BOOST_FACTOR 1.5f  
#define EDGE_GAIN_MIN 1.0f           
#define EDGE_GAIN_MAX 2.0f           

// PARAMETER FÜR AUDITING
#define AUDIT_INDEX (PIXEL_SIZE - 1) 
#define AUDIT_SWITCH_INTERVAL 150    
#define AUDIT_LOW_VAL 20.0f          
#define AUDIT_HIGH_VAL 200.0f        

// --- Globale Zustände für Radionullon-Filter (Persistent) ---
float bg[PIXEL_SIZE];
unsigned char flicker[PIXEL_SIZE];
float prevGlobalAvg = 128.0f;
double currentRadSmooth = 0.0;
double totalDoseAcc = 0.0;
int flicker_decay_counter = 0;

// Dynamische Parameter und Zähler
int dynamic_radionullon_thresh = RADIONULLON_THRESH_MIN;
double baseline_score_ema = 0.0;
int frame_counter = 0;
unsigned int frame_counter_total = 0; 
float audit_target_val = AUDIT_HIGH_VAL; 
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

// Berechnet den Gauß-Kernel (5x5)
void calculate_gaussian_kernel(float kernel[5][5]) {
    float sigma = GAUSSIAN_SIGMA;
    float sum = 0.0f;
    int r = BLUR_KERNEL_RADIUS; 

    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            kernel[y + r][x + r] = expf(-(float)(x * x + y * y) / (2.0f * sigma * sigma));
            sum += kernel[y + r][x + r];
        }
    }

    // Normalisierung
    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 5; x++) {
            kernel[y][x] /= sum;
        }
    }
}

// Führt extremen Blur, Boost und Kantenkontrast-Rekonstruktion durch
void apply_extreme_blur_and_boost(const unsigned char* src_gray, unsigned char* dst_soft_boosted, unsigned char* dst_sobel_edge) {
    int w = IMG_W;
    int h = IMG_H;
    
    // 1. Gauß-Weichzeichnung
    float kernel[5][5];
    calculate_gaussian_kernel(kernel);
    float blurred_float[PIXEL_SIZE] = {0.0f};
    
    int r = BLUR_KERNEL_RADIUS; 

    for (int y = r; y < h - r; y++) {
        for (int x = r; x < w - r; x++) {
            float sum = 0.0f;
            for (int ky = -r; ky <= r; ky++) {
                for (int kx = -r; kx <= r; kx++) {
                    sum += (float)src_gray[(y + ky) * w + (x + kx)] * kernel[ky + r][kx + r];
                }
            }
            blurred_float[y * w + x] = sum;
        }
    }
    
    // 2. Sobel Edge Detection (auf dem Originalbild)
    unsigned char sobel_mag[PIXEL_SIZE] = {0};
    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            int i = y * w + x;
            int gx = -src_gray[i - w - 1] - 2 * src_gray[i - 1] - src_gray[i + w - 1] +
                      src_gray[i - w + 1] + 2 * src_gray[i + 1] + src_gray[i + w + 1];
            
            // KORRIGIERTE ZEILE (gray durch src_gray ersetzt)
            int gy = -src_gray[i - w - 1] - 2 * src_gray[i - w] - src_gray[i - w + 1] +
                      src_gray[i + w - 1] + 2 * src_gray[i + w] + src_gray[i + w + 1];
                      
            int mag = (int)sqrt((double)gx * gx + (double)gy * gy);
            sobel_mag[i] = (unsigned char)(mag > 255 ? 255 : mag);
        }
    }

    // 3. Extremes Hochziehen und Kontrast-Rekonstruktion
    for (int i = 0; i < PIXEL_SIZE; i++) {
        float boosted = blurred_float[i] * BRIGHTNESS_BOOST_GAIN; 
        float edge_correction = (float)sobel_mag[i] * CONTRAST_BOOST_FACTOR;
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

void process_radionullon(unsigned int *output_pixels, const unsigned char* gray, float globalAvg) {
    int size = PIXEL_SIZE;
    int i, x, y;
    double totalScore = 0.0;
    
    unsigned char soft_boosted_gray[PIXEL_SIZE];
    unsigned char sobel_edge_output[PIXEL_SIZE]; 

    // --- 4.1 FILTERSTUFE: EXTREMER BLUR, BOOST, KANTENKONTRAST ---
    apply_extreme_blur_and_boost(gray, soft_boosted_gray, sobel_edge_output);
    
    // --- 4.2 AUDITING-ALGORITHMUS: Zielwert setzen ---
    audit_frame_counter++;
    if (audit_frame_counter >= AUDIT_SWITCH_INTERVAL) {
        if (audit_target_val == AUDIT_HIGH_VAL) {
            audit_target_val = AUDIT_LOW_VAL;
        } else {
            audit_target_val = AUDIT_HIGH_VAL;
        }
        audit_frame_counter = 0;
    }
    
    // Auditing-Signal auf das verarbeitete Bild (unten rechts) anwenden
    soft_boosted_gray[AUDIT_INDEX] = (unsigned char)audit_target_val;
    const unsigned char* processed_gray = soft_boosted_gray;


    // --- 4.3 Berechnung dynamischer Parameter ---
    float normalized_avg = fminf(globalAvg / BRIGHTNESS_LIMIT_HIGH, 1.0f); 

    // Dämpfungsfaktor (Sinus-Dämpfung)
    float dynamic_bg_learn_rate_base = BG_RATE_SLOW + (BG_RATE_FAST - BG_RATE_SLOW) * normalized_avg;
    float damping_factor = 0.0f;
    frame_counter_total++;
    
    if (frame_counter_total < DAMPING_DURATION_FRAMES) {
        float t_norm = ((float)frame_counter_total / DAMPING_DURATION_FRAMES) * M_PI - (M_PI / 2.0f);
        damping_factor = fmaxf(0.0f, 1.0f - (sin(t_norm) * 0.5f + 0.5f));
    }
    float initial_boost_rate = INITIAL_MAX_LEARN_RATE * damping_factor;
    float dynamic_bg_learn_rate = dynamic_bg_learn_rate_base + initial_boost_rate;
                                  
    if (dynamic_bg_learn_rate < MINIMUM_BG_LEARN_RATE) {
        dynamic_bg_learn_rate = MINIMUM_BG_LEARN_RATE;
    }
    
    int dynamic_flicker_thresh = (int)(10.0 + (globalAvg / 15.0));
    if (dynamic_flicker_thresh > 50) dynamic_flicker_thresh = 50;

    float dynamic_edge_gain = EDGE_GAIN_MIN + (EDGE_GAIN_MAX - EDGE_GAIN_MIN) * normalized_avg;

    // --- 4.4 Filter-Pipeline ---
    
    unsigned char residue[size];
    unsigned char edge[size];
    
    // --- Hintergrundmodell (EMA) und Residue ---
    for (i = 0; i < size; i++) {
        bg[i] = (1.0f - dynamic_bg_learn_rate) * bg[i] + dynamic_bg_learn_rate * (float)processed_gray[i];
        
        int diff = abs((int)processed_gray[i] - (int)bg[i]);
        residue[i] = (diff > RESIDUE_THRESH) ? (unsigned char)(diff > 255 ? 255 : diff) : 0;
    }

    // --- Flicker-Maß ---
    unsigned char currentAvg = (unsigned char)(globalAvg > 255.0f ? 255 : (globalAvg < 0.0f ? 0 : globalAvg));
    int delta = abs((int)currentAvg - (int)prevGlobalAvg);
    prevGlobalAvg = currentAvg;
    
    unsigned char boost = (delta > dynamic_flicker_thresh) ? INITIAL_FLICKER_BOOST : 1; 

    for (i = 0; i < size; i++) {
        int new_flicker = (int)flicker[i] + boost;
        flicker[i] = (unsigned char)(new_flicker > 255 ? 255 : new_flicker);
    }
    
    // ULTRA-LANGSAMER ZERFALL
    flicker_decay_counter++;
    if (flicker_decay_counter >= FLICKER_DECAY_RATE) {
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

    // --- Heatmap Overlay und Radionullon-Analyse ---
    for (i = 0; i < size; i++) {
        int score = (int)residue[i] + (int)(edge[i] / 2) + (int)(flicker[i] / 3);
        totalScore += score;
        
        if (score < dynamic_radionullon_thresh) {
            continue;
        }

        int s = score > 255 ? 255 : score;
        unsigned char r, g, b;
        
        // Farbschema
        if (s < 64) { b = 255; g = s * 4; r = 0; }
        else if (s < 128) { b = 255 - (s - 64) * 4; g = 255; r = 0; }
        else if (s < 192) { b = 0; g = 255; r = (s - 128) * 4; }
        else { b = 0; g = 255 - (s - 192) * 4; r = 255; }

        output_pixels[i] = blend_color(output_pixels[i], r, g, b, HEATMAP_ALPHA);
    }
    
    // --- 4.5 ANZEIGE DES MORPHING-MINIMUMS (Rest-Differenz) ---
    float current_bg_val = bg[AUDIT_INDEX];
    float min_morphing_residue = fabsf(audit_target_val - current_bg_val);
    
    // Visuelles Feedback am Audit-Pixel (unten rechts)
    if (min_morphing_residue > 10.0f) {
        unsigned char r_val = (unsigned char)(fminf(255.0f, min_morphing_residue * 2.0f));
        output_pixels[AUDIT_INDEX] = (0xFF << 24) | (r_val << 16) | (0x00 << 8) | 0x00; // Rot (Hohe Restdifferenz)
    } else {
        unsigned char g_val = (unsigned char)(fminf(255.0f, (10.0f - min_morphing_residue) * 25.0f));
        output_pixels[AUDIT_INDEX] = (0xFF << 24) | (0x00 << 16) | (g_val << 8) | 0x00; // Grün (Niedrige Restdifferenz)
    }

    // Ausgabe des Wertes in der Konsole zur Überprüfung
    if (audit_frame_counter == 1) { 
        SDL_Log("AUDIT: Min. Morphing Restdifferenz: %.2f (BG: %.2f | Ziel: %.0f | Rate: %f)", 
                min_morphing_residue, current_bg_val, audit_target_val, dynamic_bg_learn_rate);
    }
    
    // --- 4.6 Geiger-Werte ableiten und Dynamischen Threshold evaluieren (Alle 60s) ---
    double rawVal = totalScore / 10000.0;
    currentRadSmooth = (currentRadSmooth * 0.9) + (rawVal * 0.1);
    totalDoseAcc += currentRadSmooth * 0.001;
    
    double avgFrameScore = totalScore / (double)PIXEL_SIZE;
    
    baseline_score_ema = (1.0 - BASELINE_EMA_ALPHA) * baseline_score_ema + BASELINE_EMA_ALPHA * avgFrameScore;
    
    frame_counter++;
    
    if (frame_counter >= THRESH_UPDATE_FRAMES) { 
        int new_thresh = (int)(baseline_score_ema * THRESH_OFFSET_FACTOR);
        
        if (new_thresh < RADIONULLON_THRESH_MIN) {
            new_thresh = RADIONULLON_THRESH_MIN;
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

    unsigned int *rgb_pixels = malloc(PIXEL_SIZE * sizeof(unsigned int));
    unsigned char *gray_pixels = malloc(PIXEL_SIZE * sizeof(unsigned char));
    float current_globalAvg = 128.0f; 

    if (!rgb_pixels || !gray_pixels) {
        fprintf(stderr, "Speicherzuweisung fehlgeschlagen!\n");
        return 1;
    }

    #ifndef M_PI
    #define M_PI 3.14159265358979323846
    #endif

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

    window = SDL_CreateWindow("Radionullon Ghost Filter (SDL2) - Auditing Active",
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
