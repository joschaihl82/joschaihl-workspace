// File: paint_clone_sdl.c
// Compile (Linux):
//   sudo apt install libsdl2-dev libsdl2-ttf-dev
//   gcc paint_clone_sdl.c -o paint_clone_sdl -lSDL2 -lSDL2_ttf
//
// Windows/MinGW: link against SDL2 and SDL2_ttf accordingly.

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define WIN_W 1200
#define WIN_H 800

// Canvas area (left)
#define TOOLBAR_W 200
#define STATUS_H 24
#define CANVAS_X 0
#define CANVAS_Y 0
#define CANVAS_W (WIN_W - TOOLBAR_W)
#define CANVAS_H (WIN_H - STATUS_H)

// Palette
#define PALETTE_COLS 4
#define PALETTE_ROWS 6
#define PALETTE_CELL 28
#define PALETTE_MARGIN 10

typedef enum {
    TOOL_PENCIL,
    TOOL_BRUSH,
    TOOL_ERASER,
    TOOL_LINE,
    TOOL_RECT,
    TOOL_FILLRECT,
    TOOL_COUNT
} Tool;

typedef struct {
    Uint8 r,g,b,a;
} Color;

static Color PALETTE[PALETTE_ROWS * PALETTE_COLS] = {
    {0,0,0,255}, {128,128,128,255}, {192,192,192,255}, {255,255,255,255},
    {128,0,0,255}, {255,0,0,255}, {255,128,128,255}, {128,64,64,255},
    {128,64,0,255}, {255,128,0,255}, {255,200,128,255}, {255,255,0,255},
    {0,128,0,255}, {0,255,0,255}, {128,255,128,255}, {0,64,0,255},
    {0,128,128,255}, {0,255,255,255}, {128,255,255,255}, {0,64,64,255},
    {0,0,128,255}, {0,0,255,255}, {128,128,255,255}, {64,64,128,255}
};

// Utility
static void set_draw_color(SDL_Renderer* ren, Color c) {
    SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, c.a);
}

// Bresenham line on renderer (draws filled circles for brush thickness)
static void draw_line_thick(SDL_Renderer* ren, int x0, int y0, int x1, int y1, int thickness, Color col) {
    set_draw_color(ren, col);
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    while (1) {
        // draw circle at (x0,y0)
        for (int yy = -thickness; yy <= thickness; ++yy) {
            for (int xx = -thickness; xx <= thickness; ++xx) {
                if (xx*xx + yy*yy <= thickness*thickness) {
                    SDL_RenderDrawPoint(ren, x0 + xx, y0 + yy);
                }
            }
        }
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

// Draw rectangle outline with thickness
static void draw_rect_thick(SDL_Renderer* ren, SDL_Rect r, int thickness, Color col) {
    set_draw_color(ren, col);
    for (int t = 0; t < thickness; ++t) {
        SDL_Rect rr = { r.x - t, r.y - t, r.w + 2*t, r.h + 2*t };
        SDL_RenderDrawRect(ren, &rr);
    }
}

// Filled rectangle
static void fill_rect(SDL_Renderer* ren, SDL_Rect r, Color col) {
    set_draw_color(ren, col);
    SDL_RenderFillRect(ren, &r);
}

// Helper: clamp
static int clampi(int v, int a, int b) { if (v < a) return a; if (v > b) return b; return v; }

// Main
int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init() != 0) {
        fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* win = SDL_CreateWindow("Mini Paint (SDL2) - Pencil/Brush/Line/Rect/FillRect",
                                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       WIN_W, WIN_H, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Canvas texture (we draw into this texture to preserve pixels)
    SDL_Texture* canvas = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, CANVAS_W, CANVAS_H);
    // Initialize canvas white
    SDL_SetRenderTarget(ren, canvas);
    SDL_SetRenderDrawColor(ren, 255,255,255,255);
    SDL_RenderClear(ren);
    SDL_SetRenderTarget(ren, NULL);

    // Font
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 14);
    if (!font) font = NULL;

    // State
    Tool current_tool = TOOL_PENCIL;
    Color current_color = {0,0,0,255};
    int brush_size = 2; // radius in pixels
    bool running = true;
    bool mouse_down = false;
    int last_x = 0, last_y = 0;
    int start_x = 0, start_y = 0;
    bool preview_active = false;

    // Toolbar layout
    SDL_Rect toolbar = { CANVAS_W, 0, TOOLBAR_W, WIN_H - STATUS_H };
    // Tool buttons
    SDL_Rect tool_buttons[TOOL_COUNT];
    for (int i = 0; i < TOOL_COUNT; ++i) {
        tool_buttons[i].x = toolbar.x + 16;
        tool_buttons[i].y = 16 + i * 56;
        tool_buttons[i].w = toolbar.w - 32;
        tool_buttons[i].h = 40;
    }
    // Brush size buttons
    SDL_Rect size_buttons[4];
    for (int i = 0; i < 4; ++i) {
        size_buttons[i].x = toolbar.x + 16 + (i%2)*80;
        size_buttons[i].y = 16 + TOOL_COUNT*56 + 16 + (i/2)*44;
        size_buttons[i].w = 72;
        size_buttons[i].h = 36;
    }
    // Palette area
    SDL_Rect palette_area = { toolbar.x + PALETTE_MARGIN, toolbar.y + 16 + TOOL_COUNT*56 + 120, PALETTE_COLS * PALETTE_CELL, PALETTE_ROWS * PALETTE_CELL };

    // Status bar rect
    SDL_Rect status_rect = { 0, WIN_H - STATUS_H, WIN_W, STATUS_H };

    // Main loop
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { running = false; }
            else if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    int w = e.window.data1, h = e.window.data2;
                    // We keep canvas logical size constant; toolbar remains at right
                    // (For simplicity we don't resize canvas texture here)
                    status_rect.y = h - STATUS_H;
                    status_rect.w = w;
                    toolbar.x = CANVAS_W; // unchanged
                    toolbar.h = h - STATUS_H;
                }
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x, my = e.button.y;
                mouse_down = true;
                // Click in toolbar?
                if (mx >= CANVAS_W) {
                    // Tools
                    for (int i = 0; i < TOOL_COUNT; ++i) {
                        if (SDL_PointInRect(&(SDL_Point){mx,my}, &tool_buttons[i])) {
                            current_tool = (Tool)i;
                            preview_active = false;
                        }
                    }
                    // Brush sizes
                    for (int i = 0; i < 4; ++i) {
                        if (SDL_PointInRect(&(SDL_Point){mx,my}, &size_buttons[i])) {
                            brush_size = (i+1) * 2; // 2,4,6,8
                        }
                    }
                    // Palette click
                    int relx = mx - palette_area.x;
                    int rely = my - palette_area.y;
                    if (relx >= 0 && rely >= 0 && relx < PALETTE_COLS*PALETTE_CELL && rely < PALETTE_ROWS*PALETTE_CELL) {
                        int cx = relx / PALETTE_CELL;
                        int cy = rely / PALETTE_CELL;
                        int idx = cy * PALETTE_COLS + cx;
                        if (idx >= 0 && idx < PALETTE_ROWS*PALETTE_COLS) {
                            current_color = PALETTE[idx];
                        }
                    }
                } else {
                    // Click in canvas
                    int gx = clampi(mx - CANVAS_X, 0, CANVAS_W-1);
                    int gy = clampi(my - CANVAS_Y, 0, CANVAS_H-1);
                    start_x = gx; start_y = gy;
                    last_x = gx; last_y = gy;
                    // For pencil/brush/eraser: draw immediately
                    if (current_tool == TOOL_PENCIL || current_tool == TOOL_BRUSH || current_tool == TOOL_ERASER) {
                        SDL_SetRenderTarget(ren, canvas);
                        if (current_tool == TOOL_ERASER) {
                            Color bg = {255,255,255,255};
                            draw_line_thick(ren, last_x, last_y, gx, gy, brush_size, bg);
                        } else {
                            draw_line_thick(ren, last_x, last_y, gx, gy, brush_size, current_color);
                        }
                        SDL_SetRenderTarget(ren, NULL);
                    } else {
                        // For line/rect/fillrect: start preview
                        preview_active = true;
                    }
                }
            } else if (e.type == SDL_MOUSEBUTTONUP) {
                int mx = e.button.x, my = e.button.y;
                mouse_down = false;
                if (mx < CANVAS_W) {
                    int gx = clampi(mx - CANVAS_X, 0, CANVAS_W-1);
                    int gy = clampi(my - CANVAS_Y, 0, CANVAS_H-1);
                    // Commit shape if preview active
                    if (preview_active) {
                        SDL_SetRenderTarget(ren, canvas);
                        if (current_tool == TOOL_LINE) {
                            draw_line_thick(ren, start_x, start_y, gx, gy, brush_size, current_color);
                        } else if (current_tool == TOOL_RECT) {
                            SDL_Rect r = { (start_x < gx) ? start_x : gx, (start_y < gy) ? start_y : gy,
                                           abs(gx - start_x), abs(gy - start_y) };
                            draw_rect_thick(ren, r, brush_size, current_color);
                        } else if (current_tool == TOOL_FILLRECT) {
                            SDL_Rect r = { (start_x < gx) ? start_x : gx, (start_y < gy) ? start_y : gy,
                                           abs(gx - start_x), abs(gy - start_y) };
                            fill_rect(ren, r, current_color);
                        }
                        SDL_SetRenderTarget(ren, NULL);
                        preview_active = false;
                    }
                }
            } else if (e.type == SDL_MOUSEMOTION) {
                int mx = e.motion.x, my = e.motion.y;
                if (mouse_down && mx < CANVAS_W) {
                    int gx = clampi(mx - CANVAS_X, 0, CANVAS_W-1);
                    int gy = clampi(my - CANVAS_Y, 0, CANVAS_H-1);
                    if (current_tool == TOOL_PENCIL || current_tool == TOOL_BRUSH || current_tool == TOOL_ERASER) {
                        SDL_SetRenderTarget(ren, canvas);
                        if (current_tool == TOOL_ERASER) {
                            Color bg = {255,255,255,255};
                            draw_line_thick(ren, last_x, last_y, gx, gy, brush_size, bg);
                        } else {
                            draw_line_thick(ren, last_x, last_y, gx, gy, brush_size, current_color);
                        }
                        SDL_SetRenderTarget(ren, NULL);
                        last_x = gx; last_y = gy;
                    } else {
                        // update preview end point
                        last_x = gx; last_y = gy;
                    }
                }
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) running = false;
                else if (e.key.keysym.sym == SDLK_1) current_tool = TOOL_PENCIL;
                else if (e.key.keysym.sym == SDLK_2) current_tool = TOOL_BRUSH;
                else if (e.key.keysym.sym == SDLK_3) current_tool = TOOL_ERASER;
                else if (e.key.keysym.sym == SDLK_4) current_tool = TOOL_LINE;
                else if (e.key.keysym.sym == SDLK_5) current_tool = TOOL_RECT;
                else if (e.key.keysym.sym == SDLK_6) current_tool = TOOL_FILLRECT;
            }
        }

        // Render UI
        SDL_SetRenderDrawColor(ren, 200,200,200,255);
        SDL_RenderClear(ren);

        // Draw canvas background (frame)
        SDL_Rect canvas_frame = { CANVAS_X, CANVAS_Y, CANVAS_W, CANVAS_H };
        SDL_SetRenderDrawColor(ren, 160,160,160,255);
        SDL_RenderFillRect(ren, &canvas_frame);

        // Draw canvas texture
        SDL_Rect dst = { CANVAS_X, CANVAS_Y, CANVAS_W, CANVAS_H };
        SDL_RenderCopy(ren, canvas, NULL, &dst);

        // If preview active, draw overlay shapes
        if (preview_active) {
            // semi-transparent overlay
            SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
            Color overlay = {0,0,0,120};
            set_draw_color(ren, overlay);
            // draw shape based on current_tool using last_x,last_y as current mouse
            if (current_tool == TOOL_LINE) {
                draw_line_thick(ren, start_x, start_y, last_x, last_y, brush_size, current_color);
            } else if (current_tool == TOOL_RECT) {
                SDL_Rect r = { (start_x < last_x) ? start_x : last_x, (start_y < last_y) ? start_y : last_y,
                               abs(last_x - start_x), abs(last_y - start_y) };
                draw_rect_thick(ren, r, brush_size, current_color);
            } else if (current_tool == TOOL_FILLRECT) {
                SDL_Rect r = { (start_x < last_x) ? start_x : last_x, (start_y < last_y) ? start_y : last_y,
                               abs(last_x - start_x), abs(last_y - start_y) };
                fill_rect(ren, r, current_color);
            }
            SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_NONE);
        }

        // Draw toolbar background
        SDL_Rect tb = { CANVAS_W, 0, TOOLBAR_W, WIN_H - STATUS_H };
        SDL_SetRenderDrawColor(ren, 240,240,240,255);
        SDL_RenderFillRect(ren, &tb);

        // Draw tool buttons
        for (int i = 0; i < TOOL_COUNT; ++i) {
            SDL_Rect b = tool_buttons[i];
            SDL_SetRenderDrawColor(ren, 220,220,220,255);
            SDL_RenderFillRect(ren, &b);
            if (current_tool == i) {
                SDL_SetRenderDrawColor(ren, 0,120,215,255);
                SDL_RenderDrawRect(ren, &b);
                SDL_RenderDrawRect(ren, &(SDL_Rect){b.x+1,b.y+1,b.w-2,b.h-2});
            } else {
                SDL_SetRenderDrawColor(ren, 160,160,160,255);
                SDL_RenderDrawRect(ren, &b);
            }
            // label
            if (font) {
                const char* label = "";
                switch (i) {
                    case TOOL_PENCIL: label = "Pencil (1)"; break;
                    case TOOL_BRUSH: label = "Brush (2)"; break;
                    case TOOL_ERASER: label = "Eraser (3)"; break;
                    case TOOL_LINE: label = "Line (4)"; break;
                    case TOOL_RECT: label = "Rect (5)"; break;
                    case TOOL_FILLRECT: label = "FillRect (6)"; break;
                }
                SDL_Color textc = {0,0,0,255};
                SDL_Surface* surf = TTF_RenderUTF8_Blended(font, label, (SDL_Color){0,0,0,255});
                if (surf) {
                    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
                    SDL_Rect dstt = { b.x + 8, b.y + 10, surf->w, surf->h };
                    SDL_RenderCopy(ren, tex, NULL, &dstt);
                    SDL_DestroyTexture(tex);
                    SDL_FreeSurface(surf);
                }
            }
        }

        // Brush size buttons
        for (int i = 0; i < 4; ++i) {
            SDL_Rect b = size_buttons[i];
            SDL_SetRenderDrawColor(ren, 220,220,220,255);
            SDL_RenderFillRect(ren, &b);
            SDL_SetRenderDrawColor(ren, 160,160,160,255);
            SDL_RenderDrawRect(ren, &b);
            // draw circle representing size
            int cx = b.x + b.w/2;
            int cy = b.y + b.h/2;
            int radius = (i+1)*2;
            set_draw_color(ren, current_color);
            for (int yy = -radius; yy <= radius; ++yy)
                for (int xx = -radius; xx <= radius; ++xx)
                    if (xx*xx + yy*yy <= radius*radius)
                        SDL_RenderDrawPoint(ren, cx+xx, cy+yy);
            if (brush_size == radius) {
                SDL_SetRenderDrawColor(ren, 0,0,0,255);
                SDL_RenderDrawRect(ren, &b);
            }
        }

        // Palette
        for (int row = 0; row < PALETTE_ROWS; ++row) {
            for (int col = 0; col < PALETTE_COLS; ++col) {
                int idx = row * PALETTE_COLS + col;
                SDL_Rect pc = { palette_area.x + col*PALETTE_CELL, palette_area.y + row*PALETTE_CELL, PALETTE_CELL-2, PALETTE_CELL-2 };
                Color c = PALETTE[idx];
                SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, c.a);
                SDL_RenderFillRect(ren, &pc);
                SDL_SetRenderDrawColor(ren, 100,100,100,255);
                SDL_RenderDrawRect(ren, &pc);
                if (c.r == current_color.r && c.g == current_color.g && c.b == current_color.b) {
                    SDL_SetRenderDrawColor(ren, 255,255,255,255);
                    SDL_RenderDrawRect(ren, &(SDL_Rect){pc.x-2, pc.y-2, pc.w+4, pc.h+4});
                }
            }
        }

        // Current color preview
        SDL_Rect curc = { toolbar.x + 16, palette_area.y + PALETTE_ROWS*PALETTE_CELL + 16, 64, 64 };
        SDL_SetRenderDrawColor(ren, current_color.r, current_color.g, current_color.b, current_color.a);
        SDL_RenderFillRect(ren, &curc);
        SDL_SetRenderDrawColor(ren, 0,0,0,255);
        SDL_RenderDrawRect(ren, &curc);
        if (font) {
            char buf[64];
            snprintf(buf, sizeof(buf), "Color: #%02X%02X%02X", current_color.r, current_color.g, current_color.b);
            SDL_Surface* surf = TTF_RenderUTF8_Blended(font, buf, (SDL_Color){0,0,0,255});
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
                SDL_Rect dstt = { curc.x + 72, curc.y + 8, surf->w, surf->h };
                SDL_RenderCopy(ren, tex, NULL, &dstt);
                SDL_DestroyTexture(tex);
                SDL_FreeSurface(surf);
            }
        }

        // Status bar
        SDL_SetRenderDrawColor(ren, 220,220,220,255);
        SDL_Rect sbar = { 0, WIN_H - STATUS_H, WIN_W, STATUS_H };
        SDL_RenderFillRect(ren, &sbar);
        if (font) {
            char status[256];
            const char* toolname = "";
            switch (current_tool) {
                case TOOL_PENCIL: toolname = "Pencil"; break;
                case TOOL_BRUSH: toolname = "Brush"; break;
                case TOOL_ERASER: toolname = "Eraser"; break;
                case TOOL_LINE: toolname = "Line"; break;
                case TOOL_RECT: toolname = "Rect"; break;
                case TOOL_FILLRECT: toolname = "FillRect"; break;
            }
            snprintf(status, sizeof(status), "Tool: %s    Brush: %d    Color: #%02X%02X%02X",
                     toolname, brush_size, current_color.r, current_color.g, current_color.b);
            SDL_Surface* surf = TTF_RenderUTF8_Blended(font, status, (SDL_Color){0,0,0,255});
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
                SDL_Rect dstt = { 6, WIN_H - STATUS_H + 4, surf->w, surf->h };
                SDL_RenderCopy(ren, tex, NULL, &dstt);
                SDL_DestroyTexture(tex);
                SDL_FreeSurface(surf);
            }
        }

        SDL_RenderPresent(ren);
        SDL_Delay(8);
    }

    if (font) TTF_CloseFont(font);
    SDL_DestroyTexture(canvas);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
