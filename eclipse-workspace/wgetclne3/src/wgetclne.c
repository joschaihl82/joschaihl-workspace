/*
 wgetclone_mt.c
 Recursive web downloader using libcurl and libxml2 with 64-thread worker pool.
 Compile: gcc -O2 -pthread -o wgetclone_mt wgetclone_mt.c -lcurl -lxml2
 Usage: ./wgetclone_mt <start-url> <output-dir> <max-depth>
*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <libgen.h>
#include <pthread.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/uri.h>
#include <libxml/xpath.h>

#define WORKER_COUNT 255

typedef struct Memory {
    char *data;
    size_t size;
} Memory;

typedef struct Task {
    char *url;
    int depth;
    struct Task *next;
} Task;

typedef struct {
    Task *head;
    Task *tail;
    pthread_mutex_t mtx;
    pthread_cond_t  cond;
    int closed; // no more tasks will be added
    size_t count;
} TaskQueue;

typedef struct VisitedItem {
    char *url;
    struct VisitedItem *next;
} VisitedItem;

/* Global configuration */
static char *start_host = NULL;
static char *output_dir = NULL;
static int max_depth = 0;

/* Shared structures */
static TaskQueue queue = { .head = NULL, .tail = NULL, .mtx = PTHREAD_MUTEX_INITIALIZER, .cond = PTHREAD_COND_INITIALIZER, .closed = 0, .count = 0 };
static VisitedItem *visited = NULL;
static pthread_mutex_t visited_mtx = PTHREAD_MUTEX_INITIALIZER;

/* Utility: enqueue task */
static void enqueue_task(const char *url, int depth) {
    Task *t = malloc(sizeof(Task));
    t->url = strdup(url);
    t->depth = depth;
    t->next = NULL;

    pthread_mutex_lock(&queue.mtx);
    if (!queue.head) queue.head = queue.tail = t;
    else { queue.tail->next = t; queue.tail = t; }
    queue.count++;
    pthread_cond_signal(&queue.cond);
    pthread_mutex_unlock(&queue.mtx);
}

/* Utility: dequeue task, returns NULL if queue empty and closed */
static Task *dequeue_task(void) {
    pthread_mutex_lock(&queue.mtx);
    while (!queue.head && !queue.closed) {
        pthread_cond_wait(&queue.cond, &queue.mtx);
    }
    if (!queue.head) {
        pthread_mutex_unlock(&queue.mtx);
        return NULL;
    }
    Task *t = queue.head;
    queue.head = t->next;
    if (!queue.head) queue.tail = NULL;
    queue.count--;
    pthread_mutex_unlock(&queue.mtx);
    return t;
}

/* Mark queue closed (no more additions) */
static void close_queue(void) {
    pthread_mutex_lock(&queue.mtx);
    queue.closed = 1;
    pthread_cond_broadcast(&queue.cond);
    pthread_mutex_unlock(&queue.mtx);
}

/* Visited set check & add (returns 1 if already visited, 0 if newly added) */
static int visited_check_and_add(const char *url) {
    pthread_mutex_lock(&visited_mtx);
    VisitedItem *it = visited;
    while (it) {
        if (strcmp(it->url, url) == 0) { pthread_mutex_unlock(&visited_mtx); return 1; }
        it = it->next;
    }
    VisitedItem *ni = malloc(sizeof(VisitedItem));
    ni->url = strdup(url);
    ni->next = visited;
    visited = ni;
    pthread_mutex_unlock(&visited_mtx);
    return 0;
}

/* Memory write callback for libcurl */
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    Memory *mem = (Memory *)userp;
    char *ptr = realloc(mem->data, mem->size + realsize + 1);
    if (!ptr) return 0;
    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = '\0';
    return realsize;
}

/* Ensure directory path exists for a given file path */
static int ensure_dir_for_file(const char *filepath) {
    char *dup = strdup(filepath);
    if (!dup) return -1;
    char *dir = dirname(dup);
    if (!dir) { free(dup); return -1; }
    char tmp[4096];
    snprintf(tmp, sizeof(tmp), "%s", dir);
    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            mkdir(tmp, 0755);
            *p = '/';
        }
    }
    mkdir(tmp, 0755);
    free(dup);
    return 0;
}

/* Convert URL to local file path under output_dir */
static char *url_to_path(const char *url) {
    xmlURIPtr uri = xmlParseURI(url);
    if (!uri) return NULL;
    const char *host = uri->server ? uri->server : "unknown";
    const char *path = uri->path ? uri->path : "/";
    size_t needed = strlen(output_dir) + 1 + strlen(host) + strlen(path) + 32;
    char *out = malloc(needed);
    snprintf(out, needed, "%s/%s%s", output_dir, host, path);
    size_t len = strlen(out);
    if (out[len-1] == '/') {
        out = realloc(out, len + strlen("index.html") + 1);
        strcat(out, "index.html");
    } else {
        const char *lastdot = strrchr(out, '.');
        const char *lastslash = strrchr(out, '/');
        if (!lastdot || (lastslash && lastdot < lastslash)) {
            out = realloc(out, len + strlen(".html") + 1);
            strcat(out, ".html");
        }
    }
    xmlFreeURI(uri);
    return out;
}

/* Resolve relative URL against base using libxml */
static char *normalize_url(const char *base, const char *href) {
    xmlChar *res = xmlBuildURI((const xmlChar *)href, (const xmlChar *)base);
    if (!res) return NULL;
    char *s = strdup((const char *)res);
    xmlFree(res);
    return s;
}

/* Get host from URL */
static char *get_host_from_url(const char *url) {
    xmlURIPtr uri = xmlParseURI(url);
    if (!uri) return NULL;
    char *host = uri->server ? strdup(uri->server) : NULL;
    xmlFreeURI(uri);
    return host;
}

/* Check same host as start_host */
static int same_host(const char *url) {
    char *h = get_host_from_url(url);
    if (!h) return 0;
    int res = (strcmp(h, start_host) == 0);
    free(h);
    return res;
}

/* Save data to file (creates dirs as needed) */
static void save_to_file(const char *path, const char *data, size_t len) {
    if (ensure_dir_for_file(path) != 0) {
        fprintf(stderr, "Failed to create directory for %s\n", path);
        return;
    }
    FILE *f = fopen(path, "wb");
    if (!f) { fprintf(stderr, "Failed to open %s: %s\n", path, strerror(errno)); return; }
    fwrite(data, 1, len, f);
    fclose(f);
}

/* Extract links (href/src) and enqueue same-host HTTP/HTTPS links */
static void extract_links_and_enqueue(const char *base_url, const char *html, int next_depth) {
    if (next_depth > max_depth) return;
    htmlDocPtr doc = htmlReadMemory(html, strlen(html), base_url, NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (!doc) return;
    xmlXPathContextPtr ctx = xmlXPathNewContext(doc);
    if (!ctx) { xmlFreeDoc(doc); return; }
    xmlXPathObjectPtr obj = xmlXPathEvalExpression((xmlChar *)"//@href | //@src", ctx);
    if (!obj) { xmlXPathFreeContext(ctx); xmlFreeDoc(doc); return; }
    xmlNodeSetPtr nodes = obj->nodesetval;
    if (nodes) {
        for (int i = 0; i < nodes->nodeNr; i++) {
            xmlNodePtr attr = nodes->nodeTab[i];
            if (!attr || !attr->children || !attr->children->content) continue;
            char *href = (char *)attr->children->content;
            if (strncasecmp(href, "mailto:", 7) == 0) continue;
            if (strncasecmp(href, "javascript:", 11) == 0) continue;
            char *resolved = normalize_url(base_url, href);
            if (!resolved) continue;
            if ((strncasecmp(resolved, "http://", 7) == 0 || strncasecmp(resolved, "https://", 8) == 0) && same_host(resolved)) {
                if (!visited_check_and_add(resolved)) enqueue_task(resolved, next_depth);
            }
            free(resolved);
        }
    }
    xmlXPathFreeObject(obj);
    xmlXPathFreeContext(ctx);
    xmlFreeDoc(doc);
}

/* Download URL to memory using provided CURL handle; returns 1 on success */
static int download_url_to_memory(CURL *curl, const char *url, Memory *mem, long *http_code, char **content_type) {
    mem->data = malloc(1);
    mem->size = 0;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)mem);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "wgetclone_mt/0.1");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl failed for %s: %s\n", url, curl_easy_strerror(res));
        return 0;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, http_code);
    char *ctype = NULL;
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ctype);
    if (ctype) *content_type = strdup(ctype);
    else *content_type = NULL;
    return 1;
}

/* Worker thread function */
static void *worker_main(void *arg) {
    (void)arg;
    CURL *curl = curl_easy_init();
    if (!curl) return NULL;

    while (1) {
        Task *t = dequeue_task();
        if (!t) break; // queue closed and empty
        printf("Thread %lu visiting %s (depth %d)\n", (unsigned long)pthread_self(), t->url, t->depth);
        Memory mem = {0};
        long http_code = 0;
        char *ctype = NULL;
        int ok = download_url_to_memory(curl, t->url, &mem, &http_code, &ctype);
        if (!ok) {
            free(t->url);
            free(t);
            if (mem.data) free(mem.data);
            if (ctype) free(ctype);
            continue;
        }
        if (http_code >= 400) {
            fprintf(stderr, "HTTP %ld for %s\n", http_code, t->url);
            free(t->url);
            free(t);
            if (mem.data) free(mem.data);
            continue;
        }
        char *path = url_to_path(t->url);
        if (path) {
            save_to_file(path, mem.data, mem.size);
            free(path);
        }
        if (ctype && strstr(ctype, "text/html")) {
            extract_links_and_enqueue(t->url, mem.data, t->depth + 1);
        }
        free(ctype);
        free(mem.data);
        free(t->url);
        free(t);
    }

    curl_easy_cleanup(curl);
    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <start-url> <output-dir> <max-depth>\n", argv[0]);
        return 1;
    }
    const char *start_url = argv[1];
    const char *outdir = argv[2];
    max_depth = atoi(argv[3]);
    if (max_depth < 0) max_depth = 0;

    /* create output dir if needed and canonicalize path */
    if (mkdir(outdir, 0755) != 0 && errno != EEXIST) {
        perror("mkdir output");
        return 1;
    }
    char *real_out = realpath(outdir, NULL);
    if (!real_out) { perror("realpath"); return 1; }
    output_dir = real_out;

    /* init libraries */
    curl_global_init(CURL_GLOBAL_DEFAULT);
    xmlInitParser();

    start_host = get_host_from_url(start_url);
    if (!start_host) { fprintf(stderr, "Invalid start URL\n"); return 1; }

    /* initialize visited and queue with start URL */
    visited_check_and_add(start_url);
    enqueue_task(start_url, 0);

    /* create worker threads */
    pthread_t workers[WORKER_COUNT];
    for (int i = 0; i < WORKER_COUNT; ++i) {
        if (pthread_create(&workers[i], NULL, worker_main, NULL) != 0) {
            fprintf(stderr, "Failed to create worker %d\n", i);
            // continue but fewer workers will run
        }
    }

    /* Monitor: wait until queue empty and no tasks in-flight, then close queue */
    while (1) {
        pthread_mutex_lock(&queue.mtx);
        int empty = (queue.count == 0 && queue.head == NULL);
        pthread_mutex_unlock(&queue.mtx);
        if (empty) break;
        usleep(100000); // 100ms pause while waiting for workers to produce/consume tasks
    }

    close_queue();

    /* join workers */
    for (int i = 0; i < WORKER_COUNT; ++i) pthread_join(workers[i], NULL);

    /* cleanup visited list */
    pthread_mutex_lock(&visited_mtx);
    while (visited) {
        VisitedItem *t = visited;
        visited = visited->next;
        free(t->url);
        free(t);
    }
    pthread_mutex_unlock(&visited_mtx);

    free(start_host);
    free(output_dir);

    xmlCleanupParser();
    curl_global_cleanup();
    return 0;
}
