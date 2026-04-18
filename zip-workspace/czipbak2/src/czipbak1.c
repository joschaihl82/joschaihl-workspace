/*
 * ziparchiver_fixed.c
 *
 * For each immediate child (file or folder) of the input path, create a separate .zip
 * showing per-file percentage and ETA during compression.
 *
 * Build:
 *   gcc -O2 -o ziparchiver_fixed ziparchiver_fixed.c -lzip -lz
 *
 * Requires libzip and zlib.
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

/* Time helper */
static double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

/* Dynamic file list used for directory zips */
typedef struct {
    char **paths;
    char **names;
    uint64_t *sizes;
    uint64_t *cum;
    size_t n;
    size_t cap;
    uint64_t total;
} file_list_t;

static void fl_init(file_list_t *fl) {
    fl->paths = NULL;
    fl->names = NULL;
    fl->sizes = NULL;
    fl->cum = NULL;
    fl->n = 0;
    fl->cap = 0;
    fl->total = 0;
}

static void fl_free(file_list_t *fl) {
    if (!fl) return;
    for (size_t i = 0; i < fl->n; ++i) {
        free(fl->paths[i]);
        free(fl->names[i]);
    }
    free(fl->paths);
    free(fl->names);
    free(fl->sizes);
    free(fl->cum);
    fl_init(fl);
}

static int fl_push(file_list_t *fl, const char *path, const char *name, uint64_t size) {
    if (fl->n + 1 > fl->cap) {
        size_t newcap = fl->cap ? fl->cap * 2 : 256;
        char **p1 = realloc(fl->paths, newcap * sizeof(char*));
        char **p2 = realloc(fl->names, newcap * sizeof(char*));
        uint64_t *p3 = realloc(fl->sizes, newcap * sizeof(uint64_t));
        uint64_t *p4 = realloc(fl->cum, newcap * sizeof(uint64_t));
        if (!p1 || !p2 || !p3 || !p4) {
            free(p1); free(p2); free(p3); free(p4);
            return -1;
        }
        fl->paths = p1;
        fl->names = p2;
        fl->sizes = p3;
        fl->cum = p4;
        fl->cap = newcap;
    }
    fl->paths[fl->n] = strdup(path);
    fl->names[fl->n] = strdup(name);
    fl->sizes[fl->n] = size;
    fl->total += size;
    fl->cum[fl->n] = fl->total;
    fl->n += 1;
    return 0;
}

/* Build stored name relative to base_len */
static void build_stored_name(const char *fullpath, size_t base_len, char *out, size_t outlen) {
    const char *p = fullpath + base_len;
    if (*p == '/') ++p;
    snprintf(out, outlen, "%s", p);
}

/* Recursively collect regular files under dirpath */
static int collect_files_recursive(const char *dirpath, size_t base_len, file_list_t *fl) {
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
            if (collect_files_recursive(fullpath, base_len, fl) != 0) {
                closedir(d);
                return -1;
            }
        } else if (S_ISREG(st.st_mode)) {
            char stored[4096];
            build_stored_name(fullpath, base_len, stored, sizeof(stored));
            if (fl_push(fl, fullpath, stored, (uint64_t)st.st_size) != 0) {
                fprintf(stderr, "Out of memory collecting files\n");
                closedir(d);
                return -1;
            }
        }
    }
    closedir(d);
    return 0;
}

/* Progress userdata */
typedef struct {
    file_list_t *fl;
    double start_time;
    double last_progress;
} prog_ud_t;

/* Progress callback invoked by libzip during zip_close */
static void progress_cb(zip_t *za, double progress, void *ud_v) {
    prog_ud_t *ud = (prog_ud_t*)ud_v;
    if (!ud || !ud->fl || ud->fl->n == 0) return;
    double now = now_seconds();
    double elapsed = now - ud->start_time;
    uint64_t total = ud->fl->total;
    uint64_t bytes_done = (uint64_t)(progress * (double)total + 0.5);

    size_t idx = 0;
    while (idx < ud->fl->n && ud->fl->cum[idx] <= bytes_done) ++idx;
    uint64_t prev_cum = (idx == 0) ? 0 : ud->fl->cum[idx - 1];
    uint64_t file_done = (bytes_done > prev_cum) ? (bytes_done - prev_cum) : 0;
    uint64_t file_size = (idx < ud->fl->n) ? ud->fl->sizes[idx] : 1;

    double file_pct = (file_size > 0) ? (100.0 * (double)file_done / (double)file_size) : 100.0;
    uint64_t remaining_bytes = (bytes_done < total) ? (total - bytes_done) : 0;
    double rate = (elapsed > 0.0001) ? ((double)bytes_done / elapsed) : 0.0;
    double eta = (rate > 0.0) ? ((double)remaining_bytes / rate) : -1.0;

    int h = 0, m = 0, s = 0;
    if (eta >= 0.0) {
        h = (int)(eta / 3600);
        m = (int)((eta - h*3600) / 60);
        s = (int)(eta - h*3600 - m*60);
    }

    printf("Overall %.2f%% — File %zu/%zu: %s  %.1f%%  ETA %02d:%02d:%02d\n",
           progress * 100.0,
           (idx < ud->fl->n) ? (idx + 1) : ud->fl->n,
           ud->fl->n,
           (idx < ud->fl->n) ? ud->fl->names[idx] : "(done)",
           file_pct,
           h, m, s);
    fflush(stdout);
    ud->last_progress = progress;
}

/* free userdata called by libzip when unregistering */
static void free_prog_ud(void *ud_v) {
    prog_ud_t *ud = (prog_ud_t*)ud_v;
    if (!ud) return;
    free(ud);
}

/* Create zip for a directory path (full directory tree inside one zip) */
static int create_zip_for_dir(const char *dirpath, const char *zipname) {
    file_list_t fl;
    fl_init(&fl);

    size_t base_len = strlen(dirpath);
    if (dirpath[base_len - 1] == '/') {
        /* keep base_len */
    }

    if (collect_files_recursive(dirpath, base_len, &fl) != 0) {
        fl_free(&fl);
        return -1;
    }

    printf("Creating zip %s  total bytes %" PRIu64 "  files %zu\n", zipname, fl.total, fl.n);

    int errorp = 0;
    zip_t *za = zip_open(zipname, ZIP_CREATE | ZIP_TRUNCATE, &errorp);
    if (!za) {
        char buf[1024];
        zip_error_to_str(buf, sizeof(buf), errorp, errno);
        fprintf(stderr, "zip_open failed: %s\n", buf);
        fl_free(&fl);
        return -1;
    }

    for (size_t i = 0; i < fl.n; ++i) {
        zip_source_t *zs = zip_source_file(za, fl.paths[i], 0, 0);
        if (!zs) {
            fprintf(stderr, "zip_source_file failed for %s: %s\n", fl.paths[i], zip_strerror(za));
            zip_discard(za);
            fl_free(&fl);
            return -1;
        }
        zip_int64_t idx = zip_file_add(za, fl.names[i], zs, ZIP_FL_ENC_UTF_8);
        if (idx < 0) {
            fprintf(stderr, "zip_file_add failed for %s: %s\n", fl.names[i], zip_strerror(za));
            zip_source_free(zs);
            zip_discard(za);
            fl_free(&fl);
            return -1;
        }
        zip_set_file_compression(za, idx, ZIP_CM_DEFLATE, 9);
    }

    prog_ud_t *ud = malloc(sizeof(prog_ud_t));
    if (!ud) {
        fprintf(stderr, "Out of memory\n");
        zip_discard(za);
        fl_free(&fl);
        return -1;
    }
    ud->fl = &fl;
    ud->start_time = now_seconds();
    ud->last_progress = 0.0;

    int reg_ok = 0;
    if (zip_register_progress_callback_with_state(za, 0.01, progress_cb, free_prog_ud, ud) >= 0) {
        reg_ok = 1;
    } else {
        /* registration failed; we must free ud ourselves */
        fprintf(stderr, "Warning: progress callback registration failed, continuing without progress updates\n");
        free(ud);
        ud = NULL;
    }

    if (zip_close(za) != 0) {
        fprintf(stderr, "zip_close failed: %s\n", zip_strerror(za));
        /* If registration succeeded, libzip will free ud via free_prog_ud.
           If registration failed, we already freed ud above. */
        fl_free(&fl);
        return -1;
    }

    /* If registration succeeded, libzip already freed ud via free_prog_ud.
       If registration failed, ud was freed earlier. Do not free ud here. */

    double total_elapsed = now_seconds() - (reg_ok ? ud->start_time : now_seconds());
    /* Note: ud may be NULL here if reg_ok==0; avoid dereferencing ud in that case.
       We already printed progress during zip_close; print final summary using fl.total. */
    printf("Finished %s  bytes=%" PRIu64 "  time=%.2fs\n", zipname, fl.total, (double)(now_seconds() - (reg_ok ? ud->start_time : now_seconds())));

    fl_free(&fl);
    return 0;
}

/* Create zip containing a single file with progress reporting */
static int create_zip_for_file(const char *filepath, const char *zipname, const char *stored_name, uint64_t filesize) {
    int errorp = 0;
    zip_t *za = zip_open(zipname, ZIP_CREATE | ZIP_TRUNCATE, &errorp);
    if (!za) {
        char buf[1024];
        zip_error_to_str(buf, sizeof(buf), errorp, errno);
        fprintf(stderr, "zip_open failed: %s\n", buf);
        return -1;
    }
    zip_source_t *zs = zip_source_file(za, filepath, 0, 0);
    if (!zs) {
        fprintf(stderr, "zip_source_file failed for %s: %s\n", filepath, zip_strerror(za));
        zip_close(za);
        return -1;
    }
    if (zip_file_add(za, stored_name, zs, ZIP_FL_ENC_UTF_8) < 0) {
        fprintf(stderr, "zip_file_add failed: %s\n", zip_strerror(za));
        zip_source_free(zs);
        zip_close(za);
        return -1;
    }

    /* Build a tiny file_list for progress callback */
    file_list_t fl;
    fl_init(&fl);
    fl_push(&fl, filepath, stored_name, filesize);

    prog_ud_t *ud = malloc(sizeof(prog_ud_t));
    if (!ud) {
        fprintf(stderr, "Out of memory\n");
        zip_discard(za);
        fl_free(&fl);
        return -1;
    }
    ud->fl = &fl;
    ud->start_time = now_seconds();
    ud->last_progress = 0.0;

    int reg_ok = 0;
    if (zip_register_progress_callback_with_state(za, 0.01, progress_cb, free_prog_ud, ud) >= 0) {
        reg_ok = 1;
    } else {
        /* registration failed; free ud ourselves */
        free(ud);
        ud = NULL;
    }

    if (zip_close(za) != 0) {
        fprintf(stderr, "zip_close failed: %s\n", zip_strerror(za));
        fl_free(&fl);
        return -1;
    }

    /* If registration succeeded, libzip freed ud via free_prog_ud.
       If registration failed, ud was freed above. */

    printf("Finished %s  bytes=%" PRIu64 "  time=%.2fs\n", zipname, fl.total, (double)(now_seconds() - (reg_ok ? ud->start_time : now_seconds())));
    fl_free(&fl);
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

/* Helper to get basename from path */
static const char *get_basename(const char *path) {
    const char *p = strrchr(path, '/');
    return p ? p + 1 : path;
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
    struct stat st_out;
    if (stat(outdir, &st_out) != 0) {
        if (mkdir(outdir, 0755) != 0) {
            fprintf(stderr, "Cannot create output directory %s: %s\n", outdir, strerror(errno));
            return 1;
        }
    } else {
        if (!S_ISDIR(st_out.st_mode)) {
            fprintf(stderr, "Output path %s is not a directory\n", outdir);
            return 1;
        }
    }

    if (S_ISREG(st.st_mode)) {
        /* Input is a single file: create a zip containing that file */
        char zipname[4096];
        const char *basename = get_basename(input);
        snprintf(zipname, sizeof(zipname), "%s/%s.zip", outdir, basename);
        if (create_zip_for_file(input, zipname, basename, (uint64_t)st.st_size) != 0) {
            fprintf(stderr, "Failed to create zip for %s\n", input);
            return 1;
        }
        return 0;
    } else if (S_ISDIR(st.st_mode)) {
        /* Input is a directory. For each immediate child (file or folder) create a separate zip */
        DIR *d = opendir(input);
        if (!d) {
            fprintf(stderr, "opendir failed on %s: %s\n", input, strerror(errno));
            return 1;
        }
        struct dirent *ent;
        int any = 0;
        while ((ent = readdir(d)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            any = 1;
            char childpath[4096];
            join_path(childpath, sizeof(childpath), input, ent->d_name);
            struct stat st2;
            if (lstat(childpath, &st2) != 0) {
                fprintf(stderr, "stat failed on %s: %s\n", childpath, strerror(errno));
                continue;
            }
            char zipname[4096];
            snprintf(zipname, sizeof(zipname), "%s/%s.zip", outdir, ent->d_name);
            if (S_ISREG(st2.st_mode)) {
                /* Create zip containing single file */
                printf("Zipping file %s -> %s\n", childpath, zipname);
                if (create_zip_for_file(childpath, zipname, ent->d_name, (uint64_t)st2.st_size) != 0) {
                    fprintf(stderr, "Failed to create zip for file %s\n", childpath);
                }
            } else if (S_ISDIR(st2.st_mode)) {
                /* Create zip for directory tree */
                printf("Zipping directory %s -> %s\n", childpath, zipname);
                if (create_zip_for_dir(childpath, zipname) != 0) {
                    fprintf(stderr, "Failed to create zip for directory %s\n", childpath);
                }
            } else {
                fprintf(stderr, "Skipping unsupported type: %s\n", childpath);
            }
        }
        closedir(d);
        if (!any) {
            printf("Input directory %s is empty\n", input);
        }
        return 0;
    } else {
        fprintf(stderr, "Input path is neither file nor directory\n");
        return 1;
    }
}
