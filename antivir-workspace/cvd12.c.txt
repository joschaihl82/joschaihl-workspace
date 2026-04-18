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
static const char *OUTPUT_DIR = "extracted";

//--------------------------------------------------------------------------------------
// Simple ASCII heuristic (Nano‑AI version, safe & non‑interpreting)
//--------------------------------------------------------------------------------------
int nano_ai_is_ascii(const unsigned char *buf, size_t len) {
    if (len == 0) return 1;
    size_t printable = 0;
    for (size_t i=0; i<len; i++) {
        unsigned char c = buf[i];
        if (c == 9 || c == 10 || c == 13) { printable++; continue; }
        if (c >= 32 && c <= 126) printable++;
    }
    return printable * 100 / len >= 85;  // ≥85% printable → treat as ASCII
}

//--------------------------------------------------------------------------------------
// ASCII-art: hex + ASCII dump
//--------------------------------------------------------------------------------------
void ascii_hexdump(FILE *out, const unsigned char *buf, size_t len) {
    fprintf(out, "    OFFSET    | HEX BYTES                                           | ASCII\n");
    fprintf(out, "--------------+-----------------------------------------------------+----------------\n");
    size_t offset = 0;
    while (offset < len) {
        fprintf(out, "    %08zx | ", offset);
        for (size_t i = 0; i < 16; i++) {
            if (offset + i < len) fprintf(out, "%02x ", buf[offset+i]);
            else fprintf(out, "   ");
            if (i == 7) fputc(' ', out);
        }
        fprintf(out, "| ");
        for (size_t i = 0; i < 16 && offset+i < len; i++) {
            unsigned char c = buf[offset+i];
            fputc((c >= 32 && c <= 126) ? c : '.', out);
        }
        fputc('\n', out);
        offset += 16;
    }
}

//--------------------------------------------------------------------------------------
// TAR + GZIP extraction helpers (kept for completeness if needed elsewhere)
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
    if (inflateInit2(&s, 15+32) != Z_OK) return NULL;
    size_t cap = src_len * 4;
    if (cap == 0) cap = 1024;
    unsigned char *out = malloc(cap);
    if (!out) { inflateEnd(&s); return NULL; }
    s.next_in = (Bytef*)src;
    s.avail_in = src_len;
    size_t total = 0;
    for (;;) {
        if (total == cap) {
            cap *= 2;
            unsigned char *new_out = realloc(out, cap);
            if (!new_out) { free(out); inflateEnd(&s); return NULL; }
            out = new_out;
        }
        s.next_out = out + total;
        s.avail_out = cap - total;
        int ret = inflate(&s, Z_NO_FLUSH);
        total = cap - s.avail_out;
        if (ret == Z_STREAM_END) break;
        if (ret != Z_OK) { free(out); inflateEnd(&s); return NULL; }
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
// Type detection (for display)
//--------------------------------------------------------------------------------------
const char *detect_type(const char *line) {
    size_t n = strlen(line);

    // MD5 (32 hex)
    if (n >= 32 && strspn(line, "0123456789abcdefABCDEF") >= 32)
        return "MD5 HASH";

    // SHA1 (40 hex)
    if (n >= 40 && strspn(line, "0123456789abcdefABCDEF") >= 40)
        return "SHA1 HASH";

    // SHA256 (64 hex)
    if (n >= 64 && strspn(line, "0123456789abcdefABCDEF") >= 64)
        return "SHA256 HASH";

    // Regex indicator (heuristic)
    if (strstr(line, "/") && strstr(line, "$"))
        return "REGEX PATTERN";

    // Hex‑encoded byte pattern in NDB (heuristic)
    if (strstr(line, ":") && strstr(line, "??"))
        return "BYTE SEQUENCE";

    return "UNKNOWN PATTERN";
}

//--------------------------------------------------------------------------------------
// Virus name extraction
//--------------------------------------------------------------------------------------
// HDB/NDB lines often start with "VirusName:..."
// If colon-separated fields exist, the first field is typically the name.
char *extract_virus_name_from_line(const char *line) {
    if (!line || !*line) return NULL;

    // If line has "VirusName=" or "VirusName:" extract after it
    const char *tag = strstr(line, "VirusName");
    if (tag) {
        const char *p = tag + strlen("VirusName");
        if (*p == ':' || *p == '=') {
            p++;
            while (*p && isspace((unsigned char)*p)) p++;
            const char *end = p;
            while (*end && *end != '\n' && *end != '\r' && *end != ';') end++;
            size_t len = (size_t)(end - p);
            if (len > 0) {
                char *name = malloc(len + 1);
                if (!name) return NULL;
                memcpy(name, p, len);
                name[len] = 0;
                return name;
            }
        }
    }

    // Otherwise, take the first colon-separated token as name
    const char *c = strchr(line, ':');
    if (c) {
        size_t len = (size_t)(c - line);
        if (len > 0) {
            char *name = malloc(len + 1);
            if (!name) return NULL;
            memcpy(name, line, len);
            name[len] = 0;
            return name;
        }
    }

    return NULL;
}

// For binary buffers (ldb/cbc), try to find "VirusName=" or "VirusName:" sequences.
char *extract_virus_name_from_buf(const unsigned char *buf, size_t len) {
    if (!buf || len == 0) return NULL;
    const char *keys[] = {"VirusName=", "VirusName:"};
    for (int k = 0; k < 2; k++) {
        const char *key = keys[k];
        size_t keylen = strlen(key);
        for (size_t i = 0; i + keylen < len; i++) {
            if (memcmp(buf + i, key, keylen) == 0) {
                size_t start = i + keylen;
                while (start < len && isspace(buf[start])) start++;
                size_t end = start;
                while (end < len && buf[end] != '\n' && buf[end] != '\r' && buf[end] != ';' && buf[end] != 0) end++;
                size_t namelen = (end > start) ? (end - start) : 0;
                if (namelen > 0) {
                    char *name = malloc(namelen + 1);
                    if (!name) return NULL;
                    memcpy(name, buf + start, namelen);
                    name[namelen] = 0;
                    return name;
                }
            }
        }
    }
    return NULL;
}

// Fallback: derive a "virus" group name from filename base.
char *fallback_name_from_filename(const char *filename) {
    if (!filename) return NULL;
    const char *dot = strrchr(filename, '.');
    size_t len = dot ? (size_t)(dot - filename) : strlen(filename);
    if (len == 0) len = strlen(filename);
    char *name = malloc(len + 1);
    if (!name) return NULL;
    memcpy(name, filename, len);
    name[len] = 0;
    return name;
}

//--------------------------------------------------------------------------------------
// Group registry: print header once per virus name
//--------------------------------------------------------------------------------------
typedef struct {
    char **names;
    size_t count;
    size_t cap;
} group_registry;

void group_init(group_registry *g) {
    g->names = NULL; g->count = 0; g->cap = 0;
}

void group_free(group_registry *g) {
    if (!g) return;
    for (size_t i=0;i<g->count;i++) free(g->names[i]);
    free(g->names);
}

int group_seen(group_registry *g, const char *name) {
    for (size_t i=0;i<g->count;i++) {
        if (strcmp(g->names[i], name) == 0) return 1;
    }
    return 0;
}

int group_add(group_registry *g, const char *name) {
    if (group_seen(g, name)) return 0;
    if (g->count == g->cap) {
        size_t newcap = (g->cap == 0) ? 16 : g->cap * 2;
        char **nn = realloc(g->names, newcap * sizeof(char*));
        if (!nn) return -1;
        g->names = nn; g->cap = newcap;
    }
    g->names[g->count] = strdup(name);
    if (!g->names[g->count]) return -1;
    g->count++;
    return 0;
}

void print_group_header(FILE *out, const char *virus_name) {
    fprintf(out, "\n################################################################################\n");
    fprintf(out, "# VIRENNAME: %s\n", virus_name);
    fprintf(out, "################################################################################\n");
}

//--------------------------------------------------------------------------------------
// render pattern row (ASCII-art)
//--------------------------------------------------------------------------------------
void ascii_add_pattern(FILE *out,
                       unsigned long long pattern_index,
                       const char *type,
                       const char *content)
{
    fprintf(out, "\n==================== %llu — %s ====================\n",
            pattern_index, type);
    fprintf(out,
            "------------------------------------------------------------\n"
            "CONTENT:\n"
            "------------------------------------------------------------\n");
    fprintf(out, "%s\n", content);
}

//--------------------------------------------------------------------------------------
// Add binary (ldb) content — ASCII or hex+ASCII in ASCII-art
//--------------------------------------------------------------------------------------
void ascii_add_ldb(FILE *out,
                   const unsigned char *buf,
                   size_t len)
{
    fprintf(out, "\n==================== 1 — LOGICAL / BYTECODE SIGNATURE ====================\n");

    if (nano_ai_is_ascii(buf, len)) {
        fprintf(out,
            "------------------------------------------------------------\n"
            "ASCII DATA:\n"
            "------------------------------------------------------------\n");
        fwrite(buf, 1, len, out);
        if (len == 0 || buf[len-1] != '\n') fputc('\n', out);
        return;
    }

    fprintf(out,
        "------------------------------------------------------------\n"
        "BINARY CONTENT (HEX + ASCII):\n"
        "------------------------------------------------------------\n");
    ascii_hexdump(out, buf, len);
}

//--------------------------------------------------------------------------------------
// PROCESS FILE (hdb, ndb, ldb, cbc, anything) → ASCII-art with grouping
//--------------------------------------------------------------------------------------
void process_file(FILE *out,
                  group_registry *groups,
                  const char *filename,
                  const unsigned char *buf,
                  size_t len)
{
    unsigned long long pattern_index = 1;

    //-------------- HDB / NDB TEXT FILES --------------------------------------
    if (strstr(filename, ".hdb") || strstr(filename, ".ndb")) {

        char *copy = malloc(len + 1);
        if (!copy) return;
        memcpy(copy, buf, len);
        copy[len] = 0;

        char *saveptr = NULL;
        char *line = strtok_r(copy, "\n", &saveptr);

        while (line) {
            if (*line) {
                char *vname = extract_virus_name_from_line(line);
                if (!vname) vname = fallback_name_from_filename(filename);

                if (!group_seen(groups, vname)) {
                    group_add(groups, vname);
                    print_group_header(out, vname);
                    pattern_index = 1; // reset per group
                }

                const char *type = detect_type(line);
                ascii_add_pattern(out, pattern_index++, type, line);

                free(vname);
            }
            line = strtok_r(NULL, "\n", &saveptr);
        }

        free(copy);
        return;
    }

    //-------------- LDB / CBC BINARY SIGNATURE -------------------------------
    if (strstr(filename, ".ldb") || strstr(filename, ".cbc")) {
        char *vname = extract_virus_name_from_buf(buf, len);
        if (!vname) vname = fallback_name_from_filename(filename);

        if (!group_seen(groups, vname)) {
            group_add(groups, vname);
            print_group_header(out, vname);
        }

        ascii_add_ldb(out, buf, len);
        free(vname);
        return;
    }

    //-------------- Unknown file: try classify each line or dump binary -------
    if (nano_ai_is_ascii(buf, len)) {

        char *copy = malloc(len + 1);
        if (!copy) return;
        memcpy(copy, buf, len);
        copy[len] = 0;

        char *saveptr = NULL;
        char *line = strtok_r(copy, "\n", &saveptr);

        while (line) {
            if (*line) {
                char *vname = extract_virus_name_from_line(line);
                if (!vname) vname = fallback_name_from_filename(filename);

                if (!group_seen(groups, vname)) {
                    group_add(groups, vname);
                    print_group_header(out, vname);
                    pattern_index = 1;
                }

                const char *type = detect_type(line);
                ascii_add_pattern(out, pattern_index++, type, line);

                free(vname);
            }
            line = strtok_r(NULL, "\n", &saveptr);
        }

        free(copy);
        return;
    }

    // binary fallback
    {
        char *vname = extract_virus_name_from_buf(buf, len);
        if (!vname) vname = fallback_name_from_filename(filename);
        if (!group_seen(groups, vname)) {
            group_add(groups, vname);
            print_group_header(out, vname);
        }
        ascii_add_ldb(out, buf, len);
        free(vname);
    }
}

//--------------------------------------------------------------------------------------
// MAIN — iterate extracted files and build ASCII-art catalog grouped by virus name
//--------------------------------------------------------------------------------------
int main(void)
{
    printf("Generating Ultra ASCII Catalog (grouped by virus name) ...\n");

    DIR *d = opendir(OUTPUT_DIR);
    if (!d) {
        fprintf(stderr, "Cannot open extracted directory.\n");
        return 1;
    }

    FILE *out = fopen("virus_catalog.txt", "w");
    if (!out) {
        fprintf(stderr, "Cannot write ASCII catalog file.\n");
        closedir(d);
        return 1;
    }

    // ASCII-art header
    fprintf(out,
        "################################################################################################\n"
        "#                                                                                                #\n"
        "#                         CLAMAV VIRUS CATALOG (ULTRA) — NUR ASCII                              #\n"
        "#                              GRUPPIERT NACH VIRENNAME (GROSSSCHRIFT)                          #\n"
        "#                                                                                                #\n"
        "################################################################################################\n");

    struct dirent *de;
    char path[512];

    group_registry groups;
    group_init(&groups);

    while ((de = readdir(d))) {
        if (de->d_type != DT_REG) continue;

        snprintf(path, sizeof(path), "%s/%s", OUTPUT_DIR, de->d_name);

        FILE *f = fopen(path, "rb");
        if (!f) continue;

        fseek(f, 0, SEEK_END);
        size_t flen = (size_t)ftell(f);
        fseek(f, 0, SEEK_SET);

        unsigned char *buf = (flen ? malloc(flen) : NULL);
        if (flen && !buf) { fclose(f); continue; }

        if (flen) fread(buf, 1, flen, f);
        fclose(f);

        process_file(out, &groups, de->d_name, buf ? buf : (unsigned char*)"", flen);

        free(buf);
    }

    closedir(d);

    fprintf(out, "\n\nEND OF CATALOG\n");
    fclose(out);

    group_free(&groups);

    printf("✓ Ultra ASCII Catalog generated: virus_catalog.txt\n");
    return 0;
}


