/*
 * jlz4.c
 *
 * Single-file directory compressor / decompressor using an internal LZ4 implementation
 * with block framing for error robustness, a scan mode to report corrupted frames,
 * and a UI drawn just above the command line (no jump to 0,0).
 *
 * This file is self-contained and aims to be portable. It implements:
 *  - create:  ./jlz4 -c <dir>
 *  - extract: ./jlz4 -d <archive.lz4>
 *  - scan:    ./jlz4 -s <archive.lz4>   (reports corrupted frames without extracting)
 *
 * Compile:
 *   gcc -O3 -march=native -g3 -Wall -std=c11 jlz4.c -o jlz4
 *
 * Notes:
 *  - The LZ4 implementation here is a simple, compact matcher for demonstration.
 *  - Frame format:
 *      uint32_t FRAME_MAGIC = 0x4C5A3446 ("LZ4F")
 *      uint32_t comp_size
 *      uint32_t orig_size
 *      uint32_t crc32 (of compressed data)
 *      comp_size bytes compressed data
 *
 *  - Archive format:
 *      Header: "JLZ4ARC"
 *      Entries: type(1), path_len(4), path, mode(4), mtime(8), orig_size(8), comp_size(8), comp_size bytes
 *      End marker: type=0
 *
 *  - UI: draws an 8-line block immediately above the current command line using relative cursor moves.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <inttypes.h>
#include <limits.h>
#include <libgen.h>
#include <utime.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/* Magic */
static const char ARCHIVE_MAGIC[] = "JLZ4ARC";

/* Entry types */
enum { ET_END = 0, ET_FILE = 1, ET_DIR = 2, ET_SYMLINK = 3 };

/* Frame magic */
#define FRAME_MAGIC 0x4C5A3446u  /* 'L' 'Z' '4' 'F' */

/* UI state (used for create/extract progress) */
typedef struct {
    size_t term_width;
    size_t term_height;
    uint64_t total_bytes;
    uint64_t processed_bytes;
    time_t start_time;
    char current_file[PATH_MAX];
    uint64_t files_processed;
    uint64_t files_total;
    uint64_t errors;
    char last_error[PATH_MAX + 256];
} ui_state_t;

static ui_state_t UI;

/* Utility: get terminal size */
static void get_term_size(size_t *w, size_t *h) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        *w = ws.ws_col ? ws.ws_col : 80;
        *h = ws.ws_row ? ws.ws_row : 24;
    } else {
        *w = 80; *h = 24;
    }
}

/* Pad a line with spaces to terminal width and print */
static void ui_print_line(const char *fmt, ...) {
    char buf[4096];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    size_t len = strlen(buf);
    size_t w = UI.term_width ? UI.term_width : 80;
    if (len >= w) {
        fwrite(buf, 1, w, stdout);
    } else {
        fwrite(buf, 1, len, stdout);
        for (size_t i = len; i < w; ++i) putchar(' ');
    }
    putchar('\n');
}

/* Format seconds to H:MM:SS */
static void fmt_time(uint64_t s, char *out, size_t outlen) {
    uint64_t h = s / 3600;
    uint64_t m = (s % 3600) / 60;
    uint64_t sec = s % 60;
    if (h > 0) snprintf(out, outlen, "%" PRIu64 ":%02" PRIu64 ":%02" PRIu64, h, m, sec);
    else snprintf(out, outlen, "%02" PRIu64 ":%02" PRIu64, m, sec);
}

/* Draw UI block just above the command line (do not jump to 0,0) */
static void ui_draw() {
    get_term_size(&UI.term_width, &UI.term_height);

    /* Number of lines the UI occupies (keep 8 as before) */
    const size_t LINES_TO_DRAW = 8;

    /* If terminal is too small, clamp lines to draw */
    size_t draw_lines = LINES_TO_DRAW;
    if (UI.term_height <= 1) draw_lines = 1;
    else if (UI.term_height <= draw_lines) draw_lines = UI.term_height - 1;

    /* Move cursor up draw_lines lines from current cursor position.
       This places the UI block immediately above the command line.
       If draw_lines is 0, do nothing. */
    if (draw_lines > 0) {
        /* ESC [ <N> A  -> move cursor up N lines */
        printf("\033[%zuA", draw_lines);
    }

    /* Now print the UI lines; each ui_print_line pads to terminal width */
    double progress = UI.total_bytes ? (double)UI.processed_bytes / UI.total_bytes : 0.0;
    size_t bar_width = UI.term_width > 20 ? UI.term_width - 20 : 10;
    size_t filled = (size_t)(progress * bar_width + 0.5);
    char bar[4096];
    size_t p = 0;
    p += snprintf(bar + p, sizeof(bar) - p, "[");
    for (size_t i = 0; i < bar_width; ++i) bar[p++] = (i < filled) ? '=' : ' ';
    bar[p++] = ']';
    bar[p] = '\0';
    ui_print_line("%s %3.0f%%", bar, progress * 100.0);

    time_t now = time(NULL);
    uint64_t elapsed = now - UI.start_time;
    char elapsed_s[64], eta_s[64];
    fmt_time(elapsed, elapsed_s, sizeof(elapsed_s));
    double mbps = elapsed ? (double)UI.processed_bytes / (1024.0 * 1024.0) / (double)elapsed : 0.0;
    if (UI.processed_bytes && UI.total_bytes > UI.processed_bytes) {
        uint64_t remain = UI.total_bytes - UI.processed_bytes;
        double est = mbps > 0.0 ? (double)remain / (1024.0 * 1024.0) / mbps : 0.0;
        fmt_time((uint64_t)(est + 0.5), eta_s, sizeof(eta_s));
    } else {
        strcpy(eta_s, "--:--");
    }
    ui_print_line("Elapsed: %s  ETA: %s  Speed: %.2f MB/s", elapsed_s, eta_s, mbps);

    ui_print_line("File: %s", UI.current_file[0] ? UI.current_file : "(idle)");
    ui_print_line("Files: %" PRIu64 " / %" PRIu64 "  Errors: %" PRIu64, UI.files_processed, UI.files_total, UI.errors);
    ui_print_line("Last failure: %s", UI.last_error[0] ? UI.last_error : "(none)");

    /* Fill remaining lines up to draw_lines (so we always overwrite previous content) */
    for (size_t i = 5; i < draw_lines; ++i) ui_print_line("");

    fflush(stdout);

    /* After drawing, move cursor back down so the prompt remains where the user typed.
       Move down exactly draw_lines lines to restore original cursor row. */
    if (draw_lines > 0) {
        printf("\033[%zuB", draw_lines);
        fflush(stdout);
    }
}

/* Helper: record error */
static void record_error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(UI.last_error, sizeof(UI.last_error), fmt, ap);
    va_end(ap);
    UI.errors++;
    ui_draw();
}

/* Helper: write little-endian integers */
static int write_u8(FILE *f, uint8_t v) { return fputc(v, f) != EOF; }
static int write_u32(FILE *f, uint32_t v) {
    uint32_t le = v;
    return fwrite(&le, 4, 1, f) == 1;
}
static int write_u64(FILE *f, uint64_t v) {
    uint64_t le = v;
    return fwrite(&le, 8, 1, f) == 1;
}

/* Helper: read little-endian integers */
static int read_u8(FILE *f, uint8_t *v) {
    int c = fgetc(f);
    if (c == EOF) return 0;
    *v = (uint8_t)c;
    return 1;
}
static int read_u32(FILE *f, uint32_t *v) {
    return fread(v, 4, 1, f) == 1;
}
static int read_u64(FILE *f, uint64_t *v) {
    return fread(v, 8, 1, f) == 1;
}

/* ---------------------------
   Minimal LZ4 implementation
   --------------------------- */

#define LZ4_HASH_LOG 16
#define LZ4_HASH_SIZE (1 << LZ4_HASH_LOG)
#define LZ4_MIN_MATCH 4
#define LZ4_MAX_OFFSET 65535

static inline uint32_t lz4_hash4(const void *p) {
    const uint8_t *s = (const uint8_t*)p;
    uint32_t v = (uint32_t)s[0] | ((uint32_t)s[1] << 8) | ((uint32_t)s[2] << 16) | ((uint32_t)s[3] << 24);
    return (v * 2654435761u) >> (32 - LZ4_HASH_LOG);
}
static inline uint32_t read32(const void *p) { uint32_t v; memcpy(&v, p, 4); return v; }

int lz4_compress_default(const char* src, char* dst, int srcSize, int dstCapacity) {
    if (srcSize <= 0) return 0;
    const unsigned char *ip = (const unsigned char*)src;
    const unsigned char *anchor = ip;
    const unsigned char *iend = ip + srcSize;
    unsigned char *op = (unsigned char*)dst;
    unsigned char *olimit = (unsigned char*)dst + dstCapacity;

    uint32_t *hash_table = (uint32_t*)malloc(LZ4_HASH_SIZE * sizeof(uint32_t));
    if (!hash_table) return 0;
    for (size_t i = 0; i < LZ4_HASH_SIZE; ++i) hash_table[i] = 0;

    const unsigned char *base = ip;
    while (ip + LZ4_MIN_MATCH <= iend) {
        uint32_t h = lz4_hash4(ip);
        uint32_t refIndex = hash_table[h];
        const unsigned char *ref = refIndex ? base + (refIndex - 1) : NULL;
        hash_table[h] = (uint32_t)(ip - base + 1);

        if (ref && ref + LZ4_MIN_MATCH <= iend && ref >= base && ref < ip) {
            if (read32(ref) == read32(ip)) {
                size_t literal_len = ip - anchor;
                if (op + 1 >= olimit) { free(hash_table); return 0; }
                unsigned char *token_p = op++;
                unsigned char token = 0;
                if (literal_len >= 15) {
                    token |= 15 << 4;
                    size_t len = literal_len - 15;
                    while (len >= 255) { if (op >= olimit) { free(hash_table); return 0; } *op++ = 255; len -= 255; }
                    if (op >= olimit) { free(hash_table); return 0; }
                    *op++ = (unsigned char)len;
                } else {
                    token |= (unsigned char)(literal_len << 4);
                }
                if (op + literal_len >= olimit) { free(hash_table); return 0; }
                memcpy(op, anchor, literal_len);
                op += literal_len;

                const unsigned char *m1 = ref;
                const unsigned char *m2 = ip;
                size_t match_len = 0;
                while (m2 + match_len < iend && m1 + match_len < ip && m1[match_len] == m2[match_len]) match_len++;
                size_t offset = (size_t)(ip - ref);
                if (offset > LZ4_MAX_OFFSET) { ip++; continue; }
                if (op + 2 >= olimit) { free(hash_table); return 0; }
                *op++ = (unsigned char)(offset & 0xFF);
                *op++ = (unsigned char)((offset >> 8) & 0xFF);

                if (match_len - LZ4_MIN_MATCH >= 15) {
                    token |= 15;
                    size_t len = match_len - LZ4_MIN_MATCH - 15;
                    while (len >= 255) { if (op >= olimit) { free(hash_table); return 0; } *op++ = 255; len -= 255; }
                    if (op >= olimit) { free(hash_table); return 0; }
                    *op++ = (unsigned char)len;
                } else {
                    token |= (unsigned char)(match_len - LZ4_MIN_MATCH);
                }
                *token_p = token;

                ip += match_len;
                anchor = ip;
                const unsigned char *p = ip - match_len + 1;
                while (p + LZ4_MIN_MATCH <= iend) {
                    uint32_t hh = lz4_hash4(p);
                    hash_table[hh] = (uint32_t)(p - base + 1);
                    p++;
                }
                continue;
            }
        }
        ip++;
    }

    size_t literal_len = iend - anchor;
    if (op + 1 + literal_len + 5 >= olimit) { free(hash_table); return 0; }
    unsigned char *token_p = op++;
    unsigned char token = 0;
    if (literal_len >= 15) {
        token |= 15 << 4;
        size_t len = literal_len - 15;
        while (len >= 255) { *op++ = 255; len -= 255; }
        *op++ = (unsigned char)len;
    } else {
        token |= (unsigned char)(literal_len << 4);
    }
    memcpy(op, anchor, literal_len);
    op += literal_len;
    *token_p = token;

    int csize = (int)(op - (unsigned char*)dst);
    free(hash_table);
    return csize;
}

int lz4_decompress_safe(const char* src, char* dst, int compressedSize, int dstCapacity) {
    const unsigned char *ip = (const unsigned char*)src;
    const unsigned char *iend = ip + compressedSize;
    unsigned char *op = (unsigned char*)dst;
    unsigned char *olimit = (unsigned char*)dst + dstCapacity;

    while (ip < iend) {
        unsigned char token = *ip++;
        size_t literal_len = token >> 4;
        if (literal_len == 15) {
            unsigned char s;
            do {
                if (ip >= iend) return -1;
                s = *ip++;
                literal_len += s;
            } while (s == 255);
        }
        if (op + literal_len > olimit) return -2;
        if (ip + literal_len > iend) return -3;
        memcpy(op, ip, literal_len);
        op += literal_len;
        ip += literal_len;
        if (ip >= iend) break;

        if (ip + 2 > iend) return -4;
        unsigned int offset = ip[0] | (ip[1] << 8);
        ip += 2;
        if (offset == 0) return -5;
        if (op - (unsigned char*)dst < offset) return -6;
        unsigned char *match = op - offset;

        size_t match_len = token & 0x0F;
        if (match_len == 15) {
            unsigned char s;
            do {
                if (ip >= iend) return -7;
                s = *ip++;
                match_len += s;
            } while (s == 255);
        }
        match_len += LZ4_MIN_MATCH;
        if (op + match_len > olimit) return -8;
        while (match_len--) { *op++ = *match++; }
    }
    return (int)(op - (unsigned char*)dst);
}

/* wrappers */
static int my_LZ4_compress_default(const char *src, char *dst, int srcSize, int dstCapacity) {
    return lz4_compress_default(src, dst, srcSize, dstCapacity);
}
static int my_LZ4_decompress_safe(const char *src, char *dst, int compressedSize, int dstCapacity) {
    return lz4_decompress_safe(src, dst, compressedSize, dstCapacity);
}

/* ---------------------------
   CRC32 implementation
   --------------------------- */

static uint32_t crc32_table[256];
static void crc32_init(void) {
    uint32_t poly = 0xEDB88320u;
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t crc = i;
        for (int j = 0; j < 8; ++j) crc = (crc >> 1) ^ ((crc & 1) ? poly : 0);
        crc32_table[i] = crc;
    }
}
static uint32_t crc32_compute(const void *data, size_t len) {
    const uint8_t *p = (const uint8_t*)data;
    uint32_t crc = ~0u;
    for (size_t i = 0; i < len; ++i) crc = (crc >> 8) ^ crc32_table[(crc ^ p[i]) & 0xFF];
    return ~crc;
}

/* ---------------------------
   Archive code with framing
   --------------------------- */

/* Recursively compute total bytes and file count for progress estimation */
static int compute_totals(const char *root, uint64_t *bytes_out, uint64_t *files_out) {
    DIR *d = opendir(root);
    if (!d) return -1;
    struct dirent *ent;
    char path[PATH_MAX];
    while ((ent = readdir(d)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        if ((size_t)snprintf(path, sizeof(path), "%s/%s", root, ent->d_name) >= sizeof(path)) {
            /* path too long; skip */
            continue;
        }
        struct stat st;
        if (lstat(path, &st) != 0) continue;
        if (S_ISDIR(st.st_mode)) {
            compute_totals(path, bytes_out, files_out);
        } else if (S_ISREG(st.st_mode)) {
            *bytes_out += (uint64_t)st.st_size;
            (*files_out)++;
        } else if (S_ISLNK(st.st_mode)) {
            (*files_out)++;
        }
    }
    closedir(d);
    return 0;
}

/* Write a single frame to FILE. Returns bytes written or -1 on error. */
static ssize_t write_frame(FILE *out, const char *inbuf, size_t inlen) {
    int comp_bound = (int)(inlen + (inlen / 255) + 16 + 4);
    char *compbuf = malloc((size_t)comp_bound);
    if (!compbuf) return -1;
    int csize = my_LZ4_compress_default(inbuf, compbuf, (int)inlen, comp_bound);
    if (csize <= 0) { free(compbuf); return -1; }

    uint32_t magic = FRAME_MAGIC;
    uint32_t comp_size = (uint32_t)csize;
    uint32_t orig_size = (uint32_t)inlen;
    uint32_t crc = crc32_compute(compbuf, (size_t)csize);

    if (!write_u32(out, magic) || !write_u32(out, comp_size) || !write_u32(out, orig_size) || !write_u32(out, crc)) {
        free(compbuf); return -1;
    }
    if (fwrite(compbuf, 1, csize, out) != (size_t)csize) { free(compbuf); return -1; }
    free(compbuf);
    return (ssize_t)(4 + 4 + 4 + 4 + csize);
}

/* Archive creation: write entries with framed compressed payloads */
static int archive_create(const char *src_dir, const char *outpath) {
    FILE *out = fopen(outpath, "wb");
    if (!out) {
        fprintf(stderr, "Cannot open output file: %s\n", strerror(errno));
        return -1;
    }
    fwrite(ARCHIVE_MAGIC, 1, sizeof(ARCHIVE_MAGIC)-1, out);

    struct stack_item { char path[PATH_MAX]; char rel[PATH_MAX]; };
    struct stack_item *stack = malloc(1024 * sizeof(*stack));
    if (!stack) { fclose(out); return -1; }
    size_t stack_cap = 1024, stack_len = 0;
    if ((size_t)snprintf(stack[stack_len].path, PATH_MAX, "%s", src_dir) >= PATH_MAX) {
        fprintf(stderr, "Source path too long\n");
        free(stack);
        fclose(out);
        return -1;
    }
    snprintf(stack[stack_len].rel, PATH_MAX, "%s", ".");
    stack_len++;

    const size_t CHUNK = 1 << 20; /* 1 MiB */

    while (stack_len) {
        struct stack_item it = stack[--stack_len];
        DIR *d = opendir(it.path);
        if (!d) {
            record_error("opendir failed: %s (%s)", strerror(errno), it.path);
            continue;
        }
        struct dirent *ent;
        while ((ent = readdir(d)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            char full[PATH_MAX], relpath[PATH_MAX];
            int r1 = snprintf(relpath, PATH_MAX, "%s/%s", it.rel, ent->d_name);
            if (strcmp(it.rel, ".") == 0) r1 = snprintf(relpath, PATH_MAX, "%s", ent->d_name);
            if (r1 < 0 || (size_t)r1 >= PATH_MAX) {
                record_error("relative path too long, skipping: %s/%s", it.rel, ent->d_name);
                continue;
            }
            int r2 = snprintf(full, PATH_MAX, "%s/%s", it.path, ent->d_name);
            if (r2 < 0 || (size_t)r2 >= PATH_MAX) {
                record_error("full path too long, skipping: %s/%s", it.path, ent->d_name);
                continue;
            }
            struct stat st;
            if (lstat(full, &st) != 0) {
                record_error("lstat failed: %s (%s)", strerror(errno), full);
                continue;
            }
            if (S_ISDIR(st.st_mode)) {
                write_u8(out, ET_DIR);
                uint32_t plen = (uint32_t)strlen(relpath);
                write_u32(out, plen);
                fwrite(relpath, 1, plen, out);
                write_u32(out, (uint32_t)st.st_mode);
                write_u64(out, (uint64_t)st.st_mtime);
                write_u64(out, 0);
                write_u64(out, 0);
                if (stack_len + 1 >= stack_cap) {
                    stack_cap *= 2;
                    struct stack_item *tmp = realloc(stack, stack_cap * sizeof(*stack));
                    if (!tmp) { record_error("realloc failed"); closedir(d); free(stack); fclose(out); return -1; }
                    stack = tmp;
                }
                if ((size_t)snprintf(stack[stack_len].path, PATH_MAX, "%s", full) >= PATH_MAX) {
                    record_error("path too long when pushing stack: %s", full);
                    continue;
                }
                if ((size_t)snprintf(stack[stack_len].rel, PATH_MAX, "%s", relpath) >= PATH_MAX) {
                    record_error("relpath too long when pushing stack: %s", relpath);
                    continue;
                }
                stack_len++;
            } else if (S_ISREG(st.st_mode)) {
                FILE *f = fopen(full, "rb");
                if (!f) { record_error("fopen failed: %s (%s)", strerror(errno), full); continue; }
                UI.files_processed++;
                /* safe copy to UI.current_file */
                snprintf(UI.current_file, sizeof(UI.current_file), "%s", relpath);
                ui_draw();

                write_u8(out, ET_FILE);
                uint32_t plen = (uint32_t)strlen(relpath);
                write_u32(out, plen);
                fwrite(relpath, 1, plen, out);
                write_u32(out, (uint32_t)st.st_mode);
                write_u64(out, (uint64_t)st.st_mtime);
                write_u64(out, (uint64_t)st.st_size);
                long comp_size_pos = ftell(out);
                write_u64(out, 0); /* placeholder */

                uint64_t total_comp_bytes = 0;
                char *inbuf = malloc(CHUNK);
                if (!inbuf) { fclose(f); record_error("malloc failed for %s", full); continue; }
                size_t r;
                while ((r = fread(inbuf, 1, CHUNK, f)) > 0) {
                    ssize_t wrote = write_frame(out, inbuf, r);
                    if (wrote < 0) {
                        record_error("frame write failed for %s", full);
                        break;
                    }
                    total_comp_bytes += (uint64_t)wrote;
                    UI.processed_bytes += (uint64_t)r;
                    ui_draw();
                }
                free(inbuf);
                fclose(f);

                long cur = ftell(out);
                fseek(out, comp_size_pos, SEEK_SET);
                write_u64(out, total_comp_bytes);
                fseek(out, cur, SEEK_SET);
            } else if (S_ISLNK(st.st_mode)) {
                char target[PATH_MAX];
                ssize_t tlen = readlink(full, target, sizeof(target)-1);
                if (tlen < 0) { record_error("readlink failed: %s (%s)", strerror(errno), full); continue; }
                target[tlen] = '\0';
                write_u8(out, ET_SYMLINK);
                uint32_t plen = (uint32_t)strlen(relpath);
                write_u32(out, plen);
                fwrite(relpath, 1, plen, out);
                write_u32(out, (uint32_t)st.st_mode);
                write_u64(out, (uint64_t)st.st_mtime);
                long comp_size_pos = ftell(out);
                write_u64(out, 0); /* orig_size placeholder */
                write_u64(out, 0); /* comp_size placeholder */
                ssize_t wrote = write_frame(out, target, (size_t)tlen);
                if (wrote < 0) { record_error("frame write failed symlink %s", full); continue; }
                uint64_t total_comp_bytes = (uint64_t)wrote;
                long cur = ftell(out);
                fseek(out, comp_size_pos - 8, SEEK_SET);
                write_u64(out, (uint64_t)tlen);
                write_u64(out, total_comp_bytes);
                fseek(out, cur, SEEK_SET);
                UI.files_processed++;
                UI.processed_bytes += (uint64_t)tlen;
                snprintf(UI.current_file, sizeof(UI.current_file), "%s", relpath);
                ui_draw();
            } else {
                record_error("Skipping unsupported file type: %s", full);
            }
        }
        closedir(d);
    }

    write_u8(out, ET_END);
    fclose(out);
    free(stack);
    return 0;
}

/* Helper: try to find next frame magic in buffer starting at pos; returns index or -1 */
static ssize_t find_next_frame_magic(const unsigned char *buf, size_t buf_len, size_t start) {
    for (size_t i = start; i + 4 <= buf_len; ++i) {
        uint32_t v = buf[i] | (buf[i+1]<<8) | (buf[i+2]<<16) | (buf[i+3]<<24);
        if (v == FRAME_MAGIC) return (ssize_t)i;
    }
    return -1;
}

/* ---------------------------
   New: archive scan mode
   ---------------------------
   Scans an archive and reports corrupted frames without extracting.
*/

static int archive_scan(const char *inpath) {
    FILE *in = fopen(inpath, "rb");
    if (!in) {
        fprintf(stderr, "Cannot open archive: %s\n", strerror(errno));
        return -1;
    }
    char magic[sizeof(ARCHIVE_MAGIC)];
    if (fread(magic, 1, sizeof(ARCHIVE_MAGIC)-1, in) != sizeof(ARCHIVE_MAGIC)-1) {
        fprintf(stderr, "Invalid archive\n");
        fclose(in);
        return -1;
    }
    if (memcmp(magic, ARCHIVE_MAGIC, sizeof(ARCHIVE_MAGIC)-1) != 0) {
        fprintf(stderr, "Bad magic\n");
        fclose(in);
        return -1;
    }

    uint64_t total_entries = 0;
    uint64_t total_frames = 0;
    uint64_t total_bad_frames = 0;
    uint64_t total_truncated_frames = 0;
    uint64_t total_crc_mismatch = 0;
    uint64_t total_resyncs = 0;

    printf("Scanning archive: %s\n", inpath);
    printf("------------------------------------------------------------\n");

    while (1) {
        uint8_t type;
        if (!read_u8(in, &type)) break;
        if (type == ET_END) break;
        uint32_t plen;
        if (!read_u32(in, &plen)) break;
        if (plen >= PATH_MAX) { fprintf(stderr, "path too long in archive\n"); break; }
        char rel[PATH_MAX];
        if (fread(rel, 1, plen, in) != plen) break;
        rel[plen] = '\0';
        uint32_t mode;
        uint64_t mtime, orig_size, comp_size;
        read_u32(in, &mode);
        read_u64(in, &mtime);
        read_u64(in, &orig_size);
        read_u64(in, &comp_size);

        total_entries++;
        if (type == ET_DIR) {
            printf("DIR  : %s\n", rel);
            continue;
        } else if (type == ET_FILE || type == ET_SYMLINK) {
            printf("%s: %s  (orig %" PRIu64 " bytes, framed %" PRIu64 " bytes)\n",
                   type == ET_FILE ? "FILE" : "SYML", rel, orig_size, comp_size);
            if (comp_size == 0) {
                printf("  [no frames]\n");
                continue;
            }
            unsigned char *buf = malloc((size_t)comp_size);
            if (!buf) { fprintf(stderr, "malloc failed during scan\n"); fclose(in); return -1; }
            if (fread(buf, 1, (size_t)comp_size, in) != (size_t)comp_size) {
                fprintf(stderr, "read failed while scanning %s\n", rel);
                free(buf);
                break;
            }

            size_t pos = 0;
            size_t entry_frames = 0;
            size_t entry_bad = 0;
            size_t entry_trunc = 0;
            size_t entry_crc = 0;
            size_t entry_resyncs = 0;

            while (pos + 16 <= (size_t)comp_size) {
                uint32_t magic_v = buf[pos] | (buf[pos+1]<<8) | (buf[pos+2]<<16) | (buf[pos+3]<<24);
                if (magic_v != FRAME_MAGIC) {
                    ssize_t next = find_next_frame_magic(buf, (size_t)comp_size, pos+1);
                    if (next < 0) {
                        entry_bad++;
                        entry_trunc++;
                        total_bad_frames++;
                        total_truncated_frames++;
                        printf("  [corrupt] frame magic lost at pos %zu, cannot resync further\n", pos);
                        break;
                    } else {
                        entry_bad++;
                        entry_resyncs++;
                        total_bad_frames++;
                        total_resyncs++;
                        printf("  [resync] frame magic missing at pos %zu, resync to %zd\n", pos, next);
                        pos = (size_t)next;
                        continue;
                    }
                }
                if (pos + 16 > (size_t)comp_size) {
                    entry_bad++;
                    entry_trunc++;
                    total_bad_frames++;
                    total_truncated_frames++;
                    printf("  [truncated] incomplete frame header at pos %zu\n", pos);
                    break;
                }
                uint32_t f_comp = buf[pos+4] | (buf[pos+5]<<8) | (buf[pos+6]<<16) | (buf[pos+7]<<24);
                uint32_t f_crc  = buf[pos+12] | (buf[pos+13]<<8) | (buf[pos+14]<<16) | (buf[pos+15]<<24);
                if (pos + 16 + f_comp > (size_t)comp_size) {
                    entry_bad++;
                    entry_trunc++;
                    total_bad_frames++;
                    total_truncated_frames++;
                    printf("  [truncated] frame at pos %zu claims comp %u bytes but only %zu available\n",
                           pos, f_comp, (size_t)comp_size - pos - 16);
                    break;
                }
                unsigned char *comp_ptr = buf + pos + 16;
                uint32_t crc = crc32_compute(comp_ptr, f_comp);
                entry_frames++;
                total_frames++;
                if (crc != f_crc) {
                    entry_bad++;
                    entry_crc++;
                    total_bad_frames++;
                    total_crc_mismatch++;
                    printf("  [crc mismatch] frame %zu at pos %zu: expected 0x%08x got 0x%08x\n",
                           entry_frames, pos, f_crc, crc);
                    /* attempt resync to next frame */
                    ssize_t next = find_next_frame_magic(buf, (size_t)comp_size, pos+4);
                    if (next < 0) {
                        printf("    cannot resync after CRC error\n");
                        break;
                    } else {
                        entry_resyncs++;
                        total_resyncs++;
                        printf("    resyncing to %zd\n", next);
                        pos = (size_t)next;
                        continue;
                    }
                } else {
                    /* frame OK */
                    pos += 16 + f_comp;
                }
            }

            printf("  frames: %zu  bad: %zu  truncated: %zu  crc_mismatch: %zu  resyncs: %zu\n",
                   entry_frames, entry_bad, entry_trunc, entry_crc, entry_resyncs);

            free(buf);
        } else {
            printf("UNKNOWN entry type %u\n", type);
            /* skip if possible */
        }
    }

    printf("------------------------------------------------------------\n");
    printf("Scan summary:\n");
    printf("  entries scanned : %" PRIu64 "\n", total_entries);
    printf("  frames checked  : %" PRIu64 "\n", total_frames);
    printf("  bad frames      : %" PRIu64 "\n", total_bad_frames);
    printf("  truncated frames: %" PRIu64 "\n", total_truncated_frames);
    printf("  crc mismatches  : %" PRIu64 "\n", total_crc_mismatch);
    printf("  resync attempts : %" PRIu64 "\n", total_resyncs);

    fclose(in);
    return 0;
}

/* Archive extraction with frame verification and resync attempts */
static int archive_extract(const char *inpath, const char *outdir) {
    FILE *in = fopen(inpath, "rb");
    if (!in) {
        fprintf(stderr, "Cannot open archive: %s\n", strerror(errno));
        return -1;
    }
    char magic[sizeof(ARCHIVE_MAGIC)];
    if (fread(magic, 1, sizeof(ARCHIVE_MAGIC)-1, in) != sizeof(ARCHIVE_MAGIC)-1) {
        fprintf(stderr, "Invalid archive\n");
        fclose(in);
        return -1;
    }
    if (memcmp(magic, ARCHIVE_MAGIC, sizeof(ARCHIVE_MAGIC)-1) != 0) {
        fprintf(stderr, "Bad magic\n");
        fclose(in);
        return -1;
    }
    struct stat st;
    if (stat(outdir, &st) != 0) {
        if (mkdir(outdir, 0755) != 0) {
            fprintf(stderr, "Cannot create output dir: %s\n", strerror(errno));
            fclose(in);
            return -1;
        }
    }

    while (1) {
        uint8_t type;
        if (!read_u8(in, &type)) break;
        if (type == ET_END) break;
        uint32_t plen;
        if (!read_u32(in, &plen)) break;
        if (plen >= PATH_MAX) { record_error("path too long in archive"); break; }
        char rel[PATH_MAX];
        if (fread(rel, 1, plen, in) != plen) break;
        rel[plen] = '\0';
        uint32_t mode;
        uint64_t mtime, orig_size, comp_size;
        read_u32(in, &mode);
        read_u64(in, &mtime);
        read_u64(in, &orig_size);
        read_u64(in, &comp_size);
        char outpath[PATH_MAX];
        if ((size_t)snprintf(outpath, PATH_MAX, "%s/%s", outdir, rel) >= PATH_MAX) {
            record_error("output path too long, skipping: %s/%s", outdir, rel);
            /* skip comp_size bytes if present */
            if (comp_size) fseek(in, (long)comp_size, SEEK_CUR);
            continue;
        }
        /* ensure parent directories exist */
        char tmp[PATH_MAX];
        strncpy(tmp, outpath, PATH_MAX-1);
        tmp[PATH_MAX-1] = '\0';
        char *p = dirname(tmp);
        /* create directories recursively */
        char accum[PATH_MAX] = {0};
        const char *s = p;
        if (*s == '/') { strcpy(accum, "/"); s++; }
        while (*s) {
            const char *slash = strchr(s, '/');
            size_t len = slash ? (size_t)(slash - s) : strlen(s);
            if (accum[0] == '\0') snprintf(accum, PATH_MAX, "%.*s", (int)len, s);
            else { strncat(accum, "/", PATH_MAX - strlen(accum) - 1); strncat(accum, s, len); }
            if (stat(accum, &st) != 0) {
                mkdir(accum, 0755);
            }
            if (!slash) break;
            s = slash + 1;
        }

        if (type == ET_DIR) {
            mkdir(outpath, mode ? mode : 0755);
            utime(outpath, NULL);
        } else if (type == ET_FILE) {
            if (comp_size == 0) continue;
            unsigned char *buf = malloc((size_t)comp_size);
            if (!buf) { record_error("malloc failed during extract"); fclose(in); return -1; }
            if (fread(buf, 1, (size_t)comp_size, in) != (size_t)comp_size) { free(buf); record_error("read failed"); break; }

            size_t pos = 0;
            FILE *f = fopen(outpath, "wb");
            if (!f) { free(buf); record_error("fopen failed: %s (%s)", strerror(errno), outpath); continue; }

            while (pos + 16 <= (size_t)comp_size) {
                uint32_t magic_v = buf[pos] | (buf[pos+1]<<8) | (buf[pos+2]<<16) | (buf[pos+3]<<24);
                if (magic_v != FRAME_MAGIC) {
                    ssize_t next = find_next_frame_magic(buf, (size_t)comp_size, pos+1);
                    if (next < 0) {
                        record_error("Frame magic lost in %s at pos %zu", rel, pos);
                        break;
                    } else {
                        record_error("Resyncing in %s from %zu to %zd", rel, pos, next);
                        pos = (size_t)next;
                        continue;
                    }
                }
                uint32_t f_comp = buf[pos+4] | (buf[pos+5]<<8) | (buf[pos+6]<<16) | (buf[pos+7]<<24);
                uint32_t f_orig = buf[pos+8] | (buf[pos+9]<<8) | (buf[pos+10]<<16) | (buf[pos+11]<<24);
                uint32_t f_crc  = buf[pos+12] | (buf[pos+13]<<8) | (buf[pos+14]<<16) | (buf[pos+15]<<24);
                if (pos + 16 + f_comp > (size_t)comp_size) {
                    record_error("Truncated frame in %s", rel);
                    break;
                }
                unsigned char *comp_ptr = buf + pos + 16;
                uint32_t crc = crc32_compute(comp_ptr, f_comp);
                if (crc != f_crc) {
                    record_error("Frame CRC mismatch in %s at pos %zu", rel, pos);
                    ssize_t next = find_next_frame_magic(buf, (size_t)comp_size, pos+4);
                    if (next < 0) { break; }
                    pos = (size_t)next;
                    continue;
                }
                char *outbuf = malloc((size_t)f_orig + 1);
                if (!outbuf) { record_error("malloc failed decompress"); break; }
                int dsize = my_LZ4_decompress_safe((char*)comp_ptr, outbuf, (int)f_comp, (int)f_orig);
                if (dsize < 0) { free(outbuf); record_error("LZ4 decompress failed for %s", rel); ssize_t next = find_next_frame_magic(buf, (size_t)comp_size, pos+4); if (next < 0) break; pos = (size_t)next; continue; }
                fwrite(outbuf, 1, (size_t)dsize, f);
                free(outbuf);
                pos += 16 + f_comp;
                UI.files_processed++;
                UI.processed_bytes += (uint64_t)f_orig;
                snprintf(UI.current_file, sizeof(UI.current_file), "%s", rel);
                ui_draw();
            }
            fclose(f);
            free(buf);
            chmod(outpath, mode);
            struct utimbuf utb;
            utb.actime = time(NULL);
            utb.modtime = (time_t)mtime;
            utime(outpath, &utb);
        } else if (type == ET_SYMLINK) {
            if (comp_size == 0) continue;
            unsigned char *buf = malloc((size_t)comp_size);
            if (!buf) { record_error("malloc failed symlink"); fclose(in); return -1; }
            if (fread(buf, 1, (size_t)comp_size, in) != (size_t)comp_size) { free(buf); record_error("read failed"); break; }
            if (comp_size < 16) { free(buf); record_error("symlink frame too small %s", rel); continue; }
            uint32_t magic_v = buf[0] | (buf[1]<<8) | (buf[2]<<16) | (buf[3]<<24);
            if (magic_v != FRAME_MAGIC) { free(buf); record_error("symlink frame magic missing %s", rel); continue; }
            uint32_t f_comp = buf[4] | (buf[5]<<8) | (buf[6]<<16) | (buf[7]<<24);
            uint32_t f_orig = buf[8] | (buf[9]<<8) | (buf[10]<<16) | (buf[11]<<24);
            uint32_t f_crc  = buf[12] | (buf[13]<<8) | (buf[14]<<16) | (buf[15]<<24);
            if (16 + f_comp != comp_size) { free(buf); record_error("symlink frame size mismatch %s", rel); continue; }
            unsigned char *comp_ptr = buf + 16;
            uint32_t crc = crc32_compute(comp_ptr, f_comp);
            if (crc != f_crc) { free(buf); record_error("symlink CRC mismatch %s", rel); continue; }
            char target[PATH_MAX];
            int dsize = my_LZ4_decompress_safe((char*)comp_ptr, target, (int)f_comp, (int)f_orig);
            if (dsize < 0) { free(buf); record_error("symlink decompress failed %s", rel); continue; }
            target[dsize] = '\0';
            unlink(outpath);
            if (symlink(target, outpath) != 0) {
                record_error("symlink failed: %s (%s)", strerror(errno), outpath);
            }
            free(buf);
            UI.files_processed++;
            UI.processed_bytes += (uint64_t)f_orig;
            snprintf(UI.current_file, sizeof(UI.current_file), "%s", rel);
            ui_draw();
        } else {
            record_error("Unknown entry type in archive");
            break;
        }
    }

    fclose(in);
    return 0;
}

/* Helper: build archive filename from cwd name and timestamp */
static void build_archive_name(const char *src_dir, char *out, size_t outlen) {
    char base[PATH_MAX];
    const char *p = strrchr(src_dir, '/');
    if (!p) p = src_dir;
    else p++;
    /* safe copy of base */
    if ((size_t)snprintf(base, sizeof(base), "%s", p) >= sizeof(base)) {
        /* truncate base */
        base[sizeof(base)-1] = '\0';
    }
    time_t t = time(NULL);
    struct tm tm;
    localtime_r(&t, &tm);
    char timestr[64];
    strftime(timestr, sizeof(timestr), "%Y%m%d_%H%M%S", &tm);
    int rv = snprintf(out, outlen, "%s_%s.lz4", base, timestr);
    if (rv < 0) {
        out[0] = '\0';
    } else if ((size_t)rv >= outlen) {
        /* truncated; ensure null termination */
        out[outlen-1] = '\0';
    }
}

/* Main */
int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage:\n  %s -c <dir>\n  %s -d <archive.lz4>\n  %s -s <archive.lz4>\n", argv[0], argv[0], argv[0]);
        return 1;
    }
    crc32_init();
    UI.term_width = 80;
    UI.term_height = 24;
    UI.total_bytes = 0;
    UI.processed_bytes = 0;
    UI.start_time = time(NULL);
    UI.current_file[0] = '\0';
    UI.files_processed = 0;
    UI.files_total = 0;
    UI.errors = 0;
    UI.last_error[0] = '\0';

    if (strcmp(argv[1], "-c") == 0) {
        const char *src = argv[2];
        char outname[PATH_MAX];
        build_archive_name(src, outname, sizeof(outname));
        compute_totals(src, &UI.total_bytes, &UI.files_total);
        ui_draw();
        int r = archive_create(src, outname);
        UI.current_file[0] = '\0';
        ui_draw();
        if (r == 0) {
            printf("\nArchive created: %s\n", outname);
            return 0;
        } else {
            fprintf(stderr, "Archive creation failed\n");
            return 2;
        }
    } else if (strcmp(argv[1], "-d") == 0) {
        const char *infile = argv[2];
        char base[PATH_MAX];
        strncpy(base, infile, sizeof(base)-1);
        base[sizeof(base)-1] = '\0';
        size_t len = strlen(base);
        if (len > 4 && strcmp(base + len - 4, ".lz4") == 0) base[len - 4] = '\0';
        ui_draw();
        int r = archive_extract(infile, base);
        UI.current_file[0] = '\0';
        ui_draw();
        if (r == 0) {
            printf("\nArchive extracted to: %s\n", base);
            return 0;
        } else {
            fprintf(stderr, "Archive extraction failed\n");
            return 3;
        }
    } else if (strcmp(argv[1], "-s") == 0) {
        const char *infile = argv[2];
        int r = archive_scan(infile);
        if (r == 0) return 0;
        else return 4;
    } else {
        fprintf(stderr, "Unknown option\n");
        return 1;
    }
}
