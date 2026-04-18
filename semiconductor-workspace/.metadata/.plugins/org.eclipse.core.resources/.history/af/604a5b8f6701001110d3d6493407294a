// semiconductormachine1_swarm_contrast.c
// Lattice Boltzmann D2Q9 demo mit SDL2-Visualisierung
// Inlet: Partikel-Schwarm-Algorithmus; kontrastreichere Wellen; Partikel-Init verzögert
// Compile: gcc -O2 -std=c11 -lm -lSDL2 -lSDL2_ttf semiconductormachine1_swarm_contrast.c -o laticeboltzmann_swarm_contrast

#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NX 300
#define NY 150
#define SCALE 4
#define WINDOW_W (NX * SCALE)
#define WINDOW_H (NY * SCALE)
#define NDIR 9
#define MAX_PARTICLES 2000

// Swarm parameters
#define SWARM_COUNT 80
#define SWARM_INLET_X 12
#define SWARM_MIN_X 6
#define SWARM_MAX_X 40
#define SWARM_SIGMA 5.0
#define SWARM_MAX_SPEED 0.08
#define SWARM_COHESION 0.0025
#define SWARM_ALIGNMENT 0.06
#define SWARM_SEPARATION 0.09
#define SWARM_NOISE 0.008

// Particle tracer parameters
#define PARTICLE_INIT_DELAY 1.2   // seconds before tracer particles are created
#define PARTICLE_JITTER 0.004     // reduced random jitter

// LBM parameters
const double w[9] = {4.0/9.0, 1.0/9.0,1.0/9.0,1.0/9.0,1.0/9.0,1.0/36.0,1.0/36.0,1.0/36.0,1.0/36.0};
const int cx[9] = {0,1,0,-1,0,1,-1,-1,1};
const int cy[9] = {0,0,1,0,-1,1,1,-1,-1};
const int opp[9] = {0,3,4,1,2,7,8,5,6};

double *f, *f_next;
double rho_field[NX*NY];
double ux_field[NX*NY];
double uy_field[NX*NY];
unsigned char obstacle[NX*NY];
double speed_factor[NX*NY];

double tau = 0.53;
double omega;

// Particles for visualization
typedef struct { double x,y; double vx,vy; Uint8 r,g,b,a; } Particle;
Particle *particles;
int particle_count = 0;
int particles_initialized = 0;

// Swarm agent
typedef struct { double x,y; double vx,vy; } SwarmAgent;
SwarmAgent swarm[SWARM_COUNT];

// Utility
static inline int idx(int x,int y){ return y * NX + x; }

// D2Q9 equilibrium
static inline double feq(int k, double rho, double ux, double uy){
    double cu = 3.0 * (cx[k]*ux + cy[k]*uy);
    double u2 = 1.5 * (ux*ux + uy*uy);
    return w[k] * rho * (1.0 + cu + 0.5*cu*cu - u2);
}

void die(const char *msg){ fprintf(stderr,"%s\n",msg); exit(1); }

// Geometry
void init_geometry(){
    memset(obstacle,1,sizeof(obstacle));
    int mid = NY/2;
    int big_radius = NY/6;
    for(int y=mid-big_radius;y<=mid+big_radius;y++){
        for(int x=5;x<NX/2;x++){
            obstacle[idx(x,y)] = 0;
        }
    }
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
    for(int x=NX/2 - 4; x<NX/2 + 8; x++){
        for(int y=mid-big_radius; y<=mid+big_radius; y++){
            obstacle[idx(x,y)] = 0;
        }
    }
    for(int y=0;y<NY;y++){
        for(int x=0;x<NX;x++){
            int id = idx(x,y);
            if (obstacle[id]) { speed_factor[id] = 0.0; continue; }
            int up=y, down=y;
            while(up>0 && !obstacle[idx(x,up-1)]) up--;
            while(down<NY-1 && !obstacle[idx(x,down+1)]) down++;
            int local_width = down - up + 1;
            double factor = 1.0 + (double)(NY/6 - local_width) / (double)(NY/12);
            if (factor < 1.0) factor = 1.0;
            if (factor > 3.0) factor = 3.0;
            speed_factor[id] = factor;
        }
    }
}

// LBM init
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

// Swarm
void init_swarm(){
    srand((unsigned)time(NULL) ^ 0xC0FFEE);
    int mid = NY/2;
    int big_radius = NY/6;
    for(int i=0;i<SWARM_COUNT;i++){
        double rx = SWARM_INLET_X + (rand()/(double)RAND_MAX) * (SWARM_MAX_X - SWARM_INLET_X);
        double ry = (mid - big_radius) + (rand()/(double)RAND_MAX) * (2*big_radius);
        swarm[i].x = rx;
        swarm[i].y = ry;
        swarm[i].vx = 0.03 + (rand()/(double)RAND_MAX)*0.03;
        swarm[i].vy = (rand()/(double)RAND_MAX - 0.5) * 0.02;
    }
}

static inline void limit_speed(double *vx, double *vy, double vmax){
    double s = sqrt((*vx)*(*vx) + (*vy)*(*vy));
    if (s > vmax && s > 1e-12){
        *vx = (*vx) / s * vmax;
        *vy = (*vy) / s * vmax;
    }
}

void update_swarm(double dt){
    for(int i=0;i<SWARM_COUNT;i++){
        double cx = 0.0, cy = 0.0;
        double avx = 0.0, avy = 0.0;
        double sepx = 0.0, sepy = 0.0;
        int neighbors = 0;
        for(int j=0;j<SWARM_COUNT;j++){
            if (i==j) continue;
            double dx = swarm[j].x - swarm[i].x;
            double dy = swarm[j].y - swarm[i].y;
            double dist2 = dx*dx + dy*dy;
            double dist = sqrt(dist2);
            double neigh_radius = 12.0;
            if (dist < neigh_radius && dist > 1e-6){
                neighbors++;
                cx += swarm[j].x;
                cy += swarm[j].y;
                avx += swarm[j].vx;
                avy += swarm[j].vy;
                sepx -= (dx) / (dist*dist + 1e-6);
                sepy -= (dy) / (dist*dist + 1e-6);
            }
        }
        double dvx = 0.0, dvy = 0.0;
        if (neighbors > 0){
            cx /= neighbors; cy /= neighbors;
            avx /= neighbors; avy /= neighbors;
            dvx += (cx - swarm[i].x) * SWARM_COHESION;
            dvy += (cy - swarm[i].y) * SWARM_COHESION;
            dvx += (avx - swarm[i].vx) * SWARM_ALIGNMENT;
            dvy += (avy - swarm[i].vy) * SWARM_ALIGNMENT;
            dvx += sepx * SWARM_SEPARATION;
            dvy += sepy * SWARM_SEPARATION;
        }
        dvx += 0.003; // stronger bias to the right for more pronounced waves
        dvx += (rand()/(double)RAND_MAX - 0.5) * SWARM_NOISE;
        dvy += (rand()/(double)RAND_MAX - 0.5) * SWARM_NOISE;

        swarm[i].vx += dvx;
        swarm[i].vy += dvy;
        limit_speed(&swarm[i].vx, &swarm[i].vy, SWARM_MAX_SPEED);

        swarm[i].x += swarm[i].vx * dt * 50.0;
        swarm[i].y += swarm[i].vy * dt * 50.0;

        if (swarm[i].x < SWARM_MIN_X) swarm[i].x = SWARM_MIN_X + (rand()/(double)RAND_MAX)*2.0, swarm[i].vx = fabs(swarm[i].vx);
        if (swarm[i].x > SWARM_MAX_X) swarm[i].x = SWARM_MAX_X - (rand()/(double)RAND_MAX)*2.0, swarm[i].vx = -fabs(swarm[i].vx);
        if (swarm[i].y < 1) swarm[i].y = 1, swarm[i].vy = fabs(swarm[i].vy);
        if (swarm[i].y > NY-2) swarm[i].y = NY-2, swarm[i].vy = -fabs(swarm[i].vy);

        int ix = (int)floor(swarm[i].x);
        int iy = (int)floor(swarm[i].y);
        if (ix >= 0 && ix < NX && iy >= 0 && iy < NY && obstacle[idx(ix,iy)]){
            int found = 0;
            for(int r=1;r<10 && !found;r++){
                for(int dy=-r; dy<=r && !found; dy++){
                    int ny = iy + dy;
                    if (ny < 0 || ny >= NY) continue;
                    if (!obstacle[idx(ix,ny)]){
                        swarm[i].y = ny + 0.5;
                        swarm[i].vy = 0;
                        found = 1;
                    }
                }
            }
            if (!found){
                swarm[i].x = SWARM_INLET_X + (rand()/(double)RAND_MAX)*5.0;
                swarm[i].y = NY/2;
                swarm[i].vx = 0.03;
                swarm[i].vy = 0.0;
            }
        }
    }
}

// Particle tracers
void init_particles(){
    if (particles_initialized) return;
    particles = (Particle*)malloc(sizeof(Particle)*MAX_PARTICLES);
    particle_count = 0;
    srand((unsigned)time(NULL) ^ 0xBEEF);
    for(int i=0;i<MAX_PARTICLES;i++){
        int x = 20 + rand() % 40; // start further downstream to avoid inlet foam
        int y = NY/2 - (NY/6) + rand() % (NY/3);
        if (obstacle[idx(x,y)]) continue;
        Particle p;
        p.x = x + (rand()/(double)RAND_MAX);
        p.y = y + (rand()/(double)RAND_MAX);
        p.vx = 0; p.vy = 0;
        p.r = 200 + rand()%55; p.g = 120 + rand()%80; p.b = 255; p.a = 255;
        particles[particle_count++] = p;
    }
    particles_initialized = 1;
}

void update_particles(double dt){
    for(int i=0;i<particle_count;i++){
        Particle *p = &particles[i];
        int ix = (int)floor(p->x);
        int iy = (int)floor(p->y);
        if (ix < 1 || ix >= NX-1 || iy < 1 || iy >= NY-1 || obstacle[idx(ix,iy)]){
            p->x = 20 + rand()%40;
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
        // reduced jitter to avoid foam at start
        p->x += (rand()/(double)RAND_MAX - 0.5) * PARTICLE_JITTER;
        p->y += (rand()/(double)RAND_MAX - 0.5) * PARTICLE_JITTER;
        if (p->x < 1 || p->x >= NX-1 || p->y < 1 || p->y >= NY-1 || obstacle[idx((int)p->x,(int)p->y)]){
            p->x = 20 + rand()%40;
            p->y = NY/2 - (NY/6) + rand()%(NY/3);
        }
    }
}

// LBM step
void lbm_step(double t, double dt){
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

    // inlet based on swarm influence
    for(int y=1;y<NY-1;y++){
        int x = 6;
        int id = idx(x,y);
        if (obstacle[id]) continue;
        double influence = 0.0;
        double weight_sum = 0.0;
        for(int s=0;s<SWARM_COUNT;s++){
            double dy = swarm[s].y - (double)y;
            double kernel = exp(- (dy*dy) / (2.0 * SWARM_SIGMA * SWARM_SIGMA));
            double dx = swarm[s].x - (double)x;
            double xfall = exp(- (dx*dx) / (2.0 * (SWARM_SIGMA*1.2) * (SWARM_SIGMA*1.2)));
            double wgt = kernel * xfall;
            influence += swarm[s].vx * wgt;
            weight_sum += wgt;
        }
        double u_in = 0.0;
        if (weight_sum > 1e-12) u_in = influence / weight_sum;
        // stronger base and amplification for higher contrast waves
        double inlet_base = 0.055; // increased base
        double factor = speed_factor[id];
        double u_final = inlet_base * (1.0 + u_in*14.0) * factor; // stronger amplification
        if (u_final < 0.0) u_final = 0.0;
        if (u_final > 0.35) u_final = 0.35;
        ux_field[id] = u_final;
        uy_field[id] = 0.0;
        rho_field[id] = 1.0;
        for(int k=0;k<NDIR;k++){
            f[id*NDIR + k] = feq(k, rho_field[id], ux_field[id], uy_field[id]);
        }
    }

    // collision
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

    // streaming
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

    // bounce-back
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

// Render with stronger contrast mapping
void render(SDL_Renderer *renderer, SDL_Texture *tex, Uint32 *pixels){
    int total = NX*NY;
    for(int i=0;i<total;i++){
        if (obstacle[i]) {
            pixels[i] = 0xFF2B2B2B; // slightly darker wall
        } else {
            double vx = ux_field[i], vy = uy_field[i];
            double mag = sqrt(vx*vx + vy*vy);
            // stronger scaling for contrast
            double scaled = pow(mag * 80.0, 1.25);
            double s = fmin(1.0, scaled);
            // color map: low speeds bluish dark, high speeds warm bright
            int r = (int)(20 + 235 * s);
            int g = (int)(30 + 200 * s * (1.0 - 0.2*s));
            int b = (int)(80 + 175 * (1.0 - 0.6*s));
            // ensure bounds
            if (r<0) r=0; if (r>255) r=255;
            if (g<0) g=0; if (g>255) g=255;
            if (b<0) b=0; if (b>255) b=255;
            pixels[i] = (255<<24) | (r<<16) | (g<<8) | b;
        }
    }
    // draw particles if initialized
    if (particles_initialized){
        for(int i=0;i<particle_count;i++){
            int px = (int)particles[i].x;
            int py = (int)particles[i].y;
            if (px>=0 && px<NX && py>=0 && py<NY){
                int id = idx(px,py);
                Uint8 r = particles[i].r, g = particles[i].g, b = particles[i].b;
                pixels[id] = (255<<24) | (r<<16) | (g<<8) | b;
            }
        }
    }
    // draw swarm agents as yellow dots
    for(int s=0;s<SWARM_COUNT;s++){
        int sx = (int)round(swarm[s].x);
        int sy = (int)round(swarm[s].y);
        if (sx>=0 && sx<NX && sy>=0 && sy<NY){
            int id = idx(sx,sy);
            pixels[id] = (255<<24) | (255<<16) | (220<<8) | 40;
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
    SDL_Window *win = SDL_CreateWindow("LBM Schwarm Kontrast", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, 0);
    if (!win) die(SDL_GetError());
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!ren) die(SDL_GetError());
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, NX, NY);
    if (!tex) die(SDL_GetError());

    init_geometry();
    init_lbm();
    init_swarm();
    // do not init particles here; wait until after a short delay to avoid foam
    particles_initialized = 0;

    Uint32 *pixels = (Uint32*)malloc(sizeof(Uint32)*NX*NY);
    if (!pixels) die("pixels malloc failed");

    int running = 1;
    double t = 0.0;
    double dt = 0.016;
    Uint64 last = SDL_GetPerformanceCounter();
    double time_since_start = 0.0;

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
            update_swarm(dt / micro);
            lbm_step(t, dt / micro);
            t += dt / micro;
            time_since_start += dt / micro;
        }
        // initialize particles after delay to avoid initial foam
        if (!particles_initialized && time_since_start >= PARTICLE_INIT_DELAY){
            init_particles();
        }
        if (particles_initialized) update_particles(dt);
        render(ren, tex, pixels);

        Uint64 now = SDL_GetPerformanceCounter();
        double elapsed = (double)(now - last) / SDL_GetPerformanceFrequency();
        last = SDL_GetPerformanceCounter();
    }

    if (f) free(f);
    if (f_next) free(f_next);
    if (pixels) free(pixels);
    if (particles) free(particles);
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
