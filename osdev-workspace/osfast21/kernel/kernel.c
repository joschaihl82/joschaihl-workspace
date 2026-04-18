// kernel/kernel.c
// Amalgamated kernel with IDT, exception handlers, PIC remap, and keyboard IRQ (no margins, blue background)

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

/* --- 1. Constants & Macros --- */
#define PAGE_PRESENT (1ULL << 0)
#define PAGE_RW      (1ULL << 1)
#define PAGE_PWT     (1ULL << 3) // Write-Through caching
#define PAGE_PS      (1ULL << 7) // 2MiB Page

/* Use native 8x8 font (no stretching) and no margins */
#define CHAR_WIDTH   8
#define CHAR_HEIGHT  8
#define MARGIN_X     0
#define MARGIN_Y     0

/* Background color for the whole screen (blue).
   Use 0x0000FF for pure blue in 0xRRGGBB format. */
#define BG_COLOR 0x0000FF
#define FG_COLOR 0x00FFFFFF

// Increased heap size to 16MB to hold a full 1080p/4k backbuffer
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
// Expanded Heap for Backbuffer storage
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

// fbb will point to backbuffer (RAM) for drawing operations
volatile uint32_t *fbb;
// frontbuffer points to actual VRAM
volatile uint32_t *frontbuffer;
// backbuffer points to allocated RAM
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

void printf(const char* fmt, ...);

/* --- 5. Utilities & Low-level I/O --- */
void* memset(void* dest, int ch, size_t count) {
    uint8_t* p = (uint8_t*)dest;
    while (count--) *p++ = (uint8_t)ch;
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
    if (page_ptr + 4096 > HEAP_SIZE) return NULL; // Safety check
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
    // Save masks
    uint8_t a1 = inb(PIC1_DATA);
    uint8_t a2 = inb(PIC2_DATA);

    // Start initialization sequence (in cascade mode)
    outb(PIC1_CMD, 0x11);
    outb(PIC2_CMD, 0x11);

    // Set vector offsets
    outb(PIC1_DATA, offset1);
    outb(PIC2_DATA, offset2);

    // Tell Master PIC there is a slave at IRQ2 (0000 0100)
    outb(PIC1_DATA, 0x04);
    // Tell Slave PIC its cascade identity (0000 0010)
    outb(PIC2_DATA, 0x02);

    // Set 8086/88 (MCS-80/85) mode
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    // Restore saved masks
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
    idt[vector].selector = 0x08; // code segment selector
    idt[vector].ist = ist & 0x7;
    idt[vector].type_attr = type_attr;
    idt[vector].offset_mid = (uint16_t)((addr >> 16) & 0xFFFF);
    idt[vector].offset_high = (uint32_t)((addr >> 32) & 0xFFFFFFFF);
    idt[vector].zero = 0;
}

/* Forward declarations for assembly stubs */
#define DECL_ISR(n) __attribute__((naked)) void isr_stub_##n(void)
#define DECL_IRQ(n) __attribute__((naked)) void irq_stub_##n(void)

DECL_ISR(0);  DECL_ISR(1);  DECL_ISR(2);  DECL_ISR(3);  DECL_ISR(4);  DECL_ISR(5);  DECL_ISR(6);  DECL_ISR(7);
DECL_ISR(8);  DECL_ISR(9);  DECL_ISR(10); DECL_ISR(11); DECL_ISR(12); DECL_ISR(13); DECL_ISR(14); DECL_ISR(15);
DECL_ISR(16); DECL_ISR(17); DECL_ISR(18); DECL_ISR(19); DECL_ISR(20); DECL_ISR(21); DECL_ISR(22); DECL_ISR(23);
DECL_ISR(24); DECL_ISR(25); DECL_ISR(26); DECL_ISR(27); DECL_ISR(28); DECL_ISR(29); DECL_ISR(30); DECL_ISR(31);

DECL_IRQ(0); DECL_IRQ(1); DECL_IRQ(2); DECL_IRQ(3); DECL_IRQ(4); DECL_IRQ(5); DECL_IRQ(6); DECL_IRQ(7);
DECL_IRQ(8); DECL_IRQ(9); DECL_IRQ(10); DECL_IRQ(11); DECL_IRQ(12); DECL_IRQ(13); DECL_IRQ(14); DECL_IRQ(15);

/* Common stub entry: pushes registers, calls C handler, restores and iretq */
__attribute__((naked)) void isr_common_stub(void) {
    __asm__ volatile (
        // Save general-purpose registers (caller-saved + callee-saved)
        "pushq %rax\n\t"
        "pushq %rbx\n\t"
        "pushq %rcx\n\t"
        "pushq %rdx\n\t"
        "pushq %rsi\n\t"
        "pushq %rdi\n\t"
        "pushq %rbp\n\t"
        "pushq %r8\n\t"
        "pushq %r9\n\t"
        "pushq %r10\n\t"
        "pushq %r11\n\t"
        // rdi already contains vector number from stub
        "call isr_common_c\n\t"
        "popq %r11\n\t"
        "popq %r10\n\t"
        "popq %r9\n\t"
        "popq %r8\n\t"
        "popq %rbp\n\t"
        "popq %rdi\n\t"
        "popq %rsi\n\t"
        "popq %rdx\n\t"
        "popq %rcx\n\t"
        "popq %rbx\n\t"
        "popq %rax\n\t"
        "iretq\n\t"
    );
}

/* Each isr_stub_X moves its vector number into rdi and jumps to common stub */
#define ISR_STUB_BODY(n) \
__asm__ volatile ( "mov $" #n ", %rdi\n\tjmp isr_common_stub\n\t" );

#define IRQ_STUB_BODY(n, vec) \
__asm__ volatile ( "mov $" #vec ", %rdi\n\tjmp isr_common_stub\n\t" );

/* Define exception stubs 0..31 */
DECL_ISR(0)  { ISR_STUB_BODY(0) }
DECL_ISR(1)  { ISR_STUB_BODY(1) }
DECL_ISR(2)  { ISR_STUB_BODY(2) }
DECL_ISR(3)  { ISR_STUB_BODY(3) }
DECL_ISR(4)  { ISR_STUB_BODY(4) }
DECL_ISR(5)  { ISR_STUB_BODY(5) }
DECL_ISR(6)  { ISR_STUB_BODY(6) }
DECL_ISR(7)  { ISR_STUB_BODY(7) }
DECL_ISR(8)  { ISR_STUB_BODY(8) }
DECL_ISR(9)  { ISR_STUB_BODY(9) }
DECL_ISR(10) { ISR_STUB_BODY(10) }
DECL_ISR(11) { ISR_STUB_BODY(11) }
DECL_ISR(12) { ISR_STUB_BODY(12) }
DECL_ISR(13) { ISR_STUB_BODY(13) }
DECL_ISR(14) { ISR_STUB_BODY(14) }
DECL_ISR(15) { ISR_STUB_BODY(15) }
DECL_ISR(16) { ISR_STUB_BODY(16) }
DECL_ISR(17) { ISR_STUB_BODY(17) }
DECL_ISR(18) { ISR_STUB_BODY(18) }
DECL_ISR(19) { ISR_STUB_BODY(19) }
DECL_ISR(20) { ISR_STUB_BODY(20) }
DECL_ISR(21) { ISR_STUB_BODY(21) }
DECL_ISR(22) { ISR_STUB_BODY(22) }
DECL_ISR(23) { ISR_STUB_BODY(23) }
DECL_ISR(24) { ISR_STUB_BODY(24) }
DECL_ISR(25) { ISR_STUB_BODY(25) }
DECL_ISR(26) { ISR_STUB_BODY(26) }
DECL_ISR(27) { ISR_STUB_BODY(27) }
DECL_ISR(28) { ISR_STUB_BODY(28) }
DECL_ISR(29) { ISR_STUB_BODY(29) }
DECL_ISR(30) { ISR_STUB_BODY(30) }
DECL_ISR(31) { ISR_STUB_BODY(31) }

/* Define IRQ stubs (after PIC remap we'll use vectors 32..47).
   We'll map IRQ0..15 to vectors 32..47. */
DECL_IRQ(0)  { IRQ_STUB_BODY(0, 32) }
DECL_IRQ(1)  { IRQ_STUB_BODY(1, 33) } // keyboard IRQ -> vector 33
DECL_IRQ(2)  { IRQ_STUB_BODY(2, 34) }
DECL_IRQ(3)  { IRQ_STUB_BODY(3, 35) }
DECL_IRQ(4)  { IRQ_STUB_BODY(4, 36) }
DECL_IRQ(5)  { IRQ_STUB_BODY(5, 37) }
DECL_IRQ(6)  { IRQ_STUB_BODY(6, 38) }
DECL_IRQ(7)  { IRQ_STUB_BODY(7, 39) }
DECL_IRQ(8)  { IRQ_STUB_BODY(8, 40) }
DECL_IRQ(9)  { IRQ_STUB_BODY(9, 41) }
DECL_IRQ(10) { IRQ_STUB_BODY(10, 42) }
DECL_IRQ(11) { IRQ_STUB_BODY(11, 43) }
DECL_IRQ(12) { IRQ_STUB_BODY(12, 44) }
DECL_IRQ(13) { IRQ_STUB_BODY(13, 45) }
DECL_IRQ(14) { IRQ_STUB_BODY(14, 46) }
DECL_IRQ(15) { IRQ_STUB_BODY(15, 47) }

/* --- Exception messages --- */
static const char *exception_messages[32] = {
    "Divide Error",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
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
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved"
};

/* --- 6. C-level ISR/IRQ handlers --- */

/* isr_common_c: called with rdi = vector number */
void isr_common_c(uint64_t vector) {
    // Exceptions 0..31
    if (vector < 32) {
        // Print exception message and halt
        printf("Exception %d: ", (int)vector);
        const char *msg = exception_messages[vector];
        if (msg) printf("%s\n", msg);
        else printf("Unknown\n");
        // Halt to avoid continuing in an undefined state
        while (1) { __asm__("hlt"); }
    } else if (vector >= 32 && vector < 48) {
        // IRQs 0..15 mapped to vectors 32..47
        int irq = (int)(vector - 32);
        if (irq == 1) {
            // Keyboard IRQ
            uint8_t scancode = inb(0x60);
            // Simple handling: printable scancodes -> ASCII mapping is not implemented here.
            // We'll print scancode in hex.
            printf("Keyboard scancode: 0x%x\n", (unsigned int)scancode);
        }
        // Send EOI
        pic_send_eoi((unsigned char)irq);
    } else {
        // Other interrupts
        printf("Unhandled interrupt vector %d\n", (int)vector);
    }
}

/* --- 7. IDT initialization --- */
void init_idt() {
    memset(idt, 0, sizeof(idt));
    // Exceptions 0..31
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

    // IRQs 0..15 -> vectors 32..47
    set_idt_entry(32, irq_stub_0,  0, 0x8E);
    set_idt_entry(33, irq_stub_1,  0, 0x8E); // keyboard
    set_idt_entry(34, irq_stub_2,  0, 0x8E);
    set_idt_entry(35, irq_stub_3,  0, 0x8E);
    set_idt_entry(36, irq_stub_4,  0, 0x8E);
    set_idt_entry(37, irq_stub_5,  0, 0x8E);
    set_idt_entry(38, irq_stub_6,  0, 0x8E);
    set_idt_entry(39, irq_stub_7,  0, 0x8E);
    set_idt_entry(40, irq_stub_8,  0, 0x8E);
    set_idt_entry(41, irq_stub_9,  0, 0x8E);
    set_idt_entry(42, irq_stub_10, 0, 0x8E);
    set_idt_entry(43, irq_stub_11, 0, 0x8E);
    set_idt_entry(44, irq_stub_12, 0, 0x8E);
    set_idt_entry(45, irq_stub_13, 0, 0x8E);
    set_idt_entry(46, irq_stub_14, 0, 0x8E);
    set_idt_entry(47, irq_stub_15, 0, 0x8E);

    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint64_t)&idt;
    __asm__ volatile ("lidt %0" :: "m"(idt_ptr) : "memory");
}

/* --- 8. Drawing, scrolling, printing (unchanged except BG_COLOR/FG_COLOR) --- */
void scroll() {
    uint32_t top = MARGIN_Y;               // 0 by default
    uint32_t bottom = screen_height;       // full screen

    if (screen_height < CHAR_HEIGHT) {
        // Nothing to do
        return;
    }

    // Move every scanline up by CHAR_HEIGHT inside the full screen
    for (uint32_t y = top; y + CHAR_HEIGHT < bottom; y++) {
        uint32_t dst = y * pps;
        uint32_t src = (y + CHAR_HEIGHT) * pps;
        memcpy64((void*)&fbb[dst], (const void*)&fbb[src], (size_t)pps * 4);
    }

    // Clear the bottom CHAR_HEIGHT rows to BG_COLOR
    for (uint32_t y = bottom - CHAR_HEIGHT; y < bottom; y++) {
        uint32_t base = y * pps;
        for (uint32_t x = 0; x < pps; x++) {
            fbb[base + x] = BG_COLOR;
        }
    }

    // Adjust cursor
    if (cursor_y >= (uint32_t)CHAR_HEIGHT) cursor_y -= CHAR_HEIGHT;
    else cursor_y = top;

    // Push the updated backbuffer to VRAM
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
            if ((g[r] >> (7 - col)) & 1) {
                fbb[base + col] = FG_COLOR; // white pixel for glyph
            } else {
                fbb[base + col] = BG_COLOR; // ensure background is blue
            }
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
    // Trigger swap at end of print to make text visible
    swap_buffers();
}

/* --- 9. Paging, GDT & TSS Init (unchanged) --- */
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
    // Zero the GDT table to be safe
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

/* --- 10. Main --- */
void kmain(KernelGOPInfo *kgi) {
    // 1. Setup VRAM pointer and screen metrics
    frontbuffer = (volatile uint32_t*)(uintptr_t)kgi->FrameBufferBase;
    pps = kgi->PixelsPerScanLine;
    screen_width = kgi->Width;
    screen_height = kgi->Height;
    buffer_size_bytes = (size_t)screen_height * (size_t)pps * 4UL;

    // 2. Initialize Paging and GDT
    init_paging(kgi);
    init_gdt_tss();

    // 3. Allocate Backbuffer in RAM (continuous pages)
    int pages_needed = (int)((buffer_size_bytes + 4095) / 4096);
    void* bb_start = kernel_alloc_page();
    if (!bb_start) {
        // allocation failed; hang
        while (1) { __asm__("hlt"); }
    }
    for (int i = 1; i < pages_needed; i++) {
        if (!kernel_alloc_page()) {
            // allocation failed; hang
            while (1) { __asm__("hlt"); }
        }
    }
    backbuffer = (uint32_t*)bb_start;

    // 4. Point drawing operations (fbb) to the RAM Backbuffer
    fbb = (volatile uint32_t*)backbuffer;

    // Clear Backbuffer to BG_COLOR (blue background) across the whole screen
    for (uint32_t y = 0; y < screen_height; y++) {
        uint32_t base = y * pps;
        for (uint32_t x = 0; x < pps; x++) {
            fbb[base + x] = BG_COLOR;
        }
    }

    // Initial Swap (to clear actual screen)
    swap_buffers();

    // Reset cursor to top-left (no margins)
    cursor_x = MARGIN_X;
    cursor_y = MARGIN_Y;

    // 5. Initialize IDT, PIC, and enable keyboard IRQ
    // Remap PIC to vectors 32..47
    pic_remap(32, 40);

    // Unmask keyboard IRQ (IRQ1) only; mask others
    // Mask bits: 1 = masked. We want only IRQ1 unmasked -> mask = 0xFF except bit1 = 0
    // For master PIC (IRQs 0-7): unmask IRQ1 -> mask = 0xFF & ~(1<<1) = 0xFD
    // For slave PIC (IRQs 8-15): mask all -> 0xFF
    outb(PIC1_DATA, 0xFD);
    outb(PIC2_DATA, 0xFF);

    init_idt();

    // Enable interrupts after IDT and PIC are ready
    __asm__ volatile ("sti");

    printf("Kernel: GDT/TSS and Paging Loaded.\n");
    printf("Video: Double Buffered (RAM -> VRAM), full-screen, no margins, blue background.\n");
    printf("IDT initialized. Exceptions and IRQs registered. Keyboard IRQ enabled.\n");

    // Example loop printing to show keyboard scancodes
    for (int i = 0; i < 1000; i++) {
        printf("Loop %d\n", i);
    }

    while (1) { __asm__("hlt"); }
}

/* Entry point: pass GOP info in RCX -> RDI for kmain */
__attribute__((naked)) void _start(void) {
    __asm__ volatile (
        "cli\n\t"
        "mov %rcx, %rdi\n\t"
        "sub $8, %rsp\n\t"
        "call kmain\n\t"
        "hlt"
    );
}

