/*
 * dedup_loghash.c
 *
 * Duplicate finder using an O(log n) sampling-based approximate signature.
 * Positions are chosen exponentially (geometric towards file end).
 * Before each read a small deterministic jump of 0..64 bytes is applied (avg ~32).
 *
 * Compile:
 *   gcc -O3 -std=c11 -Wall -Wextra -o dedup_loghash dedup_loghash.c
 *
 * WARNING:
 * - Not cryptographically secure. Use full-file hash for final verification.
 * - This is a fast prefilter for deduplication.
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

#define HASH_SIZE 16 /* 128-bit signature (two 64-bit values) */
#define INITIAL_CAPACITY 1024

/* Sampling parameters */
#define WINDOW 64       /* bytes read per sampled window */
#define MAX_SAMPLES 64  /* safety cap on number of log samples */

/* FileEntry and globals */
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

/* rotate left */
static inline uint64_t rotl64(uint64_t x, unsigned r) {
    return (x << r) | (x >> (64 - r));
}

/* Deterministic 32-bit LCG for small pseudo-random offsets */
static inline uint32_t lcg_next(uint32_t s) {
    return (uint32_t)(s * 1103515245u + 12345u);
}

/*
 * compute_log_approx
 *
 * - Samples ~log2(N/WINDOW) windows at exponential positions towards file end.
 * - Before each read, applies a deterministic small jump in [0,64].
 * - Combines window hashes into two 64-bit accumulators.
 * - Returns 0 on success, -1 on error.
 */
int compute_log_approx(FileEntry *entry) {
    if (!entry) return -1;
    const char *path = entry->path;
    off_t N = entry->size;

    FILE *f = fopen(path, "rb");
    if (!f) return -1;

    /* tiny files: read whole file */
    if (N <= (off_t)WINDOW) {
        unsigned char *buf = malloc((size_t)N + 1);
        if (!buf) { fclose(f); return -1; }
        size_t r = fread(buf, 1, (size_t)N, f);
        uint64_t h = fnv1a_64(buf, r);
        memcpy(entry->hash, &h, 8);
        memcpy(entry->hash + 8, &h, 8);
        free(buf);
        fclose(f);
        return 0;
    }

    /* compute number of samples = floor(log2(N/WINDOW)) but at least 1, cap to MAX_SAMPLES */
    size_t samples = 0;
    off_t tmp = N / WINDOW;
    while (tmp > 1 && samples < MAX_SAMPLES) {
        tmp >>= 1;
        samples++;
    }
    if (samples == 0) samples = 1;
    if (samples > MAX_SAMPLES) samples = MAX_SAMPLES;

    /* deterministic seed from size and path */
    uint32_t seed = (uint32_t)((uint64_t)N ^ 0x9e3779b9u);
    for (const char *p = path; *p; ++p) seed = seed * 31u + (unsigned char)(*p);

    unsigned char win[WINDOW];
    uint64_t acc_xor = 0;
    uint64_t acc_rot = 0x9e3779b97f4a7c15ULL;

    /* For i = 1..samples compute exponential position:
       pos_i = N - (N >> i)  (so positions approach N as i increases)
       then apply small jump offset = seed % 65, update seed via LCG */
    for (size_t i = 1; i <= samples; ++i) {
        off_t pos = N - (N >> i);
        if (pos < 0) pos = 0;
        if (pos > N - WINDOW) pos = N - WINDOW;

        /* small deterministic jump 0..64 */
        seed = lcg_next(seed);
        uint32_t small = seed % 65u; /* 0..64 */
        off_t pos2 = pos + (off_t)small;
        if (pos2 > N - WINDOW) pos2 = N - WINDOW;

        if (fseeko(f, pos2, SEEK_SET) != 0) continue;
        size_t got = fread(win, 1, WINDOW, f);
        if (got == 0) continue;

        uint64_t h = fnv1a_64(win, got);
        acc_xor ^= h;
        acc_rot += rotl64(h, (unsigned)(got % 63));
    }

    /* store little-endian */
    memcpy(entry->hash, &acc_xor, 8);
    memcpy(entry->hash + 8, &acc_rot, 8);

    fclose(f);
    return 0;
}

/* Progress display */
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

    printf("Phase 2: Berechne Log-sampling Hash (O(log n)) mit kleinen 0..64 Spruengen und suche Duplikate...\n");
    time_t start_time = time(NULL);

    for (size_t i = 0; i < file_count; i++) {
        if (compute_log_approx(&file_list[i]) != 0) {
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
 	 