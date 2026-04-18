// rf_runtime_train.c
// Plain C runtime-trained Random Forest eye detector
// Dependencies: SDL2, Linux V4L2
// Compile: gcc -O3 -std=c11 rf_runtime_train.c -o rf_runtime_train -lSDL2 -lm

#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <SDL2/SDL.h>
#include <time.h>
#include <math.h>

#define PATCH_W 24
#define PATCH_H 24
#define PATCH_SZ (PATCH_W*PATCH_H)
#define MAX_POS 1024
#define MAX_NEG 4096
#define MAX_TREES 32
#define MAX_NODES 1024
#define MAX_FEATURES 4096

// --- Data structures ---
typedef struct { uint16_t a,b; } Pair;
typedef struct {
    int32_t feature; // -1 => leaf
    int32_t left;
    int32_t right;
    int32_t value; // leaf class 0/1
} Node;
typedef struct {
    Node nodes[MAX_NODES];
    int node_count;
} Tree;
typedef struct {
    Pair pairs[MAX_FEATURES];
    int K;
    int num_trees;
    Tree trees[MAX_TREES];
} Forest;

// Training dataset (flattened patches)
static uint8_t pos_patches[MAX_POS][PATCH_SZ];
static int pos_count = 0;
static uint8_t neg_patches[MAX_NEG][PATCH_SZ];
static int neg_count = 0;

// Random generator
static unsigned int rng_state = 123456789;
static inline unsigned int rnd_u32() {
    rng_state = rng_state * 1664525u + 1013904223u;
    return rng_state;
}
static inline int rnd_int(int n) { return (int)(rnd_u32() % (unsigned int)n); }

// --- V4L2 capture helpers (mmap) ---
struct buffer { void *start; size_t length; };
static int xioctl(int fd, int request, void *arg) {
    int r;
    do r = ioctl(fd, request, arg);
    while (r == -1 && errno == EINTR);
    return r;
}

// --- YUYV -> gray ---
static inline void yuyv_to_gray(const uint8_t *yuyv, uint8_t *gray, int w, int h) {
    int idx = 0;
    int stride = w*2;
    for (int r=0;r<h;r++) {
        const uint8_t *row = yuyv + r*stride;
        for (int c=0;c<w;c+=2) {
            uint8_t y0 = row[c*1 + 0];
            uint8_t u  = row[c*1 + 1];
            uint8_t y1 = row[c*1 + 2];
            uint8_t v  = row[c*1 + 3];
            gray[idx++] = y0;
            gray[idx++] = y1;
        }
    }
}

// --- Utility: copy patch centered at (cx,cy) into dest (24x24) with bounds check ---
static void copy_patch(const uint8_t *gray, int imgw, int imgh, int cx, int cy, uint8_t *dest) {
    int halfw = PATCH_W/2, halfh = PATCH_H/2;
    for (int y=0;y<PATCH_H;y++) {
        int sy = cy - halfh + y;
        if (sy < 0) sy = 0;
        if (sy >= imgh) sy = imgh-1;
        for (int x=0;x<PATCH_W;x++) {
            int sx = cx - halfw + x;
            if (sx < 0) sx = 0;
            if (sx >= imgw) sx = imgw-1;
            dest[y*PATCH_W + x] = gray[sy*imgw + sx];
        }
    }
}

// --- Feature computation for a patch (binary features) ---
static void compute_features_for_patch(const Pair *pairs, int K, const uint8_t *patch, uint8_t *out) {
    for (int i=0;i<K;i++) {
        out[i] = (patch[pairs[i].a] < patch[pairs[i].b]) ? 1 : 0;
    }
}

// --- Small decision tree learner (greedy) ---
// We build trees using binary features only. At each node we consider a random subset of features (mtry)
// and pick the feature that best splits the samples by information gain (entropy).
typedef struct {
    int *idx; // indices into dataset
    int n;
} IdxList;

static double entropy_from_counts(int c0, int c1) {
    int tot = c0 + c1;
    if (tot == 0) return 0.0;
    double p0 = (double)c0 / tot;
    double p1 = (double)c1 / tot;
    double e = 0.0;
    if (p0 > 0) e -= p0 * log2(p0);
    if (p1 > 0) e -= p1 * log2(p1);
    return e;
}

// dataset: features stored implicitly by patches arrays; labels: 1 for pos, 0 for neg
// We will create an array of pointers to patches and labels for training convenience.
typedef struct {
    uint8_t (*patches)[PATCH_SZ];
    int *labels;
    int n;
} Dataset;

// Evaluate a feature on dataset index i: returns 0/1
static inline int eval_feature_on_sample(const Pair *pairs, int K, const uint8_t *patch, int feat) {
    (void)K;
    return (patch[pairs[feat].a] < patch[pairs[feat].b]) ? 1 : 0;
}

// Build tree recursively; returns node index
static int build_tree_recursive(Forest *F, Tree *T, Dataset *ds, int *indices, int nidx, int depth, int max_depth, int mtry) {
    int node_id = T->node_count++;
    Node *node = &T->nodes[node_id];
    // compute class counts
    int cpos = 0, cneg = 0;
    for (int i=0;i<nidx;i++) {
        int lab = ds->labels[indices[i]];
        if (lab) cpos++; else cneg++;
    }
    // leaf conditions
    if (nidx == 0) {
        node->feature = -1; node->left = node->right = -1; node->value = 0;
        return node_id;
    }
    if (cpos == 0 || cneg == 0 || depth >= max_depth) {
        node->feature = -1; node->left = node->right = -1; node->value = (cpos >= cneg) ? 1 : 0;
        return node_id;
    }
    // choose best feature among mtry random features
    double base_entropy = entropy_from_counts(cneg, cpos);
    double best_gain = -1.0;
    int best_feat = -1;
    int best_left_count = 0, best_right_count = 0;
    // allocate temporary arrays for left/right indices
    int *left_idx = malloc(nidx * sizeof(int));
    int *right_idx = malloc(nidx * sizeof(int));
    int *best_left = NULL, *best_right = NULL;
    for (int t=0;t<mtry;t++) {
        int feat = rnd_int(F->K);
        int lc0=0, lc1=0, rc0=0, rc1=0;
        int li=0, ri=0;
        for (int i=0;i<nidx;i++) {
            int s = indices[i];
            int val = eval_feature_on_sample(F->pairs, F->K, ds->patches[s], feat);
            if (val) {
                right_idx[ri++] = s;
                if (ds->labels[s]) rc1++; else rc0++;
            } else {
                left_idx[li++] = s;
                if (ds->labels[s]) lc1++; else lc0++;
            }
        }
        double left_e = entropy_from_counts(lc0, lc1);
        double right_e = entropy_from_counts(rc0, rc1);
        double wleft = (double)(li) / nidx;
        double wright = (double)(ri) / nidx;
        double gain = base_entropy - (wleft * left_e + wright * right_e);
        if (gain > best_gain) {
            best_gain = gain;
            best_feat = feat;
            best_left_count = li;
            best_right_count = ri;
            // store best split
            if (best_left) free(best_left);
            if (best_right) free(best_right);
            best_left = malloc(li * sizeof(int));
            best_right = malloc(ri * sizeof(int));
            memcpy(best_left, left_idx, li * sizeof(int));
            memcpy(best_right, right_idx, ri * sizeof(int));
        }
    }
    free(left_idx); free(right_idx);
    if (best_feat == -1) {
        node->feature = -1; node->left = node->right = -1; node->value = (cpos >= cneg) ? 1 : 0;
        if (best_left) free(best_left);
        if (best_right) free(best_right);
        return node_id;
    }
    node->feature = best_feat;
    // create placeholders for children
    node->left = -1; node->right = -1; node->value = 0;
    // recursively build children
    int left_node = -1, right_node = -1;
    if (best_left_count > 0) {
        left_node = build_tree_recursive(F, T, ds, best_left, best_left_count, depth+1, max_depth, mtry);
    } else {
        // empty -> majority class of parent
        Node leaf = { .feature = -1, .left = -1, .right = -1, .value = (cpos >= cneg) ? 1 : 0 };
        int nid = T->node_count++;
        T->nodes[nid] = leaf;
        left_node = nid;
    }
    if (best_right_count > 0) {
        right_node = build_tree_recursive(F, T, ds, best_right, best_right_count, depth+1, max_depth, mtry);
    } else {
        Node leaf = { .feature = -1, .left = -1, .right = -1, .value = (cpos >= cneg) ? 1 : 0 };
        int nid = T->node_count++;
        T->nodes[nid] = leaf;
        right_node = nid;
    }
    node->left = left_node;
    node->right = right_node;
    if (best_left) free(best_left);
    if (best_right) free(best_right);
    return node_id;
}

// Train a forest with num_trees, each tree depth max_depth, mtry features per split
static void train_forest(Forest *F, int num_trees, int max_depth, int mtry) {
    if (pos_count == 0) {
        printf("No positive samples. Collect positives by pressing 's' and clicking.\n");
        return;
    }
    // build dataset arrays
    int total = pos_count + neg_count;
    uint8_t (*all_patches)[PATCH_SZ] = malloc(total * PATCH_SZ);
    int *labels = malloc(total * sizeof(int));
    for (int i=0;i<pos_count;i++) {
        memcpy(all_patches[i], pos_patches[i], PATCH_SZ);
        labels[i] = 1;
    }
    for (int i=0;i<neg_count;i++) {
        memcpy(all_patches[pos_count + i], neg_patches[i], PATCH_SZ);
        labels[pos_count + i] = 0;
    }
    Dataset ds;
    ds.patches = all_patches;
    ds.labels = labels;
    ds.n = total;
    // For each tree: bootstrap sample and build
    F->num_trees = num_trees;
    for (int t=0;t<num_trees;t++) {
        Tree *T = &F->trees[t];
        T->node_count = 0;
        // bootstrap indices
        int *indices = malloc(total * sizeof(int));
        for (int i=0;i<total;i++) indices[i] = (int)(rnd_u32() % total);
        // build tree
        build_tree_recursive(F, T, &ds, indices, total, 0, max_depth, mtry);
        free(indices);
        printf("Trained tree %d nodes=%d\n", t, T->node_count);
    }
    free(all_patches);
    free(labels);
    printf("Forest training complete: %d trees\n", F->num_trees);
}

// Evaluate a single tree iteratively on features array
static inline int eval_tree(const Tree *T, const uint8_t *features) {
    int idx = 0;
    while (1) {
        const Node *n = &T->nodes[idx];
        if (n->feature == -1) return n->value;
        int f = n->feature;
        if (features[f]) idx = n->right;
        else idx = n->left;
        if (idx < 0 || idx >= T->node_count) return 0;
    }
}

// Predict with forest (majority vote)
static inline int forest_predict(const Forest *F, const uint8_t *features) {
    int votes = 0;
    for (int t=0;t<F->num_trees;t++) votes += eval_tree(&F->trees[t], features);
    return (votes * 2 >= F->num_trees) ? 1 : 0;
}

// --- Detection over image (sliding windows) ---
typedef struct { int x,y,w,h; } Rect;
typedef struct { Rect r; float score; } Det;

static int detect_windows(const Forest *F, const uint8_t *gray, int imgw, int imgh, Det *dets, int maxdets, int stride) {
    int detc = 0;
    uint8_t features[MAX_FEATURES];
    for (int y=0; y + PATCH_H <= imgh; y += stride) {
        for (int x=0; x + PATCH_W <= imgw; x += stride) {
            // build patch on the fly into local buffer
            uint8_t patch[PATCH_SZ];
            for (int py=0;py<PATCH_H;py++) {
                int sy = y + py;
                for (int px=0;px<PATCH_W;px++) {
                    int sx = x + px;
                    patch[py*PATCH_W + px] = gray[sy*imgw + sx];
                }
            }
            compute_features_for_patch(F->pairs, F->K, patch, features);
            int pred = forest_predict(F, features);
            if (pred) {
                if (detc < maxdets) {
                    dets[detc].r.x = x; dets[detc].r.y = y; dets[detc].r.w = PATCH_W; dets[detc].r.h = PATCH_H;
                    dets[detc].score = 1.0f;
                    detc++;
                }
            }
        }
    }
    return detc;
}

// Simple NMS by center distance
static int nms(Det *in, int n, Det *out, int maxout) {
    int used[n];
    memset(used,0,sizeof(used));
    int outc = 0;
    for (int i=0;i<n;i++) {
        if (used[i]) continue;
        Det cur = in[i];
        for (int j=i+1;j<n;j++) {
            if (used[j]) continue;
            int cx1 = cur.r.x + cur.r.w/2;
            int cy1 = cur.r.y + cur.r.h/2;
            int cx2 = in[j].r.x + in[j].r.w/2;
            int cy2 = in[j].r.y + in[j].r.h/2;
            int dx = cx1 - cx2;
            int dy = cy1 - cy2;
            if (dx*dx + dy*dy < (PATCH_W/2)*(PATCH_W/2)) used[j] = 1;
        }
        if (outc < maxout) out[outc++] = cur;
    }
    return outc;
}

// --- Main: capture, UI, sampling, training, detection ---
int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr,"Usage: %s /dev/video0\n", argv[0]); return 1; }
    const char *dev = argv[1];

    // Initialize forest: generate random pairs
    Forest F;
    F.K = 2000; // default features; tune for speed/accuracy
    if (F.K > MAX_FEATURES) F.K = MAX_FEATURES;
    for (int i=0;i<F.K;i++) {
        F.pairs[i].a = rnd_int(PATCH_SZ);
        F.pairs[i].b = rnd_int(PATCH_SZ);
    }
    F.num_trees = 0;

    // Open camera
    int fd = open(dev, O_RDWR | O_NONBLOCK, 0);
    if (fd < 0) { perror("open"); return 1; }
    struct v4l2_format fmt;
    memset(&fmt,0,sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 640;
    fmt.fmt.pix.height = 480;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (xioctl(fd, VIDIOC_S_FMT, &fmt) < 0) { perror("VIDIOC_S_FMT"); close(fd); return 1; }
    int width = fmt.fmt.pix.width, height = fmt.fmt.pix.height;

    struct v4l2_requestbuffers req;
    memset(&req,0,sizeof(req));
    req.count = 4; req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; req.memory = V4L2_MEMORY_MMAP;
    if (xioctl(fd, VIDIOC_REQBUFS, &req) < 0) { perror("VIDIOC_REQBUFS"); close(fd); return 1; }
    struct buffer *buffers = calloc(req.count, sizeof(*buffers));
    for (unsigned int i=0;i<req.count;i++) {
        struct v4l2_buffer buf; memset(&buf,0,sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
        if (xioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) { perror("VIDIOC_QUERYBUF"); close(fd); return 1; }
        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[i].start == MAP_FAILED) { perror("mmap"); close(fd); return 1; }
    }
    for (unsigned int i=0;i<req.count;i++) {
        struct v4l2_buffer buf; memset(&buf,0,sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP; buf.index = i;
        xioctl(fd, VIDIOC_QBUF, &buf);
    }
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(fd, VIDIOC_STREAMON, &type);

    // SDL init
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { fprintf(stderr,"SDL_Init: %s\n", SDL_GetError()); return 1; }
    SDL_Window *win = SDL_CreateWindow("RF Runtime Train", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width, height);

    uint8_t *gray = malloc(width * height);
    uint8_t *rgb = malloc(width * height * 3);

    int sampling = 0;
    printf("Controls: s = toggle sampling mode; click to add positive patch; t = train forest; q = quit\n");

    Det raw_dets[1024], final_dets[1024];

    while (1) {
        // dequeue
        struct v4l2_buffer buf; memset(&buf,0,sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; buf.memory = V4L2_MEMORY_MMAP;
        if (xioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            if (errno == EAGAIN) { usleep(1000); continue; }
            perror("VIDIOC_DQBUF"); break;
        }
        yuyv_to_gray((uint8_t*)buffers[buf.index].start, gray, width, height);

        // detection if forest trained
        int nd = 0, nf = 0;
        if (F.num_trees > 0) {
            nd = detect_windows(&F, gray, width, height, raw_dets, 1024, 4); // stride 4
            nf = nms(raw_dets, nd, final_dets, 1024);
        }

        // convert gray->rgb
        for (int i=0;i<width*height;i++) { uint8_t v = gray[i]; rgb[3*i+0]=v; rgb[3*i+1]=v; rgb[3*i+2]=v; }
        SDL_UpdateTexture(tex, NULL, rgb, width*3);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);

        // draw detections
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        for (int i=0;i<nf;i++) {
            SDL_Rect r = { final_dets[i].r.x, final_dets[i].r.y, final_dets[i].r.w, final_dets[i].r.h };
            SDL_RenderDrawRect(ren, &r);
        }
        // draw sampling overlay
        if (sampling) {
            SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
            SDL_Rect r = {10,10,200,20};
            SDL_RenderDrawRect(ren, &r);
        }
        SDL_RenderPresent(ren);

        // requeue buffer
        xioctl(fd, VIDIOC_QBUF, &buf);

        // handle events
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) goto end;
            if (ev.type == SDL_KEYDOWN) {
                if (ev.key.keysym.sym == SDLK_q) goto end;
                if (ev.key.keysym.sym == SDLK_s) {
                    sampling = !sampling;
                    printf("Sampling %s\n", sampling ? "ON" : "OFF");
                }
                if (ev.key.keysym.sym == SDLK_t) {
                    // create negatives if none
                    if (neg_count < 1000) {
                        // sample random negatives from image
                        for (int i=0;i<1000 && neg_count < MAX_NEG;i++) {
                            int rx = rnd_int(width - PATCH_W) + PATCH_W/2;
                            int ry = rnd_int(height - PATCH_H) + PATCH_H/2;
                            copy_patch(gray, width, height, rx, ry, neg_patches[neg_count++]);
                        }
                    }
                    // train forest
                    printf("Training forest: pos=%d neg=%d\n", pos_count, neg_count);
                    train_forest(&F, 12, 8, 50); // 12 trees, depth 8, mtry 50
                }
            }
            if (ev.type == SDL_MOUSEBUTTONDOWN && sampling) {
                int mx = ev.button.x;
                int my = ev.button.y;
                if (pos_count < MAX_POS) {
                    copy_patch(gray, width, height, mx, my, pos_patches[pos_count++]);
                    printf("Collected positive %d at (%d,%d)\n", pos_count, mx, my);
                } else {
                    printf("Positive buffer full\n");
                }
            }
        }
    }

end:
    // cleanup
    enum v4l2_buf_type t2 = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(fd, VIDIOC_STREAMOFF, &t2);
    // unmap buffers
    // (we didn't store req.count here; for brevity, skip unmap in this demo)
    close(fd);
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    free(gray); free(rgb);
    return 0;
}
