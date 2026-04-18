/*
  ld.c
  Minimal ELF64 static linker for x86_64.

  Limitations:
   - ELF64 little-endian only
   - Supports R_X86_64_64 and R_X86_64_PC32 relocations
   - Merges .text, .data, .rodata, .bss
   - Produces a simple ELF executable (no dynamic linking)
*/

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* ELF definitions (minimal subset) */
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef int32_t  Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef int64_t  Elf64_Sxword;
typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;

#define EI_NIDENT 16

typedef struct {
    unsigned char e_ident[EI_NIDENT];
    Elf64_Half e_type;
    Elf64_Half e_machine;
    Elf64_Word e_version;
    Elf64_Addr e_entry;
    Elf64_Off  e_phoff;
    Elf64_Off  e_shoff;
    Elf64_Word e_flags;
    Elf64_Half e_ehsize;
    Elf64_Half e_phentsize;
    Elf64_Half e_phnum;
    Elf64_Half e_shentsize;
    Elf64_Half e_shnum;
    Elf64_Half e_shstrndx;
} Elf64_Ehdr;

typedef struct {
    Elf64_Word p_type;
    Elf64_Word p_flags;
    Elf64_Off  p_offset;
    Elf64_Addr p_vaddr;
    Elf64_Addr p_paddr;
    Elf64_Xword p_filesz;
    Elf64_Xword p_memsz;
    Elf64_Xword p_align;
} Elf64_Phdr;

typedef struct {
    Elf64_Word sh_name;
    Elf64_Word sh_type;
    Elf64_Xword sh_flags;
    Elf64_Addr sh_addr;
    Elf64_Off  sh_offset;
    Elf64_Xword sh_size;
    Elf64_Word sh_link;
    Elf64_Word sh_info;
    Elf64_Xword sh_addralign;
    Elf64_Xword sh_entsize;
} Elf64_Shdr;

typedef struct {
    Elf64_Word st_name;
    unsigned char st_info;
    unsigned char st_other;
    Elf64_Half st_shndx;
    Elf64_Addr st_value;
    Elf64_Xword st_size;
} Elf64_Sym;

typedef struct {
    Elf64_Addr r_offset;
    Elf64_Xword r_info;
    Elf64_Sxword r_addend; /* for RELA */
} Elf64_Rela;

#define ELFCLASS64 2
#define ELFDATA2LSB 1
#define EV_CURRENT 1
#define ET_EXEC 2
#define ET_REL 1
#define EM_X86_64 62

/* section types */
#define SHT_NULL     0
#define SHT_PROGBITS 1
#define SHT_SYMTAB   2
#define SHT_STRTAB   3
#define SHT_RELA     4
#define SHT_NOBITS   8

/* symbol bindings/types */
#define ELF64_ST_BIND(i) ((i)>>4)
#define ELF64_ST_TYPE(i) ((i)&0xf)
#define STB_LOCAL  0
#define STB_GLOBAL 1
#define STB_WEAK   2

/* relocation macros */
#define ELF64_R_SYM(i) ((i)>>32)
#define ELF64_R_TYPE(i) ((i)&0xffffffffUL)

/* relocation types we support */
#define R_X86_64_NONE 0
#define R_X86_64_64   1
#define R_X86_64_PC32 2

/* program header types */
#define PT_NULL 0
#define PT_LOAD 1

/* program header flags */
#define PF_X 1
#define PF_W 2
#define PF_R 4

/* helper read functions */
static ssize_t read_all(int fd, void *buf, size_t count, off_t offset) {
    if (lseek(fd, offset, SEEK_SET) < 0) return -1;
    size_t left = count;
    char *p = buf;
    while (left) {
        ssize_t r = read(fd, p, left);
        if (r < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (r == 0) break;
        left -= r;
        p += r;
    }
    return (ssize_t)(count - left);
}

/* simple dynamic arrays */
typedef struct {
    void **data;
    size_t size, cap;
} vecp;
static void vecp_init(vecp *v){v->data=NULL;v->size=0;v->cap=0;}
static void vecp_push(vecp *v, void *x){ if(v->size==v->cap){v->cap=v->cap?v->cap*2:8;v->data=realloc(v->data,v->cap*sizeof(void*));} v->data[v->size++]=x; }
static void vecp_free(vecp *v){ free(v->data); v->data=NULL; v->size=v->cap=0; }

/* string table */
typedef struct {
    char *data;
    size_t size;
} strtab;
static void strtab_init(strtab *s){ s->data=NULL; s->size=0; }
static void strtab_free(strtab *s){ free(s->data); s->data=NULL; s->size=0; }
static const char* strtab_get(const strtab *s, size_t off){ if(off>=s->size) return ""; return s->data+off; }

/* section container */
typedef struct {
    char *name;
    uint32_t type;
    uint64_t flags;
    uint64_t addralign;
    uint8_t *data; /* NULL for NOBITS */
    size_t size;
    size_t entsize;
    /* relocations (if any) */
    Elf64_Rela *relas;
    size_t rela_count;
} Section;

/* symbol entry */
typedef struct {
    char *name;
    uint8_t bind;
    uint8_t type;
    uint16_t shndx;
    uint64_t value;
    uint64_t size;
    /* resolved address in output */
    uint64_t out_addr;
    int defined; /* 1 if defined in inputs */
    Section *sec; /* section where defined */
} Symbol;

/* input object */
typedef struct {
    char *path;
    Section *sections;
    size_t section_count;
    Symbol *symbols;
    size_t symbol_count;
    strtab strtab;
} InputObj;

/* merged output sections */
typedef struct {
    char *name;
    uint8_t *data;
    size_t size;
    size_t capacity;
    uint64_t align;
    uint64_t flags;
    uint64_t vaddr; /* assigned virtual address */
} OutSection;

/* global arrays */
static InputObj *inputs = NULL;
static size_t input_count = 0;

static OutSection out_text = {0}, out_data = {0}, out_rodata = {0}, out_bss = {0};
static vecp all_symbols; /* Symbol* pointers for global table */

/* helpers */
static void *xmalloc(size_t n){ void *p = malloc(n); if(!p){ perror("malloc"); exit(1);} return p; }
static char *xstrdup(const char *s){ size_t n=strlen(s)+1; char *p=xmalloc(n); memcpy(p,s,n); return p; }

static void outsec_init(OutSection *s, const char *name, uint64_t align, uint64_t flags){
    s->name = xstrdup(name);
    s->data = NULL; s->size = s->capacity = 0; s->align = align; s->flags = flags; s->vaddr = 0;
}
static void outsec_append(OutSection *s, const void *data, size_t n){
    if(n==0) return;
    if(s->size + n > s->capacity){
        size_t newcap = s->capacity? s->capacity*2 : 4096;
        while(newcap < s->size + n) newcap *= 2;
        s->data = realloc(s->data, newcap);
        s->capacity = newcap;
    }
    memcpy(s->data + s->size, data, n);
    s->size += n;
}
static void outsec_ensure(OutSection *s, size_t n){
    if(s->size + n > s->capacity){
        size_t newcap = s->capacity? s->capacity*2 : 4096;
        while(newcap < s->size + n) newcap *= 2;
        s->data = realloc(s->data, newcap);
        s->capacity = newcap;
    }
    memset(s->data + s->size, 0, n);
    s->size += n;
}

/* read ELF object file and populate InputObj */
static int read_input(const char *path, InputObj *in) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) { perror(path); return -1; }

    Elf64_Ehdr eh;
    if (read_all(fd, &eh, sizeof(eh), 0) != sizeof(eh)) { close(fd); fprintf(stderr,"Failed to read ELF header\n"); return -1; }

    if (eh.e_ident[0] != 0x7f || eh.e_ident[1] != 'E' || eh.e_ident[2] != 'L' || eh.e_ident[3] != 'F') {
        close(fd); fprintf(stderr, "Not an ELF file: %s\n", path); return -1;
    }
    if (eh.e_ident[4] != ELFCLASS64 || eh.e_ident[5] != ELFDATA2LSB) {
        close(fd); fprintf(stderr, "Unsupported ELF class or data encoding\n"); return -1;
    }
    if (eh.e_type != ET_REL) {
        close(fd); fprintf(stderr, "Input is not relocatable object: %s\n", path); return -1;
    }
    if (eh.e_machine != EM_X86_64) {
        close(fd); fprintf(stderr, "Unsupported machine (not x86_64)\n"); return -1;
    }

    /* read section headers */
    if (eh.e_shoff == 0 || eh.e_shnum == 0) { close(fd); fprintf(stderr,"No sections\n"); return -1; }
    Elf64_Shdr *sh = xmalloc(eh.e_shentsize * eh.e_shnum);
    if (read_all(fd, sh, eh.e_shentsize * eh.e_shnum, eh.e_shoff) != (ssize_t)(eh.e_shentsize * eh.e_shnum)) { close(fd); fprintf(stderr,"Failed to read section headers\n"); return -1; }

    /* read section header string table */
    Elf64_Shdr shstr = sh[eh.e_shstrndx];
    char *shstrtab = xmalloc(shstr.sh_size + 1);
    if (read_all(fd, shstrtab, shstr.sh_size, shstr.sh_offset) != (ssize_t)shstr.sh_size) { close(fd); fprintf(stderr,"Failed to read shstrtab\n"); return -1; }
    shstrtab[shstr.sh_size] = 0;

    /* allocate sections array */
    in->sections = xmalloc(sizeof(Section) * eh.e_shnum);
    in->section_count = 0;
    for (int i = 0; i < eh.e_shnum; ++i) {
        const char *sname = shstrtab + sh[i].sh_name;
        Section sec = {0};
        sec.name = xstrdup(sname);
        sec.type = sh[i].sh_type;
        sec.flags = sh[i].sh_flags;
        sec.addralign = sh[i].sh_addralign;
        sec.entsize = sh[i].sh_entsize;
        sec.size = sh[i].sh_size;
        sec.data = NULL;
        sec.relas = NULL;
        sec.rela_count = 0;

        if (sh[i].sh_type == SHT_NOBITS) {
            sec.data = NULL;
        } else if (sh[i].sh_size > 0) {
            sec.data = xmalloc(sh[i].sh_size);
            if (read_all(fd, sec.data, sh[i].sh_size, sh[i].sh_offset) != (ssize_t)sh[i].sh_size) {
                close(fd); fprintf(stderr,"Failed to read section data\n"); return -1;
            }
        }

        in->sections[in->section_count++] = sec;
    }

    free(shstrtab);

    /* Re-read section headers to get sh_link for symtab/rela */
    {
        Elf64_Shdr *sh2 = xmalloc(eh.e_shentsize * eh.e_shnum);
        if (read_all(fd, sh2, eh.e_shentsize * eh.e_shnum, eh.e_shoff) != (ssize_t)(eh.e_shentsize * eh.e_shnum)) { close(fd); fprintf(stderr,"Failed to read section headers (2)\n"); return -1; }

        /* process symtab and rela */
        for (int i = 0; i < eh.e_shnum; ++i) {
            if (sh2[i].sh_type == SHT_SYMTAB) {
                /* read symbol table entries */
                size_t nsyms = sh2[i].sh_size / sh2[i].sh_entsize;
                Elf64_Sym *syms = xmalloc(sh2[i].sh_size);
                if (read_all(fd, syms, sh2[i].sh_size, sh2[i].sh_offset) != (ssize_t)sh2[i].sh_size) { close(fd); fprintf(stderr,"Failed to read symtab\n"); return -1; }
                /* read linked string table */
                Elf64_Shdr *strsh = &sh2[sh2[i].sh_link];
                char *strs = xmalloc(strsh->sh_size + 1);
                if (read_all(fd, strs, strsh->sh_size, strsh->sh_offset) != (ssize_t)strsh->sh_size) { close(fd); fprintf(stderr,"Failed to read symstr\n"); return -1; }
                strs[strsh->sh_size] = 0;

                in->symbols = xmalloc(sizeof(Symbol) * nsyms);
                in->symbol_count = nsyms;
                for (size_t k = 0; k < nsyms; ++k) {
                    Elf64_Sym *es = &syms[k];
                    Symbol *s = &in->symbols[k];
                    const char *nm = strs + es->st_name;
                    s->name = xstrdup(nm);
                    s->bind = ELF64_ST_BIND(es->st_info);
                    s->type = ELF64_ST_TYPE(es->st_info);
                    s->shndx = es->st_shndx;
                    s->value = es->st_value;
                    s->size = es->st_size;
                    s->out_addr = 0;
                    s->defined = (es->st_shndx != 0);
                    s->sec = NULL;
                }
                free(syms);
                free(strs);
            }
        }

        /* read relocation sections: SHT_RELA */
        /* We'll map relocation sections to target sections in a later pass using shstrtab */
        free(sh2);
    }

    /* For simplicity and robustness, re-open file and parse using a second pass that maps section headers to our Section array by comparing names via shstrtab */
    {
        Elf64_Shdr *sh3 = xmalloc(eh.e_shentsize * eh.e_shnum);
        if (read_all(fd, sh3, eh.e_shentsize * eh.e_shnum, eh.e_shoff) != (ssize_t)(eh.e_shentsize * eh.e_shnum)) { close(fd); fprintf(stderr,"Failed to read section headers (3)\n"); return -1; }
        Elf64_Shdr shstr3 = sh3[eh.e_shstrndx];
        char *shstrtab3 = xmalloc(shstr3.sh_size + 1);
        if (read_all(fd, shstrtab3, shstr3.sh_size, shstr3.sh_offset) != (ssize_t)shstr3.sh_size) { close(fd); fprintf(stderr,"Failed to read shstrtab3\n"); return -1; }
        shstrtab3[shstr3.sh_size] = 0;

        /* map section index -> Section* */
        Section **index_to_section = xmalloc(sizeof(Section*) * eh.e_shnum);
        for (int i = 0; i < eh.e_shnum; ++i) index_to_section[i] = NULL;
        for (size_t i = 0; i < in->section_count; ++i) {
            for (int j = 0; j < eh.e_shnum; ++j) {
                const char *nm = shstrtab3 + sh3[j].sh_name;
                if (strcmp(nm, in->sections[i].name) == 0) {
                    index_to_section[j] = &in->sections[i];
                    break;
                }
            }
        }

        /* now process relocation sections */
        for (int i = 0; i < eh.e_shnum; ++i) {
            if (sh3[i].sh_type == SHT_RELA) {
                Section *target = index_to_section[sh3[i].sh_info];
                if (!target) continue;
                size_t nrel = sh3[i].sh_size / sh3[i].sh_entsize;
                Elf64_Rela *relas = xmalloc(sizeof(Elf64_Rela) * nrel);
                if (read_all(fd, relas, sh3[i].sh_size, sh3[i].sh_offset) != (ssize_t)sh3[i].sh_size) { close(fd); fprintf(stderr,"Failed to read rela\n"); return -1; }
                target->relas = relas;
                target->rela_count = nrel;
            }
        }

        /* link symbols to sections */
        for (size_t si = 0; si < in->symbol_count; ++si) {
            Symbol *s = &in->symbols[si];
            if (s->shndx < (uint16_t)eh.e_shnum && s->shndx != 0) {
                Section *sec = index_to_section[s->shndx];
                s->sec = sec;
            } else {
                s->sec = NULL;
            }
        }

        free(index_to_section);
        free(shstrtab3);
        free(sh3);
    }

    close(fd);
    in->path = xstrdup(path);
    return 0;
}

/* merge input sections into output sections */
static void merge_sections() {
    outsec_init(&out_text, ".text", 16, PF_R | PF_X);
    outsec_init(&out_rodata, ".rodata", 8, PF_R);
    outsec_init(&out_data, ".data", 8, PF_R | PF_W);
    outsec_init(&out_bss, ".bss", 8, PF_R | PF_W);

    for (size_t i = 0; i < input_count; ++i) {
        InputObj *in = &inputs[i];
        for (size_t j = 0; j < in->section_count; ++j) {
            Section *s = &in->sections[j];
            if (strcmp(s->name, ".text") == 0 && s->data) {
                outsec_append(&out_text, s->data, s->size);
            } else if (strcmp(s->name, ".rodata") == 0 && s->data) {
                outsec_append(&out_rodata, s->data, s->size);
            } else if (strcmp(s->name, ".data") == 0 && s->data) {
                outsec_append(&out_data, s->data, s->size);
            } else if (strcmp(s->name, ".bss") == 0 || s->type == SHT_NOBITS) {
                /* bss: just increase size */
                outsec_ensure(&out_bss, s->size);
                /* zeroed by ensure */
            } else {
                /* ignore other sections for now */
            }
        }
    }
}

/* build global symbol table */
static void build_symbol_table() {
    vecp_init(&all_symbols);
    /* collect all symbols */
    for (size_t i = 0; i < input_count; ++i) {
        InputObj *in = &inputs[i];
        for (size_t j = 0; j < in->symbol_count; ++j) {
            Symbol *s = &in->symbols[j];
            /* skip empty names */
            if (!s->name || s->name[0] == '\0') continue;
            /* create a copy for global table */
            Symbol *gs = xmalloc(sizeof(Symbol));
            *gs = *s;
            gs->name = xstrdup(s->name);
            gs->out_addr = 0;
            gs->sec = s->sec;
            vecp_push(&all_symbols, gs);
        }
    }

    /* resolve duplicates: prefer defined global over undefined */
    for (size_t i = 0; i < all_symbols.size; ++i) {
        Symbol *si = (Symbol*)all_symbols.data[i];
        for (size_t j = i+1; j < all_symbols.size; ++j) {
            Symbol *sj = (Symbol*)all_symbols.data[j];
            if (strcmp(si->name, sj->name) == 0) {
                /* if one is defined and other not, keep defined; if both defined, keep first and ignore second (could be error) */
                if (si->defined && !sj->defined) {
                    free(sj->name);
                    free(sj);
                    for (size_t k = j+1; k < all_symbols.size; ++k) all_symbols.data[k-1] = all_symbols.data[k];
                    all_symbols.size--;
                    j--;
                } else if (!si->defined && sj->defined) {
                    free(si->name);
                    *si = *sj;
                    si->name = xstrdup(sj->name);
                    free(sj->name);
                    free(sj);
                    for (size_t k = j+1; k < all_symbols.size; ++k) all_symbols.data[k-1] = all_symbols.data[k];
                    all_symbols.size--;
                    j--;
                } else {
                    free(sj->name);
                    free(sj);
                    for (size_t k = j+1; k < all_symbols.size; ++k) all_symbols.data[k-1] = all_symbols.data[k];
                    all_symbols.size--;
                    j--;
                }
            }
        }
    }
}

/* align up helper (C, not C++ lambda) */
static inline uint64_t align_up_uint64(uint64_t a, uint64_t align)
{
    if (align == 0) return a;
    uint64_t m = (a + align - 1) & ~(align - 1);
    return m;
}

/* assign addresses to merged sections */
static void assign_addresses(uint64_t base_vaddr) {
    /* simple layout: text at base_vaddr, rodata after text, data after rodata, bss after data */
    uint64_t addr = base_vaddr;

    addr = align_up_uint64(addr, out_text.align);
    out_text.vaddr = addr;
    addr += out_text.size;

    addr = align_up_uint64(addr, out_rodata.align);
    out_rodata.vaddr = addr;
    addr += out_rodata.size;

    addr = align_up_uint64(addr, out_data.align);
    out_data.vaddr = addr;
    addr += out_data.size;

    addr = align_up_uint64(addr, out_bss.align);
    out_bss.vaddr = addr;
    addr += out_bss.size;

    /* assign symbol addresses for defined symbols */
    for (size_t i = 0; i < all_symbols.size; ++i) {
        Symbol *s = (Symbol*)all_symbols.data[i];
        if (!s->defined) continue;
        if (!s->sec) {
            /* absolute or special */
            s->out_addr = s->value;
            continue;
        }
        if (strcmp(s->sec->name, ".text") == 0) {
            s->out_addr = out_text.vaddr + s->value;
        } else if (strcmp(s->sec->name, ".rodata") == 0) {
            s->out_addr = out_rodata.vaddr + s->value;
        } else if (strcmp(s->sec->name, ".data") == 0) {
            s->out_addr = out_data.vaddr + s->value;
        } else if (s->sec->type == SHT_NOBITS || strcmp(s->sec->name, ".bss") == 0) {
            s->out_addr = out_bss.vaddr + s->value;
        } else {
            /* other sections: treat as data appended to data */
            s->out_addr = out_data.vaddr + s->value;
        }
    }
}

/* find symbol by name in global table */
static Symbol* find_symbol(const char *name) {
    for (size_t i = 0; i < all_symbols.size; ++i) {
        Symbol *s = (Symbol*)all_symbols.data[i];
        if (strcmp(s->name, name) == 0) return s;
    }
    return NULL;
}

/* apply relocations: iterate inputs and their sections */
static int apply_relocations() {
    for (size_t i = 0; i < input_count; ++i) {
        InputObj *in = &inputs[i];
        for (size_t j = 0; j < in->section_count; ++j) {
            Section *s = &in->sections[j];
            if (!s->relas || s->rela_count == 0) continue;
            /* determine where this section was placed in output */
            uint64_t sec_base_out = 0;
            if (strcmp(s->name, ".text") == 0) sec_base_out = out_text.vaddr;
            else if (strcmp(s->name, ".rodata") == 0) sec_base_out = out_rodata.vaddr;
            else if (strcmp(s->name, ".data") == 0) sec_base_out = out_data.vaddr;
            else if (s->type == SHT_NOBITS || strcmp(s->name, ".bss") == 0) sec_base_out = out_bss.vaddr;
            else continue; /* unsupported section */

            /* find where the section's contents are in merged output to patch */
            uint8_t *out_ptr = NULL;
            size_t out_offset = 0;
            if (strcmp(s->name, ".text") == 0) {
                size_t offset = 0;
                for (size_t ii = 0; ii < i; ++ii) {
                    for (size_t jj = 0; jj < inputs[ii].section_count; ++jj) {
                        Section *ss = &inputs[ii].sections[jj];
                        if (strcmp(ss->name, ".text") == 0 && ss->data) offset += ss->size;
                    }
                }
                out_ptr = out_text.data + offset;
                out_offset = offset;
            } else if (strcmp(s->name, ".rodata") == 0) {
                size_t offset = 0;
                for (size_t ii = 0; ii < i; ++ii) {
                    for (size_t jj = 0; jj < inputs[ii].section_count; ++jj) {
                        Section *ss = &inputs[ii].sections[jj];
                        if (strcmp(ss->name, ".rodata") == 0 && ss->data) offset += ss->size;
                    }
                }
                out_ptr = out_rodata.data + offset;
                out_offset = offset;
            } else if (strcmp(s->name, ".data") == 0) {
                size_t offset = 0;
                for (size_t ii = 0; ii < i; ++ii) {
                    for (size_t jj = 0; jj < inputs[ii].section_count; ++jj) {
                        Section *ss = &inputs[ii].sections[jj];
                        if (strcmp(ss->name, ".data") == 0 && ss->data) offset += ss->size;
                    }
                }
                out_ptr = out_data.data + offset;
                out_offset = offset;
            } else if (s->type == SHT_NOBITS || strcmp(s->name, ".bss") == 0) {
                size_t offset = 0;
                for (size_t ii = 0; ii < i; ++ii) {
                    for (size_t jj = 0; jj < inputs[ii].section_count; ++jj) {
                        Section *ss = &inputs[ii].sections[jj];
                        if (ss->type == SHT_NOBITS || strcmp(ss->name, ".bss") == 0) offset += ss->size;
                    }
                }
                out_ptr = out_bss.data + offset;
                out_offset = offset;
            }

            for (size_t r = 0; r < s->rela_count; ++r) {
                Elf64_Rela *rela = &s->relas[r];
                uint64_t rtype = ELF64_R_TYPE(rela->r_info);
                uint64_t rsym = ELF64_R_SYM(rela->r_info);
                if (rsym >= in->symbol_count) { fprintf(stderr,"Invalid relocation symbol index\n"); return -1; }
                Symbol *sym = &in->symbols[rsym];
                Symbol *gs = find_symbol(sym->name);
                uint64_t S = 0;
                if (gs && gs->defined) S = gs->out_addr;
                else {
                    fprintf(stderr,"Undefined symbol in relocation: %s\n", sym->name);
                    return -1;
                }
                uint64_t P = sec_base_out + rela->r_offset; /* place being relocated */
                uint64_t A = (uint64_t)rela->r_addend;
                if (rtype == R_X86_64_64) {
                    uint64_t val = S + A;
                    uint8_t *loc = out_ptr + rela->r_offset;
                    memcpy(loc, &val, 8);
                } else if (rtype == R_X86_64_PC32) {
                    int64_t val64 = (int64_t)S + (int64_t)A - (int64_t)P;
                    int32_t val32 = (int32_t)val64;
                    if ((int64_t)val32 != val64) { fprintf(stderr,"PC32 relocation overflow for symbol %s\n", sym->name); return -1; }
                    uint8_t *loc = out_ptr + rela->r_offset;
                    memcpy(loc, &val32, 4);
                } else {
                    fprintf(stderr,"Unsupported relocation type: %" PRIu64 "\n", rtype);
                    return -1;
                }
            }
        }
    }
    return 0;
}

/* write ELF executable */
static int write_executable(const char *outpath, uint64_t entry) {
    int fd = open(outpath, O_CREAT | O_TRUNC | O_WRONLY, 0755);
    if (fd < 0) { perror("open output"); return -1; }

    /* compute file layout: place ELF header + program headers at start, then segments */
    size_t phnum = 2;
    size_t ehdr_size = sizeof(Elf64_Ehdr);
    size_t phdrs_size = phnum * sizeof(Elf64_Phdr);
    size_t header_size = ((ehdr_size + phdrs_size + 0x1000 - 1) / 0x1000) * 0x1000; /* align to page */

    /* file offsets */
    size_t text_offset = header_size;
    size_t rodata_offset = text_offset + out_text.size;
    size_t data_offset = rodata_offset + out_rodata.size;
    size_t bss_offset = data_offset + out_data.size; /* bss not stored in file, memsz > filesz */

    /* virtual addresses: choose base 0x400000 */
    uint64_t base_vaddr = 0x400000;
    assign_addresses(base_vaddr + header_size); /* ensure vaddr accounts for header size */

    /* build ELF header */
    Elf64_Ehdr eh;
    memset(&eh, 0, sizeof(eh));
    eh.e_ident[0] = 0x7f; eh.e_ident[1] = 'E'; eh.e_ident[2] = 'L'; eh.e_ident[3] = 'F';
    eh.e_ident[4] = ELFCLASS64;
    eh.e_ident[5] = ELFDATA2LSB;
    eh.e_ident[6] = EV_CURRENT;
    eh.e_type = ET_EXEC;
    eh.e_machine = EM_X86_64;
    eh.e_version = EV_CURRENT;
    eh.e_entry = entry ? entry : out_text.vaddr;
    eh.e_phoff = sizeof(Elf64_Ehdr);
    eh.e_ehsize = sizeof(Elf64_Ehdr);
    eh.e_phentsize = sizeof(Elf64_Phdr);
    eh.e_phnum = phnum;

    /* program headers */
    Elf64_Phdr ph_text;
    memset(&ph_text, 0, sizeof(ph_text));
    ph_text.p_type = PT_LOAD;
    ph_text.p_offset = text_offset;
    ph_text.p_vaddr = out_text.vaddr;
    ph_text.p_paddr = out_text.vaddr;
    ph_text.p_filesz = out_text.size;
    ph_text.p_memsz = out_text.size;
    ph_text.p_flags = PF_R | PF_X;
    ph_text.p_align = 0x1000;

    Elf64_Phdr ph_data;
    memset(&ph_data, 0, sizeof(ph_data));
    ph_data.p_type = PT_LOAD;
    ph_data.p_offset = data_offset;
    ph_data.p_vaddr = out_data.vaddr;
    ph_data.p_paddr = out_data.vaddr;
    ph_data.p_filesz = out_data.size + out_rodata.size;
    ph_data.p_memsz = out_data.size + out_rodata.size + out_bss.size;
    ph_data.p_flags = PF_R | PF_W;
    ph_data.p_align = 0x1000;

    /* write header and phdrs */
    if (pwrite(fd, &eh, sizeof(eh), 0) != sizeof(eh)) { perror("write eh"); close(fd); return -1; }
    if (pwrite(fd, &ph_text, sizeof(ph_text), eh.e_phoff) != sizeof(ph_text)) { perror("write ph1"); close(fd); return -1; }
    if (pwrite(fd, &ph_data, sizeof(ph_data), eh.e_phoff + sizeof(ph_text)) != sizeof(ph_data)) { perror("write ph2"); close(fd); return -1; }

    /* pad to header_size */
    off_t cur = lseek(fd, 0, SEEK_CUR);
    if (cur < 0) { perror("lseek"); close(fd); return -1; }
    if ((size_t)cur < header_size) {
        size_t pad = header_size - (size_t)cur;
        void *zeros = calloc(1, pad);
        if (pwrite(fd, zeros, pad, cur) != (ssize_t)pad) { perror("pad"); free(zeros); close(fd); return -1; }
        free(zeros);
    }

    /* write .text */
    if (out_text.size > 0) {
        if (pwrite(fd, out_text.data, out_text.size, text_offset) != (ssize_t)out_text.size) { perror("write text"); close(fd); return -1; }
    }
    /* write .rodata then .data into data segment file area */
    if (out_rodata.size > 0) {
        if (pwrite(fd, out_rodata.data, out_rodata.size, rodata_offset) != (ssize_t)out_rodata.size) { perror("write rodata"); close(fd); return -1; }
    }
    if (out_data.size > 0) {
        if (pwrite(fd, out_data.data, out_data.size, data_offset) != (ssize_t)out_data.size) { perror("write data"); close(fd); return -1; }
    }

    close(fd);
    return 0;
}

/* free resources */
static void cleanup() {
    for (size_t i = 0; i < input_count; ++i) {
        InputObj *in = &inputs[i];
        for (size_t j = 0; j < in->section_count; ++j) {
            free(in->sections[j].name);
            free(in->sections[j].data);
            free(in->sections[j].relas);
        }
        free(in->sections);
        for (size_t k = 0; k < in->symbol_count; ++k) {
            free(in->symbols[k].name);
        }
        free(in->symbols);
        strtab_free(&in->strtab);
        free(in->path);
    }
    free(inputs);
    free(out_text.name); free(out_text.data);
    free(out_rodata.name); free(out_rodata.data);
    free(out_data.name); free(out_data.data);
    free(out_bss.name); free(out_bss.data);
    for (size_t i = 0; i < all_symbols.size; ++i) {
        Symbol *s = (Symbol*)all_symbols.data[i];
        free(s->name);
        free(s);
    }
    vecp_free(&all_symbols);
}

/* main: parse args, read inputs, link */
int ld(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s -o output input1.o input2.o ...\n", argv[0]);
        return 1;
    }
    const char *outpath = NULL;
    int argi = 1;
    while (argi < argc) {
        if (strcmp(argv[argi], "-o") == 0) {
            argi++;
            if (argi >= argc) { fprintf(stderr,"Missing -o argument\n"); return 1; }
            outpath = argv[argi++];
        } else break;
    }
    if (!outpath) { fprintf(stderr,"Missing -o output\n"); return 1; }
    if (argi >= argc) { fprintf(stderr,"No input files\n"); return 1; }

    input_count = argc - argi;
    inputs = xmalloc(sizeof(InputObj) * input_count);
    memset(inputs, 0, sizeof(InputObj) * input_count);

    for (size_t i = 0; i < input_count; ++i) {
        if (read_input(argv[argi + i], &inputs[i]) != 0) {
            fprintf(stderr,"Failed to read input %s\n", argv[argi + i]);
            cleanup();
            return 1;
        }
    }

    merge_sections();
    build_symbol_table();

    /* choose entry: symbol "main" if present */
    Symbol *main_sym = find_symbol("main");
    uint64_t entry = 0;
    if (main_sym && main_sym->defined) entry = main_sym->out_addr;

    /* apply relocations */
    if (apply_relocations() != 0) {
        fprintf(stderr,"Relocation failed\n");
        cleanup();
        return 1;
    }

    if (write_executable(outpath, entry) != 0) {
        fprintf(stderr,"Failed to write executable\n");
        cleanup();
        return 1;
    }

    printf("Linked %zu objects -> %s\n", input_count, outpath);
    cleanup();
    return 0;
}

