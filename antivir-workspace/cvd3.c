// cvd_interpret_all.c
// Parse ClamAV main.cvd: locate gzip, inflate, open tar, interpret signatures by virus name,
// print sections with headers and a final summary.
// Build: cc -O2 -Wall -o cvd_interpret_all cvd_interpret_all.c -lz -larchive
// Run:   ./cvd_interpret_all | less -R

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <zlib.h>
#include <archive.h>
#include <archive_entry.h>
#include <ctype.h>

typedef struct {
    unsigned long long files;
    unsigned long long text_files;
    unsigned long long binary_files;
    unsigned long long hdb_sigs;
    unsigned long long ndb_sigs;
    unsigned long long ldb_rules;
    unsigned long long other_lines;
} Summary;

static long find_header_end(const unsigned char *d, size_t n) {
    for (size_t i=0;i<n;i++) if (d[i]=='\n') return (long)(i+1);
    return -1;
}
static long find_magic(const unsigned char *d, size_t n, const unsigned char *m, size_t mlen, size_t start) {
    if (start >= n) return -1;
    for (size_t i = start; i + mlen <= n; ++i) if (memcmp(d+i, m, mlen) == 0) return (long)i;
    return -1;
}
static unsigned char *inflate_gzip(const unsigned char *src, size_t src_len, size_t off, size_t *out_len, int *zerr) {
    z_stream strm; memset(&strm, 0, sizeof(strm));
    int ret = inflateInit2(&strm, 16 + MAX_WBITS);
    if (ret != Z_OK) { if (zerr) *zerr = ret; return NULL; }
    strm.next_in = (Bytef *)(src + off);
    strm.avail_in = (uInt)(src_len - off);
    size_t cap = 1<<20;
    unsigned char *out = malloc(cap);
    if (!out) { inflateEnd(&strm); if (zerr) *zerr = Z_MEM_ERROR; return NULL; }
    size_t total = 0;
    do {
        if (total + 65536 > cap) {
            size_t ncap = cap * 2;
            unsigned char *p = realloc(out, ncap);
            if (!p) { free(out); inflateEnd(&strm); if (zerr) *zerr = Z_MEM_ERROR; return NULL; }
            out = p; cap = ncap;
        }
        strm.next_out = out + total;
        strm.avail_out = (uInt)(cap - total);
        ret = inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
            free(out); inflateEnd(&strm); if (zerr) *zerr = ret; return NULL;
        }
        total = cap - strm.avail_out;
    } while (ret != Z_STREAM_END);
    inflateEnd(&strm);
    if (out_len) *out_len = total;
    if (zerr) *zerr = Z_OK;
    return out;
}
static int likely_text(const unsigned char *buf, size_t n) {
    if (n==0) return 1;
    size_t bad = 0;
    for (size_t i=0;i<n;i++) {
        unsigned char c = buf[i];
        if (c==9||c==10||c==13) continue;
        if (c>=32 && c<=126) continue;
        if (c >= 0xC2 && c <= 0xF4) continue;
        if (c==0) { bad+=4; continue; }
        bad++;
    }
    return bad*10 <= n;
}

/* Helpers to print section headers */
static void print_section_header(const char *title) {
    printf("\n=== %s ===\n", title ? title : "(unnamed)");
}

/* Interpret .hdb line: md5:size:label */
static void interpret_hdb_line(const char *line, Summary *sum) {
    const char *p = strchr(line, ':');
    if (!p) { return; }
    const char *q = p + 1;
    const char *r = strchr(q, ':');
    if (!r) { return; }
    size_t lablen = strlen(r+1);
    if (lablen == 0) return;
    sum->hdb_sigs++;
    print_section_header(r+1); /* label as header */
    printf("type: hdb\n");
    printf("hash: ");
    fwrite(line, 1, (size_t)(p - line), stdout);
    printf("\nsize: %.*s\n", (int)(r - q), q);
}

/* Interpret .ndb line: Name:Targets:Offset:Pattern... (best effort) */
static void interpret_ndb_line(const char *line, Summary *sum) {
    char buf[4096]; strncpy(buf, line, sizeof(buf)-1); buf[sizeof(buf)-1] = 0;
    char *saveptr = NULL;
    char *tok = strtok_r(buf, ":", &saveptr);
    const char *name = tok; tok = strtok_r(NULL, ":", &saveptr);
    const char *target = tok; tok = strtok_r(NULL, ":", &saveptr);
    const char *ofs = tok; tok = strtok_r(NULL, ":", &saveptr);
    const char *pattern = tok;
    if (!name) return;
    sum->ndb_sigs++;
    print_section_header(name);
    printf("type: ndb\n");
    if (target) printf("target: %s\n", target);
    if (ofs)    printf("offset: %s\n", ofs);
    if (pattern) {
        printf("pattern: ");
        for (int i=0; i<64 && pattern[i]; i++) putchar(pattern[i]);
        if ((int)strlen(pattern) > 64) printf("...");
        printf("\n");
    }
}

/* Interpret .ldb block: rule name before first ';', count subsigs, show a logic sample */
static void interpret_ldb_block(const unsigned char *buf, size_t n, Summary *sum) {
    sum->ldb_rules++;
    /* Find first line */
    size_t pos = 0;
    size_t start = 0;
    while (pos < n && buf[pos] != '\n') pos++;
    size_t len = pos - start;
    char first[1024]; size_t copy = len < sizeof(first)-1 ? len : sizeof(first)-1;
    memcpy(first, buf + start, copy); first[copy] = 0;
    /* Rule name up to first ';' */
    char *semi = strchr(first, ';');
    const char *rulename = first;
    if (semi) *semi = 0;
    if (!rulename || !*rulename) rulename = "(ldb rule)";
    print_section_header(rulename);
    printf("type: ldb\n");
    /* Count "subsig=" occurrences */
    int subs = 0;
    const char *needle = "subsig=";
    const unsigned char *p = buf;
    while (p <= buf + n - strlen(needle)) {
        if (memcmp(p, needle, strlen(needle)) == 0) { subs++; p += strlen(needle); }
        else p++;
    }
    printf("subsig_count: %d\n", subs);
    /* Show up to one line that looks like logic=... */
    const unsigned char *scan = buf;
    while (scan < buf + n) {
        const unsigned char *ln = memchr(scan, '\n', (size_t)(buf + n - scan));
        size_t l = ln ? (size_t)(ln - scan) : (size_t)(buf + n - scan);
        if (l > 6 && memmem(scan, l, "logic=", 6)) {
            printf("logic: ");
            fwrite(scan, 1, l, stdout);
            printf("\n");
            break;
        }
        if (!ln) break;
        scan = ln + 1;
    }
}

/* Print short samples for other text entries */
static void print_text_samples(const unsigned char *buf, size_t n, size_t max_lines, Summary *sum) {
    size_t pos = 0, printed = 0;
    while (pos < n && printed < max_lines) {
        size_t start = pos;
        while (pos < n && buf[pos] != '\n') pos++;
        size_t len = pos - start;
        if (len) {
            sum->other_lines++;
            /* Title = first token of line (up to ':' or whitespace) */
            char title[256]; size_t tlen = 0;
            for (size_t i=0; i<len && i<sizeof(title)-1; i++) {
                if (buf[start+i] == ':' || isspace(buf[start+i])) break;
                title[tlen++] = buf[start+i];
            }
            title[tlen] = 0;
            if (tlen) print_section_header(title);
            fwrite(buf + start, 1, len, stdout);
            printf("\n");
            printed++;
        }
        if (pos < n && buf[pos] == '\n') pos++;
    }
}

int main(void) {
    Summary sum = {0};
    const char *in = "main.cvd";
    FILE *f = fopen(in, "rb");
    if (!f) { fprintf(stderr, "cannot open %s: %s\n", in, strerror(errno)); return 1; }
    if (fseek(f, 0, SEEK_END) != 0) { fprintf(stderr, "seek failed\n"); fclose(f); return 1; }
    long fsz = ftell(f);
    if (fsz < 0) { fprintf(stderr, "ftell failed\n"); fclose(f); return 1; }
    if (fseek(f, 0, SEEK_SET) != 0) { fprintf(stderr, "seek failed\n"); fclose(f); return 1; }
    unsigned char *filebuf = malloc((size_t)fsz);
    if (!filebuf) { fprintf(stderr, "OOM\n"); fclose(f); return 1; }
    size_t got = fread(filebuf,1,(size_t)fsz,f);
    fclose(f);
    if (got != (size_t)fsz) { fprintf(stderr, "short read\n"); free(filebuf); return 1; }

    long header_end = find_header_end(filebuf, (size_t)fsz);
    if (header_end < 0) { fprintf(stderr, "no header newline\n"); free(filebuf); return 1; }
    size_t hlen = (size_t)header_end;
    if (hlen>0 && filebuf[hlen-1]=='\n') hlen--;
    if (hlen>0 && filebuf[hlen-1]=='\r') hlen--;
    printf("CVD header:\n");
    fwrite(filebuf, 1, hlen, stdout);
    printf("\n");

    /* Scan for all gzip candidates and try inflating until one works */
    const unsigned char gz_magic[2] = {0x1f,0x8b};
    long start = header_end;
    long gzpos = -1;
    size_t dec_len = 0;
    unsigned char *dec = NULL;
    int zerr = Z_OK;
    for (;;) {
        long found = find_magic(filebuf, (size_t)fsz, gz_magic, 2, (size_t)start);
        if (found < 0) break;
        gzpos = found;
        dec = inflate_gzip(filebuf, (size_t)fsz, (size_t)gzpos, &dec_len, &zerr);
        if (dec) break;
        /* try next candidate */
        start = gzpos + 2;
    }
    if (!dec) {
        fprintf(stderr, "Failed to inflate any gzip member (last zlib err=%d). The CVD payload may be encrypted or non-tar.\n", zerr);
        free(filebuf);
        return 1;
    }
    fprintf(stderr, "Inflated gzip member at offset %ld -> %zu bytes\n\n", gzpos, dec_len);

    /* Open decompressed bytes with libarchive (tar) */
    struct archive *a = archive_read_new();
    if (!a) { fprintf(stderr, "archive_read_new failed\n"); free(dec); free(filebuf); return 1; }
    archive_read_support_filter_none(a);
    archive_read_support_format_all(a);
    if (archive_read_open_memory(a, dec, dec_len) != ARCHIVE_OK) {
        fprintf(stderr, "libarchive open failed: %s\n", archive_error_string(a));
        archive_read_free(a);
        free(dec); free(filebuf);
        return 1;
    }

    /* Iterate entries and interpret */
    struct archive_entry *entry;
    const size_t CHUNK = 128*1024;
    unsigned char *work = malloc(CHUNK);
    if (!work) { fprintf(stderr, "OOM\n"); archive_read_free(a); free(dec); free(filebuf); return 1; }

    printf("\n--- Interpreted signatures ---\n");
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        sum.files++;
        const char *name = archive_entry_pathname(entry);
        long long size = archive_entry_size(entry);
        printf("\n# File: %s (%lld bytes)\n", name ? name : "(no-name)", size);

        /* Read entry into buffer (cap first N bytes for speed) */
        size_t read_total = 0;
        size_t cap = 0;
        unsigned char *ent = NULL;
        la_ssize_t r;
        while ((r = archive_read_data(a, work, CHUNK)) > 0) {
            if (read_total + (size_t)r + 1 > cap) {
                size_t ncap = cap ? cap*2 : (size_t)r + 1;
                while (ncap < read_total + (size_t)r + 1) ncap *= 2;
                unsigned char *p = realloc(ent, ncap);
                if (!p) { free(ent); ent = NULL; break; }
                ent = p; cap = ncap;
            }
            if (ent) memcpy(ent + read_total, work, (size_t)r);
            read_total += (size_t)r;
            if (read_total >= (size_t)size && size >= 0) break;
        }
        if (ent) ent[read_total] = 0;

        int text = ent ? likely_text(ent, read_total) : 0;
        if (text) sum.text_files++; else sum.binary_files++;
        printf("type: %s\n", text ? "text" : "binary");

        if (text && name && strstr(name, ".hdb")) {
            /* parse each line briefly */
            char *saveptr = NULL;
            char *line = strtok_r((char*)ent, "\n", &saveptr);
            while (line) {
                if (*line) interpret_hdb_line(line, &sum);
                line = strtok_r(NULL, "\n", &saveptr);
                if (sum.hdb_sigs % 50 == 0) fflush(stdout); /* throttle */
            }
        } else if (text && name && strstr(name, ".ndb")) {
            char *saveptr = NULL;
            char *line = strtok_r((char*)ent, "\n", &saveptr);
            while (line) {
                if (*line) interpret_ndb_line(line, &sum);
                line = strtok_r(NULL, "\n", &saveptr);
                if (sum.ndb_sigs % 50 == 0) fflush(stdout);
            }
        } else if (text && name && strstr(name, ".ldb")) {
            interpret_ldb_block(ent, read_total, &sum);
        } else if (text) {
            /* Other text entries: show a couple samples with title */
            print_text_samples(ent, read_total, 2, &sum);
        } else {
            printf("note: binary content preserved (e.g., bytecode)\n");
        }

        if (ent) free(ent);
    }

    /* Summary */
    printf("\n=== Summary ===\n");
    printf("files: %llu (text=%llu, binary=%llu)\n", sum.files, sum.text_files, sum.binary_files);
    printf("hdb signatures: %llu\n", sum.hdb_sigs);
    printf("ndb signatures: %llu\n", sum.ndb_sigs);
    printf("ldb rules: %llu\n", sum.ldb_rules);
    printf("other text lines: %llu\n", sum.other_lines);

    free(work);
    archive_read_free(a);
    free(dec);
    free(filebuf);
    return 0;
}


