// wt_crawler_fixed.cpp
// Single-file Wt application (fixed): replaces non-existent WDataTable with WTable.
// Build (example):
// g++ wt_crawler_fixed.cpp -o wt_crawler -std=c++17 `pkg-config --cflags --libs wt` -lcurl -lsqlite3 -lxml2 -lcrypto -pthread
//
// Run:
// ./wt_crawler --docroot . --http-address 0.0.0.0 --http-port 8080

#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WLineEdit.h>
#include <Wt/WText.h>
#include <Wt/WServer.h>
#include <Wt/WResource.h>
#include <Wt/WAnchor.h>
#include <Wt/WTable.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WDialog.h>
#include <Wt/WTextArea.h>
#include <Wt/WProgressBar.h>
#include <Wt/WCheckBox.h>
#include <Wt/WComboBox.h>
#include <Wt/WBootstrapTheme.h>
#include <Wt/WBreak.h>
#include <Wt/Json/Object.h>
#include <Wt/Json/Array.h>
#include <Wt/Json/Serializer.h>
#include <Wt/Http/Request.h>
#include <Wt/Http/Response.h>
#include <Wt/Utils.h>

#include <curl/curl.h>
#include <sqlite3.h>
#include <openssl/sha.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <cctype>
#include <regex>
#include <queue>
#include <map>
#include <iomanip>

using namespace Wt;
using namespace std::chrono_literals;

// ----------------------------- Configuration -----------------------------
static const char* DB_FILENAME = "crawler_wt.db";
static const char* USER_AGENT = "WtAddressCrawler/1.0";
static const int DEFAULT_WORKERS = 4;
static const int POLITE_DELAY_MS = 150;

// ----------------------------- Globals -----------------------------
static sqlite3* g_db = nullptr;
static std::mutex g_db_mutex;

struct FrontierItem {
    std::string url;
    std::string normalized;
    int depth;
    double score;
    std::time_t queued_at;
};

static std::mutex g_frontier_mutex;
static std::condition_variable g_frontier_cv;
static std::deque<FrontierItem> g_frontier;
static std::atomic<bool> g_crawl_running(false);
static std::atomic<int> g_workers_count(DEFAULT_WORKERS);
static std::vector<std::thread> g_worker_threads;
static std::atomic<int> g_crawled_count(0);
static std::atomic<int> g_email_count(0);
static std::atomic<int> g_phone_count(0);

// ----------------------------- Utilities -----------------------------
static std::string now_iso() {
    auto t = std::time(nullptr);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&t));
    return std::string(buf);
}

static std::string sha256_hex(const unsigned char* data, size_t len) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, data, len);
    SHA256_Final(hash, &ctx);
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) ss << std::setw(2) << (int)hash[i];
    return ss.str();
}

static size_t curl_write_cb(void* ptr, size_t size, size_t nmemb, void* userdata) {
    std::string* s = (std::string*)userdata;
    size_t add = size * nmemb;
    s->append((char*)ptr, add);
    return add;
}

// ----------------------------- DB: schema and helpers -----------------------------
static void db_exec(const char* sql) {
    char* err = nullptr;
    std::lock_guard<std::mutex> lk(g_db_mutex);
    if (sqlite3_exec(g_db, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        std::cerr << "DB error: " << (err ? err : "(unknown)") << "\n";
        if (err) sqlite3_free(err);
    }
}

static void db_init() {
    if (sqlite3_open(DB_FILENAME, &g_db) != SQLITE_OK) {
        std::cerr << "Cannot open DB: " << sqlite3_errmsg(g_db) << "\n";
        exit(1);
    }
    // Enable WAL for concurrency
    db_exec("PRAGMA journal_mode=WAL;");
    // Core tables
    db_exec(R"SQL(
        CREATE TABLE IF NOT EXISTS urls (
            id INTEGER PRIMARY KEY,
            url TEXT UNIQUE,
            normalized TEXT UNIQUE,
            depth INTEGER,
            score REAL,
            inbound INTEGER DEFAULT 0,
            crawled INTEGER DEFAULT 0,
            content_hash TEXT,
            crawled_at TEXT
        );
        CREATE TABLE IF NOT EXISTS emails (id INTEGER PRIMARY KEY, email TEXT UNIQUE, source_url TEXT, discovered_at TEXT);
        CREATE TABLE IF NOT EXISTS phones (id INTEGER PRIMARY KEY, phone TEXT UNIQUE, source_url TEXT, discovered_at TEXT);
        CREATE VIRTUAL TABLE IF NOT EXISTS docs_fts USING fts5(url, title, body, content='');
    )SQL");
}

static bool db_insert_url_if_new(const std::string& url, const std::string& norm, int depth, double score) {
    std::lock_guard<std::mutex> lk(g_db_mutex);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT OR IGNORE INTO urls (url, normalized, depth, score) VALUES (?, ?, ?, ?);";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, norm.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, depth);
    sqlite3_bind_double(stmt, 4, score);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE && sqlite3_changes(g_db) > 0);
}

static void db_mark_crawled(const std::string& norm, const std::string& hash_hex) {
    std::lock_guard<std::mutex> lk(g_db_mutex);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "UPDATE urls SET crawled = 1, content_hash = ?, crawled_at = ? WHERE normalized = ?;";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return;
    sqlite3_bind_text(stmt, 1, hash_hex.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, now_iso().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, norm.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

static bool db_has_normalized(const std::string& norm) {
    std::lock_guard<std::mutex> lk(g_db_mutex);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT 1 FROM urls WHERE normalized = ? LIMIT 1;";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, norm.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    bool found = (rc == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return found;
}

static bool db_insert_email(const std::string& email, const std::string& source) {
    std::lock_guard<std::mutex> lk(g_db_mutex);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT OR IGNORE INTO emails (email, source_url, discovered_at) VALUES (?, ?, ?);";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, source.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, now_iso().c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE && sqlite3_changes(g_db) > 0);
}

static bool db_insert_phone(const std::string& phone, const std::string& source) {
    std::lock_guard<std::mutex> lk(g_db_mutex);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT OR IGNORE INTO phones (phone, source_url, discovered_at) VALUES (?, ?, ?);";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, phone.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, source.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, now_iso().c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE && sqlite3_changes(g_db) > 0);
}

static void db_index_doc(const std::string& url, const std::string& title, const std::string& body) {
    std::lock_guard<std::mutex> lk(g_db_mutex);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO docs_fts (rowid, url, title, body) VALUES (NULL, ?, ?, ?);";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return;
    sqlite3_bind_text(stmt, 1, url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, body.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

// ----------------------------- Simple normalization/resolution -----------------------------
static std::string normalize_url(const std::string& url) {
    std::string s = url;
    auto pos = s.find('#');
    if (pos != std::string::npos) s.erase(pos);
    while (s.size() > 1 && s.back() == '/') s.pop_back();
    return s;
}

// ----------------------------- Extraction helpers -----------------------------
static void extract_emails_and_phones(const std::string& text, const std::string& source) {
    static const std::regex email_re(R"(([A-Za-z0-9._%+\-]+@[A-Za-z0-9.\-]+\.[A-Za-z]{2,})))", std::regex::icase);
    static const std::regex phone_re(R"((\+?[0-9][0-9\-\.\s\(\)]{6,}[0-9]))", std::regex::icase);

    std::smatch m;
    std::string s = text;
    auto it = s.cbegin();
    while (std::regex_search(it, s.cend(), m, email_re)) {
        std::string email = m[1].str();
        std::transform(email.begin(), email.end(), email.begin(), ::tolower);
        if (db_insert_email(email, source)) {
            g_email_count++;
            std::cerr << "[NEW EMAIL] " << email << " (source: " << source << ")\n";
        }
        it = m.suffix().first;
    }
    it = s.cbegin();
    while (std::regex_search(it, s.cend(), m, phone_re)) {
        std::string phone = m[1].str();
        std::string norm;
        for (char c : phone) if (std::isdigit((unsigned char)c) || (c == '+' && norm.empty())) norm.push_back(c);
        if (!norm.empty()) {
            if (db_insert_phone(norm, source)) {
                g_phone_count++;
                std::cerr << "[NEW PHONE] " << norm << " (source: " << source << ")\n";
            }
        }
        it = m.suffix().first;
    }
}

// ----------------------------- Fetch + parse -----------------------------
static bool fetch_url(CURL* curl, const std::string& url, std::string& out_body, std::string& out_content_type, long& out_code) {
    out_body.clear();
    out_content_type.clear();
    out_code = 0;
    curl_easy_reset(curl);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out_body);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, +[](char* buffer, size_t size, size_t nitems, void* userdata)->size_t {
        size_t len = size * nitems;
        std::string hdr(buffer, len);
        std::string* ct = (std::string*)userdata;
        std::string key = "Content-Type:";
        if (hdr.size() >= key.size() && strncasecmp(hdr.c_str(), key.c_str(), key.size()) == 0) {
            auto p = hdr.c_str() + key.size();
            while (*p && isspace((unsigned char)*p)) ++p;
            std::string v(p, hdr.cend());
            while (!v.empty() && (v.back() == '\r' || v.back() == '\n')) v.pop_back();
            *ct = v;
        }
        return len;
    });
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &out_content_type);
    CURLcode rc = curl_easy_perform(curl);
    if (rc != CURLE_OK) {
        return false;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &out_code);
    if (!out_content_type.empty() && out_content_type.find("text/html") == std::string::npos) {
        return false;
    }
    return true;
}

static void parse_html_and_index(const std::string& html, const std::string& base_url) {
    if (html.empty()) return;
    htmlDocPtr doc = htmlReadMemory(html.c_str(), (int)html.size(), base_url.c_str(), NULL, HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_RECOVER);
    if (!doc) return;
    xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
    if (!xpathCtx) { xmlFreeDoc(doc); return; }

    std::string title;
    xmlXPathObjectPtr titleObj = xmlXPathEvalExpression((xmlChar*)"//title/text()", xpathCtx);
    if (titleObj && titleObj->nodesetval && titleObj->nodesetval->nodeNr > 0) {
        xmlNodePtr n = titleObj->nodesetval->nodeTab[0];
        if (n && n->content) title = (const char*)n->content;
    }
    if (titleObj) xmlXPathFreeObject(titleObj);

    xmlXPathObjectPtr textObj = xmlXPathEvalExpression((xmlChar*)"//text()", xpathCtx);
    std::string body;
    if (textObj && textObj->nodesetval) {
        xmlNodeSetPtr nodes = textObj->nodesetval;
        for (int i = 0; i < nodes->nodeNr; ++i) {
            xmlNodePtr node = nodes->nodeTab[i];
            if (node && node->content) {
                body.append((const char*)node->content);
                body.push_back(' ');
            }
        }
    }
    if (textObj) xmlXPathFreeObject(textObj);

    db_index_doc(base_url, title, body);
    extract_emails_and_phones(body, base_url);

    xmlXPathObjectPtr hrefObj = xmlXPathEvalExpression((xmlChar*)"//@href", xpathCtx);
    if (hrefObj && hrefObj->nodesetval) {
        xmlNodeSetPtr nodes = hrefObj->nodesetval;
        for (int i = 0; i < nodes->nodeNr; ++i) {
            xmlAttrPtr attr = (xmlAttrPtr)nodes->nodeTab[i];
            if (attr && attr->children && attr->children->content) {
                const char* href = (const char*)attr->children->content;
                std::string hrefs(href);
                if (hrefs.rfind("mailto:", 0) == 0) continue;
                if (hrefs.rfind("tel:", 0) == 0) continue;
                std::string resolved = hrefs;
                if (hrefs.rfind("http://", 0) != 0 && hrefs.rfind("https://", 0) != 0) {
                    auto pos = base_url.find_last_of('/');
                    if (pos != std::string::npos) {
                        resolved = base_url.substr(0, pos) + "/" + hrefs;
                    } else {
                        resolved = base_url + "/" + hrefs;
                    }
                }
                std::string norm = normalize_url(resolved);
                if (!db_has_normalized(norm)) {
                    FrontierItem it;
                    it.url = resolved;
                    it.normalized = norm;
                    it.depth = 0;
                    it.score = 1.0;
                    it.queued_at = std::time(nullptr);
                    {
                        std::lock_guard<std::mutex> lk(g_frontier_mutex);
                        g_frontier.push_back(it);
                        g_frontier_cv.notify_one();
                    }
                    db_insert_url_if_new(resolved, norm, it.depth, it.score);
                }
            }
        }
    }
    if (hrefObj) xmlXPathFreeObject(hrefObj);

    xmlXPathFreeContext(xpathCtx);
    xmlFreeDoc(doc);
}

// ----------------------------- Worker loop -----------------------------
static void worker_loop(int id) {
    CURL* curl = curl_easy_init();
    if (!curl) return;
    while (g_crawl_running.load()) {
        FrontierItem item;
        {
            std::unique_lock<std::mutex> lk(g_frontier_mutex);
            g_frontier_cv.wait_for(lk, 500ms, []{ return !g_frontier.empty() || !g_crawl_running.load(); });
            if (!g_crawl_running.load() && g_frontier.empty()) break;
            if (g_frontier.empty()) continue;
            item = g_frontier.front();
            g_frontier.pop_front();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(POLITE_DELAY_MS));
        std::string body, ctype;
        long code = 0;
        bool ok = fetch_url(curl, item.url, body, ctype, code);
        if (!ok) {
            std::cerr << "[worker " << id << "] fetch failed: " << item.url << "\n";
            continue;
        }
        std::string hash = sha256_hex((const unsigned char*)body.data(), body.size());
        db_mark_crawled(item.normalized, hash);
        parse_html_and_index(body, item.url);
        g_crawled_count++;
        std::cerr << "[" << now_iso() << "] [worker " << id << "] crawled: " << item.url << " code=" << code << "\n";
    }
    curl_easy_cleanup(curl);
}

// ----------------------------- API Resources -----------------------------
class ApiResource : public WResource {
public:
    ApiResource() { suggestFileName("api"); }
    ~ApiResource() override { beingDeleted(); }

    void handleRequest(const Http::Request& request, Http::Response& response) override {
        response.setMimeType("application/json");
        try {
            if (request.method() == "POST") {
                std::string path = request.pathInfo();
                std::string body;
                std::istream& in = request.in();
                std::ostringstream ss;
                ss << in.rdbuf();
                body = ss.str();

                Wt::Json::Object obj;
                try {
                    Wt::Json::parse(body, obj);
                } catch (...) {}

                if (path == "/crawl/control") {
                    std::string action;
                    if (obj.contains("action")) action = obj.get("action").toString();
                    else if (request.getParameter("action")) action = request.getParameter("action");
                    if (action == "start") {
                        std::string seed;
                        if (obj.contains("seed")) seed = obj.get("seed").toString();
                        else if (request.getParameter("seed")) seed = request.getParameter("seed");
                        int workers = DEFAULT_WORKERS;
                        if (obj.contains("workers")) workers = (int)obj.get("workers").toNumber();
                        else if (request.getParameter("workers")) workers = std::stoi(request.getParameter("workers"));
                        start_crawl(seed, workers);
                        Wt::Json::Object out;
                        out["status"] = "started";
                        out["workers"] = workers;
                        response.out() << Wt::Json::serialize(out);
                        return;
                    } else if (action == "stop") {
                        stop_crawl();
                        Wt::Json::Object out;
                        out["status"] = "stopped";
                        response.out() << Wt::Json::serialize(out);
                        return;
                    }
                } else if (path == "/frontier") {
                    std::string url;
                    if (obj.contains("url")) url = obj.get("url").toString();
                    else if (request.getParameter("url")) url = request.getParameter("url");
                    if (!url.empty()) {
                        FrontierItem it;
                        it.url = url;
                        it.normalized = normalize_url(url);
                        it.depth = 0;
                        it.score = 10.0;
                        it.queued_at = std::time(nullptr);
                        {
                            std::lock_guard<std::mutex> lk(g_frontier_mutex);
                            g_frontier.push_back(it);
                            g_frontier_cv.notify_one();
                        }
                        db_insert_url_if_new(url, it.normalized, it.depth, it.score);
                        Wt::Json::Object out;
                        out["status"] = "enqueued";
                        out["url"] = url;
                        response.out() << Wt::Json::serialize(out);
                        return;
                    }
                } else if (path == "/search") {
                    std::string q;
                    int page = 0, per_page = 20;
                    if (obj.contains("q")) q = obj.get("q").toString();
                    if (obj.contains("page")) page = (int)obj.get("page").toNumber();
                    if (obj.contains("per_page")) per_page = (int)obj.get("per_page").toNumber();

                    std::lock_guard<std::mutex> lk(g_db_mutex);
                    sqlite3_stmt* stmt = nullptr;
                    std::string sql = "SELECT url, title, snippet(docs_fts, 2, '<b>', '</b>', '...', 10) as snippet FROM docs_fts WHERE docs_fts MATCH ? LIMIT ? OFFSET ?;";
                    if (sqlite3_prepare_v2(g_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                        std::string qparam = q.empty() ? "*" : q;
                        sqlite3_bind_text(stmt, 1, qparam.c_str(), -1, SQLITE_TRANSIENT);
                        sqlite3_bind_int(stmt, 2, per_page);
                        sqlite3_bind_int(stmt, 3, page * per_page);
                        Wt::Json::Array results;
                        while (sqlite3_step(stmt) == SQLITE_ROW) {
                            const unsigned char* url = sqlite3_column_text(stmt, 0);
                            const unsigned char* title = sqlite3_column_text(stmt, 1);
                            const unsigned char* snippet = sqlite3_column_text(stmt, 2);
                            Wt::Json::Object r;
                            r["url"] = url ? (const char*)url : "";
                            r["title"] = title ? (const char*)title : "";
                            r["snippet"] = snippet ? (const char*)snippet : "";
                            results.push_back(r);
                        }
                        sqlite3_finalize(stmt);
                        Wt::Json::Object out;
                        out["results"] = results;
                        response.out() << Wt::Json::serialize(out);
                        return;
                    } else {
                        Wt::Json::Object out;
                        out["error"] = "search not available";
                        response.out() << Wt::Json::serialize(out);
                        return;
                    }
                }
            }
            Wt::Json::Object out;
            out["error"] = "invalid request";
            response.out() << Wt::Json::serialize(out);
        } catch (const std::exception& ex) {
            response.setStatus(500);
            Wt::Json::Object out;
            out["error"] = std::string("server error: ") + ex.what();
            response.out() << Wt::Json::serialize(out);
        }
    }

private:
    void start_crawl(const std::string& seed, int workers) {
        if (g_crawl_running.load()) return;
        if (!seed.empty()) {
            FrontierItem it;
            it.url = seed;
            it.normalized = normalize_url(seed);
            it.depth = 0;
            it.score = 10.0;
            it.queued_at = std::time(nullptr);
            {
                std::lock_guard<std::mutex> lk(g_frontier_mutex);
                g_frontier.push_back(it);
                g_frontier_cv.notify_one();
            }
            db_insert_url_if_new(seed, it.normalized, it.depth, it.score);
        }
        g_workers_count.store(workers);
        g_crawl_running.store(true);
        for (int i = 0; i < workers; ++i) {
            g_worker_threads.emplace_back([i](){ worker_loop(i+1); });
        }
    }

    void stop_crawl() {
        if (!g_crawl_running.load()) return;
        g_crawl_running.store(false);
        g_frontier_cv.notify_all();
        for (auto& t : g_worker_threads) if (t.joinable()) t.join();
        g_worker_threads.clear();
    }
};

// ----------------------------- Wt Application -----------------------------
class CrawlerApp : public WApplication {
public:
    CrawlerApp(const WEnvironment& env, ApiResource* api)
        : WApplication(env), api_(api)
    {
        setTitle("Wt Crawler & Search");
        auto theme = std::make_shared<WBootstrapTheme>();
        theme->setVersion(WBootstrapTheme::Version::v4);
        setTheme(theme);

        root()->addWidget(std::make_unique<WText>("<h2>Address Crawler — Dashboard</h2>"));

        auto controls = root()->addWidget(std::make_unique<WContainerWidget>());
        startBtn_ = controls->addWidget(std::make_unique<WPushButton>("Start Crawl"));
        stopBtn_  = controls->addWidget(std::make_unique<WPushButton>("Stop Crawl"));
        seedInput_ = controls->addWidget(std::make_unique<WLineEdit>());
        seedInput_->setPlaceholderText("Seed URL (optional)");
        enqueueBtn_ = controls->addWidget(std::make_unique<WPushButton>("Enqueue Seed"));
        controls->addWidget(std::make_unique<WBreak>());

        metrics_ = root()->addWidget(std::make_unique<WContainerWidget>());
        crawledText_ = metrics_->addWidget(std::make_unique<WText>("Crawled: 0"));
        queuedText_  = metrics_->addWidget(std::make_unique<WText>(" | Queued: 0"));
        emailsText_  = metrics_->addWidget(std::make_unique<WText>(" | Emails: 0"));
        phonesText_  = metrics_->addWidget(std::make_unique<WText>(" | Phones: 0"));
        metrics_->addWidget(std::make_unique<WBreak>());

        progress_ = root()->addWidget(std::make_unique<WProgressBar>());
        progress_->setRange(0, 100);
        progress_->setValue(0);

        auto searchRow = root()->addWidget(std::make_unique<WContainerWidget>());
        searchBox_ = searchRow->addWidget(std::make_unique<WLineEdit>());
        searchBox_->setPlaceholderText("Search crawled content");
        searchBtn_ = searchRow->addWidget(std::make_unique<WPushButton>("Search"));

        // Use WTable instead of non-existent WDataTable
        searchResults_ = root()->addWidget(std::make_unique<WTable>());
        searchResults_->setHeaderCount(1);
        searchResults_->elementAt(0,0)->addWidget(std::make_unique<WText>("<b>URL</b>"));
        searchResults_->elementAt(0,1)->addWidget(std::make_unique<WText>("<b>Title</b>"));
        searchResults_->elementAt(0,2)->addWidget(std::make_unique<WText>("<b>Snippet</b>"));

        root()->addWidget(std::make_unique<WText>("<h3>Frontier</h3>"));
        frontierTable_ = root()->addWidget(std::make_unique<WTable>());
        frontierTable_->setHeaderCount(1);
        frontierTable_->elementAt(0,0)->addWidget(std::make_unique<WText>("<b>URL</b>"));
        frontierTable_->elementAt(0,1)->addWidget(std::make_unique<WText>("<b>Queued At</b>"));

        root()->addWidget(std::make_unique<WText>("<h3>Recent Crawled</h3>"));
        recentTable_ = root()->addWidget(std::make_unique<WTable>());
        recentTable_->setHeaderCount(1);
        recentTable_->elementAt(0,0)->addWidget(std::make_unique<WText>("<b>URL</b>"));
        recentTable_->elementAt(0,1)->addWidget(std::make_unique<WText>("<b>Crawled At</b>"));

        startBtn_->clicked().connect([=](){
            Wt::Json::Object req;
            req["action"] = "start";
            req["seed"] = seedInput_->text().toUTF8();
            req["workers"] = (int)g_workers_count.load();
            doApiPost("/api/crawl/control", req);
        });
        stopBtn_->clicked().connect([=](){
            Wt::Json::Object req;
            req["action"] = "stop";
            doApiPost("/api/crawl/control", req);
        });
        enqueueBtn_->clicked().connect([=](){
            Wt::Json::Object req;
            req["url"] = seedInput_->text().toUTF8();
            doApiPost("/api/frontier", req);
            seedInput_->setText("");
            refreshFrontier();
        });
        searchBtn_->clicked().connect([=](){
            doSearch(searchBox_->text().toUTF8());
        });

        refreshTimerThread_ = std::thread([this](){
            while (true) {
                std::this_thread::sleep_for(1s);
                this->doJavaScript("/* tick */");
                WApplication::instance()->post([this](){ refreshMetrics(); refreshFrontier(); refreshRecent(); });
                if (!WApplication::instance()) break;
            }
        });

        refreshMetrics();
        refreshFrontier();
        refreshRecent();
    }

    ~CrawlerApp() {
        if (refreshTimerThread_.joinable()) {
            refreshTimerThread_.detach();
        }
    }

private:
    ApiResource* api_;
    WPushButton* startBtn_ = nullptr;
    WPushButton* stopBtn_ = nullptr;
    WLineEdit* seedInput_ = nullptr;
    WPushButton* enqueueBtn_ = nullptr;
    WContainerWidget* metrics_ = nullptr;
    WText* crawledText_ = nullptr;
    WText* queuedText_ = nullptr;
    WText* emailsText_ = nullptr;
    WText* phonesText_ = nullptr;
    WProgressBar* progress_ = nullptr;
    WLineEdit* searchBox_ = nullptr;
    WPushButton* searchBtn_ = nullptr;
    WTable* searchResults_ = nullptr;
    WTable* frontierTable_ = nullptr;
    WTable* recentTable_ = nullptr;
    std::thread refreshTimerThread_;

    void doApiPost(const std::string& path, const Wt::Json::Object& obj) {
        std::string url = std::string("http://127.0.0.1:8080") + path;
        CURL* curl = curl_easy_init();
        if (!curl) return;
        std::string payload = Wt::Json::serialize(obj);
        std::string resp;
        struct curl_slist* hdrs = nullptr;
        hdrs = curl_slist_append(hdrs, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)payload.size());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdrs);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
        curl_easy_perform(curl);
        curl_slist_free_all(hdrs);
        curl_easy_cleanup(curl);
    }

    void doSearch(const std::string& q) {
        std::string url = "http://127.0.0.1:8080/api/search";
        CURL* curl = curl_easy_init();
        if (!curl) return;
        Wt::Json::Object req;
        req["q"] = q;
        std::string payload = Wt::Json::serialize(req);
        std::string resp;
        struct curl_slist* hdrs = nullptr;
        hdrs = curl_slist_append(hdrs, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)payload.size());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdrs);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
        curl_easy_perform(curl);
        curl_slist_free_all(hdrs);
        curl_easy_cleanup(curl);

        try {
            Wt::Json::Value v = Wt::Json::parse(resp);
            if (v.type() == Wt::Json::ObjectType) {
                Wt::Json::Object o = v;
                if (o.contains("results")) {
                    Wt::Json::Array arr = o.get("results");
                    // clear existing rows (keep header)
                    int headerRows = searchResults_->headerCount();
                    int rows = searchResults_->rowCount();
                    for (int r = rows - 1; r >= headerRows; --r) searchResults_->removeRow(r);
                    for (size_t i = 0; i < arr.size(); ++i) {
                        Wt::Json::Object rj = arr.get(i);
                        std::string url = rj.get("url").toString();
                        std::string title = rj.get("title").toString();
                        std::string snippet = rj.get("snippet").toString();
                        int row = searchResults_->rowCount();
                        searchResults_->insertRow(row);
                        searchResults_->elementAt(row, 0)->clear();
                        searchResults_->elementAt(row, 0)->addWidget(std::make_unique<WAnchor>(url, WText(url)));
                        searchResults_->elementAt(row, 1)->clear();
                        searchResults_->elementAt(row, 1)->addWidget(std::make_unique<WText>(title));
                        searchResults_->elementAt(row, 2)->clear();
                        searchResults_->elementAt(row, 2)->addWidget(std::make_unique<WText>(snippet));
                    }
                }
            }
        } catch (...) {}
    }

    void refreshMetrics() {
        crawledText_->setText("Crawled: " + std::to_string(g_crawled_count.load()));
        emailsText_->setText(" | Emails: " + std::to_string(g_email_count.load()));
        phonesText_->setText(" | Phones: " + std::to_string(g_phone_count.load()));
        std::lock_guard<std::mutex> lk(g_frontier_mutex);
        queuedText_->setText(" | Queued: " + std::to_string((int)g_frontier.size()));
        int pct = std::min(100, (int)g_crawled_count.load());
        progress_->setValue(pct);
    }

    void refreshFrontier() {
        std::lock_guard<std::mutex> lk(g_frontier_mutex);
        int headerRows = frontierTable_->headerCount();
        int rows = frontierTable_->rowCount();
        for (int r = rows - 1; r >= headerRows; --r) frontierTable_->removeRow(r);
        int r = frontierTable_->rowCount();
        for (const auto& it : g_frontier) {
            frontierTable_->insertRow(r);
            frontierTable_->elementAt(r, 0)->clear();
            frontierTable_->elementAt(r, 0)->addWidget(std::make_unique<WText>(it.url));
            frontierTable_->elementAt(r, 1)->clear();
            frontierTable_->elementAt(r, 1)->addWidget(std::make_unique<WText>(std::to_string((long)it.queued_at)));
            ++r;
        }
    }

    void refreshRecent() {
        std::lock_guard<std::mutex> lk(g_db_mutex);
        sqlite3_stmt* stmt = nullptr;
        const char* sql = "SELECT url, crawled_at FROM urls WHERE crawled = 1 ORDER BY crawled_at DESC LIMIT 20;";
        if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            int headerRows = recentTable_->headerCount();
            int rows = recentTable_->rowCount();
            for (int r = rows - 1; r >= headerRows; --r) recentTable_->removeRow(r);
            int r = recentTable_->rowCount();
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                const unsigned char* url = sqlite3_column_text(stmt, 0);
                const unsigned char* crawled_at = sqlite3_column_text(stmt, 1);
                recentTable_->insertRow(r);
                recentTable_->elementAt(r, 0)->clear();
                recentTable_->elementAt(r, 0)->addWidget(std::make_unique<WText>(url ? (const char*)url : ""));
                recentTable_->elementAt(r, 1)->clear();
                recentTable_->elementAt(r, 1)->addWidget(std::make_unique<WText>(crawled_at ? (const char*)crawled_at : ""));
                ++r;
            }
            sqlite3_finalize(stmt);
        }
    }
};

// ----------------------------- main -----------------------------
int main(int argc, char** argv) {
    try {
        db_init();
        curl_global_init(CURL_GLOBAL_DEFAULT);

        auto api = std::make_shared<ApiResource>();

        WServer server(argc, argv, WTHTTP_CONFIGURATION);

        // Mount API at /api; ApiResource will inspect pathInfo for subpaths
        server.addResource(api.get(), "/api");

        server.addEntryPoint(Wt::EntryPointType::Application,
            [api](const WEnvironment& env) {
                return std::make_unique<CrawlerApp>(env, api.get());
            });

        if (server.start()) {
            int sig = WServer::waitForShutdown();
            std::cerr << "Shutdown (signal = " << sig << ")\n";
            server.stop();
        }

        curl_global_cleanup();
        if (g_db) sqlite3_close(g_db);
    } catch (const std::exception& ex) {
        std::cerr << "Fatal: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}
