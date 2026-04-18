/*
 wgetclone.c
 Minimal recursive web downloader using libcurl and libxml2
 Compile: gcc -o wgetclone wgetclone.c -lcurl -lxml2
 Usage: ./wgetclone <start-url> <output-dir> <max-depth>
*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>
#include <libgen.h>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/uri.h>
#include <libxml/HTMLtree.h>
#include <libxml/xpath.h>

typedef struct Memory {
    char *data;
    size_t size;
} Memory;

typedef struct Node {
    char *url;
    int depth;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
} Queue;

typedef struct VisitedItem {
    char *url;
    struct VisitedItem *next;
} VisitedItem;

static VisitedItem *visited = NULL;
static char *start_host = NULL;
static char *output_dir = NULL;
static int max_depth = 0;

static void enqueue(Queue *q, const char *url, int depth) {
    Node *n = malloc(sizeof(Node));
    n->url = strdup(url);
    n->depth = depth;
    n->next = NULL;
    if (!q->head) q->head = q->tail = n;
    else { q->tail->next = n; q->tail = n; }
}

static Node *dequeue(Queue *q) {
    if (!q->head) return NULL;
    Node *n = q->head;
    q->head = n->next;
    if (!q->head) q->tail = NULL;
    return n;
}

static int visited_check_and_add(const char *url) {
    VisitedItem *it = visited;
    while (it) {
        if (strcmp(it->url, url) == 0) return 1;
        it = it->next;
    }
    VisitedItem *ni = malloc(sizeof(VisitedItem));
    ni->url = strdup(url);
    ni->next = visited;
    visited = ni;
    return 0;
}

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    Memory *mem = (Memory *)userp;
    char *ptr = realloc(mem->data, mem->size + realsize + 1);
    if (!ptr) return 0;
    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;
    return realsize;
}

static int ensure_dir_for_file(const char *filepath) {
    char *dup = strdup(filepath);
    char *dir = dirname(dup);
    if (!dir) { free(dup); return -1; }
    // create directories recursively
    char tmp[4096];
    snprintf(tmp, sizeof(tmp), "%s", dir);
    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, 0755);
            *p = '/';
        }
    }
    mkdir(tmp, 0755);
    free(dup);
    return 0;
}

static char *url_to_path(const char *url) {
    // Convert URL to file path under output_dir.
    // e.g., https://example.com/path/page -> output_dir/example.com/path/page.html
    xmlURIPtr uri = xmlParseURI(url);
    if (!uri) return NULL;
    const char *host = uri->server ? uri->server : "unknown";
    const char *path = uri->path ? uri->path : "/";
    size_t needed = strlen(output_dir) + 1 + strlen(host) + strlen(path) + 16;
    char *out = malloc(needed);
    snprintf(out, needed, "%s/%s%s", output_dir, host, path);
    // if path ends with '/', save as index.html
    if (out[strlen(out)-1] == '/') {
        size_t nl = strlen(out) + strlen("index.html") + 1;
        out = realloc(out, nl);
        strcat(out, "index.html");
    } else {
        // if no extension, add .html
        const char *lastdot = strrchr(out, '.');
        const char *lastslash = strrchr(out, '/');
        if (!lastdot || (lastslash && lastdot < lastslash)) {
            size_t nl = strlen(out) + strlen(".html") + 1;
            out = realloc(out, nl);
            strcat(out, ".html");
        }
    }
    xmlFreeURI(uri);
    return out;
}

static char *normalize_url(const char *base, const char *href) {
    // Use libxml to resolve relative URLs
    xmlChar *res = xmlBuildURI((const xmlChar *)href, (const xmlChar *)base);
    if (!res) return NULL;
    char *s = strdup((const char *)res);
    xmlFree(res);
    return s;
}

static char *get_host_from_url(const char *url) {
    xmlURIPtr uri = xmlParseURI(url);
    if (!uri) return NULL;
    char *host = uri->server ? strdup(uri->server) : NULL;
    xmlFreeURI(uri);
    return host;
}

static int same_host(const char *url) {
    char *h = get_host_from_url(url);
    if (!h) return 0;
    int res = (strcmp(h, start_host) == 0);
    free(h);
    return res;
}

static void save_to_file(const char *path, const char *data, size_t len) {
    ensure_dir_for_file(path);
    FILE *f = fopen(path, "wb");
    if (!f) {
        fprintf(stderr, "Failed to open %s: %s\n", path, strerror(errno));
        return;
    }
    fwrite(data, 1, len, f);
    fclose(f);
}

static void extract_links_and_enqueue(const char *base_url, const char *html, Queue *q, int next_depth) {
    if (next_depth > max_depth) return;
    htmlDocPtr doc = htmlReadMemory(html, strlen(html), base_url, NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (!doc) return;
    xmlXPathContextPtr ctx = xmlXPathNewContext(doc);
    if (!ctx) { xmlFreeDoc(doc); return; }
    // Select href and src attributes
    xmlXPathObjectPtr obj = xmlXPathEvalExpression((xmlChar *)"//@href | //@src", ctx);
    if (!obj) { xmlXPathFreeContext(ctx); xmlFreeDoc(doc); return; }
    xmlNodeSetPtr nodes = obj->nodesetval;
    if (!nodes) { xmlXPathFreeObject(obj); xmlXPathFreeContext(ctx); xmlFreeDoc(doc); return; }
    for (int i = 0; i < nodes->nodeNr; i++) {
        xmlNodePtr attr = nodes->nodeTab[i];
        if (!attr || !attr->children || !attr->children->content) continue;
        char *href = (char *)attr->children->content;
        if (strncasecmp(href, "mailto:", 7) == 0) continue;
        if (strncasecmp(href, "javascript:", 11) == 0) continue;
        char *resolved = normalize_url(base_url, href);
        if (!resolved) continue;
        // Only enqueue HTTP/HTTPS URLs and same host
        if ((strncasecmp(resolved, "http://", 7) == 0 || strncasecmp(resolved, "https://", 8) == 0) && same_host(resolved)) {
            if (!visited_check_and_add(resolved)) {
                enqueue(q, resolved, next_depth);
            }
        }
        free(resolved);
    }
    xmlXPathFreeObject(obj);
    xmlXPathFreeContext(ctx);
    xmlFreeDoc(doc);
}

static int download_url_to_memory(const char *url, Memory *mem, long *http_code, char **content_type) {
    CURL *curl = curl_easy_init();
    if (!curl) return 0;
    mem->data = malloc(1);
    mem->size = 0;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)mem);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "wgetclone/0.1");
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl failed for %s: %s\n", url, curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return 0;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, http_code);
    char *ctype = NULL;
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ctype);
    if (ctype) *content_type = strdup(ctype);
    curl_easy_cleanup(curl);
    return 1;
}

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <start-url> <output-dir> <max-depth>\n", argv[0]);
        return 1;
    }
    const char *start_url = argv[1];
    output_dir = realpath(argv[2], NULL);
    if (!output_dir) {
        // try to create
        if (mkdir(argv[2], 0755) != 0 && errno != EEXIST) {
            perror("mkdir output");
            return 1;
        }
        output_dir = realpath(argv[2], NULL);
        if (!output_dir) {
            perror("realpath");
            return 1;
        }
    }
    max_depth = atoi(argv[3]);
    if (max_depth < 0) max_depth = 0;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    xmlInitParser();

    start_host = get_host_from_url(start_url);
    if (!start_host) {
        fprintf(stderr, "Invalid start URL\n");
        return 1;
    }

    Queue q = {0};
    enqueue(&q, start_url, 0);
    visited_check_and_add(start_url);

    Node *n;
    while ((n = dequeue(&q)) != NULL) {
        printf("Visiting %s (depth %d)\n", n->url, n->depth);
        Memory mem = {0};
        long http_code = 0;
        char *ctype = NULL;
        if (!download_url_to_memory(n->url, &mem, &http_code, &ctype)) {
            free(n->url);
            free(n);
            free(ctype);
            if (mem.data) free(mem.data);
            continue;
        }
        if (http_code >= 400) {
            fprintf(stderr, "HTTP %ld for %s\n", http_code, n->url);
            free(n->url);
            free(n);
            free(ctype);
            if (mem.data) free(mem.data);
            continue;
        }
        char *path = url_to_path(n->url);
        if (path) {
            save_to_file(path, mem.data, mem.size);
            free(path);
        }
        // Only parse HTML
        if (ctype && strstr(ctype, "text/html")) {
            extract_links_and_enqueue(n->url, mem.data, &q, n->depth + 1);
        }
        free(ctype);
        free(mem.data);

        free(n->url);
        free(n);
    }

    // cleanup visited
    while (visited) {
        VisitedItem *t = visited;
        visited = visited->next;
        free(t->url);
        free(t);
    }
    free(start_host);
    free(output_dir);

    xmlCleanupParser();
    curl_global_cleanup();
    return 0;
}
