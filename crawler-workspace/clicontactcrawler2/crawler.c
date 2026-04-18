/*
 * crawler.c
 *
 * Multithreaded (16 threads) BSD-style CLI crawler, single-file.
 *
 * Features:
 *  - libcurl fetch (text/html only)
 *  - SHA-256 content hashing (OpenSSL libcrypto)
 *  - sqlite3 persistence (urls, emails, phones, content_hash)
 *  - priority frontier (max-heap) with shared access across threads
 *  - duplicate-content detection (skip and randomize frontier)
 *  - extraction of emails and phone numbers (regex)
 *  - thread-safe logging of every newly discovered email and phone to stdout and crawler.log
 *
 * Build:
 *   gcc -std=c11 -O2 -o crawler crawler.c -lcurl -lsqlite3 -lcrypto -pthread
 *
 * Usage:
 *   ./crawler <seed-url> [max-pages] [max-depth]
 *
 * Notes:
 *  - This is a pragmatic implementation intended for extension.
 *  - For production use, consider robust HTML parsing (libxml2), robots.txt, per-host politeness, and thread-local DB connections.
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

/* ----------------------------- Configuration ----------------------------- */

#define DEFAULT_MAX_PAGES 200
#define DEFAULT_MAX_DEPTH 3
#define USER_AGENT "AddressCrawlerCLI/1.0"
#define DB_FILENAME "crawler.db"
#define LOG_FILENAME "crawler.log"
#define POLITE_DELAY_MS 150
#define NUM_THREADS 16

/* ----------------------------- Globals ----------------------------- */

static sqlite3 *g_db = NULL;
static int g_max_depth = DEFAULT_MAX_DEPTH;
static int g_max_pages = DEFAULT_MAX_PAGES;

/* frontier heap and synchronization */
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

static heap_t g_frontier;
static pthread_mutex_t g_frontier_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_frontier_cond = PTHREAD_COND_INITIALIZER;

/* DB mutex and counters */
static pthread_mutex_t g_db_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_count_mutex = PTHREAD_MUTEX_INITIALIZER;
static int g_pages_crawled = 0;

/* logging */
static FILE *g_log_file = NULL;
static pthread_mutex_t g_log_mutex = PTHREAD_MUTEX_INITIALIZER;

/* ----------------------------- Small helpers ----------------------------- */

static char *strdup_safe(const char *s)
{
    if (s == NULL) return NULL;
    char *r = strdup(s);
    if (r == NULL) { perror("strdup"); exit(EXIT_FAILURE); }
    return r;
}

static void sleep_polite(void) { usleep(POLITE_DELAY_MS * 1000); }

/* ----------------------------- Dynamic buffer for curl ----------------------------- */

typedef struct dynbuf { char *data; size_t len; } dynbuf_t;

static void dbuf_init(dynbuf_t *b) { b->data = NULL; b->len = 0; }
static void dbuf_free(dynbuf_t *b) { free(b->data); b->data = NULL; b->len = 0; }

static size_t dbuf_write(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t add = size * nmemb;
    dynbuf_t *b = (dynbuf_t*)userdata;
    char *n = realloc(b->data, b->len + add + 1);
    if (!n) return 0;
    memcpy(n + b->len, ptr, add);
    b->data = n;
    b->len += add;
    b->data[b->len] = '\0';
    return add;
}

/* ----------------------------- SHA-256 helpers ----------------------------- */

static char *sha256_hex(const unsigned char *data, size_t len)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX ctx;
    if (SHA256_Init(&ctx) != 1) return NULL;
    if (SHA256_Update(&ctx, data, len) != 1) return NULL;
    if (SHA256_Final(hash, &ctx) != 1) return NULL;
    char *hex = malloc(SHA256_DIGEST_LENGTH * 2 + 1);
    if (!hex) return NULL;
    for (size_t i = 0; i < SHA256_DIGEST_LENGTH; ++i) sprintf(hex + (i*2), "%02x", hash[i]);
    hex[SHA256_DIGEST_LENGTH*2] = '\0';
    return hex;
}

/* ----------------------------- URL utilities ----------------------------- */

static char *normalize_url(const char *url)
{
    if (!url) return NULL;
    char *copy = strdup_safe(url);
    char *p = strchr(copy, '#'); if (p) *p = '\0';
    size_t len = strlen(copy);
    while (len > 1 && copy[len-1] == '/') { copy[len-1] = '\0'; len--; }
    return copy;
}

static char *url_host(const char *url)
{
    if (!url) return NULL;
    const char *p = strstr(url, "://");
    const char *h = p ? p + 3 : url;
    const char *end = strchr(h, '/');
    size_t len = end ? (size_t)(end - h) : strlen(h);
    char *host = malloc(len + 1);
    if (!host) { perror("malloc"); exit(EXIT_FAILURE); }
    memcpy(host, h, len); host[len] = '\0';
    char *colon = strchr(host, ':'); if (colon) *colon = '\0';
    for (char *c = host; *c; ++c) *c = (char)tolower((unsigned char)*c);
    return host;
}

static double domain_trust(const char *url)
{
    char *host = url_host(url);
    double score = 0.5;
    if (!host) return score;
    size_t L = strlen(host);
    if (L >= 4 && strcmp(host + L - 4, ".gov") == 0) score = 5.0;
    else if (L >= 4 && strcmp(host + L - 4, ".edu") == 0) score = 4.0;
    else if (L >= 4 && strcmp(host + L - 4, ".org") == 0) score = 2.0;
    else if (L >= 4 && strcmp(host + L - 4, ".com") == 0) score = 1.5;
    free(host);
    return score;
}

static char *resolve_url(const char *base, const char *href)
{
    if (!href || href[0] == '\0') return NULL;
    if (strncasecmp(href, "http://", 7) == 0 || strncasecmp(href, "https://", 8) == 0)
        return strdup_safe(href);
    const char *p = strstr(base, "://");
    if (!p) return strdup_safe(href);
    const char *hoststart = p + 3;
    const char *pathstart = strchr(hoststart, '/');
    size_t hostlen = pathstart ? (size_t)(pathstart - base) : strlen(base);
    char *root = malloc(hostlen + 1);
    if (!root) { perror("malloc"); exit(EXIT_FAILURE); }
    memcpy(root, base, hostlen); root[hostlen] = '\0';
    char *out = NULL;
    if (href[0] == '/') { if (asprintf(&out, "%s%s", root, href) < 0) out = NULL; }
    else {
        const char *lastslash = pathstart ? strrchr(base, '/') : NULL;
        if (lastslash) {
            size_t prefixlen = (size_t)(lastslash - base) + 1;
            char *prefix = malloc(prefixlen + 1);
            if (!prefix) { perror("malloc"); exit(EXIT_FAILURE); }
            memcpy(prefix, base, prefixlen); prefix[prefixlen] = '\0';
            if (asprintf(&out, "%s%s", prefix, href) < 0) out = NULL;
            free(prefix);
        } else { if (asprintf(&out, "%s/%s", root, href) < 0) out = NULL; }
    }
    free(root);
    return out;
}

/* ----------------------------- Heap (shared frontier) ----------------------------- */

static void heap_init(heap_t *h) { h->items = NULL; h->size = 0; h->cap = 0; }

static void heap_free(heap_t *h)
{
    for (size_t i = 0; i < h->size; ++i) { free(h->items[i].url); free(h->items[i].norm); }
    free(h->items); h->items = NULL; h->size = h->cap = 0;
}

static void heap_swap(crawl_item_t *a, crawl_item_t *b) { crawl_item_t t = *a; *a = *b; *b = t; }

static void heap_push_locked(heap_t *h, crawl_item_t it)
{
    if (h->size + 1 > h->cap) {
        size_t ncap = h->cap ? h->cap * 2 : 128;
        crawl_item_t *n = realloc(h->items, ncap * sizeof(crawl_item_t));
        if (!n) { perror("realloc"); exit(EXIT_FAILURE); }
        h->items = n; h->cap = ncap;
    }
    size_t i = h->size++;
    h->items[i] = it;
    while (i > 0) {
        size_t p = (i - 1) / 2;
        if (h->items[p].score >= h->items[i].score) break;
        heap_swap(&h->items[p], &h->items[i]);
        i = p;
    }
}

static int heap_empty_locked(const heap_t *h) { return h->size == 0; }

static crawl_item_t heap_pop_max_locked(heap_t *h)
{
    if (h->size == 0) { fprintf(stderr, "heap_pop_max_locked: empty\n"); exit(EXIT_FAILURE); }
    crawl_item_t out = h->items[0];
    h->items[0] = h->items[--h->size];
    size_t i = 0;
    while (1) {
        size_t l = 2*i + 1, r = 2*i + 2, largest = i;
        if (l < h->size && h->items[l].score > h->items[largest].score) largest = l;
        if (r < h->size && h->items[r].score > h->items[largest].score) largest = r;
        if (largest == i) break;
        heap_swap(&h->items[i], &h->items[largest]);
        i = largest;
    }
    return out;
}

static crawl_item_t heap_pop_random_locked(heap_t *h)
{
    if (h->size == 0) { fprintf(stderr, "heap_pop_random_locked: empty\n"); exit(EXIT_FAILURE); }
    size_t idx = (size_t)rand() % h->size;
    crawl_item_t out = h->items[idx];
    h->items[idx] = h->items[--h->size];
    size_t i = idx;
    while (1) {
        size_t l = 2*i + 1, r = 2*i + 2, largest = i;
        if (l < h->size && h->items[l].score > h->items[largest].score) largest = l;
        if (r < h->size && h->items[r].score > h->items[largest].score) largest = r;
        if (largest == i) break;
        heap_swap(&h->items[i], &h->items[largest]);
        i = largest;
    }
    while (i > 0) {
        size_t p = (i - 1) / 2;
        if (h->items[p].score >= h->items[i].score) break;
        heap_swap(&h->items[p], &h->items[i]);
        i = p;
    }
    return out;
}

/* ----------------------------- SQLite helpers (protected by g_db_mutex) ----------------------------- */

static void db_open(const char *filename)
{
    if (sqlite3_open(filename, &g_db) != SQLITE_OK) { fprintf(stderr, "Cannot open DB %s: %s\n", filename, sqlite3_errmsg(g_db)); exit(EXIT_FAILURE); }
    const char *sql =
        "PRAGMA journal_mode=WAL;"
        "CREATE TABLE IF NOT EXISTS urls ( id INTEGER PRIMARY KEY, url TEXT UNIQUE, normalized TEXT UNIQUE, depth INTEGER, score REAL, inbound INTEGER DEFAULT 0, crawled INTEGER DEFAULT 0, content_hash TEXT );"
        "CREATE TABLE IF NOT EXISTS emails (id INTEGER PRIMARY KEY, email TEXT UNIQUE, source_url TEXT);"
        "CREATE TABLE IF NOT EXISTS phones (id INTEGER PRIMARY KEY, phone TEXT UNIQUE, source_url TEXT);";
    char *err = NULL;
    if (sqlite3_exec(g_db, sql, NULL, NULL, &err) != SQLITE_OK) { fprintf(stderr, "DB init error: %s\n", err); sqlite3_free(err); exit(EXIT_FAILURE); }
}

static void db_close(void) { if (g_db) sqlite3_close(g_db); }

static void db_insert_url_locked(const char *url, const char *norm, int depth, double score)
{
    pthread_mutex_lock(&g_db_mutex);
    sqlite3_stmt *stmt = NULL;
    const char *sql = "INSERT OR IGNORE INTO urls (url, normalized, depth, score) VALUES (?, ?, ?, ?);";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, url, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, norm, -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, depth);
        sqlite3_bind_double(stmt, 4, score);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&g_db_mutex);
}

static void db_mark_crawled_with_hash_locked(const char *norm, const char *hash_hex)
{
    pthread_mutex_lock(&g_db_mutex);
    sqlite3_stmt *stmt = NULL;
    const char *sql = "UPDATE urls SET crawled = 1, content_hash = ? WHERE normalized = ?;";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, hash_hex, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, norm, -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&g_db_mutex);
}

static int db_has_norm_locked(const char *norm)
{
    int found = 0;
    pthread_mutex_lock(&g_db_mutex);
    sqlite3_stmt *stmt = NULL;
    const char *sql = "SELECT 1 FROM urls WHERE normalized = ? LIMIT 1;";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, norm, -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) found = 1;
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&g_db_mutex);
    return found;
}

static int db_has_content_hash_locked(const char *hash_hex)
{
    int found = 0;
    pthread_mutex_lock(&g_db_mutex);
    sqlite3_stmt *stmt = NULL;
    const char *sql = "SELECT 1 FROM urls WHERE content_hash = ? LIMIT 1;";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, hash_hex, -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) found = 1;
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&g_db_mutex);
    return found;
}

/* logging helpers */

static void init_log(const char *path)
{
    pthread_mutex_lock(&g_log_mutex);
    if (g_log_file == NULL) {
        g_log_file = fopen(path, "a");
        if (g_log_file == NULL) {
            fprintf(stderr, "Warning: cannot open log file %s\n", path);
        }
    }
    pthread_mutex_unlock(&g_log_mutex);
}

static void close_log(void)
{
    pthread_mutex_lock(&g_log_mutex);
    if (g_log_file) { fclose(g_log_file); g_log_file = NULL; }
    pthread_mutex_unlock(&g_log_mutex);
}

static void log_new_contact(const char *type, const char *value, const char *source)
{
    time_t now = time(NULL);
    struct tm tm;
    localtime_r(&now, &tm);
    char timestr[64];
    strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", &tm);

    pthread_mutex_lock(&g_log_mutex);
    printf("\x1b[1;36m[NEW %s]\x1b[0m %s  (source: %s)  %s\n", type, value ? value : "", source ? source : "", timestr);
    if (g_log_file) {
        fprintf(g_log_file, "%s\t%s\t%s\t%s\n", timestr, type, value ? value : "", source ? source : "");
        fflush(g_log_file);
    }
    pthread_mutex_unlock(&g_log_mutex);
}

/* insert email/phone with logging when new */
static void db_insert_email_locked(const char *email, const char *source)
{
    pthread_mutex_lock(&g_db_mutex);
    sqlite3_stmt *stmt = NULL;
    const char *sql = "INSERT OR IGNORE INTO emails (email, source_url) VALUES (?, ?);";
    int inserted = 0;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, email, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, source, -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        inserted = sqlite3_changes(g_db);
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&g_db_mutex);
    if (inserted > 0) log_new_contact("EMAIL", email, source);
}

static void db_insert_phone_locked(const char *phone, const char *source)
{
    pthread_mutex_lock(&g_db_mutex);
    sqlite3_stmt *stmt = NULL;
    const char *sql = "INSERT OR IGNORE INTO phones (phone, source_url) VALUES (?, ?);";
    int inserted = 0;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, phone, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, source, -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        inserted = sqlite3_changes(g_db);
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&g_db_mutex);
    if (inserted > 0) log_new_contact("PHONE", phone, source);
}

static int db_count_locked(const char *table)
{
    int count = 0;
    pthread_mutex_lock(&g_db_mutex);
    sqlite3_stmt *stmt = NULL;
    char *sql = NULL;
    if (asprintf(&sql, "SELECT COUNT(*) FROM %s;", table) < 0) { pthread_mutex_unlock(&g_db_mutex); return 0; }
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    free(sql);
    pthread_mutex_unlock(&g_db_mutex);
    return count;
}

/* ----------------------------- Extraction helpers ----------------------------- */

static char *strip_tags(const char *html)
{
    if (!html) return strdup_safe("");
    size_t n = strlen(html);
    char *out = malloc(n + 1);
    if (!out) { perror("malloc"); exit(EXIT_FAILURE); }
    size_t oi = 0; int in_tag = 0;
    for (size_t i = 0; i < n; ++i) {
        char c = html[i];
        if (c == '<') { in_tag = 1; continue; }
        if (c == '>') { in_tag = 0; continue; }
        if (!in_tag) out[oi++] = c;
    }
    out[oi] = '\0';
    return out;
}

static char *normalize_phone(const char *raw)
{
    if (!raw) return NULL;
    size_t L = strlen(raw);
    char *out = malloc(L + 2);
    if (!out) { perror("malloc"); exit(EXIT_FAILURE); }
    size_t oi = 0;
    for (size_t i = 0; i < L; ++i) {
        char c = raw[i];
        if (c == '+' && oi == 0) out[oi++] = c;
        else if (isdigit((unsigned char)c)) out[oi++] = c;
    }
    out[oi] = '\0';
    if (oi == 0) { free(out); return NULL; }
    return out;
}

static void extract_emails_locked(const char *text, const char *source)
{
    regex_t re;
    const char *pattern = "([A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,})";
    if (regcomp(&re, pattern, REG_EXTENDED | REG_ICASE) != 0) return;
    const char *p = text; regmatch_t m[2];
    while (regexec(&re, p, 2, m, 0) == 0) {
        int start = m[1].rm_so, end = m[1].rm_eo;
        size_t len = (size_t)(end - start);
        char *email = malloc(len + 1);
        if (!email) break;
        memcpy(email, p + start, len); email[len] = '\0';
        for (char *c = email; *c; ++c) *c = (char)tolower((unsigned char)*c);
        db_insert_email_locked(email, source);
        free(email);
        p += end;
    }
    regfree(&re);
}

static void extract_phones_locked(const char *text, const char *source)
{
    regex_t re;
    const char *pattern = "(\\+?[0-9][0-9\\-\\.\\s\\(\\)]{6,}[0-9])";
    if (regcomp(&re, pattern, REG_EXTENDED | REG_ICASE) != 0) return;
    const char *p = text; regmatch_t m[2];
    while (regexec(&re, p, 2, m, 0) == 0) {
        int start = m[1].rm_so, end = m[1].rm_eo;
        size_t len = (size_t)(end - start);
        char *raw = malloc(len + 1);
        if (!raw) break;
        memcpy(raw, p + start, len); raw[len] = '\0';
        char *norm = normalize_phone(raw);
        if (norm) { db_insert_phone_locked(norm, source); free(norm); }
        free(raw);
        p += end;
    }
    regfree(&re);
}

/* ----------------------------- Link extraction and enqueue ----------------------------- */

static void process_href_locked(const char *href, const char *base, int base_depth)
{
    if (!href || href[0] == '\0') return;
    if (strncasecmp(href, "mailto:", 7) == 0) {
        char *after = strdup_safe(href + 7);
        char *q = strchr(after, '?'); if (q) *q = '\0';
        char *tok = strtok(after, ",");
        while (tok) { char *email = strdup_safe(tok); for (char *c = email; *c; ++c) *c = (char)tolower((unsigned char)*c); db_insert_email_locked(email, base); free(email); tok = strtok(NULL, ","); }
        free(after); return;
    }
    if (strncasecmp(href, "tel:", 4) == 0) {
        char *after = strdup_safe(href + 4);
        char *q = strchr(after, '?'); if (q) *q = '\0';
        char *norm = normalize_phone(after);
        if (norm) { db_insert_phone_locked(norm, base); free(norm); }
        free(after); return;
    }
    char *resolved = resolve_url(base, href);
    if (!resolved) return;
    char *norm = normalize_url(resolved);
    if (!norm) { free(resolved); return; }
    pthread_mutex_lock(&g_frontier_mutex);
    int seen = db_has_norm_locked(norm);
    if (!seen && base_depth + 1 <= g_max_depth) {
        double dt = domain_trust(resolved);
        int inbound = 0;
        int pathlen = (int)strlen(resolved);
        double score = 2.0 * dt + (-1.0) * (base_depth + 1) + 1.0 * inbound + (-0.5) * (pathlen / 10.0);
        crawl_item_t it; it.url = strdup_safe(resolved); it.norm = norm; it.depth = base_depth + 1; it.score = score; it.inbound = inbound;
        heap_push_locked(&g_frontier, it);
        db_insert_url_locked(resolved, it.norm, it.depth, it.score);
        pthread_cond_broadcast(&g_frontier_cond);
    } else free(norm);
    pthread_mutex_unlock(&g_frontier_mutex);
    free(resolved);
}

static void extract_links_and_enqueue_locked(const char *html, const char *base, int base_depth)
{
    regex_t re;
    const char *pattern = "href[[:space:]]*=[[:space:]]*\"([^\"]+)\"";
    if (regcomp(&re, pattern, REG_EXTENDED | REG_ICASE) != 0) return;
    const char *p = html; regmatch_t m[2];
    while (regexec(&re, p, 2, m, 0) == 0) {
        int start = m[1].rm_so, end = m[1].rm_eo;
        size_t len = (size_t)(end - start);
        char *href = malloc(len + 1);
        if (!href) break;
        memcpy(href, p + start, len); href[len] = '\0';
        process_href_locked(href, base, base_depth);
        free(href);
        p += end;
    }
    regfree(&re);
}

/* ----------------------------- Curl fetch with header callback ----------------------------- */

typedef struct fetch_meta { char content_type[128]; long response_code; } fetch_meta_t;

static size_t header_cb(char *buffer, size_t size, size_t nitems, void *userdata)
{
    size_t len = size * nitems;
    fetch_meta_t *m = (fetch_meta_t*)userdata;
    if (len > 13 && strncasecmp(buffer, "Content-Type:", 13) == 0) {
        const char *p = buffer + 13; while (*p && isspace((unsigned char)*p)) p++;
        size_t copylen = len - (p - buffer); if (copylen >= sizeof(m->content_type)) copylen = sizeof(m->content_type)-1;
        memcpy(m->content_type, p, copylen); m->content_type[copylen] = '\0';
    }
    return len;
}

static int fetch_url(CURL *curl, const char *url, dynbuf_t *out, fetch_meta_t *meta)
{
    dbuf_init(out); meta->content_type[0] = '\0'; meta->response_code = 0;
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
    if (rc != CURLE_OK) { dbuf_free(out); return 0; }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &meta->response_code);
    if (meta->content_type[0] && strstr(meta->content_type, "text/html") == NULL) { dbuf_free(out); return 0; }
    return 1;
}

/* ----------------------------- Worker thread ----------------------------- */

static void *worker_thread(void *arg)
{
    (void)arg;
    CURL *curl = curl_easy_init();
    if (!curl) { fprintf(stderr, "worker: curl init failed\n"); return NULL; }

    while (1) {
        pthread_mutex_lock(&g_frontier_mutex);
        while (g_frontier.size == 0 && g_pages_crawled < g_max_pages) pthread_cond_wait(&g_frontier_cond, &g_frontier_mutex);
        if (g_frontier.size == 0 && g_pages_crawled >= g_max_pages) { pthread_mutex_unlock(&g_frontier_mutex); break; }
        crawl_item_t it = heap_pop_max_locked(&g_frontier);
        pthread_mutex_unlock(&g_frontier_mutex);

        /* skip if already crawled */
        pthread_mutex_lock(&g_db_mutex);
        int already = 0;
        sqlite3_stmt *stmt = NULL;
        const char *sql = "SELECT crawled FROM urls WHERE normalized = ? LIMIT 1;";
        if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, it.norm, -1, SQLITE_TRANSIENT);
            if (sqlite3_step(stmt) == SQLITE_ROW) already = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
        pthread_mutex_unlock(&g_db_mutex);
        if (already) { free(it.url); free(it.norm); continue; }

        /* fetch */
        dynbuf_t body; fetch_meta_t meta;
        int ok = fetch_url(curl, it.url, &body, &meta);
        if (!ok) {
            db_mark_crawled_with_hash_locked(it.norm, "");
            pthread_mutex_lock(&g_count_mutex); g_pages_crawled++; pthread_mutex_unlock(&g_count_mutex);
            free(it.url); free(it.norm);
            sleep_polite();
            continue;
        }

        char *hash_hex = sha256_hex((unsigned char*)body.data, body.len);
        if (!hash_hex) {
            db_mark_crawled_with_hash_locked(it.norm, "");
            pthread_mutex_lock(&g_count_mutex); g_pages_crawled++; pthread_mutex_unlock(&g_count_mutex);
            dbuf_free(&body); free(it.url); free(it.norm);
            sleep_polite();
            continue;
        }

        if (db_has_content_hash_locked(hash_hex)) {
            db_mark_crawled_with_hash_locked(it.norm, hash_hex);
            pthread_mutex_lock(&g_count_mutex); g_pages_crawled++; pthread_mutex_unlock(&g_count_mutex);
            dbuf_free(&body); free(it.url); free(it.norm); free(hash_hex);
            pthread_mutex_lock(&g_frontier_mutex);
            if (g_frontier.size > 0) {
                crawl_item_t rnd = heap_pop_random_locked(&g_frontier);
                heap_push_locked(&g_frontier, rnd);
            }
            pthread_mutex_unlock(&g_frontier_mutex);
            sleep_polite();
            continue;
        }

        /* process text */
        char *text = strip_tags(body.data ? body.data : "");
        regex_t r_at, r_dot;
        regcomp(&r_at, "\\[at\\]|\\(at\\)|\\s+at\\s+", REG_ICASE | REG_EXTENDED);
        regcomp(&r_dot, "\\[dot\\]|\\(dot\\)|\\s+dot\\s+", REG_ICASE | REG_EXTENDED);
        { regmatch_t m; const char *p = text; size_t outcap = strlen(text) + 1; char *tmp = malloc(outcap); if (!tmp) { perror("malloc"); exit(EXIT_FAILURE); } tmp[0] = '\0';
          while (regexec(&r_at, p, 1, &m, 0) == 0) { strncat(tmp, p, m.rm_so); strcat(tmp, "@"); p += m.rm_eo; } strcat(tmp, p); free(text); text = tmp; }
        { regmatch_t m; const char *p = text; size_t outcap = strlen(text) + 1; char *tmp = malloc(outcap); if (!tmp) { perror("malloc"); exit(EXIT_FAILURE); } tmp[0] = '\0';
          while (regexec(&r_dot, p, 1, &m, 0) == 0) { strncat(tmp, p, m.rm_so); strcat(tmp, "."); p += m.rm_eo; } strcat(tmp, p); free(text); text = tmp; }
        regfree(&r_at); regfree(&r_dot);

        extract_emails_locked(text, it.url);
        extract_phones_locked(text, it.url);
        extract_links_and_enqueue_locked(body.data ? body.data : "", it.url, it.depth);

        db_mark_crawled_with_hash_locked(it.norm, hash_hex);

        pthread_mutex_lock(&g_count_mutex); g_pages_crawled++; pthread_mutex_unlock(&g_count_mutex);

        dbuf_free(&body);
        free(text);
        free(hash_hex);
        free(it.url);
        free(it.norm);

        sleep_polite();
    }

    curl_easy_cleanup(curl);
    return NULL;
}

/* ----------------------------- Main & helpers ----------------------------- */

static void print_header(const char *seed, int max_pages, int max_depth)
{
    printf("\x1b[1;34m===========================================\x1b[0m\n");
    printf("\x1b[1;32m  AddressCrawler CLI — Priority Email & Phone Crawler (multithreaded)\x1b[0m\n");
    printf("  Seed: %s    Depth limit: %d   Max pages: %d   Threads: %d\n", seed, max_depth, max_pages, NUM_THREADS);
    printf("\x1b[1;34m===========================================\x1b[0m\n\n");
}

static void enqueue_seed_locked(const char *seed)
{
    char *seed_norm = normalize_url(seed);
    double seed_score = 10.0 * domain_trust(seed);
    crawl_item_t seed_item;
    seed_item.url = strdup_safe(seed);
    seed_item.norm = seed_norm;
    seed_item.depth = 0;
    seed_item.score = seed_score;
    seed_item.inbound = 0;
    pthread_mutex_lock(&g_frontier_mutex);
    heap_push_locked(&g_frontier, seed_item);
    pthread_cond_broadcast(&g_frontier_cond);
    pthread_mutex_unlock(&g_frontier_mutex);
    db_insert_url_locked(seed, seed_norm, 0, seed_score);
}

/* print final results */
static void print_results(const char *dbfile)
{
    printf("\n\n\x1b[1;34mCrawl finished\x1b[0m\n");
    printf("Pages crawled: %d\n", g_pages_crawled);
    printf("Unique URLs in DB: %d\n", db_count_locked("urls"));
    printf("Emails found: %d\n", db_count_locked("emails"));
    printf("Phones found: %d\n", db_count_locked("phones"));
    printf("DB file: %s\n\n", dbfile);

    printf("\x1b[1;33mEmails found:\x1b[0m\n");
    pthread_mutex_lock(&g_db_mutex);
    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(g_db, "SELECT email, source_url FROM emails ORDER BY id;", -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char *email = sqlite3_column_text(stmt, 0);
            const unsigned char *src = sqlite3_column_text(stmt, 1);
            printf("  %-40s  (%s)\n", email ? (const char*)email : "", src ? (const char*)src : "");
        }
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&g_db_mutex);

    printf("\n\x1b[1;33mPhones found:\x1b[0m\n");
    pthread_mutex_lock(&g_db_mutex);
    if (sqlite3_prepare_v2(g_db, "SELECT phone, source_url FROM phones ORDER BY id;", -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char *phone = sqlite3_column_text(stmt, 0);
            const unsigned char *src = sqlite3_column_text(stmt, 1);
            printf("  %-20s  (%s)\n", phone ? (const char*)phone : "", src ? (const char*)src : "");
        }
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&g_db_mutex);
}

/* ----------------------------- main ----------------------------- */

int main(int argc, char **argv)
{
    if (argc < 2) { fprintf(stderr, "Usage: %s <seed-url> [max-pages] [max-depth]\n", argv[0]); return EXIT_FAILURE; }
    const char *seed = argv[1];
    g_max_pages = (argc >= 3) ? atoi(argv[2]) : DEFAULT_MAX_PAGES;
    g_max_depth = (argc >= 4) ? atoi(argv[3]) : DEFAULT_MAX_DEPTH;

    srand((unsigned int)time(NULL));
    db_open(DB_FILENAME);
    init_log(LOG_FILENAME);

    if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0) { fprintf(stderr, "curl_global_init failed\n"); db_close(); close_log(); return EXIT_FAILURE; }

    heap_init(&g_frontier);
    print_header(seed, g_max_pages, g_max_depth);
    enqueue_seed_locked(seed);

    /* create worker threads */
    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        if (pthread_create(&threads[i], NULL, worker_thread, NULL) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    /* wait for workers */
    for (int i = 0; i < NUM_THREADS; ++i) pthread_join(threads[i], NULL);

    print_results(DB_FILENAME);

    heap_free(&g_frontier);
    curl_global_cleanup();
    close_log();
    db_close();
    return EXIT_SUCCESS;
}

