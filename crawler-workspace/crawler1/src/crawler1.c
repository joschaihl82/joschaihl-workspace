/*
 crawler.c
 Multithreaded recursive crawler with persistence, Crawl-delay, and page-hash deduplication.

 - Multithreaded worker pool (default 6 threads)
 - Frontier queue (FIFO) with random-pop support
 - Per-host robots.txt parsing (User-agent: * Disallow, Crawl-delay)
 - Per-host last-access tracking to honor Crawl-delay
 - Only downloads text/html
 - Extracts email addresses (regex)
 - SHA-256 hash of page content to detect duplicates
 - If a fetched page's hash already exists, worker discards it and pops a random unvisited URL from the frontier
 - Persistent visited set (visited.txt), emails (emails.txt), and hashes (hashes.txt)
 - Graceful shutdown on SIGINT
 - Defaults: start="https://usa.gov", max-depth=16, max-pages=1000000, threads=6, default-delay=0
 - Compile: gcc -std=c11 -O2 -pthread -o crawler crawler.c -lcurl -lxml2 -lcrypto
 - Run: ./crawler [start-url] [max-depth] [max-pages] [num-threads] [default-delay]
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/uri.h>
#include <openssl/evp.h>

#define DEFAULT_START_URL "https://usa.gov"
#define DEFAULT_MAX_DEPTH 16
#define DEFAULT_MAX_PAGES 1000000UL
#define DEFAULT_THREADS 6
#define DEFAULT_DELAY 0.0

#define VISITED_FILE "visited.txt"
#define EMAILS_FILE "emails.txt"
#define HASHES_FILE "hashes.txt"
#define USER_AGENT "C-mt-crawler/1.0"

#define VISITED_BUCKETS 16384
#define HOST_BUCKETS 4096
#define HASH_BUCKETS 16384

/* Toggle verbose logging */
#define VERBOSE 1

/* ---------- Simple memory buffer for libcurl ---------- */
typedef struct { char *data; size_t len; } mem_t;
static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *ud){
    size_t s = size * nmemb;
    mem_t *m = (mem_t*)ud;
    char *p = realloc(m->data, m->len + s + 1);
    if(!p) return 0;
    m->data = p;
    memcpy(m->data + m->len, ptr, s);
    m->len += s;
    m->data[m->len] = 0;
    return s;
}
static size_t header_cb(char *buf, size_t size, size_t nitems, void *ud){
    size_t s = size * nitems;
    char **ctype = (char**)ud;
    if(strncasecmp(buf, "Content-Type:", 13) == 0){
        char *p = buf + 13;
        while(*p == ' ' || *p == '\t') p++;
        free(*ctype);
        *ctype = strndup(p, s - (p - buf));
        char *nl = strchr(*ctype, '\r'); if(nl) *nl = 0;
        nl = strchr(*ctype, '\n'); if(nl) *nl = 0;
    }
    return s;
}

/* ---------- Robots structure ---------- */
typedef struct {
    char **disallow; size_t dcount;
    double crawl_delay; /* seconds, 0 = not set */
} robots_t;

/* ---------- Visited set (hash table) ---------- */
typedef struct visited_node { char *url; struct visited_node *next; } visited_node_t;
typedef struct { visited_node_t *buckets[VISITED_BUCKETS]; pthread_mutex_t lock; } visited_set_t;

static unsigned long hash_str(const char *s){
    unsigned long h = 5381;
    while(*s) h = ((h << 5) + h) + (unsigned char)*s++;
    return h;
}
void visited_init(visited_set_t *vs){ memset(vs->buckets, 0, sizeof(vs->buckets)); pthread_mutex_init(&vs->lock, NULL); }
int visited_has(visited_set_t *vs, const char *url){
    unsigned long h = hash_str(url) % VISITED_BUCKETS;
    pthread_mutex_lock(&vs->lock);
    visited_node_t *n = vs->buckets[h];
    while(n){ if(strcmp(n->url, url)==0){ pthread_mutex_unlock(&vs->lock); return 1; } n = n->next; }
    pthread_mutex_unlock(&vs->lock);
    return 0;
}
void visited_add(visited_set_t *vs, const char *url){
    if(visited_has(vs, url)) return;
    unsigned long h = hash_str(url) % VISITED_BUCKETS;
    visited_node_t *n = malloc(sizeof(*n));
    n->url = strdup(url);
    pthread_mutex_lock(&vs->lock);
    n->next = vs->buckets[h];
    vs->buckets[h] = n;
    pthread_mutex_unlock(&vs->lock);
    if(VERBOSE) fprintf(stderr, "[VISITED] %s\n", url);
}
void visited_save(visited_set_t *vs, const char *fname){
    FILE *f = fopen(fname, "w"); if(!f) return;
    pthread_mutex_lock(&vs->lock);
    for(size_t i=0;i<VISITED_BUCKETS;i++){ visited_node_t *n = vs->buckets[i]; while(n){ fprintf(f, "%s\n", n->url); n = n->next; } }
    pthread_mutex_unlock(&vs->lock); fclose(f);
}
void visited_load(visited_set_t *vs, const char *fname){
    FILE *f = fopen(fname, "r"); if(!f) return;
    char *line = NULL; size_t len = 0;
    while(getline(&line, &len, f) != -1){ line[strcspn(line, "\r\n")] = 0; if(strlen(line) > 0) visited_add(vs, line); }
    free(line); fclose(f);
}

/* ---------- Hashset for page hashes (SHA-256 hex) ---------- */
typedef struct hash_node { char hex[65]; struct hash_node *next; } hash_node_t;
typedef struct { hash_node_t *buckets[HASH_BUCKETS]; pthread_mutex_t lock; } hash_set_t;

void hashset_init(hash_set_t *hs){ memset(hs->buckets, 0, sizeof(hs->buckets)); pthread_mutex_init(&hs->lock, NULL); }
unsigned long hash_hex(const char *hex){ unsigned long h = 2166136261u; while(*hex) h = (h ^ (unsigned char)*hex++) * 16777619u; return h; }
int hashset_has(hash_set_t *hs, const char *hex){
    unsigned long h = hash_hex(hex) % HASH_BUCKETS;
    pthread_mutex_lock(&hs->lock);
    hash_node_t *n = hs->buckets[h];
    while(n){ if(strcmp(n->hex, hex)==0){ pthread_mutex_unlock(&hs->lock); return 1; } n = n->next; }
    pthread_mutex_unlock(&hs->lock);
    return 0;
}
void hashset_insert_no_persist(hash_set_t *hs, const char *hex){
    if(hashset_has(hs, hex)) return;
    unsigned long h = hash_hex(hex) % HASH_BUCKETS;
    hash_node_t *n = malloc(sizeof(*n)); strncpy(n->hex, hex, sizeof(n->hex)); n->hex[64]=0;
    pthread_mutex_lock(&hs->lock); n->next = hs->buckets[h]; hs->buckets[h] = n; pthread_mutex_unlock(&hs->lock);
}
void hashset_add(hash_set_t *hs, const char *hex){
    if(hashset_has(hs, hex)) return;
    unsigned long h = hash_hex(hex) % HASH_BUCKETS;
    hash_node_t *n = malloc(sizeof(*n)); strncpy(n->hex, hex, sizeof(n->hex)); n->hex[64]=0;
    pthread_mutex_lock(&hs->lock); n->next = hs->buckets[h]; hs->buckets[h] = n; pthread_mutex_unlock(&hs->lock);
    FILE *f = fopen(HASHES_FILE, "a"); if(f){ fprintf(f, "%s\n", hex); fclose(f); }
    if(VERBOSE) fprintf(stderr, "[HASH ADD] %s\n", hex);
}
void hashset_load(hash_set_t *hs, const char *fname){
    FILE *f = fopen(fname, "r"); if(!f) return;
    char buf[128];
    while(fgets(buf, sizeof(buf), f)){ buf[strcspn(buf, "\r\n")] = 0; if(strlen(buf) == 64) hashset_insert_no_persist(hs, buf); }
    fclose(f);
}

/* ---------- Emails store ---------- */
typedef struct email_node { char *email; struct email_node *next; } email_node_t;
typedef struct { email_node_t *head; pthread_mutex_t lock; } emails_t;
void emails_init(emails_t *es){ es->head = NULL; pthread_mutex_init(&es->lock, NULL); }
int emails_has(emails_t *es, const char *email){
    pthread_mutex_lock(&es->lock); email_node_t *n = es->head; while(n){ if(strcmp(n->email, email)==0){ pthread_mutex_unlock(&es->lock); return 1; } n = n->next; } pthread_mutex_unlock(&es->lock); return 0;
}
void emails_add(emails_t *es, const char *email){
    if(emails_has(es, email)) return;
    email_node_t *n = malloc(sizeof(*n)); n->email = strdup(email);
    pthread_mutex_lock(&es->lock); n->next = es->head; es->head = n; pthread_mutex_unlock(&es->lock);
    FILE *f = fopen(EMAILS_FILE, "a"); if(f){ fprintf(f, "%s\n", email); fclose(f); }
    if(VERBOSE) fprintf(stderr, "[EMAIL] %s\n", email);
}
void emails_load(emails_t *es, const char *fname){
    FILE *f = fopen(fname, "r"); if(!f) return;
    char *line = NULL; size_t len = 0;
    while(getline(&line, &len, f) != -1){ line[strcspn(line, "\r\n")] = 0; if(strlen(line) > 0) emails_add(es, line); }
    free(line); fclose(f);
}

/* ---------- Frontier queue with random pop ---------- */
typedef struct frontier_item { char *url; int depth; struct frontier_item *next; } frontier_item_t;
typedef struct { frontier_item_t *head, *tail; pthread_mutex_t lock; pthread_cond_t cond; size_t size; int finished; } frontier_t;

void frontier_init(frontier_t *q){ q->head = q->tail = NULL; q->size = 0; q->finished = 0; pthread_mutex_init(&q->lock, NULL); pthread_cond_init(&q->cond, NULL); }
void frontier_push(frontier_t *q, const char *url, int depth){
    frontier_item_t *it = malloc(sizeof(*it)); it->url = strdup(url); it->depth = depth; it->next = NULL;
    pthread_mutex_lock(&q->lock);
    if(q->tail) q->tail->next = it; else q->head = it;
    q->tail = it; q->size++;
    if(VERBOSE) fprintf(stderr, "[QUEUE PUSH] size=%zu url=%s depth=%d\n", q->size, url, depth);
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->lock);
}
int frontier_pop(frontier_t *q, char **url_out, int *depth_out){
    pthread_mutex_lock(&q->lock);
    while(q->size == 0 && !q->finished) pthread_cond_wait(&q->cond, &q->lock);
    if(q->size == 0 && q->finished){ pthread_mutex_unlock(&q->lock); return 0; }
    frontier_item_t *it = q->head; q->head = it->next; if(q->head == NULL) q->tail = NULL; q->size--;
    if(VERBOSE) fprintf(stderr, "[QUEUE POP] size=%zu url=%s depth=%d\n", q->size, it->url, it->depth);
    pthread_mutex_unlock(&q->lock);
    *url_out = it->url; *depth_out = it->depth; free(it); return 1;
}
int frontier_pop_random(frontier_t *q, char **url_out, int *depth_out, unsigned int *seedp){
    pthread_mutex_lock(&q->lock);
    if(q->size == 0){ pthread_mutex_unlock(&q->lock); return 0; }
    size_t idx = (size_t)(rand_r(seedp) % q->size);
    frontier_item_t *prev = NULL; frontier_item_t *cur = q->head;
    for(size_t i=0;i<idx;i++){ prev = cur; cur = cur->next; }
    if(prev) prev->next = cur->next; else q->head = cur->next;
    if(cur == q->tail) q->tail = prev;
    q->size--;
    if(VERBOSE) fprintf(stderr, "[QUEUE RAND POP] idx=%zu size=%zu url=%s depth=%d\n", idx, q->size, cur->url, cur->depth);
    pthread_mutex_unlock(&q->lock);
    *url_out = cur->url; *depth_out = cur->depth; free(cur); return 1;
}
void frontier_finish(frontier_t *q){ pthread_mutex_lock(&q->lock); q->finished = 1; pthread_cond_broadcast(&q->cond); pthread_mutex_unlock(&q->lock); }

/* ---------- Per-host robots and last-access tracking ---------- */
typedef struct host_node { char *host; robots_t *robots; double last_access; struct host_node *next; } host_node_t;
typedef struct { host_node_t *buckets[HOST_BUCKETS]; pthread_mutex_t lock; } host_cache_t;
void host_cache_init(host_cache_t *hc){ memset(hc->buckets, 0, sizeof(hc->buckets)); pthread_mutex_init(&hc->lock, NULL); }
unsigned long host_hash(const char *s){ unsigned long h = 5381; while(*s) h = ((h<<5)+h) + (unsigned char)*s++; return h; }
host_node_t *host_cache_get(host_cache_t *hc, const char *host){
    unsigned long h = host_hash(host) % HOST_BUCKETS;
    pthread_mutex_lock(&hc->lock);
    host_node_t *n = hc->buckets[h];
    while(n){ if(strcmp(n->host, host)==0){ pthread_mutex_unlock(&hc->lock); return n; } n = n->next; }
    n = malloc(sizeof(*n)); n->host = strdup(host); n->robots = NULL; n->last_access = 0.0;
    n->next = hc->buckets[h]; hc->buckets[h] = n;
    pthread_mutex_unlock(&hc->lock);
    if(VERBOSE) fprintf(stderr, "[HOST CACHE] new host=%s\n", host);
    return n;
}

/* ---------- Globals ---------- */
static visited_set_t visited;
static emails_t emails;
static frontier_t frontier;
static host_cache_t host_cache;
static hash_set_t hashes;
static volatile sig_atomic_t stop_requested = 0;
static size_t pages_fetched = 0;
static size_t max_pages = DEFAULT_MAX_PAGES;
static int max_depth = DEFAULT_MAX_DEPTH;

/* ---------- Signal handling ---------- */
void handle_sigint(int sig){ (void)sig; stop_requested = 1; frontier_finish(&frontier); fprintf(stderr, "\nSIGINT: stopping, will persist state and exit...\n"); }

/* ---------- Utility functions ---------- */
char *normalize_url(const char *base, const char *href){ xmlChar *r = xmlBuildURI((const xmlChar*)href, (const xmlChar*)base); if(!r) return NULL; char *s = strdup((char*)r); xmlFree(r); return s; }
int same_host(const char *a, const char *b){ xmlURIPtr ua = xmlParseURI(a), ub = xmlParseURI(b); if(!ua || !ub){ if(ua) xmlFreeURI(ua); if(ub) xmlFreeURI(ub); return 0; } int ok = ua->server && ub->server && strcmp(ua->server, ub->server) == 0; xmlFreeURI(ua); xmlFreeURI(ub); return ok; }
char *url_host(const char *u){ xmlURIPtr x = xmlParseURI(u); if(!x) return NULL; char *s = x->server ? strdup(x->server) : NULL; xmlFreeURI(x); return s; }
char *url_path(const char *u){ xmlURIPtr x = xmlParseURI(u); if(!x) return NULL; char *p = x->path ? strdup(x->path) : strdup("/"); xmlFreeURI(x); return p; }

/* ---------- Robots.txt fetching and parsing ---------- */
robots_t *robots_parse_from_text(char *txt){
    if(!txt) return NULL;
    robots_t *r = calloc(1, sizeof(*r)); r->crawl_delay = 0.0;
    char *saveptr = NULL; char *line = strtok_r(txt, "\n", &saveptr); int in_group = 0;
    while(line){
        char *s = line; while(*s == ' ' || *s == '\t') s++;
        if(strncasecmp(s, "User-agent:", 11) == 0){ char *ua = s + 11; while(*ua == ' ' || *ua == '\t') ua++; in_group = (strcmp(ua, "*") == 0); }
        else if(in_group && strncasecmp(s, "Disallow:", 9) == 0){ char *p = s + 9; while(*p == ' ' || *p == '\t') p++; if(*p){ r->disallow = realloc(r->disallow, (r->dcount + 1) * sizeof(char*)); r->disallow[r->dcount++] = strdup(p); } }
        else if(in_group && strncasecmp(s, "Crawl-delay:", 12) == 0){ char *p = s + 12; while(*p == ' ' || *p == '\t') p++; r->crawl_delay = atof(p); }
        else if(*s == 0) in_group = 0;
        line = strtok_r(NULL, "\n", &saveptr);
    }
    return r;
}
robots_t *robots_fetch_for(const char *base){
    char *robots_url = normalize_url(base, "/robots.txt"); if(!robots_url) return NULL;
    CURL *c = curl_easy_init(); if(!c){ free(robots_url); return NULL; }
    mem_t m = {0}; char *ctype = NULL;
    curl_easy_setopt(c, CURLOPT_URL, robots_url); curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, write_cb); curl_easy_setopt(c, CURLOPT_WRITEDATA, &m);
    curl_easy_setopt(c, CURLOPT_HEADERFUNCTION, header_cb); curl_easy_setopt(c, CURLOPT_HEADERDATA, &ctype);
    curl_easy_setopt(c, CURLOPT_USERAGENT, USER_AGENT);
    CURLcode res = curl_easy_perform(c); curl_easy_cleanup(c); free(robots_url); free(ctype);
    if(res != CURLE_OK || m.len == 0){ free(m.data); return NULL; }
    robots_t *r = robots_parse_from_text(m.data); free(m.data);
    if(VERBOSE) fprintf(stderr, "[ROBOTS] fetched for base=%s crawl_delay=%.2f disallow_count=%zu\n", base, r->crawl_delay, r->dcount);
    return r;
}
int allowed_by_robots(robots_t *r, const char *path){ if(!r) return 1; for(size_t i=0;i<r->dcount;i++){ size_t L = strlen(r->disallow[i]); if(L == 0) continue; if(strncmp(path, r->disallow[i], L) == 0) return 0; } return 1; }

/* ---------- Email extraction ---------- */
void extract_emails_and_persist(const char *html){
    regex_t re; regcomp(&re, "[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}", REG_EXTENDED);
    const char *p = html; regmatch_t m;
    while(regexec(&re, p, 1, &m, 0) == 0){
        int len = m.rm_eo - m.rm_so; char buf[1024];
        if(len >= (int)sizeof(buf)) len = sizeof(buf)-1;
        memcpy(buf, p + m.rm_so, len); buf[len] = 0;
        if(!emails_has(&emails, buf)){ emails_add(&emails, buf); }
        p += m.rm_eo;
    }
    regfree(&re);
}

/* ---------- SHA-256 helper (EVP) ---------- */
void sha256_hex(const unsigned char *data, size_t len, char out_hex[65]){
    unsigned char digest[EVP_MAX_MD_SIZE]; unsigned int digest_len = 0;
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new(); if(!mdctx){ out_hex[0]=0; return; }
    if(1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL)){ EVP_MD_CTX_free(mdctx); out_hex[0]=0; return; }
    if(1 != EVP_DigestUpdate(mdctx, data, len)){ EVP_MD_CTX_free(mdctx); out_hex[0]=0; return; }
    if(1 != EVP_DigestFinal_ex(mdctx, digest, &digest_len)){ EVP_MD_CTX_free(mdctx); out_hex[0]=0; return; }
    EVP_MD_CTX_free(mdctx);
    for(unsigned int i = 0; i < digest_len; ++i) sprintf(out_hex + i*2, "%02x", digest[i]);
    out_hex[64] = 0;
}

/* ---------- Sleep helper ---------- */
static void sleep_seconds_interruptible(double secs){
    if(secs <= 0.0) return;
    struct timespec ts; ts.tv_sec = (time_t)secs; ts.tv_nsec = (long)((secs - ts.tv_sec) * 1e9);
    while(nanosleep(&ts, &ts) == -1){ if(errno == EINTR){ if(stop_requested) break; continue; } else break; }
}

/* ---------- Recursive HTML walker to enqueue links ---------- */
void traverse_links_and_enqueue(xmlNode *node, const char *base_url, int depth, frontier_t *q, visited_set_t *vs){
    for(xmlNode *cur = node; cur; cur = cur->next){
        if(cur->type == XML_ELEMENT_NODE && xmlStrcasecmp(cur->name, (const xmlChar*)"a") == 0){
            xmlChar *href = xmlGetProp(cur, (const xmlChar*)"href");
            if(href){
                char *n = normalize_url(base_url, (const char*)href);
                if(n){
                    if(same_host(base_url, n) && !visited_has(vs, n)){
                        frontier_push(q, n, depth + 1);
                    }
                    free(n);
                }
                xmlFree(href);
            }
        }
        if(cur->children) traverse_links_and_enqueue(cur->children, base_url, depth, q, vs);
    }
}

/* ---------- Worker thread ---------- */
typedef struct { frontier_t *frontier; visited_set_t *visited; host_cache_t *hosts; hash_set_t *hashes; int thread_id; double default_delay; unsigned int seed; } worker_arg_t;
int is_html_content(const char *ctype){ if(!ctype) return 0; return (strncasecmp(ctype, "text/html", 9) == 0); }

void *worker_main(void *arg){
    worker_arg_t *wa = (worker_arg_t*)arg;
    frontier_t *q = wa->frontier;
    visited_set_t *vs = wa->visited;
    host_cache_t *hc = wa->hosts;
    hash_set_t *hs = wa->hashes;
    double default_delay = wa->default_delay;
    unsigned int seed = wa->seed;

    CURL *curl = curl_easy_init();
    if(!curl) return NULL;
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_cb);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);

    while(!stop_requested){
        char *url = NULL; int depth = 0;
        if(!frontier_pop(q, &url, &depth)) break;

        if(stop_requested){ free(url); break; }
        if(depth > max_depth){ if(VERBOSE) fprintf(stderr, "[THREAD %d] skipping depth %d url=%s\n", wa->thread_id, depth, url); free(url); continue; }
        if(pages_fetched >= max_pages){ free(url); frontier_finish(q); break; }
        if(visited_has(vs, url)){ if(VERBOSE) fprintf(stderr, "[THREAD %d] already visited: %s\n", wa->thread_id, url); free(url); continue; }

        if(VERBOSE) fprintf(stderr, "[THREAD %d] fetching: %s (depth=%d)\n", wa->thread_id, url, depth);

        char *host = url_host(url);
        char *path = url_path(url);
        host_node_t *hn = NULL;
        if(host){
            hn = host_cache_get(hc, host);
            pthread_mutex_lock(&hc->lock);
            if(hn->robots == NULL){
                pthread_mutex_unlock(&hc->lock);
                robots_t *r = robots_fetch_for(url);
                pthread_mutex_lock(&hc->lock);
                if(hn->robots == NULL) hn->robots = r;
                else if(r){
                    for(size_t i=0;i<r->dcount;i++) free(r->disallow[i]);
                    free(r->disallow); free(r);
                }
            }
            double crawl_delay = default_delay;
            if(hn->robots && hn->robots->crawl_delay > 0.0) crawl_delay = hn->robots->crawl_delay;
            double now = (double)time(NULL);
            double wait = 0.0;
            if(hn->last_access > 0.0){
                double next_allowed = hn->last_access + crawl_delay;
                if(next_allowed > now) wait = next_allowed - now;
            }
            pthread_mutex_unlock(&hc->lock);

            if(hn->robots && !allowed_by_robots(hn->robots, path)){
                if(VERBOSE) fprintf(stderr, "[THREAD %d] disallowed by robots: %s\n", wa->thread_id, url);
                free(host); free(path); free(url);
                continue;
            }
            if(wait > 0.0){
                if(VERBOSE) fprintf(stderr, "[THREAD %d] waiting %.2fs due to Crawl-delay for host %s\n", wa->thread_id, wait, host);
                sleep_seconds_interruptible(wait);
            }
            if(stop_requested){ free(host); free(path); free(url); break; }
        }

        mem_t m = {0}; char *ctype = NULL;
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &ctype);
        CURLcode res = curl_easy_perform(curl);
        long code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

        if(res != CURLE_OK){
            if(VERBOSE) fprintf(stderr, "[THREAD %d] curl error for %s: %s\n", wa->thread_id, url, curl_easy_strerror(res));
            free(m.data); free(ctype); free(host); free(path); free(url);
            continue;
        }
        if(VERBOSE) fprintf(stderr, "[THREAD %d] HTTP %ld Content-Type=%s for %s\n", wa->thread_id, code, ctype?ctype:"(none)", url);

        if(code >= 400){ free(m.data); free(ctype); free(host); free(path); free(url); continue; }
        if(!is_html_content(ctype)){ if(VERBOSE) fprintf(stderr, "[THREAD %d] skipping non-HTML %s\n", wa->thread_id, url); free(m.data); free(ctype); free(host); free(path); free(url); continue; }
        if(!m.data || m.len == 0){ if(VERBOSE) fprintf(stderr, "[THREAD %d] empty body %s\n", wa->thread_id, url); free(m.data); free(ctype); free(host); free(path); free(url); continue; }

        /* compute SHA-256 */
        char hex[65]; sha256_hex((unsigned char*)m.data, m.len, hex);
        if(VERBOSE) fprintf(stderr, "[THREAD %d] sha256=%s for %s\n", wa->thread_id, hex, url);

        if(hashset_has(hs, hex)){
            if(VERBOSE) fprintf(stderr, "[THREAD %d] duplicate page by hash, selecting random frontier item\n", wa->thread_id);
            free(m.data); free(ctype);
            char *rurl = NULL; int rdepth = 0;
            int got = frontier_pop_random(q, &rurl, &rdepth, &seed);
            if(got){
                if(VERBOSE) fprintf(stderr, "[THREAD %d] random pick: %s (depth=%d)\n", wa->thread_id, rurl, rdepth);
                free(host); free(path); free(url);
                url = rurl; depth = rdepth;
                host = url_host(url); path = url_path(url);
                if(host){
                    hn = host_cache_get(hc, host);
                    pthread_mutex_lock(&hc->lock);
                    if(hn->robots == NULL){
                        pthread_mutex_unlock(&hc->lock);
                        robots_t *r = robots_fetch_for(url);
                        pthread_mutex_lock(&hc->lock);
                        if(hn->robots == NULL) hn->robots = r;
                        else if(r){
                            for(size_t i=0;i<r->dcount;i++) free(r->disallow[i]);
                            free(r->disallow); free(r);
                        }
                    }
                    double crawl_delay = default_delay;
                    if(hn->robots && hn->robots->crawl_delay > 0.0) crawl_delay = hn->robots->crawl_delay;
                    double now = (double)time(NULL);
                    double wait = 0.0;
                    if(hn->last_access > 0.0){
                        double next_allowed = hn->last_access + crawl_delay;
                        if(next_allowed > now) wait = next_allowed - now;
                    }
                    pthread_mutex_unlock(&hc->lock);
                    if(hn->robots && !allowed_by_robots(hn->robots, path)){
                        if(VERBOSE) fprintf(stderr, "[THREAD %d] random pick disallowed by robots: %s\n", wa->thread_id, url);
                        free(host); free(path); free(url);
                        continue;
                    }
                    if(wait > 0.0){
                        if(VERBOSE) fprintf(stderr, "[THREAD %d] waiting %.2fs for random pick host %s\n", wa->thread_id, wait, host);
                        sleep_seconds_interruptible(wait);
                    }
                    if(stop_requested){ free(host); free(path); free(url); break; }
                }
                /* perform request for the new url */
                m.data = NULL; m.len = 0; ctype = NULL;
                curl_easy_setopt(curl, CURLOPT_URL, url);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m);
                curl_easy_setopt(curl, CURLOPT_HEADERDATA, &ctype);
                res = curl_easy_perform(curl);
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
                if(res != CURLE_OK || code >= 400 || !is_html_content(ctype) || !m.data || m.len==0){
                    if(VERBOSE) fprintf(stderr, "[THREAD %d] random pick fetch failed or not HTML: %s\n", wa->thread_id, url);
                    free(m.data); free(ctype); free(host); free(path); free(url);
                    continue;
                }
                sha256_hex((unsigned char*)m.data, m.len, hex);
                if(VERBOSE) fprintf(stderr, "[THREAD %d] sha256(random)=%s for %s\n", wa->thread_id, hex, url);
            } else {
                if(VERBOSE) fprintf(stderr, "[THREAD %d] no random frontier item available\n", wa->thread_id);
                free(host); free(path); free(url);
                continue;
            }
        }

        if(!hashset_has(hs, hex)){ hashset_add(hs, hex); }

        visited_add(vs, url);
        size_t pf = __sync_add_and_fetch(&pages_fetched, 1);
        if(VERBOSE) fprintf(stderr, "[THREAD %d] counted page #%zu: %s\n", wa->thread_id, pf, url);

        if(host){ pthread_mutex_lock(&hc->lock); hn->last_access = (double)time(NULL); pthread_mutex_unlock(&hc->lock); }

        if(m.data && m.len > 0) extract_emails_and_persist(m.data);

        htmlDocPtr doc = htmlReadMemory(m.data, m.len, url, NULL, HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
        if(doc){
            xmlNode *root = xmlDocGetRootElement(doc);
            if(root) traverse_links_and_enqueue(root, url, depth, q, vs);
            xmlFreeDoc(doc);
        }

        free(m.data); free(ctype); free(host); free(path); free(url);

        if(pf >= max_pages){ if(VERBOSE) fprintf(stderr, "[MAIN] reached max pages %zu, finishing\n", pf); frontier_finish(q); break; }
    }

    curl_easy_cleanup(curl);
    return NULL;
}

/* ---------- Main ---------- */
int main(int argc, char **argv){
    const char *start = DEFAULT_START_URL;
    max_depth = DEFAULT_MAX_DEPTH;
    max_pages = DEFAULT_MAX_PAGES;
    int num_threads = DEFAULT_THREADS;
    double default_delay = DEFAULT_DELAY;

    if(argc >= 2 && argv[1][0] != '\0') start = argv[1];
    if(argc >= 3) max_depth = atoi(argv[2]);
    if(argc >= 4) max_pages = (size_t)atol(argv[3]);
    if(argc >= 5) num_threads = atoi(argv[4]);
    if(argc >= 6) default_delay = atof(argv[5]);

    if(num_threads <= 0) num_threads = DEFAULT_THREADS;
    if(default_delay < 0.0) default_delay = DEFAULT_DELAY;
    if(max_depth < 0) max_depth = 0;
    if(max_pages == 0) max_pages = DEFAULT_MAX_PAGES;

    if(VERBOSE) fprintf(stderr, "Starting crawler with:\n  start=%s\n  max_depth=%d\n  max_pages=%zu\n  threads=%d\n  default_delay=%.2f\n",
                        start, max_depth, max_pages, num_threads, default_delay);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    xmlInitParser();
    OpenSSL_add_all_algorithms();

    visited_init(&visited);
    emails_init(&emails);
    frontier_init(&frontier);
    host_cache_init(&host_cache);
    hashset_init(&hashes);

    visited_load(&visited, VISITED_FILE);
    emails_load(&emails, EMAILS_FILE);
    hashset_load(&hashes, HASHES_FILE);

    struct sigaction sa; sa.sa_handler = handle_sigint; sigemptyset(&sa.sa_mask); sa.sa_flags = 0; sigaction(SIGINT, &sa, NULL);

    frontier_push(&frontier, start, 0);

    pthread_t *threads = malloc(sizeof(pthread_t) * num_threads);
    worker_arg_t *wargs = malloc(sizeof(worker_arg_t) * num_threads);
    for(int i=0;i<num_threads;i++){
        wargs[i].frontier = &frontier;
        wargs[i].visited = &visited;
        wargs[i].hosts = &host_cache;
        wargs[i].hashes = &hashes;
        wargs[i].thread_id = i;
        wargs[i].default_delay = default_delay;
        wargs[i].seed = (unsigned int)time(NULL) ^ (unsigned int)(i * 7919);
        pthread_create(&threads[i], NULL, worker_main, &wargs[i]);
    }

    for(int i=0;i<num_threads;i++) pthread_join(threads[i], NULL);

    visited_save(&visited, VISITED_FILE);

    for(size_t i=0;i<VISITED_BUCKETS;i++){ visited_node_t *n = visited.buckets[i]; while(n){ visited_node_t *t = n; n = n->next; free(t->url); free(t); } }
    for(size_t i=0;i<HOST_BUCKETS;i++){ host_node_t *hn = host_cache.buckets[i]; while(hn){ host_node_t *t = hn; hn = hn->next; free(t->host); if(t->robots){ for(size_t j=0;j<t->robots->dcount;j++) free(t->robots->disallow[j]); free(t->robots->disallow); free(t->robots); } free(t); } }
    for(size_t i=0;i<HASH_BUCKETS;i++){ hash_node_t *hn = hashes.buckets[i]; while(hn){ hash_node_t *t = hn; hn = hn->next; free(t); } }
    email_node_t *en = emails.head; while(en){ email_node_t *t = en; en = en->next; free(t->email); free(t); }

    free(threads); free(wargs);

    xmlCleanupParser();
    EVP_cleanup();
    curl_global_cleanup();

    fprintf(stderr, "Crawl finished: pages fetched %zu, emails saved to %s\n", pages_fetched, EMAILS_FILE);
    return 0;
}
