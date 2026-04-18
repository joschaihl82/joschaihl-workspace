/*
  jlz4.c
  Streaming tar -> LZ4-like compressor and decompressor/extractor with integrated status display.

  Features:
  - True streaming: tar blocks are written into an internal chunk buffer and compressed on the fly.
  - PAX extended headers for xattrs (SCHILY.xattr.<name>.base64).
  - Device nodes preserved (typeflag 3/4 with devmajor/devminor).
  - Framed compressed output: [4 bytes LE orig_size][4 bytes LE comp_size][comp_bytes]...
  - Integrated terminal status display (progress bar, ETA, filename, MB/s, ratio, last failure, error count).
  - Modes: create, extract, tarout.

  Build:
    gcc -O2 -o jlz4 jlz4.c

  Usage:
    Create:  ./jlz4 create out.tar.lz4 input1 [input2 ...]
    Extract: ./jlz4 extract in.tar.lz4 [outdir]
    Tar out: ./jlz4 tarout in.tar.lz4 out.tar
*/

#define _XOPEN_SOURCE 700
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <sys/sysmacros.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>
#include <inttypes.h>
#include <utime.h>
#include <sys/time.h>
#include <stdarg.h>
#include <sys/ioctl.h>

/* ---------------------------
   Status display (terminal)
   --------------------------- */

typedef struct {
    double progress;            /* 0.0 .. 1.0 */
    unsigned long processed_bytes;
    unsigned long total_bytes;  /* 0 when unknown */
    unsigned long compressed_bytes;
    double current_speed_MB_s;  /* MB/s */
    double avg_ratio;           /* compressed / original */
    time_t eta_unix;            /* estimated end time */
    time_t start_time;
    char filename[1024];
    char last_failure[1024];
    unsigned int error_count;
    char last_error_file[1024];
    time_t last_update_time;
} status_t;

static status_t global_status;

static int get_terminal_width(void)
{
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0) return (int)w.ws_col;
    return 80;
}

static void print_padded_line(const char *fmt, int width, ...)
{
    va_list ap;
    va_start(ap, width);
    char buf[4096];
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    size_t len = strlen(buf);
    if ((int)len >= width) {
        fwrite(buf, 1, width, stdout);
        putchar('\n');
        return;
    }
    fwrite(buf, 1, len, stdout);
    for (int i = 0; i < width - (int)len; ++i) putchar(' ');
    putchar('\n');
}

static void format_duration(time_t target_unix, time_t now, char *out, size_t outlen)
{
    if (target_unix == 0 || target_unix <= now) {
        snprintf(out, outlen, "--:--:--");
        return;
    }
    time_t diff = target_unix - now;
    int h = (int)(diff / 3600);
    int m = (int)((diff % 3600) / 60);
    int s = (int)(diff % 60);
    snprintf(out, outlen, "%02d:%02d:%02d", h, m, s);
}

static void render_status(const status_t *s)
{
    int w = get_terminal_width();

    /* 1) Progressbar */
    const char *label = "Progress: ";
    int label_len = (int)strlen(label);
    int pct_len = 5;
    int reserved = label_len + pct_len + 3;
    int bar_width = w - reserved;
    if (bar_width < 10) bar_width = 10;
    int filled = (int)(s->progress * bar_width + 0.5);
    if (filled < 0) filled = 0;
    if (filled > bar_width) filled = bar_width;

    char barline[8192];
    int p = 0;
    p += snprintf(barline + p, sizeof(barline) - p, "%s[", label);
    for (int i = 0; i < bar_width; ++i) {
        if (i < filled) barline[p++] = '=';
        else if (i == filled) barline[p++] = '>';
        else barline[p++] = ' ';
        if (p >= (int)sizeof(barline)-10) break;
    }
    p += snprintf(barline + p, sizeof(barline) - p, "] %3d%%", (int)(s->progress * 100.0 + 0.5));
    barline[p] = '\0';
    print_padded_line("%s", w, barline);

    /* 2) ETA / Elapsed */
    time_t now = time(NULL);
    char eta_str[32];
    if (s->eta_unix != 0) format_duration(s->eta_unix, now, eta_str, sizeof(eta_str));
    else snprintf(eta_str, sizeof(eta_str), "--:--:--");
    double elapsed = difftime(now, s->start_time);
    char elapsed_str[32];
    if (s->start_time != 0) {
        int eh = (int)(elapsed / 3600);
        int em = (int)((((int)elapsed) % 3600) / 60);
        int es = (int)elapsed % 60;
        snprintf(elapsed_str, sizeof(elapsed_str), "%02d:%02d:%02d", eh, em, es);
    } else snprintf(elapsed_str, sizeof(elapsed_str), "--:--:--");
    print_padded_line("ETA: %s   Elapsed: %s", w, eta_str, elapsed_str);

    /* 3) Filename */
    char fname_line[2048];
    snprintf(fname_line, sizeof(fname_line), "File: %s", s->filename[0] ? s->filename : "(none)");
    print_padded_line("%s", w, fname_line);

    /* 4) Stats */
    double mb_processed = s->processed_bytes / (1024.0 * 1024.0);
    double mb_total = (s->total_bytes > 0) ? (s->total_bytes / (1024.0 * 1024.0)) : 0.0;
    char stats_line[512];
    if (s->total_bytes > 0) {
        snprintf(stats_line, sizeof(stats_line),
                 "Speed: %.2f MB/s | AvgRatio: %.3f | Processed: %.2f MB / %.2f MB",
                 s->current_speed_MB_s, s->avg_ratio, mb_processed, mb_total);
    } else {
        snprintf(stats_line, sizeof(stats_line),
                 "Speed: %.2f MB/s | AvgRatio: %.3f | Processed: %.2f MB / ? MB",
                 s->current_speed_MB_s, s->avg_ratio, mb_processed);
    }
    print_padded_line("%s", w, stats_line);

    /* 5) Last failure */
    char fail_line[2048];
    if (s->last_failure[0]) snprintf(fail_line, sizeof(fail_line), "Last failure: %s", s->last_failure);
    else snprintf(fail_line, sizeof(fail_line), "Last failure: (none)");
    print_padded_line("%s", w, fail_line);

    /* 6) Errors */
    char err_line[2048];
    if (s->error_count > 0) snprintf(err_line, sizeof(err_line), "Errors: %u - last error at: %s", s->error_count, s->last_error_file);
    else snprintf(err_line, sizeof(err_line), "Errors: 0");
    print_padded_line("%s", w, err_line);

    /* 7) blank */
    print_padded_line(" ", w, "");

    /* 8) Usage */
    char usage_line[2048];
    snprintf(usage_line, sizeof(usage_line),
             "Usage: ./jlz4 -c .   -> creates CWD_YYYYMMDD_HHMMSS.lz4    |  ./jlz4 -d <archive>.lz4 -> extracts into <archive>_YYYYMMDD_HHMMSS/");
    print_padded_line("%s", w, usage_line);

    /* Move cursor up to overwrite next time */
    int lines = 8;
    printf("\x1b[%dA", lines);
    fflush(stdout);
}

/* ---------------------------
   Simplified LZ4-like compressor/decompressor
   --------------------------- */

typedef uint8_t  BYTE;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

enum { MINMATCH = 4 };
enum { RUN_BITS = 4, ML_BITS = 4 };
enum { RUN_MASK = (1 << RUN_BITS) - 1, ML_MASK = (1 << ML_BITS) - 1 };

static inline U32 hash32(U32 v, unsigned hashLog)
{
    return (U32)((v * 2654435761U) >> (32 - hashLog));
}

int lz4_compress_simple(const char* source, char* dest, int inputSize, int maxOutputSize)
{
    if (inputSize <= 0) return 0;
    const BYTE* ip = (const BYTE*)source;
    const BYTE* iend = ip + inputSize;
    BYTE* op = (BYTE*)dest;
    BYTE* oend = (maxOutputSize > 0) ? (BYTE*)dest + maxOutputSize : NULL;

    const unsigned HASHLOG = 14;
    const unsigned HT_SIZE = 1u << HASHLOG;
    U32* hashTable = (U32*)calloc(HT_SIZE, sizeof(U32));
    if (!hashTable) return 0;

    const BYTE* anchor = ip;

    while (ip + MINMATCH <= iend) {
        U32 sequence = 0;
        memcpy(&sequence, ip, sizeof(U32));
        U32 h = hash32(sequence, HASHLOG);
        const BYTE* ref = (hashTable[h] == 0) ? NULL : (const BYTE*)(source + hashTable[h] - 1);
        hashTable[h] = (U32)(ip - (const BYTE*)source) + 1;

        if (ref && ref + MINMATCH <= ip && memcmp(ref, ip, MINMATCH) == 0) {
            int literalLen = (int)(ip - anchor);
            BYTE token = 0;
            if (oend && (op + 1 + literalLen + 2 > oend)) { free(hashTable); return 0; }
            if (literalLen >= RUN_MASK) {
                token = (RUN_MASK << ML_BITS);
                *op++ = token;
                int len = literalLen - RUN_MASK;
                while (len >= 255) { *op++ = 255; len -= 255; }
                *op++ = (BYTE)len;
                memcpy(op, anchor, literalLen); op += literalLen;
            } else {
                token = (BYTE)(literalLen << ML_BITS);
                *op++ = token;
                memcpy(op, anchor, literalLen); op += literalLen;
            }

            U16 offset = (U16)(ip - ref);
            *op++ = (BYTE)(offset & 0xFF);
            *op++ = (BYTE)((offset >> 8) & 0xFF);

            const BYTE* m1 = ref + MINMATCH;
            const BYTE* m2 = ip  + MINMATCH;
            while (m2 < iend && *m1 == *m2) { m1++; m2++; }
            int matchLen = (int)(m2 - (ip + MINMATCH));

            int tokenIndex = (int)(op - (BYTE*)dest) - (2 + literalLen) - 1;
            BYTE existingToken = ((BYTE*)dest)[tokenIndex];
            if (matchLen >= ML_MASK) {
                ((BYTE*)dest)[tokenIndex] = existingToken | ML_MASK;
                int len = matchLen - ML_MASK;
                while (len >= 255) { *op++ = 255; len -= 255; }
                *op++ = (BYTE)len;
            } else {
                ((BYTE*)dest)[tokenIndex] = existingToken | (BYTE)matchLen;
            }

            ip = m2;
            anchor = ip;
        } else {
            ip++;
        }
    }

    int lastRun = (int)(iend - anchor);
    if (oend && (op + 1 + lastRun > oend)) { free(hashTable); return 0; }
    if (lastRun >= RUN_MASK) {
        *op++ = (BYTE)(RUN_MASK << ML_BITS);
        int len = lastRun - RUN_MASK;
        while (len >= 255) { *op++ = 255; len -= 255; }
        *op++ = (BYTE)len;
    } else {
        *op++ = (BYTE)(lastRun << ML_BITS);
    }
    memcpy(op, anchor, lastRun); op += lastRun;

    int compressedSize = (int)(op - (BYTE*)dest);
    free(hashTable);
    return compressedSize;
}

int lz4_decompress_simple(const char* source, char* dest, int inputSize, int maxOutputSize)
{
    const BYTE* ip = (const BYTE*)source;
    const BYTE* iend = ip + inputSize;
    BYTE* op = (BYTE*)dest;
    BYTE* oend = (maxOutputSize > 0) ? (BYTE*)dest + maxOutputSize : NULL;

    while (ip < iend) {
        BYTE token = *ip++;
        int literalLen = token >> ML_BITS;
        if (literalLen == RUN_MASK) {
            int s;
            do {
                if (ip >= iend) return -1;
                s = *ip++;
                literalLen += s;
            } while (s == 255);
        }

        if (ip + literalLen > iend) return -1;
        if (oend && op + literalLen > oend) return -1;
        memcpy(op, ip, literalLen);
        ip += literalLen;
        op += literalLen;

        if (ip >= iend) break;

        if (ip + 2 > iend) return -1;
        U16 offset = (U16)ip[0] | ((U16)ip[1] << 8);
        ip += 2;
        if (offset == 0) return -1;
        if (op - (BYTE*)dest < offset) return -1;

        int matchLen = token & ML_MASK;
        if (matchLen == ML_MASK) {
            int s;
            do {
                if (ip >= iend) return -1;
                s = *ip++;
                matchLen += s;
            } while (s == 255);
        }
        matchLen += MINMATCH;

        const BYTE* matchSrc = op - offset;
        if (oend && op + matchLen > oend) return -1;
        if (matchSrc + matchLen <= op) {
            memcpy(op, matchSrc, matchLen);
            op += matchLen;
        } else {
            for (int i = 0; i < matchLen; ++i) {
                op[i] = matchSrc[i];
            }
            op += matchLen;
        }
    }

    return (int)(op - (BYTE*)dest);
}

/* ---------------------------
   Tar utilities (ustar)
   --------------------------- */

#define TAR_BLOCK 512

static void tar_write_octal(char *buf, size_t size, unsigned long val)
{
    size_t i = size;
    buf[--i] = ' ';
    buf[--i] = '\0';
    while (i > 0 && val > 0) {
        buf[--i] = '0' + (val & 7);
        val >>= 3;
    }
    while (i > 0) buf[--i] = '0';
}

static void make_tar_header(const char *name, const struct stat *st, const char *linktarget,
                            unsigned char header[TAR_BLOCK], unsigned long devmajor_val, unsigned long devminor_val)
{
    memset(header, 0, TAR_BLOCK);
    strncpy((char*)header + 0, name, 100);
    tar_write_octal((char*)header + 100, 8, st->st_mode & 07777);
    tar_write_octal((char*)header + 108, 8, (unsigned long)st->st_uid);
    tar_write_octal((char*)header + 116, 8, (unsigned long)st->st_gid);
    unsigned long size = (S_ISREG(st->st_mode)) ? (unsigned long)st->st_size : 0;
    tar_write_octal((char*)header + 124, 12, size);
    tar_write_octal((char*)header + 136, 12, (unsigned long)st->st_mtime);
    memset(header + 148, ' ', 8);
    if (S_ISREG(st->st_mode)) header[156] = '0';
    else if (S_ISLNK(st->st_mode)) header[156] = '2';
    else if (S_ISCHR(st->st_mode)) header[156] = '3';
    else if (S_ISBLK(st->st_mode)) header[156] = '4';
    else if (S_ISDIR(st->st_mode)) header[156] = '5';
    else header[156] = '0';
    if (linktarget) strncpy((char*)header + 157, linktarget, 100);
    memcpy(header + 257, "ustar", 5);
    header[262] = '0';
    struct passwd *pw = getpwuid(st->st_uid);
    if (pw) strncpy((char*)header + 265, pw->pw_name, 32);
    struct group *gr = getgrgid(st->st_gid);
    if (gr) strncpy((char*)header + 297, gr->gr_name, 32);

    if (S_ISCHR(st->st_mode) || S_ISBLK(st->st_mode)) {
        tar_write_octal((char*)header + 329, 8, devmajor_val);
        tar_write_octal((char*)header + 337, 8, devminor_val);
    }

    unsigned long sum = 0;
    for (int i = 0; i < TAR_BLOCK; ++i) sum += header[i];
    tar_write_octal((char*)header + 148, 8, sum);
}

/* ---------------------------
   PAX and base64 helpers
   --------------------------- */

static const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char *base64_encode(const unsigned char *in, size_t len)
{
    size_t outlen = ((len + 2) / 3) * 4;
    char *out = malloc(outlen + 1);
    if (!out) return NULL;
    char *p = out;
    for (size_t i = 0; i < len; i += 3) {
        unsigned a = in[i];
        unsigned b = (i+1 < len) ? in[i+1] : 0;
        unsigned c = (i+2 < len) ? in[i+2] : 0;
        unsigned triple = (a << 16) | (b << 8) | c;
        *p++ = b64chars[(triple >> 18) & 0x3F];
        *p++ = b64chars[(triple >> 12) & 0x3F];
        *p++ = (i+1 < len) ? b64chars[(triple >> 6) & 0x3F] : '=';
        *p++ = (i+2 < len) ? b64chars[triple & 0x3F] : '=';
    }
    *p = '\0';
    return out;
}

static unsigned char *base64_decode(const char *in, size_t *outlen)
{
    size_t len = strlen(in);
    size_t alloc = (len / 4) * 3 + 1;
    unsigned char *out = malloc(alloc);
    if (!out) return NULL;
    size_t o = 0;
    for (size_t i = 0; i < len; i += 4) {
        int vals[4] = {0,0,0,0};
        for (int j = 0; j < 4 && i+j < len; ++j) {
            char c = in[i+j];
            if (c >= 'A' && c <= 'Z') vals[j] = c - 'A';
            else if (c >= 'a' && c <= 'z') vals[j] = c - 'a' + 26;
            else if (c >= '0' && c <= '9') vals[j] = c - '0' + 52;
            else if (c == '+') vals[j] = 62;
            else if (c == '/') vals[j] = 63;
            else vals[j] = 0;
        }
        unsigned triple = (vals[0] << 18) | (vals[1] << 12) | (vals[2] << 6) | vals[3];
        out[o++] = (triple >> 16) & 0xFF;
        out[o++] = (triple >> 8) & 0xFF;
        out[o++] = triple & 0xFF;
    }
    *outlen = o;
    return out;
}

/* Build PAX records from xattrs; caller must free returned string */
static char *build_pax_records_from_xattrs(const char *path)
{
    ssize_t listlen = listxattr(path, NULL, 0);
    if (listlen <= 0) return NULL;
    char *list = malloc(listlen);
    if (!list) return NULL;
    listlen = listxattr(path, list, listlen);
    if (listlen <= 0) { free(list); return NULL; }

    char *p = list;
    char *out = NULL;
    size_t outlen = 0;

    while (p < list + listlen) {
        size_t n = strlen(p);
        ssize_t vlen = getxattr(path, p, NULL, 0);
        if (vlen >= 0) {
            unsigned char *vbuf = malloc(vlen);
            if (vbuf) {
                if (getxattr(path, p, vbuf, vlen) == vlen) {
                    char *b64 = base64_encode(vbuf, vlen);
                    if (b64) {
                        size_t keylen = strlen("SCHILY.xattr.") + n + strlen(".base64") + 1;
                        char *key = malloc(keylen);
                        if (key) {
                            snprintf(key, keylen, "SCHILY.xattr.%s.base64", p);
                            size_t kvlen = strlen(key) + 1 + strlen(b64) + 1;
                            char tmp[64];
                            size_t lenfield = 1;
                            size_t total;
                            while (1) {
                                total = lenfield + 1 + kvlen;
                                int nn = snprintf(tmp, sizeof(tmp), "%zu", total);
                                if ((size_t)nn == lenfield) break;
                                lenfield = (size_t)nn;
                            }
                            size_t recsize = total;
                            char *rec = malloc(recsize + 1);
                            if (rec) {
                                snprintf(rec, recsize + 1, "%zu %s=%s\n", recsize, key, b64);
                                char *nout = realloc(out, outlen + recsize);
                                if (nout) {
                                    memcpy(nout + outlen, rec, recsize);
                                    out = nout;
                                    outlen += recsize;
                                }
                                free(rec);
                            }
                            free(key);
                        }
                        free(b64);
                    }
                }
                free(vbuf);
            }
        }
        p += n + 1;
    }
    free(list);
    return out;
}

/* ---------------------------
   Chunked streaming compressor
   --------------------------- */

#define CHUNK_SIZE (4 * 1024 * 1024) /* 4 MiB */

typedef struct {
    unsigned char *buf;
    size_t len;
    size_t cap;
    FILE *out;
} chunk_buffer_t;

static int chunk_buffer_init(chunk_buffer_t *cb, size_t cap, FILE *out)
{
    cb->buf = malloc(cap);
    if (!cb->buf) return -1;
    cb->len = 0;
    cb->cap = cap;
    cb->out = out;
    return 0;
}

static void chunk_buffer_free(chunk_buffer_t *cb) { free(cb->buf); cb->buf = NULL; cb->len = cb->cap = 0; cb->out = NULL; }

static int write_u32_le_file(FILE *f, uint32_t v)
{
    unsigned char b[4];
    b[0] = v & 0xFF;
    b[1] = (v >> 8) & 0xFF;
    b[2] = (v >> 16) & 0xFF;
    b[3] = (v >> 24) & 0xFF;
    return fwrite(b, 1, 4, f) == 4 ? 0 : -1;
}

/* flush: compress cb->buf and write framed chunk; update status */
static int chunk_buffer_flush(chunk_buffer_t *cb)
{
    if (cb->len == 0) return 0;
    size_t inlen = cb->len;
    size_t outcap = inlen + (inlen / 10) + 65536;
    char *outbuf = malloc(outcap);
    if (!outbuf) return -1;

    time_t t0 = time(NULL);
    int csize = lz4_compress_simple((const char*)cb->buf, outbuf, (int)inlen, (int)outcap);
    time_t t1 = time(NULL);
    if (csize <= 0) { free(outbuf); return -1; }

    /* write framing */
    if (write_u32_le_file(cb->out, (uint32_t)inlen) != 0) { free(outbuf); return -1; }
    if (write_u32_le_file(cb->out, (uint32_t)csize) != 0) { free(outbuf); return -1; }
    if (fwrite(outbuf, 1, csize, cb->out) != (size_t)csize) { free(outbuf); return -1; }

    /* update status */
    global_status.compressed_bytes += (unsigned long)csize;
    if (global_status.processed_bytes > 0) {
        global_status.avg_ratio = (double)global_status.compressed_bytes / (double)global_status.processed_bytes;
    }
    double chunk_seconds = difftime(t1, t0);
    if (chunk_seconds < 0.001) chunk_seconds = 0.001;
    double chunk_mb = (double)inlen / (1024.0*1024.0);
    global_status.current_speed_MB_s = chunk_mb / chunk_seconds;

    if (global_status.total_bytes > 0 && global_status.current_speed_MB_s > 0.0001) {
        double remaining_mb = ((double)global_status.total_bytes - (double)global_status.processed_bytes) / (1024.0*1024.0);
        global_status.eta_unix = time(NULL) + (time_t)(remaining_mb / global_status.current_speed_MB_s);
    }

    free(outbuf);
    cb->len = 0;
    return 0;
}

/* append bytes to chunk buffer; update status and flush when full */
static int chunk_buffer_append(chunk_buffer_t *cb, const void *data, size_t n)
{
    const unsigned char *p = data;
    while (n > 0) {
        size_t space = cb->cap - cb->len;
        if (space == 0) {
            if (chunk_buffer_flush(cb) != 0) return -1;
            space = cb->cap - cb->len;
        }
        size_t tocopy = (n < space) ? n : space;
        memcpy(cb->buf + cb->len, p, tocopy);
        cb->len += tocopy;
        p += tocopy;
        n -= tocopy;

        /* STATUS update */
        global_status.processed_bytes += tocopy;
        time_t now = time(NULL);
        double elapsed = difftime(now, global_status.start_time);
        if (elapsed > 0.5) {
            global_status.current_speed_MB_s = (global_status.processed_bytes / (1024.0*1024.0)) / elapsed;
        }
        if (global_status.total_bytes > 0) {
            global_status.progress = (double)global_status.processed_bytes / (double)global_status.total_bytes;
            if (global_status.progress > 1.0) global_status.progress = 1.0;
            if (global_status.current_speed_MB_s > 0.0001) {
                double remaining_mb = ((double)global_status.total_bytes - (double)global_status.processed_bytes) / (1024.0*1024.0);
                global_status.eta_unix = now + (time_t)(remaining_mb / global_status.current_speed_MB_s);
            }
        }
        /* throttle status rendering to ~250ms */
        if (difftime(now, global_status.last_update_time) >= 0.25) {
            render_status(&global_status);
            global_status.last_update_time = now;
        }
    }
    return 0;
}

static int chunk_buffer_finalize_tar_and_flush(chunk_buffer_t *cb)
{
    size_t pad = (TAR_BLOCK - (cb->len % TAR_BLOCK)) % TAR_BLOCK;
    if (pad) {
        unsigned char zeros[TAR_BLOCK];
        memset(zeros, 0, pad);
        if (chunk_buffer_append(cb, zeros, pad) != 0) return -1;
    }
    unsigned char zeros[TAR_BLOCK];
    memset(zeros, 0, TAR_BLOCK);
    if (chunk_buffer_append(cb, zeros, TAR_BLOCK) != 0) return -1;
    if (chunk_buffer_append(cb, zeros, TAR_BLOCK) != 0) return -1;
    if (chunk_buffer_flush(cb) != 0) return -1;
    return 0;
}

/* ---------------------------
   Streamed tar write functions
   --------------------------- */

static int stream_write_file_entry(chunk_buffer_t *cb, const char *name, const struct stat *st, const char *path)
{
    unsigned char header[TAR_BLOCK];
    make_tar_header(name, st, NULL, header, 0, 0);
    if (chunk_buffer_append(cb, header, TAR_BLOCK) != 0) {
        global_status.error_count++;
        snprintf(global_status.last_failure, sizeof(global_status.last_failure), "Failed header write for %s: %s", name, strerror(errno));
        snprintf(global_status.last_error_file, sizeof(global_status.last_error_file), "%s", name);
        return -1;
    }

    /* update filename in status */
    snprintf(global_status.filename, sizeof(global_status.filename), "%s", name);

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        global_status.error_count++;
        snprintf(global_status.last_failure, sizeof(global_status.last_failure), "Open failed %s: %s", path, strerror(errno));
        snprintf(global_status.last_error_file, sizeof(global_status.last_error_file), "%s", path);
        return -1;
    }
    char buf[64*1024];
    ssize_t toread = st->st_size;
    while (toread > 0) {
        ssize_t r = read(fd, buf, (sizeof(buf) < (size_t)toread) ? sizeof(buf) : toread);
        if (r <= 0) { close(fd);
            global_status.error_count++;
            snprintf(global_status.last_failure, sizeof(global_status.last_failure), "Read failed %s: %s", path, strerror(errno));
            snprintf(global_status.last_error_file, sizeof(global_status.last_error_file), "%s", path);
            return -1;
        }
        if (chunk_buffer_append(cb, buf, r) != 0) { close(fd);
            global_status.error_count++;
            snprintf(global_status.last_failure, sizeof(global_status.last_failure), "Chunk append failed %s", path);
            snprintf(global_status.last_error_file, sizeof(global_status.last_error_file), "%s", path);
            return -1;
        }
        toread -= r;
    }
    close(fd);
    size_t pad = (TAR_BLOCK - (st->st_size % TAR_BLOCK)) % TAR_BLOCK;
    if (pad) {
        unsigned char zeros[TAR_BLOCK];
        memset(zeros, 0, pad);
        if (chunk_buffer_append(cb, zeros, pad) != 0) {
            global_status.error_count++;
            snprintf(global_status.last_failure, sizeof(global_status.last_failure), "Padding failed %s", path);
            snprintf(global_status.last_error_file, sizeof(global_status.last_error_file), "%s", path);
            return -1;
        }
    }

    /* clear filename */
    global_status.filename[0] = '\0';
    return 0;
}

static int stream_write_symlink_entry(chunk_buffer_t *cb, const char *name, const struct stat *st, const char *path)
{
    char linktarget[101];
    ssize_t L = readlink(path, linktarget, sizeof(linktarget)-1);
    if (L < 0) {
        global_status.error_count++;
        snprintf(global_status.last_failure, sizeof(global_status.last_failure), "readlink failed %s: %s", path, strerror(errno));
        snprintf(global_status.last_error_file, sizeof(global_status.last_error_file), "%s", path);
        return -1;
    }
    linktarget[L] = '\0';
    unsigned char header[TAR_BLOCK];
    make_tar_header(name, st, linktarget, header, 0, 0);
    if (chunk_buffer_append(cb, header, TAR_BLOCK) != 0) {
        global_status.error_count++;
        snprintf(global_status.last_failure, sizeof(global_status.last_failure), "Header write failed for symlink %s", name);
        snprintf(global_status.last_error_file, sizeof(global_status.last_error_file), "%s", name);
        return -1;
    }
    return 0;
}

static int stream_write_device_entry(chunk_buffer_t *cb, const char *name, const struct stat *st)
{
    unsigned long maj = (unsigned long)major(st->st_rdev);
    unsigned long min = (unsigned long)minor(st->st_rdev);
    unsigned char header[TAR_BLOCK];
    make_tar_header(name, st, NULL, header, maj, min);
    if (chunk_buffer_append(cb, header, TAR_BLOCK) != 0) {
        global_status.error_count++;
        snprintf(global_status.last_failure, sizeof(global_status.last_failure), "Header write failed for device %s", name);
        snprintf(global_status.last_error_file, sizeof(global_status.last_error_file), "%s", name);
        return -1;
    }
    return 0;
}

static int stream_write_dir_entry(chunk_buffer_t *cb, const char *name, const struct stat *st)
{
    char namebuf[PATH_MAX];
    snprintf(namebuf, sizeof(namebuf), "%s", name);
    size_t len = strlen(namebuf);
    if (len == 0 || namebuf[len-1] != '/') {
        if (len + 1 < sizeof(namebuf)) { namebuf[len] = '/'; namebuf[len+1] = '\0'; }
    }
    unsigned char header[TAR_BLOCK];
    make_tar_header(namebuf, st, NULL, header, 0, 0);
    if (chunk_buffer_append(cb, header, TAR_BLOCK) != 0) {
        global_status.error_count++;
        snprintf(global_status.last_failure, sizeof(global_status.last_failure), "Header write failed for dir %s", name);
        snprintf(global_status.last_error_file, sizeof(global_status.last_error_file), "%s", name);
        return -1;
    }
    return 0;
}

static int stream_write_pax_header(chunk_buffer_t *cb, const char *entry_name, const char *pax_records)
{
    size_t paxlen = pax_records ? strlen(pax_records) : 0;
    unsigned char header[TAR_BLOCK];
    struct stat st_zero;
    memset(&st_zero, 0, sizeof(st_zero));
    struct stat st_for_header = st_zero;
    st_for_header.st_size = paxlen;
    memset(header, 0, TAR_BLOCK);
    strncpy((char*)header + 0, entry_name, 100);
    tar_write_octal((char*)header + 100, 8, st_for_header.st_mode & 07777);
    tar_write_octal((char*)header + 108, 8, (unsigned long)st_for_header.st_uid);
    tar_write_octal((char*)header + 116, 8, (unsigned long)st_for_header.st_gid);
    tar_write_octal((char*)header + 124, 12, (unsigned long)paxlen);
    tar_write_octal((char*)header + 136, 12, (unsigned long)st_for_header.st_mtime);
    memset(header + 148, ' ', 8);
    header[156] = 'x';
    memcpy(header + 257, "ustar", 5);
    header[262] = '0';
    unsigned long sum = 0;
    for (int i = 0; i < TAR_BLOCK; ++i) sum += header[i];
    tar_write_octal((char*)header + 148, 8, sum);

    if (chunk_buffer_append(cb, header, TAR_BLOCK) != 0) {
        global_status.error_count++;
        snprintf(global_status.last_failure, sizeof(global_status.last_failure), "PAX header write failed for %s", entry_name);
        snprintf(global_status.last_error_file, sizeof(global_status.last_error_file), "%s", entry_name);
        return -1;
    }
    if (paxlen > 0) {
        if (chunk_buffer_append(cb, pax_records, paxlen) != 0) {
            global_status.error_count++;
            snprintf(global_status.last_failure, sizeof(global_status.last_failure), "PAX data write failed for %s", entry_name);
            snprintf(global_status.last_error_file, sizeof(global_status.last_error_file), "%s", entry_name);
            return -1;
        }
        size_t pad = (TAR_BLOCK - (paxlen % TAR_BLOCK)) % TAR_BLOCK;
        if (pad) {
            unsigned char zeros[TAR_BLOCK];
            memset(zeros, 0, pad);
            if (chunk_buffer_append(cb, zeros, pad) != 0) {
                global_status.error_count++;
                snprintf(global_status.last_failure, sizeof(global_status.last_failure), "PAX pad failed for %s", entry_name);
                snprintf(global_status.last_error_file, sizeof(global_status.last_error_file), "%s", entry_name);
                return -1;
            }
        }
    }
    return 0;
}

/* Recursively add path into stream */
static int add_path_stream_recursive(chunk_buffer_t *cb, const char *fullpath, const char *name_in_tar)
{
    struct stat st;
    if (lstat(fullpath, &st) != 0) {
        global_status.error_count++;
        snprintf(global_status.last_failure, sizeof(global_status.last_failure), "lstat failed %s: %s", fullpath, strerror(errno));
        snprintf(global_status.last_error_file, sizeof(global_status.last_error_file), "%s", fullpath);
        return -1;
    }

    char *pax = build_pax_records_from_xattrs(fullpath);
    if (pax) {
        /* set filename for status */
        snprintf(global_status.filename, sizeof(global_status.filename), "%s", name_in_tar);
        if (stream_write_pax_header(cb, name_in_tar, pax) != 0) { free(pax); return -1; }
        free(pax);
    }

    if (S_ISREG(st.st_mode)) {
        if (stream_write_file_entry(cb, name_in_tar, &st, fullpath) != 0) return -1;
    } else if (S_ISLNK(st.st_mode)) {
        if (stream_write_symlink_entry(cb, name_in_tar, &st, fullpath) != 0) return -1;
    } else if (S_ISDIR(st.st_mode)) {
        if (stream_write_dir_entry(cb, name_in_tar, &st) != 0) return -1;
        DIR *d = opendir(fullpath);
        if (!d) { global_status.error_count++; snprintf(global_status.last_failure, sizeof(global_status.last_failure), "opendir failed %s: %s", fullpath, strerror(errno)); snprintf(global_status.last_error_file, sizeof(global_status.last_error_file), "%s", fullpath); return -1; }
        struct dirent *ent;
        while ((ent = readdir(d)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            char childfull[PATH_MAX];
            char childtar[PATH_MAX];
            snprintf(childfull, sizeof(childfull), "%s/%s", fullpath, ent->d_name);
            snprintf(childtar, sizeof(childtar), "%s/%s", name_in_tar, ent->d_name);
            if (add_path_stream_recursive(cb, childfull, childtar) != 0) { closedir(d); return -1; }
        }
        closedir(d);
    } else if (S_ISCHR(st.st_mode) || S_ISBLK(st.st_mode)) {
        if (stream_write_device_entry(cb, name_in_tar, &st) != 0) return -1;
    } else {
        /* skip other types */
    }
    return 0;
}

/* ---------------------------
   Decompress and tar parser / extractor
   --------------------------- */

static int read_u32_le_file(FILE *f, uint32_t *out)
{
    unsigned char b[4];
    if (fread(b, 1, 4, f) != 4) return -1;
    *out = (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
    return 0;
}

typedef struct {
    char *pax_records;
    size_t pax_len;
    char outdir[PATH_MAX];
} tar_parser_t;

static int ensure_parent_dir(const char *path)
{
    char tmp[PATH_MAX];
    strncpy(tmp, path, sizeof(tmp));
    char *p = strrchr(tmp, '/');
    if (!p) return 0;
    *p = '\0';
    struct stat st;
    if (stat(tmp, &st) == 0) return 0;
    /* create recursively */
    char accum[PATH_MAX] = "";
    char *q = tmp;
    while (*q == '/') q++;
    char *tok = strtok(q, "/");
    while (tok) {
        strcat(accum, "/");
        strcat(accum, tok);
        mkdir(accum, 0755);
        tok = strtok(NULL, "/");
    }
    return 0;
}

static void apply_pax_records_to_path(const char *path, const char *pax, size_t paxlen)
{
    if (!pax || paxlen == 0) return;
    size_t i = 0;
    while (i < paxlen) {
        size_t j = i;
        while (j < paxlen && pax[j] != ' ') j++;
        if (j >= paxlen) break;
        char numbuf[32];
        size_t nlen = j - i;
        if (nlen >= sizeof(numbuf)) break;
        memcpy(numbuf, pax + i, nlen);
        numbuf[nlen] = '\0';
        size_t reclen = (size_t)strtoul(numbuf, NULL, 10);
        if (reclen == 0 || i + reclen > paxlen) break;
        size_t kvlen = reclen - (nlen + 1);
        char *kv = malloc(kvlen + 1);
        if (!kv) break;
        memcpy(kv, pax + j + 1, kvlen);
        kv[kvlen] = '\0';
        char *eq = strchr(kv, '=');
        if (eq) {
            *eq = '\0';
            char *key = kv;
            char *val = eq + 1;
            const char *prefix = "SCHILY.xattr.";
            const char *suffix = ".base64";
            if (strncmp(key, prefix, strlen(prefix)) == 0) {
                size_t keylen = strlen(key);
                if (keylen > strlen(prefix) + strlen(suffix) &&
                    strcmp(key + keylen - strlen(suffix), suffix) == 0) {
                    size_t namelen = keylen - strlen(prefix) - strlen(suffix);
                    char *xname = malloc(namelen + 1);
                    if (xname) {
                        memcpy(xname, key + strlen(prefix), namelen);
                        xname[namelen] = '\0';
                        size_t voutlen;
                        unsigned char *vout = base64_decode(val, &voutlen);
                        if (vout) {
                            setxattr(path, xname, vout, voutlen, 0);
                            free(vout);
                        }
                        free(xname);
                    }
                }
            }
        }
        free(kv);
        i += reclen;
    }
}

/* tar_parser_consume: consumes as many full tar records as possible from tar_data[0..tar_len-1]
   If extract==1, extracts files into parser->outdir; if extract==0 and out_tar_fp!=NULL, writes raw tar bytes to out_tar_fp.
   Returns number of bytes consumed (>=0) or -1 on error. */
static int tar_parser_consume(tar_parser_t *parser, const unsigned char *tar_data, size_t tar_len, int extract, FILE *out_tar_fp)
{
    size_t pos = 0;
    while (pos + TAR_BLOCK <= tar_len) {
        const unsigned char *h = tar_data + pos;
        int allzero = 1;
        for (int i = 0; i < TAR_BLOCK; ++i) if (h[i]) { allzero = 0; break; }
        if (allzero) {
            pos += TAR_BLOCK;
            if (pos + TAR_BLOCK <= tar_len) {
                int allzero2 = 1;
                for (int i = 0; i < TAR_BLOCK; ++i) if (tar_data[pos + i]) { allzero2 = 0; break; }
                if (allzero2) {
                    pos += TAR_BLOCK;
                    if (!extract && out_tar_fp) {
                        if (fwrite(tar_data, 1, pos, out_tar_fp) != pos) return -1;
                    }
                    return (int)pos;
                }
            } else break;
        }
        char name[101]; memcpy(name, h + 0, 100); name[100] = '\0';
        char sizebuf[13]; memcpy(sizebuf, h + 124, 12); sizebuf[12] = '\0';
        unsigned long filesize = strtoul(sizebuf, NULL, 8);
        char typeflag = h[156];
        char linkname[101]; memcpy(linkname, h + 157, 100); linkname[100] = '\0';

        if (typeflag == 'x') {
            size_t pax_blocks = (filesize + TAR_BLOCK - 1) / TAR_BLOCK;
            size_t pax_total = pax_blocks * TAR_BLOCK;
            if (pos + TAR_BLOCK + pax_total > tar_len) break;
            if (parser->pax_records) { free(parser->pax_records); parser->pax_records = NULL; parser->pax_len = 0; }
            if (filesize > 0) {
                parser->pax_records = malloc(filesize);
                if (!parser->pax_records) return -1;
                memcpy(parser->pax_records, tar_data + pos + TAR_BLOCK, filesize);
                parser->pax_len = filesize;
            }
            pos += TAR_BLOCK + pax_total;
            continue;
        }

        size_t data_blocks = (filesize + TAR_BLOCK - 1) / TAR_BLOCK;
        size_t total_needed = TAR_BLOCK + data_blocks * TAR_BLOCK;
        if (pos + total_needed > tar_len) break;

        if (!extract && out_tar_fp) {
            if (fwrite(tar_data + pos, 1, total_needed, out_tar_fp) != total_needed) return -1;
            pos += total_needed;
            if (parser->pax_records) { free(parser->pax_records); parser->pax_records = NULL; parser->pax_len = 0; }
            continue;
        }

        char outpath[PATH_MAX];
        if (parser->outdir[0]) snprintf(outpath, sizeof(outpath), "%s/%s", parser->outdir, name);
        else snprintf(outpath, sizeof(outpath), "%s", name);

        if (typeflag == '0' || typeflag == '\0') {
            ensure_parent_dir(outpath);
            int fd = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (fd < 0) { perror("open"); return -1; }
            const unsigned char *data = tar_data + pos + TAR_BLOCK;
            if (filesize > 0) {
                if (write(fd, data, filesize) != (ssize_t)filesize) { close(fd); perror("write"); return -1; }
            }
            close(fd);
            if (parser->pax_records) apply_pax_records_to_path(outpath, parser->pax_records, parser->pax_len);
            char modebuf[9]; memcpy(modebuf, h + 100, 8); modebuf[8] = '\0';
            mode_t mode = (mode_t)strtoul(modebuf, NULL, 8);
            chmod(outpath, mode);
            char mtimebuf[13]; memcpy(mtimebuf, h + 136, 12); mtimebuf[12] = '\0';
            time_t mtime = (time_t)strtoul(mtimebuf, NULL, 8);
            struct utimbuf ut; ut.actime = mtime; ut.modtime = mtime;
            utime(outpath, &ut);
        } else if (typeflag == '2') {
            ensure_parent_dir(outpath);
            if (symlink(linkname, outpath) != 0) { perror("symlink"); return -1; }
            if (parser->pax_records) apply_pax_records_to_path(outpath, parser->pax_records, parser->pax_len);
        } else if (typeflag == '3' || typeflag == '4') {
            ensure_parent_dir(outpath);
            char majbuf[9]; memcpy(majbuf, h + 329, 8); majbuf[8] = '\0';
            char minbuf[9]; memcpy(minbuf, h + 337, 8); minbuf[8] = '\0';
            unsigned long maj = strtoul(majbuf, NULL, 8);
            unsigned long min = strtoul(minbuf, NULL, 8);
            mode_t mode = (mode_t)strtoul((char*)(h + 100), NULL, 8);
            dev_t dev = makedev(maj, min);
            if (mknod(outpath, (typeflag == '3' ? S_IFCHR : S_IFBLK) | mode, dev) != 0) { perror("mknod"); return -1; }
            if (parser->pax_records) apply_pax_records_to_path(outpath, parser->pax_records, parser->pax_len);
        } else if (typeflag == '5') {
            ensure_parent_dir(outpath);
            mkdir(outpath, 0755);
            if (parser->pax_records) apply_pax_records_to_path(outpath, parser->pax_records, parser->pax_len);
        } else {
            /* skip */
        }

        pos += total_needed;
        if (parser->pax_records) { free(parser->pax_records); parser->pax_records = NULL; parser->pax_len = 0; }
    }

    if (!extract && out_tar_fp) {
        if (fwrite(tar_data, 1, pos, out_tar_fp) != pos) return -1;
    }

    if (pos > 0) {
        size_t rem = tar_len - pos;
        memmove((void*)tar_data, tar_data + pos, rem);
    }
    return (int)pos;
}

static int decompress_and_process(const char *inpath, int extract, const char *outpath)
{
    FILE *fin = fopen(inpath, "rb");
    if (!fin) { perror("fopen"); return -1; }
    FILE *out_tar_fp = NULL;
    if (!extract && outpath) {
        out_tar_fp = fopen(outpath, "wb");
        if (!out_tar_fp) { perror("fopen out tar"); fclose(fin); return -1; }
    }

    size_t tarbuf_cap = CHUNK_SIZE * 2;
    unsigned char *tarbuf = malloc(tarbuf_cap);
    size_t tarbuf_len = 0;

    tar_parser_t parser;
    parser.pax_records = NULL;
    parser.pax_len = 0;
    parser.outdir[0] = '\0';
    if (extract && outpath) strncpy(parser.outdir, outpath, sizeof(parser.outdir)-1);

    while (1) {
        uint32_t orig_size, comp_size;
        if (read_u32_le_file(fin, &orig_size) != 0) break;
        if (read_u32_le_file(fin, &comp_size) != 0) break;
        if (comp_size == 0 || orig_size == 0) break;
        char *compbuf = malloc(comp_size);
        if (!compbuf) { fclose(fin); free(tarbuf); return -1; }
        if (fread(compbuf, 1, comp_size, fin) != comp_size) { free(compbuf); break; }
        size_t needed = orig_size;
        if (tarbuf_len + needed > tarbuf_cap) {
            size_t newcap = tarbuf_cap;
            while (tarbuf_len + needed > newcap) newcap *= 2;
            unsigned char *n = realloc(tarbuf, newcap);
            if (!n) { free(compbuf); fclose(fin); free(tarbuf); return -1; }
            tarbuf = n; tarbuf_cap = newcap;
        }
        int dsize = lz4_decompress_simple(compbuf, (char*)(tarbuf + tarbuf_len), comp_size, (int)needed);
        free(compbuf);
        if (dsize < 0) { fprintf(stderr, "Decompression error\n"); fclose(fin); free(tarbuf); return -1; }
        tarbuf_len += dsize;

        /* update status */
        global_status.processed_bytes += orig_size;
        global_status.compressed_bytes += comp_size;
        if (global_status.processed_bytes > 0) global_status.avg_ratio = (double)global_status.compressed_bytes / (double)global_status.processed_bytes;
        time_t now = time(NULL);
        double elapsed = difftime(now, global_status.start_time);
        if (elapsed > 0.5) global_status.current_speed_MB_s = (global_status.processed_bytes / (1024.0*1024.0)) / elapsed;
        if (global_status.total_bytes > 0 && global_status.current_speed_MB_s > 0.0001) {
            double remaining_mb = ((double)global_status.total_bytes - (double)global_status.processed_bytes) / (1024.0*1024.0);
            global_status.eta_unix = now + (time_t)(remaining_mb / global_status.current_speed_MB_s);
        }
        if (difftime(now, global_status.last_update_time) >= 0.25) {
            render_status(&global_status);
            global_status.last_update_time = now;
        }

        size_t consumed = 0;
        while (1) {
            int c = tar_parser_consume(&parser, tarbuf + consumed, tarbuf_len - consumed, extract, out_tar_fp);
            if (c < 0) { fprintf(stderr, "Tar parse error\n"); fclose(fin); free(tarbuf); return -1; }
            if ((size_t)c == 0) break;
            consumed += c;
            if (consumed >= tarbuf_len) break;
        }
        if (consumed > 0) {
            size_t rem = tarbuf_len - consumed;
            memmove(tarbuf, tarbuf + consumed, rem);
            tarbuf_len = rem;
        }
    }

    if (parser.pax_records) free(parser.pax_records);
    if (out_tar_fp) fclose(out_tar_fp);
    fclose(fin);
    free(tarbuf);
    return 0;
}

/* ---------------------------
   Main and CLI
   --------------------------- */

static void usage(const char *prog)
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  Create:  %s create out.tar.lz4 input1 [input2 ...]\n", prog);
    fprintf(stderr, "  Extract: %s extract in.tar.lz4 [outdir]\n", prog);
    fprintf(stderr, "  Tar out: %s tarout in.tar.lz4 out.tar\n", prog);
}

int main(int argc, char **argv)
{
    if (argc < 2) { usage(argv[0]); return 2; }
    const char *cmd = argv[1];

    /* initialize status */
    memset(&global_status, 0, sizeof(global_status));
    global_status.start_time = time(NULL);
    global_status.last_update_time = 0;

    if (strcmp(cmd, "create") == 0) {
        if (argc < 4) { usage(argv[0]); return 2; }
        const char *outpath = argv[2];
        FILE *fout = fopen(outpath, "wb");
        if (!fout) { perror("fopen out"); return 1; }
        chunk_buffer_t cb;
        if (chunk_buffer_init(&cb, CHUNK_SIZE, fout) != 0) { fprintf(stderr, "alloc failed\n"); fclose(fout); return 1; }

        /* Optional: compute total_bytes by stat recursion to improve ETA */
        unsigned long total = 0;
        for (int i = 3; i < argc; ++i) {
            /* simple recursive size accumulation */
            /* For brevity, we do a shallow stat for files and skip directories here.
               For accurate ETA, implement a full recursion to sum file sizes. */
            struct stat st;
            if (stat(argv[i], &st) == 0) {
                if (S_ISREG(st.st_mode)) total += (unsigned long)st.st_size;
            }
        }
        global_status.total_bytes = total;

        for (int i = 3; i < argc; ++i) {
            const char *p = argv[i];
            const char *bn = strrchr(p, '/');
            const char *name_in_tar = bn ? bn + 1 : p;
            if (add_path_stream_recursive(&cb, p, name_in_tar) != 0) {
                fprintf(stderr, "Failed to add path: %s\n", p);
                chunk_buffer_free(&cb);
                fclose(fout);
                return 1;
            }
        }

        if (chunk_buffer_finalize_tar_and_flush(&cb) != 0) {
            fprintf(stderr, "Failed to finalize/flush\n");
            chunk_buffer_free(&cb);
            fclose(fout);
            return 1;
        }

        chunk_buffer_free(&cb);
        fclose(fout);
        /* final render and move cursor down */
        render_status(&global_status);
        printf("\x1b[%dB\n", 8);
        printf("Created %s\n", outpath);
        return 0;
    } else if (strcmp(cmd, "extract") == 0 || strcmp(cmd, "tarout") == 0) {
        if (argc < 3) { usage(argv[0]); return 2; }
        const char *inpath = argv[2];
        if (strcmp(cmd, "extract") == 0) {
            const char *outdir = (argc >= 4) ? argv[3] : ".";
            mkdir(outdir, 0755);
            if (decompress_and_process(inpath, 1, outdir) != 0) {
                fprintf(stderr, "Extraction failed\n");
                return 1;
            }
            render_status(&global_status);
            printf("\x1b[%dB\n", 8);
            printf("Extracted to %s\n", outdir);
            return 0;
        } else {
            if (argc < 4) { usage(argv[0]); return 2; }
            const char *outtar = argv[3];
            if (decompress_and_process(inpath, 0, outtar) != 0) {
                fprintf(stderr, "Tar reconstruction failed\n");
                return 1;
            }
            render_status(&global_status);
            printf("\x1b[%dB\n", 8);
            printf("Reconstructed tar %s\n", outtar);
            return 0;
        }
    } else {
        usage(argv[0]);
        return 2;
    }
}
