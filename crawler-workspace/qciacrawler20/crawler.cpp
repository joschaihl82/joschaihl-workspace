// crawler.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QListWidget>
#include <QLabel>
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QWebEngineUrlRequestInterceptor>
#include <QWebEngineUrlRequestInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QRegularExpression>
#include <QDateTime>
#include <QThread>
#include <QMutex>
#include <QSplitter>
#include <QUrl>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QGroupBox>
#include <QDebug>

namespace TechSpecs {
    // Tune this to your environment (CPU, network). Higher = more parallel requests.
    const int THREAD_POOL_SIZE = 16;
    const int MAX_CRAWL_DEPTH = 5;
    const QString DB_NAME = "enterprise_visual_crawler.db";
}

// --- Image blocker to prevent loading images (blocks network requests for images) ---
class ImageBlocker : public QWebEngineUrlRequestInterceptor {
public:
    explicit ImageBlocker(QObject *parent = nullptr) : QWebEngineUrlRequestInterceptor(parent) {}
    void interceptRequest(QWebEngineUrlRequestInfo &info) override {
        if (info.resourceType() == QWebEngineUrlRequestInfo::ResourceTypeImage) {
            info.block(true);
        }
    }
};

// --- WORKER CLASS (declaration only) ---
class CrawlerWorker : public QObject {
    Q_OBJECT
public:
    explicit CrawlerWorker(int id);

signals:
    void nodeAvailable(int id);

public slots:
    void init();
    void executeTask(const QString &url, int depth);

private:
    int m_id;
    QNetworkAccessManager *m_nam;

    // Cached prepared statements for this worker's DB connection
    QSqlQuery m_qInsertLead;
    QSqlQuery m_qInsertQueue;
    QSqlQuery m_qUpdateQueue;

    // Connection name for this worker
    QString m_connName;

    // Precompiled regex accessors
    static QRegularExpression &emailRegex();
    static QRegularExpression &linkRegex();
};

// Precompile regexes once (no extra option to avoid Qt version mismatch)
QRegularExpression &CrawlerWorker::emailRegex() {
    static QRegularExpression re(R"([\w\.-]+@[\w\.-]+\.[a-zA-Z]{2,})");
    return re;
}
QRegularExpression &CrawlerWorker::linkRegex() {
    static QRegularExpression re(R"(href=["'](http[s]?://[^"']+)["'])");
    return re;
}

// --- MAIN UI (declaration only) ---
class VisualCommandCenter : public QMainWindow {
    Q_OBJECT
public:
    VisualCommandCenter();

private slots:
    void onStartClicked();
    void coordinator();
    void setupVisualUI();
    void setupDatabase();
    void initThreadPool();

private:
    // UI members
    QWebEngineView *m_browser = nullptr;
    QLineEdit *m_urlInput = nullptr;
    QPushButton *m_btnStart = nullptr;
    QProgressBar *m_progress = nullptr;
    QLabel *m_lblStats = nullptr;
    QListWidget *m_listLeads = nullptr;

    // Worker management
    QMap<int, CrawlerWorker*> m_workers;
    QList<int> m_availableNodes;
    QMutex m_mutex;
    bool m_paused;
};

// --- WORKER IMPLEMENTATION ---
CrawlerWorker::CrawlerWorker(int id)
    : QObject(nullptr), m_id(id), m_nam(nullptr) {
    m_connName = QString("worker_db_%1").arg(m_id);
}

void CrawlerWorker::init() {
    m_nam = new QNetworkAccessManager(this);

    // Create/open per-worker DB connection and set fast pragmas
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_connName);
    db.setDatabaseName(TechSpecs::DB_NAME);
    if (!db.open()) {
        qWarning() << "Worker" << m_id << "failed to open DB:" << db.lastError().text();
        return;
    }

    // Performance-oriented pragmas (WAL, less synchronous, in-memory temp)
    QSqlQuery pragma(db);
    pragma.exec("PRAGMA journal_mode = WAL");
    pragma.exec("PRAGMA synchronous = OFF");
    pragma.exec("PRAGMA temp_store = MEMORY");
    pragma.exec("PRAGMA cache_size = 20000");

    // Prepare and cache statements for reuse (avoids repeated prepare overhead)
    m_qInsertLead = QSqlQuery(db);
    m_qInsertLead.prepare("INSERT OR IGNORE INTO leads (email, source) VALUES (?, ?)");

    m_qInsertQueue = QSqlQuery(db);
    m_qInsertQueue.prepare("INSERT OR IGNORE INTO queue (url, status, depth) VALUES (?, 0, ?)");

    m_qUpdateQueue = QSqlQuery(db);
    m_qUpdateQueue.prepare("UPDATE queue SET status = ? WHERE url = ?");

    emit nodeAvailable(m_id);
}

void CrawlerWorker::executeTask(const QString &url, int depth) {
    if (!m_nam) {
        qWarning() << "NetworkAccessManager not initialized for worker" << m_id;
        emit nodeAvailable(m_id);
        return;
    }

    QNetworkRequest req{ QUrl(url) };
    req.setRawHeader(QByteArray("User-Agent"), QByteArray("VisualCrawler/6.0"));

    QNetworkReply *reply = m_nam->get(req);
    if (!reply) {
        qWarning() << "Failed to start network request for" << url;
        emit nodeAvailable(m_id);
        return;
    }

    connect(reply, &QNetworkReply::finished, this, [this, reply, url, depth]() {
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "Network error for" << url << ":" << reply->errorString();
            reply->deleteLater();
            emit nodeAvailable(m_id);
            return;
        }

        const QByteArray raw = reply->readAll();
        if (raw.isEmpty()) {
            reply->deleteLater();
            emit nodeAvailable(m_id);
            return;
        }
        const QString html = QString::fromUtf8(raw);

        QSqlDatabase db = QSqlDatabase::database(m_connName);
        if (!db.isOpen()) {
            if (!db.open()) {
                qWarning() << "Worker" << m_id << "could not open DB connection:" << db.lastError().text();
                reply->deleteLater();
                emit nodeAvailable(m_id);
                return;
            }
        }

        if (!db.transaction()) {
            qWarning() << "Worker" << m_id << "failed to start transaction:" << db.lastError().text();
        }

        // Extract emails using precompiled regex
        auto it = emailRegex().globalMatch(html);
        while (it.hasNext()) {
            const QString email = it.next().captured(0).toLower();
            m_qInsertLead.addBindValue(email);
            m_qInsertLead.addBindValue(url);
            if (!m_qInsertLead.exec()) {
                qWarning() << "Insert lead failed:" << m_qInsertLead.lastError().text();
            }
            m_qInsertLead.clear();
        }

        // Extract links using precompiled regex and insert into queue
        if (depth < TechSpecs::MAX_CRAWL_DEPTH) {
            auto lIt = linkRegex().globalMatch(html);
            int count = 0;
            while (lIt.hasNext() && count++ < 15) {
                const QString foundUrl = lIt.next().captured(1);
                m_qInsertQueue.addBindValue(foundUrl);
                m_qInsertQueue.addBindValue(depth + 1);
                if (!m_qInsertQueue.exec()) {
                    qWarning() << "Insert queue failed:" << m_qInsertQueue.lastError().text();
                }
                m_qInsertQueue.clear();
            }
        }

        // Mark this URL as completed (status = 1)
        m_qUpdateQueue.addBindValue(1);
        m_qUpdateQueue.addBindValue(url);
        if (!m_qUpdateQueue.exec()) {
            qWarning() << "Update queue failed:" << m_qUpdateQueue.lastError().text();
        }
        m_qUpdateQueue.clear();

        if (!db.commit()) {
            qWarning() << "Commit failed for worker" << m_id << ":" << db.lastError().text();
            db.rollback();
        }

        reply->deleteLater();
        emit nodeAvailable(m_id);
    });
}

// --- MAIN UI IMPLEMENTATION ---
VisualCommandCenter::VisualCommandCenter()
    : QMainWindow(nullptr), m_paused(true) {
    setupDatabase();
    setupVisualUI();
    initThreadPool();
}

void VisualCommandCenter::onStartClicked() {
    m_paused = !m_paused;
    m_btnStart->setText(m_paused ? "RESUME ENGINE" : "PAUSE ENGINE");

    if (!m_paused && m_urlInput->isEnabled()) {
        QSqlDatabase db = QSqlDatabase::database("main_conn");
        if (!db.isOpen() && !db.open()) {
            qWarning() << "Main DB open failed:" << db.lastError().text();
            return;
        }
        QSqlQuery q(db);
        q.prepare("INSERT OR IGNORE INTO queue (url, status, depth) VALUES (?, 0, 0)");
        q.addBindValue(m_urlInput->text());
        if (!q.exec()) {
            qWarning() << "Failed to insert seed URL:" << q.lastError().text();
        }
        m_urlInput->setEnabled(false);

        // Immediately try to schedule work (no delay)
        QMetaObject::invokeMethod(this, "coordinator", Qt::QueuedConnection);
    }
}

void VisualCommandCenter::coordinator() {
    if (m_paused) return;

    // Quick check: if no available nodes, skip
    m_mutex.lock();
    if (m_availableNodes.isEmpty()) { m_mutex.unlock(); return; }
    const int availableSlots = m_availableNodes.size();
    m_mutex.unlock();

    QSqlDatabase db = QSqlDatabase::database("main_conn");
    if (!db.isOpen() && !db.open()) {
        qWarning() << "Main DB open failed in coordinator:" << db.lastError().text();
        return;
    }

    // Build query string with LIMIT embedded to avoid drivers that don't support binding LIMIT
    QSqlQuery q(db);
    QString qstr = QString("SELECT url, depth FROM queue WHERE status = 0 LIMIT %1").arg(QString::number(availableSlots));
    if (!q.exec(qstr)) {
        qWarning() << "Coordinator query failed:" << q.lastError().text();
        return;
    }

    while (q.next()) {
        m_mutex.lock();
        if (m_availableNodes.isEmpty()) { m_mutex.unlock(); break; }
        const int nodeId = m_availableNodes.takeFirst();
        m_mutex.unlock();

        const QString url = q.value(0).toString();
        const int depth = q.value(1).toInt();

        QSqlQuery up(db);
        up.prepare("UPDATE queue SET status = 3 WHERE url = ?");
        up.addBindValue(url);
        if (!up.exec()) {
            qWarning() << "Failed to mark queue item as in-progress:" << up.lastError().text();
        }

        if (nodeId == 0 && m_browser) {
            m_browser->setUrl(QUrl(url));
        }

        CrawlerWorker *workerObj = m_workers.value(nodeId, nullptr);
        if (workerObj) {
            QMetaObject::invokeMethod(workerObj, "executeTask", Qt::QueuedConnection,
                                      Q_ARG(QString, url), Q_ARG(int, depth));
        } else {
            qWarning() << "No worker found for nodeId" << nodeId;
            m_mutex.lock();
            m_availableNodes.append(nodeId);
            m_mutex.unlock();
        }
    }
}

void VisualCommandCenter::setupVisualUI() {
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, central);

    // --- LEFT SIDE: THE BROWSER (3/4) ---
    m_browser = new QWebEngineView(splitter);

    // Defensive: disable auto image loading
    m_browser->page()->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, false);

    // Attach interceptor to block image requests at the network level
    // Qt6 uses setUrlRequestInterceptor
    m_browser->page()->profile()->setUrlRequestInterceptor(new ImageBlocker(this));

    // Optional: disable other heavy features
    m_browser->page()->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, false);
    m_browser->page()->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, false);

    m_browser->setMinimumWidth(800);

    // --- RIGHT SIDE: COMMAND PANEL (1/4) ---
    QWidget *sidebar = new QWidget(splitter);
    QVBoxLayout *sideLayout = new QVBoxLayout(sidebar);

    QGroupBox *grpControl = new QGroupBox("Mission Control");
    QVBoxLayout *lytCtrl = new QVBoxLayout(grpControl);
    m_urlInput = new QLineEdit("https://www.wikipedia.org");
    m_btnStart = new QPushButton("START ENGINE");
    lytCtrl->addWidget(new QLabel("Seed URL:"));
    lytCtrl->addWidget(m_urlInput);
    lytCtrl->addWidget(m_btnStart);

    QGroupBox *grpStats = new QGroupBox("Metrics");
    QVBoxLayout *lytStats = new QVBoxLayout(grpStats);
    m_progress = new QProgressBar();
    m_lblStats = new QLabel("Standby...");
    lytStats->addWidget(m_progress);
    lytStats->addWidget(m_lblStats);

    m_listLeads = new QListWidget();

    sideLayout->addWidget(grpControl);
    sideLayout->addWidget(grpStats);
    sideLayout->addWidget(new QLabel("Leads Found:"));
    sideLayout->addWidget(m_listLeads);

    splitter->addWidget(m_browser);
    splitter->addWidget(sidebar);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);

    mainLayout->addWidget(splitter);
    resize(1400, 800);

    connect(m_btnStart, &QPushButton::clicked, this, &VisualCommandCenter::onStartClicked);
}

void VisualCommandCenter::setupDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "main_conn");
    db.setDatabaseName(TechSpecs::DB_NAME);
    if (!db.open()) {
        qWarning() << "Failed to open main DB:" << db.lastError().text();
        return;
    }
    QSqlQuery q(db);
    if (!q.exec("CREATE TABLE IF NOT EXISTS queue (url TEXT PRIMARY KEY, status INTEGER, depth INTEGER)")) {
        qWarning() << "Failed to create queue table:" << q.lastError().text();
    }
    if (!q.exec("CREATE TABLE IF NOT EXISTS leads (email TEXT PRIMARY KEY, source TEXT)")) {
        qWarning() << "Failed to create leads table:" << q.lastError().text();
    }

    // Apply main DB pragmas as well
    QSqlQuery pragma(db);
    pragma.exec("PRAGMA journal_mode = WAL");
    pragma.exec("PRAGMA synchronous = OFF");
    pragma.exec("PRAGMA temp_store = MEMORY");
    pragma.exec("PRAGMA cache_size = 20000");
}

void VisualCommandCenter::initThreadPool() {
    for (int i = 0; i < TechSpecs::THREAD_POOL_SIZE; ++i) {
        QThread *t = new QThread(this);
        CrawlerWorker *w = new CrawlerWorker(i);
        w->moveToThread(t);

        connect(t, &QThread::started, w, &CrawlerWorker::init);
        connect(w, &CrawlerWorker::nodeAvailable, this, [this](int id){
            m_mutex.lock();
            m_availableNodes << id;
            m_mutex.unlock();
            // Immediately try to schedule work (no delay)
            QMetaObject::invokeMethod(this, "coordinator", Qt::QueuedConnection);
        });

        t->start();
        m_workers.insert(i, w);
    }
}

// --- main ---
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    VisualCommandCenter w;
    w.show();
    return a.exec();
}

#include "crawler.moc"

