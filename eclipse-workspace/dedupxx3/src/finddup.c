/*
 * dedup.c
 *
 * Duplicate finder using XXH3 (xxHash) 128-bit hashes.
 * Requires libxxhash (link with -lxxhash).
 *
 * Compile:
 *   gcc -O3 -std=c11 -Wall -Wextra -o dedup dedup.c -lxxhash
 *
 * Usage:
 *   Run as a user with permission to traverse the filesystem.
 *   The program walks "/" (change nftw root if desired), computes XXH3-128
 *   hashes for files, and generates cleanup.sh with rm commands for duplicates.
 *
 * Notes:
 * - XXH3 is non-cryptographic but extremely fast and well-suited for deduplication.
 * - HASH_SIZE is 16 (128-bit). If you prefer 64-bit, adjust accordingly.
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
#include <xxhash.h> /* libxxhash: install and link with -lxxhash */

#define HASH_SIZE 16 /* XXH3 128-bit */
#define INITIAL_CAPACITY 1024
#define READ_BUF_SIZE 131072 /* 128 KiB */

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

/* Compute XXH3-128 for a file entry using streaming API */
int compute_xxh3(FileEntry *entry) {
    FILE *f = fopen(entry->path, "rb");
    if (!f) return -1;

    XXH3_state_t *state = XXH3_createState();
    if (!state) { fclose(f); return -1; }
    if (XXH3_128bits_reset(state) != XXH_OK) {
        XXH3_freeState(state);
        fclose(f);
        return -1;
    }

    unsigned char *buf = malloc(READ_BUF_SIZE);
    if (!buf) { XXH3_freeState(state); fclose(f); return -1; }

    size_t bytes;
    while ((bytes = fread(buf, 1, READ_BUF_SIZE, f)) != 0) {
        if (XXH3_128bits_update(state, buf, bytes) != XXH_OK) {
            free(buf);
            XXH3_freeState(state);
            fclose(f);
            return -1;
        }
    }

    XXH128_hash_t h = XXH3_128bits_digest(state);
    uint64_t low = h.low64;
    uint64_t high = h.high64;

    /* Copy into entry->hash (16 bytes, little-endian) */
    memcpy(entry->hash, &low, 8);
    memcpy(entry->hash + 8, &high, 8);

    free(buf);
    XXH3_freeState(state);
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

    printf("Phase 2: Berechne XXH3-128 und suche Duplikate...\n");
    time_t start_time = time(NULL);

    for (size_t i = 0; i < file_count; i++) {
        if (compute_xxh3(&file_list[i]) != 0) {
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
