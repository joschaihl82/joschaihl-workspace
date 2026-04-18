#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <zlib.h>
#include <errno.h>
#include <ctype.h>
#include <dirent.h>

#define HEADER_SIZE 512
static const char *CVD_FILE = "main.cvd";
static const char *OUTPUT_DIR = "extracted";
static const uid_t TARGET_UID = 1000;
static const gid_t TARGET_GID = 1000;

//--------------------------------------------------------------------------------------
// Simple ASCII heuristic (Nano‑AI version, safe & non‑interpreting)
//--------------------------------------------------------------------------------------
int nano_ai_is_ascii(const unsigned char *buf, size_t len) {
    size_t printable = 0;
    for (size_t i=0; i<len; i++) {
        unsigned char c = buf[i];
        if (c == 9 || c == 10 || c == 13) { printable++; continue; }
        if (c >= 32 && c <= 126) printable++;
    }
    return printable * 100 / len >= 85;  // ≥85% printable → treat as ASCII
}

//--------------------------------------------------------------------------------------
// Display hex + ASCII
//--------------------------------------------------------------------------------------
void html_hexdump(FILE *html, const unsigned char *buf, size_t len) {
    fprintf(html, "<pre>\n");
    size_t offset = 0;
    while (offset < len) {
        fprintf(html, "%08zx  ", offset);
        for (size_t i = 0; i < 16; i++) {
            if (offset + i < len) fprintf(html, "%02x ", buf[offset+i]);
            else fprintf(html, "   ");
            if (i == 7) fputc(' ', html);
        }
        fputc(' ', html);
        for (size_t i = 0; i < 16 && offset+i < len; i++) {
            unsigned char c = buf[offset+i];
            fputc((c >= 32 && c <= 126) ? c : '.', html);
        }
        fputc('\n', html);
        offset += 16;
    }
    fprintf(html, "</pre>\n");
}

//--------------------------------------------------------------------------------------
// TAR + GZIP extraction (unchanged from before)
//--------------------------------------------------------------------------------------
struct tar_header {
    char name[100]; char mode[8]; char uid[8]; char gid[8];
    char size[12]; char mtime[12]; char checksum[8]; char typeflag;
    char linkname[100]; char magic[6]; char version[2];
    char uname[32]; char gname[32]; char devmajor[8]; char devminor[8];
    char prefix[155]; char padding[12];
};

static size_t oct2int(const char *s, size_t n) {
    size_t v = 0;
    for (size_t i = 0; i < n && s[i]; i++)
        if (s[i] >= '0' && s[i] <= '7') v = (v << 3) + (s[i] - '0');
    return v;
}

unsigned char *gunzip_memory(const unsigned char *src, size_t src_len, size_t *out_len) {
    z_stream s; memset(&s, 0, sizeof(s));
    inflateInit2(&s, 15+32);
    size_t cap = src_len * 4;
    unsigned char *out = malloc(cap);
    if (!out) return NULL;
    s.next_in = (Bytef*)src;
    s.avail_in = src_len;
    size_t total = 0;
    for (;;) {
        if (total == cap) {
            cap *= 2;
            out = realloc(out, cap);
            if (!out) return NULL;
        }
        s.next_out = out + total;
        s.avail_out = cap - total;
        int ret = inflate(&s, Z_NO_FLUSH);
        total = cap - s.avail_out;
        if (ret == Z_STREAM_END) break;
        if (ret != Z_OK) { free(out); return NULL; }
    }
    inflateEnd(&s);
    *out_len = total;
    return out;
}

int extract_tar(const unsigned char *tar, size_t len, const char *outdir) {
    size_t offset = 0;
    while (offset + 512 <= len) {
        struct tar_header *h = (struct tar_header *)(tar+offset);
        int zero = 1;
        for (int i=0;i<512;i++) if (tar[offset+i] != 0) { zero = 0; break; }
        if (zero) break;

        char filename[512];
        snprintf(filename, sizeof(filename), "%s/%s", outdir, h->name);

        size_t fsize = oct2int(h->size, sizeof(h->size));
        if (h->typeflag == '5') {
            mkdir(filename, 0777);
        } else if (h->typeflag == '0' || h->typeflag == '\0') {
            char tmp[512];
            strcpy(tmp, filename);
            for (char *p = tmp + 1; *p; p++){
                if (*p=='/') {
                    *p=0;
                    mkdir(tmp, 0777);
                    *p='/';
                }
            }
            int fd = open(filename, O_CREAT|O_WRONLY|O_TRUNC, 0666);
            if (fd>=0) {
                write(fd, tar+offset+512, fsize);
                close(fd);
            }
        }
        size_t blocks = (fsize+511)/512;
        offset += 512 + blocks*512;
    }
    return 0;
}

//--------------------------------------------------------------------------------------
// HTML escaping
//--------------------------------------------------------------------------------------
void html_escape(FILE *f, const char *s) {
    for (; *s; s++) {
        if (*s=='<') fputs("&lt;",f);
        else if (*s=='>') fputs("&gt;",f);
        else if (*s=='&') fputs("&amp;",f);
        else fputc(*s,f);
    }
}

//--------------------------------------------------------------------------------------
// classify a single signature line
//--------------------------------------------------------------------------------------
const char *detect_type(const char *line) {

    // MD5 (32 hex characters)
    if (strlen(line) >= 32 && strspn(line, "0123456789abcdefABCDEF") >= 32)
        return "MD5 Hash";

    // SHA1 (40 hex chars)
    if (strlen(line) >= 40 && strspn(line, "0123456789abcdefABCDEF") >= 40)
        return "SHA1 Hash";

    // SHA256 (64 hex chars)
    if (strlen(line) >= 64 && strspn(line, "0123456789abcdefABCDEF") >= 64)
        return "SHA256 Hash";

    // Regex indicator in ClamAV ndb
    if (strstr(line, "/") && strstr(line, "$"))
        return "Regex Pattern";

    // Hex‑encoded byte pattern in NDB:
    if (strstr(line, ":") && strstr(line, "??"))
        return "Byte Sequence";

    // Default fallback
    return "Unknown Pattern";
}

//--------------------------------------------------------------------------------------
// render pattern row
//--------------------------------------------------------------------------------------
void html_add_pattern(FILE *html,
                      unsigned long long virus_index,
                      unsigned long long pattern_index,
                      const char *type,
                      const char *content)
{
    fprintf(html, "<h2>%llu.%llu — %s</h2>\n",
            virus_index, pattern_index, type);

    fprintf(html, "<p>");
    html_escape(html, content);
    fprintf(html, "</p>\n");
}

//--------------------------------------------------------------------------------------
// Add binary (ldb) content — ASCII or hex+ASCII
//--------------------------------------------------------------------------------------
void html_add_ldb(FILE *html,
                  unsigned long long virus_index,
                  const unsigned char *buf,
                  size_t len)
{
    fprintf(html, "<h2>%llu.1 — Logical / Bytecode Signature</h2>\n", virus_index);

    if (nano_ai_is_ascii(buf, len)) {
        fprintf(html, "<p>ASCII Data:</p>\n<pre>\n");
        fwrite(buf, 1, len, html);
        fprintf(html, "</pre>\n");
        return;
    }

    fprintf(html, "<p>Binary content (Hexdump + ASCII):</p>\n");
    html_hexdump(html, buf, len);
}



//--------------------------------------------------------------------------------------
// PROCESS FILE (hdb, ndb, ldb, cbc, anything)
//--------------------------------------------------------------------------------------
void process_file(FILE *html,
                  const char *filename,
                  const unsigned char *buf,
                  size_t len,
                  unsigned long long virus_index)
{
    unsigned long long pattern_index = 1;

    //-------------- HDB / NDB TEXT FILES --------------------------------------
    if (strstr(filename, ".hdb") || strstr(filename, ".ndb")) {

        char *copy = malloc(len + 1);
        memcpy(copy, buf, len);
        copy[len] = 0;

        char *saveptr = NULL;
        char *line = strtok_r(copy, "\n", &saveptr);

        while (line) {
            if (*line) {
                const char *type = detect_type(line);
                html_add_pattern(html, virus_index, pattern_index++, type, line);
            }
            line = strtok_r(NULL, "\n", &saveptr);
        }

        free(copy);
        return;
    }

    //-------------- LDB / CBC BINARY SIGNATURE -------------------------------
    if (strstr(filename, ".ldb") || strstr(filename, ".cbc")) {
        html_add_ldb(html, virus_index, buf, len);
        return;
    }

    //-------------- Unknown file: try classify each line or dump binary -------
    if (nano_ai_is_ascii(buf, len)) {

        char *copy = malloc(len + 1);
        memcpy(copy, buf, len);
        copy[len] = 0;

        char *saveptr = NULL;
        char *line = strtok_r(copy, "\n", &saveptr);

        while (line) {
            if (*line) {
                const char *type = detect_type(line);
                html_add_pattern(html, virus_index, pattern_index++, type, line);
            }
            line = strtok_r(NULL, "\n", &saveptr);
        }

        free(copy);
        return;
    }

    // binary fallback
    html_add_ldb(html, virus_index, buf, len);
}

//--------------------------------------------------------------------------------------
// MAIN — iterate extracted files and build HTML catalog
//--------------------------------------------------------------------------------------
int main(void)
{
    printf("Generating Ultra HTML Catalog ...\n");

    DIR *d = opendir(OUTPUT_DIR);
    if (!d) {
        fprintf(stderr, "Cannot open extracted directory.\n");
        return 1;
    }

    FILE *html = fopen("virus_catalog.html", "w");
    if (!html) {
        fprintf(stderr, "Cannot write HTML file.\n");
        return 1;
    }

    // simple HTML
    fprintf(html,
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<meta charset='UTF-8'>\n"
        "<title>Virus Catalog Ultra</title>\n"
        "<style>\n"
        "body { font-family: Arial; margin: 20px; }\n"
        "h1 { border-bottom: 1px solid #ccc; padding-bottom: 4px; }\n"
        "h2 { color: #444; margin-top: 25px; }\n"
        "pre { background:#f0f0f0; padding:10px; overflow-x:auto; }\n"
        "p { background:#fafafa; padding:6px; border:1px solid #eee; }\n"
        "</style>\n"
        "</head>\n<body>\n"
        "<h1>ClamAV Virus Catalog (Ultra)</h1>\n");

    unsigned long long virus_index = 1;

    struct dirent *de;
    char path[512];

    while ((de = readdir(d))) {
        if (de->d_type != DT_REG) continue;

        snprintf(path, sizeof(path), "%s/%s", OUTPUT_DIR, de->d_name);

        FILE *f = fopen(path, "rb");
        if (!f) continue;

        fseek(f, 0, SEEK_END);
        size_t flen = ftell(f);
        fseek(f, 0, SEEK_SET);

        unsigned char *buf = malloc(flen);
        fread(buf, 1, flen, f);
        fclose(f);

        fprintf(html, "<h1>%llu. %s</h1>\n", virus_index, de->d_name);

        process_file(html, de->d_name, buf, flen, virus_index);

        free(buf);
        virus_index++;
    }

    closedir(d);

    fprintf(html, "</body></html>\n");
    fclose(html);

    printf("✓ Ultra HTML Catalog generated: virus_catalog.html\n");
    return 0;
}

