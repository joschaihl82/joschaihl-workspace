/*
 * kernel.c
 * Integrierte Version mit:
 * - GDT + TSS (IST1 für Double Fault)
 * - IDT mit IST-Einträgen
 * - Minimal-Exception-Handler (PF, DF, GP)
 * - Keyboard ISR mit IST
 * - Sichere Reihenfolge: GDT/TSS -> Paging -> IDT/PIC -> STI
 *
 * Passe KERNEL_VIRT_BASE an dein Linker-Layout an.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

/* --- Konfiguration: anpassen falls Kernel identity-linked ist --- */
#ifndef KERNEL_VIRT_BASE
#define KERNEL_VIRT_BASE 0xFFFF800000000000ULL
#endif

/* --- Font 8x8 Basic (95 Zeichen) --- */
/* (font8x8_basic wie in deinem Originalcode; aus Platzgründen hier nicht wiederholt) */
static const uint8_t font8x8_basic[95][8] = {
    /* ... (kopiere hier dein vorhandenes font8x8_basic-Array) ... */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00} /* Platzhalter - ersetze durch Original */
};

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
#define PAGE_PWT     (1ULL << 3)
#define PAGE_PCD     (1ULL << 4)
#define PAGE_PS      (1ULL << 7)

/* Page Tables (4KB Aligned) */
uint64_t pml4[512] __attribute__((aligned(4096)));
uint64_t pdpt[512] __attribute__((aligned(4096)));

/* Globale Grafik-Variablen */
uint32_t pps;
uint32_t screen_width;
uint32_t screen_height;
volatile uint32_t *fbb; /* virtuelle Adresse im High-Half, erst setzen nach Mapping */

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

/* Virtuell -> physisch Umrechnung (nur gültig wenn Kernel im High-Half linked ist
   oder identity-linked: KERNEL_VIRT_BASE = 0) */
static inline uintptr_t virt_to_phys(void *v) {
    uintptr_t va = (uintptr_t)v;
    if (KERNEL_VIRT_BASE != 0 && va >= KERNEL_VIRT_BASE) return va - KERNEL_VIRT_BASE;
    return va;
}

/* --- I/O Ports --- */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* --- IDT / PIC Definitions --- */
struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct idt_entry idt[256] __attribute__((aligned(16)));
static struct idt_ptr idt_descriptor;

/* --- GDT / TSS --- */
struct tss {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iomap_base;
} __attribute__((packed));

static struct tss kernel_tss;

/* 1 page stack for IST1 (Double Fault safe stack) */
static uint8_t df_stack[4096] __attribute__((aligned(16)));

/* GDT: Null, Code, Data, TSS (2 entries) */
uint64_t gdt[6] __attribute__((aligned(16)));
struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) gdt_ptr;

/* --- Forward declarations --- */
void prepare_and_setup_paging(KernelGOPInfo *kgi);
static inline uintptr_t fb_highvirt(uint64_t phys_base);
void fb_scroll_fast(void);
void draw_char_fast(char c);
void k_printf(const char* fmt, ...);

/* --- ISR/Keyboard forward --- */
void isr_keyboard(void);
__attribute__((naked)) void isr_keyboard_stub(void);

/* --- Keyboard ISR state --- */
static int ctrl_pressed = 0;
static int alt_pressed = 0;

/* --- Exception handlers (C) --- */
void exc_generic_halt(const char *msg, uint64_t vec, uint64_t err) {
    __asm__ volatile("cli");
    k_printf("%s vec=%x err=%x\n", msg, vec, err);
    while (1) __asm__ volatile("hlt");
}

/* Page Fault handler: print CR2 if possible */
void exc_pf_handler(uint64_t vec, uint64_t err) {
    uint64_t cr2;
    __asm__ volatile("mov %%cr2, %0" : "=r"(cr2));
    __asm__ volatile("cli");
    k_printf("Page Fault vec=%x err=%x CR2=%x\n", vec, err, cr2);
    while (1) __asm__ volatile("hlt");
}

void exc_df_handler(uint64_t vec, uint64_t err) {
    __asm__ volatile("cli");
    k_printf("Double Fault vec=%x err=%x\n", vec, err);
    while (1) __asm__ volatile("hlt");
}

void exc_gp_handler(uint64_t vec, uint64_t err) {
    __asm__ volatile("cli");
    k_printf("General Protection Fault vec=%x err=%x\n", vec, err);
    while (1) __asm__ volatile("hlt");
}

/* --- IDT helper with IST support --- */
static void set_idt_entry_ist(int vec, void (*handler)(), uint8_t ist, uint8_t flags) {
    uint64_t addr = (uint64_t)handler;
    idt[vec].offset_low  = (uint16_t)(addr & 0xFFFF);
    idt[vec].selector    = 0x08; /* kernel code segment */
    idt[vec].ist         = ist & 0x7;
    idt[vec].type_attr   = flags;
    idt[vec].offset_mid  = (uint16_t)((addr >> 16) & 0xFFFF);
    idt[vec].offset_high = (uint32_t)((addr >> 32) & 0xFFFFFFFF);
    idt[vec].zero        = 0;
}

/* --- PIC remap --- */
static void pic_remap(void) {
    /* ICW1: start init */
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    /* ICW2: vector offsets */
    outb(0x21, 0x20); /* Master offset 0x20 */
    outb(0xA1, 0x28); /* Slave offset 0x28 */
    /* ICW3: wiring */
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    /* ICW4: 8086 mode */
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    /* Mask interrupts (unmask keyboard IRQ1 only for now) */
    outb(0x21, 0xFD); /* 11111101 -> unmask IRQ1 */
    outb(0xA1, 0xFF); /* mask all on slave */
}

/* --- Load IDT --- */
static void load_idt(void) {
    idt_descriptor.limit = sizeof(idt) - 1;
    idt_descriptor.base  = (uint64_t)&idt;
    __asm__ volatile("lidt %0" : : "m"(idt_descriptor));
}

/* --- Initialize IDT and PIC, set keyboard ISR --- */
void init_idt_and_pic(void) {
    /* Clear IDT */
    for (int i = 0; i < 256; i++) {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].ist = 0;
        idt[i].type_attr = 0;
        idt[i].offset_mid = 0;
        idt[i].offset_high = 0;
        idt[i].zero = 0;
    }

    /* Type attr: present, DPL=0, interrupt gate (0x8E) */
    uint8_t flags = 0x8E;

    /* Register minimal exception handlers with IST usage for DF */
    set_idt_entry_ist(8,  (void(*)())exc_df_handler, 1, flags);  /* Double Fault -> IST1 */
    set_idt_entry_ist(13, (void(*)())exc_gp_handler,  0, flags);  /* GP */
    set_idt_entry_ist(14, (void(*)())exc_pf_handler,   0, flags);  /* Page Fault */

    /* Set keyboard IRQ1 vector (PIC remapped to 0x20..0x2F -> IRQ1 = 0x21) */
    set_idt_entry_ist(0x21, isr_keyboard_stub, 1, flags);

    /* Remap PIC and load IDT */
    pic_remap();
    load_idt();
}

/* --- GDT + TSS initialization --- */
static void init_gdt_tss(void) {
    /* Null descriptor */
    gdt[0] = 0;

    /* Kernel Code Segment (64-bit) */
    /* Base/Limit ignored for 64-bit code; descriptor flags set for long mode */
    gdt[1] = 0x00AF9A000000FFFFULL;

    /* Kernel Data Segment */
    gdt[2] = 0x00AF92000000FFFFULL;

    /* TSS descriptor (two entries) */
    uint64_t tss_base = (uint64_t)&kernel_tss;
    uint64_t tss_limit = sizeof(kernel_tss) - 1;

    gdt[3] = (tss_limit & 0xFFFF)
           | ((tss_base & 0xFFFFFF) << 16)
           | (0x89ULL << 40) /* present, type=9 (available 64-bit TSS) */
           | ((tss_limit & 0xF0000ULL) << 32)
           | ((tss_base & 0xFF000000ULL) << 32);

    gdt[4] = (tss_base >> 32) & 0xFFFFFFFFULL;

    /* Zero remaining entry */
    gdt[5] = 0;

    /* Fill TSS */
    memset(&kernel_tss, 0, sizeof(kernel_tss));
    kernel_tss.ist1 = (uint64_t)(df_stack + sizeof(df_stack));
    kernel_tss.iomap_base = sizeof(kernel_tss);

    /* Load GDT */
    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.base  = (uint64_t)&gdt;
    __asm__ volatile("lgdt %0" : : "m"(gdt_ptr));

    /* Reload data segments (DS/ES/SS) and far jump to reload CS */
    __asm__ volatile(
        "mov $0x10, %%ax\n\t"   /* data selector (index 2 -> 0x10) */
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%ss\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        /* far return to reload CS */
        "pushq $0x08\n\t"       /* code selector (index 1 -> 0x08) */
        "lea 1f(%%rip), %%rax\n\t"
        "pushq %%rax\n\t"
        "lretq\n\t"
        "1:\n\t"
        : : : "rax", "memory"
    );

    /* Load TSS (selector 0x18 -> index 3) */
    __asm__ volatile("ltr %0" : : "r"((uint16_t)0x18));
}

/* --- Keyboard handler (C) --- */
void isr_keyboard(void) {
    uint8_t sc = inb(0x60);

    /* Scancode handling (set/reset ctrl/alt). Using set1 scancodes (make/break) */
    if (sc == 0x1D) ctrl_pressed = 1;
    else if (sc == 0x9D) ctrl_pressed = 0;
    else if (sc == 0x38) alt_pressed = 1;
    else if (sc == 0xB8) alt_pressed = 0;
    else {
        /* Check for Delete make (0x53) */
        if (sc == 0x53) {
            if (ctrl_pressed && alt_pressed) {
                /* Trigger reboot via keyboard controller (8042) */
                outb(0x64, 0xFE);
            }
        }
    }

    /* Send EOI to PIC (master) */
    outb(0x20, 0x20);
}

/* --- ISR stub (saves registers, calls C handler, iretq) --- */
__attribute__((naked)) void isr_keyboard_stub(void) {
    __asm__ volatile(
        /* Use IST1 stack automatically if IDT entry set ist=1 */
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
        "call isr_keyboard\n\t"
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

/* --- Grafikausgabe & Scrolling (optimiert) --- */
void fb_scroll_fast() {
    if (!fbb) return;

    uint32_t row_bytes = pps * sizeof(uint32_t);
    uint32_t scroll_rows = line_height;
    if (scroll_rows >= screen_height) {
        uint64_t total = (uint64_t)screen_height * pps;
        for (uint64_t i = 0; i < total; i++) ((uint32_t*)fbb)[i] = 0;
        cursor_y = 0;
        return;
    }

    uint32_t copy_rows = screen_height - scroll_rows;
    uint64_t qwords_per_row = (row_bytes / 8);
    if (qwords_per_row == 0) return;

    uint64_t *dst = (uint64_t*)fbb;
    uint64_t *src = (uint64_t*)( (uintptr_t)fbb + (scroll_rows * row_bytes) );

    __asm__ volatile("cli");
    for (uint32_t y = 0; y < copy_rows; y++) {
        uint64_t *d = dst + (y * qwords_per_row);
        uint64_t *s = src + (y * qwords_per_row);
        for (uint64_t i = 0; i < qwords_per_row; i++) {
            d[i] = s[i];
        }
    }
    uint64_t *clear_start = dst + (copy_rows * qwords_per_row);
    uint64_t clear_qwords = (uint64_t)scroll_rows * qwords_per_row;
    for (uint64_t i = 0; i < clear_qwords; i++) clear_start[i] = 0;
    __asm__ volatile("sti");

    if (cursor_y >= scroll_rows) cursor_y -= scroll_rows;
    else cursor_y = 0;
}

void process_newline() {
    cursor_x = 20;
    cursor_y += line_height;
    while (cursor_y + line_height > screen_height) {
        fb_scroll_fast();
    }
}

void draw_char_fast(char c) {
    if (!fbb) return;
    if (c == '\n') { process_newline(); return; }
    if (c < 32 || c > 126) return;
    if (cursor_x + 16 >= screen_width) process_newline();

    const uint8_t* g = font8x8_basic[c - 32];

    for (int r = 0; r < 8; r++) {
        uint32_t y = cursor_y + r*2;
        if (y + 1 >= screen_height) continue;
        for (int col = 0; col < 8; col++) {
            if ((g[r] >> (7 - col)) & 1) {
                uint32_t x = cursor_x + col*2;
                if (x + 1 >= screen_width) continue;
                uint32_t idx0 = (y) * pps + x;
                uint32_t idx1 = (y+1) * pps + x;
                uint32_t color = active_color;
                ((uint32_t*)fbb)[idx0]   = color;
                ((uint32_t*)fbb)[idx0+1] = color;
                ((uint32_t*)fbb)[idx1]   = color;
                ((uint32_t*)fbb)[idx1+1] = color;
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
                if (n == 0) draw_char_fast('0');
                else {
                    char buf[32]; int i = 0;
                    while(n > 0 && i < (int)sizeof(buf)-1) {
                        uint8_t nibble = n & 0xF;
                        buf[i++] = (nibble < 10) ? nibble + '0' : nibble - 10 + 'A';
                        n >>= 4;
                    }
                    while(i > 0) draw_char_fast(buf[--i]);
                }
            } else if (*fmt == 's') {
                const char* s = va_arg(args, const char*);
                while(*s) draw_char_fast(*s++);
            } else {
                draw_char_fast('%');
                draw_char_fast(*fmt);
            }
        } else {
            draw_char_fast(*fmt);
        }
        fmt++;
    }
    va_end(args);
}

/* --- Paging: PAT setzen, WBINVD, Page-Tables anlegen, CR3 wechseln --- */
void prepare_and_setup_paging(KernelGOPInfo *kgi) {
    uint32_t eax, edx;
    uint64_t pat;

    /* 1) Interrupts sperren */
    __asm__ volatile("cli");

    /* 2) Caches vor PAT-Änderung leeren */
    __asm__ volatile("wbinvd");

    /* 3) PAT MSR lesen, ändern, schreiben (MSR 0x277) */
    __asm__ volatile("rdmsr" : "=a"(eax), "=d"(edx) : "c"(0x277));
    pat = ((uint64_t)edx << 32) | eax;
    pat &= ~(0xFFULL << 16);
    pat |=  (0x01ULL << 16); /* PAT[2] = WC */
    eax = (uint32_t)pat; edx = (uint32_t)(pat >> 32);
    __asm__ volatile("wrmsr" : : "a"(eax), "d"(edx), "c"(0x277));

    /* 4) Caches nach PAT-Änderung erneut leeren */
    __asm__ volatile("wbinvd");

    /* 5) Page-Tables anlegen (statisch, aligned) */
    memset(pml4, 0, sizeof(pml4));
    memset(pdpt, 0, sizeof(pdpt));

    /* 1:1 mapping für die ersten 512 GB (1GB Huge Pages) */
    for (uint64_t i = 0; i < 512; i++) {
        pdpt[i] = (i << 30) | PAGE_PRESENT | PAGE_RW | PAGE_PS;
    }

    /* Framebuffer phys -> PDPT Index */
    uint64_t fb_phys = kgi->FrameBufferBase;
    uint64_t fb_pdpt_idx = fb_phys >> 30;
    if (fb_pdpt_idx < 512) {
        pdpt[fb_pdpt_idx] |= PAGE_PCD; /* in Kombination mit PAT[2] -> WC */
        pdpt[fb_pdpt_idx] &= ~PAGE_PWT;
    }

    /* 6) phys addresses for page tables */
    uint64_t pdpt_phys = (uint64_t)virt_to_phys((void*)pdpt) & ~0xFFFULL;
    uint64_t pml4_phys = (uint64_t)virt_to_phys((void*)pml4) & ~0xFFFULL;

    /* 7) PML4: Identity (Index 0) und High-Half (Index 511) auf dieselbe PDPT-phys Adresse */
    pml4[0]   = pdpt_phys | PAGE_PRESENT | PAGE_RW;
    pml4[511] = pdpt_phys | PAGE_PRESENT | PAGE_RW;

    /* 8) CR3 laden (physische Adresse der PML4) und TLB flush */
    __asm__ volatile("mov %0, %%cr3" : : "r"(pml4_phys) : "memory");
    __asm__ volatile("mov %0, %%cr3" : : "r"(pml4_phys) : "memory");

    /* 9) Optional: nochmal WBINVD falls Hardware es verlangt */
    __asm__ volatile("wbinvd");

    /* 10) Interrupts wieder freigeben (wird in kmain final gesetzt) */
    /* leave interrupts disabled here; kmain will enable after IDT/PIC are ready */
}

/* --- Hilfs: virtuelle Framebuffer-Adresse im High-Half berechnen --- */
static inline uintptr_t fb_highvirt(uint64_t phys_base) {
    const uintptr_t HIGH_BASE = KERNEL_VIRT_BASE; /* z.B. 0xFFFF800000000000 */
    return HIGH_BASE + (uintptr_t)phys_base;
}

/* --- Main --- */
void kmain(KernelGOPInfo *kgi) {
    /* 0) GDT + TSS zuerst (stellt IST/DF-Stack sicher) */
    init_gdt_tss();

    /* 1) Paging vorbereiten und aktivieren (PAT, WBINVD, PML4/PDPT, CR3) */
    prepare_and_setup_paging(kgi);

    /* 2) IDT und PIC initialisieren (Keyboard-ISR) */
    init_idt_and_pic();

    /* 3) Framebuffer virtuelle Adresse im High-Half berechnen und setzen */
    uintptr_t fbv = fb_highvirt(kgi->FrameBufferBase);
    fbb = (volatile uint32_t*)(uintptr_t)fbv;

    /* 4) Bildschirmparameter setzen */
    pps = kgi->PixelsPerScanLine;
    screen_width = kgi->Width;
    screen_height = kgi->Height;

    /* 5) Initial leeren (schnell) */
    uint64_t total_pixels = (uint64_t)screen_height * pps;
    for (uint64_t i = 0; i < total_pixels; i++) ((uint32_t*)fbb)[i] = 0;

    active_color = 0x00FF00;

    /* 6) Interrupts freigeben (erst jetzt) */
    __asm__ volatile("sti");

    /* 7) Jetzt ist k_printf sicher */
    k_printf("Kernel Online. Write-Combining aktiv. IRQs enabled.\n");

    /* 8) Stresstest für Scrolling (Beispiel) */
    for(uint64_t i = 0; i < 1000000; i++) {
        active_color = (i % 2 == 0) ? 0xFFFF00 : 0xFFFFFF;
        k_printf("Zeile %x: High-Speed Scrolling!\n", i);
    }

    while (1) { __asm__ volatile("hlt"); }
}

/* --- Start-Stub --- */
__attribute__((naked)) void _start() {
    __asm__ volatile (
        "mov %rcx, %rdi\n\t" /* UEFI: RCX -> pointer to KernelGOPInfo */
        "sub $8, %rsp\n\t"
        "call kmain\n\t"
        "hlt"
    );
}

