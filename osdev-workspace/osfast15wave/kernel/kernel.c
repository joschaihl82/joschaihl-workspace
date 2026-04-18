// kernel.c
// Kernel-mode Gerstner grid renderer with filled quads.
// No libc I/O. All allocations via kernel_alloc_page. No SDL, no math.h.
// Quads are filled as two triangles; per-quad color path and rasterizer use integer arithmetic.
// Geometry (Gerstner + projection) still uses floats; color path and rasterization inner loops are integer-only.
//
// Amalgamated patch: integer HSV->RGB, integer color helpers, Q16 fixed-point rasterizer, integer-only fill_quad.

#include <stdint.h>
#include <stddef.h>

/* --- Config --- */
#define PAGE_PRESENT (1ULL << 0)
#define PAGE_RW      (1ULL << 1)
#define PAGE_PWT     (1ULL << 3)
#define PAGE_PS      (1ULL << 7)

#define HEAP_SIZE   0x1000000
#define PAGE_SIZE   4096
#define GRID_SIZE 60

typedef struct {
    uint32_t Width;
    uint32_t Height;
    uint32_t PixelsPerScanLine;
    uint64_t FrameBufferBase;
} KernelGOPInfo;

/* --- Simple static heap and allocator --- */
static uint8_t page_heap[HEAP_SIZE] __attribute__((aligned(4096)));
static int page_ptr = 0;
void *kernel_alloc_page(void) {
    if (page_ptr + PAGE_SIZE > HEAP_SIZE) return 0;
    void *p = &page_heap[page_ptr];
    page_ptr += PAGE_SIZE;
    uint8_t *b = (uint8_t*)p;
    for (int i = 0; i < PAGE_SIZE; ++i) b[i] = 0;
    return p;
}

/* --- Minimal memory ops --- */
void *memset(void *dst, int c, size_t n) {
    uint8_t *p = (uint8_t*)dst;
    while (n--) *p++ = (uint8_t)c;
    return dst;
}
void memcpy64(void *dst, const void *src, size_t bytes) {
    uint64_t *d = (uint64_t*)dst;
    const uint64_t *s = (const uint64_t*)src;
    size_t n = bytes / 8;
    while (n--) *d++ = *s++;
    size_t rem = bytes & 7;
    if (rem) {
        uint8_t *db = (uint8_t*)d;
        const uint8_t *sb = (const uint8_t*)s;
        while (rem--) *db++ = *sb++;
    }
}

/* --- Minimal GDT/TSS/paging helpers (compact) --- */
struct GDTEntry { uint16_t a,b; uint8_t c,d,e,f; } __attribute__((packed));
struct GDTSystem { struct GDTEntry low; uint32_t up, r; } __attribute__((packed));
struct GDTPtr { uint16_t limit; uint64_t base; } __attribute__((packed));
struct TSS { uint32_t r0; uint64_t rsp0; uint64_t rsp1; uint64_t rsp2; uint64_t r1; uint64_t ist[7]; uint16_t r2; uint16_t iopb; } __attribute__((packed));

static struct { struct GDTEntry n, c, d; struct GDTSystem t; } __attribute__((packed, aligned(8))) gdt;
static struct GDTPtr gdtptr;
static struct TSS tss;
static uint8_t tss_stack[PAGE_SIZE];

void init_gdt_tss(void) {
    gdt.c.d = 0x20; gdt.c.c = 0x9A;
    gdt.d.c = 0x92;
    uint64_t base = (uint64_t)&tss;
    tss.rsp0 = (uint64_t)tss_stack + PAGE_SIZE;
    gdt.t.low.a = sizeof(tss)-1;
    gdt.t.low.b = (uint16_t)base;
    gdt.t.low.c = (uint8_t)(base >> 16);
    gdt.t.low.d = 0x89;
    gdt.t.low.e = (uint8_t)(base >> 24);
    gdt.t.up = (uint32_t)(base >> 32);
    gdtptr.limit = sizeof(gdt)-1;
    gdtptr.base = (uint64_t)&gdt;
    __asm__ volatile ("lgdt %0" :: "m"(gdtptr) : "memory");
    __asm__ volatile ("mov $0x18, %%ax\n\tltr %%ax" ::: "ax");
}

/* --- Minimal paging (identity mapping for low memory) --- */
static uint64_t pml4[512] __attribute__((aligned(4096)));
void init_paging(KernelGOPInfo *kgi) {
    const uint64_t flags = PAGE_PRESENT | PAGE_RW;
    const uint64_t fb_flags = PAGE_PRESENT | PAGE_RW | PAGE_PWT;
    uint64_t *pdpt = (uint64_t*)kernel_alloc_page();
    pml4[0] = (uint64_t)pdpt | flags;
    for (int i = 0; i < 4; ++i) {
        uint64_t *pd = (uint64_t*)kernel_alloc_page();
        pdpt[i] = (uint64_t)pd | flags;
        for (int j = 0; j < 512; ++j) {
            uint64_t addr = (uint64_t)(i * 512 + j) * 0x200000ULL;
            uint64_t f = (addr >= kgi->FrameBufferBase && addr < kgi->FrameBufferBase + 0x2000000ULL) ? fb_flags : flags;
            pd[j] = addr | f | PAGE_PS;
        }
    }
    __asm__ volatile ("mov %0, %%cr3" :: "r"(pml4) : "memory");
}

/* --- Enable SSE (small helper) --- */
void enable_sse(void) {
    uint64_t cr0, cr4;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 &= ~(1 << 2); cr0 |= (1 << 1);
    __asm__ volatile ("mov %0, %%cr0" :: "r"(cr0));
    __asm__ volatile ("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (3 << 9);
    __asm__ volatile ("mov %0, %%cr4" :: "r"(cr4));
}

/* --- Framebuffer state --- */
static uint32_t *frontbuffer = 0;
static uint32_t *backbuffer = 0;
static volatile uint32_t *fbb = 0;
static uint32_t pps = 0;
static uint32_t screen_width = 0, screen_height = 0;
static size_t buffer_size_bytes = 0;
static inline void swap_buffers(void) {
    memcpy64((void*)frontbuffer, (const void*)backbuffer, buffer_size_bytes);
}

/* --- Minimal trig approximations (range reduction + 4th-order poly) --- */
static float my_sinf(float x) {
    const float PI = 3.14159265358979323846f;
    const float TWO_PI = 6.28318530717958647692f;
    const float INV_TWO_PI = 0.15915494309189533577f;
    int k = (int)(x * INV_TWO_PI);
    x = x - (float)k * TWO_PI;
    if (x < -PI) x += TWO_PI;
    if (x > PI) x -= TWO_PI;
    float x2 = x * x;
    float x4 = x2 * x2;
    return x * (1.0f - x2 * (1.0f / 6.0f) + x4 * (1.0f / 120.0f));
}
static float my_cosf(float x) {
    const float PI_2 = 1.57079632679489661923f;
    return my_sinf(x + PI_2);
}
static inline float clampf(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

/* --- Integer color helpers (all integer arithmetic) --- */

/* Pack RGB components (0..255) into 0xRRGGBB */
static inline uint32_t pack_rgb_u8(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

/* Unpack components */
static inline void unpack_rgb_u8(uint32_t rgb, uint8_t *r, uint8_t *g, uint8_t *b) {
    *r = (uint8_t)((rgb >> 16) & 0xFF);
    *g = (uint8_t)((rgb >> 8) & 0xFF);
    *b = (uint8_t)(rgb & 0xFF);
}

/* Mix two colors by integer alpha (0..255). result = a*(255-alpha)/255 + b*alpha/255 */
static inline uint32_t mix_rgb_u8(uint32_t a, uint32_t b, uint8_t alpha) {
    uint8_t ar, ag, ab;
    uint8_t br, bg, bb;
    unpack_rgb_u8(a, &ar, &ag, &ab);
    unpack_rgb_u8(b, &br, &bg, &bb);
    int inv = 255 - alpha;
    int rr = (ar * inv + br * alpha) / 255;
    int gg = (ag * inv + bg * alpha) / 255;
    int bbv = (ab * inv + bb * alpha) / 255;
    if (rr < 0) rr = 0; if (rr > 255) rr = 255;
    if (gg < 0) gg = 0; if (gg > 255) gg = 255;
    if (bbv < 0) bbv = 0; if (bbv > 255) bbv = 255;
    return pack_rgb_u8((uint8_t)rr, (uint8_t)gg, (uint8_t)bbv);
}

/* Scale brightness of an RGB color by factor (0..255). 255 = unchanged, 128 = half brightness. */
static inline uint32_t scale_rgb_brightness(uint32_t col, uint8_t factor) {
    uint8_t r,g,b;
    unpack_rgb_u8(col, &r, &g, &b);
    int rr = (r * factor) / 255;
    int gg = (g * factor) / 255;
    int bb = (b * factor) / 255;
    return pack_rgb_u8((uint8_t)rr, (uint8_t)gg, (uint8_t)bb);
}

/* --- Integer HSV -> RGB helper (8-bit S/V, H in degrees 0..359) --- */
/* h: 0..359, s: 0..255, v: 0..255 */
static inline uint32_t hsv_to_rgb_int(int h, int s, int v) {
    if (s == 0) {
        uint8_t c = (uint8_t)v;
        return pack_rgb_u8(c, c, c);
    }
    if (h < 0) {
        h %= 360;
        if (h < 0) h += 360;
    }
    if (h >= 360) h %= 360;
    int region = h / 60;                // 0..5
    int rem = (h - region * 60) * 255 / 60; // fractional part scaled to 0..255

    // p = v * (1 - s/255)
    int p = (v * (255 - s)) / 255;
    // q = v * (1 - f*s)
    int q = (v * (255 - (s * rem) / 255)) / 255;
    // t = v * (1 - (1-f)*s)
    int t = (v * (255 - (s * (255 - rem) / 255))) / 255;

    uint8_t R, G, B;
    switch (region) {
        case 0: R = (uint8_t)v; G = (uint8_t)t; B = (uint8_t)p; break;
        case 1: R = (uint8_t)q; G = (uint8_t)v; B = (uint8_t)p; break;
        case 2: R = (uint8_t)p; G = (uint8_t)v; B = (uint8_t)t; break;
        case 3: R = (uint8_t)p; G = (uint8_t)q; B = (uint8_t)v; break;
        case 4: R = (uint8_t)t; G = (uint8_t)p; B = (uint8_t)v; break;
        default: R = (uint8_t)v; G = (uint8_t)p; B = (uint8_t)q; break;
    }
    return pack_rgb_u8(R, G, B);
}

/* --- Gerstner waves --- */
typedef struct { float amplitude, phase, frequency, dirX, dirY, steepness; } Wave;
static Wave waves[4] = {
    {15.0f, 0.5f, 0.05f, 1.0f, 0.2f, 0.4f},
    {10.0f, 1.2f, 0.08f, 0.5f, 0.8f, 0.3f},
    {5.0f, 0.2f, 0.15f, -0.7f, 0.3f, 0.2f},
    {8.0f, 0.8f, 0.1f, 0.1f, 1.0f, 0.25f}
};
static void calculate_gerstner(float x, float y, float time, float *ox, float *oy, float *oz) {
    *ox = x; *oy = y; *oz = 0.0f;
    for (int i = 0; i < 4; ++i) {
        Wave *w = &waves[i];
        float dot = (x * w->dirX + y * w->dirY) * w->frequency;
        float theta = dot + time * w->phase;
        float c = my_cosf(theta);
        float s = my_sinf(theta);
        float q = w->steepness / (w->frequency * w->amplitude * 4.0f);
        *ox += q * w->amplitude * w->dirX * c;
        *oy += q * w->amplitude * w->dirY * c;
        *oz += w->amplitude * s;
    }
}

/* --- RNG: tiny LCG for per-quad color jitter --- */
static uint32_t rng_state = 123456789u;
static inline uint32_t rand32(void) {
    rng_state = rng_state * 1664525u + 1013904223u;
    return rng_state;
}

/* --- Pixel put (writes into backbuffer) --- */
static inline void put_pixel_back(int x, int y, uint32_t color) {
    if (x < 0 || x >= (int)screen_width || y < 0 || y >= (int)screen_height) return;
    backbuffer[y * pps + x] = color;
}

/* --- Fixed-point rasterizer (Q16) and integer-only fill_quad --- */

#define FP_SHIFT 16
#define FP_ONE   (1 << FP_SHIFT)
#define FP_HALF  (1 << (FP_SHIFT - 1))

/* Convert float screen coordinate to 16.16 fixed */
static inline int32_t f_to_fp16(float f) {
    return (int32_t)(f * (float)FP_ONE + (f >= 0.0f ? 0.5f : -0.5f));
}

/* Integer fixed-point triangle rasterizer.
   x0f..x2f are 16.16 fixed coordinates (int32_t).
   color is 0xRRGGBB as before.
*/
static void rasterize_triangle_fixed(int32_t x0f, int32_t y0f,
                                     int32_t x1f, int32_t y1f,
                                     int32_t x2f, int32_t y2f,
                                     uint32_t color)
{
    /* compute integer pixel bounding box (floor(min), ceil(max)) */
    int32_t minxf = x0f; if (x1f < minxf) minxf = x1f; if (x2f < minxf) minxf = x2f;
    int32_t maxxf = x0f; if (x1f > maxxf) maxxf = x1f; if (x2f > maxxf) maxxf = x2f;
    int32_t minyf = y0f; if (y1f < minyf) minyf = y1f; if (y2f < minyf) minyf = y2f;
    int32_t maxyf = y0f; if (y1f > maxyf) maxyf = y1f; if (y2f > maxyf) maxyf = y2f;

    int minx = minxf >> FP_SHIFT;
    int maxx = (maxxf + (FP_ONE - 1)) >> FP_SHIFT; /* ceil */
    int miny = minyf >> FP_SHIFT;
    int maxy = (maxyf + (FP_ONE - 1)) >> FP_SHIFT; /* ceil */

    if (minx < 0) minx = 0;
    if (miny < 0) miny = 0;
    if (maxx > (int)screen_width) maxx = screen_width;
    if (maxy > (int)screen_height) maxy = screen_height;
    if (minx >= maxx || miny >= maxy) return;

    /* Edge coefficients in fixed units (Q16) */
    int64_t e0x = (int64_t)y1f - (int64_t)y0f; /* Q16 */
    int64_t e0y = -((int64_t)x1f - (int64_t)x0f); /* Q16 */
    int64_t e1x = (int64_t)y2f - (int64_t)y1f;
    int64_t e1y = -((int64_t)x2f - (int64_t)x1f);
    int64_t e2x = (int64_t)y0f - (int64_t)y2f;
    int64_t e2y = -((int64_t)x0f - (int64_t)x2f);

    /* constant terms c = e.x * x + e.y * y  (both operands Q16 -> product Q32) */
    int64_t c0 = e0x * (int64_t)x0f + e0y * (int64_t)y0f; /* Q32 */
    int64_t c1 = e1x * (int64_t)x1f + e1y * (int64_t)y1f;
    int64_t c2 = e2x * (int64_t)x2f + e2y * (int64_t)y2f;

    for (int y = miny; y < maxy; ++y) {
        int64_t py_fixed = ((int64_t)y << FP_SHIFT) + FP_HALF;
        for (int x = minx; x < maxx; ++x) {
            int64_t px_fixed = ((int64_t)x << FP_SHIFT) + FP_HALF;

            int64_t v0 = e0x * px_fixed + e0y * py_fixed - c0;
            int64_t v1 = e1x * px_fixed + e1y * py_fixed - c1;
            int64_t v2 = e2x * px_fixed + e2y * py_fixed - c2;

            /* top-left fill rule: accept if all non-negative or all non-positive */
            if ((v0 >= 0 && v1 >= 0 && v2 >= 0) || (v0 <= 0 && v1 <= 0 && v2 <= 0)) {
                backbuffer[y * pps + x] = color;
            }
        }
    }
}

/* Highly integer-optimized fill_quad:
   - converts float vertices to Q16 fixed
   - computes per-quad integer color using only integer ops
   - calls rasterize_triangle_fixed twice
*/
static void fill_quad(float ax, float ay, float bx, float by, float cx, float cy, float dx, float dy, uint32_t base_rgb) {
    /* Convert vertex coords to 16.16 fixed */
    int32_t axf = f_to_fp16(ax);
    int32_t ayf = f_to_fp16(ay);
    int32_t bxf = f_to_fp16(bx);
    int32_t byf = f_to_fp16(by);
    int32_t cxf = f_to_fp16(cx);
    int32_t cyf = f_to_fp16(cy);
    int32_t dxf = f_to_fp16(dx);
    int32_t dyf = f_to_fp16(dy);

    /* --- integer-only color jitter and tinting --- */
    uint32_t r = rand32();
    int rnd8 = (int)(r & 0xFF);        /* 0..255 */

    /* Map rnd8 to a brightness factor in [232..280] (center ~256).
       Use factor256 (Q8) to avoid divisions: factor256 = 256 + ((rnd8 - 128) * 24) / 128
       Then convert to 0..255 brightness factor: factor8 = (factor256 * 255) / 256
    */
    int delta = (rnd8 - 128); /* -128..127 */
    int factor256 = 256 + (delta * 24) / 128; /* ~232..280 */
    int factor8 = (factor256 * 255) / 256;
    if (factor8 < 0) factor8 = 0;
    if (factor8 > 255) factor8 = 255;

    /* Apply brightness scaling */
    uint32_t color = scale_rgb_brightness(base_rgb, (uint8_t)factor8);

    /* Small tint: use high bits of RNG to pick tint alpha 0..20 */
    uint8_t tint_alpha = (uint8_t)(((r >> 8) & 0x0F) * 20 / 15); /* 0..20 */
    const uint32_t tint_color = 0xC0F0FFu; /* light cyan tint */
    if (tint_alpha) color = mix_rgb_u8(color, tint_color, tint_alpha);

    /* Slight additional per-channel micro-perturb using low bits (cheap) */
    /* Use 3-bit perturb per channel mapped to -2..+2 */
    int pr = (int)((r >> 16) & 0x7) - 4; /* -4..3 -> map to -2..+2 by /2 */
    int pg = (int)((r >> 19) & 0x7) - 4;
    int pb = (int)((r >> 22) & 0x7) - 4;
    pr = pr / 2; pg = pg / 2; pb = pb / 2;

    uint8_t cr, cg, cb;
    unpack_rgb_u8(color, &cr, &cg, &cb);
    int ir = (int)cr + pr; int ig = (int)cg + pg; int ib = (int)cb + pb;
    if (ir < 0) ir = 0; if (ir > 255) ir = 255;
    if (ig < 0) ig = 0; if (ig > 255) ig = 255;
    if (ib < 0) ib = 0; if (ib > 255) ib = 255;
    color = pack_rgb_u8((uint8_t)ir, (uint8_t)ig, (uint8_t)ib);

    /* Rasterize two triangles using integer fixed-point rasterizer */
    rasterize_triangle_fixed(axf, ayf, bxf, byf, cxf, cyf, color);
    rasterize_triangle_fixed(axf, ayf, cxf, cyf, dxf, dyf, color);
}

/* --- Main renderer --- */
void main(KernelGOPInfo *kgi) {
    frontbuffer = (uint32_t*)(uintptr_t)kgi->FrameBufferBase;
    pps = kgi->PixelsPerScanLine;
    screen_width = kgi->Width;
    screen_height = kgi->Height;
    buffer_size_bytes = (size_t)screen_height * (size_t)pps * 4;

    init_paging(kgi);
    init_gdt_tss();
    enable_sse();

    /* allocate backbuffer pages */
    int pages_needed = (int)((buffer_size_bytes + PAGE_SIZE - 1) / PAGE_SIZE);
    void *bb = kernel_alloc_page();
    for (int i = 1; i < pages_needed; ++i) kernel_alloc_page();
    backbuffer = (uint32_t*)bb;
    fbb = (volatile uint32_t*)backbuffer;

    /* clear backbuffer */
    /* Background: hsv(210, 0.6, 0.06) -> s=153, v=15 (approx) */
    uint32_t bg = hsv_to_rgb_int(210, 153, 15);
    for (size_t i = 0; i < (size_t)screen_height * pps; ++i) backbuffer[i] = bg;
    swap_buffers();

    /* Precompute grid world coordinates */
    float spacing = 15.0f;
    float half = (float)GRID_SIZE / 2.0f;
    float grid_x[GRID_SIZE];
    float grid_y[GRID_SIZE];
    for (int i = 0; i < GRID_SIZE; ++i) {
        grid_x[i] = (i - half) * spacing;
        grid_y[i] = (i - half) * spacing;
    }

    /* Points buffer (projected) */
    float points[GRID_SIZE][GRID_SIZE][2];

    /* Projection params */
    const float proj_scale = 400.0f;
    const float proj_zoff = 50.0f;
    const float proj_base = 500.0f;

    /* Time */
    float t = 0.0f;
    const float dt = 0.05f;

    /* Base ocean-blue color (RGB) using integer HSV:
       hsv(200, 0.9, 0.45) -> h=200, s=230, v=114 (approx) */
    uint32_t base_ocean = hsv_to_rgb_int(200, 230, 114);

    /* Main loop */
    while (1) {
        /* clear backbuffer to background */
        uint32_t bgcol = hsv_to_rgb_int(210, 153, 15);
        for (size_t i = 0; i < (size_t)screen_height * pps; ++i) backbuffer[i] = bgcol;

        /* compute displaced grid and project to screen */
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = 0; j < GRID_SIZE; ++j) {
                float x = grid_x[i];
                float y = grid_y[j];
                float ox, oy, oz;
                calculate_gerstner(x, y, t, &ox, &oy, &oz);
                float scale = proj_scale / (proj_base + oy);
                float sx = (float)screen_width * 0.5f + ox * scale;
                float sy = (float)screen_height * 0.5f - (oz - proj_zoff) * scale;
                points[i][j][0] = sx;
                points[i][j][1] = sy;
            }
        }

        /* fill each quad with a slightly jittered ocean-blue */
        for (int i = 0; i < GRID_SIZE - 1; ++i) {
            for (int j = 0; j < GRID_SIZE - 1; ++j) {
                float ax = points[i][j][0], ay = points[i][j][1];
                float bx = points[i+1][j][0], by = points[i+1][j][1];
                float cx = points[i+1][j+1][0], cy = points[i+1][j+1][1];
                float dx = points[i][j+1][0], dy = points[i][j+1][1];

                /* skip degenerate quads outside screen quickly */
                if ((ax < -50 && bx < -50 && cx < -50 && dx < -50) ||
                    (ax > (int)screen_width+50 && bx > (int)screen_width+50 && cx > (int)screen_width+50 && dx > (int)screen_width+50) ||
                    (ay < -50 && by < -50 && cy < -50 && dy < -50) ||
                    (ay > (int)screen_height+50 && by > (int)screen_height+50 && cy > (int)screen_height+50 && dy > (int)screen_height+50)) {
                    continue;
                }

                fill_quad(ax, ay, bx, by, cx, cy, dx, dy, base_ocean);
            }
        }

        /* copy backbuffer to frontbuffer */
        swap_buffers();

        /* advance time */
        t += dt;

        /* throttle (no-op) */
    }
}

/* --- Entry point (naked) --- */
__attribute__((naked)) void _start(void) {
    __asm__ volatile (
        "cli\n\t"
        "mov %rcx, %rdi\n\t" /* UEFI passes GOP pointer in RCX; move to RDI for main */
        "sub $8, %rsp\n\t"
        "call main\n\t"
        "hlt\n\t"
    );
}

