// ciacc.c
// Compact tutorial-style C compiler (subset) with integrated preprocessor,
// types: char/short/int/long (signed/unsigned), pointers, structs,
// local variables, functions, control flow, expressions, string literals.
// Preprocessor supports #include "file" and #include <file> (search /usr/include).
//
// Usage: ciacc <src.c> -o <outname>
// Emits x86-64 assembly to <outname>.s
//
// Notes:
// - This is a teaching compiler, not a full C implementation.
// - Structs: named tags, simple field lists (no bitfields, no nested struct definitions in fields).
// - Member access: `a.field` and `p->field` supported.
// - Alignment: fields are laid out with natural alignment (1,2,4,8).
// - #include <...> searches /usr/include first, then the including file's directory.

#define _GNU_SOURCE

/* ------------------------------------------------------------------------- */
/* Minimal Header Implementations (replacing system includes) */
/* ------------------------------------------------------------------------- */

/* <stddef.h> */
typedef unsigned long size_t;
#define NULL ((void*)0)

/* <stdbool.h> */
#define bool int
#define true 1
#define false 0

/* ------------------------------------------------------------------------- */
/* Minimal stdarg.h emulation (moved up to define ciacc_va_list early) */
/* ------------------------------------------------------------------------- */

#ifndef CIACC_STDARG_H
#define CIACC_STDARG_H

/* Use a raw pointer as va_list. It points to the next argument's storage. */
typedef char *ciacc_va_list;

/* Alignment unit: use pointer size for natural alignment on target (8 on x86-64). */
#define CIACC_VA_ALIGN (sizeof(void*))

/* Round up 'n' to the next multiple of CIACC_VA_ALIGN */
#define CIACC_VA_ROUND(n) (((n) + (CIACC_VA_ALIGN - 1)) & ~(CIACC_VA_ALIGN - 1))

/*
 * va_start(ap, last)
 */
#define va_list ciacc_va_list
#define va_start(ap, last) \
  ((ap) = (ciacc_va_list)((char *)&(last) + CIACC_VA_ROUND(sizeof(last))))

/*
 * va_arg(ap, type)
 */
#define va_arg(ap, type) \
  (*(type *)((ap) += CIACC_VA_ROUND(sizeof(type)), (ap) - CIACC_VA_ROUND(sizeof(type))))

/* va_end: clear the pointer (no-op for this simple implementation) */
#define va_end(ap) ((ap) = (ciacc_va_list)0)

/* va_copy: copy one va_list to another */
#define va_copy(dest, src) ((dest) = (src))

#endif /* CIACC_STDARG_H */


/* <stdio.h> - Minimal type definition and declarations */

// Minimal definition for the standard C stream object.
// The missing semicolon here was the likely cause of the "invalid storage class" errors.
struct _iobuf {
    void *ptr;
    int count;
    char *base;
    int flag;
}; // <-- SEMICOLON ADDED HERE

// Define the standard C FILE type (pointer to struct _iobuf).
typedef struct _iobuf FILE;

// Declarations for system I/O functions and streams (requires linker to find libc)
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

extern int fprintf(FILE * restrict stream, const char * restrict format, ...);
// Corrected to use the locally defined ciacc_va_list to resolve type mismatch
extern int vfprintf(FILE * restrict stream, const char * restrict format, ciacc_va_list ap);
extern FILE *fopen(const char * restrict filename, const char * restrict mode);
extern int fclose(FILE *stream);
extern long ftell(FILE *stream);
extern int fseek(FILE *stream, long offset, int whence);
extern size_t fread(void * restrict ptr, size_t size, size_t nitems, FILE * restrict stream);
extern int fputc(int c, FILE *stream);
extern int snprintf(char * restrict s, size_t n, const char * restrict format, ...);
#define SEEK_END 2
#define SEEK_SET 0

/* <stdlib.h> - Declarations for memory/control functions used */
extern void *malloc(size_t size);
extern void *realloc(void *ptr, size_t size);
extern void free(void *ptr); // <-- ADDED MISSING free DECLARATION
extern char *strdup(const char *s);
extern void exit(int status);
extern long strtol(const char * restrict nptr, char ** restrict endptr, int base);

/* <string.h> - Declarations for string functions used */
extern size_t strlen(const char *s);
extern int strcmp(const char *s1, const char *s2);
extern int strncmp(const char *s1, const char *s2, size_t n); // <-- ADDED MISSING strncmp DECLARATION
extern void *memcpy(void * restrict s1, const void * restrict s2, size_t n);
extern int memcmp(const void *s1, const void *s2, size_t n);
extern char *strchr(const char *s, int c);
extern char *strrchr(const char *s, int c);
extern char *strndup(const char *s, size_t size);
extern void *memset(void *s, int c, size_t n);

/* <ctype.h> - Minimal inline implementations */
static inline int isspace(int c) { return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r'; }
static inline int isalpha(int c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
static inline int isalnum(int c) { return isalpha(c) || (c >= '0' && c <= '9'); }
static inline int isdigit(int c) { return c >= '0' && c <= '9'; }

/* ------------------------------------------------------------------------- */
//
// Utilities
//
static void *xmalloc(size_t n) {
// ... rest of the file remains unchanged ...
