// addresscrawler.cpp
//
// features:
// - state persistence using "history.crl" (binary file)
// - restartable crawl (loads history on start)
// - shutdown persistence (saves state on quit)
// - extracts emails ONLY from <a href="mailto:..."> tags
// - depth limit of 16
// - scrolling log window
//
// bug fixes:
// - **critical:** streamlined the logic in mainwindow::startcrawl to correctly handle
//   deleting/recreating the shared state object (`state`) only when initiating a fresh crawl.
// - **minor:** ensured ui labels for "seen links" and "seen emails" are correctly updated
//   when starting a fresh crawl.
//
// build requirements:
// - libcurl development headers
// - libxml2 development headers
// - qt5 or qt6
// - qmake: libs += -lcurl -lxml2

#include <QApplication>
#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QSet>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QUrl>
#include <QThread>
#include <QMetaType>
#include <QLabel>
#include <QTextEdit>
#include <QDateTime>
#include <QFile>
#include <QDataStream>

#include <vector>
#include <queue>
#include <atomic>

// external libraries
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

// ----------------------------- persistence configuration -----------------------------

const quint32 crawl_magic = 0x51744352; // "qtcr"
const quint32 crawl_version = 1;
const char *history_file = "history.crl";

// ----------------------------- types -----------------------------

struct CrawlItem {
    QString url;
    QString norm;
    int depth;
    double score;

    bool operator<(const CrawlItem &other) const {
        return score < other.score;
    }
};

Q_DECLARE_METATYPE(CrawlItem)

qdatastream &operator<<(qdatastream &out, const CrawlItem &item) {
    out << item.url << item.norm << item.depth << item.score;
    return out;
}

qdatastream &operator>>(qdatastream &in, CrawlItem &item) {
    in >> item.url >> item.norm >> item.depth >> item.score;
    return in;
}

// ----------------------------- shared state -----------------------------

class CrawlSharedState {
public:
    QMutex mutex;
    QWaitCondition cond;
    std::priority_queue<CrawlItem> frontier;
    QSet<QString> seenUrls;
    QSet<QString> seenEmails;

    std::atomic<bool> stopFlag{false};
    std::atomic<int> pagesCrawled{0};
    
    // default limits as requested by user (16 deep, 100k pages)
    int maxPages = 100000; 
    int maxDepth = 16;

    // --- persistence methods ---

    bool save(const QString &filename) {
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly)) return false;

        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_5_12); 

        // magic and version for integrity check
        out << crawl_magic << crawl_version;

        // save metrics and limits
        out << (quint32)pagesCrawled.load() << (quint32)maxPages << (quint32)maxDepth;

        // save sets
        out << seenUrls << seenEmails;

        // save frontier (convert to list first)
        QList<CrawlItem> frontierList;
        // lock while copying the frontier to prevent thread interference
        {
            QMutexLocker locker(&mutex); 
            std::priority_queue<CrawlItem> tempFrontier = frontier; 
            while (!tempFrontier.empty()) {
                frontierList.append(tempFrontier.top());
                tempFrontier.pop();
            }
        }
        out << frontierList;

        return true;
    }

    bool load(const QString &filename) {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) return false;

        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_5_12);

        quint32 magic, version;
        in >> magic >> version;

        if (magic != crawl_magic || version != crawl_version) {
            return false;
        }

        quint32 pcount, pmax, dmax;
        in >> pcount >> pmax >> dmax;
        pagesCrawled = (int)pcount;
        maxPages = (int)pmax;
        maxDepth = (int)dmax;

        in >> seenUrls >> seenEmails;

        QList<CrawlItem> frontierList;
        in >> frontierList;
        
        // rebuild priority queue
        for (const auto &item : frontierList) {
            frontier.push(item);
        }

        return true;
    }

    void addUrl(const QString &url, const QString &norm, int depth, double score) {
        QMutexLocker locker(&mutex);
        if (seenUrls.contains(norm)) return;
        seenUrls.insert(norm);

        CrawlItem item;
        item.url = url;
        item.norm = norm;
        item.depth = depth;
        item.score = score;
        frontier.push(item);
        cond.wakeOne();
    }

    bool popItem(CrawlItem &item) {
        QMutexLocker locker(&mutex);
        while (frontier.empty() && !stopFlag) {
            cond.wait(&mutex);
        }
        if (stopFlag || frontier.empty()) return false;

        item = frontier.top();
        frontier.pop();
        return true;
    }
};

// ----------------------------- worker -----------------------------

class CrawlerWorker : public QThread {
    Q_OBJECT

public:
    CrawlSharedState *state;

    explicit CrawlerWorker(CrawlSharedState *s, QObject *parent = nullptr)
        : QThread(parent), state(s) {}

protected:
    void run() override {
        // init curl per thread (required for multi-threading)
        CURL *curl = curl_easy_init();
        if (!curl) return;

        // configure curl options
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "AddressCrawlerQT/mailto-only");
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, ""); 
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

        while (!state->stopFlag) {
            if (state->pagesCrawled >= state->maxPages) {
                state->stopFlag = true;
                break;
            }

            CrawlItem item;
            if (!state->popItem(item)) break;

            emit logMessage(QString("fetching: %1 (depth %2, frontier: %3)").arg(item.url).arg(item.depth).arg(state->frontier.size()));

            currentBuffer.clear();
            curl_easy_setopt(curl, CURLOPT_URL, item.url.toUtf8().constData());

            CURLcode res = curl_easy_perform(curl);
            if (res == CURLE_OK && !currentBuffer.isEmpty()) {
                parseAndExtract(currentBuffer, item);
                state->pagesCrawled++;
                emit progressUpdated(state->pagesCrawled);
            }
            
            QThread::usleep(100); 
        }

        curl_easy_cleanup(curl);
    }

signals:
    void foundLink(double score, int depth, QString url);
    void foundEmail(QString email, QString source);
    void progressUpdated(int count);
    void logMessage(QString msg);

private:
    QByteArray currentBuffer;

    static size_t writeCallback(void *data, size_t size, size_t nmemb, void *userp) {
        size_t realSize = size * nmemb;
        CrawlerWorker *worker = static_cast<CrawlerWorker*>(userp);
        worker->currentBuffer.append((const char*)data, realSize);
        return realSize;
    }

    void parseAndExtract(const QByteArray &htmlData, const CrawlItem &currentItem) {
        if (htmlData.isEmpty()) return;

        htmlDocPtr doc = htmlReadMemory(htmlData.constData(), htmlData.size(),
                                        currentItem.url.toUtf8().constData(),
                                        nullptr,
                                        HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_RECOVER);
        if (!doc) return;

        xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
        if (!xpathCtx) {
            xmlFreeDoc(doc);
            return;
        }

        // only extract links and mailto hrefs
        if (currentItem.depth < state->maxDepth) {
            xmlXPathObjectPtr hrefObj = xmlXPathEvalExpression((xmlChar *)"//@href", xpathCtx);
            if (hrefObj && hrefObj->nodesetval) {
                for (int i = 0; i < hrefObj->nodesetval->nodeNr; ++i) {
                    xmlAttrPtr attr = (xmlAttrPtr)hrefObj->nodesetval->nodeTab[i];
                    if (attr && attr->children && attr->children->content) {
                        QString href = QString::fromUtf8((const char *)attr->children->content);
                        processHref(href, currentItem);
                    }
                }
            }
            if (hrefObj) xmlXPathFreeObject(hrefObj);
        }

        xmlXPathFreeContext(xpathCtx);
        xmlFreeDoc(doc);
    }

    void processHref(const QString &href, const CrawlItem &parentItem) {
        QString cleanHref = href.trimmed();
        if (cleanHref.isEmpty()) return;
        
        // --- 1. handle mailto (the only source for emails now) ---
        if (cleanHref.startsWith("mailto:", Qt::CaseInsensitive)) {
             QString e = cleanHref.mid(7);
             if (e.contains('?')) e = e.section('?', 0, 0);
             e = e.toLower();
             
             bool isNew = false;
             {
                 QMutexLocker l(&state->mutex);
                 if(!state->seenEmails.contains(e)) { 
                     state->seenEmails.insert(e); 
                     isNew = true;
                 }
             }
             if(isNew) {
                 emit foundEmail(e, parentItem.url);
                 emit logMessage(QString(">>> found email: %1").arg(e));
             }
             return;
        }

        // --- 2. handle regular links for crawling ---
        if (cleanHref.startsWith("javascript:", Qt::CaseInsensitive) || cleanHref.startsWith("tel:", Qt::CaseInsensitive)) return; 

        QUrl baseUrl(parentItem.url);
        QUrl resolvedUrl = baseUrl.resolved(QUrl(cleanHref));
        
        if (!resolvedUrl.isValid()) return;
        if (resolvedUrl.scheme() != "http" && resolvedUrl.scheme() != "https") return;

        QUrl normUrl = resolvedUrl;
        normUrl.setFragment(QString());
        QString normStr = normUrl.toString(QUrl::StripTrailingSlash | QUrl::RemoveUserInfo);
        normStr = normStr.toLower();

        // simple score
        double score = 1.0;
        QString host = resolvedUrl.host();
        if (host.endsWith(".gov")) score = 5.0;
        else if (host.endsWith(".edu")) score = 4.0;
        else if (host.endsWith(".org")) score = 2.0;
        else score = 1.0;
        
        int newDepth = parentItem.depth + 1;
        score -= (newDepth * 1.0); 

        {
            QMutexLocker locker(&state->mutex);
            if (!state->seenUrls.contains(normStr)) {
                state->seenUrls.insert(normStr);
                
                CrawlItem item;
                item.url = resolvedUrl.toString();
                item.norm = normStr;
                item.depth = newDepth;
                item.score = score;
                state->frontier.push(item);
                state->cond.wakeOne();

                emit foundLink(score, newDepth, item.url);
            }
        }
    }
};

// ----------------------------- main window -----------------------------

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QObject *parent = nullptr) : QMainWindow(parent) {
        // State initialization and loading is the first step
        state = new CrawlSharedState();
        bool loaded = state->load(history_file);
        
        QWidget *central = new QWidget(this);
        setCentralWidget(central);
        QVBoxLayout *layout = new QVBoxLayout(central);

        // top bar
        QHBoxLayout *top = new QHBoxLayout();
        urlInput = new QLineEdit(this);
        urlInput->setPlaceholderText("https://example.com");
        
        if (loaded && !state->frontier.empty()) {
            urlInput->setText(QString("resuming crawl. frontier size: %1").arg(state->frontier.size()));
            urlInput->setEnabled(false);
        } else {
            urlInput->setText("https://www.gnu.org");
        }

        crawlBtn = new QPushButton("start/resume crawl", this);
        top->addWidget(urlInput);
        top->addWidget(crawlBtn);
        layout->addLayout(top);

        // progress
        progressBar = new QProgressBar(this);
        progressBar->setRange(0, state->maxPages);
        progressBar->setValue(state->pagesCrawled);
        layout->addWidget(progressBar);

        // content area
        QHBoxLayout *contentLayout = new QHBoxLayout();
        
        // left: links queue
        QVBoxLayout *leftCol = new QVBoxLayout();
        linksLabel = new QLabel("crawled links (seen: " + QString::number(state->seenUrls.size()) + ")");
        leftCol->addWidget(linksLabel);
        linksTable = createTable({"score", "depth", "url"});
        leftCol->addWidget(linksTable);
        contentLayout->addLayout(leftCol, 2);

        // middle: emails found
        QVBoxLayout *midCol = new QVBoxLayout();
        emailsLabel = new QLabel("emails (from mailto, seen: " + QString::number(state->seenEmails.size()) + ")");
        midCol->addWidget(emailsLabel);
        emailsTable = createTable({"email", "source"});
        // pre-populate emails if loaded
        for(const QString& email : state->seenEmails) {
            int row = emailsTable->rowCount();
            emailsTable->insertRow(row);
            emailsTable->setItem(row, 0, new QTableWidgetItem(email));
            emailsTable->setItem(row, 1, new QTableWidgetItem("loaded from history"));
        }
        midCol->addWidget(emailsTable);
        contentLayout->addLayout(midCol, 2);

        // right: scrolling log
        QVBoxLayout *rightCol = new QVBoxLayout();
        rightCol->addWidget(new QLabel("activity log"));
        logArea = new QTextEdit(this);
        logArea->setReadOnly(true);
        logArea->setStyleSheet("font-family: monospace; font-size: 11px;");
        rightCol->addWidget(logArea);
        contentLayout->addLayout(rightCol, 3);

        layout->addLayout(contentLayout);

        connect(crawlBtn, &QPushButton::clicked, this, &MainWindow::startCrawl);

        setWindowTitle("AddressCrawler: Persistence + Mailto Only (Depth 16)");
        resize(1200, 700);
        
        if (loaded) log("history.crl loaded successfully. frontier size: " + QString::number(state->frontier.size()));

        xmlInitParser();
    }

    ~MainWindow() {
        stopCrawl();
        if (state) {
            log("saving state to history.crl...");
            if (state->save(history_file)) {
                 log("save successful. goodbye.");
            } else {
                 log("error saving state.");
            }
            // must delete the shared state object
            delete state;
        }
        xmlCleanupParser();
    }

private slots:
    void startCrawl() {
        if (isRunning) {
            stopCrawl();
            crawlBtn->setText("start/resume crawl");
            return;
        }

        bool isResume = !state->frontier.empty();

        if (isResume) {
            // Case 1: Resume an existing crawl state
            urlInput->setText(QString("resuming crawl. frontier size: %1").arg(state->frontier.size()));
            urlInput->setEnabled(false);
            log("resuming crawl from history...");
        } else {
            // Case 2: Fresh start needed (no history or history was completed/empty)
            QString seed = urlInput->text().trimmed();
            if (seed.isEmpty()) return;

            // Delete old state and create a new one to guarantee a clean slate
            if (state) delete state;
            state = new CrawlSharedState();
            state->addUrl(seed, seed.toLower(), 0, 10.0);

            // Reset UI for fresh start
            logArea->clear();
            linksTable->setRowCount(0);
            emailsTable->setRowCount(0);
            progressBar->setRange(0, state->maxPages);
            progressBar->setValue(0);
            
            // Update labels to reflect new, empty state (FIXED BUG)
            linksLabel->setText("crawled links (seen: 0)");
            emailsLabel->setText("emails (from mailto, seen: 0)");
            urlInput->setEnabled(true);
            urlInput->setText(seed);

            log("starting new crawl on seed: " + seed);
        }
        
        // Final prep before spawning workers
        progressBar->setRange(0, state->maxPages);
        progressBar->setValue(state->pagesCrawled);
        crawlBtn->setText("stop");
        isRunning = true;

        // spawn workers
        int threadCount = 8;
        for(int i=0; i<threadCount; ++i) {
            CrawlerWorker *w = new CrawlerWorker(state, this);
            connect(w, &CrawlerWorker::foundLink, this, &MainWindow::onFoundLink);
            connect(w, &CrawlerWorker::foundEmail, this, &MainWindow::onFoundEmail);
            connect(w, &CrawlerWorker::progressUpdated, this, &MainWindow::onProgress);
            connect(w, &CrawlerWorker::logMessage, this, &MainWindow::log);
            connect(w, &CrawlerWorker::finished, w, &QObject::deleteLater);
            w->start();
            workers.append(w);
        }
    }

    void onFoundLink(double score, int depth, QString url) {
        if (linksTable->rowCount() > 2000) return;
        int row = linksTable->rowCount();
        linksTable->insertRow(row);
        linksTable->setItem(row, 0, new QTableWidgetItem(QString::number(score, 'f', 1)));
        linksTable->setItem(row, 1, new QTableWidgetItem(QString::number(depth)));
        linksTable->setItem(row, 2, new QTableWidgetItem(url));
    }

    void onFoundEmail(QString email, QString source) {
        int row = emailsTable->rowCount();
        emailsTable->insertRow(row);
        emailsTable->setItem(row, 0, new QTableWidgetItem(email));
        emailsTable->setItem(row, 1, new QTableWidgetItem(source));
        // update email count label
        emailsLabel->setText("emails (from mailto, seen: " + QString::number(state->seenEmails.size()) + ")");
    }

    void onProgress(int count) {
        progressBar->setValue(count);
        // update link count label
        linksLabel->setText("crawled links (seen: " + QString::number(state->seenUrls.size()) + ")");

        if (count >= state->maxPages && isRunning) {
            log("reached max page limit.");
            stopCrawl(); // stop workers gracefully
            crawlBtn->setText("done");
        }
    }

    void log(QString msg) {
        QString ts = QDateTime::currentDateTime().toString("HH:mm:ss");
        logArea->append(QString("[%1] %2").arg(ts, msg));
    }

private:
    QLineEdit *urlInput;
    QPushButton *crawlBtn;
    QProgressBar *progressBar;
    QTableWidget *linksTable;
    QTableWidget *emailsTable;
    QTextEdit *logArea;
    QLabel *linksLabel; // added to easily update text
    QLabel *emailsLabel; // added to easily update text

    CrawlSharedState *state = nullptr;
    QList<CrawlerWorker*> workers;
    bool isRunning = false;

    void stopCrawl() {
        if (!isRunning) return; // prevent redundant log messages

        log("stopping workers...");
        if (state) {
            state->stopFlag = true;
            state->cond.wakeAll();
        }
        for (auto w : workers) {
            w->wait();
        }
        workers.clear();
        isRunning = false;
        log("stopped.");
    }

    QTableWidget* createTable(const QStringList &headers) {
        QTableWidget *t = new QTableWidget(this);
        t->setColumnCount(headers.size());
        t->setHorizontalHeaderLabels(headers);
        t->horizontalHeader()->setSectionResizeMode(headers.size()-1, QHeaderView::Stretch);
        t->setEditTriggers(QAbstractItemView::NoEditTriggers);
        return t;
    }
};

// ----------------------------- main -----------------------------

int main(int argc, char *argv[])
{
    // register metatypes for qt signals/slots
    qRegisterMetaType<int>("int");
    qRegisterMetaType<double>("double");
    qRegisterMetaType<QString>("QString");
    qRegisterMetaType<CrawlItem>("CrawlItem");

    // global init (required)
    curl_global_init(CURL_GLOBAL_ALL);

    QApplication app(argc, argv);
    MainWindow w;
    w.show();

    int ret = app.exec();

    // global cleanup (required)
    curl_global_cleanup();
    return ret;
}

#include "addresscrawler.moc"
