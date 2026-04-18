// compile: gcc -O2 -o crawler crawler.c -lcurl -lpthread
// usage:   ./crawler urls.txt mail.txt crawled.txt allowed_domains.txt
// NOTE: Use only on domains you own or have explicit permission to test.

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <curl/curl.h>
#include <regex.h>

#define MAX_THREADS 32
#define MAX_LINE 4096

typedef struct {
    char *data;
    size_t size;
} MemBuf;

typedef struct {
    char **urls;
    int start;
    int end;
    const char *mail_out;
    const char *crawled_out;
    regex_t email_rx;
    char **allowed_domains;
    int allowed_count;
} WorkerArgs;

pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER;

static size_t write_cb(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total = size * nmemb;
    MemBuf *mb = (MemBuf *)userp;
    char *p = realloc(mb->data, mb->size + total + 1);
    if (!p) return 0;
    mb->data = p;
    memcpy(mb->data + mb->size, contents, total);
    mb->size += total;
    mb->data[mb->size] = '\0';
    return total;
}

static int domain_allowed(const char *email, char **allowed, int n) {
    for (int i = 0; i < n; i++) {
        size_t len = strlen(allowed[i]);
        size_t emlen = strlen(email);
        if (emlen >= len) {
            const char *end = email + emlen - len;
            if (strcasecmp(end, allowed[i]) == 0) return 1;
        }
    }
    return 0;
}

static void dedupe_append(FILE *f, const char *path, const char *line) {
    // naive in-file dedupe: scan existing lines to avoid duplicates
    // For large scale, replace with a hash set persisted to disk.
    FILE *rf = fopen(path, "r");
    if (rf) {
        char buf[MAX_LINE];
        while (fgets(buf, sizeof(buf), rf)) {
            if (buf[strlen(buf)-1] == '\n') buf[strlen(buf)-1] = '\0';
            if (strcmp(buf, line) == 0) { fclose(rf); return; }
        }
        fclose(rf);
    }
    fprintf(f, "%s\n", line);
    fflush(f);
}

static void extract_emails_and_write(const char *html, const char *url,
                                     FILE *mailf, FILE *crawledf,
                                     regex_t *rx, char **allowed, int allowed_n) {
    const char *p = html;
    regmatch_t m;
    // record crawled URL once
    dedupe_append(crawledf, "crawled.tmp", url);

    while (regexec(rx, p, 1, &m, 0) == 0) {
        int len = (int)(m.rm_eo - m.rm_so);
        if (len > 0 && len < MAX_LINE) {
            char email[MAX_LINE];
            memcpy(email, p + m.rm_so, len);
            email[len] = '\0';
            if (domain_allowed(email, allowed, allowed_n)) {
                dedupe_append(mailf, "mail.tmp", email);
            }
        }
        p += m.rm_eo;
    }
}

static void *worker(void *arg) {
    WorkerArgs *wa = (WorkerArgs *)arg;
    CURL *curl = curl_easy_init();
    if (!curl) return NULL;

    for (int i = wa->start; i < wa->end; i++) {
        const char *url = wa->urls[i];

        MemBuf mb = { .data = NULL, .size = 0 };
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mb);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "SafeCrawler/1.0");
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip, deflate");

        CURLcode rc = curl_easy_perform(curl);
        if (rc != CURLE_OK) {
            fprintf(stderr, "Fetch error %s: %s\n", url, curl_easy_strerror(rc));
            free(mb.data);
            continue;
        }

        pthread_mutex_lock(&write_mutex);
        FILE *mailf = fopen(wa->mail_out, "a");
        FILE *crawledf = fopen(wa->crawled_out, "a");
        if (mailf && crawledf) {
            extract_emails_and_write(mb.data ? mb.data : "", url, mailf, crawledf,
                                     &wa->email_rx, wa->allowed_domains, wa->allowed_count);
        }
        if (mailf) fclose(mailf);
        if (crawledf) fclose(crawledf);
        pthread_mutex_unlock(&write_mutex);

        free(mb.data);
    }

    curl_easy_cleanup(curl);
    return NULL;
}

static char **load_lines(const char *path, int *out_count) {
    FILE *f = fopen(path, "r");
    if (!f) { *out_count = 0; return NULL; }
    char **list = NULL;
    int cap = 0, n = 0;
    char buf[MAX_LINE];
    while (fgets(buf, sizeof(buf), f)) {
        size_t L = strlen(buf);
        while (L > 0 && (buf[L-1] == '\n' || buf[L-1] == '\r')) buf[--L] = '\0';
        if (L == 0) continue;
        if (n == cap) {
            cap = cap ? cap * 2 : 64;
            list = realloc(list, cap * sizeof(char *));
        }
        list[n] = strdup(buf);
        n++;
    }
    fclose(f);
    *out_count = n;
    return list;
}

static int in_set(const char *s, char **set, int n) {
    for (int i = 0; i < n; i++) {
        if (strcmp(s, set[i]) == 0) return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 5) {
        fprintf(stderr, "Usage: %s urls.txt mail.txt crawled.txt allowed_domains.txt\n", argv[0]);
        return 1;
    }
    const char *urls_path = argv[1];
    const char *mail_path = argv[2];
    const char *crawled_path = argv[3];
    const char *allowed_path = argv[4];

    curl_global_init(CURL_GLOBAL_ALL);

    // Load inputs
    int url_count = 0, crawled_count = 0, allowed_count = 0;
    char **urls = load_lines(urls_path, &url_count);
    char **crawled = load_lines(crawled_path, &crawled_count);
    char **allowed = load_lines(allowed_path, &allowed_count);
    if (!urls || url_count == 0) {
        fprintf(stderr, "No URLs loaded from %s\n", urls_path);
        return 1;
    }
    if (!allowed || allowed_count == 0) {
        fprintf(stderr, "No allowed domains loaded from %s\n", allowed_path);
        return 1;
    }

    // Filter out previously crawled URLs (skip)
    char **todo = malloc(url_count * sizeof(char *));
    int todo_count = 0;
    for (int i = 0; i < url_count; i++) {
        if (!in_set(urls[i], crawled, crawled_count)) {
            todo[todo_count++] = urls[i];
        }
    }
    if (todo_count == 0) {
        fprintf(stderr, "Nothing to crawl; all URLs are in %s\n", crawled_path);
        return 0;
    }

    // Email regex (generic pattern)
    regex_t email_rx;
    // Basic RFC5322-inspired (limited) pattern; adjust as needed
    const char *rx = "[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}";
    if (regcomp(&email_rx, rx, REG_EXTENDED) != 0) {
        fprintf(stderr, "Failed to compile regex\n");
        return 1;
    }

    // Create threads
    int threads_n = (todo_count < MAX_THREADS) ? todo_count : MAX_THREADS;
    pthread_t threads[MAX_THREADS];
    WorkerArgs args[MAX_THREADS];

    int chunk = (todo_count + threads_n - 1) / threads_n;
    for (int t = 0; t < threads_n; t++) {
        int start = t * chunk;
        int end = start + chunk;
        if (end > todo_count) end = todo_count;

        args[t].urls = todo;
        args[t].start = start;
        args[t].end = end;
        args[t].mail_out = mail_path;
        args[t].crawled_out = crawled_path;
        args[t].email_rx = email_rx; // struct copy
        args[t].allowed_domains = allowed;
        args[t].allowed_count = allowed_count;

        pthread_create(&threads[t], NULL, worker, &args[t]);
    }

    for (int t = 0; t < threads_n; t++) {
        pthread_join(threads[t], NULL);
    }

    regfree(&email_rx);
    curl_global_cleanup();

    // free memory
    for (int i = 0; i < url_count; i++) free(urls[i]);
    free(urls);
    for (int i = 0; i < crawled_count; i++) free(crawled[i]);
    free(crawled);
    for (int i = 0; i < allowed_count; i++) free(allowed[i]);
    free(allowed);
    free(todo);

    printf("Done. Unique matches in %s. Crawled URLs appended to %s.\n", mail_path, crawled_path);
    return 0;
}
