/*
 multi_fork_email_wget_nocurl.c
 Forks one process per start site and runs a multithreaded email-only crawler in each child.
 Does NOT use libcurl. Uses the curl CLI via popen to fetch pages.

 Build:
   gcc -O2 -pthread -o multi_fork_email_wget_nocurl multi_fork_email_wget_nocurl.c -lxml2

 Warning: This program issues many HTTP requests. Respect robots.txt and site policies.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <regex.h>
#include <libxml/HTMLparser.h>
#include <libxml/uri.h>
#include <libxml/xpath.h>
#include <time.h>

#define VISITED_FILE "visited.txt"
#define EMAILS_FILE "emails.txt"

#define MAX_DEPTH 12
#define WORKER_COUNT 32
#define MONITOR_INTERVAL_US 1
#define MAX_CURL_OUT (8*1024*1024) /* 4MB cap per fetch to avoid runaway */

static const char *start_urls[] = {
    "https://dia.mil/",
    "https://cia.gov/",
    "https://state.gov/",
    "https://www.dhs.gov/",
    "https://www.fbi.gov/"
};
static const int start_sites = sizeof(start_urls) / sizeof(start_urls[0]);

/* --- data structures --- */
typedef struct Task {
    char *url;
    int depth;
    struct Task *next;
} Task;

typedef struct {
    Task *head, *tail;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    int closed;
    size_t count;
} TaskQueue;

typedef struct VisitedItem { char *url; struct VisitedItem *next; } VisitedItem;
typedef struct EmailItem { char *email; struct EmailItem *next; } EmailItem;

static TaskQueue queue = { .head=NULL, .tail=NULL, .mtx=PTHREAD_MUTEX_INITIALIZER, .cond=PTHREAD_COND_INITIALIZER, .closed=0, .count=0 };
static VisitedItem *visited = NULL;
static pthread_mutex_t visited_mtx = PTHREAD_MUTEX_INITIALIZER;
static EmailItem *emails_head = NULL;
static pthread_mutex_t emails_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t log_mtx = PTHREAD_MUTEX_INITIALIZER;
static char *start_host = NULL;

/* helpers */
static void log_locked(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    pthread_mutex_lock(&log_mtx);
    vprintf(fmt, ap);
    fflush(stdout);
    pthread_mutex_unlock(&log_mtx);
    va_end(ap);
}

static void now_str(char *buf, size_t n) {
    time_t t = time(NULL);
    struct tm tm;
    gmtime_r(&t, &tm);
    strftime(buf, n, "%Y-%m-%dT%H:%M:%SZ", &tm);
}

static char *get_host(const char *url) {
    xmlURIPtr u = xmlParseURI(url);
    if (!u) return NULL;
    char *h = u->server ? strdup(u->server) : NULL;
    xmlFreeURI(u);
    return h;
}
static char *normalize_host(const char *host) {
    if (!host) return NULL;
    const char *p = host;
    if (strncmp(p, "www.", 4) == 0) p += 4;
    char *r = strdup(p);
    for (char *c = r; *c; ++c) if (*c >= 'A' && *c <= 'Z') *c = *c - 'A' + 'a';
    return r;
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

/* visited/email sets */
static int visited_check_and_add(const char *url) {
    pthread_mutex_lock(&visited_mtx);
    VisitedItem *it = visited;
    while (it) { if (strcmp(it->url, url) == 0) { pthread_mutex_unlock(&visited_mtx); return 1; } it = it->next; }
    VisitedItem *n = malloc(sizeof(VisitedItem));
    n->url = strdup(url); n->next = visited; visited = n;
    pthread_mutex_unlock(&visited_mtx);
    FILE *f = fopen(VISITED_FILE, "a"); if (f) { fprintf(f, "%s\n", url); fclose(f); }
    return 0;
}
static int email_check_and_add(const char *email) {
    pthread_mutex_lock(&emails_mtx);
    EmailItem *it = emails_head;
    while (it) { if (strcmp(it->email, email) == 0) { pthread_mutex_unlock(&emails_mtx); return 1; } it = it->next; }
    EmailItem *n = malloc(sizeof(EmailItem));
    n->email = strdup(email); n->next = emails_head; emails_head = n;
    pthread_mutex_unlock(&emails_mtx);
    FILE *f = fopen(EMAILS_FILE, "a"); if (f) { fprintf(f, "%s\n", email); fclose(f); }
    return 0;
}

/* run curl CLI via popen; returns dynamically allocated body string (must free), or NULL */
static char *fetch_with_curl(const char *url, long *out_status) {
    /* curl options:
       -s silent, -L follow, --compressed, -A UA
       include -S to show errors to stderr but popen captures stdout only
       We use --max-filesize not reliable; instead cap after read.
    */
    const char *ua = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/127 Safari/537.36";
    char cmd[4096];
    snprintf(cmd, sizeof(cmd),
             "curl -s -S -L --compressed -A '%s' -H 'Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8' --max-redirs 10 --location-trusted --write-out '%%{http_code}' '%s'",
             ua, url);
    /* We will run curl and capture both body and final http code.
       Because write-out emits http code to stdout at end, we need to separate it.
       Strategy: use a temporary file: curl -o - --write-out 'CODE' will append CODE to stdout.
       To handle that robustly without libcurl, run curl and read into buffer, then strip trailing digits.
    */

    /* Use popen to read stdout */
    FILE *fp = popen(cmd, "r");
    if (!fp) return NULL;

    size_t cap = 16384;
    char *buf = malloc(cap);
    if (!buf) { pclose(fp); return NULL; }
    size_t len = 0;
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (len + 1 >= cap) {
            if (cap >= MAX_CURL_OUT) break;
            cap = cap * 2;
            if (cap > MAX_CURL_OUT) cap = MAX_CURL_OUT;
            char *tmp = realloc(buf, cap);
            if (!tmp) break;
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    /* close popen and get exit status (but write-out appended http code to stdout instead of using exit) */
    int rc = pclose(fp);
    /* The curl command above appended the http code as final digits to stdout:
       We try to parse trailing 3-digit code if present; otherwise set -1.
    */
    buf[len] = '\0';
    long code = -1;
    if (len >= 3) {
        /* find last newline and last non-space */
        size_t i = len;
        while (i > 0 && (buf[i-1] == '\n' || buf[i-1] == '\r' || buf[i-1] == ' ' || buf[i-1] == '\t')) --i;
        /* attempt to read last 3 digits */
        if (i >= 3) {
            char tail[4] = { buf[i-3], buf[i-2], buf[i-1], '\0' };
            int all_digits = 1;
            for (int k=0;k<3;k++) if (tail[k] < '0' || tail[k] > '9') { all_digits = 0; break; }
            if (all_digits) {
                code = strtol(tail, NULL, 10);
                /* trim trailing code from buffer */
                buf[i-3] = '\0';
                len = i-3;
            }
        }
    }
    if (out_status) *out_status = code;
    /* ensure NUL termination */
    buf = realloc(buf, len+1);
    buf[len] = '\0';
    return buf;
}

/* resolve URL with libxml and return strdup of resolved url */
static char *resolve_url(const char *base, const char *href) {
    xmlChar *r = xmlBuildURI((const xmlChar *)href, (const xmlChar *)base);
    if (!r) return NULL;
    char *s = strdup((const char *)r);
    xmlFree(r);
    return s;
}

/* extract emails using regex; add to set */
static void extract_emails_from_text(const char *text) {
    if (!text) return;
    regex_t re;
    const char *pat = "([A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,})";
    if (regcomp(&re, pat, REG_EXTENDED|REG_ICASE) != 0) return;
    const char *p = text;
    regmatch_t m;
    while (regexec(&re, p, 1, &m, 0) == 0) {
        if (m.rm_so >=0 && m.rm_eo > m.rm_so) {
            size_t L = m.rm_eo - m.rm_so;
            char *found = strndup(p + m.rm_so, L);
            for (char *c = found; *c; ++c) if (*c >= 'A' && *c <= 'Z') *c = *c - 'A' + 'a';
            if (!email_check_and_add(found)) log_locked("FOUND_EMAIL,%s\n", found);
            free(found);
        }
        p += m.rm_eo;
    }
    regfree(&re);
}

/* parse HTML and enqueue same-host links; capture mailto: */
static void parse_html_and_enqueue(const char *base, const char *html, int next_depth) {
    if (next_depth > MAX_DEPTH) return;
    htmlDocPtr doc = htmlReadMemory(html, strlen(html), base, NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (!doc) return;
    xmlXPathContextPtr ctx = xmlXPathNewContext(doc);
    if (!ctx) { xmlFreeDoc(doc); return; }
    xmlXPathObjectPtr obj = xmlXPathEvalExpression((xmlChar *)"//@href | //@src", ctx);
    if (!obj) { xmlXPathFreeContext(ctx); xmlFreeDoc(doc); return; }
    xmlNodeSetPtr nodes = obj->nodesetval;
    if (nodes) {
        for (int i=0;i<nodes->nodeNr;i++) {
            xmlNodePtr attr = nodes->nodeTab[i];
            if (!attr || !attr->children || !attr->children->content) continue;
            char *href = (char*)attr->children->content;
            if (!href) continue;
            if (strncasecmp(href, "mailto:", 7) == 0) {
                const char *addr = href + 7;
                if (*addr) {
                    char *copy = strdup(addr);
                    char *q = strchr(copy, '?'); if (q) *q = '\0';
                    char *f = strchr(copy, '#'); if (f) *f = '\0';
                    for (char *c = copy; *c; ++c) if (*c >= 'A' && *c <= 'Z') *c = *c - 'A' + 'a';
                    if (!email_check_and_add(copy)) log_locked("FOUND_MAILTO,%s\n", copy);
                    free(copy);
                }
                continue;
            }
            if (strncasecmp(href, "javascript:", 11) == 0) continue;
            char *resolved = resolve_url(base, href);
            if (!resolved) continue;
            int is_http = (strncasecmp(resolved, "http://", 7) == 0 || strncasecmp(resolved, "https://", 8) == 0);
            char *h = get_host(resolved);
            char *hn = h ? normalize_host(h) : NULL;
            if (h) free(h);
            if (!is_http) { log_locked("SKIP_NOT_HTTP,%s\n", resolved); free(hn); free(resolved); continue; }
            if (!hn) { log_locked("SKIP_NO_HOST,%s\n", resolved); free(resolved); continue; }
            if (strcmp(hn, start_host) == 0) {
                if (!visited_check_and_add(resolved)) {
                    enqueue_task(resolved, next_depth);
                    log_locked("ENQUEUE,%s,host=%s,depth=%d\n", resolved, hn, next_depth);
                } else {
                    log_locked("SKIP_VISITED,%s,host=%s\n", resolved, hn);
                }
            } else {
                log_locked("SKIP_FOREIGN,%s,host=%s,start=%s\n", resolved, hn, start_host);
            }
            free(hn);
            free(resolved);
        }
    }
    xmlXPathFreeObject(obj);
    xmlXPathFreeContext(ctx);
    xmlFreeDoc(doc);
}

/* worker thread: fetch via curl CLI and process */
static void *worker_main(void *arg) {
    (void)arg;
    while (1) {
        Task *t = dequeue_task();
        if (!t) break;
        char timestr[32]; now_str(timestr, sizeof(timestr));
        log_locked("%s,thread:%lu,visit,%s\n", timestr, (unsigned long)pthread_self(), t->url);

        long http_code = -1;
        char *body = fetch_with_curl(t->url, &http_code);
        if (!body) { log_locked("FETCH_FAIL,%s\n", t->url); free(t->url); free(t); continue; }
        log_locked("HTTP,%s,%ld\n", t->url, http_code);
        if (http_code >= 400) {
            log_locked("HTTP_FAIL,%s,%ld\n", t->url, http_code);
            free(body); free(t->url); free(t); continue;
        }

        /* extract emails from body */
        extract_emails_from_text(body);

        /* if HTML content likely, attempt parsing: crude check for <html */
        if (strcasestr(body, "<html") || strcasestr(body, "<!doctype html") || strcasestr(body, "<body")) {
            parse_html_and_enqueue(t->url, body, t->depth + 1);
        }

        free(body);
        free(t->url);
        free(t);
        struct timespec ts = {0, 150 * 1000000}; nanosleep(&ts, NULL);
    }
    return NULL;
}

/* monitor thread prints queue count until closed */
static void *monitor_main(void *arg) {
    (void)arg;
    while (1) {
        pthread_mutex_lock(&queue.mtx);
        size_t qc = queue.count;
        int closed = queue.closed;
        pthread_mutex_unlock(&queue.mtx);
        log_locked("MONITOR,queue_count=%zu\n", qc);
        if (closed && qc == 0) break;
        usleep(MONITOR_INTERVAL_US);
    }
    return NULL;
}

/* run crawler in this child for start_url */
static int run_crawler(const char *start_url) {
    xmlInitParser();

    char *raw = get_host(start_url);
    if (!raw) { fprintf(stderr, "Invalid start URL: %s\n", start_url); return 1; }
    start_host = normalize_host(raw);
    free(raw);
    if (!start_host) { fprintf(stderr, "Failed normalize start host\n"); return 1; }
    log_locked("START_HOST=%s\n", start_host);

    FILE *vf = fopen(VISITED_FILE, "a"); if (vf) fclose(vf);
    FILE *ef = fopen(EMAILS_FILE, "a"); if (ef) fclose(ef);

    visited_check_and_add(start_url);
    enqueue_task(start_url, 0);

    pthread_t workers[WORKER_COUNT];
    for (int i=0;i<WORKER_COUNT;i++) pthread_create(&workers[i], NULL, worker_main, NULL);

    pthread_t monitor;
    pthread_create(&monitor, NULL, monitor_main, NULL);

    /* wait until queue empties */
    while (1) {
        pthread_mutex_lock(&queue.mtx);
        int empty = (queue.count == 0 && queue.head == NULL);
        pthread_mutex_unlock(&queue.mtx);
        if (empty) break;
        usleep(MONITOR_INTERVAL_US);
    }

    close_queue();
    for (int i=0;i<WORKER_COUNT;i++) pthread_join(workers[i], NULL);
    pthread_join(monitor, NULL);

    /* cleanup lists */
    pthread_mutex_lock(&visited_mtx);
    while (visited) { VisitedItem *n = visited; visited = visited->next; free(n->url); free(n); }
    pthread_mutex_unlock(&visited_mtx);

    pthread_mutex_lock(&emails_mtx);
    while (emails_head) { EmailItem *n = emails_head; emails_head = emails_head->next; free(n->email); free(n); }
    pthread_mutex_unlock(&emails_mtx);

    free(start_host);
    xmlCleanupParser();
    return 0;
}

/* main: fork one child per site, each runs run_crawler */
int main(void) {
    pid_t pids[start_sites];
    for (int i=0;i<start_sites;i++) {
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); pids[i] = -1; continue; }
        if (pid == 0) {
            /* child */
            run_crawler(start_urls[i]);
            _exit(0);
        } else {
            pids[i] = pid;
        }
    }

    /* parent waits */
    for (int i=0;i<start_sites;i++) {
        if (pids[i] <= 0) continue;
        int status = 0;
        pid_t w = waitpid(pids[i], &status, 0);
        if (w > 0) {
            if (WIFEXITED(status)) fprintf(stderr, "child %d (site %s) exited %d\n", (int)w, start_urls[i], WEXITSTATUS(status));
            else if (WIFSIGNALED(status)) fprintf(stderr, "child %d (site %s) killed by signal %d\n", (int)w, start_urls[i], WTERMSIG(status));
            else fprintf(stderr, "child %d (site %s) ended status %d\n", (int)w, start_urls[i], status);
        }
    }
    return 0;
}
