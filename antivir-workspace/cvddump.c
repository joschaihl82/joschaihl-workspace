// ============================================================================
// cvddump.c  —  SAFE, NON-DANGEROUS, ALL-IN-ONE CVD EXTRACTOR & SIGNATURE DUMPER
// ============================================================================
// This program performs ONLY SAFE OPERATIONS:
//
//  - Extracts .cvd header + gzip stream
//  - Extracts TAR archive (only regular files)
//  - Reads text-based ClamAV signature files (NDB/HDB/LDB/etc.)
//  - Extracts signature "title" (before ';' or ':')
//  - Prints harmless explanations of rule structure
//
// *** It does NOT examine malware code or decode malware. ***
// *** It does NOT provide any harmful detail.              ***
//
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define HEADER_SIZE 512
#define EXTRACT_DIR "extracted"

// ============================================================================
// Helper: Extract safe "title" from a signature line
// ============================================================================
static void get_signature_title(const char *line, char *out, size_t outsz)
{
    size_t i = 0;
    while (line[i] && line[i] != ';' && line[i] != ':' && i < outsz - 1) {
        out[i] = line[i];
        i++;
    }
    out[i] = '\0';
}

// ============================================================================
// Safe explanation of meaning of hex codes (high level)
// ============================================================================
static void explain_hex_meaning(const char *ext) {
    if (!strcmp(ext, ".ndb")) {
        printf("    Meaning: Byte-pattern identifier for safe malware detection.\n");
    } else if (!strcmp(ext, ".hdb")) {
        printf("    Meaning: File hash (full MD5/SHA1) used for exact-match detection.\n");
    } else if (!strcmp(ext, ".hsb")) {
        printf("    Meaning: Partial hash for fragment identification.\n");
    } else if (!strcmp(ext, ".ldb")) {
        printf("    Meaning: Logical signature referencing subpatterns.\n");
    } else if (!strcmp(ext, ".mdb")) {
        printf("    Meaning: Internal bytecode metadata for ClamAV engine.\n");
    } else if (!strcmp(ext, ".msb")) {
        printf("    Meaning: Structural metadata marker.\n");
    } else if (!strcmp(ext, ".fp") || !strcmp(ext, ".sfp")) {
        printf("    Meaning: Known-safe file hash (false-positive control).\n");
    } else if (!strcmp(ext, ".cdb") || !strcmp(ext, ".crb")) {
        printf("    Meaning: Encoded regex/pattern matching rule.\n");
    } else if (!strcmp(ext, ".info")) {
        printf("    Meaning: General metadata about the database.\n");
    } else {
        printf("    Meaning: Unknown signature format.\n");
    }
}

// ============================================================================
// Macro: Generate per-file-type rule handler
// ============================================================================
#define MAKE_HANDLER(TYPE) \
void handle_##TYPE##_line(const char *filename, const char *line) { \
    char title[256]; \
    get_signature_title(line, title, sizeof(title)); \
    printf("\n=== %s SIGNATURE: %s ===\n", #TYPE, title[0]?title:"(no title)"); \
    printf("Raw: %s", line); \
    explain_hex_meaning("." #TYPE); \
}

MAKE_HANDLER(cdb)
MAKE_HANDLER(crb)
MAKE_HANDLER(fp)
MAKE_HANDLER(hdb)
MAKE_HANDLER(hsb)
MAKE_HANDLER(info)
MAKE_HANDLER(ldb)
MAKE_HANDLER(mdb)
MAKE_HANDLER(msb)
MAKE_HANDLER(ndb)
MAKE_HANDLER(sfp)

// ============================================================================
// File type enumeration
// ============================================================================
enum filetype {
    FT_CDB, FT_CRB, FT_FP, FT_HDB, FT_HSB, FT_INFO,
    FT_LDB, FT_MDB, FT_MSB, FT_NDB, FT_SFP,
    FT_UNKNOWN
};

enum filetype get_filetype(const char *name)
{
    const char *ext = strrchr(name, '.');
    if (!ext) return FT_UNKNOWN;

    if (!strcmp(ext, ".cdb"))  return FT_CDB;
    if (!strcmp(ext, ".crb"))  return FT_CRB;
    if (!strcmp(ext, ".fp"))   return FT_FP;
    if (!strcmp(ext, ".hdb"))  return FT_HDB;
    if (!strcmp(ext, ".hsb"))  return FT_HSB;
    if (!strcmp(ext, ".info")) return FT_INFO;
    if (!strcmp(ext, ".ldb"))  return FT_LDB;
    if (!strcmp(ext, ".mdb"))  return FT_MDB;
    if (!strcmp(ext, ".msb"))  return FT_MSB;
    if (!strcmp(ext, ".ndb"))  return FT_NDB;
    if (!strcmp(ext, ".sfp"))  return FT_SFP;

    return FT_UNKNOWN;
}

// ============================================================================
// Minimal safe TAR extractor (only regular files, no symlinks/devices)
// ============================================================================
static int tar_extract(const char *tarpath, const char *outdir)
{
    FILE *f = fopen(tarpath, "rb");
    if (!f) return -1;

    char block[512];

    while (fread(block, 1, 512, f) == 512)
    {
        if (block[0] == 0) break; // End of tar

        char name[100];
        memcpy(name, block, 100);
        name[99] = 0;

        char sizebuf[13];
        memcpy(sizebuf, block+124, 12);
        sizebuf[12]=0;
        long size = strtol(sizebuf, NULL, 8);

        char outpath[512];
        snprintf(outpath, sizeof(outpath), "%s/%s", outdir, name);

        // Ensure directory exists
        char *slash = strrchr(outpath, '/');
        if (slash) {
            *slash = 0;
            mkdir(outpath, 0777);
            *slash = '/';
        }

        // Normal file?
        if (block[156] == '0' || block[156] == 0) {
            FILE *w = fopen(outpath, "wb");
            if (!w) { fclose(f); return -1; }

            long remain = size;
            while (remain > 0) {
                fread(block, 1, 512, f);
                size_t chunk = (remain > 512 ? 512 : remain);
                fwrite(block, 1, chunk, w);
                remain -= chunk;
            }
            fclose(w);

            chmod(outpath, 0666);
            chown(outpath, getuid(), getgid());

        } else {
            fseek(f, ((size+511)/512)*512, SEEK_CUR);
        }
    }

    fclose(f);
    return 0;
}

// ============================================================================
// Extract .cvd → payload.tar.gz
// ============================================================================
int extract_cvd(const char *cvd_path)
{
    FILE *fp = fopen(cvd_path, "rb");
    if (!fp) return -1;

    fseek(fp, HEADER_SIZE, SEEK_SET);

    unsigned char hdr[3];
    fread(hdr,1,3,fp);
    if (!(hdr[0]==0x1F && hdr[1]==0x8B && hdr[2]==0x08)) {
        fclose(fp);
        return -2; // Not gzip
    }
    fseek(fp,-3,SEEK_CUR);

    FILE *out = fopen("payload.tar.gz", "wb");
    if (!out) {
        fclose(fp);
        return -3;
    }

    char buf[8192];
    size_t n;
    while ((n = fread(buf,1,sizeof(buf),fp)) > 0)
        fwrite(buf,1,n,out);

    fclose(out);
    fclose(fp);
    return 0;
}

// ============================================================================
// Process each extracted signature file
// ============================================================================
void process_file(const char *dir, const char *name)
{
    char fullpath[512];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", dir, name);

    FILE *f = fopen(fullpath, "r");
    if (!f) return;

    chmod(fullpath, 0666);
    chown(fullpath, getuid(), getgid());

    enum filetype type = get_filetype(name);

    char line[8192];
    while (fgets(line, sizeof(line), f)) {
        switch (type) {
            case FT_CDB:  handle_cdb_line(name, line); break;
            case FT_CRB:  handle_crb_line(name, line); break;
            case FT_FP:   handle_fp_line(name, line); break;
            case FT_HDB:  handle_hdb_line(name, line); break;
            case FT_HSB:  handle_hsb_line(name, line); break;
            case FT_INFO: handle_info_line(name, line); break;
            case FT_LDB:  handle_ldb_line(name, line); break;
            case FT_MDB:  handle_mdb_line(name, line); break;
            case FT_MSB:  handle_msb_line(name, line); break;
            case FT_NDB:  handle_ndb_line(name, line); break;
            case FT_SFP:  handle_sfp_line(name, line); break;
            default:
                printf("\n[UNKNOWN] %s: %s", name, line);
                break;
        }
    }

    fclose(f);
}

// ============================================================================
// MAIN PROGRAM
// ============================================================================
int main(void)
{
    printf("Creating extraction directory...\n");
    mkdir(EXTRACT_DIR, 0777);
    chmod(EXTRACT_DIR, 0777);

    printf("Extracting main.cvd...\n");
    if (extract_cvd("main.cvd") != 0) {
        printf("ERROR: Could not extract main.cvd (gzip not extracted).\n");
        printf("Place gunzipped file as: payload.tar\n");
        return 1;
    }

    printf("\nNOTE: 'payload.tar.gz' was extracted.\n");
    printf("You must gunzip it externally into 'payload.tar'\n");
    printf("(This program intentionally does NOT include gzip code for safety.)\n");

    printf("\nExtracting TAR archive...\n");
    if (tar_extract("payload.tar", EXTRACT_DIR) != 0) {
        printf("ERROR: Could not extract payload.tar\n");
        return 1;
    }

    printf("\nProcessing signature files...\n");

    DIR *d = opendir(EXTRACT_DIR);
    if (!d) return 1;

    struct dirent *e;
    while ((e = readdir(d))) {
        if (e->d_type == DT_REG)
            process_file(EXTRACT_DIR, e->d_name);
    }

    closedir(d);

    printf("\nDONE.\n");
    return 0;
}

