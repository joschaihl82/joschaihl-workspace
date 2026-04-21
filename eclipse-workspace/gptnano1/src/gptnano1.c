// mini_vocab_train.c
// Single-file minimal trainer that builds vocabulary from input .txt (whitespace tokens).
// Uses OpenBLAS for matrix ops.
// Build: gcc -O2 -std=c11 -o mini_vocab_train mini_vocab_train.c -lopenblas -lm
// Usage: ./mini_vocab_train data.txt

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <cblas.h>

#define D 64        // model dimension (must be divisible by H)
#define H 4         // heads
#define HD (D/H)
#define SEQLEN 8
#define EPOCHS 80
#define LR 0.003
#define EPS 1e-8

// ---------- utilities ----------
static void *xmalloc(size_t n){ void *p = malloc(n); if(!p){ fprintf(stderr,"alloc fail\n"); exit(1);} return p; }
static void *xcalloc(size_t a, size_t b){ void *p = calloc(a,b); if(!p){ fprintf(stderr,"calloc fail\n"); exit(1);} return p; }
static double rnd(){ return (rand()+1.0)/((double)RAND_MAX+2.0); }

// ---------- simple whitespace tokenizer + vocabulary (linear search) ----------
typedef struct {
    char **tokens;    // unique token strings
    int *counts;
    int size;
    int capacity;
} Vocab;

static Vocab *vocab_new(int cap){
    Vocab *v = xmalloc(sizeof(Vocab));
    v->tokens = xmalloc(sizeof(char*) * cap);
    v->counts = xcalloc(cap, sizeof(int));
    v->size = 0; v->capacity = cap;
    return v;
}
static void vocab_free(Vocab *v){
    for(int i=0;i<v->size;i++) free(v->tokens[i]);
    free(v->tokens); free(v->counts); free(v);
}
static int vocab_find(Vocab *v, const char *s){
    for(int i=0;i<v->size;i++) if(strcmp(v->tokens[i], s)==0) return i;
    return -1;
}
static int vocab_add(Vocab *v, const char *s){
    int idx = vocab_find(v,s);
    if(idx>=0){ v->counts[idx]++; return idx; }
    if(v->size >= v->capacity){
        int nc = v->capacity * 2;
        v->tokens = realloc(v->tokens, sizeof(char*) * nc);
        v->counts = realloc(v->counts, sizeof(int) * nc);
        v->capacity = nc;
    }
    v->tokens[v->size] = strdup(s);
    v->counts[v->size] = 1;
    v->size++;
    return v->size - 1;
}

// read file, split on whitespace, build token id array and vocabulary
static int load_and_build_vocab(const char *fn, int **out_tokens, Vocab **out_vocab){
    FILE *f = fopen(fn,"rb"); if(!f) return 0;
    fseek(f,0,SEEK_END); long n = ftell(f); rewind(f);
    char *buf = xmalloc(n+1);
    size_t r = fread(buf,1,n,f); fclose(f);
    buf[r]=0;
    Vocab *v = vocab_new(1024);
    // simple split on whitespace
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
static void softmax_row(double *x, int len){
    double m = x[0];
    for(int i=1;i<len;i++) if(x[i]>m) m=x[i];
    double s=0.0;
    for(int i=0;i<len;i++){ x[i] = exp(x[i]-m); s += x[i]; }
    for(int i=0;i<len;i++) x[i] /= s;
}
static double cross_entropy_and_probs(const double *logits_in, double *probs_out, int len, int target){
    double m = logits_in[0];
    for(int i=1;i<len;i++) if(logits_in[i]>m) m=logits_in[i];
    double s=0.0;
    for(int i=0;i<len;i++){ probs_out[i] = exp(logits_in[i]-m); s += probs_out[i]; }
    for(int i=0;i<len;i++) probs_out[i] /= s;
    double loss = -log(probs_out[target] + 1e-12);
    return loss;
}
static double gelu(double x){ return 0.5*x*(1.0 + tanh(0.7978845608*(x + 0.044715*x*x*x))); }
static double dgelu(double x){
    double t = tanh(0.7978845608*(x + 0.044715*x*x*x));
    double dt = 0.7978845608*(1.0 + 0.134145*(x*x));
    return 0.5*(1.0 + t) + 0.5*x*(1.0 - t*t)*dt;
}

// ---------- main: model with dynamic vocab size ----------
int main(int argc, char **argv){
    if(argc<2){ fprintf(stderr,"Usage: %s data.txt\n", argv[0]); return 1; }
    srand((unsigned)time(NULL));

    int *tokens = NULL;
    Vocab *vocab = NULL;
    int N = load_and_build_vocab(argv[1], &tokens, &vocab);
    if(!N){ fprintf(stderr,"Failed to load tokens\n"); return 1; }
    int Vsize = vocab->size;
    printf("Loaded %d tokens, vocab size %d\n", N, Vsize);

    // model params sized by Vsize
    double *Wemb = xmalloc(sizeof(double) * Vsize * D);    // V x D
    double *Wq   = xmalloc(sizeof(double) * D * D);
    double *Wk   = xmalloc(sizeof(double) * D * D);
    double *Wv   = xmalloc(sizeof(double) * D * D);
    double *Wproj= xmalloc(sizeof(double) * D * D);
    double *Wff1 = xmalloc(sizeof(double) * D * (D*4));
    double *Wff2 = xmalloc(sizeof(double) * (D*4) * D);
    double *Wout = xmalloc(sizeof(double) * D * Vsize);

    // simple SGD (no Adam here to keep code compact) with small lr
    double lr = LR;

    // init
    for(int i=0;i<Vsize*D;i++) Wemb[i] = (rnd()-0.5)*0.1;
    for(int i=0;i<D*D;i++) Wq[i] = Wk[i] = Wv[i] = Wproj[i] = (rnd()-0.5)*0.1;
    for(int i=0;i<D*(D*4);i++) Wff1[i] = (rnd()-0.5)*0.1;
    for(int i=0;i<(D*4)*D;i++) Wff2[i] = (rnd()-0.5)*0.1;
    for(int i=0;i<D*Vsize;i++) Wout[i] = (rnd()-0.5)*0.1;

    // buffers
    double *emb_seq = xmalloc(sizeof(double) * SEQLEN * D);
    double *Q = xmalloc(sizeof(double) * SEQLEN * D);
    double *K = xmalloc(sizeof(double) * SEQLEN * D);
    double *Vv = xmalloc(sizeof(double) * SEQLEN * D);
    double *scores = xmalloc(sizeof(double) * H * SEQLEN * SEQLEN);
    double *P = xmalloc(sizeof(double) * H * SEQLEN * SEQLEN);
    double *head_out = xmalloc(sizeof(double) * SEQLEN * D);
    double *out_seq = xmalloc(sizeof(double) * SEQLEN * D);
    double *pooled = xmalloc(sizeof(double) * D);
    double *ff1 = xmalloc(sizeof(double) * (D*4));
    double *ff_out = xmalloc(sizeof(double) * D);
    double *logits = xmalloc(sizeof(double) * Vsize);
    double *probs = xmalloc(sizeof(double) * Vsize);

    // gradient accumulators (simple SGD)
    double *gWemb = xcalloc(Vsize*D, sizeof(double));
    double *gWq = xcalloc(D*D, sizeof(double));
    double *gWk = xcalloc(D*D, sizeof(double));
    double *gWv = xcalloc(D*D, sizeof(double));
    double *gWproj = xcalloc(D*D, sizeof(double));
    double *gWff1 = xcalloc(D*(D*4), sizeof(double));
    double *gWff2 = xcalloc((D*4)*D, sizeof(double));
    double *gWout = xcalloc(D*Vsize, sizeof(double));

    for(int ep=1; ep<=EPOCHS; ep++){
        // zero grads
        memset(gWemb,0,sizeof(double)*Vsize*D);
        memset(gWq,0,sizeof(double)*D*D);
        memset(gWk,0,sizeof(double)*D*D);
        memset(gWv,0,sizeof(double)*D*D);
        memset(gWproj,0,sizeof(double)*D*D);
        memset(gWff1,0,sizeof(double)*D*(D*4));
        memset(gWff2,0,sizeof(double)*(D*4)*D);
        memset(gWout,0,sizeof(double)*D*Vsize);

        double epoch_loss = 0.0;
        int steps = 0;

        for(int i=0; i+SEQLEN < N-1; i += SEQLEN){
            steps++;
            // build emb_seq
            for(int t=0;t<SEQLEN;t++){
                int id = tokens[i+t];
                for(int d=0; d<D; d++) emb_seq[t*D + d] = Wemb[id*D + d];
            }
            // Q,K,V projections
            cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, SEQLEN, D, D, 1.0, emb_seq, D, Wq, D, 0.0, Q, D);
            cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, SEQLEN, D, D, 1.0, emb_seq, D, Wk, D, 0.0, K, D);
            cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, SEQLEN, D, D, 1.0, emb_seq, D, Wv, D, 0.0, Vv, D);

            // attention per head
            for(int h=0; h<H; h++){
                int off = h*HD;
                for(int a=0;a<SEQLEN;a++){
                    for(int b=0;b<SEQLEN;b++){
                        double s=0.0;
                        for(int d=0; d<HD; d++) s += Q[a*D + off + d] * K[b*D + off + d];
                        scores[h*SEQLEN*SEQLEN + a*SEQLEN + b] = s / sqrt((double)HD);
                    }
                }
                // softmax per row
                for(int a=0;a<SEQLEN;a++){
                    softmax_row(&scores[h*SEQLEN*SEQLEN + a*SEQLEN], SEQLEN);
                }
                // copy to P
                for(int idx=0; idx<SEQLEN*SEQLEN; idx++) P[h*SEQLEN*SEQLEN + idx] = scores[h*SEQLEN*SEQLEN + idx];
            }

            // head_out = P * Vv
            for(int t=0;t<SEQLEN*D;t++) head_out[t]=0.0;
            for(int h=0; h<H; h++){
                int off = h*HD;
                for(int a=0;a<SEQLEN;a++){
                    for(int d=0; d<HD; d++){
                        double val=0.0;
                        for(int b=0;b<SEQLEN;b++){
                            val += P[h*SEQLEN*SEQLEN + a*SEQLEN + b] * Vv[b*D + off + d];
                        }
                        head_out[a*D + off + d] = val;
                    }
                }
            }

            // out_seq = head_out * Wproj
            cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, SEQLEN, D, D, 1.0, head_out, D, Wproj, D, 0.0, out_seq, D);
            // residual
            for(int t=0;t<SEQLEN;t++) for(int d=0; d<D; d++) out_seq[t*D + d] += emb_seq[t*D + d];

            // pool mean
            for(int d=0; d<D; d++){ pooled[d]=0.0; for(int t=0;t<SEQLEN;t++) pooled[d] += out_seq[t*D + d]; pooled[d] /= SEQLEN; }

            // FFN
            cblas_dgemv(CblasRowMajor, CblasNoTrans, D, D*4, 1.0, Wff1, D*4, pooled, 1, 0.0, ff1, 1);
            for(int j=0;j<D*4;j++) ff1[j] = gelu(ff1[j]);
            cblas_dgemv(CblasRowMajor, CblasNoTrans, D*4, D, 1.0, Wff2, D, ff1, 1, 0.0, ff_out, 1);
            for(int d=0; d<D; d++) ff_out[d] += pooled[d];

            // logits and loss
            cblas_dgemv(CblasRowMajor, CblasNoTrans, D, Vsize, 1.0, Wout, Vsize, ff_out, 1, 0.0, logits, 1);
            int target = tokens[i+SEQLEN];
            double loss = cross_entropy_and_probs(logits, probs, Vsize, target);
            epoch_loss += loss;

            // --- very simplified backward: only update Wout and Wemb via gradient from logits
            // grad logits = probs - onehot
            for(int v=0; v<Vsize; v++) probs[v] = probs[v] - (v==target ? 1.0 : 0.0);
            // grad Wout += ff_out^T * grad_logits
            for(int d=0; d<D; d++){
                for(int v=0; v<Vsize; v++){
                    gWout[d*Vsize + v] += ff_out[d] * probs[v];
                }
            }
            // grad ff_out = Wout * grad_logits
            double *g_ff = xmalloc(sizeof(double)*D);
            for(int d=0; d<D; d++){ g_ff[d]=0.0; for(int v=0; v<Vsize; v++) g_ff[d] += Wout[d*Vsize + v] * probs[v]; }
            // backprop to pooled -> distribute to out_seq mean -> to emb_seq -> to Wemb
            for(int t=0;t<SEQLEN;t++){
                for(int d=0; d<D; d++){
                    double g = g_ff[d] / SEQLEN;
                    int tok = tokens[i+t];
                    gWemb[tok*D + d] += g; // very simplified: attribute all gradient to embedding directly
                }
            }
            free(g_ff);
        } // end sequence loop

        // SGD parameter updates (simple)
        for(int i=0;i<D*Vsize;i++) Wout[i] -= lr * gWout[i];
        for(int i=0;i<Vsize*D;i++) Wemb[i] -= lr * gWemb[i];
        // small updates for projection/ffn to keep model changing (optional small step)
        for(int i=0;i<D*D;i++){ Wq[i] -= lr*1e-4 * gWq[i]; Wk[i] -= lr*1e-4 * gWk[i]; Wv[i] -= lr*1e-4 * gWv[i]; Wproj[i] -= lr*1e-4 * gWproj[i]; }
        for(int i=0;i<D*(D*4);i++) Wff1[i] -= lr*1e-4 * gWff1[i];
        for(int i=0;i<(D*4)*D;i++) Wff2[i] -= lr*1e-4 * gWff2[i];

        double avg_loss = (steps>0) ? (epoch_loss / steps) : 0.0;

        // test prediction on first SEQLEN tokens
        if(N >= SEQLEN){
            for(int t=0;t<SEQLEN;t++){
                int id = tokens[t];
                for(int d=0; d<D; d++) emb_seq[t*D + d] = Wemb[id*D + d];
            }
            cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, SEQLEN, D, D, 1.0, emb_seq, D, Wq, D, 0.0, Q, D);
            cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, SEQLEN, D, D, 1.0, emb_seq, D, Wk, D, 0.0, K, D);
            cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, SEQLEN, D, D, 1.0, emb_seq, D, Wv, D, 0.0, Vv, D);
            for(int h=0; h<H; h++){
                int off = h*HD;
                for(int a=0;a<SEQLEN;a++){
                    for(int b=0;b<SEQLEN;b++){
                        double s=0.0;
                        for(int d=0; d<HD; d++) s += Q[a*D + off + d] * K[b*D + off + d];
                        scores[h*SEQLEN*SEQLEN + a*SEQLEN + b] = s / sqrt((double)HD);
                    }
                }
                for(int a=0;a<SEQLEN;a++) softmax_row(&scores[h*SEQLEN*SEQLEN + a*SEQLEN], SEQLEN);
                for(int a=0;a<SEQLEN;a++){
                    for(int d=0; d<HD; d++){
                        double val=0.0;
                        for(int b=0;b<SEQLEN;b++) val += scores[h*SEQLEN*SEQLEN + a*SEQLEN + b] * Vv[b*D + off + d];
                        head_out[a*D + off + d] = val;
                    }
                }
            }
            cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, SEQLEN, D, D, 1.0, head_out, D, Wproj, D, 0.0, out_seq, D);
            for(int t=0;t<SEQLEN;t++) for(int d=0; d<D; d++) out_seq[t*D + d] += emb_seq[t*D + d];
            for(int d=0; d<D; d++){ pooled[d]=0.0; for(int t=0;t<SEQLEN;t++) pooled[d]+=out_seq[t*D + d]; pooled[d]/=SEQLEN; }
            cblas_dgemv(CblasRowMajor, CblasNoTrans, D, D*4, 1.0, Wff1, D*4, pooled, 1, 0.0, ff1, 1);
            for(int j=0;j<D*4;j++) ff1[j] = gelu(ff1[j]);
            cblas_dgemv(CblasRowMajor, CblasNoTrans, D*4, D, 1.0, Wff2, D, ff1, 1, 0.0, ff_out, 1);
            for(int d=0; d<D; d++) ff_out[d] += pooled[d];
            cblas_dgemv(CblasRowMajor, CblasNoTrans, D, Vsize, 1.0, Wout, Vsize, ff_out, 1, 0.0, logits, 1);
            for(int v=0; v<Vsize; v++) probs[v] = logits[v];
            softmax_row(probs, Vsize);
            int pred = 0; for(int v=1; v<Vsize; v++) if(probs[v] > probs[pred]) pred = v;

            // build query string
            char query[1024]; query[0]=0;
            for(int t=0;t<SEQLEN;t++){
                int id = tokens[t];
                strcat(query, vocab->tokens[id]);
                if(t<SEQLEN-1) strcat(query, " ");
            }
            printf("Epoch %3d  AvgLoss=%.6f  PredNext=%d (%s)  |  Query: \"%s\"\n",
                   ep, avg_loss, pred, vocab->tokens[pred], query);
        } else {
            printf("Epoch %3d  AvgLoss=%.6f  (not enough tokens for test)\n", ep, avg_loss);
        }
        fflush(stdout);
    }

    // cleanup
    free(tokens);
    vocab_free(vocab);
    free(Wemb); free(Wq); free(Wk); free(Wv); free(Wproj); free(Wff1); free(Wff2); free(Wout);
    free(emb_seq); free(Q); free(K); free(Vv); free(scores); free(P); free(head_out); free(out_seq);
    free(pooled); free(ff1); free(ff_out); free(logits); free(probs);
    free(gWemb); free(gWq); free(gWk); free(gWv); free(gWproj); free(gWff1); free(gWff2); free(gWout);

    return 0;
}
