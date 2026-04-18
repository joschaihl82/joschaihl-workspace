// fast_crawler.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QPlainTextEdit>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QSet>
#include <QMutex>
#include <QMutexLocker>
#include <QThreadPool>
#include <QRunnable>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QRegularExpression>
#include <QDateTime>
#include <QMap>
#include <QTimer>
#include <QAtomicInt>
#include <QDebug>
#include <queue>

struct CrawlTask {
    QUrl url;
    int priority;
    int depth;
    bool operator<(const CrawlTask &other) const { return priority > other.priority; } // for std::priority_queue
};

class ThreadSafeQueue {
public:
    void push(const CrawlTask &t) {
        QMutexLocker lock(&m_mutex);
        if (m_queue.size() >= m_maxSize) return;
        m_queue.push(t);
    }
    bool tryPop(CrawlTask &out) {
        QMutexLocker lock(&m_mutex);
        if (m_queue.empty()) return false;
        out = m_queue.top();
        m_queue.pop();
        return true;
    }
    int size() {
        QMutexLocker lock(&m_mutex);
        return (int)m_queue.size();
    }
    void clear() {
        QMutexLocker lock(&m_mutex);
        while (!m_queue.empty()) m_queue.pop();
    }
    void setMaxSize(int s) { m_maxSize = s; }
private:
    std::priority_queue<CrawlTask> m_queue;
    QMutex m_mutex;
    int m_maxSize = 100000;
};

class DbHelper {
public:
    static QSqlDatabase openConnection(const QString &connName) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
        db.setDatabaseName("crawler_results.db");
        if (!db.open()) {
            qWarning() << "DB open failed:" << db.lastError().text();
        } else {
            QSqlQuery q(db);
            q.exec("PRAGMA journal_mode = WAL;");
            q.exec("PRAGMA synchronous = NORMAL;");
            q.exec("CREATE TABLE IF NOT EXISTS pages (url TEXT PRIMARY KEY, title TEXT, content TEXT, fetched_at INTEGER)");
        }
        return db;
    }
};

class FetchTask : public QRunnable {
public:
    FetchTask(const CrawlTask &task,
              ThreadSafeQueue *queue,
              QSet<QUrl> *visited,
              QMutex *visitedMutex,
              QAtomicInt *activeWorkers,
              bool enableJsRendering,
              int maxDepth,
              QObject *parentNotifier)
        : m_task(task), m_queue(queue), m_visited(visited),
          m_visitedMutex(visitedMutex), m_activeWorkers(activeWorkers),
          m_enableJsRendering(enableJsRendering), m_maxDepth(maxDepth),
          m_notifier(parentNotifier) {
        setAutoDelete(true);
    }

    void run() override {
        // Each thread must open its own DB connection
        QString connName = QString("conn_%1").arg((quintptr)QThread::currentThreadId());
        QSqlDatabase db = DbHelper::openConnection(connName);
        QSqlQuery insertQuery(db);
        insertQuery.prepare("INSERT OR REPLACE INTO pages (url, title, content, fetched_at) VALUES (?, ?, ?, ?)");

        QNetworkAccessManager nam;
        QEventLoop loop;
        QNetworkReply *reply = nam.get(QNetworkRequest(m_task.url));
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        // Timeout safety
        QTimer timer;
        timer.setSingleShot(true);
        QObject::connect(&timer, &QTimer::timeout, [&]() {
            if (reply->isRunning()) reply->abort();
        });
        timer.start(15000); // 15s timeout
        loop.exec();

        if (reply->error() == QNetworkReply::NoError) {
            QByteArray body = reply->readAll();
            QString html = QString::fromUtf8(body);
            QString title = extractTitle(html);
            QString text = extractTextSnippet(html);

            // Save to DB
            insertQuery.addBindValue(m_task.url.toString());
            insertQuery.addBindValue(title);
            insertQuery.addBindValue(text);
            insertQuery.addBindValue(QDateTime::currentSecsSinceEpoch());
            if (!insertQuery.exec()) {
                qWarning() << "DB insert failed:" << insertQuery.lastError().text();
            }

            // Extract links and push new tasks
            if (m_task.depth < m_maxDepth) {
                QList<QUrl> links = extractLinks(html, m_task.url);
                for (const QUrl &link : links) {
                    if (!link.isValid()) continue;
                    // Heuristic: skip mailto, javascript, fragments
                    if (link.scheme() != "http" && link.scheme() != "https") continue;
                    // Determine priority
                    int priority = (link.host().endsWith(".gov") || link.host().endsWith(".mil") || link.host().endsWith(".us")) ? 0 : 10;
                    // Check visited and push
                    {
                        QMutexLocker lock(m_visitedMutex);
                        if (m_visited->contains(link)) continue;
                        m_visited->insert(link);
                    }
                    m_queue->push({link, priority, m_task.depth + 1});
                }
            }

            // If JS rendering is enabled and heuristics say so, notify UI to render (optional)
            if (m_enableJsRendering && looksLikeRequiresJs(html)) {
                // Notify main thread to render this URL with QWebEngineView (signal)
                QMetaObject::invokeMethod(m_notifier, "enqueueJsRender", Qt::QueuedConnection, Q_ARG(QString, m_task.url.toString()));
            }
        } else {
            qDebug() << "Fetch failed for" << m_task.url << ":" << reply->errorString();
        }

        reply->deleteLater();
        db.close();
        QSqlDatabase::removeDatabase(connName);

        m_activeWorkers->fetchAndAddRelaxed(-1);
        // Notify UI to update progress
        QMetaObject::invokeMethod(m_notifier, "workerFinished", Qt::QueuedConnection);
    }

private:
    static QString extractTitle(const QString &html) {
        QRegularExpression re("<title[^>]*>(.*?)</title>", QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
        auto m = re.match(html);
        if (m.hasMatch()) return m.captured(1).trimmed();
        return QString();
    }
    static QString extractTextSnippet(const QString &html, int maxLen = 5000) {
        // Very simple strip tags -> not perfect but fast
        QString s = html;
        s.remove(QRegularExpression("<script[^>]*>.*?</script>", QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption));
        s.remove(QRegularExpression("<style[^>]*>.*?</style>", QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption));
        s.replace(QRegularExpression("<[^>]+>"), " ");
        s = s.simplified();
        if (s.length() > maxLen) s = s.left(maxLen);
        return s;
    }
    static QList<QUrl> extractLinks(const QString &html, const QUrl &base) {
        QList<QUrl> out;
        QRegularExpression re("href\\s*=\\s*['\"]([^'\"#>]+)['\"]", QRegularExpression::CaseInsensitiveOption);
        auto it = re.globalMatch(html);
        while (it.hasNext()) {
            auto m = it.next();
            QString href = m.captured(1).trimmed();
            QUrl u = base.resolved(QUrl::fromUserInput(href));
            if (u.isValid()) out.append(u);
        }
        return out;
    }
    static bool looksLikeRequiresJs(const QString &html) {
        // Heuristic: presence of <noscript> or heavy JS frameworks
        if (html.contains("<noscript", Qt::CaseInsensitive)) return true;
        if (html.contains("window.__", Qt::CaseInsensitive)) return true;
        if (html.contains("ReactDOM", Qt::CaseInsensitive) || html.contains("angular", Qt::CaseInsensitive) || html.contains("vue", Qt::CaseInsensitive)) return true;
        return false;
    }

    CrawlTask m_task;
    ThreadSafeQueue *m_queue;
    QSet<QUrl> *m_visited;
    QMutex *m_visitedMutex;
    QAtomicInt *m_activeWorkers;
    bool m_enableJsRendering;
    int m_maxDepth;
    QObject *m_notifier;
};

class CrawlerWindow : public QMainWindow {
    Q_OBJECT
public:
    CrawlerWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setupUI();
        initDatabase();

        m_queue.setMaxSize(200000);
        m_maxDepth = 2;
        m_enableJsRendering = false; // default off for speed
        m_maxWorkers = 8; // tune for your machine
        m_activeWorkers.storeRelaxed(0);

        connect(m_startBtn, &QPushButton::clicked, this, &CrawlerWindow::startInitialCrawl);
        connect(&m_uiTimer, &QTimer::timeout, this, &CrawlerWindow::dispatchWork);
        m_uiTimer.start(500); // dispatch every 500ms
    }

public slots:
    void startInitialCrawl() {
        QUrl url = QUrl::fromUserInput(m_urlEdit->text());
        if (!url.isValid()) return;
        // Reset
        {
            QMutexLocker lock(&m_visitedMutex);
            m_visited.clear();
            m_visited.insert(url);
        }
        m_queue.clear();
        m_progressBar->setValue(0);
        m_progressBar->setMaximum(1);
        // push initial task
        int priority = (url.host().endsWith(".gov") || url.host().endsWith(".mil") || url.host().endsWith(".us")) ? 0 : 10;
        m_queue.push({url, priority, 0});
        m_totalVisited = 0;
        dispatchWork();
    }

    void dispatchWork() {
        // spawn workers up to limit
        while (m_activeWorkers.loadRelaxed() < m_maxWorkers) {
            CrawlTask task;
            if (!m_queue.tryPop(task)) break;
            m_activeWorkers.fetchAndAddRelaxed(1);
            m_totalVisited++;
            updateProgress();
            // Create and start fetch task
            FetchTask *ft = new FetchTask(task, &m_queue, &m_visited, &m_visitedMutex, &m_activeWorkers, m_enableJsRendering, m_maxDepth, this);
            QThreadPool::globalInstance()->start(ft);
        }
    }

    // Called from worker threads via invokeMethod
    void workerFinished() {
        updateProgress();
    }

    // Optional: enqueue URL for JS rendering in main thread (not implemented heavy)
    void enqueueJsRender(const QString &url) {
        // For speed we keep this minimal: log and optionally render with a single hidden QWebEngineView
        qDebug() << "JS render requested for" << url;
        // Implementation left as an exercise: maintain a small pool of QWebEnginePage in main thread and render asynchronously.
    }

private:
    void setupUI() {
        QWidget *central = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(central);

        QHBoxLayout *topLayout = new QHBoxLayout();
        m_urlEdit = new QLineEdit();
        m_urlEdit->setPlaceholderText("Enter URL to start crawling...");
        m_startBtn = new QPushButton("Start Crawl");
        topLayout->addWidget(m_urlEdit);
        topLayout->addWidget(m_startBtn);

        m_progressBar = new QProgressBar();
        m_progressBar->setFixedHeight(25);
        m_progressBar->setFormat("Visited: %v | Queue: %m");

        m_log = new QPlainTextEdit();
        m_log->setReadOnly(true);
        m_log->setMaximumBlockCount(1000);

        layout->addLayout(topLayout);
        layout->addWidget(m_progressBar);
        layout->addWidget(m_log);

        setCentralWidget(central);
        resize(900, 600);
        setWindowTitle("Fast Qt6 Concurrent Crawler");
    }

    void initDatabase() {
        // Open a connection in the main thread for initial setup
        QSqlDatabase db = DbHelper::openConnection("main_conn");
        db.close();
        QSqlDatabase::removeDatabase("main_conn");
    }

    void updateProgress() {
        QMetaObject::invokeMethod(this, [this]() {
            int visitedSize;
            {
                QMutexLocker lock(&m_visitedMutex);
                visitedSize = m_visited.size();
            }
            int qsize = m_queue.size();
            m_progressBar->setMaximum(visitedSize + qsize + 1);
            m_progressBar->setValue(visitedSize);
            m_log->appendPlainText(QString("[%1] Visited=%2 Queue=%3 Active=%4")
                                   .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                                   .arg(visitedSize)
                                   .arg(qsize)
                                   .arg(m_activeWorkers.loadRelaxed()));
            // Keep log short
            if (m_log->blockCount() > 800) {
                m_log->clear();
            }
        }, Qt::QueuedConnection);
    }

private:
    QLineEdit *m_urlEdit;
    QPushButton *m_startBtn;
    QProgressBar *m_progressBar;
    QPlainTextEdit *m_log;

    ThreadSafeQueue m_queue;
    QSet<QUrl> m_visited;
    QMutex m_visitedMutex;

    QAtomicInt m_activeWorkers;
    int m_maxWorkers;
    int m_maxDepth;
    bool m_enableJsRendering;
    QTimer m_uiTimer;
    int m_totalVisited = 0;
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    // Increase threadpool if desired
    QThreadPool::globalInstance()->setMaxThreadCount(16);

    CrawlerWindow w;
    w.show();
    return a.exec();
}

#include "crawler.moc"

