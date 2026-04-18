// mega_crawler_async_with_futurewatcher.cpp
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
#include <QNetworkDiskCache>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QRegularExpression>
#include <QDateTime>
#include <QTimer>
#include <QAtomicInt>
#include <QThread>
#include <QMutex>
#include <QEventLoop>
#include <QFont>
#include <QListWidgetItem>
#include <QSplitter>
#include <QSizePolicy>
#include <QUrl>
#include <QDebug>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>

// Maximale Seiten (3x erhöht)
const int MAX_PAGES = 3000000;

// Prioritätsberechnung: kleinere Werte = höhere Priorität
static int computePriority(const QUrl &u) {
    QString host = u.host().toLower();
    if (host.endsWith(".gov")) return -100;
    if (host.endsWith(".mil")) return -100;
    if (host.endsWith(".us"))  return 1;
    return 10;
}

// --- Datenbank & Queue Logik (Haupt-Connection initialisieren) ---
class CrawlerDB {
public:
    static bool initMain() {
        if (QSqlDatabase::contains("queue_connection")) return true;
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "queue_connection");
        db.setDatabaseName("crawler_huge.db");
        if (!db.open()) {
            qWarning() << "Failed to open main DB:" << db.lastError().text();
            return false;
        }
        QSqlQuery q(db);
        q.exec("PRAGMA journal_mode = WAL;");
        q.exec("PRAGMA synchronous = NORMAL;");
        q.exec("CREATE TABLE IF NOT EXISTS queue (url TEXT PRIMARY KEY, status INTEGER, priority INTEGER, depth INTEGER)");
        q.exec("CREATE TABLE IF NOT EXISTS data (url TEXT PRIMARY KEY, title TEXT, content TEXT)");
        q.exec("CREATE TABLE IF NOT EXISTS contacts (contact TEXT PRIMARY KEY, type TEXT, source_url TEXT)");
        q.exec("CREATE INDEX IF NOT EXISTS idx_status ON queue(status, priority)");
        return true;
    }

    static void addUrlMain(const QUrl &url, int depth, int priority = -1) {
        QSqlDatabase db = QSqlDatabase::database("queue_connection");
        QSqlQuery q(db);
        int prio = priority;
        if (prio < 0) prio = computePriority(url);
        q.prepare("INSERT OR IGNORE INTO queue (url, status, priority, depth) VALUES (?, 0, ?, ?)");
        q.addBindValue(url.toString());
        q.addBindValue(prio);
        q.addBindValue(depth);
        q.exec();
    }
};

// --- Hintergrund-Funktion zum Laden von Kontakten (läuft blocking, wird via QtConcurrent aufgerufen) ---
static QPair<QStringList, QStringList> fetchContactsFromDbBlocking() {
    QStringList emails;
    QStringList phones;
    if (!QSqlDatabase::contains("queue_connection")) return {emails, phones};
    QSqlDatabase db = QSqlDatabase::database("queue_connection");
    QSqlQuery q(db);

    q.prepare("SELECT contact FROM contacts WHERE type = 'email' ORDER BY contact ASC");
    if (q.exec()) {
        while (q.next()) emails << q.value(0).toString();
    }

    q.prepare("SELECT contact FROM contacts WHERE type = 'phone' ORDER BY contact ASC");
    if (q.exec()) {
        while (q.next()) phones << q.value(0).toString();
    }
    return {emails, phones};
}

// --- FetchWorker: läuft in eigenem QThread, asynchron mit QNetworkAccessManager ---
class FetchWorker : public QObject {
    Q_OBJECT
public:
    FetchWorker(int id, QObject *parent = nullptr)
        : QObject(parent), m_id(id), m_nam(nullptr), m_dbName(QString("conn_worker_%1").arg(id)) {}

    ~FetchWorker() {
        if (m_nam) {
            m_nam->deleteLater();
            m_nam = nullptr;
        }
        if (QSqlDatabase::contains(m_dbName)) {
            QSqlDatabase::database(m_dbName).close();
            QSqlDatabase::removeDatabase(m_dbName);
        }
    }

public slots:
    void init() {
        m_nam = new QNetworkAccessManager(this);
        QNetworkDiskCache *cache = new QNetworkDiskCache(this);
        cache->setCacheDirectory(QString("netcache_worker_%1").arg(m_id));
        cache->setMaximumCacheSize(50 * 1024 * 1024);
        m_nam->setCache(cache);

        if (QSqlDatabase::contains(m_dbName)) {
            QSqlDatabase::removeDatabase(m_dbName);
        }
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_dbName);
        db.setDatabaseName("crawler_huge.db");
        if (!db.open()) {
            qWarning() << "Worker" << m_id << "failed to open DB:" << db.lastError().text();
        } else {
            QSqlQuery q(db);
            q.exec("PRAGMA journal_mode = WAL;");
            q.exec("PRAGMA synchronous = NORMAL;");
        }

        emit ready();
    }

    void fetchUrl(const QString &urlStr, int depth) {
        QUrl url(urlStr);
        if (!m_nam) {
            emit finishedFetch(urlStr);
            emit ready();
            return;
        }

        QNetworkRequest req(url);
        req.setRawHeader("Accept-Encoding", "gzip");
        req.setHeader(QNetworkRequest::UserAgentHeader, QString("MegaCrawler/1.0"));

        QNetworkReply *reply = m_nam->get(req);
        connect(reply, &QNetworkReply::finished, this, [this, reply, urlStr, depth]() {
            QByteArray data;
            if (reply->error() == QNetworkReply::NoError) {
                data = reply->readAll();
            } else {
                qDebug() << "Worker" << m_id << "network error for" << urlStr << ":" << reply->errorString();
            }

            processResponse(urlStr, depth, data, reply->error() == QNetworkReply::NoError);
            reply->deleteLater();
            emit finishedFetch(urlStr);
            emit ready();
        });
    }

signals:
    void ready();
    void finishedFetch(const QString &url);

private:
    void processResponse(const QString &urlStr, int depth, const QByteArray &data, bool success) {
        if (!QSqlDatabase::contains(m_dbName)) {
            qWarning() << "Worker" << m_id << "missing DB connection";
            return;
        }
        QSqlDatabase db = QSqlDatabase::database(m_dbName);
        if (!db.isOpen()) {
            if (!db.open()) {
                qWarning() << "Worker" << m_id << "cannot open DB:" << db.lastError().text();
                return;
            }
        }

        QSqlQuery q(db);
        if (!success) {
            q.prepare("UPDATE queue SET status = 2 WHERE url = ?");
            q.addBindValue(urlStr);
            q.exec();
            return;
        }

        QString html = QString::fromUtf8(data);

        QSqlQuery insData(db);
        insData.prepare("INSERT OR REPLACE INTO data (url, title, content) VALUES (?, ?, ?)");
        QString title = html.section("<title>", 1, 1).section("</title>", 0, 0).left(255);
        insData.addBindValue(urlStr);
        insData.addBindValue(title);
        insData.addBindValue(html.left(2000));
        insData.exec();

        QSqlQuery upd(db);
        upd.prepare("UPDATE queue SET status = 1 WHERE url = ?");
        upd.addBindValue(urlStr);
        upd.exec();

        QVector<QPair<QString, QString>> contacts;
        QVector<QPair<QString, int>> newUrls;

        QRegularExpression emailRe(R"(([\w\.\-+%]+@[\w\.\-]+\.[A-Za-z]{2,}))", QRegularExpression::CaseInsensitiveOption);
        auto emailIt = emailRe.globalMatch(html);
        while (emailIt.hasNext()) {
            QString email = emailIt.next().captured(1).trimmed().toLower();
            if (!email.isEmpty()) contacts.append(qMakePair(email, QString("email")));
        }

        QRegularExpression phoneRe(R"((\+?\d[\d\-\s().]{5,}\d))", QRegularExpression::CaseInsensitiveOption);
        auto phoneIt = phoneRe.globalMatch(html);
        while (phoneIt.hasNext()) {
            QString raw = phoneIt.next().captured(1).trimmed();
            QString cleaned;
            for (QChar c : raw) if (c.isDigit() || c == QChar('+')) cleaned.append(c);
            QString digitsOnly;
            for (QChar c : cleaned) if (c.isDigit()) digitsOnly.append(c);
            if (digitsOnly.length() >= 7) contacts.append(qMakePair(cleaned, QString("phone")));
        }

        if (depth < 5) {
            QRegularExpression re("href\\s*=\\s*['\"]([^'\"#>]+)['\"]", QRegularExpression::CaseInsensitiveOption);
            auto it = re.globalMatch(html);
            while (it.hasNext()) {
                QString href = it.next().captured(1);
                QUrl u = QUrl(urlStr).resolved(QUrl::fromUserInput(href));
                if (u.isValid() && u.scheme().startsWith("http")) {
                    newUrls.append(qMakePair(u.toString(), depth + 1));
                }
            }
        }

        db.transaction();
        if (!contacts.isEmpty()) {
            QSqlQuery insC(db);
            insC.prepare("INSERT OR IGNORE INTO contacts (contact, type, source_url) VALUES (?, ?, ?)");
            for (auto &c : contacts) {
                insC.addBindValue(c.first);
                insC.addBindValue(c.second);
                insC.addBindValue(urlStr);
                insC.exec();
            }
        }

        if (!newUrls.isEmpty()) {
            QSqlQuery insQ(db);
            insQ.prepare("INSERT OR IGNORE INTO queue (url, status, priority, depth) VALUES (?, 0, ?, ?)");
            for (auto &nu : newUrls) {
                QUrl u(nu.first);
                int prio = computePriority(u);
                insQ.addBindValue(nu.first);
                insQ.addBindValue(prio);
                insQ.addBindValue(nu.second);
                insQ.exec();
            }
        }
        db.commit();
    }

private:
    int m_id;
    QNetworkAccessManager *m_nam;
    QString m_dbName;
};

// --- Hauptfenster / Dispatcher ---
class MegaCrawler : public QMainWindow {
    Q_OBJECT
public:
    MegaCrawler(int workerCount = 8, QWidget *parent = nullptr)
        : QMainWindow(parent), m_activeWorkers(0), m_workerCount(workerCount),
          m_contactsWatcher(new QFutureWatcher<QPair<QStringList, QStringList>>(this)),
          m_uiTickCounter(0)
    {
        setupUI();
        CrawlerDB::initMain();
        createWorkers(m_workerCount);
        connect(m_startBtn, &QPushButton::clicked, this, &MegaCrawler::start);

        // UI-Refresh Timer: ruft regelmäßig updateStats, dispatch und (seltener) contacts refresh auf
        m_uiTimer = new QTimer(this);
        connect(m_uiTimer, &QTimer::timeout, this, &MegaCrawler::onUiTimerTick);
        m_uiTimer->start(1000); // 1000 ms

        // Kontakte-Watcher: wenn Hintergrundtask fertig ist, UI aktualisieren
        connect(m_contactsWatcher, &QFutureWatcher<QPair<QStringList, QStringList>>::finished,
                this, &MegaCrawler::onContactsLoaded, Qt::QueuedConnection);
    }

    ~MegaCrawler() {
        for (auto &w : m_workers) {
            if (w.worker && w.thread) {
                w.thread->quit();
                w.thread->wait();
                delete w.worker;
                delete w.thread;
            }
        }
    }

public slots:
    void start() {
        QUrl startUrl = QUrl::fromUserInput(m_urlEdit->text());
        CrawlerDB::addUrlMain(startUrl, 0, -1);
        dispatch();
    }

    void onWorkerReady(int workerId) {
        QMutexLocker locker(&m_availMutex);
        if (!m_availableWorkers.contains(workerId)) {
            m_availableWorkers.append(workerId);
        }
        dispatch();
    }

    void onWorkerFinished(const QString &url) {
        Q_UNUSED(url);
        updateStats();
        // don't refresh contacts here (expensive); timer handles it
    }

    void onUiTimerTick() {
        // lightweight updates
        updateStats();
        dispatch();

        // contacts only every 5 seconds (timer interval 1s)
        m_uiTickCounter = (m_uiTickCounter + 1) % 5;
        if (m_uiTickCounter == 0) {
            if (!m_contactsWatcher->isRunning()) {
                QFuture<QPair<QStringList, QStringList>> future = QtConcurrent::run(fetchContactsFromDbBlocking);
                m_contactsWatcher->setFuture(future);
            }
        }
    }

    void dispatch() {
        QMutexLocker locker(&m_availMutex);
        if (m_availableWorkers.isEmpty()) return;

        QSqlDatabase db = QSqlDatabase::database("queue_connection");
        QSqlQuery q(db);
        int limit = m_availableWorkers.size();
        q.prepare("SELECT url, depth FROM queue WHERE status = 0 ORDER BY priority ASC, depth ASC LIMIT ?");
        q.addBindValue(limit);
        q.exec();

        QList<QPair<QString,int>> toDispatch;
        while (q.next()) {
            QString url = q.value(0).toString();
            int depth = q.value(1).toInt();
            toDispatch.append(qMakePair(url, depth));
        }

        for (auto &p : toDispatch) {
            if (m_availableWorkers.isEmpty()) break;
            int workerId = m_availableWorkers.takeFirst();
            QSqlQuery upd(db);
            upd.prepare("UPDATE queue SET status = 3 WHERE url = ?");
            upd.addBindValue(p.first);
            upd.exec();

            FetchWorker *w = m_workers[workerId].worker;
            QMetaObject::invokeMethod(w, "fetchUrl", Qt::QueuedConnection,
                                      Q_ARG(QString, p.first), Q_ARG(int, p.second));

            m_activeWorkers.fetchAndAddRelaxed(1);

            if (m_urlList->count() > 300) delete m_urlList->takeItem(0);
            m_urlList->addItem(p.first);
        }
        updateStats();
    }

private:
    struct WorkerEntry {
        FetchWorker *worker = nullptr;
        QThread *thread = nullptr;
    };

    void createWorkers(int count) {
        for (int i = 0; i < count; ++i) {
            QThread *t = new QThread();
            FetchWorker *w = new FetchWorker(i);
            w->moveToThread(t);

            connect(t, &QThread::started, w, &FetchWorker::init);
            connect(w, &FetchWorker::ready, this, [this, i]() { this->onWorkerReady(i); }, Qt::QueuedConnection);
            connect(w, &FetchWorker::finishedFetch, this, &MegaCrawler::onWorkerFinished, Qt::QueuedConnection);

            t->start();

            WorkerEntry entry;
            entry.worker = w;
            entry.thread = t;
            m_workers.append(entry);
        }
    }

    void setupUI() {
        QWidget *c = new QWidget();
        QHBoxLayout *mainLayout = new QHBoxLayout(c);

        QSplitter *splitter = new QSplitter(Qt::Horizontal, c);

        m_webView = new QWebEngineView();
        m_webView->setZoomFactor(0.75);
        m_webView->page()->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, false);

        QWidget *leftContainer = new QWidget();
        QVBoxLayout *leftLayout = new QVBoxLayout(leftContainer);
        leftLayout->addWidget(m_webView);
        leftLayout->setContentsMargins(0,0,0,0);

        splitter->addWidget(leftContainer);

        QWidget *r = new QWidget();
        QVBoxLayout *rl = new QVBoxLayout(r);
        rl->setContentsMargins(8,8,8,8);
        rl->setSpacing(8);

        QFont rightFont;
        rightFont.setPointSize(12);

        m_urlEdit = new QLineEdit("https://www.usa.gov");
        m_urlEdit->setFont(rightFont);
        m_urlEdit->setMinimumHeight(28);
        m_urlEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        m_startBtn = new QPushButton("Start 3M Crawl");
        m_startBtn->setFont(rightFont);
        m_startBtn->setMinimumHeight(32);
        m_startBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        m_progress = new QProgressBar();
        m_progress->setFont(rightFont);
        m_progress->setMinimumHeight(20);
        m_progress->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        m_statLabel = new QLabel("Stats: 0 visited");
        m_statLabel->setFont(rightFont);
        m_statLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        m_urlList = new QListWidget();
        m_urlList->setFont(rightFont);
        m_urlList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_urlList->setMaximumHeight(200);

        QLabel *emailsTitle = new QLabel("Gefundene E‑Mails:");
        emailsTitle->setFont(rightFont);
        m_emailsList = new QListWidget();
        m_emailsList->setFont(rightFont);
        m_emailsList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_emailsList->setMaximumHeight(200);
        m_emailsList->setSelectionMode(QAbstractItemView::NoSelection);

        QLabel *phonesTitle = new QLabel("Gefundene Telefonnummern:");
        phonesTitle->setFont(rightFont);
        m_phonesList = new QListWidget();
        m_phonesList->setFont(rightFont);
        m_phonesList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_phonesList->setMaximumHeight(200);
        m_phonesList->setSelectionMode(QAbstractItemView::NoSelection);

        rl->addWidget(m_urlEdit);
        rl->addWidget(m_startBtn);
        rl->addWidget(m_statLabel);
        rl->addWidget(m_progress);
        rl->addWidget(new QLabel("Zu besuchende URLs:"));
        rl->addWidget(m_urlList);
        rl->addWidget(emailsTitle);
        rl->addWidget(m_emailsList);
        rl->addWidget(phonesTitle);
        rl->addWidget(m_phonesList);
        rl->addStretch();

        splitter->addWidget(r);
        splitter->setStretchFactor(0, 3);
        splitter->setStretchFactor(1, 1);
        r->setMaximumWidth(700);

        mainLayout->addWidget(splitter);
        setCentralWidget(c);

        resize(1280, 800);
        setWindowTitle("MegaCrawler - Async Workers + UI Refresh (FutureWatcher)");
    }

    void updateStats() {
        QSqlQuery q(QSqlDatabase::database("queue_connection"));
        q.exec("SELECT COUNT(*) FROM queue WHERE status = 1"); q.next();
        int visited = q.value(0).toInt();
        q.exec("SELECT COUNT(*) FROM queue WHERE status = 0"); q.next();
        int pending = q.value(0).toInt();

        int total = visited + pending;
        if (total <= 0) total = 1;
        m_progress->setMaximum(total);
        m_progress->setValue(visited);
        m_progress->setFormat(QString("%1 / %2 Seiten").arg(visited).arg(MAX_PAGES));
        m_statLabel->setText(QString("Warteschlange: %1").arg(pending));
    }

    // Slot: wird aufgerufen, wenn background fetchContactsFromDbBlocking fertig ist
    void onContactsLoaded() {
        auto result = m_contactsWatcher->result();
        const QStringList &emails = result.first;
        const QStringList &phones = result.second;

        // Leichtgewichtige UI-Aktualisierung: replace model items
        m_emailsList->clear();
        for (const QString &e : emails) {
            QLabel *lbl = new QLabel(QString("<a href=\"mailto:%1\">%1</a>").arg(e.toHtmlEscaped()));
            lbl->setTextFormat(Qt::RichText);
            lbl->setTextInteractionFlags(Qt::TextBrowserInteraction);
            lbl->setOpenExternalLinks(true);
            QListWidgetItem *item = new QListWidgetItem(m_emailsList);
            item->setSizeHint(lbl->sizeHint());
            m_emailsList->addItem(item);
            m_emailsList->setItemWidget(item, lbl);
        }

        m_phonesList->clear();
        for (const QString &p : phones) {
            QString telUri;
            for (QChar c : p) if (c.isDigit() || c == QChar('+')) telUri.append(c);
            if (telUri.isEmpty()) continue;
            QLabel *lbl = new QLabel(QString("<a href=\"tel:%1\">%2</a>").arg(telUri, p.toHtmlEscaped()));
            lbl->setTextFormat(Qt::RichText);
            lbl->setTextInteractionFlags(Qt::TextBrowserInteraction);
            lbl->setOpenExternalLinks(true);
            QListWidgetItem *item = new QListWidgetItem(m_phonesList);
            item->setSizeHint(lbl->sizeHint());
            m_phonesList->addItem(item);
            m_phonesList->setItemWidget(item, lbl);
        }
    }

private:
    QWebEngineView *m_webView = nullptr;
    QLineEdit *m_urlEdit = nullptr;
    QPushButton *m_startBtn = nullptr;
    QProgressBar *m_progress = nullptr;
    QListWidget *m_urlList = nullptr;
    QLabel *m_statLabel = nullptr;
    QListWidget *m_emailsList = nullptr;
    QListWidget *m_phonesList = nullptr;

    QVector<WorkerEntry> m_workers;
    QAtomicInt m_activeWorkers;
    int m_workerCount;

    QList<int> m_availableWorkers;
    QMutex m_availMutex;

    QTimer *m_uiTimer = nullptr;
    QFutureWatcher<QPair<QStringList, QStringList>> *m_contactsWatcher;
    int m_uiTickCounter;
    int m_maxWorkers = 8;
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    int ideal = qMax(2, QThread::idealThreadCount());
    int workerCount = ideal * 2;

    MegaCrawler w(workerCount);
    w.show();
    return a.exec();
}

#include "crawler.moc"

