// kernel/kernel.c [OPTIMIZED]
// Final merged kernel.c with framebuffer optimization
// - Framebuffer Write-Combining via PAT MSR
// - corrected ISR stubs (vectors 0..31)
// - keyboard IRQ1 handler with Ctrl+Alt+Del reboot
// - robust bump allocator (contiguous pages)
// - framebuffer mapped to virtual VFB_BASE
// - double-buffered rendering with optimized memcpy_fast & dirty-rectangle presents
// - optimized fb_present_dirty with 64-bit chunks (no glibc dependency)

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

/* --- Config --- */
#define PAGE_PRESENT (1ULL << 0)
#define PAGE_RW      (1ULL << 1)
#define PAGE_PWT     (1ULL << 3)
#define PAGE_PCD     (1ULL << 4)
#define PAGE_PAT     (1ULL << 7)
#define PAGE_PS      (1ULL << 7)  /* Page Size (in PDE) - same bit as PAT but different context */

#define CHAR_WIDTH   16
#define CHAR_HEIGHT  20
#define MARGIN_X     20
#define MARGIN_Y     20

#define VFB_BASE ((uint64_t)0xFFFF800000000000ULL)
#define FB_MAP_SIZE (0x2000000ULL) /* 32 MiB mapping window */

/* --- Types --- */
typedef struct {
    uint32_t Width;
    uint32_t Height;
    uint32_t PixelsPerScanLine;
    uint64_t FrameBufferBase;
} KernelGOPInfo;

/* --- GDT/TSS --- */
struct GDTEntry {
    uint16_t limit_low, base_low;
    uint8_t  base_mid, access, gran, base_high;
} __attribute__((packed));

struct GDTSystemDescriptor {
    struct GDTEntry low;
    uint32_t base_upper32, reserved;
} __attribute__((packed));

struct GDTPtr { uint16_t limit; uint64_t base; } __attribute__((packed));

struct TSS {
    uint32_t res0;
    uint64_t rsp0, rsp1, rsp2, res1;
    uint64_t ist[7];
    uint64_t res2;
    uint16_t res3, iopb_offset;
} __attribute__((packed));

/* --- Globals --- */
static struct {
    struct GDTEntry null, code, data;
    struct GDTSystemDescriptor tss;
} __attribute__((packed, aligned(8))) gdt_table;

static struct GDTPtr gdt_ptr;
static struct TSS kernel_tss;
static uint8_t tss_stack[4096];

uint64_t pml4[512] __attribute__((aligned(4096)));

uint32_t pps, screen_width, screen_height;
volatile uint32_t *fbb = NULL; /* mapped physical framebuffer at VFB_BASE + offset */

static uint32_t cursor_x = MARGIN_X, cursor_y = MARGIN_Y;

/* --- Simple robust bump allocator (contiguous pages) --- */
#define KERNEL_HEAP_BYTES (16 * 1024 * 1024) /* 16 MiB heap for backbuffer etc. */
static uint8_t page_heap[KERNEL_HEAP_BYTES] __attribute__((aligned(4096)));
static size_t page_ptr = 0; /* bytes used */

/* Allocate `pages` contiguous 4 KiB pages, zeroed. Return NULL on failure. */
void* kernel_alloc_pages(size_t pages) {
    const size_t PAGE = 4096;
    if (pages == 0) return NULL;
    size_t need = pages * PAGE;
    size_t aligned_ptr = (page_ptr + (PAGE - 1)) & ~(PAGE - 1);
    if (aligned_ptr + need > KERNEL_HEAP_BYTES) return NULL;
    void* ptr = &page_heap[aligned_ptr];
    page_ptr = aligned_ptr + need;
    uint8_t *p = (uint8_t*)ptr;
    for (size_t i = 0; i < need; ++i) p[i] = 0;
    return ptr;
}

void* kernel_alloc_page(void) { return kernel_alloc_pages(1); }
void* kernel_alloc_bytes(size_t bytes) {
    const size_t PAGE = 4096;
    size_t pages = (bytes + PAGE - 1) / PAGE;
    return kernel_alloc_pages(pages);
}

/* --- Forward declarations --- */
void draw_char(char c);
void serial_putc(char c);
int kgetc(void);
int kgets(char *buf, int maxlen);
void printf(const char* fmt, ...);
void exception_handler_c(int vec, uint64_t err);
void init_pic_idt_keyboard(void);
void init_gdt_tss(void);
void init_paging(KernelGOPInfo *kgi);
void serial_init(void);
void setup_pat_msr(void);

/* --- Font --- */
static const uint8_t font8x8_basic[95][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},{0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00},{0x36,0x36,0x00,0x00,0x00,0x00,0x00,0x00},{0x36,0x36,0x7F,0x36,0x7F,0x36,0x36,0x00},
    {0x0C,0x3E,0x03,0x1E,0x30,0x1F,0x0C,0x00},{0x00,0x63,0x33,0x18,0x0C,0x66,0x63,0x00},{0x1C,0x36,0x1C,0x3B,0x6E,0x66,0x3B,0x00},{0x06,0x06,0x03,0x00,0x00,0x00,0x00,0x00},
    {0x18,0x0C,0x06,0x06,0x06,0x0C,0x18,0x00},{0x06,0x0C,0x18,0x18,0x18,0x0C,0x06,0x00},{0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00},{0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x0C},{0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00},{0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00},{0x00,0x40,0x30,0x18,0x0C,0x06,0x02,0x00},
    {0x3E,0x63,0x67,0x6B,0x73,0x63,0x3E,0x00},{0x0C,0x1C,0x0C,0x0C,0x0C,0x0C,0x3F,0x00},{0x3E,0x63,0x03,0x1E,0x30,0x60,0x7F,0x00},{0x3E,0x63,0x03,0x1E,0x03,0x63,0x3E,0x00},
    {0x06,0x0E,0x16,0x26,0x7F,0x06,0x06,0x00},{0x7F,0x60,0x7E,0x03,0x03,0x63,0x3E,0x00},{0x1C,0x30,0x60,0x7E,0x63,0x63,0x3E,0x00},{0x7F,0x63,0x03,0x06,0x0C,0x18,0x18,0x00},
    {0x3E,0x63,0x63,0x3E,0x63,0x63,0x3E,0x00},{0x3E,0x63,0x63,0x3F,0x03,0x06,0x3C,0x00},{0x00,0x18,0x18,0x00,0x18,0x18,0x00,0x00},{0x00,0x18,0x18,0x00,0x18,0x18,0x0C,0x00},
    {0x00,0x06,0x0C,0x18,0x30,0x18,0x0C,0x06},{0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00},{0x00,0x60,0x30,0x18,0x0C,0x18,0x30,0x60},{0x3E,0x63,0x03,0x06,0x0C,0x00,0x0C,0x00},
    {0x3E,0x63,0x63,0x6F,0x6B,0x6E,0x30,0x00},{0x0C,0x1E,0x33,0x33,0x3F,0x33,0x33,0x00},{0x3F,0x66,0x66,0x3E,0x66,0x66,0x3F,0x00},{0x3E,0x63,0x60,0x60,0x60,0x63,0x3E,0x00},
    {0x3E,0x66,0x66,0x66,0x66,0x66,0x3E,0x00},{0x7F,0x60,0x60,0x7E,0x60,0x60,0x7F,0x00},{0x7F,0x60,0x60,0x7E,0x60,0x60,0x60,0x00},{0x3E,0x63,0x60,0x6E,0x63,0x63,0x3E,0x00},
    {0x66,0x66,0x66,0x7F,0x66,0x66,0x66,0x00},{0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00},{0x1E,0x0C,0x0C,0x0C,0x0C,0x6C,0x38,0x00},{0x66,0x6C,0x78,0x70,0x78,0x6C,0x66,0x00},
    {0x60,0x60,0x60,0x60,0x60,0x60,0x7F,0x00},{0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x00},{0x63,0x67,0x6F,0x7B,0x73,0x63,0x63,0x00},{0x3E,0x63,0x63,0x63,0x63,0x63,0x3E,0x00},
    {0x3F,0x66,0x66,0x3E,0x60,0x60,0x60,0x00},{0x3E,0x63,0x63,0x63,0x6B,0x67,0x3E,0x02},{0x3F,0x66,0x66,0x3E,0x6C,0x66,0x63,0x00},{0x3E,0x63,0x60,0x3E,0x03,0x63,0x3E,0x00},
    {0x7F,0x18,0x18,0x18,0x18,0x18,0x18,0x00},{0x66,0x66,0x66,0x66,0x66,0x66,0x3E,0x00},{0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00},{0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00},
    {0x63,0x63,0x36,0x1C,0x36,0x63,0x63,0x00},{0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00},{0x7F,0x06,0x0C,0x18,0x30,0x60,0x7F,0x00},{0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00},
    {0x00,0x02,0x06,0x0C,0x18,0x30,0x40,0x00},{0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00},{0x08,0x1C,0x36,0x63,0x00,0x00,0x00,0x00},{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF},
    {0x0C,0x0C,0x18,0x00,0x00,0x00,0x00,0x00},{0x00,0x00,0x3E,0x03,0x3F,0x63,0x3E,0x00},{0x60,0x60,0x7E,0x63,0x63,0x63,0x7E,0x00},{0x00,0x00,0x3E,0x63,0x60,0x63,0x3E,0x00},
    {0x03,0x03,0x3F,0x63,0x63,0x63,0x3F,0x00},{0x00,0x00,0x3E,0x63,0x7F,0x60,0x3E,0x00},{0x1C,0x36,0x30,0x78,0x30,0x30,0x30,0x00},{0x00,0x00,0x3F,0x63,0x63,0x3F,0x03,0x3E},
    {0x60,0x60,0x7E,0x63,0x63,0x63,0x63,0x00},{0x18,0x00,0x38,0x18,0x18,0x18,0x3C,0x00},{0x06,0x00,0x06,0x06,0x06,0x66,0x3C,0x00},{0x60,0x60,0x66,0x6C,0x78,0x6C,0x66,0x00},
    {0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00},{0x00,0x00,0x66,0x7F,0x6B,0x6B,0x63,0x00},{0x00,0x00,0x7E,0x63,0x63,0x63,0x63,0x00},{0x00,0x00,0x3E,0x63,0x63,0x63,0x3E,0x00},
    {0x00,0x00,0x7E,0x63,0x63,0x7E,0x60,0x60},{0x00,0x00,0x3F,0x63,0x63,0x3F,0x03,0x03},{0x00,0x00,0x7E,0x63,0x60,0x60,0x60,0x00},{0x00,0x00,0x3E,0x60,0x3E,0x03,0x3E,0x00},
    {0x30,0x30,0x7E,0x30,0x30,0x30,0x1C,0x00},{0x00,0x00,0x63,0x63,0x63,0x63,0x3E,0x00},{0x00,0x00,0x63,0x63,0x63,0x36,0x1C,0x00},{0x00,0x00,0x63,0x6B,0x6B,0x7F,0x36,0x00},
    {0x00,0x00,0x63,0x36,0x1C,0x36,0x63,0x00},{0x00,0x00,0x63,0x63,0x63,0x3F,0x03,0x3E},{0x00,0x00,0x7F,0x0C,0x18,0x30,0x7F,0x00},{0x0C,0x18,0x18,0x30,0x18,0x18,0x0C,0x00},
    {0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x00},{0x30,0x18,0x18,0x0C,0x18,0x18,0x30,0x00},{0x00,0x00,0x3B,0x6E,0x00,0x00,0x00,0x00}
};

/* --- Simple memset --- */
void* memset(void* dest, int ch, size_t count) {
    uint8_t* p = (uint8_t*)dest;
    while (count--) *p++ = (uint8_t)ch;
    return dest;
}

/* --- Optimized memcpy (no glibc dependency) --- */
/* Copy memory with 64-bit chunks, optimized for framebuffer blitting */
static void* memcpy_fast(volatile void* dst, const void* src, size_t bytes) {
    if (bytes == 0) return (void*)dst;
    
    /* Try 64-bit copies if both src and dst are 8-byte aligned */
    if (((uintptr_t)src & 7) == 0 && ((uintptr_t)dst & 7) == 0 && bytes >= 8) {
        uint64_t *s64 = (uint64_t*)src;
        volatile uint64_t *d64 = (volatile uint64_t*)dst;
        size_t count_64 = bytes / 8;
        for (size_t i = 0; i < count_64; ++i) {
            d64[i] = s64[i];
        }
        bytes -= count_64 * 8;
        dst = (volatile uint8_t*)dst + count_64 * 8;
        src = (const uint8_t*)src + count_64 * 8;
    }
    
    /* Copy remaining bytes */
    uint8_t *s8 = (uint8_t*)src;
    volatile uint8_t *d8 = (volatile uint8_t*)dst;
    while (bytes--) *d8++ = *s8++;
    
    return (void*)dst;
}

/* --- Double-buffering state --- */
static uint32_t *backbuf = NULL; /* contiguous RAM buffer */
static size_t backbuf_pixels = 0;

/* Dirty rectangle (inclusive). When empty: min > max. */
static int dirty_min_x = 1, dirty_min_y = 1, dirty_max_x = 0, dirty_max_y = 0;

/* Mark dirty rectangle (clamped) */
static void fb_mark_dirty_rect(int x0, int y0, int x1, int y1) {
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 >= (int)screen_width) x1 = screen_width - 1;
    if (y1 >= (int)screen_height) y1 = screen_height - 1;
    if (dirty_min_x > dirty_max_x) {
        dirty_min_x = x0; dirty_min_y = y0;
        dirty_max_x = x1; dirty_max_y = y1;
    } else {
        if (x0 < dirty_min_x) dirty_min_x = x0;
        if (y0 < dirty_min_y) dirty_min_y = y0;
        if (x1 > dirty_max_x) dirty_max_x = x1;
        if (y1 > dirty_max_y) dirty_max_y = y1;
    }
}

/* Present dirty rectangle from backbuf to fbb (optimized with memcpy) */
static void fb_present_dirty(void) {
    if (!fbb) return;
    if (!backbuf) return; /* single-buffer mode: drawing already in fbb */
    if (dirty_min_x > dirty_max_x) return;

    int x0 = dirty_min_x, y0 = dirty_min_y, x1 = dirty_max_x, y1 = dirty_max_y;
    int width = screen_width;

    for (int y = y0; y <= y1; ++y) {
        uint32_t *src = backbuf + (y * width + x0);
        volatile uint32_t *dst = fbb + (y * pps + x0);
        int bytes = (x1 - x0 + 1) * sizeof(uint32_t);
        
        /* Use optimized memcpy_fast for better performance (64-bit chunks) */
        memcpy_fast(dst, src, bytes);
    }

    /* reset dirty rect */
    dirty_min_x = 1; dirty_min_y = 1; dirty_max_x = 0; dirty_max_y = 0;
}

/* Clear back buffer with color (or fbb if no backbuf) */
static void fb_clear(uint32_t color) {
    if (backbuf) {
        size_t n = (size_t)screen_width * (size_t)screen_height;
        for (size_t i = 0; i < n; ++i) backbuf[i] = color;
        fb_mark_dirty_rect(0, 0, screen_width - 1, screen_height - 1);
        fb_present_dirty();
    } else if (fbb) {
        size_t n = (size_t)screen_height * (size_t)pps;
        for (size_t i = 0; i < n; ++i) fbb[i] = color;
    }
}

/* Allocate back buffer contiguous pages robustly */
static int fb_alloc_backbuf(void) {
    if (backbuf) return 0;
    size_t pixels = (size_t)screen_width * (size_t)screen_height;
    if (pixels == 0) return -1;
    size_t bytes = pixels * sizeof(uint32_t);
    size_t pages = (bytes + 4095) / 4096;
    if (pages == 0) pages = 1;
    void *buf = kernel_alloc_pages(pages);
    if (!buf) {
        backbuf = NULL;
        backbuf_pixels = 0;
        return -1;
    }
    backbuf = (uint32_t*)buf;
    backbuf_pixels = pixels;
    for (size_t i = 0; i < pixels; ++i) backbuf[i] = 0x222222;
    return 0;
}

/* --- Serial helpers --- */
#define SERIAL_PORT_BASE 0x3F8
static inline void outb_port(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" :: "a"(val), "Nd"(port));
}
static inline uint8_t inb_port(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
static inline void serial_outb(uint16_t port, uint8_t val) { outb_port(port, val); }
static inline uint8_t serial_inb(uint16_t port) { return inb_port(port); }

void serial_init(void) {
    serial_outb(SERIAL_PORT_BASE + 1, 0x00);
    serial_outb(SERIAL_PORT_BASE + 3, 0x80);
    serial_outb(SERIAL_PORT_BASE + 0, 0x01);
    serial_outb(SERIAL_PORT_BASE + 1, 0x00);
    serial_outb(SERIAL_PORT_BASE + 3, 0x03);
    serial_outb(SERIAL_PORT_BASE + 2, 0xC7);
    serial_outb(SERIAL_PORT_BASE + 4, 0x0B);
}

static void serial_wait_tx(void) {
    while ((serial_inb(SERIAL_PORT_BASE + 5) & 0x20) == 0) { __asm__ volatile ("pause"); }
}

void serial_putc(char c) {
    serial_wait_tx();
    serial_outb(SERIAL_PORT_BASE + 0, (uint8_t)c);
}

/* --- PIC/IDT/IRQ --- */
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1
#define PIC_EOI      0x20

struct IDTEntry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed));

struct IDTPtr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct IDTEntry idt[256];
static struct IDTPtr idt_ptr;

static void set_idt_entry(int n, void* handler, uint8_t type_attr, uint8_t ist) {
    uint64_t addr = (uint64_t)handler;
    idt[n].offset_low  = (uint16_t)(addr & 0xFFFF);
    idt[n].selector    = 0x08;
    idt[n].ist         = ist & 0x7;
    idt[n].type_attr   = type_attr;
    idt[n].offset_mid  = (uint16_t)((addr >> 16) & 0xFFFF);
    idt[n].offset_high = (uint32_t)((addr >> 32) & 0xFFFFFFFF);
    idt[n].zero        = 0;
}

static void pic_remap(void) {
    uint8_t a1 = inb_port(PIC1_DATA);
    uint8_t a2 = inb_port(PIC2_DATA);

    outb_port(PIC1_COMMAND, 0x11);
    outb_port(PIC2_COMMAND, 0x11);
    outb_port(PIC1_DATA, 0x20);
    outb_port(PIC2_DATA, 0x28);
    outb_port(PIC1_DATA, 0x04);
    outb_port(PIC2_DATA, 0x02);
    outb_port(PIC1_DATA, 0x01);
    outb_port(PIC2_DATA, 0x01);

    outb_port(PIC1_DATA, a1);
    outb_port(PIC2_DATA, a2);
}

static inline void pic_send_eoi(unsigned irq) {
    if (irq >= 8) outb_port(PIC2_COMMAND, PIC_EOI);
    outb_port(PIC1_COMMAND, PIC_EOI);
}

/* --- Keyboard buffer & scancodes --- */
#define KB_BUF_SIZE 256
static volatile char kb_buf[KB_BUF_SIZE];
static volatile unsigned kb_head = 0, kb_tail = 0, kb_count = 0;
static volatile int kb_ctrl_pressed = 0, kb_alt_pressed = 0, kb_ext_prefix = 0;

static const char scancode_to_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b', '\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,  'a','s',
    'd','f','g','h','j','k','l',';','\'','`', 0, '\\','z','x','c','v',
    'b','n','m',',','.','/', 0,  '*', 0,  ' ',
};

static void kb_put(char c) {
    if (kb_count < KB_BUF_SIZE) {
        kb_buf[kb_head] = c;
        kb_head = (kb_head + 1) & (KB_BUF_SIZE - 1);
        kb_count++;
    }
}

static char kb_get_nolock(void) {
    char c = kb_buf[kb_tail];
    kb_tail = (kb_tail + 1) & (KB_BUF_SIZE - 1);
    kb_count--;
    return c;
}

int kgetc(void) {
    char c;
    while (1) {
        __asm__ volatile ("cli");
        if (kb_count > 0) {
            c = kb_get_nolock();
            __asm__ volatile ("sti");
            return (int)(unsigned char)c;
        }
        __asm__ volatile ("sti");
        __asm__ volatile ("hlt");
    }
}

int kgets(char *buf, int maxlen) {
    int i = 0;
    while (i < maxlen - 1) {
        int ch = kgetc();
        if (ch == '\r') ch = '\n';
        if (ch == '\n') {
            buf[i++] = '\n';
            buf[i] = '\0';
            return i;
        } else if (ch == '\b') {
            if (i > 0) {
                i--;
                if (cursor_x > MARGIN_X) {
                    cursor_x -= CHAR_WIDTH;
                    draw_char(' ');
                    cursor_x -= CHAR_WIDTH;
                }
            }
        } else {
            buf[i++] = (char)ch;
            draw_char((char)ch);
        }
    }
    buf[i] = '\0';
    return i;
}

/* Reboot helper */
static void system_reboot(void) {
    __asm__ volatile ("cli");
    outb_port(0x64, 0xFE);
    for (;;) { __asm__ volatile ("hlt"); }
}

/* Keyboard C handler */
void keyboard_handler_c(void) {
    uint8_t sc = inb_port(0x60);

    if (sc == 0xE0) { kb_ext_prefix = 1; pic_send_eoi(1); return; }

    if (kb_ext_prefix) {
        kb_ext_prefix = 0;
        if (sc == 0x53) { /* E0 53 = Delete make */
            if (kb_ctrl_pressed && kb_alt_pressed) system_reboot();
        }
        pic_send_eoi(1);
        return;
    }

    if (sc == 0x1D) { kb_ctrl_pressed = 1; pic_send_eoi(1); return; }
    if (sc == 0x9D) { kb_ctrl_pressed = 0; pic_send_eoi(1); return; }
    if (sc == 0x38) { kb_alt_pressed = 1; pic_send_eoi(1); return; }
    if (sc == 0xB8) { kb_alt_pressed = 0; pic_send_eoi(1); return; }

    if (sc & 0x80) { pic_send_eoi(1); return; }

    if (sc < 128) {
        char c = scancode_to_ascii[sc];
        if (c) kb_put(c);
    }

    pic_send_eoi(1);
}

/* Keyboard ISR wrapper (preserve registers, align stack) */
__attribute__((naked)) void isr_keyboard_stub(void) {
    __asm__ volatile (
        "pushq %rax\n\t"
        "pushq %rcx\n\t"
        "pushq %rdx\n\t"
        "pushq %rbx\n\t"
        "pushq %rbp\n\t"
        "pushq %rsi\n\t"
        "pushq %rdi\n\t"
        "pushq %r8\n\t"
        "pushq %r9\n\t"
        "pushq %r10\n\t"
        "pushq %r11\n\t"
        "pushq %r12\n\t"
        "pushq %r13\n\t"
        "pushq %r14\n\t"
        "pushq %r15\n\t"
        "sub $8, %rsp\n\t"
        "call keyboard_handler_c\n\t"
        "add $8, %rsp\n\t"
        "popq %r15\n\t"
        "popq %r14\n\t"
        "popq %r13\n\t"
        "popq %r12\n\t"
        "popq %r11\n\t"
        "popq %r10\n\t"
        "popq %r9\n\t"
        "popq %r8\n\t"
        "popq %rdi\n\t"
        "popq %rsi\n\t"
        "popq %rbp\n\t"
        "popq %rbx\n\t"
        "popq %rdx\n\t"
        "popq %rcx\n\t"
        "popq %rax\n\t"
        "iretq\n\t"
    );
}

/* --- Exception stubs 0..31 (error-code handling) --- */
__attribute__((naked)) void isr_vector0(void)  { __asm__ volatile("mov $0,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector1(void)  { __asm__ volatile("mov $1,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector2(void)  { __asm__ volatile("mov $2,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector3(void)  { __asm__ volatile("mov $3,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector4(void)  { __asm__ volatile("mov $4,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector5(void)  { __asm__ volatile("mov $5,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector6(void)  { __asm__ volatile("mov $6,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector7(void)  { __asm__ volatile("mov $7,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector8(void)  { __asm__ volatile("mov $8,%rdi\n\tmov (%rsp),%rsi\n\tadd $8,%rsp\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector9(void)  { __asm__ volatile("mov $9,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector10(void) { __asm__ volatile("mov $10,%rdi\n\tmov (%rsp),%rsi\n\tadd $8,%rsp\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector11(void) { __asm__ volatile("mov $11,%rdi\n\tmov (%rsp),%rsi\n\tadd $8,%rsp\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector12(void) { __asm__ volatile("mov $12,%rdi\n\tmov (%rsp),%rsi\n\tadd $8,%rsp\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector13(void) { __asm__ volatile("mov $13,%rdi\n\tmov (%rsp),%rsi\n\tadd $8,%rsp\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector14(void) { __asm__ volatile("mov $14,%rdi\n\tmov (%rsp),%rsi\n\tadd $8,%rsp\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector15(void) { __asm__ volatile("mov $15,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector16(void) { __asm__ volatile("mov $16,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector17(void) { __asm__ volatile("mov $17,%rdi\n\tmov (%rsp),%rsi\n\tadd $8,%rsp\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector18(void) { __asm__ volatile("mov $18,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector19(void) { __asm__ volatile("mov $19,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector20(void) { __asm__ volatile("mov $20,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector21(void) { __asm__ volatile("mov $21,%rdi\n\tmov (%rsp),%rsi\n\tadd $8,%rsp\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector22(void) { __asm__ volatile("mov $22,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector23(void) { __asm__ volatile("mov $23,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector24(void) { __asm__ volatile("mov $24,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector25(void) { __asm__ volatile("mov $25,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector26(void) { __asm__ volatile("mov $26,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector27(void) { __asm__ volatile("mov $27,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector28(void) { __asm__ volatile("mov $28,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector29(void) { __asm__ volatile("mov $29,%rdi\n\tmov (%rsp),%rsi\n\tadd $8,%rsp\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector30(void) { __asm__ volatile("mov $30,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector31(void) { __asm__ volatile("mov $31,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }

/* C exception handler */
void exception_handler_c(int vec, uint64_t err) {
    printf("EXCEPTION: vector=%d err=0x%x\n", vec, err);
    if (vec == 14) {
        uint64_t cr2;
        __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
        printf("Page fault address: 0x%x\n", cr2);
    }
    for (;;) { __asm__ volatile ("hlt"); }
}

/* Load IDT */
static void load_idt(void) {
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base  = (uint64_t)&idt;
    __asm__ volatile ("lidt %0" :: "m"(idt_ptr) : "memory");
}

/* Initialize PIC, IDT, keyboard */
void init_pic_idt_keyboard(void) {
    for (int i = 0; i < 256; i++) {
        idt[i].offset_low = idt[i].selector = idt[i].ist = idt[i].type_attr = 0;
        idt[i].offset_mid = idt[i].offset_high = idt[i].zero = 0;
    }

    set_idt_entry(0,  isr_vector0,  0x8E, 0);
    set_idt_entry(1,  isr_vector1,  0x8E, 0);
    set_idt_entry(2,  isr_vector2,  0x8E, 0);
    set_idt_entry(3,  isr_vector3,  0x8E, 0);
    set_idt_entry(4,  isr_vector4,  0x8E, 0);
    set_idt_entry(5,  isr_vector5,  0x8E, 0);
    set_idt_entry(6,  isr_vector6,  0x8E, 0);
    set_idt_entry(7,  isr_vector7,  0x8E, 0);
    set_idt_entry(8,  isr_vector8,  0x8E, 1);
    set_idt_entry(9,  isr_vector9,  0x8E, 0);
    set_idt_entry(10, isr_vector10, 0x8E, 0);
    set_idt_entry(11, isr_vector11, 0x8E, 0);
    set_idt_entry(12, isr_vector12, 0x8E, 0);
    set_idt_entry(13, isr_vector13, 0x8E, 0);
    set_idt_entry(14, isr_vector14, 0x8E, 0);
    set_idt_entry(15, isr_vector15, 0x8E, 0);
    set_idt_entry(16, isr_vector16, 0x8E, 0);
    set_idt_entry(17, isr_vector17, 0x8E, 0);
    set_idt_entry(18, isr_vector18, 0x8E, 0);
    set_idt_entry(19, isr_vector19, 0x8E, 0);
    set_idt_entry(20, isr_vector20, 0x8E, 0);
    set_idt_entry(21, isr_vector21, 0x8E, 0);
    set_idt_entry(22, isr_vector22, 0x8E, 0);
    set_idt_entry(23, isr_vector23, 0x8E, 0);
    set_idt_entry(24, isr_vector24, 0x8E, 0);
    set_idt_entry(25, isr_vector25, 0x8E, 0);
    set_idt_entry(26, isr_vector26, 0x8E, 0);
    set_idt_entry(27, isr_vector27, 0x8E, 0);
    set_idt_entry(28, isr_vector28, 0x8E, 0);
    set_idt_entry(29, isr_vector29, 0x8E, 0);
    set_idt_entry(30, isr_vector30, 0x8E, 0);
    set_idt_entry(31, isr_vector31, 0x8E, 0);

    set_idt_entry(0x21, isr_keyboard_stub, 0x8E, 0);

    pic_remap();

    outb_port(PIC1_DATA, 0xFD);
    outb_port(PIC2_DATA, 0xFF);

    load_idt();
}

/* --- PAT MSR setup for Write-Combining on Framebuffer --- */
void setup_pat_msr(void) {
    /*
     * PAT MSR (0x277) controls memory type via PAT bits in paging structures.
     * Default PAT: 00 01 04 05 06 07 (indices for PA0-PA7)
     * We modify index 1 to WC (Write-Combining) for GPU/FB performance
     * 
     * Memory types: WB=0x06, UC=0x00, WT=0x04, UCM=0x07, WC=0x01, WP=0x05
     * 
     * Layout in MSR: bits [7:0]=PA0, [15:8]=PA1, [23:16]=PA2, ...
     * Set PA1 to WC (0x01) for best GPU framebuffer performance
     */
    uint64_t pat = 0x0106050406010100ULL;  /* PA1=WC, rest defaults */
    
    uint32_t pat_lo = (uint32_t)(pat & 0xFFFFFFFFULL);
    uint32_t pat_hi = (uint32_t)((pat >> 32) & 0xFFFFFFFFULL);
    
    __asm__ volatile(
        "wrmsr"
        :
        : "c" (0x277), "a" (pat_lo), "d" (pat_hi)
    );
    
    printf("PAT MSR configured for Write-Combining (0x%lx)\n", pat);
}

/* --- Framebuffer mapping (optimized with Write-Combining) --- */
static void map_framebuffer_virtual(uint64_t fb_phys_base, uint64_t fb_size) {
    const uint64_t page_size = 0x200000ULL;
    uint64_t pages = (fb_size + page_size - 1) / page_size;
    uint64_t vaddr = VFB_BASE;
    uint64_t phys = fb_phys_base;

    uint64_t pml4_idx = (vaddr >> 39) & 0x1FF;
    uint64_t pdpt_idx = (vaddr >> 30) & 0x1FF;
    uint64_t pd_idx   = (vaddr >> 21) & 0x1FF;

    if ((pml4[pml4_idx] & PAGE_PRESENT) == 0) {
        uint64_t *new_pdpt = (uint64_t*)kernel_alloc_page();
        pml4[pml4_idx] = (uint64_t)new_pdpt | PAGE_PRESENT | PAGE_RW;
    }
    uint64_t *pdpt = (uint64_t*)(uintptr_t)(pml4[pml4_idx] & ~0xFFFULL);

    if ((pdpt[pdpt_idx] & PAGE_PRESENT) == 0) {
        uint64_t *new_pd = (uint64_t*)kernel_alloc_page();
        pdpt[pdpt_idx] = (uint64_t)new_pd | PAGE_PRESENT | PAGE_RW;
    }
    uint64_t *pd = (uint64_t*)(uintptr_t)(pdpt[pdpt_idx] & ~0xFFFULL);

    uint64_t cur_pd_index = pd_idx;
    uint64_t cur_pdpt_idx = pdpt_idx;
    uint64_t cur_pml4_idx = pml4_idx;

    for (uint64_t i = 0; i < pages; ++i) {
        if (cur_pd_index >= 512) {
            cur_pd_index = 0;
            cur_pdpt_idx++;
            if (cur_pdpt_idx >= 512) {
                cur_pdpt_idx = 0;
                cur_pml4_idx++;
                if (cur_pml4_idx >= 512) return;
                if ((pml4[cur_pml4_idx] & PAGE_PRESENT) == 0) {
                    uint64_t *new_pdpt = (uint64_t*)kernel_alloc_page();
                    pml4[cur_pml4_idx] = (uint64_t)new_pdpt | PAGE_PRESENT | PAGE_RW;
                }
                pdpt = (uint64_t*)(uintptr_t)(pml4[cur_pml4_idx] & ~0xFFFULL);
            }
            if ((pdpt[cur_pdpt_idx] & PAGE_PRESENT) == 0) {
                uint64_t *new_pd = (uint64_t*)kernel_alloc_page();
                pdpt[cur_pdpt_idx] = (uint64_t)new_pd | PAGE_PRESENT | PAGE_RW;
            }
            pd = (uint64_t*)(uintptr_t)(pdpt[cur_pdpt_idx] & ~0xFFFULL);
        }

        /* Set PCD (Bit 4) = 0, PWT (Bit 3) = 0, PAT (Bit 7) = 0 → selects PAT index 1 = WC */
        pd[cur_pd_index] = (phys & ~(page_size - 1)) | PAGE_PRESENT | PAGE_RW | PAGE_PS;
        phys += page_size;
        cur_pd_index++;
    }

    uint64_t cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));
    __asm__ volatile ("mov %0, %%cr3" :: "r"(cr3) : "memory");
}

/* --- Paging init (optimized for Write-Combining) --- */
void init_paging(KernelGOPInfo *kgi) {
    const uint64_t flags = PAGE_PRESENT | PAGE_RW;
    /* Use Write-Combining: PCD=0, PWT=0, PAT=0 → PAT index 1 (WC) */
    const uint64_t fb_flags = PAGE_PRESENT | PAGE_RW;
    
    uint64_t *pdpt = (uint64_t*)kernel_alloc_page();
    pml4[0] = (uint64_t)pdpt | flags;

    for (int i = 0; i < 4; i++) {
        uint64_t *pd = (uint64_t*)kernel_alloc_page();
        pdpt[i] = (uint64_t)pd | flags;
        for (int j = 0; j < 512; j++) {
            uint64_t addr = (uint64_t)(i * 512 + j) * 0x200000;
            uint64_t f = (addr >= kgi->FrameBufferBase && addr < kgi->FrameBufferBase + FB_MAP_SIZE) ? fb_flags : flags;
            pd[j] = addr | f | PAGE_PS;
        }
    }

    __asm__ volatile ("mov %0, %%cr3" :: "r"(pml4) : "memory");

    map_framebuffer_virtual(kgi->FrameBufferBase, FB_MAP_SIZE);
}

/* --- GDT/TSS init --- */
void init_gdt_tss() {
    memset(&gdt_table, 0, sizeof(gdt_table));
    memset(&kernel_tss, 0, sizeof(kernel_tss));

    gdt_table.code.access = 0x9A;
    gdt_table.code.gran   = 0x20;
    gdt_table.data.access = 0x92;
    gdt_table.data.gran   = 0x00;

    uint64_t tss_base = (uint64_t)&kernel_tss;
    kernel_tss.rsp0 = (uint64_t)tss_stack + sizeof(tss_stack);
    kernel_tss.ist[0] = (uint64_t)tss_stack + sizeof(tss_stack);
    kernel_tss.iopb_offset = sizeof(kernel_tss);

    gdt_table.tss.low.limit_low = sizeof(kernel_tss) - 1;
    gdt_table.tss.low.base_low  = (uint16_t)(tss_base & 0xFFFF);
    gdt_table.tss.low.base_mid  = (uint8_t)((tss_base >> 16) & 0xFF);
    gdt_table.tss.low.access    = 0x89;
    gdt_table.tss.low.base_high = (uint8_t)((tss_base >> 24) & 0xFF);
    gdt_table.tss.base_upper32  = (uint32_t)((tss_base >> 32) & 0xFFFFFFFF);

    gdt_ptr.limit = sizeof(gdt_table) - 1;
    gdt_ptr.base = (uint64_t)&gdt_table;
    __asm__ volatile ("lgdt %0" :: "m"(gdt_ptr) : "memory");

    __asm__ volatile (
        "mov $0x10, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%ss\n\t"
        "pushq $0x08\n\t"
        "lea 1f(%%rip), %%rax\n\t"
        "pushq %%rax\n\t"
        "lretq\n\t"
        "1:\n\t"
        ::: "rax"
    );

    __asm__ volatile ("ltr %w0" :: "r"((uint16_t)0x18) : "memory");
}

void scroll();

/* --- Drawing: supports backbuf and single-buffer fallback --- */
void draw_char(char c) {
    if (c == '\r') serial_putc('\n'); else serial_putc(c);

    if (!backbuf) {
        /* single-buffer mode: draw directly into fbb (if available) */
        if (!fbb) return;
        if (c == '\n') {
            cursor_x = MARGIN_X;
            cursor_y += CHAR_HEIGHT;
            if (cursor_y + CHAR_HEIGHT >= screen_height) {
                /* simple scroll: move framebuffer up (slow) */
                for (uint32_t y = MARGIN_Y; y + CHAR_HEIGHT < screen_height; ++y) {
                    for (uint32_t x = 0; x < screen_width; ++x) {
                        fbb[y * pps + x] = fbb[(y + CHAR_HEIGHT) * pps + x];
                    }
                }
                for (uint32_t y = screen_height - CHAR_HEIGHT; y < screen_height; ++y)
                    for (uint32_t x = 0; x < screen_width; ++x) fbb[y * pps + x] = 0x222222;
                cursor_y -= CHAR_HEIGHT;
            }
            return;
        }
        if (cursor_x + CHAR_WIDTH >= screen_width) {
            cursor_x = MARGIN_X;
            cursor_y += CHAR_HEIGHT;
            if (cursor_y + CHAR_HEIGHT >= screen_height) {
                /* scroll as above */
                for (uint32_t y = MARGIN_Y; y + CHAR_HEIGHT < screen_height; ++y)
                    for (uint32_t x = 0; x < screen_width; ++x)
                        fbb[y * pps + x] = fbb[(y + CHAR_HEIGHT) * pps + x];
                for (uint32_t y = screen_height - CHAR_HEIGHT; y < screen_height; ++y)
                    for (uint32_t x = 0; x < screen_width; ++x) fbb[y * pps + x] = 0x222222;
                cursor_y -= CHAR_HEIGHT;
            }
        }
        if (c < 32 || c > 126) return;
        const uint8_t* g = font8x8_basic[c - 32];
        for (int r = 0; r < 8; ++r) {
            for (int col = 0; col < 8; ++col) {
                if ((g[r] >> (7 - col)) & 1) {
                    uint32_t base = (cursor_y + r*2) * pps + (cursor_x + col*2);
                    fbb[base] = 0xFFFFFF; fbb[base + 1] = 0xFFFFFF;
                    fbb[base + pps] = 0xFFFFFF; fbb[base + pps + 1] = 0xFFFFFF;
                }
            }
        }
        cursor_x += CHAR_WIDTH;
        return;
    }

    /* backbuffer mode */
    if (c == '\n') {
        cursor_x = MARGIN_X;
        cursor_y += CHAR_HEIGHT;
        if (cursor_y + CHAR_HEIGHT >= screen_height) scroll();
        fb_mark_dirty_rect(0, cursor_y - CHAR_HEIGHT, screen_width - 1, cursor_y + CHAR_HEIGHT);
        fb_present_dirty();
        return;
    }

    if (cursor_x + CHAR_WIDTH >= screen_width) {
        cursor_x = MARGIN_X;
        cursor_y += CHAR_HEIGHT;
        if (cursor_y + CHAR_HEIGHT >= screen_height) scroll();
    }

    if (c < 32 || c > 126) return;
    const uint8_t* g = font8x8_basic[c - 32];
    for (int r = 0; r < 8; ++r) {
        for (int col = 0; col < 8; ++col) {
            if ((g[r] >> (7 - col)) & 1) {
                int px = cursor_x + col*2;
                int py = cursor_y + r*2;
                if (px < 0 || py < 0 || px+1 >= (int)screen_width || py+1 >= (int)screen_height) continue;
                size_t base = (size_t)py * screen_width + px;
                backbuf[base] = 0xFFFFFF; backbuf[base + 1] = 0xFFFFFF;
                backbuf[base + screen_width] = 0xFFFFFF; backbuf[base + screen_width + 1] = 0xFFFFFF;
            }
        }
    }
    fb_mark_dirty_rect(cursor_x, cursor_y, cursor_x + CHAR_WIDTH - 1, cursor_y + CHAR_HEIGHT - 1);
    cursor_x += CHAR_WIDTH;
    fb_present_dirty();
}

/* scroll for backbuffer or single-buffer fallback */
void scroll() {
    if (backbuf) {
        int width = screen_width;
        int lines = CHAR_HEIGHT;
        for (int y = MARGIN_Y; y < (int)screen_height - lines; ++y) {
            uint32_t *dst = backbuf + (y * width);
            uint32_t *src = backbuf + ((y + lines) * width);
            for (int x = 0; x < width; ++x) dst[x] = src[x];
        }
        for (int y = screen_height - lines; y < (int)screen_height; ++y) {
            uint32_t *row = backbuf + (y * width);
            for (int x = 0; x < width; ++x) row[x] = 0x222222;
        }
        cursor_y -= CHAR_HEIGHT;
        fb_mark_dirty_rect(0, MARGIN_Y, screen_width - 1, screen_height - 1);
        fb_present_dirty();
    } else if (fbb) {
        for (uint32_t y = MARGIN_Y; y + CHAR_HEIGHT < screen_height; ++y) {
            for (uint32_t x = 0; x < screen_width; ++x) {
                fbb[y * pps + x] = fbb[(y + CHAR_HEIGHT) * pps + x];
            }
        }
        for (uint32_t y = screen_height - CHAR_HEIGHT; y < screen_height; ++y)
            for (uint32_t x = 0; x < screen_width; ++x) fbb[y * pps + x] = 0x222222;
        cursor_y -= CHAR_HEIGHT;
    }
}

/* Minimal printf supporting %d %x %s */
void printf(const char* fmt, ...) {
    va_list args; va_start(args, fmt);
    while (*fmt) {
        if (*fmt == '%' && *(fmt + 1)) {
            fmt++;
            if (*fmt == 'd' || *fmt == 'x') {
                uint64_t n = va_arg(args, uint64_t);
                uint64_t b = (*fmt == 'x') ? 16 : 10;
                char buf[32]; int i = 0;
                do { uint64_t m = n % b; buf[i++] = (m < 10) ? m + '0' : m - 10 + 'A'; n /= b; } while(n > 0);
                while(i > 0) draw_char(buf[--i]);
            } else if (*fmt == 's') {
                char* s = va_arg(args, char*);
                while(*s) draw_char(*s++);
            } else {
                draw_char('%'); draw_char(*fmt);
            }
        } else draw_char(*fmt);
        fmt++;
    }
    va_end(args);
}

/* --- Main entry --- */
void kmain(KernelGOPInfo *kgi) {
    pps = kgi->PixelsPerScanLine;
    screen_width = kgi->Width;
    screen_height = kgi->Height;

    /* initialize dirty rect empty */
    dirty_min_x = 1; dirty_min_y = 1; dirty_max_x = 0; dirty_max_y = 0;

    init_paging(kgi);
    init_gdt_tss();

    serial_init();

    /* Setup PAT MSR for Write-Combining BEFORE using framebuffer */
    setup_pat_msr();

    init_pic_idt_keyboard();

    __asm__ volatile ("sti");

    uint64_t phys_fb = kgi->FrameBufferBase;
    uint64_t offset = phys_fb & (FB_MAP_SIZE - 1);
    fbb = (volatile uint32_t*)(uintptr_t)(VFB_BASE + offset);

    /* allocate backbuffer; fallback to single-buffer if allocation fails */
    if (fb_alloc_backbuf() != 0) {
        backbuf = NULL;
        backbuf_pixels = 0;
        printf("Warning: backbuffer allocation failed; using single-buffer mode.\n");
        fb_clear(0x222222);
    } else {
        fb_mark_dirty_rect(0, 0, screen_width - 1, screen_height - 1);
        fb_present_dirty();
    }

    printf("Kernel: initialized. heap used=%d bytes\n", (int)page_ptr);
    
    for(int i = 0;i<200;i++) {
    	printf("Loop %d\n", i);
    }

    while (1) {
        int ch = kgetc();
        if (ch >= 0) {
            if (ch == '\r') ch = '\n';
            draw_char((char)ch);
        }
    }
}

/* Minimal _start wrapper */
__attribute__((naked)) void _start(void) {
    __asm__ volatile ("cli\n\tmov %rcx, %rdi\n\tsub $8, %rsp\n\tcall kmain\n\thlt");
}

