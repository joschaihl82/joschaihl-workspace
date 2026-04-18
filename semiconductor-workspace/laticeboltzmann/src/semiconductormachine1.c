// semiconductormachine1.c
// Lattice Boltzmann D2Q9 demo with SDL2 visualization and sinusoidal inlet.
// Compile: gcc -O2 -std=c11 -lm -lSDL2 -lSDL2_ttf semiconductormachine1.c -o laticeboltzmann

#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NX 300
#define NY 150
#define SCALE 4            // pixel scale for display
#define WINDOW_W (NX * SCALE)
#define WINDOW_H (NY * SCALE)
#define NDIR 9
#define MAX_PARTICLES 2000

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
double speed_factor[NX*NY]; // local speed multiplier (narrow pipes -> >1)

double tau = 0.53; // relaxation time
double omega; // 1/tau

// Particles for visualization
typedef struct { double x,y; double vx,vy; Uint8 r,g,b,a; } Particle;
Particle *particles;
int particle_count = 0;

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
void init_geometry(){
    memset(obstacle,1,sizeof(obstacle)); // start with walls

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
    // upper branch center
    int upper_center = mid - (big_radius + narrow_radius);
    // lower branch center
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

// Streaming + collision step
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
    double inlet_base = 0.04; // base amplitude
    double inlet = inlet_base * (0.5 + 0.5*sin(t)); // sin in [0,1]
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

    // streaming: move f_next to neighbors into f
    memset(f,0,sizeof(double)*NX*NY*NDIR);
    for(int y=0;y<NY;y++){
        for(int x=0;x<NX;x++){
            int id = idx(x,y);
            for(int k=0;k<NDIR;k++){
                int nx = x + cx[k];
                int ny = y + cy[k];
                if (nx < 0 || nx >= NX || ny < 0 || ny >= NY) continue;
                int nid = idx(nx,ny);
                f[nid*NDIR + k] = f_next[id*NDIR + k];
            }
        }
    }

    // bounce-back for obstacles: reflect distributions
    for(int y=0;y<NY;y++){
        for(int x=0;x<NX;x++){
            int id = idx(x,y);
            if (!obstacle[id]) continue;
            for(int k=0;k<NDIR;k++){
                int nx = x + cx[k];
                int ny = y + cy[k];
                if (nx < 0 || nx >= NX || ny < 0 || ny >= NY) continue;
                int nid = idx(nx,ny);
                f[nid*NDIR + opp[k]] = f_next[id*NDIR + k];
            }
        }
    }
}

// Initialize particles inside fluid region near inlet
void init_particles(){
    particles = (Particle*)malloc(sizeof(Particle)*MAX_PARTICLES);
    particle_count = 0;
    srand((unsigned)time(NULL));
    for(int i=0;i<MAX_PARTICLES;i++){
        int x = 8 + rand() % 20;
        int y = NY/2 - (NY/6) + rand() % (NY/3);
        if (obstacle[idx(x,y)]) continue;
        Particle p;
        p.x = x + (rand()/(double)RAND_MAX);
        p.y = y + (rand()/(double)RAND_MAX);
        p.vx = 0; p.vy = 0;
        p.r = 0; p.g = 150 + rand()%100; p.b = 255; p.a = 255;
        particles[particle_count++] = p;
    }
}

// Update particles by sampling velocity field and advecting
void update_particles(double dt){
    for(int i=0;i<particle_count;i++){
        Particle *p = &particles[i];
        int ix = (int)floor(p->x);
        int iy = (int)floor(p->y);
        if (ix < 1 || ix >= NX-1 || iy < 1 || iy >= NY-1 || obstacle[idx(ix,iy)]){
            p->x = 8 + rand()%20;
            p->y = NY/2 - (NY/6) + rand()%(NY/3);
            continue;
        }
        double fx = p->x - ix, fy = p->y - iy;
        double ux = 0, uy = 0;
        for(int dy=0;dy<=1;dy++){
            for(int dx=0;dx<=1;dx++){
                int sx = ix+dx, sy = iy+dy;
                double wgt = ((dx?fx:1-fx) * (dy?fy:1-fy));
                ux += ux_field[idx(sx,sy)] * wgt;
                uy += uy_field[idx(sx,sy)] * wgt;
            }
        }
        double factor = speed_factor[idx(ix,iy)];
        ux *= factor; uy *= factor;
        p->x += ux * dt * 50.0;
        p->y += uy * dt * 50.0;
        p->x += (rand()/(double)RAND_MAX - 0.5) * 0.01;
        p->y += (rand()/(double)RAND_MAX - 0.5) * 0.01;
        if (p->x < 1 || p->x >= NX-1 || p->y < 1 || p->y >= NY-1 || obstacle[idx((int)p->x,(int)p->y)]){
            p->x = 8 + rand()%20;
            p->y = NY/2 - (NY/6) + rand()%(NY/3);
        }
    }
}

// Render to SDL texture
void render(SDL_Renderer *renderer, SDL_Texture *tex, Uint32 *pixels){
    int total = NX*NY;
    for(int i=0;i<total;i++){
        if (obstacle[i]) {
            pixels[i] = 0xFF333333; // wall dark gray
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
    // draw particles as bright pixels
    for(int i=0;i<particle_count;i++){
        int px = (int)particles[i].x;
        int py = (int)particles[i].y;
        if (px>=0 && px<NX && py>=0 && py<NY){
            int id = idx(px,py);
            Uint8 r = particles[i].r, g = particles[i].g, b = particles[i].b;
            pixels[id] = (255<<24) | (r<<16) | (g<<8) | b;
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
    SDL_Window *win = SDL_CreateWindow("LBM Wasserfluss-Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, 0);
    if (!win) die(SDL_GetError());
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!ren) die(SDL_GetError());
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, NX, NY);
    if (!tex) die(SDL_GetError());

    init_geometry();
    init_lbm();
    init_particles();

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
            }
        }
        int micro = 2;
        for(int m=0;m<micro;m++){
            lbm_step(t);
            t += dt / micro;
        }
        update_particles(dt);
        render(ren, tex, pixels);

        Uint64 now = SDL_GetPerformanceCounter();
        double elapsed = (double)(now - last) / SDL_GetPerformanceFrequency();
        //if (elapsed < dt) SDL_Delay((Uint32)((dt - elapsed)*1000));
        last = SDL_GetPerformanceCounter();
    }

    free(f); free(f_next); free(pixels); free(particles);
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
