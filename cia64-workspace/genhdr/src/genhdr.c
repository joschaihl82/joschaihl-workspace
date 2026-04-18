/*
 * genhdr_readable_fixed.c
 *
 * Readable, safer prototype extractor (convention over configuration).
 * Fix: only consider top-level '{' as function-body start by tracking brace depth.
 *
 * Build:
 *   gcc -std=c11 -O2 -Wall -Wextra -o genhdr_readable_fixed genhdr_readable_fixed.c
 *
 * Usage:
 *   ./genhdr_readable_fixed [-o out.h] file1.c [file2.c ...]
 *
 * Notes:
 *  - Heuristic extractor (not a full C parser). Works best on typical C code.
 *  - All allocations use xcalloc/xreallocarray/xstrdup with overflow checks.
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/* Configuration */
#define DEFAULT_OUTPUT "prototypes.h"
#define READ_CHUNK (64 * 1024)
#define MAX_SIGNATURE_LEN (16 * 1024)
#define MAX_PARAM_CLEAN_LEN (8 * 1024)

/* Safe allocation helpers */
static int mul_overflow_size(size_t a, size_t b, size_t *out) {
    if (b != 0 && a > SIZE_MAX / b) return 1;
    *out = a * b;
    return 0;
}
static void *xcalloc(size_t nmemb, size_t size) {
    size_t total;
    if (mul_overflow_size(nmemb, size, &total)) {
        fprintf(stderr, "Allocation overflow: %zu * %zu\n", nmemb, size);
        exit(EXIT_FAILURE);
    }
    void *p = calloc(1, total);
    if (!p) { fprintf(stderr, "Out of memory allocating %zu bytes\n", total); exit(EXIT_FAILURE); }
    return p;
}
static void *xreallocarray(void *ptr, size_t nmemb, size_t size) {
    size_t total;
    if (mul_overflow_size(nmemb, size, &total)) {
        fprintf(stderr, "Reallocation overflow: %zu * %zu\n", nmemb, size);
        exit(EXIT_FAILURE);
    }
    void *p = realloc(ptr, total);
    if (!p) { fprintf(stderr, "Out of memory reallocating %zu bytes\n", total); exit(EXIT_FAILURE); }
    return p;
}
static char *xstrdup(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char *p = xcalloc(n, 1);
    memcpy(p, s, n);
    return p;
}
static void safe_strcat(char *dst, size_t cap, const char *src) {
    size_t d = strlen(dst), s = strlen(src);
    if (d + s + 1 > cap) { fprintf(stderr, "String buffer overflow prevented\n"); exit(EXIT_FAILURE); }
    memcpy(dst + d, src, s + 1);
}

/* Utilities */
static void trim_inplace(char *s) {
    if (!s) return;
    char *start = s;
    while (*start && isspace((unsigned char)*start)) start++;
    if (start != s) memmove(s, start, strlen(start) + 1);
    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) s[--len] = '\0';
}
static void usage_and_exit(const char *prog) {
    fprintf(stderr, "Usage: %s [-o out.h] file1.c [file2.c ...]\n", prog);
    exit(EXIT_FAILURE);
}

/* File reading */
static char *read_entire_file(const char *path, size_t *out_len) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    size_t cap = READ_CHUNK;
    char *buf = xcalloc(cap + 1, 1);
    size_t len = 0;
    while (1) {
        size_t want = READ_CHUNK;
        if (len + want + 1 > cap) {
            size_t newcap = cap + READ_CHUNK;
            buf = xreallocarray(buf, newcap + 1, 1);
            cap = newcap;
        }
        size_t r = fread(buf + len, 1, want, f);
        len += r;
        if (r < want) break;
    }
    buf[len] = '\0';
    fclose(f);
    if (out_len) *out_len = len;
    return buf;
}

/* Strip comments and string/char literals while preserving newlines */
static char *strip_comments_and_strings(const char *src) {
    size_t n = strlen(src);
    char *out = xcalloc(n + 1, 1);
    size_t i = 0, j = 0;
    while (i < n) {
        if (src[i] == '/' && i + 1 < n && src[i + 1] == '*') {
            i += 2;
            while (i + 1 < n && !(src[i] == '*' && src[i + 1] == '/')) {
                if (src[i] == '\n') out[j++] = '\n';
                i++;
            }
            if (i + 1 < n) i += 2;
        } else if (src[i] == '/' && i + 1 < n && src[i + 1] == '/') {
            i += 2;
            while (i < n && src[i] != '\n') i++;
        } else if (src[i] == '"' || src[i] == '\'') {
            char q = src[i++];
            out[j++] = ' ';
            while (i < n) {
                if (src[i] == '\\' && i + 1 < n) { i += 2; continue; }
                if (src[i] == q) { i++; break; }
                if (src[i] == '\n') out[j++] = '\n';
                i++;
            }
            out[j++] = ' ';
        } else {
            out[j++] = src[i++];
        }
    }
    out[j] = '\0';
    return out;
}

/* Quick rejects and attribute removal */
static int is_control_word(const char *w) {
    const char *kw[] = {"if","for","while","switch","return","sizeof","else","case","do","goto"};
    for (size_t i = 0; i < sizeof(kw)/sizeof(kw[0]); ++i) if (strcmp(w, kw[i]) == 0) return 1;
    return 0;
}
static void remove_trailing_attributes(char *s) {
    if (!s) return;
    char *p;
    while ((p = strstr(s, "__attribute__")) != NULL) {
        char *q = strchr(p, '(');
        if (!q) break;
        int depth = 0;
        char *r = q;
        while (*r) {
            if (*r == '(') depth++;
            else if (*r == ')') { depth--; if (depth == 0) { r++; break; } }
            r++;
        }
        if (*r == '\0') break;
        memmove(p, r, strlen(r) + 1);
    }
    while ((p = strstr(s, "__declspec")) != NULL) {
        char *q = strchr(p, '(');
        if (!q) break;
        int depth = 0;
        char *r = q;
        while (*r) {
            if (*r == '(') depth++;
            else if (*r == ')') { depth--; if (depth == 0) { r++; break; } }
            r++;
        }
        if (*r == '\0') break;
        memmove(p, r, strlen(r) + 1);
    }
}

/* Strip parameter names heuristically */
static char *strip_parameter_names(const char *params) {
    if (!params) return xstrdup("()");
    size_t n = strlen(params);
    if (n > MAX_PARAM_CLEAN_LEN) return xstrdup("()");
    char *out = xcalloc(n + 1, 1);
    size_t oi = 0, i = 0;
    if (params[i] == '(') out[oi++] = params[i++];
    int depth = 0;
    while (i < n) {
        if (params[i] == ')' && depth == 0) { out[oi++] = ')'; i++; break; }
        if (params[i] == '(') { depth++; out[oi++] = '('; i++; continue; }
        if (params[i] == ')') { depth--; out[oi++] = ')'; i++; continue; }
        size_t start = i, j = i;
        int local_depth = depth;
        while (j < n) {
            if (params[j] == '(') local_depth++;
            else if (params[j] == ')') { if (local_depth == 0) break; local_depth--; }
            else if (params[j] == ',' && local_depth == depth) break;
            j++;
        }
        size_t plen = j - start;
        if (plen == 0) { i = j + 1; continue; }
        char *part = xcalloc(plen + 1, 1);
        memcpy(part, params + start, plen);
        part[plen] = '\0';
        trim_inplace(part);
        size_t k = strlen(part);
        if (k > 0) {
            while (k > 0 && isspace((unsigned char)part[k - 1])) part[--k] = '\0';
            size_t endid = k;
            while (endid > 0 && (isalnum((unsigned char)part[endid - 1]) || part[endid - 1] == '_')) endid--;
            if (endid < k) {
                size_t p = endid;
                while (p > 0 && isspace((unsigned char)part[p - 1])) p--;
                if (p > 0) { part[endid] = '\0'; trim_inplace(part); }
            }
        }
        size_t plen2 = strlen(part);
        if (oi + plen2 + 4 > n) { free(part); free(out); return xstrdup("()"); }
        memcpy(out + oi, part, plen2); oi += plen2;
        free(part);
        if (j < n && params[j] == ',') { out[oi++] = ','; out[oi++] = ' '; j++; }
        i = j;
    }
    out[oi] = '\0';
    return out;
}

/*
 * Core extraction:
 * - Track brace depth while scanning the cleaned buffer.
 * - Only consider a '{' as a function-body start when brace_depth == 0.
 * - This prevents braces inside functions from being treated as new function starts.
 */
static void extract_prototypes_from_clean_buffer(const char *buf, FILE *out, const char *srcname) {
    size_t len = strlen(buf);
    size_t i = 0;
    int brace_depth = 0; /* top-level when 0 */

    while (i < len) {
        char c = buf[i];

        /* Update brace depth for non-candidate positions */
        if (c == '{') {
            /* If this '{' is at top-level (brace_depth == 0), it's a candidate for a function body start.
               Otherwise it's an inner block and must be ignored for prototype extraction. */
            if (brace_depth == 0) {
                /* Candidate: attempt to extract signature that precedes this '{' */
                size_t j = i;
                while (j > 0 && isspace((unsigned char)buf[j - 1])) j--;
                if (j == 0 || buf[j - 1] != ')') {
                    /* Not a function-like pattern; treat as normal brace */
                    brace_depth++;
                    i++;
                    continue;
                }

                /* Find matching '(' for that ')' by scanning backward */
                size_t r = j - 1;
                int depth = 0;
                while (r > 0) {
                    if (buf[r] == ')') depth++;
                    else if (buf[r] == '(') {
                        depth--;
                        if (depth == 0) break;
                    }
                    r--;
                }
                if (r == 0 && buf[r] != '(') {
                    /* malformed or not a function; treat as normal brace */
                    brace_depth++;
                    i++;
                    continue;
                }

                /* Backtrack to previous ';' or '}' or start to capture the signature */
                size_t start = r;
                while (start > 0 && buf[start - 1] != ';' && buf[start - 1] != '}') start--;

                /* Expand a bit to include return type, but cap the signature length */
                size_t ext = (start > 512) ? start - 512 : 0;
                start = ext;

                size_t siglen = i - start;
                if (siglen == 0 || siglen > MAX_SIGNATURE_LEN) {
                    brace_depth++;
                    i++;
                    continue;
                }

                char *sig = xcalloc(siglen + 1, 1);
                memcpy(sig, buf + start, siglen);
                sig[siglen] = '\0';
                trim_inplace(sig);

                /* Quick rejects */
                if (strstr(sig, "typedef") || strstr(sig, "struct") || strstr(sig, "enum") || strstr(sig, "union") || strstr(sig, "#define")) {
                    free(sig);
                    brace_depth++;
                    i++;
                    continue;
                }

                /* Find last ')' and its matching '(' inside sig */
                char *rparen = strrchr(sig, ')');
                char *lparen = NULL;
                if (rparen) {
                    int d = 0;
                    for (char *p = rparen; p >= sig; --p) {
                        if (*p == ')') d++;
                        else if (*p == '(') {
                            d--;
                            if (d == 0) { lparen = p; break; }
                        }
                    }
                }
                if (!lparen) { free(sig); brace_depth++; i++; continue; }

                /* Extract candidate name */
                char namebuf[256] = {0};
                char *p = lparen - 1;
                while (p >= sig && isspace((unsigned char)*p)) p--;
                char *end = p;
                while (p >= sig && (isalnum((unsigned char)*p) || *p == '_' || *p == '*')) p--;
                p++;
                size_t nlen = (size_t)(end - p + 1);
                if (nlen >= sizeof(namebuf)) nlen = sizeof(namebuf) - 1;
                memcpy(namebuf, p, nlen);
                namebuf[nlen] = '\0';
                trim_inplace(namebuf);

                if (namebuf[0] == '\0' || is_control_word(namebuf)) {
                    free(sig);
                    brace_depth++;
                    i++;
                    continue;
                }

                /* Skip static functions by convention */
                if (strstr(sig, "static") != NULL) {
                    free(sig);
                    brace_depth++;
                    i++;
                    continue;
                }

                /* Remove trailing attributes and clean params */
                remove_trailing_attributes(sig);
                size_t lidx = (size_t)(lparen - sig);
                size_t ridx = (size_t)(rparen - sig);
                size_t plen = ridx - lidx + 1;
                if (plen == 0 || plen > MAX_PARAM_CLEAN_LEN) { free(sig); brace_depth++; i++; continue; }

                char *params = xcalloc(plen + 1, 1);
                memcpy(params, sig + lidx, plen);
                params[plen] = '\0';
                char *clean_params = strip_parameter_names(params);

                /* Build prototype */
                size_t prelen = lidx;
                size_t proto_cap = prelen + strlen(clean_params) + 8;
                char *proto = xcalloc(proto_cap, 1);
                memcpy(proto, sig, prelen);
                proto[prelen] = '\0';
                trim_inplace(proto);
                remove_trailing_attributes(proto);
                safe_strcat(proto, proto_cap, clean_params);
                safe_strcat(proto, proto_cap, ";");

                /* Write prototype */
                fprintf(out, "%s\n\n", proto);

                /* Cleanup */
                free(params);
                free(clean_params);
                free(proto);
                free(sig);

                /* Now treat this '{' as entering a block */
                brace_depth++;
                i++;
                continue;
            } else {
                /* inner '{' inside a function or other block */
                brace_depth++;
                i++;
                continue;
            }
        } else if (c == '}') {
            if (brace_depth > 0) brace_depth--;
            i++;
            continue;
        } else {
            /* normal character */
            i++;
            continue;
        }
    }
}

/* Header helpers */
static void write_header_preamble(FILE *out, const char *outname, const char *file_list) {
    char guard[256]; size_t gj = 0;
    for (size_t i = 0; outname[i] && gj + 2 < sizeof(guard); ++i) {
        unsigned char c = (unsigned char)outname[i];
        if (isalnum(c)) guard[gj++] = toupper(c); else guard[gj++] = '_';
    }
    if (gj + 3 < sizeof(guard)) { guard[gj++] = '_'; guard[gj++] = 'H'; guard[gj] = '\0'; } else guard[gj - 1] = '\0';
    fprintf(out, "#ifndef %s\n#define %s\n\n", guard, guard);
    fprintf(out, "/* Prototypes extracted by genhdr_readable_fixed */\n");
    if (file_list && file_list[0]) fprintf(out, "/* Source files: %s */\n\n", file_list); else fprintf(out, "\n");
    fprintf(out, "#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n");
}
static void write_header_postamble(FILE *out, const char *outname) {
    char guard[256]; size_t gj = 0;
    for (size_t i = 0; outname[i] && gj + 2 < sizeof(guard); ++i) {
        unsigned char c = (unsigned char)outname[i];
        if (isalnum(c)) guard[gj++] = toupper(c); else guard[gj++] = '_';
    }
    if (gj + 3 < sizeof(guard)) { guard[gj++] = '_'; guard[gj++] = 'H'; guard[gj] = '\0'; } else guard[gj - 1] = '\0';
    fprintf(out, "#ifdef __cplusplus\n}\n#endif\n\n#endif /* %s */\n", guard);
}

/* Main */
int main(int argc, char **argv) {
    const char *prog = argc > 0 ? argv[0] : "genhdr_readable_fixed";
    const char *outname = DEFAULT_OUTPUT;
    int argi = 1;
    if (argc < 2) usage_and_exit(prog);
    if (strcmp(argv[1], "-o") == 0) {
        if (argc < 4) usage_and_exit(prog);
        outname = argv[2];
        argi = 3;
    }
    if (argi >= argc) usage_and_exit(prog);

    char file_list[1024]; file_list[0] = '\0';
    for (int i = argi; i < argc; ++i) {
        if (i > argi) strncat(file_list, ", ", sizeof(file_list) - strlen(file_list) - 1);
        strncat(file_list, argv[i], sizeof(file_list) - strlen(file_list) - 1);
    }

    FILE *out = fopen(outname, "w");
    if (!out) { fprintf(stderr, "Cannot open output file %s: %s\n", outname, strerror(errno)); return EXIT_FAILURE; }

    write_header_preamble(out, outname, file_list);

    for (int i = argi; i < argc; ++i) {
        const char *srcpath = argv[i];
        size_t len = 0;
        char *raw = read_entire_file(srcpath, &len);
        if (!raw) { fprintf(stderr, "Warning: cannot read %s\n", srcpath); continue; }
        char *clean = strip_comments_and_strings(raw);
        free(raw);
        if (!clean) { fprintf(stderr, "Memory error while processing %s\n", srcpath); fclose(out); return EXIT_FAILURE; }
        extract_prototypes_from_clean_buffer(clean, out, srcpath);
        free(clean);
    }

    write_header_postamble(out, outname);
    fclose(out);
    return EXIT_SUCCESS;
}
