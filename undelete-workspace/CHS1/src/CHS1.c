/*
 * recover_with_confidence.c
 *
 * TestDisk-like educational recovery tool with confidence scoring.
 *
 * Compile: gcc -O2 -o recover_with_confidence recover_with_confidence.c
 * Run: sudo ./recover_with_confidence /path/to/disk.img [--chs=C,H,S] [--scan-only]
 *
 * Read-only. Works on disk images or block devices.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#define SECTOR_SIZE 512
#define SCAN_BLOCK (1024*1024)
#define MAX_CANDIDATES 8192
#define GPT_HEADER_LBA 1
#define GPT_PART_ENTRY_SIZE 128
#define GPT_PART_ENTRIES 128

struct mbr_part {
    uint8_t boot;
    uint8_t chs_first[3];
    uint8_t type;
    uint8_t chs_last[3];
    uint32_t lba_first;
    uint32_t sectors;
} __attribute__((packed));

struct mbr {
    uint8_t bootcode[446];
    struct mbr_part part[4];
    uint16_t signature;
} __attribute__((packed));

struct gpt_header {
    uint8_t sig[8];
    uint32_t revision;
    uint32_t header_size;
    uint32_t header_crc32;
    uint32_t reserved;
    uint64_t my_lba;
    uint64_t alt_lba;
    uint64_t first_usable_lba;
    uint64_t last_usable_lba;
    uint8_t disk_guid[16];
    uint64_t part_entry_lba;
    uint32_t num_part_entries;
    uint32_t part_entry_size;
    uint32_t part_array_crc32;
} __attribute__((packed));

typedef struct {
    off_t offset;
    char fsname[16];
    uint8_t raw[SECTOR_SIZE];
    time_t timestamp;
    char note[256];
    int confidence;
    off_t length_est; /* optional estimated length for overlap checks */
} candidate_t;

static candidate_t candidates[MAX_CANDIDATES];
static int candidate_count = 0;

/* file signatures */
static const uint8_t sig_jpg[] = {0xFF,0xD8,0xFF};
static const uint8_t sig_png[] = {0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A};
static const uint8_t sig_pdf[] = {0x25,0x50,0x44,0x46,0x2D};
static const uint8_t sig_zip[] = {0x50,0x4B,0x03,0x04};

ssize_t pread_full(int fd, void *buf, size_t count, off_t offset) {
    size_t total = 0;
    while (total < count) {
        ssize_t r = pread(fd, (char*)buf + total, count - total, offset + total);
        if (r < 0) return -1;
        if (r == 0) break;
        total += r;
    }
    return total;
}

int add_candidate(off_t offset, const char *fsname, const uint8_t *raw, const char *note, off_t length_est) {
    for (int i = 0; i < candidate_count; ++i) {
        if (candidates[i].offset == offset) {
            if (note && note[0]) {
                strncat(candidates[i].note, " | ", sizeof(candidates[i].note)-strlen(candidates[i].note)-1);
                strncat(candidates[i].note, note, sizeof(candidates[i].note)-strlen(candidates[i].note)-1);
            }
            if (length_est > 0) candidates[i].length_est = length_est;
            return i;
        }
    }
    if (candidate_count >= MAX_CANDIDATES) return -1;
    candidate_t *c = &candidates[candidate_count++];
    c->offset = offset;
    strncpy(c->fsname, fsname ? fsname : "UNKNOWN", sizeof(c->fsname)-1);
    if (raw) memcpy(c->raw, raw, SECTOR_SIZE);
    c->timestamp = 0;
    c->note[0] = '\0';
    if (note) strncpy(c->note, note, sizeof(c->note)-1);
    c->confidence = 0;
    c->length_est = length_est;
    return candidate_count-1;
}

/* CHS conversion */
uint64_t chs_to_lba(uint8_t chs[3], uint32_t heads, uint32_t sectors_per_track) {
    uint32_t head = chs[0];
    uint32_t sector = chs[1] & 0x3F;
    uint32_t cyl = ((chs[1] & 0xC0) << 2) | chs[2];
    if (sectors_per_track == 0 || heads == 0) return 0;
    uint64_t lba = ((uint64_t)cyl * heads + head) * sectors_per_track + (sector ? (sector - 1) : 0);
    return lba;
}

void guess_chs_from_size(off_t disk_size, uint32_t *out_c, uint32_t *out_h, uint32_t *out_s) {
    uint64_t total_sectors = disk_size / SECTOR_SIZE;
    *out_h = 255; *out_s = 63;
    *out_c = (uint32_t)(total_sectors / (*out_h * *out_s));
    if (*out_c == 0) { *out_h = 16; *out_s = 63; *out_c = (uint32_t)(total_sectors / (*out_h * *out_s)); }
    if (*out_c == 0) *out_c = 1;
}

/* Basic heuristics for filesystem checks */
int is_ntfs_boot(const uint8_t *s) { return memcmp(s + 3, "NTFS", 4) == 0; }
int is_fat_boot(const uint8_t *s) { return (s[0] == 0xEB || s[0] == 0xE9) && (s[2] == 0x90 || s[2] == 0x00); }
int is_ext_superblock(const uint8_t *buf, size_t bufsize, size_t rel_offset) {
    if (rel_offset + 56 + 2 >= bufsize) return 0;
    return buf[rel_offset + 56] == 0x53 && buf[rel_offset + 57] == 0xEF;
}

/* Compute confidence for a single candidate */
int compute_confidence_for(int idx) {
    candidate_t *c = &candidates[idx];
    int score = 0;
    /* Strong signature */
    if (strcmp(c->fsname, "NTFS") == 0 || is_ntfs_boot(c->raw)) score += 30;
    if (strcmp(c->fsname, "FAT") == 0 || is_fat_boot(c->raw)) score += 20;
    if (strcmp(c->fsname, "EXT") == 0) score += 25;
    if (strcmp(c->fsname, "MBR_PART") == 0 || strcmp(c->fsname, "GPT_PART") == 0) score += 20;
    /* Alignment: 1 MiB alignment is common */
    if ((c->offset % (1024*1024)) == 0) score += 10;
    /* Filesystem-specific heuristics */
    if (is_ntfs_boot(c->raw)) {
        uint64_t total_sectors = 0;
        memcpy(&total_sectors, c->raw + 40, sizeof(uint64_t));
        uint64_t mft_lcn = 0;
        memcpy(&mft_lcn, c->raw + 48, sizeof(uint64_t));
        if (total_sectors > 0 && total_sectors < (1ULL<<50)) score += 5;
        if (mft_lcn > 0 && mft_lcn < total_sectors) score += 10;
    }
    if (is_fat_boot(c->raw)) {
        uint16_t bytes_per_sector = c->raw[11] | (c->raw[12] << 8);
        uint16_t root_entries = c->raw[17] | (c->raw[18] << 8);
        if (bytes_per_sector >= 512 && bytes_per_sector <= 4096) score += 5;
        if (root_entries > 0 && root_entries < 65536) score += 5;
    }
    /* EXT: check superblock mtime plausibility */
    /* read superblock at offset + 1024 */
    /* we only have sector-sized raw; skip heavy reads here */
    if (strcmp(c->fsname, "EXT") == 0) score += 5;
    /* Partition-table agreement: if note contains MBR/GPT markers */
    if (strstr(c->note, "MBR") || strstr(c->note, "GPT")) score += 20;
    /* Penalize overlaps: if another candidate overlaps within estimated length */
    for (int j = 0; j < candidate_count; ++j) {
        if (j == idx) continue;
        off_t a1 = c->offset;
        off_t a2 = c->length_est ? c->offset + c->length_est : c->offset + SECTOR_SIZE;
        off_t b1 = candidates[j].offset;
        off_t b2 = candidates[j].length_est ? candidates[j].offset + candidates[j].length_est : candidates[j].offset + SECTOR_SIZE;
        if (!(a2 <= b1 || b2 <= a1)) {
            score -= 10;
            break;
        }
    }
    if (score < 0) score = 0;
    if (score > 100) score = 100;
    return score;
}

/* Sorting by confidence descending */
int cmp_conf(const void *a, const void *b) {
    const candidate_t *ca = a;
    const candidate_t *cb = b;
    return cb->confidence - ca->confidence;
}

/* MBR parsing with CHS reinterpretation */
void parse_mbr(int fd, off_t disk_size, int chs_override[3]) {
    struct mbr m;
    if (pread_full(fd, &m, sizeof(m), 0) != sizeof(m)) {
        fprintf(stderr, "Failed to read MBR: %s\n", strerror(errno));
        return;
    }
    if (m.signature != 0xAA55) {
        fprintf(stderr, "No valid MBR signature found\n");
        return;
    }
    uint32_t heads = 255, sectors = 63, cyls = 0;
    if (chs_override && chs_override[0] > 0) {
        cyls = chs_override[0];
        heads = chs_override[1];
        sectors = chs_override[2];
    } else {
        guess_chs_from_size(disk_size, &cyls, &heads, &sectors);
    }
    for (int i = 0; i < 4; ++i) {
        struct mbr_part *p = &m.part[i];
        if (p->type == 0 || p->sectors == 0) continue;
        uint64_t start = p->lba_first;
        uint64_t size = p->sectors;
        off_t offset = (off_t)start * SECTOR_SIZE;
        if (offset + SECTOR_SIZE <= disk_size) {
            uint8_t raw[SECTOR_SIZE];
            pread_full(fd, raw, SECTOR_SIZE, offset);
            char note[128];
            snprintf(note, sizeof(note), "MBR partition idx=%d type=0x%02X", i+1, p->type);
            add_candidate(offset, "MBR_PART", raw, note, size * SECTOR_SIZE);
        }
        uint64_t chs_start = chs_to_lba(p->chs_first, heads, sectors);
        if (chs_start != start) {
            off_t chs_offset = chs_start * SECTOR_SIZE;
            if (chs_offset + SECTOR_SIZE <= disk_size) {
                uint8_t raw2[SECTOR_SIZE];
                pread_full(fd, raw2, SECTOR_SIZE, chs_offset);
                char note2[128];
                snprintf(note2, sizeof(note2), "CHS reinterpretation suggests LBA=%" PRIu64, (uint64_t)chs_start);
                add_candidate(chs_offset, "MBR_CHS", raw2, note2, size * SECTOR_SIZE);
            }
        }
    }
}

/* GPT parsing */
void parse_gpt(int fd, off_t disk_size) {
    uint8_t hdrbuf[SECTOR_SIZE];
    off_t hdr_off = GPT_HEADER_LBA * SECTOR_SIZE;
    if (pread_full(fd, hdrbuf, SECTOR_SIZE, hdr_off) != SECTOR_SIZE) return;
    struct gpt_header gh;
    memcpy(&gh, hdrbuf, sizeof(gh));
    if (memcmp(gh.sig, "EFI PART", 8) != 0) return;
    uint32_t entries = gh.num_part_entries ? gh.num_part_entries : GPT_PART_ENTRIES;
    uint32_t entry_size = gh.part_entry_size ? gh.part_entry_size : GPT_PART_ENTRY_SIZE;
    uint64_t part_lba = gh.part_entry_lba;
    size_t table_bytes = (size_t)entries * entry_size;
    uint8_t *table = malloc(table_bytes);
    if (!table) return;
    off_t table_off = part_lba * SECTOR_SIZE;
    if (pread_full(fd, table, table_bytes, table_off) != (ssize_t)table_bytes) { free(table); return; }
    for (uint32_t i = 0; i < entries; ++i) {
        uint8_t *ent = table + (size_t)i * entry_size;
        int allzero = 1;
        for (uint32_t j = 0; j < 16; ++j) if (ent[j]) { allzero = 0; break; }
        if (allzero) continue;
        uint64_t first = *(uint64_t*)(ent + 32);
        uint64_t last = *(uint64_t*)(ent + 40);
        char name[73]; name[0] = '\0';
        for (int k = 0; k < 36; ++k) {
            uint16_t ch = ent[56 + k*2] | (ent[56 + k*2 + 1] << 8);
            name[k] = (ch >= 32 && ch < 127) ? (char)ch : '?';
            name[k+1] = '\0';
        }
        off_t offset = (off_t)first * SECTOR_SIZE;
        char note[128];
        snprintf(note, sizeof(note), "GPT partition idx=%u name=%s", i+1, name);
        if (offset + SECTOR_SIZE <= disk_size) {
            uint8_t raw[SECTOR_SIZE];
            pread_full(fd, raw, SECTOR_SIZE, offset);
            add_candidate(offset, "GPT_PART", raw, note, (last - first + 1) * SECTOR_SIZE);
        }
    }
    free(table);
}

/* Deeper scan for boot signatures */
void deeper_scan(int fd, off_t disk_size) {
    uint8_t *buf = malloc(SCAN_BLOCK);
    if (!buf) return;
    off_t offset = 0;
    while (offset < disk_size) {
        size_t toread = SCAN_BLOCK;
        if (offset + toread > disk_size) toread = disk_size - offset;
        ssize_t r = pread_full(fd, buf, toread, offset);
        if (r <= 0) break;
        for (size_t i = 0; i + SECTOR_SIZE <= (size_t)r; i += SECTOR_SIZE) {
            uint8_t *s = buf + i;
            if (memcmp(s + 3, "NTFS", 4) == 0) {
                add_candidate(offset + i, "NTFS", s, "NTFS boot sector", 0);
            }
            if ((s[0] == 0xEB || s[0] == 0xE9) && (s[2] == 0x90 || s[2] == 0x00)) {
                add_candidate(offset + i, "FAT", s, "FAT boot sector", 0);
            }
            if (i + 1024 + 56 < (size_t)r) {
                if (buf[i + 1024 + 56] == 0x53 && buf[i + 1024 + 57] == 0xEF) {
                    add_candidate(offset + i, "EXT", s, "EXT superblock candidate", 0);
                }
            }
        }
        offset += toread;
    }
    free(buf);
}

/* Carving simplified (unchanged) */
void carve_files(int fd, off_t disk_size) {
    const size_t bufsize = 4 * 1024 * 1024;
    uint8_t *buf = malloc(bufsize);
    if (!buf) return;
    off_t offset = 0;
    int filecount = 0;
    mkdir("recovered", 0755);
    while (offset < disk_size) {
        size_t toread = bufsize;
        if (offset + toread > disk_size) toread = disk_size - offset;
        ssize_t r = pread_full(fd, buf, toread, offset);
        if (r <= 0) break;
        for (ssize_t i = 0; i < r; ++i) {
            if (i + 3 <= r && memcmp(buf + i, sig_jpg, 3) == 0) {
                off_t start = offset + i;
                off_t end = start + (1<<20);
                if (end > disk_size) end = disk_size;
                char outname[256];
                snprintf(outname, sizeof(outname), "recovered/file_%04d.jpg", ++filecount);
                FILE *out = fopen(outname, "wb");
                if (!out) continue;
                off_t remain = end - start;
                uint8_t *wbuf = malloc(64*1024);
                off_t pos = start;
                while (remain > 0) {
                    size_t to = remain > 64*1024 ? 64*1024 : remain;
                    ssize_t rr = pread_full(fd, wbuf, to, pos);
                    if (rr <= 0) break;
                    fwrite(wbuf, 1, rr, out);
                    pos += rr;
                    remain -= rr;
                }
                free(wbuf);
                fclose(out);
                printf("Carved %s\n", outname);
            }
        }
        offset += toread;
    }
    free(buf);
}

/* Report: compute confidence, sort, and print */
void report_and_extract(int fd, off_t disk_size) {
    for (int i = 0; i < candidate_count; ++i) {
        candidates[i].confidence = compute_confidence_for(i);
    }
    qsort(candidates, candidate_count, sizeof(candidate_t), cmp_conf);
    printf("\n=== Candidates (sorted by confidence) ===\n");
    for (int i = 0; i < candidate_count; ++i) {
        candidate_t *c = &candidates[i];
        printf("Candidate %d: offset=%" PRIu64 " fs=%s confidence=%d note=%s\n",
               i+1, (uint64_t)c->offset, c->fsname, c->confidence, c->note);
    }
    printf("\nStarting file carving. Output directory: recovered/\n");
    carve_files(fd, disk_size);
}

/* Get disk size */
off_t get_disk_size(int fd) {
    struct stat st;
    if (fstat(fd, &st) < 0) return -1;
    if (S_ISREG(st.st_mode)) return st.st_size;
#ifdef BLKGETSIZE64
    #include <sys/ioctl.h>
    #include <linux/fs.h>
    unsigned long long size = 0;
    if (ioctl(fd, BLKGETSIZE64, &size) == 0) return (off_t)size;
#endif
    off_t cur = lseek(fd, 0, SEEK_CUR);
    off_t end = lseek(fd, 0, SEEK_END);
    if (cur >= 0) lseek(fd, cur, SEEK_SET);
    return end;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <disk-image-or-device> [--chs=C,H,S] [--scan-only]\n", argv[0]);
        return 1;
    }
    const char *path = argv[1];
    int chs_override[3] = {0,0,0};
    int scan_only = 0;
    for (int i = 2; i < argc; ++i) {
        if (strncmp(argv[i], "--chs=", 6) == 0) {
            int c,h,s;
            if (sscanf(argv[i]+6, "%d,%d,%d", &c, &h, &s) == 3) {
                chs_override[0] = c; chs_override[1] = h; chs_override[2] = s;
            }
        } else if (strcmp(argv[i], "--scan-only") == 0) {
            scan_only = 1;
        }
    }
    int fd = open(path, O_RDONLY);
    if (fd < 0) { perror("open"); return 1; }
    off_t disk_size = get_disk_size(fd);
    if (disk_size <= 0) { fprintf(stderr, "Failed to determine disk size\n"); close(fd); return 1; }
    printf("Opened %s size=%" PRIu64 " bytes\n", path, (uint64_t)disk_size);
    if (!scan_only) parse_mbr(fd, disk_size, chs_override);
    parse_gpt(fd, disk_size);
    deeper_scan(fd, disk_size);
    report_and_extract(fd, disk_size);
    close(fd);
    printf("Done. Recovered files are in ./recovered\n");
    return 0;
}
