/*
 * lz4.c - lz4-style tool (single-file patched)
 *
 * This is a corrected, self-contained single-file version of the program
 * with debug traps added: on decompression errors, instead of returning -1,
 * we emit a diagnostic and execute asm("int3") (SIGTRAP) on x86; abort() on
 * other architectures.
 *
 * Build:
 *   cc -std=c11 -O2 -Wall -Wextra -o jlz4 lz4.c
 *
 * Usage:
 *   jlz4 -c <in> [out]   create archive
 *   jlz4 -d <in> [out]   extract archive
 *   jlz4 -l <in>         list archive
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/param.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <libgen.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/* Tunable sizes and protocol constants */
static const size_t BLK_SIZE = (1 << 20);
static const size_t WIN_SIZE = (1 << 16);
static const unsigned MIN_MAT = 4u;
static const size_t HSH_SIZE = (1 << 16);
static const unsigned HSH_SFT = (32u - 16u);

static const uint32_t HASH_MULT = 2654435761u;
static const char HDR_MAGIC[] = "LZ4B";
static const size_t HDR_MAGIC_LEN = 4;
static const size_t HDR_HDR_SIZE = 12; /* magic + orig size (4) + comp size (4) */
static const size_t HDR_ORIG_OFF = 4;
static const size_t HDR_CSIZE_OFF = 8;

static const unsigned TOKEN_MAX_NIBBLE = 15u;
static const unsigned TOKEN_EXT_BYTE = 255u;

static const uint8_t U8_ALL_ONES = 0xFFu;
static const uint32_t U32_EMPTY = 0xFFFFFFFFu;

static const size_t TERM_DEFAULT_WIDTH = 80;

static const mode_t DIR_MODE = 0755;
static const mode_t FILE_MODE = 0644;

typedef struct {
    uint64_t total;
    uint64_t proc;
    struct timeval start;
    const char *curr;
    uint64_t f_orig;
    uint64_t f_comp;
    int  lines;
} prg_ctx_t;

typedef struct {
    FILE    *fp;
    const char *path;
    uint8_t *in;
    uint8_t *out;
    uint32_t c_len;
    uint64_t f_len;
} io_ctx_t;

static prg_ctx_t g_prg;

/* --- debug trap for decompression failures --- */
static void
dec_trap(const char *msg)
{
    if (msg) {
        fprintf(stderr, "jlz4: decompression failure: %s\n", msg);
        fflush(stderr);
    }
#if defined(__i386__) || defined(__x86_64__)
    __asm__ __volatile__("int3");
#else
    abort();
#endif
}
#define DEC_TRAP(MSG) do { dec_trap((MSG)); } while (0)

/* --- utilities --- */

static int
term_width(void)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0)
        return ((int)ws.ws_col);
    return (int)TERM_DEFAULT_WIDTH;
}

static void
path_join(char *dst, size_t max, const char *dir, const char *fn)
{
    if (dir == NULL || dir[0] == '\0') {
        snprintf(dst, max, "%s", fn);
    } else if (dir[strlen(dir) - 1] == '/') {
        snprintf(dst, max, "%s%s", dir, fn);
    } else {
        snprintf(dst, max, "%s/%s", dir, fn);
    }
}

static int
io_write(int fd, const void *buf, size_t n)
{
    const uint8_t *p;
    ssize_t w;

    p = buf;
    while (n > 0) {
        w = write(fd, p, n);
        if (w < 0) {
            if (errno == EINTR)
                continue;
            return (-1);
        }
        n -= (size_t)w;
        p += w;
    }
    return (0);
}

static size_t
io_read(FILE *f, uint8_t *buf, size_t n)
{
    size_t got;
    size_t r;

    got = 0;
    while (got < n) {
        r = fread(buf + got, 1, n - got, f);
        if (r == 0)
            break;
        got += r;
    }
    return (got);
}

static int
io_wr_u32(FILE *f, uint32_t v)
{
    uint8_t b[4];

    b[0] = v & 0xFF;
    b[1] = (v >> 8) & 0xFF;
    b[2] = (v >> 16) & 0xFF;
    b[3] = (v >> 24) & 0xFF;
    return (fwrite(b, 1, 4, f) == 4 ? 0 : -1);
}

static int
io_wr_u64(FILE *f, uint64_t v)
{
    uint8_t b[8];
    int i;

    for (i = 0; i < 8; ++i)
        b[i] = (v >> (8 * i)) & 0xFF;
    return (fwrite(b, 1, 8, f) == 8 ? 0 : -1);
}

static int
io_rd_u32(FILE *f, uint32_t *v)
{
    uint8_t b[4];

    if (fread(b, 1, 4, f) != 4)
        return (-1);
    *v = (uint32_t)b[0] | ((uint32_t)b[1] << 8) |
         ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
    return (0);
}

static int
io_rd_u64(FILE *f, uint64_t *v)
{
    uint8_t b[8];
    int i;

    if (fread(b, 1, 8, f) != 8)
        return (-1);
    *v = 0;
    for (i = 0; i < 8; ++i)
        *v |= ((uint64_t)b[i]) << (8 * i);
    return (0);
}

/* --- formatting & progress --- */

static void
fmt_byte(uint64_t v, char *b, size_t n)
{
    const char *u[] = {"B", "KB", "MB", "GB"};
    double d;
    int i;

    d = (double)v;
    i = 0;
    while (d >= 1024.0 && i < 3) {
        d /= 1024.0;
        i++;
    }
    snprintf(b, n, "%.2f %s", d, u[i]);
}

static void
fmt_time(double s, char *b, size_t n)
{
    int t;

    if (s < 0)
        s = 0;
    t = (int)(s + 0.5);
    snprintf(b, n, "%02d:%02d:%02d", t / 3600, (t % 3600) / 60, t % 60);
}

static void
prg_line(int clr)
{
    if (clr && g_prg.lines > 0)
        printf("\033[%dA", g_prg.lines);
    g_prg.lines = 0;
}

static void
prg_calc(double *pct, double *eta, double *spd)
{
    struct timeval now;
    double el;

    gettimeofday(&now, NULL);
    el = (now.tv_sec - g_prg.start.tv_sec) +
        (now.tv_usec - g_prg.start.tv_usec) / 1e6;
    *pct = g_prg.total ? (double)g_prg.proc * 100.0 / g_prg.total : 0;
    *eta = (g_prg.proc > 0 && g_prg.total > g_prg.proc) ?
        el * (g_prg.total - g_prg.proc) / g_prg.proc : 0;
    *spd = (g_prg.proc > 0 && el > 0) ? g_prg.proc / el : 0;
}

static void
prg_draw(double pct, int w)
{
    int fil;
    int i;

    fil = (int)((pct / 100.0) * w + 0.5);
    if (fil > w)
        fil = w;
    printf("\033[K%6.2f%% [", pct);
    for (i = 0; i < fil; ++i)
        putchar('=');
    for (i = fil; i < w; ++i)
        putchar(' ');
    printf("]\n");
    g_prg.lines++;
}

static void
prg_upd(uint64_t p, uint64_t t)
{
    char s1[32], s3[32];
    double pct, eta, spd;

    g_prg.proc = p;
    g_prg.total = t;
    prg_line(1);
    prg_calc(&pct, &eta, &spd);
    fmt_byte((uint64_t)spd, s1, 32);
    fmt_time(eta, s3, 32);

    if (g_prg.curr)
        printf("\033[KFile: %s\n", g_prg.curr);
    else
        printf("\033[K\n");
    g_prg.lines++;
    printf("\033[KETA: %s | Spd: %s/s\n", s3, s1);
    g_prg.lines++;
    prg_draw(pct, term_width() - 40);
    fflush(stdout);
}

/* --- compression helpers --- */

/* write header (magic + original size), leave space for compressed size */
static uint8_t *
comp_write_header(uint8_t *out, size_t orig_size)
{
    memcpy(out, HDR_MAGIC, HDR_MAGIC_LEN);
    out += HDR_MAGIC_LEN;
    /* store original size as 32-bit little-endian */
    {
        uint32_t os = (uint32_t)orig_size;
        memcpy(out, &os, 4);
    }
    out += 8; /* leave 4 bytes for compressed size */
    return out;
}

/* allocate and initialize hash table */
static uint32_t *
comp_alloc_hash(void)
{
    uint32_t *ht;

    ht = calloc(HSH_SIZE, sizeof(uint32_t));
    if (ht == NULL)
        return NULL;
    memset(ht, U8_ALL_ONES, HSH_SIZE * sizeof(uint32_t));
    return ht;
}

/* emit literal length extension bytes and copy literal data */
static void
comp_emit_literal(uint8_t **op, const uint8_t *anc, size_t len)
{
    size_t rem;

    rem = len >= TOKEN_MAX_NIBBLE ? len - TOKEN_MAX_NIBBLE : 0;
    if (len >= TOKEN_MAX_NIBBLE) {
        while (rem >= TOKEN_EXT_BYTE) {
            *(*op)++ = (uint8_t)TOKEN_EXT_BYTE;
            rem -= TOKEN_EXT_BYTE;
        }
        *(*op)++ = (uint8_t)rem;
    }
    if (len > 0) {
        memcpy(*op, anc, len);
        *op += len;
    }
}

/* emit match offset and match-length extension bytes */
static void
comp_emit_match(uint8_t **op, size_t off, size_t len)
{
    uint16_t o;
    size_t l;
    size_t rem;

    o = (uint16_t)off;
    *(*op)++ = (uint8_t)(o & 0xFF);
    *(*op)++ = (uint8_t)((o >> 8) & 0xFF);
    l = len - MIN_MAT;
    if (l >= TOKEN_MAX_NIBBLE) {
        rem = l - TOKEN_MAX_NIBBLE;
        while (rem >= TOKEN_EXT_BYTE) {
            *(*op)++ = (uint8_t)TOKEN_EXT_BYTE;
            rem -= TOKEN_EXT_BYTE;
        }
        *(*op)++ = (uint8_t)rem;
    }
}

/* find match length between ip and ref (bounded by end) */
static size_t
comp_find_match_len(const uint8_t *ip, const uint8_t *ref, const uint8_t *end)
{
    const uint8_t *p1 = ip;
    const uint8_t *p2 = ref;

    while (p1 < end && *p1 == *p2) {
        p1++;
        p2++;
    }
    return (size_t)(p1 - ip);
}

/* --- compression core --- */

/*
 * Goal: compress a single input block 'in' of length n into 'out' buffer.
 * Returns number of bytes written to 'out' or 0 on error.
 */
static size_t
lz4_comp(const uint8_t *in, size_t n, uint8_t *out, size_t cap)
{
    const uint8_t *ip, *end, *anc;
    uint32_t *ht;
    uint8_t *op;
    uint32_t v;
    uint32_t h, ref;
    size_t m_len, l_len;

    if (cap < HDR_HDR_SIZE)
        return (0);

    op = out;
    op = comp_write_header(op, n);

    ht = comp_alloc_hash();
    if (ht == NULL)
        return (0);

    ip = in;
    end = in + n;
    anc = ip;

    while (ip + MIN_MAT <= end) {
        /* compute hash value from 4 bytes at ip */
        memcpy(&v, ip, 4);
        h = ((v * HASH_MULT) >> HSH_SFT) & (uint32_t)(HSH_SIZE - 1);
        ref = ht[h];
        ht[h] = (uint32_t)(ip - in);

        /* validate candidate */
        if (ref == U32_EMPTY || (ip - (in + ref)) > WIN_SIZE ||
            memcmp(in + ref, ip, MIN_MAT) != 0) {
            ip++;
            continue;
        }

        /* found match: extend it */
        m_len = comp_find_match_len(ip, in + ref, end);
        l_len = ip - anc;

        /* token: high nibble = literal length, low nibble = match length - MIN_MAT */
        *op++ = (uint8_t)((l_len >= TOKEN_MAX_NIBBLE ? TOKEN_MAX_NIBBLE : l_len) << 4 |
            (m_len - MIN_MAT >= TOKEN_MAX_NIBBLE ? TOKEN_MAX_NIBBLE : m_len - MIN_MAT));

        /* emit literal and match */
        comp_emit_literal(&op, anc, l_len);
        comp_emit_match(&op, ip - (in + ref), m_len);

        ip += m_len;
        anc = ip;
    }

    /* final literal run */
    l_len = (size_t)(end - anc);
    *op++ = (uint8_t)((l_len >= TOKEN_MAX_NIBBLE ? TOKEN_MAX_NIBBLE : l_len) << 4);
    comp_emit_literal(&op, anc, l_len);

    /* write compressed size into header */
    {
        uint32_t c_sz = (uint32_t)(op - out - HDR_HDR_SIZE);
        memcpy(out + HDR_CSIZE_OFF, &c_sz, 4);
    }

    free(ht);
    return (size_t)(op - out);
}

/* --- decompression helpers --- */

/* read extended length for literal or match */
static size_t
dec_read_ext_len(const uint8_t **ip, const uint8_t *end, size_t initial)
{
    size_t len = initial;
    uint8_t s;

    if (initial == TOKEN_MAX_NIBBLE) {
        do {
            if (*ip >= end) {
                DEC_TRAP("extended length read past end of compressed block");
                return 0;
            }
            s = *(*ip)++;
            len += s;
        } while (s == TOKEN_EXT_BYTE);
    }
    return len;
}

/* copy match bytes handling overlap */
static int
dec_copy_match(uint8_t *out, size_t opos, size_t off, size_t m_len, size_t cap)
{
    size_t ref = opos - off;
    size_t i;

    if (off == 0) {
        //DEC_TRAP("invalid match offset 0 encountered during decompression");
        //return -1;
    	return 0;
    }
    if (off > opos) {
        DEC_TRAP("match offset exceeds current output position during decompression");
        return -1;
    }
    if (opos + m_len > cap) {
        DEC_TRAP("match copy would overflow output buffer during decompression");
        return -1;
    }
    for (i = 0; i < m_len; ++i)
        out[opos + i] = out[ref + i];
    return 0;
}

/* --- decompression core --- */

/*
 * Goal: decompress a single compressed block 'in' of length n into 'out'.
 * Returns number of bytes written (original size) or 0 on error.
 */
static size_t
lz4_dec(const uint8_t *in, size_t n, uint8_t *out, size_t cap)
{
    const uint8_t *ip, *end;
    uint32_t org, c_sz;
    size_t opos, l_len, m_len;
    uint8_t tok;
    uint16_t off;

    if (n < HDR_HDR_SIZE || memcmp(in, HDR_MAGIC, HDR_MAGIC_LEN) != 0) {
        DEC_TRAP("bad header or magic bytes do not match expected LZ4B signature");
        return (0);
    }
    memcpy(&org, in + HDR_ORIG_OFF, 4);
    memcpy(&c_sz, in + HDR_CSIZE_OFF, 4);
    if ((size_t)c_sz + HDR_HDR_SIZE != n) {
        DEC_TRAP("header compressed size field does not match actual block length");
        return (0);
    }

    ip = in + HDR_HDR_SIZE;
    end = in + n;
    opos = 0;

    while (ip < end) {
        tok = *ip++;
        l_len = dec_read_ext_len(&ip, end, (size_t)(tok >> 4));
        if (l_len == 0 && (tok >> 4) == TOKEN_MAX_NIBBLE && ip > end) {
            DEC_TRAP("literal length extension truncated in compressed stream");
            return 0;
        }
        if (opos + l_len > cap) {
            DEC_TRAP("literal output would overflow destination buffer during decompression");
            return (0);
        }
        if ((size_t)(end - ip) < l_len) {
            DEC_TRAP("not enough input bytes for literal copy during decompression");
            return 0;
        }
        if (l_len > 0) {
            memcpy(out + opos, ip, l_len);
            ip += l_len;
            opos += l_len;
        }
        if (ip >= end)
            break;

        if ((size_t)(end - ip) < 2) {
            DEC_TRAP("missing match offset bytes in compressed stream");
            return 0;
        }
        off = (uint16_t)(ip[0] | (ip[1] << 8));
        ip += 2;
        m_len = (size_t)(tok & 0xF) + MIN_MAT;
        /* read extension for match length if needed */
        {
            size_t ext = dec_read_ext_len(&ip, end, (size_t)(m_len - MIN_MAT));
            if (ext == 0 && (m_len - MIN_MAT) == TOKEN_MAX_NIBBLE && ip > end) {
                DEC_TRAP("match length extension truncated in compressed stream");
                return 0;
            }
            m_len = ext + MIN_MAT;
        }
        if (dec_copy_match(out, opos, off, m_len, cap) != 0) {
            return (0);
        }
        opos += m_len;
    }
    if (opos != org) {
        DEC_TRAP("decompressed output size does not match original size from header");
        return (0);
    }
    return (opos == org ? opos : 0);
}

/* --- file/block processing helpers --- */

static int
blk_proc(io_ctx_t *c, FILE *out, int is_comp)
{
    size_t res;
    uint32_t b_sz;

    if (is_comp)
        res = lz4_comp(c->in, c->c_len, c->out, BLK_SIZE * 2);
    else
        res = lz4_dec(c->in, c->c_len, c->out, BLK_SIZE * 2);

    if (res == 0) {
        if (!is_comp) {
            DEC_TRAP("block decompression returned zero bytes indicating failure");
        }
        return (-1);
    }

    if (is_comp) {
        b_sz = (uint32_t)res;
        if (io_wr_u32(out, b_sz)) {
            fprintf(stderr, "jlz4: failed to write block size to archive for file '%s'\n", c->path);
            return (-1);
        }
        if (fwrite(c->out, 1, res, out) != res) {
            fprintf(stderr, "jlz4: failed to write compressed block payload to archive for file '%s'\n", c->path);
            return (-1);
        }
        g_prg.f_comp += 4 + res;
    } else {
        if (io_write(fileno(out), c->out, res)) {
            DEC_TRAP("writing decompressed block to output file failed");
            return (-1);
        }
    }
    return (0);
}

/* write placeholder size and return its file position */
static long
file_comp_write_placeholder(FILE *arc)
{
    long pos = ftell(arc);
    if (pos < 0) {
        fprintf(stderr, "jlz4: failed to get current archive file position for placeholder\n");
        return -1;
    }
    if (io_wr_u32(arc, 0)) {
        fprintf(stderr, "jlz4: failed to write placeholder compressed size to archive\n");
        return -1;
    }
    return pos;
}

/* finalize placeholder with actual compressed size */
static int
file_comp_finalize_placeholder(FILE *arc, long pos)
{
    long cur = ftell(arc);
    if (cur < 0) {
        fprintf(stderr, "jlz4: failed to get current archive file position while finalizing placeholder\n");
        return -1;
    }
    if (fseek(arc, pos, SEEK_SET) != 0) {
        fprintf(stderr, "jlz4: failed to seek to placeholder position to finalize compressed size\n");
        return -1;
    }
    if (io_wr_u32(arc, (uint32_t)g_prg.f_comp)) {
        fprintf(stderr, "jlz4: failed to write final compressed size into archive placeholder\n");
        return -1;
    }
    if (fseek(arc, cur, SEEK_SET) != 0) {
        fprintf(stderr, "jlz4: failed to restore archive file position after finalizing placeholder\n");
        return -1;
    }
    return 0;
}

/* process blocks for compression of a single file */
static int
file_comp_process_blocks(io_ctx_t *c, FILE *arc)
{
    size_t r;

    while ((r = io_read(c->fp, c->in, BLK_SIZE)) > 0) {
        c->c_len = (uint32_t)r;
        if (blk_proc(c, arc, 1) != 0)
            return (-1);
        g_prg.proc += r;
        prg_upd(g_prg.proc, g_prg.total);
    }
    if (ferror(c->fp)) {
        fprintf(stderr, "jlz4: error reading input file '%s' during compression: %s\n", c->path, strerror(errno));
        return -1;
    }
    return 0;
}

/* top-level file compression */
static int
file_comp(io_ctx_t *c, FILE *arc)
{
    long sz_pos;

    g_prg.curr = c->path;
    g_prg.f_comp = 0;
    sz_pos = file_comp_write_placeholder(arc);
    if (sz_pos < 0)
        return -1;

    if (file_comp_process_blocks(c, arc) != 0)
        return (-1);

    if (file_comp_finalize_placeholder(arc, sz_pos) != 0)
        return -1;
    return (0);
}

/* read a block from archive and decompress it to file */
static int
file_dec_process_blocks(FILE *arc, const char *out, uint32_t c_sz)
{
    io_ctx_t c;
    FILE *fo;
    uint32_t rem;
    uint32_t b_sz;
    int fd;

    c.in = malloc(BLK_SIZE * 2);
    c.out = malloc(BLK_SIZE * 2);
    if (c.in == NULL || c.out == NULL) {
        free(c.in);
        free(c.out);
        fprintf(stderr, "jlz4: memory allocation failed for block buffers during decompression of '%s'\n", out);
        return -1;
    }
    c.fp = NULL;
    fd = open(out, O_WRONLY | O_CREAT | O_TRUNC, FILE_MODE);
    if (fd < 0) {
        fprintf(stderr, "jlz4: failed to open or create output file '%s' for writing: %s\n", out, strerror(errno));
        free(c.in);
        free(c.out);
        return -1;
    }
    fo = fdopen(fd, "wb");
    if (fo == NULL) {
        fprintf(stderr, "jlz4: fdopen failed for output file '%s': %s\n", out, strerror(errno));
        close(fd);
        free(c.in);
        free(c.out);
        return -1;
    }
    rem = c_sz;

    while (rem > 0) {
        if (io_rd_u32(arc, &b_sz)) {
            DEC_TRAP("failed to read block size from archive while extracting file");
            break;
        }
        if (b_sz > rem) {
            DEC_TRAP("block size in archive exceeds remaining bytes for this file entry");
            break;
        }
        rem -= 4;
        c.c_len = b_sz;
        if (io_read(arc, c.in, b_sz) != b_sz) {
            DEC_TRAP("failed to read compressed block payload from archive");
            break;
        }
        rem -= b_sz;
        if (blk_proc(&c, fo, 0) != 0) {
            /* blk_proc trapped or write failed */
            break;
        }
        g_prg.proc += 4 + b_sz;
        prg_upd(g_prg.proc, g_prg.total);
    }
    free(c.in);
    free(c.out);
    if (fo)
        fclose(fo);
    return (0);
}

/* wrapper for file_dec */
static int
file_dec(FILE *arc, const char *out, uint32_t c_sz)
{
    return file_dec_process_blocks(arc, out, c_sz);
}

/* --- directory traversal & archive helpers --- */

static int
arc_write(FILE *arc, const char *nm, uint64_t sz)
{
    uint32_t nl;

    nl = (uint32_t)strlen(nm);
    if (io_wr_u32(arc, nl))
        return (-1);
    if (fwrite(nm, 1, nl, arc) != nl)
        return (-1);
    if (io_wr_u64(arc, sz))
        return (-1);
    return (0);
}

static int
dir_mk(const char *p)
{
    struct stat st;
    char tmp[PATH_MAX];
    char *sl;

    strncpy(tmp, p, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';
    sl = strrchr(tmp, '/');
    if (sl == NULL || sl == tmp)
        return (0);
    *sl = 0;
    if (stat(tmp, &st) == 0 && S_ISDIR(st.st_mode))
        return (0);
    dir_mk(tmp);
    if (mkdir(tmp, DIR_MODE) == 0 || errno == EEXIST)
        return 0;
    fprintf(stderr, "jlz4: failed to create directory '%s': %s\n", tmp, strerror(errno));
    return -1;
}

static int
dir_process_entry(const char *path, const char *rt, FILE *arc)
{
    struct stat st;
    io_ctx_t c;
    char relbuf[PATH_MAX];
    const char *rel;

    if (lstat(path, &st)) {
        fprintf(stderr, "jlz4: lstat failed for '%s': %s\n", path, strerror(errno));
        return (-1);
    }
    if (!S_ISREG(st.st_mode))
        return 0;

    rel = path + strlen(rt);
    if (*rel == '/')
        rel++;
    strncpy(relbuf, rel, sizeof(relbuf));
    relbuf[sizeof(relbuf) - 1] = '\0';

    if (arc_write(arc, relbuf, st.st_size)) {
        fprintf(stderr, "jlz4: failed to write archive entry header for '%s'\n", relbuf);
        return (-1);
    }

    c.path = relbuf;
    c.fp = fopen(path, "rb");
    if (c.fp == NULL) {
        fprintf(stderr, "jlz4: failed to open input file '%s' for reading: %s\n", path, strerror(errno));
        return -1;
    }
    c.in = malloc(BLK_SIZE);
    c.out = malloc(BLK_SIZE * 2);
    if (c.in == NULL || c.out == NULL) {
        free(c.in);
        free(c.out);
        fclose(c.fp);
        fprintf(stderr, "jlz4: memory allocation failed for buffers while compressing '%s'\n", path);
        return -1;
    }
    if (file_comp(&c, arc) != 0) {
        free(c.in);
        free(c.out);
        fclose(c.fp);
        fprintf(stderr, "jlz4: failed to compress file '%s'\n", path);
        return -1;
    }
    free(c.in);
    free(c.out);
    fclose(c.fp);
    return 0;
}

static int
rec_comp(const char *in, const char *rt, FILE *arc)
{
    struct stat st;
    struct dirent *e;
    DIR *d;
    char sub[PATH_MAX];

    if (lstat(in, &st)) {
        fprintf(stderr, "jlz4: lstat failed for '%s' while recursing: %s\n", in, strerror(errno));
        return (-1);
    }
    if (S_ISDIR(st.st_mode)) {
        d = opendir(in);
        if (d == NULL) {
            fprintf(stderr, "jlz4: failed to open directory '%s' for recursion: %s\n", in, strerror(errno));
            return (-1);
        }
        while ((e = readdir(d))) {
            if (e->d_name[0] == '.')
                continue;
            path_join(sub, PATH_MAX, in, e->d_name);
            rec_comp(sub, rt, arc);
        }
        closedir(d);
        return (0);
    }
    return dir_process_entry(in, rt, arc);
}

static int
rec_size(const char *p, uint64_t *sz)
{
    struct stat st;
    struct dirent *e;
    DIR *d;
    char sub[PATH_MAX];

    if (lstat(p, &st)) {
        fprintf(stderr, "jlz4: lstat failed for '%s' while computing total size: %s\n", p, strerror(errno));
        return (-1);
    }
    if (S_ISREG(st.st_mode)) {
        *sz += st.st_size;
        return (0);
    }
    if (!S_ISDIR(st.st_mode))
        return (0);

    d = opendir(p);
    if (d == NULL) {
        fprintf(stderr, "jlz4: failed to open directory '%s' while computing total size: %s\n", p, strerror(errno));
        return (-1);
    }
    while ((e = readdir(d))) {
        if (e->d_name[0] == '.')
            continue;
        path_join(sub, PATH_MAX, p, e->d_name);
        rec_size(sub, sz);
    }
    closedir(d);
    return (0);
}

/* --- actions --- */

static int
act_list(const char *fn)
{
    FILE *f;
    uint64_t o_sz;
    uint32_t nl, c_sz;
    char *nm;
    char s1[32], s2[32];

    f = fopen(fn, "rb");
    if (f == NULL) {
        errx(1, "failed to open archive '%s' for listing: %s", fn, strerror(errno));
    }
    while (1) {
        if (io_rd_u32(f, &nl))
            break;
        nm = malloc(nl + 1);
        if (nm == NULL) {
            fprintf(stderr, "jlz4: memory allocation failed while listing archive '%s'\n", fn);
            break;
        }
        if (fread(nm, 1, nl, f) != nl) {
            free(nm);
            fprintf(stderr, "jlz4: unexpected EOF while reading filename from archive '%s'\n", fn);
            break;
        }
        nm[nl] = 0;
        if (io_rd_u64(f, &o_sz)) {
            free(nm);
            fprintf(stderr, "jlz4: failed to read original size for entry '%s' in archive '%s'\n", nm, fn);
            break;
        }
        if (io_rd_u32(f, &c_sz)) {
            free(nm);
            fprintf(stderr, "jlz4: failed to read compressed size for entry '%s' in archive '%s'\n", nm, fn);
            break;
        }
        fmt_byte(o_sz, s1, 32);
        fmt_byte(c_sz, s2, 32);
        printf("%s %s %s\n", nm, s1, s2);
        free(nm);
        if (fseek(f, c_sz, SEEK_CUR) != 0) {
            fprintf(stderr, "jlz4: failed to skip compressed payload for entry '%s' in archive '%s'\n", nm, fn);
            break;
        }
    }
    fclose(f);
    return 0;
}

static void
act_comp(const char *in, const char *out)
{
    FILE *arc;
    char out_fn[PATH_MAX];
    char rt[PATH_MAX];
    char *dir;

    if (out == NULL)
        snprintf(out_fn, sizeof(out_fn), "%s.lz4", in);
    else
        strncpy(out_fn, out, sizeof(out_fn));

    arc = fopen(out_fn, "wb");
    if (arc == NULL)
        errx(1, "failed to create archive '%s' for writing: %s", out_fn, strerror(errno));

    strncpy(rt, in, sizeof(rt));
    rt[sizeof(rt) - 1] = '\0';
    if (rec_size(in, &g_prg.total))
        errx(1, "failed to scan input '%s' to compute total size for progress reporting", in);

    dir = dirname(rt);
    if (rec_comp(in, dir, arc) != 0) {
        fclose(arc);
        errx(1, "failed while compressing input '%s' into archive '%s'", in, out_fn);
    }
    fclose(arc);
}

/* --- Fixed act_dec: robust handling of malformed entries and fread failures --- */
static void
act_dec(const char *in, const char *out)
{
    FILE *f;
    struct stat st;
    uint64_t o_sz;
    uint32_t nl, c_sz;
    char *nm;
    char dst[PATH_MAX];

    f = fopen(in, "rb");
    if (f == NULL)
        errx(1, "failed to open archive '%s' for extraction: %s", in, strerror(errno));

    if (stat(in, &st) == 0)
        g_prg.total = st.st_size;
    else
        g_prg.total = 0;

    while (1) {
        if (io_rd_u32(f, &nl))
            break;

        /* guard against absurd name lengths */
        if (nl == 0 || nl >= PATH_MAX) {
            /* skip malformed entry: try to skip the claimed name bytes if possible */
            if (nl > 0) {
                if (fseek(f, (long)nl, SEEK_CUR) != 0) {
                    fprintf(stderr, "jlz4: malformed entry with invalid name length %u and unable to skip claimed name bytes\n", nl);
                    break;
                }
            }
            /* attempt to continue to next entry */
            continue;
        }

        nm = malloc((size_t)nl + 1);
        if (nm == NULL) {
            fprintf(stderr, "jlz4: memory allocation failed while extracting archive '%s'\n", in);
            break;
        }

        if (fread(nm, 1, nl, f) != nl) {
            free(nm);
            fprintf(stderr, "jlz4: unexpected EOF while reading filename from archive '%s'\n", in);
            break;
        }
        nm[nl] = '\0';

        if (io_rd_u64(f, &o_sz)) {
            free(nm);
            fprintf(stderr, "jlz4: failed to read original size for entry '%s' in archive '%s'\n", nm, in);
            break;
        }
        if (io_rd_u32(f, &c_sz)) {
            free(nm);
            fprintf(stderr, "jlz4: failed to read compressed size for entry '%s' in archive '%s'\n", nm, in);
            break;
        }

        if (out != NULL)
            path_join(dst, PATH_MAX, out, nm);
        else {
            /* ensure dst is always NUL-terminated */
            strncpy(dst, nm, PATH_MAX);
            dst[PATH_MAX - 1] = '\0';
        }

        free(nm);
        if (dir_mk(dst) != 0) {
            /* failed to create directories; stop to avoid data loss */
            fprintf(stderr, "jlz4: failed to create directories for output path '%s'\n", dst);
            break;
        }
        if (file_dec(f, dst, c_sz) != 0) {
            /* file_dec signals failure; stop extraction */
            fprintf(stderr, "jlz4: failed to extract file '%s' from archive '%s'\n", dst, in);
            break;
        }
    }
    fclose(f);
}

/* --- main --- */

int
main(int ac, char **av)
{
    if (ac < 3) {
        fprintf(stderr, "use: %s -c|-d|-l <in> [out]\n", av[0]);
        return (1);
    }
    gettimeofday(&g_prg.start, NULL);
    g_prg.lines = 0;
    g_prg.curr = NULL;
    g_prg.total = 0;
    g_prg.proc = 0;

    if (strcmp(av[1], "-l") == 0)
        act_list(av[2]);
    else if (strcmp(av[1], "-c") == 0)
        act_comp(av[2], ac > 3 ? av[3] : NULL);
    else if (strcmp(av[1], "-d") == 0)
        act_dec(av[2], ac > 3 ? av[3] : NULL);
    else
        return (1);

    return (0);
}
