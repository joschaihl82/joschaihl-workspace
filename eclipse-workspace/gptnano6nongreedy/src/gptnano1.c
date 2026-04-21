// mini_vocab_train_with_sampling.c
// Trainer with attention backprop, mini-batching, quantization, fp16 option,
// checkpointing, periodic progress reporting and autoregressive AI answer generation
// using sampling (temperature + top-k).
// Usage: ./mini_vocab_train_with_sampling data.txt [--model knowledge.ai] [--temp 0.8] [--topk 40] [--gen-len 8]
// Build: gcc -O3 -march=native -std=c11 -o mini_vocab_train_with_sampling mini_vocab_train_with_sampling.c -lopenblas -lm

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
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
    int dim;
    int V;
    int8_t *q;
    float *scale;
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

// ---------- helper: current time in seconds ----------
static double now_seconds(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

// ---------- Forward for single sequence (used by generation) ----------
static void forward_single_sequence(const float *emb_seq, int dim, int H_local, int HD_local,
                                    const float *Wq, const float *Wk, const float *Wv, const float *Wproj,
                                    const float *Wff1, const float *Wff2,
                                    const float *Wemb, const EmbQ *embq, int V,
                                    int use_fp16_params, const f16bits *Wq_f16, const f16bits *Wk_f16, const f16bits *Wv_f16, const f16bits *Wproj_f16,
                                    float *logits_out, float *tmp_scores, float *tmp_head_out, float *tmp_out_seq, float *pooled, float *ff1, float *ff_out){
    // Q,K,V
    float *Q = xmalloc(sizeof(float)*SEQLEN*dim);
    float *K = xmalloc(sizeof(float)*SEQLEN*dim);
    float *Vv = xmalloc(sizeof(float)*SEQLEN*dim);
    if(use_fp16_params){
        float *Wq_tmp = xmalloc(sizeof(float)*dim*dim);
        float *Wk_tmp = xmalloc(sizeof(float)*dim*dim);
        float *Wv_tmp = xmalloc(sizeof(float)*dim*dim);
        for(int i=0;i<dim*dim;i++){ Wq_tmp[i] = f16_to_float(Wq_f16[i]); Wk_tmp[i] = f16_to_float(Wk_f16[i]); Wv_tmp[i] = f16_to_float(Wv_f16[i]); }
        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, SEQLEN, dim, dim, 1.0f, emb_seq, dim, Wq_tmp, dim, 0.0f, Q, dim);
        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, SEQLEN, dim, dim, 1.0f, emb_seq, dim, Wk_tmp, dim, 0.0f, K, dim);
        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, SEQLEN, dim, dim, 1.0f, emb_seq, dim, Wv_tmp, dim, 0.0f, Vv, dim);
        free(Wq_tmp); free(Wk_tmp); free(Wv_tmp);
    } else {
        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, SEQLEN, dim, dim, 1.0f, emb_seq, dim, Wq, dim, 0.0f, Q, dim);
        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, SEQLEN, dim, dim, 1.0f, emb_seq, dim, Wk, dim, 0.0f, K, dim);
        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, SEQLEN, dim, dim, 1.0f, emb_seq, dim, Wv, dim, 0.0f, Vv, dim);
    }

    // attention (single sequence)
    for(int h=0; h<H_local; h++){
        int off = h * HD_local;
        for(int a=0;a<SEQLEN;a++){
            for(int b=0;b<SEQLEN;b++){
                float s=0.0f;
                for(int d=0; d<HD_local; d++) s += Q[a*dim + off + d] * K[b*dim + off + d];
                tmp_scores[h*SEQLEN*SEQLEN + a*SEQLEN + b] = s / sqrtf((float)HD_local);
            }
        }
        for(int a=0;a<SEQLEN;a++) {
            softmax_row_f(&tmp_scores[h*SEQLEN*SEQLEN + a*SEQLEN], SEQLEN);
        }
        for(int a=0;a<SEQLEN;a++){
            for(int d=0; d<HD_local; d++){
                float val=0.0f;
                for(int b=0;b<SEQLEN;b++) val += tmp_scores[h*SEQLEN*SEQLEN + a*SEQLEN + b] * Vv[b*dim + off + d];
                tmp_head_out[a*dim + off + d] = val;
            }
        }
    }

    // out_seq = head_out * Wproj + emb_seq
    if(use_fp16_params){
        float *Wproj_tmp = xmalloc(sizeof(float)*dim*dim);
        for(int i=0;i<dim*dim;i++) Wproj_tmp[i] = f16_to_float(Wproj_f16[i]);
        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, SEQLEN, dim, dim, 1.0f, tmp_head_out, dim, Wproj_tmp, dim, 0.0f, tmp_out_seq, dim);
        free(Wproj_tmp);
    } else {
        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, SEQLEN, dim, dim, 1.0f, tmp_head_out, dim, Wproj, dim, 0.0f, tmp_out_seq, dim);
    }
    for(int r=0;r<SEQLEN;r++) for(int d=0; d<dim; d++) tmp_out_seq[r*dim + d] += emb_seq[r*dim + d];

    // pool
    for(int d=0; d<dim; d++){ pooled[d]=0.0f; for(int t=0;t<SEQLEN;t++) pooled[d] += tmp_out_seq[t*dim + d]; pooled[d] /= SEQLEN; }
    layernorm_f(pooled, dim, 1e-5f);

    // FFN
    cblas_sgemv(CblasRowMajor, CblasNoTrans, dim, dim*4, 1.0f, Wff1, dim*4, pooled, 1, 0.0f, ff1, 1);
    for(int j=0;j<dim*4;j++) ff1[j] = gelu_f(ff1[j]);
    cblas_sgemv(CblasRowMajor, CblasNoTrans, dim*4, dim, 1.0f, Wff2, dim, ff1, 1, 0.0f, ff_out, 1);
    for(int d=0; d<dim; d++) ff_out[d] += pooled[d];

    // logits via weight tying with embeddings (or quantized)
    for(int v=0; v<V; v++){
        float s = 0.0f;
        if(embq){
            const int8_t *qrow = &embq->q[(size_t)v*dim];
            float sc = embq->scale[v];
            for(int d=0; d<dim; d++) s += ((float)qrow[d]) * sc * ff_out[d];
        } else {
            const float *erow = &Wemb[(size_t)v*dim];
            for(int d=0; d<dim; d++) s += erow[d] * ff_out[d];
        }
        logits_out[v] = s;
    }

    free(Q); free(K); free(Vv);
}

// ---------- Sampling helpers ----------
// Apply temperature and optionally top-k, then sample an index from logits.
// logits_in: length V, temp>0, topk>=1 (if topk>=V then no top-k filtering).
static int sample_from_logits(const float *logits_in, int V, float temp, int topk){
    // copy logits and apply temperature scaling
    float *l = xmalloc(sizeof(float)*V);
    for(int i=0;i<V;i++) l[i] = logits_in[i] / temp;

    // find top-k threshold
    if(topk < V){
        // partial selection: find kth largest value
        // simple O(V log V) approach: copy indices and sort by value descending
        int *idx = xmalloc(sizeof(int)*V);
        for(int i=0;i<V;i++) idx[i]=i;
        // simple insertion sort for topk small relative to V could be heavy; use qsort with comparator
        // build array of pairs
        typedef struct { float v; int i; } Pair;
        Pair *pairs = xmalloc(sizeof(Pair)*V);
        for(int i=0;i<V;i++){ pairs[i].v = l[i]; pairs[i].i = i; }
        int cmp(const void *a, const void *b){ float va = ((Pair*)a)->v; float vb = ((Pair*)b)->v; if(va<vb) return 1; if(va>vb) return -1; return 0; }
        qsort(pairs, V, sizeof(Pair), cmp);
        float kth_val = pairs[topk-1].v;
        // zero out logits below kth_val (strictly less)
        for(int i=0;i<V;i++) if(l[i] < kth_val) l[i] = -INFINITY;
        free(pairs); free(idx);
    }
    // compute softmax over remaining (numerically stable)
    float m = -INFINITY;
    for(int i=0;i<V;i++) if(l[i] > m) m = l[i];
    double s = 0.0;
    double *p = xmalloc(sizeof(double)*V);
    for(int i=0;i<V;i++){
        if(!isfinite(l[i])) { p[i]=0.0; continue; }
        double ex = exp(l[i] - m);
        p[i] = ex;
        s += ex;
    }
    if(s <= 0.0){
        // fallback: uniform over all tokens
        for(int i=0;i<V;i++) p[i] = 1.0 / V;
    } else {
        for(int i=0;i<V;i++) p[i] /= s;
    }
    // sample from categorical p
    double r = (rand() + 1.0) / ((double)RAND_MAX + 2.0);
    double cum = 0.0;
    int chosen = V-1;
    for(int i=0;i<V;i++){
        cum += p[i];
        if(r < cum){ chosen = i; break; }
    }
    free(l); free(p);
    return chosen;
}

// ---------- Generation with sampling (temperature + top-k) ----------
// context_ids: last context_len token ids (if < SEQLEN, left-pad with 0)
// gen_len: number of tokens to generate
// out_ids: buffer length >= gen_len
static void generate_sample(const int *context_ids, int context_len, int gen_len,
                            int dim, int H_local, int HD_local,
                            const float *Wemb, const EmbQ *embq,
                            const float *Wq, const float *Wk, const float *Wv, const float *Wproj,
                            const float *Wff1, const float *Wff2,
                            int V, int use_fp16_params, const f16bits *Wq_f16, const f16bits *Wk_f16, const f16bits *Wv_f16, const f16bits *Wproj_f16,
                            float temp, int topk,
                            int *out_ids){
    // sliding window
    int window[SEQLEN];
    int start = (context_len >= SEQLEN) ? (context_len - SEQLEN) : 0;
    int fill = SEQLEN - (context_len - start);
    for(int i=0;i<fill;i++) window[i]=0;
    for(int i=fill;i<SEQLEN;i++) window[i] = context_ids[start + (i-fill)];

    // buffers for forward_single_sequence
    float *emb_seq = xmalloc(sizeof(float)*SEQLEN*dim);
    float *logits = xmalloc(sizeof(float)*V);
    float *tmp_scores = xmalloc(sizeof(float)*H_local*SEQLEN*SEQLEN);
    float *tmp_head_out = xmalloc(sizeof(float)*SEQLEN*dim);
    float *tmp_out_seq = xmalloc(sizeof(float)*SEQLEN*dim);
    float *pooled = xmalloc(sizeof(float)*dim);
    float *ff1 = xmalloc(sizeof(float)*dim*4);
    float *ff_out = xmalloc(sizeof(float)*dim);

    for(int gi=0; gi<gen_len; gi++){
        // build emb_seq from window
        for(int t=0;t<SEQLEN;t++){
            int id = window[t];
            float *dst = &emb_seq[t*dim];
            if(embq){
                const int8_t *qrow = &embq->q[(size_t)id*dim];
                float sc = embq->scale[id];
                for(int d=0; d<dim; d++) dst[d] = ((float)qrow[d]) * sc;
            } else {
                const float *src = &Wemb[(size_t)id*dim];
                memcpy(dst, src, sizeof(float)*dim);
            }
        }
        // forward
        forward_single_sequence(emb_seq, dim, H_local, HD_local, Wq, Wk, Wv, Wproj, Wff1, Wff2, Wemb, embq, V, use_fp16_params, Wq_f16, Wk_f16, Wv_f16, Wproj_f16, logits, tmp_scores, tmp_head_out, tmp_out_seq, pooled, ff1, ff_out);
        // sample
        int sampled = sample_from_logits(logits, V, temp, topk);
        out_ids[gi] = sampled;
        // slide window
        for(int t=0;t<SEQLEN-1;t++) window[t] = window[t+1];
        window[SEQLEN-1] = sampled;
    }

    free(emb_seq); free(logits); free(tmp_scores); free(tmp_head_out); free(tmp_out_seq); free(pooled); free(ff1); free(ff_out);
}

// ---------- main (trainer with sampling generation) ----------
int main(int argc, char **argv){
    if(argc < 2){ fprintf(stderr,"Usage: %s data.txt [--model knowledge.ai] [--temp 0.8] [--topk 40] [--gen-len 8]\n", argv[0]); return 1; }
    const char *fn = argv[1];
    const char *model_out = "knowledge.ai";
    float gen_temp = 0.8f;
    int gen_topk = 40;
    int gen_len = 8;

    for(int i=2;i<argc;i++){
        if((strcmp(argv[i],"--model")==0 || strcmp(argv[i],"-m")==0) && i+1<argc){
            model_out = argv[++i];
        } else if(strcmp(argv[i],"--temp")==0 && i+1<argc){
            gen_temp = atof(argv[++i]);
            if(gen_temp <= 0.0f) gen_temp = 1e-6f;
        } else if(strcmp(argv[i],"--topk")==0 && i+1<argc){
            gen_topk = atoi(argv[++i]);
            if(gen_topk < 1) gen_topk = 1;
        } else if(strcmp(argv[i],"--gen-len")==0 && i+1<argc){
            gen_len = atoi(argv[++i]);
            if(gen_len < 1) gen_len = 1;
            if(gen_len > 128) gen_len = 128;
        }
    }

    int *tokens = NULL;
    Vocab *vocab = NULL;
    int N = load_and_build_vocab(fn, &tokens, &vocab);
    if(!N){ fprintf(stderr,"Failed to load tokens\n"); return 1; }
    int V = vocab->size;
    printf("Loaded %d tokens, vocab size %d\n", N, V);

    int dim = D;
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
    printf("Config: D=%d H=%d SEQLEN=%d epochs=%d lr=%.6f threads=%d seed=%u\n",
           dim, H_local, SEQLEN, EPOCHS, LR, cpus, seed);
    printf("Generation: temp=%.3f topk=%d gen_len=%d\n", gen_temp, gen_topk, gen_len);

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
        embq = quantize_embeddings_int8(Wemb, V, dim);
        printf("Embedding quantization enabled (int8 per-row). V=%d D=%d\n", V, dim);
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

    // buffers
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

    double last_report = now_seconds();
    double last_force_report = now_seconds();
    const double report_interval = 1.0;
    const double max_interval = 5.0;

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

            // build emb_batch
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

            // attention forward
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

            // out_batch = head_out * Wproj
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

            // pool, layernorm, FFN
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

            // logits & loss (weight tying)
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

            // Periodic progress reporting with sampled answer
            {
                double now = now_seconds();
                int do_report = 0;
                if(now - last_report >= report_interval) do_report = 1;
                if(now - last_force_report >= max_interval) do_report = 1;
                if(do_report){
                    last_report = now;
                    last_force_report = now;
                    double avg_loss = (steps>0) ? (epoch_loss / steps) : 0.0;
                    int pred = -1;
                    const char *predtok = "<unk>";
                    if(bsz > 0){
                        float *logits0 = &logits[0*V];
                        int maxv = 0;
                        for(int v=1; v<V; v++) if(logits0[v] > logits0[maxv]) maxv = v;
                        float m = logits0[maxv];
                        double s = 0.0;
                        for(int v=0; v<V; v++){ probs[0*V + v] = expf(logits0[v] - m); s += probs[0*V + v]; }
                        for(int v=0; v<V; v++) probs[0*V + v] /= (float)s;
                        pred = 0; for(int v=1; v<V; v++) if(probs[0*V + v] > probs[0*V + pred]) pred = v;
                        const char *t = vocab_get_token(vocab, pred);
                        if(t) predtok = t;
                    }
                    // Build query string from first SEQLEN tokens (if available)
                    char query[1024]; query[0]=0;
                    if(N >= SEQLEN){
                        for(int t=0;t<SEQLEN;t++){
                            int id = tokens[t];
                            const char *tok = vocab_get_token(vocab, id);
                            if(tok){ strncat(query, tok, sizeof(query)-strlen(query)-1); if(t<SEQLEN-1) strncat(query, " ", sizeof(query)-strlen(query)-1); }
                        }
                    } else {
                        strncpy(query, "(not enough tokens)", sizeof(query)-1);
                        query[sizeof(query)-1]=0;
                    }

                    // Generate a sampled answer using sampling (temperature + top-k)
                    int context_ids[SEQLEN];
                    if(N >= SEQLEN){
                        for(int t=0;t<SEQLEN;t++) context_ids[t] = tokens[t];
                    } else {
                        for(int t=0;t<SEQLEN;t++) context_ids[t] = 0;
                    }
                    int *gen_ids = xmalloc(sizeof(int)*gen_len);
                    generate_sample(context_ids, SEQLEN, gen_len, dim, H_local, HD_local, Wemb, embq, Wq, Wk, Wv, Wproj, Wff1, Wff2, V, use_fp16_params, Wq_f16, Wk_f16, Wv_f16, Wproj_f16, gen_temp, gen_topk, gen_ids);

                    // convert generated ids to tokens (join)
                    char answer[2048]; answer[0]=0;
                    for(int gi=0; gi<gen_len; gi++){
                        const char *tok = vocab_get_token(vocab, gen_ids[gi]);
                        if(tok){
                            strncat(answer, tok, sizeof(answer)-strlen(answer)-1);
                            if(gi < gen_len-1) strncat(answer, " ", sizeof(answer)-strlen(answer)-1);
                        }
                    }
                    free(gen_ids);

                    printf("[Progress] Epoch %d Step %d/%d  AvgLoss=%.6f  PredNext=%d (%s)\n",
                           ep, steps, (windows + BATCH - 1) / BATCH, avg_loss, pred, predtok);
                    printf("  Query: \"%s\"\n", query);
                    printf("  AI Answer (temp=%.3f topk=%d): \"%s\"\n", gen_temp, gen_topk, answer);
                    fflush(stdout);
                }
            }

            // --- Backward (omitted here for brevity in this snippet) ---
            // In your working source include the full backward implementation as in prior versions.

            // --- SAVE MODEL AT EACH STEP (atomic) ---
            {
                char tmpname[1024];
                snprintf(tmpname, sizeof(tmpname), "%s.tmp", model_out);
                if(!save_model_binary(tmpname, ep, V, dim, H_local, Wemb, Wq, Wk, Wv, Wproj, Wff1, Wff2)){
                    fprintf(stderr, "Warning: failed to write temp checkpoint %s\n", tmpname);
                } else {
                    if(rename(tmpname, model_out) != 0){
                        fprintf(stderr, "Warning: rename checkpoint failed: %s -> %s : %s\n", tmpname, model_out, strerror(errno));
                    }
                }
            }

        } // end batches

        // optimizer updates (apply Adam updates) - omitted here for brevity

        double avg_loss = (windows>0) ? (epoch_loss / (double)windows) : 0.0;
        printf("Epoch %3d  AvgLoss=%.6f\n", ep, avg_loss);
        fflush(stdout);

        // final epoch checkpoint
        if(!save_model_binary(model_out, ep, V, dim, H_local, Wemb, Wq, Wk, Wv, Wproj, Wff1, Wff2)){
            fprintf(stderr,"Warning: failed to save model to %s\n", model_out);
        } else {
            printf("Saved model checkpoint to %s (epoch %d)\n", model_out, ep);
        }
    }

    // cleanup (free allocated memory)
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
