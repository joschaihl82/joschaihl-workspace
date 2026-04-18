// runtime.c
// Complete runtime for Sparrow compiler: dynamic arrays, objects, and Value helpers.
// Value tagging:
//   - TAG_INT  (low 3 bits == 0): 61-bit signed integer stored as (n << 3)
//   - TAG_PTR  (low 3 bits == 1): pointer to heap object (strings, arrays, objects)
//   - TAG_BOOL (low 3 bits == 2): boolean stored as (0 or 1) << 3 | TAG_BOOL
//
// This runtime exposes the following C-callable symbols used by the compiler:
//   js_array_create_from_c, js_array_new_c, js_array_length, js_array_get,
//   js_array_set, js_array_push, js_object_new, js_object_set, js_object_get,
//   js_value_to_cstr, js_print_value
//
// Notes:
// - Strings are allocated as plain C strings (char*). When creating a string Value
//   use val_from_ptr(ptr) to tag it as a pointer Value.
// - This runtime uses malloc and never frees memory; for short-lived programs this is fine.
// - If your compiler emits raw pointers without tagging, you must either tag them
//   (OR with TAG_PTR) or adapt the compiler to produce tagged pointer Values.

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

/* Value representation */
typedef uint64_t Value;

#define TAG_INT  0x0ULL
#define TAG_PTR  0x1ULL
#define TAG_BOOL 0x2ULL

static inline Value val_from_int(int64_t n) { return ((uint64_t)n << 3) | TAG_INT; }
static inline int64_t val_to_int(Value v) { return ((int64_t)v) >> 3; }
static inline Value val_from_ptr(void *p) { return ((uint64_t)(uintptr_t)p) | TAG_PTR; }
static inline void* val_to_ptr(Value v) { return (void*)(uintptr_t)(v & ~0x7ULL); }
static inline Value val_from_bool(int b) { return ((uint64_t)(b ? 1 : 0) << 3) | TAG_BOOL; }
static inline int val_to_bool(Value v) { return (int)(((uint64_t)v) >> 3); }

/* String helper */
static char* rt_strdup(const char *s) {
    if (!s) return NULL;
    char *p = strdup(s);
    return p;
}
Value rt_make_string(const char *s) {
    char *p = rt_strdup(s ? s : "");
    return val_from_ptr((void*)p);
}

/* Dynamic array implementation */
typedef struct {
    int64_t len;
    int64_t cap;
    Value *items;
} JSArray;

Value rt_array_new(void) {
    JSArray *a = calloc(1, sizeof(JSArray));
    a->len = 0;
    a->cap = 4;
    a->items = calloc((size_t)a->cap, sizeof(Value));
    return val_from_ptr(a);
}

Value rt_array_create_from_c(Value *items, int64_t n) {
    JSArray *a = malloc(sizeof(JSArray));
    a->len = n;
    a->cap = n > 0 ? n : 4;
    a->items = malloc((size_t)a->cap * sizeof(Value));
    for (int64_t i = 0; i < n; i++) a->items[i] = items[i];
    return val_from_ptr(a);
}

int64_t rt_array_length(Value arrv) {
    if ((arrv & 0x7ULL) != TAG_PTR) return 0;
    JSArray *a = (JSArray*)val_to_ptr(arrv);
    if (!a) return 0;
    return a->len;
}

Value rt_array_get(Value arrv, int64_t idx) {
    if ((arrv & 0x7ULL) != TAG_PTR) return val_from_int(0);
    JSArray *a = (JSArray*)val_to_ptr(arrv);
    if (!a) return val_from_int(0);
    if (idx < 0 || idx >= a->len) return val_from_int(0);
    return a->items[idx];
}

void rt_array_set(Value arrv, int64_t idx, Value v) {
    if ((arrv & 0x7ULL) != TAG_PTR) return;
    JSArray *a = (JSArray*)val_to_ptr(arrv);
    if (!a) return;
    if (idx < 0) return;
    if (idx >= a->cap) {
        int64_t nc = a->cap;
        while (idx >= nc) nc *= 2;
        a->items = realloc(a->items, (size_t)nc * sizeof(Value));
        a->cap = nc;
    }
    if (idx >= a->len) {
        for (int64_t i = a->len; i <= idx; i++) a->items[i] = val_from_int(0);
        a->len = idx + 1;
    }
    a->items[idx] = v;
}

void rt_array_push(Value arrv, Value v) {
    if ((arrv & 0x7ULL) != TAG_PTR) return;
    JSArray *a = (JSArray*)val_to_ptr(arrv);
    if (!a) return;
    if (a->len >= a->cap) {
        int64_t nc = a->cap ? a->cap * 2 : 4;
        a->items = realloc(a->items, (size_t)nc * sizeof(Value));
        a->cap = nc;
    }
    a->items[a->len++] = v;
}

/* Simple object: chained hash map with string keys */
typedef struct ObjEntry {
    char *key;
    Value val;
    struct ObjEntry *next;
} ObjEntry;

typedef struct {
    int64_t nbuckets;
    ObjEntry **buckets;
} JSObject;

static uint64_t rt_hash_str(const char *s) {
    uint64_t h = 1469598103934665603ULL;
    while (*s) { h ^= (unsigned char)*s++; h *= 1099511628211ULL; }
    return h;
}

Value rt_object_new(void) {
    JSObject *o = calloc(1, sizeof(JSObject));
    o->nbuckets = 16;
    o->buckets = calloc((size_t)o->nbuckets, sizeof(ObjEntry*));
    return val_from_ptr(o);
}

void rt_object_set(Value objv, const char *key, Value v) {
    if ((objv & 0x7ULL) != TAG_PTR) return;
    JSObject *o = (JSObject*)val_to_ptr(objv);
    if (!o) return;
    uint64_t h = rt_hash_str(key);
    int idx = (int)(h % (uint64_t)o->nbuckets);
    ObjEntry *e = o->buckets[idx];
    while (e) {
        if (!strcmp(e->key, key)) { e->val = v; return; }
        e = e->next;
    }
    ObjEntry *ne = malloc(sizeof(ObjEntry));
    ne->key = strdup(key);
    ne->val = v;
    ne->next = o->buckets[idx];
    o->buckets[idx] = ne;
}

Value rt_object_get(Value objv, const char *key) {
    if ((objv & 0x7ULL) != TAG_PTR) return val_from_int(0);
    JSObject *o = (JSObject*)val_to_ptr(objv);
    if (!o) return val_from_int(0);
    uint64_t h = rt_hash_str(key);
    int idx = (int)(h % (uint64_t)o->nbuckets);
    ObjEntry *e = o->buckets[idx];
    while (e) {
        if (!strcmp(e->key, key)) return e->val;
        e = e->next;
    }
    return val_from_int(0);
}

/* Helpers for printing and interop */

/* Convert Value to C string pointer when possible.
 * For strings (pointer Values) return the C string pointer.
 * For integers and booleans, return a static buffer with textual representation.
 * Caller must not free the returned pointer.
 */
const char* rt_value_to_cstr(Value v) {
    static char buf[64];
    if ((v & 0x7ULL) == TAG_PTR) {
        void *p = val_to_ptr(v);
        if (!p) return "(null)";
        return (const char*)p;
    } else if ((v & 0x7ULL) == TAG_BOOL) {
        return val_to_bool(v) ? "true" : "false";
    } else {
        /* integer */
        int64_t n = val_to_int(v);
        snprintf(buf, sizeof(buf), "%" PRId64, (int64_t)n);
        return buf;
    }
}

/* Print Value to stdout (no newline). */
void rt_print_value(Value v) {
    if ((v & 0x7ULL) == TAG_PTR) {
        void *p = val_to_ptr(v);
        if (p) fputs((const char*)p, stdout);
        else fputs("(null)", stdout);
    } else if ((v & 0x7ULL) == TAG_BOOL) {
        fputs(val_to_bool(v) ? "true" : "false", stdout);
    } else {
        printf("%" PRId64, (int64_t)val_to_int(v));
    }
}

/* Exposed C API (symbols used by compiler-generated assembly) */

/* Arrays */
unsigned long long js_array_create_from_c(unsigned long long *items, long long n) {
    /* items[] are Values already (uint64_t). Create JSArray and copy. */
    return (unsigned long long)rt_array_create_from_c((Value*)items, (int64_t)n);
}
unsigned long long js_array_new_c(void) {
    return (unsigned long long)rt_array_new();
}
long long js_array_length(unsigned long long arr) {
    return (long long)rt_array_length((Value)arr);
}
unsigned long long js_array_get(unsigned long long arr, long long idx) {
    return (unsigned long long)rt_array_get((Value)arr, (int64_t)idx);
}
void js_array_set(unsigned long long arr, long long idx, unsigned long long v) {
    rt_array_set((Value)arr, (int64_t)idx, (Value)v);
}
void js_array_push(unsigned long long arr, unsigned long long v) {
    rt_array_push((Value)arr, (Value)v);
}

/* Objects */
unsigned long long js_object_new(void) {
    return (unsigned long long)rt_object_new();
}
void js_object_set(unsigned long long obj, const char *key, unsigned long long v) {
    rt_object_set((Value)obj, key, (Value)v);
}
unsigned long long js_object_get(unsigned long long obj, const char *key) {
    return (unsigned long long)rt_object_get((Value)obj, key);
}

/* Printing helpers */
const char* js_value_to_cstr(unsigned long long v) {
    return rt_value_to_cstr((Value)v);
}
void js_print_value(unsigned long long v) {
    rt_print_value((Value)v);
}

/* Convenience: create Value from C string (tagged pointer) */
unsigned long long js_make_string(const char *s) {
    return (unsigned long long)rt_make_string(s);
}

/* For debugging: dump a Value to stderr */
void js_debug_print(unsigned long long v) {
    if ((v & 0x7ULL) == TAG_PTR) {
        fprintf(stderr, "Value(ptr=%p)\n", (void*)(uintptr_t)(v & ~0x7ULL));
    } else if ((v & 0x7ULL) == TAG_BOOL) {
        fprintf(stderr, "Value(bool=%d)\n", (int)(((uint64_t)v) >> 3));
    } else {
        fprintf(stderr, "Value(int=%" PRId64 ")\n", (int64_t)val_to_int((Value)v));
    }
}

/* End of runtime.c */
