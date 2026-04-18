#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <zlib.h>
#include <archive.h>
#include <archive_entry.h>
#include <ctype.h>

/* Portable fallback for memmem if missing */
#ifndef HAVE_MEMMEM
void *portable_memmem(const void *haystack, size_t haystacklen, const void *needle, size_t needlelen) {
    if (!needlelen) return (void *)haystack;
    if (haystacklen < needlelen) return NULL;
    const unsigned char *h = (const unsigned char *)haystack;
    const unsigned char *n = (const unsigned char *)needle;
    for (size_t i = 0; i + needlelen <= haystacklen; ++i) {
        if (h[i] == n[0] && memcmp(h + i, n, needlelen) == 0) return (void *)(h + i);
    }
    return NULL;
}
#define memmem(a,b,c,d) portable_memmem((a),(b),(c),(d))
#endif

static long find_header_end(const unsigned char *d, size_t n) {
    for (size_t i=0;i<n;i++) if (d[i]=='\n') return (long)(i+1);
    return -1;
}
static long find_magic(const unsigned char *d, size_t n, const unsigned char *m, size_t mlen, size_t start) {
    if (start >= n) return -1;
    for (size_t i = start; i + mlen <= n; ++i) if (memcmp(d+i, m, mlen) == 0) return (long)i;
    return -1;
}

static unsigned char *inflate_gzip(const unsigned char *src, size_t src_len, size_t off, size_t *out_len) {
    z_stream strm;
    memset(&strm, 0, sizeof(strm));
    if (inflateInit2(&strm, 16 + MAX_WBITS) != Z_OK) return NULL;
    strm.next_in = (Bytef *)(src + off);
    strm.avail_in = (uInt)(src_len - off);
    size_t cap = 1<<20;
    unsigned char *out = malloc(cap);
    if (!out) { inflateEnd(&strm); return NULL; }
    size_t total = 0;
    int ret;
    do {
        if (total + 65536 > cap) {
            size_t ncap = cap * 2;
            unsigned char *p = realloc(out, ncap);
            if (!p) { free(out); inflateEnd(&strm); return NULL; }
            out = p; cap = ncap;
        }
        strm.next_out = out + total;
        strm.avail_out = (uInt)(cap - total);
        ret = inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) { free(out); inflateEnd(&strm); return NULL; }
        total = cap - strm.avail_out;
    } while (ret != Z_STREAM_END);
    inflateEnd(&strm);
    *out_len = total;
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

static void print_sample_lines(const unsigned char *buf, size_t n, size_t max_lines) {
    size_t pos = 0, printed = 0;
    while (pos < n && printed < max_lines) {
        size_t start = pos;
        while (pos < n && buf[pos] != '\n') pos++;
        size_t len = pos - start;
        fwrite(buf + start, 1, len, stdout);
        printf("\n");
        printed++;
        if (pos < n && buf[pos] == '\n') pos++;
    }
    if (pos < n) printf("  ...\n");
}

static void parse_hdb_line(const char *line) {
    const char *p = strchr(line, ':');
    if (!p) { printf("    hdb: %s\n", line); return; }
    size_t hlen = p - line;
    printf("    hash: ");
    for (size_t i=0;i< (hlen>16?16:hlen); i++) putchar(line[i]);
    if (hlen > 16) printf("...");
    printf("\n");
    const char *q = p + 1;
    const char *r = strchr(q, ':');
    if (r) {
        printf("    size: %.*s\n", (int)(r - q), q);
        printf("    label: %s\n", r + 1);
    } else {
        printf("    rest: %s\n", q);
    }
}

static void parse_ndb_line(const char *line) {
    char tmp[2048];
    strncpy(tmp, line, sizeof(tmp)-1); tmp[sizeof(tmp)-1]=0;
    char *saveptr = NULL;
    char *tok = strtok_r(tmp, ":", &saveptr);
    int i = 0;
    printf("    ndb:");
    while (tok && i < 4) {
        if (i==0) printf(" name=%s;", tok);
        else if (i==1) printf(" target=%s;", tok);
        else if (i==2) printf(" ofs=%s;", tok);
        else if (i==3) {
            printf(" pattern=");
            for (int j=0; j<40 && tok[j]; j++) putchar(tok[j]);
            if (strlen(tok) > 40) printf("...");
            printf(";");
        }
        tok = strtok_r(NULL, ":", &saveptr);
        i++;
    }
    printf("\n");
}

/* count "subsig=" occurrences (simple) */
static int count_subsigs(const unsigned char *buf, size_t n) {
    int count = 0;
    const char *needle = "subsig=";
    const unsigned char *p = (const unsigned char *)buf;
    while (p <= (const unsigned char *)buf + n - strlen(needle)) {
        unsigned char *found = memmem((const void*)p, (size_t)((const unsigned char*)buf + n - p), needle, strlen(needle));
        if (!found) break;
        count++;
        p = found + 1;
    }
    return count;
}

int main(void) {
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
    printf("CVD header: ");
    fwrite(filebuf, 1, hlen, stdout);
    printf("\n\n");

    const unsigned char gz_magic[2] = {0x1f,0x8b};
    long gz = find_magic(filebuf, (size_t)fsz, gz_magic, 2, (size_t)header_end);
    if (gz < 0) gz = find_magic(filebuf, (size_t)fsz, gz_magic, 2, 0);
    if (gz < 0) { fprintf(stderr, "gzip magic not found\n"); free(filebuf); return 1; }
    fprintf(stderr, "Located gzip at offset %ld\n", gz);

    size_t dec_len = 0;
    unsigned char *dec = inflate_gzip(filebuf, (size_t)fsz, (size_t)gz, &dec_len);
    if (!dec) { fprintf(stderr, "inflate failed\n"); free(filebuf); return 1; }
    fprintf(stderr, "Decompressed %zu bytes\n\n", dec_len);

    struct archive *a = archive_read_new();
    if (!a) { fprintf(stderr, "archive_read_new failed\n"); free(dec); free(filebuf); return 1; }
    archive_read_support_filter_none(a);
    archive_read_support_format_all(a);
    if (archive_read_open_memory(a, dec, dec_len) != ARCHIVE_OK) {
        fprintf(stderr, "libarchive open failed: %s\n", archive_error_string(a));
        archive_read_free(a); free(dec); free(filebuf); return 1;
    }

    struct archive_entry *entry;
    const size_t CHUNK = 64*1024;
    unsigned char *work = malloc(CHUNK);
    if (!work) { fprintf(stderr, "OOM\n"); archive_read_free(a); free(dec); free(filebuf); return 1; }

    printf("Entries (concise):\n\n");
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        const char *name = archive_entry_pathname(entry);
        long long size = archive_entry_size(entry);
        printf("- %s  (%lld bytes)\n", name ? name : "(no-name)", size);

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
            if (read_total >= 65536) break;
        }
        if (ent) ent[read_total] = 0;
        int text = ent ? likely_text(ent, read_total) : 0;
        printf("  type: %s\n", text ? "text" : "binary");

        if (name && strstr(name, ".hdb") && text && read_total>0) {
            char *line = strtok((char*)ent, "\n");
            while (line && *line=='\0') line = strtok(NULL, "\n");
            if (line) {
                printf("  format: hdb\n");
                parse_hdb_line(line);
            }
        } else if (name && strstr(name, ".ndb") && text && read_total>0) {
            printf("  format: ndb\n");
            size_t printed = 0;
            char *saveptr = NULL;
            char *p = strtok_r((char*)ent, "\n", &saveptr);
            while (p && printed < 3) { if (*p) { parse_ndb_line(p); printed++; } p = strtok_r(NULL, "\n", &saveptr); }
            if (printed==0) printf("    (no sample lines)\n");
        } else if (name && strstr(name, ".ldb") && text && read_total>0) {
            int subs = count_subsigs(ent, read_total);
            printf("  format: ldb; subsig count ~%d\n", subs);
            print_sample_lines(ent, read_total, 2);
        } else if (name && (strstr(name,"bytecode") || strstr(name,"bytec"))) {
            printf("  format: bytecode (VM blob)\n");
        } else {
            if (text && read_total>0) {
                printf("  sample lines:\n"); print_sample_lines(ent, read_total, 2);
            } else {
                printf("  sample hexdump (first 32 bytes): ");
                size_t show = read_total < 32 ? read_total : 32;
                for (size_t i=0;i<show;i++) printf("%02x", ent[i]);
                if (read_total > show) printf("...");
                printf("\n");
            }
        }

        if (ent) free(ent);
        printf("\n");
    }

    free(work);
    archive_read_free(a);
    free(dec);
    free(filebuf);
    return 0;
}

