/*
 slinker.c - Minimaler ELF64 Linker (x86_64, Linux)
 Korrigierte Version: C99-kompatibel, keine C++-Konstrukte.
 Einschränkungen siehe vorherige Beschreibung (nur einfache Fälle).
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
#include <dirent.h>

/* ELF-Typen (kleiner Satz) */
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
    Elf64_Sxword r_addend;
} Elf64_Rela;

#define ELFCLASS64 2
#define ELFDATA2LSB 1
#define EV_CURRENT 1
#define ET_REL 1
#define ET_EXEC 2
#define EM_X86_64 62

#define SHT_NULL     0
#define SHT_PROGBITS 1
#define SHT_SYMTAB   2
#define SHT_STRTAB   3
#define SHT_RELA     4
#define SHT_NOBITS   8

#define ELF64_R_SYM(i) ((uint32_t)((i) >> 32))
#define ELF64_R_TYPE(i) ((uint32_t)((i) & 0xffffffffUL))

#define R_X86_64_NONE 0
#define R_X86_64_64   1
#define R_X86_64_PC32 2

#define ELF64_ST_BIND(i) ((i) >> 4)
#define ELF64_ST_TYPE(i) ((i) & 0xf)

/* SectionData erweitert um out_offset (Offset in zusammengefügtem Abschnitt) */
typedef struct {
    char *name;
    unsigned char *data;
    size_t size;
    size_t align;
    int is_bss;
    size_t out_offset; /* Offset innerhalb des zusammengefügten Abschnitts oder (size_t)-1 */
} SectionData;

typedef struct {
    char *name;
    Elf64_Sym *symtab;
    char *strtab;
    size_t symcount;
} SymtabData;

typedef struct {
    char *filename;
    SectionData text, rodata, data, bss;
    SymtabData symtab;
    Elf64_Rela *rela_text;
    size_t rela_text_count;
    Elf64_Rela *rela_data;
    size_t rela_data_count;
    Elf64_Rela *rela_rodata;
    size_t rela_rodata_count;
} ObjFile;

/* Globals */
ObjFile *objs = NULL;
size_t obj_count = 0;

unsigned char *out_text = NULL;
size_t out_text_size = 0;
size_t out_text_align = 16;

unsigned char *out_rodata = NULL;
size_t out_rodata_size = 0;
size_t out_rodata_align = 16;

unsigned char *out_data = NULL;
size_t out_data_size = 0;
size_t out_data_align = 8;

size_t out_bss_size = 0;
size_t out_bss_align = 8;

typedef struct {
    char *name;
    Elf64_Addr value;
    size_t size;
    int defined;
    int is_func;
} GlobalSym;

GlobalSym *gsyms = NULL;
size_t gsym_count = 0;

/* Hilfsfunktionen */
static void *read_file(const char *path, size_t *out_size) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long s = ftell(f);
    if (s < 0) { fclose(f); return NULL; }
    rewind(f);
    unsigned char *buf = malloc((size_t)s);
    if (!buf) { fclose(f); return NULL; }
    if (fread(buf, 1, (size_t)s, f) != (size_t)s) { free(buf); fclose(f); return NULL; }
    fclose(f);
    *out_size = (size_t)s;
    return buf;
}

static uint64_t align_up(uint64_t v, uint64_t a) {
    if (a == 0) return v;
    return (v + a - 1) & ~(a - 1);
}

/* parse_obj: liest .o und füllt ObjFile */
static int parse_obj(const char *path) {
    size_t sz;
    unsigned char *buf = read_file(path, &sz);
    if (!buf) {
        fprintf(stderr, "Error: cannot read %s: %s\n", path, strerror(errno));
        return -1;
    }
    if (sz < sizeof(Elf64_Ehdr)) { free(buf); fprintf(stderr,"%s: too small\n",path); return -1; }
    Elf64_Ehdr *eh = (Elf64_Ehdr*)buf;
    if (eh->e_ident[0] != 0x7f || eh->e_ident[1] != 'E' || eh->e_ident[2] != 'L' || eh->e_ident[3] != 'F') {
        free(buf); fprintf(stderr,"%s: not ELF\n",path); return -1;
    }
    if (eh->e_ident[4] != ELFCLASS64 || eh->e_ident[5] != ELFDATA2LSB) {
        free(buf); fprintf(stderr,"%s: unsupported ELF class/data\n",path); return -1;
    }
    if (eh->e_type != ET_REL) {
        free(buf); fprintf(stderr,"%s: not relocatable object (ET_REL required)\n",path); return -1;
    }
    if (eh->e_machine != EM_X86_64) {
        free(buf); fprintf(stderr,"%s: unsupported machine\n",path); return -1;
    }

    Elf64_Shdr *sh = (Elf64_Shdr*)(buf + eh->e_shoff);
    if (eh->e_shstrndx >= eh->e_shnum) { free(buf); fprintf(stderr,"%s: bad shstrndx\n",path); return -1; }
    Elf64_Shdr *shstr = &sh[eh->e_shstrndx];
    char *shstrtab = (char*)(buf + shstr->sh_offset);

    ObjFile obj;
    memset(&obj, 0, sizeof(obj));
    obj.filename = strdup(path);
    obj.text.out_offset = (size_t)-1;
    obj.rodata.out_offset = (size_t)-1;
    obj.data.out_offset = (size_t)-1;
    obj.bss.out_offset = (size_t)-1;

    for (int i = 0; i < eh->e_shnum; ++i) {
        Elf64_Shdr *s = &sh[i];
        const char *name = shstrtab + s->sh_name;
        if (s->sh_type == SHT_PROGBITS || s->sh_type == SHT_NOBITS) {
            if (strcmp(name, ".text") == 0) {
                obj.text.size = s->sh_size;
                obj.text.align = s->sh_addralign ? s->sh_addralign : 16;
                obj.text.is_bss = (s->sh_type == SHT_NOBITS);
                if (!obj.text.is_bss && s->sh_size) {
                    obj.text.data = malloc(s->sh_size);
                    memcpy(obj.text.data, buf + s->sh_offset, s->sh_size);
                } else obj.text.data = NULL;
                obj.text.name = strdup(".text");
            } else if (strcmp(name, ".rodata") == 0 || strcmp(name, ".rdata") == 0) {
                obj.rodata.size = s->sh_size;
                obj.rodata.align = s->sh_addralign ? s->sh_addralign : 16;
                obj.rodata.is_bss = (s->sh_type == SHT_NOBITS);
                if (!obj.rodata.is_bss && s->sh_size) {
                    obj.rodata.data = malloc(s->sh_size);
                    memcpy(obj.rodata.data, buf + s->sh_offset, s->sh_size);
                } else obj.rodata.data = NULL;
                obj.rodata.name = strdup(".rodata");
            } else if (strcmp(name, ".data") == 0) {
                obj.data.size = s->sh_size;
                obj.data.align = s->sh_addralign ? s->sh_addralign : 8;
                obj.data.is_bss = (s->sh_type == SHT_NOBITS);
                if (!obj.data.is_bss && s->sh_size) {
                    obj.data.data = malloc(s->sh_size);
                    memcpy(obj.data.data, buf + s->sh_offset, s->sh_size);
                } else obj.data.data = NULL;
                obj.data.name = strdup(".data");
            } else if (strcmp(name, ".bss") == 0) {
                obj.bss.size = s->sh_size;
                obj.bss.align = s->sh_addralign ? s->sh_addralign : 8;
                obj.bss.is_bss = 1;
                obj.bss.data = NULL;
                obj.bss.name = strdup(".bss");
            }
        } else if (s->sh_type == SHT_SYMTAB) {
            Elf64_Sym *symtab = (Elf64_Sym*)(buf + s->sh_offset);
            size_t nsyms = s->sh_size / s->sh_entsize;
            if (s->sh_link >= eh->e_shnum) { free(buf); fprintf(stderr,"%s: bad symtab link\n",path); return -1; }
            Elf64_Shdr *strsh = &sh[s->sh_link];
            char *strtab = (char*)(buf + strsh->sh_offset);
            obj.symtab.symcount = nsyms;
            obj.symtab.symtab = malloc(nsyms * sizeof(Elf64_Sym));
            memcpy(obj.symtab.symtab, symtab, nsyms * sizeof(Elf64_Sym));
            obj.symtab.strtab = malloc(strsh->sh_size + 1);
            memcpy(obj.symtab.strtab, strtab, strsh->sh_size);
            obj.symtab.strtab[strsh->sh_size] = '\0';
            obj.symtab.name = strdup(".symtab");
        } else if (s->sh_type == SHT_RELA) {
            if (s->sh_info >= eh->e_shnum) { free(buf); fprintf(stderr,"%s: bad rela sh_info\n",path); return -1; }
            Elf64_Shdr *target = &sh[s->sh_info];
            const char *tname = shstrtab + target->sh_name;
            Elf64_Rela *rela = (Elf64_Rela*)(buf + s->sh_offset);
            size_t nrel = s->sh_size / s->sh_entsize;
            if (strcmp(tname, ".text") == 0) {
                obj.rela_text = malloc(nrel * sizeof(Elf64_Rela));
                memcpy(obj.rela_text, rela, nrel * sizeof(Elf64_Rela));
                obj.rela_text_count = nrel;
            } else if (strcmp(tname, ".data") == 0) {
                obj.rela_data = malloc(nrel * sizeof(Elf64_Rela));
                memcpy(obj.rela_data, rela, nrel * sizeof(Elf64_Rela));
                obj.rela_data_count = nrel;
            } else if (strcmp(tname, ".rodata") == 0) {
                obj.rela_rodata = malloc(nrel * sizeof(Elf64_Rela));
                memcpy(obj.rela_rodata, rela, nrel * sizeof(Elf64_Rela));
                obj.rela_rodata_count = nrel;
            }
        }
    }

    objs = realloc(objs, (obj_count + 1) * sizeof(ObjFile));
    objs[obj_count] = obj;
    obj_count++;

    free(buf);
    return 0;
}

/* Layout: concat sections, set out_offset in each SectionData */
static void layout_sections() {
    size_t tsize = 0, rsize = 0, dsize = 0, bsize = 0;
    for (size_t i = 0; i < obj_count; ++i) {
        ObjFile *o = &objs[i];
        if (o->text.size) {
            tsize = align_up(tsize, o->text.align);
            tsize += o->text.size;
            if (o->text.align > out_text_align) out_text_align = o->text.align;
        }
        if (o->rodata.size) {
            rsize = align_up(rsize, o->rodata.align);
            rsize += o->rodata.size;
            if (o->rodata.align > out_rodata_align) out_rodata_align = o->rodata.align;
        }
        if (o->data.size) {
            dsize = align_up(dsize, o->data.align);
            dsize += o->data.size;
            if (o->data.align > out_data_align) out_data_align = o->data.align;
        }
        if (o->bss.size) {
            bsize = align_up(bsize, o->bss.align);
            bsize += o->bss.size;
            if (o->bss.align > out_bss_align) out_bss_align = o->bss.align;
        }
    }
    out_text_size = tsize;
    out_rodata_size = rsize;
    out_data_size = dsize;
    out_bss_size = bsize;

    if (out_text_size) {
        out_text = malloc(out_text_size);
        memset(out_text, 0x90, out_text_size);
    }
    if (out_rodata_size) {
        out_rodata = malloc(out_rodata_size);
        memset(out_rodata, 0, out_rodata_size);
    }
    if (out_data_size) {
        out_data = malloc(out_data_size);
        memset(out_data, 0, out_data_size);
    }

    size_t toff = 0, roff = 0, doff = 0, boff = 0;
    for (size_t i = 0; i < obj_count; ++i) {
        ObjFile *o = &objs[i];
        if (o->text.size) {
            toff = align_up(toff, o->text.align);
            if (o->text.data) memcpy(out_text + toff, o->text.data, o->text.size);
            o->text.out_offset = toff;
            toff += o->text.size;
        } else {
            o->text.out_offset = (size_t)-1;
        }
        if (o->rodata.size) {
            roff = align_up(roff, o->rodata.align);
            if (o->rodata.data) memcpy(out_rodata + roff, o->rodata.data, o->rodata.size);
            o->rodata.out_offset = roff;
            roff += o->rodata.size;
        } else {
            o->rodata.out_offset = (size_t)-1;
        }
        if (o->data.size) {
            doff = align_up(doff, o->data.align);
            if (o->data.data) memcpy(out_data + doff, o->data.data, o->data.size);
            o->data.out_offset = doff;
            doff += o->data.size;
        } else {
            o->data.out_offset = (size_t)-1;
        }
        if (o->bss.size) {
            boff = align_up(boff, o->bss.align);
            o->bss.out_offset = boff;
            boff += o->bss.size;
        } else {
            o->bss.out_offset = (size_t)-1;
        }
    }
}

/* Hilf: fügt globales Symbol hinzu oder markiert undefiniert */
static int add_or_update_gsym(const char *name, Elf64_Addr val, size_t sz, int defined, int is_func) {
    for (size_t i = 0; i < gsym_count; ++i) {
        if (strcmp(gsyms[i].name, name) == 0) {
            if (defined && gsyms[i].defined) {
                fprintf(stderr, "Error: multiple definition of symbol %s\n", name);
                return -1;
            }
            if (defined) {
                gsyms[i].value = val;
                gsyms[i].size = sz;
                gsyms[i].defined = 1;
                gsyms[i].is_func = is_func;
            }
            return 0;
        }
    }
    gsyms = realloc(gsyms, (gsym_count + 1) * sizeof(GlobalSym));
    gsyms[gsym_count].name = strdup(name);
    gsyms[gsym_count].value = val;
    gsyms[gsym_count].size = sz;
    gsyms[gsym_count].defined = defined;
    gsyms[gsym_count].is_func = is_func;
    gsym_count++;
    return 0;
}

/* Build global symbols: berechne virtuelle Adressen basierend auf out_offsets */
static int build_global_symbols(Elf64_Addr base_text_vaddr, Elf64_Addr base_rodata_vaddr, Elf64_Addr base_data_vaddr) {
    for (size_t i = 0; i < obj_count; ++i) {
        ObjFile *o = &objs[i];
        if (!o->symtab.symtab) continue;
        for (size_t s = 0; s < o->symtab.symcount; ++s) {
            Elf64_Sym *sym = &o->symtab.symtab[s];
            const char *sname = o->symtab.strtab + sym->st_name;
            if (!sname || sname[0] == '\0') continue;
            int bind = ELF64_ST_BIND(sym->st_info);
            int type = ELF64_ST_TYPE(sym->st_info);
            if (bind == 0) continue; /* local skip */
            if (sym->st_shndx != 0) {
                Elf64_Addr val = 0;
                if (o->text.out_offset != (size_t)-1 && sym->st_value < o->text.size) {
                    val = base_text_vaddr + o->text.out_offset + sym->st_value;
                } else if (o->rodata.out_offset != (size_t)-1 && sym->st_value < o->rodata.size) {
                    val = base_rodata_vaddr + o->rodata.out_offset + sym->st_value;
                } else if (o->data.out_offset != (size_t)-1 && sym->st_value < o->data.size) {
                    val = base_data_vaddr + o->data.out_offset + sym->st_value;
                } else if (o->bss.out_offset != (size_t)-1 && sym->st_value < o->bss.size) {
                    val = base_data_vaddr + o->bss.out_offset + sym->st_value;
                } else {
                    if (type == 2 && o->text.out_offset != (size_t)-1) {
                        val = base_text_vaddr + o->text.out_offset + sym->st_value;
                    } else {
                        /* cannot map symbol; skip */
                        continue;
                    }
                }
                if (add_or_update_gsym(sname, val, sym->st_size, 1, (type == 2)) != 0) return -1;
            } else {
                /* undefined */
                if (add_or_update_gsym(sname, 0, 0, 0, 0) != 0) return -1;
            }
        }
    }
    for (size_t g = 0; g < gsym_count; ++g) {
        if (!gsyms[g].defined) {
            fprintf(stderr, "Error: undefined symbol: %s\n", gsyms[g].name);
            return -1;
        }
    }
    return 0;
}

/* Resolve symbol index in object to absolute address using global table */
static Elf64_Addr resolve_symbol_address(ObjFile *o, uint32_t symidx) {
    if (!o->symtab.symtab) return 0;
    if (symidx >= o->symtab.symcount) return 0;
    Elf64_Sym *sym = &o->symtab.symtab[symidx];
    const char *sname = o->symtab.strtab + sym->st_name;
    if (!sname) return 0;
    for (size_t g = 0; g < gsym_count; ++g) {
        if (strcmp(gsyms[g].name, sname) == 0) return gsyms[g].value;
    }
    return 0;
}

/* Apply relocations (R_X86_64_64, R_X86_64_PC32) */
static int apply_relocations(Elf64_Addr base_text_vaddr, Elf64_Addr base_rodata_vaddr, Elf64_Addr base_data_vaddr) {
    for (size_t i = 0; i < obj_count; ++i) {
        ObjFile *o = &objs[i];
        /* text */
        for (size_t r = 0; r < o->rela_text_count; ++r) {
            Elf64_Rela *rela = &o->rela_text[r];
            uint32_t sym = ELF64_R_SYM(rela->r_info);
            uint32_t type = ELF64_R_TYPE(rela->r_info);
            if (o->text.out_offset == (size_t)-1) { fprintf(stderr,"relocation target missing .text\n"); return -1; }
            Elf64_Addr where = base_text_vaddr + o->text.out_offset + rela->r_offset;
            Elf64_Addr S = resolve_symbol_address(o, sym);
            Elf64_Sxword A = rela->r_addend;
            if (type == R_X86_64_64) {
                uint64_t val = (uint64_t)(S + A);
                size_t off = (size_t)(where - base_text_vaddr);
                if (off + 8 > out_text_size) { fprintf(stderr,"relocation out of range\n"); return -1; }
                memcpy(out_text + off, &val, 8);
            } else if (type == R_X86_64_PC32) {
                Elf64_Addr P = where;
                int32_t val32 = (int32_t)((S + A) - P);
                size_t off = (size_t)(where - base_text_vaddr);
                if (off + 4 > out_text_size) { fprintf(stderr,"relocation out of range\n"); return -1; }
                memcpy(out_text + off, &val32, 4);
            } else {
                fprintf(stderr, "Unsupported relocation type %u in %s\n", type, o->filename);
                return -1;
            }
        }
        /* data */
        for (size_t r = 0; r < o->rela_data_count; ++r) {
            Elf64_Rela *rela = &o->rela_data[r];
            uint32_t sym = ELF64_R_SYM(rela->r_info);
            uint32_t type = ELF64_R_TYPE(rela->r_info);
            if (o->data.out_offset == (size_t)-1) { fprintf(stderr,"relocation target missing .data\n"); return -1; }
            Elf64_Addr where = base_data_vaddr + o->data.out_offset + rela->r_offset;
            Elf64_Addr S = resolve_symbol_address(o, sym);
            Elf64_Sxword A = rela->r_addend;
            if (type == R_X86_64_64) {
                uint64_t val = (uint64_t)(S + A);
                size_t off = (size_t)(where - base_data_vaddr);
                if (off + 8 > out_data_size) { fprintf(stderr,"relocation out of range\n"); return -1; }
                memcpy(out_data + off, &val, 8);
            } else if (type == R_X86_64_PC32) {
                Elf64_Addr P = where;
                int32_t val32 = (int32_t)((S + A) - P);
                size_t off = (size_t)(where - base_data_vaddr);
                if (off + 4 > out_data_size) { fprintf(stderr,"relocation out of range\n"); return -1; }
                memcpy(out_data + off, &val32, 4);
            } else {
                fprintf(stderr, "Unsupported relocation type %u in %s\n", type, o->filename);
                return -1;
            }
        }
        /* rodata */
        for (size_t r = 0; r < o->rela_rodata_count; ++r) {
            Elf64_Rela *rela = &o->rela_rodata[r];
            uint32_t sym = ELF64_R_SYM(rela->r_info);
            uint32_t type = ELF64_R_TYPE(rela->r_info);
            if (o->rodata.out_offset == (size_t)-1) { fprintf(stderr,"relocation target missing .rodata\n"); return -1; }
            Elf64_Addr where = base_rodata_vaddr + o->rodata.out_offset + rela->r_offset;
            Elf64_Addr S = resolve_symbol_address(o, sym);
            Elf64_Sxword A = rela->r_addend;
            if (type == R_X86_64_64) {
                uint64_t val = (uint64_t)(S + A);
                size_t off = (size_t)(where - base_rodata_vaddr);
                if (off + 8 > out_rodata_size) { fprintf(stderr,"relocation out of range\n"); return -1; }
                memcpy(out_rodata + off, &val, 8);
            } else {
                fprintf(stderr, "Unsupported relocation type %u in %s\n", type, o->filename);
                return -1;
            }
        }
    }
    return 0;
}

/* write little-endian helpers */
static void write64le(unsigned char *p, uint64_t v) {
    for (int i = 0; i < 8; ++i) p[i] = (unsigned char)((v >> (i*8)) & 0xff);
}
static void write32le(unsigned char *p, uint32_t v) {
    for (int i = 0; i < 4; ++i) p[i] = (unsigned char)((v >> (i*8)) & 0xff);
}

/* Write final ELF executable (vereinfachte PHDR-Berechnung) */
static int write_output(const char *outpath, Elf64_Addr base_vaddr,
                        Elf64_Addr base_text_vaddr, Elf64_Addr base_rodata_vaddr, Elf64_Addr base_data_vaddr,
                        Elf64_Addr entry) {
    int fd = open(outpath, O_CREAT | O_TRUNC | O_WRONLY, 0755);
    if (fd < 0) { perror("open output"); return -1; }

    size_t phnum = 2;
    size_t ehdr_size = sizeof(Elf64_Ehdr);
    size_t phdr_size = 56;
    size_t phdrs_total = phnum * phdr_size;
    size_t offset = align_up(ehdr_size + phdrs_total, 0x1000);

    size_t seg0_fileoff = offset;
    size_t seg0_filesz = out_text_size + out_rodata_size;
    size_t seg0_memsz = seg0_filesz;
    offset = seg0_fileoff + seg0_filesz;
    offset = align_up(offset, 0x1000);

    size_t seg1_fileoff = offset;
    size_t seg1_filesz = out_data_size;
    size_t seg1_memsz = out_data_size + out_bss_size;
    offset = seg1_fileoff + seg1_filesz;

    Elf64_Ehdr eh;
    memset(&eh, 0, sizeof(eh));
    eh.e_ident[0] = 0x7f; eh.e_ident[1] = 'E'; eh.e_ident[2] = 'L'; eh.e_ident[3] = 'F';
    eh.e_ident[4] = ELFCLASS64;
    eh.e_ident[5] = ELFDATA2LSB;
    eh.e_ident[6] = EV_CURRENT;
    eh.e_type = ET_EXEC;
    eh.e_machine = EM_X86_64;
    eh.e_version = EV_CURRENT;
    eh.e_entry = entry;
    eh.e_phoff = sizeof(Elf64_Ehdr);
    eh.e_ehsize = sizeof(Elf64_Ehdr);
    eh.e_phentsize = phdr_size;
    eh.e_phnum = phnum;

    unsigned char *phdrs = calloc(phnum, phdr_size);
    if (!phdrs) { close(fd); return -1; }

    /* PHDR 0: text+rodata RX */
    unsigned char *p0 = phdrs;
    write32le(p0 + 0, 1); /* PT_LOAD */
    write32le(p0 + 4, 5); /* PF_R | PF_X */
    write64le(p0 + 8, seg0_fileoff);
    write64le(p0 + 16, base_text_vaddr); /* vaddr for text segment */
    write64le(p0 + 24, base_text_vaddr);
    write64le(p0 + 32, seg0_filesz);
    write64le(p0 + 40, seg0_memsz);
    write64le(p0 + 48, 0x1000);

    /* PHDR 1: data RW */
    unsigned char *p1 = phdrs + phdr_size;
    write32le(p1 + 0, 1); /* PT_LOAD */
    write32le(p1 + 4, 6); /* PF_R | PF_W */
    write64le(p1 + 8, seg1_fileoff);
    write64le(p1 + 16, base_data_vaddr);
    write64le(p1 + 24, base_data_vaddr);
    write64le(p1 + 32, seg1_filesz);
    write64le(p1 + 40, seg1_memsz);
    write64le(p1 + 48, 0x1000);

    if (write(fd, &eh, sizeof(eh)) != sizeof(eh)) { perror("write eh"); free(phdrs); close(fd); return -1; }
    if (write(fd, phdrs, phnum * phdr_size) != (ssize_t)(phnum * phdr_size)) { perror("write ph"); free(phdrs); close(fd); return -1; }
    free(phdrs);

    off_t cur = lseek(fd, 0, SEEK_CUR);
    if (cur < 0) { perror("lseek"); close(fd); return -1; }
    if ((size_t)cur > seg0_fileoff) { fprintf(stderr,"internal layout error\n"); close(fd); return -1; }
    size_t pad = seg0_fileoff - (size_t)cur;
    if (pad) {
        unsigned char *z = calloc(1, pad);
        if (!z) { close(fd); return -1; }
        if (write(fd, z, pad) != (ssize_t)pad) { perror("write pad"); free(z); close(fd); return -1; }
        free(z);
    }

    if (out_text_size) {
        if (write(fd, out_text, out_text_size) != (ssize_t)out_text_size) { perror("write text"); close(fd); return -1; }
    }
    if (out_rodata_size) {
        if (write(fd, out_rodata, out_rodata_size) != (ssize_t)out_rodata_size) { perror("write rodata"); close(fd); return -1; }
    }

    cur = lseek(fd, 0, SEEK_CUR);
    if ((size_t)cur > seg1_fileoff) { fprintf(stderr,"internal layout error 2\n"); close(fd); return -1; }
    pad = seg1_fileoff - (size_t)cur;
    if (pad) {
        unsigned char *z = calloc(1, pad);
        if (!z) { close(fd); return -1; }
        if (write(fd, z, pad) != (ssize_t)pad) { perror("write pad2"); free(z); close(fd); return -1; }
        free(z);
    }

    if (out_data_size) {
        if (write(fd, out_data, out_data_size) != (ssize_t)out_data_size) { perror("write data"); close(fd); return -1; }
    }

    close(fd);
    return 0;
}

/* find entry (main) */
static Elf64_Addr find_entry_address(Elf64_Addr base_text_vaddr) {
    for (size_t g = 0; g < gsym_count; ++g) {
        if (strcmp(gsyms[g].name, "main") == 0 && gsyms[g].defined) return gsyms[g].value;
    }
    for (size_t g = 0; g < gsym_count; ++g) {
        if (gsyms[g].defined && gsyms[g].is_func) return gsyms[g].value;
    }
    return base_text_vaddr;
}

/* main */
int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s file1.o file2.o ... -o output\n", argv[0]);
        return 1;
    }
    const char *outpath = NULL;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) { outpath = argv[i+1]; i++; }
        else {
            if (parse_obj(argv[i]) != 0) return 1;
        }
    }
    if (!outpath) { fprintf(stderr, "No output specified (-o)\n"); return 1; }
    if (obj_count == 0) { fprintf(stderr, "No input objects\n"); return 1; }

    layout_sections();

    Elf64_Addr base_vaddr = 0x400000;
    Elf64_Addr base_text_vaddr = base_vaddr + 0x1000;
    Elf64_Addr base_rodata_vaddr = base_text_vaddr + align_up(out_text_size, 0x1000);
    Elf64_Addr base_data_vaddr = align_up(base_rodata_vaddr + out_rodata_size, 0x1000);

    if (build_global_symbols(base_text_vaddr, base_rodata_vaddr, base_data_vaddr) != 0) return 1;
    if (apply_relocations(base_text_vaddr, base_rodata_vaddr, base_data_vaddr) != 0) return 1;

    Elf64_Addr entry = find_entry_address(base_text_vaddr);

    if (write_output(outpath, base_vaddr, base_text_vaddr, base_rodata_vaddr, base_data_vaddr, entry) != 0) {
        fprintf(stderr, "Failed to write output\n");
        return 1;
    }

    printf("Linked %zu objects -> %s (entry 0x%lx)\n", obj_count, outpath, (unsigned long)entry);
    return 0;
}
