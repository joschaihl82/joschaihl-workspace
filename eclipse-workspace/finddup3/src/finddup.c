#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <sys/stat.h>
#include <openssl/md4.h> // Wechsel zu MD4
#include <unistd.h>
#include <time.h>

#define HASH_SIZE 16 // MD4 erzeugt ebenfalls 128-bit (16 Bytes)

typedef struct {
    char *path;
    off_t size;
    unsigned char hash[HASH_SIZE];
    time_t mtime;
    int hash_computed;
    int is_duplicate;
} FileEntry;

FileEntry *file_list = NULL;
size_t file_count = 0;
size_t file_capacity = 0;
unsigned long long total_bytes = 0;
unsigned long long processed_bytes = 0;

void add_to_vector(const char *fpath, off_t size, time_t mtime) {
    if (file_count >= file_capacity) {
        file_capacity = (file_capacity == 0) ? 4096 : file_capacity * 2;
        file_list = realloc(file_list, file_capacity * sizeof(FileEntry));
    }
    file_list[file_count].path = strdup(fpath);
    file_list[file_count].size = size;
    file_list[file_count].mtime = mtime;
    file_list[file_count].hash_computed = 0;
    file_list[file_count].is_duplicate = 0;
    file_count++;
    total_bytes += (unsigned long long)size;
}

static int index_files(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
    if (tflag == FTW_F) {
        if (strstr(fpath, "cleanup.sh") || sb->st_size == 0) return 0;
        add_to_vector(fpath, sb->st_size, sb->st_mtime);
    }
    return 0;
}

// MD4 Implementierung
int compute_md4(FileEntry *entry) {
    if (entry->hash_computed) return 0;

    FILE *f = fopen(entry->path, "rb");
    if (!f) return -1;

    MD4_CTX ctx;
    MD4_Init(&ctx);
    unsigned char buffer[131072];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), f)) != 0) {
        MD4_Update(&ctx, buffer, bytes);
    }
    MD4_Final(entry->hash, &ctx);
    fclose(f);
    entry->hash_computed = 1;
    return 0;
}

void update_ui(time_t start_time) {
    if (total_bytes == 0) return;
    double percent = (double)processed_bytes / total_bytes * 100.0;
    time_t now = time(NULL);
    double elapsed = difftime(now, start_time);
    double mb_per_sec = (processed_bytes / 1024.0 / 1024.0) / (elapsed > 0 ? elapsed : 1);
    unsigned long long remaining_bytes = total_bytes - processed_bytes;
    long eta_sec = (mb_per_sec > 0.1) ? (long)(remaining_bytes / 1024.0 / 1024.0 / mb_per_sec) : 0;

    printf("\rProgress: [%-30s] %.1f%% | ETA: %02ld:%02ld:%02ld | %.1f MB/s   ",
           "", percent, eta_sec/3600, (eta_sec%3600)/60, eta_sec%60, mb_per_sec);
    fflush(stdout);
}

int main() {
    printf("--- Phase 1: Indexierung (MD4-Modus) ---\n");
    nftw("/", index_files, 40, FTW_PHYS | FTW_MOUNT);

    printf("\nIndex fertig: %zu Dateien | %.2f GB\n", file_count, (double)total_bytes/1024/1024/1024);

    FILE *cleanup = fopen("cleanup.sh", "w");
    fprintf(cleanup, "#!/bin/sh\n\n");

    printf("--- Phase 2: Hashing & Vergleich ---\n");
    time_t start_time = time(NULL);

    for (size_t i = 0; i < file_count; i++) {
        if (file_list[i].is_duplicate) continue;

        for (size_t j = i + 1; j < file_count; j++) {
            if (file_list[i].size == file_list[j].size && !file_list[j].is_duplicate) {

                if (compute_md4(&file_list[i]) != 0) break;
                if (compute_md4(&file_list[j]) != 0) continue;

                if (memcmp(file_list[i].hash, file_list[j].hash, HASH_SIZE) == 0) {
                    // Zeitstempel-Vergleich
                    if (file_list[j].mtime > file_list[i].mtime) {
                        fprintf(cleanup, "rm \"%s\"\n", file_list[i].path);
                        file_list[i].is_duplicate = 1;
                        break;
                    } else {
                        fprintf(cleanup, "rm \"%s\"\n", file_list[j].path);
                        file_list[j].is_duplicate = 1;
                    }
                }
            }
        }
        processed_bytes += file_list[i].size;
        if (i % 20 == 0) update_ui(start_time);
    }

    fclose(cleanup);
    chmod("cleanup.sh", 0755);
    printf("\n\nFertig! 'cleanup.sh' wurde erstellt.\n");
    return 0;
}
