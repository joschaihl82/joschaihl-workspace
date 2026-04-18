/*
  ld.c - Professional Minimal ELF64 Linker (expanded, verbose, doubled size)
  Supports: .o and .a (Static Archives)
  Relocations: R_X86_64_64, R_X86_64_PC32, R_X86_64_PLT32
  Features: Symtab, Strtab, Section Header Table, Ar-Parsing, verbose mode,
            section header emission, simple symbol table emission, cleanup,
            additional safety checks, and more helper utilities.

  This file is intentionally expanded with additional helpers, comments,
  diagnostics, and optional features to make the codebase larger and
  easier to extend. It remains a minimal educational linker and omits
  many production features (dynamic linking, full relocation set, PLT/GOT).
*/

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h>
#include <inttypes.h>

/* ELF64 Types */
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint64_t Elf64_Xword;
typedef int64_t  Elf64_Sxword;

#define EI_NIDENT 16
#define ELF64_ST_BIND(i) ((i)>>4)
#define ELF64_ST_TYPE(i) ((i)&0xf)
#define ELF64_ST_INFO(b,t) (((b)<<4)+((t)&0xf))
#define ELF64_R_SYM(i) ((uint32_t)((i)>>32))
#define ELF64_R_TYPE(i) ((uint32_t)((i)&0xffffffffUL))

typedef struct {
    unsigned char e_ident[EI_NIDENT];
    Elf64_Half e_type, e_machine; Elf64_Word e_version;
    Elf64_Addr e_entry; Elf64_Off e_phoff, e_shoff;
    Elf64_Word e_flags; Elf64_Half e_ehsize, e_phentsize, e_phnum, e_shentsize, e_shnum, e_shstrndx;
} Elf64_Ehdr;

typedef struct {
    Elf64_Word p_type, p_flags; Elf64_Off p_offset;
    Elf64_Addr p_vaddr, p_paddr; Elf64_Xword p_filesz, p_memsz, p_align;
} Elf64_Phdr;

typedef struct {
    Elf64_Word sh_name, sh_type; Elf64_Xword sh_flags;
    Elf64_Addr sh_addr; Elf64_Off sh_offset; Elf64_Xword sh_size;
    Elf64_Word sh_link, sh_info; Elf64_Xword sh_addralign, sh_entsize;
} Elf64_Shdr;

typedef struct {
    Elf64_Word st_name; unsigned char st_info, st_other;
    Elf64_Half st_shndx; Elf64_Addr st_value; Elf64_Xword st_size;
} Elf64_Sym;

typedef struct { Elf64_Addr r_offset; Elf64_Xword r_info; Elf64_Sxword r_addend; } Elf64_Rela;

/* Constants */
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_NOBITS 8
#define STB_GLOBAL 1
#define STT_FUNC 2
#define R_X86_64_64 1
#define R_X86_64_PC32 2
#define R_X86_64_PLT32 4

/* Linker Structures */
typedef struct {
    char *name; uint32_t type; uint8_t *data; size_t size;
    Elf64_Rela *relas; size_t rela_count; size_t out_offset;
} Section;

typedef struct {
    char *name; uint8_t bind; uint16_t shndx; uint64_t value; uint64_t out_addr;
    int defined; Section *sec;
} Symbol;

typedef struct {
    Section *sections; size_t section_count;
    Symbol *symbols; size_t symbol_count;
} InputObj;

typedef struct {
    char *name; uint8_t *data; size_t size; uint64_t vaddr;
} OutSection;

/* Globals */
static OutSection o_text = { .name = ".text", .data = NULL, .size = 0, .vaddr = 0 };
static OutSection o_rodata = { .name = ".rodata", .data = NULL, .size = 0, .vaddr = 0 };
static OutSection o_data = { .name = ".data", .data = NULL, .size = 0, .vaddr = 0 };
static OutSection o_bss = { .name = ".bss", .data = NULL, .size = 0, .vaddr = 0 };

static InputObj *objs = NULL; static size_t obj_count = 0, obj_cap = 0;
static Symbol **globals = NULL; static size_t global_count = 0, global_cap = 0;

/* Config */
static int verbose = 0;
static int emit_shdr = 1; /* emit section headers in output ELF */
static int emit_symtab = 1; /* emit a simple symbol table in output ELF */

/* Utility: verbose printing */
static void vprintf_dbg(const char *fmt, ...) {
    if (!verbose) return;
    va_list ap; va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

/* Safe alloc wrappers */
static void *xmalloc(size_t n) {
    void *p = calloc(1, n);
    if (!p) { perror("calloc"); exit(1); }
    return p;
}
static void *xrealloc(void *p, size_t n) {
    void *q = realloc(p, n);
    if (!q) { perror("realloc"); exit(1); }
    return q;
}

/* IO helpers */
static ssize_t read_exact(int fd, void *buf, size_t len) {
    size_t off = 0;
    while (off < len) {
        ssize_t r = read(fd, (char*)buf + off, len - off);
        if (r < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (r == 0) break;
        off += (size_t)r;
    }
    return (ssize_t)off;
}
static off_t lseek_checked(int fd, off_t off, int whence) {
    off_t r = lseek(fd, off, whence);
    if (r == (off_t)-1) { perror("lseek"); exit(1); }
    return r;
}

/* Symbol table helpers */
static void add_global(Symbol *s) {
    if (!s || !s->name) return;
    for (size_t i = 0; i < global_count; i++) {
        if (strcmp(globals[i]->name, s->name) == 0) {
            if (s->defined && !globals[i]->defined) globals[i] = s;
            return;
        }
    }
    if (global_count == global_cap) {
        global_cap = global_cap ? global_cap * 2 : 16;
        globals = xrealloc(globals, global_cap * sizeof(Symbol*));
    }
    globals[global_count++] = s;
}
static Symbol* find_global(const char *name) {
    if (!name) return NULL;
    for (size_t i = 0; i < global_count; i++) if (!strcmp(globals[i]->name, name)) return globals[i];
    return NULL;
}

/* Append section bytes into output section */
static void append_sec(OutSection *o, Section *s) {
    if (!o || !s) return;
    s->out_offset = o->size;
    if (s->type != SHT_NOBITS && s->size > 0) {
        o->data = xrealloc(o->data, o->size + s->size);
        memcpy(o->data + o->size, s->data, s->size);
    }
    o->size += s->size;
    vprintf_dbg("Appended section %s size=%zu to %s (new size=%zu)\n", s->name ? s->name : "<anon>", s->size, o->name, o->size);
}

/* Load an ELF object file at fd:offset */
static int load_obj(int fd, off_t offset) {
    Elf64_Ehdr eh;
    lseek_checked(fd, offset, SEEK_SET);
    if (read_exact(fd, &eh, sizeof(eh)) != (ssize_t)sizeof(eh)) {
        vprintf_dbg("Failed to read ELF header at offset %" PRIu64 "\n", (uint64_t)offset);
        return -1;
    }
    /* Basic sanity */
    if (eh.e_shentsize == 0 || eh.e_shnum == 0) {
        vprintf_dbg("Object has no section headers\n");
        return -1;
    }
    size_t sh_table_bytes = (size_t)eh.e_shentsize * eh.e_shnum;
    Elf64_Shdr *sh = xmalloc(sh_table_bytes);
    lseek_checked(fd, offset + eh.e_shoff, SEEK_SET);
    if (read_exact(fd, sh, sh_table_bytes) != (ssize_t)sh_table_bytes) { free(sh); return -1; }

    if (eh.e_shstrndx >= eh.e_shnum) { free(sh); return -1; }
    Elf64_Shdr *shstr = &sh[eh.e_shstrndx];
    char *sn = xmalloc((size_t)shstr->sh_size + 1);
    lseek_checked(fd, offset + shstr->sh_offset, SEEK_SET);
    if (read_exact(fd, sn, (size_t)shstr->sh_size) != (ssize_t)shstr->sh_size) { free(sn); free(sh); return -1; }
    sn[shstr->sh_size] = '\0';

    if (obj_count == obj_cap) {
        obj_cap = obj_cap ? obj_cap * 2 : 16;
        objs = xrealloc(objs, obj_cap * sizeof(InputObj));
    }
    InputObj *in = &objs[obj_count++];
    in->sections = xmalloc(sizeof(Section) * eh.e_shnum);
    in->section_count = eh.e_shnum;
    in->symbols = NULL; in->symbol_count = 0;

    for (int i = 0; i < eh.e_shnum; i++) {
        Section *sec = &in->sections[i];
        memset(sec, 0, sizeof(*sec));
        const char *name = (sh[i].sh_name < shstr->sh_size) ? (sn + sh[i].sh_name) : "";
        sec->name = strdup(name);
        sec->type = sh[i].sh_type;
        sec->size = (size_t)sh[i].sh_size;
        sec->relas = NULL; sec->rela_count = 0; sec->out_offset = 0;
        if (sh[i].sh_type != SHT_NOBITS && sh[i].sh_size > 0) {
            sec->data = xmalloc((size_t)sh[i].sh_size);
            lseek_checked(fd, offset + sh[i].sh_offset, SEEK_SET);
            if (read_exact(fd, sec->data, (size_t)sh[i].sh_size) != (ssize_t)sh[i].sh_size) {
                free(sec->data); sec->data = NULL; sec->size = 0;
            }
        } else {
            sec->data = NULL;
        }
    }

    /* Parse symbol tables and relocations */
    for (int i = 0; i < eh.e_shnum; i++) {
        if (sh[i].sh_type == SHT_SYMTAB) {
            size_t n = (size_t)(sh[i].sh_size / sh[i].sh_entsize);
            if (n == 0) continue;
            Elf64_Sym *es = xmalloc((size_t)sh[i].sh_size);
            lseek_checked(fd, offset + sh[i].sh_offset, SEEK_SET);
            if (read_exact(fd, es, (size_t)sh[i].sh_size) != (ssize_t)sh[i].sh_size) { free(es); continue; }

            Elf64_Shdr *strsh = &sh[sh[i].sh_link];
            char *st = xmalloc((size_t)strsh->sh_size + 1);
            lseek_checked(fd, offset + strsh->sh_offset, SEEK_SET);
            if (read_exact(fd, st, (size_t)strsh->sh_size) != (ssize_t)strsh->sh_size) { free(st); free(es); continue; }
            st[strsh->sh_size] = '\0';

            in->symbols = xmalloc(sizeof(Symbol) * n);
            in->symbol_count = n;
            for (size_t k = 0; k < n; k++) {
                Symbol *sym = &in->symbols[k];
                memset(sym, 0, sizeof(*sym));
                const char *sname = (es[k].st_name < strsh->sh_size) ? (st + es[k].st_name) : "";
                sym->name = strdup(sname);
                sym->bind = ELF64_ST_BIND(es[k].st_info);
                sym->shndx = es[k].st_shndx;
                sym->value = es[k].st_value;
                sym->defined = (es[k].st_shndx != 0 && es[k].st_shndx < 0xff00);
                sym->sec = NULL;
                if (sym->defined && sym->shndx < in->section_count) sym->sec = &in->sections[sym->shndx];
                if (sym->bind == STB_GLOBAL) add_global(sym);
            }
            free(es); free(st);
        }
        if (sh[i].sh_type == SHT_RELA) {
            if (sh[i].sh_info >= (Elf64_Word)in->section_count) continue;
            Section *tgt = &in->sections[sh[i].sh_info];
            size_t nrel = (size_t)(sh[i].sh_size / sh[i].sh_entsize);
            if (nrel == 0) continue;
            tgt->rela_count = nrel;
            tgt->relas = xmalloc((size_t)sh[i].sh_size);
            lseek_checked(fd, offset + sh[i].sh_offset, SEEK_SET);
            if (read_exact(fd, tgt->relas, (size_t)sh[i].sh_size) != (ssize_t)sh[i].sh_size) {
                free(tgt->relas); tgt->relas = NULL; tgt->rela_count = 0;
            }
        }
    }

    free(sn); free(sh);
    vprintf_dbg("Loaded object at offset %" PRIu64 " with %zu sections\n", (uint64_t)offset, in->section_count);
    return 0;
}

/* Minimal ar (.a) archive loader */
static void load_archive(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) { perror(path); return; }
    char magic[8];
    if (read_exact(fd, magic, 8) != 8) { close(fd); return; }
    if (memcmp(magic, "!<arch>\n", 8) != 0) { close(fd); return; }

    while (1) {
        struct ar_hdr {
            char name[16];
            char date[12];
            char uid[6];
            char gid[6];
            char mode[8];
            char size[10];
            char fmag[2];
        } hdr;
        ssize_t got = read_exact(fd, &hdr, sizeof(hdr));
        if (got == 0) break;
        if (got != (ssize_t)sizeof(hdr)) break;
        char tmp[32]; memcpy(tmp, hdr.size, 10); tmp[10] = '\0';
        size_t sz = (size_t)atol(tmp);
        off_t start = lseek_checked(fd, 0, SEEK_CUR);
        unsigned char peek[4];
        if (sz >= 4) {
            if (lseek(fd, start, SEEK_SET) == (off_t)-1) break;
            if (read_exact(fd, peek, 4) == 4 && peek[0] == 0x7f && peek[1] == 'E' && peek[2] == 'L' && peek[3] == 'F') {
                load_obj(fd, start);
            }
        }
        off_t next = start + (off_t)sz;
        if (sz & 1) next++;
        lseek_checked(fd, next, SEEK_SET);
    }
    close(fd);
}

/* Emit a simple section header table and symbol table into the output ELF.
   This is optional and simplified: it creates a minimal shstrtab, shdrs,
   symtab and strtab and appends them to the file. */
static int emit_section_and_symtabs(const char *outpath, int fd, uint64_t file_end, uint64_t base_vaddr) {
    if (!emit_shdr && !emit_symtab) return 0;

    /* Build shstrtab */
    /* We'll create names for: null, .text, .rodata, .data, .bss, .shstrtab, .symtab, .strtab */
    const char *names[] = {"", ".text", ".rodata", ".data", ".bss", ".shstrtab", ".symtab", ".strtab"};
    size_t n_names = sizeof(names)/sizeof(names[0]);
    /* compute shstrtab content */
    size_t shstr_size = 1; /* initial null */
    for (size_t i = 1; i < n_names; i++) shstr_size += strlen(names[i]) + 1;
    char *shstr = xmalloc(shstr_size);
    size_t off = 0; shstr[off++] = '\0';
    for (size_t i = 1; i < n_names; i++) { strcpy(shstr + off, names[i]); off += strlen(names[i]) + 1; }

    /* Build simple strtab for symbol names */
    /* Count globals to emit */
    size_t emit_sym_count = 0;
    for (size_t i = 0; i < global_count; i++) if (globals[i] && globals[i]->defined) emit_sym_count++;
    /* symtab entries include a leading null symbol */
    size_t symtab_entries = emit_sym_count + 1;
    /* Build strtab content */
    size_t strtab_size = 1;
    for (size_t i = 0; i < global_count; i++) if (globals[i] && globals[i]->defined) strtab_size += strlen(globals[i]->name) + 1;
    char *strtab = xmalloc(strtab_size);
    size_t str_off = 0; strtab[str_off++] = '\0';
    for (size_t i = 0; i < global_count; i++) {
        if (globals[i] && globals[i]->defined) {
            strcpy(strtab + str_off, globals[i]->name);
            str_off += strlen(globals[i]->name) + 1;
        }
    }

    /* Prepare symtab entries */
    Elf64_Sym *symtab = xmalloc(sizeof(Elf64_Sym) * symtab_entries);
    memset(symtab, 0, sizeof(Elf64_Sym) * symtab_entries);
    /* entry 0 is all zeros */
    size_t idx = 1;
    for (size_t i = 0; i < global_count; i++) {
        if (globals[i] && globals[i]->defined) {
            symtab[idx].st_name = (Elf64_Word)0; /* we'll compute offsets below */
            symtab[idx].st_info = (unsigned char)((STB_GLOBAL<<4) | (STT_FUNC & 0xf));
            symtab[idx].st_other = 0;
            symtab[idx].st_shndx = globals[i]->sec ? (Elf64_Half)globals[i]->sec->out_offset : 0;
            symtab[idx].st_value = globals[i]->out_addr;
            symtab[idx].st_size = 0;
            idx++;
        }
    }

    /* Now compute file layout: append shstrtab, strtab, symtab, then section headers */
    uint64_t cur = file_end;
    /* align to 8 */
    if (cur & 7) cur = (cur + 7) & ~7ULL;

    uint64_t shstr_off = cur;
    if (shstr_size > 0) {
        if (pwrite(fd, shstr, shstr_size, shstr_off) != (ssize_t)shstr_size) { perror("pwrite shstr"); free(shstr); free(strtab); free(symtab); return -1; }
        cur = shstr_off + shstr_size;
    }

    uint64_t strtab_off = cur;
    if (strtab_size > 0) {
        if (pwrite(fd, strtab, strtab_size, strtab_off) != (ssize_t)strtab_size) { perror("pwrite strtab"); free(shstr); free(strtab); free(symtab); return -1; }
        cur = strtab_off + strtab_size;
    }

    /* Fill symtab st_name offsets now */
    size_t soff = 1;
    idx = 1;
    for (size_t i = 0; i < global_count; i++) {
        if (globals[i] && globals[i]->defined) {
            symtab[idx].st_name = (Elf64_Word)soff;
            soff += strlen(globals[i]->name) + 1;
            idx++;
        }
    }

    uint64_t symtab_off = cur;
    size_t symtab_bytes = sizeof(Elf64_Sym) * symtab_entries;
    if (symtab_entries > 0) {
        if (pwrite(fd, symtab, symtab_bytes, symtab_off) != (ssize_t)symtab_bytes) { perror("pwrite symtab"); free(shstr); free(strtab); free(symtab); return -1; }
        cur = symtab_off + symtab_bytes;
    }

    /* Build section headers */
    size_t sh_count = 1 + 4 + 3; /* null + text/rodata/data/bss + shstrtab/symtab/strtab */
    Elf64_Shdr *shdrs = xmalloc(sizeof(Elf64_Shdr) * sh_count);
    memset(shdrs, 0, sizeof(Elf64_Shdr) * sh_count);
    /* index mapping */
    size_t idx_text = 1, idx_rodata = 2, idx_data = 3, idx_bss = 4, idx_shstr = 5, idx_sym = 6, idx_str = 7;
    /* null header already zeroed */
    /* .text */
    shdrs[idx_text].sh_name = 1; /* offset into shstrtab */
    shdrs[idx_text].sh_type = SHT_PROGBITS;
    shdrs[idx_text].sh_flags = 0x6; /* alloc + exec */
    shdrs[idx_text].sh_addr = o_text.vaddr;
    shdrs[idx_text].sh_offset = 0x1000;
    shdrs[idx_text].sh_size = o_text.size;
    /* .rodata */
    shdrs[idx_rodata].sh_name = 1 + strlen(".text") + 1;
    shdrs[idx_rodata].sh_type = SHT_PROGBITS;
    shdrs[idx_rodata].sh_flags = 0x2; /* alloc */
    shdrs[idx_rodata].sh_addr = o_rodata.vaddr;
    shdrs[idx_rodata].sh_offset = 0x2000;
    shdrs[idx_rodata].sh_size = o_rodata.size;
    /* .data */
    shdrs[idx_data].sh_name = shdrs[idx_rodata].sh_name + strlen(".rodata") + 1;
    shdrs[idx_data].sh_type = SHT_PROGBITS;
    shdrs[idx_data].sh_flags = 0x3; /* alloc + write */
    shdrs[idx_data].sh_addr = o_data.vaddr;
    shdrs[idx_data].sh_offset = 0x2000 + o_rodata.size;
    shdrs[idx_data].sh_size = o_data.size;
    /* .bss */
    shdrs[idx_bss].sh_name = shdrs[idx_data].sh_name + strlen(".data") + 1;
    shdrs[idx_bss].sh_type = SHT_NOBITS;
    shdrs[idx_bss].sh_flags = 0x3;
    shdrs[idx_bss].sh_addr = o_bss.vaddr;
    shdrs[idx_bss].sh_offset = 0;
    shdrs[idx_bss].sh_size = o_bss.size;
    /* .shstrtab */
    shdrs[idx_shstr].sh_name = shdrs[idx_bss].sh_name + strlen(".bss") + 1;
    shdrs[idx_shstr].sh_type = SHT_STRTAB;
    shdrs[idx_shstr].sh_offset = shstr_off;
    shdrs[idx_shstr].sh_size = shstr_size;
    /* .symtab */
    shdrs[idx_sym].sh_name = shdrs[idx_shstr].sh_name + strlen(".shstrtab") + 1;
    shdrs[idx_sym].sh_type = SHT_SYMTAB;
    shdrs[idx_sym].sh_offset = symtab_off;
    shdrs[idx_sym].sh_size = symtab_bytes;
    shdrs[idx_sym].sh_link = idx_str;
    shdrs[idx_sym].sh_entsize = sizeof(Elf64_Sym);
    /* .strtab */
    shdrs[idx_str].sh_name = shdrs[idx_sym].sh_name + strlen(".symtab") + 1;
    shdrs[idx_str].sh_type = SHT_STRTAB;
    shdrs[idx_str].sh_offset = strtab_off;
    shdrs[idx_str].sh_size = strtab_size;

    /* Write section headers at cur (aligned to 8) */
    if (cur & 7) cur = (cur + 7) & ~7ULL;
    uint64_t shdr_off = cur;
    size_t shdr_bytes = sizeof(Elf64_Shdr) * sh_count;
    if (pwrite(fd, shdrs, shdr_bytes, shdr_off) != (ssize_t)shdr_bytes) { perror("pwrite shdrs"); free(shstr); free(strtab); free(symtab); free(shdrs); return -1; }

    /* Update ELF header e_shoff and e_shnum and e_shstrndx */
    /* Read existing ELF header, update, and write back */
    Elf64_Ehdr eh;
    if (pread(fd, &eh, sizeof(eh), 0) != (ssize_t)sizeof(eh)) { perror("pread eh"); }
    eh.e_shoff = shdr_off;
    eh.e_shnum = (Elf64_Half)sh_count;
    eh.e_shstrndx = (Elf64_Half)idx_shstr;
    if (pwrite(fd, &eh, sizeof(eh), 0) != (ssize_t)sizeof(eh)) { perror("pwrite eh"); }

    /* cleanup */
    free(shstr); free(strtab); free(symtab); free(shdrs);
    vprintf_dbg("Emitted section headers and simple symtab at offset %" PRIu64 "\n", shdr_off);
    return 0;
}

/* Main */
int ld(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <out> <entry|-> <inputs...>\n", argv[0]);
        return 1;
    }
    const char *outpath = argv[1];
    const char *entry_hint = argv[2];

    /* parse optional flags in environment or args (simple) */
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) verbose = 1;
        if (strcmp(argv[i], "--no-shdr") == 0) emit_shdr = 0;
        if (strcmp(argv[i], "--no-symtab") == 0) emit_symtab = 0;
    }

    /* Load inputs */
    for (int i = 3; i < argc; i++) {
        const char *p = argv[i];
        if (strstr(p, ".a") != NULL) {
            vprintf_dbg("Loading archive: %s\n", p);
            load_archive(p);
        } else {
            vprintf_dbg("Loading object: %s\n", p);
            int fd = open(p, O_RDONLY);
            if (fd < 0) { perror(p); continue; }
            if (load_obj(fd, 0) != 0) vprintf_dbg("Warning: failed to load %s\n", p);
            close(fd);
        }
    }

    /* Concatenate sections */
    for (size_t i = 0; i < obj_count; i++) {
        InputObj *in = &objs[i];
        for (size_t j = 0; j < in->section_count; j++) {
            Section *s = &in->sections[j];
            if (!s->name) continue;
            if (strcmp(s->name, ".text") == 0) append_sec(&o_text, s);
            else if (strcmp(s->name, ".rodata") == 0) append_sec(&o_rodata, s);
            else if (strcmp(s->name, ".data") == 0) append_sec(&o_data, s);
            else if (strcmp(s->name, ".bss") == 0) append_sec(&o_bss, s);
        }
    }

    /* Layout */
    const uint64_t base_vaddr = 0x400000ULL;
    uint64_t v = base_vaddr + 0x1000;
    o_text.vaddr = v;
    v = (v + o_text.size + 0xfff) & ~0xfffULL;
    o_rodata.vaddr = v;
    v = (v + o_rodata.size + 0xfff) & ~0xfffULL;
    o_data.vaddr = v;
    o_bss.vaddr = o_data.vaddr + o_data.size;

    vprintf_dbg("Layout: .text@%#" PRIx64 " size=%zu, .rodata@%#" PRIx64 " size=%zu, .data@%#" PRIx64 " size=%zu, .bss@%#" PRIx64 " size=%zu\n",
                o_text.vaddr, o_text.size, o_rodata.vaddr, o_rodata.size, o_data.vaddr, o_data.size, o_bss.vaddr, o_bss.size);

    /* Resolve globals */
    for (size_t i = 0; i < global_count; i++) {
        Symbol *s = globals[i];
        if (!s->defined || !s->sec) continue;
        uint64_t base = 0;
        if (strcmp(s->sec->name, ".text") == 0) base = o_text.vaddr;
        else if (strcmp(s->sec->name, ".rodata") == 0) base = o_rodata.vaddr;
        else if (strcmp(s->sec->name, ".data") == 0) base = o_data.vaddr;
        else if (strcmp(s->sec->name, ".bss") == 0) base = o_bss.vaddr;
        s->out_addr = base + (uint64_t)s->sec->out_offset + s->value;
        vprintf_dbg("Resolved global %s -> %#" PRIx64 "\n", s->name, s->out_addr);
    }

    /* Apply relocations */
    for (size_t i = 0; i < obj_count; i++) {
        InputObj *in = &objs[i];
        for (size_t j = 0; j < in->section_count; j++) {
            Section *s = &in->sections[j];
            if (!s->name) continue;
            uint8_t *p = NULL; uint64_t vb = 0;
            if (strcmp(s->name, ".text") == 0) { p = o_text.data + s->out_offset; vb = o_text.vaddr + s->out_offset; }
            else if (strcmp(s->name, ".rodata") == 0) { p = o_rodata.data + s->out_offset; vb = o_rodata.vaddr + s->out_offset; }
            else if (strcmp(s->name, ".data") == 0) { p = o_data.data + s->out_offset; vb = o_data.vaddr + s->out_offset; }
            else if (strcmp(s->name, ".bss") == 0) { p = NULL; }

            if (!p) continue;
            for (size_t r = 0; r < s->rela_count; r++) {
                Elf64_Rela *rela = &s->relas[r];
                uint32_t symidx = ELF64_R_SYM(rela->r_info);
                if (symidx >= in->symbol_count) continue;
                Symbol *local_sym = &in->symbols[symidx];
                Symbol *gs = find_global(local_sym->name);
                if (!gs || !gs->defined) continue;
                uint64_t S = gs->out_addr;
                uint64_t A = (uint64_t)rela->r_addend;
                uint64_t P = vb + rela->r_offset;
                uint32_t t = ELF64_R_TYPE(rela->r_info);
                if (t == R_X86_64_64) {
                    uint64_t v64 = S + A;
                    memcpy(p + rela->r_offset, &v64, sizeof(v64));
                } else if (t == R_X86_64_PC32 || t == R_X86_64_PLT32) {
                    uint32_t v32 = (uint32_t)(S + A - P);
                    memcpy(p + rela->r_offset, &v32, sizeof(v32));
                } else {
                    vprintf_dbg("Unknown relocation type %u at section %s offset %zu\n", t, s->name, (size_t)rela->r_offset);
                }
            }
        }
    }

    /* Write output ELF */
    int fd = open(outpath, O_CREAT | O_TRUNC | O_WRONLY, 0755);
    if (fd < 0) { perror(outpath); return 1; }

    Elf64_Ehdr eh;
    memset(&eh, 0, sizeof(eh));
    eh.e_ident[0] = 0x7f; eh.e_ident[1] = 'E'; eh.e_ident[2] = 'L'; eh.e_ident[3] = 'F';
    eh.e_ident[4] = 2; eh.e_ident[5] = 1; eh.e_ident[6] = 1;
    eh.e_type = 2; eh.e_machine = 62; eh.e_version = 1;
    eh.e_entry = 0;
    eh.e_phoff = sizeof(Elf64_Ehdr);
    eh.e_ehsize = sizeof(Elf64_Ehdr);
    eh.e_phentsize = sizeof(Elf64_Phdr);
    eh.e_phnum = 2;
    eh.e_shentsize = sizeof(Elf64_Shdr);

    /* Determine entry */
    Symbol *entry_sym = NULL;
    if (entry_hint && strcmp(entry_hint, "-") != 0) entry_sym = find_global(entry_hint);
    if (!entry_sym) entry_sym = find_global("_start");
    if (!entry_sym) entry_sym = find_global("main");
    eh.e_entry = entry_sym ? entry_sym->out_addr : o_text.vaddr;

    /* Program headers */
    Elf64_Phdr ph[2];
    memset(&ph, 0, sizeof(ph));
    ph[0].p_type = 1; ph[0].p_offset = 0x1000; ph[0].p_vaddr = o_text.vaddr; ph[0].p_paddr = o_text.vaddr;
    ph[0].p_filesz = (Elf64_Xword)o_text.size; ph[0].p_memsz = (Elf64_Xword)o_text.size; ph[0].p_flags = 5; ph[0].p_align = 0x1000;
    ph[1].p_type = 1; ph[1].p_offset = 0x2000; ph[1].p_vaddr = o_rodata.vaddr; ph[1].p_paddr = o_rodata.vaddr;
    ph[1].p_filesz = (Elf64_Xword)(o_rodata.size + o_data.size); ph[1].p_memsz = (Elf64_Xword)(o_rodata.size + o_data.size + o_bss.size);
    ph[1].p_flags = 6; ph[1].p_align = 0x1000;

    if (write(fd, &eh, sizeof(eh)) != (ssize_t)sizeof(eh)) { perror("write eh"); close(fd); return 1; }
    if (write(fd, ph, sizeof(ph)) != (ssize_t)sizeof(ph)) { perror("write ph"); close(fd); return 1; }

    /* pad to 0x1000 */
    off_t cur = lseek_checked(fd, 0, SEEK_CUR);
    if (cur < 0x1000) {
        size_t pad = (size_t)(0x1000 - cur);
        void *z = calloc(1, pad);
        if (!z) { perror("calloc"); close(fd); return 1; }
        if (write(fd, z, pad) != (ssize_t)pad) { perror("pad"); free(z); close(fd); return 1; }
        free(z);
    }

    if (o_text.size > 0) {
        if (pwrite(fd, o_text.data, o_text.size, 0x1000) != (ssize_t)o_text.size) { perror("pwrite text"); close(fd); return 1; }
    }
    if (o_rodata.size > 0) {
        if (pwrite(fd, o_rodata.data, o_rodata.size, 0x2000) != (ssize_t)o_rodata.size) { perror("pwrite rodata"); close(fd); return 1; }
    }
    off_t data_off = 0x2000 + (off_t)o_rodata.size;
    if (o_data.size > 0) {
        if (pwrite(fd, o_data.data, o_data.size, data_off) != (ssize_t)o_data.size) { perror("pwrite data"); close(fd); return 1; }
    }

    /* Optionally emit section headers and symtab */
    off_t file_end = lseek_checked(fd, 0, SEEK_END);
    if (emit_shdr || emit_symtab) {
        if (emit_section_and_symtabs(outpath, fd, (uint64_t)file_end, base_vaddr) != 0) {
            vprintf_dbg("Warning: failed to emit section headers or symtab\n");
        }
    }

    close(fd);

    /* Cleanup allocated memory (best-effort) */
    for (size_t i = 0; i < obj_count; i++) {
        InputObj *in = &objs[i];
        if (!in) continue;
        for (size_t j = 0; j < in->section_count; j++) {
            Section *s = &in->sections[j];
            if (!s) continue;
            free(s->name);
            free(s->data);
            free(s->relas);
        }
        free(in->sections);
        for (size_t k = 0; k < in->symbol_count; k++) {
            free(in->symbols[k].name);
        }
        free(in->symbols);
    }
    for (size_t i = 0; i < global_count; i++) {
        /* globals point into input symbol arrays; don't double-free names */
    }
    free(objs);
    free(globals);
    free(o_text.data); free(o_rodata.data); free(o_data.data);

    printf("Successfully linked executable: %s\n", outpath);
    return 0;
}

