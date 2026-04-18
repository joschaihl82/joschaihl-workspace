/*
 wgetclone_links.c
 64-thread randomized same-host crawler that does NOT save page contents.
 It records crawled URLs to crawled.txt and all discovered links to links.txt.
 Compile: gcc -O2 -pthread -o wgetclone_links wgetclone_links.c -lcurl -lxml2
 Usage: ./wgetclone_links <start-url> <output-dir> <max-depth>
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
#include <time.h>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/uri.h>
#include <libxml/xpath.h>

#define WORKER_COUNT 64

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
    int closed;
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

/* File handles and their mutexes */
static FILE *links_fp = NULL;   /* append discovered links */
static FILE *crawled_fp = NULL; /* append crawled URLs */
static pthread_mutex_t file_mtx = PTHREAD_MUTEX_INITIALIZER;

/* Per-thread RNG seed */
static __thread unsigned int thread_seed = 0;

/* Task queue operations */
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

static void close_queue(void) {
    pthread_mutex_lock(&queue.mtx);
    queue.closed = 1;
    pthread_cond_broadcast(&queue.cond);
    pthread_mutex_unlock(&queue.mtx);
}

/* Visited set: returns 1 if already visited, 0 if newly added */
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

/* libcurl write callback (store in memory only so we can parse) */
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

/* Append a discovered link to links.txt (thread-safe) */
static void record_link(const char *link) {
    pthread_mutex_lock(&file_mtx);
    if (links_fp) {
        fprintf(links_fp, "%s\n", link);
        fflush(links_fp);
    }
    pthread_mutex_unlock(&file_mtx);
}

/* Append a crawled URL to crawled.txt (thread-safe) */
static void record_crawled(const char *url) {
    pthread_mutex_lock(&file_mtx);
    if (crawled_fp) {
        fprintf(crawled_fp, "%s\n", url);
        fflush(crawled_fp);
    }
    pthread_mutex_unlock(&file_mtx);
}

/* Fisher-Yates shuffle using thread-local RNG */
static void shuffle_array(char **arr, int n) {
    if (n <= 1) return;
    if (thread_seed == 0) {
        thread_seed = (unsigned int)time(NULL) ^ (unsigned int)(uintptr_t)pthread_self();
        if (thread_seed == 0) thread_seed = 1;
    }
    for (int i = n - 1; i > 0; --i) {
        unsigned int r = rand_r(&thread_seed);
        int j = r % (i + 1);
        char *tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

/* Extract same-host links and append all discovered links to links.txt.
   Select random k in [1, cnt], shuffle, enqueue first k (if not visited). */
static void extract_links_and_enqueue(const char *base_url, const char *html, int next_depth) {
    if (next_depth > max_depth) return;
    htmlDocPtr doc = htmlReadMemory(html, strlen(html), base_url, NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (!doc) return;
    xmlXPathContextPtr ctx = xmlXPathNewContext(doc);
    if (!ctx) { xmlFreeDoc(doc); return; }
    xmlXPathObjectPtr obj = xmlXPathEvalExpression((xmlChar *)"//@href | //@src", ctx);
    if (!obj) { xmlXPathFreeContext(ctx); xmlFreeDoc(doc); return; }
    xmlNodeSetPtr nodes = obj->nodesetval;
    if (!nodes) { xmlXPathFreeObject(obj); xmlXPathFreeContext(ctx); xmlFreeDoc(doc); return; }

    int cap = nodes->nodeNr;
    char **cands = malloc(sizeof(char*) * cap);
    int cnt = 0;
    for (int i = 0; i < nodes->nodeNr; i++) {
        xmlNodePtr attr = nodes->nodeTab[i];
        if (!attr || !attr->children || !attr->children->content) continue;
        char *href = (char *)attr->children->content;
        if (strncasecmp(href, "mailto:", 7) == 0) continue;
        if (strncasecmp(href, "javascript:", 11) == 0) continue;
        char *resolved = normalize_url(base_url, href);
        if (!resolved) continue;
        if ((strncasecmp(resolved, "http://", 7) == 0 || strncasecmp(resolved, "https://", 8) == 0) && same_host(resolved)) {
            /* record discovered link regardless of visited state */
            record_link(resolved);
            cands[cnt++] = resolved; /* ownership passed */
        } else {
            free(resolved);
        }
    }

    if (cnt > 0) {
        if (thread_seed == 0) {
            thread_seed = (unsigned int)time(NULL) ^ (unsigned int)(uintptr_t)pthread_self();
            if (thread_seed == 0) thread_seed = 1;
        }
        int k = (rand_r(&thread_seed) % cnt) + 1; /* 1..cnt */
        shuffle_array(cands, cnt);
        for (int i = 0; i < k; ++i) {
            char *resolved = cands[i];
            if (!visited_check_and_add(resolved)) {
                enqueue_task(resolved, next_depth);
            }
            free(resolved);
        }
        for (int i = k; i < cnt; ++i) free(cands[i]);
    }
    free(cands);

    xmlXPathFreeObject(obj);
    xmlXPathFreeContext(ctx);
    xmlFreeDoc(doc);
}

/* Download URL into memory using provided CURL handle; returns 1 on success */
static int download_url_to_memory(CURL *curl, const char *url, Memory *mem, long *http_code, char **content_type) {
    mem->data = malloc(1);
    mem->size = 0;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)mem);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "wgetclone_links/0.1");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        /* network/transfer failure */
        return 0;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, http_code);
    char *ctype = NULL;
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ctype);
    if (ctype) *content_type = strdup(ctype);
    else *content_type = NULL;
    return 1;
}

/* Worker thread */
static void *worker_main(void *arg) {
    (void)arg;
    if (thread_seed == 0) {
        thread_seed = (unsigned int)time(NULL) ^ (unsigned int)(uintptr_t)pthread_self();
        if (thread_seed == 0) thread_seed = 1;
    }

    CURL *curl = curl_easy_init();
    if (!curl) return NULL;

    while (1) {
        Task *t = dequeue_task();
        if (!t) break; /* queue closed and empty */
        /* Download page into memory only, do not write page to disk */
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
            free(t->url);
            free(t);
            if (mem.data) free(mem.data);
            if (ctype) free(ctype);
            continue;
        }
        /* record that we crawled this URL (only once because visited was enforced before enqueueing for start)
           but in case other threads enqueued the same URL before visited was added, guard again */
        record_crawled(t->url);

        /* parse HTML and extract same-host links, selecting random subset to enqueue */
        if (ctype && strstr(ctype, "text/html")) {
            extract_links_and_enqueue(t->url, mem.data, t->depth + 1);
        } else {
            /* even for non-HTML we attempt link extraction on the raw body (some pages omit content-type) */
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

    if (mkdir(outdir, 0755) != 0 && errno != EEXIST) {
        perror("mkdir output");
        return 1;
    }
    char *real_out = realpath(outdir, NULL);
    if (!real_out) { perror("realpath"); return 1; }
    output_dir = real_out;

    /* open links.txt and crawled.txt for append */
    char path_links[4096];
    char path_crawled[4096];
    snprintf(path_links, sizeof(path_links), "%s/links.txt", output_dir);
    snprintf(path_crawled, sizeof(path_crawled), "%s/crawled.txt", output_dir);
    links_fp = fopen(path_links, "a");
    if (!links_fp) { fprintf(stderr, "Failed to open %s: %s\n", path_links, strerror(errno)); /* continue without file */ }
    crawled_fp = fopen(path_crawled, "a");
    if (!crawled_fp) { fprintf(stderr, "Failed to open %s: %s\n", path_crawled, strerror(errno)); /* continue without file */ }

    curl_global_init(CURL_GLOBAL_DEFAULT);
    xmlInitParser();

    start_host = get_host_from_url(start_url);
    if (!start_host) { fprintf(stderr, "Invalid start URL\n"); return 1; }

    srand((unsigned int)time(NULL) ^ (unsigned int)getpid());

    /* mark start URL visited and enqueue */
    visited_check_and_add(start_url);
    enqueue_task(start_url, 0);

    /* create workers */
    pthread_t workers[WORKER_COUNT];
    for (int i = 0; i < WORKER_COUNT; ++i) {
        if (pthread_create(&workers[i], NULL, worker_main, NULL) != 0) {
            fprintf(stderr, "Failed to create worker %d\n", i);
        }
    }

    /* wait until queue drained */
    while (1) {
        pthread_mutex_lock(&queue.mtx);
        int empty = (queue.count == 0 && queue.head == NULL);
        pthread_mutex_unlock(&queue.mtx);
        if (empty) break;
        usleep(100000);
    }

    close_queue();

    for (int i = 0; i < WORKER_COUNT; ++i) pthread_join(workers[i], NULL);

    /* cleanup visited */
    pthread_mutex_lock(&visited_mtx);
    while (visited) {
        VisitedItem *t = visited;
        visited = visited->next;
        free(t->url);
        free(t);
    }
    pthread_mutex_unlock(&visited_mtx);

    if (links_fp) fclose(links_fp);
    if (crawled_fp) fclose(crawled_fp);

    free(start_host);
    free(output_dir);

    xmlCleanupParser();
    curl_global_cleanup();
    return 0;
}
