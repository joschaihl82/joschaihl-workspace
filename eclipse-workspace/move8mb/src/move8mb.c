#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

// Struktur für eine verknüpfte Liste
typedef struct FileNode {
    char *path;
    struct FileNode *next;
} FileNode;

long long total_size = 0;
FileNode *head = NULL;

void add_path(const char *path) {
    FileNode *new_node = malloc(sizeof(FileNode));
    new_node->path = strdup(path);
    new_node->next = head;
    head = new_node;
}

long long calculate_size(const char *path) {
    struct stat st;
    if (lstat(path, &st) != 0) return 0;

    long long size = st.st_size;

    if (S_ISDIR(st.st_mode)) {
        DIR *dir = opendir(path);
        if (!dir) return size;

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            char full_path[PATH_MAX];
            if (snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name) < PATH_MAX) {
                size += calculate_size(full_path);
            }
        }
        closedir(dir);
    }
    return size;
}

void find_targets(const char *base_path) {
    DIR *dir = opendir(base_path);
    if (!dir) {
        // GEÄNDERT: Warnung ausgeben, wenn ein Ordner nicht gelesen werden kann
        fprintf(stderr, "Warnung: Konnte Verzeichnis nicht oeffnen (Rechte?): %s\n", base_path);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char path[PATH_MAX];
        if (snprintf(path, sizeof(path), "%s/%s", base_path, entry->d_name) >= PATH_MAX) {
            fprintf(stderr, "Warnung: Pfad zu lang, uebersprungen: %s/%s\n", base_path, entry->d_name);
            continue;
        }

        int is_git = (strncmp(entry->d_name, ".git", 4) == 0);
        int is_obj = 0;

        size_t len = strlen(entry->d_name);
        if (len > 2 && strcmp(entry->d_name + len - 2, ".o") == 0) is_obj = 1;
        if (len > 4 && strcmp(entry->d_name + len - 4, ".obj") == 0) is_obj = 1;

        if (is_git || is_obj) {
            total_size += calculate_size(path);
            add_path(path);
        } else {
            struct stat st;
            // GEÄNDERT: lstat() statt stat() verhindert Endlosschleifen durch Symlinks
            if (lstat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
                find_targets(path);
            }
        }
    }
    closedir(dir);
}

int main() {
    printf("Suche nach Dateien und berechne Groesse...\n");
    find_targets(".");

    FILE *script = fopen("cleanup.sh", "w");
    if (!script) {
        perror("Fehler beim Erstellen der cleanup.sh");
        return 1;
    }

    fprintf(script, "#!/bin/sh\n");
    fprintf(script, "# %.2f MB zu loeschen\n", (double)total_size / (1024 * 1024));

    FileNode *current = head;
    while (current) {
        fprintf(script, "rm -fr \"%s\"\n", current->path);
        FileNode *temp = current;
        current = current->next;
        free(temp->path);
        free(temp);
    }

    fclose(script);
    chmod("cleanup.sh", 0755);

    printf("Erfolg: 'cleanup.sh' wurde erstellt (Groesse: %.2f MB).\n", (double)total_size / (1024 * 1024));
    return 0;
}
