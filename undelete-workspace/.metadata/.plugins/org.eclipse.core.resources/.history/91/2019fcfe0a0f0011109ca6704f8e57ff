/*
 * btrfs_undelete.c
 *
 * Practical wrapper around `btrfs restore` to list recoverable deleted files
 * under a path prefix and to restore a single deleted file by filename.
 *
 * - Uses fork+exec to call btrfs restore (no shell).
 * - List mode: runs `btrfs restore -D -v device tmpdir` and parses stdout.
 * - Restore mode: runs `btrfs restore --path-regex '^<path>$' -v device outdir`.
 *
 * Limitations:
 * - Relies on btrfs-progs (btrfs restore) being installed.
 * - Parsing is heuristic: if btrfs-progs output format changes, adjust parsing.
 *
 * Build:
 *   gcc -O2 -o btrfs_undelete btrfs_undelete.c
 *
 * Run as root or with read access to the device/image.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <dirent.h>

static void usage(const char *p) {
    fprintf(stderr,
        "Usage:\n"
        "  %s --device DEVICE --list PATH_PREFIX\n"
        "  %s --device DEVICE --restore PATH_TO_FILE --outdir OUTDIR\n"
        "\nExamples:\n"
        "  %s --device /dev/sdb1 --list /home/josh\n"
        "  %s --device /dev/sdb1 --restore /home/josh/secret.txt --outdir ./recovered\n",
        p, p, p, p);
}

/* Create a temporary directory and return its path (caller must free) */
static char *make_tempdir(void) {
    char *tmpl = strdup("/tmp/btrfs_undelete.XXXXXX");
    if (!tmpl) return NULL;
    char *d = mkdtemp(tmpl);
    if (!d) { free(tmpl); return NULL; }
    return tmpl; /* caller frees */
}

/* Run a command with argv (NULL-terminated) and capture stdout into a temporary file.
 * Returns path to temp file (caller frees) or NULL on error.
 */
static char *run_command_capture_stdout(char *const argv[]) {
    int pipefd[2];
    if (pipe(pipefd) < 0) return NULL;
    pid_t pid = fork();
    if (pid < 0) { close(pipefd[0]); close(pipefd[1]); return NULL; }
    if (pid == 0) {
        /* child */
        close(pipefd[0]);
        if (dup2(pipefd[1], STDOUT_FILENO) < 0) _exit(127);
        close(pipefd[1]);
        /* stderr left as-is so errors appear on console */
        execvp(argv[0], argv);
        _exit(127);
    }
    /* parent */
    close(pipefd[1]);
    /* write stdout to temp file */
    char *tmpname = strdup("/tmp/btrfs_undelete_out.XXXXXX");
    if (!tmpname) { close(pipefd[0]); return NULL; }
    int tmpfd = mkstemp(tmpname);
    if (tmpfd < 0) { close(pipefd[0]); free(tmpname); return NULL; }
    ssize_t r;
    char buf[4096];
    while ((r = read(pipefd[0], buf, sizeof(buf))) > 0) {
        ssize_t w = write(tmpfd, buf, r);
        if (w != r) { /* ignore partial write errors for now */ break; }
    }
    close(pipefd[0]);
    close(tmpfd);
    int status = 0;
    waitpid(pid, &status, 0);
    /* return temp filename even if command failed; caller can inspect */
    return tmpname;
}

/* Heuristic: check if a line contains a path under prefix.
 * We accept lines that contain the prefix as a substring and a '/' before it.
 */
static int line_contains_prefix(const char *line, const char *prefix) {
    const char *p = strstr(line, prefix);
    if (!p) return 0;
    /* ensure prefix is path component start (either line start or preceded by space or ':') */
    if (p == line) return 1;
    char prev = *(p - 1);
    if (prev == ' ' || prev == ':' || prev == '\t' || prev == '/') return 1;
    return 0;
}

/* List recoverable files under prefix by running `btrfs restore -D -v device tmpdir`
 * and scanning output for lines that contain the prefix.
 */
static int list_deleted(const char *device, const char *prefix) {
    char *tmpdir = make_tempdir();
    if (!tmpdir) {
        fprintf(stderr, "Failed to create temporary directory\n");
        return 1;
    }

    /* argv: btrfs restore -D -v device tmpdir */
    char *const argv[] = { "btrfs", "restore", "-D", "-v", (char*)device, tmpdir, NULL };
    char *outfile = run_command_capture_stdout(argv);
    if (!outfile) {
        fprintf(stderr, "Failed to run btrfs restore\n");
        rmdir(tmpdir);
        free(tmpdir);
        return 1;
    }

    FILE *f = fopen(outfile, "r");
    if (!f) {
        perror("fopen");
        unlink(outfile);
        free(outfile);
        rmdir(tmpdir);
        free(tmpdir);
        return 1;
    }

    printf("Recoverable files under %s:\n", prefix);
    char line[4096];
    int found = 0;
    while (fgets(line, sizeof(line), f)) {
        /* trim newline */
        size_t L = strlen(line);
        while (L && (line[L-1] == '\n' || line[L-1] == '\r')) { line[--L] = '\0'; }
        if (line_contains_prefix(line, prefix)) {
            /* try to extract a path-like substring */
            const char *p = strstr(line, prefix);
            if (p) {
                /* print from prefix to end of line */
                printf("%s\n", p);
                found = 1;
            }
        }
    }
    if (!found) printf("(none found or btrfs restore could not list recoverable files)\n");

    fclose(f);
    unlink(outfile);
    free(outfile);
    rmdir(tmpdir);
    free(tmpdir);
    return 0;
}

/* Restore a single file by exact path using --path-regex.
 * We build a regex that matches the exact path: ^<path>$ (anchored).
 */
static int restore_file(const char *device, const char *path, const char *outdir) {
    /* ensure outdir exists */
    struct stat st;
    if (stat(outdir, &st) < 0) {
        if (mkdir(outdir, 0700) < 0) {
            perror("mkdir outdir");
            return 1;
        }
    } else if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "Outdir exists and is not a directory\n");
        return 1;
    }

    /* build regex argument */
    char regex[PATH_MAX + 10];
    if (snprintf(regex, sizeof(regex), "^%s$", path) >= (int)sizeof(regex)) {
        fprintf(stderr, "Path too long\n");
        return 1;
    }

    /* argv: btrfs restore --path-regex regex -v device outdir */
    char *const argv[] = { "btrfs", "restore", "--path-regex", regex, "-v", (char*)device, (char*)outdir, NULL };

    /* run and stream output to console */
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid == 0) {
        execvp(argv[0], argv);
        perror("execvp");
        _exit(127);
    }
    int status = 0;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        printf("Restore completed (check %s for recovered file(s)).\n", outdir);
        return 0;
    } else {
        fprintf(stderr, "btrfs restore failed or returned nonzero status\n");
        return 1;
    }
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

    if (list_prefix) {
        return list_deleted(device, list_prefix);
    } else if (restore_path) {
        if (!outdir) { fprintf(stderr, "--outdir is required for --restore\n"); return 1; }
        return restore_file(device, restore_path, outdir);
    } else {
        usage(argv[0]);
        return 1;
    }
}
