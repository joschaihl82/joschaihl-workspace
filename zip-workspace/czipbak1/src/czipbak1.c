/*
 * ziparchiver.c
 *
 * Create one zip per folder. Prints ETA for each compressed file.
 *
 * Build:
 *   gcc -O2 -o ziparchiver ziparchiver.c -lzip -lz
 *
 * Requires libzip (https://libzip.org) and zlib.
 */

#define _XOPEN_SOURCE 700
#include <zip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>

static double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

/* Recursively add files from dirpath into zip archive.
 * We only add regular files. Directory entries are added as directory entries.
 * base_len is the length of the path prefix to strip for stored names.
 * bytes_total and bytes_done are used for ETA calculation.
 */
static int add_dir_to_zip(zip_t *za, const char *dirpath, size_t base_len,
                          uint64_t *bytes_done, uint64_t bytes_total, double start_time) {
    DIR *d = opendir(dirpath);
    if (!d) {
        fprintf(stderr, "opendir failed on %s: %s\n", dirpath, strerror(errno));
        return -1;
    }
    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        char fullpath[4096];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, ent->d_name);

        struct stat st;
        if (lstat(fullpath, &st) != 0) {
            fprintf(stderr, "stat failed on %s: %s\n", fullpath, strerror(errno));
            closedir(d);
            return -1;
        }

        if (S_ISDIR(st.st_mode)) {
            /* Add directory entry to zip (optional) */
            char entryname[4096];
            snprintf(entryname, sizeof(entryname), "%s/", fullpath + base_len);
            zip_dir_add(za, entryname, ZIP_FL_ENC_UTF_8);
            /* Recurse */
            if (add_dir_to_zip(za, fullpath, base_len, bytes_done, bytes_total, start_time) != 0) {
                closedir(d);
                return -1;
            }
        } else if (S_ISREG(st.st_mode)) {
            /* Add file */
            const char *srcpath = fullpath;
            char entryname[4096];
            snprintf(entryname, sizeof(entryname), "%s", fullpath + base_len);

            /* Create zip source from file */
            zip_source_t *zs = zip_source_file(za, srcpath, 0, 0);
            if (!zs) {
                fprintf(stderr, "zip_source_file failed for %s: %s\n", srcpath, zip_strerror(za));
                closedir(d);
                return -1;
            }
            zip_int64_t idx = zip_file_add(za, entryname, zs, ZIP_FL_ENC_UTF_8);
            if (idx < 0) {
                fprintf(stderr, "zip_file_add failed for %s: %s\n", entryname, zip_strerror(za));
                zip_source_free(zs);
                closedir(d);
                return -1;
            }
            /* Set compression method to deflate with default level */
            if (zip_set_file_compression(za, idx, ZIP_CM_DEFLATE, 9) < 0) {
                /* Not fatal; continue */
            }

            /* Update progress and compute ETA */
            *bytes_done += (uint64_t)st.st_size;
            double elapsed = now_seconds() - start_time;
            double rate = (elapsed > 0.000001) ? ((double)*bytes_done / elapsed) : 0.0;
            uint64_t remaining = (bytes_total > *bytes_done) ? (bytes_total - *bytes_done) : 0;
            double eta = (rate > 0.0) ? (remaining / rate) : -1.0;

            /* Print ETA for this file */
            if (eta >= 0.0) {
                int hours = (int)(eta / 3600);
                int mins = (int)((eta - hours*3600) / 60);
                int secs = (int)(eta - hours*3600 - mins*60);
                printf("Added %s  size=%" PRIu64 "  ETA %02d:%02d:%02d\n",
                       entryname, (uint64_t)st.st_size, hours, mins, secs);
            } else {
                printf("Added %s  size=%" PRIu64 "  ETA unknown\n",
                       entryname, (uint64_t)st.st_size);
            }
            fflush(stdout);
        } else {
            /* skip other types (symlinks, devices) */
        }
    }
    closedir(d);
    return 0;
}

/* Compute total bytes of regular files inside a directory recursively */
static int compute_total_bytes(const char *dirpath, uint64_t *total) {
    DIR *d = opendir(dirpath);
    if (!d) {
        fprintf(stderr, "opendir failed on %s: %s\n", dirpath, strerror(errno));
        return -1;
    }
    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        char fullpath[4096];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, ent->d_name);
        struct stat st;
        if (lstat(fullpath, &st) != 0) {
            fprintf(stderr, "stat failed on %s: %s\n", fullpath, strerror(errno));
            closedir(d);
            return -1;
        }
        if (S_ISDIR(st.st_mode)) {
            if (compute_total_bytes(fullpath, total) != 0) {
                closedir(d);
                return -1;
            }
        } else if (S_ISREG(st.st_mode)) {
            *total += (uint64_t)st.st_size;
        }
    }
    closedir(d);
    return 0;
}

/* Create zip for a directory path. zipname is full path to output zip file. */
static int create_zip_for_dir(const char *dirpath, const char *zipname) {
    /* Compute total bytes */
    uint64_t total_bytes = 0;
    if (compute_total_bytes(dirpath, &total_bytes) != 0) {
        return -1;
    }
    printf("Creating zip %s  total bytes %" PRIu64 "\n", zipname, total_bytes);

    int errorp = 0;
    zip_t *za = zip_open(zipname, ZIP_CREATE | ZIP_TRUNCATE, &errorp);
    if (!za) {
        char buf[1024];
        zip_error_to_str(buf, sizeof(buf), errorp, errno);
        fprintf(stderr, "zip_open failed: %s\n", buf);
        return -1;
    }

    uint64_t bytes_done = 0;
    double start_time = now_seconds();

    /* base_len: strip leading path and the slash so stored names are relative */
    size_t base_len = strlen(dirpath);
    /* If dirpath does not end with '/', we want to strip the slash too when building names */
    if (dirpath[base_len - 1] == '/') {
        /* keep base_len as-is */
    } else {
        /* when building entryname we used fullpath + base_len, which will start with '/'.
           To avoid leading slash in stored names, we will add 1 to base_len if next char is '/'. */
        /* We'll ensure stored names do not start with a slash by adding 1 when building entryname */
    }

    /* Add files recursively */
    if (add_dir_to_zip(za, dirpath, base_len + 1, &bytes_done, total_bytes, start_time) != 0) {
        zip_close(za);
        return -1;
    }

    if (zip_close(za) != 0) {
        fprintf(stderr, "zip_close failed: %s\n", zip_strerror(za));
        return -1;
    }

    double total_elapsed = now_seconds() - start_time;
    printf("Finished %s  bytes=%" PRIu64 "  time=%.2fs\n", zipname, bytes_done, total_elapsed);
    return 0;
}

/* Helper to join paths safely */
static void join_path(char *out, size_t outlen, const char *a, const char *b) {
    if (a[0] == '\0') {
        snprintf(out, outlen, "%s", b);
        return;
    }
    if (a[strlen(a)-1] == '/') {
        snprintf(out, outlen, "%s%s", a, b);
    } else {
        snprintf(out, outlen, "%s/%s", a, b);
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <path-to-compress> <output-zip-dir>\n", argv[0]);
        return 2;
    }
    const char *input = argv[1];
    const char *outdir = argv[2];

    struct stat st;
    if (stat(input, &st) != 0) {
        fprintf(stderr, "stat failed on input %s: %s\n", input, strerror(errno));
        return 1;
    }
    /* Ensure output directory exists */
    if (stat(outdir, &st) != 0) {
        if (mkdir(outdir, 0755) != 0) {
            fprintf(stderr, "Cannot create output directory %s: %s\n", outdir, strerror(errno));
            return 1;
        }
    } else {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "Output path %s is not a directory\n", outdir);
            return 1;
        }
    }

    if (S_ISREG(st.st_mode)) {
        /* Input is a single file: create a zip containing that file */
        char zipname[4096];
        const char *basename = strrchr(input, '/');
        basename = basename ? basename + 1 : input;
        snprintf(zipname, sizeof(zipname), "%s/%s.zip", outdir, basename);
        /* Create a temporary directory name to reuse create_zip_for_dir logic is not necessary.
           Instead create zip and add single file. */
        int errorp = 0;
        zip_t *za = zip_open(zipname, ZIP_CREATE | ZIP_TRUNCATE, &errorp);
        if (!za) {
            char buf[1024];
            zip_error_to_str(buf, sizeof(buf), errorp, errno);
            fprintf(stderr, "zip_open failed: %s\n", buf);
            return 1;
        }
        zip_source_t *zs = zip_source_file(za, input, 0, 0);
        if (!zs) {
            fprintf(stderr, "zip_source_file failed for %s: %s\n", input, zip_strerror(za));
            zip_close(za);
            return 1;
        }
        if (zip_file_add(za, basename, zs, ZIP_FL_ENC_UTF_8) < 0) {
            fprintf(stderr, "zip_file_add failed: %s\n", zip_strerror(za));
            zip_source_free(zs);
            zip_close(za);
            return 1;
        }
        zip_close(za);
        printf("Created %s\n", zipname);
        return 0;
    } else if (S_ISDIR(st.st_mode)) {
        /* Input is a directory. We'll iterate immediate children.
           If the directory contains files directly, we will create a zip for the input directory itself.
           For each immediate child directory, create a zip named childdir.zip.
        */
        DIR *d = opendir(input);
        if (!d) {
            fprintf(stderr, "opendir failed on %s: %s\n", input, strerror(errno));
            return 1;
        }
        struct dirent *ent;
        int found_subdirs = 0;
        /* First pass: check if there are immediate files in input dir */
        uint64_t immediate_files_bytes = 0;
        int has_immediate_files = 0;
        rewinddir(d);
        while ((ent = readdir(d)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            char fullpath[4096];
            join_path(fullpath, sizeof(fullpath), input, ent->d_name);
            struct stat st2;
            if (lstat(fullpath, &st2) != 0) continue;
            if (S_ISREG(st2.st_mode)) {
                has_immediate_files = 1;
                immediate_files_bytes += (uint64_t)st2.st_size;
            }
        }
        rewinddir(d);
        /* If there are immediate files, create a zip for the input directory itself */
        if (has_immediate_files) {
            char zipname[4096];
            const char *basename = strrchr(input, '/');
            basename = basename ? basename + 1 : input;
            snprintf(zipname, sizeof(zipname), "%s/%s.zip", outdir, basename);
            if (create_zip_for_dir(input, zipname) != 0) {
                fprintf(stderr, "Failed to create zip for %s\n", input);
            }
        }
        /* Now create zip for each immediate subdirectory */
        while ((ent = readdir(d)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            char fullpath[4096];
            join_path(fullpath, sizeof(fullpath), input, ent->d_name);
            struct stat st2;
            if (lstat(fullpath, &st2) != 0) continue;
            if (S_ISDIR(st2.st_mode)) {
                found_subdirs = 1;
                char zipname[4096];
                snprintf(zipname, sizeof(zipname), "%s/%s.zip", outdir, ent->d_name);
                if (create_zip_for_dir(fullpath, zipname) != 0) {
                    fprintf(stderr, "Failed to create zip for %s\n", fullpath);
                }
            }
        }
        closedir(d);
        if (!found_subdirs && !has_immediate_files) {
            printf("No files or subdirectories found in %s\n", input);
        }
        return 0;
    } else {
        fprintf(stderr, "Input path is neither file nor directory\n");
        return 1;
    }
}
