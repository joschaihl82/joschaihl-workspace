/*
 * scan_ext4_btrfs.c
 *
 * Single-file, plain C (POSIX) tool that:
 *  - parses ext4 superblock/group descriptors/inode tables and prints directory entries
 *  - performs a heuristic raw scan for printable filename-like strings on btrfs images
 *
 * Limitations:
 *  - ext4: supports classic linear directory entries parsing only; does not implement extent/indexed directory special cases
 *  - btrfs: heuristic scan only; full btrfs B-tree parsing is not implemented
 *
 * Build: gcc -O2 -o scan_ext4_btrfs scan_ext4_btrfs.c
 * Usage: sudo ./scan_ext4_btrfs /dev/sdXN
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

/* --- ext4 on-disk structures (minimal) --- */

/* ext4 superblock is at offset 1024 */
#define EXT4_SUPER_OFFSET 1024
#define EXT4_SUPER_SIZE   1024
#define EXT4_NAME_LEN     255

/* on-disk superblock (only fields we need) */
struct ext4_super {
    uint32_t s_inodes_count;
    uint32_t s_blocks_count_lo;
    uint32_t s_r_blocks_count_lo;
    uint32_t s_free_blocks_count_lo;
    uint32_t s_free_inodes_count;
    uint32_t s_first_data_block;
    uint32_t s_log_block_size;
    uint32_t s_log_cluster_size;
    uint32_t s_blocks_per_group;
    uint32_t s_clusters_per_group;
    uint32_t s_inodes_per_group;
    uint32_t s_mtime;
    uint32_t s_wtime;
    uint16_t s_mnt_count;
    uint16_t s_max_mnt_count;
    uint16_t s_magic;
    uint16_t s_state;
    uint16_t s_errors;
    uint16_t s_minor_rev_level;
    uint32_t s_lastcheck;
    uint32_t s_checkinterval;
    uint32_t s_creator_os;
    uint32_t s_rev_level;
    uint16_t s_def_resuid;
    uint16_t s_def_resgid;
    /* ext4 additions */
    uint32_t s_first_ino;
    uint16_t s_inode_size;
    uint16_t s_block_group_nr;
    uint32_t s_feature_compat;
    uint32_t s_feature_incompat;
    uint32_t s_feature_ro_compat;
    uint8_t  s_uuid[16];
    char     s_volume_name[16];
    char     s_last_mounted[64];
    /* ... rest omitted */
} __attribute__((packed));

/* group descriptor (minimal) */
struct ext4_group_desc {
    uint32_t bg_block_bitmap_lo;
    uint32_t bg_inode_bitmap_lo;
    uint32_t bg_inode_table_lo;
    uint16_t bg_free_blocks_count_lo;
    uint16_t bg_free_inodes_count_lo;
    uint16_t bg_used_dirs_count_lo;
    uint16_t bg_flags;
    uint32_t bg_exclude_bitmap_lo;
    uint16_t bg_block_bitmap_csum_lo;
    uint16_t bg_inode_bitmap_csum_lo;
    uint16_t bg_itable_unused;
    uint16_t bg_checksum;
} __attribute__((packed));

/* on-disk inode (minimal fields) */
struct ext4_inode {
    uint16_t i_mode;
    uint16_t i_uid;
    uint32_t i_size_lo;
    uint32_t i_atime;
    uint32_t i_ctime;
    uint32_t i_mtime;
    uint32_t i_dtime;
    uint16_t i_gid;
    uint16_t i_links_count;
    uint32_t i_blocks_lo;
    uint32_t i_flags;
    uint32_t i_osd1;
    uint32_t i_block[15];
    uint32_t i_generation;
    uint32_t i_file_acl_lo;
    uint32_t i_size_high;
    uint32_t i_obso_faddr;
    uint8_t  i_osd2[12];
} __attribute__((packed));

/* ext2/3/4 directory entry (version 2) */
struct ext2_dir_entry {
    uint32_t inode;
    uint16_t rec_len;
    uint8_t  name_len;
    uint8_t  file_type;
    char     name[]; /* not NUL-terminated */
} __attribute__((packed));

/* --- helpers --- */

static ssize_t pread_all(int fd, void *buf, size_t count, off_t offset) {
    size_t done = 0;
    while (done < count) {
        ssize_t r = pread(fd, (char*)buf + done, count - done, offset + done);
        if (r <= 0) return r;
        done += r;
    }
    return done;
}

static uint16_t le16(const void *p) {
    const uint8_t *b = p;
    return (uint16_t)b[0] | ((uint16_t)b[1] << 8);
}
static uint32_t le32(const void *p) {
    const uint8_t *b = p;
    return (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
}

/* --- ext4 scanner --- */

static void scan_ext4(int fd) {
    struct ext4_super sb;
    if (pread_all(fd, &sb, sizeof(sb), EXT4_SUPER_OFFSET) != sizeof(sb)) {
        fprintf(stderr, "Failed to read ext4 superblock: %s\n", strerror(errno));
        return;
    }

    if (le16(&sb.s_magic) != 0xEF53) {
        fprintf(stderr, "No ext filesystem magic found\n");
        return;
    }

    uint32_t inodes_count = le32(&sb.s_inodes_count);
    uint32_t inodes_per_group = le32(&sb.s_inodes_per_group);
    uint32_t first_ino = le32(&sb.s_first_ino);
    uint16_t inode_size = le16(&sb.s_inode_size);
    if (inode_size == 0) inode_size = 128;

    uint32_t log_block = le32(&sb.s_log_block_size);
    uint32_t block_size = 1024U << log_block;

    printf("EXT4 detected: inodes=%u inodes_per_group=%u inode_size=%u block_size=%u\n",
           inodes_count, inodes_per_group, inode_size, block_size);

    /* group descriptor table follows the superblock; its location depends on block size */
    off_t gd_table_off;
    if (block_size == 1024) gd_table_off = 2 * block_size; else gd_table_off = block_size;
    uint32_t groups_count = (le32(&sb.s_blocks_count_lo) + le32(&sb.s_blocks_count_lo) - 1) / le32(&sb.s_blocks_per_group);
    if (groups_count == 0) groups_count = (inodes_count + inodes_per_group - 1) / inodes_per_group;

    /* read group descriptors (read a reasonable number) */
    size_t gd_size = sizeof(struct ext4_group_desc);
    size_t gdt_bytes = groups_count * gd_size;
    uint8_t *gdt = malloc(gdt_bytes);
    if (!gdt) return;
    if (pread_all(fd, gdt, gdt_bytes, gd_table_off) != (ssize_t)gdt_bytes) {
        /* if we can't read full table, try reading fewer groups */
        /* fall back to reading first group only */
    }

    /* iterate groups and inodes */
    for (uint32_t g = 0; g < groups_count; ++g) {
        struct ext4_group_desc *gd = (struct ext4_group_desc *)(gdt + g * gd_size);
        uint32_t itable_lo = le32(&gd->bg_inode_table_lo);
        if (itable_lo == 0) continue;
        off_t inode_table_off = (off_t)itable_lo * block_size;

        /* number of inodes in this group */
        uint32_t inodes_in_group = inodes_per_group;
        uint32_t start_inode = g * inodes_per_group + 1;
        if (start_inode + inodes_in_group - 1 > inodes_count)
            inodes_in_group = inodes_count - (start_inode - 1);

        /* read each inode */
        for (uint32_t i = 0; i < inodes_in_group; ++i) {
            off_t inode_off = inode_table_off + (off_t)i * inode_size;
            uint8_t *inode_buf = malloc(inode_size);
            if (!inode_buf) continue;
            if (pread_all(fd, inode_buf, inode_size, inode_off) != (ssize_t)inode_size) {
                free(inode_buf);
                continue;
            }
            struct ext4_inode inode;
            memcpy(&inode, inode_buf, sizeof(struct ext4_inode) < inode_size ? sizeof(struct ext4_inode) : inode_size);
            free(inode_buf);

            uint16_t mode = le16(&inode.i_mode);
            if ((mode & 0xF000) == 0x4000) { /* directory */
                /* iterate data blocks in i_block[] (only direct blocks handled here) */
                for (int bi = 0; bi < 12; ++bi) {
                    uint32_t blk = le32(&inode.i_block[bi]);
                    if (blk == 0) continue;
                    off_t block_off = (off_t)blk * block_size;
                    uint8_t *block = malloc(block_size);
                    if (!block) continue;
                    if (pread_all(fd, block, block_size, block_off) != (ssize_t)block_size) {
                        free(block);
                        continue;
                    }
                    /* iterate directory entries */
                    uint32_t pos = 0;
                    while (pos + sizeof(struct ext2_dir_entry) < block_size) {
                        struct ext2_dir_entry *de = (struct ext2_dir_entry *)(block + pos);
                        uint32_t inode_no = le32(&de->inode);
                        uint16_t rec_len = le16(&de->rec_len);
                        uint8_t name_len = de->name_len;
                        if (rec_len == 0) break;
                        if (inode_no != 0 && name_len > 0 && name_len <= EXT4_NAME_LEN && pos + rec_len <= block_size) {
                            char name[EXT4_NAME_LEN + 1];
                            if (name_len > EXT4_NAME_LEN) name_len = EXT4_NAME_LEN;
                            memcpy(name, de->name, name_len);
                            name[name_len] = '\0';
                            printf("ext4: dir_inode=%u entry_inode=%u name=%s\n",
                                   (unsigned)(g * inodes_per_group + i + 1),
                                   (unsigned)inode_no,
                                   name);
                        }
                        pos += rec_len;
                    }
                    free(block);
                }
            }
        }
    }

    free(gdt);
}

/* --- btrfs heuristic scanner --- */

/*
 * btrfs superblock is typically at 64KiB offset (and mirrored). We locate it
 * by scanning a few known offsets and then perform a raw printable-string scan
 * across the device to find candidate filenames.
 *
 * This is a heuristic only. Proper btrfs recovery requires parsing B-trees and
 * decoding directory items.
 */
static void scan_btrfs_heuristic(int fd) {
    const off_t candidates[] = { 65536, 65536 * 2, 65536 * 3, 0 }; /* check first few */
    char buf[16];
    int found = 0;
    for (int i = 0; candidates[i] != 0; ++i) {
        if (pread_all(fd, buf, sizeof(buf), candidates[i]) == (ssize_t)sizeof(buf)) {
            if (memcmp(buf, "BTRFS\x00\x00\x00", 8) == 0) {
                printf("btrfs superblock found at offset %lld\n", (long long)candidates[i]);
                found = 1;
                break;
            }
        }
    }
    if (!found) {
        /* try scanning first 1MB for the magic */
        off_t scan_end = 1024 * 1024;
        uint8_t *sbuf = malloc(scan_end);
        if (!sbuf) return;
        if (pread_all(fd, sbuf, scan_end, 0) == (ssize_t)scan_end) {
            for (off_t o = 0; o < scan_end - 8; ++o) {
                if (memcmp(sbuf + o, "BTRFS\x00\x00\x00", 8) == 0) {
                    printf("btrfs superblock found at offset %lld\n", (long long)o);
                    found = 1;
                    break;
                }
            }
        }
        free(sbuf);
    }
    if (!found) {
        fprintf(stderr, "No btrfs superblock found in scanned areas; continuing heuristic scan\n");
    }

    /* Heuristic: scan the whole device in chunks and print printable strings of length >= 4 and <=255 */
    const size_t CHUNK = 65536;
    uint8_t *chunk = malloc(CHUNK);
    if (!chunk) return;
    off_t off = 0;
    ssize_t r;
    while ((r = pread(fd, chunk, CHUNK, off)) > 0) {
        size_t i = 0;
        while (i < (size_t)r) {
            /* find run of printable bytes excluding '/' and NUL; allow UTF-8 bytes as printable if >= 0x20 */
            size_t j = i;
            while (j < (size_t)r && chunk[j] >= 0x20 && chunk[j] != 0x7F) j++;
            size_t len = j - i;
            if (len >= 4 && len <= 255) {
                /* filter out runs that contain control chars or many non-filename chars */
                int slash = 0;
                int okchars = 0;
                for (size_t k = i; k < j; ++k) {
                    if (chunk[k] == '/') slash = 1;
                    if (isprint(chunk[k])) okchars++;
                }
                if (!slash && okchars >= (int)len) {
                    char *s = malloc(len + 1);
                    if (s) {
                        memcpy(s, chunk + i, len);
                        s[len] = '\0';
                        printf("btrfs-heuristic: offset=%lld name=%s\n", (long long)(off + i), s);
                        free(s);
                    }
                }
            }
            i = (j < (size_t)r) ? j + 1 : j;
        }
        off += r;
    }
    free(chunk);
}

/* --- main --- */

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <device-or-image>\n", argv[0]);
        return 1;
    }
    const char *path = argv[1];
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Failed to open %s: %s\n", path, strerror(errno));
        return 2;
    }

    printf("Scanning %s\n", path);
    /* Try ext4 first */
    scan_ext4(fd);

    /* Then run btrfs heuristic scan */
    scan_btrfs_heuristic(fd);

    close(fd);
    return 0;
}
