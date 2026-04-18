// lz4_packer_recursive.c
// Compile: gcc -o lz4_packer_recursive lz4_packer_recursive.c -llz4 -Wall -O2
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include <lz4frame.h>
#include <unistd.h>

#define CHUNK 65536
#define SIZE_THRESHOLD (2ULL * 1024ULL * 1024ULL * 1024ULL) // 2 GiB

static const char *root_path = NULL;

static void perror_exit(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

static int has_lz4_extension(const char *path) {
    size_t len = strlen(path);
    return (len > 4 && strcmp(path + len - 4, ".lz4") == 0);
}

static int compress_stream_to_file(FILE *in, const char *outpath) {
    FILE *out = fopen(outpath, "wb");
    if (!out) return -1;

    LZ4F_compressionContext_t ctx;
    size_t r = LZ4F_createCompressionContext(&ctx, LZ4F_VERSION);
    if (LZ4F_isError(r)) { fclose(out); return -1; }

    LZ4F_preferences_t prefs;
    memset(&prefs, 0, sizeof(prefs));
    prefs.frameInfo.blockSizeID = LZ4F_max64KB;

    unsigned char header[LZ4F_HEADER_SIZE_MAX];
    size_t headerSize = LZ4F_compressBegin(ctx, header, sizeof(header), &prefs);
    if (LZ4F_isError(headerSize)) { LZ4F_freeCompressionContext(ctx); fclose(out); return -1; }
    if (fwrite(header, 1, headerSize, out) != headerSize) { LZ4F_freeCompressionContext(ctx); fclose(out); return -1; }

    unsigned char inbuf[CHUNK];
    size_t maxC = LZ4F_compressBound(CHUNK, &prefs);
    unsigned char *cbuf = malloc(maxC);
    if (!cbuf) { LZ4F_freeCompressionContext(ctx); fclose(out); return -1; }

    size_t read;
    while ((read = fread(inbuf, 1, CHUNK, in)) > 0) {
        size_t cSize = LZ4F_compressUpdate(ctx, cbuf, maxC, inbuf, read, NULL);
        if (LZ4F_isError(cSize)) { free(cbuf); LZ4F_freeCompressionContext(ctx); fclose(out); return -1; }
        if (fwrite(cbuf, 1, cSize, out) != cSize) { free(cbuf); LZ4F_freeCompressionContext(ctx); fclose(out); return -1; }
    }
    if (ferror(in)) { free(cbuf); LZ4F_freeCompressionContext(ctx); fclose(out); return -1; }

    size_t endSize = LZ4F_compressEnd(ctx, cbuf, maxC, NULL);
    if (LZ4F_isError(endSize)) { free(cbuf); LZ4F_freeCompressionContext(ctx); fclose(out); return -1; }
    if (fwrite(cbuf, 1, endSize, out) != endSize) { free(cbuf); LZ4F_freeCompressionContext(ctx); fclose(out); return -1; }

    free(cbuf);
    LZ4F_freeCompressionContext(ctx);
    fclose(out);
    return 0;
}

static int handle_file(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    (void)ftwbuf;
    if (typeflag == FTW_SL) {
        // skip symbolic links
        return 0;
    }

    if (typeflag != FTW_F) {
        // only regular files
        return 0;
    }

    // skip files that already have .lz4 extension
    if (has_lz4_extension(fpath)) {
        printf("Übersprungen (bereits .lz4): %s\n", fpath);
        return 0;
    }

    // check size threshold
    unsigned long long filesize = (unsigned long long) sb->st_size;
    if (filesize <= SIZE_THRESHOLD) {
        // Datei ist <= 2 GiB: nichts tun
        return 0;
    }

    // build output path: fpath + ".lz4"
    char outpath[PATH_MAX];
    if (snprintf(outpath, sizeof(outpath), "%s.lz4", fpath) >= (int)sizeof(outpath)) {
        fprintf(stderr, "Ausgabepfad zu lang für %s\n", fpath);
        return 0;
    }

    // If output exists, skip to avoid overwriting
    if (access(outpath, F_OK) == 0) {
        fprintf(stderr, "Ausgabedatei existiert, übersprungen: %s\n", outpath);
        return 0;
    }

    FILE *in = fopen(fpath, "rb");
    if (!in) {
        fprintf(stderr, "Kann Datei nicht öffnen: %s (%s)\n", fpath, strerror(errno));
        return 0;
    }

    printf("Komprimiere große Datei: %s (%.0f MiB) -> %s\n", fpath, (double)filesize / (1024.0 * 1024.0), outpath);
    if (compress_stream_to_file(in, outpath) != 0) {
        fprintf(stderr, "Kompression fehlgeschlagen für %s\n", fpath);
        // remove partial output if created
        unlink(outpath);
    } else {
        printf("Fertig: %s\n", outpath);
    }

    fclose(in);
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s /pfad/zum/verzeichnis\n", argv[0]);
        return EXIT_FAILURE;
    }

    root_path = argv[1];

    struct stat st;
    if (stat(root_path, &st) != 0) {
        perror("stat");
        return EXIT_FAILURE;
    }
    if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "Der angegebene Pfad ist kein Verzeichnis: %s\n", root_path);
        return EXIT_FAILURE;
    }

    // nftw: max 20 open fds, FTW_PHYS to avoid following symlinks
    if (nftw(root_path, handle_file, 20, FTW_PHYS) == -1) {
        perror("nftw");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
