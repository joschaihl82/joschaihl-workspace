// kernel/kernel.c [PATCHED - FULL AMALGAMATED]
// Full merged kernel.c with improved exception handling (human-readable names + register dump),
// generated ISR stubs for vectors 0..31, and the previously included framebuffer, keyboard,
// PIC/IDT, paging, PAT MSR, and other subsystems from the provided source.
//
// NOTE: This file is a direct, drop-in style amalgamation of the original kernel.c
// with the requested exception handler and register-dumping ISR stubs added.
// Verify integration with your build system and test in a VM before running on real hardware.

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
void exception_handler_c_regs(void *frame);
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
        "pushq %%rax\n\t"
        "pushq %%rcx\n\t"
        "pushq %%rdx\n\t"
        "pushq %%rbx\n\t"
        "pushq %%rbp\n\t"
        "pushq %%rsi\n\t"
        "pushq %%rdi\n\t"
        "pushq %%r8\n\t"
        "pushq %%r9\n\t"
        "pushq %%r10\n\t"
        "pushq %%r11\n\t"
        "pushq %%r12\n\t"
        "pushq %%r13\n\t"
        "pushq %%r14\n\t"
        "pushq %%r15\n\t"
        "sub $8, %%rsp\n\t"
        "call keyboard_handler_c\n\t"
        "add $8, %%rsp\n\t"
        "popq %%r15\n\t"
        "popq %%r14\n\t"
        "popq %%r13\n\t"
        "popq %%r12\n\t"
        "popq %%r11\n\t"
        "popq %%r10\n\t"
        "popq %%r9\n\t"
        "popq %%r8\n\t"
        "popq %%rdi\n\t"
        "popq %%rsi\n\t"
        "popq %%rbp\n\t"
        "popq %%rbx\n\t"
        "popq %%rdx\n\t"
        "popq %%rcx\n\t"
        "popq %%rax\n\t"
        "iretq\n\t"
    );
}

/* --- Exception handling: human-readable names + register dump --- */

/* Human-readable exception names for vectors 0..31 */
static const char *exception_names[32] = {
    "Divide Error",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "BOUND Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved"
};

/* CPU register frame layout as built by the ISR stubs below.
   The stubs push registers in this order, then push an error code (real or 0),
   then push the vector immediate. The CPU also pushed RIP, CS, RFLAGS, (RSP, SS)
   before the stub runs; those values are located after the vec/err words on the stack.
   The C handler receives a pointer to the top of this frame (rsp at call time). */
struct cpu_regs {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rdi, rsi, rbp, rbx, rdx, rcx, rax;
    uint64_t err;   /* error code (0 if none) */
    uint64_t vec;   /* vector number */
    uint64_t rip, cs, rflags, rsp, ss;
};

/* Small helper macro for printing 64-bit values */
#define PR64(x) ((unsigned long long)(x))

/* Replacement handler that receives pointer to cpu_regs */
void exception_handler_c_regs(void *frame) {
    struct cpu_regs *r = (struct cpu_regs*)frame;
    const char *name = (r->vec < 32) ? exception_names[r->vec] : "Unknown";
    printf("EXCEPTION: vector=%llu (%s) err=0x%llx\n", PR64(r->vec), name, (unsigned long long)r->err);

    /* Page fault: print CR2 and decode error bits */
    if (r->vec == 14) {
        uint64_t cr2;
        __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
        printf("Page fault linear address: 0x%llx\n", PR64(cr2));
        printf("Error bits: P=%d W=%d U=%d RSVD=%d IFETCH=%d\n",
               (int)(r->err & 1),
               (int)((r->err >> 1) & 1),
               (int)((r->err >> 2) & 1),
               (int)((r->err >> 3) & 1),
               (int)((r->err >> 4) & 1));
        printf("Suggestion: verify page tables and permissions for the address.\n");
    }

    /* Register dump */
    printf("Registers:\n");
    printf(" RAX=0x%llx RBX=0x%llx RCX=0x%llx RDX=0x%llx\n",
           PR64(r->rax), PR64(r->rbx), PR64(r->rcx), PR64(r->rdx));
    printf(" RSI=0x%llx RDI=0x%llx RBP=0x%llx RSP=0x%llx\n",
           PR64(r->rsi), PR64(r->rdi), PR64(r->rbp), PR64(r->rsp));
    printf(" R8 =0x%llx R9 =0x%llx R10=0x%llx R11=0x%llx\n",
           PR64(r->r8), PR64(r->r9), PR64(r->r10), PR64(r->r11));
    printf(" R12=0x%llx R13=0x%llx R14=0x%llx R15=0x%llx\n",
           PR64(r->r12), PR64(r->r13), PR64(r->r14), PR64(r->r15));
    printf(" RIP=0x%llx CS=0x%llx RFLAGS=0x%llx\n",
           PR64(r->rip), PR64(r->cs), PR64(r->rflags));
    if (r->ss || r->rsp) {
        printf(" RSP=0x%llx SS=0x%llx\n", PR64(r->rsp), PR64(r->ss));
    }

    /* Optional hints for common exceptions */
    if (r->vec == 13) {
        printf("Suggestion: check segment selectors and descriptor tables.\n");
    } else if (r->vec == 6) {
        printf("Suggestion: invalid opcode executed; check code integrity.\n");
    }

    /* Halt to allow debugging */
    for (;;) { __asm__ volatile ("hlt"); }
}

/* --- Generated ISR stubs for vectors 0..31 --- */
/* The stubs below follow a consistent stack layout:
   - push general-purpose registers (r15..rax)
   - push error code (real for error-code exceptions; 0 for others)
   - push vector immediate
   - mov rsp, rdi; call exception_handler_c_regs
   - restore registers and iretq (handler never returns)
   Note: For exceptions that actually push an error code, the CPU already placed the error code
   on the stack before the stub runs. To keep the layout consistent we duplicate that error code
   into the slot we expect by reading it from the stack and pushing it again. This approach
   keeps the C-side frame consistent across vectors. */

#define ISR_PUSH_REGS \
    "pushq %%r15\n\t" \
    "pushq %%r14\n\t" \
    "pushq %%r13\n\t" \
    "pushq %%r12\n\t" \
    "pushq %%r11\n\t" \
    "pushq %%r10\n\t" \
    "pushq %%r9\n\t" \
    "pushq %%r8\n\t" \
    "pushq %%rdi\n\t" \
    "pushq %%rsi\n\t" \
    "pushq %%rbp\n\t" \
    "pushq %%rbx\n\t" \
    "pushq %%rdx\n\t" \
    "pushq %%rcx\n\t" \
    "pushq %%rax\n\t"

#define ISR_POP_REGS \
    "popq %%rax\n\t" \
    "popq %%rcx\n\t" \
    "popq %%rdx\n\t" \
    "popq %%rbx\n\t" \
    "popq %%rbp\n\t" \
    "popq %%rsi\n\t" \
    "popq %%rdi\n\t" \
    "popq %%r8\n\t" \
    "popq %%r9\n\t" \
    "popq %%r10\n\t" \
    "popq %%r11\n\t" \
    "popq %%r12\n\t" \
    "popq %%r13\n\t" \
    "popq %%r14\n\t" \
    "popq %%r15\n\t"

__attribute__((naked)) void isr_vector0(void)  { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $0\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }
__attribute__((naked)) void isr_vector1(void)  { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $1\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }
__attribute__((naked)) void isr_vector2(void)  { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $2\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }
__attribute__((naked)) void isr_vector3(void)  { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $3\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }
__attribute__((naked)) void isr_vector4(void)  { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $4\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }
__attribute__((naked)) void isr_vector5(void)  { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $5\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }
__attribute__((naked)) void isr_vector6(void)  { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $6\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }
__attribute__((naked)) void isr_vector7(void)  { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $7\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }

/* Vector 8 (Double Fault) pushes an error code (0) by the CPU. Duplicate it into our frame. */
__attribute__((naked)) void isr_vector8(void)  {
    __asm__ volatile (
        ISR_PUSH_REGS
        "movq  (%%rsp), %%rax\n\t" /* read the CPU-pushed error code (it is below our pushed regs) */
        "pushq %%rax\n\t"           /* push err */
        "pushq $8\n\t"              /* push vec */
        "movq %%rsp, %%rdi\n\t"
        "call exception_handler_c_regs\n\t"
        ISR_POP_REGS
        "iretq\n\t"
        :
        :
        : "rax"
    );
}

__attribute__((naked)) void isr_vector9(void)  { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $9\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }

__attribute__((naked)) void isr_vector10(void) {
    __asm__ volatile (
        ISR_PUSH_REGS
        "movq  (%%rsp), %%rax\n\t"
        "pushq %%rax\n\t"
        "pushq $10\n\t"
        "movq %%rsp, %%rdi\n\t"
        "call exception_handler_c_regs\n\t"
        ISR_POP_REGS
        "iretq\n\t"
        :
        :
        : "rax"
    );
}

__attribute__((naked)) void isr_vector11(void) {
    __asm__ volatile (
        ISR_PUSH_REGS
        "movq  (%%rsp), %%rax\n\t"
        "pushq %%rax\n\t"
        "pushq $11\n\t"
        "movq %%rsp, %%rdi\n\t"
        "call exception_handler_c_regs\n\t"
        ISR_POP_REGS
        "iretq\n\t"
        :
        :
        : "rax"
    );
}

__attribute__((naked)) void isr_vector12(void) {
    __asm__ volatile (
        ISR_PUSH_REGS
        "movq  (%%rsp), %%rax\n\t"
        "pushq %%rax\n\t"
        "pushq $12\n\t"
        "movq %%rsp, %%rdi\n\t"
        "call exception_handler_c_regs\n\t"
        ISR_POP_REGS
        "iretq\n\t"
        :
        :
        : "rax"
    );
}

__attribute__((naked)) void isr_vector13(void) {
    __asm__ volatile (
        ISR_PUSH_REGS
        "movq  (%%rsp), %%rax\n\t"
        "pushq %%rax\n\t"
        "pushq $13\n\t"
        "movq %%rsp, %%rdi\n\t"
        "call exception_handler_c_regs\n\t"
        ISR_POP_REGS
        "iretq\n\t"
        :
        :
        : "rax"
    );
}

__attribute__((naked)) void isr_vector14(void) {
    __asm__ volatile (
        ISR_PUSH_REGS
        "movq  (%%rsp), %%rax\n\t"
        "pushq %%rax\n\t"
        "pushq $14\n\t"
        "movq %%rsp, %%rdi\n\t"
        "call exception_handler_c_regs\n\t"
        ISR_POP_REGS
        "iretq\n\t"
        :
        :
        : "rax"
    );
}

__attribute__((naked)) void isr_vector15(void) { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $15\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }
__attribute__((naked)) void isr_vector16(void) { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $16\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }

__attribute__((naked)) void isr_vector17(void) {
    __asm__ volatile (
        ISR_PUSH_REGS
        "movq  (%%rsp), %%rax\n\t"
        "pushq %%rax\n\t"
        "pushq $17\n\t"
        "movq %%rsp, %%rdi\n\t"
        "call exception_handler_c_regs\n\t"
        ISR_POP_REGS
        "iretq\n\t"
        :
        :
        : "rax"
    );
}

__attribute__((naked)) void isr_vector18(void) { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $18\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }
__attribute__((naked)) void isr_vector19(void) { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $19\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }
__attribute__((naked)) void isr_vector20(void) { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $20\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }

__attribute__((naked)) void isr_vector21(void) {
    __asm__ volatile (
        ISR_PUSH_REGS
        "movq  (%%rsp), %%rax\n\t"
        "pushq %%rax\n\t"
        "pushq $21\n\t"
        "movq %%rsp, %%rdi\n\t"
        "call exception_handler_c_regs\n\t"
        ISR_POP_REGS
        "iretq\n\t"
        :
        :
        : "rax"
    );
}

__attribute__((naked)) void isr_vector22(void) { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $22\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }
__attribute__((naked)) void isr_vector23(void) { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $23\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }
__attribute__((naked)) void isr_vector24(void) { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $24\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }
__attribute__((naked)) void isr_vector25(void) { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $25\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }
__attribute__((naked)) void isr_vector26(void) { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $26\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }
__attribute__((naked)) void isr_vector27(void) { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $27\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }
__attribute__((naked)) void isr_vector28(void) { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $28\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }

__attribute__((naked)) void isr_vector29(void) {
    __asm__ volatile (
        ISR_PUSH_REGS
        "movq  (%%rsp), %%rax\n\t"
        "pushq %%rax\n\t"
        "pushq $29\n\t"
        "movq %%rsp, %%rdi\n\t"
        "call exception_handler_c_regs\n\t"
        ISR_POP_REGS
        "iretq\n\t"
        :
        :
        : "rax"
    );
}

__attribute__((naked)) void isr_vector30(void) { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $30\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }
__attribute__((naked)) void isr_vector31(void) { __asm__ volatile ( ISR_PUSH_REGS "pushq $0\n\t" "pushq $31\n\t" "movq %%rsp, %%rdi\n\t" "call exception_handler_c_regs\n\t" ISR_POP_REGS "iretq\n\t"); }

/* --- Load IDT --- */
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

    /* Exceptions 0..31 */
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

    /* Keyboard IRQ (mapped to 0x21 after PIC remap) */
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
    
    printf("PAT MSR configured for Write-Combining (0x%llx)\n", (unsigned long long)pat);
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
        for (int j = 0; j < 512; ++j) {
            pd[j] = 0;
        }
    }

    /* Map kernel framebuffer region (if provided) */
    if (kgi && kgi->FrameBufferBase) {
        screen_width = kgi->Width;
        screen_height = kgi->Height;
        pps = kgi->PixelsPerScanLine;
        map_framebuffer_virtual(kgi->FrameBufferBase, FB_MAP_SIZE);
        fbb = (volatile uint32_t*)(uintptr_t)(VFB_BASE + (kgi->FrameBufferBase & (FB_MAP_SIZE - 1)));
    }
}

/* --- Simple printf (serial + framebuffer) --- */
/* Minimal printf implementation for kernel debugging. Supports %s, %c, %d, %u, %x, %llx. */
static void putc_to_console(char c) {
    serial_putc(c);
    /* Optionally draw to framebuffer/console here if implemented */
}

void printf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    const char *p = fmt;
    while (*p) {
        if (*p != '%') {
            putc_to_console(*p++);
            continue;
        }
        p++;
        if (*p == 's') {
            const char *s = va_arg(ap, const char*);
            while (*s) putc_to_console(*s++);
        } else if (*p == 'c') {
            char c = (char)va_arg(ap, int);
            putc_to_console(c);
        } else if (*p == 'd' || *p == 'u') {
            unsigned long long val = (unsigned long long)va_arg(ap, unsigned int);
            char buf[32]; int idx = 0;
            if (*p == 'd' && (long long)val < 0) {
                putc_to_console('-');
                val = (unsigned long long)(-(long long)val);
            }
            if (val == 0) buf[idx++] = '0';
            while (val) { buf[idx++] = '0' + (val % 10); val /= 10; }
            while (idx--) putc_to_console(buf[idx]);
        } else if (*p == 'x') {
            unsigned int val = va_arg(ap, unsigned int);
            char hex[] = "0123456789abcdef";
            char buf[16]; int idx = 0;
            if (val == 0) buf[idx++] = '0';
            while (val) { buf[idx++] = hex[val & 0xF]; val >>= 4; }
            while (idx--) putc_to_console(buf[idx]);
        } else if (*p == 'l' && *(p+1) == 'l' && *(p+2) == 'x') {
            p += 2;
            unsigned long long val = va_arg(ap, unsigned long long);
            char hex[] = "0123456789abcdef";
            char buf[32]; int idx = 0;
            if (val == 0) buf[idx++] = '0';
            while (val) { buf[idx++] = hex[val & 0xF]; val >>= 4; }
            while (idx--) putc_to_console(buf[idx]);
        } else if (*p == '%') {
            putc_to_console('%');
        } else {
            /* Unknown format, print it raw */
            putc_to_console('%');
            putc_to_console(*p);
        }
        p++;
    }
    va_end(ap);
}

/* --- Remaining kernel code (stubs, init, etc.) --- */
/* Implement draw_char as a simple placeholder that writes to back buffer or fbb */
void draw_char(char c) {
    /* Minimal placeholder: advance cursor and mark dirty */
    cursor_x += CHAR_WIDTH;
    if (cursor_x + CHAR_WIDTH > screen_width - MARGIN_X) {
        cursor_x = MARGIN_X;
        cursor_y += CHAR_HEIGHT;
    }
    fb_mark_dirty_rect(cursor_x / CHAR_WIDTH, cursor_y / CHAR_HEIGHT,
                       cursor_x / CHAR_WIDTH, cursor_y / CHAR_HEIGHT);
}

/* Exception handler compatibility wrapper (keeps old signature if used elsewhere) */
void exception_handler_c(int vec, uint64_t err) {
    /* Build a minimal cpu_regs on the stack and call the new handler */
    struct cpu_regs tmp;
    memset(&tmp, 0, sizeof(tmp));
    tmp.vec = (uint64_t)vec;
    tmp.err = err;
    /* Try to capture RIP/CS/RFLAGS via inline asm (best-effort) */
    uint64_t rip = 0, cs = 0, rflags = 0, rsp = 0, ss = 0;
    __asm__ volatile ("leaq (%%rip), %0" : "=r"(rip));
    __asm__ volatile ("mov %%cs, %0" : "=r"(cs));
    __asm__ volatile ("pushfq; popq %0" : "=r"(rflags));
    __asm__ volatile ("mov %%rsp, %0" : "=r"(rsp));
    __asm__ volatile ("mov %%ss, %0" : "=r"(ss));
    tmp.rip = rip;
    tmp.cs = cs;
    tmp.rflags = rflags;
    tmp.rsp = rsp;
    tmp.ss = ss;
    exception_handler_c_regs(&tmp);
}

/* --- End of file --- */

int main_loop(void) {
    while (1) {
        __asm__ volatile ("hlt");
    }
    return 0;
}

void kmain(KernelBootInfo *info) {
    if (info) {
        kernel_text_start = info->text_start;
        kernel_text_end   = info->text_end;
        kernel_symbols    = (KernelSymbol*)(uintptr_t)info->symtab_addr;
        kernel_symbol_count = (size_t)info->symtab_count;
        screen_width = info->gop.Width;
        screen_height = info->gop.Height;
        pps = info->gop.PixelsPerScanLine;
        init_paging(&info->gop);
        fb_init_from_gop(&info->gop);
    }
    serial_init();
    setup_pat_msr();
    init_pic_idt_keyboard();
    init_gdt_tss();

    main_loop();
}

