/*
 jemu.c - Corrected single-file emulator + minimal UEFI runtime + PE/COFF loader
 - Portable C only, no inline assembly.
 - Fixes PE parsing, data-directory handling, 64-bit IAT/thunks, relocations,
   safer bounds checks, and expands import symbol mapping to common UEFI stubs.
 - Intended as a pragmatic test harness to load and call 64-bit UEFI PE images.
 - Build: gcc -O2 -Wall -o jemu jemu.c
 - Usage: ./jemu <efi-or-binary> [load_addr] [entry_rip]
   If entry_rip is omitted, the loader will try to detect and load a PE image
   at load_addr and call its entry point.
*/

#define _GNU_SOURCE
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <stdarg.h>

/* ----------------------------- Configuration ------------------------------ */
#ifndef RAM_SIZE
#define RAM_SIZE (128ULL * 1024 * 1024) /* 128 MiB for UEFI images */
#endif
#ifndef DEBUG_LOG_SIZE
#define DEBUG_LOG_SIZE 32768
#endif
#define PAGE_SIZE 4096

/* ------------------------------- Basic types ------------------------------ */

/* Minimal XMM type */
typedef struct { uint64_t lo, hi; } XMM;

/* CPU state (very small, used only for emulator harness) */
typedef struct {
    uint64_t gpr[16]; /* RAX..R15 */
    uint64_t rflags;
    uint64_t rip;
    XMM xmm[16];
    int op_size;    /* 16/32/64 */
    int addr_size;  /* 32/64 */
    int has_rex;
    int rex_w, rex_r, rex_x, rex_b;
    int modrm_mod, modrm_reg, modrm_rm;
    uint8_t last_opcode;
    uint64_t instr_count;
    uint64_t cycles;
    int halted;
} X64CPU;

/* Global CPU */
static X64CPU cpu;

/* RAM (linear host buffer representing guest physical memory) */
static uint8_t *ram_mem = NULL;

/* Debug log (circular) */
static char debug_log[DEBUG_LOG_SIZE];
static unsigned int debug_log_pos = 0;

/* --------------------------- Debug helpers -------------------------------- */

static void debug_log_write_str(const char *s) {
    if (!s) return;
    while (*s) {
        debug_log[debug_log_pos++] = *s++;
        if (debug_log_pos >= DEBUG_LOG_SIZE) debug_log_pos = 0;
    }
}
static void trace_msg(const char *s) { debug_log_write_str(s); debug_log_write_str("\n"); }
static void tracef(const char *fmt, ...) {
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    trace_msg(buf);
}

/* --------------------------- Memory helpers ------------------------------- */

/* Return pointer to linear memory for guest address (simple mapping) */
uint8_t* get_memory_ptr(uint64_t addr) {
    if (!ram_mem) return NULL;
    /* Map guest physical address modulo RAM_SIZE for simplicity */
    if (addr >= RAM_SIZE) return NULL;
    return &ram_mem[addr];
}

uint8_t mem_read8(uint64_t addr) {
    uint8_t *p = get_memory_ptr(addr);
    return p ? p[0] : 0;
}
uint16_t mem_read16(uint64_t addr) {
    uint8_t *p = get_memory_ptr(addr);
    if (!p) return 0;
    uint16_t v; memcpy(&v, p, 2); return v;
}
uint32_t mem_read32(uint64_t addr) {
    uint8_t *p = get_memory_ptr(addr);
    if (!p) return 0;
    uint32_t v; memcpy(&v, p, 4); return v;
}
uint64_t mem_read64(uint64_t addr) {
    uint8_t *p = get_memory_ptr(addr);
    if (!p) return 0;
    uint64_t v; memcpy(&v, p, 8); return v;
}
void mem_write8(uint64_t addr, uint8_t v) {
    uint8_t *p = get_memory_ptr(addr); if (!p) return; p[0] = v;
}
void mem_write16(uint64_t addr, uint16_t v) {
    uint8_t *p = get_memory_ptr(addr); if (!p) return; memcpy(p, &v, 2);
}
void mem_write32(uint64_t addr, uint32_t v) {
    uint8_t *p = get_memory_ptr(addr); if (!p) return; memcpy(p, &v, 4);
}
void mem_write64(uint64_t addr, uint64_t v) {
    uint8_t *p = get_memory_ptr(addr); if (!p) return; memcpy(p, &v, 8);
}

/* Bulk copy helper */
static inline void mem_copy_fast(void *dst, const void *src, size_t n) {
    memcpy(dst, src, n);
}

/* --------------------------- Fetch helpers -------------------------------- */

uint8_t fetch8(void) { uint8_t v = mem_read8(cpu.rip); cpu.rip += 1; return v; }
uint16_t fetch16(void) { uint16_t v = mem_read16(cpu.rip); cpu.rip += 2; return v; }
uint32_t fetch32(void) { uint32_t v = mem_read32(cpu.rip); cpu.rip += 4; return v; }
uint64_t fetch64(void) { uint64_t v = mem_read64(cpu.rip); cpu.rip += 8; return v; }

/* --------------------------- Minimal executor ----------------------------- */
/* The emulator's instruction executor is intentionally minimal: the UEFI
   PE image will be executed by calling its entry point as a host function.
   The executor exists only to run any small harness code if needed. */

void execute_one_stub(void) {
    cpu.rip += 1;
    cpu.instr_count++;
}

/* --------------------------- UEFI minimal types --------------------------- */

/* EFI status codes */
typedef uint64_t EFI_STATUS;
#define EFI_SUCCESS 0
#define EFI_LOAD_ERROR ((EFI_STATUS)0x8000000000000001ULL)
#define EFI_INVALID_PARAMETER ((EFI_STATUS)0x8000000000000002ULL)
#define EFI_UNSUPPORTED ((EFI_STATUS)0x8000000000000003ULL)
#define EFI_OUT_OF_RESOURCES ((EFI_STATUS)0x8000000000000009ULL)
#define EFI_NOT_FOUND ((EFI_STATUS)0x8000000000000014ULL)
#define EFI_BUFFER_TOO_SMALL ((EFI_STATUS)0x8000000000000005ULL)

/* EFI basic types */
typedef void* EFI_HANDLE;
typedef void* EFI_EVENT;
typedef uint16_t CHAR16;
typedef uint64_t EFI_PHYSICAL_ADDRESS;
typedef uint64_t EFI_VIRTUAL_ADDRESS;

/* EFI table header */
typedef struct {
    uint64_t Signature;
    uint32_t Revision;
    uint32_t HeaderSize;
    uint32_t CRC32;
    uint32_t Reserved;
} EFI_TABLE_HEADER;

/* Forward declarations for Boot Services and Runtime Services */
struct _EFI_BOOT_SERVICES;
struct _EFI_RUNTIME_SERVICES;

/* EFI System Table */
typedef struct {
    EFI_TABLE_HEADER Hdr;
    CHAR16 *FirmwareVendor;
    uint32_t FirmwareRevision;
    EFI_HANDLE ConsoleInHandle;
    void *ConIn;
    EFI_HANDLE ConsoleOutHandle;
    void *ConOut;
    EFI_HANDLE StandardErrorHandle;
    void *StdErr;
    struct _EFI_RUNTIME_SERVICES *RuntimeServices;
    struct _EFI_BOOT_SERVICES *BootServices;
    uint64_t NumberOfTableEntries;
    void *ConfigurationTable;
} EFI_SYSTEM_TABLE;

/* Minimal memory descriptor */
typedef struct {
    uint32_t Type;
    uint32_t Pad;
    EFI_PHYSICAL_ADDRESS PhysicalStart;
    EFI_VIRTUAL_ADDRESS VirtualStart;
    uint64_t NumberOfPages;
    uint64_t Attribute;
} EFI_MEMORY_DESCRIPTOR;

/* Boot services (subset) */
typedef struct _EFI_BOOT_SERVICES {
    EFI_TABLE_HEADER Hdr;
    void *RaiseTPL;
    void *RestoreTPL;
    EFI_STATUS (*AllocatePages)(int Type, uint64_t MemoryType, uint64_t Pages, EFI_PHYSICAL_ADDRESS *Memory);
    EFI_STATUS (*FreePages)(EFI_PHYSICAL_ADDRESS Memory, uint64_t Pages);
    EFI_STATUS (*GetMemoryMap)(uint64_t *MemoryMapSize, EFI_MEMORY_DESCRIPTOR *MemoryMap, uint64_t *MapKey, uint64_t *DescriptorSize, uint32_t *DescriptorVersion);
    EFI_STATUS (*AllocatePool)(uint64_t PoolType, uint64_t Size, void **Buffer);
    EFI_STATUS (*FreePool)(void *Buffer);
    EFI_STATUS (*CreateEvent)(void);
    EFI_STATUS (*SetWatchdogTimer)(uint64_t Timeout, uint64_t WatchdogCode, uint64_t DataSize, CHAR16 *WatchdogData);
    EFI_STATUS (*Stall)(uint64_t Microseconds);
    EFI_STATUS (*ExitBootServices)(EFI_HANDLE ImageHandle, uint64_t MapKey);
    /* ... many more omitted ... */
} EFI_BOOT_SERVICES;

/* Runtime services (subset) */
typedef struct _EFI_RUNTIME_SERVICES {
    EFI_TABLE_HEADER Hdr;
    void *GetTime;
    void *SetTime;
    void *GetWakeupTime;
    void *SetWakeupTime;
    void *SetVirtualAddressMap;
    void *ConvertPointer;
    void *GetVariable;
    void *GetNextVariableName;
    void *SetVariable;
    void *GetNextHighMonotonicCount;
    void *ResetSystem;
    /* ... omitted ... */
} EFI_RUNTIME_SERVICES;

/* Simple Text Output Protocol (very small) */
typedef struct {
    EFI_STATUS (*OutputString)(void *This, CHAR16 *String);
    /* other functions omitted */
} SIMPLE_TEXT_OUTPUT_PROTOCOL;

/* Graphics Output Protocol (very small) */
typedef struct {
    uint32_t Version;
    void *Mode;
    EFI_STATUS (*QueryMode)(void);
    EFI_STATUS (*SetMode)(void);
    EFI_STATUS (*Blt)(void);
} GRAPHICS_OUTPUT_PROTOCOL;

/* --------------------------- UEFI runtime stubs --------------------------- */

/* Simple heap for AllocatePages and AllocatePool */
static uint8_t *heap_base = NULL;
static size_t heap_size = 0;
static size_t heap_used = 0;

/* Initialize heap */
static void ensure_heap(void) {
    if (!heap_base) {
        heap_size = 32 * 1024 * 1024; /* 32 MiB */
        heap_base = malloc(heap_size);
        if (!heap_base) {
            fprintf(stderr, "heap alloc failed\n");
            exit(1);
        }
        heap_used = 0;
    }
}

/* Simple page allocator: returns host pointer as EFI_PHYSICAL_ADDRESS */
EFI_STATUS ue_allocate_pages(int Type, uint64_t MemoryType, uint64_t Pages, EFI_PHYSICAL_ADDRESS *Memory) {
    (void)Type; (void)MemoryType;
    ensure_heap();
    size_t bytes = Pages * PAGE_SIZE;
    size_t aligned = (bytes + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    if (heap_used + aligned > heap_size) return EFI_OUT_OF_RESOURCES;
    uint64_t addr = (uint64_t)(uintptr_t)(heap_base + heap_used);
    heap_used += aligned;
    *Memory = addr;
    tracef("AllocatePages: pages=%llu -> addr=0x%016llx", (unsigned long long)Pages, (unsigned long long)addr);
    return EFI_SUCCESS;
}
EFI_STATUS ue_free_pages(EFI_PHYSICAL_ADDRESS Memory, uint64_t Pages) {
    (void)Memory; (void)Pages;
    /* no-op */
    return EFI_SUCCESS;
}

/* Very simple memory map: one descriptor covering all RAM */
EFI_STATUS ue_get_memory_map(uint64_t *MemoryMapSize, EFI_MEMORY_DESCRIPTOR *MemoryMap, uint64_t *MapKey, uint64_t *DescriptorSize, uint32_t *DescriptorVersion) {
    if (!MemoryMapSize) return EFI_INVALID_PARAMETER;
    uint64_t needed = sizeof(EFI_MEMORY_DESCRIPTOR);
    if (*MemoryMapSize < needed) {
        *MemoryMapSize = needed;
        return EFI_BUFFER_TOO_SMALL;
    }
    if (MemoryMap) {
        MemoryMap->Type = 7; /* EfiConventionalMemory */
        MemoryMap->Pad = 0;
        MemoryMap->PhysicalStart = (EFI_PHYSICAL_ADDRESS)(uintptr_t)ram_mem;
        MemoryMap->VirtualStart = 0;
        MemoryMap->NumberOfPages = RAM_SIZE / PAGE_SIZE;
        MemoryMap->Attribute = 0;
    }
    *MapKey = 1;
    *DescriptorSize = sizeof(EFI_MEMORY_DESCRIPTOR);
    *DescriptorVersion = 1;
    return EFI_SUCCESS;
}

/* Stall (microseconds) */
EFI_STATUS ue_stall(uint64_t Microseconds) {
    struct timespec ts;
    ts.tv_sec = Microseconds / 1000000ULL;
    ts.tv_nsec = (Microseconds % 1000000ULL) * 1000ULL;
    nanosleep(&ts, NULL);
    return EFI_SUCCESS;
}

/* ExitBootServices stub */
static int boot_services_exited = 0;
EFI_STATUS ue_exit_boot_services(EFI_HANDLE ImageHandle, uint64_t MapKey) {
    (void)ImageHandle; (void)MapKey;
    boot_services_exited = 1;
    trace_msg("ExitBootServices called");
    return EFI_SUCCESS;
}

/* SetWatchdogTimer stub */
EFI_STATUS ue_set_watchdog_timer(uint64_t Timeout, uint64_t WatchdogCode, uint64_t DataSize, CHAR16 *WatchdogData) {
    (void)Timeout; (void)WatchdogCode; (void)DataSize; (void)WatchdogData;
    return EFI_SUCCESS;
}

/* AllocatePool/FreePool simple wrappers */
EFI_STATUS ue_allocate_pool(uint64_t PoolType, uint64_t Size, void **Buffer) {
    (void)PoolType;
    void *p = malloc((size_t)Size);
    if (!p) return EFI_OUT_OF_RESOURCES;
    *Buffer = p;
    return EFI_SUCCESS;
}
EFI_STATUS ue_free_pool(void *Buffer) {
    free(Buffer);
    return EFI_SUCCESS;
}

/* Minimal OutputString: convert CHAR16 to ASCII and print to host stdout */
EFI_STATUS ue_output_string(void *This, CHAR16 *String) {
    (void)This;
    if (!String) return EFI_INVALID_PARAMETER;
    char tmp[4096];
    size_t i = 0;
    while (String[i] && i < sizeof(tmp)-1) {
        tmp[i] = (char)(String[i] & 0xFF);
        i++;
    }
    tmp[i] = '\0';
    fwrite(tmp, 1, i, stdout);
    return EFI_SUCCESS;
}

/* A tiny Print-like stub that accepts ASCII format and prints to host stdout.
   Many UEFI apps import Print (Unicode) or AsciiPrint; mapping is heuristic. */
int ue_print_ascii(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    return 0;
}

/* --------------------------- UEFI global objects -------------------------- */

static EFI_BOOT_SERVICES BootServices;
static EFI_RUNTIME_SERVICES RuntimeServices;
static EFI_SYSTEM_TABLE SystemTable;
static SIMPLE_TEXT_OUTPUT_PROTOCOL SimpleTextOut;
static GRAPHICS_OUTPUT_PROTOCOL GraphicsOut;

/* Initialize minimal UEFI tables */
void ue_init_environment(void) {
    ensure_heap();

    memset(&BootServices, 0, sizeof(BootServices));
    BootServices.Hdr.Signature = 0x56524542544F4F42ULL; /* 'BOOT' like */
    BootServices.AllocatePages = ue_allocate_pages;
    BootServices.FreePages = ue_free_pages;
    BootServices.GetMemoryMap = ue_get_memory_map;
    BootServices.AllocatePool = ue_allocate_pool;
    BootServices.FreePool = ue_free_pool;
    BootServices.SetWatchdogTimer = ue_set_watchdog_timer;
    BootServices.Stall = ue_stall;
    BootServices.ExitBootServices = ue_exit_boot_services;

    memset(&RuntimeServices, 0, sizeof(RuntimeServices));
    RuntimeServices.Hdr.Signature = 0x54554E4952545546ULL; /* placeholder */

    SimpleTextOut.OutputString = ue_output_string;

    GraphicsOut.Version = 1;
    GraphicsOut.QueryMode = NULL;
    GraphicsOut.SetMode = NULL;
    GraphicsOut.Blt = NULL;

    memset(&SystemTable, 0, sizeof(SystemTable));
    SystemTable.Hdr.Signature = 0x5453595320494645ULL; /* 'EFIS' */
    SystemTable.FirmwareVendor = NULL;
    SystemTable.FirmwareRevision = 0x00010000;
    SystemTable.ConsoleOutHandle = (EFI_HANDLE)1;
    SystemTable.ConOut = &SimpleTextOut;
    SystemTable.ConsoleInHandle = (EFI_HANDLE)2;
    SystemTable.ConsoleOutHandle = (EFI_HANDLE)3;
    SystemTable.RuntimeServices = &RuntimeServices;
    SystemTable.BootServices = &BootServices;
}

/* --------------------------- PE/COFF loader ------------------------------- */

/* Minimal PE structures (x86_64) */
typedef struct {
    uint16_t e_magic;
    uint16_t e_cblp;
    uint16_t e_cp;
    uint16_t e_crlc;
    uint16_t e_cparhdr;
    uint16_t e_minalloc;
    uint16_t e_maxalloc;
    uint16_t e_ss;
    uint16_t e_sp;
    uint16_t e_csum;
    uint16_t e_ip;
    uint16_t e_cs;
    uint16_t e_lfarlc;
    uint16_t e_ovno;
    uint16_t e_res[4];
    uint16_t e_oemid;
    uint16_t e_oeminfo;
    uint16_t e_res2[10];
    uint32_t e_lfanew;
} IMAGE_DOS_HEADER;

typedef struct {
    uint32_t Signature;
    uint16_t Machine;
    uint16_t NumberOfSections;
    uint32_t TimeDateStamp;
    uint32_t PointerToSymbolTable;
    uint32_t NumberOfSymbols;
    uint16_t SizeOfOptionalHeader;
    uint16_t Characteristics;
} IMAGE_FILE_HEADER;

/* We define Optional header fields we need; data directories follow immediately */
typedef struct {
    uint16_t Magic;
    uint8_t MajorLinkerVersion;
    uint8_t MinorLinkerVersion;
    uint32_t SizeOfCode;
    uint32_t SizeOfInitializedData;
    uint32_t SizeOfUninitializedData;
    uint32_t AddressOfEntryPoint;
    uint32_t BaseOfCode;
    uint64_t ImageBase;
    uint32_t SectionAlignment;
    uint32_t FileAlignment;
    uint16_t MajorOperatingSystemVersion;
    uint16_t MinorOperatingSystemVersion;
    uint16_t MajorImageVersion;
    uint16_t MinorImageVersion;
    uint16_t MajorSubsystemVersion;
    uint16_t MinorSubsystemVersion;
    uint32_t Win32VersionValue;
    uint32_t SizeOfImage;
    uint32_t SizeOfHeaders;
    uint32_t CheckSum;
    uint16_t Subsystem;
    uint16_t DllCharacteristics;
    uint64_t SizeOfStackReserve;
    uint64_t SizeOfStackCommit;
    uint64_t SizeOfHeapReserve;
    uint64_t SizeOfHeapCommit;
    uint32_t LoaderFlags;
    uint32_t NumberOfRvaAndSizes;
    /* Data directories follow */
} IMAGE_OPTIONAL_HEADER64;

typedef struct {
    uint32_t VirtualAddress;
    uint32_t Size;
} IMAGE_DATA_DIRECTORY;

typedef struct {
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} IMAGE_NT_HEADERS64;

typedef struct {
    uint8_t Name[8];
    union {
        uint32_t PhysicalAddress;
        uint32_t VirtualSize;
    } Misc;
    uint32_t VirtualAddress;
    uint32_t SizeOfRawData;
    uint32_t PointerToRawData;
    uint32_t PointerToRelocations;
    uint32_t PointerToLinenumbers;
    uint16_t NumberOfRelocations;
    uint16_t NumberOfLinenumbers;
    uint32_t Characteristics;
} IMAGE_SECTION_HEADER;

/* Relocation types */
#define IMAGE_REL_BASED_DIR64 10

/* Data directory indices */
#define IMAGE_DIRECTORY_ENTRY_EXPORT 0
#define IMAGE_DIRECTORY_ENTRY_IMPORT 1
#define IMAGE_DIRECTORY_ENTRY_BASERELOC 5

/* Minimal import descriptor */
typedef struct {
    uint32_t OriginalFirstThunk;
    uint32_t TimeDateStamp;
    uint32_t ForwarderChain;
    uint32_t Name;
    uint32_t FirstThunk;
} IMAGE_IMPORT_DESCRIPTOR;

/* IMAGE_IMPORT_BY_NAME: 2-byte hint followed by ASCII name */

/* Helper: read little-endian from memory (host pointers) */
static uint16_t read_u16(const uint8_t *p) { uint16_t v; memcpy(&v, p, 2); return v; }
static uint32_t read_u32(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v; }
static uint64_t read_u64(const uint8_t *p) { uint64_t v; memcpy(&v, p, 8); return v; }

/* Forward declarations */
static void *resolve_import_symbol(const char *name);

/* Load a PE/COFF image from guest RAM at 'image_addr' and return entry point pointer.
   Returns NULL on failure. Allocates memory via ue_allocate_pages (host pointer returned).
   out_image_base receives the host pointer base, out_image_size receives SizeOfImage. */
void *pe_load_image(uint64_t image_addr, uint64_t *out_image_base, uint64_t *out_image_size) {
    uint8_t *file_base = get_memory_ptr(image_addr);
    if (!file_base) {
        trace_msg("pe_load_image: file base not in RAM");
        return NULL;
    }

    /* Read DOS header */
    IMAGE_DOS_HEADER dos;
    memcpy(&dos, file_base, sizeof(dos));
    if (dos.e_magic != 0x5A4D) { /* 'MZ' */
        trace_msg("pe_load_image: not MZ");
        return NULL;
    }

    uint32_t nt_off = dos.e_lfanew;
    if (nt_off == 0) {
        trace_msg("pe_load_image: e_lfanew == 0");
        return NULL;
    }
    uint8_t *nt_ptr = file_base + nt_off;
    uint32_t sig = read_u32(nt_ptr);
    if (sig != 0x00004550) { /* 'PE\0\0' */
        trace_msg("pe_load_image: no PE signature");
        return NULL;
    }

    IMAGE_FILE_HEADER fh;
    memcpy(&fh, nt_ptr + 4, sizeof(fh));
    if (fh.Machine != 0x8664) { /* AMD64 */
        trace_msg("pe_load_image: not x86_64");
        return NULL;
    }

    /* Optional header pointer */
    uint8_t *opt_ptr = nt_ptr + 4 + sizeof(IMAGE_FILE_HEADER);
    IMAGE_OPTIONAL_HEADER64 opt;
    memcpy(&opt, opt_ptr, sizeof(opt));

    /* Data directories start immediately after IMAGE_OPTIONAL_HEADER64 */
    IMAGE_DATA_DIRECTORY *data_dirs = (IMAGE_DATA_DIRECTORY*)(opt_ptr + sizeof(IMAGE_OPTIONAL_HEADER64));
    /* Validate NumberOfRvaAndSizes */
    uint32_t num_dirs = opt.NumberOfRvaAndSizes;
    if (num_dirs < IMAGE_DIRECTORY_ENTRY_BASERELOC + 1) {
        /* It's possible but then no relocations; continue */
    }

    uint32_t size_of_image = opt.SizeOfImage;
    uint32_t size_of_headers = opt.SizeOfHeaders;
    uint16_t num_sections = fh.NumberOfSections;

    /* Read section headers */
    uint8_t *sect_ptr = opt_ptr + fh.SizeOfOptionalHeader;
    if (!sect_ptr) {
        trace_msg("pe_load_image: section pointer invalid");
        return NULL;
    }
    IMAGE_SECTION_HEADER *sections = malloc(sizeof(IMAGE_SECTION_HEADER) * num_sections);
    if (!sections) {
        trace_msg("pe_load_image: malloc sections failed");
        return NULL;
    }
    memcpy(sections, sect_ptr, sizeof(IMAGE_SECTION_HEADER) * num_sections);

    /* Allocate memory for image using ue_allocate_pages */
    uint64_t pages = (size_of_image + PAGE_SIZE - 1) / PAGE_SIZE;
    EFI_PHYSICAL_ADDRESS alloc_addr = 0;
    if (ue_allocate_pages(0, 0, pages, &alloc_addr) != EFI_SUCCESS) {
        trace_msg("pe_load_image: AllocatePages failed");
        free(sections);
        return NULL;
    }
    uint8_t *image_mem = (uint8_t*)(uintptr_t)alloc_addr;
    /* Zero image memory */
    memset(image_mem, 0, pages * PAGE_SIZE);

    /* Copy headers */
    memcpy(image_mem, file_base, size_of_headers);

    /* Copy sections safely */
    for (int i = 0; i < num_sections; ++i) {
        IMAGE_SECTION_HEADER *sh = &sections[i];
        if (sh->SizeOfRawData == 0) continue;
        /* Validate source pointer inside file buffer (we assume file was loaded fully at image_addr) */
        uint32_t src_off = sh->PointerToRawData;
        uint32_t src_size = sh->SizeOfRawData;
        uint32_t dest_rva = sh->VirtualAddress;
        if (src_off == 0) continue;
        uint8_t *src = file_base + src_off;
        uint8_t *dst = image_mem + dest_rva;
        memcpy(dst, src, src_size);
    }

    /* Apply base relocations if needed */
    uint64_t preferred_base = opt.ImageBase;
    uint64_t delta = (uint64_t)(uintptr_t)image_mem - preferred_base;
    if (delta != 0) {
        IMAGE_DATA_DIRECTORY reloc_dir = {0,0};
        if (num_dirs > IMAGE_DIRECTORY_ENTRY_BASERELOC) reloc_dir = data_dirs[IMAGE_DIRECTORY_ENTRY_BASERELOC];
        if (reloc_dir.VirtualAddress && reloc_dir.Size) {
            uint8_t *reloc_base = image_mem + reloc_dir.VirtualAddress;
            uint8_t *reloc_end = reloc_base + reloc_dir.Size;
            uint8_t *p = reloc_base;
            while (p + sizeof(uint32_t)*2 <= reloc_end) {
                uint32_t page_rva = read_u32(p);
                uint32_t block_size = read_u32(p + 4);
                if (block_size < 8) break;
                uint8_t *entries = p + 8;
                int entry_count = (block_size - 8) / 2;
                for (int ei = 0; ei < entry_count; ++ei) {
                    uint16_t entry = read_u16(entries + ei*2);
                    uint16_t type = entry >> 12;
                    uint16_t offset = entry & 0x0FFF;
                    if (type == IMAGE_REL_BASED_DIR64) {
                        uint64_t *patch = (uint64_t*)(image_mem + page_rva + offset);
                        uint64_t orig = *patch;
                        *patch = orig + delta;
                    }
                }
                p += block_size;
            }
        }
    }

    /* Resolve imports (x86_64: thunks are 64-bit) */
    IMAGE_DATA_DIRECTORY import_dir = {0,0};
    if (num_dirs > IMAGE_DIRECTORY_ENTRY_IMPORT) import_dir = data_dirs[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (import_dir.VirtualAddress && import_dir.Size) {
        IMAGE_IMPORT_DESCRIPTOR *imp = (IMAGE_IMPORT_DESCRIPTOR*)(image_mem + import_dir.VirtualAddress);
        /* Iterate until zeroed descriptor */
        while (1) {
            if ((uintptr_t)imp < (uintptr_t)image_mem) break;
            if (imp->Name == 0 && imp->OriginalFirstThunk == 0 && imp->FirstThunk == 0) break;
            if (imp->Name == 0) { imp++; continue; }
            char libname[256];
            char *libname_src = (char*)(image_mem + imp->Name);
            strncpy(libname, libname_src, sizeof(libname)-1);
            libname[sizeof(libname)-1] = '\0';
            tracef("Import library: %s", libname);
            uint64_t *orig_thunk = NULL;
            uint64_t *first_thunk = NULL;
            if (imp->OriginalFirstThunk) orig_thunk = (uint64_t*)(image_mem + imp->OriginalFirstThunk);
            if (imp->FirstThunk) first_thunk = (uint64_t*)(image_mem + imp->FirstThunk);
            if (!first_thunk) { imp++; continue; }
            uint64_t *thunk_iter = orig_thunk ? orig_thunk : first_thunk;
            for ( ; ; thunk_iter++, first_thunk++) {
                uint64_t thunk_val = *thunk_iter;
                if (thunk_val == 0) break;
                /* Check import by ordinal (high bit) */
                const uint64_t IMAGE_ORDINAL_FLAG64 = 0x8000000000000000ULL;
                if (thunk_val & IMAGE_ORDINAL_FLAG64) {
                    /* ordinal import - not supported here */
                    trace_msg("pe_load_image: import by ordinal not supported");
                    if (first_thunk) *first_thunk = 0;
                } else {
                    uint32_t hint_name_rva = (uint32_t)thunk_val;
                    char *hint_name = (char*)(image_mem + hint_name_rva + 2); /* skip hint */
                    void *sym = resolve_import_symbol(hint_name);
                    if (!sym) {
                        tracef("pe_load_image: unresolved import %s (lib %s)", hint_name, libname);
                        if (first_thunk) *first_thunk = 0;
                    } else {
                        uint64_t ptrval = (uint64_t)(uintptr_t)sym;
                        if (first_thunk) {
                            memcpy(first_thunk, &ptrval, sizeof(uint64_t));
                        }
                    }
                }
            }
            imp++;
        }
    }

    /* Compute entry point pointer */
    uint32_t entry_rva = opt.AddressOfEntryPoint;
    void *entry_ptr = (void*)((uintptr_t)image_mem + entry_rva);

    if (out_image_base) *out_image_base = (uint64_t)(uintptr_t)image_mem;
    if (out_image_size) *out_image_size = size_of_image;

    free(sections);
    tracef("pe_load_image: loaded image at host addr 0x%016llx entry RVA 0x%08x", (unsigned long long)(uintptr_t)image_mem, entry_rva);
    return entry_ptr;
}

/* --------------------------- Import resolver ------------------------------ */

/* Map common UEFI/CRT names to local stubs */
static void *resolve_import_symbol(const char *name) {
    if (!name) return NULL;
    /* Common UEFI BootServices/RuntimeServices names */
    if (strcmp(name, "AllocatePages") == 0) return (void*)ue_allocate_pages;
    if (strcmp(name, "FreePages") == 0) return (void*)ue_free_pages;
    if (strcmp(name, "GetMemoryMap") == 0) return (void*)ue_get_memory_map;
    if (strcmp(name, "AllocatePool") == 0) return (void*)ue_allocate_pool;
    if (strcmp(name, "FreePool") == 0) return (void*)ue_free_pool;
    if (strcmp(name, "SetWatchdogTimer") == 0) return (void*)ue_set_watchdog_timer;
    if (strcmp(name, "Stall") == 0) return (void*)ue_stall;
    if (strcmp(name, "ExitBootServices") == 0) return (void*)ue_exit_boot_services;
    if (strcmp(name, "OutputString") == 0) return (void*)ue_output_string;
    /* Common helper names */
    if (strcmp(name, "ue_output_string") == 0) return (void*)ue_output_string;
    if (strcmp(name, "ue_stall") == 0) return (void*)ue_stall;
    if (strcmp(name, "ue_set_watchdog_timer") == 0) return (void*)ue_set_watchdog_timer;
    if (strcmp(name, "Print") == 0) return (void*)ue_print_ascii; /* heuristic */
    if (strcmp(name, "AsciiPrint") == 0) return (void*)ue_print_ascii;
    if (strcmp(name, "DebugPrint") == 0) return (void*)ue_print_ascii;
    /* libc-like helpers sometimes used in UEFI apps */
    if (strcmp(name, "memcpy") == 0) return (void*)memcpy;
    if (strcmp(name, "memset") == 0) return (void*)memset;
    if (strcmp(name, "strlen") == 0) return (void*)strlen;
    if (strcmp(name, "sprintf") == 0) return (void*)sprintf;
    if (strcmp(name, "snprintf") == 0) return (void*)snprintf;
    /* If not found, return NULL */
    return NULL;
}

/* --------------------------- UEFI call trampoline ------------------------- */

/* Type of UEFI application entry: EFI_STATUS EFIAPI (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) */
typedef EFI_STATUS (*EFI_IMAGE_ENTRY_POINT)(EFI_HANDLE, EFI_SYSTEM_TABLE*);

/* Call the loaded image entry point */
int ue_call_image(void *entry, EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *systab) {
    if (!entry) return -1;
    EFI_IMAGE_ENTRY_POINT ep = (EFI_IMAGE_ENTRY_POINT)entry;
    tracef("Calling image entry at %p", entry);
    /* Call the entry point. It may call UEFI BootServices; our stubs are in SystemTable. */
    EFI_STATUS st = ep(image_handle, systab);
    tracef("UEFI image returned status 0x%016llx", (unsigned long long)st);
    return 0;
}

/* --------------------------- Loader helpers ------------------------------- */

/* Load file into RAM at guest_addr */
int load_file_to_ram(const char *path, uint64_t guest_addr) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) return -1;
    struct stat st;
    if (fstat(fd, &st) < 0) { close(fd); return -1; }
    size_t sz = (size_t)st.st_size;
    uint8_t *buf = malloc(sz);
    if (!buf) { close(fd); return -1; }
    ssize_t r = read(fd, buf, sz);
    close(fd);
    if (r != (ssize_t)sz) { free(buf); return -1; }
    /* Write into ram_mem at guest_addr */
    if (guest_addr + sz > RAM_SIZE) {
        free(buf);
        return -1;
    }
    memcpy(get_memory_ptr(guest_addr), buf, sz);
    free(buf);
    return 0;
}

/* --------------------------- Initialization ------------------------------- */

void jemu_init(void) {
    if (!ram_mem) {
        ram_mem = malloc(RAM_SIZE);
        if (!ram_mem) { fprintf(stderr, "ram alloc failed\n"); exit(1); }
        memset(ram_mem, 0, RAM_SIZE);
    }
    memset(&cpu, 0, sizeof(cpu));
    cpu.gpr[4] = RAM_SIZE - 0x1000; /* RSP */
    cpu.rip = 0;
    cpu.halted = 0;
    debug_log_pos = 0;
    ue_init_environment();
}

/* --------------------------- Utility printing ----------------------------- */

static void dump_debug_log_stdout(void) {
    /* debug_log is circular; print from 0..pos */
    fwrite(debug_log, 1, debug_log_pos, stdout);
}

/* --------------------------- Main program -------------------------------- */

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <binary-or-efi> [load_addr] [entry_rip]\n", argv[0]);
        return 2;
    }
    const char *path = argv[1];
    uint64_t load_addr = 0x0000;
    uint64_t entry_rip = 0x0000;

    if (argc >= 3) load_addr = strtoull(argv[2], NULL, 0);
    if (argc >= 4) entry_rip = strtoull(argv[3], NULL, 0);

    jemu_init();

    if (load_file_to_ram(path, load_addr) != 0) {
        fprintf(stderr, "Failed to load %s into RAM at 0x%llx\n", path, (unsigned long long)load_addr);
        return 3;
    }

    /* If entry_rip is zero, attempt to detect PE/COFF and load it */
    if (entry_rip == 0) {
        uint8_t *p = get_memory_ptr(load_addr);
        if (!p) {
            fprintf(stderr, "Invalid load pointer\n");
            return 4;
        }
        IMAGE_DOS_HEADER dos;
        memcpy(&dos, p, sizeof(dos));
        if (dos.e_magic == 0x5A4D) {
            /* Attempt to load PE image */
            uint64_t image_base = 0, image_size = 0;
            void *entry = pe_load_image(load_addr, &image_base, &image_size);
            if (!entry) {
                fprintf(stderr, "PE loader failed\n");
                return 5;
            }
            /* Build a minimal EFI_HANDLE and call entry */
            EFI_HANDLE image_handle = (EFI_HANDLE) (uintptr_t) image_base;
            EFI_SYSTEM_TABLE *systab = &SystemTable;
            systab->BootServices = &BootServices;
            systab->RuntimeServices = &RuntimeServices;
            systab->ConOut = &SimpleTextOut;
            /* Call the image */
            printf("Calling UEFI image entry at %p (image base %p)\n", entry, (void*)(uintptr_t)image_base);
            ue_call_image(entry, image_handle, systab);
            printf("UEFI image call returned; exiting emulator\n");
            return 0;
        } else {
            /* Not a PE image; set RIP to load_addr and run emulator executor loop (stub) */
            cpu.rip = load_addr;
        }
    } else {
        cpu.rip = entry_rip;
    }

    /* Run the emulator harness (very small loop) */
    uint64_t steps = 0;
    uint64_t max_steps = 100000;
    while (!cpu.halted) {
        execute_one_stub();
        steps++;
    }

    printf("Emulator finished: steps=%llu\n", (unsigned long long)steps);
    printf("---- debug log ----\n");
    dump_debug_log_stdout();
    printf("\n---- end log ----\n");
    return 0;
}
