// kernel.c
// Minimal, fast kernel framebuffer renderer: simplified "stormy sea".
// Single-file, no C library, no logging, no dynamic alloc/free beyond kernel_alloc_page.

#include <stdint.h>
#include <stddef.h>

/* --- Config --- */
#define PAGE_PRESENT (1ULL << 0)
#define PAGE_RW      (1ULL << 1)
#define PAGE_PWT     (1ULL << 3)
#define PAGE_PS      (1ULL << 7)

#define HEAP_SIZE   0x1000000
#define PAGE_SIZE   4096

typedef struct {
    uint32_t Width;
    uint32_t Height;
    uint32_t PixelsPerScanLine;
    uint64_t FrameBufferBase;
} KernelGOPInfo;

/* --- Minimal kernel state --- */
static uint64_t pml4[512] __attribute__((aligned(4096)));
static uint8_t page_heap[HEAP_SIZE] __attribute__((aligned(4096)));
static int page_ptr = 0;

static uint32_t *frontbuffer;   /* pointer to actual framebuffer */
static uint32_t *backbuffer;    /* allocated pages for backbuffer */
static volatile uint32_t *fbb;  /* alias used by some helpers */

static uint32_t pps, screen_width, screen_height;
static size_t buffer_size_bytes;

/* --- Tiny font (kept for compatibility but not used) --- */
static const uint8_t font8x8_basic[95][8] = { {0} };

/* --- Very small libc replacements --- */
void *memset(void *dst, int c, size_t n) {
    uint8_t *p = (uint8_t*)dst;
    while (n--) *p++ = (uint8_t)c;
    return dst;
}

/* copy 8-byte chunks (count must be multiple of 8 ideally) */
void memcpy64(void *dst, const void *src, size_t bytes) {
    uint64_t *d = (uint64_t*)dst;
    const uint64_t *s = (const uint64_t*)src;
    size_t n = bytes / 8;
    while (n--) *d++ = *s++;
    /* tail bytes */
    size_t rem = bytes & 7;
    if (rem) {
        uint8_t *db = (uint8_t*)d;
        const uint8_t *sb = (const uint8_t*)s;
        while (rem--) *db++ = *sb++;
    }
}

/* simple page allocator */
void *kernel_alloc_page(void) {
    if (page_ptr + PAGE_SIZE > HEAP_SIZE) return 0;
    void *p = &page_heap[page_ptr];
    page_ptr += PAGE_SIZE;
    memset(p, 0, PAGE_SIZE);
    return p;
}

/* swap: copy backbuffer to frontbuffer */
static inline void swap_buffers(void) {
    memcpy64((void*)frontbuffer, (const void*)backbuffer, buffer_size_bytes);
}

/* --- cheap sinf approximation (range reduction + 4th order poly) --- */
static float sinf(float x) {
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

/* clamp */
static inline float clampf(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

/* HSV -> RGB (H degrees 0..360, S/V 0..1) */
static uint32_t hsv_to_rgb(float h, float s, float v) {
    while (h < 0.0f) h += 360.0f;
    while (h >= 360.0f) h -= 360.0f;
    int i = (int)(h / 60.0f) % 6;
    float f = (h / 60.0f) - (float)i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);
    float r=0,g=0,b=0;
    switch (i) {
        case 0: r=v; g=t; b=p; break;
        case 1: r=q; g=v; b=p; break;
        case 2: r=p; g=v; b=t; break;
        case 3: r=p; g=q; b=v; break;
        case 4: r=t; g=p; b=v; break;
        default: r=v; g=p; b=q; break;
    }
    uint8_t R = (uint8_t)(clampf(r,0.0f,1.0f) * 255.0f);
    uint8_t G = (uint8_t)(clampf(g,0.0f,1.0f) * 255.0f);
    uint8_t B = (uint8_t)(clampf(b,0.0f,1.0f) * 255.0f);
    return ((uint32_t)R << 16) | ((uint32_t)G << 8) | (uint32_t)B;
}

/* small cubic positive boost for crests */
static inline float pos_cubic(float v) {
    if (v <= 0.0f) return 0.0f;
    return v * v * v;
}

/* --- minimal GDT/TSS/paging stubs (kept small) --- */
struct GDTEntry { uint16_t a,b; uint8_t c,d,e,f; } __attribute__((packed));
struct GDTSystem { struct GDTEntry low; uint32_t up, r; } __attribute__((packed));
struct GDTPtr { uint16_t limit; uint64_t base; } __attribute__((packed));
struct TSS { uint32_t r0; uint64_t rsp0; uint64_t rsp1; uint64_t rsp2; uint64_t r1; uint64_t ist[7]; uint16_t r2; uint16_t iopb; } __attribute__((packed));

static struct { struct GDTEntry n, c, d; struct GDTSystem t; } __attribute__((packed, aligned(8))) gdt;
static struct GDTPtr gdtptr;
static struct TSS tss;
static uint8_t tss_stack[PAGE_SIZE];

/* init gdt/tss minimal */
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

/* init simple 1:1 paging for first few GB and mark framebuffer pages as write-through */
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

/* --- Simplified, faster renderer --- */
/* Strategy:
   - Compute per-row base brightness using 3 sin components (large/mid/small).
   - For each row compute a single 32-bit RGB value (HSV->RGB).
   - Fill the row by writing two pixels at a time using 64-bit stores.
   - Add cheap per-pixel noise/specular using integer ops (no extra sinf calls).
*/

void main(KernelGOPInfo *kgi) {
    frontbuffer = (uint32_t*)(uintptr_t)kgi->FrameBufferBase;
    pps = kgi->PixelsPerScanLine;
    screen_width = kgi->Width;
    screen_height = kgi->Height;
    buffer_size_bytes = (size_t)screen_height * (size_t)pps * 4;

    init_paging(kgi);
    init_gdt_tss();

    /* allocate contiguous pages for backbuffer */
    int pages_needed = (int)((buffer_size_bytes + PAGE_SIZE - 1) / PAGE_SIZE);
    void *bb = kernel_alloc_page();
    for (int i = 1; i < pages_needed; ++i) kernel_alloc_page();
    backbuffer = (uint32_t*)bb;
    fbb = (volatile uint32_t*)backbuffer;

    /* parameters (kept simple) */
    const float hue = 200.0f;
    const float sat = 1.0f;
    const float base_val = 0.6f;
    float t = 0.0f;
    const float dt = 0.12f; /* faster animation */

    const float f1 = 0.035f, a1 = 0.55f, s1 = 0.9f;
    const float f2 = 0.12f,  a2 = 0.22f, s2 = 1.6f;
    const float f3 = 0.45f,  a3 = 0.08f, s3 = 2.8f;

    const float foam_threshold = 0.65f;
    const float foam_boost = 0.45f;
    const float depth_darkening = 0.45f;
    const float horizon_fade = 0.6f;

    /* clear backbuffer once */
    for (size_t i = 0; i < (size_t)screen_height * pps; ++i) backbuffer[i] = 0x000000;
    swap_buffers();

    /* main loop: compute per-row color and fill row quickly */
    while (1) {
        /* for each row compute a single rgb and fill row */
        for (uint32_t y = 0; y < screen_height; ++y) {
            float yf = (float)y;

            /* depth factor: nearer rows brighter */
            float depth_factor = 1.0f - ((float)y / (float)screen_height) * horizon_fade;
            if (depth_factor < 0.0f) depth_factor = 0.0f;

            /* three wave components per row (only 3 sinf calls per row) */
            float w = a1 * sinf(yf * f1 - t * s1)
                    + a2 * sinf(yf * f2 - t * s2 + 1.7f)
                    + a3 * sinf(yf * f3 - t * s3 + 0.9f);
            if (w < -1.0f) w = -1.0f;
            if (w >  1.0f) w =  1.0f;

            float row_base = base_val + w * 0.6f;
            if (row_base < 0.0f) row_base = 0.0f;
            if (row_base > 1.0f) row_base = 1.0f;

            /* crest/foam boost (per-row scalar) */
            float crest = (row_base - foam_threshold);
            if (crest < 0.0f) crest = 0.0f;
            else crest = crest * crest * crest * foam_boost; /* cubic */

            /* depth darkening scalar */
            float depth_dark = 1.0f - depth_darkening * (1.0f - depth_factor);

            /* compute a base RGB for the row (will be modulated per-pixel cheaply) */
            float final_v_row = row_base + crest;
            if (final_v_row > 1.0f) final_v_row = 1.0f;
            final_v_row *= depth_dark;

            uint32_t rgb_row = hsv_to_rgb(hue, sat, final_v_row);

            /* prepare 64-bit pattern: two identical pixels */
            uint64_t pattern = ((uint64_t)rgb_row << 32) | (uint64_t)rgb_row;

            /* fill row quickly: write two pixels per iteration */
            uint32_t *dst = backbuffer + (size_t)y * pps;
            uint32_t pairs = screen_width / 2;
            uint32_t rem = screen_width & 1U;

            uint64_t *dst64 = (uint64_t*)dst;
            for (uint32_t i = 0; i < pairs; ++i) {
                dst64[i] = pattern;
            }

            /* handle odd pixel */
            if (rem) {
                dst[screen_width - 1] = rgb_row;
            }

            /* cheap per-pixel sparkle: every Nth row add tiny highlight on a few pixels
               (we avoid per-pixel floating math by toggling a few pixels using integer ops) */
            if ((y + (int)t) & 7U) continue; /* only on some rows */
            /* set a few bright pixels across the row to simulate specular highlights */
            uint32_t step = (screen_width / 8) + 1;
            for (uint32_t x = 0; x < screen_width; x += step) {
                uint32_t idx = (size_t)y * pps + x;
                /* brighten by mixing with white (simple average) */
                uint32_t px = backbuffer[idx];
                uint32_t r = ((px >> 16) & 0xFF);
                uint32_t g = ((px >> 8) & 0xFF);
                uint32_t b = (px & 0xFF);
                /* average with white scaled by small factor */
                r = (r + 255) >> 1;
                g = (g + 255) >> 1;
                b = (b + 255) >> 1;
                backbuffer[idx] = (r << 16) | (g << 8) | b;
            }
        }

        /* copy to frontbuffer (frameflip) */
        swap_buffers();

        /* advance time */
        t += dt;

        /* simple delay loop (keeps CPU busy but deterministic) */
        for (volatile int d = 0; d < 12000; ++d) { __asm__ volatile("nop"); }
    }
}

/* --- minimal entry point --- */
__attribute__((naked)) void _start(void) {
    __asm__ volatile (
        "cli\n\t"            /* disable interrupts */
        "mov %rcx, %rdi\n\t" /* pass pointer from RCX to RDI (UEFI convention) */
        "sub $8, %rsp\n\t"
        "call main\n\t"
        "hlt\n\t"
    );
}

