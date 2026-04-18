// face.c
// Einzeldatei mit V4L2 MMAP Streaming, SDL2, HOG, vereinfachtem HPM und Online-Training.
// Build: gcc -O2 -o face face.c -lSDL2 -lm
// Run:   ./face

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <linux/videodev2.h>
#include <SDL2/SDL.h>

// ------------------------------ V4L2 MMAP --------------------------------

typedef struct {
    void  *start;
    size_t length;
} MMapBuf;

typedef struct {
        int fd;
        int width, height;
        uint32_t pixfmt;
        MMapBuf *buffers;
        unsigned int n_buffers;
} V4L2Ctx;

static int xioctl(int fd, int request, void *arg) {
    int r;
    do { r = ioctl(fd, request, arg); } while (r == -1 && errno == EINTR);
    return r;
}

int v4l2_open_mmap(V4L2Ctx *vc, const char *dev, int w, int h, uint32_t pixfmt) {
    memset(vc, 0, sizeof(*vc));
    vc->fd = open(dev, O_RDWR | O_NONBLOCK);
    if (vc->fd < 0) { perror("open v4l2"); return -1; }

    struct v4l2_capability cap;
    if (xioctl(vc->fd, VIDIOC_QUERYCAP, &cap) < 0) { perror("VIDIOC_QUERYCAP"); return -1; }
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) { fprintf(stderr, "No capture capability\n"); return -1; }
    if (!(cap.capabilities & V4L2_CAP_STREAMING))    { fprintf(stderr, "No streaming capability\n"); return -1; }

    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = w;
    fmt.fmt.pix.height = h;
    fmt.fmt.pix.pixelformat = pixfmt;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (xioctl(vc->fd, VIDIOC_S_FMT, &fmt) < 0) { perror("VIDIOC_S_FMT"); return -1; }
    vc->width  = fmt.fmt.pix.width;
    vc->height = fmt.fmt.pix.height;
    vc->pixfmt = fmt.fmt.pix.pixelformat;

    struct v4l2_requestbuffers req = {0};
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (xioctl(vc->fd, VIDIOC_REQBUFS, &req) < 0) { perror("VIDIOC_REQBUFS"); return -1; }
    if (req.count < 2) { fprintf(stderr, "Insufficient buffer memory\n"); return -1; }

    vc->buffers = calloc(req.count, sizeof(MMapBuf));
    vc->n_buffers = req.count;

    for (unsigned int i = 0; i < vc->n_buffers; ++i) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(vc->fd, VIDIOC_QUERYBUF, &buf) < 0) { perror("VIDIOC_QUERYBUF"); return -1; }

        vc->buffers[i].length = buf.length;
        vc->buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, vc->fd, buf.m.offset);
        if (vc->buffers[i].start == MAP_FAILED) { perror("mmap"); return -1; }
    }

    for (unsigned int i = 0; i < vc->n_buffers; ++i) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(vc->fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); return -1; }
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(vc->fd, VIDIOC_STREAMON, &type) < 0) { perror("VIDIOC_STREAMON"); return -1; }

    return 0;
}

int v4l2_dequeue_frame(V4L2Ctx *vc, void **data, size_t *len) {
    fd_set fds; FD_ZERO(&fds); FD_SET(vc->fd, &fds);
    struct timeval tv = { .tv_sec = 2, .tv_usec = 0 };
    int r = select(vc->fd + 1, &fds, NULL, NULL, &tv);
    if (r == -1) { if (errno != EINTR) perror("select"); return -1; }
    if (r == 0)  { fprintf(stderr, "select timeout\n"); return -1; }

    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    if (xioctl(vc->fd, VIDIOC_DQBUF, &buf) < 0) {
        if (errno == EAGAIN) return 1;
        perror("VIDIOC_DQBUF");
        return -1;
    }

    *data = vc->buffers[buf.index].start;
    *len  = buf.bytesused;

    if (xioctl(vc->fd, VIDIOC_QBUF, &buf) < 0) { perror("VIDIOC_QBUF"); return -1; }
    return 0;
}

void v4l2_close_mmap(V4L2Ctx *vc) {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (vc->fd >= 0) xioctl(vc->fd, VIDIOC_STREAMOFF, &type);
    if (vc->buffers) {
        for (unsigned int i = 0; i < vc->n_buffers; ++i) {
            if (vc->buffers[i].start && vc->buffers[i].start != MAP_FAILED)
                munmap(vc->buffers[i].start, vc->buffers[i].length);
        }
        free(vc->buffers);
    }
    if (vc->fd >= 0) close(vc->fd);
}

// ------------------------------ Utility & SDL2 ---------------------------

static inline uint8_t clamp_u8(int v) { return (uint8_t)(v < 0 ? 0 : v > 255 ? 255 : v); }

void yuyv_to_gray(uint8_t *dst, const uint8_t *src, int w, int h) {
    int pixels = w * h;
    for (int i = 0; i < pixels; i += 2) {
        uint8_t Y0 = src[2*i + 0];
        uint8_t Y1 = src[2*i + 2];
        dst[i] = Y0;
        dst[i+1] = Y1;
    }
}

typedef struct {
    SDL_Window *win;
    SDL_Renderer *ren;
    SDL_Texture *tex;
    int w, h;
} SDLCtx;

int sdl_init(SDLCtx *sc, int w, int h) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        fprintf(stderr, "SDL_Init err: %s\n", SDL_GetError());
        return -1;
    }
    sc->win = SDL_CreateWindow("HPM Occlusion Face Demo",
                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               w, h, SDL_WINDOW_SHOWN);
    if (!sc->win) { fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError()); return -1; }
    sc->ren = SDL_CreateRenderer(sc->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sc->ren) { fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError()); return -1; }
    sc->tex = SDL_CreateTexture(sc->ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, w, h);
    if (!sc->tex) { fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError()); return -1; }
    sc->w = w; sc->h = h;
    return 0;
}

void sdl_destroy(SDLCtx *sc) {
    if (sc->tex) SDL_DestroyTexture(sc->tex);
    if (sc->ren) SDL_DestroyRenderer(sc->ren);
    if (sc->win) SDL_DestroyWindow(sc->win);
    SDL_Quit();
}

void gray_to_rgb(uint8_t *rgb, const uint8_t *gray, int w, int h) {
    int N = w*h;
    for (int i = 0; i < N; ++i) {
        rgb[3*i+0] = gray[i];
        rgb[3*i+1] = gray[i];
        rgb[3*i+2] = gray[i];
    }
}

// ------------------------------ HOG -------------------------------------

typedef struct {
    int w, h;
    int cell;
    int bins;
    float *feat; // [ch * cw * bins]
    int cw, ch;
} HOG;

HOG *hog_compute(const uint8_t *gray, int w, int h, int cell, int bins) {
    HOG *hg = (HOG*)calloc(1, sizeof(HOG));
    hg->w = w; hg->h = h; hg->cell = cell; hg->bins = bins;
    hg->cw = w / cell; hg->ch = h / cell;
    hg->feat = (float*)calloc(hg->cw * hg->ch * bins, sizeof(float));
    for (int y = 1; y < h-1; ++y) {
        for (int x = 1; x < w-1; ++x) {
            float gx = (float)gray[y*w + (x+1)] - (float)gray[y*w + (x-1)];
            float gy = (float)gray[(y+1)*w + x] - (float)gray[(y-1)*w + x];
            float mag = sqrtf(gx*gx + gy*gy);
            float ang = atan2f(gy, gx);
            if (ang < 0) ang += 2.0f * M_PI;
            int bin = (int)(bins * ang / (2.0f * M_PI));
            if (bin < 0) bin = 0; if (bin >= bins) bin = bins - 1;
            int cx = x / cell, cy = y / cell;
            if (cx >= 0 && cx < hg->cw && cy >= 0 && cy < hg->ch) {
                hg->feat[(cy*hg->cw + cx)*bins + bin] += mag;
            }
        }
    }
    for (int cy = 0; cy < hg->ch; ++cy) {
        for (int cx = 0; cx < hg->cw; ++cx) {
            float sum = 0.f;
            float *f = &hg->feat[(cy*hg->cw + cx)*bins];
            for (int b = 0; b < bins; ++b) sum += f[b]*f[b];
            sum = sqrtf(sum) + 1e-6f;
            for (int b = 0; b < bins; ++b) f[b] /= sum;
        }
    }
    return hg;
}

void hog_free(HOG *hg) { if (!hg) return; free(hg->feat); free(hg); }

// ------------------------------ HPM -------------------------------------

#define MAX_PARTS   10
#define MAX_LMS     68
#define MAX_SHAPES  3
#define MAX_OCCS    4

typedef struct {
    int parent;
    int S;
    int O;
    int lm_indices[12];
    int lm_count;
    float spring_w[MAX_SHAPES][4];     // dx^2, dy^2, dx, dy
    uint8_t occ_mask[MAX_OCCS][12];    // occlusion per landmark per pattern
} Part;

typedef struct {
    int bins;
    int S;
    int part_id;
    float *tmpl;                        // size S*bins (visible)
    int ax[MAX_SHAPES], ay[MAX_SHAPES];// anchor offsets (cells) per shape
} Landmark;

typedef struct {
    int num_parts, num_lms;
    int cell;
    Part parts[MAX_PARTS];
    Landmark lms[MAX_LMS];
    float bias[MAX_PARTS][MAX_SHAPES][MAX_OCCS][MAX_SHAPES][MAX_OCCS];
} HPM;

void hpm_init_dummy(HPM *m, int hog_cell, int bins) {
    memset(m, 0, sizeof(*m));
    m->num_parts = 5;
    m->num_lms = 10;
    m->cell = hog_cell;
    m->parts[0].parent = -1; m->parts[0].S = 3; m->parts[0].O = 4;
    for (int p=1;p<=4;++p){ m->parts[p].parent=0; m->parts[p].S=3; m->parts[p].O=4; }

    int idx=0;
    // left eye
    m->parts[1].lm_indices[0]=idx; m->parts[1].lm_indices[1]=idx+1; m->parts[1].lm_count=2;
    m->lms[idx+0].bins=bins; m->lms[idx+0].S=3; m->lms[idx+0].part_id=1; m->lms[idx+0].tmpl=calloc(bins*3,sizeof(float));
    m->lms[idx+1].bins=bins; m->lms[idx+1].S=3; m->lms[idx+1].part_id=1; m->lms[idx+1].tmpl=calloc(bins*3,sizeof(float));
    for(int s=0;s<3;++s){ m->lms[idx+0].ax[s]=-6; m->lms[idx+0].ay[s]=-4; m->lms[idx+1].ax[s]=-5; m->lms[idx+1].ay[s]=-4; }
    idx+=2;
    // right eye
    m->parts[2].lm_indices[0]=idx; m->parts[2].lm_indices[1]=idx+1; m->parts[2].lm_count=2;
    m->lms[idx+0].bins=bins; m->lms[idx+0].S=3; m->lms[idx+0].part_id=2; m->lms[idx+0].tmpl=calloc(bins*3,sizeof(float));
    m->lms[idx+1].bins=bins; m->lms[idx+1].S=3; m->lms[idx+1].part_id=2; m->lms[idx+1].tmpl=calloc(bins*3,sizeof(float));
    for(int s=0;s<3;++s){ m->lms[idx+0].ax[s]= 5; m->lms[idx+0].ay[s]=-4; m->lms[idx+1].ax[s]= 6; m->lms[idx+1].ay[s]=-4; }
    idx+=2;
    // nose
    m->parts[3].lm_indices[0]=idx; m->parts[3].lm_indices[1]=idx+1; m->parts[3].lm_count=2;
    m->lms[idx+0].bins=bins; m->lms[idx+0].S=3; m->lms[idx+0].part_id=3; m->lms[idx+0].tmpl=calloc(bins*3,sizeof(float));
    m->lms[idx+1].bins=bins; m->lms[idx+1].S=3; m->lms[idx+1].part_id=3; m->lms[idx+1].tmpl=calloc(bins*3,sizeof(float));
    for(int s=0;s<3;++s){ m->lms[idx+0].ax[s]=0; m->lms[idx+0].ay[s]=-1; m->lms[idx+1].ax[s]=0; m->lms[idx+1].ay[s]=0; }
    idx+=2;
    // mouth
    m->parts[4].lm_indices[0]=idx; m->parts[4].lm_indices[1]=idx+1; m->parts[4].lm_indices[2]=idx+2; m->parts[4].lm_indices[3]=idx+3; m->parts[4].lm_count=4;
    for (int t=0;t<4;++t){ m->lms[idx+t].bins=bins; m->lms[idx+t].S=3; m->lms[idx+t].part_id=4; m->lms[idx+t].tmpl=calloc(bins*3,sizeof(float)); }
    m->lms[idx+0].ax[0]=-3; m->lms[idx+0].ay[0]=2;
    m->lms[idx+1].ax[0]=-1; m->lms[idx+1].ay[0]=2;
    m->lms[idx+2].ax[0]= 1; m->lms[idx+2].ay[0]=2;
    m->lms[idx+3].ax[0]= 3; m->lms[idx+3].ay[0]=2;
    for (int s=1;s<=2;++s){
        m->lms[idx+0].ax[s]=m->lms[idx+0].ax[0]; m->lms[idx+0].ay[s]=m->lms[idx+0].ay[0];
        m->lms[idx+1].ax[s]=m->lms[idx+1].ax[0]; m->lms[idx+1].ay[s]=m->lms[idx+1].ay[0];
        m->lms[idx+2].ax[s]=m->lms[idx+2].ax[0]; m->lms[idx+2].ay[s]=m->lms[idx+2].ay[0];
        m->lms[idx+3].ax[s]=m->lms[idx+3].ax[0]; m->lms[idx+3].ay[s]=m->lms[idx+3].ay[0];
    }
    idx+=4;

    for (int p=1;p<5;++p) for (int s=0;s<3;++s) {
        m->parts[p].spring_w[s][0] = -0.05f;
        m->parts[p].spring_w[s][1] = -0.05f;
        m->parts[p].spring_w[s][2] = 0.0f;
        m->parts[p].spring_w[s][3] = 0.0f;
    }
    for (int p=1;p<5;++p){
        for (int u=0; u<m->parts[p].lm_count; ++u) {
            m->parts[p].occ_mask[0][u] = 0;
            m->parts[p].occ_mask[1][u] = (u < m->parts[p].lm_count/2) ? 1 : 0;
            m->parts[p].occ_mask[2][u] = (u >= m->parts[p].lm_count/2) ? 1 : 0;
            m->parts[p].occ_mask[3][u] = 1;
        }
    }
    for (int p=1;p<5;++p)
    for (int sj=0; sj<3; ++sj)
    for (int oj=0; oj<4; ++oj)
    for (int si=0; si<3; ++si)
    for (int oi=0; oi<4; ++oi)
        m->bias[p][sj][oj][si][oi] = (oj==0 && oi==0) ? 0.2f : -0.05f;
}

void hpm_free(HPM *m){ for(int i=0;i<m->num_lms;++i) free(m->lms[i].tmpl); }

float score_landmark(const HOG *hg, const Landmark *lm, int cx, int cy, int shape_id) {
    if (cx < 0 || cy < 0 || cx >= hg->cw || cy >= hg->ch) return -1e6f;
    const float *feat = &hg->feat[(cy*hg->cw + cx) * hg->bins];
    const float *tmpl = &lm->tmpl[shape_id * lm->bins];
    float s=0.f; for(int b=0;b<lm->bins;++b) s+=feat[b]*tmpl[b];
    return s;
}

typedef struct {
    int found;
    int face_cx, face_cy;
    struct { int x, y; int occluded; } lm_pos[MAX_LMS];
    int lm_count;
    float score;
    int l_part_x[MAX_PARTS], l_part_y[MAX_PARTS];
    int shape_part[MAX_PARTS], occ_part[MAX_PARTS];
} Detection;

Detection hpm_detect(const HPM *m, const HOG *hg) {
    Detection det = {0};
    int shape_id = 1;
    float best_score = -1e9f; int best_cx=0, best_cy=0;

    for (int cy = hg->ch/6; cy < 5*hg->ch/6; ++cy) {
        for (int cx = hg->cw/6; cx < 5*hg->cw/6; ++cx) {
            float s=0.f;
            int lmpos_x[MAX_LMS], lmpos_y[MAX_LMS];
            for (int i=0;i<m->num_lms;++i) {
                int px = cx + m->lms[i].ax[shape_id];
                int py = cy + m->lms[i].ay[shape_id];
                lmpos_x[i]=px; lmpos_y[i]=py;
                s += score_landmark(hg, &m->lms[i], px, py, shape_id);
            }
            for (int p=1;p<m->num_parts;++p) {
                float ax=0, ay=0;
                for (int u=0; u<m->parts[p].lm_count; ++u) {
                    int idx = m->parts[p].lm_indices[u];
                    ax += lmpos_x[idx]; ay += lmpos_y[idx];
                }
                ax/=m->parts[p].lm_count; ay/=m->parts[p].lm_count;
                float dx=ax-cx, dy=ay-cy;
                float *w=m->parts[p].spring_w[shape_id];
                s += w[0]*dx*dx + w[1]*dy*dy + w[2]*dx + w[3]*dy;
                s += m->bias[p][shape_id][0][shape_id][0];
            }
            if (s>best_score){ best_score=s; best_cx=cx; best_cy=cy; }
        }
    }

    det.found = (best_score > 0.5f);
    det.face_cx=best_cx; det.face_cy=best_cy; det.score=best_score; det.lm_count=m->num_lms;
    if (det.found) {
        int pivot_x=best_cx, pivot_y=best_cy+2; int quad=2;
        for (int i=0;i<m->num_lms;++i) {
            int lx = best_cx + m->lms[i].ax[shape_id];
            int ly = best_cy + m->lms[i].ay[shape_id];
            det.lm_pos[i].x = lx * m->cell + m->cell/2;
            det.lm_pos[i].y = ly * m->cell + m->cell/2;
            int occ=0;
            if      (quad==0) occ=(lx<pivot_x && ly<pivot_y);
            else if (quad==1) occ=(lx>=pivot_x && ly<pivot_y);
            else if (quad==2) occ=(lx>=pivot_x && ly>=pivot_y);
            else if (quad==3) occ=(lx<pivot_x && ly>=pivot_y);
            det.lm_pos[i].occluded=occ;
        }
        for (int p=0;p<m->num_parts;++p) {
            if (m->parts[p].parent<0){ det.l_part_x[p]=best_cx; det.l_part_y[p]=best_cy; }
            else {
                float ax=0, ay=0;
                for (int u=0; u<m->parts[p].lm_count; ++u) {
                    int idx = m->parts[p].lm_indices[u];
                    ax += best_cx + m->lms[idx].ax[shape_id];
                    ay += best_cy + m->lms[idx].ay[shape_id];
                }
                ax/=m->parts[p].lm_count; ay/=m->parts[p].lm_count;
                det.l_part_x[p]=(int)ax; det.l_part_y[p]=(int)ay;
            }
            det.shape_part[p]=shape_id; det.occ_part[p]=0;
        }
    }
    return det;
}

void draw_landmark(SDL_Renderer *ren, int x, int y, int occluded) {
    SDL_Rect r = { x-2, y-2, 4, 4 };
    if (occluded) SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
    else SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
    SDL_RenderFillRect(ren, &r);
}

// ------------------------------ Online Training -------------------------

typedef struct { float *vec; int dim; } FeatureVec;

void append(float **dst, int *dim, const float *src, int n) {
    *dst = (float*)realloc(*dst, (*dim + n) * sizeof(float));
    memcpy(*dst + *dim, src, n * sizeof(float));
    *dim += n;
}

FeatureVec hpm_features(const HOG *hg, const HPM *m,
                        const int *l_part_x, const int *l_part_y,
                        const int *shape_part, const int *occ_part) {
    FeatureVec fv = {0};
    for (int p=0;p<m->num_parts;++p) {
        for (int u=0; u<m->parts[p].lm_count; ++u) {
            int lm = m->parts[p].lm_indices[u];
            int s  = shape_part[p];
            int o  = occ_part[p];
            int occ = m->parts[p].occ_mask[o][u];
            if (occ) { float z=0.f; append(&fv.vec, &fv.dim, &z, 1); continue; }
            int lx = l_part_x[p] + m->lms[lm].ax[s];
            int ly = l_part_y[p] + m->lms[lm].ay[s];
            if (lx<0||ly<0||lx>=hg->cw||ly>=hg->ch) { float z=0.f; append(&fv.vec,&fv.dim,&z,1); continue; }
            const float *feat = &hg->feat[(ly*hg->cw+lx)*hg->bins];
            append(&fv.vec, &fv.dim, feat, m->lms[lm].bins);
        }
    }
    for (int p=0;p<m->num_parts;++p) {
        int parent=m->parts[p].parent; if (parent<0) continue;
        float dx=(float)(l_part_x[p]-l_part_x[parent]);
        float dy=(float)(l_part_y[p]-l_part_y[parent]);
        float def[4]={dx*dx, dy*dy, dx, dy};
        append(&fv.vec, &fv.dim, def, 4);
    }
    for (int p=0;p<m->num_parts;++p) {
        if (m->parts[p].parent<0) continue;
        float one=1.f; append(&fv.vec, &fv.dim, &one, 1);
    }
    return fv;
}

typedef struct {
    float *w; int dim;
    float lr, l2, m;
} OnlineModel;

void online_init(OnlineModel *om, int dim, float lr, float l2, float m) {
    om->w=(float*)calloc(dim,sizeof(float)); om->dim=dim; om->lr=lr; om->l2=l2; om->m=m;
}
void online_free(OnlineModel *om){ free(om->w); }

static inline float dot(const float *a, const float *b, int n){ float s=0.f; for(int i=0;i<n;++i) s+=a[i]*b[i]; return s; }

float online_score(const OnlineModel *om, const FeatureVec *fv) {
    int d=(fv->dim<om->dim)?fv->dim:om->dim; return dot(om->w, fv->vec, d);
}

void online_l2_shrink(OnlineModel *om){
    if (om->l2<=0.f) return; float f=(1.f - om->lr*om->l2);
    for(int i=0;i<om->dim;++i) om->w[i]*=f;
}

float occlusion_rate_parts(const HPM *m, const int *occ_part){
    int occL=0, totL=0;
    for (int p=0;p<m->num_parts;++p)
        for (int u=0; u<m->parts[p].lm_count; ++u){ totL++; occL += m->parts[p].occ_mask[occ_part[p]][u]?1:0; }
    return totL ? ((float)occL/(float)totL) : 0.f;
}

void online_update_positive(OnlineModel *om, const FeatureVec *fv, float score){
    if (score>=1.f) return;
    int d=(fv->dim<om->dim)?fv->dim:om->dim;
    for(int i=0;i<d;++i) om->w[i] += om->lr * fv->vec[i];
}

void online_update_negative(OnlineModel *om, const FeatureVec *fv, float score, float rho){
    float margin = -(1.f - om->m * rho);
    if (score<=margin) return;
    int d=(fv->dim<om->dim)?fv->dim:om->dim;
    for(int i=0;i<d;++i) om->w[i] -= om->lr * fv->vec[i];
}

void online_train_step(const HOG *hg, const HPM *m, OnlineModel *om,
                       const int *l_part_x, const int *l_part_y,
                       const int *shape_part, const int *occ_part,
                       int is_positive) {
    FeatureVec fv = hpm_features(hg, m, l_part_x, l_part_y, shape_part, occ_part);
    float score = online_score(om, &fv);
    online_l2_shrink(om);
    if (is_positive) {
        online_update_positive(om, &fv, score);
        for (int o=1; o<m->parts[1].O; ++o) {
            int occ_var[MAX_PARTS]; memcpy(occ_var, occ_part, sizeof(int)*m->num_parts);
            for (int p=1;p<m->num_parts;++p) occ_var[p]=o;
            FeatureVec fv2 = hpm_features(hg, m, l_part_x, l_part_y, shape_part, occ_var);
            float sc2 = online_score(om, &fv2);
            online_update_positive(om, &fv2, sc2);
            free(fv2.vec);
        }
    } else {
        float rho = occlusion_rate_parts(m, occ_part);
        online_update_negative(om, &fv, score, rho);
    }
    free(fv.vec);
}

// ------------------------------ Main ------------------------------------

int main(int argc, char **argv) {
    const char *dev = "/dev/video0";
    int W = 640, H = 480;
    if (argc == 3) { W = atoi(argv[1]); H = atoi(argv[2]); }

    // Pixel-Format: stelle sicher, dass deine Kamera YUYV unterstützt (siehe v4l2-ctl --list-formats-ext)
    uint32_t pixfmt = V4L2_PIX_FMT_YUYV;

    V4L2Ctx vc;
    if (v4l2_open_mmap(&vc, dev, W, H, pixfmt) != 0) return 1;

    SDLCtx sc;
    if (sdl_init(&sc, vc.width, vc.height) != 0) { v4l2_close_mmap(&vc); return 1; }

    uint8_t *gray = (uint8_t*)malloc(vc.width * vc.height);
    uint8_t *rgb  = (uint8_t*)malloc(vc.width * vc.height * 3);

    int hog_cell = 8, hog_bins = 9;
    HPM model; hpm_init_dummy(&model, hog_cell, hog_bins);

    int approx_dim = (model.num_lms * hog_bins) + (4 * (model.num_parts-1)) + (model.num_parts-1);
    OnlineModel om; online_init(&om, approx_dim, 1e-3f, 1e-5f, 0.5f);

    int running = 1; SDL_Event e; int frame_idx = 0;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = 0;
        }

        void *data; size_t len;
        int r = v4l2_dequeue_frame(&vc, &data, &len);
        if (r < 0) break;
        if (r > 0) continue;

        if (vc.pixfmt == V4L2_PIX_FMT_YUYV) {
            yuyv_to_gray(gray, (uint8_t*)data, vc.width, vc.height);
        } else {
            // TODO: MJPEG decode if pixfmt == V4L2_PIX_FMT_MJPEG
            fprintf(stderr, "Unsupported pixfmt in this demo\n");
            break;
        }

        gray_to_rgb(rgb, gray, vc.width, vc.height);
        HOG *hg = hog_compute(gray, vc.width, vc.height, hog_cell, hog_bins);
        Detection det = hpm_detect(&model, hg);

        SDL_UpdateTexture(sc.tex, NULL, rgb, vc.width*3);
        SDL_SetRenderDrawColor(sc.ren, 0, 0, 0, 255);
        SDL_RenderClear(sc.ren);
        SDL_RenderCopy(sc.ren, sc.tex, NULL, NULL);

        if (det.found) {
            for (int i = 0; i < det.lm_count; ++i)
                draw_landmark(sc.ren, det.lm_pos[i].x, det.lm_pos[i].y, det.lm_pos[i].occluded);
            int cx_pix = det.face_cx * hog_cell + hog_cell/2;
            int cy_pix = det.face_cy * hog_cell + hog_cell/2;
            SDL_SetRenderDrawColor(sc.ren, 0, 128, 255, 255);
            SDL_Rect rc = { cx_pix-3, cy_pix-3, 6, 6 };
            SDL_RenderFillRect(sc.ren, &rc);

            online_train_step(hg, &model, &om,
                              det.l_part_x, det.l_part_y,
                              det.shape_part, det.occ_part, 1);
        }

        if ((frame_idx++ % 10) == 0) {
            for (int k = 0; k < 4; ++k) {
                int lnx[MAX_PARTS]={0}, lny[MAX_PARTS]={0}, shp[MAX_PARTS]={0}, occ[MAX_PARTS]={0};
                int rx = (rand() % (hg->cw-10)) + 5;
                int ry = (rand() % (hg->ch-10)) + 5;
                for (int p=0;p<model.num_parts;++p) {
                    if (model.parts[p].parent<0) { lnx[p]=rx; lny[p]=ry; }
                    else { lnx[p]=rx + (rand()%5 - 2); lny[p]=ry + (rand()%5 - 2); }
                    shp[p]=1; occ[p]=0;
                }
                online_train_step(hg, &model, &om, lnx, lny, shp, occ, 0);
            }
        }

        SDL_RenderPresent(sc.ren);
        hog_free(hg);
    }

    online_free(&om);
    hpm_free(&model);
    free(gray); free(rgb);
    sdl_destroy(&sc);
    v4l2_close_mmap(&vc);
    return 0;
}

