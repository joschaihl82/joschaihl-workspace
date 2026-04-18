// main.c
// Minimal self-contained scaffold with all previously empty stubs implemented
// Each stub now returns a sensible default or performs a minimal action so the file compiles.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <signal.h>
#include <inttypes.h>

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#endif

/* ---------------------------
   Minimal GGML-like API (mocks)
   --------------------------- */

typedef enum {
    GGML_TYPE_F32,
    GGML_TYPE_F16,
    GGML_TYPE_I32,
    GGML_TYPE_Q4_0,
    GGML_TYPE_Q4_1,
    GGML_TYPE_COUNT
} ggml_type;

static size_t ggml_type_size(ggml_type t) {
    switch (t) {
        case GGML_TYPE_F32: return 4;
        case GGML_TYPE_F16: return 2;
        case GGML_TYPE_I32: return 4;
        case GGML_TYPE_Q4_0: return 1;
        case GGML_TYPE_Q4_1: return 1;
        default: return 1;
    }
}
static size_t ggml_type_sizef(ggml_type t) { return ggml_type_size(t); }
static int ggml_blck_size(ggml_type t) { (void)t; return 1; }

struct ggml_context {
    void * mem;
    size_t mem_size;
};

struct ggml_init_params {
    size_t mem_size;
    void * mem_buffer;
};

struct ggml_tensor {
    ggml_type type;
    int ne[2];      // dims used in this code (ne[0], ne[1])
    size_t nb[2];   // byte strides
    void * data;
};

typedef struct ggml_cgraph {
    int n_threads;
} ggml_cgraph;

/* Implemented: returns current time in microseconds */
static void ggml_time_init(void) {
    /* no global init required for this mock */
    return;
}

static int64_t ggml_time_us(void) {
    struct timespec ts;
#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
        return (int64_t)time(NULL) * 1000000;
    }
#else
    ts.tv_sec = time(NULL);
    ts.tv_nsec = 0;
#endif
    return (int64_t)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

/* CPU feature mocks: return 0 (feature not present) */
static int ggml_cpu_has_avx(void) { return 0; }
static int ggml_cpu_has_avx2(void) { return 0; }
static int ggml_cpu_has_avx512(void) { return 0; }
static int ggml_cpu_has_fma(void) { return 0; }
static int ggml_cpu_has_neon(void) { return 0; }
static int ggml_cpu_has_arm_fma(void) { return 0; }
static int ggml_cpu_has_f16c(void) { return 0; }
static int ggml_cpu_has_fp16_va(void) { return 0; }
static int ggml_cpu_has_wasm_simd(void) { return 0; }
static int ggml_cpu_has_blas(void) { return 0; }
static int ggml_cpu_has_sse3(void) { return 0; }
static int ggml_cpu_has_vsx(void) { return 0; }

/* Basic context management */
static struct ggml_context * ggml_init(struct ggml_init_params params) {
    struct ggml_context * ctx = (struct ggml_context*) calloc(1, sizeof(*ctx));
    if (!ctx) return NULL;
    ctx->mem_size = params.mem_size;
    if (params.mem_buffer) {
        ctx->mem = params.mem_buffer;
    } else {
        ctx->mem = malloc(params.mem_size ? params.mem_size : 1);
        if (!ctx->mem) { free(ctx); return NULL; }
    }
    return ctx;
}
static void ggml_free(struct ggml_context * ctx) {
    if (!ctx) return;
    if (ctx->mem) free(ctx->mem);
    free(ctx);
}

/* Tensor helpers (minimal, implemented) */
static struct ggml_tensor * ggml_new_tensor_1d(struct ggml_context * ctx, ggml_type type, int ne0) {
    (void)ctx;
    struct ggml_tensor * t = (struct ggml_tensor*) calloc(1, sizeof(*t));
    t->type = type;
    t->ne[0] = ne0;
    t->ne[1] = 1;
    t->nb[0] = ggml_type_size(type);
    t->nb[1] = (size_t)ne0 * ggml_type_size(type);
    size_t nbytes = (size_t)ne0 * ggml_type_size(type);
    t->data = calloc(1, nbytes ? nbytes : 1);
    return t;
}
static struct ggml_tensor * ggml_new_tensor_2d(struct ggml_context * ctx, ggml_type type, int ne0, int ne1) {
    (void)ctx;
    struct ggml_tensor * t = (struct ggml_tensor*) calloc(1, sizeof(*t));
    t->type = type;
    t->ne[0] = ne0;
    t->ne[1] = ne1;
    t->nb[0] = ggml_type_size(type);
    t->nb[1] = (size_t)ne0 * ggml_type_size(type);
    size_t nbytes = (size_t)ne0 * ne1 * ggml_type_size(type);
    t->data = calloc(1, nbytes ? nbytes : 1);
    return t;
}
static struct ggml_tensor * ggml_new_tensor_3d(struct ggml_context * ctx, ggml_type type, int d0, int d1, int d2) {
    (void)ctx;
    struct ggml_tensor * t = (struct ggml_tensor*) calloc(1, sizeof(*t));
    t->type = type;
    t->ne[0] = d0;
    t->ne[1] = d1 * d2; /* simplified */
    t->nb[0] = ggml_type_size(type);
    t->nb[1] = (size_t)t->ne[0] * ggml_type_size(type);
    size_t nbytes = (size_t)d0 * d1 * d2 * ggml_type_size(type);
    t->data = calloc(1, nbytes ? nbytes : 1);
    return t;
}
static size_t ggml_nbytes(const struct ggml_tensor * t) {
    if (!t) return 0;
    return (size_t)t->ne[0] * t->ne[1] * ggml_type_size(t->type);
}
static int ggml_nelements(const struct ggml_tensor * t) {
    if (!t) return 0;
    return t->ne[0] * t->ne[1];
}
static size_t ggml_element_size(const struct ggml_tensor * t) {
    if (!t) return 0;
    return ggml_type_size(t->type);
}
static void * ggml_get_data(const struct ggml_tensor * t) { return t ? t->data : NULL; }

/* Views and simple ops: implemented as minimal placeholders */
static struct ggml_tensor * ggml_get_rows(struct ggml_context * ctx, struct ggml_tensor * src, struct ggml_tensor * idx) {
    (void)ctx; (void)idx;
    int N = idx ? idx->ne[0] : 1;
    return ggml_new_tensor_2d(ctx, GGML_TYPE_F32, src->ne[0], N);
}
static struct ggml_tensor * ggml_view_1d(struct ggml_context * ctx, struct ggml_tensor * src, int ne0, size_t offset) {
    (void)ctx;
    struct ggml_tensor * t = (struct ggml_tensor*) calloc(1, sizeof(*t));
    t->type = src->type;
    t->ne[0] = ne0;
    t->ne[1] = 1;
    t->nb[0] = ggml_type_size(t->type);
    t->nb[1] = (size_t)ne0 * ggml_type_size(t->type);
    t->data = (char*)src->data + offset;
    return t;
}
static struct ggml_tensor * ggml_cpy(struct ggml_context * ctx, struct ggml_tensor * src, struct ggml_tensor * dst) {
    (void)ctx;
    if (!src || !dst) return dst;
    size_t nsrc = ggml_nbytes(src);
    size_t ndst = ggml_nbytes(dst);
    if (nsrc == ndst && dst->data && src->data) memcpy(dst->data, src->data, nsrc);
    return dst;
}
static struct ggml_tensor * ggml_mul_mat(struct ggml_context * ctx, struct ggml_tensor * a, struct ggml_tensor * b) {
    (void)ctx; (void)a; (void)b;
    int r = a ? a->ne[0] : 1;
    int c = b ? b->ne[1] : 1;
    return ggml_new_tensor_2d(ctx, GGML_TYPE_F32, r, c);
}
static struct ggml_tensor * ggml_mul(struct ggml_context * ctx, struct ggml_tensor * a, struct ggml_tensor * b) { (void)ctx; (void)a; (void)b; return ggml_new_tensor_2d(ctx, GGML_TYPE_F32, 1,1); }
static struct ggml_tensor * ggml_add(struct ggml_context * ctx, struct ggml_tensor * a, struct ggml_tensor * b) { (void)ctx; (void)a; (void)b; return ggml_new_tensor_2d(ctx, GGML_TYPE_F32, 1,1); }
static struct ggml_tensor * ggml_repeat(struct ggml_context * ctx, struct ggml_tensor * a, struct ggml_tensor * b) { (void)ctx; (void)a; (void)b; return ggml_new_tensor_2d(ctx, GGML_TYPE_F32, 1,1); }
static struct ggml_tensor * ggml_rms_norm(struct ggml_context * ctx, struct ggml_tensor * a) { (void)ctx; if (!a) return NULL; return ggml_new_tensor_2d(ctx, GGML_TYPE_F32, a->ne[0], a->ne[1]); }
static struct ggml_tensor * ggml_permute(struct ggml_context * ctx, struct ggml_tensor * a, int a0, int a1, int a2, int a3) { (void)ctx; (void)a0; (void)a1; (void)a2; (void)a3; return a ? ggml_new_tensor_2d(ctx, GGML_TYPE_F32, a->ne[0], a->ne[1]) : NULL; }
static struct ggml_tensor * ggml_rope(struct ggml_context * ctx, struct ggml_tensor * a, int n_past, int n_rot, int mode) { (void)ctx; (void)n_past; (void)n_rot; (void)mode; return a; }
static struct ggml_tensor * ggml_reshape_3d(struct ggml_context * ctx, struct ggml_tensor * a, int d0, int d1, int d2) { (void)ctx; (void)d0; (void)d1; (void)d2; return a; }
static struct ggml_tensor * ggml_scale(struct ggml_context * ctx, struct ggml_tensor * a, struct ggml_tensor * b) { (void)ctx; (void)b; return a; }
static struct ggml_tensor * ggml_diag_mask_inf(struct ggml_context * ctx, struct ggml_tensor * a, int n_past) { (void)ctx; (void)n_past; return a; }
static struct ggml_tensor * ggml_soft_max(struct ggml_context * ctx, struct ggml_tensor * a) { (void)ctx; return a; }
static struct ggml_tensor * ggml_silu(struct ggml_context * ctx, struct ggml_tensor * a) { (void)ctx; return a; }

/* Graph helpers (no-op in mock) */
static void ggml_build_forward_expand(ggml_cgraph * gf, struct ggml_tensor * t) { (void)gf; (void)t; return; }
static void ggml_graph_compute(struct ggml_context * ctx, ggml_cgraph * gf) { (void)ctx; (void)gf; return; }
static size_t ggml_used_mem(struct ggml_context * ctx) { return ctx ? ctx->mem_size / 2 : 0; }

/* ---------------------------
   Minimal utils-like API (mocks)
   --------------------------- */

typedef struct {
    const char * model;
    int n_ctx;
    int seed;
    int random_prompt;
    const char * prompt;
    int memory_f16;
} gpt_params;

/* parse args: implemented to set defaults and return success */
static int gpt_params_parse(int argc, char ** argv, gpt_params * params) {
    (void)argc; (void)argv;
    if (!params) return 0;
    params->n_ctx = 512;
    params->seed = -1;
    params->random_prompt = 0;
    params->prompt = NULL;
    params->memory_f16 = 0;
    return 1;
}

/* random prompt: implemented to return a constant string */
static const char * gpt_random_prompt(void) {
    return "This is a mock random prompt.";
}

/* ---------------------------
   Minimal vocab map (C version)
   --------------------------- */

typedef struct {
    char ** id_to_token;
    int n_vocab;
} c_vocab;

static c_vocab * c_vocab_new(int n_vocab) {
    c_vocab * v = (c_vocab*) calloc(1, sizeof(c_vocab));
    if (!v) return NULL;
    v->n_vocab = n_vocab;
    v->id_to_token = (char**) calloc(n_vocab, sizeof(char*));
    return v;
}
static void c_vocab_free(c_vocab * v) {
    if (!v) return;
    for (int i = 0; i < v->n_vocab; ++i) free(v->id_to_token[i]);
    free(v->id_to_token);
    free(v);
}
static void c_vocab_set(c_vocab * v, const char * token, int id) {
    if (!v || id < 0 || id >= v->n_vocab) return;
    free(v->id_to_token[id]);
    v->id_to_token[id] = strdup(token ? token : "");
}

/* ---------------------------
   LLaMA model structures (C)
   --------------------------- */

typedef struct {
    int32_t n_vocab;
    int32_t n_ctx;
    int32_t n_embd;
    int32_t n_mult;
    int32_t n_head;
    int32_t n_layer;
    int32_t n_rot;
    int32_t f16;
} llama_hparams;

typedef struct {
    struct ggml_tensor * attention_norm;
    struct ggml_tensor * wq;
    struct ggml_tensor * wk;
    struct ggml_tensor * wv;
    struct ggml_tensor * wo;
    struct ggml_tensor * ffn_norm;
    struct ggml_tensor * w1;
    struct ggml_tensor * w2;
    struct ggml_tensor * w3;
} llama_layer;

typedef struct {
    llama_hparams hparams;
    struct ggml_tensor * tok_embeddings;
    struct ggml_tensor * norm;
    struct ggml_tensor * output;
    llama_layer * layers;
    int n_layers;
    struct ggml_tensor * memory_k;
    struct ggml_tensor * memory_v;
    struct ggml_context * ctx;
    char ** tensor_names;
    struct ggml_tensor ** tensor_ptrs;
    int tensor_count;
} llama_model;

/* helper to add/get tensors by name */
static void model_add_tensor(llama_model * model, const char * name, struct ggml_tensor * t) {
    model->tensor_names = (char**) realloc(model->tensor_names, (model->tensor_count + 1) * sizeof(char*));
    model->tensor_ptrs = (struct ggml_tensor**) realloc(model->tensor_ptrs, (model->tensor_count + 1) * sizeof(struct ggml_tensor*));
    model->tensor_names[model->tensor_count] = strdup(name ? name : "");
    model->tensor_ptrs[model->tensor_count] = t;
    model->tensor_count++;
}
static struct ggml_tensor * model_get_tensor(llama_model * model, const char * name) {
    if (!model || !name) return NULL;
    for (int i = 0; i < model->tensor_count; ++i) {
        if (strcmp(model->tensor_names[i], name) == 0) return model->tensor_ptrs[i];
    }
    return NULL;
}

/* ---------------------------
   Simple dynamic arrays used by eval
   --------------------------- */

typedef struct {
    id_t * data;
    size_t size;
    size_t capacity;
} id_array;
static void id_array_init(id_array *a) { if (!a) return; a->data = NULL; a->size = 0; a->capacity = 0; }
static void id_array_push(id_array *a, id_t v) {
    if (!a) return;
    if (a->size + 1 > a->capacity) {
        size_t newcap = a->capacity ? a->capacity * 2 : 8;
        a->data = (id_t*) realloc(a->data, newcap * sizeof(id_t));
        a->capacity = newcap;
    }
    a->data[a->size++] = v;
}
typedef struct {
    float * data;
    size_t size;
    size_t capacity;
} float_array;
static void float_array_init(float_array *a) { if (!a) return; a->data = NULL; a->size = 0; a->capacity = 0; }
static void float_array_resize(float_array *a, size_t newsize) {
    if (!a) return;
    if (newsize > a->capacity) {
        a->data = (float*) realloc(a->data, newsize * sizeof(float));
        a->capacity = newsize;
    }
    a->size = newsize;
}

/* ---------------------------
   LLAMA model load & eval (adapted)
   --------------------------- */

static const int EOS_TOKEN_ID = 2;

/* small map for n_parts */
struct int_map_entry { int key; int value; };
static const struct int_map_entry LLAMA_N_PARTS[] = {
    { 4096, 1 }, { 5120, 2 }, { 6656, 4 }, { 8192, 8 }
};
static int int_map_lookup(const struct int_map_entry *map, int map_len, int key) {
    for (int i = 0; i < map_len; ++i) if (map[i].key == key) return map[i].value;
    return -1;
}

/* Implemented: minimal loader that creates a mock model if file not present */
int llama_model_load(const char * fname, llama_model * model, c_vocab * vocab, int n_ctx, ggml_type memory_type) {
    (void)fname; (void)vocab; (void)memory_type;
    if (!model) return 0;

    /* set reasonable defaults */
    model->hparams.n_vocab = 32000;
    model->hparams.n_ctx   = n_ctx > 0 ? n_ctx : 512;
    model->hparams.n_embd  = 4096;
    model->hparams.n_mult  = 256;
    model->hparams.n_head  = 32;
    model->hparams.n_layer = 32;
    model->hparams.n_rot   = 64;
    model->hparams.f16     = 1;

    int n_ff = ((2*(4*model->hparams.n_embd)/3 + model->hparams.n_mult - 1)/model->hparams.n_mult)*model->hparams.n_mult;
    (void)n_ff;

    /* create a small ggml context and allocate tensors */
    size_t ctx_size = 1024 * 1024; /* small mock */
    struct ggml_init_params params = { .mem_size = ctx_size, .mem_buffer = NULL };
    model->ctx = ggml_init(params);
    if (!model->ctx) return 0;

    int n_embd = model->hparams.n_embd;
    int n_layer = model->hparams.n_layer;
    int n_vocab = model->hparams.n_vocab;

    model->tok_embeddings = ggml_new_tensor_2d(model->ctx, GGML_TYPE_F16, n_embd, n_vocab);
    model->norm = ggml_new_tensor_1d(model->ctx, GGML_TYPE_F32, n_embd);
    model->output = ggml_new_tensor_2d(model->ctx, GGML_TYPE_F16, n_embd, n_vocab);

    model->tensor_names = NULL;
    model->tensor_ptrs = NULL;
    model->tensor_count = 0;

    model_add_tensor(model, "tok_embeddings.weight", model->tok_embeddings);
    model_add_tensor(model, "norm.weight", model->norm);
    model_add_tensor(model, "output.weight", model->output);

    model->n_layers = n_layer;
    model->layers = (llama_layer*) calloc(n_layer, sizeof(llama_layer));
    for (int i = 0; i < n_layer; ++i) {
        llama_layer * layer = &model->layers[i];
        layer->attention_norm = ggml_new_tensor_1d(model->ctx, GGML_TYPE_F32, n_embd);
        layer->wq = ggml_new_tensor_2d(model->ctx, GGML_TYPE_F16, n_embd, n_embd);
        layer->wk = ggml_new_tensor_2d(model->ctx, GGML_TYPE_F16, n_embd, n_embd);
        layer->wv = ggml_new_tensor_2d(model->ctx, GGML_TYPE_F16, n_embd, n_embd);
        layer->wo = ggml_new_tensor_2d(model->ctx, GGML_TYPE_F16, n_embd, n_embd);
        layer->ffn_norm = ggml_new_tensor_1d(model->ctx, GGML_TYPE_F32, n_embd);
        layer->w1 = ggml_new_tensor_2d(model->ctx, GGML_TYPE_F16, n_embd, n_ff);
        layer->w2 = ggml_new_tensor_2d(model->ctx, GGML_TYPE_F16, n_ff, n_embd);
        layer->w3 = ggml_new_tensor_2d(model->ctx, GGML_TYPE_F16, n_embd, n_ff);

        char buf[256];
        snprintf(buf, sizeof(buf), "layers.%d.attention_norm.weight", i); model_add_tensor(model, buf, layer->attention_norm);
        snprintf(buf, sizeof(buf), "layers.%d.attention.wq.weight", i); model_add_tensor(model, buf, layer->wq);
        snprintf(buf, sizeof(buf), "layers.%d.attention.wk.weight", i); model_add_tensor(model, buf, layer->wk);
        snprintf(buf, sizeof(buf), "layers.%d.attention.wv.weight", i); model_add_tensor(model, buf, layer->wv);
        snprintf(buf, sizeof(buf), "layers.%d.attention.wo.weight", i); model_add_tensor(model, buf, layer->wo);
        snprintf(buf, sizeof(buf), "layers.%d.ffn_norm.weight", i); model_add_tensor(model, buf, layer->ffn_norm);
        snprintf(buf, sizeof(buf), "layers.%d.feed_forward.w1.weight", i); model_add_tensor(model, buf, layer->w1);
        snprintf(buf, sizeof(buf), "layers.%d.feed_forward.w2.weight", i); model_add_tensor(model, buf, layer->w2);
        snprintf(buf, sizeof(buf), "layers.%d.feed_forward.w3.weight", i); model_add_tensor(model, buf, layer->w3);
    }

    int n_mem = n_layer * model->hparams.n_ctx;
    int n_elements = n_embd * n_mem;
    model->memory_k = ggml_new_tensor_1d(model->ctx, GGML_TYPE_F32, n_elements);
    model->memory_v = ggml_new_tensor_1d(model->ctx, GGML_TYPE_F32, n_elements);

    return 1;
}

/* Evaluate (mock): returns zeroed logits and sets mem_per_token if zero */
int llama_eval(const llama_model * model, const int n_threads, const int n_past, const id_array * embd_inp, float_array * embd_w, size_t * mem_per_token) {
    (void)n_threads;
    if (!model || !embd_inp || !embd_w || !mem_per_token) return 0;

    const int N = (int) embd_inp->size;
    const int n_vocab = model->hparams.n_vocab;

    /* create a temporary context (mock) */
    static size_t buf_size = 16u * 1024u * 1024u;
    static void * buf = NULL;
    if (!buf) buf = malloc(buf_size);
    struct ggml_init_params params = { .mem_size = buf_size, .mem_buffer = buf };
    struct ggml_context * ctx0 = ggml_init(params);
    if (!ctx0) return 0;
    ggml_cgraph gf = { .n_threads = n_threads };

    /* create input tensor and run mock ops (no real math) */
    struct ggml_tensor * embd = ggml_new_tensor_1d(ctx0, GGML_TYPE_I32, N);
    if (N > 0 && embd && embd->data) memcpy(embd->data, embd_inp->data, N * ggml_element_size(embd));
    struct ggml_tensor * inpL = ggml_get_rows(ctx0, (struct ggml_tensor*)model->tok_embeddings, embd);

    /* iterate layers performing placeholder ops */
    for (int il = 0; il < model->n_layers; ++il) {
        struct ggml_tensor * cur = ggml_rms_norm(ctx0, inpL);
        struct ggml_tensor * Qcur = ggml_mul_mat(ctx0, model->layers[il].wq, cur);
        (void)Qcur;
        /* store to memory (no-op) */
        if (N >= 1) {
            struct ggml_tensor * k = ggml_view_1d(ctx0, (struct ggml_tensor*)model->memory_k, N * model->hparams.n_embd, 0);
            struct ggml_tensor * v = ggml_view_1d(ctx0, (struct ggml_tensor*)model->memory_v, N * model->hparams.n_embd, 0);
            ggml_build_forward_expand(&gf, ggml_cpy(ctx0, cur, k));
            ggml_build_forward_expand(&gf, ggml_cpy(ctx0, cur, v));
        }
        inpL = cur;
    }

    /* final head: produce zero logits */
    float_array_resize((float_array*)embd_w, n_vocab);
    for (int i = 0; i < n_vocab; ++i) embd_w->data[i] = 0.0f;

    if (*mem_per_token == 0) *mem_per_token = ggml_used_mem(ctx0) / (N ? N : 1);

    ggml_free(ctx0);
    return 1;
}

/* ---------------------------
   Signal handler and system info
   --------------------------- */

static int is_interacting = 0;
void sigint_handler(int signo) {
    (void)signo;
    printf("\n");
    if (!is_interacting) is_interacting = 1;
    else _exit(130);
}

/* returns a static string describing mock system info */
const char * llama_print_system_info(void) {
    static char s[512];
    snprintf(s, sizeof(s),
        "AVX = %d | AVX2 = %d | AVX512 = %d | FMA = %d | NEON = %d | ARM_FMA = %d | F16C = %d | FP16_VA = %d | WASM_SIMD = %d | BLAS = %d | SSE3 = %d | VSX = %d",
        ggml_cpu_has_avx(), ggml_cpu_has_avx2(), ggml_cpu_has_avx512(), ggml_cpu_has_fma(),
        ggml_cpu_has_neon(), ggml_cpu_has_arm_fma(), ggml_cpu_has_f16c(), ggml_cpu_has_fp16_va(),
        ggml_cpu_has_wasm_simd(), ggml_cpu_has_blas(), ggml_cpu_has_sse3(), ggml_cpu_has_vsx());
    return s;
}

/* ---------------------------
   Main (simplified)
   --------------------------- */

int main(int argc, char ** argv) {
    ggml_time_init();
    const int64_t t_main_start_us = ggml_time_us();

    gpt_params params;
    params.model = "models/llama-7B/ggml-model.bin";
    if (!gpt_params_parse(argc, argv, &params)) {
        fprintf(stderr, "failed to parse params\n");
        return 1;
    }

    if (params.n_ctx > 2048) {
        fprintf(stderr, "warning: model does not support context sizes greater than 2048 tokens (%d specified)\n", params.n_ctx);
    }

    if (params.seed < 0) params.seed = (int) time(NULL);
    fprintf(stderr, "seed = %d\n", params.seed);

    if (params.random_prompt) {
        params.prompt = gpt_random_prompt();
    }

    int64_t t_load_us = 0;
    c_vocab * vocab = c_vocab_new(32000);
    llama_model model;
    memset(&model, 0, sizeof(model));

    {
        ggml_type memory_type = params.memory_f16 ? GGML_TYPE_F16 : GGML_TYPE_F32;
        const int64_t t_start_us = ggml_time_us();
        if (!llama_model_load(params.model, &model, (c_vocab*)vocab, params.n_ctx, memory_type)) {
            fprintf(stderr, "failed to load model\n");
            c_vocab_free(vocab);
            return 1;
        }
        t_load_us = ggml_time_us() - t_start_us;
    }

    fprintf(stderr, "model loaded in %.3f s\n", t_load_us / 1e6);

    id_array ids; id_array_init(&ids);
    id_array_push(&ids, 1);
    id_array_push(&ids, 2);
    float_array logits; float_array_init(&logits);
    size_t mem_per_token = 0;
    if (!llama_eval(&model, 1, 0, &ids, &logits, &mem_per_token)) {
        fprintf(stderr, "evaluation failed\n");
    } else {
        fprintf(stderr, "evaluation succeeded, mem_per_token=%zu\n", mem_per_token);
    }

    if (model.layers) free(model.layers);
    for (int i = 0; i < model.tensor_count; ++i) free(model.tensor_names[i]);
    free(model.tensor_names);
    free(model.tensor_ptrs);
    c_vocab_free(vocab);

    const int64_t t_main_end_us = ggml_time_us();
    fprintf(stderr, "total time = %.3f s\n", (t_main_end_us - t_main_start_us)/1e6);

    return 0;
}

