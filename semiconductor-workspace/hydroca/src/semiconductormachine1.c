// hydro_ca_sdl2_thick_loop_noinput.c
// Kompiliere mit: gcc -O2 -o hydro_noinput hydro_ca_sdl2_thick_loop_noinput.c `sdl2-config --cflags --libs` -lm

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#define W 220
#define H 140
#define CELL_SIZE 6

// pipe mask bits
#define P_N 1
#define P_E 2
#define P_S 4
#define P_W 8

// Simulation parameters (anpassbar)
const double C = 0.22;           // Flusskoeffizient (Leitfähigkeit)
const double dt = 0.5;           // Zeitschritt
const double damping = 0.998;    // leichte Dämpfung
const double wave_threshold = 0.03; // Schwelle für dunkle Wellenkante
double max_h_display = 2.0;      // max Höhe für Farbskalierung (erhöht für bessere Sichtbarkeit)

// Grid
static uint8_t pipe_mask[H][W];
static uint8_t pipe_present[H][W];
static double h[H][W];
static double h_next[H][W];

// Hilfsfunktionen
static inline int in_bounds(int y, int x) { return x>=0 && x<W && y>=0 && y<H; }

void clear_grid() {
    memset(pipe_mask, 0, sizeof(pipe_mask));
    memset(pipe_present, 0, sizeof(pipe_present));
    for (int y=0;y<H;y++) for (int x=0;x<W;x++) { h[y][x]=0.0; h_next[y][x]=0.0; }
}

// setze Pipe-Präsenz (einzelne Zelle)
void set_pipe_cell(int y, int x) {
    if (!in_bounds(y,x)) return;
    pipe_present[y][x] = 1;
}

// setze rechteckige dicke Leitung (x..x+w-1, y..y+h-1)
void set_pipe_rect(int y0, int x0, int hrect, int wrect) {
    for (int y=y0; y<y0+hrect; y++) for (int x=x0; x<x0+wrect; x++) set_pipe_cell(y,x);
}

// setze Rohr entlang einer param. Kurve (Kreis/Bogen) mit diskreten Punkten
void set_pipe_arc(int cy, int cx, int radius, double start_ang, double end_ang, int thickness) {
    for (int t=0; t<thickness; t++) {
        double r = radius + t;
        int steps = (int)(r * fabs(end_ang - start_ang) * 1.5) + 8;
        for (int i=0;i<=steps;i++) {
            double a = start_ang + (end_ang - start_ang) * ((double)i / (double)steps);
            int x = (int)round(cx + r * cos(a));
            int y = (int)round(cy + r * sin(a));
            set_pipe_cell(y,x);
        }
    }
}

// nach dem Setzen von pipe_present: berechne pipe_mask (Verbindungen)
void compute_pipe_mask_from_present() {
    memset(pipe_mask, 0, sizeof(pipe_mask));
    for (int y=0;y<H;y++) {
        for (int x=0;x<W;x++) {
            if (!pipe_present[y][x]) continue;
            uint8_t m = 0;
            if (in_bounds(y-1,x) && pipe_present[y-1][x]) m |= P_N;
            if (in_bounds(y,x+1) && pipe_present[y][x+1]) m |= P_E;
            if (in_bounds(y+1,x) && pipe_present[y+1][x]) m |= P_S;
            if (in_bounds(y,x-1) && pipe_present[y][x-1]) m |= P_W;
            pipe_mask[y][x] = m;
        }
    }
}

// Erzeuge ein Netzwerk mit dicken Anfangs- und Endleitungen und Kreisrückführung
void build_example_network_thick_loop() {
    clear_grid();

    int mid = H/2;
    // Dicke linke Anfangsleitung: 10 Zellen breit, 24 Zellen hoch, links bei x=4
    int thick = 10; // 10 Zellen dick
    int left_x = 4;
    int left_h = 24;
    int left_y = mid - left_h/2;
    set_pipe_rect(left_y, left_x, left_h, thick);

    // Dicke rechte Endleitung: 10 Zellen breit, 24 Zellen hoch, rechts bei x = W - 4 - thick
    int right_x = W - 4 - thick;
    int right_h = 24;
    int right_y = mid - right_h/2;
    set_pipe_rect(right_y, right_x, right_h, thick);

    // Verbinde linke dicke Leitung mit einer horizontalen Hauptleitung (mittig)
    int main_y = mid;
    int main_x0 = left_x + thick;
    int main_x1 = right_x - thick;
    for (int x = main_x0; x <= main_x1; x++) set_pipe_cell(main_y, x);

    // Erzeuge einige Verzweigungen oben/unten (dünnere Äste)
    for (int x = main_x0 + 8; x < main_x1 - 8; x += 28) {
        // oberer Ast
        for (int y = main_y-1; y > main_y-12; y--) set_pipe_cell(y, x);
        // unterer Ast
        for (int y = main_y+1; y < main_y+12; y++) set_pipe_cell(y, x+6);
    }

    // Kreisförmige Rückführung: Bogen oben und unten, verbindet rechte dicke Leitung zurück zur linken
    int cx = (left_x + right_x) / 2;
    int cy = mid;
    int radius = (right_x - left_x) / 2 + 6;
    // oberer Bogen
    set_pipe_arc(cy, cx, radius, -M_PI*0.9, -M_PI*0.1, 3);
    // unterer Bogen
    set_pipe_arc(cy, cx, radius, M_PI*0.1, M_PI*0.9, 3);

    // kleine Verbreiterungen an Übergängen (sanftere Verbindung)
    for (int dx=0; dx<4; dx++) {
        set_pipe_rect(left_y-2, main_x0-dx, 4, 2);
        set_pipe_rect(right_y-2, main_x1+dx-1, 4, 2);
    }

    // compute connectivity bits
    compute_pipe_mask_from_present();

    // Initiale Wasserverteilung: viel Wasser in der linken dicken Leitung (klar sichtbar)
    for (int y = left_y; y < left_y + left_h; y++) {
        for (int x = left_x; x < left_x + thick; x++) {
            h[y][x] = 1.6 + 0.4 * (0.5 - ((double)(y - left_y) / (double)left_h)); // 1.2..2.0
        }
    }
    // etwas Wasser in Hauptleitung
    for (int x = main_x0; x < main_x0 + 30; x++) h[main_y][x] = 1.0;
    // Störungen um Wellen zu erzeugen
    h[main_y-6][main_x0+6] += 0.8;
    h[main_y+8][main_x0+12] += 0.6;
}

// Simulation: ein Zeitschritt
void step_simulation() {
    // Nullsetzen
    for (int y=0;y<H;y++) for (int x=0;x<W;x++) h_next[y][x] = h[y][x];

    // Wir berechnen Flüsse paarweise, um Konservierung zu gewährleisten.
    for (int y=0;y<H;y++) {
        for (int x=0;x<W;x++) {
            if (!pipe_mask[y][x]) continue;
            double hi = h[y][x];
            // N
            if (pipe_mask[y][x] & P_N) {
                int ny=y-1, nx=x;
                if (in_bounds(ny,nx) && (pipe_mask[ny][nx] & P_S)) {
                    double hj = h[ny][nx];
                    double diff = hi - hj;
                    if (diff > 0) {
                        double F = C * diff * dt;
                        double maxmove = hi;
                        if (F > maxmove) F = maxmove;
                        h_next[y][x] -= F;
                        h_next[ny][nx] += F;
                    }
                }
            }
            // E
            if (pipe_mask[y][x] & P_E) {
                int ny=y, nx=x+1;
                if (in_bounds(ny,nx) && (pipe_mask[ny][nx] & P_W)) {
                    double hj = h[ny][nx];
                    double diff = hi - hj;
                    if (diff > 0) {
                        double F = C * diff * dt;
                        double maxmove = hi;
                        if (F > maxmove) F = maxmove;
                        h_next[y][x] -= F;
                        h_next[ny][nx] += F;
                    }
                }
            }
            // S
            if (pipe_mask[y][x] & P_S) {
                int ny=y+1, nx=x;
                if (in_bounds(ny,nx) && (pipe_mask[ny][nx] & P_N)) {
                    double hj = h[ny][nx];
                    double diff = hi - hj;
                    if (diff > 0) {
                        double F = C * diff * dt;
                        double maxmove = hi;
                        if (F > maxmove) F = maxmove;
                        h_next[y][x] -= F;
                        h_next[ny][nx] += F;
                    }
                }
            }
            // W
            if (pipe_mask[y][x] & P_W) {
                int ny=y, nx=x-1;
                if (in_bounds(ny,nx) && (pipe_mask[ny][nx] & P_E)) {
                    double hj = h[ny][nx];
                    double diff = hi - hj;
                    if (diff > 0) {
                        double F = C * diff * dt;
                        double maxmove = hi;
                        if (F > maxmove) F = maxmove;
                        h_next[y][x] -= F;
                        h_next[ny][nx] += F;
                    }
                }
            }
        }
    }

    // Dämpfung und Kopieren zurück
    for (int y=0;y<H;y++) {
        for (int x=0;x<W;x++) {
            if (!pipe_mask[y][x]) { h[y][x]=0.0; continue; }
            if (h_next[y][x] < 0.0) h_next[y][x] = 0.0;
            h[y][x] = h_next[y][x] * damping;
            if (h[y][x] > 10.0) h[y][x] = 10.0;
        }
    }
}

// Zeichnen
void render(SDL_Renderer *ren) {
    // Hintergrund schwarz
    SDL_SetRenderDrawColor(ren, 0,0,0,255);
    SDL_RenderClear(ren);

    // Zellen zeichnen
    for (int y=0;y<H;y++) {
        for (int x=0;x<W;x++) {
            int sx = x * CELL_SIZE;
            int sy = y * CELL_SIZE;
            SDL_Rect cell = { sx, sy, CELL_SIZE, CELL_SIZE };

            if (pipe_mask[y][x]) {
                // Rohrgrundfarbe: weiß (leicht grauer Rand für Kontrast)
                SDL_SetRenderDrawColor(ren, 240,240,240,255);
                SDL_RenderFillRect(ren, &cell);

                // Wasser: kräftiges Blau, Höhe skaliert
                double hh = h[y][x];
                double frac = hh / max_h_display;
                if (frac > 1.0) frac = 1.0;
                if (frac > 0.0001) {
                    int water_h_px = (int)round(frac * CELL_SIZE);
                    SDL_Rect water_rect = { sx, sy + (CELL_SIZE - water_h_px), CELL_SIZE, water_h_px };
                    // Farbskala: kräftiges Blau -> tieferes Blau
                    int r = 0;
                    int g = (int)(110 + (1.0-frac)*40);  // 110..150
                    int b = (int)(200 + frac*55);        // 200..255
                    SDL_SetRenderDrawColor(ren, r,g,b,255);
                    SDL_RenderFillRect(ren, &water_rect);

                    // Wellenkante: wenn starker Gradient zu einem Nachbarn
                    double max_grad = 0.0;
                    const int dy[4] = {-1,0,1,0};
                    const int dx[4] = {0,1,0,-1};
                    for (int k=0;k<4;k++) {
                        int ny=y+dy[k], nx=x+dx[k];
                        if (!in_bounds(ny,nx)) continue;
                        if (!(pipe_mask[ny][nx])) continue;
                        double grad = fabs(h[y][x] - h[ny][nx]);
                        if (grad > max_grad) max_grad = grad;
                    }
                    if (max_grad > wave_threshold) {
                        // zeichne dunkle Kante oben der Wasserfläche (dicker)
                        SDL_SetRenderDrawColor(ren, 0,0,140,255); // dunkelblau
                        SDL_Rect edge = { sx, sy + (CELL_SIZE - water_h_px), CELL_SIZE, 2 };
                        if (edge.y < sy) edge.y = sy;
                        SDL_RenderFillRect(ren, &edge);
                    }
                }
            }
        }
    }

    SDL_RenderPresent(ren);
}

int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Fehler: %s\n", SDL_GetError());
        return 1;
    }

    int win_w = W * CELL_SIZE;
    int win_h = H * CELL_SIZE;
    SDL_Window *win = SDL_CreateWindow("Hydrodynamischer Zellularautomat - autonom",
                                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       win_w, win_h, SDL_WINDOW_SHOWN);
    if (!win) {
        fprintf(stderr, "SDL_CreateWindow Fehler: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!ren) {
        fprintf(stderr, "SDL_CreateRenderer Fehler: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    build_example_network_thick_loop();

    int running = 1;
    Uint32 last = SDL_GetTicks();
    double accumulator = 0.0;
    const double sim_step_ms = 16.0; // ~60 Hz visual
    while (running) {
        SDL_Event ev;
        // Nur SDL_QUIT verarbeiten; alle anderen Events ignorieren (keine Interaktion)
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = 0;
        }

        Uint32 now = SDL_GetTicks();
        double elapsed = (double)(now - last);
        last = now;
        accumulator += elapsed;

        // mehrere Simulationsschritte, falls nötig
        while (accumulator >= sim_step_ms) {
            step_simulation();
            accumulator -= sim_step_ms;
        }

        render(ren);
        SDL_Delay(8); // kleine Pause, CPU schonen
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
