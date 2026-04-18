/*
 * crawler.c
 *
 * Patched: parallel crawler with libxml2 extraction for emails/phones/links.
 * Added logging:
 *  - Logs every newly crawled page with its ranking (score).
 *  - Logs every newly inserted E-Mail address and Phone number.
 *
 * Build:
 *   gcc -std=c11 -O2 -o addresscrawler_cli crawler.c \
 *       -lcurl -lsqlite3 -lcrypto -lxml2 -pthread
 *
 * Usage:
 *   ./addresscrawler_cli <seed-url> [max-pages] [max-depth] [workers]
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include <curl/curl.h>
#include <sqlite3.h>
#include <openssl/sha.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

/* ----------------------------- Configuration ----------------------------- */

#define DEFAULT_MAX_PAGES 200
#define DEFAULT_MAX_DEPTH 3
#define USER_AGENT "AddressCrawlerCLI/1.0"
#define DB_FILENAME "crawler.db"
#define POLITE_DELAY_MS 150
#define DEFAULT_WORKERS 6

/* ----------------------------- Globals ----------------------------- */

static sqlite3 *g_db = NULL;
static int g_max_depth = DEFAULT_MAX_DEPTH;
static pthread_mutex_t g_db_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_frontier_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_frontier_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t g_log_lock = PTHREAD_MUTEX_INITIALIZER;
static int g_stop = 0;
static int g_workers = DEFAULT_WORKERS;

/* ----------------------------- Small helpers ----------------------------- */

static char *
strdup_safe(const char *s)
{
    if (s == NULL)
        return NULL;
    char *r = strdup(s);
    if (r == NULL) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }
    return r;
}

static void
sleep_polite(void)
{
    usleep(POLITE_DELAY_MS * 1000);
}

static void
log_info(const char *fmt, ...)
{
    va_list ap;
    time_t t = time(NULL);
    struct tm tm;
    char timestr[64];
    localtime_r(&t, &tm);
    strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", &tm);

    pthread_mutex_lock(&g_log_lock);
    printf("[%s] ", timestr);
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
    fflush(stdout);
    pthread_mutex_unlock(&g_log_lock);
}

/* ----------------------------- Dynamic buffer for curl ----------------------------- */

typedef struct dynbuf {
    char *data;
    size_t len;
} dynbuf_t;

static void
dbuf_init(dynbuf_t *b)
{
    b->data = NULL;
    b->len = 0;
}

static void
dbuf_free(dynbuf_t *b)
{
    free(b->data);
    b->data = NULL;
    b->len = 0;
}

static size_t
dbuf_write(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t add = size * nmemb;
    dynbuf_t *b = (dynbuf_t *)userdata;
    char *n = realloc(b->data, b->len + add + 1);
    if (n == NULL)
        return 0;
    memcpy(n + b->len, ptr, add);
    b->data = n;
    b->len += add;
    b->data[b->len] = '\0';
    return add;
}

/* ----------------------------- SHA-256 helpers ----------------------------- */

static char *
sha256_hex(const unsigned char *data, size_t len)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX ctx;
    if (SHA256_Init(&ctx) != 1)
        return NULL;
    if (SHA256_Update(&ctx, data, len) != 1)
        return NULL;
    if (SHA256_Final(hash, &ctx) != 1)
        return NULL;

    char *hex = malloc(SHA256_DIGEST_LENGTH * 2 + 1);
    if (hex == NULL)
        return NULL;
    for (size_t i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        sprintf(hex + (i * 2), "%02x", hash[i]);
    SHA256_DIGEST_LENGTH * 2;
    hex[SHA256_DIGEST_LENGTH * 2] = '\0';
    return hex;
}

/* ----------------------------- URL utilities ----------------------------- */

static char *
normalize_url(const char *url)
{
    if (url == NULL)
        return NULL;
    char *copy = strdup_safe(url);
    char *p = strchr(copy, '#');
    if (p)
        *p = '\0';
    size_t len = strlen(copy);
    while (len > 1 && copy[len - 1] == '/') {
        copy[len - 1] = '\0';
        len--;
    }
    return copy;
}

static char *
url_host(const char *url)
{
    if (url == NULL)
        return NULL;
    const char *p = strstr(url, "://");
    const char *h = (p != NULL) ? p + 3 : url;
    const char *end = strchr(h, '/');
    size_t len = (end != NULL) ? (size_t)(end - h) : strlen(h);
    char *host = malloc(len + 1);
    if (host == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    memcpy(host, h, len);
    host[len] = '\0';
    char *colon = strchr(host, ':');
    if (colon)
        *colon = '\0';
    for (char *c = host; *c; ++c)
        *c = (char)tolower((unsigned char)*c);
    return host;
}

static double
domain_trust(const char *url)
{
    char *host = url_host(url);
    double score = 0.5;
    if (host == NULL)
        return score;
    size_t L = strlen(host);
    if (L >= 4 && strcmp(host + L - 4, ".gov") == 0)
        score = 5.0;
    else if (L >= 4 && strcmp(host + L - 4, ".edu") == 0)
        score = 4.0;
    else if (L >= 4 && strcmp(host + L - 4, ".org") == 0)
        score = 2.0;
    else if (L >= 4 && strcmp(host + L - 4, ".com") == 0)
        score = 1.5;
    free(host);
    return score;
}

static char *
resolve_url(const char *base, const char *href)
{
    if (href == NULL || href[0] == '\0')
        return NULL;
    if (strncasecmp(href, "http://", 7) == 0 || strncasecmp(href, "https://", 8) == 0)
        return strdup_safe(href);

    const char *p = strstr(base, "://");
    if (p == NULL)
        return strdup_safe(href);
    const char *hoststart = p + 3;
    const char *pathstart = strchr(hoststart, '/');
    size_t hostlen = pathstart ? (size_t)(pathstart - base) : strlen(base);
    char *root = malloc(hostlen + 1);
    if (root == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    memcpy(root, base, hostlen);
    root[hostlen] = '\0';

    char *out = NULL;
    if (href[0] == '/') {
        if (asprintf(&out, "%s%s", root, href) < 0)
            out = NULL;
    } else {
        const char *lastslash = pathstart ? strrchr(base, '/') : NULL;
        if (lastslash) {
            size_t prefixlen = (size_t)(lastslash - base) + 1;
            char *prefix = malloc(prefixlen + 1);
            if (prefix == NULL) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            memcpy(prefix, base, prefixlen);
            prefix[prefixlen] = '\0';
            if (asprintf(&out, "%s%s", prefix, href) < 0)
                out = NULL;
            free(prefix);
        } else {
            if (asprintf(&out, "%s/%s", root, href) < 0)
                out = NULL;
        }
    }
    free(root);
    return out;
}

/* ----------------------------- Heap (max-heap) for frontier ----------------------------- */

typedef struct crawl_item {
    char *url;
    char *norm;
    int depth;
    double score;
    int inbound;
} crawl_item_t;

typedef struct heap {
    crawl_item_t *items;
    size_t size;
    size_t cap;
} heap_t;

static void
heap_init(heap_t *h)
{
    h->items = NULL;
    h->size = 0;
    h->cap = 0;
}

static void
heap_free(heap_t *h)
{
    for (size_t i = 0; i < h->size; ++i) {
        free(h->items[i].url);
        free(h->items[i].norm);
    }
    free(h->items);
    h->items = NULL;
    h->size = h->cap = 0;
}

static void
heap_swap(crawl_item_t *a, crawl_item_t *b)
{
    crawl_item_t t = *a;
    *a = *b;
    *b = t;
}

static void
heap_push(heap_t *h, crawl_item_t it)
{
    if (h->size + 1 > h->cap) {
        size_t ncap = h->cap ? h->cap * 2 : 64;
        crawl_item_t *n = realloc(h->items, ncap * sizeof(crawl_item_t));
        if (n == NULL) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        h->items = n;
        h->cap = ncap;
    }
    size_t i = h->size++;
    h->items[i] = it;
    while (i > 0) {
        size_t p = (i - 1) / 2;
        if (h->items[p].score >= h->items[i].score)
            break;
        heap_swap(&h->items[p], &h->items[i]);
        i = p;
    }
}

static int
heap_empty(const heap_t *h)
{
    return h->size == 0;
}

static crawl_item_t
heap_pop_max(heap_t *h)
{
    if (h->size == 0) {
        fprintf(stderr, "heap_pop_max: empty\n");
        exit(EXIT_FAILURE);
    }
    crawl_item_t out = h->items[0];
    h->items[0] = h->items[--h->size];
    size_t i = 0;
    while (1) {
        size_t l = 2 * i + 1;
        size_t r = 2 * i + 2;
        size_t largest = i;
        if (l < h->size && h->items[l].score > h->items[largest].score)
            largest = l;
        if (r < h->size && h->items[r].score > h->items[largest].score)
            largest = r;
        if (largest == i)
            break;
        heap_swap(&h->items[i], &h->items[largest]);
        i = largest;
    }
    return out;
}

static crawl_item_t
heap_pop_random(heap_t *h)
{
    if (h->size == 0) {
        fprintf(stderr, "heap_pop_random: empty\n");
        exit(EXIT_FAILURE);
    }
    size_t idx = (size_t)rand() % h->size;
    crawl_item_t out = h->items[idx];
    h->items[idx] = h->items[--h->size];
    size_t i = idx;
    while (1) {
        size_t l = 2 * i + 1;
        size_t r = 2 * i + 2;
        size_t largest = i;
        if (l < h->size && h->items[l].score > h->items[largest].score)
            largest = l;
        if (r < h->size && h->items[r].score > h->items[largest].score)
            largest = r;
        if (largest == i)
            break;
        heap_swap(&h->items[i], &h->items[largest]);
        i = largest;
    }
    while (i > 0) {
        size_t p = (i - 1) / 2;
        if (h->items[p].score >= h->items[i].score)
            break;
        heap_swap(&h->items[p], &h->items[i]);
        i = p;
    }
    return out;
}

/* ----------------------------- Thread-safe heap wrappers ----------------------------- */

static int
ts_heap_pop_max(heap_t *h, crawl_item_t *out)
{
    pthread_mutex_lock(&g_frontier_lock);
    while (heap_empty(h) && !g_stop) {
        pthread_cond_wait(&g_frontier_cond, &g_frontier_lock);
    }
    if (g_stop && heap_empty(h)) {
        pthread_mutex_unlock(&g_frontier_lock);
        return 0;
    }
    if (heap_empty(h)) {
        pthread_mutex_unlock(&g_frontier_lock);
        return 0;
    }
    *out = heap_pop_max(h);
    pthread_mutex_unlock(&g_frontier_lock);
    return 1;
}

static void
ts_heap_push(heap_t *h, crawl_item_t it)
{
    pthread_mutex_lock(&g_frontier_lock);
    heap_push(h, it);
    pthread_cond_signal(&g_frontier_cond);
    pthread_mutex_unlock(&g_frontier_lock);
}

/* ----------------------------- SQLite helpers ----------------------------- */

static void
db_open(const char *filename)
{
    if (sqlite3_open(filename, &g_db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open DB %s: %s\n", filename, sqlite3_errmsg(g_db));
        exit(EXIT_FAILURE);
    }
    const char *sql =
        "PRAGMA journal_mode=WAL;"
        "CREATE TABLE IF NOT EXISTS urls ("
        " id INTEGER PRIMARY KEY,"
        " url TEXT UNIQUE,"
        " normalized TEXT UNIQUE,"
        " depth INTEGER,"
        " score REAL,"
        " inbound INTEGER DEFAULT 0,"
        " crawled INTEGER DEFAULT 0,"
        " content_hash TEXT"
        ");"
        "CREATE TABLE IF NOT EXISTS emails (id INTEGER PRIMARY KEY, email TEXT UNIQUE, source_url TEXT);"
        "CREATE TABLE IF NOT EXISTS phones (id INTEGER PRIMARY KEY, phone TEXT UNIQUE, source_url TEXT);";
    char *err = NULL;
    if (sqlite3_exec(g_db, sql, NULL, NULL, &err) != SQLITE_OK) {
        fprintf(stderr, "DB init error: %s\n", err);
        sqlite3_free(err);
        exit(EXIT_FAILURE);
    }
}

static void
db_close(void)
{
    if (g_db)
        sqlite3_close(g_db);
}

/* Return 1 if inserted, 0 otherwise */
static int
db_insert_url(const char *url, const char *norm, int depth, double score)
{
    sqlite3_stmt *stmt = NULL;
    const char *sql = "INSERT OR IGNORE INTO urls (url, normalized, depth, score) VALUES (?, ?, ?, ?);";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return 0;
    sqlite3_bind_text(stmt, 1, url, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, norm, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, depth);
    sqlite3_bind_double(stmt, 4, score);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc == SQLITE_DONE && sqlite3_changes(g_db) > 0)
        return 1;
    return 0;
}

static void
db_mark_crawled_with_hash(const char *norm, const char *hash_hex)
{
    sqlite3_stmt *stmt = NULL;
    const char *sql = "UPDATE urls SET crawled = 1, content_hash = ? WHERE normalized = ?;";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return;
    sqlite3_bind_text(stmt, 1, hash_hex, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, norm, -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

static int
db_has_norm(const char *norm)
{
    sqlite3_stmt *stmt = NULL;
    const char *sql = "SELECT 1 FROM urls WHERE normalized = ? LIMIT 1;";
    int found = 0;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, norm, -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW)
            found = 1;
    }
    sqlite3_finalize(stmt);
    return found;
}

static int
db_has_content_hash(const char *hash_hex)
{
    sqlite3_stmt *stmt = NULL;
    const char *sql = "SELECT 1 FROM urls WHERE content_hash = ? LIMIT 1;";
    int found = 0;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, hash_hex, -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW)
            found = 1;
    }
    sqlite3_finalize(stmt);
    return found;
}

/* Return 1 if inserted, 0 otherwise */
static int
db_insert_email(const char *email, const char *source)
{
    sqlite3_stmt *stmt = NULL;
    const char *sql = "INSERT OR IGNORE INTO emails (email, source_url) VALUES (?, ?);";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return 0;
    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, source, -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc == SQLITE_DONE && sqlite3_changes(g_db) > 0)
        return 1;
    return 0;
}

/* Return 1 if inserted, 0 otherwise */
static int
db_insert_phone(const char *phone, const char *source)
{
    sqlite3_stmt *stmt = NULL;
    const char *sql = "INSERT OR IGNORE INTO phones (phone, source_url) VALUES (?, ?);";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return 0;
    sqlite3_bind_text(stmt, 1, phone, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, source, -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc == SQLITE_DONE && sqlite3_changes(g_db) > 0)
        return 1;
    return 0;
}

static int
db_count_crawled(void)
{
    sqlite3_stmt *stmt = NULL;
    const char *sql = "SELECT COUNT(*) FROM urls WHERE crawled = 1;";
    int count = 0;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW)
            count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count;
}

/* Thread-safe DB wrappers with logging */

static void
db_insert_url_threadsafe(const char *url, const char *norm, int depth, double score)
{
    pthread_mutex_lock(&g_db_lock);
    int inserted = db_insert_url(url, norm, depth, score);
    pthread_mutex_unlock(&g_db_lock);
    if (inserted) {
        log_info("[NEW URL] queued score=%.3f url=%s", score, url);
    }
}

static void
db_mark_crawled_with_hash_threadsafe(const char *norm, const char *hash_hex)
{
    pthread_mutex_lock(&g_db_lock);
    db_mark_crawled_with_hash(norm, hash_hex);
    pthread_mutex_unlock(&g_db_lock);
}

static int
db_has_norm_threadsafe(const char *norm)
{
    pthread_mutex_lock(&g_db_lock);
    int r = db_has_norm(norm);
    pthread_mutex_unlock(&g_db_lock);
    return r;
}

static int
db_has_content_hash_threadsafe(const char *hash_hex)
{
    pthread_mutex_lock(&g_db_lock);
    int r = db_has_content_hash(hash_hex);
    pthread_mutex_unlock(&g_db_lock);
    return r;
}

static void
db_insert_email_threadsafe(const char *email, const char *source)
{
    pthread_mutex_lock(&g_db_lock);
    int inserted = db_insert_email(email, source);
    pthread_mutex_unlock(&g_db_lock);
    if (inserted) {
        log_info("[NEW EMAIL] %s  (source: %s)", email, source ? source : "(unknown)");
    }
}

static void
db_insert_phone_threadsafe(const char *phone, const char *source)
{
    pthread_mutex_lock(&g_db_lock);
    int inserted = db_insert_phone(phone, source);
    pthread_mutex_unlock(&g_db_lock);
    if (inserted) {
        log_info("[NEW PHONE] %s  (source: %s)", phone, source ? source : "(unknown)");
    }
}

/* ----------------------------- Extraction helpers ----------------------------- */

static char *
strip_tags(const char *html)
{
    if (html == NULL)
        return strdup_safe("");
    size_t n = strlen(html);
    char *out = malloc(n + 1);
    if (out == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    size_t oi = 0;
    int in_tag = 0;
    for (size_t i = 0; i < n; ++i) {
        char c = html[i];
        if (c == '<') {
            in_tag = 1;
            continue;
        }
        if (c == '>') {
            in_tag = 0;
            continue;
        }
        if (!in_tag)
            out[oi++] = c;
    }
    out[oi] = '\0';
    return out;
}

static char *
normalize_phone(const char *raw)
{
    if (raw == NULL)
        return NULL;
    size_t L = strlen(raw);
    char *out = malloc(L + 2);
    if (out == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    size_t oi = 0;
    for (size_t i = 0; i < L; ++i) {
        char c = raw[i];
        if (c == '+' && oi == 0)
            out[oi++] = c;
        else if (isdigit((unsigned char)c))
            out[oi++] = c;
    }
    out[oi] = '\0';
    if (oi == 0) {
        free(out);
        return NULL;
    }
    return out;
}

/* Original regex-based extractors kept but DB writes are thread-safe wrappers */

static void
extract_emails(const char *text, const char *source)
{
    regex_t re;
    const char *pattern = "([A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,})";
    if (regcomp(&re, pattern, REG_EXTENDED | REG_ICASE) != 0)
        return;
    const char *p = text;
    regmatch_t m[2];
    while (regexec(&re, p, 2, m, 0) == 0) {
        int start = m[1].rm_so;
        int end = m[1].rm_eo;
        size_t len = (size_t)(end - start);
        char *email = malloc(len + 1);
        if (email == NULL)
            break;
        memcpy(email, p + start, len);
        email[len] = '\0';
        for (char *c = email; *c; ++c)
            *c = (char)tolower((unsigned char)*c);
        db_insert_email_threadsafe(email, source);
        free(email);
        p += end;
    }
    regfree(&re);
}

static void
extract_phones(const char *text, const char *source)
{
    regex_t re;
    const char *pattern = "(\\+?[0-9][0-9\\-\\.\\s\\(\\)]{6,}[0-9])";
    if (regcomp(&re, pattern, REG_EXTENDED | REG_ICASE) != 0)
        return;
    const char *p = text;
    regmatch_t m[2];
    while (regexec(&re, p, 2, m, 0) == 0) {
        int start = m[1].rm_so;
        int end = m[1].rm_eo;
        size_t len = (size_t)(end - start);
        char *raw = malloc(len + 1);
        if (raw == NULL)
            break;
        memcpy(raw, p + start, len);
        raw[len] = '\0';
        char *norm = normalize_phone(raw);
        if (norm) {
            db_insert_phone_threadsafe(norm, source);
            free(norm);
        }
        free(raw);
        p += end;
    }
    regfree(&re);
}

/* ----------------------------- Link extraction and enqueue ----------------------------- */

/* process_href adapted to accept frontier pointer and use thread-safe DB/heap */
static void
process_href(const char *href, const char *base, int base_depth, heap_t *frontier)
{
    if (href == NULL || href[0] == '\0')
        return;

    if (strncasecmp(href, "mailto:", 7) == 0) {
        char *after = strdup_safe(href + 7);
        char *q = strchr(after, '?');
        if (q)
            *q = '\0';
        char *tok = strtok(after, ",");
        while (tok) {
            char *email = strdup_safe(tok);
            for (char *c = email; *c; ++c)
                *c = (char)tolower((unsigned char)*c);
            db_insert_email_threadsafe(email, base);
            free(email);
            tok = strtok(NULL, ",");
        }
        free(after);
        return;
    }

    if (strncasecmp(href, "tel:", 4) == 0) {
        char *after = strdup_safe(href + 4);
        char *q = strchr(after, '?');
        if (q)
            *q = '\0';
        char *norm = normalize_phone(after);
        if (norm) {
            db_insert_phone_threadsafe(norm, base);
            free(norm);
        }
        free(after);
        return;
    }

    char *resolved = resolve_url(base, href);
    if (resolved == NULL)
        return;

    char *norm = normalize_url(resolved);
    if (norm == NULL) {
        free(resolved);
        return;
    }

    if (!db_has_norm_threadsafe(norm) && base_depth + 1 <= g_max_depth) {
        double dt = domain_trust(resolved);
        int inbound = 0;
        int pathlen = (int)strlen(resolved);
        double score = 2.0 * dt + (-1.0) * (base_depth + 1) + 1.0 * inbound + (-0.5) * (pathlen / 10.0);

        crawl_item_t it;
        it.url = strdup_safe(resolved);
        it.norm = norm;
        it.depth = base_depth + 1;
        it.score = score;
        it.inbound = inbound;
        ts_heap_push(frontier, it);
        db_insert_url_threadsafe(resolved, it.norm, it.depth, it.score);
    } else {
        free(norm);
    }
    free(resolved);
}

/* ----------------------------- Curl fetch with header callback ----------------------------- */

typedef struct fetch_meta {
    char content_type[128];
    long response_code;
} fetch_meta_t;

static size_t
header_cb(char *buffer, size_t size, size_t nitems, void *userdata)
{
    size_t len = size * nitems;
    fetch_meta_t *m = (fetch_meta_t *)userdata;
    if (len > 13 && strncasecmp(buffer, "Content-Type:", 13) == 0) {
        const char *p = buffer + 13;
        while (*p && isspace((unsigned char)*p))
            p++;
        size_t copylen = len - (p - buffer);
        if (copylen >= sizeof(m->content_type))
            copylen = sizeof(m->content_type) - 1;
        memcpy(m->content_type, p, copylen);
        m->content_type[copylen] = '\0';
    }
    return len;
}

static int
fetch_url(CURL *curl, const char *url, dynbuf_t *out, fetch_meta_t *meta)
{
    dbuf_init(out);
    meta->content_type[0] = '\0';
    meta->response_code = 0;
    curl_easy_reset(curl);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, dbuf_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, out);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_cb);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, meta);
    CURLcode rc = curl_easy_perform(curl);
    if (rc != CURLE_OK) {
        dbuf_free(out);
        return 0;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &meta->response_code);
    if (meta->content_type[0] && strstr(meta->content_type, "text/html") == NULL) {
        dbuf_free(out);
        return 0;
    }
    return 1;
}

/* ----------------------------- libxml2 parsing and extraction ----------------------------- */

static void
parse_html_and_extract(const char *html, size_t html_len, const char *base, int base_depth, heap_t *frontier, const char *source_url)
{
    if (html == NULL || html_len == 0)
        return;

    htmlDocPtr doc = htmlReadMemory(html, (int)html_len, base, NULL, HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_RECOVER);
    if (doc == NULL)
        return;

    xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
    if (xpathCtx == NULL) {
        xmlFreeDoc(doc);
        return;
    }

    /* 1) Collect all text nodes into a single buffer */
    xmlXPathObjectPtr textObj = xmlXPathEvalExpression((xmlChar *)"//text()", xpathCtx);
    if (textObj && textObj->nodesetval) {
        xmlNodeSetPtr nodes = textObj->nodesetval;
        size_t cap = 4096;
        char *buf = malloc(cap);
        if (buf) {
            buf[0] = '\0';
            size_t len = 0;
            for (int i = 0; i < nodes->nodeNr; ++i) {
                xmlNodePtr node = nodes->nodeTab[i];
                if (node && node->content) {
                    const char *s = (const char *)node->content;
                    size_t sl = strlen(s);
                    if (len + sl + 2 > cap) {
                        cap = (len + sl + 2) * 2;
                        char *n = realloc(buf, cap);
                        if (!n) break;
                        buf = n;
                    }
                    memcpy(buf + len, s, sl);
                    len += sl;
                    buf[len++] = ' ';
                    buf[len] = '\0';
                }
            }

            /* deobfuscate common patterns: [at], (at), " at " -> @ ; [dot], (dot), " dot " -> . */
            regex_t r_at, r_dot;
            regcomp(&r_at, "\\[at\\]|\\(at\\)|\\s+at\\s+", REG_ICASE | REG_EXTENDED);
            regcomp(&r_dot, "\\[dot\\]|\\(dot\\)|\\s+dot\\s+", REG_ICASE | REG_EXTENDED);

            /* replace [at] -> @ */
            {
                regmatch_t m;
                const char *p = buf;
                size_t outcap = strlen(buf) + 1;
                char *tmp = malloc(outcap);
                if (tmp == NULL) {
                    perror("malloc");
                    exit(EXIT_FAILURE);
                }
                tmp[0] = '\0';
                while (regexec(&r_at, p, 1, &m, 0) == 0) {
                    strncat(tmp, p, m.rm_so);
                    strcat(tmp, "@");
                    p += m.rm_eo;
                }
                strcat(tmp, p);
                free((void*)buf);
                buf = tmp;
            }

            /* replace [dot] -> . */
            {
                regmatch_t m;
                const char *p = buf;
                size_t outcap = strlen(buf) + 1;
                char *tmp = malloc(outcap);
                if (tmp == NULL) {
                    perror("malloc");
                    exit(EXIT_FAILURE);
                }
                tmp[0] = '\0';
                while (regexec(&r_dot, p, 1, &m, 0) == 0) {
                    strncat(tmp, p, m.rm_so);
                    strcat(tmp, ".");
                    p += m.rm_eo;
                }
                strcat(tmp, p);
                free((void*)buf);
                buf = tmp;
            }

            regfree(&r_at);
            regfree(&r_dot);

            /* run extractors on buf */
            extract_emails(buf, source_url);
            extract_phones(buf, source_url);
            free(buf);
        }
    }
    if (textObj) xmlXPathFreeObject(textObj);

    /* 2) Collect all href attributes and enqueue */
    xmlXPathObjectPtr hrefObj = xmlXPathEvalExpression((xmlChar *)"//@href", xpathCtx);
    if (hrefObj && hrefObj->nodesetval) {
        xmlNodeSetPtr nodes = hrefObj->nodesetval;
        for (int i = 0; i < nodes->nodeNr; ++i) {
            xmlAttrPtr attr = (xmlAttrPtr)nodes->nodeTab[i];
            if (attr && attr->children && attr->children->content) {
                const char *href = (const char *)attr->children->content;
                process_href(href, base, base_depth, frontier);
            }
        }
    }
    if (hrefObj) xmlXPathFreeObject(hrefObj);

    xmlXPathFreeContext(xpathCtx);
    xmlFreeDoc(doc);
}

/* ----------------------------- CLI printing helpers ----------------------------- */

static void
print_header(const char *seed, int max_pages, int max_depth)
{
    printf("\x1b[1;34m===========================================\x1b[0m\n");
    printf("\x1b[1;32m  AddressCrawler CLI — Priority Email & Phone Crawler\x1b[0m\n");
    printf("  Seed: %s    Depth limit: %d   Max pages: %d\n", seed, max_depth, max_pages);
    printf("\x1b[1;34m===========================================\x1b[0m\n\n");
}

static void
print_progress(int crawled, int planned, const char *current, int queued, int emails, int phones)
{
    printf("\r[Progress] Crawled %3d / %3d  | Current: %.60s\n", crawled, planned, current ? current : "(idle)");
    printf("[Frontier] queued: %d  | emails: %d  | phones: %d\n", queued, emails, phones);
}

/* ----------------------------- Main subfunctions ----------------------------- */

/* init_db: 1 param */
static void
init_db(const char *filename)
{
    db_open(filename);
}

/* init_curl_thread: returns CURL* (per-thread) */
static CURL *
init_curl_thread(void)
{
    CURL *curl = curl_easy_init();
    if (curl == NULL) {
        fprintf(stderr, "curl init failed\n");
        exit(EXIT_FAILURE);
    }
    /* common options can be set by caller */
    return curl;
}

/* enqueue_seed: 2 params */
static void
enqueue_seed(heap_t *frontier, const char *seed)
{
    char *seed_norm = normalize_url(seed);
    double seed_score = 10.0 * domain_trust(seed);
    crawl_item_t seed_item;
    seed_item.url = strdup_safe(seed);
    seed_item.norm = seed_norm;
    seed_item.depth = 0;
    seed_item.score = seed_score;
    seed_item.inbound = 0;
    ts_heap_push(frontier, seed_item);
    db_insert_url_threadsafe(seed, seed_norm, 0, seed_score);
}

/* ----------------------------- Worker thread ----------------------------- */

static void *
worker_thread(void *arg)
{
    heap_t *frontier = (heap_t *)arg;
    CURL *curl = init_curl_thread();
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, ""); /* enable gzip/deflate */
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, 120L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, 60L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

    while (!g_stop) {
        crawl_item_t it;
        if (!ts_heap_pop_max(frontier, &it)) break;

        /* check crawled flag (threadsafe) */
        int already = 0;
        pthread_mutex_lock(&g_db_lock);
        {
            sqlite3_stmt *stmt = NULL;
            const char *sql = "SELECT crawled FROM urls WHERE normalized = ? LIMIT 1;";
            if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) == SQLITE_OK) {
                sqlite3_bind_text(stmt, 1, it.norm, -1, SQLITE_TRANSIENT);
                if (sqlite3_step(stmt) == SQLITE_ROW)
                    already = sqlite3_column_int(stmt, 0);
            }
            sqlite3_finalize(stmt);
        }
        pthread_mutex_unlock(&g_db_lock);
        if (already) {
            free(it.url);
            free(it.norm);
            continue;
        }

        dynbuf_t body;
        fetch_meta_t meta;
        int ok = fetch_url(curl, it.url, &body, &meta);
        if (!ok) {
            pthread_mutex_lock(&g_db_lock);
            db_mark_crawled_with_hash(it.norm, "");
            pthread_mutex_unlock(&g_db_lock);
            dbuf_free(&body);
            free(it.url);
            free(it.norm);
            sleep_polite();
            continue;
        }

        char *hash_hex = sha256_hex((unsigned char *)body.data, body.len);
        if (hash_hex == NULL) {
            pthread_mutex_lock(&g_db_lock);
            db_mark_crawled_with_hash(it.norm, "");
            pthread_mutex_unlock(&g_db_lock);
            dbuf_free(&body);
            free(it.url);
            free(it.norm);
            sleep_polite();
            continue;
        }

        int dup;
        pthread_mutex_lock(&g_db_lock);
        dup = db_has_content_hash(hash_hex);
        pthread_mutex_unlock(&g_db_lock);
        if (dup) {
            pthread_mutex_lock(&g_db_lock);
            db_mark_crawled_with_hash(it.norm, hash_hex);
            pthread_mutex_unlock(&g_db_lock);
            free(hash_hex);
            dbuf_free(&body);
            free(it.url);
            free(it.norm);
            sleep_polite();
            continue;
        }

        /* Log newly crawled page with ranking (score) */
        log_info("[CRAWLED] score=%.3f url=%s", it.score, it.url);

        /* parse and extract using libxml2 */
        parse_html_and_extract(body.data, body.len, it.url, it.depth, frontier, it.url);

        pthread_mutex_lock(&g_db_lock);
        db_mark_crawled_with_hash(it.norm, hash_hex);
        pthread_mutex_unlock(&g_db_lock);

        free(hash_hex);
        dbuf_free(&body);
        free(it.url);
        free(it.norm);

        sleep_polite();
    }

    curl_easy_cleanup(curl);
    return NULL;
}

/* ----------------------------- Crawl coordinator ----------------------------- */

static void
crawl_loop_parallel(heap_t *frontier, int max_pages)
{
    pthread_t *workers = malloc(sizeof(pthread_t) * g_workers);
    if (!workers) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < g_workers; ++i) {
        if (pthread_create(&workers[i], NULL, worker_thread, frontier) != 0) {
            fprintf(stderr, "Failed to create worker %d\n", i);
        }
    }

    /* Monitor progress and stop when enough pages crawled */
    while (1) {
        int crawled = db_count_crawled();
        if (crawled >= max_pages)
            break;
        sleep(1);
    }

    /* signal stop and wake workers */
    pthread_mutex_lock(&g_frontier_lock);
    g_stop = 1;
    pthread_cond_broadcast(&g_frontier_cond);
    pthread_mutex_unlock(&g_frontier_lock);

    for (int i = 0; i < g_workers; ++i)
        pthread_join(workers[i], NULL);

    free(workers);
}

/* ----------------------------- Main ----------------------------- */

int
main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <seed-url> [max-pages] [max-depth] [workers]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *seed = argv[1];
    int max_pages = (argc >= 3) ? atoi(argv[2]) : DEFAULT_MAX_PAGES;
    g_max_depth = (argc >= 4) ? atoi(argv[3]) : DEFAULT_MAX_DEPTH;
    g_workers = (argc >= 5) ? atoi(argv[4]) : DEFAULT_WORKERS;
    if (g_workers < 1) g_workers = 1;

    srand((unsigned int)time(NULL));

    print_header(seed, max_pages, g_max_depth);

    /* init DB */
    init_db(DB_FILENAME);

    /* init libxml2 and curl global */
    xmlInitParser();
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0) {
        fprintf(stderr, "curl_global_init failed\n");
        return EXIT_FAILURE;
    }

    /* frontier */
    heap_t frontier;
    heap_init(&frontier);

    /* enqueue seed */
    enqueue_seed(&frontier, seed);

    /* start crawl */
    crawl_loop_parallel(&frontier, max_pages);

    /* cleanup */
    heap_free(&frontier);
    db_close();
    curl_global_cleanup();
    xmlCleanupParser();

    printf("\nCrawl finished. Crawled pages: %d\n", db_count_crawled());
    return EXIT_SUCCESS;
}

