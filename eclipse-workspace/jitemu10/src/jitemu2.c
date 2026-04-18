/*
 jitemu.c
 Robust EFI loader:
 - Handles malformed OptionalHeader (e.g., magic 0x0200) by preferring named sections.
 - Filters heuristic false positives (.idata fragments like "idata","ta","a"; same for xdata).
 - Deduplicates and prevents overlapping mapping.
 - Uses Microsoft x64 ABI trampoline (RCX=ImageHandle, RDX=SystemTable).
 - Minimal SystemTable/BootServices/ConOut wiring with OutputString and a host service stub.
 WARNING: executes loaded .efi in-process. Run only trusted images.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>

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

/* ---------- helpers ---------- */

static uint8_t* read_file(const char* path, size_t* out_size) {
    if (!path || !out_size) return NULL;
    FILE* f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "[error] fopen('%s') failed: %s\n", path, strerror(errno));
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0) {
        fprintf(stderr, "[error] fseek end failed: %s\n", strerror(errno));
        fclose(f); return NULL;
    }
    long sz = ftell(f);
    if (sz < 0) {
        fprintf(stderr, "[error] ftell failed: %s\n", strerror(errno));
        fclose(f); return NULL;
    }
    rewind(f);
    uint8_t* buf = malloc((size_t)sz);
    if (!buf) { fprintf(stderr, "[error] malloc(%ld) failed\n", sz); fclose(f); return NULL; }
    size_t got = fread(buf, 1, (size_t)sz, f);
    if (got != (size_t)sz) {
        fprintf(stderr, "[error] fread expected %ld bytes, got %zu\n", sz, got);
        free(buf); fclose(f); return NULL;
    }
    fclose(f);
    *out_size = (size_t)sz;
    fprintf(stderr, "[info] read '%s' size=%zu bytes\n", path, *out_size);
    return buf;
}

static void* alloc_executable(size_t size) {
    long ps = sysconf(_SC_PAGE_SIZE);
    if (ps <= 0) ps = 4096;
    size_t page = (size_t)ps;
    size = (size + page - 1) & ~(page - 1);
    void* p = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (p == MAP_FAILED) {
        fprintf(stderr, "[error] mmap exec size=%zu failed: %s\n", size, strerror(errno));
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

/* Validate basic section candidates */
static int validate_section(const IMAGE_SECTION_HEADER* sh, size_t file_size, uint32_t headers_limit) {
    if (!sh) return 0;
    uint32_t ptr = sh->PointerToRawData;
    uint32_t rawsz = sh->SizeOfRawData;

    /* accept virtual-only sections (rawsz==0) but treat carefully */
    if (rawsz > 0) {
        if ((size_t)ptr + rawsz > file_size) return 0;
        /* disallow pointers into headers range to kill ASCII-fragment matches */
        if (ptr < headers_limit) return 0;
        /* tiny blobs tend to be false positives */
        if (rawsz < 16) return 0;
    }
    if (sh->VirtualAddress > 0x10000000u) return 0;
    return 1;
}

/* Find section headers by name in the raw file (padded 8-byte name) */
static IMAGE_SECTION_HEADER* find_section_by_name(uint8_t* file, size_t fsz, const char* name, uint32_t headers_limit) {
    size_t name_len = strlen(name);
    if (name_len > 8) return NULL;
    char padded[8]; memset(padded, 0, 8); memcpy(padded, name, name_len);

    for (size_t off = 0; off + sizeof(IMAGE_SECTION_HEADER) <= fsz; ++off) {
        if (memcmp(file + off, padded, 8) == 0) {
            const IMAGE_SECTION_HEADER* cand = (const IMAGE_SECTION_HEADER*)(file + off);
            if (validate_section(cand, fsz, headers_limit)) {
                IMAGE_SECTION_HEADER* copy = malloc(sizeof(IMAGE_SECTION_HEADER));
                if (!copy) return NULL;
                memcpy(copy, cand, sizeof(IMAGE_SECTION_HEADER));
                fprintf(stderr, "[info] found section by name: %s RVA=0x%08x RawPtr=0x%08x RawSz=0x%08x\n",
                        name, copy->VirtualAddress, copy->PointerToRawData, copy->SizeOfRawData);
                return copy;
            }
        }
    }
    return NULL;
}

/* Heuristic discovery near PE header when named lookup fails */
static IMAGE_SECTION_HEADER* heuristic_discover(uint8_t* base, size_t base_off, size_t file_size, int* out_count, uint32_t headers_limit) {
    const size_t scan_limit = 0x400;
    size_t start = base_off;
    size_t end = base_off + (scan_limit > file_size - base_off ? file_size - base_off : scan_limit);
    int max_sections = 256;
    IMAGE_SECTION_HEADER* found = calloc(max_sections, sizeof(IMAGE_SECTION_HEADER));
    if (!found) { *out_count = 0; return NULL; }
    int found_count = 0;

    for (size_t off = start; off + sizeof(IMAGE_SECTION_HEADER) <= end; off += 1) {
        const IMAGE_SECTION_HEADER* cand = (const IMAGE_SECTION_HEADER*)(base + off);
        /* quick checks: printable name, raw bounds, RVA reasonableness */
        int printable = 1;
        for (int i = 0; i < 8; ++i) {
            unsigned char c = cand->Name[i];
            if (c == 0) break;
            if ((c < 0x20 || c > 0x7E) && c != '.' && c != '_' && c != '$') { printable = 0; break; }
        }
        if (!printable) continue;

        if (!validate_section(cand, file_size, headers_limit)) continue;

        if (found_count < max_sections) {
            memcpy(&found[found_count], cand, sizeof(IMAGE_SECTION_HEADER));
            found_count++;
        } else break;
    }
    *out_count = found_count;
    if (found_count > 0) fprintf(stderr, "[info] discovered %d plausible section headers by heuristic scan\n", found_count);
    else { free(found); found = NULL; }
    return found;
}

/* ---------- loader (prefers named sections, filters/dedup heuristic) ---------- */

static void* load_pe_to_executable_verbose(const char* path,
                                          void (**entry_ptr)(void),
                                          size_t* out_alloc_size,
                                          uint64_t* out_entry_rva)
{
    size_t fsz = 0;
    uint8_t* file = read_file(path, &fsz);
    if (!file) { fprintf(stderr, "[fail] cannot open or read file '%s'\n", path); return NULL; }
    if (fsz < sizeof(IMAGE_DOS_HEADER)) { fprintf(stderr, "[fail] file too small for IMAGE_DOS_HEADER\n"); free(file); return NULL; }

    IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)file;
    fprintf(stderr, "[info] DOS header e_magic=0x%04x e_lfanew=0x%08x\n", dos->e_magic, dos->e_lfanew);
    if (dos->e_magic != 0x5A4D) { fprintf(stderr, "[fail] invalid DOS signature\n"); free(file); return NULL; }

    if ((size_t)dos->e_lfanew + 4 + sizeof(IMAGE_FILE_HEADER) > fsz) {
        fprintf(stderr, "[fail] e_lfanew points beyond file\n"); free(file); return NULL;
    }

    uint8_t* pe_ptr = file + dos->e_lfanew;
    uint32_t pe_sig = *(uint32_t*)pe_ptr;
    fprintf(stderr, "[info] PE signature at offset 0x%08x = 0x%08x\n", dos->e_lfanew, pe_sig);
    if (pe_sig != 0x00004550) { fprintf(stderr, "[fail] invalid PE signature\n"); free(file); return NULL; }

    IMAGE_FILE_HEADER* fh = (IMAGE_FILE_HEADER*)(pe_ptr + 4);
    fprintf(stderr, "[info] IMAGE_FILE_HEADER: Machine=0x%04x NumberOfSections=%u SizeOfOptionalHeader=%u Characteristics=0x%04x\n",
            fh->Machine, fh->NumberOfSections, fh->SizeOfOptionalHeader, fh->Characteristics);

    uint8_t* opt_hdr = pe_ptr + 4 + sizeof(IMAGE_FILE_HEADER);
    size_t opt_size = fh->SizeOfOptionalHeader;
    if ((size_t)(opt_hdr - file) + opt_size > fsz) {
        fprintf(stderr, "[warn] optional header size exceeds file bounds; capping\n");
        opt_size = (size_t)(fsz - (size_t)(opt_hdr - file));
    }
    uint16_t magic = 0;
    if (opt_size >= 2) magic = *(uint16_t*)opt_hdr;
    fprintf(stderr, "[info] OptionalHeader magic=0x%04x (opt_size=%zu)\n", magic, opt_size);
    if (magic != 0x10b && magic != 0x20b) {
        fprintf(stderr, "[warn] unknown OptionalHeader magic: 0x%04x (not PE32/PE32+). Fallbacks engaged\n", magic);
        size_t dump_n = opt_size < 64 ? opt_size : 64;
        fprintf(stderr, "[debug] optional header first %zu bytes:\n", dump_n);
        for (size_t i = 0; i < dump_n; ++i) {
            if ((i & 15) == 0) fprintf(stderr, "  %04zx: ", i);
            fprintf(stderr, "%02x ", opt_hdr[i]);
            if ((i & 15) == 15) fprintf(stderr, "\n");
        }
        if (dump_n & 15) fprintf(stderr, "\n");
    }

    /* Define a conservative "headers limit" to reject pointer-to-raw inside header area */
    uint32_t headers_limit = (uint32_t)(dos->e_lfanew + 0x200);

    /* Prefer named sections */
    const char* names[] = { ".text", ".rdata", ".pdata", ".xdata", ".idata" };
    const int nameN = sizeof(names)/sizeof(names[0]);
    IMAGE_SECTION_HEADER** named = calloc(nameN, sizeof(IMAGE_SECTION_HEADER*));
    int named_count = 0;
    if (named) {
        for (int i = 0; i < nameN; ++i) {
            IMAGE_SECTION_HEADER* sh = find_section_by_name(file, fsz, names[i], headers_limit);
            if (sh) named[named_count++] = sh;
        }
    }

    IMAGE_SECTION_HEADER* sections = NULL;
    int sections_count = 0;

    if (named_count > 0) {
        sections = malloc(sizeof(IMAGE_SECTION_HEADER) * named_count);
        if (!sections) { fprintf(stderr, "[fail] malloc sections\n"); for (int i=0;i<named_count;i++) free(named[i]); free(named); free(file); return NULL; }
        for (int i = 0; i < named_count; ++i) {
            memcpy(&sections[i], named[i], sizeof(IMAGE_SECTION_HEADER));
            free(named[i]);
        }
        free(named);
        sections_count = named_count;
        fprintf(stderr, "[info] using %d named sections\n", sections_count);
    } else {
        /* fallback: try declared section table if plausible */
        uint16_t nsec = fh->NumberOfSections;
        uint8_t* sec_table = opt_hdr + opt_size;
        size_t need = (size_t)nsec * sizeof(IMAGE_SECTION_HEADER);
        if (nsec > 0 && nsec <= 1024 && (size_t)(sec_table - file) + need <= fsz) {
            sections = malloc(need);
            if (!sections) { fprintf(stderr, "[fail] malloc sections\n"); free(file); return NULL; }
            memcpy(sections, sec_table, need);
            sections_count = nsec;
            fprintf(stderr, "[info] read %d section headers from file table\n", sections_count);
        } else {
            /* heuristic scan near PE header */
            int discovered = 0;
            sections = heuristic_discover(file, (size_t)(pe_ptr + 4 + sizeof(IMAGE_FILE_HEADER) - file), fsz, &discovered, headers_limit);
            sections_count = discovered;
            if (sections_count == 0) {
                fprintf(stderr, "[warn] no usable section table found; will map entire file and set entry=0\n");
            }
        }
    }

    /* Choose entry RVA: use OptionalHeader if valid, else .text, else first executable, else 0 */
    uint64_t entry_rva = 0;
    int recognized_optional = 0;
    if (magic == 0x10b || magic == 0x20b) {
        if (opt_size >= 20) {
            entry_rva = *(uint32_t*)(opt_hdr + 16);
            recognized_optional = 1;
            fprintf(stderr, "[info] AddressOfEntryPoint (RVA) = 0x%08" PRIx64 "\n", entry_rva);
        }
    } else {
        fprintf(stderr, "[warn] OptionalHeader not PE32/PE32+; prefer discovered sections for entry\n");
    }

    uint64_t max_va = 0;
    uint64_t candidate_entry = 0;
    int found_text = 0;
    const uint32_t IMAGE_SCN_MEM_EXECUTE = 0x20000000u;

    if (sections_count > 0 && sections) {
        for (int i = 0; i < sections_count; ++i) {
            IMAGE_SECTION_HEADER* sh = &sections[i];
            char name[9]; memcpy(name, sh->Name, 8); name[8] = 0;
            fprintf(stderr, "  [%02d] Name='%.8s' RVA=0x%08x VSize=0x%08x RawPtr=0x%08x RawSz=0x%08x Char=0x%08x\n",
                    i, name, sh->VirtualAddress, sh->Misc.VirtualSize, sh->PointerToRawData, sh->SizeOfRawData, sh->Characteristics);
            uint64_t va_end = (uint64_t)sh->VirtualAddress + (uint64_t)sh->Misc.VirtualSize;
            if (va_end > max_va) max_va = va_end;

            if (!recognized_optional) {
                if (!found_text && strcmp(name, ".text") == 0) {
                    candidate_entry = sh->VirtualAddress;
                    found_text = 1;
                    fprintf(stderr, "[info] found .text section, candidate entry RVA = 0x%08" PRIx64 "\n", candidate_entry);
                } else if ((sh->Characteristics & IMAGE_SCN_MEM_EXECUTE) && candidate_entry == 0) {
                    candidate_entry = sh->VirtualAddress;
                    fprintf(stderr, "[info] found executable section '%.8s', candidate entry RVA = 0x%08" PRIx64 "\n", name, candidate_entry);
                }
            }
        }
    } else {
        max_va = fsz;
    }
    if (!recognized_optional) {
        if (candidate_entry != 0) {
            entry_rva = candidate_entry;
            fprintf(stderr, "[info] using candidate entry RVA: 0x%08" PRIx64 "\n", entry_rva);
        } else {
            entry_rva = 0;
            fprintf(stderr, "[warn] no candidate entry; using entry RVA = 0\n");
        }
    }

    if (max_va == 0) max_va = fsz;
    size_t alloc_size = (size_t)((max_va + 0xFFF) & ~0xFFFULL);
    if (alloc_size < fsz) alloc_size = ((fsz + 0xFFF) & ~0xFFFULL);
    if (alloc_size < 0x1000) alloc_size = 0x1000;
    fprintf(stderr, "[info] allocating exec buffer size=%zu (max_va=0x%zx file_size=%zu)\n", alloc_size, (size_t)max_va, fsz);

    void* exec = alloc_executable(alloc_size);
    if (!exec) { fprintf(stderr, "[fail] alloc_executable(%zu) failed\n", alloc_size); if (sections) free(sections); free(file); return NULL; }
    memset(exec, 0, alloc_size);

    /* Build an index to filter, dedup, and avoid overlaps before mapping */
    typedef struct { IMAGE_SECTION_HEADER sh; int keep; } SecEntry;
    SecEntry* index = NULL;
    int idx_count = sections_count;

    if (idx_count > 0 && sections) {
        index = calloc(idx_count, sizeof(SecEntry));
        if (!index) { fprintf(stderr, "[fail] calloc index\n"); free_executable(exec, alloc_size); free(sections); free(file); return NULL; }
        for (int i = 0; i < idx_count; ++i) { index[i].sh = sections[i]; index[i].keep = 0; }

        /* Step 1: basic validation & filtering */
        for (int i = 0; i < idx_count; ++i) {
            IMAGE_SECTION_HEADER* sh = &index[i].sh;
            uint32_t rawsz = sh->SizeOfRawData;
            uint32_t rawptr = sh->PointerToRawData;

            if (rawsz > 0) {
                if (rawsz < 16) { fprintf(stderr, "[skip] '%.8s' rawsz=0x%x too small\n", sh->Name, rawsz); continue; }
                if (rawptr < headers_limit) { fprintf(stderr, "[skip] '%.8s' rawptr=0x%08x in headers\n", sh->Name, rawptr); continue; }
                if ((size_t)rawptr + rawsz > fsz) { fprintf(stderr, "[skip] '%.8s' raw out of bounds ptr=0x%08x sz=0x%08x\n", sh->Name, rawptr, rawsz); continue; }
            }
            if (sh->VirtualAddress > 0x10000000u) { fprintf(stderr, "[skip] '%.8s' RVA=0x%08x implausible\n", sh->Name, sh->VirtualAddress); continue; }

            index[i].keep = 1;
        }

        /* Step 2: deduplicate by Name or VirtualAddress, keep larger rawsz */
        for (int i = 0; i < idx_count; ++i) {
            if (!index[i].keep) continue;
            for (int j = i + 1; j < idx_count; ++j) {
                if (!index[j].keep) continue;
                int same_name = (strncmp((char*)index[i].sh.Name, (char*)index[j].sh.Name, 8) == 0);
                int same_va   = (index[i].sh.VirtualAddress == index[j].sh.VirtualAddress);
                if (same_name || same_va) {
                    uint32_t szi = index[i].sh.SizeOfRawData;
                    uint32_t szj = index[j].sh.SizeOfRawData;
                    if (szj > szi) {
                        index[i].keep = 0;
                        fprintf(stderr, "[dedup] keep j=%d over i=%d for '%.8s' (raw 0x%x > 0x%x)\n",
                                j, i, index[j].sh.Name, szj, szi);
                    } else {
                        index[j].keep = 0;
                        fprintf(stderr, "[dedup] keep i=%d over j=%d for '%.8s' (raw 0x%x >= 0x%x)\n",
                                i, j, index[i].sh.Name, szi, szj);
                    }
                }
            }
        }

        /* Step 3: avoid overlapping exec ranges, keep larger mapped size */
        for (int i = 0; i < idx_count; ++i) {
            if (!index[i].keep) continue;
            uint64_t va_i = index[i].sh.VirtualAddress;
            uint64_t end_i = va_i + (index[i].sh.Misc.VirtualSize ? index[i].sh.Misc.VirtualSize : index[i].sh.SizeOfRawData);
            for (int j = i + 1; j < idx_count; ++j) {
                if (!index[j].keep) continue;
                uint64_t va_j = index[j].sh.VirtualAddress;
                uint64_t end_j = va_j + (index[j].sh.Misc.VirtualSize ? index[j].sh.Misc.VirtualSize : index[j].sh.SizeOfRawData);
                if (!(end_i <= va_j || end_j <= va_i)) {
                    uint64_t len_i = end_i - va_i;
                    uint64_t len_j = end_j - va_j;
                    if (len_j > len_i) {
                        index[i].keep = 0;
                        fprintf(stderr, "[overlap] drop '%.8s' at 0x%08" PRIx64 " (smaller)\n", index[i].sh.Name, va_i);
                    } else {
                        index[j].keep = 0;
                        fprintf(stderr, "[overlap] drop '%.8s' at 0x%08" PRIx64 " (smaller)\n", index[j].sh.Name, va_j);
                    }
                }
            }
        }

        /* Step 4: map remaining kept entries */
        for (int i = 0; i < idx_count; ++i) {
            if (!index[i].keep) continue;
            IMAGE_SECTION_HEADER* sh = &index[i].sh;
            uint32_t vaddr = sh->VirtualAddress;
            uint32_t rawsz = sh->SizeOfRawData;
            uint32_t rawptr = sh->PointerToRawData;
            uint32_t vsize = sh->Misc.VirtualSize;
            char name[9]; memcpy(name, sh->Name, 8); name[8] = 0;

            if (rawsz > 0) {
                if ((size_t)vaddr + rawsz > alloc_size) {
                    fprintf(stderr, "[skip] '%.8s' would overflow exec buffer (RVA 0x%08x raw 0x%08x)\n", name, vaddr, rawsz);
                    continue;
                }
                memcpy((uint8_t*)exec + vaddr, file + rawptr, rawsz);
            }
            if (vsize > rawsz) {
                size_t end = (size_t)vaddr + vsize;
                if (end <= alloc_size) memset((uint8_t*)exec + vaddr + rawsz, 0, vsize - rawsz);
            }
            fprintf(stderr, "[info] mapped section '%.8s' -> exec+0x%08x (raw=0x%08x vsize=0x%08x)\n",
                    name, vaddr, rawsz, vsize);
        }

        free(index);
    } else {
        /* No usable sections; copy whole file */
        if (fsz > alloc_size) { fprintf(stderr, "[fail] file larger than alloc_size\n"); free_executable(exec, alloc_size); if (sections) free(sections); free(file); return NULL; }
        memcpy(exec, file, fsz);
        fprintf(stderr, "[info] copied entire file into exec buffer (size=%zu)\n", fsz);
    }

    /* Entry dump */
    void* entry = (uint8_t*)exec + (size_t)entry_rva;
    fprintf(stderr, "[info] final entry RVA=0x%08" PRIx64 " -> exec@%p\n", entry_rva, entry);
    fprintf(stderr, "[debug] first 32 bytes at entry:\n");
    for (int i = 0; i < 32; ++i) {
        if (i % 16 == 0) fprintf(stderr, "  %04x: ", (unsigned)((size_t)entry_rva + i));
        fprintf(stderr, "%02x ", ((uint8_t*)entry)[i]);
        if (i % 16 == 15) fprintf(stderr, "\n");
    }
    if (32 % 16) fprintf(stderr, "\n");

    *entry_ptr = (void(*)(void*))entry;
    if (out_alloc_size) *out_alloc_size = alloc_size;
    if (out_entry_rva) *out_entry_rva = entry_rva;

    if (sections) free(sections);
    free(file);
    return exec;
}

/* ---------- minimal ConOut OutputString and host stubs (ms_abi) ---------- */

#if defined(__GNUC__) && defined(__x86_64__)
  #define EFIAPI __attribute__((ms_abi))
#else
  #define EFIAPI
#endif

static uint64_t EFIAPI host_OutputString(void* This, const uint16_t* str) {
    (void)This;
    if (!str) return 1;
    char buf[2048];
    size_t bi = 0;
    for (const uint16_t* p = str; *p != 0; ++p) {
        uint16_t ch = *p;
        if (ch < 0x80) {
            if (bi + 1 >= sizeof(buf)) break;
            buf[bi++] = (char)ch;
        } else {
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

/* Stub that the image may call via BootServices[0] (your code pattern calls *[rcx]) */
static void EFIAPI host_service_stub(void* bs_ptr, void* data_ptr) {
    (void)bs_ptr;
    fprintf(stderr, "[host_stub] called by EFI image. data_ptr=%p\n", data_ptr);
    fflush(stderr);
}

/* Build minimal SystemTable/BootServices/ConOut */
static int build_fake_system_table(void** out_st, void** out_bs, void** out_conout) {
    if (!out_st || !out_bs || !out_conout) return -1;
    size_t st_size = 0x200, bs_size = 0x200, conout_size = 0x100;
    uint8_t* st = calloc(1, st_size);
    uint8_t* bs = calloc(1, bs_size);
    uint8_t* conout = calloc(1, conout_size);
    if (!st || !bs || !conout) { free(st); free(bs); free(conout); return -1; }

    /* SystemTable->BootServices at +0x30 (per your efi_main usage) */
    uint64_t bs_ptr = (uint64_t)(uintptr_t)bs;
    memcpy(st + 0x30, &bs_ptr, sizeof(bs_ptr));

    /* SystemTable->ConOut variants (populate a few common offsets) */
    uint64_t conout_ptr = (uint64_t)(uintptr_t)conout;
    memcpy(st + 0x18, &conout_ptr, sizeof(conout_ptr));
    memcpy(st + 0x20, &conout_ptr, sizeof(conout_ptr));
    memcpy(st + 0x28, &conout_ptr, sizeof(conout_ptr));
    memcpy(st + 0x40, &conout_ptr, sizeof(conout_ptr));
    memcpy(st + 0x48, &conout_ptr, sizeof(conout_ptr));

    /* BootServices[0] = host_service_stub */
    uint64_t stub_ptr = (uint64_t)(uintptr_t)host_service_stub;
    memcpy(bs + 0x00, &stub_ptr, sizeof(stub_ptr));

    /* ConOut->OutputString = host_OutputString at offset 0 */
    uint64_t outstr_ptr = (uint64_t)(uintptr_t)host_OutputString;
    memcpy(conout + 0x00, &outstr_ptr, sizeof(outstr_ptr));

    *out_st = st; *out_bs = bs; *out_conout = conout;
    return 0;
}

/* Microsoft x64 ABI trampoline: RCX=ImageHandle, RDX=SystemTable, CALL entry */
static void* build_trampoline_msabi(void (*entry)(void), void* image_handle, void* system_table) {
    uint8_t* tramp = alloc_executable(4096);
    if (!tramp) return NULL;
    uint8_t* cp = tramp;

    /* mov rcx, imm64 -> 48 B9 imm64 */
    *cp++ = 0x48; *cp++ = 0xB9;
    uint64_t ih = (uint64_t)(uintptr_t)image_handle;
    memcpy(cp, &ih, 8); cp += 8;

    /* mov rdx, imm64 -> 48 BA imm64 */
    *cp++ = 0x48; *cp++ = 0xBA;
    uint64_t stp = (uint64_t)(uintptr_t)system_table;
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

/* ---------- main ---------- */

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <bootx64.efi>\n", argv[0]);
        return 1;
    }
    const char* path = argv[1];

    void (*entry_fn)(void) = NULL;
    size_t exec_size = 0;
    uint64_t entry_rva = 0;
    void* exec_buf = load_pe_to_executable_verbose(path, &entry_fn, &exec_size, &entry_rva);
    if (!exec_buf || !entry_fn) {
        fprintf(stderr, "[fatal] Failed to load EFI image '%s'. See messages above for details.\n", path);
        return 2;
    }
    fprintf(stderr, "[ok] loaded image exec@%p size=%zu entry_rva=0x%08" PRIx64 "\n", exec_buf, exec_size, entry_rva);

    void* st_buf = NULL; void* bs_buf = NULL; void* conout_buf = NULL;
    if (build_fake_system_table(&st_buf, &bs_buf, &conout_buf) != 0) {
        fprintf(stderr, "[fatal] Failed to build fake SystemTable/BootServices/ConOut\n");
        free_executable(exec_buf, exec_size);
        return 3;
    }
    fprintf(stderr, "[info] SystemTable=%p BootServices=%p ConOut=%p\n", st_buf, bs_buf, conout_buf);

    void* image_handle = exec_buf;
    void* tramp = build_trampoline_msabi(entry_fn, image_handle, st_buf);
    if (!tramp) {
        fprintf(stderr, "[fatal] Failed to build trampoline\n");
        free(st_buf); free(bs_buf); free(conout_buf);
        free_executable(exec_buf, exec_size);
        return 4;
    }

    fprintf(stderr, "[info] invoking entry via trampoline %p\n", tramp);
    typedef void (*TrampFn)(void);
    TrampFn tf = (TrampFn)tramp;

    /* Call the EFI image entry (ms_abi: RCX=ImageHandle, RDX=SystemTable) */
    tf();

    fprintf(stderr, "[info] returned from EFI image entry\n");

    free_executable(tramp, 4096);
    free(st_buf); free(bs_buf); free(conout_buf);
    free_executable(exec_buf, exec_size);

    return 0;
}
