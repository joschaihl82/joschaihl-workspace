/*
 * az.c
 *
 * Simple archive + ASCII-friendly LZ77 prototype compressor.
 * - Fixes opendir on files vs directories
 * - Recursively packs directories
 * - Archive format: sequence of entries:
 *     [uint32 filename_len][filename bytes][uint64 orig_size][uint64 comp_size][comp bytes]
 * - Decompress extracts files into output directory (creates directories as needed)
 *
 * Not production-grade: prototype for experimentation and debugging.
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

/* ---------------------------
   Simple helpers
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

static size_t read_file_to_buf(const char *path, uint8_t **out) {
    FILE *f = fopen(path, "rb");
    if (!f) { perror(path); return 0; }
    if (fseek(f, 0, SEEK_END) != 0) { perror("fseek"); fclose(f); return 0; }
    long sz = ftell(f);
    if (sz < 0) { perror("ftell"); fclose(f); return 0; }
    rewind(f);
    uint8_t *buf = xmalloc((size_t)sz + 1);
    size_t r = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    *out = buf;
    return r;
}

static int ensure_dir_exists(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) return 0;
        errno = ENOTDIR;
        return -1;
    }
    /* try to create recursively */
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

/* safe path join: dest must be freed by caller */
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

/* ---------------------------
   Simple LZ77-like compressor (prototype)
   - Fast hash table, minimal format
   - Good for ASCII text; not optimized for speed
   --------------------------- */

#define LZ_HASH_SIZE (1 << 16)
#define LZ_MIN_MATCH 4
#define LZ_MAX_MATCH 255 + LZ_MIN_MATCH

static inline uint32_t lz_hash(const uint8_t *p) {
    /* simple 3-byte rolling hash */
    return ((uint32_t)p[0] * 2654435761u ^ (uint32_t)p[1] * 97531u ^ (uint32_t)p[2]) & (LZ_HASH_SIZE - 1);
}

/* Encode format (stream of chunks):
   For each chunk:
     uint8 literal_len (0..255)
     literal bytes (literal_len)
     uint8 flag
       if flag == 0: no match, continue
       if flag == 1: uint16 offset (big-endian), uint8 match_len_minus_min
*/
static void lz77_encode(const uint8_t *in, size_t inlen, uint8_t **out, size_t *outlen) {
    uint32_t *table = xmalloc(sizeof(uint32_t) * LZ_HASH_SIZE);
    for (size_t i = 0; i < LZ_HASH_SIZE; ++i) table[i] = UINT32_MAX;

    size_t ip = 0;
    size_t cap = inlen + 1024;
    uint8_t *obuf = xmalloc(cap);
    size_t op = 0;

    while (ip < inlen) {
        /* try to find match at ip */
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
            /* emit zero literal len, then match */
            if (op + 1 + 2 + 1 >= cap) { cap = (cap + 4096) * 2; obuf = xrealloc(obuf, cap); }
            obuf[op++] = 0;
            /* write offset big-endian */
            obuf[op++] = (uint8_t)((best_off >> 8) & 0xFF);
            obuf[op++] = (uint8_t)(best_off & 0xFF);
            obuf[op++] = (uint8_t)(best_len - LZ_MIN_MATCH);
            ip += best_len;
        } else {
            /* emit a literal run up to 255 bytes or until a match appears */
            size_t lit_start = ip;
            size_t lit_len = 0;
            while (ip < inlen && lit_len < 255) {
                /* update hash table for this position */
                if (ip + 3 < inlen) {
                    uint32_t hh = lz_hash(in + ip);
                    table[hh] = (uint32_t)ip;
                }
                ip++; lit_len++;
                /* peek for match at next pos */
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
            obuf[op++] = 0; /* flag 0 = no match follows */
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
        if (ip >= inlen) break;
        uint8_t litlen = in[ip++];
        if (ip + litlen > inlen) { free(obuf); return -1; }
        if (op + litlen > cap) { cap = (op + litlen) * 2; obuf = xrealloc(obuf, cap); }
        memcpy(obuf + op, in + ip, litlen); ip += litlen; op += litlen;
        if (ip >= inlen) break;
        /* check next bytes: if next byte is 0 and litlen != 0, it's flag 0; if litlen==0 then the next bytes are offset+len */
        if (litlen != 0) {
            uint8_t flag = in[ip++];
            (void)flag; /* flag 0 expected */
            continue;
        } else {
            /* litlen == 0 => match encoded as offset(2) + len(1) */
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
   Archive packing/unpacking
   --------------------------- */

/* write uint32 big-endian */
static void write_u32(FILE *f, uint32_t v) {
    uint8_t b[4];
    b[0] = (v >> 24) & 0xFF;
    b[1] = (v >> 16) & 0xFF;
    b[2] = (v >> 8) & 0xFF;
    b[3] = v & 0xFF;
    fwrite(b, 1, 4, f);
}

/* write uint64 big-endian */
static void write_u64(FILE *f, uint64_t v) {
    uint8_t b[8];
    for (int i = 0; i < 8; ++i) b[7-i] = (v >> (i*8)) & 0xFF;
    fwrite(b, 1, 8, f);
}

/* read helpers */
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

/* pack a single file into archive stream */
static int pack_file_to_stream(FILE *outf, const char *filepath, const char *relpath) {
    uint8_t *buf = NULL;
    size_t inlen = read_file_to_buf(filepath, &buf);
    if (inlen == 0 && errno) { fprintf(stderr, "read failed: %s\n", filepath); return -1; }
    uint8_t *comp = NULL;
    size_t complen = 0;
    lz77_encode(buf, inlen, &comp, &complen);

    uint32_t namelen = (uint32_t)strlen(relpath);
    write_u32(outf, namelen);
    fwrite(relpath, 1, namelen, outf);
    write_u64(outf, (uint64_t)inlen);
    write_u64(outf, (uint64_t)complen);
    if (fwrite(comp, 1, complen, outf) != complen) { perror("fwrite"); free(buf); free(comp); return -1; }

    free(buf);
    free(comp);
    return 0;
}

/* Recursively pack directory. relroot is the path prefix to store inside archive (relative path) */
static int pack_path_recursive(FILE *outf, const char *path, const char *relroot) {
    struct stat st;
    if (stat(path, &st) != 0) {
        fprintf(stderr, "stat('%s') failed: %s\n", path, strerror(errno));
        return -1;
    }
    if (S_ISDIR(st.st_mode)) {
        DIR *d = opendir(path);
        if (!d) {
            fprintf(stderr, "opendir('%s') failed: %s\n", path, strerror(errno));
            return -1;
        }
        struct dirent *ent;
        while ((ent = readdir(d)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            char *child = join_path(path, ent->d_name);
            char *child_rel;
            if (relroot && relroot[0]) {
                child_rel = join_path(relroot, ent->d_name);
            } else {
                child_rel = strdup(ent->d_name);
            }
            int r = pack_path_recursive(outf, child, child_rel);
            free(child);
            free(child_rel);
            if (r != 0) { closedir(d); return r; }
        }
        closedir(d);
        return 0;
    } else if (S_ISREG(st.st_mode)) {
        /* pack file */
        const char *rel = relroot && relroot[0] ? relroot : path;
        return pack_file_to_stream(outf, path, rel);
    } else {
        /* skip other types */
        fprintf(stderr, "skipping non-regular file: %s\n", path);
        return 0;
    }
}

/* ---------------------------
   Unpack archive
   --------------------------- */

static int unpack_archive(const char *archive_path, const char *out_dir) {
    FILE *f = fopen(archive_path, "rb");
    if (!f) { perror(archive_path); return -1; }
    while (1) {
        uint32_t namelen;
        if (read_u32(f, &namelen) != 0) break; /* EOF */
        char *name = xmalloc(namelen + 1);
        if (fread(name, 1, namelen, f) != namelen) { free(name); fclose(f); return -1; }
        name[namelen] = 0;
        uint64_t orig_size, comp_size;
        if (read_u64(f, &orig_size) != 0) { free(name); fclose(f); return -1; }
        if (read_u64(f, &comp_size) != 0) { free(name); fclose(f); return -1; }
        uint8_t *comp = xmalloc((size_t)comp_size);
        if (fread(comp, 1, (size_t)comp_size, f) != comp_size) { free(name); free(comp); fclose(f); return -1; }

        /* build output path */
        char *outpath = join_path(out_dir, name);
        /* ensure parent directories exist */
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
            fprintf(stderr, "decompression failed for %s\n", name);
            free(name); free(comp); free(outpath);
            fclose(f);
            return -1;
        }
        FILE *of = fopen(outpath, "wb");
        if (!of) { perror(outpath); free(name); free(comp); free(dec); free(outpath); fclose(f); return -1; }
        if (fwrite(dec, 1, declen, of) != declen) { perror("fwrite"); fclose(of); free(name); free(comp); free(dec); free(outpath); fclose(f); return -1; }
        fclose(of);
        free(name); free(comp); free(dec); free(outpath);
    }
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

    if (compress) {
        FILE *outf = fopen(output, "wb");
        if (!outf) { perror(output); return 1; }
        /* determine if input is file or directory */
        struct stat st;
        if (stat(input, &st) != 0) { fprintf(stderr, "stat('%s') failed: %s\n", input, strerror(errno)); fclose(outf); return 1; }
        int r = 0;
        if (S_ISDIR(st.st_mode)) {
            /* pack directory recursively; store relative paths */
            r = pack_path_recursive(outf, input, "");
        } else if (S_ISREG(st.st_mode)) {
            /* pack single file; store basename as entry name */
            const char *base = strrchr(input, '/');
            const char *rel = base ? base + 1 : input;
            r = pack_file_to_stream(outf, input, rel);
        } else {
            fprintf(stderr, "input is neither file nor directory: %s\n", input);
            r = 1;
        }
        fclose(outf);
        return r;
    } else {
        /* decompress into output directory */
        if (ensure_dir_exists(output) != 0) {
            fprintf(stderr, "cannot create output directory '%s': %s\n", output, strerror(errno));
            return 1;
        }
        return unpack_archive(input, output);
    }
}
