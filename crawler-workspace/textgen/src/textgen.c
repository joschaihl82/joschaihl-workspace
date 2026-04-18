/*
 texttrain_dict.c
 Minimaler byte-level Trainer mit Wörter-Dictionary aus input.txt.
 - Liest input.txt (argv[1])
 - Baut Dictionary aus allen Wörtern (a-z,A-Z) im Input
 - Trainiert einfache Embedding->Head Next-token-Model (sehr klein, schnell)
 - Belohnt (skaliert Lernrate) wenn generierte Fortsetzungen Wörter aus Dictionary enthalten
 - Speichert model.bin und gibt Samples aus
 Build:
   gcc -O3 -std=c11 -march=native -o texttrain_dict texttrain_dict.c -lm
 Usage:
   ./texttrain_dict input.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

static double now_s(){ return (double)clock()/CLOCKS_PER_SEC; }
static float frand(){ return (float)rand()/(RAND_MAX+1.0f); }
static void *xmalloc(size_t n){ void *p = malloc(n); if(!p){fprintf(stderr,"oom\n"); exit(1);} return p; }
static void *xcalloc(size_t n,size_t s){ void *p = calloc(n,s); if(!p){fprintf(stderr,"oom\n"); exit(1);} return p; }

/* ---------------- read file ---------------- */
static uint8_t *read_all(const char *fn,int *lenp){
    FILE *f = fopen(fn,"rb"); if(!f){ *lenp=0; return NULL; }
    fseek(f,0,SEEK_END); long L = ftell(f); rewind(f);
    if(L<=0){ fclose(f); *lenp=0; return NULL; }
    uint8_t *buf = xmalloc(L);
    if(fread(buf,1,L,f)!=(size_t)L){ free(buf); fclose(f); *lenp=0; return NULL; }
    fclose(f); *lenp=(int)L; return buf;
}

/* ---------------- dictionary from input ----------------
   Extract lowercase words (a-z) and store unique set.
*/
typedef struct {
    char **words;
    int n, cap;
} dict_t;

static void dict_add(dict_t *d, const char *w){
    if(!w || !*w) return;
    /* binary search or linear: small dict -> linear ok */
    for(int i=0;i<d->n;++i) if(strcmp(d->words[i], w)==0) return;
    if(d->n==d->cap){ d->cap = d->cap? d->cap*2 : 256; d->words = realloc(d->words, d->cap * sizeof(char*)); }
    d->words[d->n++] = strdup(w);
}

static dict_t *build_dict_from_bytes(const uint8_t *buf,int len){
    dict_t *d = xcalloc(1,sizeof(dict_t));
    char tmp[256]; int p=0;
    for(int i=0;i<=len;++i){
        int is_letter = (i<len) && ((buf[i]>='A'&&buf[i]<='Z')||(buf[i]>='a'&&buf[i]<='z'));
        if(is_letter){
            if(p < (int)sizeof(tmp)-1) tmp[p++] = (char)tolower(buf[i]);
        } else {
            if(p>0){ tmp[p]=0; dict_add(d,tmp); p=0; }
        }
    }
    return d;
}

/* ---------------- tiny model (embeddings + head) ---------------- */
typedef struct {
    int V, D;
    float *tok_emb; /* V x D */
    float *pos_emb; /* L x D */
    float *head_W;  /* D x V */
    float *head_b;  /* V */
    /* adam state */
    float *m_tok, *v_tok, *m_head, *v_head, *m_head_b, *v_head_b;
} model_t;

static float gauss(){ static int has=0; static float g; if(has){ has=0; return g; } float u,v,s; do{ u=2*frand()-1; v=2*frand()-1; s=u*u+v*v; }while(s==0||s>=1); g=u*sqrtf(-2*logf(s)/s); has=1; return g; }

static model_t *model_new(int V,int D,int L){
    model_t *m = xcalloc(1,sizeof(model_t));
    m->V=V; m->D=D;
    m->tok_emb = xmalloc(V*D*sizeof(float));
    m->pos_emb = xmalloc(L*D*sizeof(float));
    m->head_W  = xmalloc(D*V*sizeof(float));
    m->head_b  = xcalloc(V,sizeof(float));
    for(int i=0;i<V*D;++i) m->tok_emb[i] = gauss()*0.02f;
    for(int i=0;i<L*D;++i) m->pos_emb[i] = gauss()*0.02f;
    for(int i=0;i<D*V;++i) m->head_W[i] = gauss()*0.02f;
    m->m_tok = xcalloc(V*D,sizeof(float)); m->v_tok = xcalloc(V*D,sizeof(float));
    m->m_head = xcalloc(D*V,sizeof(float)); m->v_head = xcalloc(D*V,sizeof(float));
    m->m_head_b = xcalloc(V,sizeof(float)); m->v_head_b = xcalloc(V,sizeof(float));
    return m;
}

static void model_free(model_t *m){
    if(!m) return;
    free(m->tok_emb); free(m->pos_emb); free(m->head_W); free(m->head_b);
    free(m->m_tok); free(m->v_tok); free(m->m_head); free(m->v_head); free(m->m_head_b); free(m->v_head_b);
    free(m);
}

/* save/load model (embeddings + head + pos) */
static int model_save(const char *fn, model_t *m,int L){
    FILE *f=fopen(fn,"wb"); if(!f) return 0;
    fwrite(&m->V,sizeof(int),1,f); fwrite(&m->D,sizeof(int),1,f); fwrite(&L,sizeof(int),1,f);
    fwrite(m->tok_emb,sizeof(float),m->V*m->D,f);
    fwrite(m->pos_emb,sizeof(float),L*m->D,f);
    fwrite(m->head_W,sizeof(float),m->D*m->V,f);
    fwrite(m->head_b,sizeof(float),m->V,f);
    fclose(f); return 1;
}

/* softmax inplace */
static void softmax(float *p,int n){
    float m=p[0]; for(int i=1;i<n;++i) if(p[i]>m) m=p[i];
    float s=0; for(int i=0;i<n;++i){ p[i]=expf(p[i]-m); s+=p[i]; } for(int i=0;i<n;++i) p[i]/=s;
}

/* sample continuation using full tiny transformer-like forward (we keep simple: use embeddings+pos only) */
static uint8_t *sample_continuation_simple(model_t *m, const uint8_t *prefix,int plen,int out_len,int Lctx){
    int V=m->V, D=m->D;
    uint8_t *out = xcalloc(out_len,1);
    float *ctx = xcalloc(Lctx*D,sizeof(float));
    int ctx_pos=0;
    for(int i=0;i<plen && i<Lctx;++i){
        int t = prefix[i];
        for(int d=0;d<D;++d) ctx[(i%Lctx)*D + d] = m->tok_emb[t*D + d] + m->pos_emb[(i%Lctx)*D + d];
        ctx_pos = (i+1)%Lctx;
    }
    for(int t=0;t<out_len;++t){
        float *x_in = ctx + ((ctx_pos==0?Lctx-1:ctx_pos-1)*D);
        /* logits = head_W^T * x_in + head_b */
        float *logits = xcalloc(V,sizeof(float));
        for(int v=0; v<V; ++v){
            float s = m->head_b[v];
            for(int d=0; d<D; ++d) s += m->head_W[d*V + v] * x_in[d];
            logits[v] = s;
        }
        softmax(logits,V);
        /* sample */
        float r = frand(); float acc=0; int pick=V-1;
        for(int i=0;i<V;++i){ acc+=logits[i]; if(acc>=r){ pick=i; break; } }
        out[t] = (uint8_t)pick;
        /* advance context */
        for(int d=0; d<D; ++d) ctx[ctx_pos*D + d] = m->tok_emb[pick*D + d] + m->pos_emb[ctx_pos*D + d];
        ctx_pos = (ctx_pos+1)%Lctx;
        free(logits);
    }
    free(ctx);
    return out;
}

/* score generated bytes by fraction of words present in dictionary */
static float score_generated_against_dict(dict_t *dict, const uint8_t *bytes,int len){
    int matches=0, total=0;
    char buf[256]; int p=0;
    for(int i=0;i<=len;++i){
        int is_letter = (i<len) && isalpha(bytes[i]);
        if(is_letter){
            if(p < (int)sizeof(buf)-1) buf[p++] = (char)tolower(bytes[i]);
        } else {
            if(p>0){ buf[p]=0; total++;
                /* check membership */
                for(int j=0;j<dict->n;++j) if(strcmp(dict->words[j], buf)==0){ matches++; break; }
                p=0;
            }
        }
    }
    if(total==0) return 0.0f;
    return (float)matches / (float)total;
}

/* ---------------- training (embeddings+head) with reward scaling ---------------- */
static void adam_update(float *param, float *m, float *v, float *grad, int n, float lr, int t){
    const float b1=0.9f, b2=0.999f, eps=1e-8f;
    float b1t = 1.0f - powf(b1, t);
    float b2t = 1.0f - powf(b2, t);
    for(int i=0;i<n;++i){
        m[i] = b1*m[i] + (1-b1)*grad[i];
        v[i] = b2*v[i] + (1-b2)*grad[i]*grad[i];
        float mhat = m[i]/b1t;
        float vhat = v[i]/b2t;
        param[i] -= lr * mhat / (sqrtf(vhat) + eps);
    }
}

static void train_with_dict(model_t *m, uint8_t *data,int data_len, dict_t *dict,
                            int epochs,int batch,int seq_len,int Lctx,float base_lr){
    int V=m->V, D=m->D;
    int steps = data_len / (batch * seq_len); if(steps<1) steps=1;
    /* allocate grads */
    float *g_tok = xcalloc(V*D,sizeof(float));
    float *g_head = xcalloc(D*V,sizeof(float));
    float *g_head_b = xcalloc(V,sizeof(float));
    int tstep = 1;
    for(int ep=0; ep<epochs; ++ep){
        double epoch_loss = 0.0;
        double t0 = now_s();
        /* compute reward by sampling a few continuations and scoring against dict */
        int reward_samples = 3;
        float reward_sum = 0.0f;
        for(int s=0;s<reward_samples;++s){
            /* pick random prefix from data */
            int pos = rand() % (data_len - 8);
            uint8_t prefix[8];
            for(int i=0;i<8;++i) prefix[i] = data[pos+i];
            uint8_t *gen = sample_continuation_simple(m, prefix, 8, 40, Lctx);
            float r = score_generated_against_dict(dict, gen, 40);
            reward_sum += r;
            free(gen);
        }
        float reward = reward_sum / (float)reward_samples;
        /* scale learning rate: lr_eff = base_lr * (1 + alpha * reward) */
        float alpha = 2.0f;
        float lr_eff = base_lr * (1.0f + alpha * reward);

        for(int step=0; step<steps; ++step){
            /* zero grads */
            memset(g_tok,0,V*D*sizeof(float));
            memset(g_head,0,D*V*sizeof(float));
            memset(g_head_b,0,V*sizeof(float));
            /* sample batch starts */
            int *starts = xmalloc(batch*sizeof(int));
            for(int b=0;b<batch;++b) starts[b] = rand() % (data_len - seq_len - 1);
            /* for each position in sequence */
            for(int pos=0; pos<seq_len; ++pos){
                for(int b=0;b<batch;++b){
                    int idx = starts[b] + pos;
                    int tok = data[idx];
                    /* h = tok_emb + pos_emb */
                    float *h = xmalloc(D*sizeof(float));
                    for(int d=0; d<D; ++d) h[d] = m->tok_emb[tok*D + d] + m->pos_emb[(pos%Lctx)*D + d];
                    /* logits */
                    float *logits = xmalloc(V*sizeof(float));
                    for(int v=0; v<V; ++v){
                        float s = m->head_b[v];
                        for(int d=0; d<D; ++d) s += m->head_W[d*V + v] * h[d];
                        logits[v] = s;
                    }
                    softmax(logits,V);
                    int target = data[idx+1];
                    epoch_loss += -logf(fmaxf(1e-12f, logits[target]));
                    logits[target] -= 1.0f; /* dL/dlogits */
                    /* accumulate grads */
                    for(int v=0; v<V; ++v){
                        g_head_b[v] += logits[v];
                        for(int d=0; d<D; ++d) g_head[d*V + v] += logits[v] * h[d];
                    }
                    /* dh = W_head * dlogits */
                    float *dh = xmalloc(D*sizeof(float));
                    for(int d=0; d<D; ++d){
                        float s = 0.0f;
                        for(int v=0; v<V; ++v) s += m->head_W[d*V + v] * logits[v];
                        dh[d] = s;
                    }
                    /* accumulate into token embedding grad (pos emb fixed) */
                    for(int d=0; d<D; ++d) g_tok[tok*D + d] += dh[d];
                    free(h); free(logits); free(dh);
                }
            }
            free(starts);
            /* average grads */
            float scale = 1.0f / (batch * seq_len);
            for(int i=0;i<V*D;++i) g_tok[i] *= scale;
            for(int i=0;i<D*V;++i) g_head[i] *= scale;
            for(int i=0;i<V;++i) g_head_b[i] *= scale;
            /* Adam updates with lr_eff */
            adam_update(m->tok_emb, m->m_tok, m->v_tok, g_tok, V*D, lr_eff, tstep);
            adam_update(m->head_W, m->m_head, m->v_head, g_head, D*V, lr_eff, tstep);
            adam_update(m->head_b, m->m_head_b, m->v_head_b, g_head_b, V, lr_eff, tstep);
            tstep++;
        }
        double t1 = now_s();
        double avg_loss = epoch_loss / (double)(steps * batch * seq_len);
        printf("Epoch %d/%d  loss=%.6f  reward=%.3f  lr_eff=%.6g  time=%.2fs\n",
               ep+1, epochs, avg_loss, reward, lr_eff, t1 - t0);
        /* print a sample continuation using a random template from dict (or fallback) */
        if(dict->n>0){
            int pick = rand() % dict->n;
            const char *w = dict->words[pick];
            printf("Sample prompt: \"%s\" -> ", w);
            uint8_t pref[64]; int plen = (int)strlen(w);
            for(int i=0;i<plen;++i) pref[i] = (uint8_t)w[i];
            uint8_t *gen = sample_continuation_simple(m, pref, plen, 80, Lctx);
            for(int i=0;i<80;++i) putchar((char)gen[i]);
            putchar('\n');
            free(gen);
        }
    }
    free(g_tok); free(g_head); free(g_head_b);
}

/* ---------------- main ---------------- */
int main(int argc,char **argv){
    if(argc<2){ fprintf(stderr,"Usage: %s input.txt\n", argv[0]); return 1; }
    srand((unsigned)time(NULL));
    int data_len; uint8_t *data = read_all(argv[1], &data_len);
    if(!data){ fprintf(stderr,"failed to read %s\n", argv[1]); return 1; }
    /* build dictionary from input */
    dict_t *dict = build_dict_from_bytes(data, data_len);
    fprintf(stderr,"Dictionary: %d unique words\n", dict->n);
    /* auto hyperparams */
    int D,Lctx,N_unused=1;
    int epochs,batch,seq_len;
    if(data_len < 10000){ D=64; Lctx=64; epochs=30; batch=16; seq_len=32; }
    else if(data_len < 200000){ D=128; Lctx=128; epochs=12; batch=32; seq_len=64; }
    else { D=256; Lctx=256; epochs=6; batch=64; seq_len=128; }
    int V = 256;
    model_t *m = model_new(V,D,Lctx);
    /* train with dictionary reward */
    float base_lr = 1e-3f;
    train_with_dict(m, data, data_len, dict, epochs, batch, seq_len, Lctx, base_lr);
    /* save model */
    if(model_save("model.bin", m, Lctx)) fprintf(stderr,"Saved model.bin\n");
    else fprintf(stderr,"Failed to save model.bin\n");
    /* cleanup */
    for(int i=0;i<dict->n;++i) free(dict->words[i]);
    free(dict->words); free(dict);
    model_free(m); free(data);
    return 0;
}
