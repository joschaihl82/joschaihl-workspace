// kernel_fast_fill.c
// Kernel-mode Gerstner grid renderer using a very fast integer scanline fill
// algorithm (incremental fixed-point edge walking + 64-bit span writes).
// No libc I/O, no math.h, no SDL. All allocations via kernel_alloc_page.
// Focus: fastest practical polygon fill per-scanline for many small quads.

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

typedef struct { uint32_t Width, Height, PixelsPerScanLine; uint64_t FrameBufferBase; } KernelGOPInfo;

/* --- tiny heap & allocator --- */
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

/* --- minimal mem ops --- */
void *memset(void *dst, int c, size_t n) { uint8_t *p=(uint8_t*)dst; while(n--) *p++=(uint8_t)c; return dst; }
void memcpy64(void *dst, const void *src, size_t bytes) {
    uint64_t *d=(uint64_t*)dst; const uint64_t *s=(const uint64_t*)src;
    size_t n = bytes/8; while(n--) *d++ = *s++;
    size_t rem = bytes & 7; if(rem){ uint8_t *db=(uint8_t*)d; const uint8_t *sb=(const uint8_t*)s; while(rem--) *db++ = *sb++; }
}

/* --- GDT/TSS/paging minimal (compact) --- */
struct GDTEntry { uint16_t a,b; uint8_t c,d,e,f; } __attribute__((packed));
struct GDTSystem { struct GDTEntry low; uint32_t up,r; } __attribute__((packed));
struct GDTPtr { uint16_t limit; uint64_t base; } __attribute__((packed));
struct TSS { uint32_t r0; uint64_t rsp0,rsp1,rsp2,r1; uint64_t ist[7]; uint16_t r2,iopb; } __attribute__((packed));
static struct { struct GDTEntry n,c,d; struct GDTSystem t; } __attribute__((packed,aligned(8))) gdt;
static struct GDTPtr gdtptr; static struct TSS tss; static uint8_t tss_stack[PAGE_SIZE];
void init_gdt_tss(void) {
    gdt.c.d=0x20; gdt.c.c=0x9A; gdt.d.c=0x92;
    uint64_t base=(uint64_t)&tss; tss.rsp0=(uint64_t)tss_stack+PAGE_SIZE;
    gdt.t.low.a = sizeof(tss)-1; gdt.t.low.b=(uint16_t)base; gdt.t.low.c=(uint8_t)(base>>16);
    gdt.t.low.d=0x89; gdt.t.low.e=(uint8_t)(base>>24); gdt.t.up=(uint32_t)(base>>32);
    gdtptr.limit = sizeof(gdt)-1; gdtptr.base=(uint64_t)&gdt;
    __asm__ volatile("lgdt %0" :: "m"(gdtptr) : "memory");
    __asm__ volatile("mov $0x18, %%ax\n\tltr %%ax" ::: "ax");
}
static uint64_t pml4[512] __attribute__((aligned(4096)));
void init_paging(KernelGOPInfo *kgi) {
    const uint64_t flags = PAGE_PRESENT|PAGE_RW;
    const uint64_t fb_flags = PAGE_PRESENT|PAGE_RW|PAGE_PWT;
    uint64_t *pdpt = (uint64_t*)kernel_alloc_page(); pml4[0] = (uint64_t)pdpt | flags;
    for(int i=0;i<4;i++){ uint64_t *pd=(uint64_t*)kernel_alloc_page(); pdpt[i]=(uint64_t)pd|flags;
        for(int j=0;j<512;j++){ uint64_t addr=(uint64_t)(i*512+j)*0x200000ULL; uint64_t f=(addr>=kgi->FrameBufferBase && addr<kgi->FrameBufferBase+0x2000000ULL)?fb_flags:flags; pd[j]=addr|f|PAGE_PS; }
    }
    __asm__ volatile("mov %0, %%cr3" :: "r"(pml4) : "memory");
}
void enable_sse(void){ uint64_t cr0,cr4; __asm__ volatile("mov %%cr0,%0":"=r"(cr0)); cr0 &= ~(1<<2); cr0 |= (1<<1); __asm__ volatile("mov %0,%%cr0"::"r"(cr0)); __asm__ volatile("mov %%cr4,%0":"=r"(cr4)); cr4 |= (3<<9); __asm__ volatile("mov %0,%%cr4"::"r"(cr4)); }

/* --- fast sin table (used by Gerstner) --- */
#define SIN_BITS 12
#define SIN_SIZE (1<<SIN_BITS)
#define SIN_MASK (SIN_SIZE-1)
static float sin_table[SIN_SIZE];
static void init_sin_table(void){
    const float TWO_PI = 6.28318530717958647692f;
    for(int i=0;i<SIN_SIZE;i++){
        float a = (TWO_PI * (float)i) / (float)SIN_SIZE;
        float x=a; if(x>3.14159265f) x -= TWO_PI;
        float x2=x*x; float x4=x2*x2;
        sin_table[i] = x * (1.0f - x2*(1.0f/6.0f) + x4*(1.0f/120.0f));
    }
}
static inline float sin_fast(float x){
    const float INV_TWO_PI = 0.15915494309189533577f;
    float f = x * INV_TWO_PI * (float)SIN_SIZE;
    int idx = (int)f;
    float frac = f - (float)idx;
    idx &= SIN_MASK;
    int idx2 = (idx+1)&SIN_MASK;
    return sin_table[idx] + (sin_table[idx2]-sin_table[idx]) * frac;
}
static inline float cos_fast(float x){ const float PI_2 = 1.57079632679489661923f; return sin_fast(x + PI_2); }

/* --- Gerstner waves --- */
typedef struct { float amplitude, phase, frequency, dirX, dirY, steepness; } Wave;
static Wave waves[4] = {
    {15.0f,0.5f,0.05f,1.0f,0.2f,0.4f},
    {10.0f,1.2f,0.08f,0.5f,0.8f,0.3f},
    {5.0f,0.2f,0.15f,-0.7f,0.3f,0.2f},
    {8.0f,0.8f,0.1f,0.1f,1.0f,0.25f}
};
static void calculate_gerstner(float x, float y, float time, float *ox, float *oy, float *oz) {
    *ox = x; *oy = y; *oz = 0.0f;
    for (int i = 0; i < 4; ++i) {
        Wave *w = &waves[i];
        float dot = (x * w->dirX + y * w->dirY) * w->frequency;
        float theta = dot + time * w->phase;
        float c = cos_fast(theta);
        float s = sin_fast(theta);
        float q = w->steepness / (w->frequency * w->amplitude * 4.0f);
        *ox += q * w->amplitude * w->dirX * c;
        *oy += q * w->amplitude * w->dirY * c;
        *oz += w->amplitude * s;
    }
}

/* --- RNG xorshift for color jitter --- */
static uint32_t rng_state = 0xC0FFEEu;
static inline uint32_t xorshift32(void){ uint32_t x=rng_state; x ^= x<<13; x ^= x>>17; x ^= x<<5; return rng_state = x; }

/* --- fast HSV->RGB (approx) --- */
static inline uint32_t hsv_to_rgb_fast(float h, float s, float v){
    while(h<0) h+=360.0f; while(h>=360.0f) h-=360.0f;
    int i = (int)(h/60.0f) % 6;
    float f = (h/60.0f) - (float)i;
    float p = v*(1.0f - s);
    float q = v*(1.0f - f*s);
    float t = v*(1.0f - (1.0f - f)*s);
    float rf=0,gf=0,bf=0;
    switch(i){ case 0: rf=v; gf=t; bf=p; break; case 1: rf=q; gf=v; bf=p; break; case 2: rf=p; gf=v; bf=t; break;
                case 3: rf=p; gf=q; bf=v; break; case 4: rf=t; gf=p; bf=v; break; default: rf=v; gf=p; bf=q; break; }
    return ((uint32_t)(rf*255.0f)<<16) | ((uint32_t)(gf*255.0f)<<8) | (uint32_t)(bf*255.0f);
}

/* --- framebuffer state --- */
static uint32_t *frontbuffer = 0, *backbuffer = 0; static volatile uint32_t *fbb = 0;
static uint32_t pps = 0, screen_width = 0, screen_height = 0; static size_t buffer_size_bytes = 0;
static inline void swap_buffers(void){ memcpy64((void*)frontbuffer,(const void*)backbuffer,buffer_size_bytes); }

/* --- helpers: clamp and 64-bit span write --- */
static inline uint32_t clamp8(int v){ if(v<0) v=0; if(v>255) v=255; return (uint32_t)v; }
static inline void write_span(int y, int x0, int x1, uint32_t color){
    if(y<0 || y>=(int)screen_height) return;
    if(x0<0) x0=0; if(x1>=(int)screen_width) x1 = screen_width-1;
    if(x0>x1) return;
    uint32_t *dst = backbuffer + (size_t)y * pps + x0;
    uint64_t pat = ((uint64_t)color << 32) | color;
    int len = x1 - x0 + 1;
    int pairs = len / 2;
    for(int i=0;i<pairs;++i) { ((uint64_t*)dst)[i] = pat; }
    if(len & 1) dst[len-1] = color;
}

/* --- Fast triangle fill using incremental fixed-point edge walking.
       This is a well-known high-performance approach: sort vertices by Y,
       compute fixed-point X increments for left/right edges and fill spans per scanline.
       Uses 16.16 fixed point for subpixel precision and integer arithmetic in the inner loop. */
static inline int to_fixed(float v){ return (int)(v * 65536.0f); }
static void fill_triangle_fast(float axf,float ayf,float bxf,float byf,float cxf,float cyf,uint32_t color){
    /* convert to fixed */
    int ax = to_fixed(axf), ay = to_fixed(ayf);
    int bx = to_fixed(bxf), by = to_fixed(byf);
    int cx = to_fixed(cxf), cy = to_fixed(cyf);

    /* sort vertices by Y ascending (ay <= by <= cy) */
    if (ay > by) { int t; t=ay; ay=by; by=t; t=ax; ax=bx; bx=t; }
    if (ay > cy) { int t; t=ay; ay=cy; cy=t; t=ax; ax=cx; cx=t; }
    if (by > cy) { int t; t=by; by=cy; cy=t; t=bx; bx=cx; cx=t; }

    if (ay == cy) return; /* degenerate */

    /* compute deltas */
    int dy_ab = by - ay;
    int dy_ac = cy - ay;
    int dy_bc = cy - by;

    /* edge increments (fixed-point dx per 1 in Y fixed units) */
    int dx_ab = (dy_ab != 0) ? ((bx - ax) / dy_ab) : 0;
    int dx_ac = (dy_ac != 0) ? ((cx - ax) / dy_ac) : 0;
    int dx_bc = (dy_bc != 0) ? ((cx - bx) / dy_bc) : 0;

    /* starting x positions */
    int x_left = ax;
    int x_right = ax;

    /* determine which side is left/right for upper part by comparing slopes */
    int left_dx = dx_ac, right_dx = dx_ab;
    if (dx_ab > dx_ac) { left_dx = dx_ab; right_dx = dx_ac; /* swap */ }

    /* Upper part: from ay to by (exclusive of by) */
    int y_start = (ay + 65535) >> 16;
    int y_end = (by + 65535) >> 16;
    if (y_start < 0) y_start = 0;
    if (y_end > (int)screen_height) y_end = screen_height;
    /* initialize x_left/x_right to value at y_start */
    int scan_y_fixed = (y_start << 16) + 32768; /* center of pixel */
    /* compute initial x positions by stepping from ay to scan_y_fixed */
    int step = scan_y_fixed - ay;
    x_left = ax + left_dx * step;
    x_right = ax + right_dx * step;

    for (int y = y_start; y < y_end; ++y) {
        int xl = x_left >> 16;
        int xr = x_right >> 16;
        if (xl > xr) { int t = xl; xl = xr; xr = t; }
        write_span(y, xl, xr, color);
        x_left += left_dx * 65536; /* advance by 1.0 in fixed Y */
        x_right += right_dx * 65536;
    }

    /* Lower part: from by to cy */
    if (by < cy) {
        /* determine left/right for lower part */
        /* left edge continues from ac or ab depending on earlier comparison */
        /* recompute left/right starting at by */
        int y_start2 = (by + 65535) >> 16;
        int y_end2 = (cy + 65535) >> 16;
        if (y_start2 < 0) y_start2 = 0;
        if (y_end2 > (int)screen_height) y_end2 = screen_height;

        /* compute x at by for both edges */
        /* x on AC at by: ax + dx_ac * (by - ay) */
        int x_ac_at_by = ax + dx_ac * (by - ay);
        int x_bc_at_by = bx; /* start of BC */
        /* determine which is left */
        int left_dx2, right_dx2;
        int x_left2, x_right2;
        if (dx_ac < dx_bc) {
            /* AC is left, BC is right */
            left_dx2 = dx_ac;
            right_dx2 = dx_bc;
            x_left2 = x_ac_at_by;
            x_right2 = x_bc_at_by;
        } else {
            left_dx2 = dx_bc;
            right_dx2 = dx_ac;
            x_left2 = x_bc_at_by;
            x_right2 = x_ac_at_by;
        }

        /* step to first scanline center */
        int scan_y_fixed2 = (y_start2 << 16) + 32768;
        int step2 = scan_y_fixed2 - by;
        x_left2 += left_dx2 * step2;
        x_right2 += right_dx2 * step2;

        for (int y = y_start2; y < y_end2; ++y) {
            int xl = x_left2 >> 16;
            int xr = x_right2 >> 16;
            if (xl > xr) { int t = xl; xl = xr; xr = t; }
            write_span(y, xl, xr, color);
            x_left2 += left_dx2 * 65536;
            x_right2 += right_dx2 * 65536;
        }
    }
}

/* --- fill quad by splitting into two triangles and applying tiny color jitter --- */
static inline uint32_t mix_color_jitter(uint32_t base){
    uint32_t r = xorshift32();
    int j = (int)(r & 31) - 16; /* -16..15 */
    int br = (int)((base>>16)&0xFF) + j;
    int bg = (int)((base>>8)&0xFF) + j;
    int bb = (int)(base & 0xFF) + j;
    if (br<0) br=0; if(br>255) br=255;
    if (bg<0) bg=0; if(bg>255) bg=255;
    if (bb<0) bb=0; if(bb>255) bb=255;
    return ((uint32_t)br<<16) | ((uint32_t)bg<<8) | (uint32_t)bb;
}
static void fill_quad_fast(float ax,float ay,float bx,float by,float cx,float cy,float dx,float dy,uint32_t base_color){
    uint32_t color = mix_color_jitter(base_color);
    /* triangle ABC */
    fill_triangle_fast(ax,ay,bx,by,cx,cy,color);
    /* triangle A C D */
    fill_triangle_fast(ax,ay,cx,cy,dx,dy,color);
}

/* --- main renderer --- */
void main(KernelGOPInfo *kgi){
    frontbuffer = (uint32_t*)(uintptr_t)kgi->FrameBufferBase;
    pps = kgi->PixelsPerScanLine;
    screen_width = kgi->Width; screen_height = kgi->Height;
    buffer_size_bytes = (size_t)screen_height * (size_t)pps * 4;

    init_paging(kgi); init_gdt_tss(); enable_sse(); init_sin_table();

    /* allocate backbuffer */
    int pages_needed = (int)((buffer_size_bytes + PAGE_SIZE - 1) / PAGE_SIZE);
    void *bb = kernel_alloc_page(); for(int i=1;i<pages_needed;++i) kernel_alloc_page();
    backbuffer = (uint32_t*)bb; fbb = (volatile uint32_t*)backbuffer;

    /* precompute grid */
    float spacing = 15.0f; float half = (float)GRID_SIZE/2.0f;
    float grid_x[GRID_SIZE], grid_y[GRID_SIZE];
    for(int i=0;i<GRID_SIZE;++i){ grid_x[i] = (i-half)*spacing; grid_y[i] = (i-half)*spacing; }

    /* projection params */
    const float proj_scale = 400.0f, proj_zoff = 50.0f, proj_base = 500.0f;
    float points[GRID_SIZE][GRID_SIZE][2];

    /* base ocean color */
    uint32_t base_ocean = hsv_to_rgb_fast(200.0f,0.9f,0.45f);

    float t = 0.0f; const float dt = 0.06f;

    /* main loop */
    while(1){
        /* clear backbuffer to dark */
        uint32_t bg = hsv_to_rgb_fast(210.0f,0.6f,0.06f);
        for(size_t i=0;i<(size_t)screen_height*pps;++i) backbuffer[i]=bg;

        /* compute displaced grid and project */
        for(int i=0;i<GRID_SIZE;++i){
            for(int j=0;j<GRID_SIZE;++j){
                float x = grid_x[i], y = grid_y[j];
                float ox, oy, oz;
                calculate_gerstner(x,y,t,&ox,&oy,&oz);
                float scale = proj_scale / (proj_base + oy);
                float sx = (float)screen_width * 0.5f + ox * scale;
                float sy = (float)screen_height * 0.5f - (oz - proj_zoff) * scale;
                points[i][j][0] = sx; points[i][j][1] = sy;
            }
        }

        /* fill quads fast */
        for(int i=0;i<GRID_SIZE-1;++i){
            for(int j=0;j<GRID_SIZE-1;++j){
                float ax = points[i][j][0], ay = points[i][j][1];
                float bx = points[i+1][j][0], by = points[i+1][j][1];
                float cx = points[i+1][j+1][0], cy = points[i+1][j+1][1];
                float dx = points[i][j+1][0], dy = points[i][j+1][1];
                /* quick cull */
                if((ax< -64 && bx< -64 && cx< -64 && dx< -64) || (ax>screen_width+64 && bx>screen_width+64 && cx>screen_width+64 && dx>screen_width+64)) continue;
                if((ay< -64 && by< -64 && cy< -64 && dy< -64) || (ay>screen_height+64 && by>screen_height+64 && cy>screen_height+64 && dy>screen_height+64)) continue;
                fill_quad_fast(ax,ay,bx,by,cx,cy,dx,dy,base_ocean);
            }
        }

        /* present */
        swap_buffers();

        /* advance time */
        t += dt;

        /* throttle */
    }
}

/* --- entry point --- */
__attribute__((naked)) void _start(void){
    __asm__ volatile("cli\n\tmov %rcx, %rdi\n\tsub $8, %rsp\n\tcall main\n\thlt");
}

