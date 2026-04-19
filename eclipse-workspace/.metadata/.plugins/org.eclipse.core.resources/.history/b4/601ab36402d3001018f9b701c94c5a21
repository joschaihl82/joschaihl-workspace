// main.c
// Trainer with progress bars and per-epoch saving using ggml for tensor allocation.
// Usage: ./trainer data.txt model_size_MBytes
// Produces model_epoch_<n>.bin after each epoch and model.bin at the end.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

#include "ggml.h"

#define MAGIC 0x4D4F444C
#define ASCII_VOCAB 128

/* ---------- Configurable defaults ---------- */
#define DEFAULT_EPOCHS 5
#define DEFAULT_LR 0.01f
#define MIN_HID 16
#define MIN_POOL_BYTES (32 * 1024 * 1024) // 32 MB

/* ---------- Utilities ---------- */

static void die(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

static double now_seconds(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

static float randf_small(void) {
    return ((float)rand() / (float)RAND_MAX - 0.5f) * 0.1f;
}

/* ---------- File I/O ---------- */

static char *load_text_file(const char *path, long *out_size) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long fsize = ftell(f);
    if (fsize < 0) { fclose(f); return NULL; }
    rewind(f);
    char *buf = malloc((size_t)fsize + 1);
    if (!buf) { fclose(f); return NULL; }
    if (fread(buf, 1, (size_t)fsize, f) != (size_t)fsize) { free(buf); fclose(f); return NULL; }
    buf[fsize] = '\0';
    fclose(f);
    *out_size = fsize;
    return buf;
}

/* ---------- Model sizing ---------- */

static void compute_dims_from_budget(size_t target_bytes, int *out_in, int *out_hid, int *out_out) {
    const int in_dim = ASCII_VOCAB;
    const int out_dim = ASCII_VOCAB;
    const size_t bytes_per_param = sizeof(float);
    long long target_params = (long long)(target_bytes / bytes_per_param);
    long long denom = (long long)in_dim + (long long)out_dim + 1LL;
    long long hid = 0;
    if (target_params > out_dim) hid = (target_params - out_dim) / denom;
    if (hid < MIN_HID) hid = MIN_HID;
    if (hid > (1LL<<20)) hid = (1LL<<20);
    *out_in = in_dim;
    *out_hid = (int)hid;
    *out_out = out_dim;
}

/* ---------- ggml context init ---------- */

static struct ggml_context *init_ggml_context_for_model(long long params_bytes) {
    size_t mem_size = (size_t)params_bytes + (size_t)(16 * 1024 * 1024); // +16MB overhead
    if (mem_size < MIN_POOL_BYTES) mem_size = MIN_POOL_BYTES;
    struct ggml_init_params ip = {
        .mem_size = mem_size,
        .mem_buffer = NULL
    };
    return ggml_init(ip);
}

/* ---------- Model struct and creation ---------- */

struct model_t {
    int in_dim;
    int hid_dim;
    int out_dim;
    struct ggml_tensor *tW1;
    struct ggml_tensor *tb1;
    struct ggml_tensor *tW2;
    struct ggml_tensor *tb2;
    float *W1;
    float *b1;
    float *W2;
    float *b2;
};

static int create_model_params(struct ggml_context *ctx, struct model_t *m) {
    int ne2_W1[2] = { m->in_dim, m->hid_dim }; // in x hid
    m->tW1 = ggml_new_tensor(ctx, GGML_TYPE_F32, 2, ne2_W1);
    m->tb1 = ggml_new_tensor_1d(ctx, GGML_TYPE_F32, m->hid_dim);
    int ne2_W2[2] = { m->hid_dim, m->out_dim }; // hid x out
    m->tW2 = ggml_new_tensor(ctx, GGML_TYPE_F32, 2, ne2_W2);
    m->tb2 = ggml_new_tensor_1d(ctx, GGML_TYPE_F32, m->out_dim);

    if (!m->tW1 || !m->tb1 || !m->tW2 || !m->tb2) return 0;

    m->W1 = ggml_get_data_f32(m->tW1);
    m->b1 = ggml_get_data_f32(m->tb1);
    m->W2 = ggml_get_data_f32(m->tW2);
    m->b2 = ggml_get_data_f32(m->tb2);

    if (!m->W1 || !m->b1 || !m->W2 || !m->b2) return 0;

    // initialize
    for (long i = 0; i < (long)m->in_dim * m->hid_dim; ++i) m->W1[i] = randf_small();
    for (int i = 0; i < m->hid_dim; ++i) m->b1[i] = 0.0f;
    for (long i = 0; i < (long)m->hid_dim * m->out_dim; ++i) m->W2[i] = randf_small();
    for (int i = 0; i < m->out_dim; ++i) m->b2[i] = 0.0f;

    return 1;
}

/* ---------- Save model ---------- */

static void save_model_bin(const char *out,
                           int in_dim, int hid, int out_dim,
                           const float *W1, const float *b1,
                           const float *W2, const float *b2) {
    FILE *f = fopen(out, "wb");
    if (!f) { perror("fopen"); return; }
    uint32_t magic = MAGIC;
    fwrite(&magic, sizeof(magic), 1, f);
    fwrite(&in_dim, sizeof(int), 1, f);
    fwrite(&hid, sizeof(int), 1, f);
    fwrite(&out_dim, sizeof(int), 1, f);
    fwrite(W1, sizeof(float), (size_t)in_dim * hid, f);
    fwrite(b1, sizeof(float), (size_t)hid, f);
    fwrite(W2, sizeof(float), (size_t)hid * out_dim, f);
    fwrite(b2, sizeof(float), (size_t)out_dim, f);
    fclose(f);
}

/* ---------- Progress bar helpers ---------- */

static void print_progress_bar_line(const char *label, double fraction, int width, double elapsed, double eta) {
    if (fraction < 0.0) fraction = 0.0;
    if (fraction > 1.0) fraction = 1.0;
    int filled = (int)round(fraction * width);
    printf("%s [", label);
    for (int i = 0; i < filled; ++i) putchar('=');
    if (filled < width) putchar('>');
    for (int i = filled + 1; i < width; ++i) putchar(' ');
    printf("] %3.0f%%  elapsed: %5.1fs  eta: %5.1fs\n", fraction * 100.0, elapsed, eta);
}

static void print_progress_bar_inline(const char *label, double fraction, int width, double elapsed, double eta) {
    if (fraction < 0.0) fraction = 0.0;
    if (fraction > 1.0) fraction = 1.0;
    int filled = (int)round(fraction * width);
    printf("\r%s [", label);
    for (int i = 0; i < filled; ++i) putchar('=');
    if (filled < width) putchar('>');
    for (int i = filled + 1; i < width; ++i) putchar(' ');
    printf("] %3.0f%%  elapsed: %5.1fs  eta: %5.1fs", fraction * 100.0, elapsed, eta);
    fflush(stdout);
}

/* ---------- Training routine (with progress bars) ---------- */

static void train_model_with_progress(struct model_t *m, const int *data, long data_len, int epochs, float lr) {
    const int V = ASCII_VOCAB;
    // host buffers
    float *h = malloc((size_t)m->hid_dim * sizeof(float));
    float *o = malloc((size_t)m->out_dim * sizeof(float));
    float *do_grad = malloc((size_t)m->out_dim * sizeof(float));
    float *dh = malloc((size_t)m->hid_dim * sizeof(float));
    if (!h || !o || !do_grad || !dh) die("Out of memory for training buffers");

    const int bar_width = 40;
    const int update_every = 100; // update progress every N steps to reduce overhead

    double t_start_all = now_seconds();

    for (int ep = 0; ep < epochs; ++ep) {
        double t_epoch_start = now_seconds();
        double epoch_elapsed = 0.0;
        long steps = data_len > 1 ? data_len - 1 : 0;

        double last_update = t_epoch_start;
        double loss = 0.0;

        for (long t = 0; t < steps; ++t) {
            int x = data[t];
            int y = data[t + 1];

            // forward
            for (int j = 0; j < m->hid_dim; ++j) h[j] = m->b1[j];
            for (int i = 0; i < m->in_dim; ++i) {
                if ((i % V) == x) {
                    long base = (long)i * m->hid_dim;
                    for (int j = 0; j < m->hid_dim; ++j) h[j] += m->W1[base + j];
                }
            }
            for (int j = 0; j < m->hid_dim; ++j) h[j] = tanhf(h[j]);

            for (int k = 0; k < m->out_dim; ++k) o[k] = m->b2[k];
            for (int j = 0; j < m->hid_dim; ++j) {
                long base = (long)j * m->out_dim;
                float hj = h[j];
                for (int k = 0; k < m->out_dim; ++k) o[k] += m->W2[base + k] * hj;
            }

            // softmax
            float maxv = o[0];
            for (int k = 1; k < m->out_dim; ++k) if (o[k] > maxv) maxv = o[k];
            float sum = 0.0f;
            for (int k = 0; k < m->out_dim; ++k) { o[k] = expf(o[k] - maxv); sum += o[k]; }
            for (int k = 0; k < m->out_dim; ++k) o[k] /= sum;

            loss += -logf(o[y] + 1e-9f);

            // backprop
            for (int k = 0; k < m->out_dim; ++k) do_grad[k] = o[k];
            do_grad[y] -= 1.0f;

            for (int k = 0; k < m->out_dim; ++k) m->b2[k] -= lr * do_grad[k];
            for (int j = 0; j < m->hid_dim; ++j) {
                long base = (long)j * m->out_dim;
                float hj = h[j];
                for (int k = 0; k < m->out_dim; ++k) {
                    float g = do_grad[k] * hj;
                    m->W2[base + k] -= lr * g;
                }
            }

            for (int j = 0; j < m->hid_dim; ++j) {
                float s = 0.0f;
                long base = (long)j * m->out_dim;
                for (int k = 0; k < m->out_dim; ++k) s += m->W2[base + k] * do_grad[k];
                dh[j] = (1.0f - h[j] * h[j]) * s;
            }

            for (int j = 0; j < m->hid_dim; ++j) m->b1[j] -= lr * dh[j];
            for (int i = 0; i < m->in_dim; ++i) {
                if ((i % V) == x) {
                    long base = (long)i * m->hid_dim;
                    for (int j = 0; j < m->hid_dim; ++j) m->W1[base + j] -= lr * dh[j];
                }
            }

            // update progress periodically
            if ((t % update_every) == 0 || t == steps - 1) {
                double now = now_seconds();
                epoch_elapsed = now - t_epoch_start;
                double frac = steps > 0 ? (double)(t + 1) / (double)steps : 1.0;
                double est_total_epoch = epoch_elapsed / frac;
                double eta_epoch = est_total_epoch - epoch_elapsed;

                double overall_elapsed = now - t_start_all;
                double overall_frac = (double)(ep + frac) / (double)epochs;
                double est_total_all = overall_elapsed / overall_frac;
                double eta_all = est_total_all - overall_elapsed;

                // print two lines: epoch inline bar and overall bar below
                // clear previous two lines by printing \r and then overwrite
                print_progress_bar_inline("Epoch", frac, bar_width, epoch_elapsed, eta_epoch);
                // print overall on next line
                printf("\n");
                print_progress_bar_line("Overall", (double)(ep + frac) / (double)epochs, bar_width, overall_elapsed, eta_all);
                // move cursor up one line so next inline update overwrites epoch line
                printf("\033[F"); // ANSI: move cursor up one line
                fflush(stdout);

                last_update = now;
            }
        } // steps

        // finish epoch: print final epoch and overall bars
        double t_epoch_end = now_seconds();
        double epoch_time = t_epoch_end - t_epoch_start;
        double overall_elapsed = t_epoch_end - t_start_all;
        // final epoch line
        print_progress_bar_inline("Epoch", 1.0, bar_width, epoch_time, 0.0);
        printf("\n");
        // final overall line for this epoch
        print_progress_bar_line("Overall", (double)(ep + 1) / (double)epochs, bar_width, overall_elapsed, (double)(epochs - ep - 1) * epoch_time);

        // compute and print epoch summary
        double avg_loss = loss / (steps > 0 ? steps : 1);
        printf("Epoch %d/%d finished. epoch_time=%.2fs avg_loss=%.6f\n", ep + 1, epochs, epoch_time, avg_loss);

        // save model after this epoch
        char fname[256];
        snprintf(fname, sizeof(fname), "model_epoch_%d.bin", ep + 1);
        save_model_bin(fname, m->in_dim, m->hid_dim, m->out_dim, m->W1, m->b1, m->W2, m->b2);
        printf("Saved %s\n", fname);
    } // epochs

    // final save as model.bin
    save_model_bin("model.bin", m->in_dim, m->hid_dim, m->out_dim, m->W1, m->b1, m->W2, m->b2);
    printf("Saved model.bin (final)\n");

    free(h); free(o); free(do_grad); free(dh);
}

/* ---------- main (small) ---------- */

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s data.txt model_size_MBytes\n", argv[0]);
        return 1;
    }

    const char *path = argv[1];
    double model_mb = atof(argv[2]);
    if (model_mb <= 0.0) die("Invalid model size (MB)");

    long text_size = 0;
    char *text = load_text_file(path, &text_size);
    if (!text) die("Failed to read training file");

    size_t target_bytes = (size_t)llround(model_mb * 1024.0 * 1024.0);

    int in_dim = 0, hid_dim = 0, out_dim = 0;
    compute_dims_from_budget(target_bytes, &in_dim, &hid_dim, &out_dim);

    long long params = (long long)in_dim * hid_dim + (long long)hid_dim * out_dim + hid_dim + out_dim;
    long long bytes = params * (long long)sizeof(float);

    printf("File: %s (%ld bytes)\n", path, text_size);
    printf("Model target: %.3f MB -> target bytes: %zu\n", model_mb, target_bytes);
    printf("Using dims: in=%d hid=%d out=%d\n", in_dim, hid_dim, out_dim);
    printf("Estimated params: %lld, bytes: %lld\n", params, bytes);

    // prepare data indices
    int *data = malloc((size_t)text_size * sizeof(int));
    if (!data) die("malloc failed");
    for (long i = 0; i < text_size; ++i) {
        unsigned char c = (unsigned char)text[i];
        data[i] = (c < ASCII_VOCAB) ? (int)c : 0;
    }

    // init ggml
    struct ggml_context *ctx = init_ggml_context_for_model(bytes);
    if (!ctx) die("ggml_init failed");

    // create model
    struct model_t model;
    model.in_dim = in_dim;
    model.hid_dim = hid_dim;
    model.out_dim = out_dim;

    if (!create_model_params(ctx, &model)) die("create_model_params failed");

    // train with progress and per-epoch saving
    srand((unsigned)time(NULL));
    const int epochs = DEFAULT_EPOCHS;
    const float lr = DEFAULT_LR;
    train_model_with_progress(&model, data, text_size, epochs, lr);

    // cleanup
    free(text);
    free(data);
    ggml_free(ctx);

    return 0;
}
