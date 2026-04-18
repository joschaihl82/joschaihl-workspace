// kernel/kernel.c
// Merged kernel with corrected ISR stubs, keyboard buffer, serial output,
// mapping of the physical framebuffer to a virtual address, and Ctrl+Alt+Del reboot.

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

/* --- 1. Constants & Macros --- */
#define PAGE_PRESENT (1ULL << 0)
#define PAGE_RW      (1ULL << 1)
#define PAGE_PWT     (1ULL << 3) /* Write-Through caching */
#define PAGE_PS      (1ULL << 7) /* 2MiB Page */

#define CHAR_WIDTH   16
#define CHAR_HEIGHT  20
#define MARGIN_X     20
#define MARGIN_Y     20

/* Virtual framebuffer base (canonical high-half) and mapping size */
#define VFB_BASE ((uint64_t)0xFFFF800000000000ULL)
#define FB_MAP_SIZE (0x2000000ULL) /* 32 MiB mapping window */

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
    uint32_t res0;
    uint64_t rsp0, rsp1, rsp2, res1;
    uint64_t ist[7];
    uint64_t res2;
    uint16_t res3, iopb_offset;
} __attribute__((packed));

/* --- 3. Global State --- */
uint64_t pml4[512] __attribute__((aligned(4096)));
uint8_t page_heap[0x100000] __attribute__((aligned(4096)));
static int page_ptr = 0;

static struct {
    struct GDTEntry null, code, data;
    struct GDTSystemDescriptor tss;
} __attribute__((packed, aligned(8))) gdt_table;

static struct GDTPtr gdt_ptr;
static struct TSS kernel_tss;
static uint8_t tss_stack[4096];

uint32_t pps, screen_width, screen_height;
volatile uint32_t *fbb;
static uint32_t cursor_x = MARGIN_X, cursor_y = MARGIN_Y;

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

/* --- 5. Utilities & Scrolling --- */
void* memset(void* dest, int ch, size_t count) {
    uint8_t* p = (uint8_t*)dest;
    while (count--) *p++ = (uint8_t)ch;
    return dest;
}

void* kernel_alloc_page() {
    void* ptr = &page_heap[page_ptr];
    page_ptr += 4096;
    memset(ptr, 0, 4096);
    return ptr;
}

void scroll() {
    for (uint32_t y = MARGIN_Y; y < screen_height - CHAR_HEIGHT; y++) {
        for (uint32_t x = 0; x < screen_width; x++) {
            fbb[y * pps + x] = fbb[(y + CHAR_HEIGHT) * pps + x];
        }
    }
    for (uint32_t y = screen_height - CHAR_HEIGHT; y < screen_height; y++) {
        for (uint32_t x = 0; x < screen_width; x++) {
            fbb[y * pps + x] = 0x222222;
        }
    }
    cursor_y -= CHAR_HEIGHT;
}

/* --- Serial (COM1) helpers --- */
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

/* --- 6. PIC, IDT, Exceptions, and Keyboard ISR additions --- */
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
    idt[n].selector    = 0x08; /* code segment */
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

/* Minimal US scancode set (make codes only) */
static const char scancode_to_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b', '\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,  'a','s',
    'd','f','g','h','j','k','l',';','\'','`', 0, '\\','z','x','c','v',
    'b','n','m',',','.','/', 0,  '*', 0,  ' ', /* rest zeros */
};

/* --- Keyboard ring buffer --- */
#define KB_BUF_SIZE 256
static volatile char kb_buf[KB_BUF_SIZE];
static volatile unsigned kb_head = 0;
static volatile unsigned kb_tail = 0;
static volatile unsigned kb_count = 0;

/* Modifier and extended-prefix state for keyboard handler */
static volatile int kb_ctrl_pressed = 0;
static volatile int kb_alt_pressed  = 0;
static volatile int kb_ext_prefix   = 0;

/* push a character into the buffer (called from ISR) */
static void kb_put(char c) {
    if (kb_count < KB_BUF_SIZE) {
        kb_buf[kb_head] = c;
        kb_head = (kb_head + 1) & (KB_BUF_SIZE - 1);
        kb_count++;
    }
}

/* pop a character from the buffer; caller must ensure buffer not empty */
static char kb_get_nolock(void) {
    char c = kb_buf[kb_tail];
    kb_tail = (kb_tail + 1) & (KB_BUF_SIZE - 1);
    kb_count--;
    return c;
}

/* Blocking getchar for kernel */
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

/* Read a line into buffer (null-terminated). Returns length (excluding null). */
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

/* Reboot helper: attempt keyboard-controller reset (outb 0x64, 0xFE) */
static void system_reboot(void) {
    __asm__ volatile ("cli");
    /* Try keyboard controller reset */
    outb_port(0x64, 0xFE);
    /* If not reset, halt */
    for (;;) { __asm__ volatile ("hlt"); }
}

/* C-level keyboard handler called from assembly stub
   - handles extended prefix 0xE0
   - tracks Ctrl and Alt (left) make/break
   - detects Ctrl+Alt+Del (E0 53) and triggers system_reboot()
*/
void keyboard_handler_c(void) {
    uint8_t sc = inb_port(0x60);

    /* Extended prefix 0xE0 */
    if (sc == 0xE0) {
        kb_ext_prefix = 1;
        pic_send_eoi(1);
        return;
    }

    /* If extended prefix was seen, handle extended scancode */
    if (kb_ext_prefix) {
        kb_ext_prefix = 0;
        /* Extended Delete make code is 0x53 (E0 53). Break would be E0 D3. */
        if (sc == 0x53) {
            /* Make code for Delete (extended) */
            if (kb_ctrl_pressed && kb_alt_pressed) {
                /* Reboot on Ctrl+Alt+Del */
                system_reboot();
                /* If reboot doesn't occur, continue */
            } else {
                /* Optionally enqueue a delete character or ignore */
            }
        } else if (sc == 0xD3) {
            /* Extended Delete break - ignore */
        } else {
            /* Other extended keys can be handled here if desired */
        }
        pic_send_eoi(1);
        return;
    }

    /* Non-extended scancodes */

    /* Left Ctrl: make 0x1D, break 0x9D */
    if (sc == 0x1D) {
        kb_ctrl_pressed = 1;
        pic_send_eoi(1);
        return;
    } else if (sc == 0x9D) {
        kb_ctrl_pressed = 0;
        pic_send_eoi(1);
        return;
    }

    /* Left Alt: make 0x38, break 0xB8 */
    if (sc == 0x38) {
        kb_alt_pressed = 1;
        pic_send_eoi(1);
        return;
    } else if (sc == 0xB8) {
        kb_alt_pressed = 0;
        pic_send_eoi(1);
        return;
    }

    /* Ignore break codes for other keys (scancodes with high bit set) */
    if (sc & 0x80) {
        pic_send_eoi(1);
        return;
    }

    /* Normal make code for printable keys */
    if (sc < 128) {
        char c = scancode_to_ascii[sc];
        if (c) kb_put(c);
    }

    pic_send_eoi(1);
}

/* Assembly stub for IRQ1 (keyboard). Preserve registers and call C handler.
   Align stack for SysV ABI by subtracting 8 before call and restoring after.
*/
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

/* -------------------------
   Corrected exception stubs 0..31
   - For vectors with error code: read error code from (%rsp), pop it (add $8, %rsp),
     align stack for call (sub $8, %rsp), call exception_handler_c, restore stack (add $8).
   - For vectors without error code: set rsi=0, align stack for call, call, restore.
   ------------------------- */

__attribute__((naked)) void isr_vector0(void)  { __asm__ volatile("mov $0,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector1(void)  { __asm__ volatile("mov $1,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector2(void)  { __asm__ volatile("mov $2,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector3(void)  { __asm__ volatile("mov $3,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector4(void)  { __asm__ volatile("mov $4,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector5(void)  { __asm__ volatile("mov $5,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector6(void)  { __asm__ volatile("mov $6,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector7(void)  { __asm__ volatile("mov $7,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }

/* vector 8 has error code */
__attribute__((naked)) void isr_vector8(void)  { __asm__ volatile("mov $8,%rdi\n\tmov (%rsp),%rsi\n\tadd $8,%rsp\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }

__attribute__((naked)) void isr_vector9(void)  { __asm__ volatile("mov $9,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }

__attribute__((naked)) void isr_vector10(void) { __asm__ volatile("mov $10,%rdi\n\tmov (%rsp),%rsi\n\tadd $8,%rsp\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector11(void) { __asm__ volatile("mov $11,%rdi\n\tmov (%rsp),%rsi\n\tadd $8,%rsp\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector12(void) { __asm__ volatile("mov $12,%rdi\n\tmov (%rsp),%rsi\n\tadd $8,%rsp\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector13(void) { __asm__ volatile("mov $13,%rdi\n\tmov (%rsp),%rsi\n\tadd $8,%rsp\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector14(void) { __asm__ volatile("mov $14,%rdi\n\tmov (%rsp),%rsi\n\tadd $8,%rsp\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }

__attribute__((naked)) void isr_vector15(void) { __asm__ volatile("mov $15,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector16(void) { __asm__ volatile("mov $16,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }

/* vector 17 has error code */
__attribute__((naked)) void isr_vector17(void) { __asm__ volatile("mov $17,%rdi\n\tmov (%rsp),%rsi\n\tadd $8,%rsp\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }

__attribute__((naked)) void isr_vector18(void) { __asm__ volatile("mov $18,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector19(void) { __asm__ volatile("mov $19,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector20(void) { __asm__ volatile("mov $20,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }

/* vector 21 has error code */
__attribute__((naked)) void isr_vector21(void) { __asm__ volatile("mov $21,%rdi\n\tmov (%rsp),%rsi\n\tadd $8,%rsp\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }

__attribute__((naked)) void isr_vector22(void) { __asm__ volatile("mov $22,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector23(void) { __asm__ volatile("mov $23,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector24(void) { __asm__ volatile("mov $24,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector25(void) { __asm__ volatile("mov $25,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector26(void) { __asm__ volatile("mov $26,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector27(void) { __asm__ volatile("mov $27,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector28(void) { __asm__ volatile("mov $28,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }

/* vector 29 has error code */
__attribute__((naked)) void isr_vector29(void) { __asm__ volatile("mov $29,%rdi\n\tmov (%rsp),%rsi\n\tadd $8,%rsp\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }

__attribute__((naked)) void isr_vector30(void) { __asm__ volatile("mov $30,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }
__attribute__((naked)) void isr_vector31(void) { __asm__ volatile("mov $31,%rdi\n\txor %rsi,%rsi\n\tsub $8,%rsp\n\tcall exception_handler_c\n\tadd $8,%rsp\n\tiretq\n\t"); }

/* C exception handler: prints vector and error code, then halts safely. */
void exception_handler_c(int vec, uint64_t err) {
    printf("EXCEPTION: vector=%d err=0x%x\n", vec, err);
    if (vec == 14) {
        uint64_t cr2;
        __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
        printf("Page fault address: 0x%x\n", cr2);
    }
    while (1) { __asm__ volatile ("hlt"); }
}

/* Load IDT */
static void load_idt(void) {
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base  = (uint64_t)&idt;
    __asm__ volatile ("lidt %0" :: "m"(idt_ptr) : "memory");
}

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
    set_idt_entry(8,  isr_vector8,  0x8E, 1); /* double-fault uses IST[0] */
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

/* --- Framebuffer virtual mapping helpers --- */
static void map_framebuffer_virtual(uint64_t fb_phys_base, uint64_t fb_size) {
    const uint64_t page_size = 0x200000ULL; /* 2 MiB */
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

        pd[cur_pd_index] = (phys & ~(page_size - 1)) | PAGE_PRESENT | PAGE_RW | PAGE_PS;

        phys += page_size;
        cur_pd_index++;
    }

    uint64_t cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));
    __asm__ volatile ("mov %0, %%cr3" :: "r"(cr3) : "memory");
}

/* --- 7. Paging, GDT & TSS Init --- */
void init_paging(KernelGOPInfo *kgi) {
    const uint64_t flags = PAGE_PRESENT | PAGE_RW;
    const uint64_t fb_flags = PAGE_PRESENT | PAGE_RW | PAGE_PWT;
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

void init_gdt_tss() {
    memset(&gdt_table, 0, sizeof(gdt_table));
    memset(&kernel_tss, 0, sizeof(kernel_tss));

    gdt_table.code.access = 0x9A;
    gdt_table.code.gran   = 0x20; /* L bit */

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

/* draw_char echoes to serial and draws to virtual framebuffer */
void draw_char(char c) {
    if (c == '\r') serial_putc('\n'); else serial_putc(c);

    if (c == '\n') {
        cursor_x = MARGIN_X;
        cursor_y += CHAR_HEIGHT;
        if (cursor_y + CHAR_HEIGHT >= screen_height) scroll();
        return;
    }
    if (cursor_x + CHAR_WIDTH >= screen_width) {
        cursor_x = MARGIN_X;
        cursor_y += CHAR_HEIGHT;
        if (cursor_y + CHAR_HEIGHT >= screen_height) scroll();
    }
    if (c < 32 || c > 126) return;
    const uint8_t* g = font8x8_basic[c - 32];
    for (int r = 0; r < 8; r++) {
        for (int col = 0; col < 8; col++) {
            if ((g[r] >> (7 - col)) & 1) {
                uint32_t base = (cursor_y + r*2) * pps + (cursor_x + col*2);
                fbb[base] = 0xFFFFFF; fbb[base + 1] = 0xFFFFFF;
                fbb[base + pps] = 0xFFFFFF; fbb[base + pps + 1] = 0xFFFFFF;
            }
        }
    }
    cursor_x += CHAR_WIDTH;
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
            }
        } else draw_char(*fmt);
        fmt++;
    }
    va_end(args);
}

/* --- 8. Main --- */
void kmain(KernelGOPInfo *kgi) {
    pps = kgi->PixelsPerScanLine;
    screen_width = kgi->Width;
    screen_height = kgi->Height;

    init_paging(kgi);
    init_gdt_tss();

    serial_init();

    init_pic_idt_keyboard();

    __asm__ volatile ("sti");

    uint64_t phys_fb = kgi->FrameBufferBase;
    uint64_t offset = phys_fb & (FB_MAP_SIZE - 1);
    fbb = (volatile uint32_t*)(uintptr_t)(VFB_BASE + offset);

    for(uint32_t i = 0; i < screen_height * pps; i++) fbb[i] = 0x222222;

    printf("Kernel: GDT/TSS and Paging (Write-Through) Loaded.\n");
    for(int i = 0; i < 200; i++) {
        printf("Loop %d\n", i);
    }

    while (1) {
        int ch = kgetc();
        if (ch >= 0) {
            if (ch == '\r') ch = '\n';
            draw_char((char)ch);
        }
    }

    while (1) { __asm__("hlt"); }
}

__attribute__((naked)) void _start(void) {
    __asm__ volatile ("cli\n\tmov %rcx, %rdi\n\tsub $8, %rsp\n\tcall kmain\n\thlt");
}

