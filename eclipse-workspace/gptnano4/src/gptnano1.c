// mini_vocab_train_opt_modelsave_fixed2.c
// Trainer with full attention backprop, mini-batching, shuffling,
// FP16 storage option, embedding int8 quantization (per-row scale),
// performance optimizations, and model checkpointing via --model / -m <filename>.
// Only argument: data.txt (optional: --model <file>)
// Build: gcc -O3 -march=native -std=c11 -o mini_vocab_train_opt_modelsave_fixed2 mini_vocab_train_opt_modelsave_fixed2.c -lopenblas -lm

#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <stdint.h>
#include <cblas.h>
#include <unistd.h>
#include <errno.h>

#ifndef D
#define D 64
#endif
#ifndef H
#define H 4
#endif
#define SEQLEN 8

typedef float f32;
typedef uint16_t f16bits;

// ---------- util ----------
static void *aligned_malloc(size_t size, size_t align){
    void *p = NULL;
    if(posix_memalign(&p, align, size) != 0) return NULL;
    return p;
}
static void *xmalloc(size_t n){ void *p = malloc(n); if(!p){ fprintf(stderr,"alloc fail %zu\n",n); exit(1);} return p; }
static void *xcalloc(size_t a, size_t b){ void *p = calloc(a,b); if(!p){ fprintf(stderr,"calloc fail\n"); exit(1);} return p; }
static void *xrealloc(void *p, size_t n){ void *q = realloc(p,n); if(!q){ fprintf(stderr,"realloc fail %zu\n",n); exit(1);} return q; }
static float frnd(){ return (rand()+1.0f)/((float)RAND_MAX+2.0f); }
static int get_num_cpus(){ long n = sysconf(_SC_NPROCESSORS_ONLN); return (n>0)?(int)n:1; }
static void shuffle_int(int *a, int n){ for(int i=n-1;i>0;i--){ int j = rand() % (i+1); int t=a[i]; a[i]=a[j]; a[j]=t; } }

// ---------- FP16 helpers (IEEE 754 half) ----------
static inline f16bits float_to_f16(float x){
    uint32_t f = *((uint32_t*)&x);
    uint32_t sign = (f >> 16) & 0x8000;
    uint32_t val = (f & 0x7fffffff);
    if(val > 0x47ffefff){
        if(val == 0x7f800000) return (f16bits)(sign | 0x7c00);
        return (f16bits)(sign | 0x7c00);
    }
    if(val < 0x38800000){
        uint32_t t = (val & 0x7fffff) | 0x800000;
        int shift = 113 - (val >> 23);
        t = (t >> shift) + ((t >> (shift-1)) & 1);
        return (f16bits)(sign | (t & 0x3ff));
    }
    uint32_t exp = ((val >> 23) - 127 + 15) & 0x1f;
    uint32_t mant = (val >> 13) & 0x3ff;
    return (f16bits)(sign | (exp << 10) | mant);
}
static inline float f16_to_float(f16bits h){
    uint32_t sign = (h & 0x8000) << 16;
    uint32_t exp = (h & 0x7c00) >> 10;
    uint32_t mant = (h & 0x03ff);
    uint32_t f;
    if(exp == 0){
        if(mant == 0){
            f = sign;
        } else {
            exp = 1;
            while((mant & 0x0400) == 0){ mant <<= 1; exp--; }
            mant &= 0x03ff;
            uint32_t e = (exp + (127 - 15)) << 23;
            uint32_t m = mant << 13;
            f = sign | e | m;
        }
    } else if(exp == 0x1f){
        f = sign | 0x7f800000 | (mant << 13);
    } else {
        uint32_t e = (exp + (127 - 15)) << 23;
        uint32_t m = mant << 13;
        f = sign | e | m;
    }
    return *((float*)&f);
}

// ---------- small hash vocab ----------
typedef struct VNode { char *s; int id; int count; struct VNode *next; } VNode;
typedef struct { VNode **buckets; int nbuckets; int size; } Vocab;
static unsigned long hash_str(const char *s){
    unsigned long h = 1469598103934665603UL;
    while(*s){ h ^= (unsigned char)*s; h *= 1099511628211UL; s++; }
    return h;
}
static Vocab *vocab_new(int nb){ Vocab *v = xmalloc(sizeof(Vocab)); v->nbuckets = nb; v->buckets = xcalloc(nb, sizeof(VNode*)); v->size = 0; return v; }
static void vocab_free(Vocab *v){
    for(int i=0;i<v->nbuckets;i++){ VNode *n=v->buckets[i]; while(n){ VNode *nx=n->next; free(n->s); free(n); n=nx; } }
    free(v->buckets); free(v);
}
static int vocab_find(Vocab *v, const char *s){
    unsigned long h = hash_str(s) % v->nbuckets;
    VNode *n = v->buckets[h];
    while(n){ if(strcmp(n->s,s)==0) return n->id; n=n->next; }
    return -1;
}
static int vocab_add(Vocab *v, const char *s){
    int idx = vocab_find(v,s);
    if(idx>=0){
        unsigned long h = hash_str(s) % v->nbuckets;
        VNode *n = v->buckets[h];
        while(n){ if(strcmp(n->s,s)==0){ n->count++; break; } n=n->next; }
        return idx;
    }
    VNode *node = xmalloc(sizeof(VNode));
    node->s = strdup(s);
    node->id = v->size;
    node->count = 1;
    unsigned long h = hash_str(s) % v->nbuckets;
    node->next = v->buckets[h];
    v->buckets[h] = node;
    v->size++;
    return node->id;
}
static const char *vocab_get_token(Vocab *v, int id){
    for(int i=0;i<v->nbuckets;i++){ VNode *n=v->buckets[i]; while(n){ if(n->id==id) return n->s; n=n->next; } }
    return NULL;
}

// ---------- tokenizer ----------
static int load_and_build_vocab(const char *fn, int **out_tokens, Vocab **out_vocab){
    FILE *f = fopen(fn,"rb"); if(!f){ fprintf(stderr,"cannot open %s\n",fn); return 0; }
    if(fseek(f,0,SEEK_END)!=0){ fclose(f); return 0; }
    long n = ftell(f); rewind(f);
    char *buf = xmalloc(n+1);
    size_t r = fread(buf,1,n,f); fclose(f);
    buf[r]=0;
    int nb = 4096;
    if(n > 1<<20) nb = 16384;
    Vocab *v = vocab_new(nb);
    int *tokens = xmalloc(sizeof(int) * (r/2 + 4));
    int tcount = 0;
    char *p = buf;
    while(*p){
        while(*p && isspace((unsigned char)*p)) p++;
        if(!*p) break;
        char *start = p;
        while(*p && !isspace((unsigned char)*p)) p++;
        size_t len = p - start;
        char *tok = xmalloc(len+1);
        memcpy(tok, start, len); tok[len]=0;
        int id = vocab_add(v, tok);
        free(tok);
        tokens[tcount++] = id;
    }
    free(buf);
    *out_tokens = tokens;
    *out_vocab = v;
    return tcount;
}

// ---------- numerics ----------
static inline float gelu_f(float x){ return 0.5f*x*(1.0f + tanhf(0.7978845608f*(x + 0.044715f*x*x*x))); }
static inline float dgelu_f(float x){
    float t = tanhf(0.7978845608f*(x + 0.044715f*x*x*x));
    float dt = 0.7978845608f*(1.0f + 0.134145f*(x*x));
    return 0.5f*(1.0f + t) + 0.5f*x*(1.0f - t*t)*dt;
}
static void softmax_row_f(float *x, int len){
    float m = x[0];
    for(int i=1;i<len;i++) if(x[i]>m) m=x[i];
    float s=0.0f;
    for(int i=0;i<len;i++){ x[i] = expf(x[i]-m); s += x[i]; }
    for(int i=0;i<len;i++) x[i] /= s;
}
static float cross_entropy_and_probs_f(const float *logits_in, float *probs_out, int len, int target){
    float m = logits_in[0];
    for(int i=1;i<len;i++) if(logits_in[i]>m) m=logits_in[i];
    float s=0.0f;
    for(int i=0;i<len;i++){ probs_out[i] = expf(logits_in[i]-m); s += probs_out[i]; }
    for(int i=0;i<len;i++) probs_out[i] /= s;
    float loss = -logf(probs_out[target] + 1e-12f);
    return loss;
}
static void layernorm_f(float *x, int n, float eps){
    float mean=0.0f;
    for(int i=0;i<n;i++) mean += x[i];
    mean /= n;
    float var=0.0f;
    for(int i=0;i<n;i++){ float t=x[i]-mean; var += t*t; }
    var /= n;
    float denom = 1.0f / sqrtf(var + eps);
    for(int i=0;i<n;i++) x[i] = (x[i]-mean) * denom;
}

// ---------- AdamW ----------
typedef struct { float *m; float *v; int n; } AdamState;
static AdamState *adam_new(int n){ AdamState *s = xmalloc(sizeof(AdamState)); s->m = xcalloc(n, sizeof(float)); s->v = xcalloc(n, sizeof(float)); s->n = n; return s; }
static void adam_free(AdamState *s){ free(s->m); free(s->v); free(s); }
static void adam_update(float *W, float *gW, AdamState *st, int n, float lr, float wd, int t){
    const float b1 = 0.9f, b2 = 0.999f, eps = 1e-8f;
    float b1t = powf(b1, t), b2t = powf(b2, t);
    for(int i=0;i<n;i++){
        float g = gW[i];
        st->m[i] = b1 * st->m[i] + (1.0f - b1) * g;
        st->v[i] = b2 * st->v[i] + (1.0f - b2) * g * g;
        float mhat = st->m[i] / (1.0f - b1t);
        float vhat = st->v[i] / (1.0f - b2t);
        W[i] -= lr * (mhat / (sqrtf(vhat) + eps) + wd * W[i]);
    }
}

// ---------- Embedding quantization helpers (int8 per-row) ----------
typedef struct {
    int dim;        // dimension (replaces any 'D' field to avoid macro collision)
    int V;
    int8_t *q;      // V x dim int8
    float *scale;   // V scales
} EmbQ;

static EmbQ *embq_new(int V, int dim){
    EmbQ *e = xmalloc(sizeof(EmbQ));
    e->V = V; e->dim = dim;
    e->q = xmalloc((size_t)V * dim * sizeof(int8_t));
    e->scale = xmalloc((size_t)V * sizeof(float));
    return e;
}
static void embq_free(EmbQ *e){ free(e->q); free(e->scale); free(e); }

static EmbQ *quantize_embeddings_int8(const float *Wemb, int V, int dim){
    EmbQ *e = embq_new(V, dim);
    for(int v=0; v<V; v++){
        const float *row = &Wemb[(size_t)v*dim];
        float maxabs = 0.0f;
        for(int d=0; d<dim; d++){ float a = fabsf(row[d]); if(a>maxabs) maxabs=a; }
        float scale = (maxabs > 0.0f) ? (maxabs / 127.0f) : 1e-8f;
        e->scale[v] = scale;
        int8_t *qrow = &e->q[(size_t)v*dim];
        for(int d=0; d<dim; d++){
            int vq = (int)roundf(row[d] / scale);
            if (vq > 127) vq = 127;
            if (vq < -128) vq = -128;
            qrow[d] = (int8_t)vq;
        }
    }
    return e;
}

// dequantize a block of rows [start, start+rows) into float buffer dest (rows x dim)
static void embq_dequant_block(const EmbQ *e, int start, int rows, float *dest){
    int dim = e->dim;
    for(int r=0; r<rows; r++){
        int v = start + r;
        const int8_t *qrow = &e->q[(size_t)v*dim];
        float s = e->scale[v];
        float *dstrow = &dest[(size_t)r*dim];
        for(int d=0; d<dim; d++) dstrow[d] = ((float)qrow[d]) * s;
    }
}

// ---------- Model save (binary) ----------
static int save_model_binary(const char *fn, int epoch, int V, int dim, int heads,
                             const float *Wemb, const float *Wq, const float *Wk, const float *Wv,
                             const float *Wproj, const float *Wff1, const float *Wff2){
    FILE *f = fopen(fn, "wb");
    if(!f) return 0;
    uint32_t magic = 0x4B4E4F57; // "KNOW"
    uint32_t version = 1;
    fwrite(&magic, sizeof(magic), 1, f);
    fwrite(&version, sizeof(version), 1, f);
    fwrite(&epoch, sizeof(epoch), 1, f);
    fwrite(&V, sizeof(V), 1, f);
    fwrite(&dim, sizeof(dim), 1, f);
    fwrite(&heads, sizeof(heads), 1, f);
    uint32_t seqlen = SEQLEN;
    fwrite(&seqlen, sizeof(seqlen), 1, f);
    fwrite(Wemb, sizeof(float), (size_t)V * dim, f);
    fwrite(Wq, sizeof(float), (size_t)dim * dim, f);
    fwrite(Wk, sizeof(float), (size_t)dim * dim, f);
    fwrite(Wv, sizeof(float), (size_t)dim * dim, f);
    fwrite(Wproj, sizeof(float), (size_t)dim * dim, f);
    fwrite(Wff1, sizeof(float), (size_t)dim * (dim*4), f);
    fwrite(Wff2, sizeof(float), (size_t)(dim*4) * dim, f);
    fclose(f);
    return 1;
}

// ---------- main (optimized trainer with model saving) ----------
int main(int argc, char **argv){
    if(argc < 2){ fprintf(stderr,"Usage: %s data.txt [--model knowledge.ai]\n", argv[0]); return 1; }
    const char *fn = argv[1];
    const char *model_out = "knowledge.ai";

    for(int i=2;i<argc;i++){
        if((strcmp(argv[i],"--model")==0 || strcmp(argv[i],"-m")==0) && i+1<argc){
            model_out = argv[++i];
        }
    }

    int *tokens = NULL;
    Vocab *vocab = NULL;
    int N = load_and_build_vocab(fn, &tokens, &vocab);
    if(!N){ fprintf(stderr,"Failed to load tokens\n"); return 1; }
    int V = vocab->size;
    printf("Loaded %d tokens, vocab size %d\n", N, V);

    size_t dim = D;
    int H_local = H;
    int HD_local = dim / H_local;
    int EPOCHS = 80;
    if(N < 1000) EPOCHS = 400;
    else if(N < 10000) EPOCHS = 200;
    else if(N < 50000) EPOCHS = 100;
    else EPOCHS = 40;
    float LR = 0.003f;
    int cpus = get_num_cpus();
    if(getenv("OPENBLAS_NUM_THREADS")==NULL){
        char buf[16]; snprintf(buf,sizeof(buf),"%d",cpus); setenv("OPENBLAS_NUM_THREADS", buf, 1);
    }
    unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)getpid();
    srand(seed);
    printf("Config: D=%zu H=%d SEQLEN=%d epochs=%d lr=%.6f threads=%d seed=%u\n",
           dim, H_local, SEQLEN, EPOCHS, LR, cpus, seed);

    int windows = (N - 1) / SEQLEN;
    int max_batch = 32;
    int BATCH = windows < max_batch ? (windows>0?windows:1) : max_batch;
    if(windows < 4) BATCH = 1;
    printf("Mini-batch size: %d (sequence windows: %d)\n", BATCH, windows);

    size_t VxD = (size_t)V * dim;
    float *Wemb = aligned_malloc(sizeof(float) * VxD, 64);
    float *Wq = aligned_malloc(sizeof(float) * dim * dim, 64);
    float *Wk = aligned_malloc(sizeof(float) * dim * dim, 64);
    float *Wv = aligned_malloc(sizeof(float) * dim * dim, 64);
    float *Wproj = aligned_malloc(sizeof(float) * dim * dim, 64);
    float *Wff1 = aligned_malloc(sizeof(float) * dim * (dim*4), 64);
    float *Wff2 = aligned_malloc(sizeof(float) * (dim*4) * dim, 64);
    if(!Wemb || !Wq || !Wk || !Wv || !Wproj || !Wff1 || !Wff2) { fprintf(stderr,"aligned alloc failed\n"); return 1; }

    for(size_t i=0;i<VxD;i++) Wemb[i] = (frnd()-0.5f)*0.1f;
    for(size_t i=0;i<(size_t)dim*dim;i++) Wq[i] = Wk[i] = Wv[i] = Wproj[i] = (frnd()-0.5f)*0.1f;
    for(size_t i=0;i<(size_t)dim*(dim*4);i++) Wff1[i] = (frnd()-0.5f)*0.1f;
    for(size_t i=0;i<(size_t)(dim*4)*dim;i++) Wff2[i] = (frnd()-0.5f)*0.1f;

    AdamState *st_Wemb = adam_new((int)VxD);
    AdamState *st_Wq = adam_new(dim*dim);
    AdamState *st_Wk = adam_new(dim*dim);
    AdamState *st_Wv = adam_new(dim*dim);
    AdamState *st_Wproj = adam_new(dim*dim);
    AdamState *st_Wff1 = adam_new(dim*(dim*4));
    AdamState *st_Wff2 = adam_new((dim*4)*dim);

    int use_emb_quant = 0;
    EmbQ *embq = NULL;
    size_t mem_est = VxD * sizeof(float);
    if(V > 20000 || mem_est > (size_t)1<<26){
        use_emb_quant = 1;
        embq = quantize_embeddings_int8(Wemb, V, (int)dim);
        printf("Embedding quantization enabled (int8 per-row). V=%d D=%zu\n", V, dim);
    } else {
        printf("Embedding quantization disabled (small vocab).\n");
    }

    int use_fp16_params = 0;
    f16bits *Wq_f16 = NULL, *Wk_f16 = NULL, *Wv_f16 = NULL, *Wproj_f16 = NULL;
    if((size_t)dim*dim > 1<<14){
        use_fp16_params = 1;
        Wq_f16 = xmalloc(sizeof(f16bits) * dim * dim);
        Wk_f16 = xmalloc(sizeof(f16bits) * dim * dim);
        Wv_f16 = xmalloc(sizeof(f16bits) * dim * dim);
        Wproj_f16 = xmalloc(sizeof(f16bits) * dim * dim);
        for(int i=0;i<dim*dim;i++){ Wq_f16[i] = float_to_f16(Wq[i]); Wk_f16[i] = float_to_f16(Wk[i]); Wv_f16[i] = float_to_f16(Wv[i]); Wproj_f16[i] = float_to_f16(Wproj[i]); }
        printf("FP16 parameter storage enabled for projection matrices.\n");
    }

    float *emb_batch = aligned_malloc(sizeof(float) * BATCH * SEQLEN * dim, 64);
    float *Qb = aligned_malloc(sizeof(float) * BATCH * SEQLEN * dim, 64);
    float *Kb = aligned_malloc(sizeof(float) * BATCH * SEQLEN * dim, 64);
    float *Vb = aligned_malloc(sizeof(float) * BATCH * SEQLEN * dim, 64);
    float *scores = aligned_malloc(sizeof(float) * BATCH * H_local * SEQLEN * SEQLEN, 64);
    float *P = aligned_malloc(sizeof(float) * BATCH * H_local * SEQLEN * SEQLEN, 64);
    float *head_out = aligned_malloc(sizeof(float) * BATCH * SEQLEN * dim, 64);
    float *out_batch = aligned_malloc(sizeof(float) * BATCH * SEQLEN * dim, 64);
    float *pooled = aligned_malloc(sizeof(float) * BATCH * dim, 64);
    float *ff1 = aligned_malloc(sizeof(float) * BATCH * (dim*4), 64);
    float *ff_out = aligned_malloc(sizeof(float) * BATCH * dim, 64);
    float *logits = aligned_malloc(sizeof(float) * BATCH * V, 64);
    float *probs = aligned_malloc(sizeof(float) * BATCH * V, 64);

    float *gWemb = xcalloc((size_t)V*dim, sizeof(float));
    float *gWq = xcalloc(dim*dim, sizeof(float));
    float *gWk = xcalloc(dim*dim, sizeof(float));
    float *gWv = xcalloc(dim*dim, sizeof(float));
    float *gWproj = xcalloc(dim*dim, sizeof(float));
    float *gWff1 = xcalloc(dim*(dim*4), sizeof(float));
    float *gWff2 = xcalloc((dim*4)*dim, sizeof(float));

    int *starts = xmalloc(sizeof(int) * windows);
    for(int i=0;i<windows;i++) starts[i] = i * SEQLEN;

    int tstep = 1;
    for(int ep=1; ep<=EPOCHS; ep++){
        shuffle_int(starts, windows);
        memset(gWemb,0,sizeof(float)*(size_t)V*dim);
        memset(gWq,0,sizeof(float)*dim*dim);
        memset(gWk,0,sizeof(float)*dim*dim);
        memset(gWv,0,sizeof(float)*dim*dim);
        memset(gWproj,0,sizeof(float)*dim*dim);
        memset(gWff1,0,sizeof(float)*dim*(dim*4));
        memset(gWff2,0,sizeof(float)*(dim*4)*dim);

        double epoch_loss = 0.0;
        int steps = 0;

        for(int w=0; w < windows; w += BATCH){
            int bsz = (w + BATCH <= windows) ? BATCH : (windows - w);
            steps++;

            for(int bi=0; bi<bsz; bi++){
                int start = starts[w+bi];
                for(int t=0;t<SEQLEN;t++){
                    int id = tokens[start + t];
                    float *dst = &emb_batch[(bi*SEQLEN + t)*dim];
                    if(use_emb_quant){
                        const int8_t *qrow = &embq->q[(size_t)id*dim];
                        float s = embq->scale[id];
                        for(int d=0; d<dim; d++) dst[d] = ((float)qrow[d]) * s;
                    } else {
                        const float *src = &Wemb[(size_t)id*dim];
                        memcpy(dst, src, sizeof(float)*dim);
                    }
                }
            }

            int rows = bsz * SEQLEN;
            if(use_fp16_params){
                float *Wq_tmp = aligned_malloc(sizeof(float) * dim * dim, 64);
                float *Wk_tmp = aligned_malloc(sizeof(float) * dim * dim, 64);
                float *Wv_tmp = aligned_malloc(sizeof(float) * dim * dim, 64);
                for(int i=0;i<dim*dim;i++){ Wq_tmp[i] = f16_to_float(Wq_f16[i]); Wk_tmp[i] = f16_to_float(Wk_f16[i]); Wv_tmp[i] = f16_to_float(Wv_f16[i]); }
                cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, rows, dim, dim, 1.0f, emb_batch, dim, Wq_tmp, dim, 0.0f, Qb, dim);
                cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, rows, dim, dim, 1.0f, emb_batch, dim, Wk_tmp, dim, 0.0f, Kb, dim);
                cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, rows, dim, dim, 1.0f, emb_batch, dim, Wv_tmp, dim, 0.0f, Vb, dim);
                free(Wq_tmp); free(Wk_tmp); free(Wv_tmp);
            } else {
                cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, rows, dim, dim, 1.0f, emb_batch, dim, Wq, dim, 0.0f, Qb, dim);
                cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, rows, dim, dim, 1.0f, emb_batch, dim, Wk, dim, 0.0f, Kb, dim);
                cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, rows, dim, dim, 1.0f, emb_batch, dim, Wv, dim, 0.0f, Vb, dim);
            }

            for(int bi=0; bi<bsz; bi++){
                float *Qseq = &Qb[(bi*SEQLEN)*dim];
                float *Kseq = &Kb[(bi*SEQLEN)*dim];
                float *Vseq = &Vb[(bi*SEQLEN)*dim];
                float *scores_seq = &scores[(bi*H_local*SEQLEN*SEQLEN)];
                float *Pseq = &P[(bi*H_local*SEQLEN*SEQLEN)];
                float *head_out_seq = &head_out[(bi*SEQLEN)*dim];

                for(int h=0; h<H_local; h++){
                    int off = h * HD_local;
                    for(int a=0;a<SEQLEN;a++){
                        for(int b2=0;b2<SEQLEN;b2++){
                            float s=0.0f;
                            for(int d=0; d<HD_local; d++) s += Qseq[a*dim + off + d] * Kseq[b2*dim + off + d];
                            scores_seq[h*SEQLEN*SEQLEN + a*SEQLEN + b2] = s / sqrtf((float)HD_local);
                        }
                    }
                    for(int a=0;a<SEQLEN;a++) softmax_row_f(&scores_seq[h*SEQLEN*SEQLEN + a*SEQLEN], SEQLEN);
                    for(int idx=0; idx<SEQLEN*SEQLEN; idx++) Pseq[h*SEQLEN*SEQLEN + idx] = scores_seq[h*SEQLEN*SEQLEN + idx];
                    for(int a=0;a<SEQLEN;a++){
                        for(int d=0; d<HD_local; d++){
                            float val=0.0f;
                            for(int b2=0;b2<SEQLEN;b2++) val += Pseq[h*SEQLEN*SEQLEN + a*SEQLEN + b2] * Vseq[b2*dim + off + d];
                            head_out_seq[a*dim + off + d] = val;
                        }
                    }
                }
            }

            if(use_fp16_params){
                float *Wproj_tmp = aligned_malloc(sizeof(float) * dim * dim, 64);
                for(int i=0;i<dim*dim;i++) Wproj_tmp[i] = f16_to_float(Wproj_f16[i]);
                cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, rows, dim, dim, 1.0f, head_out, dim, Wproj_tmp, dim, 0.0f, out_batch, dim);
                free(Wproj_tmp);
            } else {
                cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, rows, dim, dim, 1.0f, head_out, dim, Wproj, dim, 0.0f, out_batch, dim);
            }
            for(int r=0;r<rows;r++){
                for(int d=0; d<dim; d++) out_batch[r*dim + d] += emb_batch[r*dim + d];
            }

            for(int bi=0; bi<bsz; bi++){
                for(int d=0; d<dim; d++){
                    float s = 0.0f;
                    for(int t=0;t<SEQLEN;t++) s += out_batch[(bi*SEQLEN + t)*dim + d];
                    pooled[bi*dim + d] = s / SEQLEN;
                }
                layernorm_f(&pooled[bi*dim], dim, 1e-5f);
                cblas_sgemv(CblasRowMajor, CblasNoTrans, dim, dim*4, 1.0f, Wff1, dim*4, &pooled[bi*dim], 1, 0.0f, &ff1[bi*(dim*4)], 1);
                for(int j=0;j<dim*4;j++) ff1[bi*(dim*4) + j] = gelu_f(ff1[bi*(dim*4) + j]);
                cblas_sgemv(CblasRowMajor, CblasNoTrans, dim*4, dim, 1.0f, Wff2, dim, &ff1[bi*(dim*4)], 1, 0.0f, &ff_out[bi*dim], 1);
                for(int d=0; d<dim; d++) ff_out[bi*dim + d] += pooled[bi*dim + d];
            }

            for(int bi=0; bi<bsz; bi++){
                for(int v=0; v<V; v++){
                    float s = 0.0f;
                    if(use_emb_quant){
                        const int8_t *qrow = &embq->q[(size_t)v*dim];
                        float sc = embq->scale[v];
                        for(int d=0; d<dim; d++) s += ((float)qrow[d]) * sc * ff_out[bi*dim + d];
                    } else {
                        const float *erow = &Wemb[(size_t)v*dim];
                        for(int d=0; d<dim; d++) s += erow[d] * ff_out[bi*dim + d];
                    }
                    logits[bi*V + v] = s;
                }
                int target = tokens[ starts[w+bi] + SEQLEN ];
                float loss = cross_entropy_and_probs_f(&logits[bi*V], &probs[bi*V], V, target);
                epoch_loss += loss;
            }

            // Backward: exact attention backprop per sequence (mirrors earlier full implementation)
            for(int bi=0; bi<bsz; bi++){
                int target = tokens[ starts[w+bi] + SEQLEN ];
                float *probs_b = &probs[bi*V];
                for(int v=0; v<V; v++) probs_b[v] = probs_b[v] - (v==target ? 1.0f : 0.0f);

                for(int v=0; v<V; v++){
                    float pv = probs_b[v];
                    if(pv==0.0f) continue;
                    for(int d=0; d<dim; d++) gWemb[(size_t)v*dim + d] += pv * ff_out[bi*dim + d];
                }

                float *g_ff = xmalloc(sizeof(float)*dim);
                for(int d=0; d<dim; d++) g_ff[d]=0.0f;
                for(int v=0; v<V; v++){
                    float pv = probs_b[v];
                    if(pv==0.0f) continue;
                    if(use_emb_quant){
                        const int8_t *qrow = &embq->q[(size_t)v*dim];
                        float sc = embq->scale[v];
                        for(int d=0; d<dim; d++) g_ff[d] += ((float)qrow[d]) * sc * pv;
                    } else {
                        const float *erow = &Wemb[(size_t)v*dim];
                        for(int d=0; d<dim; d++) g_ff[d] += erow[d] * pv;
                    }
                }

                for(int j=0;j<dim*4;j++){
                    for(int d=0; d<dim; d++){
                        gWff2[j*dim + d] += ff1[bi*(dim*4) + j] * g_ff[d];
                    }
                }
                float *g_ff1 = xmalloc(sizeof(float)*(dim*4));
                for(int j=0;j<dim*4;j++){ g_ff1[j]=0.0f; for(int d=0; d<dim; d++) g_ff1[j] += Wff2[j*dim + d] * g_ff[d]; }
                for(int j=0;j<dim*4;j++) g_ff1[j] *= dgelu_f(ff1[bi*(dim*4) + j]);
                for(int d=0; d<dim; d++){
                    for(int j=0;j<dim*4;j++){
                        gWff1[d*(dim*4) + j] += pooled[bi*dim + d] * g_ff1[j];
                    }
                }
                float *g_pooled = xmalloc(sizeof(float)*dim);
                for(int d=0; d<dim; d++){
                    g_pooled[d] = g_ff[d];
                    for(int j=0;j<dim*4;j++) g_pooled[d] += Wff1[d*(dim*4) + j] * g_ff1[j];
                }

                float *g_out_seq = xmalloc(sizeof(float) * SEQLEN * dim);
                for(int t=0;t<SEQLEN;t++) for(int d=0; d<dim; d++) g_out_seq[t*dim + d] = g_pooled[d] / SEQLEN;

                float *g_head_out = xmalloc(sizeof(float) * SEQLEN * dim);
                if(use_fp16_params){
                    float *Wproj_tmp = aligned_malloc(sizeof(float) * dim * dim, 64);
                    for(int i=0;i<dim*dim;i++) Wproj_tmp[i] = f16_to_float(Wproj_f16[i]);
                    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasTrans, SEQLEN, dim, dim, 1.0f, g_out_seq, dim, Wproj_tmp, dim, 0.0f, g_head_out, dim);
                    free(Wproj_tmp);
                } else {
                    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasTrans, SEQLEN, dim, dim, 1.0f, g_out_seq, dim, Wproj, dim, 0.0f, g_head_out, dim);
                }

                cblas_sgemm(CblasRowMajor, CblasTrans, CblasNoTrans, dim, dim, SEQLEN, 1.0f, &head_out[(bi*SEQLEN)*dim], dim, g_out_seq, dim, 1.0f, gWproj, dim);

                float *g_emb_seq = xmalloc(sizeof(float) * SEQLEN * dim);
                for(int t=0;t<SEQLEN;t++) for(int d=0; d<dim; d++) g_emb_seq[t*dim + d] = g_out_seq[t*dim + d];

                float *Qseq = &Qb[(bi*SEQLEN)*dim];
                float *Kseq = &Kb[(bi*SEQLEN)*dim];
                float *Vseq = &Vb[(bi*SEQLEN)*dim];
                float *Pseq = &P[(bi*H_local*SEQLEN*SEQLEN)];
                float *gP = xcalloc(H_local * SEQLEN * SEQLEN, sizeof(float));
                float *gVseq = xcalloc(SEQLEN * dim, sizeof(float));
                for(int h=0; h<H_local; h++){
                    int off = h * HD_local;
                    for(int a=0;a<SEQLEN;a++){
                        for(int b2=0;b2<SEQLEN;b2++){
                            float acc = 0.0f;
                            for(int d=0; d<HD_local; d++){
                                acc += g_head_out[a*dim + off + d] * Vseq[b2*dim + off + d];
                            }
                            gP[h*SEQLEN*SEQLEN + a*SEQLEN + b2] += acc;
                        }
                    }
                    for(int b2=0;b2<SEQLEN;b2++){
                        for(int d=0; d<HD_local; d++){
                            float acc = 0.0f;
                            for(int a=0;a<SEQLEN;a++){
                                acc += Pseq[h*SEQLEN*SEQLEN + a*SEQLEN + b2] * g_head_out[a*dim + off + d];
                            }
                            gVseq[b2*dim + off + d] += acc;
                        }
                    }
                }

                float *g_scores = xcalloc(H_local * SEQLEN * SEQLEN, sizeof(float));
                for(int h=0; h<H_local; h++){
                    for(int a=0;a<SEQLEN;a++){
                        float dot = 0.0f;
                        for(int c=0;c<SEQLEN;c++) dot += gP[h*SEQLEN*SEQLEN + a*SEQLEN + c] * Pseq[h*SEQLEN*SEQLEN + a*SEQLEN + c];
                        for(int b2=0;b2<SEQLEN;b2++){
                            g_scores[h*SEQLEN*SEQLEN + a*SEQLEN + b2] = Pseq[h*SEQLEN*SEQLEN + a*SEQLEN + b2] * (gP[h*SEQLEN*SEQLEN + a*SEQLEN + b2] - dot);
                        }
                    }
                }

                float scale = 1.0f / sqrtf((float)HD_local);
                float *gQseq = xcalloc(SEQLEN * dim, sizeof(float));
                float *gKseq = xcalloc(SEQLEN * dim, sizeof(float));
                for(int h=0; h<H_local; h++){
                    int off = h * HD_local;
                    for(int a=0;a<SEQLEN;a++){
                        for(int b2=0;b2<SEQLEN;b2++){
                            float gs = g_scores[h*SEQLEN*SEQLEN + a*SEQLEN + b2] * scale;
                            for(int d=0; d<HD_local; d++){
                                gQseq[a*dim + off + d] += gs * Kseq[b2*dim + off + d];
                                gKseq[b2*dim + off + d] += gs * Qseq[a*dim + off + d];
                            }
                        }
                    }
                }

                float *emb_seq_local = xmalloc(sizeof(float) * SEQLEN * dim);
                for(int t=0;t<SEQLEN;t++){
                    int id = tokens[ starts[w+bi] + t ];
                    if(use_emb_quant){
                        const int8_t *qrow = &embq->q[(size_t)id*dim];
                        float sc = embq->scale[id];
                        for(int d=0; d<dim; d++) emb_seq_local[t*dim + d] = ((float)qrow[d]) * sc;
                    } else {
                        const float *erow = &Wemb[(size_t)id*dim];
                        for(int d=0; d<dim; d++) emb_seq_local[t*dim + d] = erow[d];
                    }
                }

                cblas_sgemm(CblasRowMajor, CblasTrans, CblasNoTrans, dim, dim, SEQLEN, 1.0f, emb_seq_local, dim, gVseq, dim, 1.0f, gWv, dim);
                cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasTrans, SEQLEN, dim, dim, 1.0f, gVseq, dim, Wv, dim, 1.0f, g_emb_seq, dim);

                cblas_sgemm(CblasRowMajor, CblasTrans, CblasNoTrans, dim, dim, SEQLEN, 1.0f, emb_seq_local, dim, gQseq, dim, 1.0f, gWq, dim);
                cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasTrans, SEQLEN, dim, dim, 1.0f, gQseq, dim, Wq, dim, 1.0f, g_emb_seq, dim);

                cblas_sgemm(CblasRowMajor, CblasTrans, CblasNoTrans, dim, dim, SEQLEN, 1.0f, emb_seq_local, dim, gKseq, dim, 1.0f, gWk, dim);
                cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasTrans, SEQLEN, dim, dim, 1.0f, gKseq, dim, Wk, dim, 1.0f, g_emb_seq, dim);

                for(int t=0;t<SEQLEN;t++){
                    int tok = tokens[ starts[w+bi] + t ];
                    for(int d=0; d<dim; d++){
                        gWemb[(size_t)tok*dim + d] += g_emb_seq[t*dim + d];
                    }
                }

                free(gP); free(g_scores); free(gVseq);
                free(gQseq); free(gKseq);
                free(emb_seq_local);
                free(g_out_seq); free(g_head_out); free(g_emb_seq);
                free(g_ff); free(g_ff1); free(g_pooled);
            } // end per-batch-item backward

        } // end batches

        adam_update(Wemb, gWemb, st_Wemb, (int)VxD, LR, 1e-2f, tstep);
        adam_update(Wq, gWq, st_Wq, dim*dim, LR*0.1f, 1e-3f, tstep);
        adam_update(Wk, gWk, st_Wk, dim*dim, LR*0.1f, 1e-3f, tstep);
        adam_update(Wv, gWv, st_Wv, dim*dim, LR*0.1f, 1e-3f, tstep);
        adam_update(Wproj, gWproj, st_Wproj, dim*dim, LR*0.1f, 1e-3f, tstep);
        adam_update(Wff1, gWff1, st_Wff1, dim*(dim*4), LR*0.1f, 1e-3f, tstep);
        adam_update(Wff2, gWff2, st_Wff2, (dim*4)*dim, LR*0.1f, 1e-3f, tstep);

        if(use_fp16_params){
            for(int i=0;i<dim*dim;i++){ Wq_f16[i] = float_to_f16(Wq[i]); Wk_f16[i] = float_to_f16(Wk[i]); Wv_f16[i] = float_to_f16(Wv[i]); Wproj_f16[i] = float_to_f16(Wproj[i]); }
        }
        if(use_emb_quant && (tstep % 5 == 0)){
            for(int v=0; v<V; v++){
                const float *row = &Wemb[(size_t)v*dim];
                float maxabs = 0.0f;
                for(int d=0; d<dim; d++){ float a = fabsf(row[d]); if(a>maxabs) maxabs=a; }
                float scale = (maxabs > 0.0f) ? (maxabs / 127.0f) : 1e-8f;
                embq->scale[v] = scale;
                int8_t *qrow = &embq->q[(size_t)v*dim];
                for(int d=0; d<dim; d++){
                    int vq = (int)roundf(row[d] / scale);
                    if (vq > 127) vq = 127;
                    if (vq < -128) vq = -128;
                    qrow[d] = (int8_t)vq;
                }
            }
        }

        tstep++;
        double avg_loss = (steps>0) ? (epoch_loss / steps) : 0.0;
        printf("Epoch %3d  AvgLoss=%.6f\n", ep, avg_loss);
        fflush(stdout);

        if(!save_model_binary(model_out, ep, V, (int)dim, H_local, Wemb, Wq, Wk, Wv, Wproj, Wff1, Wff2)){
            fprintf(stderr,"Warning: failed to save model to %s\n", model_out);
        } else {
            printf("Saved model checkpoint to %s (epoch %d)\n", model_out, ep);
        }
    }

    free(tokens);
    vocab_free(vocab);
    free(Wemb); free(Wq); free(Wk); free(Wv); free(Wproj); free(Wff1); free(Wff2);
    if(use_fp16_params){ free(Wq_f16); free(Wk_f16); free(Wv_f16); free(Wproj_f16); }
    if(use_emb_quant){ embq_free(embq); }
    free(emb_batch); free(Qb); free(Kb); free(Vb); free(scores); free(P); free(head_out); free(out_batch);
    free(pooled); free(ff1); free(ff_out); free(logits); free(probs);
    free(gWemb); free(gWq); free(gWk); free(gWv); free(gWproj); free(gWff1); free(gWff2);
    adam_free(st_Wemb); adam_free(st_Wq); adam_free(st_Wk); adam_free(st_Wv); adam_free(st_Wproj);
    adam_free(st_Wff1); adam_free(st_Wff2);
    free(starts);

    return 0;
}
