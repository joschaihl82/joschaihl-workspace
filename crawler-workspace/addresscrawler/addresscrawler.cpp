// addresscrawler.cpp
// Single-file Qt6 application: Priority-aware email & phone crawler with UI.
// Requires Qt6 Widgets, WebEngineWidgets, WebEngineCore, Network.
// Build with qmake/CMake linking Qt6::Widgets Qt6::WebEngineWidgets Qt6::Network

#include <QApplication>
#include <QMainWindow>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QWebEngineUrlRequestInterceptor>
#include <QWebEngineUrlRequestInfo>
#include <QtWebEngine>   // preferred
// or, if your Qt exposes it differently:

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
        // Allow only main frame and subframe requests (HTML pages).
        // Block images, stylesheets, scripts, fonts, media, favicons, etc.
        auto type = info.resourceType();

        // Use the enum values directly (unscoped) to be compatible across Qt versions
        switch (type) {
        case QWebEngineUrlRequestInfo::ResourceTypeMainFrame:
        case QWebEngineUrlRequestInfo::ResourceTypeSubFrame:
            // Force Accept header to text/html
            info.setHttpHeader("Accept", QByteArray("text/html"));
            break;

        default:
            // Block everything else to avoid fetching non-HTML media
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

    bool operator<(CrawlItem const& other) const {
        // priority_queue pops the largest element first; keep this semantics
        return score < other.score;
    }
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

    // Frontier and dedup
    std::priority_queue<CrawlItem> frontier;
    QSet<QString> seenUrls;
    QSet<QString> seenEmails;
    QSet<QString> seenPhones;
    QMutex frontierMutex;

    // Tracking inbound counts for popularity scoring
    QHash<QString,int> inboundCounts;

    // Current item being processed
    CrawlItem currentItem;
    bool isProcessing = false;

    // Crawl counters for bottom progress
    int pagesCrawledCount = 0;

    // Configurable weights for scoring
    double wDomainTrust = 2.0;
    double wDepth = -1.0;
    double wInbound = 1.0;
    double wPathLength = -0.5;

    // Helpers
    double scoreUrl(const QUrl &url, int depth);
    double domainTrust(const QUrl &url);
    void enqueueUrl(const QUrl &url, int depth, int inboundDelta = 0);
    void extractLinksFromJsResult(const QVariant &variantList, const QUrl &baseUrl, int baseDepth);
    QString normalizeUrl(const QUrl &url);
    QString domainOf(const QUrl &url);

    // Email helpers
    void extractEmailsFromText(const QString &text, const QUrl &source);
    void processMailtoListVariant(const QVariant &variantList, const QUrl &source);

    // Phone helpers
    void extractPhonesFromText(const QString &text, const QUrl &source);
    void processTelListVariant(const QVariant &variantList, const QUrl &source);
    QString normalizePhone(const QString &raw);
};

// ----------------------------- Implementation -----------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // URL bar and crawl button
    urlBar = new QLineEdit(this);
    urlBar->setPlaceholderText("https://example.com");

    crawlButton = new QPushButton("Crawl", this);
    connect(crawlButton, &QPushButton::clicked, this, &MainWindow::onCrawlClicked);

    // Top progress bar (page loading)
    topProgressBar = new QProgressBar(this);
    topProgressBar->setRange(0, 100);
    topProgressBar->setValue(0);
    topProgressBar->setTextVisible(true);
    topProgressBar->setVisible(false);

    // Web view
    webView = new QWebEngineView(this);
    webView->setContextMenuPolicy(Qt::NoContextMenu);
    connect(webView, &QWebEngineView::loadProgress, this, &MainWindow::onLoadProgress);
    connect(webView, &QWebEngineView::loadFinished, this, &MainWindow::onLoadFinished);

    // Install request interceptor on profile to block non-HTML resources
    RequestInterceptor *interceptor = new RequestInterceptor(this);
    QWebEngineProfile *profile = webView->page()->profile();
    // Use the correct API name available in Qt: setUrlRequestInterceptor
    profile->setUrlRequestInterceptor(interceptor);

    // Links table (left)
    linksTable = new QTableWidget(this);
    linksTable->setColumnCount(3);
    linksTable->setHorizontalHeaderLabels({"Priority", "Depth", "URL"});
    linksTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    linksTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Emails and phones (right)
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

    // Bottom progress bar (crawl progress)
    bottomProgressBar = new QProgressBar(this);
    bottomProgressBar->setRange(0, 1);
    bottomProgressBar->setValue(0);
    bottomProgressBar->setTextVisible(true);

    // Layouts
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
}

MainWindow::~MainWindow()
{
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
        while (!frontier.empty()) frontier.pop();
    }
    seenUrls.clear();
    seenEmails.clear();
    seenPhones.clear();
    inboundCounts.clear();
    linksTable->setRowCount(0);
    emailsTable->setRowCount(0);
    phonesTable->setRowCount(0);
    pagesCrawledCount = 0;
    isProcessing = false;

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

void MainWindow::onLoadFinished(bool ok)
{
    topProgressBar->setVisible(false);
    topProgressBar->setValue(0);

    if (!isProcessing) return;

    if (!ok) {
        qDebug() << "Failed to load:" << currentItem.url;
        pagesCrawledCount++;
        updateCrawlProgress();
        isProcessing = false;
        QTimer::singleShot(100, this, &MainWindow::scheduleNext);
        return;
    }

    CrawlItem processingItem = currentItem;

    // 1) Extract visible text for emails and phones
    webView->page()->runJavaScript(QStringLiteral("document.body ? document.body.innerText : ''"),
        [this, processingItem](const QVariant &vText) {
            if (vText.isValid() && vText.canConvert<QString>()) {
                QString bodyText = vText.toString();
                extractEmailsFromText(bodyText, processingItem.url);
                extractPhonesFromText(bodyText, processingItem.url);
            } else {
                qDebug() << "innerText JS returned invalid variant.";
            }

            // 2) Extract mailto anchors
            webView->page()->runJavaScript(
                QStringLiteral("Array.from(document.querySelectorAll('a[href^=\"mailto:\"]')).map(a=>a.getAttribute('href'))"),
                [this, processingItem](const QVariant &vMailto) {
                    if (vMailto.isValid()) {
                        processMailtoListVariant(vMailto, processingItem.url);
                    } else {
                        qDebug() << "mailto JS returned invalid variant.";
                    }

                    // 3) Extract tel anchors
                    webView->page()->runJavaScript(
                        QStringLiteral("Array.from(document.querySelectorAll('a[href^=\"tel:\"]')).map(a=>a.getAttribute('href'))"),
                        [this, processingItem](const QVariant &vTel) {
                            if (vTel.isValid()) {
                                processTelListVariant(vTel, processingItem.url);
                            } else {
                                qDebug() << "tel JS returned invalid variant.";
                            }

                            // 4) Extract links for crawling
                            webView->page()->runJavaScript(
                                QStringLiteral("Array.from(document.links).map(l=>l.href)"),
                                [this, processingItem](const QVariant &vLinks) {
                                    if (!vLinks.isValid()) {
                                        qDebug() << "links JS returned invalid variant.";
                                    } else {
                                        extractLinksFromJsResult(vLinks, processingItem.url, processingItem.depth);
                                    }

                                    // Mark crawled and schedule next
                                    pagesCrawledCount++;
                                    updateCrawlProgress();

                                    isProcessing = false;
                                    updateLinkTable();
                                    updateEmailTable();
                                    updatePhoneTable();
                                    QTimer::singleShot(200, this, &MainWindow::scheduleNext);
                                }
                            );
                        }
                    );
                }
            );
        }
    );
}

double MainWindow::domainTrust(const QUrl &url)
{
    QString host = url.host().toLower();
    if (host.endsWith(".gov") || host.endsWith(".gov.")) return 5.0;
    if (host.endsWith(".edu") || host.endsWith(".edu.")) return 4.0;
    if (host.endsWith(".org")) return 2.0;
    if (host.endsWith(".com")) return 1.5;
    return 0.5;
}

double MainWindow::scoreUrl(const QUrl &url, int depth)
{
    double dt = domainTrust(url);
    int inbound = inboundCounts.value(domainOf(url), 0);
    int pathLen = url.path().length();

    double score = wDomainTrust * dt
                 + wDepth * depth
                 + wInbound * inbound
                 + wPathLength * (pathLen / 10.0);

    return score;
}

QString MainWindow::domainOf(const QUrl &url)
{
    return url.host().toLower();
}

QString MainWindow::normalizeUrl(const QUrl &url)
{
    QUrl u = url;
    u.setFragment(QString());
    if ((u.scheme() == "http" && u.port() == 80) || (u.scheme() == "https" && u.port() == 443))
        u.setPort(-1);
    return u.toString(QUrl::RemoveUserInfo | QUrl::RemoveFragment | QUrl::StripTrailingSlash);
}

void MainWindow::enqueueUrl(const QUrl &url, int depth, int inboundDelta)
{
    if (!url.isValid()) return;
    QString norm = normalizeUrl(url);
    if (norm.isEmpty()) return;

    if (seenUrls.contains(norm)) {
        if (inboundDelta != 0) inboundCounts[domainOf(url)] += inboundDelta;
        updateCrawlProgress();
        return;
    }

    if (inboundDelta != 0) inboundCounts[domainOf(url)] += inboundDelta;

    double sc = scoreUrl(url, depth);
    CrawlItem item{url, sc, depth, inboundCounts.value(domainOf(url), 0)};

    {
        QMutexLocker locker(&frontierMutex);
        frontier.push(item);
    }

    updateLinkTable();
    updateCrawlProgress();
}

void MainWindow::scheduleNext()
{
    if (isProcessing) return;

    CrawlItem next;
    {
        QMutexLocker locker(&frontierMutex);
        if (frontier.empty()) {
            qDebug() << "Frontier empty: done.";
            updateCrawlProgress();
            return;
        }
        next = frontier.top();
        frontier.pop();
    }

    currentItem = next;
    isProcessing = true;

    QString norm = normalizeUrl(currentItem.url);
    seenUrls.insert(norm);

    updateLinkTable();

    webView->load(currentItem.url);
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
        if (!resolved.isValid()) {
            resolved = baseUrl.resolved(QUrl(href));
        } else {
            resolved = QUrl(resolved);
        }

        if (!resolved.isValid()) continue;

        QString norm = normalizeUrl(resolved);
        if (norm.isEmpty()) continue;
        if (seenUrls.contains(norm)) continue;
        if (discovered.contains(norm)) continue;

        discovered.insert(norm);

        inboundCounts[domainOf(resolved)] += 1;

        int newDepth = baseDepth + 1;
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
                if (!email.isEmpty() && !seenEmails.contains(email)) {
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
        } else {
            if (href.contains("mailto:", Qt::CaseInsensitive)) {
                int pos = href.indexOf("mailto:", 0, Qt::CaseInsensitive);
                QString after = href.mid(pos + 7);
                int qpos = after.indexOf('?');
                if (qpos != -1) after = after.left(qpos);
                QStringList parts = after.split(',', Qt::SkipEmptyParts);
                for (QString p : parts) {
                    QString email = p.trimmed().toLower();
                    email.replace(QRegularExpression(R"(\[at\]|\(at\)|\s+at\s+)", QRegularExpression::CaseInsensitiveOption), "@");
                    email.replace(QRegularExpression(R"(\[dot\]|\(dot\)|\s+dot\s+)", QRegularExpression::CaseInsensitiveOption), ".");
                    email.replace(QRegularExpression(R"(\s*@\s*)"), "@");
                    email.replace(QRegularExpression(R"(\s*\.\s*)"), ".");
                    if (!email.isEmpty() && !seenEmails.contains(email)) {
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
            if (!normalized.isEmpty() && !seenPhones.contains(normalized)) {
                seenPhones.insert(normalized);
                int row = phonesTable->rowCount();
                phonesTable->insertRow(row);
                phonesTable->setItem(row, 0, new QTableWidgetItem(normalized));
                phonesTable->setItem(row, 1, new QTableWidgetItem(source.toString()));
            }
        } else {
            if (href.contains("tel:", Qt::CaseInsensitive)) {
                int pos = href.indexOf("tel:", 0, Qt::CaseInsensitive);
                QString after = href.mid(pos + 4);
                int qpos = after.indexOf('?');
                if (qpos != -1) after = after.left(qpos);
                QString normalized = normalizePhone(after);
                if (!normalized.isEmpty() && !seenPhones.contains(normalized)) {
                    seenPhones.insert(normalized);
                    int row = phonesTable->rowCount();
                    phonesTable->insertRow(row);
                    phonesTable->setItem(row, 0, new QTableWidgetItem(normalized));
                    phonesTable->setItem(row, 1, new QTableWidgetItem(source.toString()));
                }
            }
        }
    }
}

void MainWindow::updateLinkTable()
{
    std::vector<CrawlItem> items;
    {
        QMutexLocker locker(&frontierMutex);
        std::priority_queue<CrawlItem> copy = frontier;
        while (!copy.empty()) {
            items.push_back(copy.top());
            copy.pop();
        }
    }

    std::sort(items.begin(), items.end(), [](const CrawlItem &a, const CrawlItem &b){
        return a.score > b.score;
    });

    linksTable->setRowCount(0);
    for (const CrawlItem &it : items) {
        int row = linksTable->rowCount();
        linksTable->insertRow(row);
        linksTable->setItem(row, 0, new QTableWidgetItem(QString::number(it.score, 'f', 2)));
        linksTable->setItem(row, 1, new QTableWidgetItem(QString::number(it.depth)));
        linksTable->setItem(row, 2, new QTableWidgetItem(it.url.toString()));
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
        planned += static_cast<int>(frontier.size());
    }
    if (isProcessing) planned += 1;
    if (planned <= 0) planned = 1;

    bottomProgressBar->setRange(0, planned);
    bottomProgressBar->setValue(pagesCrawledCount);
    bottomProgressBar->setFormat(QString("Crawled %1 / %2").arg(pagesCrawledCount).arg(planned));
}

// ----------------------------- main -----------------------------
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Initialize Qt WebEngine
    QtWebEngine::initialize();

    MainWindow w;
    w.show();

    return app.exec();
}

#include "addresscrawler.moc"

