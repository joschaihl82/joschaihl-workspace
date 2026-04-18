#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <math.h>


// SDL2 Includes (muss Linker-Flags -lSDL2 -lm enthalten)
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>

// --- Konfiguration ---
#define DEVICE_NODE "/dev/video0"
#define IMG_W 640
#define IMG_H 480
#define GRAPH_POINTS 100 // Anzahl der Punkte pro Graph

// --- Radionullon-Tunables ---
#define BG_LEARN_RATE 0.01f
#define RESIDUE_THRESH 15
#define FLICKER_THRESH 20
#define EDGE_GAIN 1.6f
#define HEATMAP_ALPHA 155
#define RADIONULLON_THRESH 30

// --- NEUE GEIGER-TUNABLES ---
#define HOT_PIXEL_THRESH 200 // Y_HOT_THRESH (Graustufen)
#define GEIGER_SMOOTH_FACTOR 0.3
#define GEIGER_NORM_FACTOR 100.0
#define GEIGER_ALERT_THRESH 5.0

// --- Datenstrukturen ---
struct Buffer {
    void *start;
    size_t length;
};

// Geiger Stats für die Anzeige
typedef struct {
    double currentRad;
    double avgRad;
    double totalDose;
    int alertState;
    // History-Puffer (als einfache C-Arrays)
    double shortHistory[GRAPH_POINTS];
    double avgHistory[GRAPH_POINTS];
    double longHistory[GRAPH_POINTS];
    // Start- und aktuelle Zeit für Dosis-Berechnung (Sekunden)
    long long startTimeSec;
    long long currentTimeSec;
} GeigerStats;

// Logik-Zustand
typedef struct {
    float *bg; // Hintergrund (IMG_W * IMG_H)
    uint8_t *flicker; // Flicker (IMG_W * IMG_H)
    float prevGlobalAvg;

    double currentRadSmooth;
    double totalDoseAcc;
    int longTermCounter;

    GeigerStats stats;
} GeigerLogicState;


// --- V4L2 Funktionen ---

static int v4l2_fd = -1;
static struct Buffer *v4l2_buffers = NULL;
static unsigned int v4l2_n_buffers = 0;

static int xioctl(int fh, int request, void *arg) {
    int r;
    do {
        r = ioctl(fh, request, arg);
    } while (-1 == r && EINTR == errno);
    return r;
}

int v4l2_init() {
    struct v4l2_format fmt;
    struct v4l2_requestbuffers req;
    enum v4l2_buf_type type;

    v4l2_fd = open(DEVICE_NODE, O_RDWR | O_NONBLOCK, 0);
    if (v4l2_fd == -1) {
        perror("Fehler bei DEVICE_NODE");
        return -1;
    }

    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = IMG_W;
    fmt.fmt.pix.height = IMG_H;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    if (xioctl(v4l2_fd, VIDIOC_S_FMT, &fmt) == -1) {
        perror("VIDIOC_S_FMT");
        return -1;
    }

    // Puffer anfordern
    memset(&req, 0, sizeof(req));
    req.count = 2;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (xioctl(v4l2_fd, VIDIOC_REQBUFS, &req) == -1) {
        perror("VIDIOC_REQBUFS");
        return -1;
    }

    v4l2_buffers = calloc(req.count, sizeof(*v4l2_buffers));
    for (v4l2_n_buffers = 0; v4l2_n_buffers < req.count; ++v4l2_n_buffers) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = v4l2_n_buffers;

        if (xioctl(v4l2_fd, VIDIOC_QUERYBUF, &buf) == -1) {
            perror("VIDIOC_QUERYBUF");
            return -1;
        }

        v4l2_buffers[v4l2_n_buffers].length = buf.length;
        v4l2_buffers[v4l2_n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, v4l2_fd, buf.m.offset);
        if (MAP_FAILED == v4l2_buffers[v4l2_n_buffers].start) {
            perror("mmap");
            return -1;
        }
    }

    // Puffer in die Queue
    for (unsigned int i = 0; i < v4l2_n_buffers; ++i) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(v4l2_fd, VIDIOC_QBUF, &buf) == -1) {
            perror("VIDIOC_QBUF");
            return -1;
        }
    }

    // Stream starten
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(v4l2_fd, VIDIOC_STREAMON, &type) == -1) {
        perror("VIDIOC_STREAMON");
        return -1;
    }

    return 0;
}

// YUYV zu RGB32 Konvertierung (analog zur Qt-Logik)
void yuyv_to_rgb32(const uint8_t *src, uint32_t *dst, int width, int height) {
    for (int y = 0; y < height; y++) {
        const uint8_t *srcLine = src + (y * width * 2);
        uint32_t *dstLine = dst + (y * width);
        for (int x = 0; x < width; x += 2) {
            int Y1 = srcLine[0];
            int U = srcLine[1];
            int Y2 = srcLine[2];
            int V = srcLine[3];
            srcLine += 4;

            // YUV zu RGB Konvertierung (C-Style, ohne Qt-Funktionen)
            int C, R, G, B;
            
            // Pixel 1 (Y1)
            C = Y1 - 16;
            int D = U - 128;
            int E = V - 128;
            R = (298 * C + 409 * E + 128) >> 8;
            G = (298 * C - 100 * D - 208 * E + 128) >> 8;
            B = (298 * C + 516 * D + 128) >> 8;
            // dstLine[0] = 0xFF000000 | (R << 16) | (G << 8) | B; // Little-Endian ARGB
            dstLine[0] = (uint32_t)( (R>255?255:(R<0?0:R)) << 16 | (G>255?255:(G<0?0:G)) << 8 | (B>255?255:(B<0?0:B)) ); // RGB
            
            // Pixel 2 (Y2)
            C = Y2 - 16;
            R = (298 * C + 409 * E + 128) >> 8;
            G = (298 * C - 100 * D - 208 * E + 128) >> 8;
            B = (298 * C + 516 * D + 128) >> 8;
            // dstLine[1] = 0xFF000000 | (R << 16) | (G << 8) | B; // Little-Endian ARGB
            dstLine[1] = (uint32_t)( (R>255?255:(R<0?0:R)) << 16 | (G>255?255:(G<0?0:G)) << 8 | (B>255?255:(B<0?0:B)) ); // RGB
            
            dstLine += 2;
        }
    }
}

// Gibt 0 bei Erfolg zurück, füllt den Puffer mit RGB32-Daten
int v4l2_read_frame(uint32_t *rgb32_buffer) {
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    // Puffer aus der Queue nehmen
    if (xioctl(v4l2_fd, VIDIOC_DQBUF, &buf) == -1) {
        if (errno == EAGAIN) return 1; // Kein Puffer verfügbar
        perror("VIDIOC_DQBUF");
        return -1;
    }

    // Konvertierung
    uint8_t *src = (uint8_t *)v4l2_buffers[buf.index].start;
    yuyv_to_rgb32(src, rgb32_buffer, IMG_W, IMG_H);

    // Puffer zurück in die Queue
    if (xioctl(v4l2_fd, VIDIOC_QBUF, &buf) == -1) {
        perror("VIDIOC_QBUF");
        return -1;
    }

    return 0; // Erfolg
}

void v4l2_close() {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(v4l2_fd, VIDIOC_STREAMOFF, &type);
    for (unsigned int i = 0; i < v4l2_n_buffers; ++i) {
        if (v4l2_buffers[i].start) {
            munmap(v4l2_buffers[i].start, v4l2_buffers[i].length);
        }
    }
    free(v4l2_buffers);
    if (v4l2_fd != -1) close(v4l2_fd);
}


// --- Radionullon/Geiger Logik Funktionen ---

static inline uint8_t clamp_u8(int val) {
    if (val < 0) return 0;
    if (val > 255) return 255;
    return (uint8_t)val;
}

static inline uint8_t rgb32_to_grayscale(uint32_t rgb) {
    // Standard Luminanz-Formel Y = 0.299*R + 0.587*G + 0.114*B (analog zu qGray)
    uint8_t r = (uint8_t)((rgb >> 16) & 0xFF);
    uint8_t g = (uint8_t)((rgb >> 8) & 0xFF);
    uint8_t b = (uint8_t)(rgb & 0xFF);
    return (uint8_t)((299 * r + 587 * g + 114 * b) / 1000);
}

static inline void rotate_buffer(double *buf, int size, double val) {
    for (int i = 0; i < size - 1; ++i) {
        buf[i] = buf[i + 1];
    }
    buf[size - 1] = val;
}

void init_geiger_logic(GeigerLogicState *state) {
    int size = IMG_W * IMG_H;
    state->bg = (float *)calloc(size, sizeof(float));
    state->flicker = (uint8_t *)calloc(size, sizeof(uint8_t));
    state->prevGlobalAvg = 128.0f;
    state->currentRadSmooth = 0.0;
    state->totalDoseAcc = 0.0;
    state->longTermCounter = 0;
    
    // Historien initialisieren
    memset(state->stats.shortHistory, 0, sizeof(state->stats.shortHistory));
    memset(state->stats.avgHistory, 0, sizeof(state->stats.avgHistory));
    memset(state->stats.longHistory, 0, sizeof(state->stats.longHistory));
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    state->stats.startTimeSec = tv.tv_sec;
}

void cleanup_geiger_logic(GeigerLogicState *state) {
    free(state->bg);
    free(state->flicker);
}


// Radionullon/Geiger-Verarbeitung (Kernlogik)
void process_radionullon(GeigerLogicState *state, const uint32_t *img_in, uint32_t *img_out) {
    int size = IMG_W * IMG_H;
    
    // Temporäre Puffer (müssten eigentlich statisch oder wiederverwendet werden für Performance)
    uint8_t *gray = (uint8_t *)malloc(size);
    uint8_t *residue = (uint8_t *)malloc(size);
    uint8_t *edge = (uint8_t *)malloc(size);
    if (!gray || !residue || !edge) return;
    memset(edge, 0, size); // Ränder für Sobel auf 0

    memcpy(img_out, img_in, size * sizeof(uint32_t)); // Kopiere Original für Output-Frame
    
    int hotPixels = 0;
    float globalAvg = 0.0f;

    // 1. RGB32 zu Graustufen konvertieren und Hot Pixels zählen
    for (int i = 0; i < size; ++i) {
        uint8_t g = rgb32_to_grayscale(img_in[i]);
        gray[i] = g;
        
        if (g > HOT_PIXEL_THRESH) {
            hotPixels++;
        }
    }

    // 2. Hintergrundmodell (EMA) und Residue (Bewegungsmaske)
    for (int i = 0; i < size; i++) {
        state->bg[i] = (1.0f - BG_LEARN_RATE) * state->bg[i] + BG_LEARN_RATE * (float)gray[i];
        
        int diff = (int)fabsf((float)gray[i] - state->bg[i]);
        residue[i] = (uint8_t)((diff > RESIDUE_THRESH) ? (diff > 255 ? 255 : diff) : 0);
        globalAvg += (float)gray[i];
    }
    globalAvg /= size;

    // 3. Flicker-Maß (Temporale Helligkeitsschwankung)
    uint8_t currentAvg = clamp_u8((int)globalAvg);
    int delta = (int)fabsf((float)currentAvg - state->prevGlobalAvg);
    state->prevGlobalAvg = currentAvg;
    uint8_t boost = (delta > FLICKER_THRESH) ? 20 : 1;

    for (int i = 0; i < size; i++) {
        state->flicker[i] = clamp_u8((int)state->flicker[i] + boost);
        if (state->flicker[i] > 0) state->flicker[i] = (uint8_t)(state->flicker[i] - 1);
    }

    // 4. Sobel Edge Detection
    for (int y = 1; y < IMG_H - 1; y++) {
        for (int x = 1; x < IMG_W - 1; x++) {
            int i = y * IMG_W + x;
            int gx = -gray[i - IMG_W - 1] - 2 * gray[i - 1] - gray[i + IMG_W - 1] +
                     gray[i - IMG_W + 1] + 2 * gray[i + 1] + gray[i + IMG_W + 1];
            int gy = -gray[i - IMG_W - 1] - 2 * gray[i - IMG_W] - gray[i - IMG_W + 1] +
                     gray[i + IMG_W - 1] + 2 * gray[i + IMG_W] + gray[i + IMG_W + 1];
            int mag = (int)(EDGE_GAIN * sqrtf((float)(gx * gx + gy * gy)));
            edge[i] = clamp_u8(mag);
        }
    }

    // 5. Heatmap Overlay und Radionullon-Analyse
    double totalScore = 0.0;
    for (int i = 0; i < size; ++i) {
        int score = (int)residue[i] + (int)(edge[i] / 2) + (int)(state->flicker[i] / 3);
        totalScore += score;
        
        if (score < RADIONULLON_THRESH) {
            uint8_t g = gray[i];
            img_out[i] = (uint32_t)(g << 16 | g << 8 | g); // Graustufe (Schwarz-Weiß)
            continue;
        }

        // Map Score to Heat Color
        int s = (score > 255) ? 255 : score;
        uint8_t r = 0, g = 0, b = 0;
        if (s < 64) { b = 255; g = s * 4; r = 0; }
        else if (s < 128) { b = 255 - (s - 64) * 4; g = 255; r = 0; }
        else if (s < 192) { b = 0; g = 255; r = (s - 128) * 4; }
        else { b = 0; g = 255 - (s - 192) * 4; r = 255; }
        
        // Alpha Blend
        uint32_t original_rgb = img_out[i];
        uint8_t R0 = (uint8_t)((original_rgb >> 16) & 0xFF);
        uint8_t G0 = (uint8_t)((original_rgb >> 8) & 0xFF);
        uint8_t B0 = (uint8_t)(original_rgb & 0xFF);
        
        int alpha = HEATMAP_ALPHA;
        uint8_t R_blend = (uint8_t)((R0 * (255 - alpha) + r * alpha) / 255);
        uint8_t G_blend = (uint8_t)((G0 * (255 - alpha) + g * alpha) / 255);
        uint8_t B_blend = (uint8_t)((B0 * (255 - alpha) + b * alpha) / 255);
        
        img_out[i] = (uint32_t)(R_blend << 16 | G_blend << 8 | B_blend);
    }
    
    // 6. Geiger-Werte aus Hot-Pixel-Zählung ableiten (NEUE LOGIK)
    double rawVal = (double)hotPixels / GEIGER_NORM_FACTOR;
    state->currentRadSmooth = (state->currentRadSmooth * (1.0 - GEIGER_SMOOTH_FACTOR)) + (rawVal * GEIGER_SMOOTH_FACTOR);
    state->totalDoseAcc += state->currentRadSmooth * 0.001; // Annahme: Frame-Rate ist ca. 1000ms

    // Historien
    rotate_buffer(state->stats.shortHistory, GRAPH_POINTS, rawVal);
    rotate_buffer(state->stats.avgHistory, GRAPH_POINTS, state->currentRadSmooth);
    state->longTermCounter++;
    if (state->longTermCounter >= 10) {
        rotate_buffer(state->stats.longHistory, GRAPH_POINTS, state->currentRadSmooth);
        state->longTermCounter = 0;
    }

    // Stats packen
    struct timeval tv;
    gettimeofday(&tv, NULL);
    state->stats.currentTimeSec = tv.tv_sec;
    state->stats.currentRad = rawVal;
    state->stats.avgRad = state->currentRadSmooth;
    state->stats.totalDose = state->totalDoseAcc;
    state->stats.alertState = state->currentRadSmooth > GEIGER_ALERT_THRESH;

    // Speicher freigeben
    free(gray);
    free(residue);
    free(edge);
}


// --- SDL2 UI Rendering und Main Loop ---
#define WINDOW_W (IMG_W * 2 + 50)
#define WINDOW_H (IMG_H + 150)

// Funktion zum Zeichnen eines Graphen (vereinfacht)
void draw_graph_sdl(SDL_Renderer *renderer, int x, int y, int w, int h, const double *data, int size, uint32_t color) {
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(renderer, &rect);

    if (size == 0) return;

    double maxVal = 0.001;
    for(int i = 0; i < size; ++i) if(data[i] > maxVal) maxVal = data[i];
    maxVal = fmax(maxVal, 3.0); // Mindest-Skala

    SDL_SetRenderDrawColor(renderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, (color >> 24) & 0xFF);

    double stepX = (double)w / (size - 1);
    for (int i = 1; i < size; ++i) {
        double valNorm1 = fmin(data[i-1] / maxVal, 1.0);
        double valNorm2 = fmin(data[i] / maxVal, 1.0);
        
        int x1 = (int)(x + (i - 1) * stepX);
        int y1 = (int)(y + h - (valNorm1 * h * 0.8)); // Skalierung 0.8
        int x2 = (int)(x + i * stepX);
        int y2 = (int)(y + h - (valNorm2 * h * 0.8));

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}


int main(int argc, char *argv[]) {
    // Puffer für die Bilder
    uint32_t *original_frame = (uint32_t *)malloc(IMG_W * IMG_H * sizeof(uint32_t));
    uint32_t *processed_frame = (uint32_t *)malloc(IMG_W * IMG_H * sizeof(uint32_t));
    
    // Logik-Zustand
    GeigerLogicState logic_state;
    init_geiger_logic(&logic_state);

    // V4L2 initialisieren
    if (v4l2_init() != 0) {
        fprintf(stderr, "Fehler beim Initialisieren von V4L2.\n");
        free(original_frame);
        free(processed_frame);
        cleanup_geiger_logic(&logic_state);
        return 1;
    }
    
    // SDL2 initialisieren
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL konnte nicht initialisiert werden: %s\n", SDL_GetError());
        v4l2_close();
        free(original_frame);
        free(processed_frame);
        cleanup_geiger_logic(&logic_state);
        return 1;
    }
    
    SDL_Window *window = SDL_CreateWindow("qgeister1 (C/V4L2/SDL2) | Original vs. Heatmap",
                                        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                        WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) {
        fprintf(stderr, "Fenster konnte nicht erstellt werden: %s\n", SDL_GetError());
        SDL_Quit();
        v4l2_close();
        free(original_frame);
        free(processed_frame);
        cleanup_geiger_logic(&logic_state);
        return 1;
    }
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *tex_original = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, IMG_W, IMG_H);
    SDL_Texture *tex_processed = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, IMG_W, IMG_H);
    
    if (!renderer || !tex_original || !tex_processed) {
        fprintf(stderr, "Renderer/Texture konnte nicht erstellt werden: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        v4l2_close();
        free(original_frame);
        free(processed_frame);
        cleanup_geiger_logic(&logic_state);
        return 1;
    }
    
    int running = 1;
    SDL_Event event;
    
    // --- Haupt-Loop ---
    while (running) {
        // Events verarbeiten
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = 0;
            }
        }

        // Frame von V4L2 lesen
        if (v4l2_read_frame(original_frame) == 0) {
            // Frame verarbeiten
            process_radionullon(&logic_state, original_frame, processed_frame);

            // Texturen aktualisieren
            SDL_UpdateTexture(tex_original, NULL, original_frame, IMG_W * sizeof(uint32_t));
            SDL_UpdateTexture(tex_processed, NULL, processed_frame, IMG_W * sizeof(uint32_t));
        }

        // Rendering
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        int w_win, h_win;
        SDL_GetWindowSize(window, &w_win, &h_win);
        
        const int gap = 8;
        const int bottomArea = 120;
        int contentW = w_win - gap * 3;
        int contentH = h_win - gap * 3 - bottomArea;
        
        int maxFrameW = contentW / 2;
        int maxFrameH = contentH;
        
        // Skalierung: Original-Aspect beibehalten
        double frameAspect = (double)IMG_W / (double)IMG_H;
        int frameW, frameH;
        if (maxFrameW / frameAspect <= maxFrameH) {
            frameW = maxFrameW;
            frameH = (int)round(maxFrameW / frameAspect);
        } else {
            frameH = maxFrameH;
            frameW = (int)round(maxFrameH * frameAspect);
        }

        int yOffset = gap + (contentH - frameH) / 2;
        int leftX = gap;
        int rightX = gap * 2 + frameW;

        // 1. Linkes Bild: Original
        SDL_Rect dst_rect_original = {leftX, yOffset, frameW, frameH};
        SDL_RenderCopy(renderer, tex_original, NULL, &dst_rect_original);

        // 2. Rechtes Bild: Processed
        SDL_Rect dst_rect_processed = {rightX, yOffset, frameW, frameH};
        SDL_RenderCopy(renderer, tex_processed, NULL, &dst_rect_processed);

        // HIER MÜSSTEN DIE SDL_TTF/GFX FÜR TEXT UND GRAPHDARSTELLUNG FOLGEN
        // (Wurde weggelassen, da es ein eigener grosser Schritt ist)

        // Vereinfachte Graphen-Platzhalter
        int graphH = 80;
        int margin = 10;
        int graphsAreaW = w_win - (4 * margin);
        int singleGraphW = (graphsAreaW / 3) > 100 ? (graphsAreaW / 3) : 100;
        int yPos = h_win - graphH - margin;

        draw_graph_sdl(renderer, margin, yPos, singleGraphW, graphH, logic_state.stats.longHistory, GRAPH_POINTS, 0xFF0064FF); // Blau
        draw_graph_sdl(renderer, margin*2 + singleGraphW, yPos, singleGraphW, graphH, logic_state.stats.avgHistory, GRAPH_POINTS, 0xFF00C8FF); // Gelb
        uint32_t peak_color = logic_state.stats.alertState ? 0xFF0000FF : 0xFF00FF00;
        draw_graph_sdl(renderer, margin*3 + singleGraphW*2, yPos, singleGraphW, graphH, logic_state.stats.shortHistory, GRAPH_POINTS, peak_color);

        SDL_RenderPresent(renderer);

        // FPS-Limit (einfach, ca. 30 FPS)
        SDL_Delay(33); 
    }

    // Aufräumen
    SDL_DestroyTexture(tex_original);
    SDL_DestroyTexture(tex_processed);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    v4l2_close();
    free(original_frame);
    free(processed_frame);
    cleanup_geiger_logic(&logic_state);

    return 0;
}

