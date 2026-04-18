// semiconductormachine1.c
// Lattice Boltzmann D2Q9 demo with SDL2 visualization and sinusoidal inlet.
// Includes: global inlet_multiplier, directed return-channel mapping (no right-side bounce),
// and a semicircular return channel highlighted for visibility.
// Compile: gcc -O2 -std=c11 -lm -lSDL2 semiconductormachine1.c -o laticeboltzmann

#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NX 300
#define NY 150
#define SCALE 4            // pixel scale for display
#define WINDOW_W (NX * SCALE)
#define WINDOW_H (NY * SCALE)
#define NDIR 9

// LBM parameters
const double w[9] = {4.0/9.0, 1.0/9.0,1.0/9.0,1.0/9.0,1.0/9.0,1.0/36.0,1.0/36.0,1.0/36.0,1.0/36.0};
const int cx[9] = {0,1,0,-1,0,1,-1,-1,1};
const int cy[9] = {0,0,1,0,-1,1,1,-1,-1};
const int opp[9] = {0,3,4,1,2,7,8,5,6};

double *f, *f_next;
double rho_field[NX*NY];
double ux_field[NX*NY];
double uy_field[NX*NY];
unsigned char obstacle[NX*NY]; // 1 = wall, 0 = fluid
unsigned char return_channel[NX*NY]; // 1 = return channel (highlight)
double speed_factor[NX*NY]; // local speed multiplier (narrow pipes -> >1)

double tau = 0.53; // relaxation time
double omega; // 1/tau

// Global multiplier for inlet "stauchen" (applies to amplitude and frequency)
double inlet_multiplier = 20.0;

// Return-channel entrance coordinates (gesetzt in init_geometry)
int return_connector_x = 10;
int return_center_y = 10;

// Utility: convert (x,y) to linear index
static inline int idx(int x,int y){ return y * NX + x; }

// D2Q9 equilibrium distribution
static inline double feq(int k, double rho, double ux, double uy){
    double cu = 3.0 * (cx[k]*ux + cy[k]*uy);
    double u2 = 1.5 * (ux*ux + uy*uy);
    return w[k] * rho * (1.0 + cu + 0.5*cu*cu - u2);
}

void die(const char *msg){ fprintf(stderr,"%s\n",msg); exit(1); }

// Initialize geometry: big pipe left -> splits into two narrow pipes right
// plus ein halbkreisförmiger Rückleitungsarm oberhalb, der die rechten Verzweigungen
// zurück zur linken Seite führt (Uhrzeigersinn).
void init_geometry(){
    memset(obstacle,1,sizeof(obstacle)); // start with walls
    memset(return_channel,0,sizeof(return_channel)); // clear highlight flags

    // carve a big horizontal pipe on left half
    int mid = NY/2;
    int big_radius = NY/6; // thick pipe
    for(int y=mid-big_radius;y<=mid+big_radius;y++){
        for(int x=5;x<NX/2;x++){
            obstacle[idx(x,y)] = 0;
        }
    }

    // carve two narrower pipes on right half (branch)
    int narrow_radius = NY/12;
    int branch_x = NX/2;
    int upper_center = mid - (big_radius + narrow_radius);
    int lower_center = mid + (big_radius + narrow_radius);

    for(int x=branch_x;x<NX-5;x++){
        for(int y=0;y<NY;y++){
            if (abs(y - upper_center) <= narrow_radius) obstacle[idx(x,y)] = 0;
            if (abs(y - lower_center) <= narrow_radius) obstacle[idx(x,y)] = 0;
        }
    }

    // connect big pipe to branches with a smooth Y-split region
    for(int x=NX/2 - 4; x<NX/2 + 8; x++){
        for(int y=mid-big_radius; y<=mid+big_radius; y++){
            obstacle[idx(x,y)] = 0;
        }
    }

    // --- Halbkreisförmiger Rückleitungsarm ---
    // Positionierung: oberhalb der großen Leitung, halbkreis öffnet nach links
    int return_y = mid - big_radius - 8; // Basishöhe des Halbkreises
    if (return_y < 6) return_y = 6;
    int return_half_width = 3; // Kanaldicke (halbe Höhe)
    int center_x = NX - 40;    // Kreiszentrum x (rechts)
    int center_y = return_y;   // Kreiszentrum y
    int radius = 36;           // Radius des Halbkreises

    // set global entrance coordinates for redirection
    return_connector_x = 10;      // verbindet zurück in die Nähe des Einlasses
    return_center_y = center_y;   // Höhe des Rückkanal-Eingangs

    // carve semicircle (linke Halbseite des Kreises) und markieren
    for(int x = center_x - radius; x <= center_x + radius; x++){
        for(int y = center_y - radius; y <= center_y + radius; y++){
            if (x < 0 || x >= NX || y < 0 || y >= NY) continue;
            int dx = x - center_x;
            int dy = y - center_y;
            if (dx*dx + dy*dy <= radius*radius){
                // nur die linke Halbseite behalten (halbkreis, offen nach links)
                if (x <= center_x){
                    // erweitere die Fläche um Kanaldicke
                    for(int yy = y - return_half_width; yy <= y + return_half_width; yy++){
                        if (yy < 0 || yy >= NY) continue;
                        obstacle[idx(x,yy)] = 0;
                        return_channel[idx(x,yy)] = 1;
                    }
                }
            }
        }
    }

    // horizontale Verbindung vom linken Rand des Halbkreises zur linken Region (connector)
    int left_of_semi = center_x - radius - 2;
    if (left_of_semi < 6) left_of_semi = 6;
    int connector_x = return_connector_x; // verbindet zurück in die Nähe des Einlasses
    int conn_half_h = 3;
    for(int x = connector_x; x <= left_of_semi; x++){
        for(int y = center_y - conn_half_h; y <= center_y + conn_half_h; y++){
            if (y < 0 || y >= NY) continue;
            obstacle[idx(x,y)] = 0;
            return_channel[idx(x,y)] = 1;
        }
    }

    // vertikaler Connector von der Verbindungslinie hinunter in die große Leitung
    for(int y = center_y; y <= mid - big_radius + 2; y++){
        for(int dx = -1; dx <= 1; dx++){
            int x = connector_x + dx;
            if (x < 0 || x >= NX || y < 0 || y >= NY) continue;
            obstacle[idx(x,y)] = 0;
            return_channel[idx(x,y)] = 1;
        }
    }

    // Verbinder von den rechten Verzweigungen hoch zum Halbkreis (sicherstellen, dass Fluss dorthin gelangen kann)
    int right_conn_start = NX - 18;
    for(int x = right_conn_start; x < NX-6; x++){
        // obere Verbindung
        for(int y = upper_center - narrow_radius; y <= center_y + return_half_width; y++){
            if (y < 0 || y >= NY) continue;
            obstacle[idx(x,y)] = 0;
            return_channel[idx(x,y)] = 1;
        }
        // untere Verbindung (führt nach oben um zur Rückleitung zu gelangen)
        for(int y = lower_center - narrow_radius; y <= center_y + return_half_width; y++){
            if (y < 0 || y >= NY) continue;
            obstacle[idx(x,y)] = 0;
            return_channel[idx(x,y)] = 1;
        }
    }

    // kleine Glättung am rechten Ende des Halbkreises
    for(int x = center_x - 4; x <= center_x + 4; x++){
        for(int y = center_y - radius - 2; y <= center_y + radius + 2; y++){
            if (x < 0 || x >= NX || y < 0 || y >= NY) continue;
            obstacle[idx(x,y)] = 0;
            return_channel[idx(x,y)] = 1;
        }
    }

    // compute speed_factor: narrow channels -> larger factor
    for(int y=0;y<NY;y++){
        for(int x=0;x<NX;x++){
            int id = idx(x,y);
            if (obstacle[id]) { speed_factor[id] = 0.0; continue; }
            // measure local width by scanning vertically
            int up=y, down=y;
            while(up>0 && !obstacle[idx(x,up-1)]) up--;
            while(down<NY-1 && !obstacle[idx(x,down+1)]) down++;
            int local_width = down - up + 1;
            // normalize: wide -> factor ~1.0, narrow -> up to 3.0
            double factor = 1.0 + (double)(NY/6 - local_width) / (double)(NY/12);
            if (factor < 1.0) factor = 1.0;
            if (factor > 3.0) factor = 3.0;
            speed_factor[id] = factor;
        }
    }
}

// Initialize LBM fields
void init_lbm(){
    f = (double*)malloc(sizeof(double)*NX*NY*NDIR);
    f_next = (double*)malloc(sizeof(double)*NX*NY*NDIR);
    if(!f || !f_next) die("malloc failed");
    for(int y=0;y<NY;y++){
        for(int x=0;x<NX;x++){
            int id = idx(x,y);
            rho_field[id] = 1.0;
            ux_field[id] = 0.0;
            uy_field[id] = 0.0;
            for(int k=0;k<NDIR;k++){
                f[id*NDIR + k] = w[k];
            }
        }
    }
    omega = 1.0 / tau;
}

// Streaming + collision step with directed return mapping (no periodic X wrap)
void lbm_step(double t){
    // macroscopic
    for(int y=0;y<NY;y++){
        for(int x=0;x<NX;x++){
            int id = idx(x,y);
            if (obstacle[id]) { rho_field[id]=0; ux_field[id]=0; uy_field[id]=0; continue; }
            double rho = 0.0, ux = 0.0, uy = 0.0;
            for(int k=0;k<NDIR;k++){
                double val = f[id*NDIR + k];
                rho += val;
                ux += val * cx[k];
                uy += val * cy[k];
            }
            ux /= rho; uy /= rho;
            rho_field[id] = rho;
            ux_field[id] = ux;
            uy_field[id] = uy;
        }
    }

    // apply inlet sinusoidal velocity on leftmost fluid cells
    // uses global inlet_multiplier for amplitude and frequency scaling
    double inlet_base = 0.04 * inlet_multiplier; // amplitude scaled
    double inlet = inlet_base * (0.5 + 0.5 * sin(inlet_multiplier * t)); // frequency scaled
    for(int y=1;y<NY-1;y++){
        int x = 6; // near left boundary inside pipe
        int id = idx(x,y);
        if (obstacle[id]) continue;
        double factor = speed_factor[id];
        double u_in = inlet * factor;
        ux_field[id] = u_in;
        uy_field[id] = 0.0;
        rho_field[id] = 1.0;
        for(int k=0;k<NDIR;k++){
            f[id*NDIR + k] = feq(k, rho_field[id], ux_field[id], uy_field[id]);
        }
    }

    // collision (BGK) and compute post-collision
    for(int y=0;y<NY;y++){
        for(int x=0;x<NX;x++){
            int id = idx(x,y);
            if (obstacle[id]) {
                for(int k=0;k<NDIR;k++) f_next[id*NDIR + k] = f[id*NDIR + k];
                continue;
            }
            double rho = rho_field[id];
            double ux = ux_field[id];
            double uy = uy_field[id];
            for(int k=0;k<NDIR;k++){
                double feqk = feq(k, rho, ux, uy);
                double fpost = f[id*NDIR + k] - omega * (f[id*NDIR + k] - feqk);
                f_next[id*NDIR + k] = fpost;
            }
        }
    }

    // streaming: NO periodic X wrap. If streaming would leave domain in X,
    // redirect that distribution into the return-channel entrance (return_connector_x, return_center_y).
    memset(f,0,sizeof(double)*NX*NY*NDIR);
    for(int y=0;y<NY;y++){
        for(int x=0;x<NX;x++){
            int id = idx(x,y);
            for(int k=0;k<NDIR;k++){
                int nx = x + cx[k];
                int ny = y + cy[k];
                // If X would go out of bounds, redirect into return-channel entrance
                if (nx < 0 || nx >= NX){
                    nx = return_connector_x;
                    ny = return_center_y + (ny - y); // try to preserve vertical offset
                }
                // keep Y bounded (no vertical wrap)
                if (ny < 0 || ny >= NY) continue;
                int nid = idx(nx,ny);
                f[nid*NDIR + k] = f_next[id*NDIR + k];
            }
        }
    }

    // bounce-back for obstacles: reflect distributions (standard)
    for(int y=0;y<NY;y++){
        for(int x=0;x<NX;x++){
            int id = idx(x,y);
            if (!obstacle[id]) continue;
            for(int k=0;k<NDIR;k++){
                int nx = x + cx[k];
                int ny = y + cy[k];
                // If neighbor would be out of bounds in X, redirect bounce target into return entrance
                if (nx < 0 || nx >= NX){
                    nx = return_connector_x;
                    ny = return_center_y;
                }
                if (ny < 0 || ny >= NY) continue;
                int nid = idx(nx,ny);
                // reflect into neighbor's opposite direction
                f[nid*NDIR + opp[k]] = f_next[id*NDIR + k];
            }
        }
    }
}

// Render to SDL texture
void render(SDL_Renderer *renderer, SDL_Texture *tex, Uint32 *pixels){
    int total = NX*NY;
    for(int i=0;i<total;i++){
        if (obstacle[i]) {
            pixels[i] = 0xFF333333; // wall dark gray
        } else if (return_channel[i]) {
            // Hervorhebung des Rückflusskanals: helles Rot
            pixels[i] = (255<<24) | (220<<16) | (40<<8) | 40;
        } else {
            double vx = ux_field[i], vy = uy_field[i];
            double mag = sqrt(vx*vx + vy*vy);
            double ang = atan2(vy, vx);
            double norm = (ang + M_PI) / (2*M_PI); // 0..1
            int r = (int)(128 + 127 * cos(2*M_PI*norm));
            int g = (int)(128 + 127 * cos(2*M_PI*(norm+0.33)));
            int b = (int)(128 + 127 * cos(2*M_PI*(norm+0.66)));
            double s = fmin(1.0, mag*40.0);
            r = (int)(r * s + 20*(1-s));
            g = (int)(g * s + 20*(1-s));
            b = (int)(b * s + 20*(1-s));
            pixels[i] = (255<<24) | (r<<16) | (g<<8) | b;
        }
    }

    SDL_UpdateTexture(tex, NULL, pixels, NX * sizeof(Uint32));
    SDL_RenderClear(renderer);
    SDL_Rect dst = {0,0, WINDOW_W, WINDOW_H};
    SDL_RenderCopy(renderer, tex, NULL, &dst);
    SDL_RenderPresent(renderer);
}

int main(int argc, char **argv){
    if (SDL_Init(SDL_INIT_VIDEO) != 0) die(SDL_GetError());
    SDL_Window *win = SDL_CreateWindow("LBM Wasserfluss-Simulator (gezielte Rückleitung)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, 0);
    if (!win) die(SDL_GetError());
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!ren) die(SDL_GetError());
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, NX, NY);
    if (!tex) die(SDL_GetError());

    init_geometry();
    init_lbm();

    Uint32 *pixels = (Uint32*)malloc(sizeof(Uint32)*NX*NY);
    if (!pixels) die("pixels malloc failed");

    int running = 1;
    double t = 0.0;
    double dt = 0.016; // ~60 FPS
    Uint64 last = SDL_GetPerformanceCounter();
    while(running){
        SDL_Event ev;
        while(SDL_PollEvent(&ev)){
            if (ev.type == SDL_QUIT) running = 0;
            if (ev.type == SDL_KEYDOWN){
                if (ev.key.keysym.sym == SDLK_ESCAPE) running = 0;
                // runtime control: +/- to change inlet_multiplier
                if (ev.key.keysym.sym == SDLK_PLUS || ev.key.keysym.sym == SDLK_EQUALS) inlet_multiplier *= 1.2;
                if (ev.key.keysym.sym == SDLK_MINUS) inlet_multiplier /= 1.2;
            }
        }
        int micro = 2;
        for(int m=0;m<micro;m++){
            lbm_step(t);
            t += dt / micro;
        }

        render(ren, tex, pixels);

        Uint64 now = SDL_GetPerformanceCounter();
        double elapsed = (double)(now - last) / SDL_GetPerformanceFrequency();
        last = SDL_GetPerformanceCounter();
    }

    free(f); free(f_next); free(pixels);
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
