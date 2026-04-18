#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <sys/stat.h>
#include <openssl/md5.h>
#include <unistd.h>

#define HASH_SIZE 16

typedef struct FileNode {
    char *path;
    unsigned char hash[HASH_SIZE];
    time_t mtime;
    struct FileNode *next;
} FileNode;

FileNode *head = NULL;
FILE *cleanup_file = NULL;

// MD5-Berechnung
int calculate_md5(const char *path, unsigned char *out_hash) {
    FILE *f = fopen(path, "rb");
    if (!f) return -1;

    MD5_CTX ctx;
    MD5_Init(&ctx);
    unsigned char buffer[8192];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), f)) != 0)
        MD5_Update(&ctx, buffer, bytes);

    MD5_Final(out_hash, &ctx);
    fclose(f);
    return 0;
}

// Inhaltsvergleich bei Hash-Gleichheit
int files_are_identical(const char *p1, const char *p2) {
    FILE *f1 = fopen(p1, "rb");
    FILE *f2 = fopen(p2, "rb");
    if (!f1 || !f2) {
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return 0;
    }

    char buf1[8192], buf2[8192];
    size_t r1, r2;
    do {
        r1 = fread(buf1, 1, sizeof(buf1), f1);
        r2 = fread(buf2, 1, sizeof(buf2), f2);
        if (r1 != r2 || memcmp(buf1, buf2, r1) != 0) {
            fclose(f1); fclose(f2);
            return 0;
        }
    } while (r1 > 0);

    fclose(f1); fclose(f2);
    return 1;
}

// Callback-Funktion für nftw
static int process_file(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
    // Nur reguläre Dateien bearbeiten
    if (tflag != FTW_F) return 0;

    // Das Cleanup-Skript selbst ignorieren
    if (strstr(fpath, "cleanup.sh")) return 0;

    unsigned char current_hash[HASH_SIZE];
    if (calculate_md5(fpath, current_hash) != 0) return 0;

    FileNode *curr = head;
    int found_duplicate = 0;

    while (curr) {
        if (memcmp(curr->hash, current_hash, HASH_SIZE) == 0) {
            if (files_are_identical(curr->path, fpath)) {
                found_duplicate = 1;
                if (sb->st_mtime > curr->mtime) {
                    fprintf(cleanup_file, "rm \"%s\"\n", curr->path);
                    free(curr->path);
                    curr->path = strdup(fpath);
                    curr->mtime = sb->st_mtime;
                } else {
                    fprintf(cleanup_file, "rm \"%s\"\n", fpath);
                }
                break;
            }
        }
        curr = curr->next;
    }

    if (!found_duplicate) {
        FileNode *new_node = malloc(sizeof(FileNode));
        new_node->path = strdup(fpath);
        memcpy(new_node->hash, current_hash, HASH_SIZE);
        new_node->mtime = sb->st_mtime;
        new_node->next = head;
        head = new_node;
    }

    return 0;
}

int main() {
    cleanup_file = fopen("cleanup.sh", "w");
    if (!cleanup_file) {
        perror("Konnte cleanup.sh nicht erstellen");
        return 1;
    }
    fprintf(cleanup_file, "#!/bin/sh\n\n");

    printf("Starte rekursiven Scan von / ...\n");
    printf("Dies kann je nach Festplattengröße sehr lange dauern.\n");

    // nftw Parameter: Pfad, Callback, max_open_fds, Flags
    // FTW_PHYS: Symbolischen Links nicht folgen
    // FTW_MOUNT: Nicht auf andere Dateisysteme wechseln
    if (nftw("/", process_file, 20, FTW_PHYS | FTW_MOUNT) == -1) {
        perror("nftw");
        return 1;
    }

    fclose(cleanup_file);
    chmod("cleanup.sh", 0755);
    printf("\nScan abgeschlossen. 'cleanup.sh' wurde erstellt.\n");

    return 0;
}
