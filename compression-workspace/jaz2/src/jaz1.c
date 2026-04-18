/*
 * az.c
 *
 * Archive tool with ASCII-friendly LZ77 prototype compressor and enhanced progress UI.
 * - Terminal-width adaptive progress bar
 * - Two-line status: progress bar + filename / stats line
 * - ETA, remaining time, countdown, MB/s, per-file and average compression ratio
 * - Error counter and last error filename/message
 *
 * Build:
 *   gcc -O2 -std=c11 -o az az.c
 *
 * Usage:
 *   Compress:   ./az -c -i <input-file-or-dir> -o <archive.az>
 *   Decompress: ./az -d -i <archive.az> -o <out-dir>
 *
 * Notes:
 * - Prototype quality; not production hardened.
 * - Designed to be readable and easy to extend.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <inttypes.h>
#include <ctype.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/types.h>

/* ---------------------------
   Helpers
   --------------------------- */

static void *xmalloc(size_t n) {
    void *p = malloc(n);
    if (!p) { fprintf(stderr, "malloc failed\n"); exit(1); }
    return p;
}
static void *xrealloc(void *p, size_t n) {
    void *q = realloc(p, n);
    if (!q) { fprintf(stderr, "realloc failed\n"); exit(1); }
    return q;
}

static double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static void format_time_hms(double seconds, char *buf, size_t buflen) {
    if (seconds < 0) seconds = 0;
    int s = (int)(seconds + 0.5);
    int h = s / 3600;
    int m = (s % 3600) / 60;
    int sec = s % 60;
    if (h > 99) {
        snprintf(buf, buflen, "%02dh", h);
    } else if (h > 0) {
        snprintf(buf, buflen, "%02d:%02d:%02d", h, m, sec);
    } else {
        snprintf(buf, buflen, "%02d:%02d", m, sec);
    }
}

static const char *human_unit(double v, double *outval) {
    static const char *units[] = {"B","KB","MB","GB","TB"};
    int u = 0;
    while (v >= 1024.0 && u < 4) { v /= 1024.0; u++; }
    *outval = v;
    return units[u];
}

static int get_terminal_width(void) {
    struct winsize w;
    if (ioctl(STDERR_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0) return (int)w.ws_col;
    return 80;
}

/* ---------------------------
   Global progress / error state
   --------------------------- */

typedef struct {
    uint64_t total_bytes;        /* total input bytes to process (packing) or archive size (unpacking) */
    uint64_t processed_bytes;    /* bytes processed so far */
    double start_time;
    uint64_t total_input_files;  /* count of files (optional) */
    uint64_t processed_files;
    uint64_t total_input_bytes_for_ratio; /* sum of original sizes */
    uint64_t total_compressed_bytes_for_ratio; /* sum of compressed sizes */
    uint64_t error_count;
    char last_error_file[512];
    char last_error_msg[256];
    char current_file[512];
} prog_state_t;

static void prog_init(prog_state_t *ps) {
    memset(ps, 0, sizeof(*ps));
    ps->start_time = now_seconds();
}

/* print two-line progress block to stderr and keep cursor above it for updates.
   When finished==1, the function prints final lines and does not move cursor up. */
static void print_progress_block(prog_state_t *ps, const char *phase, int finished) {
    int termw = get_terminal_width();
    if (termw < 40) termw = 40;
    /* reserve second line for filename and stats; first line is bar */
    int bar_width = termw - 40; /* leave space for percent and sizes */
    if (bar_width < 10) bar_width = 10;

    double now = now_seconds();
    double elapsed = now - ps->start_time;
    double rate = (elapsed > 0.0001) ? (double)ps->processed_bytes / elapsed : 0.0;
    double remaining = (rate > 0.0001 && ps->total_bytes > ps->processed_bytes) ? ((double)(ps->total_bytes - ps->processed_bytes) / rate) : -1.0;

    double pfrac = (ps->total_bytes > 0) ? (double)ps->processed_bytes / (double)ps->total_bytes : 0.0;
    if (pfrac < 0) pfrac = 0;
    if (pfrac > 1) pfrac = 1;
    int filled = (int)(pfrac * bar_width + 0.5);

    char elapsed_s[32], remain_s[32];
    format_time_hms(elapsed, elapsed_s, sizeof(elapsed_s));
    if (remaining >= 0.0) format_time_hms(remaining, remain_s, sizeof(remain_s));
    else snprintf(remain_s, sizeof(remain_s), "--:--");

    double pval, tval;
    const char *punit = human_unit((double)ps->processed_bytes, &pval);
    const char *tunit = human_unit((double)ps->total_bytes, &tval);

    /* compute MB/s and average compression ratio */
    double mbps = rate / (1024.0 * 1024.0);
    double avg_ratio = 0.0;
    if (ps->total_compressed_bytes_for_ratio > 0) {
        avg_ratio = (double)ps->total_input_bytes_for_ratio / (double)ps->total_compressed_bytes_for_ratio;
    }

    /* per-file compression ratio if available (we can compute from totals or show N/A) */
    char ratio_s[64];
    if (ps->processed_files > 0 && ps->total_compressed_bytes_for_ratio > 0) {
        snprintf(ratio_s, sizeof(ratio_s), "avg ratio: %.2fx", avg_ratio);
    } else {
        snprintf(ratio_s, sizeof(ratio_s), "avg ratio: N/A");
    }

    /* prepare first line: [phase] [bar] XX% processed/total MB Elapsed Remain ETA */
    char bar[1024];
    int i;
    for (i = 0; i < filled && i < (int)sizeof(bar)-1; ++i) bar[i] = '#';
    for (; i < bar_width && i < (int)sizeof(bar)-1; ++i) bar[i] = '-';
    bar[i] = 0;

    /* percent */
    int percent = (int)(pfrac * 100.0 + 0.5);

    /* build first and second lines */
    char line1[2048];
    snprintf(line1, sizeof(line1), "%s [%s] %3d%% %6.2f%s/%6.2f%s Elapsed:%s Remain:%s",
             phase, bar, percent, pval, punit, tval, tunit, elapsed_s, remain_s);

    /* second line: filename, MB/s, files processed, errors, last error */
    char line2[2048];
    const char *curfile = ps->current_file[0] ? ps->current_file : "(none)";
    snprintf(line2, sizeof(line2),
             "File: %s | %6.2f MB/s | files: %" PRIu64 "/%" PRIu64 " | %s | errors: %" PRIu64 " last: %s %s",
             curfile, mbps, ps->processed_files, ps->total_input_files, ratio_s, ps->error_count,
             ps->last_error_file[0] ? ps->last_error_file : "(none)",
             ps->last_error_msg[0] ? ps->last_error_msg : "");

    /* print: clear two lines and write new ones. To update in-place, we print and then move cursor up two lines
       so next update overwrites. When finished, we leave the cursor after the block. */
    /* Clear current line and print line1 */
    fprintf(stderr, "\r\033[K%s\n", line1);
    /* Clear next line and print line2 */
    fprintf(stderr, "\r\033[K%s\n", line2);
    fflush(stderr);

    if (!finished) {
        /* move cursor up two lines so next print overwrites them */
        fprintf(stderr, "\033[2A");
        fflush(stderr);
    }
}

/* ---------------------------
   Simple LZ77-like compressor (prototype)
   --------------------------- */

#define LZ_HASH_SIZE (1 << 16)
#define LZ_MIN_MATCH 4
#define LZ_MAX_MATCH 255 + LZ_MIN_MATCH

static inline uint32_t lz_hash(const uint8_t *p) {
    return ((uint32_t)p[0] * 2654435761u ^ (uint32_t)p[1] * 97531u ^ (uint32_t)p[2]) & (LZ_HASH_SIZE - 1);
}

/* Encode format (stream of chunks):
   For each chunk:
     uint8 literal_len (0..255)
     literal bytes (literal_len)
     if literal_len != 0: uint8 flag (0)
     if literal_len == 0: uint16 offset (big-endian), uint8 match_len_minus_min
*/
static void lz77_encode(const uint8_t *in, size_t inlen, uint8_t **out, size_t *outlen) {
    uint32_t *table = xmalloc(sizeof(uint32_t) * LZ_HASH_SIZE);
    for (size_t i = 0; i < LZ_HASH_SIZE; ++i) table[i] = UINT32_MAX;

    size_t ip = 0;
    size_t cap = inlen + 1024;
    uint8_t *obuf = xmalloc(cap);
    size_t op = 0;

    while (ip < inlen) {
        size_t best_len = 0;
        size_t best_off = 0;
        if (ip + 3 < inlen) {
            uint32_t h = lz_hash(in + ip);
            uint32_t prev = table[h];
            if (prev != UINT32_MAX && prev < ip) {
                size_t p = prev;
                size_t maxlen = 0;
                while (ip + maxlen < inlen && p + maxlen < inlen && in[ip + maxlen] == in[p + maxlen] && maxlen < LZ_MAX_MATCH) maxlen++;
                if (maxlen >= LZ_MIN_MATCH) {
                    best_len = maxlen;
                    best_off = ip - p;
                }
            }
            table[h] = (uint32_t)ip;
        }

        if (best_len >= LZ_MIN_MATCH) {
            if (op + 1 + 2 + 1 >= cap) { cap = (cap + 4096) * 2; obuf = xrealloc(obuf, cap); }
            obuf[op++] = 0;
            obuf[op++] = (uint8_t)((best_off >> 8) & 0xFF);
            obuf[op++] = (uint8_t)(best_off & 0xFF);
            obuf[op++] = (uint8_t)(best_len - LZ_MIN_MATCH);
            ip += best_len;
        } else {
            size_t lit_start = ip;
            size_t lit_len = 0;
            while (ip < inlen && lit_len < 255) {
                if (ip + 3 < inlen) {
                    uint32_t hh = lz_hash(in + ip);
                    table[hh] = (uint32_t)ip;
                }
                ip++; lit_len++;
                if (ip + 3 < inlen) {
                    uint32_t hh = lz_hash(in + ip);
                    uint32_t prev = table[hh];
                    if (prev != UINT32_MAX && prev < ip) {
                        size_t p = prev;
                        size_t maxlen = 0;
                        while (ip + maxlen < inlen && p + maxlen < inlen && in[ip + maxlen] == in[p + maxlen] && maxlen < LZ_MIN_MATCH) maxlen++;
                        if (maxlen >= LZ_MIN_MATCH) break;
                    }
                }
            }
            if (op + 1 + lit_len + 1 >= cap) { cap = (cap + lit_len + 4096) * 2; obuf = xrealloc(obuf, cap); }
            obuf[op++] = (uint8_t)lit_len;
            memcpy(obuf + op, in + lit_start, lit_len); op += lit_len;
            obuf[op++] = 0;
        }
    }

    free(table);
    *out = obuf;
    *outlen = op;
}

static int lz77_decode(const uint8_t *in, size_t inlen, uint8_t **out, size_t *outlen) {
    size_t ip = 0;
    size_t cap = inlen * 3 + 1024;
    uint8_t *obuf = xmalloc(cap);
    size_t op = 0;

    while (ip < inlen) {
        uint8_t litlen = in[ip++];
        if (ip + litlen > inlen) { free(obuf); return -1; }
        if (op + litlen > cap) { cap = (op + litlen) * 2; obuf = xrealloc(obuf, cap); }
        memcpy(obuf + op, in + ip, litlen); ip += litlen; op += litlen;
        if (ip >= inlen) break;
        if (litlen != 0) {
            uint8_t flag = in[ip++];
            (void)flag;
            continue;
        } else {
            if (ip + 3 > inlen) { free(obuf); return -1; }
            uint32_t off = ((uint32_t)in[ip] << 8) | (uint32_t)in[ip+1];
            uint8_t lenb = in[ip+2];
            ip += 3;
            size_t matchlen = (size_t)lenb + LZ_MIN_MATCH;
            if (off == 0 || off > op) { free(obuf); return -1; }
            if (op + matchlen > cap) { cap = (op + matchlen) * 2; obuf = xrealloc(obuf, cap); }
            size_t src = op - off;
            for (size_t k = 0; k < matchlen; ++k) obuf[op++] = obuf[src + k];
        }
    }

    *out = obuf;
    *outlen = op;
    return 0;
}

/* ---------------------------
   Archive helpers
   --------------------------- */

static void write_u32(FILE *f, uint32_t v) {
    uint8_t b[4];
    b[0] = (v >> 24) & 0xFF;
    b[1] = (v >> 16) & 0xFF;
    b[2] = (v >> 8) & 0xFF;
    b[3] = v & 0xFF;
    fwrite(b, 1, 4, f);
}
static void write_u64(FILE *f, uint64_t v) {
    uint8_t b[8];
    for (int i = 0; i < 8; ++i) b[7-i] = (v >> (i*8)) & 0xFF;
    fwrite(b, 1, 8, f);
}
static int read_u32(FILE *f, uint32_t *out) {
    uint8_t b[4];
    if (fread(b,1,4,f) != 4) return -1;
    *out = ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[2] << 8) | (uint32_t)b[3];
    return 0;
}
static int read_u64(FILE *f, uint64_t *out) {
    uint8_t b[8];
    if (fread(b,1,8,f) != 8) return -1;
    uint64_t v = 0;
    for (int i = 0; i < 8; ++i) v = (v << 8) | b[i];
    *out = v;
    return 0;
}

/* ---------------------------
   Pre-scan to compute totals
   --------------------------- */

static uint64_t compute_total_input_size_and_count(const char *path, uint64_t *file_count) {
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    if (S_ISDIR(st.st_mode)) {
        uint64_t total = 0;
        DIR *d = opendir(path);
        if (!d) return 0;
        struct dirent *ent;
        while ((ent = readdir(d)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            char *child = NULL;
            size_t la = strlen(path), lb = strlen(ent->d_name);
            int need_sep = (la > 0 && path[la-1] != '/');
            child = xmalloc(la + (need_sep?1:0) + lb + 1);
            strcpy(child, path);
            if (need_sep) strcat(child, "/");
            strcat(child, ent->d_name);
            total += compute_total_input_size_and_count(child, file_count);
            free(child);
        }
        closedir(d);
        return total;
    } else if (S_ISREG(st.st_mode)) {
        if (file_count) (*file_count)++;
        return (uint64_t)st.st_size;
    } else {
        return 0;
    }
}

/* ---------------------------
   Packing with progress and error tracking
   --------------------------- */

static int ensure_dir_exists(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) return 0;
        errno = ENOTDIR;
        return -1;
    }
    char *tmp = strdup(path);
    if (!tmp) return -1;
    for (char *p = tmp + 1; *p; ++p) {
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, 0755);
            *p = '/';
        }
    }
    int r = mkdir(tmp, 0755);
    free(tmp);
    if (r == 0 || errno == EEXIST) return 0;
    return -1;
}

static char *join_path(const char *a, const char *b) {
    size_t la = strlen(a), lb = strlen(b);
    int need_sep = (la > 0 && a[la-1] != '/');
    size_t total = la + (need_sep ? 1 : 0) + lb + 1;
    char *out = xmalloc(total);
    strcpy(out, a);
    if (need_sep) strcat(out, "/");
    strcat(out, b);
    return out;
}

static size_t read_file_to_buf(const char *path, uint8_t **out) {
    FILE *f = fopen(path, "rb");
    if (!f) { return 0; }
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return 0; }
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return 0; }
    rewind(f);
    uint8_t *buf = xmalloc((size_t)sz + 1);
    size_t r = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    *out = buf;
    return r;
}

static int pack_file_to_stream(FILE *outf, const char *filepath, const char *relpath, prog_state_t *ps) {
    /* update current file */
    strncpy(ps->current_file, relpath, sizeof(ps->current_file)-1);
    ps->current_file[sizeof(ps->current_file)-1] = 0;
    print_progress_block(ps, "Packing", 0);

    uint8_t *buf = NULL;
    size_t inlen = read_file_to_buf(filepath, &buf);
    if (inlen == 0 && errno) {
        ps->error_count++;
        strncpy(ps->last_error_file, relpath, sizeof(ps->last_error_file)-1);
        snprintf(ps->last_error_msg, sizeof(ps->last_error_msg)-1, "read failed: %s", strerror(errno));
        print_progress_block(ps, "Packing", 0);
        return -1;
    }
    uint8_t *comp = NULL;
    size_t complen = 0;
    lz77_encode(buf, inlen, &comp, &complen);

    uint32_t namelen = (uint32_t)strlen(relpath);
    write_u32(outf, namelen);
    fwrite(relpath, 1, namelen, outf);
    write_u64(outf, (uint64_t)inlen);
    write_u64(outf, (uint64_t)complen);
    if (fwrite(comp, 1, complen, outf) != complen) {
        ps->error_count++;
        strncpy(ps->last_error_file, relpath, sizeof(ps->last_error_file)-1);
        snprintf(ps->last_error_msg, sizeof(ps->last_error_msg)-1, "write failed: %s", strerror(errno));
        free(buf); free(comp);
        print_progress_block(ps, "Packing", 0);
        return -1;
    }

    /* update stats */
    ps->processed_bytes += (uint64_t)inlen;
    ps->processed_files++;
    ps->total_input_bytes_for_ratio += (uint64_t)inlen;
    ps->total_compressed_bytes_for_ratio += (uint64_t)complen;

    free(buf);
    free(comp);

    print_progress_block(ps, "Packing", 0);
    return 0;
}

static int pack_path_recursive(FILE *outf, const char *path, const char *relroot, prog_state_t *ps) {
    struct stat st;
    if (stat(path, &st) != 0) {
        ps->error_count++;
        strncpy(ps->last_error_file, path, sizeof(ps->last_error_file)-1);
        snprintf(ps->last_error_msg, sizeof(ps->last_error_msg)-1, "stat failed: %s", strerror(errno));
        print_progress_block(ps, "Packing", 0);
        return -1;
    }
    if (S_ISDIR(st.st_mode)) {
        DIR *d = opendir(path);
        if (!d) {
            ps->error_count++;
            strncpy(ps->last_error_file, path, sizeof(ps->last_error_file)-1);
            snprintf(ps->last_error_msg, sizeof(ps->last_error_msg)-1, "opendir failed: %s", strerror(errno));
            print_progress_block(ps, "Packing", 0);
            return -1;
        }
        struct dirent *ent;
        while ((ent = readdir(d)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            char *child = join_path(path, ent->d_name);
            char *child_rel;
            if (relroot && relroot[0]) child_rel = join_path(relroot, ent->d_name);
            else child_rel = strdup(ent->d_name);
            int r = pack_path_recursive(outf, child, child_rel, ps);
            free(child);
            free(child_rel);
            if (r != 0) { closedir(d); return r; }
        }
        closedir(d);
        return 0;
    } else if (S_ISREG(st.st_mode)) {
        const char *rel = relroot && relroot[0] ? relroot : path;
        return pack_file_to_stream(outf, path, rel, ps);
    } else {
        /* unsupported file type */
        ps->error_count++;
        strncpy(ps->last_error_file, path, sizeof(ps->last_error_file)-1);
        snprintf(ps->last_error_msg, sizeof(ps->last_error_msg)-1, "unsupported file type");
        print_progress_block(ps, "Packing", 0);
        return 0;
    }
}

/* ---------------------------
   Unpacking with progress and error tracking
   --------------------------- */

static int unpack_archive(const char *archive_path, const char *out_dir, prog_state_t *ps) {
    struct stat st;
    if (stat(archive_path, &st) != 0) {
        fprintf(stderr, "stat('%s') failed: %s\n", archive_path, strerror(errno));
        return -1;
    }
    ps->total_bytes = (uint64_t)st.st_size;
    ps->start_time = now_seconds();
    print_progress_block(ps, "Unpacking", 0);

    FILE *f = fopen(archive_path, "rb");
    if (!f) { fprintf(stderr, "open archive failed: %s\n", strerror(errno)); return -1; }

    while (1) {
        uint32_t namelen;
        long pos_before = ftell(f);
        if (read_u32(f, &namelen) != 0) break; /* EOF */
        char *name = xmalloc(namelen + 1);
        if (fread(name, 1, namelen, f) != namelen) { free(name); fclose(f); return -1; }
        name[namelen] = 0;
        uint64_t orig_size, comp_size;
        if (read_u64(f, &orig_size) != 0) { free(name); fclose(f); return -1; }
        if (read_u64(f, &comp_size) != 0) { free(name); fclose(f); return -1; }
        uint8_t *comp = xmalloc((size_t)comp_size);
        if (fread(comp, 1, (size_t)comp_size, f) != comp_size) { free(name); free(comp); fclose(f); return -1; }

        /* update current file */
        strncpy(ps->current_file, name, sizeof(ps->current_file)-1);
        ps->current_file[sizeof(ps->current_file)-1] = 0;
        print_progress_block(ps, "Unpacking", 0);

        /* build output path */
        char *outpath = join_path(out_dir, name);
        char *last_slash = strrchr(outpath, '/');
        if (last_slash) {
            *last_slash = 0;
            ensure_dir_exists(outpath);
            *last_slash = '/';
        } else {
            ensure_dir_exists(out_dir);
        }

        uint8_t *dec = NULL;
        size_t declen = 0;
        if (lz77_decode(comp, (size_t)comp_size, &dec, &declen) != 0) {
            ps->error_count++;
            strncpy(ps->last_error_file, name, sizeof(ps->last_error_file)-1);
            snprintf(ps->last_error_msg, sizeof(ps->last_error_msg)-1, "decompress failed");
            free(name); free(comp); free(outpath);
            print_progress_block(ps, "Unpacking", 0);
            fclose(f);
            return -1;
        }
        FILE *of = fopen(outpath, "wb");
        if (!of) {
            ps->error_count++;
            strncpy(ps->last_error_file, outpath, sizeof(ps->last_error_file)-1);
            snprintf(ps->last_error_msg, sizeof(ps->last_error_msg)-1, "fopen failed: %s", strerror(errno));
            free(name); free(comp); free(dec); free(outpath);
            print_progress_block(ps, "Unpacking", 0);
            fclose(f);
            return -1;
        }
        if (fwrite(dec, 1, declen, of) != declen) {
            ps->error_count++;
            strncpy(ps->last_error_file, outpath, sizeof(ps->last_error_file)-1);
            snprintf(ps->last_error_msg, sizeof(ps->last_error_msg)-1, "write failed: %s", strerror(errno));
            fclose(of); free(name); free(comp); free(dec); free(outpath);
            print_progress_block(ps, "Unpacking", 0);
            fclose(f);
            return -1;
        }
        fclose(of);

        /* update stats */
        long pos_after = ftell(f);
        if (pos_after < 0) pos_after = 0;
        ps->processed_bytes = (uint64_t)pos_after;
        ps->processed_files++;
        ps->total_input_bytes_for_ratio += orig_size;
        ps->total_compressed_bytes_for_ratio += comp_size;

        free(name); free(comp); free(dec); free(outpath);

        print_progress_block(ps, "Unpacking", 0);
    }

    /* final */
    print_progress_block(ps, "Unpacking", 1);
    fclose(f);
    return 0;
}

/* ---------------------------
   CLI
   --------------------------- */

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage:\n"
        "  %s -c -i <input-file-or-dir> -o <archive.az>   # compress\n"
        "  %s -d -i <archive.az> -o <out-dir>             # decompress\n", prog, prog);
}

int main(int argc, char **argv) {
    int compress = -1; /* 1 compress, 0 decompress */
    const char *input = NULL;
    const char *output = NULL;

    if (argc < 2) { usage(argv[0]); return 1; }
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-c") == 0) compress = 1;
        else if (strcmp(argv[i], "-d") == 0) compress = 0;
        else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) { input = argv[++i]; }
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) { output = argv[++i]; }
        else { fprintf(stderr, "unknown arg: %s\n", argv[i]); usage(argv[0]); return 1; }
    }
    if (compress == -1 || !input || !output) { usage(argv[0]); return 1; }

    prog_state_t ps;
    prog_init(&ps);

    if (compress) {
        FILE *outf = fopen(output, "wb");
        if (!outf) { perror(output); return 1; }
        struct stat st;
        if (stat(input, &st) != 0) { fprintf(stderr, "stat('%s') failed: %s\n", input, strerror(errno)); fclose(outf); return 1; }

        /* compute totals */
        ps.total_bytes = compute_total_input_size_and_count(input, &ps.total_input_files);
        ps.start_time = now_seconds();
        print_progress_block(&ps, "Packing", 0);

        int r = 0;
        if (S_ISDIR(st.st_mode)) {
            r = pack_path_recursive(outf, input, "", &ps);
        } else if (S_ISREG(st.st_mode)) {
            const char *base = strrchr(input, '/');
            const char *rel = base ? base + 1 : input;
            ps.total_input_files = 1;
            r = pack_file_to_stream(outf, input, rel, &ps);
        } else {
            fprintf(stderr, "input is neither file nor directory: %s\n", input);
            r = 1;
        }

        /* final progress */
        ps.processed_bytes = ps.total_bytes;
        print_progress_block(&ps, "Packing", 1);
        fclose(outf);
        return r;
    } else {
        if (ensure_dir_exists(output) != 0) {
            fprintf(stderr, "cannot create output directory '%s': %s\n", output, strerror(errno));
            return 1;
        }
        return unpack_archive(input, output, &ps);
    }
}
