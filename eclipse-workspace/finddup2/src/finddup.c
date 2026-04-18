#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <sys/stat.h>
#include <openssl/md5.h>
#include <unistd.h>
#include <time.h>

#define HASH_SIZE 16

// Struktur für eine Datei in der Vektor-Tabelle
typedef struct {
    char *path;
    off_t size;
    unsigned char hash[HASH_SIZE];
    time_t mtime;
    int is_duplicate;
} FileEntry;

// Globaler Vektor
FileEntry *file_list = NULL;
size_t file_count = 0;
size_t file_capacity = 0;

unsigned long long total_bytes = 0;
unsigned long long processed_bytes = 0;

// Funktion zum Hinzufügen einer Datei zum Vektor
void add_file(const char *fpath, off_t size, time_t mtime) {
    if (file_count >= file_capacity) {
        file_capacity = (file_capacity == 0) ? 1024 : file_capacity * 2;
        file_list = realloc(file_list, file_capacity * sizeof(FileEntry));
    }
    file_list[file_count].path = strdup(fpath);
    file_list[file_count].size = size;
    file_list[file_count].mtime = mtime;
    file_list[file_count].is_duplicate = 0;
    file_count++;
    total_bytes += size;
}

// Erster Durchgang: Nur Pfade und Größen sammeln
static int collect_metadata(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
    if (tflag == FTW_F && strcmp(fpath, "./cleanup.sh") != 0) {
        add_file(fpath, sb->st_size, sb->st_mtime);
    }
    return 0;
}

// MD5 Berechnung für einen Eintrag
int compute_md5(FileEntry *entry) {
    FILE *f = fopen(entry->path, "rb");
    if (!f) return -1;

    MD5_CTX ctx;
    MD5_Init(&ctx);
    unsigned char buffer[131072]; // 128KB Buffer für Speed
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), f)) != 0) {
        MD5_Update(&ctx, buffer, bytes);
    }
    MD5_Final(entry->hash, &ctx);
    fclose(f);
    return 0;
}

void print_status(time_t start_time) {
    if (total_bytes == 0) return;

    double percent = (double)processed_bytes / total_bytes * 100.0;
    time_t now = time(NULL);
    double elapsed = difftime(now, start_time);

    double mb_per_sec = (processed_bytes / 1024.0 / 1024.0) / (elapsed > 0 ? elapsed : 1);
    unsigned long long remaining_bytes = total_bytes - processed_bytes;

    // Zeitberechnung
    long total_sec_left = (mb_per_sec > 0.1) ? (long)(remaining_bytes / 1024.0 / 1024.0 / mb_per_sec) : 0;
    int h = total_sec_left / 3600;
    int m = (total_sec_left % 3600) / 60;
    int s = total_sec_left % 60;

    printf("\rProgress: [%-40s] %.1f%% | Speed: %.1f MB/s | ETA: %02d:%02d:%02d   ",
           "", percent, mb_per_sec, h, m, s); // Balken wird hier simpel gehalten
    fflush(stdout);
}

int main() {
    printf("Phase 1: Sammle Datei-Metadaten von / (Indexierung)...\n");
    if (nftw("/", collect_metadata, 20, FTW_PHYS | FTW_MOUNT) == -1) {
        perror("nftw");
    }

    printf("\nIndex fertig. %zu Dateien gefunden. Gesamtvolumen: %.2f GB\n",
           file_count, (double)total_bytes / 1024 / 1024 / 1024);

    FILE *cleanup = fopen("cleanup.sh", "w");
    fprintf(cleanup, "#!/bin/sh\n\n");

    printf("Phase 2: Berechne MD5 und suche Duplikate...\n");
    time_t start_time = time(NULL);

    for (size_t i = 0; i < file_count; i++) {
        if (compute_md5(&file_list[i]) != 0) {
            processed_bytes += file_list[i].size;
            continue;
        }

        // Vergleich mit bisherigen Dateien im Vektor (nur bis zum aktuellen Index i)
        for (size_t j = 0; j < i; j++) {
            // Nur vergleichen, wenn Größe identisch (schnellster Vor-Filter)
            if (file_list[i].size == file_list[j].size && !file_list[j].is_duplicate) {
                if (memcmp(file_list[i].hash, file_list[j].hash, HASH_SIZE) == 0) {
                    // Falls Hash-Gleichheit, markiere das ältere als Duplikat
                    if (file_list[i].mtime > file_list[j].mtime) {
                        fprintf(cleanup, "rm \"%s\"\n", file_list[j].path);
                        file_list[j].is_duplicate = 1;
                    } else {
                        fprintf(cleanup, "rm \"%s\"\n", file_list[i].path);
                        file_list[i].is_duplicate = 1;
                        break; // Aktuelle Datei ist weg, kein weiterer Vergleich nötig
                    }
                }
            }
        }

        processed_bytes += file_list[i].size;
        if (i % 5 == 0) print_status(start_time);
    }

    fclose(cleanup);
    chmod("cleanup.sh", 0755);

    // Cleanup RAM
    for(size_t i = 0; i < file_count; i++) free(file_list[i].path);
    free(file_list);

    printf("\n\nFertig! 'cleanup.sh' wurde generiert.\n");
    return 0;
}
