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
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QRegularExpression>
#include <QDateTime>
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QSplitter>
#include <QUrl>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QGroupBox>
#include <QDebug>

namespace TechSpecs {
    const int THREAD_POOL_SIZE = 8;
    const int MAX_CRAWL_DEPTH = 5;
    const QString DB_NAME = "enterprise_visual_crawler.db";
}

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
};

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
    QTimer *m_uiTimer = nullptr;
    bool m_paused;
};

// --- WORKER IMPLEMENTATION ---
CrawlerWorker::CrawlerWorker(int id)
    : QObject(nullptr), m_id(id), m_nam(nullptr) {}

void CrawlerWorker::init() {
    m_nam = new QNetworkAccessManager(this);

    const QString connName = QString("worker_db_%1").arg(m_id);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
    db.setDatabaseName(TechSpecs::DB_NAME);
    if (!db.open()) {
        qWarning() << "Worker" << m_id << "failed to open DB:" << db.lastError().text();
    } else {
        emit nodeAvailable(m_id);
    }
}

void CrawlerWorker::executeTask(const QString &url, int depth) {
    if (!m_nam) {
        qWarning() << "NetworkAccessManager not initialized for worker" << m_id;
        emit nodeAvailable(m_id);
        return;
    }

    // Use brace initialization to avoid vexing-parse warning
    QNetworkRequest req{ QUrl(url) };
    req.setRawHeader(QByteArray("User-Agent"), QByteArray("VisualCrawler/6.0"));

    // Pass the QNetworkRequest to get()
    QNetworkReply *reply = m_nam->get(req);

    connect(reply, &QNetworkReply::finished, this, [this, reply, url, depth]() {
        if (reply->error() == QNetworkReply::NoError) {
            const QString html = QString::fromUtf8(reply->readAll());
            const QString connName = QString("worker_db_%1").arg(m_id);
            QSqlDatabase db = QSqlDatabase::database(connName);
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

            // Extraction: emails
            QRegularExpression eRe(R"([\w\.-]+@[\w\.-]+\.[a-zA-Z]{2,})");
            auto it = eRe.globalMatch(html);
            while (it.hasNext()) {
                const QString email = it.next().captured(0).toLower();
                QSqlQuery q(db);
                q.prepare("INSERT OR IGNORE INTO leads (email, source) VALUES (?, ?)");
                q.addBindValue(email);
                q.addBindValue(url);
                if (!q.exec()) {
                    qWarning() << "Insert lead failed:" << q.lastError().text();
                }
            }

            // Links
            if (depth < TechSpecs::MAX_CRAWL_DEPTH) {
                QRegularExpression lRe(R"(href=["'](http[s]?://[^"']+)["'])");
                auto lIt = lRe.globalMatch(html);
                int count = 0;
                while (lIt.hasNext() && count++ < 15) {
                    const QString foundUrl = lIt.next().captured(1);
                    QSqlQuery q(db);
                    q.prepare("INSERT OR IGNORE INTO queue (url, status, depth) VALUES (?, 0, ?)");
                    q.addBindValue(foundUrl);
                    q.addBindValue(depth + 1);
                    if (!q.exec()) {
                        qWarning() << "Insert queue failed:" << q.lastError().text();
                    }
                }
            }

            QSqlQuery up(db);
            up.prepare("UPDATE queue SET status = 1 WHERE url = ?");
            up.addBindValue(url);
            if (!up.exec()) {
                qWarning() << "Update queue failed:" << up.lastError().text();
            }

            if (!db.commit()) {
                qWarning() << "Commit failed for worker" << m_id << ":" << db.lastError().text();
            }
        } else {
            qWarning() << "Network error for" << url << ":" << reply->errorString();
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

    m_uiTimer = new QTimer(this);
    connect(m_uiTimer, &QTimer::timeout, this, &VisualCommandCenter::coordinator);
    m_uiTimer->start(1000);
}

void VisualCommandCenter::onStartClicked() {
    // Toggle paused state
    m_paused = !m_paused;

    // Update button text: when running (m_paused == false) show "PAUSE ENGINE"
    m_btnStart->setText(m_paused ? "RESUME ENGINE" : "PAUSE ENGINE");

    // On first resume, insert seed URL and disable input
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
    }
}

void VisualCommandCenter::coordinator() {
    if (m_paused) return;

    m_mutex.lock();
    if (m_availableNodes.isEmpty()) { m_mutex.unlock(); return; }

    QSqlDatabase db = QSqlDatabase::database("main_conn");
    if (!db.isOpen() && !db.open()) {
        qWarning() << "Main DB open failed in coordinator:" << db.lastError().text();
        m_mutex.unlock();
        return;
    }

    QSqlQuery q(db);
    // Note: some drivers don't allow binding LIMIT; if that becomes an issue, build the query string.
    q.prepare("SELECT url, depth FROM queue WHERE status = 0 LIMIT ?");
    q.addBindValue(m_availableNodes.size());

    if (q.exec()) {
        while (q.next()) {
            if (m_availableNodes.isEmpty()) break;
            const QString url = q.value(0).toString();
            const int depth = q.value(1).toInt();
            const int nodeId = m_availableNodes.takeFirst();

            QSqlQuery up(db);
            up.prepare("UPDATE queue SET status = 3 WHERE url = ?");
            up.addBindValue(url);
            if (!up.exec()) {
                qWarning() << "Failed to mark queue item as in-progress:" << up.lastError().text();
            }

            // UI Master Update: show in browser for node 0
            if (nodeId == 0 && m_browser) m_browser->setUrl(QUrl(url));

            // Invoke worker slot in its thread
            CrawlerWorker *workerObj = m_workers.value(nodeId, nullptr);
            if (workerObj) {
                QMetaObject::invokeMethod(workerObj, "executeTask", Qt::QueuedConnection,
                                          Q_ARG(QString, url), Q_ARG(int, depth));
            } else {
                qWarning() << "No worker found for nodeId" << nodeId;
                // return node to pool
                m_availableNodes.append(nodeId);
            }
        }
    } else {
        qWarning() << "Coordinator query failed:" << q.lastError().text();
    }

    m_mutex.unlock();
}

void VisualCommandCenter::setupVisualUI() {
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, central);

    // --- LEFT SIDE: THE BROWSER (3/4) ---
    m_browser = new QWebEngineView(splitter);
    m_browser->page()->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, false);
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
            // call coordinator asynchronously to avoid reentrancy issues
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

