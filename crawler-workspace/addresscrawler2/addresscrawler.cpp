// addresscrawler.cpp
// Single-file Qt5 application: Priority-aware email & phone crawler with UI.
// Integrates canonicalization, preflight HEAD checks, per-host queues & quotas,
// politeness delays, robots.txt basic respect, SHA256 page dedupe, novelty bias,
// and simple persistence.
//
// Build with qmake (Qt5) or CMake linking Qt5::Widgets Qt5::WebEngineWidgets Qt5::WebEngineCore Qt5::Network
// qmake .pro should include: QT += widgets webengine webenginewidgets webenginecore network
//
// Notes:
// - This is a demonstration crawler for research/learning. Respect robots.txt and site policies.
// - For production, add robust robots parsing, error handling, persistence (SQLite), rate limiting, and legal checks.

#include <QApplication>
#include <QMainWindow>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QWebEngineUrlRequestInterceptor>
#include <QWebEngineUrlRequestInfo>
#include <QtWebEngine/QtWebEngine>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QRegularExpression>
#include <QTimer>
#include <QSet>
#include <QHash>
#include <QMutex>
#include <QMutexLocker>
#include <QUrl>
#include <QDebug>
#include <QCryptographicHash>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QThread>
#include <queue>
#include <vector>
#include <algorithm>
#include <QVariant>
#include <QVariantList>

// ----------------------------- RequestInterceptor -----------------------------
class RequestInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT
public:
    explicit RequestInterceptor(QObject *parent = nullptr) : QWebEngineUrlRequestInterceptor(parent) {}
    void interceptRequest(QWebEngineUrlRequestInfo &info) override
    {
        // Allow main frame and subframe only; block images/scripts/styles to reduce bandwidth.
        auto type = info.resourceType();

        // Use Qt5 enum names (unscoped)
        switch (type) {
        case QWebEngineUrlRequestInfo::ResourceTypeMainFrame:
        case QWebEngineUrlRequestInfo::ResourceTypeSubFrame:
            info.setHttpHeader("Accept", QByteArray("text/html"));
            break;
        default:
            info.block(true);
            break;
        }
    }
};

// ----------------------------- Crawl data structures -----------------------------
struct CrawlItem {
    QUrl url;
    double score;
    int depth;
    int inboundCount;
};

struct HostQueue {
    std::queue<CrawlItem> q;
};

// ----------------------------- MainWindow -----------------------------
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onCrawlClicked();
    void onLoadFinished(bool ok);
    void onLoadProgress(int progress);
    void scheduleNext();
    void updateLinkTable();
    void updateEmailTable();
    void updatePhoneTable();
    void updateCrawlProgress();

private:
    // UI
    QLineEdit *urlBar = nullptr;
    QPushButton *crawlButton = nullptr;
    QWebEngineView *webView = nullptr;
    QTableWidget *linksTable = nullptr;
    QTableWidget *emailsTable = nullptr;
    QTableWidget *phonesTable = nullptr;
    QProgressBar *topProgressBar = nullptr;
    QProgressBar *bottomProgressBar = nullptr;

    // Network manager for HEAD and robots
    QNetworkAccessManager *netman = nullptr;

    // Per-host queues and dedup
    QHash<QString, HostQueue> perHostQueues;
    QSet<QString> seenUrls;    // canonical URLs processed
    QSet<QString> queuedUrls;  // canonical URLs enqueued
    QSet<QString> seenEmails;
    QSet<QString> seenPhones;
    QSet<QString> seenPageHashes; // SHA256 of page text
    QMutex frontierMutex;

    // Host tracking
    QHash<QString,int> hostActiveCount;
    QHash<QString,int> hostTotalCount;
    QHash<QString,QDateTime> hostNextAllowed;
    QHash<QString, QStringList> hostRobotsDisallow; // simple robots.txt disallow prefixes

    // Inbound counts for scoring
    QHash<QString,int> inboundCounts;

    // Current item being processed
    CrawlItem currentItem;
    bool isProcessing = false;
    QString currentHost;

    // Crawl counters and limits
    int pagesCrawledCount = 0;
    int maxTotalPages = 1000;
    int maxPerHostPerRun = 200;
    int maxConcurrentPerHost = 2;
    int domainHopLimit = 3;
    int consecutiveSameHost = 0;
    QString lastHost;

    // Politeness
    int politenessDelayMs = 1500; // 1.5s default

    // Scoring weights
    double wDomainTrust = 2.0;
    double wDepth = -1.0;
    double wInbound = 1.0;
    double wPathLength = -0.5;
    double noveltyBonusValue = 3.0;

    // Helpers
    double scoreUrl(const QUrl &url, int depth);
    double domainTrust(const QUrl &url);
    void enqueueUrl(const QUrl &url, int depth, int inboundDelta = 0);
    void extractLinksFromJsResult(const QVariant &variantList, const QUrl &baseUrl, int baseDepth);
    QString canonicalizeUrl(const QUrl &url);
    QString domainOf(const QUrl &url);

    // Email helpers
    void extractEmailsFromText(const QString &text, const QUrl &source);
    void processMailtoListVariant(const QVariant &variantList, const QUrl &source);

    // Phone helpers
    void extractPhonesFromText(const QString &text, const QUrl &source);
    void processTelListVariant(const QVariant &variantList, const QUrl &source);
    QString normalizePhone(const QString &raw);

    // Preflight HEAD
    void preflightAndMaybeLoad(const CrawlItem &item);

    // Robots
    void fetchRobotsIfNeeded(const QString &host);

    // Persistence (simple file-based)
    void loadPersistentSets();
    void savePersistentSets();

    // Utility
    bool isAllowedByRobots(const QUrl &url);
    double noveltyBonus(const QString &host);
    void markHostAccessed(const QString &host);
};

// ----------------------------- Implementation -----------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // UI setup
    urlBar = new QLineEdit(this);
    urlBar->setPlaceholderText("https://example.com");

    crawlButton = new QPushButton("Crawl", this);
    connect(crawlButton, &QPushButton::clicked, this, &MainWindow::onCrawlClicked);

    topProgressBar = new QProgressBar(this);
    topProgressBar->setRange(0, 100);
    topProgressBar->setValue(0);
    topProgressBar->setVisible(false);

    webView = new QWebEngineView(this);
    webView->setContextMenuPolicy(Qt::NoContextMenu);
    connect(webView, &QWebEngineView::loadProgress, this, &MainWindow::onLoadProgress);
    connect(webView, &QWebEngineView::loadFinished, this, &MainWindow::onLoadFinished);

    RequestInterceptor *interceptor = new RequestInterceptor(this);
    QWebEngineProfile *profile = webView->page()->profile();
    // Qt5 API
    profile->setRequestInterceptor(interceptor);

    linksTable = new QTableWidget(this);
    linksTable->setColumnCount(3);
    linksTable->setHorizontalHeaderLabels({"Score", "Depth", "URL"});
    linksTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    linksTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    emailsTable = new QTableWidget(this);
    emailsTable->setColumnCount(2);
    emailsTable->setHorizontalHeaderLabels({"Email", "Source URL"});
    emailsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    emailsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    phonesTable = new QTableWidget(this);
    phonesTable->setColumnCount(2);
    phonesTable->setHorizontalHeaderLabels({"Phone", "Source URL"});
    phonesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    phonesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QWidget *rightWidget = new QWidget(this);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0,0,0,0);
    rightLayout->addWidget(emailsTable, 1);
    rightLayout->addWidget(phonesTable, 1);

    bottomProgressBar = new QProgressBar(this);
    bottomProgressBar->setRange(0, 1);
    bottomProgressBar->setValue(0);
    bottomProgressBar->setTextVisible(true);

    QHBoxLayout *topBar = new QHBoxLayout();
    topBar->addWidget(urlBar);
    topBar->addWidget(crawlButton);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(linksTable, 1);
    mainLayout->addWidget(webView, 3);
    mainLayout->addWidget(rightWidget, 1);

    QVBoxLayout *root = new QVBoxLayout(central);
    root->addLayout(topBar);
    root->addWidget(topProgressBar);
    root->addLayout(mainLayout);
    root->addWidget(bottomProgressBar);

    setWindowTitle("Priority Email & Phone Crawler");
    resize(1200, 800);

    netman = new QNetworkAccessManager(this);

    loadPersistentSets();
}

MainWindow::~MainWindow()
{
    savePersistentSets();
}

void MainWindow::loadPersistentSets()
{
    // Simple file-based load (seenUrls and seenPageHashes). Files in current working dir.
    QFile f1("seen_urls.txt");
    if (f1.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&f1);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty()) seenUrls.insert(line);
        }
        f1.close();
    }
    QFile f2("seen_hashes.txt");
    if (f2.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&f2);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty()) seenPageHashes.insert(line);
        }
        f2.close();
    }
}

void MainWindow::savePersistentSets()
{
    QFile f1("seen_urls.txt");
    if (f1.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&f1);
        for (const QString &s : seenUrls) out << s << "\n";
        f1.close();
    }
    QFile f2("seen_hashes.txt");
    if (f2.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&f2);
        for (const QString &h : seenPageHashes) out << h << "\n";
        f2.close();
    }
}

QString MainWindow::canonicalizeUrl(const QUrl &url)
{
    if (!url.isValid()) return QString();
    QUrl u = url;
    u.setFragment(QString());
    if ((u.scheme() == "http" && u.port() == 80) || (u.scheme() == "https" && u.port() == 443))
        u.setPort(-1);

    // Normalize path: remove duplicate slashes
    QString path = u.path();
    path.replace(QRegularExpression("/{2,}"), "/");
    u.setPath(path);

    // Sort query parameters to reduce duplicates
    QUrlQuery q(u);
    QList<QPair<QString,QString>> items = q.queryItems();
    std::sort(items.begin(), items.end(), [](const QPair<QString,QString> &a, const QPair<QString,QString> &b){
        if (a.first == b.first) return a.second < b.second;
        return a.first < b.first;
    });
    QUrlQuery q2;
    for (auto &p : items) q2.addQueryItem(p.first, p.second);
    u.setQuery(q2);

    return u.toString(QUrl::RemoveUserInfo | QUrl::RemoveFragment | QUrl::StripTrailingSlash);
}

QString MainWindow::domainOf(const QUrl &url)
{
    return url.host().toLower();
}

double MainWindow::domainTrust(const QUrl &url)
{
    QString host = domainOf(url);
    if (host.endsWith(".gov")) return 5.0;
    if (host.endsWith(".edu")) return 4.0;
    if (host.endsWith(".org")) return 2.0;
    if (host.endsWith(".com")) return 1.5;
    return 0.5;
}

double MainWindow::noveltyBonus(const QString &host)
{
    int total = hostTotalCount.value(host, 0);
    if (total == 0) return noveltyBonusValue;
    if (total < 5) return noveltyBonusValue * 0.5;
    return 0.0;
}

double MainWindow::scoreUrl(const QUrl &url, int depth)
{
    double dt = domainTrust(url);
    int inbound = inboundCounts.value(domainOf(url), 0);
    int pathLen = url.path().length();

    double base = wDomainTrust * dt
                + wDepth * depth
                + wInbound * inbound
                + wPathLength * (pathLen / 10.0);

    QString host = domainOf(url);
    double novelty = noveltyBonus(host);
    return base + novelty;
}

void MainWindow::enqueueUrl(const QUrl &url, int depth, int inboundDelta)
{
    if (!url.isValid()) return;
    QString norm = canonicalizeUrl(url);
    if (norm.isEmpty()) return;

    QString host = domainOf(url);
    {
        QMutexLocker locker(&frontierMutex);
        if (seenUrls.contains(norm) || queuedUrls.contains(norm)) {
            if (inboundDelta != 0) inboundCounts[host] += inboundDelta;
            return;
        }

        if (inboundDelta != 0) inboundCounts[host] += inboundDelta;

        CrawlItem item;
        item.url = QUrl(norm);
        item.depth = depth;
        item.inboundCount = inboundCounts.value(host, 0);
        item.score = scoreUrl(item.url, depth);

        perHostQueues[host].q.push(item);
        queuedUrls.insert(norm);
    }

    updateLinkTable();
    updateCrawlProgress();
}

void MainWindow::onCrawlClicked()
{
    QString text = urlBar->text().trimmed();
    if (text.isEmpty()) return;

    QUrl seed = QUrl::fromUserInput(text);
    if (!seed.isValid()) return;

    // Reset state
    {
        QMutexLocker locker(&frontierMutex);
        perHostQueues.clear();
        queuedUrls.clear();
        hostActiveCount.clear();
        hostTotalCount.clear();
        hostNextAllowed.clear();
        hostRobotsDisallow.clear();
    }
    seenEmails.clear();
    seenPhones.clear();
    // keep seenUrls and seenPageHashes persistent across runs (loaded at startup)
    linksTable->setRowCount(0);
    emailsTable->setRowCount(0);
    phonesTable->setRowCount(0);
    pagesCrawledCount = 0;
    isProcessing = false;
    consecutiveSameHost = 0;
    lastHost.clear();

    enqueueUrl(seed, 0);
    updateLinkTable();
    updateCrawlProgress();
    scheduleNext();
}

void MainWindow::onLoadProgress(int progress)
{
    topProgressBar->setVisible(true);
    topProgressBar->setValue(progress);
}

bool MainWindow::isAllowedByRobots(const QUrl &url)
{
    QString host = domainOf(url);
    QString path = url.path();
    QMutexLocker locker(&frontierMutex);
    if (!hostRobotsDisallow.contains(host)) return true; // no robots info -> allow by default
    const QStringList &dis = hostRobotsDisallow[host];
    for (const QString &prefix : dis) {
        if (path.startsWith(prefix)) return false;
    }
    return true;
}

void MainWindow::fetchRobotsIfNeeded(const QString &host)
{
    QMutexLocker locker(&frontierMutex);
    if (hostRobotsDisallow.contains(host)) return; // already fetched
    locker.unlock();

    QUrl robotsUrl;
    robotsUrl.setScheme("https");
    robotsUrl.setHost(host);
    robotsUrl.setPath("/robots.txt");

    QNetworkRequest req(robotsUrl);
    QNetworkReply *r = netman->get(req);
    connect(r, &QNetworkReply::finished, this, [this, r, host]() {
        if (r->error() == QNetworkReply::NoError) {
            QByteArray data = r->readAll();
            QString text = QString::fromUtf8(data);
            QStringList disallowList;
            QStringList lines = text.split('\n');
            bool applies = false;
            for (QString ln : lines) {
                ln = ln.trimmed();
                if (ln.isEmpty()) continue;
                if (ln.startsWith("#")) continue;
                if (ln.startsWith("User-agent:", Qt::CaseInsensitive)) {
                    QString ua = ln.mid(QString("User-agent:").length()).trimmed();
                    applies = (ua == "*" || ua.contains("Mozilla") || ua.contains("curl"));
                } else if (applies && ln.startsWith("Disallow:", Qt::CaseInsensitive)) {
                    QString path = ln.mid(QString("Disallow:").length()).trimmed();
                    if (!path.isEmpty()) disallowList << path;
                } else if (ln.startsWith("User-agent:", Qt::CaseInsensitive)) {
                    applies = false;
                }
            }
            QMutexLocker locker(&frontierMutex);
            hostRobotsDisallow[host] = disallowList;
        } else {
            QMutexLocker locker(&frontierMutex);
            hostRobotsDisallow[host] = QStringList(); // treat as no restrictions
        }
        r->deleteLater();
    });
}

void MainWindow::preflightAndMaybeLoad(const CrawlItem &item)
{
    // Preflight HEAD to check Content-Type and small size
    QUrl u = item.url;
    QNetworkRequest req(u);
    req.setHeader(QNetworkRequest::UserAgentHeader, "AddressCrawler/1.0 (+https://example)");
    req.setRawHeader("Accept", "text/html");
    QNetworkReply *reply = netman->head(req);

    connect(reply, &QNetworkReply::finished, this, [this, reply, item]() {
        bool allow = true;
        if (reply->error() != QNetworkReply::NoError) {
            allow = false;
        } else {
            QVariant ct = reply->header(QNetworkRequest::ContentTypeHeader);
            QString cts = ct.toString().toLower();
            if (!cts.contains("text/html")) allow = false;
            QVariant cl = reply->header(QNetworkRequest::ContentLengthHeader);
            if (cl.isValid()) {
                qint64 len = cl.toLongLong();
                if (len > 10 * 1024 * 1024) allow = false; // skip huge pages
            }
        }
        reply->deleteLater();

        if (!allow) {
            // Skip this URL: mark as seen and free host slot, then schedule next
            QString norm = canonicalizeUrl(item.url);
            {
                QMutexLocker locker(&frontierMutex);
                if (!norm.isEmpty()) {
                    seenUrls.insert(norm);
                    queuedUrls.remove(norm);
                }
                QString host = domainOf(item.url);
                if (hostActiveCount.value(host,0) > 0) hostActiveCount[host] -= 1;
                hostNextAllowed[host] = QDateTime::currentDateTimeUtc().addMSecs(politenessDelayMs);
            }
            isProcessing = false;
            updateLinkTable();
            updateCrawlProgress();
            QTimer::singleShot(0, this, &MainWindow::scheduleNext);
            return;
        }

        // Allowed: load into webView
        webView->load(item.url);
    });
}

void MainWindow::onLoadFinished(bool ok)
{
    topProgressBar->setVisible(false);
    topProgressBar->setValue(0);

    if (!isProcessing) return;

    if (!ok) {
        qDebug() << "Failed to load:" << currentItem.url;
        // mark as seen and free host slot
        QString norm = canonicalizeUrl(currentItem.url);
        {
            QMutexLocker locker(&frontierMutex);
            if (!norm.isEmpty()) {
                seenUrls.insert(norm);
                queuedUrls.remove(norm);
            }
            if (hostActiveCount.value(currentHost,0) > 0) hostActiveCount[currentHost] -= 1;
            hostNextAllowed[currentHost] = QDateTime::currentDateTimeUtc().addMSecs(politenessDelayMs);
        }
        isProcessing = false;
        QTimer::singleShot(0, this, &MainWindow::scheduleNext);
        return;
    }

    CrawlItem processingItem = currentItem;

    // 1) Get visible text and compute SHA256
    webView->page()->runJavaScript(QStringLiteral("document.body ? document.body.innerText : ''"),
        [this, processingItem](const QVariant &vText) {
            QString bodyText;
            if (vText.isValid() && vText.canConvert<QString>()) {
                bodyText = vText.toString();
            } else {
                bodyText = QString();
            }

            QByteArray bytes = bodyText.toUtf8();
            QByteArray sha = QCryptographicHash::hash(bytes, QCryptographicHash::Sha256);
            QString shaHex = QString::fromUtf8(sha.toHex());

            // Check duplicate content
            {
                QMutexLocker locker(&frontierMutex);
                if (seenPageHashes.contains(shaHex)) {
                    qDebug() << "Duplicate page content (SHA256) detected, skipping:" << processingItem.url;
                    // mark URL as seen and free host slot
                    QString norm = canonicalizeUrl(processingItem.url);
                    if (!norm.isEmpty()) {
                        seenUrls.insert(norm);
                        queuedUrls.remove(norm);
                    }
                    if (hostActiveCount.value(currentHost,0) > 0) hostActiveCount[currentHost] -= 1;
                    hostNextAllowed[currentHost] = QDateTime::currentDateTimeUtc().addMSecs(politenessDelayMs);
                    // do NOT increment pagesCrawledCount (we didn't process)
                    isProcessing = false;
                    updateLinkTable();
                    updateCrawlProgress();
                    QTimer::singleShot(0, this, &MainWindow::scheduleNext);
                    return;
                } else {
                    seenPageHashes.insert(shaHex);
                }
            }

            // New content: extract emails/phones from visible text
            if (!bodyText.isEmpty()) {
                extractEmailsFromText(bodyText, processingItem.url);
                extractPhonesFromText(bodyText, processingItem.url);
            }

            // Extract mailto anchors
            webView->page()->runJavaScript(
                QStringLiteral("Array.from(document.querySelectorAll('a[href^=\"mailto:\"]')).map(a=>a.getAttribute('href'))"),
                [this, processingItem](const QVariant &vMailto) {
                    if (vMailto.isValid()) processMailtoListVariant(vMailto, processingItem.url);

                    // Extract tel anchors
                    webView->page()->runJavaScript(
                        QStringLiteral("Array.from(document.querySelectorAll('a[href^=\"tel:\"]')).map(a=>a.getAttribute('href'))"),
                        [this, processingItem](const QVariant &vTel) {
                            if (vTel.isValid()) processTelListVariant(vTel, processingItem.url);

                            // Extract links for crawling
                            webView->page()->runJavaScript(
                                QStringLiteral("Array.from(document.links).map(l=>l.href)"),
                                [this, processingItem](const QVariant &vLinks) {
                                    if (vLinks.isValid()) {
                                        extractLinksFromJsResult(vLinks, processingItem.url, processingItem.depth);
                                    }

                                    // Mark processed and free host slot
                                    {
                                        QMutexLocker locker(&frontierMutex);
                                        QString norm = canonicalizeUrl(processingItem.url);
                                        if (!norm.isEmpty()) {
                                            seenUrls.insert(norm);
                                            queuedUrls.remove(norm);
                                        }
                                        if (hostActiveCount.value(currentHost,0) > 0) hostActiveCount[currentHost] -= 1;
                                        hostNextAllowed[currentHost] = QDateTime::currentDateTimeUtc().addMSecs(politenessDelayMs);
                                        hostTotalCount[currentHost] += 1;
                                    }

                                    pagesCrawledCount++;
                                    isProcessing = false;
                                    updateLinkTable();
                                    updateEmailTable();
                                    updatePhoneTable();
                                    updateCrawlProgress();
                                    QTimer::singleShot(0, this, &MainWindow::scheduleNext);
                                }
                            );
                        }
                    );
                }
            );
        }
    );
}

void MainWindow::extractLinksFromJsResult(const QVariant &variantList, const QUrl &baseUrl, int baseDepth)
{
    if (!variantList.canConvert<QVariantList>()) return;
    QVariantList list = variantList.toList();

    QSet<QString> discovered;
    for (const QVariant &v : list) {
        if (!v.canConvert<QString>()) continue;
        QString href = v.toString().trimmed();
        if (href.isEmpty()) continue;

        QUrl resolved(href);
        if (!resolved.isValid()) resolved = baseUrl.resolved(QUrl(href));
        if (!resolved.isValid()) continue;

        QString norm = canonicalizeUrl(resolved);
        if (norm.isEmpty()) continue;
        if (discovered.contains(norm)) continue;

        QString host = domainOf(resolved);
        {
            QMutexLocker locker(&frontierMutex);
            if (seenUrls.contains(norm) || queuedUrls.contains(norm)) {
                inboundCounts[host] += 1;
                continue;
            }
            discovered.insert(norm);
            inboundCounts[host] += 1;
        }

        int newDepth = baseDepth + 1;
        // Respect robots before enqueueing
        if (!isAllowedByRobots(resolved)) continue;
        enqueueUrl(resolved, newDepth, 0);
    }
}

void MainWindow::extractEmailsFromText(const QString &text, const QUrl &source)
{
    if (text.isEmpty()) return;

    QString t = text;
    t.replace(QRegularExpression(R"(\[at\]|\(at\)|\s+at\s+)", QRegularExpression::CaseInsensitiveOption), "@");
    t.replace(QRegularExpression(R"(\[dot\]|\(dot\)|\s+dot\s+)", QRegularExpression::CaseInsensitiveOption), ".");
    t.replace(QRegularExpression(R"(\s*@\s*)"), "@");
    t.replace(QRegularExpression(R"(\s*\.\s*)"), ".");
    t.replace(" at ", "@");

    QRegularExpression emailRx(R"(([\w\.\-+%]+@[\w\.\-]+\.[A-Za-z]{2,}))");
    QRegularExpressionMatchIterator it = emailRx.globalMatch(t);
    while (it.hasNext()) {
        QRegularExpressionMatch m = it.next();
        QString email = m.captured(1).toLower();
        if (email.isEmpty()) continue;

        QMutexLocker locker(&frontierMutex);
        if (!seenEmails.contains(email)) {
            seenEmails.insert(email);
            int row = emailsTable->rowCount();
            emailsTable->insertRow(row);
            emailsTable->setItem(row, 0, new QTableWidgetItem(email));
            emailsTable->setItem(row, 1, new QTableWidgetItem(source.toString()));
        }
    }
}

void MainWindow::processMailtoListVariant(const QVariant &variantList, const QUrl &source)
{
    if (!variantList.canConvert<QVariantList>()) return;
    QVariantList list = variantList.toList();

    for (const QVariant &v : list) {
        if (!v.canConvert<QString>()) continue;
        QString href = v.toString().trimmed();
        if (href.isEmpty()) continue;

        if (href.startsWith("mailto:", Qt::CaseInsensitive)) {
            QString after = href.mid(7);
            int qpos = after.indexOf('?');
            if (qpos != -1) after = after.left(qpos);
            QStringList parts = after.split(',', Qt::SkipEmptyParts);
            for (QString p : parts) {
                QString email = p.trimmed().toLower();
                email.replace(QRegularExpression(R"(\[at\]|\(at\)|\s+at\s+)", QRegularExpression::CaseInsensitiveOption), "@");
                email.replace(QRegularExpression(R"(\[dot\]|\(dot\)|\s+dot\s+)", QRegularExpression::CaseInsensitiveOption), ".");
                email.replace(QRegularExpression(R"(\s*@\s*)"), "@");
                email.replace(QRegularExpression(R"(\s*\.\s*)"), ".");
                if (email.isEmpty()) continue;

                QMutexLocker locker(&frontierMutex);
                if (!seenEmails.contains(email)) {
                    QRegularExpression emailRx(R"((^[\w\.\-+%]+@[\w\.\-]+\.[A-Za-z]{2,}$))");
                    if (emailRx.match(email).hasMatch()) {
                        seenEmails.insert(email);
                        int row = emailsTable->rowCount();
                        emailsTable->insertRow(row);
                        emailsTable->setItem(row, 0, new QTableWidgetItem(email));
                        emailsTable->setItem(row, 1, new QTableWidgetItem(source.toString()));
                    }
                }
            }
        }
    }
}

QString MainWindow::normalizePhone(const QString &raw)
{
    QString s = raw;
    s.replace(QRegularExpression(R"(\[dash\]|\(dash\)|\s+dash\s+)", QRegularExpression::CaseInsensitiveOption), "-");
    QString out;
    for (QChar c : s) {
        if (c.isDigit() || c == '+') out.append(c);
    }
    return out;
}

void MainWindow::extractPhonesFromText(const QString &text, const QUrl &source)
{
    if (text.isEmpty()) return;

    QString t = text;
    t.replace(QRegularExpression(R"(\s*\(\s*)"), "(");
    t.replace(QRegularExpression(R"(\s*\)\s*)"), ")");

    QRegularExpression phoneRx(R"((?:\+1[\s\-\.]?)?(?:\(?\d{3}\)?[\s\-\.]?\d{3}[\s\-\.]?\d{4}))");
    QRegularExpressionMatchIterator it = phoneRx.globalMatch(t);
    while (it.hasNext()) {
        QRegularExpressionMatch m = it.next();
        QString raw = m.captured(0);
        QString normalized = normalizePhone(raw);
        if (normalized.isEmpty()) continue;

        QMutexLocker locker(&frontierMutex);
        if (!seenPhones.contains(normalized)) {
            seenPhones.insert(normalized);
            int row = phonesTable->rowCount();
            phonesTable->insertRow(row);
            phonesTable->setItem(row, 0, new QTableWidgetItem(normalized));
            phonesTable->setItem(row, 1, new QTableWidgetItem(source.toString()));
        }
    }
}

void MainWindow::processTelListVariant(const QVariant &variantList, const QUrl &source)
{
    if (!variantList.canConvert<QVariantList>()) return;
    QVariantList list = variantList.toList();

    for (const QVariant &v : list) {
        if (!v.canConvert<QString>()) continue;
        QString href = v.toString().trimmed();
        if (href.isEmpty()) continue;

        if (href.startsWith("tel:", Qt::CaseInsensitive)) {
            QString after = href.mid(4);
            int qpos = after.indexOf('?');
            if (qpos != -1) after = after.left(qpos);
            QString normalized = normalizePhone(after);
            if (normalized.isEmpty()) continue;

            QMutexLocker locker(&frontierMutex);
            if (!seenPhones.contains(normalized)) {
                seenPhones.insert(normalized);
                int row = phonesTable->rowCount();
                phonesTable->insertRow(row);
                phonesTable->setItem(row, 0, new QTableWidgetItem(normalized));
                phonesTable->setItem(row, 1, new QTableWidgetItem(source.toString()));
            }
        }
    }
}

void MainWindow::updateLinkTable()
{
    // Build a combined list of top items from each host for display
    struct DisplayItem { double score; int depth; QString url; QString host; };
    std::vector<DisplayItem> items;

    {
        QMutexLocker locker(&frontierMutex);
        for (auto it = perHostQueues.begin(); it != perHostQueues.end(); ++it) {
            std::queue<CrawlItem> copy = it->q;
            while (!copy.empty()) {
                CrawlItem ci = copy.front(); copy.pop();
                items.push_back({ci.score, ci.depth, ci.url.toString(), it.key()});
            }
        }
    }

    std::sort(items.begin(), items.end(), [](const DisplayItem &a, const DisplayItem &b){
        return a.score > b.score;
    });

    linksTable->setRowCount(0);
    for (const auto &it : items) {
        int row = linksTable->rowCount();
        linksTable->insertRow(row);
        linksTable->setItem(row, 0, new QTableWidgetItem(QString::number(it.score, 'f', 2)));
        linksTable->setItem(row, 1, new QTableWidgetItem(QString::number(it.depth)));
        linksTable->setItem(row, 2, new QTableWidgetItem(it.url));
    }

    if (isProcessing) {
        linksTable->insertRow(0);
        linksTable->setItem(0, 0, new QTableWidgetItem(QString::number(currentItem.score, 'f', 2)));
        linksTable->setItem(0, 1, new QTableWidgetItem(QString::number(currentItem.depth)));
        linksTable->setItem(0, 2, new QTableWidgetItem(currentItem.url.toString()));
    }
}

void MainWindow::updateEmailTable()
{
    emailsTable->resizeColumnsToContents();
}

void MainWindow::updatePhoneTable()
{
    phonesTable->resizeColumnsToContents();
}

void MainWindow::updateCrawlProgress()
{
    int planned = pagesCrawledCount;
    {
        QMutexLocker locker(&frontierMutex);
        // count queued items
        int queued = 0;
        for (auto it = perHostQueues.begin(); it != perHostQueues.end(); ++it) queued += it->q.size();
        planned += queued;
    }
    if (isProcessing) planned += 1;
    if (planned <= 0) planned = 1;

    bottomProgressBar->setRange(0, planned);
    bottomProgressBar->setValue(pagesCrawledCount);
    bottomProgressBar->setFormat(QString("Processed %1 / %2").arg(pagesCrawledCount).arg(planned));
}

void MainWindow::markHostAccessed(const QString &host)
{
    QMutexLocker locker(&frontierMutex);
    hostNextAllowed[host] = QDateTime::currentDateTimeUtc().addMSecs(politenessDelayMs);
}

void MainWindow::scheduleNext()
{
    if (isProcessing) return;

    // Global limits
    if (pagesCrawledCount >= maxTotalPages) {
        qDebug() << "Reached max total pages limit.";
        return;
    }

    // Choose best host and item respecting quotas, politeness, and domain hop limit
    QString bestHost;
    CrawlItem bestItem;
    double bestScore = -1e18;

    QDateTime now = QDateTime::currentDateTimeUtc();

    {
        QMutexLocker locker(&frontierMutex);

        // If domain hop limit reached for lastHost, prefer other hosts
        bool forceDifferentHost = (consecutiveSameHost >= domainHopLimit && !lastHost.isEmpty());

        for (auto it = perHostQueues.begin(); it != perHostQueues.end(); ++it) {
            QString host = it.key();
            if (it->q.empty()) continue;
            if (hostActiveCount.value(host,0) >= maxConcurrentPerHost) continue;
            if (hostTotalCount.value(host,0) >= maxPerHostPerRun) continue;
            if (hostNextAllowed.value(host, QDateTime()) > now) continue;
            if (forceDifferentHost && host == lastHost) continue;

            CrawlItem candidate = it->q.front();
            double s = candidate.score;
            if (s > bestScore) {
                bestScore = s;
                bestHost = host;
                bestItem = candidate;
            }
        }

        // If none found and we forced different host, relax constraint
        if (bestHost.isEmpty() && consecutiveSameHost >= domainHopLimit) {
            for (auto it = perHostQueues.begin(); it != perHostQueues.end(); ++it) {
                QString host = it.key();
                if (it->q.empty()) continue;
                if (hostActiveCount.value(host,0) >= maxConcurrentPerHost) continue;
                if (hostTotalCount.value(host,0) >= maxPerHostPerRun) continue;
                if (hostNextAllowed.value(host, QDateTime()) > now) continue;
                CrawlItem candidate = it->q.front();
                double s = candidate.score;
                if (s > bestScore) {
                    bestScore = s;
                    bestHost = host;
                    bestItem = candidate;
                }
            }
        }

        if (bestHost.isEmpty()) {
            // nothing ready now
            return;
        }

        // Pop chosen item
        perHostQueues[bestHost].q.pop();
        QString norm = canonicalizeUrl(bestItem.url);
        if (!norm.isEmpty()) queuedUrls.remove(norm);

        // Mark active
        hostActiveCount[bestHost] += 1;
        // We'll increment hostTotalCount when page processed successfully
    }

    // Before loading, ensure robots fetched for host
    fetchRobotsIfNeeded(bestHost);

    // Check robots allow
    if (!isAllowedByRobots(bestItem.url)) {
        // mark as seen and free host slot
        QString norm = canonicalizeUrl(bestItem.url);
        {
            QMutexLocker locker(&frontierMutex);
            if (!norm.isEmpty()) seenUrls.insert(norm);
            if (hostActiveCount.value(bestHost,0) > 0) hostActiveCount[bestHost] -= 1;
            hostNextAllowed[bestHost] = QDateTime::currentDateTimeUtc().addMSecs(politenessDelayMs);
        }
        // schedule next
        QTimer::singleShot(0, this, &MainWindow::scheduleNext);
        return;
    }

    // Set current item and host, update consecutive host counters
    currentItem = bestItem;
    currentHost = bestHost;
    if (lastHost == currentHost) {
        consecutiveSameHost++;
    } else {
        consecutiveSameHost = 1;
        lastHost = currentHost;
    }

    // Preflight HEAD then load
    isProcessing = true;
    currentItem.score = scoreUrl(currentItem.url, currentItem.depth); // refresh score
    updateLinkTable();
    preflightAndMaybeLoad(currentItem);
}

// ----------------------------- main -----------------------------
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Initialize Qt WebEngine (Qt5)
    QtWebEngine::initialize();

    MainWindow w;
    w.show();

    return app.exec();
}

#include "addresscrawler.moc"

