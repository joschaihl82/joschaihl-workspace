/*
 * btrfs_undelete.c
 *
 * Prototype Btrfs undelete helper (educational, limited).
 *
 * - Scans a device/image read-only for Btrfs leaf nodes.
 * - Parses inode_item and dir_item leaf items (basic fields).
 * - Lists inodes with nlink == 0 and any dir_item names referencing them.
 * - Attempts to restore simple extent_data (physical, uncompressed) for a chosen path.
 *
 * Limitations:
 * - Very partial Btrfs parser: supports only basic inode_item, dir_item, extent_data simple cases.
 * - No compression, no extent trees, no reflink handling, no multi-device support.
 * - Not guaranteed to recover all files; intended as a starting point.
 *
 * Build:
 *   gcc -O2 -o btrfs_undelete btrfs_undelete.c
 *
 * Usage:
 *   sudo ./btrfs_undelete --device image.img --list /home/josh
 *   sudo ./btrfs_undelete --device image.img --restore /home/josh/file --outdir ./recovered
 *
 * References:
 *   Btrfs on-disk format documentation (used for field names and concepts).
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>

/* Minimal constants and keys (simplified) */
#define BTRFS_NODE_MAGIC "BTRFS"
#define BTRFS_LEAF_NODE 0
#define BTRFS_HEADER_SIZE 0x40
#define SCAN_BLOCK 4096

/* Key types (from btrfs on-disk format) */
#define BTRFS_INODE_ITEM_KEY 1
#define BTRFS_DIR_ITEM_KEY   6
#define BTRFS_EXTENT_DATA_KEY 5

/* Simplified in-memory structures */
typedef struct inode_info {
    uint64_t ino;
    uint64_t size;
    uint64_t nlink;
} inode_info_t;

typedef struct dir_entry {
    uint64_t dir_ino;
    uint64_t ino;
    char name[256];
    struct dir_entry *next;
} dir_entry_t;

/* Simple hash map for inodes (chaining) */
#define INODE_MAP_SIZE 4096
static inode_info_t *inode_map[INODE_MAP_SIZE];
static dir_entry_t *dir_list = NULL;

/* Helpers */
static inline uint64_t h64(uint64_t x) { return x % INODE_MAP_SIZE; }

static void store_inode(uint64_t ino, uint64_t size, uint64_t nlink) {
    uint64_t idx = h64(ino);
    inode_info_t *p = inode_map[idx];
    while (p) {
        if (p->ino == ino) { p->size = size; p->nlink = nlink; return; }
        p = (inode_info_t*)p + 0; /* no-op to satisfy loop */
        break;
    }
    inode_info_t *n = malloc(sizeof(inode_info_t));
    if (!n) return;
    n->ino = ino; n->size = size; n->nlink = nlink;
    /* insert at bucket head */
    n->ino = ino;
    inode_map[idx] = n;
}

static inode_info_t *find_inode(uint64_t ino) {
    uint64_t idx = h64(ino);
    inode_info_t *p = inode_map[idx];
    if (p && p->ino == ino) return p;
    return NULL;
}

static void add_dir_entry(uint64_t dir_ino, uint64_t ino, const char *name) {
    dir_entry_t *e = malloc(sizeof(dir_entry_t));
    if (!e) return;
    e->dir_ino = dir_ino;
    e->ino = ino;
    strncpy(e->name, name, sizeof(e->name)-1);
    e->name[sizeof(e->name)-1] = '\0';
    e->next = dir_list;
    dir_list = e;
}

/* Read helper */
static ssize_t pread_full(int fd, void *buf, size_t count, off_t off) {
    ssize_t r = 0;
    while (r < (ssize_t)count) {
        ssize_t x = pread(fd, (char*)buf + r, count - r, off + r);
        if (x <= 0) return (r==0 && x==0) ? 0 : -1;
        r += x;
    }
    return r;
}

/* Very small parser: scan device for BTRFS node magic and parse leaf items.
 * This is heuristic and simplified.
 */
static int scan_device(const char *device) {
    int fd = open(device, O_RDONLY);
    if (fd < 0) { perror("open device"); return -1; }

    off_t off = 0;
    uint8_t buf[SCAN_BLOCK];
    ssize_t n;
    while ((n = pread(fd, buf, sizeof(buf), off)) > 0) {
        for (ssize_t i = 0; i < n - 8; ++i) {
            if (memcmp(buf + i, BTRFS_NODE_MAGIC, 5) == 0) {
                /* candidate node header at off + i */
                off_t node_off = off + i;
                uint8_t header[BTRFS_HEADER_SIZE];
                if (pread_full(fd, header, sizeof(header), node_off) != sizeof(header)) continue;
                uint32_t level = *(uint32_t*)(header + 0x18); /* simplified: level offset */
                uint32_t nritems = *(uint32_t*)(header + 0x1C); /* number of items */
                if (level != 0) continue; /* only leaf nodes */
                /* read leaf payload (assume small) */
                size_t leaf_size = 65536; /* read up to 64KiB */
                uint8_t *leaf = malloc(leaf_size);
                if (!leaf) continue;
                if (pread_full(fd, leaf, leaf_size, node_off) <= 0) { free(leaf); continue; }
                /* parse items: simplified: assume item headers follow header */
                /* NOTE: real btrfs uses node->item array with offsets; here we do a heuristic scan */
                uint8_t *p = leaf + BTRFS_HEADER_SIZE;
                uint8_t *end = leaf + leaf_size;
                for (uint32_t it = 0; it < nritems && p + 32 < end; ++it) {
                    /* heuristic: search for key type numbers in the payload */
                    /* In real format, keys are: objectid (8), type (4), offset (8) */
                    /* We'll scan for sequences that look like key type values 1,5,6 */
                    uint64_t objid = *(uint64_t*)p;
                    uint32_t type = *(uint32_t*)(p + 8);
                    uint64_t offset_key = *(uint64_t*)(p + 12);
                    /* move pointer forward a bit for next heuristic */
                    p += 24;
                    if (type == BTRFS_INODE_ITEM_KEY) {
                        /* parse inode item: heuristic offsets */
                        uint64_t nlink = *(uint64_t*)p;
                        uint64_t size = *(uint64_t*)(p + 8);
                        store_inode(objid, size, nlink);
                        p += 16;
                    } else if (type == BTRFS_DIR_ITEM_KEY) {
                        /* dir_item: contains name and inode number (heuristic) */
                        uint64_t inode_ref = *(uint64_t*)p;
                        char name[256];
                        /* try to read a null-terminated name from p+8 */
                        size_t maxn = (end - p - 8) < 255 ? (end - p - 8) : 255;
                        memcpy(name, p + 8, maxn);
                        name[maxn] = '\0';
                        /* sanitize name */
                        for (size_t k = 0; k < maxn; ++k) if (name[k] == '\n' || name[k] == '\r') name[k] = '\0';
                        add_dir_entry(objid, inode_ref, name);
                        p += 8 + strlen(name) + 1;
                    } else if (type == BTRFS_EXTENT_DATA_KEY) {
                        /* skip for now; extent parsing done in restore step */
                        p += 16;
                    } else {
                        /* unknown; skip a bit */
                        p += 8;
                    }
                    if (p >= end) break;
                }
                free(leaf);
            }
        }
        off += n;
    }

    close(fd);
    return 0;
}

/* List deleted candidates under prefix */
static int list_deleted(const char *device, const char *prefix) {
    memset(inode_map, 0, sizeof(inode_map));
    dir_list = NULL;
    if (scan_device(device) < 0) return 1;

    printf("Deleted inode candidates (nlink==0) and known names under prefix '%s':\n", prefix);
    int found = 0;
    /* iterate buckets */
    for (size_t b = 0; b < INODE_MAP_SIZE; ++b) {
        inode_info_t *p = inode_map[b];
        if (!p) continue;
        if (p->nlink == 0) {
            /* search dir_list for names referencing this inode */
            int printed = 0;
            for (dir_entry_t *d = dir_list; d; d = d->next) {
                if (d->ino == p->ino) {
                    /* build path heuristic: we only have name and parent dir ino; we cannot reconstruct full path reliably */
                    /* print parent inode and name */
                    char pathbuf[512];
                    snprintf(pathbuf, sizeof(pathbuf), "(parent_ino=%" PRIu64 ")/%s", d->dir_ino, d->name);
                    if (strstr(pathbuf, prefix)) {
                        printf("inode %" PRIu64 " size=%" PRIu64 " bytes: %s\n", p->ino, p->size, pathbuf);
                        printed = 1; found = 1;
                    }
                }
            }
            if (!printed) {
                /* no dir_item found */
                /* we cannot show filename; print inode only */
                printf("inode %" PRIu64 " size=%" PRIu64 " bytes: (no dir_item found)\n", p->ino, p->size);
                found = 1;
            }
        }
    }
    if (!found) printf("(no candidates found)\n");
    return 0;
}

/* Very simplified restore: find inode by matching dir entries to path, then attempt to read extent_data items
 * for that inode by scanning device for extent_data keys referencing the inode.
 * This is heuristic and will only work for simple physical extents.
 */
static int restore_file(const char *device, const char *path, const char *outdir) {
    /* split path into parent name heuristic: we only have parent_ino/name pairs in dir_list */
    /* For prototype, match any dir_entry whose name equals the final component of path and whose parent path contains prefix */
    const char *basename = strrchr(path, '/');
    if (!basename) basename = path; else basename++;
    uint64_t target_ino = 0;
    for (dir_entry_t *d = dir_list; d; d = d->next) {
        if (strcmp(d->name, basename) == 0) {
            /* pick first match */
            target_ino = d->ino;
            break;
        }
    }
    if (target_ino == 0) {
        fprintf(stderr, "Could not find dir_item matching '%s' in scanned metadata\n", basename);
        return 1;
    }
    inode_info_t *ii = find_inode(target_ino);
    if (!ii) {
        fprintf(stderr, "Found dir_item for inode %" PRIu64 " but no inode_item present\n", target_ino);
        return 1;
    }
    /* create outdir if needed */
    struct stat st;
    if (stat(outdir, &st) < 0) {
        if (mkdir(outdir, 0700) < 0) { perror("mkdir outdir"); return 1; }
    }
    char outpath[PATH_MAX];
    snprintf(outpath, sizeof(outpath), "%s/recovered_ino_%" PRIu64 ".bin", outdir, target_ino);
    int outfd = open(outpath, O_CREAT|O_WRONLY, 0600);
    if (outfd < 0) { perror("open out"); return 1; }

    /* scan device for extent_data keys referencing target_ino and write raw payloads */
    int fd = open(device, O_RDONLY);
    if (fd < 0) { perror("open device"); close(outfd); return 1; }
    off_t off = 0;
    uint8_t buf[SCAN_BLOCK];
    ssize_t n;
    size_t total_written = 0;
    while ((n = pread(fd, buf, sizeof(buf), off)) > 0) {
        for (ssize_t i = 0; i < n - 8; ++i) {
            if (memcmp(buf + i, BTRFS_NODE_MAGIC, 5) == 0) {
                off_t node_off = off + i;
                uint8_t header[BTRFS_HEADER_SIZE];
                if (pread_full(fd, header, sizeof(header), node_off) != sizeof(header)) continue;
                uint32_t level = *(uint32_t*)(header + 0x18);
                uint32_t nritems = *(uint32_t*)(header + 0x1C);
                if (level != 0) continue;
                size_t leaf_size = 65536;
                uint8_t *leaf = malloc(leaf_size);
                if (!leaf) continue;
                if (pread_full(fd, leaf, leaf_size, node_off) <= 0) { free(leaf); continue; }
                uint8_t *p = leaf + BTRFS_HEADER_SIZE;
                uint8_t *end = leaf + leaf_size;
                for (uint32_t it = 0; it < nritems && p + 24 < end; ++it) {
                    uint64_t objid = *(uint64_t*)p;
                    uint32_t type = *(uint32_t*)(p + 8);
                    uint64_t offset_key = *(uint64_t*)(p + 12);
                    p += 24;
                    if (type == BTRFS_EXTENT_DATA_KEY && objid == target_ino) {
                        /* heuristic: next bytes contain physical address and length */
                        uint64_t phys = *(uint64_t*)p;
                        uint64_t len = *(uint64_t*)(p + 8);
                        if (phys == 0 || len == 0) { p += 16; continue; }
                        /* read physical data and write */
                        uint8_t *dat = malloc(len);
                        if (!dat) { p += 16; continue; }
                        if (pread_full(fd, dat, len, (off_t)phys) != (ssize_t)len) { free(dat); p += 16; continue; }
                        if (write(outfd, dat, len) != (ssize_t)len) { free(dat); p += 16; continue; }
                        total_written += len;
                        free(dat);
                        p += 16;
                    } else {
                        p += 8;
                    }
                    if (p >= end) break;
                }
                free(leaf);
            }
        }
        off += n;
    }
    close(fd);
    close(outfd);
    if (total_written == 0) {
        fprintf(stderr, "No extent_data found for inode %" PRIu64 " or extents not in supported format\n", target_ino);
        unlink(outpath);
        return 1;
    }
    printf("Wrote %" PRIu64 " bytes to %s\n", (uint64_t)total_written, outpath);
    return 0;
}

static void usage(const char *p) {
    fprintf(stderr,
        "Usage:\n"
        "  %s --device DEVICE --list PATH_PREFIX\n"
        "  %s --device DEVICE --restore PATH_TO_FILE --outdir OUTDIR\n",
        p, p);
}

int main(int argc, char **argv) {
    if (argc < 2) { usage(argv[0]); return 1; }
    const char *device = NULL;
    const char *list_prefix = NULL;
    const char *restore_path = NULL;
    const char *outdir = NULL;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--device") == 0 && i+1 < argc) { device = argv[++i]; continue; }
        if (strcmp(argv[i], "--list") == 0 && i+1 < argc) { list_prefix = argv[++i]; continue; }
        if (strcmp(argv[i], "--restore") == 0 && i+1 < argc) { restore_path = argv[++i]; continue; }
        if (strcmp(argv[i], "--outdir") == 0 && i+1 < argc) { outdir = argv[++i]; continue; }
        usage(argv[0]); return 1;
    }
    if (!device) { fprintf(stderr, "--device is required\n"); return 1; }
    if (list_prefix && restore_path) { fprintf(stderr, "Choose either --list or --restore\n"); return 1; }
    /* pre-scan to populate dir entries and inode items */
    memset(inode_map, 0, sizeof(inode_map));
    dir_list = NULL;
    if (scan_device(device) < 0) return 1;
    if (list_prefix) return list_deleted(device, list_prefix);
    if (restore_path) {
        if (!outdir) { fprintf(stderr, "--outdir required for --restore\n"); return 1; }
        return restore_file(device, restore_path, outdir);
    }
    usage(argv[0]);
    return 1;
}
