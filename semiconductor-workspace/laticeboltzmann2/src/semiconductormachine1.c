#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROOT_NODES 30000

typedef struct {
    int x, y;
    int thickness;
} RootNode;

RootNode roots[MAX_ROOT_NODES];
int root_count = 0;

int NX = 400;
int NY = 200;
int SCALE = 3;

/* D2Q9 */
int ex[9] = {0, 1, 0, -1, 0, 1, -1, -1, 1};
int ey[9] = {0, 0, 1, 0, -1, 1, 1, -1, -1};
double w[9] = {
    4.0/9.0,
    1.0/9.0, 1.0/9.0, 1.0/9.0, 1.0/9.0,
    1.0/36.0, 1.0/36.0, 1.0/36.0, 1.0/36.0
};

double tau = 0.8;
double omega = 1.0 / 0.8;

#define IDX(x,y,k) (((y) * NX + (x)) * 9 + (k))
#define SOLID_IDX(x,y) ((y) * NX + (x))

/* ----------------------------------------------------------
   WURZEL-GENERATOR
   ---------------------------------------------------------- */

void grow_root(int start_y, int thickness) {
    int x = 0;
    int y = start_y;

    while (x < NX - 2 && root_count < MAX_ROOT_NODES - 10) {
        roots[root_count].x = x;
        roots[root_count].y = y;
        roots[root_count].thickness = thickness;
        root_count++;

        x++;

        /* vertikale Variation */
        int dy = (rand() % 3) - 1;
        y += dy;

        if (y < 5) y = 5;
        if (y > NY - 5) y = NY - 5;

        /* Dicke variieren */
        if (rand() % 20 == 0) thickness++;
        if (rand() % 25 == 0 && thickness > 2) thickness--;

        /* Verzweigung */
        if (rand() % 120 == 0 && thickness > 3) {
            grow_root(y, thickness - 1);
        }
    }
}

/* ----------------------------------------------------------
   HAUPTPROGRAMM
   ---------------------------------------------------------- */

int main(int argc, char** argv) {
    srand(12345);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init failed\n");
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "LBM Wurzel-Wasseradern",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        NX * SCALE, NY * SCALE, SDL_WINDOW_SHOWN
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    size_t total_cells = NX * NY;
    size_t total_f = total_cells * 9;

    double* f = malloc(total_f * sizeof(double));
    double* f_next = malloc(total_f * sizeof(double));
    int* is_solid = malloc(total_cells * sizeof(int));

    /* ----------------------------------------------------------
       WURZELNETZ ERZEUGEN
       ---------------------------------------------------------- */

    grow_root(NY/3, 4);
    grow_root(NY/2, 5);
    grow_root(NY/4*3, 6);

    /* alles solid */
    for (int i = 0; i < total_cells; i++)
        is_solid[i] = 1;

    /* Wurzeladern als Fluid markieren */
    for (int i = 0; i < root_count; i++) {
        int x = roots[i].x;
        int y = roots[i].y;
        int th = roots[i].thickness;

        for (int dy = -th; dy <= th; dy++) {
            int yy = y + dy;
            if (yy >= 0 && yy < NY)
                is_solid[SOLID_IDX(x, yy)] = 0;
        }
    }

    /* ----------------------------------------------------------
       LBM INITIALISIERUNG
       ---------------------------------------------------------- */

    for (int y = 0; y < NY; y++) {
        for (int x = 0; x < NX; x++) {
            double rho0 = 1.0;
            double ux0 = 0.0;
            double uy0 = 0.0;
            double u2 = ux0*ux0 + uy0*uy0;

            for (int k = 0; k < 9; k++) {
                double eu = ex[k]*ux0 + ey[k]*uy0;
                double feq = w[k] * rho0 *
                    (1.0 + 3.0*eu + 4.5*eu*eu - 1.5*u2);
                f[IDX(x,y,k)] = feq;
            }
        }
    }

    memset(f_next, 0, total_f * sizeof(double));

    /* ----------------------------------------------------------
       SIMULATION LOOP
       ---------------------------------------------------------- */

    int running = 1;
    SDL_Event e;
    int step = 0;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
                running = 0;
        }

        /* ---------------- LBM COLLISION + STREAMING ---------------- */

        for (int y = 0; y < NY; y++) {
            for (int x = 0; x < NX; x++) {

                if (is_solid[SOLID_IDX(x,y)]) {
                    for (int k = 0; k < 9; k++)
                        f_next[IDX(x,y,k)] = 0.0;
                    continue;
                }

                double rho = 0, ux = 0, uy = 0;

                for (int k = 0; k < 9; k++) {
                    double fk = f[IDX(x,y,k)];
                    rho += fk;
                    ux += fk * ex[k];
                    uy += fk * ey[k];
                }

                ux /= rho;
                uy /= rho;

                /* Einlass links */
                if (x == 0) {
                    ux = 0.05;
                    uy = 0.0;
                    rho = 1.0;
                }

                double u2 = ux*ux + uy*uy;

                for (int k = 0; k < 9; k++) {
                    double eu = ex[k]*ux + ey[k]*uy;
                    double feq = w[k] * rho *
                        (1.0 + 3.0*eu + 4.5*eu*eu - 1.5*u2);

                    double f_post = f[IDX(x,y,k)] - omega *
                        (f[IDX(x,y,k)] - feq);

                    int xn = x + ex[k];
                    int yn = y + ey[k];

                    if (xn < 0) xn = 0;
                    if (xn >= NX) xn = NX - 1;
                    if (yn < 0) yn = 0;
                    if (yn >= NY) yn = NY - 1;

                    f_next[IDX(xn,yn,k)] += f_post;
                }
            }
        }

        /* ---------------- BOUNCE BACK ---------------- */

        for (int y = 0; y < NY; y++) {
            for (int x = 0; x < NX; x++) {
                if (!is_solid[SOLID_IDX(x,y)]) continue;

                double tmp[9];
                for (int k = 0; k < 9; k++)
                    tmp[k] = f_next[IDX(x,y,k)];

                f_next[IDX(x,y,1)] = tmp[3];
                f_next[IDX(x,y,3)] = tmp[1];
                f_next[IDX(x,y,2)] = tmp[4];
                f_next[IDX(x,y,4)] = tmp[2];
                f_next[IDX(x,y,5)] = tmp[7];
                f_next[IDX(x,y,7)] = tmp[5];
                f_next[IDX(x,y,6)] = tmp[8];
                f_next[IDX(x,y,8)] = tmp[6];
            }
        }

        /* swap */
        double* tmp = f;
        f = f_next;
        f_next = tmp;
        memset(f_next, 0, total_f * sizeof(double));

        /* ----------------------------------------------------------
           VISUALISIERUNG
           ---------------------------------------------------------- */

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        /* Wasser + Wurzeln */
        for (int y = 0; y < NY; y++) {
            for (int x = 0; x < NX; x++) {

                if (is_solid[SOLID_IDX(x,y)]) {
                    SDL_SetRenderDrawColor(renderer, 25, 15, 10, 255);
                } else {
                    double rho=0, ux=0, uy=0;
                    for (int k=0;k<9;k++){
                        double fk=f[IDX(x,y,k)];
                        rho+=fk;
                        ux+=fk*ex[k];
                        uy+=fk*ey[k];
                    }
                    ux/=rho; uy/=rho;
                    double speed = sqrt(ux*ux+uy*uy);

                    Uint8 r = 40 + (Uint8)(speed*200);
                    Uint8 g = 80 + (Uint8)(speed*150);
                    Uint8 b = 200 + (Uint8)(speed*55);

                    SDL_SetRenderDrawColor(renderer, r,g,b,255);
                }

                SDL_Rect rc = { x*SCALE, y*SCALE, SCALE, SCALE };
                SDL_RenderFillRect(renderer, &rc);
            }
        }

        /* ----------------------------------------------------------
           Wellenkanten entlang der Wurzeln
           ---------------------------------------------------------- */

        double t = step * 0.05;

        for (int i = 0; i < root_count; i++) {
            int x = roots[i].x;
            int y = roots[i].y;
            int th = roots[i].thickness;

            double rho=0, ux=0, uy=0;
            for (int k=0;k<9;k++){
                double fk=f[IDX(x,y,k)];
                rho+=fk;
                ux+=fk*ex[k];
                uy+=fk*ey[k];
            }
            ux/=rho; uy/=rho;
            double speed = sqrt(ux*ux+uy*uy);

            double wave = sin(0.12*x + t*(1.0 + speed*10.0));
            int y_top = y - th + (int)(wave * 2.0);
            int y_bot = y + th - (int)(wave * 2.0);

            SDL_SetRenderDrawColor(renderer, 0,255,200,255);

            SDL_RenderDrawPoint(renderer, x*SCALE, y_top*SCALE);
            SDL_RenderDrawPoint(renderer, x*SCALE, y_bot*SCALE);
        }

        SDL_RenderPresent(renderer);
        step++;
    }

    free(f);
    free(f_next);
    free(is_solid);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
