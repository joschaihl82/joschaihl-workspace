/* tiny_email_wget_fbi.c
   Multithreaded email-only crawler (no site files saved).
   Amalgamates host normalization, detailed logging, retries, and monitor output.

   Build: gcc -O2 -pthread -o tiny_email_wget_fbi tiny_email_wget_fbi.c -lcurl -lxml2
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
#include <time.h>

#define START_URL "https://www.fbi.gov/"
#define VISITED_FILE "visited.txt"
#define EMAILS_FILE "emails.txt"
#define MAX_DEPTH 8
#define WORKER_COUNT 12
#define CURL_RETRY 1           /* number of retries on transient curl error */
#define MONITOR_INTERVAL_US 200000  /* monitor sleep (200ms) */

typedef struct { char *data; size_t size; } Memory;
typedef struct Task { char *url; int depth; struct Task *next; } Task;
typedef struct {
    Task *head;
    Task *tail;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    int closed;
    size_t count;
} TaskQueue;
typedef struct VisitedItem { char *url; struct VisitedItem *next; } VisitedItem;
typedef struct EmailItem { char *email; struct EmailItem *next; } EmailItem;

static TaskQueue queue = { .head = NULL, .tail = NULL, .mtx = PTHREAD_MUTEX_INITIALIZER, .cond = PTHREAD_COND_INITIALIZER, .closed = 0, .count = 0 };
static VisitedItem *visited = NULL;
static pthread_mutex_t visited_mtx = PTHREAD_MUTEX_INITIALIZER;
static EmailItem *emails_head = NULL;
static pthread_mutex_t emails_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t log_print_mtx = PTHREAD_MUTEX_INITIALIZER;
static char *start_host = NULL;

/* utility: lowercase copy */
static char *str_tolower_dup(const char *s) {
    if (!s) return NULL;
    char *r = strdup(s);
    for (char *p = r; *p; ++p) if (*p >= 'A' && *p <= 'Z') *p = *p - 'A' + 'a';
    return r;
}

/* normalize host: strip leading www. and lowercase */
static char *normalize_host(const char *host) {
    if (!host) return NULL;
    const char *h = host;
    if (strncmp(h, "www.", 4) == 0) h += 4;
    return str_tolower_dup(h);
}

/* get host from URL */
static char *get_host_from_url(const char *url) {
    xmlURIPtr uri = xmlParseURI(url);
    if (!uri) return NULL;
    char *host = uri->server ? strdup(uri->server) : NULL;
    xmlFreeURI(uri);
    return host;
}

/* queue ops */
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

/* dequeue */
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

/* visited set with immediate append to file */
static int visited_check_and_add(const char *url) {
    pthread_mutex_lock(&visited_mtx);
    VisitedItem *it = visited;
    while (it) { if (strcmp(it->url, url) == 0) { pthread_mutex_unlock(&visited_mtx); return 1; } it = it->next; }
    VisitedItem *ni = malloc(sizeof(VisitedItem));
    ni->url = strdup(url);
    ni->next = visited;
    visited = ni;
    pthread_mutex_unlock(&visited_mtx);
    FILE *vf = fopen(VISITED_FILE, "a");
    if (vf) { fprintf(vf, "%s\n", url); fclose(vf); }
    return 0;
}

/* emails set with immediate append to file */
static int email_check_and_add(const char *email) {
    pthread_mutex_lock(&emails_mtx);
    EmailItem *it = emails_head;
    while (it) { if (strcmp(it->email, email) == 0) { pthread_mutex_unlock(&emails_mtx); return 1; } it = it->next; }
    EmailItem *ni = malloc(sizeof(EmailItem));
    ni->email = strdup(email);
    ni->next = emails_head;
    emails_head = ni;
    FILE *ef = fopen(EMAILS_FILE, "a");
    if (ef) { fprintf(ef, "%s\n", email); fclose(ef); }
    pthread_mutex_unlock(&emails_mtx);
    return 0;
}

/* console logging helpers */
static void log_printf_locked(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    pthread_mutex_lock(&log_print_mtx);
    vprintf(fmt, ap);
    fflush(stdout);
    pthread_mutex_unlock(&log_print_mtx);
    va_end(ap);
}

static void log_visit_console(const char *url) {
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
    log_printf_locked("%s,%lu,%s,%s\n", timestr, tid, host ? host : "-", url);
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

/* resolve href with libxml */
static char *normalize_url(const char *base, const char *href) {
    xmlChar *res = xmlBuildURI((const xmlChar *)href, (const xmlChar *)base);
    if (!res) return NULL;
    char *s = strdup((const char *)res);
    xmlFree(res);
    return s;
}

/* same-host check using normalized hosts */
static int same_host_norm(const char *url) {
    char *raw = get_host_from_url(url);
    if (!raw) return 0;
    char *norm = normalize_host(raw);
    free(raw);
    if (!norm) return 0;
    int r = (strcmp(norm, start_host) == 0);
    free(norm);
    return r;
}

/* extract emails via regex from arbitrary text */
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
            if (!email_check_and_add(found)) {
                log_printf_locked("FOUND_EMAIL,%s\n", found);
            }
            free(found);
        }
        p += m.rm_eo;
    }
    regfree(&re);
}

/* parse HTML, capture mailto:, and decide to enqueue or skip links with logging */
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
        for (int i = 0; i < nodes->nodeNr; ++i) {
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
                    char *lc = str_tolower_dup(copy);
                    free(copy);
                    if (!email_check_and_add(lc)) log_printf_locked("FOUND_MAILTO,%s\n", lc);
                    free(lc);
                }
                continue;
            }
            if (strncasecmp(href, "javascript:", 11) == 0) continue;
            char *resolved = normalize_url(base_url, href);
            if (!resolved) continue;

            /* Log decision info */
            char *link_raw_host = get_host_from_url(resolved);
            char *link_norm_host = link_raw_host ? normalize_host(link_raw_host) : NULL;
            if (link_raw_host) free(link_raw_host);

            /* determine HTTP(s) scheme presence */
            int is_http = (strncasecmp(resolved, "http://", 7) == 0 || strncasecmp(resolved, "https://", 8) == 0);

            if (!is_http) {
                log_printf_locked("SKIP_NOT_HTTP,%s\n", resolved);
            } else if (!link_norm_host) {
                log_printf_locked("SKIP_NO_HOST,%s\n", resolved);
            } else {
                if (strcmp(link_norm_host, start_host) == 0) {
                    if (!visited_check_and_add(resolved)) {
                        enqueue_task(resolved, next_depth);
                        log_printf_locked("ENQUEUE,%s,host=%s,depth=%d\n", resolved, link_norm_host, next_depth);
                    } else {
                        log_printf_locked("SKIP_VISITED,%s,host=%s\n", resolved, link_norm_host);
                    }
                } else {
                    log_printf_locked("SKIP_FOREIGN_HOST,%s,host=%s,start_host=%s\n", resolved, link_norm_host, start_host);
                }
                free(link_norm_host);
            }
            free(resolved);
        }
    }
    xmlXPathFreeObject(obj);
    xmlXPathFreeContext(ctx);
    xmlFreeDoc(doc);
}

/* download with optional retry; returns 1 on success, sets content_type and http_code */
static int download_url_to_memory(CURL *curl, const char *url, Memory *mem, long *http_code, char **content_type) {
    int attempt = 0;
    while (attempt <= CURL_RETRY) {
        mem->data = malloc(1);
        mem->size = 0;
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)mem);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "tiny_email_wget_fbi/0.1");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            free(mem->data); mem->data = NULL; mem->size = 0;
            log_printf_locked("CURL_ERROR,%s,attempt=%d,%s\n", url, attempt, curl_easy_strerror(res));
            attempt++;
            if (attempt > CURL_RETRY) return 0;
            /* small backoff */
            struct timespec ts = {0, 200 * 1000000}; nanosleep(&ts, NULL);
            continue;
        }
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, http_code);
        char *ctype = NULL;
        curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ctype);
        if (ctype) *content_type = strdup(ctype); else *content_type = NULL;
        log_printf_locked("HTTP,%s,%ld\n", url, *http_code);
        return 1;
    }
    return 0;
}

/* worker */
static void *worker_main(void *arg) {
    (void)arg;
    CURL *curl = curl_easy_init();
    if (!curl) return NULL;
    while (1) {
        Task *t = dequeue_task();
        if (!t) break;
        log_visit_console(t->url);

        Memory mem = {0};
        long http_code = 0;
        char *ctype = NULL;
        int ok = download_url_to_memory(curl, t->url, &mem, &http_code, &ctype);
        if (!ok) { free(t->url); free(t); continue; }
        if (http_code >= 400) {
            log_printf_locked("HTTP_FAIL,%s,%ld\n", t->url, http_code);
            free(mem.data); free(ctype); free(t->url); free(t);
            continue;
        }

        /* scan raw text for emails */
        extract_emails_from_text(mem.data);

        /* if HTML, parse links and enqueue */
        if (ctype && strstr(ctype, "text/html")) {
            extract_links_and_enqueue(t->url, mem.data, t->depth + 1);
        }

        free(mem.data);
        free(ctype);
        free(t->url);
        free(t);

        struct timespec ts = {0, 150 * 1000000}; nanosleep(&ts, NULL);
    }
    curl_easy_cleanup(curl);
    return NULL;
}

/* monitor thread prints queue stats periodically until closed */
static void *monitor_main(void *arg) {
    (void)arg;
    while (1) {
        pthread_mutex_lock(&queue.mtx);
        int empty = (queue.count == 0 && queue.head == NULL && queue.closed);
        size_t qc = queue.count;
        pthread_mutex_unlock(&queue.mtx);
        log_printf_locked("MONITOR,queue_count=%zu\n", qc);
        if (empty) break;
        usleep(MONITOR_INTERVAL_US);
    }
    return NULL;
}

int main(void) {
    FILE *vf = fopen(VISITED_FILE, "a"); if (vf) fclose(vf);
    FILE *ef = fopen(EMAILS_FILE, "a"); if (ef) fclose(ef);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    xmlInitParser();

    char *raw_start_host = get_host_from_url(START_URL);
    if (!raw_start_host) { fprintf(stderr, "Invalid START_URL\n"); return 1; }
    start_host = normalize_host(raw_start_host);
    free(raw_start_host);
    if (!start_host) { fprintf(stderr, "Failed normalize start host\n"); return 1; }
    log_printf_locked("START_HOST=%s\n", start_host);

    visited_check_and_add(START_URL);
    enqueue_task(START_URL, 0);

    pthread_t workers[WORKER_COUNT];
    for (int i = 0; i < WORKER_COUNT; ++i) {
        if (pthread_create(&workers[i], NULL, worker_main, NULL) != 0) { /* continue with fewer */ }
    }

    pthread_t monitor;
    pthread_create(&monitor, NULL, monitor_main, NULL);

    /* wait until queue drained and no more tasks */
    while (1) {
        pthread_mutex_lock(&queue.mtx);
        int empty = (queue.count == 0 && queue.head == NULL);
        pthread_mutex_unlock(&queue.mtx);
        if (empty) break;
        usleep(MONITOR_INTERVAL_US);
    }

    /* close and join */
    close_queue();
    for (int i = 0; i < WORKER_COUNT; ++i) pthread_join(workers[i], NULL);
    pthread_join(monitor, NULL);

    /* cleanup in-memory lists */
    pthread_mutex_lock(&visited_mtx);
    while (visited) { VisitedItem *t = visited; visited = visited->next; free(t->url); free(t); }
    pthread_mutex_unlock(&visited_mtx);

    pthread_mutex_lock(&emails_mtx);
    while (emails_head) { EmailItem *t = emails_head; emails_head = emails_head->next; free(t->email); free(t); }
    pthread_mutex_unlock(&emails_mtx);

    free(start_host);
    xmlCleanupParser();
    curl_global_cleanup();
    return 0;
}
