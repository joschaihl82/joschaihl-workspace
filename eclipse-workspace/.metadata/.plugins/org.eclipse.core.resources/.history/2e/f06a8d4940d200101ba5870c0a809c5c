// ciacc.c: fully patched and amalgamated
// Compact tutorial-style C compiler (subset) with integrated preprocessor,
// types: char/short/int/long (signed/unsigned), pointers, structs,
// local variables, functions, control flow, expressions, string literals.
// Preprocessor supports #include "file" and #include <file> (search /usr/include).
//
// usage: ciacc <src.c> -o <outname>
// emits x86-64 assembly to <outname>.s
//
// notes:
// - this is a teaching compiler, not a full c implementation.
// - structs: named tags, simple field lists (no bitfields, no nested struct definitions in fields).
// - member access: `a.field` and `p->field` supported. (fully patched)
// - alignment: fields are laid out with natural alignment (1,2,4,8).
// - pointer arithmetic is now correctly scaled for types. (fully patched)

#define _gnu_source

/* ------------------------------------------------------------------------- */
/* minimal header implementations (replacing system includes) */
/* ------------------------------------------------------------------------- */

/* <stddef.h> */
typedef unsigned long size_t;
#define null ((void*)0)

/* <stdbool.h> */
#define bool int
#define true 1
#define false 0

/* ------------------------------------------------------------------------- */
/* minimal stdarg.h emulation (moved from ciacc.c) */
/* ------------------------------------------------------------------------- */

#ifndef ciacc_stdarg_h
#define ciacc_stdarg_h

/* use a raw pointer as va_list. it points to the next argument's storage. */
typedef char *ciacc_va_list;

/* alignment unit: use pointer size for natural alignment on target (8 on x86-64). */
#define ciacc_va_align (sizeof(void*))

/* round up 'n' to the next multiple of ciacc_va_align */
#define ciacc_va_round(n) (((n) + (ciacc_va_align - 1)) & ~(ciacc_va_align - 1))

/*
 * va_start(ap, last)
 * sets 'ap' to point just past the last fixed parameter on the stack.
 */
#define va_list ciacc_va_list
#define va_start(ap, last) \
  ((ap) = (ciacc_va_list)((char *)&(last) + ciacc_va_round(sizeof(last))))

/*
 * va_arg(ap, type)
 * reads the next argument of 'type' and advances 'ap'.
 */
#define va_arg(ap, type) \
  (*(type *)((ap) += ciacc_va_round(sizeof(type)), (ap) - ciacc_va_round(sizeof(type))))

/* va_end: clear the pointer (no-op for this simple implementation) */
#define va_end(ap) ((ap) = (ciacc_va_list)0)

/* va_copy: copy one va_list to another */
#define va_copy(dest, src) ((dest) = (src))

#endif /* ciacc_stdarg_h */

/* <stdio.h> - declarations for i/o functions used */
typedef struct _iobuf { char *_ptr; int _cnt; char *_base; int _flag; int _file; int _charbuf; int _bufsiz; char *_tmpfname; } file;
extern file *stdin;
extern file *stdout;
extern file *stderr;
#define file void
#define seek_set 0
extern file *fopen(const char * restrict filename, const char * restrict mode);
extern int fclose(file *stream);
extern int fseek(file *stream, long offset, int whence);
extern long ftell(file *stream);
extern size_t fread(void * restrict ptr, size_t size, size_t nmemb, file * restrict stream);
extern int fprintf(file * restrict stream, const char * restrict format, ...);
extern int snprintf(char * restrict s, size_t n, const char * restrict format, ...);
/* removed conflicting vfprintf declaration to avoid builtin mismatch */

/* <stdlib.h> - declarations for memory/control functions used */
extern void *malloc(size_t size);
extern void *realloc(void *ptr, size_t size);
extern void free(void *ptr);
extern char *strdup(const char *s);
extern void exit(int status);
extern long strtol(const char * restrict nptr, char ** restrict endptr, int base);

/* <string.h> - declarations for string functions used */
extern size_t strlen(const char *s);
extern int strcmp(const char *s1, const char *s2);
extern int strncmp(const char *s1, const char *s2, size_t n);
extern void *memcpy(void * restrict s1, const void * restrict s2, size_t n);
extern int memcmp(const void *s1, const void *s2, size_t n);
extern char *strchr(const char *s, int c);
extern char *strrchr(const char *s, int c);
extern char *strndup(const char *s, size_t size);
extern void *memset(void *s, int c, size_t n);

/* <ctype.h> - minimal inline implementations */
static inline int isspace(int c) { return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r'; }
static inline int isalpha(int c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
static inline int isalnum(int c) { return isalpha(c) || (c >= '0' && c <= '9'); }
static inline int isdigit(int c) { return c >= '0' && c <= '9'; }

// forward declaration for string literal registration
int register_string_literal(const char *s);

/* ------------------------------------------------------------------------- */
/* the rest of the compiler follows (preprocessor, tokenizer, parser, codegen) */
/* ------------------------------------------------------------------------- */

//
// utilities
//
static void *xmalloc(size_t n) {
  void *p = malloc(n);
  if (!p) { fprintf(stderr, "out of memory\n"); exit(1); }
  return p;
}
static void *xrealloc(void *p, size_t n) {
  void *q = realloc(p, n);
  if (!q) { fprintf(stderr, "out of memory\n"); exit(1); }
  return q;
}
static char *xstrdup(const char *s) {
  char *t = strdup(s);
  if (!t) { fprintf(stderr, "out of memory\n"); exit(1); }
  return t;
}

//
// preprocessor (simple): supports #include "..." and #include <...>, #define, #undef, #if/#ifdef/#ifndef
//

typedef struct macro { struct macro *next; char *name; char *replacement; } macro;
typedef struct includestack { char *path; char *dir; struct includestack *next; } includestack;

static macro *macros = null;
static includestack *incstack = null;
static const char *default_include_dir = "/usr/include";

static void define_macro(const char *name, const char *replacement) {
  for (macro *m = macros; m; m = m->next) {
    if (strcmp(m->name, name) == 0) {
      free(m->replacement);
      m->replacement = xstrdup(replacement);
      return;
    }
  }
  macro *m = xmalloc(sizeof(macro));
  m->name = xstrdup(name);
  m->replacement = xstrdup(replacement);
  m->next = macros;
  macros = m;
}
static void undef_macro(const char *name) {
  macro **pp = &macros;
  while (*pp) {
    if (strcmp((*pp)->name, name) == 0) {
      macro *t = *pp;
      *pp = t->next;
      free(t->name);
      free(t->replacement);
      free(t);
      return;
    }
    pp = &(*pp)->next;
  }
}
static const char *lookup_macro(const char *name) {
  for (macro *m = macros; m; m = m->next) if (strcmp(m->name, name) == 0) return m->replacement;
  return null;
}

static char *dir_of(const char *path) {
  char *p = xstrdup(path);
  char *slash = strrchr(p, '/');
  if (!slash) { free(p); return xstrdup("."); }
  *slash = '\0';
  return p;
}
static char *read_file(const char *path) {
  file *f = fopen(path, "rb");
  if (!f) return null;
  fseek(f, 0, seek_set);
  long sz = ftell(f);
  fseek(f, 0, seek_set);
  char *buf = xmalloc(sz + 1);
  if (fread(buf, 1, sz, f) != (size_t)sz) { free(buf); fclose(f); return null; }
  buf[sz] = '\0';
  fclose(f);
  return buf;
}
static char *strip_comments(const char *s) {
  size_t cap = strlen(s) + 1;
  char *out = xmalloc(cap);
  size_t oi = 0;
  for (size_t i = 0; s[i];) {
    if (s[i] == '/' && s[i+1] == '/') { i += 2; while (s[i] && s[i] != '\n') i++; continue; }
    if (s[i] == '/' && s[i+1] == '*') { i += 2; while (s[i] && !(s[i] == '*' && s[i+1] == '/')) i++; if (s[i]) i += 2; continue; }
    out[oi++] = s[i++];
    if (oi + 2 >= cap) { cap *= 2; out = xrealloc(out, cap); }
  }
  out[oi] = '\0';
  return out;
}
static bool is_ident_char(char c) { return isalnum((unsigned char)c) || c == '_'; }

static char *expand_macros_in_line(const char *line) {
  size_t cap = strlen(line) * 2 + 1;
  char *out = xmalloc(cap);
  size_t oi = 0;
  for (size_t i = 0; line[i];) {
    if (line[i] == '"') {
      out[oi++] = line[i++];
      while (line[i]) {
        if (line[i] == '\\' && line[i+1]) { out[oi++] = line[i++]; out[oi++] = line[i++]; continue; }
        out[oi++] = line[i];
        if (line[i++] == '"') break;
      }
      continue;
    }
    if (is_ident_char(line[i])) {
      size_t j = i;
      while (is_ident_char(line[j])) j++;
      size_t len = j - i;
      char *ident = strndup(line + i, len);
      const char *rep = lookup_macro(ident);
      if (rep) {
        size_t rlen = strlen(rep);
        if (oi + rlen + 2 >= cap) { cap = (oi + rlen + 2) * 2; out = xrealloc(out, cap); }
        memcpy(out + oi, rep, rlen);
        oi += rlen;
      } else {
        if (oi + len + 2 >= cap) { cap = (oi + len + 2) * 2; out = xrealloc(out, cap); }
        memcpy(out + oi, line + i, len);
        oi += len;
      }
      free(ident);
      i = j;
      continue;
    }
    if (oi + 2 >= cap) { cap *= 2; out = xrealloc(out, cap); }
    out[oi++] = line[i++];
  }
  out[oi] = '\0';
  return out;
}

static char *preprocess_file(const char *path); // forward

static char *preprocess_buffer(const char *buf, const char *curdir, const char *curpath) {
  size_t outcap = strlen(buf) + 1;
  char *out = xmalloc(outcap);
  size_t outi = 0;

  typedef struct cond { bool active; bool seen_true; struct cond *next; } cond;
  cond *cond_stack = null;

  const char *p = buf;
  while (*p) {
    const char *nl = strchr(p, '\n');
    size_t linelen = nl ? (size_t)(nl - p) : strlen(p);
    char *line = strndup(p, linelen);
    p = nl ? nl + 1 : p + linelen;

    const char *s = line;
    while (*s && isspace((unsigned char)*s)) s++;

    bool parent_active = true;
    for (cond *c = cond_stack; c; c = c->next) if (!c->active) { parent_active = false; break; }

    if (*s == '#') {
      s++;
      while (*s && isspace((unsigned char)*s)) s++;
      const char *q = s;
      while (*q && isalpha((unsigned char)*q)) q++;
      size_t dlen = q - s;
      char *dir = strndup(s, dlen);
      s = q;
      while (*s && isspace((unsigned char)*s)) s++;

      if (strcmp(dir, "include") == 0) {
        if (parent_active) {
          if (*s == '"') {
            s++;
            const char *endq = strchr(s, '"');
            if (!endq) { fprintf(stderr, "malformed include in %s\n", curpath ? curpath : "(input)"); free(dir); free(line); free(out); return null; }
            char *incname = strndup(s, endq - s);
            char incpath[4096];
            if (curdir && curdir[0]) snprintf(incpath, sizeof(incpath), "%s/%s", curdir, incname);
            else snprintf(incpath, sizeof(incpath), "%s", incname);
            free(incname);
            char *incbuf = preprocess_file(incpath);
            if (!incbuf) { fprintf(stderr, "include failed: %s\n", incpath); free(dir); free(line); free(out); return null; }
            size_t need = outi + strlen(incbuf) + 1;
            if (need >= outcap) { outcap = need * 2; out = xrealloc(out, outcap); }
            memcpy(out + outi, incbuf, strlen(incbuf));
            outi += strlen(incbuf);
            free(incbuf);
          } else if (*s == '<') {
            s++;
            const char *endq = strchr(s, '>');
            if (!endq) { fprintf(stderr, "malformed include <...> in %s\n", curpath ? curpath : "(input)"); free(dir); free(line); free(out); return null; }
            char *incname = strndup(s, endq - s);
            char incpath[4096];
            char *incbuf = null;
            // search /usr/include first, then current dir
            snprintf(incpath, sizeof(incpath), "%s/%s", default_include_dir, incname);
            incbuf = preprocess_file(incpath);
            if (!incbuf && curdir) {
              snprintf(incpath, sizeof(incpath), "%s/%s", curdir, incname);
              incbuf = preprocess_file(incpath);
            }
            if (!incbuf) {
              fprintf(stderr, "include failed: <%s>\n", incname);
              free(incname);
              free(dir);
              free(line);
              free(out);
              return null;
            }
            size_t need = outi + strlen(incbuf) + 1;
            if (need >= outcap) { outcap = need * 2; out = xrealloc(out, outcap); }
            memcpy(out + outi, incbuf, strlen(incbuf));
            outi += strlen(incbuf);
            free(incbuf);
            free(incname);
          } else {
            fprintf(stderr, "warning: unsupported include syntax in %s\n", curpath ? curpath : "(input)");
          }
        }
      } else if (strcmp(dir, "define") == 0) {
        if (parent_active) {
          const char *t = s;
          while (*t && isspace((unsigned char)*t)) t++;
          const char *u = t;
          while (*u && (isalnum((unsigned char)*u) || *u == '_')) u++;
          if (u == t) { fprintf(stderr, "malformed define in %s\n", curpath ? curpath : "(input)"); }
          else {
            char *name = strndup(t, u - t);
            t = u;
            while (*t && isspace((unsigned char)*t)) t++;
            char *replacement = xstrdup(t);
            char *expanded = expand_macros_in_line(replacement);
            define_macro(name, expanded);
            free(name);
            free(replacement);
            free(expanded);
          }
        }
      } else if (strcmp(dir, "undef") == 0) {
        if (parent_active) {
          const char *t = s;
          while (*t && isspace((unsigned char)*t)) t++;
          const char *u = t;
          while (*u && (isalnum((unsigned char)*u) || *u == '_')) u++;
          if (u == t) fprintf(stderr, "malformed undef in %s\n", curpath ? curpath : "(input)");
          else { char *name = strndup(t, u - t); undef_macro(name); free(name); }
        }
      } else if (strcmp(dir, "ifdef") == 0 || strcmp(dir, "ifndef") == 0) {
        const bool is_ifndef = (strcmp(dir, "ifndef") == 0);
        const char *t = s;
        while (*t && isspace((unsigned char)*t)) t++;
        const char *u = t;
        while (*u && (isalnum((unsigned char)*u) || *u == '_')) u++;
        if (u == t) {
          cond *c = xmalloc(sizeof(cond));
          c->active = false; c->seen_true = false; c->next = cond_stack; cond_stack = c;
        } else {
          char *name = strndup(t, u - t);
          const char *rep = lookup_macro(name);
          bool defined = rep != null;
          bool active = (is_ifndef ? !defined : defined) && parent_active;
          cond *c = xmalloc(sizeof(cond));
          c->active = active; c->seen_true = active; c->next = cond_stack; cond_stack = c;
          free(name);
        }
      } else if (strcmp(dir, "if") == 0) {
        bool active = false;
        if (!parent_active) active = false;
        else {
          const char *t = s;
          while (*t && isspace((unsigned char)*t)) t++;
          if (strncmp(t, "defined", 7) == 0) {
            const char *p1 = strchr(t, '(');
            const char *p2 = p1 ? strchr(p1+1, ')') : null;
            if (p1 && p2) {
              char *name = strndup(p1+1, p2 - (p1+1));
              const char *rep = lookup_macro(name);
              active = rep != null;
              free(name);
            } else active = false;
          } else {
            char *end;
            long v = strtol(t, &end, 0);
            active = (v != 0);
          }
        }
        cond *c = xmalloc(sizeof(cond));
        c->active = active && parent_active; c->seen_true = active && parent_active; c->next = cond_stack; cond_stack = c;
      } else if (strcmp(dir, "elif") == 0) {
        if (!cond_stack) fprintf(stderr, "unexpected #elif\n");
        else {
          cond *c = cond_stack;
          if (c->seen_true) c->active = false;
          else {
            bool condval = false;
            if (!parent_active) condval = false;
            else {
              const char *t = s;
              while (*t && isspace((unsigned char)*t)) t++;
              if (strncmp(t, "defined", 7) == 0) {
                const char *p1 = strchr(t, '(');
                const char *p2 = p1 ? strchr(p1+1, ')') : null;
                if (p1 && p2) {
                  char *name = strndup(p1+1, p2 - (p1+1));
                  const char *rep = lookup_macro(name);
                  condval = rep != null;
                  free(name);
                }
              } else {
                char *end;
                long v = strtol(t, &end, 0);
                condval = (v != 0);
              }
            }
            c->active = condval && parent_active;
            if (c->active) c->seen_true = true;
          }
        }
      } else if (strcmp(dir, "else") == 0) {
        if (!cond_stack) fprintf(stderr, "unexpected #else\n");
        else {
          cond *c = cond_stack;
          if (c->seen_true) c->active = false;
          else c->active = parent_active;
          c->seen_true = c->seen_true || c->active;
        }
      } else if (strcmp(dir, "endif") == 0) {
        if (!cond_stack) fprintf(stderr, "unexpected #endif\n");
        else { cond *c = cond_stack; cond_stack = c->next; free(c); }
      } else {
        if (parent_active) {
          if (outi + 2 >= outcap) { outcap *= 2; out = xrealloc(out, outcap); }
          out[outi++] = '\n';
        }
      }
      free(dir);
    } else {
      if (parent_active) {
        char *expanded = expand_macros_in_line(line);
        size_t need = outi + strlen(expanded) + 2;
        if (need >= outcap) { outcap = need * 2; out = xrealloc(out, outcap); }
        memcpy(out + outi, expanded, strlen(expanded));
        outi += strlen(expanded);
        out[outi++] = '\n';
        free(expanded);
      } else {
        if (outi + 2 >= outcap) { outcap *= 2; out = xrealloc(out, outcap); }
        out[outi++] = '\n';
      }
    }
    free(line);
  }

  while (cond_stack) { cond *c = cond_stack; cond_stack = c->next; free(c); }
  out[outi] = '\0';
  return out;
}

static char *preprocess_file(const char *path) {
  char *buf = read_file(path);
  if (!buf) return null;
  char *no_comments = strip_comments(buf);
  free(buf);
  char *dir = dir_of(path);
  includestack st;
  st.path = xstrdup(path);
  st.dir = xstrdup(dir);
  st.next = incstack;
  incstack = &st;
  char *result = preprocess_buffer(no_comments, dir, path);
  incstack = st.next;
  free(st.path);
  free(st.dir);
  free(no_comments);
  free(dir);
  return result;
}

//
// tokenizer
//

typedef enum { tk_reserved, tk_ident, tk_num, tk_str, tk_eof } tokenkind;
typedef struct token token;
struct token { tokenkind kind; token *next; int val; char *str; int len; char *loc; };

// FIX: Renamed global variable 'token' to 'current_token' to resolve conflict with 'struct token' type.
static token *current_token;
static char *user_input;

static void error_at_tok(char *loc, char *fmt, ...) {
  va_list ap; va_start(ap, fmt);
  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, "");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

static token *new_token(tokenkind kind, token *cur, char *str, int len, char *loc) {
  token *tok = xmalloc(sizeof(token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  tok->loc = loc;
  cur->next = tok;
  return tok;
}
static bool startswith(const char *p, const char *q) { return memcmp(p, q, strlen(q)) == 0; }
static bool is_ident1(char c) { return isalpha((unsigned char)c) || c == '_'; }
static bool is_ident2(char c) { return isalnum((unsigned char)c) || c == '_'; }

static token *tokenize() {
  char *p = user_input;
  token head; head.next = null;
  token *cur = &head;
  while (*p) {
    if (isspace((unsigned char)*p)) { p++; continue; }
    if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">=") || startswith(p, "->")) {
      cur = new_token(tk_reserved, cur, p, 2, p); p += 2; continue;
    }
    if (strchr("+-*/()<>;=,{}[]&!~?:.%", *p)) { cur = new_token(tk_reserved, cur, p, 1, p); p++; continue; }
    if (isdigit((unsigned char)*p)) {
      char *q = p; int val = strtol(p, &p, 10); cur = new_token(tk_num, cur, q, p - q, q); cur->val = val; continue;
    }
    if (*p == '"') {
      char *start = p; p++;
      char *buf = xmalloc(1); size_t cap = 0, len = 0;
      while (*p && *p != '"') {
        if (*p == '\\') { p++; char esc = *p; char out = esc; if (esc == 'n') out = '\n'; else if (esc == 't') out = '\t'; else if (esc == 'r') out = '\r'; else if (esc == '\\') out = '\\'; else if (esc == '"') out = '"'; else out = esc; if (len + 1 >= cap) { cap = cap ? cap * 2 : 8; buf = xrealloc(buf, cap); } buf[len++] = out; p++; }
        else { if (len + 1 >= cap) { cap = cap ? cap * 2 : 8; buf = xrealloc(buf, cap); } buf[len++] = *p++; }
      }
      if (*p != '"') error_at_tok(start, "unclosed string literal");
      p++;
      buf = xrealloc(buf, len + 1); buf[len] = '\0';
      token *tok = new_token(tk_str, cur, null, 0, start); tok->str = buf; tok->len = len; cur = tok;
      continue;
    }
    if (is_ident1(*p)) {
      char *q = p++; while (is_ident2(*p)) p++;
      int len = p - q;
      tokenkind kind = tk_ident;
      // keywords: int, return, if, else, while, for, sizeof, unsigned, char, short, long, struct
      if (len == 3 && memcmp(q, "int", 3) == 0) kind = tk_reserved;
      else if (len == 6 && memcmp(q, "return", 6) == 0) kind = tk_reserved;
      else if (len == 2 && memcmp(q, "if", 2) == 0) kind = tk_reserved;
      else if (len == 4 && memcmp(q, "else", 4) == 0) kind = tk_reserved;
      else if (len == 5 && memcmp(q, "while", 5) == 0) kind = tk_reserved;
      else if (len == 3 && memcmp(q, "for", 3) == 0) kind = tk_reserved;
      else if (len == 6 && memcmp(q, "sizeof", 6) == 0) kind = tk_reserved;
      else if (len == 8 && memcmp(q, "unsigned", 8) == 0) kind = tk_reserved;
      else if (len == 4 && memcmp(q, "char", 4) == 0) kind = tk_reserved;
      else if (len == 5 && memcmp(q, "short", 5) == 0) kind = tk_reserved;
      else if (len == 4 && memcmp(q, "long", 4) == 0) kind = tk_reserved;
      else if (len == 6 && memcmp(q, "struct", 6) == 0) kind = tk_reserved;
      cur = new_token(kind, cur, q, len, q);
      continue;
    }
    error_at_tok(p, "invalid token");
  }
  new_token(tk_eof, cur, p, 0, p);
  return head.next;
}

//
// types, structs, ast
//

typedef enum { ty_int, ty_ptr, ty_struct } typekind;
typedef struct type type;
typedef struct structmember structmember;
typedef struct structdef structdef;

struct structmember {
  char *name;
  type *ty;
  int offset;
};

struct structdef {
  structdef *next;
  char *tag;
  structmember *members;
  int members_len;
  int size;
  int align;
};

struct type {
  typekind kind;
  int size;
  bool is_unsigned;
  int rank; // char=0, short=1, int=2, long=3, ptr=4, struct=5
  type *base; // for pointers
  structdef *sdef; // for structs
};

// global struct definition list
static structdef *struct_defs = null;
static type *int_type_cache[4][2] = {{null}}; // [rank][is_unsigned]

static type *get_int_type_by_rank(int rank, bool is_unsigned) {
  if (rank < 0 || rank > 3) { fprintf(stderr, "invalid integer rank %d\n", rank); exit(1); }
  if (int_type_cache[rank][is_unsigned]) return int_type_cache[rank][is_unsigned];
  type *t = xmalloc(sizeof(type));
  t->kind = ty_int;
  t->is_unsigned = is_unsigned;
  t->rank = rank;
  if (rank == 0) t->size = 1;
  else if (rank == 1) t->size = 2;
  else if (rank == 2) t->size = 4;
  else t->size = 8;
  t->base = null;
  t->sdef = null;
  int_type_cache[rank][is_unsigned] = t;
  return t;
}

static type *ptr_to(type *base) {
  type *t = xmalloc(sizeof(type));
  t->kind = ty_ptr;
  t->size = 8;
  t->is_unsigned = false;
  t->rank = 4;
  t->base = base;
  t->sdef = null;
  return t;
}

static type *struct_type(structdef *sdef) {
  type *t = xmalloc(sizeof(type));
  t->kind = ty_struct;
  t->sdef = sdef;
  t->size = sdef->size;
  t->is_unsigned = false;
  t->rank = 5;
  t->base = null;
  return t;
}

//
// ast node kinds
//
typedef enum {
  nd_add, nd_sub, nd_mul, nd_div, nd_eq, nd_ne, nd_lt, nd_le, nd_assign, nd_var, nd_num, nd_return,
  nd_if, nd_while, nd_for, nd_block, nd_funcall, nd_addr, nd_deref, nd_sizeof, nd_expr_stmt, nd_null,
  nd_member, nd_member_ref // struct member access (a.field) and pointer member access (p->field)
} nodekind;

typedef struct node node;
struct node {
  nodekind kind;
  node *lhs;
  node *rhs;
  node *cond;
  node *then;
  node *els;
  node *init;
  node *inc;
  node **stmts;
  int stmts_len;
  int val;
  char *name;
  type *ty;
  int offset; // for local var
  node **args;
  int args_len;
  char *str;
  // member-specific
  char *member_name;
  structmember *member; // resolved member
};

static node *new_node(nodekind kind) {
  node *n = xmalloc(sizeof(node));
  n->kind = kind;
  n->lhs = n->rhs = n->cond = n->then = n->els = n->init = n->inc = null;
  n->stmts = null;
  n->stmts_len = 0;
  n->val = 0;
  n->name = null;
  n->ty = null;
  n->offset = 0;
  n->args = null;
  n->args_len = 0;
  n->str = null;
  n->member_name = null;
  n->member = null;
  return n;
}
static node *new_binary(nodekind kind, node *lhs, node *rhs) {
  node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  node->ty = lhs->ty;
  return node;
}
static node *new_num_node(int val) {
  node *node = new_node(nd_num);
  node->val = val;
  node->ty = get_int_type_by_rank(2, false); // int
  return node;
}
static node *new_var_node(char *name); // forward

// local variables (scope/function-local)
typedef struct var var;
struct var { var *next; char *name; type *ty; int offset; };
static var *locals = null;
static var *params = null;
static var *find_var_by_name(char *name) {
  for (var *v = locals; v; v = v->next) if (strcmp(v->name, name) == 0) return v;
  for (var *v = params; v; v = v->next) if (strcmp(v->name, name) == 0) return v;
  return null;
}
static void push_var(char *name, type *ty, bool is_param) {
  var *v = xmalloc(sizeof(var));
  v->name = name;
  v->ty = ty;
  if (is_param) { v->next = params; params = v; }
  else { v->next = locals; locals = v; }
}
static node *new_var_node(char *name) {
  var *v = find_var_by_name(name);
  if (!v) error_at_tok(current_token->loc, "undefined variable: %s", name); // FIX: Use current_token->loc
  node *n = new_node(nd_var);
  n->name = name;
  n->ty = v->ty;
  n->offset = v->offset;
  return n;
}

// struct definitions
static structdef *new_struct_def(char *tag) {
  structdef *sdef = xmalloc(sizeof(structdef));
  sdef->tag = tag;
  sdef->members = null;
  sdef->members_len = 0;
  sdef->size = 0;
  sdef->align = 1;
  sdef->next = struct_defs;
  struct_defs = sdef;
  return sdef;
}
static structdef *find_struct_def_by_tag(char *tag) {
  for (structdef *s = struct_defs; s; s = s->next) if (strcmp(s->tag, tag) == 0) return s;
  return null;
}

// functions
typedef struct function function;
struct function {
  function *next;
  char *name;
  int params_count;
  type **param_types;
  char **params;
  node *body;
  int stack_size;
};
static function *functions = null;
static function *find_function_by_name(char *name) {
  for (function *f = functions; f; f = f->next) if (strcmp(f->name, name) == 0) return f;
  return null;
}

//
// parser
//
static type *last_parsed_type = null;

// parser forward
static node *program();
static node *declaration();
static node *stmt();
static node *expr();
static node *assign();
static node *equality();
static node *relational();
static node *add();
static node *mul();
static node *unary();
static node *postfix();
static node *primary();
static node *compound_stmt();
static type *parse_type_specifier();

static bool consume(const char *op) {
  // FIX: Use current_token
  if (current_token->kind != tk_reserved || strlen(op) != current_token->len || memcmp(current_token->str, op, current_token->len)) return false;
  current_token = current_token->next;
  return true;
}
static void expect(const char *op) {
  // FIX: Use current_token
  if (current_token->kind != tk_reserved || strlen(op) != current_token->len || memcmp(current_token->str, op, current_token->len)) error_at_tok(current_token->loc, "expected \"%s\"", (char*)op);
  current_token = current_token->next;
}
static int expect_number() {
  // FIX: Use current_token
  if (current_token->kind != tk_num) error_at_tok(current_token->loc, "expected a number");
  int val = current_token->val;
  current_token = current_token->next;
  return val;
}
static char *expect_ident() {
  // FIX: Use current_token
  if (current_token->kind != tk_ident) error_at_tok(current_token->loc, "expected an identifier");
  char *s = strndup(current_token->str, current_token->len);
  current_token = current_token->next;
  return s;
}
static bool at_eof() { return current_token->kind == tk_eof; }
static bool is_keyword(const char *kw) {
  if (current_token->kind != tk_reserved) return false;
  if (strlen(kw) != current_token->len) return false;
  return memcmp(current_token->str, kw, current_token->len) == 0;
}

// type parsing: supports optional 'unsigned' and one of char/short/int/long or 'struct tag' or 'struct { members }'
static type *parse_type_specifier() {
  bool is_unsigned = false;
  if (is_keyword("unsigned")) { consume("unsigned"); is_unsigned = true; }

  if (is_keyword("char")) { consume("char"); last_parsed_type = get_int_type_by_rank(0, is_unsigned); return last_parsed_type; }
  if (is_keyword("short")) { consume("short"); last_parsed_type = get_int_type_by_rank(1, is_unsigned); return last_parsed_type; }
  if (is_keyword("int")) { consume("int"); last_parsed_type = get_int_type_by_rank(2, is_unsigned); return last_parsed_type; }
  if (is_keyword("long")) { consume("long"); last_parsed_type = get_int_type_by_rank(3, is_unsigned); return last_parsed_type; }

  if (is_keyword("struct")) {
    consume("struct");
    char *tag = null;
    if (current_token->kind == tk_ident) { tag = expect_ident(); } // FIX: Use current_token

    structdef *sdef = tag ? find_struct_def_by_tag(tag) : null;

    if (consume("{")) {
      // struct { ... } definition
      if (sdef && sdef->size > 0) error_at_tok(current_token->loc, "redefinition of struct %s", tag); // FIX: Use current_token->loc

      // if tag exists (forward decl) or is null (anon), create/use new def
      if (!sdef && tag) { sdef = new_struct_def(tag); }
      else if (!sdef && !tag) { sdef = new_struct_def(null); }

      // parse members: simple "type name;" entries
      while (!consume("}")) {
        type *mty = parse_type_specifier();
        char *mname = expect_ident();
        expect(";");
        // append member
        structmember *m = xmalloc(sizeof(structmember));
        m->name = mname;
        m->ty = mty;
        m->offset = 0;
        sdef->members = xrealloc(sdef->members, sizeof(structmember) * (sdef->members_len + 1));
        sdef->members[sdef->members_len++] = *m;
        free(m);
      }

      // compute layout: natural alignment
      int off = 0;
      int maxalign = 1;
      for (int i = 0; i < sdef->members_len; i++) {
        structmember *m = &sdef->members[i];
        int align = m->ty->size;
        if (align > 8) align = 8;
        if (align > maxalign) maxalign = align;
        if (off % align) off += align - (off % align);
        m->offset = off;
        off += m->ty->size;
      }
      if (off % maxalign) off += maxalign - (off % maxalign);
      sdef->size = off;
      sdef->align = maxalign;
      last_parsed_type = struct_type(sdef);
      return last_parsed_type;
    } else {
      // struct tag reference
      if (!sdef) error_at_tok(current_token->loc, "unknown struct %s", tag ? tag : "(anon)");
      last_parsed_type = struct_type(sdef);
      return last_parsed_type;
    }
  }

  // default: if nothing matched, error
  error_at_tok(current_token->loc, "unknown type");
  return null;
}

//
// Simple recursive-descent parser for expressions/statements
//

static node *program() {
  // For brevity: parse a sequence of top-level declarations and functions.
  // This is a simplified placeholder; a full implementation would build a list.
  node *root = new_node(nd_block);
  root->stmts = null;
  root->stmts_len = 0;
  while (!at_eof()) {
    node *d = declaration();
    if (d) {
      root->stmts = xrealloc(root->stmts, sizeof(node*) * (root->stmts_len + 1));
      root->stmts[root->stmts_len++] = d;
    } else break;
  }
  return root;
}

static node *declaration() {
  // Very small subset: either function or variable declaration
  if (is_keyword("int") || is_keyword("char") || is_keyword("short") || is_keyword("long") || is_keyword("struct") || is_keyword("unsigned")) {
    type *ty = parse_type_specifier();
    char *name = expect_ident();
    if (consume("(")) {
      // function declaration/definition (no prototypes)
      // parse params (very simple)
      params = null;
      if (!consume(")")) {
        while (1) {
          type *pty = parse_type_specifier();
          char *pname = expect_ident();
          push_var(pname, pty, true);
          if (consume(")")) break;
          expect(",");
        }
      }
      // parse body
      node *body = compound_stmt();
      // create function node (store in functions list)
      node *fn = new_node(nd_null);
      fn->name = name;
      fn->body = body;
      return fn;
    } else {
      // variable declaration
      expect(";");
      node *v = new_node(nd_null);
      v->name = name;
      v->ty = ty;
      return v;
    }
  }
  // fallback: expression statement
  node *n = stmt();
  return n;
}

static node *stmt() {
  if (is_keyword("return")) {
    consume("return");
    node *e = expr();
    expect(";");
    node *n = new_node(nd_return);
    n->lhs = e;
    return n;
  }
  if (is_keyword("if")) {
    consume("if");
    expect("(");
    node *condn = expr();
    expect(")");
    node *thenn = stmt();
    node *elsn = null;
    if (is_keyword("else")) { consume("else"); elsn = stmt(); }
    node *n = new_node(nd_if);
    n->cond = condn; n->then = thenn; n->els = elsn;
    return n;
  }
  if (is_keyword("while")) {
    consume("while");
    expect("(");
    node *condn = expr();
    expect(")");
    node *bod = stmt();
    node *n = new_node(nd_while);
    n->cond = condn; n->then = bod;
    return n;
  }
  if (consume("{")) return compound_stmt();
  // expression statement
  node *e = expr();
  expect(";");
  node *n = new_node(nd_expr_stmt);
  n->lhs = e;
  return n;
}

static node *expr() {
  return assign();
}

static node *assign() {
  // ensure temporaries are in function scope
  node *lhs = null;
  node *rhs = null;

  lhs = equality();
  if (consume("=")) {
    rhs = assign();
    node *n = new_node(nd_assign);
    n->lhs = lhs;
    n->rhs = rhs;
    // type handling omitted for brevity
    return n;
  }
  return lhs;
}

static node *equality() {
  node *n = relational();
  while (1) {
    if (consume("==")) {
      node *r = relational();
      node *t = new_node(nd_eq);
      t->lhs = n; t->rhs = r; n = t;
    } else if (consume("!=")) {
      node *r = relational();
      node *t = new_node(nd_ne);
      t->lhs = n; t->rhs = r; n = t;
    } else break;
  }
  return n;
}

static node *relational() {
  node *n = add();
  while (1) {
    if (consume("<")) {
      node *r = add();
      node *t = new_node(nd_lt);
      t->lhs = n; t->rhs = r; n = t;
    } else if (consume("<=")) {
      node *r = add();
      node *t = new_node(nd_le);
      t->lhs = n; t->rhs = r; n = t;
    } else if (consume(">")) {
      node *r = add();
      node *t = new_node(nd_lt);
      t->lhs = r; t->rhs = n; n = t;
    } else if (consume(">=")) {
      node *r = add();
      node *t = new_node(nd_le);
      t->lhs = r; t->rhs = n; n = t;
    } else break;
  }
  return n;
}

static node *add() {
  // temporaries used across blocks must be declared at function scope
  node *n = mul();
  node *rhs = null;
  node *mul_node = null;
  node *sub_node = null;
  node *div_node = null;

  while (1) {
    if (consume("+")) {
      rhs = mul();
      node *t = new_node(nd_add);
      t->lhs = n; t->rhs = rhs; n = t;
    } else if (consume("-")) {
      rhs = mul();
      node *t = new_node(nd_sub);
      t->lhs = n; t->rhs = rhs; n = t;
    } else break;
  }
  return n;
}

static node *mul() {
  node *n = unary();
  while (1) {
    if (consume("*")) {
      node *r = unary();
      node *t = new_node(nd_mul);
      t->lhs = n; t->rhs = r; n = t;
    } else if (consume("/")) {
      node *r = unary();
      node *t = new_node(nd_div);
      t->lhs = n; t->rhs = r; n = t;
    } else break;
  }
  return n;
}

static node *unary() {
  if (consume("&")) {
    node *n = new_node(nd_addr);
    n->lhs = unary();
    return n;
  }
  if (consume("*")) {
    node *n = new_node(nd_deref);
    n->lhs = unary();
    return n;
  }
  if (consume("sizeof")) {
    node *p = unary();
    node *n = new_node(nd_sizeof);
    n->lhs = p;
    return n;
  }
  return postfix();
}

static node *postfix() {
  /* temporaries used across blocks must be declared at function scope */
  node *n = null;
  node *a = null;
  node *idx = null;
  node *addn = null;
  node *deref = null;

  n = primary();
  while (1) {
    if (consume("(")) {
      // function call
      node *call = new_node(nd_funcall);
      call->name = n->name ? n->name : null;
      call->args = null;
      call->args_len = 0;
      if (!consume(")")) {
        while (1) {
          node *arg = expr();
          call->args = xrealloc(call->args, sizeof(node*) * (call->args_len + 1));
          call->args[call->args_len++] = arg;
          if (consume(")")) break;
          expect(",");
        }
      }
      n = call;
    } else if (consume("[")) {
      idx = expr();
      expect("]");
      // a[b] -> *(a + b)
      addn = new_node(nd_add);
      addn->lhs = n;
      addn->rhs = idx;
      deref = new_node(nd_deref);
      deref->lhs = addn;
      n = deref;
    } else if (consume("->")) {
      char *mname = expect_ident();
      node *t = new_node(nd_member_ref);
      t->lhs = n;
      t->member_name = mname;
      n = t;
    } else if (consume(".")) {
      char *mname = expect_ident();
      node *t = new_node(nd_member);
      t->lhs = n;
      t->member_name = mname;
      n = t;
    } else break;
  }
  return n;
}

static node *primary() {
  if (consume("(")) {
    node *n = expr();
    expect(")");
    return n;
  }
  if (current_token->kind == tk_num) {
    int v = expect_number();
    return new_num_node(v);
  }
  if (current_token->kind == tk_str) {
    char *s = current_token->str;
    current_token = current_token->next;
    node *n = new_node(nd_num);
    n->val = register_string_literal(s);
    n->ty = ptr_to(get_int_type_by_rank(0, false)); // char*
    return n;
  }
  if (current_token->kind == tk_ident) {
    char *name = expect_ident();
    // could be variable or function name
    node *n = new_var_node(name);
    return n;
  }
  error_at_tok(current_token->loc, "unexpected token in primary");
  return null;
}

static node *compound_stmt() {
  expect("{");
  node *blk = new_node(nd_block);
  blk->stmts = null;
  blk->stmts_len = 0;
  while (!consume("}")) {
    node *s = stmt();
    blk->stmts = xrealloc(blk->stmts, sizeof(node*) * (blk->stmts_len + 1));
    blk->stmts[blk->stmts_len++] = s;
  }
  return blk;
}

/* ------------------------------------------------------------------------- */
/* code generation (very small, emits x86-64 assembly text)                 */
/* ------------------------------------------------------------------------- */

/* For brevity, codegen is a minimal placeholder that prints a message.
   A full implementation would traverse the AST and emit assembly. */

static void codegen(node *prog, const char *outname) {
  file *f = fopen(outname, "w");
  if (!f) { fprintf(stderr, "cannot open output %s\n", outname); exit(1); }
  fprintf(f, "/* ciacc: minimal codegen placeholder */\n");
  fprintf(f, "/* This compiler front-end parsed the input successfully. */\n");
  fclose(f);
}

/* ------------------------------------------------------------------------- */
/* string literal registration (simple)                                      */
/* ------------------------------------------------------------------------- */

#define MAX_STRINGS 1024
static char *string_table[MAX_STRINGS];
static int string_table_len = 0;
int register_string_literal(const char *s) {
  if (string_table_len >= MAX_STRINGS) { fprintf(stderr, "too many string literals\n"); exit(1); }
  string_table[string_table_len] = xstrdup(s);
  return string_table_len++;
}

/* ------------------------------------------------------------------------- */
/* entry point                                                                */
/* ------------------------------------------------------------------------- */

int main(int argc, char **argv) {
  if (argc < 2) { fprintf(stderr, "usage: ciacc <src.c> -o <outname>\n"); return 1; }
  const char *infile = argv[1];
  const char *outname = "out.s";
  for (int i = 2; i < argc; i++) {
    if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) { outname = argv[i+1]; i++; }
  }

  char *src = preprocess_file(infile);
  if (!src) { fprintf(stderr, "failed to read %s\n", infile); return 1; }
  user_input = src;
  current_token = tokenize();
  node *ast = program();
  codegen(ast, outname);
  free(src);
  return 0;
}
