// main.c
// Ncurses trainer with two progress bars and scrolling chat log.
// Adds periodic sampling: every 5 seconds during training the UI logs
// one input request (prompt) and the model's generated answer.
// Usage: ./trainer data.txt model_size_MBytes
// Build: gcc -O2 -std=c11 main.c ggml.c -lncurses -lm -o trainer

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <ncurses.h>

#include "ggml.h"

#define MAGIC 0x4D4F444C
#define ASCII_VOCAB 128

/* Config */
#define DEFAULT_EPOCHS 5
#define DEFAULT_LR 0.01f
#define MIN_HID 16
#define MIN_POOL_BYTES (32 * 1024 * 1024)
#define EVAL_SAMPLES 4
#define PROMPT_MAX_CHARS 160
#define REPLY_MAX_CHARS 120
#define BAR_PADDING 4
#define PERIODIC_SAMPLE_INTERVAL 5.0  // seconds

/* Utilities */
static void die_nc(const char *msg) {
	endwin();
	fprintf(stderr, "%s\n", msg);
	exit(1);
}
static double now_seconds(void) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec * 1e-6;
}
static float randf_small(void) {
	return ((float) rand() / RAND_MAX - 0.5f) * 0.1f;
}

/* File I/O */
static char* load_text_file(const char *path, long *out_size) {
	FILE *f = fopen(path, "rb");
	if (!f)
		return NULL;
	if (fseek(f, 0, SEEK_END) != 0) {
		fclose(f);
		return NULL;
	}
	long fsize = ftell(f);
	if (fsize < 0) {
		fclose(f);
		return NULL;
	}
	rewind(f);
	char *buf = malloc((size_t) fsize + 1);
	if (!buf) {
		fclose(f);
		return NULL;
	}
	if (fread(buf, 1, (size_t) fsize, f) != (size_t) fsize) {
		free(buf);
		fclose(f);
		return NULL;
	}
	buf[fsize] = '\0';
	fclose(f);
	*out_size = fsize;
	return buf;
}

/* Sentence starts */
static long* collect_sentence_starts(const char *text, long text_len,
		int *out_count) {
	long *starts = malloc(sizeof(long) * (text_len / 10 + 16));
	if (!starts)
		return NULL;
	int count = 0;
	long i = 0;
	while (i < text_len
			&& (text[i] == ' ' || text[i] == '\n' || text[i] == '\r'
					|| text[i] == '\t'))
		++i;
	if (i < text_len)
		starts[count++] = i;
	for (; i < text_len; ++i) {
		char c = text[i];
		if (c == '.' || c == '?' || c == '!') {
			long j = i + 1;
			while (j < text_len
					&& (text[j] == '"' || text[j] == '\'' || text[j] == ')'
							|| text[j] == ' ' || text[j] == '\n'
							|| text[j] == '\r' || text[j] == '\t'))
				++j;
			if (j < text_len)
				starts[count++] = j;
		}
	}
	*out_count = count;
	return starts;
}

/* Model sizing */
static void compute_dims_from_budget(size_t target_bytes, int *out_in,
		int *out_hid, int *out_out) {
	const int in_dim = ASCII_VOCAB, out_dim = ASCII_VOCAB;
	const size_t bytes_per_param = sizeof(float);
	long long target_params = (long long) (target_bytes / bytes_per_param);
	long long denom = (long long) in_dim + (long long) out_dim + 1LL;
	long long hid = 0;
	if (target_params > out_dim)
		hid = (target_params - out_dim) / denom;
	if (hid < MIN_HID)
		hid = MIN_HID;
	if (hid > (1LL << 20))
		hid = (1LL << 20);
	*out_in = in_dim;
	*out_hid = (int) hid;
	*out_out = out_dim;
}

/* ggml init */
static struct ggml_context* init_ggml_context_for_model(long long params_bytes) {
	size_t mem_size = (size_t) params_bytes + (size_t) (16 * 1024 * 1024);
	if (mem_size < MIN_POOL_BYTES)
		mem_size = MIN_POOL_BYTES;
	struct ggml_init_params ip = { .mem_size = mem_size, .mem_buffer = NULL };
	return ggml_init(ip);
}

/* Model struct */
struct model_t {
	int in_dim, hid_dim, out_dim;
	struct ggml_tensor *tW1, *tb1, *tW2, *tb2;
	float *W1, *b1, *W2, *b2;
};

static int create_model_params(struct ggml_context *ctx, struct model_t *m) {
	int ne2_W1[2] = { m->in_dim, m->hid_dim };
	m->tW1 = ggml_new_tensor(ctx, GGML_TYPE_F32, 2, ne2_W1);
	m->tb1 = ggml_new_tensor_1d(ctx, GGML_TYPE_F32, m->hid_dim);
	int ne2_W2[2] = { m->hid_dim, m->out_dim };
	m->tW2 = ggml_new_tensor(ctx, GGML_TYPE_F32, 2, ne2_W2);
	m->tb2 = ggml_new_tensor_1d(ctx, GGML_TYPE_F32, m->out_dim);
	if (!m->tW1 || !m->tb1 || !m->tW2 || !m->tb2)
		return 0;
	m->W1 = ggml_get_data_f32(m->tW1);
	m->b1 = ggml_get_data_f32(m->tb1);
	m->W2 = ggml_get_data_f32(m->tW2);
	m->b2 = ggml_get_data_f32(m->tb2);
	if (!m->W1 || !m->b1 || !m->W2 || !m->b2)
		return 0;
	for (long i = 0; i < (long) m->in_dim * m->hid_dim; ++i)
		m->W1[i] = randf_small();
	for (int i = 0; i < m->hid_dim; ++i)
		m->b1[i] = 0.0f;
	for (long i = 0; i < (long) m->hid_dim * m->out_dim; ++i)
		m->W2[i] = randf_small();
	for (int i = 0; i < m->out_dim; ++i)
		m->b2[i] = 0.0f;
	return 1;
}

/* Save model */
static void save_model_bin(const char *out, int in_dim, int hid, int out_dim,
		const float *W1, const float *b1, const float *W2, const float *b2) {
	FILE *f = fopen(out, "wb");
	if (!f)
		return;
	uint32_t magic = MAGIC;
	fwrite(&magic, sizeof(magic), 1, f);
	fwrite(&in_dim, sizeof(int), 1, f);
	fwrite(&hid, sizeof(int), 1, f);
	fwrite(&out_dim, sizeof(int), 1, f);
	fwrite(W1, sizeof(float), (size_t) in_dim * hid, f);
	fwrite(b1, sizeof(float), (size_t) hid, f);
	fwrite(W2, sizeof(float), (size_t) hid * out_dim, f);
	fwrite(b2, sizeof(float), (size_t) out_dim, f);
	fclose(f);
}

/* Sampling */
static int sample_next_char_from_logits(const float *logits, int vocab,
		float temperature) {
	float maxv = logits[0];
	for (int i = 1; i < vocab; ++i)
		if (logits[i] > maxv)
			maxv = logits[i];
	double sum = 0.0;
	double probs[ASCII_VOCAB];
	for (int i = 0; i < vocab; ++i) {
		double v = exp((double) (logits[i] - maxv) / temperature);
		probs[i] = v;
		sum += v;
	}
	double r = ((double) rand() / RAND_MAX) * sum;
	double c = 0.0;
	for (int i = 0; i < vocab; ++i) {
		c += probs[i];
		if (r <= c)
			return i;
	}
	return vocab - 1;
}

/* Generate reply */
static void generate_reply(const struct model_t *m, int start_char, int gen_len,
		char *out_buf, float temperature) {
	int last = start_char;
	float *h = malloc((size_t) m->hid_dim * sizeof(float));
	float *o = malloc((size_t) m->out_dim * sizeof(float));
	if (!h || !o) {
		free(h);
		free(o);
		return;
	}
	for (int t = 0; t < gen_len; ++t) {
		for (int j = 0; j < m->hid_dim; ++j)
			h[j] = m->b1[j];
		for (int i = 0; i < m->in_dim; ++i)
			if ((i % ASCII_VOCAB) == last) {
				long base = (long) i * m->hid_dim;
				for (int j = 0; j < m->hid_dim; ++j)
					h[j] += m->W1[base + j];
			}
		for (int j = 0; j < m->hid_dim; ++j)
			h[j] = tanhf(h[j]);
		for (int k = 0; k < m->out_dim; ++k)
			o[k] = m->b2[k];
		for (int j = 0; j < m->hid_dim; ++j) {
			long base = (long) j * m->out_dim;
			float hj = h[j];
			for (int k = 0; k < m->out_dim; ++k)
				o[k] += m->W2[base + k] * hj;
		}
		int next = sample_next_char_from_logits(o, m->out_dim, temperature);
		out_buf[t] = (char) next;
		last = next;
	}
	free(h);
	free(o);
}

/* Evaluate chat samples and append to log window */
static void evaluate_chat_samples_ncurses(WINDOW *logwin, struct model_t *m,
		const char *text, long text_len, long *sent_starts, int sent_count,
		int n_samples) {
	if (!sent_starts || sent_count < 2) {
		wprintw(logwin, "Not enough sentences for chat evaluation\n");
		wrefresh(logwin);
		return;
	}
	for (int s = 0; s < n_samples; ++s) {
		int idx = rand() % (sent_count - 1);
		long ppos = sent_starts[idx];
		long next = sent_starts[idx + 1];
		int plen = (int) (next - ppos);
		if (plen <= 0)
			continue;
		int use_plen = plen > PROMPT_MAX_CHARS ? PROMPT_MAX_CHARS : plen;
		char *prompt = malloc(use_plen + 1);
		if (!prompt)
			break;
		memcpy(prompt, text + ppos, (size_t) use_plen);
		prompt[use_plen] = 0;
		int gt_len = (int) (
				(idx + 2 < sent_count) ?
						(sent_starts[idx + 2] - next) : (text_len - next));
		if (gt_len <= 0) {
			free(prompt);
			continue;
		}
		int use_gt = gt_len > REPLY_MAX_CHARS ? REPLY_MAX_CHARS : gt_len;
		char *gt = malloc(use_gt + 1);
		if (!gt) {
			free(prompt);
			break;
		}
		memcpy(gt, text + next, (size_t) use_gt);
		gt[use_gt] = 0;
		int start_char = 0;
		if (use_plen > 0) {
			unsigned char c = (unsigned char) prompt[use_plen - 1];
			start_char = (c < ASCII_VOCAB) ? (int) c : 0;
		}
		char *gen = malloc(use_gt + 1);
		if (!gen) {
			free(prompt);
			free(gt);
			break;
		}
		generate_reply(m, start_char, use_gt, gen, 1.0f);
		gen[use_gt] = 0;
		int correct = 0;
		for (int i = 0; i < use_gt; ++i) {
			unsigned char gc = (unsigned char) gen[i];
			unsigned char tc = (unsigned char) gt[i];
			if (gc == tc)
				++correct;
		}
		double acc = (double) correct / (double) use_gt;
		wprintw(logwin, "=== Sample %d ===\n", s + 1);
		wprintw(logwin, "Prompt: \"");
		for (int i = 0; i < use_plen; ++i) {
			unsigned char c = (unsigned char) prompt[i];
			waddch(logwin, (c >= 32 && c < 127) ? (char) c : '.');
		}
		wprintw(logwin, "\"\nGenerated: \"");
		for (int i = 0; i < use_gt; ++i) {
			unsigned char c = (unsigned char) gen[i];
			waddch(logwin, (c >= 32 && c < 127) ? (char) c : '.');
		}
		wprintw(logwin, "\"\nGround-truth: \"");
		for (int i = 0; i < use_gt; ++i) {
			unsigned char c = (unsigned char) gt[i];
			waddch(logwin, (c >= 32 && c < 127) ? (char) c : '.');
		}
		wprintw(logwin, "\"\n  acc=%.3f\n\n", acc);
		wrefresh(logwin);
		free(prompt);
		free(gt);
		free(gen);
	}
}

/* Periodic single sample and log (every PERIODIC_SAMPLE_INTERVAL seconds) */
static void periodic_sample_and_log(WINDOW *logwin, struct model_t *m,
		const char *text, long text_len, long *sent_starts, int sent_count) {
	if (!sent_starts || sent_count < 2)
		return;
	int idx = rand() % (sent_count - 1);
	long ppos = sent_starts[idx];
	long next = sent_starts[idx + 1];
	int plen = (int) (next - ppos);
	if (plen <= 0)
		return;
	int use_plen = plen > PROMPT_MAX_CHARS ? PROMPT_MAX_CHARS : plen;
	char *prompt = malloc(use_plen + 1);
	if (!prompt)
		return;
	memcpy(prompt, text + ppos, (size_t) use_plen);
	prompt[use_plen] = 0;
	int gt_len = (int) (
			(idx + 2 < sent_count) ?
					(sent_starts[idx + 2] - next) : (text_len - next));
	int use_gt = gt_len > REPLY_MAX_CHARS ? REPLY_MAX_CHARS : gt_len;
	if (use_gt <= 0) {
		free(prompt);
		return;
	}
	char *gen = malloc(use_gt + 1);
	if (!gen) {
		free(prompt);
		return;
	}
	int start_char = 0;
	if (use_plen > 0) {
		unsigned char c = (unsigned char) prompt[use_plen - 1];
		start_char = (c < ASCII_VOCAB) ? (int) c : 0;
	}
	generate_reply(m, start_char, use_gt, gen, 1.0f);
	gen[use_gt] = 0;
	// log input request and output answer
	wprintw(logwin, "[Periodic sample] Prompt: \"");
	for (int i = 0; i < use_plen; ++i) {
		unsigned char c = (unsigned char) prompt[i];
		waddch(logwin, (c >= 32 && c < 127) ? (char) c : '.');
	}
	wprintw(logwin, "\"\n[Periodic sample] Answer: \"");
	for (int i = 0; i < use_gt; ++i) {
		unsigned char c = (unsigned char) gen[i];
		waddch(logwin, (c >= 32 && c < 127) ? (char) c : '.');
	}
	wprintw(logwin, "\"\n\n");
	wrefresh(logwin);
	free(prompt);
	free(gen);
}

/* Training with ncurses UI (two bars + log) and periodic sampling */
static void train_with_ncurses(struct model_t *m, const int *data,
		long data_len, int epochs, float lr, const char *text, long text_len,
		long *sent_starts, int sent_count) {
	int rows, cols;
	getmaxyx(stdscr, rows, cols);
	int bar_height = 5; // header + two bars + spacing
	int log_height = rows - bar_height - 1;
	if (log_height < 6)
		log_height = 6;
	WINDOW *barwin = newwin(bar_height, cols, 0, 0);
	WINDOW *logwin = newwin(log_height, cols, bar_height, 0);
	scrollok(logwin, TRUE);
	box(barwin, 0, 0);
	mvwprintw(barwin, 0, 2, " Training Progress ");
	wrefresh(barwin);

	float *h = malloc((size_t) m->hid_dim * sizeof(float));
	float *o = malloc((size_t) m->out_dim * sizeof(float));
	float *do_grad = malloc((size_t) m->out_dim * sizeof(float));
	float *dh = malloc((size_t) m->hid_dim * sizeof(float));
	if (!h || !o || !do_grad || !dh)
		die_nc("Out of memory for training buffers");

	const int update_every = 200;
	double t_start_all = now_seconds();
	double last_periodic = now_seconds();

	for (int ep = 0; ep < epochs; ++ep) {
		double t_epoch_start = now_seconds();
		long steps = data_len > 1 ? data_len - 1 : 0;
		double loss = 0.0;
		for (long t = 0; t < steps; ++t) {
			int x = data[t], y = data[t + 1];
			// forward
			for (int j = 0; j < m->hid_dim; ++j)
				h[j] = m->b1[j];
			for (int i = 0; i < m->in_dim; ++i)
				if ((i % ASCII_VOCAB) == x) {
					long base = (long) i * m->hid_dim;
					for (int j = 0; j < m->hid_dim; ++j)
						h[j] += m->W1[base + j];
				}
			for (int j = 0; j < m->hid_dim; ++j)
				h[j] = tanhf(h[j]);
			for (int k = 0; k < m->out_dim; ++k)
				o[k] = m->b2[k];
			for (int j = 0; j < m->hid_dim; ++j) {
				long base = (long) j * m->out_dim;
				float hj = h[j];
				for (int k = 0; k < m->out_dim; ++k)
					o[k] += m->W2[base + k] * hj;
			}
			// softmax
			float maxv = o[0];
			for (int k = 1; k < m->out_dim; ++k)
				if (o[k] > maxv)
					maxv = o[k];
			double sum = 0.0;
			for (int k = 0; k < m->out_dim; ++k) {
				o[k] = expf(o[k] - maxv);
				sum += o[k];
			}
			for (int k = 0; k < m->out_dim; ++k)
				o[k] /= (float) sum;
			loss += -logf(o[y] + 1e-9f);
			// backprop
			for (int k = 0; k < m->out_dim; ++k)
				do_grad[k] = o[k];
			do_grad[y] -= 1.0f;
			for (int k = 0; k < m->out_dim; ++k)
				m->b2[k] -= lr * do_grad[k];
			for (int j = 0; j < m->hid_dim; ++j) {
				long base = (long) j * m->out_dim;
				float hj = h[j];
				for (int k = 0; k < m->out_dim; ++k) {
					float g = do_grad[k] * hj;
					m->W2[base + k] -= lr * g;
				}
			}
			for (int j = 0; j < m->hid_dim; ++j) {
				float s = 0.0f;
				long base = (long) j * m->out_dim;
				for (int k = 0; k < m->out_dim; ++k)
					s += m->W2[base + k] * do_grad[k];
				dh[j] = (1.0f - h[j] * h[j]) * s;
			}
			for (int j = 0; j < m->hid_dim; ++j)
				m->b1[j] -= lr * dh[j];
			for (int i = 0; i < m->in_dim; ++i)
				if ((i % ASCII_VOCAB) == x) {
					long base = (long) i * m->hid_dim;
					for (int j = 0; j < m->hid_dim; ++j)
						m->W1[base + j] -= lr * dh[j];
				}

			// periodic sampling every PERIODIC_SAMPLE_INTERVAL seconds
			double now = now_seconds();
			if (now - last_periodic >= PERIODIC_SAMPLE_INTERVAL) {
				periodic_sample_and_log(logwin, m, text, text_len, sent_starts,
						sent_count);
				last_periodic = now;
			}

			if ((t % update_every) == 0 || t == steps - 1) {
				double nowu = now_seconds();
				double epoch_elapsed = nowu - t_epoch_start;
				double frac_epoch =
						steps > 0 ? (double) (t + 1) / (double) steps : 1.0;
				double est_total_epoch = epoch_elapsed / frac_epoch;
				double eta_epoch = est_total_epoch - epoch_elapsed;
				double overall_elapsed = nowu - t_start_all;
				double overall_frac = (double) (ep + frac_epoch)
						/ (double) epochs;
				double est_total_all = overall_elapsed / overall_frac;
				double eta_all = est_total_all - overall_elapsed;

				// draw two bars in barwin
				werase(barwin);
				box(barwin, 0, 0);
				mvwprintw(barwin, 0, 2, " Training Progress ");
				int inner_w = cols - BAR_PADDING;
				if (inner_w < 10)
					inner_w = 10;
				// Epoch bar (line 1)
				mvwprintw(barwin, 1, 2, "Epoch %d/%d:", ep + 1, epochs);
				int filled_epoch = (int) round(frac_epoch * inner_w);
				mvwprintw(barwin, 1, 16, "[");
				for (int i = 0; i < filled_epoch; ++i)
					waddch(barwin, '=');
				if (filled_epoch < inner_w)
					waddch(barwin, '>');
				for (int i = filled_epoch + 1; i < inner_w; ++i)
					waddch(barwin, ' ');
				waddch(barwin, ']');
				mvwprintw(barwin, 2, 2, "ETA epoch: %5.1fs", eta_epoch);
				// Overall bar (line 3)
				double frac_overall = (double) (ep + frac_epoch)
						/ (double) epochs;
				mvwprintw(barwin, 3, 2, "Overall:");
				int filled_over = (int) round(frac_overall * inner_w);
				mvwprintw(barwin, 3, 12, "[");
				for (int i = 0; i < filled_over; ++i)
					waddch(barwin, '=');
				if (filled_over < inner_w)
					waddch(barwin, '>');
				for (int i = filled_over + 1; i < inner_w; ++i)
					waddch(barwin, ' ');
				waddch(barwin, ']');
				mvwprintw(barwin, 3, 16 + inner_w + 2, "ETA total: %5.1fs",
						eta_all);
				wrefresh(barwin);
			}
		} // steps

		double t_epoch_end = now_seconds();
		double epoch_time = t_epoch_end - t_epoch_start;
		double avg_loss = loss / (steps > 0 ? steps : 1);

		char fname[256];
		snprintf(fname, sizeof(fname), "model_epoch_%d.bin", ep + 1);
		save_model_bin(fname, m->in_dim, m->hid_dim, m->out_dim, m->W1, m->b1,
				m->W2, m->b2);
		wprintw(logwin,
				"Epoch %d/%d finished: time=%.2fs avg_loss=%.6f\nSaved %s\n",
				ep + 1, epochs, epoch_time, avg_loss, fname);
		wrefresh(logwin);

		evaluate_chat_samples_ncurses(logwin, m, text, text_len, sent_starts,
				sent_count, EVAL_SAMPLES);
	}

	save_model_bin("model.bin", m->in_dim, m->hid_dim, m->out_dim, m->W1, m->b1,
			m->W2, m->b2);
	wprintw(logwin, "Saved model.bin (final)\n");
	wrefresh(logwin);

	free(h);
	free(o);
	free(do_grad);
	free(dh);
	delwin(barwin);
	delwin(logwin);
}

/* main (small) */
int main(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "Usage: %s data.txt model_size_MBytes\n", argv[0]);
		return 1;
	}
	const char *path = argv[1];
	double model_mb = atof(argv[2]);
	if (model_mb <= 0.0) {
		fprintf(stderr, "Invalid model size\n");
		return 1;
	}
	long text_len = 0;
	char *text = load_text_file(path, &text_len);
	if (!text) {
		fprintf(stderr, "Failed to read file\n");
		return 1;
	}
	size_t target_bytes = (size_t) llround(model_mb * 1024.0 * 1024.0);
	int in_dim = 0, hid_dim = 0, out_dim = 0;
	compute_dims_from_budget(target_bytes, &in_dim, &hid_dim, &out_dim);
	long long params = (long long) in_dim * hid_dim
			+ (long long) hid_dim * out_dim + hid_dim + out_dim;
	long long bytes = params * (long long) sizeof(float);
	int *data = malloc((size_t) text_len * sizeof(int));
	if (!data) {
		free(text);
		fprintf(stderr, "malloc failed\n");
		return 1;
	}
	for (long i = 0; i < text_len; ++i) {
		unsigned char c = (unsigned char) text[i];
		data[i] = (c < ASCII_VOCAB) ? (int) c : 0;
	}
	int sent_count = 0;
	long *sent_starts = collect_sentence_starts(text, text_len, &sent_count);
	struct ggml_context *ctx = init_ggml_context_for_model(bytes);
	if (!ctx) {
		free(text);
		free(data);
		free(sent_starts);
		fprintf(stderr, "ggml_init failed\n");
		return 1;
	}
	struct model_t model;
	model.in_dim = in_dim;
	model.hid_dim = hid_dim;
	model.out_dim = out_dim;
	if (!create_model_params(ctx, &model)) {
		ggml_free(ctx);
		free(text);
		free(data);
		free(sent_starts);
		fprintf(stderr, "create_model_params failed\n");
		return 1;
	}

	// init ncurses
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	mvprintw(0, 0,
			"Trainer UI - two progress bars (top) and chat log below. Press Ctrl+C to quit.");
	mvprintw(1, 0, "Model dims: in=%d hid=%d out=%d  Params=%lld  Bytes=%lld",
			in_dim, hid_dim, out_dim, params, bytes);
	refresh();

	srand((unsigned) time(NULL));
	train_with_ncurses(&model, data, text_len, DEFAULT_EPOCHS, DEFAULT_LR, text,
			text_len, sent_starts, sent_count);

	mvprintw(LINES - 1, 0, "Training complete. Press any key to exit.");
	getch();
	endwin();

	free(text);
	free(data);
	free(sent_starts);
	ggml_free(ctx);
	return 0;
}
