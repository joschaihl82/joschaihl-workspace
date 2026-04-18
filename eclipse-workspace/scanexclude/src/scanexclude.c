/*
 * scan_excluded.c
 *
 * Scannt rekursiv ein Verzeichnis und listet alle regulären Dateien auf,
 * die NICHT in der erlaubten Endungs- oder Dateinamensliste vorkommen.
 * Zusätzlich werden Anzahl und Gesamtgröße (in MB) der gefundenen Dateien
 * sowie Anzahl und Gesamtgröße der Dateien mit erlaubter Endung ausgegeben.
 *
 * Kompilieren:
 *   gcc -std=c11 -D_XOPEN_SOURCE=700 -O2 -o scan_excluded scan_excluded.c
 *
 * Nutzung:
 *   ./scan_excluded /pfad/zum/quellcode
 *
 * Rückgabewert:
 *   0  -> erfolgreich (auch wenn keine Dateien gefunden wurden)
 *   2  -> Fehler bei Aufruf / Verzeichnis nicht erreichbar
 *
 */

#define _XOPEN_SOURCE 700
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <libgen.h>
#include <sys/stat.h>
#include <errno.h>

/* Erlaubte Endungen (ohne Binaries: .o, .so, .a, .dll, .class wurden entfernt) */
static const char *allowed_exts[] = {
    /* C/C++ */
    ".c", ".cpp", ".cxx", ".h", ".hpp", ".hh", ".inl",
    /* Assembly */
    ".S", ".s", ".asm",
    /* Build / project */
    ".pro", ".mk", ".makefile", ".cmake",
    /* Scripts and languages */
    ".sh", ".py", ".pl", ".rb", ".php", ".java", ".kt", ".kts", ".go", ".rs",
    /* Web / frontend */
    ".js", ".jsx", ".ts", ".tsx", ".html", ".css",
    /* Data / config / docs */
    ".txt", ".md", ".rst", ".yml", ".yaml", ".json", ".xml", ".ini", ".cfg",
    /* Images / assets */
    ".png", ".jpg", ".jpeg", ".gif", ".bmp", ".svg", ".ico",
    /* package / build descriptors */
    ".gradle"
};

static const size_t allowed_exts_count = sizeof(allowed_exts) / sizeof(allowed_exts[0]);

/* Erlaubte Dateinamen (exakt) */
static const char *allowed_names[] = {
    "Makefile",
    "CMakeLists.txt",
    "README",
    "README.md",
    "LICENSE",
    ".gitignore",
    ".gitattributes",
    ".editorconfig",
    ".clang-format",
    ".clang-tidy",
    "configure",
    "configure.ac",
    "autogen.sh",
    "build.gradle",
    "pom.xml",
    "package.json",
    "Dockerfile",
    ".dockerignore"
};

static const size_t allowed_names_count = sizeof(allowed_names) / sizeof(allowed_names[0]);

/* Option: set to 1 für Fallunempfindlichkeit bei Endungen */
static const int CASE_INSENSITIVE_EXT = 1;

/* Zähler und Summen für ausgegrenzte Dateien */
static unsigned long long total_excluded_bytes = 0ULL;
static size_t total_excluded_files = 0;

/* Zähler und Summen für Dateien mit erlaubter Endung */
static unsigned long long total_allowed_ext_bytes = 0ULL;
static size_t total_allowed_ext_files = 0;

/* Hilfsfunktion: prüft, ob name in allowed_names ist (exakt) */
static int is_allowed_name(const char *name) {
    for (size_t i = 0; i < allowed_names_count; ++i) {
        if (strcmp(name, allowed_names[i]) == 0) return 1;
    }
    return 0;
}

/* Hilfsfunktion: prüft, ob dateiname eine erlaubte Endung hat */
static int has_allowed_ext(const char *name) {
    const char *dot = strrchr(name, '.');
    if (!dot) return 0;
    for (size_t i = 0; i < allowed_exts_count; ++i) {
        if (CASE_INSENSITIVE_EXT) {
            if (strcasecmp(dot, allowed_exts[i]) == 0) return 1;
        } else {
            if (strcmp(dot, allowed_exts[i]) == 0) return 1;
        }
    }
    return 0;
}

/* nftw callback */
static int visitor(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    (void) ftwbuf;
    if (typeflag != FTW_F) return 0; /* nur reguläre Dateien */

    const char *base = strrchr(fpath, '/');
    const char *fname = base ? base + 1 : fpath;

    /* Erlaubte Dateinamen (exakt) -> nicht zählen, nicht ausgeben */
    if (is_allowed_name(fname)) return 0;

    /* Erlaubte Endungen -> Größe und Anzahl für erlaubte Endungen aufsummieren */
    if (has_allowed_ext(fname)) {
        total_allowed_ext_bytes += (unsigned long long) sb->st_size;
        total_allowed_ext_files += 1;
        return 0;
    }

    /* Wenn weder Name noch Endung erlaubt, ausgeben und Größe aufsummieren */
    puts(fpath);
    total_excluded_bytes += (unsigned long long) sb->st_size;
    total_excluded_files += 1;

    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source-directory>\n", argv[0]);
        return 2;
    }

    const char *start = argv[1];

    /* Prüfen, ob Startverzeichnis existiert und zugreifbar ist */
    struct stat st;
    if (stat(start, &st) != 0) {
        fprintf(stderr, "Fehler: Kann '%s' nicht statten: %s\n", start, strerror(errno));
        return 2;
    }
    if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "Fehler: '%s' ist kein Verzeichnis\n", start);
        return 2;
    }

    /* Rekursiver Scan mit nftw; 20 offene Deskriptoren, FTW_PHYS um Symlinks nicht zu folgen */
    if (nftw(start, visitor, 20, FTW_PHYS) != 0) {
        fprintf(stderr, "Fehler beim Durchlaufen von '%s'\n", start);
        return 2;
    }

    /* Ausgabe der Statistik: Anzahl und Gesamtgröße in Megabyte */
    double excluded_mb = (double) total_excluded_bytes / (1024.0 * 1024.0);
    double allowed_ext_mb = (double) total_allowed_ext_bytes / (1024.0 * 1024.0);

    printf("\nGefundene nicht erlaubte Dateien: %zu\n", total_excluded_files);
    printf("Gesamtgröße der nicht erlaubten Dateien: %.2f MB\n", excluded_mb);

    printf("\nDateien mit erlaubter Endung: %zu\n", total_allowed_ext_files);
    printf("Gesamtgröße der Dateien mit erlaubter Endung: %.2f MB\n", allowed_ext_mb);

    return 0;
}
