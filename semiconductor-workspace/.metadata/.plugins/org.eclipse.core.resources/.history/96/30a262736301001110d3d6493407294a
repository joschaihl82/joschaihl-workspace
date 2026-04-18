// File: semiconductor_ca_flow.c
// Compile: gcc -O2 -o semiconductor_ca_flow semiconductor_ca_flow.c `sdl2-config --cflags --libs` -lSDL2_ttf -lm
//
// Beschreibung:
// Erweiterter zellulärer Automat (8-bit Bitmap) mit Visualisierung der Stromrichtung:
// - Es gibt ein 1D-Bitmap (GRID_W x GRID_H) mit Zelltypen BT_I, BT_S, BT_C, BT_P, BT_M.
// - Einfache "Ladungen" (charges) werden an Leiterzellen (BT_C) neben Pluspolen (BT_P) erzeugt.
// - Jede Simulationsiteration wird eine Distanzkarte zu allen Minuspolen (BT_M) per BFS berechnet.
// - Ladungen bewegen sich pro Schritt genau ein Feld in Richtung eines Nachbarn mit kleinerer Distanz
//   (also zielgerichtet von Plus nach Minus).
// - Die Bewegung wird visualisiert: auf jeder Leiterzelle mit Ladung wird ein kleiner, verschobener
//   Rechteck-Overlay gezeichnet, das die Bewegungsrichtung anzeigt.
//
// Steuerung:
// - Linksklick in Toolbox wählt Werkzeug (Isolator, Halbleiter, Leiter, Plus, Minus).
// - Linksklick im Gitter setzt die Zelle auf das aktuelle Werkzeug.
// - Space startet/pausiert die Simulation.
// - C setzt das Feld zurück (Standardhintergrund: Isolator).
// - +/- Zoom in/out.
// - Brush und Linienwerkzeug wurden entfernt; Fokus liegt auf CA + Flussvisualisierung.
//
// Hinweis: Die Simulationsregeln sind bewusst einfach gehalten, um die Flussvisualisierung klar zu zeigen.

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#define GRID_W 160
#define GRID_H 120

#define CELL_MIN 2
#define CELL_MAX 20
#define CELL_DEFAULT 5

#define TOOLBOX_W 160

enum {
    BT_I = 0, // Isolator
    BT_S = 1, // Halbleiter
    BT_C = 2, // Leiter
    BT_P = 3, // Pluspol
    BT_M = 4  // Minuspol
};

static SDL_Color COLORS[] = {
    {80, 80, 80, 255},    // I Grau
    {0, 180, 0, 255},     // S Grün
    {255, 255, 0, 255},   // C Gelb
    {255, 0, 0, 255},     // P Rot
    {0, 0, 255, 255}      // M Blau
};

const char* TOOL_LABELS[] = {
    "Isolator",
    "Halbleiter",
    "Leiter",
    "Pluspol",
    "Minuspol"
};

typedef struct {
    int cell_size;
    int window_w;
    int window_h;
    SDL_Window* win;
    SDL_Renderer* ren;
    TTF_Font* font;
    uint8_t bitmap[GRID_W * GRID_H]; // 1D 8-bit per cell
    uint8_t charge[GRID_W * GRID_H]; // 0/1: presence of moving charge
    int dist[GRID_W * GRID_H];       // distance to nearest M (BFS)
    int8_t dir[GRID_W * GRID_H];     // movement direction for visualization: -1 none, 0=right,1=left,2=down,3=up
    int current_tool;
    bool running;
} AppState;

static inline int idx(int x, int y) { return y * GRID_W + x; }

static void init_bitmap(AppState* app) {
    // Standardhintergrund: Isolator
    for (int i = 0; i < GRID_W * GRID_H; ++i) {
        app->bitmap[i] = BT_I;
        app->charge[i] = 0;
        app->dist[i] = 0x3fffffff;
        app->dir[i] = -1;
    }
    // Polstreifen
    for (int y = 0; y < GRID_H; ++y) {
        app->bitmap[idx(5, y)] = BT_P;
        app->bitmap[idx(GRID_W - 6, y)] = BT_M;
    }
    // ein paar Halbleiter zufällig
    srand((unsigned)time(NULL));
    for (int i = 0; i < 800; ++i) {
        int x = rand() % GRID_W;
        int y = rand() % GRID_H;
        if ((rand() & 3) == 0) app->bitmap[idx(x, y)] = BT_S;
    }
}

// BFS: berechne Manhattan-Distanz (4-neighborhood) zu nächsten Minuspol (BT_M)
static void compute_dist_to_minus(AppState* app) {
    const int N = GRID_W * GRID_H;
    // initialize
    for (int i = 0; i < N; ++i) {
        app->dist[i] = 0x3fffffff;
    }
    // queue
    int *q = (int*)malloc(sizeof(int) * N);
    if (!q) return;
    int qh = 0, qt = 0;
    // push all M cells
    for (int y = 0; y < GRID_H; ++y) {
        for (int x = 0; x < GRID_W; ++x) {
            if (app->bitmap[idx(x,y)] == BT_M) {
                app->dist[idx(x,y)] = 0;
                q[qt++] = idx(x,y);
            }
        }
    }
    // BFS 4-neighbors
    while (qh < qt) {
        int cur = q[qh++];
        int cx = cur % GRID_W;
        int cy = cur / GRID_W;
        int cd = app->dist[cur];
        const int nx[4] = {1,-1,0,0};
        const int ny[4] = {0,0,1,-1};
        for (int k = 0; k < 4; ++k) {
            int nxp = cx + nx[k];
            int nyp = cy + ny[k];
            if (nxp < 0 || nxp >= GRID_W || nyp < 0 || nyp >= GRID_H) continue;
            int ni = idx(nxp, nyp);
            if (app->dist[ni] > cd + 1) {
                app->dist[ni] = cd + 1;
                q[qt++] = ni;
            }
        }
    }
    free(q);
}

// Spawn charges at conductor cells adjacent to Plus (BT_P)
static void spawn_charges(AppState* app) {
    for (int y = 0; y < GRID_H; ++y) {
        for (int x = 0; x < GRID_W; ++x) {
            if (app->bitmap[idx(x,y)] == BT_P) {
                // for each 4-neighbor that is conductor, spawn a charge if none present
                const int nx[4] = {1,-1,0,0};
                const int ny[4] = {0,0,1,-1};
                for (int k = 0; k < 4; ++k) {
                    int nxp = x + nx[k];
                    int nyp = y + ny[k];
                    if (nxp < 0 || nxp >= GRID_W || nyp < 0 || nyp >= GRID_H) continue;
                    int ni = idx(nxp, nyp);
                    if (app->bitmap[ni] == BT_C && app->charge[ni] == 0) {
                        app->charge[ni] = 1;
                        app->dir[ni] = -1; // direction will be set on movement
                    }
                }
            }
        }
    }
}

// Move charges one step towards smaller dist (towards minus). Only move through BT_C or into BT_M.
// Resolve conflicts by first-come (scan order) and preventing multiple charges into same cell.
static void move_charges(AppState* app) {
    int N = GRID_W * GRID_H;
    uint8_t *new_charge = (uint8_t*)calloc(N, 1);
    int8_t *new_dir = (int8_t*)malloc(N);
    if (!new_charge || !new_dir) {
        free(new_charge);
        free(new_dir);
        return;
    }
    for (int i = 0; i < N; ++i) new_dir[i] = -1;

    // For each cell with charge, pick neighbor with strictly smaller dist and valid type
    for (int y = 0; y < GRID_H; ++y) {
        for (int x = 0; x < GRID_W; ++x) {
            int i = idx(x,y);
            if (app->charge[i] == 0) continue;
            int curd = app->dist[i];
            int bestd = curd;
            int bestx = x, besty = y;
            int bestdir = -1;
            // neighbors: right(0), left(1), down(2), up(3)
            const int nx[4] = {1,-1,0,0};
            const int ny[4] = {0,0,1,-1};
            for (int k = 0; k < 4; ++k) {
                int nxp = x + nx[k];
                int nyp = y + ny[k];
                if (nxp < 0 || nxp >= GRID_W || nyp < 0 || nyp >= GRID_H) continue;
                int ni = idx(nxp, nyp);
                // can move into conductor or minus pole
                if (app->bitmap[ni] == BT_C || app->bitmap[ni] == BT_M) {
                    int nd = app->dist[ni];
                    if (nd < bestd) {
                        bestd = nd;
                        bestx = nxp;
                        besty = nyp;
                        bestdir = k;
                    }
                }
            }
            int ti = idx(bestx, besty);
            if (ti != i && new_charge[ti] == 0) {
                // move to target
                new_charge[ti] = 1;
                new_dir[ti] = (int8_t)bestdir;
            } else {
                // stay in place if cannot move
                if (new_charge[i] == 0) {
                    new_charge[i] = 1;
                    new_dir[i] = -1; // no movement this step
                }
            }
        }
    }

    // copy back
    memcpy(app->charge, new_charge, N);
    for (int i = 0; i < N; ++i) app->dir[i] = new_dir[i];

    free(new_charge);
    free(new_dir);
}

// Eine Simulationsiteration: einfache CA-Regeln + charge spawning + movement
static void step_simulation(AppState* app) {
    // 1) simple CA update (keeps P, M, I fixed; S <-> C transitions simplified)
    uint8_t copy[GRID_W * GRID_H];
    memcpy(copy, app->bitmap, sizeof(copy));
    for (int y = 0; y < GRID_H; ++y) {
        for (int x = 0; x < GRID_W; ++x) {
            int i = idx(x,y);
            uint8_t orig = copy[i];
            if (orig == BT_I || orig == BT_P || orig == BT_M) continue;
            // count neighbors
            int cntP=0,cntM=0,cntC=0;
            for (int dy=-1; dy<=1; ++dy) for (int dx=-1; dx<=1; ++dx) {
                if (dx==0 && dy==0) continue;
                int nx = x+dx, ny = y+dy;
                if (nx<0||nx>=GRID_W||ny<0||ny>=GRID_H) continue;
                uint8_t t = copy[idx(nx,ny)];
                if (t==BT_P) cntP++;
                else if (t==BT_M) cntM++;
                else if (t==BT_C) cntC++;
            }
            if (orig == BT_S) {
                if (cntC >= 3 || (cntP+cntM)>=4) app->bitmap[i] = BT_C;
                else app->bitmap[i] = BT_S;
            } else if (orig == BT_C) {
                if (cntC==0 && cntP==0 && cntM==0) {
                    if ((rand()%100) < 5) app->bitmap[i] = BT_S;
                    else app->bitmap[i] = BT_C;
                } else {
                    app->bitmap[i] = BT_C;
                }
            }
        }
    }

    // 2) compute distance to minus poles
    compute_dist_to_minus(app);

    // 3) spawn charges at conductors adjacent to plus poles
    spawn_charges(app);

    // 4) move charges one step towards minus
    move_charges(app);
}

// draw small overlay for charge direction inside cell
static void draw_charge_overlay(AppState* app, int x, int y, int dir) {
    int cs = app->cell_size;
    int cx = x * cs;
    int cy = y * cs;
    int w = cs / 2;
    int h = cs / 2;
    if (w < 2) w = 2;
    if (h < 2) h = 2;
    int ox = cx + (cs - w) / 2;
    int oy = cy + (cs - h) / 2;
    int shift = cs / 4;
    if (dir == 0) ox += shift;      // right
    else if (dir == 1) ox -= shift; // left
    else if (dir == 2) oy += shift; // down
    else if (dir == 3) oy -= shift; // up
    SDL_Rect r = { ox, oy, w, h };
    SDL_SetRenderDrawColor(app->ren, 255, 255, 255, 255);
    SDL_RenderFillRect(app->ren, &r);
}

// Rendering
static void draw_text(AppState* app, const char* text, int x, int y, SDL_Color color) {
    if (!app->font) return;
    SDL_Surface* surf = TTF_RenderUTF8_Blended(app->font, text, color);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(app->ren, surf);
    SDL_Rect dst = { x, y, surf->w, surf->h };
    SDL_FreeSurface(surf);
    if (tex) {
        SDL_RenderCopy(app->ren, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
    }
}

static void render(AppState* app) {
    SDL_SetRenderDrawColor(app->ren, 20, 20, 20, 255);
    SDL_RenderClear(app->ren);

    // draw grid cells
    for (int y = 0; y < GRID_H; ++y) {
        for (int x = 0; x < GRID_W; ++x) {
            uint8_t v = app->bitmap[idx(x,y)];
            SDL_Color col = COLORS[v];
            SDL_SetRenderDrawColor(app->ren, col.r, col.g, col.b, 255);
            SDL_Rect r = { x * app->cell_size, y * app->cell_size, app->cell_size, app->cell_size };
            SDL_RenderFillRect(app->ren, &r);
        }
    }

    // toolbox background
    SDL_SetRenderDrawColor(app->ren, 40, 40, 40, 255);
    SDL_Rect toolbox = { GRID_W * app->cell_size, 0, TOOLBOX_W, app->window_h };
    SDL_RenderFillRect(app->ren, &toolbox);

    // tools
    for (int i = 0; i < 5; ++i) {
        SDL_Color c = COLORS[i];
        SDL_SetRenderDrawColor(app->ren, c.r, c.g, c.b, 255);
        SDL_Rect r = { GRID_W * app->cell_size + 16, 16 + i * 56, 40, 40 };
        SDL_RenderFillRect(app->ren, &r);
        if (i == app->current_tool) {
            SDL_SetRenderDrawColor(app->ren, 255, 255, 255, 255);
            SDL_Rect br = { r.x - 2, r.y - 2, r.w + 4, r.h + 4 };
            SDL_RenderDrawRect(app->ren, &br);
        }
        SDL_Color white = { 230, 230, 230, 255 };
        draw_text(app, TOOL_LABELS[i], GRID_W * app->cell_size + 64, 24 + i * 56, white);
    }

    // buttons
    SDL_Rect btn_start = { GRID_W * app->cell_size + 16, app->window_h - 160, 120, 36 };
    SDL_Rect btn_step  = { GRID_W * app->cell_size + 16, app->window_h - 116, 120, 36 };
    SDL_Rect btn_zoom_in  = { GRID_W * app->cell_size + 16, app->window_h - 72, 56, 36 };
    SDL_Rect btn_zoom_out = { GRID_W * app->cell_size + 80, app->window_h - 72, 56, 36 };
    SDL_Rect btn_clear = { GRID_W * app->cell_size + 16, app->window_h - 32, 120, 24 };

    SDL_SetRenderDrawColor(app->ren, 70, 70, 70, 255);
    SDL_RenderFillRect(app->ren, &btn_start);
    SDL_RenderFillRect(app->ren, &btn_step);
    SDL_RenderFillRect(app->ren, &btn_zoom_in);
    SDL_RenderFillRect(app->ren, &btn_zoom_out);
    SDL_RenderFillRect(app->ren, &btn_clear);

    SDL_Color white = { 230, 230, 230, 255 };
    draw_text(app, app->running ? "Pause" : "Start", btn_start.x + 18, btn_start.y + 8, white);
    draw_text(app, "Step", btn_step.x + 40, btn_step.y + 8, white);
    draw_text(app, "Zoom+", btn_zoom_in.x + 6, btn_zoom_in.y + 8, white);
    draw_text(app, "Zoom-", btn_zoom_out.x + 6, btn_zoom_out.y + 8, white);
    draw_text(app, "Clear", btn_clear.x + 36, btn_clear.y + 2, white);

    // draw charges overlay (direction)
    for (int y = 0; y < GRID_H; ++y) {
        for (int x = 0; x < GRID_W; ++x) {
            int i = idx(x,y);
            if (app->charge[i]) {
                // draw small bright overlay; if dir known, shift it to indicate movement direction
                draw_charge_overlay(app, x, y, app->dir[i]);
            }
        }
    }

    SDL_RenderPresent(app->ren);
}

static bool point_in_rect(int px, int py, SDL_Rect* r) {
    return (px >= r->x && px < r->x + r->w && py >= r->y && py < r->y + r->h);
}

static void update_window_size(AppState* app) {
    app->window_w = GRID_W * app->cell_size + TOOLBOX_W;
    app->window_h = GRID_H * app->cell_size;
    SDL_SetWindowSize(app->win, app->window_w, app->window_h);
}


int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init() != 0) {
        fprintf(stderr, "TTF_Init error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    AppState app;
    app.cell_size = CELL_DEFAULT;
    app.current_tool = BT_C;
    app.running = false;
    app.font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 14);
    if (!app.font) app.font = NULL;

    app.window_w = GRID_W * app.cell_size + TOOLBOX_W;
    app.window_h = GRID_H * app.cell_size;

    app.win = SDL_CreateWindow("Zellularautomat mit Flussvisualisierung",
                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               app.window_w, app.window_h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!app.win) {
        fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    app.ren = SDL_CreateRenderer(app.win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!app.ren) {
        fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
        SDL_DestroyWindow(app.win);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    init_bitmap(&app);

    bool quit = false;
    int last_gx = -1, last_gy = -1;

    Uint32 last_step = SDL_GetTicks();
    const Uint32 step_interval = 120; // ms per simulation step

    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    int w = e.window.data1;
                    int h = e.window.data2;
                    app.window_w = w;
                    app.window_h = h;
                }
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x;
                int my = e.button.y;
                int logical_x = 0, logical_y = 0, gx = -1, gy = -1;
                // map window coords to logical grid coords
                int logical_w = GRID_W * app.cell_size + TOOLBOX_W;
                int logical_h = GRID_H * app.cell_size;
                int win_w = 0, win_h = 0;
                SDL_GetWindowSize(app.win, &win_w, &win_h);
                if (win_w <= 0) win_w = 1;
                if (win_h <= 0) win_h = 1;
                float sx = (float)logical_w / (float)win_w;
                float sy = (float)logical_h / (float)win_h;
                float lx = mx * sx;
                float ly = my * sy;
                logical_x = (int)lx;
                logical_y = (int)ly;
                gx = (int)(lx) / app.cell_size;
                gy = (int)(ly) / app.cell_size;

                if (logical_x >= 0 && logical_x < GRID_W * app.cell_size &&
                    logical_y >= 0 && logical_y < GRID_H * app.cell_size &&
                    gx >= 0 && gx < GRID_W && gy >= 0 && gy < GRID_H) {
                    // set cell to current tool
                    app.bitmap[idx(gx,gy)] = (uint8_t)app.current_tool;
                    // clear any charge there
                    app.charge[idx(gx,gy)] = 0;
                    app.dir[idx(gx,gy)] = -1;
                } else {
                    // toolbox / buttons
                    int lx_i = logical_x;
                    int ly_i = logical_y;
                    for (int i = 0; i < 5; ++i) {
                        SDL_Rect r = { GRID_W * app.cell_size + 16, 16 + i * 56, 40, 40 };
                        if (point_in_rect(lx_i, ly_i, &r)) {
                            app.current_tool = i;
                        }
                    }
                    SDL_Rect btn_start = { GRID_W * app.cell_size + 16, app.window_h - 160, 120, 36 };
                    SDL_Rect btn_step  = { GRID_W * app.cell_size + 16, app.window_h - 116, 120, 36 };
                    SDL_Rect btn_zoom_in  = { GRID_W * app.cell_size + 16, app.window_h - 72, 56, 36 };
                    SDL_Rect btn_zoom_out = { GRID_W * app.cell_size + 80, app.window_h - 72, 56, 36 };
                    SDL_Rect btn_clear = { GRID_W * app.cell_size + 16, app.window_h - 32, 120, 24 };

                    if (point_in_rect(lx_i, ly_i, &btn_start)) {
                        app.running = !app.running;
                    } else if (point_in_rect(lx_i, ly_i, &btn_step)) {
                        step_simulation(&app);
                    } else if (point_in_rect(lx_i, ly_i, &btn_zoom_in)) {
                        if (app.cell_size < CELL_MAX) { app.cell_size++; update_window_size(&app); }
                    } else if (point_in_rect(lx_i, ly_i, &btn_zoom_out)) {
                        if (app.cell_size > CELL_MIN) { app.cell_size--; update_window_size(&app); }
                    } else if (point_in_rect(lx_i, ly_i, &btn_clear)) {
                        init_bitmap(&app);
                    }
                }
            } else if (e.type == SDL_MOUSEMOTION && (e.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT))) {
                // drag painting
                int mx = e.motion.x;
                int my = e.motion.y;
                int logical_w = GRID_W * app.cell_size + TOOLBOX_W;
                int logical_h = GRID_H * app.cell_size;
                int win_w = 0, win_h = 0;
                SDL_GetWindowSize(app.win, &win_w, &win_h);
                if (win_w <= 0) win_w = 1;
                if (win_h <= 0) win_h = 1;
                float sx = (float)logical_w / (float)win_w;
                float sy = (float)logical_h / (float)win_h;
                float lx = mx * sx;
                float ly = my * sy;
                int gx = (int)(lx) / app.cell_size;
                int gy = (int)(ly) / app.cell_size;
                if (gx >= 0 && gx < GRID_W && gy >= 0 && gy < GRID_H) {
                    app.bitmap[idx(gx,gy)] = (uint8_t)app.current_tool;
                    app.charge[idx(gx,gy)] = 0;
                    app.dir[idx(gx,gy)] = -1;
                }
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_SPACE) {
                    app.running = !app.running;
                } else if (e.key.keysym.sym == SDLK_c) {
                    init_bitmap(&app);
                } else if (e.key.keysym.sym == SDLK_EQUALS || e.key.keysym.sym == SDLK_PLUS) {
                    if (app.cell_size < CELL_MAX) { app.cell_size++; update_window_size(&app); }
                } else if (e.key.keysym.sym == SDLK_MINUS) {
                    if (app.cell_size > CELL_MIN) { app.cell_size--; update_window_size(&app); }
                } else if (e.key.keysym.sym == SDLK_1) {
                    app.current_tool = BT_I;
                } else if (e.key.keysym.sym == SDLK_2) {
                    app.current_tool = BT_S;
                } else if (e.key.keysym.sym == SDLK_3) {
                    app.current_tool = BT_C;
                } else if (e.key.keysym.sym == SDLK_4) {
                    app.current_tool = BT_P;
                } else if (e.key.keysym.sym == SDLK_5) {
                    app.current_tool = BT_M;
                }
            }
        }

        Uint32 now = SDL_GetTicks();
        if (app.running && now - last_step >= step_interval) {
            step_simulation(&app);
            last_step = now;
        }

        render(&app);
        SDL_Delay(8);
    }

    if (app.font) TTF_CloseFont(app.font);
    TTF_Quit();
    SDL_DestroyRenderer(app.ren);
    SDL_DestroyWindow(app.win);
    SDL_Quit();
    return 0;
}
