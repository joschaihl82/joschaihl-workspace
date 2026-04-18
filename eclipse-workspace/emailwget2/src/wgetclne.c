/* tiny_email_wget_mt_logged_console.c
   Multithreaded email-only crawler (no site files saved).
   Logs visits to stdout instead of a file.

   Edit macros below for configuration.
   Build: gcc -O2 -pthread -o tiny_email_wget_mt_logged_console tiny_email_wget_mt_logged_console.c -lcurl -lxml2
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/uri.h>
#include <libxml/xpath.h>
#include <regex.h>
#include <sys/stat.h>
#include <time.h>

#define START_URL "https://usa.gov/"   /* change to your start URL */
#define VISITED_FILE "visited.txt"
#define EMAILS_FILE "emails.txt"
#define MAX_DEPTH 64      /* 0 = only start page */
#define WORKER_COUNT 128  /* number of worker threads */

/* --- simple dynamic buffer for curl --- */
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
    pthread_cond_t cond;
    int closed;
    size_t count;
} TaskQueue;

typedef struct VisitedItem {
    char *url;
    struct VisitedItem *next;
} VisitedItem;

static TaskQueue queue = { .head = NULL, .tail = NULL, .mtx = PTHREAD_MUTEX_INITIALIZER, .cond = PTHREAD_COND_INITIALIZER, .closed = 0, .count = 0 };
static VisitedItem *visited = NULL;
static pthread_mutex_t visited_mtx = PTHREAD_MUTEX_INITIALIZER;

/* in-memory set of seen emails for this run */
typedef struct EmailItem {
    char *email;
    struct EmailItem *next;
} EmailItem;
static EmailItem *emails_head = NULL;
static pthread_mutex_t emails_mtx = PTHREAD_MUTEX_INITIALIZER;

/* mutex for printing logs to console to avoid interleaving */
static pthread_mutex_t log_print_mtx = PTHREAD_MUTEX_INITIALIZER;

/* enqueue task */
static void enqueue_task(const char *url, int depth) {
    Task *t = malloc(sizeof(Task));
    if (!t) return;
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

/* dequeue task */
static Task *dequeue_task(void) {
    pthread_mutex_lock(&queue.mtx);
    while (!queue.head && !queue.closed) pthread_cond_wait(&queue.cond, &queue.mtx);
    if (!queue.head) { pthread_mutex_unlock(&queue.mtx); return NULL; }
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

/* visited set: returns 1 if already visited, 0 if newly added */
static int visited_check_and_add(const char *url) {
    pthread_mutex_lock(&visited_mtx);
    VisitedItem *it = visited;
    while (it) { if (strcmp(it->url, url) == 0) { pthread_mutex_unlock(&visited_mtx); return 1; } it = it->next; }
    VisitedItem *ni = malloc(sizeof(VisitedItem));
    ni->url = strdup(url);
    ni->next = visited;
    visited = ni;
    pthread_mutex_unlock(&visited_mtx);
    /* append to visited file immediately */
    FILE *vf = fopen(VISITED_FILE, "a");
    if (vf) { fprintf(vf, "%s\n", url); fclose(vf); }
    return 0;
}

/* email set: returns 1 if already present, 0 if newly added (and writes to file) */
static int email_check_and_add(const char *email) {
    pthread_mutex_lock(&emails_mtx);
    EmailItem *it = emails_head;
    while (it) { if (strcmp(it->email, email) == 0) { pthread_mutex_unlock(&emails_mtx); return 1; } it = it->next; }
    EmailItem *ni = malloc(sizeof(EmailItem));
    ni->email = strdup(email);
    ni->next = emails_head;
    emails_head = ni;
    /* append to emails file */
    FILE *ef = fopen(EMAILS_FILE, "a");
    if (ef) { fprintf(ef, "%s\n", email); fclose(ef); }
    pthread_mutex_unlock(&emails_mtx);
    return 0;
}

/* thread-safe logging to stdout: timestamp, thread id, host, url */
static void log_search_visit_console(const char *url) {
    char timestr[32];
    time_t now = time(NULL);
    struct tm tm;
    gmtime_r(&now, &tm);
    strftime(timestr, sizeof(timestr), "%Y-%m-%dT%H:%M:%SZ", &tm);

    char *host = NULL;
    xmlURIPtr uri = xmlParseURI(url);
    if (uri && uri->server) host = strdup(uri->server);
    if (uri) xmlFreeURI(uri);

    unsigned long tid = (unsigned long)pthread_self();

    pthread_mutex_lock(&log_print_mtx);
    printf("%s,%lu,%s,%s\n", timestr, tid, host ? host : "-", url);
    fflush(stdout);
    pthread_mutex_unlock(&log_print_mtx);

    if (host) free(host);
}

/* curl write callback */
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    Memory *mem = (Memory *)userp;
    char *ptr = realloc(mem->data, mem->size + realsize + 1);
    if (!ptr) return 0;
    mem->data = ptr;
    memcpy(mem->data + mem->size, contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = '\0';
    return realsize;
}

/* Resolve href relative to base using libxml */
static char *normalize_url(const char *base, const char *href) {
    xmlChar *res = xmlBuildURI((const xmlChar *)href, (const xmlChar *)base);
    if (!res) return NULL;
    char *s = strdup((const char *)res);
    xmlFree(res);
    return s;
}

/* get host from URL */
static char *get_host_from_url(const char *url) {
    xmlURIPtr uri = xmlParseURI(url);
    if (!uri) return NULL;
    char *host = uri->server ? strdup(uri->server) : NULL;
    xmlFreeURI(uri);
    return host;
}

/* global start host for same-host restriction */
static char *start_host = NULL;

/* check same host as start */
static int same_host(const char *url) {
    char *h = get_host_from_url(url);
    if (!h) return 0;
    int res = (strcmp(h, start_host) == 0);
    free(h);
    return res;
}

/* extract emails from text using POSIX regex and add them */
static void extract_emails_from_text(const char *text) {
    if (!text) return;
    regex_t re;
    const char *pattern = "([A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,})";
    if (regcomp(&re, pattern, REG_EXTENDED|REG_ICASE) != 0) return;
    const char *p = text;
    regmatch_t m;
    while (regexec(&re, p, 1, &m, 0) == 0) {
        if (m.rm_so >= 0 && m.rm_eo > m.rm_so) {
            size_t len = m.rm_eo - m.rm_so;
            char *found = strndup(p + m.rm_so, len);
            for (char *c = found; *c; ++c) if (*c >= 'A' && *c <= 'Z') *c = *c - 'A' + 'a';
            email_check_and_add(found);
            free(found);
        }
        p += m.rm_eo;
    }
    regfree(&re);
}

/* extract links from HTML and enqueue same-host http/https links */
static void extract_links_and_enqueue(const char *base_url, const char *html, int next_depth) {
    if (next_depth > MAX_DEPTH) return;
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
            if (!href) continue;
            if (strncasecmp(href, "mailto:", 7) == 0) {
                const char *addr = href + 7;
                if (*addr) {
                    char *copy = strdup(addr);
                    char *q = strchr(copy, '?'); if (q) *q = '\0';
                    char *f = strchr(copy, '#'); if (f) *f = '\0';
                    for (char *c = copy; *c; ++c) if (*c >= 'A' && *c <= 'Z') *c = *c - 'A' + 'a';
                    email_check_and_add(copy);
                    free(copy);
                }
                continue;
            }
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

/* Download URL to memory; return 1 on success */
static int download_url_to_memory(CURL *curl, const char *url, Memory *mem, long *http_code, char **content_type) {
    mem->data = malloc(1);
    mem->size = 0;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)mem);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "tiny_email_wget_mt/0.1");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        free(mem->data); mem->data = NULL; mem->size = 0;
        return 0;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, http_code);
    char *ctype = NULL;
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ctype);
    if (ctype) *content_type = strdup(ctype); else *content_type = NULL;
    return 1;
}

/* Worker thread */
static void *worker_main(void *arg) {
    (void)arg;
    CURL *curl = curl_easy_init();
    if (!curl) return NULL;
    while (1) {
        Task *t = dequeue_task();
        if (!t) break;
        /* Log visit to console before downloading */
        log_search_visit_console(t->url);

        Memory mem = {0};
        long http_code = 0;
        char *ctype = NULL;
        int ok = download_url_to_memory(curl, t->url, &mem, &http_code, &ctype);
        if (!ok) {
            free(t->url);
            free(t);
            continue;
        }
        if (http_code >= 400) {
            free(mem.data);
            free(ctype);
            free(t->url);
            free(t);
            continue;
        }
        /* append found emails from raw text */
        extract_emails_from_text(mem.data);
        /* if HTML content, parse and follow links */
        if (ctype && strstr(ctype, "text/html")) {
            extract_links_and_enqueue(t->url, mem.data, t->depth + 1);
        }
        free(mem.data);
        free(ctype);
        free(t->url);
        free(t);
        /* polite tiny delay to avoid hammering */
        struct timespec ts = {0, 100 * 1000000}; /* 100ms */
        nanosleep(&ts, NULL);
    }
    curl_easy_cleanup(curl);
    return NULL;
}

/* initialize and run */
int main(void) {
    /* create files if not exist */
    FILE *vf = fopen(VISITED_FILE, "a");
    if (vf) fclose(vf);
    FILE *ef = fopen(EMAILS_FILE, "a");
    if (ef) fclose(ef);

    /* init libs */
    curl_global_init(CURL_GLOBAL_DEFAULT);
    xmlInitParser();

    start_host = get_host_from_url(START_URL);
    if (!start_host) { fprintf(stderr, "Invalid START_URL\n"); return 1; }

    /* mark start visited and enqueue */
    visited_check_and_add(START_URL);
    enqueue_task(START_URL, 0);

    /* create worker threads */
    pthread_t workers[WORKER_COUNT];
    for (int i = 0; i < WORKER_COUNT; ++i) {
        if (pthread_create(&workers[i], NULL, worker_main, NULL) != 0) {
            /* continue with fewer workers */
        }
    }

    /* monitor until queue empty then close */
    while (1) {
        pthread_mutex_lock(&queue.mtx);
        int empty = (queue.count == 0 && queue.head == NULL);
        pthread_mutex_unlock(&queue.mtx);
        if (empty) break;
        usleep(100000);
    }
    close_queue();

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

    /* cleanup emails list */
    pthread_mutex_lock(&emails_mtx);
    while (emails_head) {
        EmailItem *t = emails_head;
        emails_head = emails_head->next;
        free(t->email);
        free(t);
    }
    pthread_mutex_unlock(&emails_mtx);

    free(start_host);
    xmlCleanupParser();
    curl_global_cleanup();
    return 0;
}
