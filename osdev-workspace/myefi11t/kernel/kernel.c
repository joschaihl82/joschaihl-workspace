#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

/* --- Strukturen --- */
typedef struct {
    uint32_t Width;
    uint32_t Height;
    uint32_t PixelsPerScanLine;
    uint64_t FrameBufferBase;
} KernelGOPInfo;

/* Page Table Flags */
#define PAGE_PRESENT (1ULL << 0)
#define PAGE_RW      (1ULL << 1)
#define PAGE_PWT     (1ULL << 3)  // Page-level Write-Through
#define PAGE_PCD     (1ULL << 4)  // Page-level Cache Disable
#define PAGE_PS      (1ULL << 7)  // Page Size (1GB huge page)

/* Page Tables (4KB Aligned) */
uint64_t pml4[512] __attribute__((aligned(4096)));
uint64_t pdpt[512] __attribute__((aligned(4096)));

/* Globale Grafik-Variablen */
uint32_t pps;
uint32_t screen_width;
uint32_t screen_height;
volatile uint32_t *fbb;

static uint32_t cursor_x = 20;
static uint32_t cursor_y = 20;
static uint32_t active_color = 0xFFFFFF;
static const uint32_t line_height = 20;

/* --- Hilfsfunktionen --- */
void* memset(void* dest, int ch, size_t count) {
    uint8_t* p = (uint8_t*)dest;
    while (count--) *p++ = (uint8_t)ch;
    return dest;
}

void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t*)dest;
    const uint8_t *s = (const uint8_t*)src;
    while (n--) *d++ = *s++;
    return dest;
}

/* ---------------------------
   Simple heap (page allocator)
   --------------------------- */

static uint8_t heap[0x1000000] __attribute__((aligned(4096)));
static size_t heap_ptr = 0;

void *kernel_alloc_page(void) {
    void *ptr = &heap[heap_ptr];
    heap_ptr += 4096;
    memset(ptr, 0, 4096);
    return ptr;
}

/* ---------------------------
   Port I/O primitives
   --------------------------- */

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
static inline void outl(uint16_t port, uint32_t val) {
    __asm__ volatile ("outl %0, %1" : : "a"(val), "Nd"(port));
}
static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* ---------------------------
   Serial (COM1) for fallback output
   --------------------------- */

#define COM1_PORT 0x3F8

static void serial_init(void) {
    outb(COM1_PORT + 1, 0x00);    // Disable all interrupts
    outb(COM1_PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(COM1_PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(COM1_PORT + 1, 0x00);    //                  (hi byte)
    outb(COM1_PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(COM1_PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(COM1_PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

static int serial_is_transmit_empty(void) {
    return inb(COM1_PORT + 5) & 0x20;
}

static void serial_putc(char c) {
    while (!serial_is_transmit_empty()) { __asm__("pause"); }
    outb(COM1_PORT, (uint8_t)c);
}

/* ---------------------------
   Framebuffer and font
   --------------------------- */

/* --- Font 8x8 Basic (95 Zeichen) --- */
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

/* --- Grafikausgabe & Scrolling --- */
void fb_scroll() {
    for (uint32_t y = 0; y < screen_height - line_height; y++) {
        for (uint32_t x = 0; x < pps; x++) {
            fbb[y * pps + x] = fbb[(y + line_height) * pps + x];
        }
    }
    for (uint32_t y = screen_height - line_height; y < screen_height; y++) {
        for (uint32_t x = 0; x < pps; x++) {
            fbb[y * pps + x] = 0;
        }
    }
    cursor_y -= line_height;
}

void process_newline() {
    cursor_x = 20;
    cursor_y += line_height;
    while (cursor_y + line_height > screen_height) {
        fb_scroll();
    }
}

void draw_char(char c) {
    if (!fbb) return;
    if (c == '\n') { process_newline(); return; }
    if (c < 32 || c > 126) return;
    if (cursor_x + 16 >= screen_width) process_newline();

    const uint8_t* g = font8x8_basic[c - 32];
    for (int r = 0; r < 8; r++) {
        for (int col = 0; col < 8; col++) {
            if ((g[r] >> (7 - col)) & 1) {
                for(int i=0; i<2; i++) {
                    for(int j=0; j<2; j++) {
                        fbb[(cursor_y + r*2 + i) * pps + (cursor_x + col*2 + j)] = active_color;
                    }
                }
            }
        }
    }
    cursor_x += 16;
}

void k_printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    while (*fmt) {
        if (*fmt == '%' && *(fmt + 1)) {
            fmt++;
            if (*fmt == 'x') {
                uint64_t n = va_arg(args, uint64_t);
                if (n == 0) draw_char('0');
                else {
                    char buf[16]; int i = 0;
                    while(n > 0) {
                        uint8_t nibble = n & 0xF;
                        buf[i++] = (nibble < 10) ? nibble + '0' : nibble - 10 + 'A';
                        n >>= 4;
                    }
                    while(i > 0) draw_char(buf[--i]);
                }
            } else if (*fmt == 's') {
                const char* s = va_arg(args, const char*);
                while(*s) draw_char(*s++);
            }
        } else {
            draw_char(*fmt);
        }
        fmt++;
    }
    va_end(args);
}

/* --- Paging mit Ultra-Fast Write-Combining --- */
void setup_paging(KernelGOPInfo *kgi) {
    /* PAT (Page Attribute Table) MSR 0x277 konfigurieren */
    uint32_t eax, edx;
    uint64_t pat;

    // Lese aktuelles PAT MSR
    __asm__ volatile("rdmsr" : "=a"(eax), "=d"(edx) : "c"(0x277));
    pat = ((uint64_t)edx << 32) | eax;

    // PAT Eintrag 2 (Bits 16-23) auf Write-Combining (0x01) setzen
    pat &= ~(0xFFULL << 16);
    pat |=  (0x01ULL << 16);

    eax = (uint32_t)pat;
    edx = (uint32_t)(pat >> 32);
    // Schreibe modifiziertes PAT zurück
    __asm__ volatile("wrmsr" : : "a"(eax), "d"(edx), "c"(0x277));

    memset(pml4, 0, sizeof(pml4));
    memset(pdpt, 0, sizeof(pdpt));

    // 1:1 Mapping für die ersten 512 GB (1GB Huge Pages)
    for (uint64_t i = 0; i < 512; i++) {
        pdpt[i] = (i << 30) | PAGE_PRESENT | PAGE_RW | PAGE_PS;
    }

    // Framebuffer auf PAT Index 2 setzen (PCD=1, PWT=0) -> Write-Combining
    uint64_t fb_idx = kgi->FrameBufferBase >> 30;
    if (fb_idx < 512) {
        pdpt[fb_idx] |= PAGE_PCD; 
    }

    pml4[0] = ((uint64_t)(uintptr_t)pdpt) | PAGE_PRESENT | PAGE_RW;
    
    // CR3 laden
    uint64_t pml4_addr = (uint64_t)(uintptr_t)pml4;
    __asm__ volatile("mov %0, %%cr3" : : "r"(pml4_addr) : "memory");
}


/* ---------------------------
   NVMe / PCI types and constants
   --------------------------- */

#define NVME_READ_SECTORS               0x02
#define NVME_CREATE_IO_SUBMISSION_QUEUE 0x01
#define NVME_CREATE_IO_COMPLETION_QUEUE 0x05
#define NVME_QUEUE_ENTRY_COUNT          2

#pragma pack(push, 1)
typedef struct {
    uint64_t cap; uint32_t vs; uint32_t intms; uint32_t intmc;
    struct { uint32_t en:1, rsvd:6, css:3, mps:4, ams:3, shn:2, iosqes:4, iocqes:4, rsvd2:5; } cc;
    uint32_t rsvd1;
    struct { uint32_t rdy:1, cfs:1, shst:2, nssro:1, rsvd:27; } csts;
    uint32_t nssr;
    struct { uint32_t asqs:12, rsvd:4, acqs:12, rsvd2:4; } aqa;
    uint64_t asq; uint64_t acq;
} NVME_REGS;

typedef struct {
    uint8_t opc, fuse_psdt; uint16_t cid; uint32_t nsid, rsvd[2];
    uint64_t mptr, prp[2];
    uint32_t cdw10, cdw11, cdw12, cdw13, cdw14, cdw15;
} NVME_SQE;

typedef struct {
    uint32_t dw0, dw1; uint16_t sqhd, sqid, cid, status;
} NVME_CQE;

typedef struct {
    NVME_SQE *sq_entries;
    volatile NVME_CQE *cq_entries;
    volatile uint32_t *sq_doorbell;
    volatile uint32_t *cq_doorbell;
    uint16_t sq_tail;
    uint16_t cq_head;
    uint16_t last_cid;
    uint8_t expected_phase;
} nvme_queue_t;

typedef struct {
    volatile NVME_REGS *regs;
    nvme_queue_t admin_q;
    nvme_queue_t io_q;
    uint32_t nsid;
} nvme_ctrl_t;
#pragma pack(pop)



/* ---------------------------
   Output helpers (framebuffer + serial)
   --------------------------- */

/* Put a character to framebuffer only (safe bounds checks) */
static void putc_fb(char c) {
    if (!fbb) return;
    if (c == '\n') {
        cursor_x = 20;
        cursor_y += line_height;
        if (cursor_y + line_height > screen_height) {
            // scroll up by line_height
            for (uint32_t y = 0; y + line_height < screen_height; y++) {
                for (uint32_t x = 0; x < pps; x++) {
                    fbb[y * pps + x] = fbb[(y + line_height) * pps + x];
                }
            }
            // clear bottom lines
            for (uint32_t y = screen_height - line_height; y < screen_height; y++) {
                for (uint32_t x = 0; x < pps; x++) fbb[y * pps + x] = 0;
            }
            cursor_y -= line_height;
        }
        return;
    }
    if (c < 32 || c > 126) return;
    if (cursor_x + 16 >= screen_width) {
        cursor_x = 20;
        cursor_y += line_height;
    }
    const uint8_t *g = font8x8_basic[c - 32];
    for (int r = 0; r < 8; r++) {
        for (int col = 0; col < 8; col++) {
            if ((g[r] >> (7 - col)) & 1) {
                for (int i = 0; i < 2; i++) {
                    for (int j = 0; j < 2; j++) {
                        uint32_t yy = cursor_y + r*2 + i;
                        uint32_t xx = cursor_x + col*2 + j;
                        if (yy < screen_height && xx < pps) {
                            fbb[yy * pps + xx] = active_color;
                        }
                    }
                }
            }
        }
    }
    cursor_x += 16;
}

/* Put a character to serial only */
static void putc_serial(char c) {
    if (c == '\n') serial_putc('\r');
    serial_putc(c);
}

/* Put a character to both serial and framebuffer (if available) */
static void putc_both(char c) {
    putc_serial(c);
    if (fbb) putc_fb(c);
}

/* Put a null-terminated string to both outputs */
static void puts_both(const char *s) {
    while (*s) putc_both(*s++);
}

/* ---------------------------
   printf implementation (prints to serial and framebuffer)
   --------------------------- */

/* Replace previous k_printf with printf that writes to both outputs.
   Supports: %s, %x (hex), %p (pointer), %% */
int printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int written = 0;
    while (*fmt) {
        if (*fmt == '%' && *(fmt + 1)) {
            fmt++;
            if (*fmt == '%') {
                putc_both('%'); written++;
            } else if (*fmt == 's') {
                const char *str = va_arg(ap, const char*);
                if (!str) str = "(null)";
                const char *p = str;
                while (*p) { putc_both(*p++); written++; }
            } else if (*fmt == 'x') {
                uint64_t n = va_arg(ap, uint64_t);
                char buf[32]; int i = 0;
                if (n == 0) { putc_both('0'); written++; }
                else {
                    while (n && i < (int)sizeof(buf)-1) {
                        uint8_t d = n & 0xF;
                        buf[i++] = (d < 10) ? ('0' + d) : ('A' + d - 10);
                        n >>= 4;
                    }
                    while (i--) { putc_both(buf[i]); written++; }
                }
            } else if (*fmt == 'p') {
                uintptr_t pval = va_arg(ap, uintptr_t);
                /* print 0x... */
                putc_both('0'); putc_both('x'); written += 2;
                uint64_t n = (uint64_t)pval;
                char buf[32]; int i = 0;
                if (n == 0) { putc_both('0'); written++; }
                else {
                    while (n && i < (int)sizeof(buf)-1) {
                        uint8_t d = n & 0xF;
                        buf[i++] = (d < 10) ? ('0' + d) : ('A' + d - 10);
                        n >>= 4;
                    }
                    while (i--) { putc_both(buf[i]); written++; }
                }
            } else {
                /* unknown specifier: print literally */
                putc_both('%'); putc_both(*fmt); written += 2;
            }
        } else {
            putc_both(*fmt); written++;
        }
        fmt++;
    }
    va_end(ap);
    return written;
}

/* ---------------------------
   Debug hexdump
   --------------------------- */

static void debug_hexdump(void *ptr, int len) {
    uint8_t *data = (uint8_t*)ptr;
    for (int i = 0; i < len; i++) {
        if (i % 16 == 0) {
            printf("\n%p: ", (uintptr_t)(data + i));
        }
        uint8_t v = data[i];
        uint8_t hi = (v >> 4) & 0xF;
        uint8_t lo = v & 0xF;
        putc_both(hi < 10 ? '0' + hi : 'A' + hi - 10);
        putc_both(lo < 10 ? '0' + lo : 'A' + lo - 10);
        putc_both(' ');
    }
    printf("\n");
}

/* ---------------------------
   PCI helpers
   --------------------------- */

static void pci_write(uint32_t b, uint32_t s, uint32_t f, uint32_t o, uint32_t v) {
    uint32_t addr = (uint32_t)((1U << 31) | (b << 16) | (s << 11) | (f << 8) | (o & 0xFC));
    outl(0xCF8, addr);
    outl(0xCFC, v);
}

static uint32_t pci_read(uint32_t b, uint32_t s, uint32_t f, uint32_t o) {
    uint32_t addr = (uint32_t)((1U << 31) | (b << 16) | (s << 11) | (f << 8) | (o & 0xFC));
    outl(0xCF8, addr);
    return inl(0xCFC);
}

/* ---------------------------
   NVMe helpers (best-effort)
   --------------------------- */

static void nvme_wait(nvme_queue_t *q) {
    while ((q->cq_entries[q->cq_head].status & 1) != q->expected_phase) {
        __asm__("pause");
    }
    q->cq_head = (q->cq_head + 1) % NVME_QUEUE_ENTRY_COUNT;
    if (q->cq_head == 0) q->expected_phase = !q->expected_phase;
    *q->cq_doorbell = q->cq_head;
}

static void nvme_read_sector(nvme_ctrl_t *ctrl, uint64_t lba, void *buffer) {
    nvme_queue_t *q = &ctrl->io_q;
    NVME_SQE *sqe = &q->sq_entries[q->sq_tail];
    memset(sqe, 0, sizeof(NVME_SQE));
    sqe->opc = NVME_READ_SECTORS;
    sqe->nsid = ctrl->nsid;
    q->last_cid++;
    sqe->cid = q->last_cid;
    sqe->prp[0] = (uintptr_t)buffer;
    sqe->cdw10 = (uint32_t)lba;
    sqe->cdw11 = (uint32_t)(lba >> 32);
    sqe->cdw12 = 0;
    q->sq_tail = (q->sq_tail + 1) % NVME_QUEUE_ENTRY_COUNT;
    *q->sq_doorbell = q->sq_tail;
    nvme_wait(q);
}

static int nvme_init(nvme_ctrl_t *ctrl) {
    uint32_t bar0 = 0;
    for (int b = 0; b < 256 && !bar0; b++) {
        for (int s = 0; s < 32 && !bar0; s++) {
            uint32_t cls = pci_read(b, s, 0, 0x08);
            if ((cls >> 8) == 0x010802) { // class 0x01, subclass 0x08, prog-if 0x02
                bar0 = pci_read(b, s, 0, 0x10) & 0xFFFFFFF0;
                uint32_t cmd = pci_read(b, s, 0, 0x04);
                cmd |= 0x6; // enable bus master + memory space
                pci_write(b, s, 0, 0x04, cmd);
                break;
            }
        }
    }
    if (!bar0) {
        printf("NVMe: device not found\n");
        return -1;
    }

    memset(ctrl, 0, sizeof(*ctrl));
    ctrl->regs = (volatile NVME_REGS *)(uintptr_t)bar0;

    // disable controller
    ctrl->regs->cc.en = 0;
    while (ctrl->regs->csts.rdy) { __asm__("pause"); }

    // allocate admin queues
    ctrl->admin_q.sq_entries = kernel_alloc_page();
    ctrl->admin_q.cq_entries = kernel_alloc_page();
    ctrl->admin_q.expected_phase = 1;
    ctrl->regs->asq = (uintptr_t)ctrl->admin_q.sq_entries;
    ctrl->regs->acq = (uintptr_t)ctrl->admin_q.cq_entries;
    ctrl->regs->aqa.asqs = NVME_QUEUE_ENTRY_COUNT - 1;
    ctrl->regs->aqa.acqs = NVME_QUEUE_ENTRY_COUNT - 1;

    // doorbell offsets
    uint32_t d = 4 * (1 << ((ctrl->regs->cap >> 32) & 0xF));
    uintptr_t db_base = (uintptr_t)ctrl->regs + 0x1000;
    ctrl->admin_q.sq_doorbell = (volatile uint32_t*)(db_base);
    ctrl->admin_q.cq_doorbell = (volatile uint32_t*)(db_base + d);
    ctrl->io_q.sq_doorbell    = (volatile uint32_t*)(db_base + 2*d);
    ctrl->io_q.cq_doorbell    = (volatile uint32_t*)(db_base + 3*d);

    // enable controller
    ctrl->regs->cc.en = 1;
    while (!ctrl->regs->csts.rdy) { __asm__("pause"); }

    // allocate io queues
    ctrl->io_q.sq_entries = kernel_alloc_page();
    ctrl->io_q.cq_entries = kernel_alloc_page();
    ctrl->io_q.expected_phase = 1;

    // create IO completion queue (admin)
    NVME_SQE *sqe = &ctrl->admin_q.sq_entries[0];
    memset(sqe, 0, sizeof(NVME_SQE));
    sqe->opc = NVME_CREATE_IO_COMPLETION_QUEUE;
    ctrl->admin_q.last_cid++;
    sqe->cid = ctrl->admin_q.last_cid;
    sqe->prp[0] = (uintptr_t)ctrl->io_q.cq_entries;
    sqe->cdw10 = (NVME_QUEUE_ENTRY_COUNT - 1) << 16 | 0x01;
    sqe->cdw11 = 0x01;
    *ctrl->admin_q.sq_doorbell = 1;
    nvme_wait(&ctrl->admin_q);

    // create IO submission queue (admin)
    sqe = &ctrl->admin_q.sq_entries[1];
    memset(sqe, 0, sizeof(NVME_SQE));
    sqe->opc = NVME_CREATE_IO_SUBMISSION_QUEUE;
    ctrl->admin_q.last_cid++;
    sqe->cid = ctrl->admin_q.last_cid;
    sqe->prp[0] = (uintptr_t)ctrl->io_q.sq_entries;
    sqe->cdw10 = (NVME_QUEUE_ENTRY_COUNT - 1) << 16 | 0x01;
    sqe->cdw11 = 0x01 | (0x01 << 16);
    *ctrl->admin_q.sq_doorbell = 2;
    nvme_wait(&ctrl->admin_q);

    ctrl->nsid = 1;
    printf("NVMe: initialized (BAR0=%p)\n", (uintptr_t)bar0);
    return 0;
}


/* --- Main --- */
void kmain(KernelGOPInfo *kgi) {
    fbb = (uint32_t*)(uintptr_t)kgi->FrameBufferBase;
    pps = kgi->PixelsPerScanLine;
    screen_width = kgi->Width;
    screen_height = kgi->Height;

    // Initial leeren
    for(uint32_t i=0; i < (screen_height * pps); i++) fbb[i] = 0;

    active_color = 0x00FF00;
    k_printf("Kernel Online. Write-Combining aktiv.\n");
    
    setup_paging(kgi);
    
    // Stresstest für Scrolling
    for(uint64_t i = 0; i < 100; i++) {
        active_color = (i % 2 == 0) ? 0xFFFF00 : 0xFFFFFF;
        k_printf("Zeile %x: High-Speed Scrolling!\n", i);
        
        // Kleiner Sicherheits-Break für echte Hardware (optional)
        // if(i % 1000 == 0) __asm__ volatile("pause");
    }
    
    
    printf("Kernel Online. Initializing NVMe...\n");

    nvme_ctrl_t ctrl;
    if (nvme_init(&ctrl) == 0) {
        active_color = 0xFFFF00;
        printf("NVMe Ready. Reading LBA 0...\n");
        void *buf = kernel_alloc_page();
        if (buf) {
            nvme_read_sector(&ctrl, 0, buf);
            active_color = 0xFFFFFF;
            debug_hexdump(buf, 512);
        } else {
            printf("Failed to allocate buffer for NVMe read\n");
        }
    } else {
        printf("NVMe initialization failed or not present.\n");
    }

    printf("Kernel: entering idle loop\n");
    for (;;) { __asm__("hlt"); }

    while (1) { __asm__ volatile("hlt"); }
}

__attribute__((naked)) void _start() {
    __asm__ volatile (
        "mov %rcx, %rdi\n\t"
        "sub $8, %rsp\n\t"
        "call kmain\n\t"
        "hlt"
    );
}
