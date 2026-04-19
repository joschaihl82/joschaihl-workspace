/*
 jitemu.c
 EFI loader that maps a PE/COFF .efi into executable memory and invokes it.
 This version builds a minimal, host-backed EFI SystemTable that includes:
  - a ConsoleOut (ConOut) object whose first function pointer is OutputString
  - a BootServices pointer at SystemTable+0x30 (kept for compatibility)
  - ExitBootServices / Stall / AllocatePool / FreePool stubs can be added if needed
 The loader sets registers so the loaded image sees a SystemTable pointer and
 can call ConOut->OutputString(SystemTable->ConOut, msg).
 WARNING: This executes code from the .efi file in the host process. Only run trusted images.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <inttypes.h>

/* ---------------- PE/COFF parsing structs ---------------- */
#pragma pack(push,1)
typedef struct {
    uint16_t e_magic;    /* "MZ" */
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
    uint32_t e_lfanew;   /* offset to PE header */
} IMAGE_DOS_HEADER;

typedef struct {
    uint32_t Signature; /* "PE\0\0" */
    uint16_t Machine;
    uint16_t NumberOfSections;
    uint32_t TimeDateStamp;
    uint32_t PointerToSymbolTable;
    uint32_t NumberOfSymbols;
    uint16_t SizeOfOptionalHeader;
    uint16_t Characteristics;
} IMAGE_FILE_HEADER;

typedef struct {
    uint16_t Magic; /* 0x10b or 0x20b */
    uint8_t  MajorLinkerVersion;
    uint8_t  MinorLinkerVersion;
    uint32_t SizeOfCode;
    uint32_t SizeOfInitializedData;
    uint32_t SizeOfUninitializedData;
    uint32_t AddressOfEntryPoint;
    uint32_t BaseOfCode;
    /* rest omitted */
} IMAGE_OPTIONAL_HEADER32;

typedef struct {
    uint8_t  Name[8];
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
#pragma pack(pop)

/* ---------------- helpers ---------------- */

static uint8_t* read_file(const char* path, size_t* out_size) {
    if (!path || !out_size) return NULL;
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return NULL; }
    rewind(f);
    uint8_t* buf = malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) { free(buf); fclose(f); return NULL; }
    fclose(f);
    *out_size = (size_t)sz;
    return buf;
}

static void* alloc_executable(size_t size) {
    long ps = sysconf(_SC_PAGE_SIZE);
    if (ps <= 0) ps = 4096;
    size_t page = (size_t)ps;
    size = (size + page - 1) & ~(page - 1);
    void* p = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (p == MAP_FAILED) {
        perror("mmap exec");
        return NULL;
    }
    return p;
}
static void free_executable(void* p, size_t size) {
    if (!p || p == MAP_FAILED) return;
    long ps = sysconf(_SC_PAGE_SIZE);
    if (ps <= 0) ps = 4096;
    size_t page = (size_t)ps;
    size = (size + page - 1) & ~(page - 1);
    munmap(p, size);
}

/* ---------------- PE loader -> executable buffer ----------------
   Copies sections into an executable buffer and returns:
     - exec buffer pointer
     - entry function pointer (exec + entry_rva)
   Caller must free exec buffer with free_executable(exec, alloc_size).
*/
static void* load_pe_to_executable(const char* path, void (**entry_ptr)(void), size_t* out_alloc_size, uint64_t* out_entry_rva) {
    size_t fsz = 0;
    uint8_t* file = read_file(path, &fsz);
    if (!file) {
        fprintf(stderr, "[loader] cannot read file: %s\n", path);
        return NULL;
    }

    if (fsz < sizeof(IMAGE_DOS_HEADER)) { free(file); return NULL; }
    IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)file;
    if (dos->e_magic != 0x5A4D) { free(file); return NULL; }
    if ((size_t)dos->e_lfanew + 4 + sizeof(IMAGE_FILE_HEADER) > fsz) { free(file); return NULL; }

    uint8_t* pe_ptr = file + dos->e_lfanew;
    uint32_t pe_sig = *(uint32_t*)pe_ptr;
    if (pe_sig != 0x00004550) { free(file); return NULL; }

    IMAGE_FILE_HEADER* fh = (IMAGE_FILE_HEADER*)(pe_ptr + 4);
    uint16_t nsec = fh->NumberOfSections;
    uint16_t opt_size = fh->SizeOfOptionalHeader;
    uint8_t* opt_hdr = pe_ptr + 4 + sizeof(IMAGE_FILE_HEADER);
    if ((size_t)(opt_hdr - file) + opt_size > fsz) { free(file); return NULL; }

    uint16_t magic = *(uint16_t*)opt_hdr;
    if (magic != 0x20b && magic != 0x10b) { free(file); return NULL; }

    uint64_t entry_rva = *(uint32_t*)(opt_hdr + 16); /* AddressOfEntryPoint */

    uint8_t* sec_table = opt_hdr + opt_size;
    if ((size_t)(sec_table - file) + nsec * sizeof(IMAGE_SECTION_HEADER) > fsz) { free(file); return NULL; }

    uint64_t max_va = 0;
    for (int i = 0; i < nsec; ++i) {
        IMAGE_SECTION_HEADER* sh = (IMAGE_SECTION_HEADER*)(sec_table + i * sizeof(IMAGE_SECTION_HEADER));
        uint64_t va_end = (uint64_t)sh->VirtualAddress + (uint64_t)sh->Misc.VirtualSize;
        if (va_end > max_va) max_va = va_end;
    }
    if (max_va == 0) max_va = 0x1000;
    size_t alloc_size = (size_t)((max_va + 0xFFF) & ~0xFFFULL);
    if (alloc_size < 0x1000) alloc_size = 0x1000;

    void* exec = alloc_executable(alloc_size);
    if (!exec) { free(file); return NULL; }
    memset(exec, 0, alloc_size);

    for (int i = 0; i < nsec; ++i) {
        IMAGE_SECTION_HEADER* sh = (IMAGE_SECTION_HEADER*)(sec_table + i * sizeof(IMAGE_SECTION_HEADER));
        uint32_t vaddr = sh->VirtualAddress;
        uint32_t rawsz = sh->SizeOfRawData;
        uint32_t rawptr = sh->PointerToRawData;
        uint32_t vsize = sh->Misc.VirtualSize;

        if ((size_t)rawptr + rawsz > fsz) {
            fprintf(stderr, "[loader] section %.8s raw out of file bounds\n", (char*)sh->Name);
            free_executable(exec, alloc_size); free(file); return NULL;
        }
        if ((size_t)vaddr + rawsz > alloc_size) {
            fprintf(stderr, "[loader] section %.8s would overflow exec buffer\n", (char*)sh->Name);
            free_executable(exec, alloc_size); free(file); return NULL;
        }

        memcpy((uint8_t*)exec + vaddr, file + rawptr, rawsz);
        if (vsize > rawsz) memset((uint8_t*)exec + vaddr + rawsz, 0, vsize - rawsz);

        printf("[loader] section %.8s -> exec[0x%08x] raw=0x%x vsize=0x%x\n",
               (char*)sh->Name, vaddr, rawsz, vsize);
    }

    void* entry = (uint8_t*)exec + (size_t)entry_rva;
    printf("[loader] entry RVA=0x%08" PRIx64 " -> exec@%p\n", entry_rva, entry);

    *entry_ptr = (void(*)(void*))entry;
    if (out_alloc_size) *out_alloc_size = alloc_size;
    if (out_entry_rva) *out_entry_rva = entry_rva;
    free(file);
    return exec;
}

/* ---------------- Host-provided UEFI-like services ----------------
   We implement a minimal ConsoleOut->OutputString and a few placeholders.
*/

/* OutputString signature:
   EFI_STATUS OutputString(void* This, const CHAR16* String)
   CHAR16 is 16-bit UTF-16 code unit. We'll convert ASCII-range characters to UTF-8 and print.
*/
static uint64_t host_OutputString(void* This, const uint16_t* str) {
    (void)This;
    if (!str) return 1;
    /* Convert UTF-16 (basic BMP) to UTF-8 for ASCII-range; non-ASCII replaced with '?' */
    char buf[1024];
    size_t bi = 0;
    for (const uint16_t* p = str; *p != 0; ++p) {
        uint16_t ch = *p;
        if (ch < 0x80) {
            if (bi + 1 >= sizeof(buf)) break;
            buf[bi++] = (char)ch;
        } else {
            /* non-ASCII: replace with '?' to keep it simple */
            if (bi + 1 >= sizeof(buf)) break;
            buf[bi++] = '?';
        }
    }
    if (bi > 0) {
        fwrite(buf, 1, bi, stdout);
        fflush(stdout);
    }
    return 0; /* EFI_SUCCESS */
}

/* A simple host stub that may be called by the image if it dereferences BootServices first qword */
static void host_service_stub(void) {
    printf("[host_stub] called by EFI image\n");
    fflush(stdout);
}

/* Build a fake SystemTable and related objects:
   - st_buf: allocated buffer representing SystemTable
   - bs_buf: BootServices buffer
   - conout_buf: SimpleTextOutputProtocol-like buffer whose first qword is pointer to OutputString
   The function writes pointers into st_buf at multiple offsets to maximize compatibility:
     - BootServices pointer at offset 0x30 (observed in the provided image)
     - ConOut pointer at several likely offsets (0x18, 0x20, 0x28, 0x40) to increase chance the image finds it
*/
static int build_fake_system_table(void** out_st, void** out_bs, void** out_conout) {
    if (!out_st || !out_bs || !out_conout) return -1;
    size_t st_size = 0x200;
    size_t bs_size = 0x200;
    size_t conout_size = 0x100;

    uint8_t* st = calloc(1, st_size);
    uint8_t* bs = calloc(1, bs_size);
    uint8_t* conout = calloc(1, conout_size);
    if (!st || !bs || !conout) { free(st); free(bs); free(conout); return -1; }

    /* Place BootServices pointer at offset 0x30 (observed in disassembly) */
    uint64_t bs_ptr = (uint64_t)(uintptr_t)bs;
    memcpy(st + 0x30, &bs_ptr, sizeof(bs_ptr));

    /* Place ConOut pointer at several offsets commonly used by different toolchains/headers */
    uint64_t conout_ptr = (uint64_t)(uintptr_t)conout;
    /* offsets to try: 0x18, 0x20, 0x28, 0x40 */
    memcpy(st + 0x18, &conout_ptr, sizeof(conout_ptr));
    memcpy(st + 0x20, &conout_ptr, sizeof(conout_ptr));
    memcpy(st + 0x28, &conout_ptr, sizeof(conout_ptr));
    memcpy(st + 0x40, &conout_ptr, sizeof(conout_ptr));

    /* BootServices first qword: point to host_service_stub for compatibility with images that call [rcx] */
    uint64_t stub_ptr = (uint64_t)(uintptr_t)host_service_stub;
    memcpy(bs + 0x00, &stub_ptr, sizeof(stub_ptr));

    /* ConOut first qword: pointer to OutputString function */
    uint64_t outstr_ptr = (uint64_t)(uintptr_t)host_OutputString;
    memcpy(conout + 0x00, &outstr_ptr, sizeof(outstr_ptr));

    /* Optionally, place pointer to ConOut also at st+0x48 (some layouts) */
    memcpy(st + 0x48, &conout_ptr, sizeof(conout_ptr));

    *out_st = st;
    *out_bs = bs;
    *out_conout = conout;
    return 0;
}

/* Build a trampoline that sets registers and calls the entry:
   - RDI = ImageHandle (we pass exec buffer pointer)
   - RDX = SystemTable pointer (some images expect SystemTable in RDX)
   - RSI = SystemTable pointer as well (some images expect RSI)
   Then CALL entry (relative).
   The trampoline is allocated in executable memory and returned.
*/
static void* build_trampoline(void (*entry)(void), void* image_handle, void* system_table) {
    uint8_t* tramp = alloc_executable(4096);
    if (!tramp) return NULL;
    uint8_t* cp = tramp;

    /* mov rdi, imm64 -> 48 BF imm64 */
    *cp++ = 0x48; *cp++ = 0xBF;
    uint64_t ih = (uint64_t)(uintptr_t)image_handle;
    memcpy(cp, &ih, 8); cp += 8;

    /* mov rdx, imm64 -> 48 BA imm64 */
    *cp++ = 0x48; *cp++ = 0xBA;
    uint64_t stp = (uint64_t)(uintptr_t)system_table;
    memcpy(cp, &stp, 8); cp += 8;

    /* mov rsi, imm64 -> 48 BE imm64 */
    *cp++ = 0x48; *cp++ = 0xBE;
    memcpy(cp, &stp, 8); cp += 8;

    /* call rel32 -> E8 rel32 */
    *cp++ = 0xE8;
    int64_t rel = (int64_t)((uint8_t*)entry - (cp + 4));
    int32_t rel32 = (int32_t)rel;
    memcpy(cp, &rel32, 4); cp += 4;

    /* ret */
    *cp++ = 0xC3;

    return tramp;
}

/* ---------------- main ---------------- */

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <bootx64.efi>\n", argv[0]);
        return 1;
    }
    const char* path = argv[1];

    /* Load PE into executable buffer */
    void (*entry_fn)(void) = NULL;
    size_t exec_size = 0;
    uint64_t entry_rva = 0;
    void* exec_buf = load_pe_to_executable(path, &entry_fn, &exec_size, &entry_rva);
    if (!exec_buf || !entry_fn) {
        fprintf(stderr, "Failed to load EFI image\n");
        return 2;
    }
    printf("[main] loaded image exec@%p size=%zu entry_rva=0x%08" PRIx64 "\n", exec_buf, exec_size, entry_rva);

    /* Build fake SystemTable / BootServices / ConOut */
    void* st_buf = NULL;
    void* bs_buf = NULL;
    void* conout_buf = NULL;
    if (build_fake_system_table(&st_buf, &bs_buf, &conout_buf) != 0) {
        fprintf(stderr, "Failed to build fake SystemTable\n");
        free_executable(exec_buf, exec_size);
        return 3;
    }
    printf("[main] SystemTable=%p BootServices=%p ConOut=%p\n", st_buf, bs_buf, conout_buf);

    /* ImageHandle: pass exec_buf as handle */
    void* image_handle = exec_buf;

    /* Build trampoline and call */
    void* tramp = build_trampoline(entry_fn, image_handle, st_buf);
    if (!tramp) {
        fprintf(stderr, "Failed to build trampoline\n");
        free(st_buf); free(bs_buf); free(conout_buf);
        free_executable(exec_buf, exec_size);
        return 4;
    }

    printf("[main] invoking entry via trampoline %p\n", tramp);
    typedef void (*TrampFn)(void);
    TrampFn tf = (TrampFn)tramp;
    tf();
    printf("[main] returned from EFI image entry\n");

    /* Cleanup */
    free_executable(tramp, 4096);
    free(st_buf);
    free(bs_buf);
    free(conout_buf);
    free_executable(exec_buf, exec_size);

    return 0;
}
