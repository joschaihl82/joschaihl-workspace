// crawler.cpp
// Single-file Qt GUI crawler (widgets only).
// - Left: URL table (narrow).
// - Right: HTML preview (scaled via CSS wrapper to simulate zoom).
// - Bottom: progress bar and remaining time.
// Build with qmake (QT += widgets) or compile with pkg-config for Qt5/Qt6 widgets.
// Requires: libcurl, libxml2, sqlite3, openssl.

#include <QtWidgets>
#include <QApplication>
#include <QMainWindow>
#include <QTextBrowser>
#include <QTableWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTimer>

#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>
#include <openssl/sha.h>
#include <sqlite3.h>

#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <iomanip>
#include <regex>
#include <cstring>
#include <strings.h>

using namespace std;

/* ----------------------------- Configuration ----------------------------- */

static const int DEFAULT_MAX_PAGES = 200;
static const int DEFAULT_MAX_DEPTH = 3;
static const int DEFAULT_WORKERS = 8;
static const int POLITE_DELAY_MS_DEFAULT = 100;
static const char *USER_AGENT = "AddressCrawlerCLI/1.0";
static const char *DB_FILENAME = "crawler.db";

/* ----------------------------- Globals & forward decls ----------------------------- */

static std::atomic<int> g_polite_delay_ms(POLITE_DELAY_MS_DEFAULT);

class QtCrawlerBridge;
extern QtCrawlerBridge *g_bridge;

void bridge_notifyNewUrl(const std::string &url, const std::string &norm, int depth, double score);
void bridge_notifyNewEmail(const std::string &email, const std::string &source);
void bridge_notifyNewPhone(const std::string &phone, const std::string &source);
void bridge_notifyPageCrawled(const std::string &url, const std::string &norm, int depth, double score, const std::string &title);
void bridge_notifyHtmlFetched(const std::string &url, const std::string &html);

/* ----------------------------- Utility helpers ----------------------------- */

static void sleep_polite() {
    int ms = g_polite_delay_ms.load(std::memory_order_relaxed);
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

static string sha256_hex(const unsigned char *data, size_t len) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, data, len);
    SHA256_Final(hash, &ctx);
    std::ostringstream oss;
    oss << hex << setfill('0');
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        oss << setw(2) << (int)hash[i];
    return oss.str();
}

static string normalize_url(const string &url) {
    if (url.empty()) return string();
    string copy = url;
    auto p = copy.find('#');
    if (p != string::npos) copy.resize(p);
    while (copy.size() > 1 && copy.back() == '/') copy.pop_back();
    return copy;
}

static string resolve_url(const string &base, const string &href) {
    if (href.empty()) return string();
    if (strncasecmp(href.c_str(), "http://", 7) == 0 || strncasecmp(href.c_str(), "https://", 8) == 0)
        return href;
    auto p = base.find("://");
    if (p == string::npos) return href;
    const char *hoststart = base.c_str() + p + 3;
    const char *pathstart = strchr(hoststart, '/');
    size_t hostlen = pathstart ? (size_t)(pathstart - base.c_str()) : base.size();
    string root = base.substr(0, hostlen);
    if (!href.empty() && href[0] == '/') {
        return root + href;
    } else {
        const char *lastslash = pathstart ? strrchr(base.c_str(), '/') : NULL;
        if (lastslash) {
            size_t prefixlen = (size_t)(lastslash - base.c_str()) + 1;
            string prefix = base.substr(0, prefixlen);
            return prefix + href;
        } else {
            return root + "/" + href;
        }
    }
}

/* ----------------------------- Simple frontier & DB (minimal) ----------------------------- */

struct CrawlItem { string url, norm; int depth; double score; int inbound; };

struct Heap {
    vector<CrawlItem> items;
    void push(CrawlItem it) {
        items.push_back(move(it));
        push_heap(items.begin(), items.end(), [](const CrawlItem &a, const CrawlItem &b){ return a.score < b.score; });
    }
    bool empty() const { return items.empty(); }
    CrawlItem pop_max() {
        pop_heap(items.begin(), items.end(), [](const CrawlItem &a, const CrawlItem &b){ return a.score < b.score; });
        CrawlItem it = move(items.back());
        items.pop_back();
        return it;
    }
    size_t size() const { return items.size(); }
};

class Frontier {
public:
    void push(CrawlItem it) { unique_lock<mutex> lk(mtx); heap.push(move(it)); cv.notify_one(); }
    bool pop(CrawlItem &out) { unique_lock<mutex> lk(mtx); while (heap.empty() && !stop_flag) cv.wait(lk); if (stop_flag && heap.empty()) return false; out = heap.pop_max(); return true; }
    void stop() { unique_lock<mutex> lk(mtx); stop_flag = true; cv.notify_all(); }
    size_t queued() { unique_lock<mutex> lk(mtx); return heap.size(); }
private:
    Heap heap; mutex mtx; condition_variable cv; bool stop_flag = false;
};

class DB {
public:
    DB(): db(nullptr) {}
    ~DB(){ close(); }
    void open(const char *filename) {
        if (sqlite3_open(filename, &db) != SQLITE_OK) { cerr << "Cannot open DB: " << sqlite3_errmsg(db) << endl; exit(EXIT_FAILURE); }
        const char *sql =
            "PRAGMA journal_mode=WAL;"
            "CREATE TABLE IF NOT EXISTS urls ( id INTEGER PRIMARY KEY, url TEXT UNIQUE, normalized TEXT UNIQUE, depth INTEGER, score REAL, inbound INTEGER DEFAULT 0, crawled INTEGER DEFAULT 0, content_hash TEXT );"
            "CREATE TABLE IF NOT EXISTS emails (id INTEGER PRIMARY KEY, email TEXT UNIQUE, source_url TEXT);"
            "CREATE TABLE IF NOT EXISTS phones (id INTEGER PRIMARY KEY, phone TEXT UNIQUE, source_url TEXT);";
        char *err = nullptr;
        if (sqlite3_exec(db, sql, NULL, NULL, &err) != SQLITE_OK) { cerr << "DB init error: " << err << endl; sqlite3_free(err); exit(EXIT_FAILURE); }
    }
    void close(){ if (db) sqlite3_close(db); db = nullptr; }
    bool insert_url(const string &url, const string &norm, int depth, double score) {
        lock_guard<mutex> lk(db_mtx);
        sqlite3_stmt *stmt = nullptr;
        const char *sql = "INSERT OR IGNORE INTO urls (url, normalized, depth, score) VALUES (?, ?, ?, ?);";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return false;
        sqlite3_bind_text(stmt, 1, url.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, norm.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, depth);
        sqlite3_bind_double(stmt, 4, score);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return (rc == SQLITE_DONE && sqlite3_changes(db) > 0);
    }
    void mark_crawled_with_hash(const string &norm, const string &hash_hex) {
        lock_guard<mutex> lk(db_mtx);
        sqlite3_stmt *stmt = nullptr;
        const char *sql = "UPDATE urls SET crawled = 1, content_hash = ? WHERE normalized = ?;";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return;
        sqlite3_bind_text(stmt, 1, hash_hex.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, norm.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    bool has_norm(const string &norm) {
        lock_guard<mutex> lk(db_mtx);
        sqlite3_stmt *stmt = nullptr;
        const char *sql = "SELECT 1 FROM urls WHERE normalized = ? LIMIT 1;";
        bool found = false;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, norm.c_str(), -1, SQLITE_TRANSIENT);
            if (sqlite3_step(stmt) == SQLITE_ROW) found = true;
        }
        sqlite3_finalize(stmt);
        return found;
    }
    bool insert_email(const string &email, const string &source) {
        lock_guard<mutex> lk(db_mtx);
        sqlite3_stmt *stmt = nullptr;
        const char *sql = "INSERT OR IGNORE INTO emails (email, source_url) VALUES (?, ?);";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return false;
        sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, source.c_str(), -1, SQLITE_TRANSIENT);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return (rc == SQLITE_DONE && sqlite3_changes(db) > 0);
    }
    bool insert_phone(const string &phone, const string &source) {
        lock_guard<mutex> lk(db_mtx);
        sqlite3_stmt *stmt = nullptr;
        const char *sql = "INSERT OR IGNORE INTO phones (phone, source_url) VALUES (?, ?);";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return false;
        sqlite3_bind_text(stmt, 1, phone.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, source.c_str(), -1, SQLITE_TRANSIENT);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return (rc == SQLITE_DONE && sqlite3_changes(db) > 0);
    }
    int count_crawled() {
        lock_guard<mutex> lk(db_mtx);
        sqlite3_stmt *stmt = nullptr;
        const char *sql = "SELECT COUNT(*) FROM urls WHERE crawled = 1;";
        int count = 0;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) count = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
        return count;
    }

    // Added: check whether a content hash already exists in DB
    bool has_content_hash(const string &hash_hex) {
        lock_guard<mutex> lk(db_mtx);
        sqlite3_stmt *stmt = nullptr;
        const char *sql = "SELECT 1 FROM urls WHERE content_hash = ? LIMIT 1;";
        bool found = false;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, hash_hex.c_str(), -1, SQLITE_TRANSIENT);
            if (sqlite3_step(stmt) == SQLITE_ROW) found = true;
        }
        sqlite3_finalize(stmt);
        return found;
    }

private:
    sqlite3 *db;
    mutex db_mtx;
};

/* ----------------------------- Simple extractors ----------------------------- */

static string normalize_phone(const string &raw) {
    string out; out.reserve(raw.size());
    for (char c : raw) if ((c == '+' && out.empty()) || isdigit((unsigned char)c)) out.push_back(c);
    return out;
}

static void extract_emails(const string &text, const string &source, DB &db) {
    try {
        static const regex re(R"(([A-Za-z0-9._%+\-]+@[A-Za-z0-9.\-]+\.[A-Za-z]{2,}))", regex::icase);
        for (sregex_iterator it(text.begin(), text.end(), re), end; it != end; ++it) {
            string email = (*it)[1].str();
            transform(email.begin(), email.end(), email.begin(), [](unsigned char c){ return tolower(c); });
            if (db.insert_email(email, source)) bridge_notifyNewEmail(email, source);
        }
    } catch(...) {}
}

static void extract_phones(const string &text, const string &source, DB &db) {
    try {
        static const regex re(R"((\+?[0-9][0-9\-\.\s\(\)]{6,}[0-9]))", regex::icase);
        for (sregex_iterator it(text.begin(), text.end(), re), end; it != end; ++it) {
            string raw = (*it)[1].str();
            string norm = normalize_phone(raw);
            if (!norm.empty() && db.insert_phone(norm, source)) bridge_notifyNewPhone(norm, source);
        }
    } catch(...) {}
}

/* ----------------------------- Minimal HTML parse & href processing ----------------------------- */

static void process_href(const string &href, const string &base, int base_depth, Frontier &frontier, DB &db) {
    if (href.empty()) return;
    if (strncasecmp(href.c_str(), "mailto:", 7) == 0) {
        string after = href.substr(7);
        auto q = after.find('?'); if (q != string::npos) after.resize(q);
        size_t pos = 0;
        while (pos < after.size()) {
            size_t comma = after.find(',', pos);
            string tok = (comma == string::npos) ? after.substr(pos) : after.substr(pos, comma - pos);
            transform(tok.begin(), tok.end(), tok.begin(), [](unsigned char c){ return tolower(c); });
            if (db.insert_email(tok, base)) bridge_notifyNewEmail(tok, base);
            if (comma == string::npos) break;
            pos = comma + 1;
        }
        return;
    }
    if (strncasecmp(href.c_str(), "tel:", 4) == 0) {
        string after = href.substr(4);
        auto q = after.find('?'); if (q != string::npos) after.resize(q);
        string norm = normalize_phone(after);
        if (!norm.empty() && db.insert_phone(norm, base)) bridge_notifyNewPhone(norm, base);
        return;
    }
    string resolved = resolve_url(base, href);
    if (resolved.empty()) return;
    string norm = normalize_url(resolved);
    if (norm.empty()) return;
    if (!db.has_norm(norm) && base_depth + 1 <= DEFAULT_MAX_DEPTH) {
        double dt = 1.0; // simplified trust
        int inbound = 0;
        int pathlen = (int)resolved.size();
        double score = 2.0 * dt + (-1.0) * (base_depth + 1) + 1.0 * inbound + (-0.5) * (pathlen / 10.0);
        CrawlItem it; it.url = resolved; it.norm = norm; it.depth = base_depth + 1; it.score = score; it.inbound = inbound;
        frontier.push(it);
        if (db.insert_url(resolved, norm, it.depth, it.score)) bridge_notifyNewUrl(resolved, norm, it.depth, it.score);
    }
}

static void parse_html_and_extract(const string &html, const string &base, int base_depth, Frontier &frontier, DB &db, const string &source_url) {
    if (html.empty()) return;
    // quick regex pre-scan for emails/phones/hrefs (faster than full parse)
    try {
        static const regex re_email(R"(([A-Za-z0-9._%+\-]+@[A-Za-z0-9.\-]+\.[A-Za-z]{2,}))", regex::icase);
        for (sregex_iterator it(html.begin(), html.end(), re_email), end; it != end; ++it) {
            string email = (*it)[1].str();
            transform(email.begin(), email.end(), email.begin(), [](unsigned char c){ return tolower(c); });
            if (db.insert_email(email, source_url)) bridge_notifyNewEmail(email, source_url);
        }
        static const regex re_phone(R"((\+?[0-9][0-9\-\.\s\(\)]{6,}[0-9]))", regex::icase);
        for (sregex_iterator it(html.begin(), html.end(), re_phone), end; it != end; ++it) {
            string raw = (*it)[1].str();
            string norm = normalize_phone(raw);
            if (!norm.empty() && db.insert_phone(norm, source_url)) bridge_notifyNewPhone(norm, source_url);
        }
        static const regex re_href(R"((?:href|HREF)\s*=\s*['\"]([^'\"]+)['\"])", regex::icase);
        for (sregex_iterator it(html.begin(), html.end(), re_href), end; it != end; ++it) {
            string href = (*it)[1].str();
            process_href(href, base, base_depth, frontier, db);
        }
    } catch(...) {}
    // For completeness, run libxml on small pages
    if (html.size() < 200000) {
        htmlDocPtr doc = htmlReadMemory(html.c_str(), (int)html.size(), base.c_str(), NULL, HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_RECOVER);
        if (!doc) return;
        xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
        if (!xpathCtx) { xmlFreeDoc(doc); return; }
        xmlXPathObjectPtr hrefObj = xmlXPathEvalExpression((xmlChar*)"//@href", xpathCtx);
        if (hrefObj && hrefObj->nodesetval) {
            xmlNodeSetPtr nodes = hrefObj->nodesetval;
            for (int i = 0; i < nodes->nodeNr; ++i) {
                xmlAttrPtr attr = (xmlAttrPtr)nodes->nodeTab[i];
                if (attr && attr->children && attr->children->content) {
                    const char *href = (const char *)attr->children->content;
                    if (href) process_href(string(href), base, base_depth, frontier, db);
                }
            }
        }
        if (hrefObj) xmlXPathFreeObject(hrefObj);
        xmlXPathFreeContext(xpathCtx);
        xmlFreeDoc(doc);
    }
}

/* ----------------------------- Curl fetch (simple) ----------------------------- */

struct FetchMeta { string content_type; long response_code = 0; };
struct DynBuf { string data; };

static size_t curl_write_cb(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t add = size * nmemb;
    DynBuf *b = (DynBuf *)userdata;
    b->data.append((char*)ptr, add);
    return add;
}

static size_t header_cb(char *buffer, size_t size, size_t nitems, void *userdata) {
    size_t len = size * nitems;
    FetchMeta *m = (FetchMeta *)userdata;
    const char prefix[] = "Content-Type:";
    if (len > strlen(prefix) && strncasecmp(buffer, prefix, strlen(prefix)) == 0) {
        const char *p = buffer + strlen(prefix);
        while (*p && isspace((unsigned char)*p)) p++;
        m->content_type = string(p, len - (p - buffer));
        while (!m->content_type.empty() && (m->content_type.back() == '\r' || m->content_type.back() == '\n')) m->content_type.pop_back();
    }
    return len;
}

static bool fetch_url(CURL *curl, const string &url, DynBuf &out, FetchMeta &meta) {
    out.data.clear(); meta.content_type.clear(); meta.response_code = 0;
    curl_easy_reset(curl);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_cb);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &meta);
    CURLcode rc = curl_easy_perform(curl);
    if (rc != CURLE_OK) return false;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &meta.response_code);
    if (!meta.content_type.empty() && meta.content_type.find("text/html") == string::npos) return false;
    return true;
}

/* ----------------------------- Crawler engine (simple) ----------------------------- */

class CrawlerEngine {
public:
    CrawlerEngine()
        : frontier(), db(), workers(DEFAULT_WORKERS), max_pages(DEFAULT_MAX_PAGES), max_depth(DEFAULT_MAX_DEPTH),
          stop_flag(false) { curl_global_init(CURL_GLOBAL_ALL); }
    ~CrawlerEngine() { stop(); curl_global_cleanup(); }

    void init_db(const string &filename) { db.open(filename.c_str()); }
    void enqueue_seed(const string &seed) {
        string seed_norm = normalize_url(seed);
        double seed_score = 1.0;
        CrawlItem seed_item; seed_item.url = seed; seed_item.norm = seed_norm; seed_item.depth = 0; seed_item.score = seed_score; seed_item.inbound = 0;
        frontier.push(seed_item);
        db.insert_url(seed, seed_norm, 0, seed_score);
        bridge_notifyNewUrl(seed, seed_norm, 0, seed_score);
    }
    void start(int worker_count = DEFAULT_WORKERS, int maxPages = DEFAULT_MAX_PAGES, int maxDepth = DEFAULT_MAX_DEPTH) {
        workers = worker_count; max_pages = maxPages; max_depth = maxDepth; stop_flag = false;
        for (int i = 0; i < workers; ++i) threads.emplace_back(&CrawlerEngine::worker_loop, this, i);
    }
    void stop() { stop_flag = true; frontier.stop(); for (auto &t : threads) if (t.joinable()) t.join(); threads.clear(); }
    size_t queued() { return frontier.queued(); }
    int crawled_count() { return db.count_crawled(); }
    int get_max_pages() const { return max_pages; }

private:
    void worker_loop(int /*worker_id*/) {
        CURL *curl = curl_easy_init();
        if (!curl) return;
        DynBuf out;
        while (!stop_flag) {
            CrawlItem it;
            if (!frontier.pop(it)) break;
            sleep_polite();
            FetchMeta meta;
            bool ok = fetch_url(curl, it.url, out, meta);
            if (!ok) { db.mark_crawled_with_hash(it.norm, ""); continue; }
            string hash = sha256_hex((const unsigned char*)out.data.data(), out.data.size());
            if (!hash.empty() && db.has_content_hash(hash)) { db.mark_crawled_with_hash(it.norm, hash); continue; }
            parse_html_and_extract(out.data, it.url, it.depth, frontier, db, it.url);
            db.mark_crawled_with_hash(it.norm, hash);

            // title extraction (quick)
            string title;
            htmlDocPtr doc = htmlReadMemory(out.data.c_str(), (int)out.data.size(), it.url.c_str(), NULL, HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_RECOVER);
            if (doc) {
                xmlXPathContextPtr ctx = xmlXPathNewContext(doc);
                if (ctx) {
                    xmlXPathObjectPtr obj = xmlXPathEvalExpression((xmlChar*)"//title", ctx);
                    if (obj && obj->nodesetval && obj->nodesetval->nodeNr > 0) {
                        xmlNodePtr node = obj->nodesetval->nodeTab[0];
                        if (node && node->children && node->children->content) title = (const char*)node->children->content;
                    }
                    if (obj) xmlXPathFreeObject(obj);
                    xmlXPathFreeContext(ctx);
                }
                xmlFreeDoc(doc);
            }

            bridge_notifyPageCrawled(it.url, it.norm, it.depth, it.score, title);
            bridge_notifyHtmlFetched(it.url, out.data);

            if (db.count_crawled() >= max_pages) { stop_flag = true; frontier.stop(); break; }
        }
        curl_easy_cleanup(curl);
    }

    Frontier frontier;
    DB db;
    int workers;
    int max_pages;
    int max_depth;
    vector<thread> threads;
    atomic<bool> stop_flag;
};

/* ----------------------------- Qt bridge ----------------------------- */

class QtCrawlerBridge : public QObject {
    Q_OBJECT
public:
    explicit QtCrawlerBridge(QObject *parent = nullptr) : QObject(parent) { g_bridge = this; }
    ~QtCrawlerBridge() { g_bridge = nullptr; }

    Q_INVOKABLE void notifyNewUrl(const QString &url, const QString &norm, int depth, double score) { emit newUrl(url, norm, depth, score); }
    Q_INVOKABLE void notifyNewEmail(const QString &email, const QString &source) { emit newEmail(email, source); }
    Q_INVOKABLE void notifyNewPhone(const QString &phone, const QString &source) { emit newPhone(phone, source); }
    Q_INVOKABLE void notifyPageCrawled(const QString &url, const QString &norm, int depth, double score, const QString &title) { emit pageCrawled(url, norm, depth, score, title); }
    Q_INVOKABLE void notifyHtmlFetched(const QString &url, const QString &html) { emit htmlFetched(url, html); }

signals:
    void pageCrawled(QString url, QString norm, int depth, double score, QString title);
    void htmlFetched(QString url, QString html);
    void newUrl(QString url, QString norm, int depth, double score);
    void newEmail(QString email, QString source);
    void newPhone(QString phone, QString source);
};

QtCrawlerBridge *g_bridge = nullptr;

void bridge_notifyNewUrl(const std::string &url, const std::string &norm, int depth, double score) {
    if (!g_bridge) return;
    QMetaObject::invokeMethod(g_bridge, "notifyNewUrl", Qt::QueuedConnection,
                              Q_ARG(QString, QString::fromStdString(url)),
                              Q_ARG(QString, QString::fromStdString(norm)),
                              Q_ARG(int, depth),
                              Q_ARG(double, score));
}
void bridge_notifyNewEmail(const std::string &email, const std::string &source) {
    if (!g_bridge) return;
    QMetaObject::invokeMethod(g_bridge, "notifyNewEmail", Qt::QueuedConnection,
                              Q_ARG(QString, QString::fromStdString(email)),
                              Q_ARG(QString, QString::fromStdString(source)));
}
void bridge_notifyNewPhone(const std::string &phone, const std::string &source) {
    if (!g_bridge) return;
    QMetaObject::invokeMethod(g_bridge, "notifyNewPhone", Qt::QueuedConnection,
                              Q_ARG(QString, QString::fromStdString(phone)),
                              Q_ARG(QString, QString::fromStdString(source)));
}
void bridge_notifyPageCrawled(const std::string &url, const std::string &norm, int depth, double score, const std::string &title) {
    if (!g_bridge) return;
    QMetaObject::invokeMethod(g_bridge, "notifyPageCrawled", Qt::QueuedConnection,
                              Q_ARG(QString, QString::fromStdString(url)),
                              Q_ARG(QString, QString::fromStdString(norm)),
                              Q_ARG(int, depth),
                              Q_ARG(double, score),
                              Q_ARG(QString, QString::fromStdString(title)));
}
void bridge_notifyHtmlFetched(const std::string &url, const std::string &html) {
    if (!g_bridge) return;
    QMetaObject::invokeMethod(g_bridge, "notifyHtmlFetched", Qt::QueuedConnection,
                              Q_ARG(QString, QString::fromStdString(url)),
                              Q_ARG(QString, QString::fromStdString(html)));
}

/* ----------------------------- MainWindow: left table small + HTML scaled to 25% ----------------------------- */

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent), engine(nullptr), bridge(new QtCrawlerBridge(this)) {
        setupUi();
        connectSignals();
    }
    ~MainWindow() {
        if (engine) { engine->stop(); delete engine; engine = nullptr; }
    }

private slots:
    void onStartClicked() {
        if (engine) { engine->stop(); delete engine; engine = nullptr; }
        engine = new CrawlerEngine();
        engine->init_db(DB_FILENAME);
        string seed = seedEdit->text().toStdString();
        if (seed.empty()) { QMessageBox::warning(this, "Seed required", "Please enter a seed URL."); delete engine; engine = nullptr; return; }
        engine->enqueue_seed(seed);
        int workers = workersSpin->value();
        int maxPages = maxPagesSpin->value();
        g_polite_delay_ms.store(delaySpin->value());
        engine->start(workers, maxPages, DEFAULT_MAX_DEPTH);
        uiStart->setEnabled(false);
        uiStop->setEnabled(true);
        progressTimer->start(500);
    }

    void onStopClicked() {
        if (engine) { engine->stop(); delete engine; engine = nullptr; }
        uiStart->setEnabled(true);
        uiStop->setEnabled(false);
        progressTimer->stop();
    }

    void onPageCrawled(QString url, QString norm, int depth, double score, QString title) {
        Q_UNUSED(norm); Q_UNUSED(depth); Q_UNUSED(score); Q_UNUSED(title);
        int row = urlTable->rowCount();
        urlTable->insertRow(row);
        urlTable->setItem(row, 0, new QTableWidgetItem(url));
        QFont f = urlTable->font();
        f.setPointSize(8);
        urlTable->setFont(f);
        urlTable->scrollToBottom();
    }

    void onHtmlFetched(QString url, QString html) {
        // Wrap HTML in a scaling container to simulate 25% zoom.
        // Using CSS transform: scale(0.25) with transform-origin top-left.
        // Also set width to 400% so content fits the scaled container.
        QString wrapped = QString(
            "<!doctype html><html><head><meta charset='utf-8'>"
            "<style>body{margin:0;padding:0;} .scale-wrap{transform:scale(0.25);transform-origin:0 0;width:400%;}</style>"
            "</head><body><div class='scale-wrap'>%1</div></body></html>"
        ).arg(QString::fromUtf8(html.toUtf8()));
        htmlView->setHtml(wrapped);
        currentUrlLabel->setText(url);
    }

    void refreshProgress() {
        if (!engine) return;
        int crawled = engine->crawled_count();
        int maxp = engine->get_max_pages();
        if (maxp <= 0) maxp = 1;
        int percent = (int)((100.0 * crawled) / maxp);
        progressBar->setValue(percent);
        remainingLabel->setText(QString("Crawled: %1 / %2").arg(crawled).arg(maxp));
        if (crawled >= maxp) {
            progressTimer->stop();
            uiStart->setEnabled(true);
            uiStop->setEnabled(false);
        }
    }

private:
    void setupUi() {
        QWidget *central = new QWidget(this);
        QVBoxLayout *mainLayout = new QVBoxLayout(central);

        // Top controls
        QHBoxLayout *top = new QHBoxLayout();
        seedEdit = new QLineEdit("https://example.com");
        seedEdit->setMinimumWidth(300);
        maxPagesSpin = new QSpinBox(); maxPagesSpin->setRange(1, 100000); maxPagesSpin->setValue(DEFAULT_MAX_PAGES);
        workersSpin = new QSpinBox(); workersSpin->setRange(1, 64); workersSpin->setValue(DEFAULT_WORKERS);
        delaySpin = new QSpinBox(); delaySpin->setRange(0, 2000); delaySpin->setValue(POLITE_DELAY_MS_DEFAULT);
        uiStart = new QPushButton("Start");
        uiStop = new QPushButton("Stop"); uiStop->setEnabled(false);

        top->addWidget(new QLabel("Seed:"));
        top->addWidget(seedEdit);
        top->addWidget(new QLabel("Max pages:"));
        top->addWidget(maxPagesSpin);
        top->addWidget(new QLabel("Workers:"));
        top->addWidget(workersSpin);
        top->addWidget(new QLabel("Delay ms:"));
        top->addWidget(delaySpin);
        top->addWidget(uiStart);
        top->addWidget(uiStop);

        mainLayout->addLayout(top);

        // Splitter: left table (small) + right HTML preview (large)
        QSplitter *split = new QSplitter(Qt::Horizontal);

        // Left: URL table (make it visually small)
        QWidget *leftWidget = new QWidget();
        QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
        urlTable = new QTableWidget(0, 1);
        urlTable->setHorizontalHeaderLabels(QStringList() << "URLs");
        urlTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        urlTable->verticalHeader()->setVisible(false);
        urlTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        urlTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        urlTable->setShowGrid(false);
        urlTable->setAlternatingRowColors(true);
        QFont tf = urlTable->font(); tf.setPointSize(8); urlTable->setFont(tf);
        leftLayout->addWidget(urlTable);
        split->addWidget(leftWidget);

        // Right: HTML preview
        QWidget *rightWidget = new QWidget();
        QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
        htmlView = new QTextBrowser();
        htmlView->setOpenExternalLinks(true);
        rightLayout->addWidget(htmlView);
        split->addWidget(rightWidget);

        // Make left side smaller: set stretch factors so left is narrow
        split->setStretchFactor(0, 1);
        split->setStretchFactor(1, 6);

        // Additionally set a maximum width for the left panel to ensure it's small
        leftWidget->setMaximumWidth(360); // keep URL table narrow

        mainLayout->addWidget(split, 1);

        // Bottom: progress and labels
        QHBoxLayout *bottom = new QHBoxLayout();
        progressBar = new QProgressBar();
        progressBar->setRange(0, 100);
        progressBar->setValue(0);
        remainingLabel = new QLabel("Idle");
        currentUrlLabel = new QLabel("(idle)");
        bottom->addWidget(progressBar, 3);
        bottom->addWidget(remainingLabel, 1);
        bottom->addWidget(currentUrlLabel, 2);
        mainLayout->addLayout(bottom);

        setCentralWidget(central);
        setWindowTitle("AddressCrawler — Narrow URL table + scaled HTML preview");

        progressTimer = new QTimer(this);
        connect(progressTimer, &QTimer::timeout, this, &MainWindow::refreshProgress);
    }

    void connectSignals() {
        connect(uiStart, &QPushButton::clicked, this, &MainWindow::onStartClicked);
        connect(uiStop, &QPushButton::clicked, this, &MainWindow::onStopClicked);
        connect(bridge, &QtCrawlerBridge::pageCrawled, this, &MainWindow::onPageCrawled);
        connect(bridge, &QtCrawlerBridge::htmlFetched, this, &MainWindow::onHtmlFetched);
    }

    // UI
    QLineEdit *seedEdit;
    QSpinBox *maxPagesSpin;
    QSpinBox *workersSpin;
    QSpinBox *delaySpin;
    QPushButton *uiStart;
    QPushButton *uiStop;
    QTableWidget *urlTable;
    QTextBrowser *htmlView;
    QProgressBar *progressBar;
    QLabel *remainingLabel;
    QLabel *currentUrlLabel;
    QTimer *progressTimer;

    // engine
    CrawlerEngine *engine = nullptr;
    QtCrawlerBridge *bridge = nullptr;
};

/* ----------------------------- main() ----------------------------- */

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    // initialize libxml2
    xmlInitParser();

    MainWindow w;
    w.resize(1200, 800);
    w.show();

    int rc = app.exec();

    xmlCleanupParser();
    return rc;
}

#include "crawler.moc"

