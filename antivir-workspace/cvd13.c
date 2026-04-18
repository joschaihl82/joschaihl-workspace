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

// Config
#define HEADER_SIZE 512
static const char *OUTPUT_DIR = "extracted";

// =====================================================================================
// Utilities (ASCII heuristic, hex dump)
// =====================================================================================
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

// =====================================================================================
// TAR + GZIP helpers (kept if needed upstream in your pipeline)
// =====================================================================================
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

// =====================================================================================
// Type detection and virus-name extraction
// =====================================================================================
const char *detect_type(const char *line) {
    size_t n = strlen(line);
    if (n >= 64 && strspn(line, "0123456789abcdefABCDEF") >= 64) return "SHA256 HASH";
    if (n >= 40 && strspn(line, "0123456789abcdefABCDEF") >= 40) return "SHA1 HASH";
    if (n >= 32 && strspn(line, "0123456789abcdefABCDEF") >= 32) return "MD5 HASH";
    if (strstr(line, "/") && strstr(line, "$")) return "REGEX PATTERN";
    if (strstr(line, ":") && strstr(line, "??")) return "BYTE SEQUENCE";
    return "UNKNOWN PATTERN";
}

// Try VirusName= / VirusName:, else first colon token, else filename base
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
            if (len > 0) { char *name = malloc(len+1); if (!name) return NULL; memcpy(name,p,len); name[len]=0; return name; }
        }
    }
    const char *c = strchr(line, ':');
    if (c) {
        size_t len = (size_t)(c - line);
        if (len > 0) { char *name = malloc(len+1); if (!name) return NULL; memcpy(name, line, len); name[len]=0; return name; }
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
                if (namelen > 0) { char *name = malloc(namelen+1); if (!name) return NULL; memcpy(name, buf+start, namelen); name[namelen]=0; return name; }
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
    char *name = malloc(len+1);
    if (!name) return NULL;
    memcpy(name, filename, len);
    name[len]=0;
    return name;
}

// =====================================================================================
// Red-Black Tree for grouping by virus name (sorted output)
// =====================================================================================
typedef enum { RB_RED=0, RB_BLACK=1 } rb_color;
typedef enum { ENTRY_TEXT=0, ENTRY_BINARY=1 } entry_kind;

typedef struct entry {
    entry_kind kind;
    char *type_label;           // for text entries
    char *text_line;            // for text entries
    unsigned char *bin;         // for binary entries
    size_t bin_len;
    struct entry *next;
} entry;

typedef struct rb_node {
    char *key;                  // virus name
    entry *items;               // linked list of entries (insertion order)
    rb_color color;
    struct rb_node *left, *right, *parent;
} rb_node;

typedef struct {
    rb_node *root;
} rb_tree;

rb_node *rb_new_node(const char *key) {
    rb_node *n = (rb_node*)calloc(1, sizeof(rb_node));
    if (!n) return NULL;
    n->key = strdup(key);
    if (!n->key) { free(n); return NULL; }
    n->color = RB_RED;
    return n;
}

void rb_left_rotate(rb_tree *T, rb_node *x) {
    rb_node *y = x->right;
    x->right = y->left;
    if (y->left) y->left->parent = x;
    y->parent = x->parent;
    if (!x->parent) T->root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void rb_right_rotate(rb_tree *T, rb_node *y) {
    rb_node *x = y->left;
    y->left = x->right;
    if (x->right) x->right->parent = y;
    x->parent = y->parent;
    if (!y->parent) T->root = x;
    else if (y == y->parent->left) y->parent->left = x;
    else y->parent->right = x;
    x->right = y;
    y->parent = x;
}

void rb_insert_fixup(rb_tree *T, rb_node *z) {
    while (z->parent && z->parent->color == RB_RED) {
        if (z->parent == z->parent->parent->left) {
            rb_node *y = z->parent->parent->right;
            if (y && y->color == RB_RED) {
                z->parent->color = RB_BLACK;
                y->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) { z = z->parent; rb_left_rotate(T, z); }
                z->parent->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                rb_right_rotate(T, z->parent->parent);
            }
        } else {
            rb_node *y = z->parent->parent->left;
            if (y && y->color == RB_RED) {
                z->parent->color = RB_BLACK;
                y->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) { z = z->parent; rb_right_rotate(T, z); }
                z->parent->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                rb_left_rotate(T, z->parent->parent);
            }
        }
    }
    if (T->root) T->root->color = RB_BLACK;
}

rb_node *rb_insert_or_get(rb_tree *T, const char *key) {
    rb_node *y = NULL;
    rb_node *x = T->root;

    while (x) {
        y = x;
        int cmp = strcmp(key, x->key);
        if (cmp == 0) return x;
        else if (cmp < 0) x = x->left;
        else x = x->right;
    }
    rb_node *z = rb_new_node(key);
    if (!z) return NULL;
    z->parent = y;
    if (!y) T->root = z;
    else if (strcmp(key, y->key) < 0) y->left = z;
    else y->right = z;

    rb_insert_fixup(T, z);
    return z;
}

void rb_free_node_chain(entry *e) {
    while (e) {
        entry *n = e->next;
        free(e->type_label);
        free(e->text_line);
        free(e->bin);
        free(e);
        e = n;
    }
}

void rb_free_subtree(rb_node *n) {
    if (!n) return;
    rb_free_subtree(n->left);
    rb_free_subtree(n->right);
    rb_free_node_chain(n->items);
    free(n->key);
    free(n);
}

void rb_free(rb_tree *T) {
    rb_free_subtree(T->root);
    T->root = NULL;
}

// =====================================================================================
// Output helpers (ASCII headings and entries)
// =====================================================================================
void print_group_header(FILE *out, const char *virus_name) {
    fprintf(out, "\n################################################################################\n");
    fprintf(out, "# VIRENNAME: %s\n", virus_name);
    fprintf(out, "################################################################################\n");
}

void print_text_entry(FILE *out, unsigned long long idx, const entry *e) {
    fprintf(out, "\n==================== %llu — %s ====================\n", idx, e->type_label ? e->type_label : "SIGNATURE");
    fprintf(out, "------------------------------------------------------------\n");
    fprintf(out, "CONTENT:\n");
    fprintf(out, "------------------------------------------------------------\n");
    fprintf(out, "%s\n", e->text_line ? e->text_line : "");
}

void print_binary_entry(FILE *out, unsigned long long idx, const entry *e) {
    fprintf(out, "\n==================== %llu — LOGICAL / BYTECODE SIGNATURE ====================\n", idx);
    fprintf(out, "------------------------------------------------------------\n");
    if (nano_ai_is_ascii(e->bin, e->bin_len)) {
        fprintf(out, "ASCII DATA:\n");
        fprintf(out, "------------------------------------------------------------\n");
        fwrite(e->bin, 1, e->bin_len, out);
        if (e->bin_len == 0 || e->bin[e->bin_len-1] != '\n') fputc('\n', out);
    } else {
        fprintf(out, "BINARY CONTENT (HEX + ASCII):\n");
        fprintf(out, "------------------------------------------------------------\n");
        ascii_hexdump(out, e->bin, e->bin_len);
    }
}

// In-order traversal for sorted output
void rb_inorder_print(FILE *out, rb_node *n) {
    if (!n) return;
    rb_inorder_print(out, n->left);

    print_group_header(out, n->key);
    unsigned long long idx = 1;
    for (entry *e = n->items; e; e = e->next, idx++) {
        if (e->kind == ENTRY_TEXT) print_text_entry(out, idx, e);
        else print_binary_entry(out, idx, e);
    }

    rb_inorder_print(out, n->right);
}

// Append entry to group's list (preserve insertion order)
int group_append_entry(rb_node *group, entry *e) {
    if (!group || !e) return -1;
    e->next = NULL;
    if (!group->items) { group->items = e; return 0; }
    entry *cur = group->items;
    while (cur->next) cur = cur->next;
    cur->next = e;
    return 0;
}

// =====================================================================================
// Build grouped entries
// =====================================================================================
void add_text_line(rb_tree *T, const char *filename, const char *line) {
    char *vname = extract_virus_name_from_line(line);
    if (!vname) vname = fallback_name_from_filename(filename);

    rb_node *group = rb_insert_or_get(T, vname);
    free(vname);
    if (!group) return;

    entry *e = (entry*)calloc(1, sizeof(entry));
    if (!e) return;
    e->kind = ENTRY_TEXT;
    const char *type = detect_type(line);
    e->type_label = strdup(type);
    e->text_line = strdup(line);

    group_append_entry(group, e);
}

void add_binary_blob(rb_tree *T, const char *filename, const unsigned char *buf, size_t len) {
    char *vname = extract_virus_name_from_buf(buf, len);
    if (!vname) vname = fallback_name_from_filename(filename);

    rb_node *group = rb_insert_or_get(T, vname);
    free(vname);
    if (!group) return;

    entry *e = (entry*)calloc(1, sizeof(entry));
    if (!e) return;
    e->kind = ENTRY_BINARY;
    if (len) {
        e->bin = (unsigned char*)malloc(len);
        if (!e->bin) { free(e); return; }
        memcpy(e->bin, buf, len);
        e->bin_len = len;
    }
    group_append_entry(group, e);
}

void process_file(rb_tree *T,
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
            if (*line) add_text_line(T, filename, line);
            line = strtok_r(NULL, "\n", &saveptr);
        }
        free(copy);
        return;
    }
    // Binary signatures (ldb/cbc/others)
    add_binary_blob(T, filename, buf, len);
}

// =====================================================================================
// Main: build RB-tree catalog and print sorted ASCII output
// =====================================================================================
int main(void) {
    printf("Generating Ultra ASCII Catalog (red-black tree grouping) ...\n");

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

    fprintf(out,
        "################################################################################################\n"
        "#                                                                                                #\n"
        "#                CLAMAV VIRUS CATALOG (ULTRA) — NUR ASCII, SORTIERT NACH VIRENNAME              #\n"
        "#                          RED/BLACK TREE (ROT-SCHWARZ BAUM) GRUPPIERUNG                        #\n"
        "#                                                                                                #\n"
        "################################################################################################\n");

    rb_tree T = { .root = NULL };

    struct dirent *de;
    char path[1024];

    while ((de = readdir(d))) {
        if (de->d_type != DT_REG) continue;

        snprintf(path, sizeof(path), "%s/%s", OUTPUT_DIR, de->d_name);

        FILE *f = fopen(path, "rb");
        if (!f) continue;

        fseek(f, 0, SEEK_END);
        size_t flen = (size_t)ftell(f);
        fseek(f, 0, SEEK_SET);

        unsigned char *buf = NULL;
        if (flen) {
            buf = (unsigned char*)malloc(flen);
            if (!buf) { fclose(f); continue; }
            fread(buf, 1, flen, f);
        }
        fclose(f);

        process_file(&T, de->d_name, buf ? buf : (unsigned char*)"", flen);

        free(buf);
    }

    closedir(d);

    rb_inorder_print(out, T.root);

    fprintf(out, "\n\nEND OF CATALOG\n");
    fclose(out);

    rb_free(&T);

    printf("✓ Ultra ASCII Catalog generated: virus_catalog.txt\n");
    return 0;
}

