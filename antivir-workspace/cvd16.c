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
#include <stdint.h>

/* Config */
#define HEADER_SIZE 512
static const char *OUTPUT_DIR = "extracted";

/* ANSI Farben und Cursorsteuerung */
#define ANSI_RESET        "\x1b[0m"
#define ANSI_BOLD         "\x1b[1m"
#define ANSI_FAINT        "\x1b[2m"
#define ANSI_DIMGRAY      "\x1b[90m"
#define ANSI_GREEN        "\x1b[32m"
#define ANSI_YELLOW       "\x1b[33m"
#define ANSI_BLUE         "\x1b[34m"
#define ANSI_MAGENTA      "\x1b[35m"
#define ANSI_CYAN         "\x1b[36m"

#define CSI               "\x1b["
#define HIDE_CURSOR       "\x1b[?25l"
#define SHOW_CURSOR       "\x1b[?25h"
#define CLEAR_LINE        "\x1b[2K"

/* Feste Zeile für Status (nicht mit Fortschrittsbalken überlappen) */
static const int STATUS_ROW = 3;

/* =====================================================================================
   Utilities: ASCII heuristic, hex dump
   ===================================================================================== */
int nano_ai_is_ascii(const unsigned char *buf, size_t len) {
    if (len == 0) return 1;
    size_t printable = 0;
    for (size_t i=0; i<len; i++) {
        unsigned char c = buf[i];
        if (c == 9 || c == 10 || c == 13) { printable++; continue; }
        if (c >= 32 && c <= 126) printable++;
    }
    return printable * 100 / len >= 85;
}

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

/* =====================================================================================
   TAR + GZIP helpers (falls upstream genutzt)
   ===================================================================================== */
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
    size_t cap = src_len ? src_len * 4 : 1024;
    unsigned char *out = (unsigned char*)malloc(cap);
    if (!out) { inflateEnd(&s); return NULL; }
    s.next_in = (Bytef*)src;
    s.avail_in = src_len;
    size_t total = 0;
    for (;;) {
        if (total == cap) {
            cap *= 2;
            unsigned char *new_out = (unsigned char*)realloc(out, cap);
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

/* =====================================================================================
   Type detection and virus-name extraction
   ===================================================================================== */
const char *detect_type(const char *line) {
    size_t n = strlen(line);
    if (n >= 64 && strspn(line, "0123456789abcdefABCDEF") >= 64) return "SHA256 HASH";
    if (n >= 40 && strspn(line, "0123456789abcdefABCDEF") >= 40) return "SHA1 HASH";
    if (n >= 32 && strspn(line, "0123456789abcdefABCDEF") >= 32) return "MD5 HASH";
    if (strstr(line, "/") && strstr(line, "$")) return "REGEX PATTERN";
    if (strstr(line, ":") && strstr(line, "??")) return "BYTE SEQUENCE";
    return "UNKNOWN PATTERN";
}

char *extract_virus_name_from_line(const char *line) {
    if (!line || !*line) return NULL;
    const char *tag = strstr(line, "VirusName");
    if (tag) {
        const char *p = tag + strlen("VirusName");
        if (*p == ':' || *p == '=') {
            p++;
            while (*p && isspace((unsigned char)*p)) p++;
            const char *end = p;
            while (*end && *end != '\n' && *end != '\r' && *end != ';') end++;
            size_t len = (size_t)(end - p);
            if (len > 0) { char *name = (char*)malloc(len+1); if (!name) return NULL; memcpy(name,p,len); name[len]=0; return name; }
        }
    }
    const char *c = strchr(line, ':');
    if (c) {
        size_t len = (size_t)(c - line);
        if (len > 0) { char *name = (char*)malloc(len+1); if (!name) return NULL; memcpy(name, line, len); name[len]=0; return name; }
    }
    return NULL;
}

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
                if (namelen > 0) { char *name = (char*)malloc(namelen+1); if (!name) return NULL; memcpy(name, buf+start, namelen); name[namelen]=0; return name; }
            }
        }
    }
    return NULL;
}

char *fallback_name_from_filename(const char *filename) {
    if (!filename) return NULL;
    const char *dot = strrchr(filename, '.');
    size_t len = dot ? (size_t)(dot - filename) : strlen(filename);
    if (len == 0) len = strlen(filename);
    char *name = (char*)malloc(len+1);
    if (!name) return NULL;
    memcpy(name, filename, len);
    name[len]=0;
    return name;
}

/* =====================================================================================
   Entry structures
   ===================================================================================== */
typedef enum { ENTRY_TEXT=0, ENTRY_BINARY=1 } entry_kind;

typedef struct entry {
    entry_kind kind;
    char *type_label;
    char *text_line;
    unsigned char *bin;
    size_t bin_len;
    struct entry *next;
} entry;

typedef struct group {
    char *key;       /* virus name */
    entry *items;    /* linked list (insertion order) */
} group;

/* =====================================================================================
   Robin Hood Hash Map (open addressing) für schnelle Gruppierung
   ===================================================================================== */
typedef struct {
    group *slots;
    size_t cap;
    size_t size;
} rh_map;

static size_t hash_str(const char *s) {
    uint64_t h = 1469598103934665603ULL; /* FNV-1a 64-bit */
    for (const unsigned char *p = (const unsigned char*)s; *p; ++p) {
        h ^= *p;
        h *= 1099511628211ULL;
    }
    return (size_t)h;
}

void rh_init(rh_map *m, size_t initial_cap) {
    if (initial_cap < 16) initial_cap = 16;
    size_t cap = 1;
    while (cap < initial_cap) cap <<= 1;
    m->slots = (group*)calloc(cap, sizeof(group));
    m->cap = m->slots ? cap : 0;
    m->size = 0;
}

void rh_free(rh_map *m) {
    if (!m || !m->slots) return;
    for (size_t i=0;i<m->cap;i++) {
        if (m->slots[i].key) {
            free(m->slots[i].key);
            entry *e = m->slots[i].items;
            while (e) {
                entry *n = e->next;
                free(e->type_label);
                free(e->text_line);
                free(e->bin);
                free(e);
                e = n;
            }
        }
    }
    free(m->slots);
    m->slots = NULL; m->cap = 0; m->size = 0;
}

int rh_should_resize(rh_map *m) { return m->size * 100 >= m->cap * 85; }

int rh_put_if_absent(rh_map *m, const char *key, group **out_g);

int rh_rehash(rh_map *m, size_t new_cap) {
    group *old = m->slots; size_t old_cap = m->cap;
    rh_init(m, new_cap);
    if (!m->slots) { free(old); return -1; }
    for (size_t i=0;i<old_cap;i++) {
        if (!old[i].key) continue;
        group *g = NULL;
        if (rh_put_if_absent(m, old[i].key, &g) != 0) { free(old); return -1; }
        g->key = old[i].key;           /* Ownership verschieben */
        g->items = old[i].items;
        old[i].key = NULL;
        old[i].items = NULL;
    }
    free(old);
    return 0;
}

int rh_put_if_absent(rh_map *m, const char *key, group **out_g) {
    if (!m->slots) return -1;
    if (rh_should_resize(m)) {
        if (rh_rehash(m, m->cap ? m->cap * 2 : 32) != 0) return -1;
    }

    size_t cap = m->cap;
    size_t mask = cap - 1;
    size_t h = hash_str(key);
    size_t idx = h & mask;
    size_t dist = 0;

    char *key_copy = NULL;
    entry *tmp_items = NULL;

    for (;;) {
        if (!m->slots[idx].key) {
            if (!key_copy) { key_copy = strdup(key); if (!key_copy) return -1; }
            m->slots[idx].key = key_copy;
            m->slots[idx].items = tmp_items; /* normal NULL */
            m->size++;
            if (out_g) *out_g = &m->slots[idx];
            return 0;
        }
        if (strcmp(m->slots[idx].key, key) == 0) {
            if (out_g) *out_g = &m->slots[idx];
            return 0;
        }
        size_t victim_h = hash_str(m->slots[idx].key);
        size_t victim_dist = (idx - (victim_h & mask)) & mask;
        if (victim_dist < dist) {
            if (!key_copy) { key_copy = strdup(key); if (!key_copy) return -1; }
            char *swap_key = key_copy; key_copy = m->slots[idx].key; m->slots[idx].key = swap_key;
            entry *swap_items = tmp_items; tmp_items = m->slots[idx].items; m->slots[idx].items = swap_items;
            dist = victim_dist;
        }
        idx = (idx + 1) & mask;
        dist++;
    }
}

int group_append_entry(group *g, entry *e) {
    if (!g || !e) return -1;
    e->next = NULL;
    if (!g->items) { g->items = e; return 0; }
    entry *cur = g->items;
    while (cur->next) cur = cur->next;
    cur->next = e;
    return 0;
}

/* =====================================================================================
   MSD Sort (Strings)
   ===================================================================================== */
static inline unsigned char char_at(const char *s, size_t d) { return (unsigned char)s[d]; }

void insertion_sort_msd(char **a, int lo, int hi, size_t d) {
    for (int i = lo + 1; i <= hi; i++) {
        char *v = a[i];
        int j = i - 1;
        while (j >= lo) {
            const char *sj = a[j];
            size_t dj = d;
            while (char_at(v, dj) == char_at(sj, dj) && char_at(v, dj) != 0) dj++;
            if (char_at(v, dj) < char_at(sj, dj)) { a[j+1] = a[j]; j--; }
            else break;
        }
        a[j+1] = v;
    }
}

void msd_sort(char **a, int lo, int hi, size_t d) {
    if (hi <= lo) return;
    if (hi - lo <= 16) { insertion_sort_msd(a, lo, hi, d); return; }

    int lt = lo, gt = hi;
    unsigned char v = char_at(a[lo], d);
    int i = lo + 1;
    while (i <= gt) {
        unsigned char t = char_at(a[i], d);
        if (t < v) { char *tmp = a[lt]; a[lt] = a[i]; a[i] = tmp; lt++; i++; }
        else if (t > v) { char *tmp = a[i]; a[i] = a[gt]; a[gt] = tmp; gt--; }
        else i++;
    }
    msd_sort(a, lo, lt - 1, d);
    if (v != 0) msd_sort(a, lt, gt, d + 1);
    msd_sort(a, gt + 1, hi, d);
}

/* =====================================================================================
   Build entries
   ===================================================================================== */
const char *detect_type(const char *line); /* forward */

void add_text_line(rh_map *M, const char *filename, const char *line) {
    char *vname = extract_virus_name_from_line(line);
    if (!vname) vname = fallback_name_from_filename(filename);

    group *g = NULL;
    if (rh_put_if_absent(M, vname, &g) != 0) { free(vname); return; }
    free(vname);

    entry *e = (entry*)calloc(1, sizeof(entry));
    if (!e) return;
    e->kind = ENTRY_TEXT;
    e->type_label = strdup(detect_type(line));
    e->text_line = strdup(line);
    group_append_entry(g, e);
}

void add_binary_blob(rh_map *M, const char *filename, const unsigned char *buf, size_t len) {
    char *vname = extract_virus_name_from_buf(buf, len);
    if (!vname) vname = fallback_name_from_filename(filename);

    group *g = NULL;
    if (rh_put_if_absent(M, vname, &g) != 0) { free(vname); return; }
    free(vname);

    entry *e = (entry*)calloc(1, sizeof(entry));
    if (!e) return;
    e->kind = ENTRY_BINARY;
    if (len) {
        e->bin = (unsigned char*)malloc(len);
        if (!e->bin) { free(e); return; }
        memcpy(e->bin, buf, len);
        e->bin_len = len;
    }
    group_append_entry(g, e);
}

void process_file(rh_map *M,
                  const char *filename,
                  const unsigned char *buf,
                  size_t len)
{
    int is_text = nano_ai_is_ascii(buf, len);
    if (strstr(filename, ".hdb") || strstr(filename, ".ndb") || is_text) {
        char *copy = (char*)malloc(len + 1);
        if (!copy) return;
        memcpy(copy, buf, len);
        copy[len] = 0;

        char *saveptr = NULL;
        char *line = strtok_r(copy, "\n", &saveptr);
        while (line) {
            if (*line) add_text_line(M, filename, line);
            line = strtok_r(NULL, "\n", &saveptr);
        }
        free(copy);
        return;
    }
    /* Binary signatures (ldb/cbc/others) */
    add_binary_blob(M, filename, buf, len);
}

/* =====================================================================================
   Output
   ===================================================================================== */
void print_group_header(FILE *out, const char *virus_name) {
    fprintf(out, "\n" ANSI_BOLD ANSI_BLUE "################################################################################" ANSI_RESET "\n");
    fprintf(out, ANSI_BOLD "# VIRENNAME: %s\n" ANSI_RESET, virus_name);
    fprintf(out, ANSI_BOLD ANSI_BLUE "################################################################################" ANSI_RESET "\n");
}

void print_text_entry(FILE *out, unsigned long long idx, const entry *e) {
    fprintf(out, "\n" ANSI_BOLD ANSI_MAGENTA "==================== %llu — %s ====================" ANSI_RESET "\n",
            idx, e->type_label ? e->type_label : "SIGNATURE");
    fprintf(out, ANSI_DIMGRAY "------------------------------------------------------------\n" ANSI_RESET);
    fprintf(out, ANSI_BOLD "CONTENT:\n" ANSI_RESET);
    fprintf(out, ANSI_DIMGRAY "------------------------------------------------------------\n" ANSI_RESET);
    fprintf(out, "%s\n", e->text_line ? e->text_line : "");
}

void print_binary_entry(FILE *out, unsigned long long idx, const entry *e) {
    fprintf(out, "\n" ANSI_BOLD ANSI_CYAN "==================== %llu — LOGICAL / BYTECODE SIGNATURE ====================" ANSI_RESET "\n", idx);
    fprintf(out, ANSI_DIMGRAY "------------------------------------------------------------\n" ANSI_RESET);
    if (nano_ai_is_ascii(e->bin, e->bin_len)) {
        fprintf(out, ANSI_BOLD "ASCII DATA:\n" ANSI_RESET);
        fprintf(out, ANSI_DIMGRAY "------------------------------------------------------------\n" ANSI_RESET);
        fwrite(e->bin, 1, e->bin_len, out);
        if (e->bin_len == 0 || e->bin[e->bin_len-1] != '\n') fputc('\n', out);
    } else {
        fprintf(out, ANSI_BOLD "BINARY CONTENT (HEX + ASCII):\n" ANSI_RESET);
        fprintf(out, ANSI_DIMGRAY "------------------------------------------------------------\n" ANSI_RESET);
        ascii_hexdump(out, e->bin, e->bin_len);
    }
}

void emit_sorted(FILE *out, rh_map *M) {
    size_t n = M->size;
    char **keys = (char**)malloc(n * sizeof(char*));
    if (!keys) return;
    size_t k = 0;
    for (size_t i=0;i<M->cap;i++) {
        if (M->slots[i].key) keys[k++] = M->slots[i].key;
    }
    if (k) msd_sort(keys, 0, (int)k-1, 0);

    for (size_t i=0;i<k;i++) {
        const char *key = keys[i];
        size_t mask = M->cap - 1;
        size_t idx = hash_str(key) & mask;
        for (;;) {
            if (!M->slots[idx].key) break;
            if (strcmp(M->slots[idx].key, key) == 0) {
                print_group_header(out, key);
                unsigned long long idx_entry = 1;
                for (entry *e = M->slots[idx].items; e; e = e->next, idx_entry++) {
                    if (e->kind == ENTRY_TEXT) print_text_entry(out, idx_entry, e);
                    else print_binary_entry(out, idx_entry, e);
                }
                break;
            }
            idx = (idx + 1) & mask;
        }
    }
    free(keys);
}

/* =====================================================================================
   Fortschrittsbalken + Statuszeile (ANSI)
   ===================================================================================== */
void status_line_update(const char *msg) {
    fprintf(stderr, CSI "s");                   /* Cursor speichern */
    fprintf(stderr, CSI "%d;1H", STATUS_ROW);   /* Zu Zeile STATUS_ROW, Spalte 1 */
    fprintf(stderr, CLEAR_LINE);                /* Zeile löschen */
    fprintf(stderr, ANSI_BOLD ANSI_CYAN "STATUS:" ANSI_RESET " %s", msg);
    fprintf(stderr, CSI "u");                   /* Cursor wiederherstellen */
    fflush(stderr);
}

void progress_bar_start(size_t total) {
    fprintf(stderr, ANSI_BOLD "Verarbeite Dateien: " ANSI_RESET "(%zu gefunden)\n", total);
    /* Nach Kopfzeile: Fortschrittsbalken liegt in der aktuellen Zeile (z.B. Zeile 2) */
}

void progress_bar_update(size_t i, size_t total, const char *now_doing) {
    const int width = 40;
    double ratio = total ? (double)i / (double)total : 1.0;
    int filled = (int)(ratio * width);
    if (filled > width) filled = width;

    /* Fortschrittsbalken in aktueller Zeile aktualisieren */
    fprintf(stderr, "\r[");
    for (int j=0; j<width; j++) {
        if (j < filled) fprintf(stderr, ANSI_GREEN "=" ANSI_RESET);
        else fprintf(stderr, " ");
    }
    fprintf(stderr, "] %3.0f%%", ratio * 100.0);
    fflush(stderr);

    /* Statuszeile separat aktualisieren */
    if (now_doing) status_line_update(now_doing);
}

void progress_bar_finish(void) {
    fprintf(stderr, "\n" ANSI_GREEN ANSI_BOLD "✓ Fertig." ANSI_RESET "\n");
}

/* =====================================================================================
   Main
   ===================================================================================== */
int main(void) {
    printf(ANSI_BOLD "Generating Ultra ASCII Catalog (ANSI + Progressbar + Statuszeile) ..." ANSI_RESET "\n");

    fprintf(stderr, HIDE_CURSOR); /* optional: Cursor verstecken während der Arbeit */

    DIR *d = opendir(OUTPUT_DIR);
    if (!d) {
        fprintf(stderr, ANSI_YELLOW "Cannot open extracted directory." ANSI_RESET "\n");
        fprintf(stderr, SHOW_CURSOR);
        return 1;
    }

    /* Anzahl Dateien für Fortschrittsbalken zählen */
    size_t total_files = 0;
    struct dirent *de;
    while ((de = readdir(d))) {
        if (de->d_type == DT_REG) total_files++;
    }
    rewinddir(d);

    progress_bar_start(total_files);
    status_line_update("Initialisierung ...");

    FILE *out = fopen("virus_catalog.txt", "w");
    if (!out) {
        fprintf(stderr, ANSI_YELLOW "Cannot write ASCII catalog file." ANSI_RESET "\n");
        closedir(d);
        fprintf(stderr, SHOW_CURSOR);
        return 1;
    }

    fprintf(out,
        "################################################################################################\n"
        "#                                                                                                #\n"
        "#      CLAMAV VIRUS CATALOG (ULTRA) — NUR ASCII, GRUPPIERUNG: ROBIN-HOOD HASH, SORTIERUNG: MSD  #\n"
        "#                                                                                                #\n"
        "################################################################################################\n");

    rh_map M; rh_init(&M, 1024);

    char path[1024];
    size_t processed = 0;

    while ((de = readdir(d))) {
        if (de->d_type != DT_REG) continue;

        snprintf(path, sizeof(path), "%s/%s", OUTPUT_DIR, de->d_name);

        progress_bar_update(processed, total_files, "Öffne Datei ...");
        FILE *f = fopen(path, "rb");
        if (!f) {
            progress_bar_update(++processed, total_files, "Datei konnte nicht geöffnet werden");
            continue;
        }

        progress_bar_update(processed, total_files, "Lese Datei ...");
        fseek(f, 0, SEEK_END);
        size_t flen = (size_t)ftell(f);
        fseek(f, 0, SEEK_SET);

        unsigned char *buf = NULL;
        if (flen) {
            buf = (unsigned char*)malloc(flen);
            if (!buf) {
                fclose(f);
                progress_bar_update(++processed, total_files, "Speicherfehler beim Lesen");
                continue;
            }
            fread(buf, 1, flen, f);
        }
        fclose(f);

        progress_bar_update(processed, total_files, "Verarbeite Signaturen ...");
        process_file(&M, de->d_name, buf ? buf : (unsigned char*)"", flen);
        free(buf);

        progress_bar_update(++processed, total_files, "Fertig mit aktueller Datei");
    }

    closedir(d);
    progress_bar_finish();
    status_line_update("Erzeuge Ausgabe ...");

    emit_sorted(out, &M);

    fprintf(out, "\n\nEND OF CATALOG\n");
    fclose(out);

    rh_free(&M);

    status_line_update("Abgeschlossen");
    fprintf(stderr, SHOW_CURSOR);

    printf(ANSI_GREEN ANSI_BOLD "✓ Ultra ASCII Catalog generated: virus_catalog.txt" ANSI_RESET "\n");
    return 0;
}

