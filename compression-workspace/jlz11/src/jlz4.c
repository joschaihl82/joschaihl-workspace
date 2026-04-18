/*
  jlz4.c - compact lz4-style compressor/decompressor with archive, listing and progress
  - single-file, no external libs (POSIX)
  - supports:
      ./jlz4 -c <input> [<output>]    (if input is a directory and no output given, creates archive named <folder>-yyyymmddhhmm.lz4)
      ./jlz4 -d <input> [<output>]    (decompress archive or single file)
      ./jlz4 -l <archive.lz4>         (list archive contents)
  - archive format: sequence of entries:
      [uint32 name_len][name bytes][uint64 orig_size][uint32 comp_size][comp_size bytes]
    where comp_size bytes are the same block-framed compressed bytes used by this tool.
*/

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
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
#include <sys/param.h>
#include <sys/uio.h>
#include <stdbool.h>

/* tunables */
#define BLOCK_SIZE (1 << 20)      /* 1 MiB input blocks */
/* block_size + 64kb is a safe margin for lz4 worst-case compression expansion */
#define MAX_COMP_SIZE (BLOCK_SIZE + (64 * 1024))
#define WINDOW_SIZE (1 << 16)     /* 64 KiB sliding window */
#define MIN_MATCH 4
#define HASH_SIZE (1 << 16)
#define HASH_SHIFT (32 - 16)

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/* progress globals */
static uint64_t g_total_bytes = 0;
static uint64_t g_processed_bytes = 0;
static struct timeval g_start_time;
static int g_blocks_processed = 0;

/* per-file status for progress display */
static const char *g_current_file = NULL;
static uint64_t g_file_orig_size = 0;
static uint64_t g_file_comp_size = 0;
static int g_lines_printed = 0;

/* --- command structure --- */

typedef enum {
    MODE_NONE,
    MODE_COMPRESS,
    MODE_DECOMPRESS,
    MODE_LIST
} command_mode;

typedef struct {
    command_mode mode;
    const char *inpath;
    const char *outpath;
} command_args;

/* --- archive entry metadata for extraction ordering --- */

typedef struct {
    char name[PATH_MAX];
    uint64_t orig_size;
    uint32_t comp_size;
    long data_pos; // file offset where compressed data starts
    int is_dir;    // 1 if directory, 0 if file
} entry_metadata;

static entry_metadata *g_entries = NULL;
static size_t g_num_entries = 0;
static size_t g_max_entries = 0;

/* --- terminal utilities --- */

static int get_terminal_width(void) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0) {
        return (int)ws.ws_col;
    }
    return 80;
}

/* --- general utilities --- */

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
        /* check if 'a' is just a drive letter on windows or similar */
        if (la > 0 && a[la - 1] == ':') {
             snprintf(out, outlen, "%s%s", a, b);
        } else {
             snprintf(out, outlen, "%s/%s", a, b);
        }
    }
}

/*
 * checks if the file is a regular, readable file.
 * returns 0 on success, -1 on failure (or if not regular).
 */
static int check_input_file_readability(const char *path, mode_t st_mode) {
    if (!S_ISREG(st_mode)) {
        /* directories are handled recursively by the caller, other types are skipped. */
        return -1;
    }
    if (access(path, R_OK) != 0) {
        /* warning, not critical failure unless file is required */
        fprintf(stderr, "warning: input '%s' is not readable: %s (skipping)\n", path, strerror(errno));
        return -1;
    }
    return 0;
}

/*
 * ensures parent path exists and is writable, creating intermediate dirs if necessary.
 * returns 0 on success, -1 on failure.
 */
static int ensure_output_dir_writable(const char *path) {
    char tmp[PATH_MAX];
    if (strlen(path) >= sizeof(tmp)) {
        fprintf(stderr, "error: path '%s' too long\n", path);
        return -1;
    }
    strncpy(tmp, path, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';

    /* find the parent directory */
    char *last_slash = strrchr(tmp, '/');
    if (!last_slash) return 0; /* file in current dir. */
    *last_slash = '\0'; /* parent directory path */
    
    if (strlen(tmp) == 0) return 0; /* path was something like /file.txt, parent is root */

    /* traverse and create intermediate directories */
    char accum[PATH_MAX] = {0};
    const char *p = tmp;
    
    /* handle leading slash */
    if (*p == '/') { 
        strcat(accum, "/"); 
        p++; 
    }

    while (*p) {
        const char *slash = strchr(p, '/');
        size_t len = slash ? (size_t)(slash - p) : strlen(p);
        
        if (len == 0) { 
            if (!slash) break; 
            p = slash + 1; 
            continue; 
        }

        /* ensure we don't exceed buffer size */
        if (strlen(accum) + len + 2 >= PATH_MAX) {
            fprintf(stderr, "error: parent path component too long\n");
            return -1;
        }

        /* append component */
        if (strlen(accum) > 0 && accum[strlen(accum)-1] != '/') strcat(accum, "/");
        strncat(accum, p, len);

        struct stat st;
        if (stat(accum, &st) == 0) {
            if (!S_ISDIR(st.st_mode)) {
                fprintf(stderr, "error: path component '%s' is a file, not a directory\n", accum);
                return -1;
            }
        } else {
            /* directory doesn't exist, create it */
            if (mkdir(accum, 0755) != 0) {
                if (errno != EEXIST) {
                     fprintf(stderr, "error: failed to create parent directory '%s': %s\n", accum, strerror(errno));
                     return -1;
                }
            }
        }

        if (slash) p = slash + 1; else break;
    }

    /* final check on the created parent directory */
    if (strlen(accum) > 0 && access(accum, W_OK) != 0) {
        fprintf(stderr, "error: output directory '%s' is not writable: %s\n", accum, strerror(errno));
        return -1;
    }

    return 0;
}

static int write_all_fd(int fd, const void *buf, size_t size) {
    const uint8_t *p = buf;
    size_t left = size;
    while (left) {
        ssize_t w = write(fd, p, left);
        if (w < 0) {
            if (errno == EINVAL) { /* handle non-blocking/interrupts */ continue; }
            if (errno == EINTR) { continue; }
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
    const char *units[] = {"b", "kb", "mb", "gb", "tb"};
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

/* --- progress bar --- */

static void progress_update(uint64_t processed, uint64_t total) {
    /* drosselung: nur alle 10 blöcke oder bei start/ende aktualisieren */
    if (g_blocks_processed % 10 != 0 && processed < total) {
        return;
    }
    
    g_processed_bytes = processed;
    g_total_bytes = total;

    if (g_lines_printed > 0) {
        printf("\033[%dA", g_lines_printed); /* geht hoch */
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
        printf("\033[Kprocessing: %s\n", g_current_file);
    } else {
        printf("\033[K\n");
    }
    g_lines_printed++;

    printf("\033[Kelapsed: %s | eta: %s | speed: %s\n", elapsed_s, eta_s, hspeed);
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
        printf("\033[Kprocessed: %s / %s | current file savings: %.2f%%\n", hproc, htot, file_ratio);
    } else {
        printf("\033[Kprocessed: %s / %s | ratio: n/a\n", hproc, htot);
    }
    g_lines_printed++;

    fflush(stdout);

    if (processed >= total) {
        for (int i = 0; i < g_lines_printed; ++i) printf("\n");
        g_lines_printed = 0;
    }
}

/* --- lz4-like compressor/decompressor --- */

static inline uint32_t hash4(const uint8_t *p) {
    uint32_t v;
    /* uses unaligned memory access, common in fast compressors */
    memcpy(&v, p, 4); 
    v = v * 2654435761u;
    return (v >> HASH_SHIFT) & (HASH_SIZE - 1);
}

static size_t compress_block(const uint8_t *in, size_t in_size, uint8_t *out, size_t out_capacity) {
    if (out_capacity < 12 || in_size > BLOCK_SIZE) return 0;
    uint8_t *out_start = out;
    memcpy(out, "lz4b", 4); out += 4;
    uint32_t orig = (uint32_t)in_size;
    memcpy(out, &orig, 4); out += 4;
    uint8_t *comp_size_ptr = out; out += 4;

    /* initialize hash table with 0xffffffff (invalid reference) */
    uint32_t *hash_table = calloc(HASH_SIZE, sizeof(uint32_t));
    if (!hash_table) return 0;
    for (size_t i = 0; i < HASH_SIZE; ++i) hash_table[i] = 0xffffffffu;

    const uint8_t *ip = in;
    const uint8_t *in_end = in + in_size;
    const uint8_t *anchor = ip;

    while (ip + MIN_MATCH <= in_end) {
        uint32_t h = hash4(ip);
        uint32_t ref = hash_table[h];
        hash_table[h] = (uint32_t)(ip - in);

        size_t match_len = 0;
        size_t match_off = 0;
        if (ref != 0xffffffffu) {
            const uint8_t *rp = in + ref;
            /* check for valid match reference within the sliding window */
            if (ip > rp && (ip - rp) <= WINDOW_SIZE) {
                const uint8_t *p1 = ip;
                const uint8_t *p2 = rp;
                const uint8_t *mend = in_end;
                /* find match length */
                while (p1 < mend && *p1 == *p2) { p1++; p2++; }
                match_len = (size_t)(p1 - ip);
                if (match_len >= MIN_MATCH) match_off = (size_t)(ip - rp);
                else match_len = 0;
            }
        }

        if (match_len >= MIN_MATCH) {
            size_t literal_len = (size_t)(ip - anchor);
            
            /* overflow check for output buffer */
            if ((size_t)(out - out_start) + 1 + literal_len + 2 + (match_len / 255) + (literal_len / 255) + 5 >= out_capacity) { 
                free(hash_table); 
                return 0; 
            }

            uint8_t token = (uint8_t)((literal_len >= 15 ? 15 : literal_len) << 4);
            size_t ml = match_len - MIN_MATCH;
            token |= (uint8_t)(ml >= 15 ? 15 : ml);
            
            *out++ = token;
            
            /* literals length extension */
            if (literal_len >= 15) {
                size_t rem = literal_len - 15;
                while (rem >= 255) { *out++ = 255; rem -= 255; }
                *out++ = (uint8_t)rem;
            }
            
            /* literals copy */
            if (literal_len) { memcpy(out, anchor, literal_len); out += literal_len; }
            
            /* offset (2 bytes, little endian) */
            uint16_t off16 = (uint16_t)match_off;
            *out++ = (uint8_t)(off16 & 0xff);
            *out++ = (uint8_t)((off16 >> 8) & 0xff);
            
            /* match length extension */
            if (ml >= 15) {
                size_t rem = ml - 15;
                while (rem >= 255) { *out++ = 255; rem -= 255; }
                *out++ = (uint8_t)rem;
            }
            
            /* update ip and anchor */
            ip += match_len;
            anchor = ip;
            
            /* hash update for previous matches that overlap with current match */
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

    /* last literals */
    size_t literal_len = (size_t)(in_end - anchor);
    if ((size_t)(out - out_start) + 1 + literal_len + (literal_len / 255) + 5 >= out_capacity) { free(hash_table); return 0; }
    uint8_t token = (uint8_t)((literal_len >= 15 ? 15 : literal_len) << 4);
    *out++ = token;
    if (literal_len >= 15) {
        size_t rem = literal_len - 15;
        while (rem >= 255) { *out++ = 255; rem -= 255; }
        *out++ = (uint8_t)rem;
    }
    if (literal_len) { memcpy(out, anchor, literal_len); out += literal_len; }

    /* write final compressed size */
    uint32_t comp_size_val = (uint32_t)(out - comp_size_ptr - 4);
    memcpy(comp_size_ptr, &comp_size_val, 4);
    size_t total = (size_t)(out - out_start);
    free(hash_table);
    return total;
}

static size_t decompress_block(const uint8_t *in, size_t in_size, uint8_t *out, size_t out_capacity) {
    if (in_size < 12) return 0;
    if (memcmp(in, "lz4b", 4) != 0) return 0;
    uint32_t orig;
    memcpy(&orig, in + 4, 4);
    uint32_t comp_size;
    memcpy(&comp_size, in + 8, 4);
    
    /* robustness: check header against input size */
    if (comp_size + 12 != in_size) return 0;
    
    /* robustness: check original size against output capacity */
    if (orig > out_capacity) return 0; 
    
    size_t out_pos = 0;
    const uint8_t *ip = in + 12;
    const uint8_t *iend = in + in_size;
    
    while (ip < iend) {
        if (ip + 1 > iend) return 0; /* next token not present */
        uint8_t token = *ip++;
        size_t literal_len = (token >> 4);
        
        /* decode literal length extension */
        if (literal_len == 15) {
            uint8_t s;
            do {
                if (ip >= iend) return 0;
                s = *ip++;
                literal_len += s;
            } while (s == 255);
        }
        
        /* copy literals */
        if (ip + literal_len > iend) return 0;
        if (out_pos + literal_len > out_capacity) return 0;
        memcpy(out + out_pos, ip, literal_len);
        ip += literal_len;
        out_pos += literal_len;

        if (ip >= iend) break; /* end of block after literals (last block) */
        
        /* decode offset */
        if (ip + 2 > iend) return 0;
        uint16_t offset;
        memcpy(&offset, ip, 2);
        ip += 2;
        
        /* decode match length extension */
        size_t match_len = (token & 0x0f);
        if (match_len == 15) {
            uint8_t s;
            do {
                if (ip >= iend) return 0;
                s = *ip++;
                match_len += s;
            } while (s == 255);
        }
        match_len += MIN_MATCH;
        
        /* match safety check */
        if (offset == 0 || offset > out_pos) return 0;
        size_t ref = out_pos - offset;
        if (out_pos + match_len > out_capacity) return 0;
        
        /* copy match data (handle overlapping copy) */
        if (ref + match_len <= out_pos) {
            /* non-overlapping, use memcpy for speed */
            memcpy(out + out_pos, out + ref, match_len);
        } else {
            /* overlapping, must use byte-by-byte copy */
            for (size_t i = 0; i < match_len; ++i) {
                out[out_pos + i] = out[ref + i];
            }
        }
        out_pos += match_len;
    }

    if (out_pos != orig) return 0;
    return out_pos;
}

/* --- block streaming helpers (little-endian access) --- */

static int fwrite_u32(FILE *f, uint32_t v) {
    uint8_t b[4];
    b[0] = v & 0xff; b[1] = (v >> 8) & 0xff; b[2] = (v >> 16) & 0xff; b[3] = (v >> 24) & 0xff;
    return fwrite(b, 1, 4, f) == 4 ? 0 : -1;
}
static int fwrite_u64(FILE *f, uint64_t v) {
    uint8_t b[8];
    for (int i = 0; i < 8; ++i) b[i] = (v >> (8 * i)) & 0xff;
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

/* --- compression / archive routines --- */

/* creates a single file entry in the archive, performs compression */
static int compress_archive_entry_data(FILE *archive, const char *relname, const char *inpath, uint64_t *out_comp_size) {
    g_current_file = relname;
    g_file_comp_size = 0;

    FILE *fin = fopen(inpath, "rb");
    if (!fin) { fprintf(stderr, "open input '%s' failed: %s\n", inpath, strerror(errno)); return -1; }

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
    if (fwrite_u32(archive, 0) != 0) { fclose(fin); return -1; } /* comp_size placeholder */

    uint8_t *inbuf = malloc(BLOCK_SIZE);
    uint8_t *outbuf = malloc(MAX_COMP_SIZE);
    if (!inbuf || !outbuf) { 
        fprintf(stderr, "memory allocation failed\n");
        fclose(fin); 
        free(inbuf); 
        free(outbuf); 
        return -1; 
    }

    uint32_t total_comp = 0;
    size_t r;
    while ((r = fread_all(fin, inbuf, BLOCK_SIZE)) > 0) {
        size_t comp = compress_block(inbuf, r, outbuf, MAX_COMP_SIZE);
        if (comp == 0) { 
            fprintf(stderr, "compression failed for block\n"); 
            free(inbuf); 
            free(outbuf); 
            fclose(fin); 
            return -1; 
        }
        uint32_t block_total = (uint32_t)comp;

        if (fwrite_u32(archive, block_total) != 0) { 
            free(inbuf); 
            free(outbuf); 
            fclose(fin); 
            return -1; 
        }

        if (fwrite(outbuf, 1, comp, archive) != comp) { 
            free(inbuf); 
            free(outbuf); 
            fclose(fin); 
            return -1; 
        }
        total_comp += 4 + block_total;
        g_file_comp_size = total_comp;
        g_processed_bytes += r;
        g_blocks_processed++;
        progress_update(g_processed_bytes, g_total_bytes);
    }

    /* check for read errors not at eof */
    if (ferror(fin)) {
        fprintf(stderr, "read error on input file '%s': %s\n", inpath, strerror(errno));
        free(inbuf);
        free(outbuf);
        fclose(fin);
        return -1;
    }

    long cur = ftell(archive);
    if (cur < 0) { free(inbuf); free(outbuf); fclose(fin); return -1; }
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

/* creates a directory entry in the archive (orig_size=0, comp_size=0) */
static int write_directory_entry(FILE *archive, const char *relname) {
    uint32_t name_len = (uint32_t)strlen(relname);
    if (name_len == 0 || name_len >= PATH_MAX) return 0; /* ignore empty/invalid names */
    if (fwrite_u32(archive, name_len) != 0) return -1;
    if (fwrite(relname, 1, name_len, archive) != name_len) return -1;
    if (fwrite_u64(archive, 0) != 0) return -1;  /* orig_size = 0 for dir */
    if (fwrite_u32(archive, 0) != 0) return -1;  /* comp_size = 0 for dir */
    return 0;
}

/* decompress a single file from the current position in the archive */
static int decompress_archive_entry_data(FILE *archive, const char *outpath, uint32_t comp_size, uint64_t orig_size) {
    g_current_file = outpath;
    g_file_orig_size = orig_size;
    g_file_comp_size = comp_size;
    
    if (ensure_output_dir_writable(outpath) != 0) return -1;
    int fout = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fout < 0) { fprintf(stderr, "open output '%s' failed: %s\n", outpath, strerror(errno)); return -1; }

    uint8_t *inbuf = malloc(MAX_COMP_SIZE);
    uint8_t *outbuf = malloc(BLOCK_SIZE); /* decompress max BLOCK_SIZE */
    if (!inbuf || !outbuf) { 
        fprintf(stderr, "memory allocation failed\n");
        close(fout); 
        free(inbuf); 
        free(outbuf); 
        return -1; 
    }

    uint32_t remaining = comp_size;
    while (remaining > 0) {
        uint32_t block_total;
        if (remaining < 4 || fread_u32(archive, &block_total) != 0) { 
            fprintf(stderr, "archive corrupt (block header)\n"); 
            free(inbuf); free(outbuf); close(fout); return -1; 
        }

        remaining -= 4;
        if (block_total > MAX_COMP_SIZE || block_total > remaining) { 
            fprintf(stderr, "block size invalid/too large\n"); 
            free(inbuf); free(outbuf); close(fout); return -1; 
        }
        
        size_t got = fread_all(archive, inbuf, block_total);
        if (got != block_total) { 
            fprintf(stderr, "unexpected EOF in block\n"); 
            free(inbuf); free(outbuf); close(fout); return -1; 
        }
        remaining -= block_total;
        
        /* decompress up to BLOCK_SIZE */
        size_t dec = decompress_block(inbuf, block_total, outbuf, BLOCK_SIZE);
        if (dec == 0) { 
            fprintf(stderr, "decompression failed for block\n"); 
            free(inbuf); free(outbuf); close(fout); return -1; 
        }
        
        if (write_all_fd(fout, outbuf, dec) != 0) { 
            perror("write"); 
            free(inbuf); free(outbuf); close(fout); return -1; 
        }
        
        g_processed_bytes += 4 + block_total;
        g_blocks_processed++;
        progress_update(g_processed_bytes, g_total_bytes);
    }
    
    if (remaining != 0) {
        fprintf(stderr, "comp_size mismatch: expected 0 remaining bytes, got %u\n", remaining);
        free(inbuf); free(outbuf); close(fout); return -1; 
    }

    free(inbuf);
    free(outbuf);
    close(fout);
    
    g_current_file = NULL;
    g_file_orig_size = 0;
    g_file_comp_size = 0;
    progress_update(g_processed_bytes, g_total_bytes);

    return 0;
}

/* single-file compress/decompress */

static int compress_file_path(const char *inpath, const char *outpath) {
    g_current_file = outpath;
    g_file_comp_size = 0;

    struct stat st;
    if (lstat(inpath, &st) != 0) { 
        fprintf(stderr, "lstat failed for %s: %s\n", inpath, strerror(errno)); 
        return -1; 
    }
    if (check_input_file_readability(inpath, st.st_mode) != 0) return -1;

    FILE *fin = fopen(inpath, "rb");
    if (!fin) { fprintf(stderr, "open input '%s' failed: %s\n", inpath, strerror(errno)); return -1; }
    if (ensure_output_dir_writable(outpath) != 0) { fclose(fin); return -1; }
    int fout = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fout < 0) { fprintf(stderr, "open output '%s' failed: %s\n", outpath, strerror(errno)); fclose(fin); return -1; }

    uint64_t orig_size = (uint64_t)st.st_size;
    g_file_orig_size = orig_size;

    uint8_t *inbuf = malloc(BLOCK_SIZE);
    uint8_t *outbuf = malloc(MAX_COMP_SIZE);
    if (!inbuf || !outbuf) { 
        fprintf(stderr, "memory allocation failed\n");
        fclose(fin); 
        close(fout); 
        free(inbuf); 
        free(outbuf); 
        return -1; 
    }

    uint32_t total_comp = 0;
    size_t r;
    while ((r = fread_all(fin, inbuf, BLOCK_SIZE)) > 0) {
        size_t comp = compress_block(inbuf, r, outbuf, MAX_COMP_SIZE);
        if (comp == 0) { 
            fprintf(stderr, "compression failed for block\n"); 
            free(inbuf); 
            free(outbuf); 
            fclose(fin); 
            close(fout); 
            return -1; 
        }
        uint32_t block_total = (uint32_t)comp;
        if (write_all_fd(fout, &block_total, 4) != 0) { 
            perror("write block size"); 
            free(inbuf); 
            free(outbuf); 
            fclose(fin); 
            close(fout); 
            return -1; 
        }
        if (write_all_fd(fout, outbuf, comp) != 0) { 
            perror("write block data"); 
            free(inbuf); 
            free(outbuf); 
            fclose(fin); 
            close(fout); 
            return -1; 
        }
        total_comp += 4 + block_total;
        g_file_comp_size = total_comp;
        g_processed_bytes += r;
        g_blocks_processed++;
        progress_update(g_processed_bytes, g_total_bytes);
    }
    
    if (ferror(fin)) {
        fprintf(stderr, "read error on input file '%s': %s\n", inpath, strerror(errno));
        free(inbuf); free(outbuf); fclose(fin); close(fout); return -1;
    }


    free(inbuf);
    free(outbuf);
    fclose(fin);
    close(fout);

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
    if (!fin) { fprintf(stderr, "open input '%s' failed: %s\n", inpath, strerror(errno)); return -1; }
    if (ensure_output_dir_writable(outpath) != 0) { fclose(fin); return -1; }
    int fout = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fout < 0) { fprintf(stderr, "open output '%s' failed: %s\n", outpath, strerror(errno)); fclose(fin); return -1; }

    uint8_t *inbuf = malloc(MAX_COMP_SIZE);
    uint8_t *outbuf = malloc(BLOCK_SIZE);
    if (!inbuf || !outbuf) { 
        fprintf(stderr, "memory allocation failed\n");
        fclose(fin); 
        close(fout); 
        free(inbuf); 
        free(outbuf); 
        return -1; 
    }

    while (1) {
        uint32_t block_total;
        if (fread_u32(fin, &block_total) != 0) {
            if (feof(fin)) break;
            fprintf(stderr, "corrupt input (block header)\n"); 
            free(inbuf); 
            free(outbuf); 
            fclose(fin); 
            close(fout); 
            return -1;
        }

        if (block_total > MAX_COMP_SIZE) { 
            fprintf(stderr, "block too large\n"); 
            free(inbuf); 
            free(outbuf); 
            fclose(fin); 
            close(fout); 
            return -1; 
        }
        
        size_t got = fread_all(fin, inbuf, block_total);
        if (got != block_total) { 
            fprintf(stderr, "unexpected EOF in block\n"); 
            free(inbuf); 
            free(outbuf); 
            fclose(fin); 
            close(fout); 
            return -1; 
        }
        
        size_t dec = decompress_block(inbuf, block_total, outbuf, BLOCK_SIZE);
        if (dec == 0) { 
            fprintf(stderr, "decompression failed for block\n"); 
            free(inbuf); 
            free(outbuf); 
            fclose(fin); 
            close(fout); 
            return -1; 
        }
        
        if (write_all_fd(fout, outbuf, dec) != 0) { 
            perror("write"); 
            free(inbuf); 
            free(outbuf); 
            fclose(fin); 
            close(fout); 
            return -1; 
        }
        
        g_processed_bytes += 4 + block_total;
        g_blocks_processed++;
        progress_update(g_processed_bytes, g_total_bytes);
    }

    free(inbuf);
    free(outbuf);
    fclose(fin);
    close(fout);

    g_current_file = NULL;
    g_file_orig_size = 0;
    g_file_comp_size = 0;
    progress_update(g_processed_bytes, g_total_bytes);

    return 0;
}

/* --- directory recursion and totals --- */

static int compute_total_size_recursive(const char *path, uint64_t *accum) {
    struct stat st;
    /* use lstat to handle symlinks correctly (skip them) */
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
        if (check_input_file_readability(path, st.st_mode) == 0) {
            *accum += (uint64_t)st.st_size;
        }
        return 0;
    } else return 0; /* skip other file types (symlinks, devices, etc.) */
}

static int compute_base_root(const char *input, char *base_root, size_t blen) {
    struct stat st;
    if (lstat(input, &st) != 0) return -1;
    
    if (realpath(input, base_root) == NULL) {
         return -1;
    }
    
    if (S_ISDIR(st.st_mode)) {
        /* if input is a dir, base_root is the directory itself */
        size_t l = strlen(base_root);
        /* remove trailing slash for consistency (unless it's root '/') */
        if (l > 1 && base_root[l - 1] == '/') base_root[l - 1] = '\0';
        return 0;
    } else {
        /* if input is a file, base_root is the parent directory */
        char *d = dirname(base_root); /* dirname works on the buffer in place */
        strncpy(base_root, d, blen);
        base_root[blen - 1] = '\0';
        size_t l = strlen(base_root);
        if (l > 1 && base_root[l - 1] == '/') base_root[l - 1] = '\0';
        return 0;
    }
}

/*
 * creates a relative path from target to base.
 * returns 0 on success, -1 on failure.
 */
static int make_relative(const char *base, const char *target, char *out, size_t outlen) {
    size_t bl = strlen(base);
    size_t tl = strlen(target);
    
    if (bl == 0 || tl == 0) return -1;
    
    /* handle cases where base is a prefix of target */
    if (strncmp(base, target, bl) == 0) {
        const char *p = target + bl;
        if (*p == '/') p++;
        strncpy(out, p, outlen - 1);
        out[outlen - 1] = '\0';
        return 0;
    }
    
    /* handles case where base is the parent and target is the child,
       e.g. base=/home/user, target=/home/user/file.txt, but realpath makes them different.
       since we rely on realpath, this should work if base is really a prefix of target.
       if they are identical, this is a special case. */
    if (bl == tl && strcmp(base, target) == 0) {
        out[0] = '\0'; /* relative path is empty string */
        return 0;
    }
    
    return -1;
}

/* recursively writes file and directory entries to the archive */
static int process_compress_to_archive_recursive(FILE *archive, const char *inpath, const char *base_root) {
    struct stat st;
    if (lstat(inpath, &st) != 0) { 
        fprintf(stderr, "lstat '%s' failed: %s\n", inpath, strerror(errno)); 
        return -1; 
    }

    char full_path[PATH_MAX];
    if (realpath(inpath, full_path) == NULL) {
         fprintf(stderr, "realpath failed for %s: %s\n", inpath, strerror(errno));
         return -1;
    }

    char rel[PATH_MAX];
    char tmp_path_for_basename[PATH_MAX];
    strncpy(tmp_path_for_basename, full_path, sizeof(tmp_path_for_basename) - 1);
    tmp_path_for_basename[sizeof(tmp_path_for_basename) - 1] = '\0';
    
    if (make_relative(base_root, full_path, rel, sizeof(rel)) != 0) {
        /* korrektur: verwende basename als fallback, um absolute pfade im archiv zu vermeiden */
        const char *bn = basename(tmp_path_for_basename);
        strncpy(rel, bn, sizeof(rel)-1);
        rel[sizeof(rel)-1] = '\0';
    }

    /* ensure root directory is stored as "" or "." */
    if (rel[0] == '\0' && S_ISDIR(st.st_mode)) {
        /* skip explicit entry for the root directory itself to prevent double extraction */
        ;
    } else if (S_ISDIR(st.st_mode)) {
        /* write directory entry first (needed for correct extraction order) */
        if (write_directory_entry(archive, rel) != 0) return -1;

        DIR *d = opendir(inpath);
        if (!d) { fprintf(stderr, "opendir '%s' failed: %s\n", inpath, strerror(errno)); return -1; }
        struct dirent *ent;
        while ((ent = readdir(d)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            char child[PATH_MAX];
            join_path(child, sizeof(child), inpath, ent->d_name);
            if (process_compress_to_archive_recursive(archive, child, base_root) != 0) { closedir(d); return -1; }
        }
        closedir(d);
        return 0;
    } else if (S_ISREG(st.st_mode)) {
        if (check_input_file_readability(inpath, st.st_mode) != 0) return 0; /* skip unreadable/non-regular */

        uint64_t comp_size = 0;
        if (compress_archive_entry_data(archive, rel, inpath, &comp_size) != 0) return -1;
        (void)comp_size;
        return 0;
    } else {
        fprintf(stderr, "skipping non-regular: %s\n", inpath);
        return 0;
    }
}

/* --- archive reading / extraction --- */

static int archive_add_entry(const char *name, uint64_t orig_size, uint32_t comp_size, long data_pos, int is_dir) {
    if (g_num_entries + 1 > g_max_entries) {
        size_t newcap = g_max_entries ? g_max_entries * 2 : 256;
        entry_metadata *n = realloc(g_entries, newcap * sizeof(entry_metadata));
        if (!n) return -1;
        g_entries = n;
        g_max_entries = newcap;
    }
    entry_metadata *e = &g_entries[g_num_entries++];
    strncpy(e->name, name, sizeof(e->name)-1);
    e->name[sizeof(e->name)-1] = '\0';
    e->orig_size = orig_size;
    e->comp_size = comp_size;
    e->data_pos = data_pos;
    e->is_dir = is_dir;
    return 0;
}

static int read_archive_index(FILE *archive) {
    g_num_entries = 0;
    while (1) {
        uint32_t name_len;
        if (fread_u32(archive, &name_len) != 0) {
            if (feof(archive)) break;
            fprintf(stderr, "archive corrupt (name_len read error)\n");
            return -1;
        }
        
        if (name_len == 0 || name_len >= PATH_MAX) { 
            fprintf(stderr, "archive corrupt (name_len invalid: %u)\n", name_len); 
            return -1; 
        }
        
        char name[PATH_MAX];
        if (fread(name, 1, name_len, archive) != name_len) { 
            fprintf(stderr, "archive corrupt (name read error)\n"); 
            return -1; 
        }
        name[name_len] = '\0';
        
        uint64_t orig_size;
        if (fread_u64(archive, &orig_size) != 0) { fprintf(stderr, "archive corrupt (orig_size read error)\n"); return -1; }
        
        uint32_t comp_size;
        if (fread_u32(archive, &comp_size) != 0) { fprintf(stderr, "archive corrupt (comp_size read error)\n"); return -1; }
        
        long data_pos = ftell(archive);
        if (data_pos < 0) return -1;
        
        /* robustness: check if comp_size is too large for the rest of the file */
        if (comp_size > 0 && comp_size > (uint32_t)(BLOCK_SIZE * (orig_size / BLOCK_SIZE + 1)) + 64 * 1024) {
            fprintf(stderr, "archive corrupt (comp_size implausibly large)\n");
            return -1;
        }
        
        /* skip comp_size bytes (comp_size includes per-block headers) */
        if (fseek(archive, comp_size, SEEK_CUR) != 0) { 
            fprintf(stderr, "archive corrupt (skip data failed)\n"); 
            return -1; 
        }
        
        int is_dir = (orig_size == 0 && comp_size == 0) ? 1 : 0;
        if (archive_add_entry(name, orig_size, comp_size, data_pos, is_dir) != 0) return -1;
    }
    return 0;
}

static int cmp_entries_for_extract(const void *a, const void *b) {
    const entry_metadata *ea = a;
    const entry_metadata *eb = b;
    /* directories first (to create parents before children files) */
    if (ea->is_dir != eb->is_dir) return eb->is_dir - ea->is_dir;
    return strcmp(ea->name, eb->name);
}

static int extract_archive(FILE *archive, const char *out_root) {
    if (read_archive_index(archive) != 0) return -1;
    if (g_num_entries == 0) return 0;
    qsort(g_entries, g_num_entries, sizeof(entry_metadata), cmp_entries_for_extract);
    
    /* compute total size for progress bar for decompression (sum of comp_size + metadata) */
    uint64_t total_compressed = 0;
    for (size_t i = 0; i < g_num_entries; ++i) {
        entry_metadata *e = &g_entries[i];
        /* 4 bytes for name_len, name_len bytes for name, 8 bytes for orig_size, 4 bytes for comp_size */
        total_compressed += 4 + strlen(e->name) + 8 + 4; 
        total_compressed += e->comp_size;
    }
    g_total_bytes = total_compressed;
    g_processed_bytes = 0;
    gettimeofday(&g_start_time, NULL);
    g_blocks_processed = 0;


    for (size_t i = 0; i < g_num_entries; ++i) {
        entry_metadata *e = &g_entries[i];
        char outpath[PATH_MAX];
        
        /* handle special case for root path stored as "" */
        if (e->name[0] == '\0') {
             strncpy(outpath, out_root ? out_root : ".", sizeof(outpath)-1);
             outpath[sizeof(outpath)-1] = '\0';
        } else if (out_root && out_root[0] != '\0') {
            join_path(outpath, sizeof(outpath), out_root, e->name);
        } else {
            strncpy(outpath, e->name, sizeof(outpath)-1);
            outpath[sizeof(outpath)-1] = '\0';
        }

        if (e->is_dir) {
            g_current_file = outpath;
            /* create directory */
            struct stat st;
            if (stat(outpath, &st) != 0) {
                if (mkdir(outpath, 0755) != 0 && errno != EEXIST) {
                    fprintf(stderr, "failed to create dir '%s': %s\n", outpath, strerror(errno));
                    return -1;
                }
            }
            g_processed_bytes += 4 + strlen(e->name) + 8 + 4;
            progress_update(g_processed_bytes, g_total_bytes);
            g_current_file = NULL;

        } else {
            /* seek to data_pos and decompress */
            if (fseek(archive, e->data_pos, SEEK_SET) != 0) { fprintf(stderr, "seek failed\n"); return -1; }
            if (decompress_archive_entry_data(archive, outpath, e->comp_size, e->orig_size) != 0) return -1;
            
            /* decompress_archive_entry_data updates g_processed_bytes and progress */
        }
    }
    return 0;
}
/* ... rest of the functions (list_archive_contents, parse_args, main) */


/* --- listing functionality --- */

static int list_archive_contents(FILE *archive) {
    if (read_archive_index(archive) != 0) {
        return -1;
    }
    
    printf("mode    orig_size  comp_size  ratio  name\n");
    printf("---     ---------  ---------  -----  ----\n");

    uint64_t total_orig = 0;
    uint64_t total_comp = 0;
    
    for (size_t i = 0; i < g_num_entries; ++i) {
        entry_metadata *e = &g_entries[i];
        char h_orig[32], h_comp[32];
        human_bytes(e->orig_size, h_orig, sizeof(h_orig));
        human_bytes(e->comp_size, h_comp, sizeof(h_comp));
        
        const char *mode = e->is_dir ? "dir" : "file";
        double ratio = 0.0;
        if (!e->is_dir && e->orig_size > 0) {
            ratio = (1.0 - (double)e->comp_size / (double)e->orig_size) * 100.0;
        }
        
        printf("%-5s %10s %10s %5.1f%% %s\n", 
               mode, 
               h_orig, 
               h_comp, 
               ratio, 
               e->name);
               
        if (!e->is_dir) {
            total_orig += e->orig_size;
            total_comp += e->comp_size;
        }
    }
    
    char h_tot_orig[32], h_tot_comp[32];
    human_bytes(total_orig, h_tot_orig, sizeof(h_tot_orig));
    human_bytes(total_comp, h_tot_comp, sizeof(h_tot_comp));
    double overall_ratio = 0.0;
    if (total_orig > 0) {
        overall_ratio = (1.0 - (double)total_comp / (double)total_orig) * 100.0;
    }
    
    printf("\n");
    printf("total   %10s %10s %5.1f%% (%zu files/dirs)\n", 
           h_tot_orig, 
           h_tot_comp, 
           overall_ratio, 
           g_num_entries);

    return 0;
}


/* --- main logic --- */

static void print_usage(const char *progname) {
    fprintf(stderr, "usage: %s -c <input> [<output>]\n", progname);
    fprintf(stderr, "       %s -d <input.lz4> [<output>]\n", progname);
    fprintf(stderr, "       %s -l <archive.lz4>\n", progname);
    fprintf(stderr, "\n");
    fprintf(stderr, "  -c: compress (creates archive if input is directory)\n");
    fprintf(stderr, "  -d: decompress\n");
    fprintf(stderr, "  -l: list archive contents\n");
}

static int parse_args(int argc, char **argv, command_args *args) {
    if (argc < 3) {
        print_usage(argv[0]);
        return -1;
    }
    
    args->mode = MODE_NONE;
    args->inpath = NULL;
    args->outpath = NULL;
    
    if (strcmp(argv[1], "-c") == 0) args->mode = MODE_COMPRESS;
    else if (strcmp(argv[1], "-d") == 0) args->mode = MODE_DECOMPRESS;
    else if (strcmp(argv[1], "-l") == 0) args->mode = MODE_LIST;
    else {
        print_usage(argv[0]);
        return -1;
    }

    args->inpath = argv[2];
    if (argc >= 4) args->outpath = argv[3];

    /* mode-specific checks */
    if (args->mode == MODE_LIST && argc > 3) {
        fprintf(stderr, "error: -l mode takes only one argument (archive path)\n");
        return -1;
    }

    return 0;
}

static int generate_archive_name(const char *inpath, char *outbuf, size_t buflen) {
    char tmp[PATH_MAX];
    strncpy(tmp, inpath, sizeof(tmp)-1);
    tmp[sizeof(tmp)-1] = '\0';
    char *base = basename(tmp);
    
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    
    if (!tm) return -1;
    
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "-%Y%m%d%H%M", tm);
    
    if (snprintf(outbuf, buflen, "%s%s.lz4", base, timestamp) >= (int)buflen) {
        return -1;
    }
    return 0;
}

int main(int argc, char **argv) {
    command_args args;
    if (parse_args(argc, argv, &args) != 0) return EXIT_FAILURE;

    struct stat st;
    if (lstat(args.inpath, &st) != 0) die("lstat input");

    gettimeofday(&g_start_time, NULL);
    g_blocks_processed = 0;
    
    if (args.mode == MODE_COMPRESS) {
        if (S_ISDIR(st.st_mode)) {
            /* compression of a directory into an archive */
            char archive_name[PATH_MAX];
            if (!args.outpath) {
                if (generate_archive_name(args.inpath, archive_name, sizeof(archive_name)) != 0) {
                    fprintf(stderr, "error: failed to generate archive name\n");
                    return EXIT_FAILURE;
                }
                args.outpath = archive_name;
            } else {
                 strncpy(archive_name, args.outpath, sizeof(archive_name)-1);
                 archive_name[sizeof(archive_name)-1] = '\0';
            }
            
            if (ensure_output_dir_writable(archive_name) != 0) return EXIT_FAILURE;

            /* compute total size for progress bar */
            if (compute_total_size_recursive(args.inpath, &g_total_bytes) != 0) {
                 fprintf(stderr, "warning: could not compute total size, progress will be inaccurate\n");
                 g_total_bytes = 0;
            }

            FILE *archive = fopen(archive_name, "wb");
            if (!archive) die("open output archive");

            char base_root[PATH_MAX];
            if (compute_base_root(args.inpath, base_root, sizeof(base_root)) != 0) {
                fprintf(stderr, "error: could not determine base root for archive\n");
                fclose(archive);
                return EXIT_FAILURE;
            }
            
            int ret = process_compress_to_archive_recursive(archive, args.inpath, base_root);

            fclose(archive);
            if (ret != 0) {
                fprintf(stderr, "compression failed, incomplete archive '%s' left behind.\n", archive_name);
                return EXIT_FAILURE;
            }
            
        } else if (S_ISREG(st.st_mode)) {
            /* compression of a single file */
            if (!args.outpath) {
                fprintf(stderr, "error: output path required for single file compression\n");
                print_usage(argv[0]);
                return EXIT_FAILURE;
            }
            g_total_bytes = (uint64_t)st.st_size;
            if (compress_file_path(args.inpath, args.outpath) != 0) return EXIT_FAILURE;

        } else {
            fprintf(stderr, "error: input '%s' is not a file or directory\n", args.inpath);
            return EXIT_FAILURE;
        }

    } else if (args.mode == MODE_DECOMPRESS) {
        /* decompression - determine if it's an archive or a single block-framed file */
        if (!args.outpath) {
             fprintf(stderr, "error: output path required for decompression\n");
             print_usage(argv[0]);
             return EXIT_FAILURE;
        }
        
        /* check if it looks like an archive first */
        FILE *f = fopen(args.inpath, "rb");
        if (!f) die("open input file");
        
        int ret = read_archive_index(f);
        
        if (ret == 0 && g_num_entries > 0) {
            /* it's an archive */
            rewind(f);
            if (extract_archive(f, args.outpath) != 0) { fclose(f); return EXIT_FAILURE; }
            
        } else {
            /* it's a single block-framed file */
            if (g_entries) free(g_entries);
            g_entries = NULL;
            g_num_entries = 0;
            g_max_entries = 0;
            fclose(f);
            
            /* assume file mode, total size cannot be known accurately for single file progress */
            struct stat ist;
            if (lstat(args.inpath, &ist) == 0) {
                g_total_bytes = (uint64_t)ist.st_size;
            }
            
            if (decompress_file_path(args.inpath, args.outpath) != 0) return EXIT_FAILURE;
        }
        
        if (f) fclose(f);

    } else if (args.mode == MODE_LIST) {
        /* list archive contents */
        FILE *f = fopen(args.inpath, "rb");
        if (!f) die("open input archive");
        if (list_archive_contents(f) != 0) { fclose(f); return EXIT_FAILURE; }
        fclose(f);
    }
    
    if (g_entries) free(g_entries);

    return EXIT_SUCCESS;
}