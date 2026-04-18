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

// --- Konfiguration & Radionullon-Tunables ---
#define DEVICE_NODE "/dev/video0"
#define IMG_W 640
#define IMG_H 480
#define PIXEL_SIZE (IMG_W * IMG_H)

// Radionullon-Konstanten (wie im Original-Quellcode)
#define BG_LEARN_RATE 0.01f
#define RESIDUE_THRESH 15
#define FLICKER_THRESH 20
#define EDGE_GAIN 1.6f
#define HEATMAP_ALPHA 155
#define RADIONULLON_THRESH 30

// --- Globale Zustände für Radionullon-Filter (Persistent) ---
float bg[PIXEL_SIZE];
unsigned char flicker[PIXEL_SIZE];
float prevGlobalAvg = 128.0f;
double currentRadSmooth = 0.0;
double totalDoseAcc = 0.0;

// --- V4L2 Strukturen & Zustand ---
struct Buffer { void *start; size_t length; };
struct Buffer *buffers;
int n_buffers = 0;
int fd_v4l2 = -1;

// =================================================================
// 1. V4L2 CAPTURE LOGIK
// =================================================================

int v4l2_init() {
    // 1. Gerät öffnen
    fd_v4l2 = open(DEVICE_NODE, O_RDWR | O_NONBLOCK, 0);
    if (fd_v4l2 == -1) {
        perror("V4L2: Fehler beim Oeffnen des Geraets");
        return -1;
    }

    // 2. Format setzen (YUYV)
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = IMG_W;
    fmt.fmt.pix.height = IMG_H;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; // YUYV (2 Bytes/Pixel)
    if (ioctl(fd_v4l2, VIDIOC_S_FMT, &fmt) == -1) {
        perror("V4L2: VIDIOC_S_FMT Fehler");
        close(fd_v4l2); return -1;
    }

    // 3. Puffer anfordern und mappen
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
    
    // 4. Puffer in die Queue stellen und Streaming starten
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

    // Initialisiere Radionullon-Zustände
    memset(bg, 0, sizeof(bg));
    memset(flicker, 0, sizeof(flicker));

    return 0;
}

unsigned char* v4l2_read_frame() {
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    // Dequeue Buffer (Blockiert oder Fehler bei NONBLOCK)
    if (ioctl(fd_v4l2, VIDIOC_DQBUF, &buf) == -1) {
        // Bei NONBLOCK würde man hier kurz warten oder überspringen
        return NULL;
    }

    unsigned char* frame_data = (unsigned char*)buffers[buf.index].start;
    
    // Buffer wieder enqueuen
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

void yuyv_to_rgb32_and_gray(const unsigned char* src_yuyv, unsigned int* dst_rgb32, unsigned char* dst_gray) {
    for (int y = 0; y < IMG_H; y++) {
        const unsigned char* srcLine = src_yuyv + (y * IMG_W * 2);
        unsigned int* dstRgbLine = dst_rgb32 + (y * IMG_W);
        unsigned char* dstGrayLine = dst_gray + (y * IMG_W);

        for (int x = 0; x < IMG_W; x += 2) {
            int Y1 = srcLine[0]; int U = srcLine[1]; int Y2 = srcLine[2]; int V = srcLine[3]; srcLine += 4;
            
            // Pixel 1
            int C1 = Y1 - 16; int D = U - 128; int E = V - 128;
            int R1 = (298 * C1 + 409 * E + 128) >> 8;
            int G1 = (298 * C1 - 100 * D - 208 * E + 128) >> 8;
            int B1 = (298 * C1 + 516 * D + 128) >> 8;
            
            // Pixel 2
            int C2 = Y2 - 16;
            int R2 = (298 * C2 + 409 * E + 128) >> 8;
            int G2 = (298 * C2 - 100 * D - 208 * E + 128) >> 8;
            int B2 = (298 * C2 + 516 * D + 128) >> 8;

            // Clamping und Speichern als ABGR (SDL_PIXELFORMAT_ARGB8888 für Little Endian)
            #define CLAMP(v) ((unsigned char)((v) > 255 ? 255 : ((v) < 0 ? 0 : (v))))

            unsigned char r1_c = CLAMP(R1); unsigned char g1_c = CLAMP(G1); unsigned char b1_c = CLAMP(B1);
            dstRgbLine[0] = (0xFF << 24) | (r1_c << 16) | (g1_c << 8) | b1_c;
            
            unsigned char r2_c = CLAMP(R2); unsigned char g2_c = CLAMP(G2); unsigned char b2_c = CLAMP(B2);
            dstRgbLine[1] = (0xFF << 24) | (r2_c << 16) | (g2_c << 8) | b2_c;
            
            dstRgbLine += 2;

            // Graustufen für den Filter (Verwenden des Y-Kanals)
            dstGrayLine[0] = CLAMP(Y1);
            dstGrayLine[1] = CLAMP(Y2);
            dstGrayLine += 2;
            #undef CLAMP
        }
    }
}

// =================================================================
// 3. RADIONULLON FILTER
// =================================================================

// Hilfsfunktion: Alpha Blend (ABGR-Format)
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

void process_radionullon(unsigned int *output_pixels, const unsigned char* gray) {
    int size = PIXEL_SIZE;
    
    unsigned char residue[size];
    unsigned char edge[size];
    
    int i, x, y;
    double totalScore = 0.0;
    
    // --- 2. Hintergrundmodell (EMA) und Residue (Bewegungsmaske) ---
    float globalAvg = 0.0f;
    for (i = 0; i < size; i++) {
        bg[i] = (1.0f - BG_LEARN_RATE) * bg[i] + BG_LEARN_RATE * (float)gray[i];
        
        int diff = abs((int)gray[i] - (int)bg[i]);
        residue[i] = (diff > RESIDUE_THRESH) ? (unsigned char)(diff > 255 ? 255 : diff) : 0;
        globalAvg += (float)gray[i];
    }
    globalAvg /= size;

    // --- 3. Flicker-Maß ---
    unsigned char currentAvg = (unsigned char)(globalAvg > 255.0f ? 255 : (globalAvg < 0.0f ? 0 : globalAvg));
    int delta = abs((int)currentAvg - (int)prevGlobalAvg);
    prevGlobalAvg = currentAvg;
    unsigned char boost = (delta > FLICKER_THRESH) ? 20 : 1;

    for (i = 0; i < size; i++) {
        int new_flicker = (int)flicker[i] + boost;
        flicker[i] = (unsigned char)(new_flicker > 255 ? 255 : new_flicker);
        if (flicker[i] > 0) flicker[i] = (unsigned char)(flicker[i] - 1);
    }

    // --- 4. Sobel Edge Detection ---
    for (y = 1; y < IMG_H - 1; y++) {
        for (x = 1; x < IMG_W - 1; x++) {
            i = y * IMG_W + x;
            
            int gx = -gray[i - IMG_W - 1] - 2 * gray[i - 1] - gray[i + IMG_W - 1] +
                      gray[i - IMG_W + 1] + 2 * gray[i + 1] + gray[i + IMG_W + 1];
            int gy = -gray[i - IMG_W - 1] - 2 * gray[i - IMG_W] - gray[i - IMG_W + 1] +
                      gray[i + IMG_W - 1] + 2 * gray[i + IMG_W] + gray[i + IMG_W + 1];
                      
            int mag = (int)(EDGE_GAIN * sqrt((double)gx * gx + (double)gy * gy));
            edge[i] = (unsigned char)(mag > 255 ? 255 : mag);
        }
    }

    // --- 5. Heatmap Overlay und Radionullon-Analyse ---
    for (i = 0; i < size; i++) {
        // Anomaly Score: Residue + Edge/2 + Flicker/3
        int score = (int)residue[i] + (int)(edge[i] / 2) + (int)(flicker[i] / 3);
        totalScore += score;
        
        if (score < RADIONULLON_THRESH) {
            continue;
        }

        int s = score > 255 ? 255 : score;
        unsigned char r, g, b;
        
        // Map Score to Heat Color
        if (s < 64) { b = 255; g = s * 4; r = 0; }
        else if (s < 128) { b = 255 - (s - 64) * 4; g = 255; r = 0; }
        else if (s < 192) { b = 0; g = 255; r = (s - 128) * 4; }
        else { b = 0; g = 255 - (s - 192) * 4; r = 255; }

        output_pixels[i] = blend_color(output_pixels[i], r, g, b, HEATMAP_ALPHA);
    }

    // --- 6. Geiger-Werte ableiten ---
    double rawVal = totalScore / 10000.0;
    currentRadSmooth = (currentRadSmooth * 0.9) + (rawVal * 0.1);
    totalDoseAcc += currentRadSmooth * 0.001;
    
    // Einfache Konsolenausgabe für Statistiken
    // SDL_Log("RADIONULLON: %.2f Sv/h | DOSE: %.4f Sv", currentRadSmooth, totalDoseAcc);
}

// =================================================================
// 4. SDL2 HAUPTPROGRAMM
// =================================================================

int main(int argc, char *argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* texture = NULL;
    int running = 1;
    SDL_Event event;

    // Speicher für die Bilddaten
    unsigned int *rgb_pixels = malloc(PIXEL_SIZE * sizeof(unsigned int));
    unsigned char *gray_pixels = malloc(PIXEL_SIZE * sizeof(unsigned char));

    if (!rgb_pixels || !gray_pixels) {
        fprintf(stderr, "Speicherzuweisung fehlgeschlagen!\n");
        return 1;
    }

    // 1. Initialisierung
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

    // 2. SDL-Fenster und Renderer erstellen
    window = SDL_CreateWindow("Radionullon Ghost Filter (SDL2)",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              IMG_W, IMG_H,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    // Fenster maximieren
    SDL_MaximizeWindow(window); 

    // 3. SDL-Textur erstellen (ABGR-Format)
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 
                                SDL_TEXTUREACCESS_STREAMING, IMG_W, IMG_H);

    // 4. Haupt-Loop
    while (running) {
        // Events verarbeiten
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = 0;
            }
        }

        // Frame von der Kamera lesen
        unsigned char *yuyv_data = v4l2_read_frame();
        if (yuyv_data) {
            // A: YUYV -> RGB32 (ABGR) + Graustufen
            yuyv_to_rgb32_and_gray(yuyv_data, rgb_pixels, gray_pixels);

            // B: Radionullon-Algorithmus anwenden
            process_radionullon(rgb_pixels, gray_pixels);
            
            // C: Textur aktualisieren
            SDL_UpdateTexture(texture, NULL, rgb_pixels, IMG_W * sizeof(unsigned int));
        }

        // Textur rendern und anzeigen
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        
        // Die VSync-Einstellung des Renderers sollte die FPS automatisch regeln
    }

    // 5. Cleanup
    free(rgb_pixels);
    free(gray_pixels);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    v4l2_cleanup();
    SDL_Quit();

    return 0;
}
