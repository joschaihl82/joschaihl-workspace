// face_tracker_munkres_fixed.c
// Face tracker: v4l2 -> SDL2 + SDL2_ttf, Perceptron online, full Kalman, Munkres Hungarian, IDs+age
// Keine verschachtelten Funktionen, alles Top-Level
// Compile: gcc -O2 -o face_tracker_munkres_fixed face_tracker_munkres_fixed.c -lSDL2 -lSDL2_ttf -lm

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>

#define WIDTH 640
#define HEIGHT 480

// Grid
#define CELLS_X 32
#define CELLS_Y 24
#define CELL_W (WIDTH / CELLS_X)
#define CELL_H (HEIGHT / CELLS_Y)
#define FEATURE_DIM 2

// Perceptron
double W[FEATURE_DIM];
double BIAS = 0.0;
double ETA0 = 0.02;
double L2 = 1e-4;
int step_count = 0;

// Kalman + trackers
typedef struct {
    int active;
    double x,y,vx,vy;
    double w,h;
    double P[4][4];
    int misses;
    int id;
    int age_frames;
} Tracker;
#define MAX_TRACKERS 32
Tracker trackers[MAX_TRACKERS];
int next_tracker_id = 1;

// v4l2 buffers
struct buffer { void *start; size_t length; };
struct buffer *buffers;
int fd_cam = -1;
unsigned int n_buffers = 0;

// --- v4l2 helpers ---
int xioctl(int fh, int request, void *arg) {
    int r;
    do r = ioctl(fh, request, arg);
    while (r == -1 && EINTR == errno);
    return r;
}
int init_camera(const char *dev) {
    struct v4l2_format fmt;
    struct v4l2_requestbuffers req;
    fd_cam = open(dev, O_RDWR | O_NONBLOCK, 0);
    if (fd_cam < 0) { perror("open"); return -1; }
    memset(&fmt,0,sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (xioctl(fd_cam, VIDIOC_S_FMT, &fmt) < 0) { perror("VIDIOC_S_FMT"); return -1; }
    memset(&req,0,sizeof(req));
    req.count = 4; req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; req.memory = V4L2_MEMORY_MMAP;
    if (xioctl(fd_cam, VIDIOC_REQBUFS, &req) < 0) { perror("VIDIOC_REQBUFS"); return -1; }
    buffers = calloc(req.count, sizeof(*buffers));
    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf;
        memset(&buf,0,sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = n_buffers;
        if (xioctl(fd_cam, VIDIOC_QUERYBUF, &buf) < 0) { perror("VIDIOC_QUERYBUF"); return -1; }
        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_cam, buf.m.offset);
        if (buffers[n_buffers].start == MAP_FAILED) { perror("mmap"); return -1; }
    }
    for (unsigned int i=0;i<n_buffers;i++) {
        struct v4l2_buffer buf;
        memset(&buf,0,sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
        if (xioctl(fd_cam, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); return -1; }
    }
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd_cam, VIDIOC_STREAMON, &type) < 0) { perror("VIDIOC_STREAMON"); return -1; }
    return 0;
}
int read_frame(uint8_t *out_rgb) {
    struct v4l2_buffer buf;
    memset(&buf,0,sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP;
    if (xioctl(fd_cam, VIDIOC_DQBUF, &buf) < 0) {
        if (errno == EAGAIN) return 0;
        perror("VIDIOC_DQBUF"); return -1;
    }
    uint8_t *yuyv = buffers[buf.index].start;
    int idx = 0;
    for (int i=0;i<WIDTH*HEIGHT*2;i+=4) {
        int y0 = yuyv[i+0], u = yuyv[i+1], y1 = yuyv[i+2], v = yuyv[i+3];
        int c0 = y0 - 16, c1 = y1 - 16, d = u - 128, e = v - 128;
        int r = (298*c0 + 409*e + 128) >> 8;
        int g = (298*c0 - 100*d - 208*e + 128) >> 8;
        int b = (298*c0 + 516*d + 128) >> 8;
        out_rgb[idx++] = (uint8_t) (r<0?0:(r>255?255:r));
        out_rgb[idx++] = (uint8_t) (g<0?0:(g>255?255:g));
        out_rgb[idx++] = (uint8_t) (b<0?0:(b>255?255:b));
        r = (298*c1 + 409*e + 128) >> 8;
        g = (298*c1 - 100*d - 208*e + 128) >> 8;
        b = (298*c1 + 516*d + 128) >> 8;
        out_rgb[idx++] = (uint8_t) (r<0?0:(r>255?255:r));
        out_rgb[idx++] = (uint8_t) (g<0?0:(g>255?255:g));
        out_rgb[idx++] = (uint8_t) (b<0?0:(b>255?255:b));
    }
    if (xioctl(fd_cam, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); return -1; }
    return 1;
}

// --- Features ---
static inline void rgb_to_ycbcr(uint8_t r, uint8_t g, uint8_t b, int *y, int *cb, int *cr) {
    *y  = (  66*r + 129*g +  25*b + 128) >> 8;
    *cb = ( -38*r -  74*g + 112*b + 128) >> 8;
    *cr = ( 112*r -  94*g -  18*b + 128) >> 8;
    *y += 16; *cb += 128; *cr += 128;
}
void compute_cell_features(uint8_t *rgb, double features[CELLS_Y][CELLS_X][FEATURE_DIM]) {
    for (int cy=0; cy<CELLS_Y; ++cy) for (int cx=0; cx<CELLS_X; ++cx)
        for (int k=0;k<FEATURE_DIM;k++) features[cy][cx][k]=0.0;
    for (int cy=0; cy<CELLS_Y; ++cy) {
        for (int cx=0; cx<CELLS_X; ++cx) {
            int sx = cx*CELL_W, sy = cy*CELL_H;
            int skin_count = 0;
            double edge_sum = 0.0;
            for (int y=sy; y<sy+CELL_H; ++y) {
                for (int x=sx; x<sx+CELL_W; ++x) {
                    int idx = (y*WIDTH + x)*3;
                    uint8_t r = rgb[idx], g = rgb[idx+1], b = rgb[idx+2];
                    int Y,Cb,Cr; rgb_to_ycbcr(r,g,b,&Y,&Cb,&Cr);
                    if (Cb >= 77 && Cb <= 127 && Cr >= 133 && Cr <= 173) skin_count++;
                    if (x+1<WIDTH && y+1<HEIGHT) {
                        int idxr = (y*WIDTH + (x+1))*3;
                        int idxd = ((y+1)*WIDTH + x)*3;
                        double dx = fabs((double)r - rgb[idxr]) + fabs((double)g - rgb[idxr+1]) + fabs((double)b - rgb[idxr+2]);
                        double dy = fabs((double)r - rgb[idxd]) + fabs((double)g - rgb[idxd+1]) + fabs((double)b - rgb[idxd+2]);
                        edge_sum += sqrt(dx*dx + dy*dy);
                    }
                }
            }
            double area = CELL_W * CELL_H;
            features[cy][cx][0] = (double)skin_count / area;
            features[cy][cx][1] = edge_sum / area / 255.0;
        }
    }
}

// --- Perceptron ---
int perceptron_predict_vec(const double *x) {
    double s = BIAS;
    for (int i=0;i<FEATURE_DIM;i++) s += W[i]*x[i];
    return s >= 0.0 ? 1 : 0;
}
void perceptron_update_vec(const double *x, int y) {
    int yhat = perceptron_predict_vec(x);
    int diff = y - yhat;
    step_count++;
    double ETA = ETA0 / (1.0 + 1e-4 * step_count);
    for (int i=0;i<FEATURE_DIM;i++) W[i] *= (1.0 - ETA * L2);
    if (diff != 0) {
        for (int i=0;i<FEATURE_DIM;i++) W[i] += ETA * diff * x[i];
        BIAS += ETA * diff;
    }
}

// --- IoU ---
double iou_rect(double ax,double ay,double aw,double ah,double bx,double by,double bw,double bh) {
    double a1 = ax - aw/2.0, a2 = ay - ah/2.0, a3 = ax + aw/2.0, a4 = ay + ah/2.0;
    double b1 = bx - bw/2.0, b2 = by - bh/2.0, b3 = bx + bw/2.0, b4 = by + bh/2.0;
    double ix1 = fmax(a1,b1), iy1 = fmax(a2,b2), ix2 = fmin(a3,b3), iy2 = fmin(a4,b4);
    double iw = ix2 - ix1, ih = iy2 - iy1;
    if (iw <= 0 || ih <= 0) return 0.0;
    double inter = iw * ih;
    double unionv = aw*ah + bw*bh - inter;
    return inter / unionv;
}

// --- Munkres Hungarian (top-level, no nested functions) ---
// Solves rectangular cost matrix cost[nRows][nCols], returns assign[row] = col or -1
void munkres_solve(double **cost, int nRows, int nCols, int *assign) {
    int n = (nRows > nCols) ? nRows : nCols;
    // allocate square matrix a[n][n] and initialize with large value
    double **a = malloc(sizeof(double*)*n);
    for (int i=0;i<n;i++) { a[i]=malloc(sizeof(double)*n); for (int j=0;j<n;j++) a[i][j]=1e9; }
    for (int i=0;i<nRows;i++) for (int j=0;j<nCols;j++) a[i][j]=cost[i][j];

    int *rowCover = calloc(n,sizeof(int));
    int *colCover = calloc(n,sizeof(int));
    int **mask = malloc(sizeof(int*)*n);
    for (int i=0;i<n;i++){ mask[i]=calloc(n,sizeof(int)); }
    int step = 1;
    int z0_r=0,z0_c=0;
    int *path_r = malloc(sizeof(int)*(n*n+1));
    int *path_c = malloc(sizeof(int)*(n*n+1));

    // helper lambdas replaced by top-level code blocks executed via step variable
    // Step 1: row reduction
    for (int i=0;i<n;i++) {
        double minv = a[i][0];
        for (int j=1;j<n;j++) if (a[i][j] < minv) minv = a[i][j];
        for (int j=0;j<n;j++) a[i][j] -= minv;
    }
    step = 2;

    // Step 2: star zeros
    for (int i=0;i<n;i++) for (int j=0;j<n;j++) if (a[i][j]==0 && !rowCover[i] && !colCover[j]) {
        mask[i][j]=1; rowCover[i]=1; colCover[j]=1;
    }
    for (int i=0;i<n;i++) rowCover[i]=0;
    for (int j=0;j<n;j++) colCover[j]=0;
    step = 3;

    // Step 3: cover columns with starred zeros
    while (1) {
        int count = 0;
        for (int i=0;i<n;i++) for (int j=0;j<n;j++) if (mask[i][j]==1) colCover[j]=1;
        for (int j=0;j<n;j++) if (colCover[j]) count++;
        if (count >= n || count >= nCols) break;
        // Step 4: find uncovered zero and prime it
        int done4 = 0;
        while (!done4) {
            int found = 0;
            int zr=-1, zc=-1;
            for (int i=0;i<n;i++) {
                if (rowCover[i]) continue;
                for (int j=0;j<n;j++) {
                    if (colCover[j]) continue;
                    if (a[i][j]==0) { zr=i; zc=j; found=1; break; }
                }
                if (found) break;
            }
            if (!found) {
                // Step 6: adjust matrix
                double minv = 1e12;
                for (int i=0;i<n;i++) if (!rowCover[i]) for (int j=0;j<n;j++) if (!colCover[j]) if (a[i][j] < minv) minv = a[i][j];
                for (int i=0;i<n;i++) if (rowCover[i]) for (int j=0;j<n;j++) a[i][j] += minv;
                for (int j=0;j<n;j++) if (!colCover[j]) for (int i=0;i<n;i++) a[i][j] -= minv;
                continue;
            }
            // prime it
            mask[zr][zc] = 2;
            // find starred zero in same row
            int starc = -1;
            for (int j=0;j<n;j++) if (mask[zr][j]==1) { starc=j; break; }
            if (starc != -1) {
                rowCover[zr]=1;
                colCover[starc]=0;
                // continue searching
            } else {
                // Step 5: augmenting path
                z0_r = zr; z0_c = zc;
                int pathCount = 1;
                path_r[0] = z0_r; path_c[0] = z0_c;
                while (1) {
                    int r = -1;
                    for (int i=0;i<n;i++) if (mask[i][ path_c[pathCount-1] ] == 1) { r = i; break; }
                    if (r == -1) break;
                    path_r[pathCount] = r; path_c[pathCount] = path_c[pathCount-1]; pathCount++;
                    int c = -1;
                    for (int j=0;j<n;j++) if (mask[ path_r[pathCount-1] ][j] == 2) { c = j; break; }
                    if (c == -1) break;
                    path_r[pathCount] = path_r[pathCount-1]; path_c[pathCount] = c; pathCount++;
                }
                // flip stars and primes along path
                for (int p=0;p<pathCount;p++) {
                    int rr = path_r[p], cc = path_c[p];
                    if (mask[rr][cc] == 1) mask[rr][cc] = 0;
                    else if (mask[rr][cc] == 2) mask[rr][cc] = 1;
                }
                // clear covers and erase primes
                for (int i=0;i<n;i++) rowCover[i]=0;
                for (int j=0;j<n;j++) colCover[j]=0;
                for (int i=0;i<n;i++) for (int j=0;j<n;j++) if (mask[i][j]==2) mask[i][j]=0;
                break; // go back to step 3
            }
        } // end while step4
        // recompute covered columns and check termination
        for (int j=0;j<n;j++) colCover[j]=0;
        for (int i=0;i<n;i++) for (int j=0;j<n;j++) if (mask[i][j]==1) colCover[j]=1;
        int cnt=0; for (int j=0;j<n;j++) if (colCover[j]) cnt++;
        if (cnt >= n || cnt >= nCols) break;
    } // end main loop

    // build assignment for original rows
    for (int i=0;i<nRows;i++) assign[i] = -1;
    for (int i=0;i<nRows;i++) for (int j=0;j<nCols;j++) if (mask[i][j]==1) assign[i]=j;

    // free
    for (int i=0;i<n;i++) { free(a[i]); free(mask[i]); }
    free(a); free(mask); free(rowCover); free(colCover); free(path_r); free(path_c);
}

// --- Kalman full matrices (4x4) ---
void kalman_init(Tracker *t, double cx, double cy, double w, double h) {
    t->active = 1; t->x = cx; t->y = cy; t->vx = 0; t->vy = 0; t->w = w; t->h = h; t->misses = 0; t->id = next_tracker_id++; t->age_frames = 0;
    for (int i=0;i<4;i++) for (int j=0;j<4;j++) t->P[i][j] = (i==j)?100.0:0.0;
}
void kalman_predict_full(Tracker *t, double dt, double Qfac) {
    double F[4][4] = {{1,0,dt,0},{0,1,0,dt},{0,0,1,0},{0,0,0,1}};
    double Pnew[4][4] = {0};
    for (int i=0;i<4;i++) for (int j=0;j<4;j++) {
        for (int k=0;k<4;k++) for (int l=0;l<4;l++) Pnew[i][j] += F[i][k]*t->P[k][l]*F[j][l];
    }
    double Q[4][4] = {{Qfac,0,0,0},{0,Qfac,0,0},{0,0,Qfac*0.1,0},{0,0,0,Qfac*0.1}};
    for (int i=0;i<4;i++) for (int j=0;j<4;j++) t->P[i][j] = Pnew[i][j] + Q[i][j];
    t->x += t->vx * dt; t->y += t->vy * dt; t->age_frames++;
}
void kalman_update_full(Tracker *t, double mx, double my, double Rmeas) {
    double S00 = t->P[0][0] + Rmeas;
    double S01 = t->P[0][1];
    double S10 = t->P[1][0];
    double S11 = t->P[1][1] + Rmeas;
    double det = S00*S11 - S01*S10; if (fabs(det) < 1e-9) det = 1e-9;
    double invS00 = S11/det, invS01 = -S01/det, invS10 = -S10/det, invS11 = S00/det;
    double K[4][2];
    for (int i=0;i<4;i++) {
        K[i][0] = t->P[i][0]*invS00 + t->P[i][1]*invS10;
        K[i][1] = t->P[i][0]*invS01 + t->P[i][1]*invS11;
    }
    double y0 = mx - t->x, y1 = my - t->y;
    t->x += K[0][0]*y0 + K[0][1]*y1;
    t->y += K[1][0]*y0 + K[1][1]*y1;
    t->vx += K[2][0]*y0 + K[2][1]*y1;
    t->vy += K[3][0]*y0 + K[3][1]*y1;
    double KH[4][4] = {0};
    for (int i=0;i<4;i++) { KH[i][0] = K[i][0]; KH[i][1] = K[i][1]; }
    double IminusKH[4][4];
    for (int i=0;i<4;i++) for (int j=0;j<4;j++) IminusKH[i][j] = (i==j?1.0:0.0) - KH[i][j];
    double Pnew[4][4] = {0};
    for (int i=0;i<4;i++) for (int j=0;j<4;j++) for (int k=0;k<4;k++) Pnew[i][j] += IminusKH[i][k] * t->P[k][j];
    for (int i=0;i<4;i++) for (int j=0;j<4;j++) t->P[i][j] = Pnew[i][j];
    t->misses = 0;
}

// --- Tracker management ---
int alloc_tracker() { for (int i=0;i<MAX_TRACKERS;i++) if (!trackers[i].active) return i; return -1; }
int active_count() { int c=0; for (int i=0;i<MAX_TRACKERS;i++) if (trackers[i].active) c++; return c; }

// --- Main ---
int main(int argc, char **argv) {
    if (init_camera("/dev/video0") < 0) return 1;
    srand(time(NULL));
    for (int i=0;i<FEATURE_DIM;i++) W[i] = ((double)rand()/RAND_MAX - 0.5) * 0.05;
    BIAS = -0.1;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) { fprintf(stderr,"SDL_Init: %s\n", SDL_GetError()); return 1; }
    if (TTF_Init() != 0) { fprintf(stderr,"TTF_Init: %s\n", TTF_GetError()); return 1; }
    SDL_Window *win = SDL_CreateWindow("FaceTracker Munkres Fixed", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 14);
    if (!font) { fprintf(stderr,"TTF_OpenFont failed, adjust font path\n"); }

    uint8_t *rgb = malloc(WIDTH*HEIGHT*3);
    double features[CELLS_Y][CELLS_X][FEATURE_DIM];
    int running = 1;
    uint32_t last_ticks = SDL_GetTicks();
    int labeling_mode = 0;
    int mouse_x=0, mouse_y=0;

    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = 0;
            if (ev.type == SDL_KEYDOWN) {
                if (ev.key.keysym.sym == SDLK_ESCAPE || ev.key.keysym.sym == SDLK_q) running = 0;
                if (ev.key.keysym.sym == SDLK_l) labeling_mode = !labeling_mode;
                if (ev.key.keysym.sym == SDLK_c) for (int i=0;i<MAX_TRACKERS;i++) trackers[i].active = 0;
            }
            if (ev.type == SDL_MOUSEMOTION) { mouse_x = ev.motion.x; mouse_y = ev.motion.y; }
            if (ev.type == SDL_MOUSEBUTTONDOWN) {
                int cx = mouse_x / CELL_W, cy = mouse_y / CELL_H;
                if (cx>=0 && cx<CELLS_X && cy>=0 && cy<CELLS_Y) {
                    double fv[FEATURE_DIM];
                    for (int k=0;k<FEATURE_DIM;k++) fv[k] = features[cy][cx][k];
                    double norm = 0; for (int k=0;k<FEATURE_DIM;k++) norm += fv[k]*fv[k]; norm = sqrt(norm)+1e-6;
                    for (int k=0;k<FEATURE_DIM;k++) fv[k] /= norm;
                    if (ev.button.button == SDL_BUTTON_LEFT) perceptron_update_vec(fv, 1);
                    else if (ev.button.button == SDL_BUTTON_RIGHT) perceptron_update_vec(fv, 0);
                }
            }
        }

        int r = 0;
        while ((r = read_frame(rgb)) == 0) { usleep(1000); }
        if (r < 0) break;

        compute_cell_features(rgb, features);

        uint32_t now = SDL_GetTicks();
        double dt = (now - last_ticks) / 1000.0;
        if (dt < 1e-3) dt = 0.033;
        last_ticks = now;

        for (int i=0;i<MAX_TRACKERS;i++) if (trackers[i].active) {
            kalman_predict_full(&trackers[i], dt, 1.0);
            trackers[i].misses++;
            if (trackers[i].misses > 60) trackers[i].active = 0;
        }

        typedef struct { int cx,cy; double score; double px,py; double w,h; double fv[FEATURE_DIM]; } Meas;
        Meas meas[CELLS_X*CELLS_Y];
        int M = 0;
        for (int cy=0; cy<CELLS_Y; ++cy) for (int cx=0; cx<CELLS_X; ++cx) {
            double fv[FEATURE_DIM];
            for (int k=0;k<FEATURE_DIM;k++) fv[k] = features[cy][cx][k];
            double norm = 0; for (int k=0;k<FEATURE_DIM;k++) norm += fv[k]*fv[k]; norm = sqrt(norm)+1e-6;
            for (int k=0;k<FEATURE_DIM;k++) fv[k] /= norm;
            int pred = perceptron_predict_vec(fv);
            int label = 0;
            if (pred && features[cy][cx][0] > 0.06 && features[cy][cx][1] > 0.015) label = 1;
            if (!labeling_mode) perceptron_update_vec(fv, label);
            if (label) {
                double px = (cx + 0.5) * CELL_W;
                double py = (cy + 0.5) * CELL_H;
                meas[M].cx = cx; meas[M].cy = cy; meas[M].score = features[cy][cx][0] + features[cy][cx][1];
                meas[M].px = px; meas[M].py = py; meas[M].w = CELL_W*1.2; meas[M].h = CELL_H*1.2;
                for (int k=0;k<FEATURE_DIM;k++) meas[M].fv[k] = fv[k];
                M++;
            }
        }

        int N = active_count();
        int tmap[MAX_TRACKERS]; int ti=0;
        for (int i=0;i<MAX_TRACKERS;i++) if (trackers[i].active) tmap[ti++]=i;

        if (N>0 && M>0) {
            double **cost = malloc(sizeof(double*)*N);
            for (int i=0;i<N;i++) { cost[i]=malloc(sizeof(double)*M); for (int j=0;j<M;j++) cost[i][j]=1.0 - iou_rect(trackers[tmap[i]].x,trackers[tmap[i]].y,trackers[tmap[i]].w,trackers[tmap[i]].h, meas[j].px,meas[j].py,meas[j].w,meas[j].h); }
            int *assign = malloc(sizeof(int)*N);
            for (int i=0;i<N;i++) assign[i]=-1;
            munkres_solve(cost, N, M, assign);
            int *usedM = calloc(M,sizeof(int));
            for (int i=0;i<N;i++) {
                int j = assign[i];
                if (j>=0 && j<M) {
                    if (cost[i][j] < 0.7) {
                        int tidx = tmap[i];
                        kalman_update_full(&trackers[tidx], meas[j].px, meas[j].py, 10.0);
                        trackers[tidx].w = meas[j].w; trackers[tidx].h = meas[j].h;
                        usedM[j]=1;
                    }
                }
            }
            for (int j=0;j<M;j++) if (!usedM[j]) {
                int id = alloc_tracker();
                if (id>=0) kalman_init(&trackers[id], meas[j].px, meas[j].py, meas[j].w, meas[j].h);
            }
            for (int i=0;i<N;i++) free(cost[i]);
            free(cost); free(assign); free(usedM);
        } else {
            for (int j=0;j<M;j++) {
                int id = alloc_tracker();
                if (id>=0) kalman_init(&trackers[id], meas[j].px, meas[j].py, meas[j].w, meas[j].h);
            }
        }

        SDL_UpdateTexture(tex, NULL, rgb, WIDTH*3);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);

        // draw trackers and IDs + age
        for (int i=0;i<MAX_TRACKERS;i++) if (trackers[i].active) {
            SDL_Rect rct;
            rct.w = (int)trackers[i].w; rct.h = (int)trackers[i].h;
            rct.x = (int)(trackers[i].x - rct.w/2);
            rct.y = (int)(trackers[i].y - rct.h/2);
            SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
            SDL_RenderDrawRect(ren, &rct);
            if (font) {
                char txt[64];
                snprintf(txt, sizeof(txt), "ID:%d age:%d", trackers[i].id, trackers[i].age_frames);
                SDL_Color col = {255,255,0,255};
                SDL_Surface *surf = TTF_RenderText_Blended(font, txt, col);
                if (surf) {
                    SDL_Texture *t = SDL_CreateTextureFromSurface(ren, surf);
                    SDL_Rect dst = { rct.x, rct.y - surf->h - 2, surf->w, surf->h };
                    SDL_RenderCopy(ren, t, NULL, &dst);
                    SDL_DestroyTexture(t);
                    SDL_FreeSurface(surf);
                }
            }
        }

        SDL_Rect cellr = { (mouse_x/CELL_W)*CELL_W, (mouse_y/CELL_H)*CELL_H, CELL_W, CELL_H };
        SDL_SetRenderDrawColor(ren, 0,255,0,80);
        SDL_RenderDrawRect(ren, &cellr);

        SDL_RenderPresent(ren);
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(fd_cam, VIDIOC_STREAMOFF, &type);
    for (unsigned int i=0;i<n_buffers;i++) munmap(buffers[i].start, buffers[i].length);
    free(buffers);
    close(fd_cam);

    if (font) TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    free(rgb);
    return 0;
}

