// kernel.c
// Gerstner wave grid renderer for a libc-free kernel environment.
// No stdio, no file I/O, no math.h, no SDL. All allocations via kernel_alloc_page.
// Renders directly to the framebuffer provided by KernelGOPInfo.
//
// Entry: _start (UEFI passes GOP pointer in RCX -> moved to RDI for main)

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

/* --- Kernel static heap and simple page allocator --- */
static uint8_t page_heap[HEAP_SIZE] __attribute__((aligned(4096)));
static int page_ptr = 0;
void *kernel_alloc_page(void) {
    if (page_ptr + PAGE_SIZE > HEAP_SIZE) return 0;
    void *p = &page_heap[page_ptr];
    page_ptr += PAGE_SIZE;
    /* simple memset */
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

/* --- Very small GDT/TSS/paging helpers (compact) --- */
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

/* --- Minimal math: sinf/cosf approximation (range reduction + 4th-order poly) --- */
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

/* --- HSV to RGB --- */
static uint32_t hsv_to_rgb(float h, float s, float v) {
    while (h < 0.0f) h += 360.0f;
    while (h >= 360.0f) h -= 360.0f;
    int i = (int)(h / 60.0f) % 6;
    float f = (h / 60.0f) - (float)i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);
    float r=0,g=0,b=0;
    switch(i){
        case 0: r=v; g=t; b=p; break;
        case 1: r=q; g=v; b=p; break;
        case 2: r=p; g=v; b=t; break;
        case 3: r=p; g=q; b=v; break;
        case 4: r=t; g=p; b=v; break;
        case 5: r=v; g=p; b=q; break;
    }
    uint8_t R = (uint8_t)(clampf(r,0.0f,1.0f) * 255.0f);
    uint8_t G = (uint8_t)(clampf(g,0.0f,1.0f) * 255.0f);
    uint8_t B = (uint8_t)(clampf(b,0.0f,1.0f) * 255.0f);
    return ((uint32_t)R << 16) | ((uint32_t)G << 8) | (uint32_t)B;
}

/* --- Gerstner wave definition --- */
typedef struct {
    float amplitude;
    float phase;
    float frequency;
    float dirX, dirY;
    float steepness;
} Wave;

static Wave waves[4] = {
    {15.0f, 0.5f, 0.05f, 1.0f, 0.2f, 0.4f},
    {10.0f, 1.2f, 0.08f, 0.5f, 0.8f, 0.3f},
    {5.0f, 0.2f, 0.15f, -0.7f, 0.3f, 0.2f},
    {8.0f, 0.8f, 0.1f, 0.1f, 1.0f, 0.25f}
};

/* Gerstner displacement (no external libs) */
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

/* --- Simple pixel and line drawing (integer) --- */
static inline void put_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= (int)screen_width || y < 0 || y >= (int)screen_height) return;
    frontbuffer[y * pps + x] = color;
}
static void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = x1 - x0; int sx = dx >= 0 ? 1 : -1; if (dx < 0) dx = -dx;
    int dy = y1 - y0; int sy = dy >= 0 ? 1 : -1; if (dy < 0) dy = -dy;
    int err = (dx > dy ? dx : -dy) / 2;
    while (1) {
        put_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 <  dy) { err += dx; y0 += sy; }
    }
}

/* --- Main renderer: compute grid, project, draw lines --- */
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

    /* clear backbuffer and frontbuffer initially */
    for (size_t i = 0; i < (size_t)screen_height * pps; ++i) backbuffer[i] = 0x00102030;
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

    /* Main loop: compute grid, draw lines, swap */
    while (1) {
        /* clear backbuffer to dark blue */
        uint32_t bg = hsv_to_rgb(210.0f, 0.6f, 0.06f);
        for (size_t i = 0; i < (size_t)screen_height * pps; ++i) backbuffer[i] = bg;

        /* compute displaced grid and project */
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

        /* draw grid lines into backbuffer using integer Bresenham */
        uint32_t line_color = hsv_to_rgb(200.0f, 0.9f, 0.6f);
        for (int i = 0; i < GRID_SIZE - 1; ++i) {
            for (int j = 0; j < GRID_SIZE - 1; ++j) {
                int x0 = (int)(points[i][j][0] + 0.5f);
                int y0 = (int)(points[i][j][1] + 0.5f);
                int x1 = (int)(points[i+1][j][0] + 0.5f);
                int y1 = (int)(points[i+1][j][1] + 0.5f);
                /* draw horizontal neighbor */
                /* draw into backbuffer directly */
                /* temporarily set frontbuffer pointer to backbuffer for drawing convenience */
                uint32_t *saved_front = frontbuffer;
                frontbuffer = backbuffer;
                draw_line(x0, y0, x1, y1, line_color);
                /* vertical neighbor */
                int x2 = (int)(points[i][j+1][0] + 0.5f);
                int y2 = (int)(points[i][j+1][1] + 0.5f);
                draw_line(x0, y0, x2, y2, line_color);
                frontbuffer = saved_front;
            }
        }

        /* swap backbuffer -> frontbuffer (frameflip) */
        swap_buffers();

        /* advance time */
        t += dt;

        /* small busy-wait to throttle */
        for (volatile int d = 0; d < 12000; ++d) { __asm__ volatile("nop"); }
    }
}

/* --- Entry point (naked) --- */
__attribute__((naked)) void _start(void) {
    __asm__ volatile (
        "cli\n\t"            /* disable interrupts */
        "mov %rcx, %rdi\n\t" /* move UEFI RCX (GOP pointer) to RDI for main */
        "sub $8, %rsp\n\t"
        "call main\n\t"
        "hlt\n\t"
    );
}

