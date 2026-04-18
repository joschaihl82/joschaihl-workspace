/*
 gdrive1.c
 Konsolen-Version: Metadaten abrufen, Pfade rekonstruieren, Dateien herunterladen.
 Basierend auf: DriveSync - Ordnerstruktur rekonstruieren und alle Dateien herunterladen (Qt6)
 Hinweis: Zwei-Wege-Synchronisation, Tray-Icon und UI-Teile wurden entfernt; Fokus: Konsolen-Download.

 Compile:
   gcc -o gdrive1 gdrive1.c -lcurl -lcjson
 Abhängigkeiten:
   libcurl, cJSON (z.B. apt install libcurl4-openssl-dev libcjson-dev)
 Usage:
   ./gdrive1 --token-file tokens.json --sync-dir /home/you/google_drive
*/

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <libgen.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <curl/curl.h>
#include <cjson/cJSON.h>

/* --- Data structures --- */

typedef struct {
    char *id;
    char *name;
    char *mimeType;
    char **parents; /* NULL-terminated */
    char *size;
    char *modifiedTime;
    cJSON *raw; /* duplicate of original JSON object */
} FileMeta;

typedef struct {
    FileMeta **items;
    size_t count;
} MetaList;

/* --- Utilities --- */

static void *xmalloc(size_t s) { void *p = malloc(s); if (!p) { fprintf(stderr, "Out of memory\n"); exit(1); } return p; }
static char *strdup_safe(const char *s) { if (!s) return NULL; char *r = strdup(s); if (!r) r = strdup(""); return r; }

/* Ensure directory for a given file path exists (recursive mkdir -p) */
static int ensure_dir_for_file(const char *path) {
    if (!path || path[0] == '\0') return -1;
    char *tmp = strdup_safe(path);
    if (!tmp) return -1;
    char *dirpart = dirname(tmp); /* may modify tmp */
    if (!dirpart) { free(tmp); return -1; }

    /* Create directories recursively */
    char cur[PATH_MAX];
    cur[0] = '\0';

    const char *s = dirpart;
    /* If absolute path, start with "/" */
    if (s[0] == '/') {
        strcpy(cur, "/");
        s++;
    }

    while (*s) {
        const char *slash = strchr(s, '/');
        size_t len = slash ? (size_t)(slash - s) : strlen(s);
        if (len == 0) { s += 1; continue; }
        if (strlen(cur) + len + 2 >= sizeof(cur)) break;
        if (strcmp(cur, "/") != 0 && cur[0] != '\0') strcat(cur, "/");
        strncat(cur, s, len);
        if (access(cur, F_OK) != 0) {
            if (mkdir(cur, 0755) != 0 && errno != EEXIST) {
                fprintf(stderr, "mkdir %s failed: %s\n", cur, strerror(errno));
                free(tmp);
                return -1;
            }
        }
        if (!slash) break;
        s = slash + 1;
    }

    free(tmp);
    return 0;
}

/* --- CURL helpers --- */

struct Memory {
    char *data;
    size_t size;
};

static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t realsz = size * nmemb;
    struct Memory *mem = (struct Memory*)userdata;
    char *p = realloc(mem->data, mem->size + realsz + 1);
    if (!p) return 0;
    mem->data = p;
    memcpy(mem->data + mem->size, ptr, realsz);
    mem->size += realsz;
    mem->data[mem->size] = 0;
    return realsz;
}

static int http_get_to_memory(CURL *curl, const char *url, const char *bearer, struct Memory *out) {
    CURLcode res;
    curl_easy_reset(curl);
    out->data = NULL; out->size = 0;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, out);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    struct curl_slist *hdrs = NULL;
    if (bearer) {
        char auth[4096];
        snprintf(auth, sizeof(auth), "Authorization: Bearer %s", bearer);
        hdrs = curl_slist_append(hdrs, auth);
    }
    hdrs = curl_slist_append(hdrs, "User-Agent: DriveSync-C");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdrs);
    res = curl_easy_perform(curl);
    curl_slist_free_all(hdrs);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl error: %s\n", curl_easy_strerror(res));
        free(out->data); out->data = NULL; out->size = 0;
        return -1;
    }
    return 0;
}

/* --- JSON parsing helpers --- */

static char **parse_parents_array(cJSON *arr) {
    if (!arr || !cJSON_IsArray(arr)) return NULL;
    size_t n = cJSON_GetArraySize(arr);
    char **res = calloc(n + 1, sizeof(char*));
    for (size_t i = 0; i < n; ++i) {
        cJSON *it = cJSON_GetArrayItem(arr, i);
        res[i] = strdup_safe(cJSON_IsString(it) ? it->valuestring : "");
    }
    res[n] = NULL;
    return res;
}

static FileMeta *filemeta_from_json(cJSON *obj) {
    if (!obj) return NULL;
    FileMeta *m = xmalloc(sizeof(FileMeta));
    memset(m, 0, sizeof(FileMeta));
    cJSON *j;
    j = cJSON_GetObjectItemCaseSensitive(obj, "id");
    m->id = strdup_safe(cJSON_IsString(j) ? j->valuestring : "");
    j = cJSON_GetObjectItemCaseSensitive(obj, "name");
    m->name = strdup_safe(cJSON_IsString(j) ? j->valuestring : "");
    j = cJSON_GetObjectItemCaseSensitive(obj, "mimeType");
    m->mimeType = strdup_safe(cJSON_IsString(j) ? j->valuestring : "");
    j = cJSON_GetObjectItemCaseSensitive(obj, "parents");
    m->parents = parse_parents_array(j);
    j = cJSON_GetObjectItemCaseSensitive(obj, "size");
    if (cJSON_IsString(j)) m->size = strdup_safe(j->valuestring);
    else if (cJSON_IsNumber(j)) {
        char tmp[32];
        snprintf(tmp, sizeof(tmp), "%lld", (long long)j->valuedouble);
        m->size = strdup_safe(tmp);
    } else m->size = strdup_safe("0");
    j = cJSON_GetObjectItemCaseSensitive(obj, "modifiedTime");
    m->modifiedTime = strdup_safe(cJSON_IsString(j) ? j->valuestring : "");
    m->raw = cJSON_Duplicate(obj, 1);
    return m;
}

static void free_filemeta(FileMeta *m) {
    if (!m) return;
    free(m->id); free(m->name); free(m->mimeType); free(m->size); free(m->modifiedTime);
    if (m->parents) {
        for (char **p = m->parents; p && *p; ++p) free(*p);
        free(m->parents);
    }
    if (m->raw) cJSON_Delete(m->raw);
    free(m);
}

/* --- Index and path reconstruction --- */

static FileMeta *find_by_id(MetaList *list, const char *id) {
    if (!id) return NULL;
    for (size_t i = 0; i < list->count; ++i) {
        if (strcmp(list->items[i]->id, id) == 0) return list->items[i];
    }
    return NULL;
}

/* Build full path by following parents (first parent chosen). Detect cycles. */
static char *build_full_path(MetaList *list, const char *id) {
    if (!id) return strdup_safe("");
    size_t max = list->count ? list->count : 1;
    char **visited = calloc(max, sizeof(char*));
    size_t vcount = 0;
    char **parts = calloc(max + 1, sizeof(char*));
    size_t pcount = 0;
    const char *cur = id;
    while (cur && cur[0]) {
        int cyc = 0;
        for (size_t i = 0; i < vcount; ++i) if (strcmp(visited[i], cur) == 0) { cyc = 1; break; }
        if (cyc) { fprintf(stderr, "Cycle detected in parents for id=%s\n", cur); break; }
        visited[vcount++] = strdup_safe(cur);
        FileMeta *m = find_by_id(list, cur);
        if (!m) break;
        if (m->name && m->name[0]) parts[pcount++] = strdup_safe(m->name);
        if (m->parents && m->parents[0] && m->parents[0][0]) cur = m->parents[0];
        else break;
    }
    if (pcount == 0) {
        for (size_t i = 0; i < vcount; ++i) free(visited[i]);
        free(visited);
        free(parts);
        return strdup_safe("");
    }
    size_t total_len = 0;
    for (size_t i = 0; i < pcount; ++i) total_len += strlen(parts[i]) + 1;
    char *out = xmalloc(total_len + 1);
    out[0] = '\0';
    for (size_t i = 0; i < pcount; ++i) {
        if (i) strcat(out, "/");
        strcat(out, parts[pcount - 1 - i]); /* reverse order */
    }
    for (size_t i = 0; i < vcount; ++i) free(visited[i]);
    free(visited);
    for (size_t i = 0; i < pcount; ++i) free(parts[i]);
    free(parts);
    return out;
}

/* --- Metadata fetch (pagination) --- */

static int fetch_all_metadata(CURL *curl, const char *token, MetaList *outList) {
    const char *base = "https://www.googleapis.com/drive/v3/files";
    char pageToken[2048] = {0};
    outList->items = NULL; outList->count = 0;

    while (1) {
        char url[4096];
        if (pageToken[0]) {
            snprintf(url, sizeof(url), "%s?pageSize=1000&fields=nextPageToken,files(id,name,mimeType,parents,size,modifiedTime,md5Checksum)&pageToken=%s", base, pageToken);
        } else {
            snprintf(url, sizeof(url), "%s?pageSize=1000&fields=nextPageToken,files(id,name,mimeType,parents,size,modifiedTime,md5Checksum)", base);
        }
        struct Memory mem = {0};
        if (http_get_to_memory(curl, url, token, &mem) != 0) {
            fprintf(stderr, "Failed to fetch metadata\n");
            return -1;
        }
        cJSON *doc = cJSON_Parse(mem.data ? mem.data : "{}");
        free(mem.data);
        if (!doc) { fprintf(stderr, "Invalid JSON from metadata\n"); return -1; }
        cJSON *files = cJSON_GetObjectItemCaseSensitive(doc, "files");
        if (cJSON_IsArray(files)) {
            size_t n = cJSON_GetArraySize(files);
            for (size_t i = 0; i < n; ++i) {
                cJSON *it = cJSON_GetArrayItem(files, i);
                FileMeta *m = filemeta_from_json(it);
                outList->items = realloc(outList->items, sizeof(FileMeta*) * (outList->count + 1));
                outList->items[outList->count++] = m;
            }
        }
        cJSON *npt = cJSON_GetObjectItemCaseSensitive(doc, "nextPageToken");
        if (npt && cJSON_IsString(npt) && npt->valuestring && npt->valuestring[0]) {
            strncpy(pageToken, npt->valuestring, sizeof(pageToken) - 1);
            cJSON_Delete(doc);
            continue;
        } else {
            cJSON_Delete(doc);
            break;
        }
    }
    return 0;
}

/* --- Download file (single, blocking) --- */

static int download_file(CURL *curl, const char *token, const char *fileId, const char *outPath) {
    char url[1024];
    snprintf(url, sizeof(url), "https://www.googleapis.com/drive/v3/files/%s?alt=media", fileId);
    char tmpPath[PATH_MAX];
    snprintf(tmpPath, sizeof(tmpPath), "%s.part", outPath);
    FILE *f = fopen(tmpPath, "wb");
    if (!f) { fprintf(stderr, "Cannot open %s: %s\n", tmpPath, strerror(errno)); return -1; }
    curl_easy_reset(curl);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    struct curl_slist *hdrs = NULL;
    if (token) {
        char auth[4096];
        snprintf(auth, sizeof(auth), "Authorization: Bearer %s", token);
        hdrs = curl_slist_append(hdrs, auth);
    }
    hdrs = curl_slist_append(hdrs, "User-Agent: DriveSync-C");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdrs);
    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(hdrs);
    fclose(f);
    if (res != CURLE_OK) {
        fprintf(stderr, "Download failed: %s\n", curl_easy_strerror(res));
        remove(tmpPath);
        return -1;
    }
    if (rename(tmpPath, outPath) != 0) {
        fprintf(stderr, "Rename failed: %s\n", strerror(errno));
        remove(tmpPath);
        return -1;
    }
    return 0;
}

/* --- Main --- */

int main(int argc, char **argv) {
    const char *token_file = "tokens.json";
    const char *sync_dir = NULL;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--token-file") == 0 && i + 1 < argc) token_file = argv[++i];
        else if (strcmp(argv[i], "--sync-dir") == 0 && i + 1 < argc) sync_dir = argv[++i];
        else fprintf(stderr, "Unknown arg: %s\n", argv[i]);
    }
    if (!sync_dir) {
        char *home = getenv("HOME");
        if (!home) home = ".";
        static char default_dir[PATH_MAX];
        snprintf(default_dir, sizeof(default_dir), "%s/google_drive", home);
        sync_dir = default_dir;
    }

    /* Read token file (expects JSON with access_token) */
    FILE *tf = fopen(token_file, "rb");
    if (!tf) { fprintf(stderr, "Cannot open token file %s\n", token_file); return 1; }
    fseek(tf, 0, SEEK_END); long sz = ftell(tf); fseek(tf, 0, SEEK_SET);
    char *buf = malloc(sz + 1);
    if (!buf) { fclose(tf); fprintf(stderr, "Out of memory\n"); return 1; }
    fread(buf, 1, sz, tf); buf[sz] = 0; fclose(tf);
    cJSON *tokdoc = cJSON_Parse(buf); free(buf);
    if (!tokdoc) { fprintf(stderr, "Invalid token JSON\n"); return 1; }
    cJSON *at = cJSON_GetObjectItemCaseSensitive(tokdoc, "access_token");
    if (!at || !cJSON_IsString(at)) { fprintf(stderr, "access_token missing in %s\n", token_file); cJSON_Delete(tokdoc); return 1; }
    const char *access_token = at->valuestring;

    /* Ensure sync dir exists */
    struct stat st;
    if (stat(sync_dir, &st) != 0) {
        if (mkdir(sync_dir, 0755) != 0) { fprintf(stderr, "Cannot create sync dir %s: %s\n", sync_dir, strerror(errno)); cJSON_Delete(tokdoc); return 1; }
    }

    CURL *curl = curl_easy_init();
    if (!curl) { fprintf(stderr, "curl init failed\n"); cJSON_Delete(tokdoc); return 1; }

    MetaList list = {0};
    if (fetch_all_metadata(curl, access_token, &list) != 0) {
        fprintf(stderr, "Failed to fetch metadata\n");
        curl_easy_cleanup(curl);
        cJSON_Delete(tokdoc);
        return 1;
    }
    printf("Fetched %zu metadata entries\n", list.count);

    /* Create folders first */
    for (size_t i = 0; i < list.count; ++i) {
        FileMeta *m = list.items[i];
        if (m->mimeType && strcmp(m->mimeType, "application/vnd.google-apps.folder") == 0) {
            char *path = build_full_path(&list, m->id);
            if (path && path[0]) {
                char full[PATH_MAX];
                snprintf(full, sizeof(full), "%s/%s", sync_dir, path);
                /* mkdir -p equivalent */
                if (ensure_dir_for_file(full) != 0) {
                    /* fallback to system call if ensure_dir_for_file fails for directory path */
                    char cmd[PATH_MAX + 32];
                    snprintf(cmd, sizeof(cmd), "mkdir -p \"%s\"", full);
                    system(cmd);
                }
                printf("Created dir: %s\n", full);
            }
            free(path);
        }
    }

    /* Queue files for download (simple heuristic: if file missing locally) */
    size_t queue_count = 0;
    FileMeta **queue = NULL;

    for (size_t i = 0; i < list.count; ++i) {
        FileMeta *m = list.items[i];
        if (!m->mimeType) continue;
        if (strcmp(m->mimeType, "application/vnd.google-apps.folder") == 0) continue;
        if (!m->name || !m->name[0]) continue;
        char *rel = build_full_path(&list, m->id);
        if (!rel || strlen(rel) == 0) {
            free(rel);
            rel = strdup_safe(m->name);
        }
        char outpath[PATH_MAX];
        snprintf(outpath, sizeof(outpath), "%s/%s", sync_dir, rel);
        free(rel);
        /* If file exists, skip (mtime comparison omitted for brevity) */
        if (access(outpath, F_OK) == 0) {
            printf("Exists, skipping: %s\n", outpath);
            continue;
        }
        if (ensure_dir_for_file(outpath) != 0) {
            fprintf(stderr, "Failed to create dir for %s\n", outpath);
            continue;
        }
        queue = realloc(queue, sizeof(FileMeta*) * (queue_count + 1));
        queue[queue_count++] = m;
    }

    printf("Queueing %zu files for download\n", queue_count);

    for (size_t i = 0; i < queue_count; ++i) {
        FileMeta *m = queue[i];
        char *rel = build_full_path(&list, m->id);
        if (!rel || strlen(rel) == 0) rel = strdup_safe(m->name);
        char outpath[PATH_MAX];
        snprintf(outpath, sizeof(outpath), "%s/%s", sync_dir, rel);
        free(rel);
        printf("Downloading %s -> %s\n", m->name, outpath);
        if (download_file(curl, access_token, m->id, outpath) == 0) {
            printf("Downloaded: %s\n", outpath);
        } else {
            fprintf(stderr, "Failed to download %s\n", m->name);
        }
    }

    /* Cleanup */
    for (size_t i = 0; i < list.count; ++i) free_filemeta(list.items[i]);
    free(list.items);
    free(queue);
    curl_easy_cleanup(curl);
    cJSON_Delete(tokdoc);
    printf("Done.\n");
    return 0;
}
