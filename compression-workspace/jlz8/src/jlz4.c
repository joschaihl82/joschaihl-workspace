/*
  jlz4.c - LZ4-style compressor/decompressor with archive, listing and progress
  - Single-file, no external libs
  - Behavior change: on per-file compress/decompress errors, do NOT exit.
    Instead append a timestamped message to "lz4log.txt" and continue.
  Compile:
    gcc -O3 -std=c11 -Wall -Wextra jlz4.c -o jlz4
*/

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <inttypes.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/* Tunables */
#define BLOCK_SIZE (1 << 20)      /* 1 MiB input blocks */
#define WINDOW_SIZE (1 << 16)     /* 64 KiB sliding window */
#define MIN_MATCH 4
#define HASH_SIZE (1 << 16)
#define HASH_SHIFT (32 - 16)

/* Progress globals */
static uint64_t g_total_bytes = 0;
static uint64_t g_processed_bytes = 0;
static struct timeval g_start_time;

/* Per-file status for progress display */
static const char *g_current_file = NULL;
static uint64_t g_file_orig_size = 0;
static uint64_t g_file_comp_size = 0;
static int g_lines_printed = 0;

/* Log file name */
static const char *LOG_FILENAME = "lz4log.txt";

/* --- Logging helper --- */
/* Append a timestamped message to lz4log.txt. Always returns 0 on success, -1 on failure. */
static int log_error(const char *fmt, ...) {
    FILE *f = fopen(LOG_FILENAME, "a");
    if (!f) return -1;
    time_t t = time(NULL);
    struct tm tm;
    localtime_r(&t, &tm);
    char timestr[64];
    snprintf(timestr, sizeof(timestr), "%04d-%02d-%02d %02d:%02d:%02d",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fprintf(f, "[%s] ", timestr);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
    fprintf(f, "\n");
    fclose(f);
    return 0;
}

/* --- Terminal utilities --- */

static int get_terminal_width(void) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0) {
        return (int)ws.ws_col;
    }
    return 80;
}

/* --- General utilities --- */

static void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

static void join_path(char *out, size_t outlen, const char *a, const char *b) {
    if (!a || a[0] == '\0') {
        snprintf(out, outlen, "%s", b);
        return;
    }
    size_t la = strlen(a);
    if (la > 0 && a[la - 1] == '/') {
        snprintf(out, outlen, "%s%s", a, b);
    } else {
        snprintf(out, outlen, "%s/%s", a, b);
    }
}

static int ensure_parent_dir(const char *path) {
    char tmp[PATH_MAX];
    if (strlen(path) >= sizeof(tmp)) return -1;
    strncpy(tmp, path, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';
    char *last_slash = strrchr(tmp, '/');
    if (!last_slash) return 0;
    if (last_slash == tmp) return 0;
    *last_slash = '\0';
    struct stat st;
    if (stat(tmp, &st) == 0) {
        if (S_ISDIR(st.st_mode)) return 0;
    }
    char accum[PATH_MAX] = {0};
    const char *p = tmp;
    if (*p == '/') { strcat(accum, "/"); p++; }
    while (*p) {
        const char *slash = strchr(p, '/');
        size_t len = slash ? (size_t)(slash - p) : strlen(p);
        if (len == 0) { if (!slash) break; p = slash + 1; continue; }
        size_t current_len = strlen(accum);
        if (current_len + len + 2 >= PATH_MAX) return -1;
        strncat(accum, p, len);
        if (mkdir(accum, 0755) != 0) {
            if (errno != EEXIST) return -1;
        }
        if (slash) { strcat(accum, "/"); p = slash + 1; } else break;
    }
    return 0;
}

static int write_all(int fd, const void *buf, size_t size) {
    const uint8_t *p = buf;
    size_t left = size;
    while (left) {
        ssize_t w = write(fd, p, left);
        if (w < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        left -= (size_t)w;
        p += w;
    }
    return 0;
}

static size_t fread_all(FILE *f, uint8_t *buf, size_t size) {
    size_t got = 0;
    while (got < size) {
        size_t r = fread(buf + got, 1, size - got, f);
        if (r == 0) break;
        got += r;
    }
    return got;
}

static void human_bytes(uint64_t v, char *out, size_t outlen) {
    const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    double val = (double)v;
    int i = 0;
    while (val >= 1024.0 && i < 4) { val /= 1024.0; i++; }
    snprintf(out, outlen, "%.2f %s", val, units[i]);
}

static void format_seconds(double s, char *out, size_t outlen) {
    if (s < 0) s = 0;
    int sec = (int)(s + 0.5);
    int h = sec / 3600;
    int m = (sec % 3600) / 60;
    int sec_r = sec % 60;
    snprintf(out, outlen, "%02d:%02d:%02d", h, m, sec_r);
}

/* --- Progress bar --- */

static void progress_update(uint64_t processed, uint64_t total) {
    g_processed_bytes = processed;
    g_total_bytes = total;

    if (g_lines_printed > 0) {
        printf("\033[%dA", g_lines_printed);
    }
    g_lines_printed = 0;

    struct timeval now;
    gettimeofday(&now, NULL);
    double elapsed = (now.tv_sec - g_start_time.tv_sec) + (now.tv_usec - g_start_time.tv_usec) / 1e6;
    double pct = 0.0;
    if (total > 0) pct = (double)processed * 100.0 / (double)total;
    double eta = -1.0;
    if (processed > 0 && total > processed) eta = elapsed * (double)(total - processed) / (double)processed;
    else if (processed >= total) eta = 0.0;

    double speed = (processed > 0 && elapsed > 0.0) ? (double)processed / elapsed : 0.0;
    char hspeed[32] = {0};
    human_bytes((uint64_t)speed, hspeed, sizeof(hspeed));
    strncat(hspeed, "/s", sizeof(hspeed) - strlen(hspeed) - 1);

    double file_ratio = 0.0;
    if (g_file_orig_size > 0 && g_file_comp_size > 0) {
        file_ratio = (1.0 - (double)g_file_comp_size / (double)g_file_orig_size) * 100.0;
    }

    char hproc[32], htot[32], elapsed_s[16], eta_s[16];
    human_bytes(processed, hproc, sizeof(hproc));
    human_bytes(total, htot, sizeof(htot));
    format_seconds(elapsed, elapsed_s, sizeof(elapsed_s));
    format_seconds(eta, eta_s, sizeof(eta_s));

    int term_width = get_terminal_width();
    int bar_width = term_width - 40;
    if (bar_width < 10) bar_width = 10;

    if (g_current_file) {
        printf("\033[KProcessing: %s\n", g_current_file);
    } else {
        printf("\033[K\n");
    }
    g_lines_printed++;

    printf("\033[KElapsed: %s | ETA: %s | Speed: %s\n", elapsed_s, eta_s, hspeed);
    g_lines_printed++;

    int filled = (int)((pct / 100.0) * bar_width + 0.5);
    if (filled < 0) filled = 0;
    if (filled > bar_width) filled = bar_width;
    printf("\033[K%6.2f%% [", pct);
    for (int i = 0; i < filled; ++i) putchar('=');
    for (int i = filled; i < bar_width; ++i) putchar(' ');
    printf("]\n");
    g_lines_printed++;

    if (g_file_orig_size > 0 && g_file_comp_size > 0) {
        printf("\033[KProcessed: %s / %s | Current file savings: %.2f%%\n", hproc, htot, file_ratio);
    } else {
        printf("\033[KProcessed: %s / %s | Ratio: n/a\n", hproc, htot);
    }
    g_lines_printed++;

    fflush(stdout);

    if (processed >= total) {
        for (int i = 0; i < g_lines_printed; ++i) printf("\n");
        g_lines_printed = 0;
    }
}

/* --- LZ4-like compressor/decompressor --- */

static inline uint32_t hash4(const uint8_t *p) {
    uint32_t v;
    memcpy(&v, p, 4);
    v = v * 2654435761u;
    return (v >> HASH_SHIFT) & (HASH_SIZE - 1);
}

static size_t compress_block(const uint8_t *in, size_t in_size, uint8_t *out, size_t out_capacity) {
    if (out_capacity < 12) return 0;
    uint8_t *out_start = out;
    memcpy(out, "LZ4B", 4); out += 4;
    uint32_t orig = (uint32_t)in_size;
    memcpy(out, &orig, 4); out += 4;
    uint8_t *comp_size_ptr = out; out += 4;

    uint32_t *hash_table = calloc(HASH_SIZE, sizeof(uint32_t));
    if (!hash_table) return 0;
    for (size_t i = 0; i < HASH_SIZE; ++i) hash_table[i] = 0xFFFFFFFFu;

    const uint8_t *ip = in;
    const uint8_t *in_end = in + in_size;
    const uint8_t *anchor = ip;

    while (ip + MIN_MATCH <= in_end) {
        uint32_t h = hash4(ip);
        uint32_t ref = hash_table[h];
        hash_table[h] = (uint32_t)(ip - in);

        size_t match_len = 0;
        size_t match_off = 0;
        if (ref != 0xFFFFFFFFu) {
            const uint8_t *rp = in + ref;
            if (ip > rp && (ip - rp) <= WINDOW_SIZE) {
                const uint8_t *p1 = ip;
                const uint8_t *p2 = rp;
                const uint8_t *mend = in_end;
                while (p1 < mend && *p1 == *p2) { p1++; p2++; }
                match_len = (size_t)(p1 - ip);
                if (match_len >= MIN_MATCH) match_off = (size_t)(ip - rp);
                else match_len = 0;
            }
        }

        if (match_len >= MIN_MATCH) {
            size_t literal_len = (size_t)(ip - anchor);
            uint8_t token = (uint8_t)((literal_len >= 15 ? 15 : literal_len) << 4);
            size_t ml = match_len - MIN_MATCH;
            token |= (uint8_t)(ml >= 15 ? 15 : ml);
            if ((size_t)(out - out_start) + 1 + literal_len + 2 + 5 >= out_capacity) { free(hash_table); return 0; }
            *out++ = token;
            if (literal_len >= 15) {
                size_t rem = literal_len - 15;
                while (rem >= 255) { *out++ = 255; rem -= 255; }
                *out++ = (uint8_t)rem;
            }
            if (literal_len) { memcpy(out, anchor, literal_len); out += literal_len; }
            uint16_t off16 = (uint16_t)match_off;
            *out++ = (uint8_t)(off16 & 0xFF);
            *out++ = (uint8_t)((off16 >> 8) & 0xFF);
            if (ml >= 15) {
                size_t rem = ml - 15;
                while (rem >= 255) { *out++ = 255; rem -= 255; }
                *out++ = (uint8_t)rem;
            }
            ip += match_len;
            anchor = ip;
            const uint8_t *p = ip - (match_len - 1);
            while (p + MIN_MATCH <= in_end && p < ip) {
                uint32_t hh = hash4(p);
                hash_table[hh] = (uint32_t)(p - in);
                p++;
            }
        } else {
            ip++;
        }
    }

    size_t literal_len = (size_t)(in_end - anchor);
    if ((size_t)(out - out_start) + 1 + literal_len + 5 >= out_capacity) { free(hash_table); return 0; }
    uint8_t token = (uint8_t)((literal_len >= 15 ? 15 : literal_len) << 4);
    *out++ = token;
    if (literal_len >= 15) {
        size_t rem = literal_len - 15;
        while (rem >= 255) { *out++ = 255; rem -= 255; }
        *out++ = (uint8_t)rem;
    }
    if (literal_len) { memcpy(out, anchor, literal_len); out += literal_len; }

    uint32_t comp_size_val = (uint32_t)(out - comp_size_ptr - 4);
    memcpy(comp_size_ptr, &comp_size_val, 4);
    size_t total = (size_t)(out - out_start);
    free(hash_table);
    return total;
}

static size_t decompress_block(const uint8_t *in, size_t in_size, uint8_t *out, size_t out_capacity) {
    if (in_size < 12) return 0;
    if (memcmp(in, "LZ4B", 4) != 0) return 0;
    uint32_t orig;
    memcpy(&orig, in + 4, 4);
    uint32_t comp_size;
    memcpy(&comp_size, in + 8, 4);
    if (comp_size + 12 != in_size) return 0;
    size_t out_pos = 0;
    const uint8_t *ip = in + 12;
    const uint8_t *iend = in + in_size;
    while (ip < iend) {
        uint8_t token = *ip++;
        size_t literal_len = (token >> 4);
        if (literal_len == 15) {
            uint8_t s;
            do {
                if (ip >= iend) return 0;
                s = *ip++;
                literal_len += s;
            } while (s == 255);
        }
        if (ip + literal_len > iend) return 0;
        if (out_pos + literal_len > out_capacity) return 0;
        memcpy(out + out_pos, ip, literal_len);
        ip += literal_len;
        out_pos += literal_len;

        if (ip >= iend) break;
        if (ip + 2 > iend) return 0;
        uint16_t offset = (uint16_t)ip[0] | ((uint16_t)ip[1] << 8);
        ip += 2;
        size_t match_len = (token & 0x0F);
        if (match_len == 15) {
            uint8_t s;
            do {
                if (ip >= iend) return 0;
                s = *ip++;
                match_len += s;
            } while (s == 255);
        }
        match_len += MIN_MATCH;
        if (offset == 0 || offset > out_pos) return 0;
        size_t ref = out_pos - offset;
        if (out_pos + match_len > out_capacity) return 0;
        for (size_t i = 0; i < match_len; ++i) {
            out[out_pos + i] = out[ref + i];
        }
        out_pos += match_len;
    }

    if (out_pos != orig) return 0;
    return out_pos;
}

/* --- Block streaming helpers --- */

static int fwrite_u32(FILE *f, uint32_t v) {
    uint8_t b[4];
    b[0] = v & 0xFF; b[1] = (v >> 8) & 0xFF; b[2] = (v >> 16) & 0xFF; b[3] = (v >> 24) & 0xFF;
    return fwrite(b, 1, 4, f) == 4 ? 0 : -1;
}
static int fwrite_u64(FILE *f, uint64_t v) {
    uint8_t b[8];
    for (int i = 0; i < 8; ++i) b[i] = (v >> (8 * i)) & 0xFF;
    return fwrite(b, 1, 8, f) == 8 ? 0 : -1;
}
static int fread_u32(FILE *f, uint32_t *pv) {
    uint8_t b[4];
    if (fread(b, 1, 4, f) != 4) return -1;
    *pv = (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
    return 0;
}
static int fread_u64(FILE *f, uint64_t *pv) {
    uint8_t b[8];
    if (fread(b, 1, 8, f) != 8) return -1;
    uint64_t v = 0;
    for (int i = 0; i < 8; ++i) v |= ((uint64_t)b[i]) << (8 * i);
    *pv = v;
    return 0;
}

/* --- Compression / archive routines --- */

/*
 Return values:
   -1 : fatal archive-level error (stop processing)
    0 : success or file skipped (non-fatal)
*/
static int compress_file_to_archive(FILE *archive, const char *relname, const char *inpath, uint64_t *out_comp_size) {
    g_current_file = relname;
    g_file_comp_size = 0;

    FILE *fin = fopen(inpath, "rb");
    if (!fin) {
        log_error("Failed to open input '%s' for archiving: %s", inpath, strerror(errno));
        /* write header with comp_size 0 so archive remains consistent */
        uint32_t name_len = (uint32_t)strlen(relname);
        if (fwrite_u32(archive, name_len) != 0) return -1;
        if (fwrite(relname, 1, name_len, archive) != name_len) return -1;
        if (fwrite_u64(archive, 0) != 0) return -1;
        if (fwrite_u32(archive, 0) != 0) return -1;
        g_current_file = NULL;
        return 0; /* skip file, non-fatal */
    }

    /* write header name and placeholders */
    uint32_t name_len = (uint32_t)strlen(relname);
    if (fwrite_u32(archive, name_len) != 0) { fclose(fin); return -1; }
    if (fwrite(relname, 1, name_len, archive) != name_len) { fclose(fin); return -1; }

    uint64_t orig_size = 0;
    struct stat st;
    if (stat(inpath, &st) == 0) orig_size = (uint64_t)st.st_size;
    g_file_orig_size = orig_size;

    if (fwrite_u64(archive, orig_size) != 0) { fclose(fin); return -1; }
    long comp_size_pos = ftell(archive);
    if (comp_size_pos < 0) { fclose(fin); return -1; }
    if (fwrite_u32(archive, 0) != 0) { fclose(fin); return -1; }

    uint8_t *inbuf = malloc(BLOCK_SIZE);
    uint8_t *outbuf = malloc(BLOCK_SIZE + 64 * 1024);
    if (!inbuf || !outbuf) {
        log_error("Memory allocation failed while compressing '%s'", inpath);
        /* write comp_size 0 and skip */
        long cur = ftell(archive);
        if (cur >= 0) {
            if (fseek(archive, comp_size_pos, SEEK_SET) == 0) fwrite_u32(archive, 0);
            fseek(archive, cur, SEEK_SET);
        }
        fclose(fin);
        free(inbuf); free(outbuf);
        g_current_file = NULL;
        return 0;
    }

    uint32_t total_comp = 0;
    size_t r;
    int file_error = 0;
    while ((r = fread_all(fin, inbuf, BLOCK_SIZE)) > 0) {
        size_t comp = compress_block(inbuf, r, outbuf, BLOCK_SIZE + 64 * 1024);
        if (comp == 0) {
            log_error("Compression failed for block in '%s'", inpath);
            file_error = 1;
            break;
        }
        uint32_t block_total = (uint32_t)comp;
        if (fwrite(&block_total, 4, 1, archive) != 1) { log_error("Write failed while archiving '%s': %s", inpath, strerror(errno)); file_error = 1; break; }
        if (fwrite(outbuf, 1, comp, archive) != comp) { log_error("Write failed while archiving '%s': %s", inpath, strerror(errno)); file_error = 1; break; }
        total_comp += 4 + block_total;
        g_file_comp_size = total_comp;
        g_processed_bytes += r;
        progress_update(g_processed_bytes, g_total_bytes);
    }

    /* If file_error occurred, ensure comp_size placeholder is set to 0 and skip file */
    long cur = ftell(archive);
    if (cur < 0) { /* fatal */
        free(inbuf); free(outbuf); fclose(fin); return -1;
    }
    if (file_error) {
        if (fseek(archive, comp_size_pos, SEEK_SET) == 0) fwrite_u32(archive, 0);
        if (fseek(archive, cur, SEEK_SET) != 0) { free(inbuf); free(outbuf); fclose(fin); return -1; }
        log_error("Skipped file '%s' during archiving due to error", inpath);
        free(inbuf); free(outbuf); fclose(fin);
        g_current_file = NULL;
        g_file_orig_size = 0;
        g_file_comp_size = 0;
        progress_update(g_processed_bytes, g_total_bytes);
        return 0; /* non-fatal skip */
    }

    /* normal completion: write total_comp into placeholder */
    if (fseek(archive, comp_size_pos, SEEK_SET) != 0) { free(inbuf); free(outbuf); fclose(fin); return -1; }
    if (fwrite_u32(archive, total_comp) != 0) { free(inbuf); free(outbuf); fclose(fin); return -1; }
    if (fseek(archive, cur, SEEK_SET) != 0) { free(inbuf); free(outbuf); fclose(fin); return -1; }

    free(inbuf);
    free(outbuf);
    fclose(fin);
    if (out_comp_size) *out_comp_size = total_comp;

    g_current_file = NULL;
    g_file_orig_size = 0;
    g_file_comp_size = 0;
    progress_update(g_processed_bytes, g_total_bytes);

    return 0;
}

/*
 Decompress a single file entry from archive.
 On per-file errors, skip remaining bytes of this entry and return 0 (non-fatal).
 On archive-level corruption, return -1 (fatal).
*/
static int decompress_file_from_archive(FILE *archive, const char *outpath, uint32_t comp_size, const char *name, uint64_t orig_size) {
    g_current_file = name;
    g_file_orig_size = orig_size;
    g_file_comp_size = comp_size;

    if (ensure_parent_dir(outpath) != 0) {
        log_error("Failed to create parent directory for '%s'", outpath);
        /* skip comp_size bytes to continue */
        if (fseek(archive, comp_size, SEEK_CUR) != 0) return -1;
        g_current_file = NULL;
        return 0;
    }
    int fout = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fout < 0) {
        log_error("Failed to open output '%s' for extraction: %s", outpath, strerror(errno));
        /* skip comp_size bytes to continue */
        if (fseek(archive, comp_size, SEEK_CUR) != 0) return -1;
        g_current_file = NULL;
        return 0;
    }

    uint8_t *inbuf = malloc(BLOCK_SIZE + 64 * 1024);
    uint8_t *outbuf = malloc(BLOCK_SIZE + 64 * 1024);
    if (!inbuf || !outbuf) {
        log_error("Memory allocation failed while extracting '%s'", outpath);
        close(fout);
        free(inbuf); free(outbuf);
        if (fseek(archive, comp_size, SEEK_CUR) != 0) return -1;
        g_current_file = NULL;
        return 0;
    }

    uint32_t remaining = comp_size;
    int file_error = 0;
    while (remaining > 0) {
        uint32_t block_total;
        if (fread(&block_total, 4, 1, archive) != 1) {
            log_error("Archive corrupt while reading block header for '%s'", name);
            file_error = -1; break;
        }
        remaining -= 4;
        if (block_total > BLOCK_SIZE + 64 * 1024) {
            log_error("Block too large in archive for '%s'", name);
            file_error = -1; break;
        }
        size_t got = fread_all(archive, inbuf, block_total);
        if (got != block_total) {
            log_error("Unexpected EOF in block for '%s'", name);
            file_error = -1; break;
        }
        remaining -= block_total;
        size_t dec = decompress_block(inbuf, block_total, outbuf, BLOCK_SIZE + 64 * 1024);
        if (dec == 0) {
            log_error("Decompression failed for block in '%s'", name);
            file_error = 1;
            /* cannot reliably continue decompressing this file; skip remaining bytes */
            if (remaining > 0) {
                if (fseek(archive, remaining, SEEK_CUR) != 0) { file_error = -1; break; }
                remaining = 0;
            }
            break;
        }
        if (write_all(fout, outbuf, dec) != 0) {
            log_error("Write failed while extracting '%s': %s", outpath, strerror(errno));
            file_error = 1;
            if (remaining > 0) {
                if (fseek(archive, remaining, SEEK_CUR) != 0) { file_error = -1; break; }
                remaining = 0;
            }
            break;
        }
        g_processed_bytes += 4 + block_total;
        progress_update(g_processed_bytes, g_total_bytes);
    }

    free(inbuf);
    free(outbuf);
    close(fout);

    if (file_error == -1) {
        /* fatal archive-level error */
        g_current_file = NULL;
        return -1;
    }
    if (file_error == 1) {
        log_error("Skipped file '%s' during extraction due to error", name);
        g_current_file = NULL;
        g_file_orig_size = 0;
        g_file_comp_size = 0;
        progress_update(g_processed_bytes, g_total_bytes);
        return 0; /* non-fatal skip */
    }

    /* success */
    g_current_file = NULL;
    g_file_orig_size = 0;
    g_file_comp_size = 0;
    progress_update(g_processed_bytes, g_total_bytes);
    return 0;
}

/* Single-file compress/decompress (legacy) */
/* On per-file errors, log and return 0 (skip). Fatal errors return -1. */

static int compress_file_path(const char *inpath, const char *outpath) {
    g_current_file = outpath;
    g_file_comp_size = 0;

    FILE *fin = fopen(inpath, "rb");
    if (!fin) {
        log_error("open input '%s' failed: %s", inpath, strerror(errno));
        g_current_file = NULL;
        return 0; /* skip */
    }
    if (ensure_parent_dir(outpath) != 0) {
        log_error("mkdir parent failed for %s", outpath);
        fclose(fin);
        g_current_file = NULL;
        return 0;
    }
    int fout = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fout < 0) {
        log_error("open output '%s' failed: %s", outpath, strerror(errno));
        fclose(fin);
        g_current_file = NULL;
        return 0;
    }

    uint64_t orig_size = 0;
    struct stat st;
    if (stat(inpath, &st) == 0) orig_size = (uint64_t)st.st_size;
    g_file_orig_size = orig_size;

    uint8_t *inbuf = malloc(BLOCK_SIZE);
    uint8_t *outbuf = malloc(BLOCK_SIZE + 64 * 1024);
    if (!inbuf || !outbuf) {
        log_error("Memory allocation failed while compressing '%s'", inpath);
        fclose(fin); close(fout); free(inbuf); free(outbuf);
        g_current_file = NULL;
        return 0;
    }

    uint32_t total_comp = 0;
    size_t r;
    int file_error = 0;
    while ((r = fread_all(fin, inbuf, BLOCK_SIZE)) > 0) {
        size_t comp = compress_block(inbuf, r, outbuf, BLOCK_SIZE + 64 * 1024);
        if (comp == 0) {
            log_error("compression failed for block in '%s'", inpath);
            file_error = 1;
            break;
        }
        uint32_t block_total = (uint32_t)comp;
        if (write_all(fout, &block_total, 4) != 0) { log_error("write failed while compressing '%s': %s", outpath, strerror(errno)); file_error = 1; break; }
        if (write_all(fout, outbuf, comp) != 0) { log_error("write failed while compressing '%s': %s", outpath, strerror(errno)); file_error = 1; break; }
        total_comp += 4 + block_total;
        g_file_comp_size = total_comp;
        g_processed_bytes += r;
        progress_update(g_processed_bytes, g_total_bytes);
    }

    free(inbuf);
    free(outbuf);
    fclose(fin);
    close(fout);

    if (file_error) {
        log_error("Skipped file '%s' during compression due to error", inpath);
        /* remove incomplete output file if exists */
        unlink(outpath);
        g_current_file = NULL;
        g_file_orig_size = 0;
        g_file_comp_size = 0;
        progress_update(g_processed_bytes, g_total_bytes);
        return 0;
    }

    if (outpath) {
        /* success */
    }

    g_current_file = NULL;
    g_file_orig_size = 0;
    g_file_comp_size = 0;
    progress_update(g_processed_bytes, g_total_bytes);
    return 0;
}

static int decompress_file_path(const char *inpath, const char *outpath) {
    g_current_file = inpath;
    g_file_orig_size = 0;
    g_file_comp_size = 0;

    FILE *fin = fopen(inpath, "rb");
    if (!fin) {
        log_error("open input '%s' failed: %s", inpath, strerror(errno));
        g_current_file = NULL;
        return 0;
    }
    if (ensure_parent_dir(outpath) != 0) {
        log_error("mkdir parent failed for %s", outpath);
        fclose(fin);
        g_current_file = NULL;
        return 0;
    }
    int fout = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fout < 0) {
        log_error("open output '%s' failed: %s", outpath, strerror(errno));
        fclose(fin);
        g_current_file = NULL;
        return 0;
    }

    uint8_t *inbuf = malloc(BLOCK_SIZE + 64 * 1024);
    uint8_t *outbuf = malloc(BLOCK_SIZE + 64 * 1024);
    if (!inbuf || !outbuf) {
        log_error("Memory allocation failed while decompressing '%s'", inpath);
        fclose(fin); close(fout); free(inbuf); free(outbuf);
        g_current_file = NULL;
        return 0;
    }

    int file_error = 0;
    while (1) {
        uint32_t block_total;
        size_t r = fread(&block_total, 1, 4, fin);
        if (r == 0) break;
        if (r != 4) { log_error("corrupt input (block header) in '%s'", inpath); file_error = 1; break; }
        if (block_total > BLOCK_SIZE + 64 * 1024) { log_error("block too large in '%s'", inpath); file_error = 1; break; }
        size_t got = fread_all(fin, inbuf, block_total);
        if (got != block_total) { log_error("unexpected EOF in block in '%s'", inpath); file_error = 1; break; }
        size_t dec = decompress_block(inbuf, block_total, outbuf, BLOCK_SIZE + 64 * 1024);
        if (dec == 0) { log_error("decompression failed for block in '%s'", inpath); file_error = 1; break; }
        if (write_all(fout, outbuf, dec) != 0) { log_error("write failed while decompressing '%s': %s", outpath, strerror(errno)); file_error = 1; break; }
        g_processed_bytes += 4 + block_total;
        progress_update(g_processed_bytes, g_total_bytes);
    }

    free(inbuf);
    free(outbuf);
    fclose(fin);
    close(fout);

    if (file_error) {
        log_error("Skipped file '%s' during decompression due to error", inpath);
        unlink(outpath);
        g_current_file = NULL;
        g_file_orig_size = 0;
        g_file_comp_size = 0;
        progress_update(g_processed_bytes, g_total_bytes);
        return 0;
    }

    g_current_file = NULL;
    g_file_orig_size = 0;
    g_file_comp_size = 0;
    progress_update(g_processed_bytes, g_total_bytes);
    return 0;
}

/* --- Directory recursion and totals --- */

static int compute_total_size_recursive(const char *path, uint64_t *accum) {
    struct stat st;
    if (lstat(path, &st) != 0) return -1;
    if (S_ISDIR(st.st_mode)) {
        DIR *d = opendir(path);
        if (!d) return -1;
        struct dirent *ent;
        while ((ent = readdir(d)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            char child[PATH_MAX];
            join_path(child, sizeof(child), path, ent->d_name);
            if (compute_total_size_recursive(child, accum) != 0) { closedir(d); return -1; }
        }
        closedir(d);
        return 0;
    } else if (S_ISREG(st.st_mode)) {
        *accum += (uint64_t)st.st_size;
        return 0;
    } else return 0;
}

static int compute_total_compressed_input_recursive(const char *path, uint64_t *accum) {
    return compute_total_size_recursive(path, accum);
}

static int compute_base_root(const char *input, char *base_root, size_t blen) {
    struct stat st;
    if (lstat(input, &st) != 0) return -1;
    if (S_ISDIR(st.st_mode)) {
        strncpy(base_root, input, blen - 1);
        base_root[blen - 1] = '\0';
        size_t l = strlen(base_root);
        if (l > 1 && base_root[l - 1] == '/') base_root[l - 1] = '\0';
        return 0;
    } else {
        char tmp[PATH_MAX];
        strncpy(tmp, input, sizeof(tmp) - 1);
        tmp[sizeof(tmp) - 1] = '\0';
        char *d = dirname(tmp);
        strncpy(base_root, d, blen - 1);
        base_root[blen - 1] = '\0';
        return 0;
    }
}

static int make_relative(const char *base, const char *target, char *out, size_t outlen) {
    size_t bl = strlen(base);
    if (bl == 0) return -1;
    if (strncmp(base, target, bl) == 0) {
        const char *p = target + bl;
        if (*p == '/') p++;
        strncpy(out, p, outlen - 1);
        out[outlen - 1] = '\0';
        return 0;
    }
    return -1;
}

/* process_compress_to_archive: treat negative return as fatal, zero as success/skip */
static int process_compress_to_archive(FILE *archive, const char *inpath, const char *base_root) {
    struct stat st;
    if (lstat(inpath, &st) != 0) { fprintf(stderr, "stat '%s' failed: %s\n", inpath, strerror(errno)); return -1; }
    if (S_ISDIR(st.st_mode)) {
        DIR *d = opendir(inpath);
        if (!d) { fprintf(stderr, "opendir '%s' failed: %s\n", inpath, strerror(errno)); return -1; }
        struct dirent *ent;
        while ((ent = readdir(d)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            char child[PATH_MAX];
            join_path(child, sizeof(child), inpath, ent->d_name);
            int r = process_compress_to_archive(archive, child, base_root);
            if (r < 0) { closedir(d); return -1; }
        }
        closedir(d);
        return 0;
    } else if (S_ISREG(st.st_mode)) {
        char rel[PATH_MAX];
        if (make_relative(base_root, inpath, rel, sizeof(rel)) != 0) {
            const char *base = strrchr(inpath, '/');
            const char *fname = base ? base + 1 : inpath;
            strncpy(rel, fname, sizeof(rel) - 1);
            rel[sizeof(rel) - 1] = '\0';
        }
        uint64_t comp_size = 0;
        int r = compress_file_to_archive(archive, rel, inpath, &comp_size);
        if (r < 0) return -1;
        /* r == 0 means success or skipped; continue */
        (void)comp_size;
        return 0;
    } else {
        fprintf(stderr, "Skipping non-regular: %s\n", inpath);
        return 0;
    }
}

static int process_compress_recursive(const char *inpath, const char *base_root, const char *outroot) {
    struct stat st;
    if (lstat(inpath, &st) != 0) { fprintf(stderr, "stat '%s' failed: %s\n", inpath, strerror(errno)); return -1; }
    if (S_ISDIR(st.st_mode)) {
        DIR *d = opendir(inpath);
        if (!d) { fprintf(stderr, "opendir '%s' failed: %s\n", inpath, strerror(errno)); return -1; }
        struct dirent *ent;
        while ((ent = readdir(d)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            char child[PATH_MAX];
            join_path(child, sizeof(child), inpath, ent->d_name);
            if (process_compress_recursive(child, base_root, outroot) != 0) { closedir(d); return -1; }
        }
        closedir(d);
        return 0;
    } else if (S_ISREG(st.st_mode)) {
        char outpath[PATH_MAX];
        if (outroot && outroot[0] != '\0') {
            struct stat ost;
            if (stat(outroot, &ost) == 0 && S_ISDIR(ost.st_mode)) {
                char rel[PATH_MAX];
                if (make_relative(base_root, inpath, rel, sizeof(rel)) == 0 && rel[0] != '\0') join_path(outpath, sizeof(outpath), outroot, rel);
                else {
                    const char *base = strrchr(inpath, '/');
                    const char *fname = base ? base + 1 : inpath;
                    join_path(outpath, sizeof(outpath), outroot, fname);
                }
            } else {
                strncpy(outpath, outroot, sizeof(outpath) - 1);
                outpath[sizeof(outpath) - 1] = '\0';
            }
        } else {
            strncpy(outpath, inpath, sizeof(outpath) - 1);
            outpath[sizeof(outpath) - 1] = '\0';
        }
        size_t len = strlen(outpath);
        if (len + 5 >= sizeof(outpath)) { fprintf(stderr, "path too long\n"); return -1; }
        strcat(outpath, ".lz4");
        int r = compress_file_path(inpath, outpath);
        if (r < 0) return -1;
        return 0;
    } else {
        fprintf(stderr, "Skipping non-regular: %s\n", inpath);
        return 0;
    }
}

static int process_decompress_archive(FILE *archive, const char *outroot) {
    while (1) {
        uint32_t name_len;
        if (fread_u32(archive, &name_len) != 0) {
            if (feof(archive)) return 0;
            fprintf(stderr, "archive corrupt (name_len)\n");
            return -1;
        }
        if (name_len == 0 || name_len > PATH_MAX - 1) { fprintf(stderr, "archive corrupt (name_len invalid)\n"); return -1; }
        char name[PATH_MAX];
        if (fread(name, 1, name_len, archive) != name_len) { fprintf(stderr, "archive corrupt (name)\n"); return -1; }
        name[name_len] = '\0';
        uint64_t orig_size;
        if (fread_u64(archive, &orig_size) != 0) { fprintf(stderr, "archive corrupt (orig_size)\n"); return -1; }
        uint32_t comp_size;
        if (fread_u32(archive, &comp_size) != 0) { fprintf(stderr, "archive corrupt (comp_size)\n"); return -1; }
        char outpath[PATH_MAX];
        if (outroot && outroot[0] != '\0') join_path(outpath, sizeof(outpath), outroot, name);
        else strncpy(outpath, name, sizeof(outpath) - 1);
        int r = decompress_file_from_archive(archive, outpath, comp_size, name, orig_size);
        if (r < 0) return -1; /* fatal */
        /* r == 0 means success or skipped; continue */
    }
    return 0;
}

/* List archive contents */

static int list_archive(const char *archive_path) {
    FILE *f = fopen(archive_path, "rb");
    if (!f) { fprintf(stderr, "open archive '%s' failed: %s\n", archive_path, strerror(errno)); return -1; }
    printf("Listing archive: %s\n", archive_path);
    while (1) {
        uint32_t name_len;
        if (fread_u32(f, &name_len) != 0) {
            if (feof(f)) { fclose(f); return 0; }
            fprintf(stderr, "archive corrupt (name_len)\n"); fclose(f); return -1;
        }
        if (name_len == 0 || name_len > PATH_MAX - 1) { fprintf(stderr, "archive corrupt (name_len invalid)\n"); fclose(f); return -1; }
        char name[PATH_MAX];
        if (fread(name, 1, name_len, f) != name_len) { fprintf(stderr, "archive corrupt (name)\n"); fclose(f); return -1; }
        name[name_len] = '\0';
        uint64_t orig_size;
        if (fread_u64(f, &orig_size) != 0) { fprintf(stderr, "archive corrupt (orig_size)\n"); fclose(f); return -1; }
        uint32_t comp_size;
        if (fread_u32(f, &comp_size) != 0) { fprintf(stderr, "archive corrupt (comp_size)\n"); fclose(f); return -1; }
        if (fseek(f, comp_size, SEEK_CUR) != 0) { fprintf(stderr, "archive corrupt (skip)\n"); fclose(f); return -1; }
        char horig[32], hcomp[32];
        human_bytes(orig_size, horig, sizeof(horig));
        human_bytes(comp_size, hcomp, sizeof(hcomp));
        printf("%s  orig=%s  comp=%s\n", name, horig, hcomp);
    }
    /* unreachable */
    fclose(f);
    return 0;
}

/* --- CLI and main --- */

static void print_usage(const char *p) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s -c <input> [<output>]    # compress (if input is a directory and output omitted, creates <folder>-YYYYMMDDhhmm.lz4)\n", p);
    fprintf(stderr, "  %s -d <input> [<output>]    # decompress (archive or single file)\n", p);
    fprintf(stderr, "  %s -l <archive.lz4>         # list archive contents\n", p);
}

/* helper: build timestamped archive name: <basename>-YYYYMMDDhhmm.lz4 */
static void make_timestamped_archive(const char *dirpath, char *out, size_t outlen) {
    char tmp[PATH_MAX];
    strncpy(tmp, dirpath, sizeof(tmp)-1);
    tmp[sizeof(tmp)-1] = '\0';
    char *base = basename(tmp);
    time_t t = time(NULL);
    struct tm tm;
    localtime_r(&t, &tm);
    char stamp[32];
    snprintf(stamp, sizeof(stamp), "%04d%02d%02d%02d%02d",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
    snprintf(out, outlen, "%s-%s.lz4", base, stamp);
}

int main(int argc, char **argv) {
    if (argc < 2) { print_usage(argv[0]); return 1; }
    const char *mode = argv[1];

    if (strcmp(mode, "-l") == 0) {
        if (argc != 3) { print_usage(argv[0]); return 1; }
        const char *archive = argv[2];
        return list_archive(archive) == 0 ? 0 : 2;
    }

    if (strcmp(mode, "-c") != 0 && strcmp(mode, "-d") != 0) {
        print_usage(argv[0]);
        return 1;
    }

    int do_compress = (strcmp(mode, "-c") == 0);
    int do_decompress = (strcmp(mode, "-d") == 0);

    if (argc < 3) { print_usage(argv[0]); return 1; }

    const char *inpath = argv[2];
    const char *outroot = (argc >= 4) ? argv[3] : NULL;

    struct stat st;
    if (lstat(inpath, &st) != 0) {
        fprintf(stderr, "stat '%s' failed: %s\n", inpath, strerror(errno));
        return 1;
    }

    char base_root[PATH_MAX];
    if (compute_base_root(inpath, base_root, sizeof(base_root)) != 0) {
        fprintf(stderr, "compute base root failed for '%s'\n", inpath);
        return 1;
    }

    uint64_t total = 0;
    if (do_compress) {
        if (S_ISDIR(st.st_mode)) {
            if (compute_total_size_recursive(inpath, &total) != 0) { fprintf(stderr, "failed to compute total size\n"); return 1; }
        } else if (S_ISREG(st.st_mode)) total = (uint64_t)st.st_size;
        else { fprintf(stderr, "Input not file or directory\n"); return 1; }
    } else {
        if (S_ISREG(st.st_mode)) total = (uint64_t)st.st_size;
        else if (S_ISDIR(st.st_mode)) {
            if (compute_total_compressed_input_recursive(inpath, &total) != 0) { fprintf(stderr, "failed to compute total size\n"); return 1; }
        } else { fprintf(stderr, "Input not file or directory\n"); return 1; }
    }

    g_total_bytes = total;
    g_processed_bytes = 0;
    gettimeofday(&g_start_time, NULL);
    progress_update(0, g_total_bytes);

    int rc = 0;
    if (do_compress) {
        /* If input is a directory and no explicit output given, create timestamped archive in CWD */
        if (S_ISDIR(st.st_mode) && (outroot == NULL || outroot[0] == '\0')) {
            char archive_name[PATH_MAX];
            make_timestamped_archive(inpath, archive_name, sizeof(archive_name));
            printf("Creating archive: %s\n", archive_name);
            FILE *archive = fopen(archive_name, "wb");
            if (!archive) { fprintf(stderr, "open archive '%s' failed: %s\n", archive_name, strerror(errno)); return 2; }
            rc = process_compress_to_archive(archive, inpath, base_root);
            fclose(archive);
        } else {
            int out_is_archive_file = 0;
            if (outroot && strlen(outroot) > 4 && strcmp(outroot + strlen(outroot) - 4, ".lz4") == 0) out_is_archive_file = 1;

            if (out_is_archive_file && S_ISDIR(st.st_mode)) {
                FILE *archive = fopen(outroot, "wb");
                if (!archive) { fprintf(stderr, "open archive '%s' failed: %s\n", outroot, strerror(errno)); return 2; }
                rc = process_compress_to_archive(archive, inpath, base_root);
                fclose(archive);
            } else if (S_ISDIR(st.st_mode)) {
                rc = process_compress_recursive(inpath, base_root, outroot);
            } else if (S_ISREG(st.st_mode)) {
                char outpath[PATH_MAX];
                if (outroot && outroot[0] != '\0') {
                    struct stat ost;
                    if (stat(outroot, &ost) == 0 && S_ISDIR(ost.st_mode)) {
                        const char *base = strrchr(inpath, '/');
                        const char *fname = base ? base + 1 : inpath;
                        join_path(outpath, sizeof(outpath), outroot, fname);
                    } else {
                        strncpy(outpath, outroot, sizeof(outpath) - 1);
                        outpath[sizeof(outpath) - 1] = '\0';
                    }
                } else {
                    strncpy(outpath, inpath, sizeof(outpath) - 1);
                    outpath[sizeof(outpath) - 1] = '\0';
                }
                strncat(outpath, ".lz4", sizeof(outpath) - strlen(outpath) - 1);
                printf("Compress: %s -> %s\n", inpath, outpath);
                rc = compress_file_path(inpath, outpath);
            } else { fprintf(stderr, "Input not file or directory\n"); return 1; }
        }
    } else {
        if (S_ISREG(st.st_mode)) {
            FILE *f = fopen(inpath, "rb");
            if (!f) { fprintf(stderr, "open '%s' failed: %s\n", inpath, strerror(errno)); return 2; }
            uint32_t name_len = 0;
            int is_archive = 0;
            if (fread(&name_len, 4, 1, f) == 1) {
                if (name_len > 0 && name_len < PATH_MAX) is_archive = 1;
            }
            fclose(f);

            if (is_archive) {
                FILE *archive = fopen(inpath, "rb");
                if (!archive) { fprintf(stderr, "open archive '%s' failed: %s\n", inpath, strerror(errno)); return 2; }
                rc = process_decompress_archive(archive, outroot);
                fclose(archive);
            } else {
                char outpath[PATH_MAX];
                size_t inlen = strlen(inpath);
                const char *suf = ".lz4";
                if (inlen > strlen(suf) && strcmp(inpath + inlen - strlen(suf), suf) == 0) {
                    strncpy(outpath, inpath, inlen - strlen(suf));
                    outpath[inlen - strlen(suf)] = '\0';
                } else snprintf(outpath, sizeof(outpath), "%s.decompressed", inpath);
                if (outroot && outroot[0] != '\0') {
                    struct stat ost;
                    if (stat(outroot, &ost) == 0 && S_ISDIR(ost.st_mode)) {
                        const char *base = strrchr(outpath, '/');
                        const char *fname = base ? base + 1 : outpath;
                        char tmp[PATH_MAX];
                        join_path(tmp, sizeof(tmp), outroot, fname);
                        strncpy(outpath, tmp, sizeof(outpath) - 1);
                        outpath[sizeof(outpath) - 1] = '\0';
                    } else strncpy(outpath, outroot, sizeof(outpath) - 1);
                }
                printf("Decompress: %s -> %s\n", inpath, outpath);
                rc = decompress_file_path(inpath, outpath);
            }
        } else {
            fprintf(stderr, "Input not a regular file for decompression\n");
            return 1;
        }
    }

    if (rc != 0) {
        fprintf(stderr, "\nOperation finished with non-zero code %d (see lz4log.txt for per-file errors)\n", rc);
        /* do not exit immediately on per-file errors; rc may be non-zero for archive-level issues */
        return 2;
    }

    progress_update(g_total_bytes, g_total_bytes);
    return 0;
}

