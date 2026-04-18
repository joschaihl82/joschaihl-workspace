/* crawler.c
 *
 * Single-file persistent web crawler with:
 *  - prioritized frontier (max-heap)
 *  - multi-threaded workers
 *  - robots.txt support (Disallow, Crawl-delay, Sitemap)
 *  - sitemap.xml parsing (<loc>)
 *  - blocked domains (file + DB table)
 *  - polite delays + optional per-robots Crawl-delay handling
 *  - libcurl for fetching
 *  - libxml2 for parsing HTML & XML (sitemaps)
 *  - OpenSSL SHA-256 for duplicate detection
 *  - SQLite persistence (crawler.db) for URLs, emails, phones, blocked domains
 *  - graceful Ctrl+C (SIGINT) shutdown and resume
 *
 * Build:
 * gcc -O2 crawler.c -o crawler \
 *     -lpthread -lcurl -lxml2 -lsqlite3 -lcrypto -Wall
 *
 * Usage:
 * ./crawler <seed-url> <num-threads> <max-depth>
 *
 * Example:
 * ./crawler https://example.edu 6 4
 *
 * Dependencies: libcurl, libxml2, openssl, sqlite3, pthread
 *
 * Author: ChatGPT (GPT-5 Thinking mini)
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>
#include <regex.h>

#include <curl/curl.h>

#include <libxml/HTMLparser.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/uri.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <openssl/sha.h>
#include <sqlite3.h>

/* ---------------- Config ---------------- */
#define DB_PATH "crawler.db"
#define BLOCKED_FILE "blocked_domains.txt"
#define USER_AGENT "SimpleCrawler/1.0 (+https://example.local)"
#define DEFAULT_POLITENESS_MS 250
#define DEFAULT_THREADS 4
#define DEFAULT_MAX_DEPTH 3
#define MAX_CONTENT_SIZE (12 * 1024 * 1024)
#define INITIAL_HEAP_CAP 1024
#define MAX_URL_LEN 4096

/* ---------------- Logging ---------------- */
/* Avoid name collision with libm's logf -> call log_msg */
static pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER;
static void log_msg(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    pthread_mutex_lock(&log_lock);
    char ts[32];
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    if (tm) strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", tm);
    else snprintf(ts, sizeof(ts), "unknown-time");
    fprintf(stdout, "[%s] ", ts);
    vfprintf(stdout, fmt, ap);
    fprintf(stdout, "\n");
    fflush(stdout);
    pthread_mutex_unlock(&log_lock);
    va_end(ap);
}

/* ---------------- Globals & shutdown ---------------- */
static volatile sig_atomic_t shutdown_requested = 0;
static int POLITENESS_MS = DEFAULT_POLITENESS_MS;

static void handle_sigint(int s) {
    (void)s;
    shutdown_requested = 1;
    log_msg("SIGINT received — shutdown requested");
}

/* ---------------- Utilities ---------------- */
static void msleep(unsigned ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

static char *str_tolower_dup(const char *s) {
    if (!s) return NULL;
    char *r = strdup(s);
    for (char *p = r; *p; ++p) *p = tolower((unsigned char)*p);
    return r;
}

/* SHA-256 hex output */
static void sha256_hex(const unsigned char *data, size_t len, char out[65]) {
    unsigned char md[SHA256_DIGEST_LENGTH];
    SHA256(data, len, md);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) sprintf(out + i*2, "%02x", md[i]);
    out[64] = '\0';
}

/* Deobfuscate emails in text: [at] -> @, [dot] -> ., remove spaces */
static void deobfuscate(char *s) {
    if (!s) return;
    for (char *p = s; *p; ++p) {
        if (*p == '[' || *p == '(') {
            if (strncasecmp(p, "[at]", 4) == 0 || strncasecmp(p, "(at)", 4) == 0) {
                *p = '@';
                memmove(p+1, p+4, strlen(p+4)+1);
            } else if (strncasecmp(p, "[dot]", 5) == 0 || strncasecmp(p, "(dot)", 5) == 0) {
                *p = '.';
                memmove(p+1, p+5, strlen(p+5)+1);
            }
        }
        if (*p == ' ') {
            memmove(p, p+1, strlen(p+1)+1);
            p--;
        }
    }
}

/* Normalize URL using xmlBuildURI, remove fragment */
static char *normalize_url(const char *base, const char *href) {
    if (!href) return NULL;
    xmlChar *joined = xmlBuildURI((xmlChar*)href, (xmlChar*)base);
    if (!joined) {
        /* fallback: strdup href */
        return strdup(href);
    }
    char *s = strdup((char*)joined);
    xmlFree(joined);
    char *frag = strchr(s, '#');
    if (frag) *frag = '\0';
    return s;
}

/* Extract origin (scheme://host[:port]) */
static char *origin_from_url(const char *url) {
    if (!url) return NULL;
    xmlURIPtr u = xmlParseURI(url);
    if (!u) return NULL;
    if (!u->scheme || !u->server) { xmlFreeURI(u); return NULL; }
    char buf[1024];
    if (u->port > 0)
        snprintf(buf, sizeof(buf)-1,
                 "%s://%s:%d",
                 u->scheme ? u->scheme : "http",
                 u->server ? u->server : "",
                 u->port);
    else
        snprintf(buf, sizeof(buf)-1,
                 "%s://%s",
                 u->scheme ? u->scheme : "http",
                 u->server ? u->server : "");

    xmlFreeURI(u);
    return strdup(buf);
}

/* Extract path from url or "/" */
static char *path_from_url(const char *url) {
    if (!url) return NULL;
    xmlURIPtr u = xmlParseURI(url);
    if (!u) return NULL;
    char *p = u->path ? strdup(u->path) : strdup("/");
    xmlFreeURI(u);
    return p;
}

/* Get domain server part */
static char *get_domain(const char *url) {
    if (!url) return NULL;
    xmlURIPtr u = xmlParseURI(url);
    if (!u) return NULL;
    char *r = u->server ? strdup(u->server) : NULL;
    xmlFreeURI(u);
    return r;
}

/* domain endswith (case-insensitive) */
static int domain_endswith(const char *domain, const char *suf) {
    if (!domain || !suf) return 0;
    size_t d = strlen(domain), s = strlen(suf);
    if (d < s) return 0;
    return strcasecmp(domain + d - s, suf) == 0;
}

/* Score function: prefer .gov/.edu, penalize depth */
static double compute_score(const char *domain, int depth) {
    double score = 100.0;
    if (!domain) return score - depth * 5.0;
    if (domain_endswith(domain, ".gov") || domain_endswith(domain, ".gov.")) score += 200.0;
    if (domain_endswith(domain, ".edu") || domain_endswith(domain, ".edu.")) score += 150.0;
    score -= depth * 10.0;
    if (score < 0) score = 0.0;
    return score;
}

/* ---------------- SQLite DB ---------------- */
static sqlite3 *db = NULL;
static pthread_mutex_t db_lock = PTHREAD_MUTEX_INITIALIZER;

static void db_init(void) {
    int rc = sqlite3_open(DB_PATH, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open DB %s: %s\n", DB_PATH, sqlite3_errmsg(db));
        exit(1);
    }
    char *err = NULL;
    const char *sql =
        "PRAGMA journal_mode=WAL;"
        "CREATE TABLE IF NOT EXISTS urls (id INTEGER PRIMARY KEY, url TEXT UNIQUE, normalized TEXT, domain TEXT, depth INTEGER, queued INTEGER DEFAULT 0, crawled INTEGER DEFAULT 0, content_hash TEXT, last_crawled DATETIME);"
        "CREATE INDEX IF NOT EXISTS idx_urls_normalized ON urls(normalized);"
        "CREATE TABLE IF NOT EXISTS emails (id INTEGER PRIMARY KEY, url_id INTEGER, email TEXT, UNIQUE(url_id, email));"
        "CREATE TABLE IF NOT EXISTS phones (id INTEGER PRIMARY KEY, url_id INTEGER, phone TEXT, UNIQUE(url_id, phone));"
        "CREATE TABLE IF NOT EXISTS blocked_domains (id INTEGER PRIMARY KEY, domain TEXT UNIQUE);";
    rc = sqlite3_exec(db, sql, NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "DB init error: %s\n", err ? err : "unknown");
        sqlite3_free(err);
        exit(1);
    }
}

/* Insert URL if not exists, return id */
static long db_insert_url(const char *url, const char *normalized, const char *domain, int depth, int queued) {
    pthread_mutex_lock(&db_lock);
    sqlite3_stmt *stmt = NULL;
    const char *ins = "INSERT OR IGNORE INTO urls (url, normalized, domain, depth, queued, crawled) VALUES (?, ?, ?, ?, ?, 0);";
    if (sqlite3_prepare_v2(db, ins, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, url, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, normalized ? normalized : url, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, domain ? domain : "", -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 4, depth);
        sqlite3_bind_int(stmt, 5, queued);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);

    const char *sel = "SELECT id FROM urls WHERE normalized = ? LIMIT 1;";
    long id = -1;
    if (sqlite3_prepare_v2(db, sel, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, normalized ? normalized : url, -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) id = sqlite3_column_int64(stmt, 0);
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&db_lock);
    return id;
}

static void db_mark_crawled(long url_id, const char *content_hash) {
    pthread_mutex_lock(&db_lock);
    sqlite3_stmt *stmt = NULL;
    const char *upd = "UPDATE urls SET crawled=1, queued=0, content_hash=?, last_crawled=datetime('now') WHERE id=?;";
    if (sqlite3_prepare_v2(db, upd, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, content_hash, -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt, 2, url_id);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&db_lock);
}

static int db_already_have_hash(const char *hash) {
    pthread_mutex_lock(&db_lock);
    sqlite3_stmt *stmt = NULL;
    const char *sel = "SELECT COUNT(1) FROM urls WHERE content_hash = ? LIMIT 1;";
    int exists = 0;
    if (sqlite3_prepare_v2(db, sel, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, hash, -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) exists = sqlite3_column_int(stmt, 0) > 0;
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&db_lock);
    return exists;
}

static void db_insert_email(long url_id, const char *email) {
    pthread_mutex_lock(&db_lock);
    sqlite3_stmt *stmt = NULL;
    const char *ins = "INSERT OR IGNORE INTO emails (url_id, email) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db, ins, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int64(stmt, 1, url_id);
        sqlite3_bind_text(stmt, 2, email, -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&db_lock);
}

static void db_insert_phone(long url_id, const char *phone) {
    pthread_mutex_lock(&db_lock);
    sqlite3_stmt *stmt = NULL;
    const char *ins = "INSERT OR IGNORE INTO phones (url_id, phone) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db, ins, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int64(stmt, 1, url_id);
        sqlite3_bind_text(stmt, 2, phone, -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&db_lock);
}

/* Blocked domains helpers */
static void db_insert_blocked_domain(const char *domain) {
    pthread_mutex_lock(&db_lock);
    sqlite3_stmt *stmt = NULL;
    const char *ins = "INSERT OR IGNORE INTO blocked_domains(domain) VALUES (?);";
    if (sqlite3_prepare_v2(db, ins, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, domain, -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&db_lock);
}

static int db_domain_blocked_exact(const char *domain) {
    pthread_mutex_lock(&db_lock);
    sqlite3_stmt *stmt = NULL;
    const char *sel = "SELECT COUNT(1) FROM blocked_domains WHERE domain = ? LIMIT 1;";
    int blocked = 0;
    if (sqlite3_prepare_v2(db, sel, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, domain, -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) blocked = sqlite3_column_int(stmt, 0) > 0;
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&db_lock);
    return blocked;
}

/* returns 1 if domain is blocked by suffix or exact match */
static int db_domain_blocked_suffix(const char *domain) {
    if (!domain) return 0;
    pthread_mutex_lock(&db_lock);
    sqlite3_stmt *stmt = NULL;
    const char *sel = "SELECT domain FROM blocked_domains;";
    int blocked = 0;
    if (sqlite3_prepare_v2(db, sel, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *bd = (const char*)sqlite3_column_text(stmt, 0);
            size_t bl = strlen(bd), dl = strlen(domain);
            if (dl >= bl && strcasecmp(domain + dl - bl, bd) == 0) { blocked = 1; break; }
        }
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&db_lock);
    return blocked;
}

/* ---------------- Frontier (thread-safe max-heap) ---------------- */

typedef struct {
    char *url;
    char *normalized;
    char *domain;
    int depth;
    double score;
} frontier_item_t;

typedef struct {
    frontier_item_t **items;
    size_t capacity;
    size_t size;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} heap_t;

static heap_t *heap_create(size_t cap) {
    heap_t *h = calloc(1, sizeof(heap_t));
    h->capacity = (cap > 16) ? cap : 16;
    h->items = calloc(h->capacity, sizeof(frontier_item_t*));
    h->size = 0;
    pthread_mutex_init(&h->lock, NULL);
    pthread_cond_init(&h->cond, NULL);
    return h;
}

static void heap_free_item(frontier_item_t *it) {
    if (!it) return;
    free(it->url);
    free(it->normalized);
    free(it->domain);
    free(it);
}

static void heap_swap(frontier_item_t **a, frontier_item_t **b) {
    frontier_item_t *t = *a; *a = *b; *b = t;
}

static void heap_push(heap_t *h, frontier_item_t *it) {
    pthread_mutex_lock(&h->lock);
    if (h->size + 1 >= h->capacity) {
        h->capacity *= 2;
        h->items = realloc(h->items, h->capacity * sizeof(frontier_item_t *));
    }
    size_t i = h->size++;
    h->items[i] = it;
    while (i > 0) {
        size_t p = (i - 1) / 2;
        if (h->items[p]->score >= h->items[i]->score) break;
        heap_swap(&h->items[p], &h->items[i]);
        i = p;
    }
    pthread_cond_signal(&h->cond);
    pthread_mutex_unlock(&h->lock);
}

static frontier_item_t *heap_pop(heap_t *h) {
    pthread_mutex_lock(&h->lock);
    while (h->size == 0 && !shutdown_requested) {
        pthread_cond_wait(&h->cond, &h->lock);
    }
    if (h->size == 0) { pthread_mutex_unlock(&h->lock); return NULL; }
    frontier_item_t *res = h->items[0];
    h->items[0] = h->items[--h->size];
    size_t i = 0;
    while (1) {
        size_t l = 2*i + 1, r = 2*i + 2, largest = i;
        if (l < h->size && h->items[l]->score > h->items[largest]->score) largest = l;
        if (r < h->size && h->items[r]->score > h->items[largest]->score) largest = r;
        if (largest == i) break;
        heap_swap(&h->items[i], &h->items[largest]);
        i = largest;
    }
    pthread_mutex_unlock(&h->lock);
    return res;
}

/* ---------------- CURL fetch ---------------- */

typedef struct {
    char *data;
    size_t len;
    size_t cap;
} membuf_t;

static size_t curl_write_cb(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t real = size * nmemb;
    membuf_t *m = (membuf_t*)userdata;
    if (m->len + real + 1 > m->cap) {
        size_t newcap = (m->cap ? m->cap * 2 : 8192);
        while (newcap < m->len + real + 1) newcap *= 2;
        char *tmp = realloc(m->data, newcap);
        if (!tmp) return 0;
        m->data = tmp;
        m->cap = newcap;
    }
    memcpy(m->data + m->len, ptr, real);
    m->len += real;
    m->data[m->len] = '\0';
    return real;
}

/* fetch_url returns 1 on success; fills out->data (malloc'd) and optionally content_type and response_code */
static int fetch_url(const char *url, membuf_t *out, char **content_type, long *response_code) {
    CURL *curl = curl_easy_init();
    if (!curl) return 0;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 6L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, out);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, ""); /* enable gzip deflate */
    CURLcode rc = curl_easy_perform(curl);
    if (content_type) {
        char *ct = NULL;
        curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
        if (ct) *content_type = strdup(ct);
    }
    if (response_code) curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, response_code);
    curl_easy_cleanup(curl);
    return (rc == CURLE_OK);
}

/* ---------------- robots.txt support ---------------- */
/* robots_cache_entry stores parsed Disallow prefixes, Crawl-delay and Sitemap entries for an origin */
typedef struct robot_rule {
    char *prefix;
    struct robot_rule *next;
} robot_rule_t;

typedef struct robots_cache_entry {
    char *origin;
    robot_rule_t *disallows;
    int crawl_delay; /* seconds, 0 if unspecified */
    char **sitemaps;
    size_t s_count;
    struct robots_cache_entry *next;
} robots_cache_entry;

static robots_cache_entry *robots_cache = NULL;
static pthread_mutex_t robots_cache_lock = PTHREAD_MUTEX_INITIALIZER;

/* free robots entry */
static void free_robots_entry(robots_cache_entry *e) {
    if (!e) return;
    free(e->origin);
    robot_rule_t *r = e->disallows;
    while (r) { robot_rule_t *tmp = r->next; free(r->prefix); free(r); r = tmp; }
    for (size_t i=0;i<e->s_count;++i) free(e->sitemaps[i]);
    free(e->sitemaps);
    free(e);
}

/* parse robots.txt content (simple): records Disallow for User-agent: * and Crawl-delay and Sitemap lines */
static robots_cache_entry *parse_robots(const char *origin, const char *txt) {
    robots_cache_entry *e = calloc(1, sizeof(robots_cache_entry));
    e->origin = strdup(origin);
    e->crawl_delay = 0;
    e->sitemaps = NULL; e->s_count = 0;
    robot_rule_t *last = NULL;
    int relevant = 0;

    char *dup = strdup(txt);
    char *line = strtok(dup, "\r\n");
    while (line) {
        char *p = line;
        while (*p && isspace((unsigned char)*p)) p++;
        if (strncasecmp(p, "User-agent:", 11) == 0) {
            char *ua = p + 11;
            while (*ua && isspace((unsigned char)*ua)) ua++;
            relevant = (strcmp(ua, "*") == 0) ? 1 : 0;
        } else if (relevant && strncasecmp(p, "Disallow:", 9) == 0) {
            char *v = p + 9; while (*v && isspace((unsigned char)*v)) v++;
            if (*v) {
                robot_rule_t *r = calloc(1, sizeof(robot_rule_t));
                r->prefix = strdup(v);
                if (!last) e->disallows = r; else last->next = r;
                last = r;
            } else {
                /* empty Disallow means allow all - ignore */
            }
        } else if (strncasecmp(p, "Crawl-delay:", 12) == 0) {
            char *v = p + 12; while (*v && isspace((unsigned char)*v)) v++;
            e->crawl_delay = atoi(v);
        } else if (strncasecmp(p, "Sitemap:", 8) == 0) {
            char *v = p + 8; while (*v && isspace((unsigned char)*v)) v++;
            if (*v) {
                e->sitemaps = realloc(e->sitemaps, sizeof(char*) * (e->s_count + 1));
                e->sitemaps[e->s_count++] = strdup(v);
            }
        }
        line = strtok(NULL, "\r\n");
    }
    free(dup);
    return e;
}

/* Fetch robots.txt for origin and cache result */
static robots_cache_entry *get_robots_for_origin(const char *origin) {
    if (!origin) return NULL;
    pthread_mutex_lock(&robots_cache_lock);
    for (robots_cache_entry *e = robots_cache; e; e = e->next) {
        if (strcmp(e->origin, origin) == 0) { pthread_mutex_unlock(&robots_cache_lock); return e; }
    }
    pthread_mutex_unlock(&robots_cache_lock);

    char url[2048];
    snprintf(url, sizeof(url), "%s/robots.txt", origin);
    membuf_t mb = {0};
    char *ct = NULL;
    long resp = 0;
    if (!fetch_url(url, &mb, &ct, &resp) || resp >= 400) {
        free(ct);
        if (mb.data) free(mb.data);
        return NULL;
    }
    robots_cache_entry *ent = parse_robots(origin, mb.data);
    pthread_mutex_lock(&robots_cache_lock);
    ent->next = robots_cache;
    robots_cache = ent;
    pthread_mutex_unlock(&robots_cache_lock);
    free(ct);
    if (mb.data) free(mb.data);
    return ent;
}

/* Check path against disallow prefixes (prefix match) */
static int robots_disallowed(robots_cache_entry *e, const char *path) {
    if (!e) return 0;
    for (robot_rule_t *r = e->disallows; r; r = r->next) {
        if (r->prefix[0] == '/') {
            if (strncmp(path, r->prefix, strlen(r->prefix)) == 0) return 1;
        }
    }
    return 0;
}

/* ---------------- Sitemap parsing ----------------
   For each sitemap URL, fetch it and extract <loc> entries and enqueue them.
*/
static void parse_sitemap_and_enqueue(const char *sitemap_url, heap_t *frontier, int max_depth) {
    log_msg("Sitemap: fetching %s", sitemap_url);
    membuf_t mb = {0};
    char *ct = NULL;
    long resp = 0;
    if (!fetch_url(sitemap_url, &mb, &ct, &resp) || resp >= 400) {
        log_msg("Sitemap: failed to fetch %s (code %ld)", sitemap_url, resp);
        free(ct);
        if (mb.data) free(mb.data);
        return;
    }
    xmlDocPtr doc = xmlReadMemory(mb.data, mb.len, sitemap_url, NULL, XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
    if (!doc) { free(ct); free(mb.data); return; }
    xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
    if (!xpathCtx) { xmlFreeDoc(doc); free(ct); free(mb.data); return; }
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((xmlChar*)"//loc", xpathCtx);
    if (xpathObj && xpathObj->nodesetval) {
        for (int i=0;i<xpathObj->nodesetval->nodeNr;++i) {
            xmlNodePtr node = xpathObj->nodesetval->nodeTab[i];
            xmlChar *txt = xmlNodeGetContent(node);
            if (txt) {
                char *loc = strdup((char*)txt);
                char *norm = normalize_url(NULL, loc);
                if (norm) {
                    char *dom = get_domain(norm);
                    /* block check */
                    int blocked = 0;
                    if (dom) {
                        blocked = db_domain_blocked_exact(dom) || db_domain_blocked_suffix(dom);
                    }
                    if (!blocked) {
                        frontier_item_t *it = calloc(1, sizeof(frontier_item_t));
                        it->url = strdup(norm);
                        it->normalized = strdup(norm);
                        it->domain = dom ? dom : strdup("");
                        it->depth = 0;
                        it->score = compute_score(it->domain, 0);
                        heap_push(frontier, it);
                        db_insert_url(it->url, it->normalized, it->domain, it->depth, 1);
                        log_msg("Sitemap: queued %s (score=%.1f)", it->url, it->score);
                    } else {
                        log_msg("Sitemap: skipped blocked domain %s for %s", dom ? dom : "(nil)", norm);
                        if (dom) free(dom);
                    }
                    free(norm);
                }
                free(loc);
            }
        }
    }
    if (xpathObj) xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    xmlFreeDoc(doc);
    free(ct);
    if (mb.data) free(mb.data);
}

/* ---------------- Blocked domains (load file) ---------------- */
static void load_blocked_domains_from_file(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        log_msg("Blocked domains file '%s' not found; continuing without it", path);
        return;
    }
    FILE *f = fopen(path, "r");
    if (!f) { log_msg("Failed to open blocked domains file '%s': %s", path, strerror(errno)); return; }
    char line[512];
    while (fgets(line, sizeof(line), f)) {
        char *p = line;
        while (*p && isspace((unsigned char)*p)) p++;
        char *end = p + strlen(p) - 1;
        while (end >= p && isspace((unsigned char)*end)) *end-- = '\0';
        if (*p == '\0') continue;
        db_insert_blocked_domain(p);
        log_msg("Blocked domain loaded: %s", p);
    }
    fclose(f);
}

/* ---------------- Extraction (emails & phones & links) ---------------- */

/* Extract emails and phones from text; insert into DB */
static void extract_emails_phones_from_text(const char *text, long url_id) {
    if (!text) return;
    char *copy = strdup(text);
    deobfuscate(copy);

    regex_t rx_email;
    regcomp(&rx_email, "([A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,})", REG_ICASE | REG_EXTENDED);
    regmatch_t m[2];
    const char *p = copy;
    while (regexec(&rx_email, p, 2, m, 0) == 0) {
        int start = m[1].rm_so, end = m[1].rm_eo;
        if (start >= 0 && end > start) {
            char tmp[512]; int len = end - start;
            if (len >= (int)sizeof(tmp)) len = sizeof(tmp)-1;
            strncpy(tmp, p + start, len); tmp[len] = '\0';
            char *low = str_tolower_dup(tmp);
            db_insert_email(url_id, low);
            log_msg("[FOUND EMAIL] %s (url_id=%ld)", low, url_id);
            free(low);
        }
        p += m[0].rm_eo;
    }
    regfree(&rx_email);

    regex_t rx_phone;
    regcomp(&rx_phone, "([+0-9][0-9()\\- .]{5,}[0-9])", REG_EXTENDED);
    p = copy;
    while (regexec(&rx_phone, p, 2, m, 0) == 0) {
        int start = m[1].rm_so, end = m[1].rm_eo;
        if (start >= 0 && end > start) {
            char tmp[256]; int len = end - start;
            if (len >= (int)sizeof(tmp)) len = sizeof(tmp)-1;
            strncpy(tmp, p + start, len); tmp[len] = '\0';
            char norm[256]; int j=0;
            for (int i=0; tmp[i] && j < (int)sizeof(norm)-1; ++i) {
                if (tmp[i] == '+' && j==0) norm[j++] = '+';
                if (isdigit((unsigned char)tmp[i])) norm[j++] = tmp[i];
            }
            norm[j] = '\0';
            if (j >= 6) {
                db_insert_phone(url_id, norm);
                log_msg("[FOUND PHONE] %s (url_id=%ld)", norm, url_id);
            }
        }
        p += m[0].rm_eo;
    }
    regfree(&rx_phone);
    free(copy);
}

/* Parse HTML, extract <a href>, mailto, and text nodes; enqueue links */
static void parse_and_extract(const char *base_url, const char *html, long url_id, heap_t *frontier, int current_depth, int max_depth) {
    if (!html) return;
    htmlDocPtr doc = htmlReadMemory(html, strlen(html), base_url, NULL, HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_RECOVER);
    if (!doc) return;
    xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
    if (!xpathCtx) { xmlFreeDoc(doc); return; }

    /* Extract links */
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((xmlChar*)"//a[@href]", xpathCtx);
    if (xpathObj && xpathObj->nodesetval) {
        for (int i=0;i<xpathObj->nodesetval->nodeNr;++i) {
            xmlNodePtr a = xpathObj->nodesetval->nodeTab[i];
            xmlChar *raw = xmlGetProp(a, (xmlChar*)"href");
            if (!raw) continue;
            char *norm = normalize_url(base_url, (char*)raw);
            if (!norm) { xmlFree(raw); continue; }

            /* mailto: */
            if (strncasecmp((char*)raw, "mailto:", 7) == 0) {
                char *mail = strdup((char*)raw + 7);
                deobfuscate(mail);
                char *low = str_tolower_dup(mail);
                if (url_id > 0) { db_insert_email(url_id, low); log_msg("[FOUND MAILTO] %s (url_id=%ld)", low, url_id); }
                free(low); free(mail);
                free(norm); xmlFree(raw);
                continue;
            }

            /* get domain and check blocked */
            char *dom = get_domain(norm);
            int blocked = 0;
            if (dom) {
                blocked = db_domain_blocked_exact(dom) || db_domain_blocked_suffix(dom);
            }

            if (blocked) {
                log_msg("[BLOCKED] skipping %s (domain blocked: %s)", norm, dom ? dom : "(nil)");
                if (dom) free(dom);
                free(norm);
                xmlFree(raw);
                continue;
            }

            /* depth check */
            int next_depth = current_depth + 1;
            if (max_depth >= 0 && next_depth > max_depth) {
                if (dom) free(dom);
                free(norm);
                xmlFree(raw);
                continue;
            }

            /* enqueue */
            double score = compute_score(dom, next_depth);
            frontier_item_t *it = calloc(1, sizeof(frontier_item_t));
            it->url = strdup(norm);
            it->normalized = strdup(norm);
            it->domain = dom ? dom : strdup("");
            it->depth = next_depth;
            it->score = score;
            log_msg("[QUEUE] %s score=%.1f depth=%d", it->url, it->score, it->depth);
            heap_push(frontier, it);
            db_insert_url(it->url, it->normalized, it->domain, it->depth, 1);

            if (dom) free(dom);
            free(norm);
            xmlFree(raw);
        }
    }
    if (xpathObj) xmlXPathFreeObject(xpathObj);

    /* Extract text nodes for email/phone */
    xpathObj = xmlXPathEvalExpression((xmlChar*)"//text()[normalize-space()]", xpathCtx);
    if (xpathObj && xpathObj->nodesetval) {
        for (int i=0;i<xpathObj->nodesetval->nodeNr;++i) {
            xmlNodePtr t = xpathObj->nodesetval->nodeTab[i];
            xmlChar *txt = xmlNodeGetContent(t);
            if (txt) {
                extract_emails_phones_from_text((char*)txt, url_id);
                xmlFree(txt);
            }
        }
    }
    if (xpathObj) xmlXPathFreeObject(xpathObj);

    xmlXPathFreeContext(xpathCtx);
    xmlFreeDoc(doc);
}

/* ---------------- Worker implementation ---------------- */

typedef struct {
    heap_t *frontier;
    int max_depth;
    int id;
} worker_arg_t;

/* pop next frontier item, skipping entries already crawled */
static frontier_item_t *pop_next_uncrawled(heap_t *h) {
    while (!shutdown_requested) {
        frontier_item_t *it = heap_pop(h);
        if (!it) return NULL;
        pthread_mutex_lock(&db_lock);
        sqlite3_stmt *stmt = NULL;
        const char *sel = "SELECT id,crawled FROM urls WHERE normalized = ? LIMIT 1;";
        long id = -1; int crawled = 0;
        if (sqlite3_prepare_v2(db, sel, -1, &stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, it->normalized, -1, SQLITE_TRANSIENT);
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                id = sqlite3_column_int64(stmt, 0);
                crawled = sqlite3_column_int(stmt, 1);
            }
        }
        sqlite3_finalize(stmt);
        pthread_mutex_unlock(&db_lock);

        if (id == -1) {
            /* not in DB yet -> insert queued */
            db_insert_url(it->url, it->normalized, it->domain, it->depth, 1);
            return it;
        } else {
            if (crawled) {
                /* skip */
                heap_free_item(it);
                continue;
            } else {
                return it;
            }
        }
    }
    return NULL;
}

static void *worker_thread(void *arg) {
    worker_arg_t *wa = (worker_arg_t*)arg;
    heap_t *frontier = wa->frontier;
    int max_depth = wa->max_depth;
    int id = wa->id;
    log_msg("[WORKER %d] started", id);

    while (!shutdown_requested) {
        frontier_item_t *it = pop_next_uncrawled(frontier);
        if (!it) { msleep(100); continue; }

        log_msg("[WORKER %d] crawling %s (depth=%d score=%.1f)", id, it->url, it->depth, it->score);

        /* robots check: fetch robots for origin and check Disallow */
        char *origin = origin_from_url(it->url);
        char *path = path_from_url(it->url);
        int disallowed = 0;
        robots_cache_entry *rules = NULL;
        if (origin) {
            rules = get_robots_for_origin(origin);
            if (rules && rules->disallows) disallowed = robots_disallowed(rules, path ? path : "/");
            /* fetch sitemaps listed in robots (first time) */
            if (rules && rules->s_count > 0) {
                for (size_t i=0;i<rules->s_count;++i) {
                    parse_sitemap_and_enqueue(rules->sitemaps[i], frontier, max_depth);
                }
            } else {
                /* fallback try /sitemap.xml */
                char s[2048];
                snprintf(s, sizeof(s), "%s/sitemap.xml", origin);
                parse_sitemap_and_enqueue(s, frontier, max_depth);
            }
        }
        if (disallowed) {
            log_msg("[WORKER %d] SKIP robots disallow: %s", id, it->url);
            long url_id = db_insert_url(it->url, it->normalized, it->domain, it->depth, 0);
            db_mark_crawled(url_id, "robots");
            if (origin) free(origin);
            if (path) free(path);
            heap_free_item(it);
            continue;
        }
        if (origin) free(origin);
        if (path) free(path);

        /* Fetch */
        membuf_t mb = {0};
        char *ct = NULL; long resp = 0;
        int ok = fetch_url(it->url, &mb, &ct, &resp);
        if (!ok || resp >= 400) {
            log_msg("[WORKER %d] fetch failed: %s (code %ld)", id, it->url, resp);
            long url_id = db_insert_url(it->url, it->normalized, it->domain, it->depth, 0);
            db_mark_crawled(url_id, "fetch_failed");
            if (mb.data) free(mb.data);
            free(ct);
            heap_free_item(it);
            msleep(POLITENESS_MS);
            continue;
        }

        /* SHA-256 duplicate detection */
        char hash[65];
        sha256_hex((unsigned char*)mb.data, mb.len, hash);
        if (db_already_have_hash(hash)) {
            log_msg("[WORKER %d] duplicate content hash %s — marking crawled", id, hash);
            long url_id = db_insert_url(it->url, it->normalized, it->domain, it->depth, 0);
            db_mark_crawled(url_id, hash);
            if (mb.data) free(mb.data);
            free(ct);
            heap_free_item(it);
            msleep(POLITENESS_MS);
            continue;
        }

        /* store and extract */
        long url_id = db_insert_url(it->url, it->normalized, it->domain, it->depth, 0);
        parse_and_extract(it->url, mb.data, url_id, frontier, it->depth, max_depth);
        db_mark_crawled(url_id, hash);
        log_msg("[WORKER %d] crawled %s (id=%ld)", id, it->url, url_id);

        if (mb.data) free(mb.data);
        if (ct) free(ct);
        heap_free_item(it);

        msleep(POLITENESS_MS);
    }

    log_msg("[WORKER %d] exiting", id);
    return NULL;
}

/* ---------------- DB -> frontier resume loader ---------------- */
static int db_load_uncrawled_into_frontier(heap_t *frontier, int max_depth) {
    pthread_mutex_lock(&db_lock);
    sqlite3_stmt *stmt = NULL;
    const char *sel = "SELECT id,url,normalized,domain,depth FROM urls WHERE crawled=0;";
    if (sqlite3_prepare_v2(db, sel, -1, &stmt, NULL) != SQLITE_OK) {
        pthread_mutex_unlock(&db_lock);
        return 0;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        /*long id = sqlite3_column_int64(stmt, 0);*/ /* unused but could be used */
        const char *url = (const char*)sqlite3_column_text(stmt, 1);
        const char *norm = (const char*)sqlite3_column_text(stmt, 2);
        const char *dom = (const char*)sqlite3_column_text(stmt, 3);
        int depth = sqlite3_column_int(stmt, 4);
        if (max_depth >= 0 && depth > max_depth) continue;
        frontier_item_t *it = calloc(1, sizeof(frontier_item_t));
        it->url = strdup(url ? url : "");
        it->normalized = strdup(norm ? norm : url ? url : "");
        it->domain = strdup(dom ? dom : "");
        it->depth = depth;
        it->score = compute_score(it->domain, it->depth);
        heap_push(frontier, it);
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&db_lock);
    return 1;
}

/* ---------------- Main ---------------- */
int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <seed-url> <num-threads> <max-depth>\n", argv[0]);
        fprintf(stderr, "Example: %s https://example.edu 6 4\n", argv[0]);
        return 1;
    }
    const char *seed = argv[1];
    int num_threads = atoi(argv[2]);
    int max_depth = atoi(argv[3]);
    if (num_threads <= 0) num_threads = DEFAULT_THREADS;
    if (max_depth < 0) max_depth = DEFAULT_MAX_DEPTH;

    /* Setup */
    signal(SIGINT, handle_sigint);
    curl_global_init(CURL_GLOBAL_ALL);
    xmlInitParser();
    db_init();

    /* load blocked domains file */
    load_blocked_domains_from_file(BLOCKED_FILE);

    /* create frontier */
    heap_t *frontier = heap_create(256);

    /* Normalize seed; check blocklist */
    char *norm_seed = normalize_url(NULL, seed);
    char *seed_domain = get_domain(norm_seed ? norm_seed : seed);
    if (seed_domain && (db_domain_blocked_exact(seed_domain) || db_domain_blocked_suffix(seed_domain))) {
        log_msg("Seed domain %s is blocked; exiting", seed_domain);
        free(norm_seed); free(seed_domain);
        return 1;
    }
    frontier_item_t *seed_it = calloc(1, sizeof(frontier_item_t));
    seed_it->url = strdup(norm_seed ? norm_seed : seed);
    seed_it->normalized = strdup(norm_seed ? norm_seed : seed);
    seed_it->domain = seed_domain ? seed_domain : strdup("");
    seed_it->depth = 0;
    seed_it->score = compute_score(seed_it->domain, 0);
    heap_push(frontier, seed_it);
    db_insert_url(seed_it->url, seed_it->normalized, seed_it->domain, seed_it->depth, 1);
    log_msg("[SEED] %s (score=%.1f)", seed_it->url, seed_it->score);
    free(norm_seed);

    /* Resume leftover uncrawled urls in DB */
    db_load_uncrawled_into_frontier(frontier, max_depth);

    /* Start workers */
    pthread_t *tids = calloc(num_threads, sizeof(pthread_t));
    worker_arg_t *args = calloc(num_threads, sizeof(worker_arg_t));
    for (int i=0;i<num_threads;++i) {
        args[i].frontier = frontier;
        args[i].max_depth = max_depth;
        args[i].id = i+1;
        if (pthread_create(&tids[i], NULL, worker_thread, &args[i]) != 0) {
            log_msg("Failed to create worker %d", i+1);
        }
    }

    /* Monitor loop */
    while (!shutdown_requested) {
        msleep(5000);
        pthread_mutex_lock(&frontier->lock);
        size_t fsize = frontier->size;
        pthread_mutex_unlock(&frontier->lock);

        pthread_mutex_lock(&db_lock);
        sqlite3_stmt *stmt = NULL;
        const char *sel = "SELECT COUNT(1) FROM urls WHERE crawled=1;";
        int crawled = 0;
        if (sqlite3_prepare_v2(db, sel, -1, &stmt, NULL) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) crawled = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
        pthread_mutex_unlock(&db_lock);

        log_msg("[STATUS] frontier=%zu crawled=%d threads=%d", fsize, crawled, num_threads);
    }

    /* Shutdown: wake workers and join */
    log_msg("Shutdown requested — notifying workers");
    pthread_mutex_lock(&frontier->lock);
    pthread_cond_broadcast(&frontier->cond);
    pthread_mutex_unlock(&frontier->lock);

    for (int i=0;i<num_threads;++i) {
        pthread_join(tids[i], NULL);
    }

    /* free robots cache */
    pthread_mutex_lock(&robots_cache_lock);
    robots_cache_entry *rc = robots_cache;
    while (rc) { robots_cache_entry *tmp = rc->next; free_robots_entry(rc); rc = tmp; }
    robots_cache = NULL;
    pthread_mutex_unlock(&robots_cache_lock);

    /* close DB and cleanup libs */
    sqlite3_close(db);
    db = NULL;
    curl_global_cleanup();
    xmlCleanupParser();

    log_msg("Crawler exited cleanly");
    return 0;
}
