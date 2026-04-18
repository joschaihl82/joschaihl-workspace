#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <zlib.h>
#include <errno.h>
#include <ctype.h>

#define HEADER_SIZE 512

//--------------------------------------------------------------
// CONFIG
//--------------------------------------------------------------
static const char *CVD_FILE = "main.cvd";
static const char *OUTPUT_DIR = "extracted";
static const uid_t TARGET_UID = 1000;
static const gid_t TARGET_GID = 1000;

//--------------------------------------------------------------
// TAR header
//--------------------------------------------------------------
struct tar_header {
    char name[100]; char mode[8]; char uid[8]; char gid[8];
    char size[12]; char mtime[12]; char checksum[8]; char typeflag;
    char linkname[100]; char magic[6]; char version[2];
    char uname[32]; char gname[32]; char devmajor[8]; char devminor[8];
    char prefix[155]; char padding[12];
};

static size_t oct2int(const char *str, size_t size) {
    size_t v = 0;
    for (size_t i = 0; i < size && str[i]; i++)
        if (str[i] >= '0' && str[i] <= '7') v = (v << 3) + (str[i] - '0');
    return v;
}

//--------------------------------------------------------------
// GUNZIP
//--------------------------------------------------------------
unsigned char *gunzip_memory(const unsigned char *src, size_t src_len, size_t *out_len) {
    z_stream s; memset(&s, 0, sizeof(s));
    inflateInit2(&s, 15 + 32); // auto-detect gzip wrapper

    size_t cap = src_len * 4;
    unsigned char *out = malloc(cap);
    if (!out) return NULL;

    s.next_in = (Bytef*)src;
    s.avail_in = src_len;
    size_t total = 0;

    for (;;) {
        if (total == cap) {
            cap *= 2;
            out = realloc(out, cap);
            if (!out) return NULL;
        }

        s.next_out = out + total;
        s.avail_out = cap - total;

        int ret = inflate(&s, Z_NO_FLUSH);
        total = cap - s.avail_out;

        if (ret == Z_STREAM_END) break;
        if (ret != Z_OK) {
            fprintf(stderr, "inflate error\n");
            free(out);
            return NULL;
        }
    }

    inflateEnd(&s);
    *out_len = total;
    return out;
}

//--------------------------------------------------------------
// TAR extraction
//--------------------------------------------------------------
int extract_tar(const unsigned char *tar, size_t len, const char *outdir) {
    size_t offset = 0;

    while (offset + 512 <= len) {
        struct tar_header *h = (struct tar_header *)(tar + offset);

        int zero = 1;
        for (int i = 0; i < 512; i++) if (tar[offset + i] != 0) { zero = 0; break; }
        if (zero) break;

        char filename[256];
        snprintf(filename, sizeof(filename), "%s/%s", outdir, h->name);
        size_t fsize = oct2int(h->size, sizeof(h->size));

        if (h->typeflag == '5') {
            mkdir(filename, 0777);
            chown(filename, TARGET_UID, TARGET_GID);
            chmod(filename, 0777);
        } else if (h->typeflag == '0' || h->typeflag == '\0') {
            char pathbuf[256];
            strcpy(pathbuf, filename);
            for (char *p = pathbuf + 1; *p; p++) if (*p == '/') { *p = 0; mkdir(pathbuf, 0777); chown(pathbuf, TARGET_UID, TARGET_GID); chmod(pathbuf, 0777); *p = '/'; }
            int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (fd >= 0) { write(fd, tar + offset + 512, fsize); close(fd); chown(filename, TARGET_UID, TARGET_GID); chmod(filename, 0666); }
        }

        size_t blocks = (fsize + 511) / 512;
        offset += 512 + blocks * 512;
    }
    return 0;
}

//--------------------------------------------------------------
// Helpers for interpretation
//--------------------------------------------------------------
int likely_text(const unsigned char *buf, size_t n) {
    size_t bad = 0;
    for (size_t i = 0; i < n; i++) {
        unsigned char c = buf[i];
        if (c == 9 || c == 10 || c == 13) continue;
        if (c >= 32 && c <= 126) continue;
        if (c == 0) { bad += 4; continue; }
        bad++;
    }
    return bad * 10 <= n;
}

void interpret_hdb_line(const char *line, unsigned long long *counter) {
    const char *p = strchr(line, ':');
    const char *q = p ? strchr(p + 1, ':') : NULL;
    printf("\n# Virus %llu (hdb)\n", (*counter)++);
    printf("%s\n", line);
}

void interpret_ndb_line(const char *line, unsigned long long *counter) {
    printf("\n# Virus %llu (ndb)\n", (*counter)++);
    printf("%s\n", line);
}

void interpret_ldb_file(const unsigned char *buf, size_t len, unsigned long long *counter) {
    printf("\n# Virus %llu (ldb)\n", (*counter)++);
    fwrite(buf, 1, len, stdout);
    printf("\n");
}

//--------------------------------------------------------------
// Main
//--------------------------------------------------------------
#include <dirent.h>

int main(void) {
    printf("Reading CVD file: %s\n", CVD_FILE);
    FILE *fp = fopen(CVD_FILE, "rb");
    if (!fp) { perror("open"); return 1; }

    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    unsigned char *data = malloc(size);
    fread(data, 1, size, fp);
    fclose(fp);

    // Skip header
    unsigned char *gzptr = data + HEADER_SIZE;
    size_t gzlen = size - HEADER_SIZE;

    if (!(gzptr[0] == 0x1F && gzptr[1] == 0x8B)) { fprintf(stderr, "No gzip payload\n"); free(data); return 1; }

    size_t tar_len;
    unsigned char *tar_data = gunzip_memory(gzptr, gzlen, &tar_len);
    free(data);
    if (!tar_data) { fprintf(stderr, "Gzip decompression failed\n"); return 1; }

    mkdir(OUTPUT_DIR, 0777);
    chown(OUTPUT_DIR, TARGET_UID, TARGET_GID);

    if (extract_tar(tar_data, tar_len, OUTPUT_DIR) != 0) { fprintf(stderr, "TAR extraction failed\n"); free(tar_data); return 1; }
    free(tar_data);
    printf("✓ Extraction complete: %s\n", OUTPUT_DIR);

    // Iterate through extracted files for virus listing
    unsigned long long virus_counter = 1;
    DIR *d = opendir(OUTPUT_DIR);
    if (!d) return 1;
    struct dirent *de;
    char path[512];

    while ((de = readdir(d))) {
        if (de->d_type != DT_REG) continue;
        snprintf(path, sizeof(path), "%s/%s", OUTPUT_DIR, de->d_name);

        FILE *f = fopen(path, "rb");
        if (!f) continue;

        fseek(f, 0, SEEK_END);
        size_t flen = ftell(f);
        fseek(f, 0, SEEK_SET);
        unsigned char *buf = malloc(flen + 1);
        fread(buf, 1, flen, f);
        fclose(f);
        buf[flen] = 0;

        if (strstr(de->d_name, ".hdb")) {
            char *saveptr = NULL, *line = strtok_r((char*)buf, "\n", &saveptr);
            while (line) { if (*line) interpret_hdb_line(line, &virus_counter); line = strtok_r(NULL, "\n", &saveptr); }
        } else if (strstr(de->d_name, ".ndb")) {
            char *saveptr = NULL, *line = strtok_r((char*)buf, "\n", &saveptr);
            while (line) { if (*line) interpret_ndb_line(line, &virus_counter); line = strtok_r(NULL, "\n", &saveptr); }
        } else if (strstr(de->d_name, ".ldb")) {
            interpret_ldb_file(buf, flen, &virus_counter);
        }

        free(buf);
    }
    closedir(d);

    printf("\n✓ Virus listing complete (%llu entries)\n", virus_counter - 1);
    return 0;
}

