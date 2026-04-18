// kernel/kernel.c
// Amalgamated kernel with corrected ISR/IRQ stubs, safe keyboard IRQ handling,
// full-screen blue background, native 8x8 font, paging, GDT/TSS, IDT, PIC remap.
// Keyboard scancodes are buffered in IRQ context and processed in main context.

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

/* --- 1. Constants & Macros --- */
#define PAGE_PRESENT (1ULL << 0)
#define PAGE_RW      (1ULL << 1)
#define PAGE_PWT     (1ULL << 3)
#define PAGE_PS      (1ULL << 7)

#define CHAR_WIDTH   8
#define CHAR_HEIGHT  8
#define MARGIN_X     0
#define MARGIN_Y     0

/* Colors (0xRRGGBB) */
#define BG_COLOR 0x0000FF   /* blue background */
#define FG_COLOR 0x00FFFFFF /* white foreground */

#define HEAP_SIZE   0x1000000UL

/* PIC ports */
#define PIC1_CMD 0x20
#define PIC1_DATA 0x21
#define PIC2_CMD 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

typedef struct {
    uint32_t Width;
    uint32_t Height;
    uint32_t PixelsPerScanLine;
    uint64_t FrameBufferBase;
} KernelGOPInfo;

/* --- 2. GDT & TSS Structures --- */
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
    uint32_t res0; uint64_t rsp0, rsp1, rsp2, res1, ist[7], res2;
    uint16_t res3, iopb_offset;
} __attribute__((packed));

/* --- 3. Global State --- */
uint64_t pml4[512] __attribute__((aligned(4096)));
uint8_t page_heap[HEAP_SIZE] __attribute__((aligned(4096)));
static size_t page_ptr = 0;

static struct {
    struct GDTEntry null, code, data;
    struct GDTSystemDescriptor tss;
} __attribute__((packed, aligned(8))) gdt_table;

static struct GDTPtr gdt_ptr;
static struct TSS kernel_tss;
static uint8_t tss_stack[4096];

uint32_t pps, screen_width, screen_height;
size_t buffer_size_bytes;

volatile uint32_t *fbb;        /* backbuffer in RAM for drawing */
volatile uint32_t *frontbuffer;/* VRAM pointer */
uint32_t *backbuffer;

static uint32_t cursor_x = MARGIN_X, cursor_y = MARGIN_Y;

/* --- 4. Font 8x8 Basic (Full Array) --- */
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

/* --- IDT structures --- */
struct IDTEntry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed));

struct IDTPtr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct IDTEntry idt[256] __attribute__((aligned(16)));
static struct IDTPtr idt_ptr;

/* --- 5. Utilities & Low-level I/O --- */
void* memset(void* dest, int ch, size_t count) {
    uint8_t* p = (uint8_t*)dest;
    while (count--) *p++ = (uint8_t)ch;
    return dest;
}

void* memcpy(void* dest, const void* src, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    while (n--) *d++ = *s++;
    return dest;
}

/* memcpy64: optimized copy using 64-bit words, handles remainder bytes */
void memcpy64(void* dest, const void* src, size_t count) {
    uint8_t* d8 = (uint8_t*)dest;
    const uint8_t* s8 = (const uint8_t*)src;
    size_t n64 = count / 8;
    uint64_t* d64 = (uint64_t*)d8;
    const uint64_t* s64 = (const uint64_t*)s8;
    while (n64--) *d64++ = *s64++;
    size_t rem = count % 8;
    d8 = (uint8_t*)d64;
    s8 = (const uint8_t*)s64;
    while (rem--) *d8++ = *s8++;
}

/* I/O ports */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" :: "a"(val), "Nd"(port));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* kernel_alloc_page: allocate a zeroed 4KiB page from page_heap */
void* kernel_alloc_page() {
    if (page_ptr + 4096 > HEAP_SIZE) return NULL;
    void* ptr = &page_heap[page_ptr];
    page_ptr += 4096;
    memset(ptr, 0, 4096);
    return ptr;
}

/* Blit the Backbuffer to the Frontbuffer (VRAM) */
void swap_buffers() {
    if (!frontbuffer || !backbuffer) return;
    memcpy64((void*)frontbuffer, (const void*)backbuffer, buffer_size_bytes);
}

/* --- PIC remap and helpers --- */
void pic_remap(int offset1, int offset2) {
    uint8_t a1 = inb(PIC1_DATA);
    uint8_t a2 = inb(PIC2_DATA);

    outb(PIC1_CMD, 0x11);
    outb(PIC2_CMD, 0x11);

    outb(PIC1_DATA, offset1);
    outb(PIC2_DATA, offset2);

    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}

static inline void pic_send_eoi(unsigned char irq) {
    if (irq >= 8) outb(PIC2_CMD, PIC_EOI);
    outb(PIC1_CMD, PIC_EOI);
}

/* --- IDT helpers --- */
void set_idt_entry(int vector, void* handler, uint8_t ist, uint8_t type_attr) {
    uint64_t addr = (uint64_t)handler;
    idt[vector].offset_low = (uint16_t)(addr & 0xFFFF);
    idt[vector].selector = 0x08; /* kernel code segment */
    idt[vector].ist = ist & 0x7;
    idt[vector].type_attr = type_attr;
    idt[vector].offset_mid = (uint16_t)((addr >> 16) & 0xFFFF);
    idt[vector].offset_high = (uint32_t)((addr >> 32) & 0xFFFFFFFF);
    idt[vector].zero = 0;
}

/* --- Exception messages --- */
static const char *exception_messages[32] = {
    "Divide Error","Debug","NMI","Breakpoint","Overflow","BOUND Range Exceeded","Invalid Opcode","Device Not Available",
    "Double Fault","Coprocessor Segment Overrun","Invalid TSS","Segment Not Present","Stack-Segment Fault","General Protection Fault",
    "Page Fault","Reserved","x87 FPU","Alignment Check","Machine Check","SIMD FP","Virtualization","Control Protection",
    "Reserved","Reserved","Reserved","Reserved","Reserved","Hypervisor Injection","VMM Communication","Security Exception","Reserved"
};

/* --- 6. Keyboard scancode buffer (IRQ-safe) --- */
#define KBD_BUF_SIZE 256
volatile uint8_t kbd_buf[KBD_BUF_SIZE];
volatile unsigned int kbd_head = 0;
volatile unsigned int kbd_tail = 0;

/* Minimal keyboard IRQ handler: read scancode and push to ring buffer.
   Must be tiny and safe to run in IRQ context. */
static inline void irq_keyboard_minimal(void) {
    uint8_t sc = inb(0x60);
    unsigned int next = (kbd_head + 1) & (KBD_BUF_SIZE - 1);
    if (next != kbd_tail) { /* not full */
        kbd_buf[kbd_head] = sc;
        kbd_head = next;
    }
    /* else drop scancode if buffer full */
}

/* --- 7. ISR/IRQ stubs and dispatcher --- */
/* We'll provide two kinds of stubs:
   - no-error-code stubs (most IRQs and many exceptions)
   - error-code stubs (exceptions that push an error code)
   Each stub:
     - reads error code (if present) into RSI (or 0)
     - moves vector number into RDI
     - saves general registers
     - calls isr_handler_c(vector, errcode)
     - restores registers and iretq
*/

/* Forward declaration of C handler */
void isr_handler_c(uint64_t vector, uint64_t errcode);

/* Define naked stubs for exceptions 0..31 and IRQs 32..47.
   For exceptions that push an error code: vectors 8,10,11,12,13,14,17
*/
#define ISR_NOERR_STUB(n) \
__attribute__((naked)) void isr_stub_##n(void) { \
    __asm__ volatile ( \
        "xorq %rax, %rax\n\t" /* ensure rax not used */ \
        "movq $" #n ", %rdi\n\t" /* vector -> rdi */ \
        "xorq %rsi, %rsi\n\t" /* errcode = 0 -> rsi */ \
        /* save caller registers */ \
        "pushq %rax\n\t" \
        "pushq %rbx\n\t" \
        "pushq %rcx\n\t" \
        "pushq %rdx\n\t" \
        "pushq %rsi\n\t" \
        "pushq %rdi\n\t" \
        "pushq %rbp\n\t" \
        "pushq %r8\n\t" \
        "pushq %r9\n\t" \
        "pushq %r10\n\t" \
        "pushq %r11\n\t" \
        "call isr_handler_c\n\t" \
        "popq %r11\n\t" \
        "popq %r10\n\t" \
        "popq %r9\n\t" \
        "popq %r8\n\t" \
        "popq %rbp\n\t" \
        "popq %rdi\n\t" \
        "popq %rsi\n\t" \
        "popq %rdx\n\t" \
        "popq %rcx\n\t" \
        "popq %rbx\n\t" \
        "popq %rax\n\t" \
        "iretq\n\t" \
    ); \
}

#define ISR_ERR_STUB(n) \
__attribute__((naked)) void isr_stub_##n(void) { \
    __asm__ volatile ( \
        /* error code is at top of stack now; read it into rsi */ \
        "movq (%rsp), %rsi\n\t" \
        "movq $" #n ", %rdi\n\t" \
        /* save registers */ \
        "pushq %rax\n\t" \
        "pushq %rbx\n\t" \
        "pushq %rcx\n\t" \
        "pushq %rdx\n\t" \
        "pushq %rsi\n\t" \
        "pushq %rdi\n\t" \
        "pushq %rbp\n\t" \
        "pushq %r8\n\t" \
        "pushq %r9\n\t" \
        "pushq %r10\n\t" \
        "pushq %r11\n\t" \
        "call isr_handler_c\n\t" \
        "popq %r11\n\t" \
        "popq %r10\n\t" \
        "popq %r9\n\t" \
        "popq %r8\n\t" \
        "popq %rbp\n\t" \
        "popq %rdi\n\t" \
        "popq %rsi\n\t" \
        "popq %rdx\n\t" \
        "popq %rcx\n\t" \
        "popq %rbx\n\t" \
        "popq %rax\n\t" \
        "iretq\n\t" \
    ); \
}

/* Create stubs for exceptions 0..31 */
ISR_NOERR_STUB(0)  ISR_NOERR_STUB(1)  ISR_NOERR_STUB(2)  ISR_NOERR_STUB(3)
ISR_NOERR_STUB(4)  ISR_NOERR_STUB(5)  ISR_NOERR_STUB(6)  ISR_NOERR_STUB(7)
ISR_ERR_STUB(8)    ISR_NOERR_STUB(9)  ISR_ERR_STUB(10) ISR_ERR_STUB(11)
ISR_ERR_STUB(12)   ISR_ERR_STUB(13)   ISR_ERR_STUB(14)  ISR_NOERR_STUB(15)
ISR_NOERR_STUB(16) ISR_ERR_STUB(17)   ISR_NOERR_STUB(18) ISR_NOERR_STUB(19)
ISR_NOERR_STUB(20) ISR_NOERR_STUB(21) ISR_NOERR_STUB(22) ISR_NOERR_STUB(23)
ISR_NOERR_STUB(24) ISR_NOERR_STUB(25) ISR_NOERR_STUB(26) ISR_NOERR_STUB(27)
ISR_NOERR_STUB(28) ISR_NOERR_STUB(29) ISR_NOERR_STUB(30) ISR_NOERR_STUB(31)

/* Create IRQ stubs for vectors 32..47 (no error codes) */
ISR_NOERR_STUB(32) ISR_NOERR_STUB(33) ISR_NOERR_STUB(34) ISR_NOERR_STUB(35)
ISR_NOERR_STUB(36) ISR_NOERR_STUB(37) ISR_NOERR_STUB(38) ISR_NOERR_STUB(39)
ISR_NOERR_STUB(40) ISR_NOERR_STUB(41) ISR_NOERR_STUB(42) ISR_NOERR_STUB(43)
ISR_NOERR_STUB(44) ISR_NOERR_STUB(45) ISR_NOERR_STUB(46) ISR_NOERR_STUB(47)

/* --- 8. C-level ISR/IRQ handler --- */
/* isr_handler_c: called with RDI=vector, RSI=errcode (0 if none) */
void isr_handler_c(uint64_t vector, uint64_t errcode) {
    if (vector < 32) {
        /* Exception: print message and halt */
        /* Avoid complex operations if possible; printing is allowed here
           because exceptions are fatal in this simple kernel. */
        /* Use printf (which uses swap_buffers) to show message on screen. */
        extern void printf(const char*, ...);
        const char *msg = exception_messages[vector];
        if (!msg) msg = "Unknown";
        printf("Exception %d: %s\n", (int)vector, msg);
        if (vector == 14) { /* page fault: read CR2 */
            uint64_t cr2;
            __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
            printf("CR2=0x%x\n", (unsigned int)cr2);
        }
        while (1) { __asm__("hlt"); }
    } else if (vector >= 32 && vector < 48) {
        int irq = (int)(vector - 32);
        if (irq == 1) {
            /* keyboard IRQ: minimal handler */
            irq_keyboard_minimal();
        }
        /* send EOI for PIC */
        pic_send_eoi((unsigned char)irq);
        /* return to interrupted code */
    } else {
        /* other interrupts: ignore for now */
    }
}

/* --- 9. IDT initialization --- */
void init_idt() {
    memset(idt, 0, sizeof(idt));
    /* Exceptions 0..31 */
    set_idt_entry(0,  isr_stub_0,  0, 0x8E);
    set_idt_entry(1,  isr_stub_1,  0, 0x8E);
    set_idt_entry(2,  isr_stub_2,  0, 0x8E);
    set_idt_entry(3,  isr_stub_3,  0, 0x8E);
    set_idt_entry(4,  isr_stub_4,  0, 0x8E);
    set_idt_entry(5,  isr_stub_5,  0, 0x8E);
    set_idt_entry(6,  isr_stub_6,  0, 0x8E);
    set_idt_entry(7,  isr_stub_7,  0, 0x8E);
    set_idt_entry(8,  isr_stub_8,  0, 0x8E);
    set_idt_entry(9,  isr_stub_9,  0, 0x8E);
    set_idt_entry(10, isr_stub_10, 0, 0x8E);
    set_idt_entry(11, isr_stub_11, 0, 0x8E);
    set_idt_entry(12, isr_stub_12, 0, 0x8E);
    set_idt_entry(13, isr_stub_13, 0, 0x8E);
    set_idt_entry(14, isr_stub_14, 0, 0x8E);
    set_idt_entry(15, isr_stub_15, 0, 0x8E);
    set_idt_entry(16, isr_stub_16, 0, 0x8E);
    set_idt_entry(17, isr_stub_17, 0, 0x8E);
    set_idt_entry(18, isr_stub_18, 0, 0x8E);
    set_idt_entry(19, isr_stub_19, 0, 0x8E);
    set_idt_entry(20, isr_stub_20, 0, 0x8E);
    set_idt_entry(21, isr_stub_21, 0, 0x8E);
    set_idt_entry(22, isr_stub_22, 0, 0x8E);
    set_idt_entry(23, isr_stub_23, 0, 0x8E);
    set_idt_entry(24, isr_stub_24, 0, 0x8E);
    set_idt_entry(25, isr_stub_25, 0, 0x8E);
    set_idt_entry(26, isr_stub_26, 0, 0x8E);
    set_idt_entry(27, isr_stub_27, 0, 0x8E);
    set_idt_entry(28, isr_stub_28, 0, 0x8E);
    set_idt_entry(29, isr_stub_29, 0, 0x8E);
    set_idt_entry(30, isr_stub_30, 0, 0x8E);
    set_idt_entry(31, isr_stub_31, 0, 0x8E);

    /* IRQs 32..47 */
    set_idt_entry(32, isr_stub_32, 0, 0x8E);
    set_idt_entry(33, isr_stub_33, 0, 0x8E); /* keyboard */
    set_idt_entry(34, isr_stub_34, 0, 0x8E);
    set_idt_entry(35, isr_stub_35, 0, 0x8E);
    set_idt_entry(36, isr_stub_36, 0, 0x8E);
    set_idt_entry(37, isr_stub_37, 0, 0x8E);
    set_idt_entry(38, isr_stub_38, 0, 0x8E);
    set_idt_entry(39, isr_stub_39, 0, 0x8E);
    set_idt_entry(40, isr_stub_40, 0, 0x8E);
    set_idt_entry(41, isr_stub_41, 0, 0x8E);
    set_idt_entry(42, isr_stub_42, 0, 0x8E);
    set_idt_entry(43, isr_stub_43, 0, 0x8E);
    set_idt_entry(44, isr_stub_44, 0, 0x8E);
    set_idt_entry(45, isr_stub_45, 0, 0x8E);
    set_idt_entry(46, isr_stub_46, 0, 0x8E);
    set_idt_entry(47, isr_stub_47, 0, 0x8E);

    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint64_t)&idt;
    __asm__ volatile ("lidt %0" :: "m"(idt_ptr) : "memory");
}

/* --- 10. Drawing, scrolling, printing --- */
void scroll() {
    uint32_t top = MARGIN_Y;
    uint32_t bottom = screen_height;
    if (screen_height < CHAR_HEIGHT) return;

    for (uint32_t y = top; y + CHAR_HEIGHT < bottom; y++) {
        uint32_t dst = y * pps;
        uint32_t src = (y + CHAR_HEIGHT) * pps;
        memcpy64((void*)&fbb[dst], (const void*)&fbb[src], (size_t)pps * 4);
    }

    for (uint32_t y = bottom - CHAR_HEIGHT; y < bottom; y++) {
        uint32_t base = y * pps;
        for (uint32_t x = 0; x < pps; x++) fbb[base + x] = BG_COLOR;
    }

    if (cursor_y >= (uint32_t)CHAR_HEIGHT) cursor_y -= CHAR_HEIGHT;
    else cursor_y = top;

    swap_buffers();
}

void draw_char(char c) {
    if (c == '\n') {
        cursor_x = MARGIN_X;
        cursor_y += CHAR_HEIGHT;
        if (cursor_y + CHAR_HEIGHT > screen_height) scroll();
        return;
    }
    if (c == '\r') {
        cursor_x = MARGIN_X;
        return;
    }
    if (cursor_x + CHAR_WIDTH > screen_width) {
        cursor_x = MARGIN_X;
        cursor_y += CHAR_HEIGHT;
        if (cursor_y + CHAR_HEIGHT > screen_height) scroll();
    }
    if ((unsigned char)c < 32 || (unsigned char)c > 126) return;
    const uint8_t* g = font8x8_basic[(unsigned char)c - 32];
    for (int r = 0; r < 8; r++) {
        uint32_t y = cursor_y + r;
        if (y >= screen_height) continue;
        uint32_t base = y * pps + cursor_x;
        for (int col = 0; col < 8; col++) {
            uint32_t x = cursor_x + col;
            if (x >= screen_width) continue;
            if ((g[r] >> (7 - col)) & 1) fbb[base + col] = FG_COLOR;
            else fbb[base + col] = BG_COLOR;
        }
    }
    cursor_x += CHAR_WIDTH;
}

/* Minimal printf supporting %d %x %s and literal characters. */
void printf(const char* fmt, ...) {
    va_list args; va_start(args, fmt);
    while (*fmt) {
        if (*fmt == '%' && *(fmt + 1)) {
            fmt++;
            if (*fmt == 'd') {
                int n = va_arg(args, int);
                if (n == 0) { draw_char('0'); }
                else {
                    char buf[32]; int i = 0; int neg = 0;
                    long long v = n;
                    if (v < 0) { neg = 1; v = -v; }
                    while (v > 0) { buf[i++] = '0' + (v % 10); v /= 10; }
                    if (neg) buf[i++] = '-';
                    while (i > 0) draw_char(buf[--i]);
                }
            } else if (*fmt == 'x') {
                unsigned int n = va_arg(args, unsigned int);
                if (n == 0) { draw_char('0'); }
                else {
                    char buf[32]; int i = 0;
                    unsigned int v = n;
                    while (v > 0) {
                        unsigned int m = v & 0xF;
                        buf[i++] = (m < 10) ? ('0' + m) : ('A' + (m - 10));
                        v >>= 4;
                    }
                    while (i > 0) draw_char(buf[--i]);
                }
            } else if (*fmt == 's') {
                const char* s = va_arg(args, const char*);
                if (!s) s = "(null)";
                while (*s) draw_char(*s++);
            } else if (*fmt == '%') {
                draw_char('%');
            } else {
                draw_char('%'); draw_char(*fmt);
            }
        } else {
            draw_char(*fmt);
        }
        fmt++;
    }
    va_end(args);
    swap_buffers();
}

/* --- 11. Paging, GDT & TSS Init --- */
void init_paging(KernelGOPInfo *kgi) {
    const uint64_t flags = PAGE_PRESENT | PAGE_RW;
    const uint64_t fb_flags = PAGE_PRESENT | PAGE_RW | PAGE_PWT;
    uint64_t *pdpt = (uint64_t*)kernel_alloc_page();
    if (!pdpt) return;
    pml4[0] = (uint64_t)pdpt | flags;
    for (int i = 0; i < 4; i++) {
        uint64_t *pd = (uint64_t*)kernel_alloc_page();
        if (!pd) return;
        pdpt[i] = (uint64_t)pd | flags;
        for (int j = 0; j < 512; j++) {
            uint64_t addr = (uint64_t)(i * 512 + j) * 0x200000ULL;
            uint64_t f = (addr >= kgi->FrameBufferBase && addr < kgi->FrameBufferBase + 0x2000000ULL) ? fb_flags : flags;
            pd[j] = addr | f | PAGE_PS;
        }
    }
    __asm__ volatile ("mov %0, %%cr3" :: "r"(pml4) : "memory");
}

void init_gdt_tss() {
    memset(&gdt_table, 0, sizeof(gdt_table));
    gdt_table.code.access = 0x9A; gdt_table.code.gran = 0x20;
    gdt_table.data.access = 0x92;
    uint64_t tss_base = (uint64_t)&kernel_tss;
    memset(&kernel_tss, 0, sizeof(kernel_tss));
    kernel_tss.rsp0 = kernel_tss.ist[0] = (uint64_t)tss_stack + sizeof(tss_stack);
    kernel_tss.iopb_offset = sizeof(kernel_tss);
    gdt_table.tss.low.limit_low = sizeof(kernel_tss) - 1;
    gdt_table.tss.low.base_low = (uint16_t)tss_base;
    gdt_table.tss.low.base_mid = (uint8_t)(tss_base >> 16);
    gdt_table.tss.low.access = 0x89;
    gdt_table.tss.low.base_high = (uint8_t)(tss_base >> 24);
    gdt_table.tss.base_upper32 = (uint32_t)(tss_base >> 32);
    gdt_ptr.limit = sizeof(gdt_table) - 1;
    gdt_ptr.base = (uint64_t)&gdt_table;
    __asm__ volatile ("lgdt %0\n\tmov $0x18, %%ax\n\tltr %%ax" :: "m"(gdt_ptr) : "rax", "memory");
}

/* --- 12. Keyboard processing in main context --- */
void process_keyboard_buffer(void) {
    while (kbd_tail != kbd_head) {
        uint8_t sc = kbd_buf[kbd_tail];
        kbd_tail = (kbd_tail + 1) & (KBD_BUF_SIZE - 1);
        /* For now print scancode in hex. Converting to ASCII requires scancode map. */
        printf("Keyboard scancode: 0x%x\n", (unsigned int)sc);
    }
}

/* --- 13. Main --- */
void kmain(KernelGOPInfo *kgi) {
    frontbuffer = (volatile uint32_t*)(uintptr_t)kgi->FrameBufferBase;
    pps = kgi->PixelsPerScanLine;
    screen_width = kgi->Width;
    screen_height = kgi->Height;
    buffer_size_bytes = (size_t)screen_height * (size_t)pps * 4UL;

    init_paging(kgi);
    init_gdt_tss();

    int pages_needed = (int)((buffer_size_bytes + 4095) / 4096);
    void* bb_start = kernel_alloc_page();
    if (!bb_start) { while (1) { __asm__("hlt"); } }
    for (int i = 1; i < pages_needed; i++) {
        if (!kernel_alloc_page()) { while (1) { __asm__("hlt"); } }
    }
    backbuffer = (uint32_t*)bb_start;
    fbb = (volatile uint32_t*)backbuffer;

    /* Clear backbuffer to BG_COLOR */
    for (uint32_t y = 0; y < screen_height; y++) {
        uint32_t base = y * pps;
        for (uint32_t x = 0; x < pps; x++) fbb[base + x] = BG_COLOR;
    }
    swap_buffers();

    cursor_x = MARGIN_X;
    cursor_y = MARGIN_Y;

    /* Initialize IDT and PIC, enable keyboard IRQ only */
    pic_remap(32, 40);

    /* Mask all IRQs except keyboard (IRQ1). Master mask: unmask IRQ1 (bit1=0) */
    outb(PIC1_DATA, 0xFD); /* 11111101 -> only IRQ1 unmasked */
    outb(PIC2_DATA, 0xFF); /* mask all on slave */

    init_idt();

    /* Enable interrupts */
    __asm__ volatile ("sti");

    printf("Kernel: GDT/TSS and Paging Loaded.\n");
    printf("Video: Double Buffered (RAM -> VRAM), full-screen, blue background.\n");
    printf("IDT initialized. Keyboard IRQ enabled (buffered).\n");

    /* Main loop: process keyboard buffer and halt until next interrupt */
    while (1) {
        process_keyboard_buffer();
        __asm__ volatile ("hlt");
    }
}

/* Entry point: RCX contains GOP pointer; move to RDI and call kmain */
__attribute__((naked)) void _start(void) {
    __asm__ volatile (
        "cli\n\t"
        "mov %rcx, %rdi\n\t"
        "sub $8, %rsp\n\t"
        "call kmain\n\t"
        "hlt\n\t"
    );
}

