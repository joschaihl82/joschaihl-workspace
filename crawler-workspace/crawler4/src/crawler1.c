/*
 * crawler.c
 *
 * Parallel persistent crawler with libxml2 extraction for emails/phones/links.
 * Logs every newly crawled page with its ranking (score).
 * Logs every newly inserted E-Mail address and Phone number.
 * Supports persistent crawling: quitting with Ctrl+C and continuing later.
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
#include <signal.h>
#include <stdarg.h>

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

/* ----------------------------- Signal handling ----------------------------- */

static void handle_sigint(int sig) {
    (void)sig;
    log_info("Caught SIGINT, stopping crawl...");
    pthread_mutex_lock(&g_frontier_lock);
    g_stop = 1;
    pthread_cond_broadcast(&g_frontier_cond);
    pthread_mutex_unlock(&g_frontier_lock);
}

/* ----------------------------- Small helpers ----------------------------- */

static char *strdup_safe(const char *s) {
    if (!s) return NULL;
    char *r = strdup(s);
    if (!r) { perror("strdup"); exit(EXIT_FAILURE); }
    return r;
}

static void sleep_polite(void) { usleep(POLITE_DELAY_MS * 1000); }

void log_info(const char *fmt, ...) {
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

typedef struct dynbuf { char *data; size_t len; } dynbuf_t;

static void dbuf_init(dynbuf_t *b) { b->data = NULL; b->len = 0; }
static void dbuf_free(dynbuf_t *b) { free(b->data); b->data = NULL; b->len = 0; }
static size_t dbuf_write(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t add = size * nmemb;
    dynbuf_t *b = (dynbuf_t *)userdata;
    char *n = realloc(b->data, b->len + add + 1);
    if (!n) return 0;
    memcpy(n + b->len, ptr, add);
    b->data = n;
    b->len += add;
    b->data[b->len] = '\0';
    return add;
}

/* ----------------------------- SHA-256 helpers ----------------------------- */

static char *sha256_hex(const unsigned char *data, size_t len) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX ctx;
    if (!SHA256_Init(&ctx)) return NULL;
    if (!SHA256_Update(&ctx, data, len)) return NULL;
    if (!SHA256_Final(hash, &ctx)) return NULL;

    char *hex = malloc(SHA256_DIGEST_LENGTH * 2 + 1);
    if (!hex) return NULL;
    for (size_t i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        sprintf(hex + (i * 2), "%02x", hash[i]);
    hex[SHA256_DIGEST_LENGTH * 2] = '\0';
    return hex;
}

/* ----------------------------- URL utilities ----------------------------- */

static char *normalize_url(const char *url) {
    if (!url) return NULL;
    char *copy = strdup_safe(url);
    char *p = strchr(copy, '#'); if (p) *p = '\0';
    size_t len = strlen(copy);
    while (len > 1 && copy[len - 1] == '/') { copy[--len] = '\0'; }
    return copy;
}

static char *url_host(const char *url) {
    if (!url) return NULL;
    const char *p = strstr(url, "://");
    const char *h = (p ? p + 3 : url);
    const char *end = strchr(h, '/');
    size_t len = end ? (size_t)(end - h) : strlen(h);
    char *host = malloc(len + 1);
    if (!host) { perror("malloc"); exit(EXIT_FAILURE); }
    memcpy(host, h, len);
    host[len] = '\0';
    char *colon = strchr(host, ':'); if (colon) *colon = '\0';
    for (char *c = host; *c; ++c) *c = (char)tolower((unsigned char)*c);
    return host;
}

static double domain_trust(const char *url) {
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

static char *resolve_url(const char *base, const char *href) {
    if (!href || !*href) return NULL;
    if (!strncasecmp(href, "http://", 7) || !strncasecmp(href, "https://", 8))
        return strdup_safe(href);

    const char *p = strstr(base, "://");
    const char *hoststart = p ? p + 3 : base;
    const char *pathstart = strchr(hoststart, '/');
    size_t hostlen = pathstart ? (size_t)(pathstart - base) : strlen(base);
    char *root = malloc(hostlen + 1);
    if (!root) { perror("malloc"); exit(EXIT_FAILURE); }
    memcpy(root, base, hostlen); root[hostlen] = '\0';

    char *out = NULL;
    if (href[0] == '/') {
        if (asprintf(&out, "%s%s", root, href) < 0) out = NULL;
    } else {
        const char *lastslash = pathstart ? strrchr(base, '/') : NULL;
        if (lastslash) {
            size_t prefixlen = (size_t)(lastslash - base) + 1;
            char *prefix = malloc(prefixlen + 1);
            if (!prefix) { perror("malloc"); exit(EXIT_FAILURE); }
            memcpy(prefix, base, prefixlen); prefix[prefixlen] = '\0';
            if (asprintf(&out, "%s%s", prefix, href) < 0) out = NULL;
            free(prefix);
        } else {
            if (asprintf(&out, "%s/%s", root, href) < 0) out = NULL;
        }
    }
    free(root);
    return out;
}

/* ----------------------------- Heap (max-heap) for frontier ----------------------------- */

typedef struct crawl_item { char *url; char *norm; int depth; double score; int inbound; } crawl_item_t;
typedef struct heap { crawl_item_t *items; size_t size; size_t cap; } heap_t;

static void heap_init(heap_t *h) { h->items = NULL; h->size = h->cap = 0; }
static void heap_swap(crawl_item_t *a, crawl_item_t *b) { crawl_item_t t = *a; *a = *b; *b = t; }
static int heap_empty(const heap_t *h) { return h->size == 0; }

static void heap_push(heap_t *h, crawl_item_t it) {
    if (h->size + 1 > h->cap) {
        size_t ncap = h->cap ? h->cap * 2 : 64;
        crawl_item_t *n = realloc(h->items, ncap * sizeof(crawl_item_t));
        if (!n) { perror("realloc"); exit(EXIT_FAILURE); }
        h->items = n; h->cap = ncap;
    }
    size_t i = h->size++; h->items[i] = it;
    while (i > 0) { size_t p = (i-1)/2; if (h->items[p].score >= h->items[i].score) break; heap_swap(&h->items[p], &h->items[i]); i=p; }
}

static crawl_item_t heap_pop_max(heap_t *h) {
    if (!h->size) { fprintf(stderr,"heap_pop_max empty\n"); exit(EXIT_FAILURE); }
    crawl_item_t out = h->items[0]; h->items[0] = h->items[--h->size];
    size_t i=0;
    while (1) {
        size_t l=2*i+1, r=2*i+2, largest=i;
        if (l<h->size && h->items[l].score>h->items[largest].score) largest=l;
        if (r<h->size && h->items[r].score>h->items[largest].score) largest=r;
        if (largest==i) break;
        heap_swap(&h->items[i], &h->items[largest]); i=largest;
    }
    return out;
}

static void heap_free(heap_t *h) {
    for (size_t i=0;i<h->size;++i) { free(h->items[i].url); free(h->items[i].norm); }
    free(h->items); h->items=NULL; h->size=h->cap=0;
}

/* ----------------------------- Thread-safe heap ----------------------------- */

static int ts_heap_pop_max(heap_t *h, crawl_item_t *out) {
    pthread_mutex_lock(&g_frontier_lock);
    while (heap_empty(h) && !g_stop) pthread_cond_wait(&g_frontier_cond, &g_frontier_lock);
    if (g_stop && heap_empty(h)) { pthread_mutex_unlock(&g_frontier_lock); return 0; }
    if (heap_empty(h)) { pthread_mutex_unlock(&g_frontier_lock); return 0; }
    *out = heap_pop_max(h);
    pthread_mutex_unlock(&g_frontier_lock);
    return 1;
}

static void ts_heap_push(heap_t *h, crawl_item_t it) {
    pthread_mutex_lock(&g_frontier_lock);
    heap_push(h,it);
    pthread_cond_signal(&g_frontier_cond);
    pthread_mutex_unlock(&g_frontier_lock);
}

/* ----------------------------- SQLite helpers ----------------------------- */

static void db_open(const char *filename) {
    if (sqlite3_open(filename,&g_db)!=SQLITE_OK) { fprintf(stderr,"Cannot open DB %s: %s\n",filename,sqlite3_errmsg(g_db)); exit(EXIT_FAILURE); }
    const char *sql=
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
    char *err=NULL;
    if (sqlite3_exec(g_db,sql,NULL,NULL,&err)!=SQLITE_OK) { fprintf(stderr,"DB init error: %s\n",err); sqlite3_free(err); exit(EXIT_FAILURE);}
}

static void db_close(void){ if(g_db) sqlite3_close(g_db); }

static int db_insert_url(const char *url,const char *norm,int depth,double score) {
    sqlite3_stmt *stmt=NULL;
    const char *sql="INSERT OR IGNORE INTO urls(url,normalized,depth,score) VALUES(?,?,?,?);";
    if(sqlite3_prepare_v2(g_db,sql,-1,&stmt,NULL)!=SQLITE_OK) return 0;
    sqlite3_bind_text(stmt,1,url,-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,2,norm,-1,SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt,3,depth);
    sqlite3_bind_double(stmt,4,score);
    int rc=sqlite3_step(stmt); sqlite3_finalize(stmt);
    return rc==SQLITE_DONE && sqlite3_changes(g_db)>0;
}

static void db_mark_crawled_with_hash(const char *norm,const char *hash_hex) {
    sqlite3_stmt *stmt=NULL;
    const char *sql="UPDATE urls SET crawled=1,content_hash=? WHERE normalized=?;";
    if(sqlite3_prepare_v2(g_db,sql,-1,&stmt,NULL)!=SQLITE_OK) return;
    sqlite3_bind_text(stmt,1,hash_hex,-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,2,norm,-1,SQLITE_TRANSIENT);
    sqlite3_step(stmt); sqlite3_finalize(stmt);
}

static int db_has_norm(const char *norm) {
    sqlite3_stmt *stmt=NULL;
    const char *sql="SELECT 1 FROM urls WHERE normalized=? LIMIT 1;";
    int found=0;
    if(sqlite3_prepare_v2(g_db,sql,-1,&stmt,NULL)==SQLITE_OK) {
        sqlite3_bind_text(stmt,1,norm,-1,SQLITE_TRANSIENT);
        if(sqlite3_step(stmt)==SQLITE_ROW) found=1;
    }
    sqlite3_finalize(stmt);
    return found;
}

static int db_has_content_hash(const char *hash_hex) {
    sqlite3_stmt *stmt=NULL;
    const char *sql="SELECT 1 FROM urls WHERE content_hash=? LIMIT 1;";
    int found=0;
    if(sqlite3_prepare_v2(g_db,sql,-1,&stmt,NULL)==SQLITE_OK) {
        sqlite3_bind_text(stmt,1,hash_hex,-1,SQLITE_TRANSIENT);
        if(sqlite3_step(stmt)==SQLITE_ROW) found=1;
    }
    sqlite3_finalize(stmt);
    return found;
}

static int db_insert_email(const char *email,const char *source) {
    sqlite3_stmt *stmt=NULL;
    const char *sql="INSERT OR IGNORE INTO emails(email,source_url) VALUES(?,?);";
    if(sqlite3_prepare_v2(g_db,sql,-1,&stmt,NULL)!=SQLITE_OK) return 0;
    sqlite3_bind_text(stmt,1,email,-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,2,source,-1,SQLITE_TRANSIENT);
    int rc=sqlite3_step(stmt); sqlite3_finalize(stmt);
    return rc==SQLITE_DONE && sqlite3_changes(g_db)>0;
}

static int db_insert_phone(const char *phone,const char *source) {
    sqlite3_stmt *stmt=NULL;
    const char *sql="INSERT OR IGNORE INTO phones(phone,source_url) VALUES(?,?);";
    if(sqlite3_prepare_v2(g_db,sql,-1,&stmt,NULL)!=SQLITE_OK) return 0;
    sqlite3_bind_text(stmt,1,phone,-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,2,source,-1,SQLITE_TRANSIENT);
    int rc=sqlite3_step(stmt); sqlite3_finalize(stmt);
    return rc==SQLITE_DONE && sqlite3_changes(g_db)>0;
}

static int db_count_crawled(void){
    sqlite3_stmt *stmt=NULL;
    const char *sql="SELECT COUNT(*) FROM urls WHERE crawled=1;";
    int count=0;
    if(sqlite3_prepare_v2(g_db,sql,-1,&stmt,NULL)==SQLITE_OK)
        if(sqlite3_step(stmt)==SQLITE_ROW) count=sqlite3_column_int(stmt,0);
    sqlite3_finalize(stmt);
    return count;
}

/* ----------------------------- Thread-safe DB wrappers ----------------------------- */

static void db_insert_url_threadsafe(const char *url,const char *norm,int depth,double score) {
    pthread_mutex_lock(&g_db_lock);
    int inserted=db_insert_url(url,norm,depth,score);
    pthread_mutex_unlock(&g_db_lock);
    if(inserted) log_info("[NEW URL] queued score=%.3f url=%s",score,url);
}

static void db_mark_crawled_with_hash_threadsafe(const char *norm,const char *hash_hex){
    pthread_mutex_lock(&g_db_lock);
    db_mark_crawled_with_hash(norm,hash_hex);
    pthread_mutex_unlock(&g_db_lock);
}

static int db_has_norm_threadsafe(const char *norm){
    pthread_mutex_lock(&g_db_lock);
    int r=db_has_norm(norm);
    pthread_mutex_unlock(&g_db_lock);
    return r;
}

static int db_has_content_hash_threadsafe(const char *hash_hex){
    pthread_mutex_lock(&g_db_lock);
    int r=db_has_content_hash(hash_hex);
    pthread_mutex_unlock(&g_db_lock);
    return r;
}

static void db_insert_email_threadsafe(const char *email,const char *source){
    pthread_mutex_lock(&g_db_lock);
    int inserted=db_insert_email(email,source);
    pthread_mutex_unlock(&g_db_lock);
    if(inserted) log_info("[EMAIL] %s",email);
}

static void db_insert_phone_threadsafe(const char *phone,const char *source){
    pthread_mutex_lock(&g_db_lock);
    int inserted=db_insert_phone(phone,source);
    pthread_mutex_unlock(&g_db_lock);
    if(inserted) log_info("[PHONE] %s",phone);
}

/* ----------------------------- Frontier loading ----------------------------- */

static void load_uncrawled_frontier(heap_t *frontier){
    pthread_mutex_lock(&g_db_lock);
    sqlite3_stmt *stmt=NULL;
    const char *sql="SELECT url, normalized, depth, score, inbound FROM urls WHERE crawled=0;";
    if(sqlite3_prepare_v2(g_db,sql,-1,&stmt,NULL)==SQLITE_OK){
        while(sqlite3_step(stmt)==SQLITE_ROW){
            crawl_item_t it;
            const unsigned char *u=sqlite3_column_text(stmt,0);
            const unsigned char *n=sqlite3_column_text(stmt,1);
            it.url=strdup_safe((const char *)u);
            it.norm=strdup_safe((const char *)n);
            it.depth=sqlite3_column_int(stmt,2);
            it.score=sqlite3_column_double(stmt,3);
            it.inbound=sqlite3_column_int(stmt,4);
            ts_heap_push(frontier,it);
        }
    }
    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&g_db_lock);
}

/* ----------------------------- URL extraction ----------------------------- */

static void extract_links(const char *html,const char *base,heap_t *frontier,int depth){
    htmlDocPtr doc=htmlReadMemory(html,strlen(html),base,NULL,HTML_PARSE_NOERROR|HTML_PARSE_NOWARNING);
    if(!doc) return;
    xmlXPathContextPtr xpath=xmlXPathNewContext(doc);
    xmlXPathObjectPtr obj=xmlXPathEvalExpression((xmlChar *)"//a/@href",xpath);
    if(obj && obj->nodesetval){
        for(int i=0;i<obj->nodesetval->nodeNr;++i){
            xmlNodePtr node=obj->nodesetval->nodeTab[i];
            if(node && node->children){
                char *href=(char*)node->children->content;
                char *resolved=resolve_url(base,href);
                if(resolved){
                    char *norm=normalize_url(resolved);
                    if(norm && !db_has_norm_threadsafe(norm)){
                        double score=domain_trust(resolved);
                        db_insert_url_threadsafe(resolved,norm,depth,score);
                        ts_heap_push(frontier,(crawl_item_t){.url=resolved,.norm=norm,.depth=depth,.score=score,.inbound=0});
                    } else { free(resolved); free(norm); }
                }
            }
        }
    }
    if(obj) xmlXPathFreeObject(obj);
    if(xpath) xmlXPathFreeContext(xpath);
    xmlFreeDoc(doc);
}

/* ----------------------------- Email & Phone extraction ----------------------------- */

static void extract_emails_phones(const char *html,const char *url){
    regex_t re_email,re_phone;
    regcomp(&re_email,"[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}",REG_EXTENDED);
    regcomp(&re_phone,"\\+?[0-9][0-9\\-\\s]{7,}[0-9]",REG_EXTENDED);

    const char *p=html; regmatch_t m;
    while(regexec(&re_email,p,1,&m,0)==0){
        size_t len=m.rm_eo-m.rm_so; char buf[len+1]; memcpy(buf,p+m.rm_so,len); buf[len]='\0';
        db_insert_email_threadsafe(buf,url); p+=m.rm_eo;
    }
    p=html;
    while(regexec(&re_phone,p,1,&m,0)==0){
        size_t len=m.rm_eo-m.rm_so; char buf[len+1]; memcpy(buf,p+m.rm_so,len); buf[len]='\0';
        db_insert_phone_threadsafe(buf,url); p+=m.rm_eo;
    }

    regfree(&re_email); regfree(&re_phone);
}

/* ----------------------------- Worker ----------------------------- */

typedef struct worker_args { heap_t *frontier; int max_pages; } worker_args_t;

static void *worker_thread(void *arg){
    worker_args_t *wa=(worker_args_t*)arg;
    heap_t *frontier=wa->frontier;
    CURL *curl=curl_easy_init();
    if(!curl){ log_info("CURL init failed"); return NULL; }

    while(!g_stop && db_count_crawled()<wa->max_pages){
        crawl_item_t item;
        if(!ts_heap_pop_max(frontier,&item)) break;

        log_info("[CRAWL] depth=%d url=%s",item.depth,item.url);

        dynbuf_t buf; dbuf_init(&buf);
        curl_easy_setopt(curl,CURLOPT_URL,item.url);
        curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
        curl_easy_setopt(curl,CURLOPT_USERAGENT,USER_AGENT);
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,dbuf_write);
        curl_easy_setopt(curl,CURLOPT_WRITEDATA,&buf);
        CURLcode res=curl_easy_perform(curl);
        if(res==CURLE_OK){
            char *hash=sha256_hex((unsigned char*)buf.data,buf.len);
            if(!db_has_content_hash_threadsafe(hash)){
                db_mark_crawled_with_hash_threadsafe(item.norm,hash);
                extract_emails_phones(buf.data,item.url);
                if(item.depth<g_max_depth) extract_links(buf.data,item.url,frontier,item.depth+1);
            } else db_mark_crawled_with_hash_threadsafe(item.norm,hash);
            free(hash);
        } else {
            log_info("[FAILED] %s : %s",item.url,curl_easy_strerror(res));
        }
        dbuf_free(&buf);
        //sleep_polite();
        free(item.url); free(item.norm);
    }
    curl_easy_cleanup(curl);
    return NULL;
}

/* ----------------------------- Main ----------------------------- */

int main(int argc,char **argv){
    const char *seed=argc>1?argv[1]:"http://usa.gov";
    int max_pages=argc>2?atoi(argv[2]):DEFAULT_MAX_PAGES;
    g_max_depth=argc>3?atoi(argv[3]):DEFAULT_MAX_DEPTH;
    g_workers=argc>4?atoi(argv[4]):DEFAULT_WORKERS;

    signal(SIGINT,handle_sigint);

    curl_global_init(CURL_GLOBAL_ALL);
    xmlInitParser();
    db_open(DB_FILENAME);

    heap_t frontier; heap_init(&frontier);
    load_uncrawled_frontier(&frontier);

    if(heap_empty(&frontier)){
        char *norm=normalize_url(seed);
        double score=domain_trust(seed);
        db_insert_url_threadsafe(seed,norm,0,score);
        ts_heap_push(&frontier,(crawl_item_t){.url=strdup_safe(seed),.norm=norm,.depth=0,.score=score,.inbound=0});
    }

    pthread_t threads[g_workers];
    worker_args_t wa={.frontier=&frontier,.max_pages=max_pages};
    for(int i=0;i<g_workers;++i) pthread_create(&threads[i],NULL,worker_thread,&wa);
    for(int i=0;i<g_workers;++i) pthread_join(threads[i],NULL);

    heap_free(&frontier);
    db_close();
    xmlCleanupParser();
    curl_global_cleanup();
    log_info("Crawl finished.");
    return 0;
}
