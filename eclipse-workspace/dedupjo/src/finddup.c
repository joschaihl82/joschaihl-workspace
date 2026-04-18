/*
 * dedup_approx.c
 *
 * Duplicate finder using a fast sampling-based approximate file signature.
 * Replaces the original XXH3-based full-file hashing with a partial-read,
 * sampling algorithm (ApproxHash) that reads only small windows from each file.
 *
 * Purpose:
 * - Much faster for very large files because it avoids reading the whole file.
 * - Not cryptographically secure. Use full-file SHA256/XXH3 if you need exact integrity.
 * - Good for fast deduplication prefiltering; tune parameters for your workload.
 *
 * Compile:
 *   gcc -O3 -std=c11 -Wall -Wextra -o dedup_approx dedup_approx.c
 *
 * Notes:
 * - HASH_SIZE remains 16 bytes (128-bit) to keep the rest of the program unchanged.
 * - The algorithm:
 *     * Determine number of samples based on file size (one sample per SAMPLE_SPAN bytes, capped).
 *     * Deterministically pick offsets (so same file -> same signature).
 *     * Read small windows (WINDOW bytes) at those offsets.
 *     * Compute a fast non-crypto 64-bit FNV-1a per window.
 *     * Combine all window hashes into two 64-bit accumulators (XOR and rotated-sum).
 *     * Store accumulators as 16-byte signature (little-endian).
 *
 * Tuning constants:
 * - SAMPLE_SPAN: average distance between samples (64 KiB default).
 * - MAX_SAMPLES: upper cap on number of windows read.
 * - WINDOW: bytes read per sample (64 B default).
 *
 * Limitations:
 * - Collisions possible; not a replacement for full-file cryptographic hashes.
 * - Works best as a fast prefilter; confirm duplicates with a full hash if needed.
 */

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>

#define HASH_SIZE 16 /* 128-bit signature (two 64-bit values) */
#define INITIAL_CAPACITY 1024
#define READ_BUF_SIZE 131072 /* 128 KiB for fallback full reads if needed */

/* ApproxHash tuning */
#define SAMPLE_SPAN (64 * 1024) /* 64 KiB between nominal samples */
#define MAX_SAMPLES 1024        /* never sample more than this many windows */
#define WINDOW 64               /* bytes per sampled window */

/* FileEntry and globals (unchanged) */
typedef struct {
    char *path;
    off_t size;
    unsigned char hash[HASH_SIZE];
    time_t mtime;
    int is_duplicate;
} FileEntry;

static FileEntry *file_list = NULL;
static size_t file_count = 0;
static size_t file_capacity = 0;

static unsigned long long total_bytes = 0;
static unsigned long long processed_bytes = 0;

void add_file(const char *fpath, off_t size, time_t mtime) {
    if (file_count >= file_capacity) {
        size_t newcap = (file_capacity == 0) ? INITIAL_CAPACITY : file_capacity * 2;
        FileEntry *tmp = realloc(file_list, newcap * sizeof(FileEntry));
        if (!tmp) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        file_list = tmp;
        file_capacity = newcap;
    }
    file_list[file_count].path = strdup(fpath);
    if (!file_list[file_count].path) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }
    file_list[file_count].size = size;
    file_list[file_count].mtime = mtime;
    file_list[file_count].is_duplicate = 0;
    memset(file_list[file_count].hash, 0, HASH_SIZE);
    file_count++;
    total_bytes += (unsigned long long)size;
}

/* nftw callback to collect files */
static int collect_metadata(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
    (void)ftwbuf;
    if (tflag == FTW_F) {
        if (strcmp(fpath, "./cleanup.sh") != 0) {
            add_file(fpath, sb->st_size, sb->st_mtime);
        }
    }
    return 0;
}

/* ---------------------------
   Small utility functions
   --------------------------- */

/* 64-bit rotate left */
static inline uint64_t rotl64(uint64_t x, unsigned r) {
    return (x << r) | (x >> (64 - r));
}

/* FNV-1a 64-bit (fast, non-crypto) */
static uint64_t fnv1a_64(const unsigned char *data, size_t len) {
    const uint64_t FNV_OFFSET = 14695981039346656037ULL;
    const uint64_t FNV_PRIME  = 1099511628211ULL;
    uint64_t h = FNV_OFFSET;
    for (size_t i = 0; i < len; ++i) {
        h ^= (uint64_t)data[i];
        h *= FNV_PRIME;
    }
    return h;
}

/* Read exactly 'len' bytes from FILE* at offset 'pos' into buf.
   Returns number of bytes read (may be < len at EOF) or -1 on error. */
static ssize_t read_at(FILE *f, off_t pos, unsigned char *buf, size_t len) {
    if (fseeko(f, pos, SEEK_SET) != 0) return -1;
    return (ssize_t)fread(buf, 1, len, f);
}

/* Compress vector of 64-bit block hashes into two 64-bit accumulators.
   acc1 = XOR of all blocks, acc2 = rotated-sum to mix order. */
static void combine_hashes(uint64_t *blocks, size_t nblocks, uint64_t *out_lo, uint64_t *out_hi) {
    uint64_t acc_xor = 0;
    uint64_t acc_rot = 0x9e3779b97f4a7c15ULL; /* arbitrary seed */
    for (size_t i = 0; i < nblocks; ++i) {
        acc_xor ^= blocks[i];
        acc_rot += rotl64(blocks[i], (unsigned)(i % 63));
        acc_rot ^= (blocks[i] >> ((i * 7) % 61));
    }
    *out_lo = acc_xor;
    *out_hi = acc_rot;
}

/* ---------------------------
   ApproxHash implementation
   --------------------------- */

/* Compute an approximate 128-bit signature for entry.
   Returns 0 on success, -1 on error (e.g., cannot open file).
   The function reads only small windows from the file (sampling). */
int compute_approxhash(FileEntry *entry) {
    if (!entry) return -1;
    const char *path = entry->path;
    off_t N = entry->size;

    FILE *f = fopen(path, "rb");
    if (!f) return -1;

    /* If file is tiny, read whole file and hash it directly */
    if (N <= (off_t)WINDOW) {
        unsigned char *buf = malloc((size_t)N + 1);
        if (!buf) { fclose(f); return -1; }
        size_t r = fread(buf, 1, (size_t)N, f);
        uint64_t h = fnv1a_64(buf, r);
        /* duplicate into both halves for small files */
        memcpy(entry->hash, &h, 8);
        memcpy(entry->hash + 8, &h, 8);
        free(buf);
        fclose(f);
        return 0;
    }

    /* Determine number of samples */
    size_t nominal = (size_t)(N / SAMPLE_SPAN) + 1;
    size_t samples = nominal;
    if (samples > MAX_SAMPLES) samples = MAX_SAMPLES;
    if (samples == 0) samples = 1;

    /* Compute deterministic start offset based on file size and path hash */
    /* Simple path-influenced seed to avoid pathological alignment for many files of same size */
    uint64_t seed = (uint64_t)N ^ 0x9e3779b97f4a7c15ULL;
    for (const char *p = path; *p; ++p) seed = seed * 31 + (unsigned char)(*p);

    off_t usable = N - WINDOW + 1;
    off_t step = (off_t)(usable / samples);
    if (step <= 0) step = 1;
    off_t start = (off_t)(seed % (uint64_t)step);

    /* Allocate buffer for window and for block hashes */
    unsigned char win[WINDOW];
    uint64_t *blocks = malloc(sizeof(uint64_t) * samples);
    if (!blocks) { fclose(f); return -1; }
    size_t collected = 0;

    /* Sample windows */
    for (size_t i = 0; i < samples; ++i) {
        off_t pos = start + (off_t)i * step;
        if (pos < 0) pos = 0;
        if (pos > usable - 1) pos = usable - 1;
        ssize_t got = read_at(f, pos, win, WINDOW);
        if (got <= 0) continue;
        uint64_t h = fnv1a_64(win, (size_t)got);
        blocks[collected++] = h;
    }

    /* If nothing collected (shouldn't happen), fallback to first and last windows */
    if (collected == 0) {
        ssize_t g1 = read_at(f, 0, win, WINDOW);
        if (g1 > 0) blocks[collected++] = fnv1a_64(win, (size_t)g1);
        ssize_t g2 = read_at(f, N - WINDOW, win, WINDOW);
        if (g2 > 0) blocks[collected++] = fnv1a_64(win, (size_t)g2);
    }

    /* Combine collected block hashes into two 64-bit values */
    uint64_t lo = 0, hi = 0;
    combine_hashes(blocks, collected, &lo, &hi);

    /* Store little-endian into entry->hash */
    memcpy(entry->hash, &lo, 8);
    memcpy(entry->hash + 8, &hi, 8);

    free(blocks);
    fclose(f);
    return 0;
}

/* Progress display (unchanged) */
void print_status(time_t start_time, size_t current_index) {
    if (total_bytes == 0) return;

    double percent = (double)processed_bytes / (double)total_bytes * 100.0;
    time_t now = time(NULL);
    double elapsed = difftime(now, start_time);
    double mb_per_sec = (processed_bytes / 1024.0 / 1024.0) / (elapsed > 0 ? elapsed : 1);
    unsigned long long remaining_bytes = (processed_bytes > total_bytes) ? 0 : (total_bytes - processed_bytes);

    long total_sec_left = (mb_per_sec > 0.1) ? (long)(remaining_bytes / 1024.0 / 1024.0 / mb_per_sec) : 0;
    int h = total_sec_left / 3600;
    int m = (total_sec_left % 3600) / 60;
    int s = total_sec_left % 60;

    int bar_width = 40;
    int filled = (int)((percent / 100.0) * bar_width);
    if (filled < 0) filled = 0;
    if (filled > bar_width) filled = bar_width;

    char bar[41];
    for (int i = 0; i < bar_width; ++i) bar[i] = (i < filled) ? '=' : ' ';
    bar[bar_width] = '\0';

    printf("\rProgress: [%-40s] %6.2f%% | Speed: %6.2f MB/s | ETA: %02d:%02d:%02d | File: %zu/%zu   ",
           bar, percent, mb_per_sec, h, m, s, current_index + 1, file_count);
    fflush(stdout);
}

/* Main (mostly unchanged) */
int main(void) {
    printf("Phase 1: Sammle Datei-Metadaten von / (Indexierung)...\n");
    if (nftw("/", collect_metadata, 20, FTW_PHYS | FTW_MOUNT) == -1) {
        perror("nftw");
        /* continue if some files were collected */
    }

    printf("\nIndex fertig. %zu Dateien gefunden. Gesamtvolumen: %.2f GB\n",
           file_count, (double)total_bytes / 1024.0 / 1024.0 / 1024.0);

    FILE *cleanup = fopen("cleanup.sh", "w");
    if (!cleanup) {
        perror("fopen cleanup.sh");
        return EXIT_FAILURE;
    }
    fprintf(cleanup, "#!/bin/sh\n\n");

    printf("Phase 2: Berechne ApproxHash (sampling) und suche Duplikate...\n");
    time_t start_time = time(NULL);

    for (size_t i = 0; i < file_count; i++) {
        if (compute_approxhash(&file_list[i]) != 0) {
            /* Error reading file: count bytes as processed and continue */
            processed_bytes += (unsigned long long)file_list[i].size;
            if (i % 5 == 0) print_status(start_time, i);
            continue;
        }

        /* Compare with previous entries */
        for (size_t j = 0; j < i; j++) {
            if (file_list[j].is_duplicate) continue;
            if (file_list[i].size == file_list[j].size) {
                if (memcmp(file_list[i].hash, file_list[j].hash, HASH_SIZE) == 0) {
                    /* Mark older file as duplicate (remove older) */
                    if (file_list[i].mtime > file_list[j].mtime) {
                        fprintf(cleanup, "rm -v \"%s\"\n", file_list[j].path);
                        file_list[j].is_duplicate = 1;
                    } else {
                        fprintf(cleanup, "rm -v \"%s\"\n", file_list[i].path);
                        file_list[i].is_duplicate = 1;
                        break;
                    }
                }
            }
        }

        processed_bytes += (unsigned long long)file_list[i].size;
        if (i % 5 == 0) print_status(start_time, i);
    }

    /* Final status */
    print_status(start_time, file_count ? file_count - 1 : 0);
    printf("\n");

    fclose(cleanup);
    if (chmod("cleanup.sh", 0755) != 0) {
        perror("chmod cleanup.sh");
    }

    /* Free memory */
    for (size_t i = 0; i < file_count; i++) {
        free(file_list[i].path);
    }
    free(file_list);

    printf("\nFertig! 'cleanup.sh' wurde generiert.\n");
    return 0;
}
