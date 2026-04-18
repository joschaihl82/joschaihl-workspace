#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QPlainTextEdit>
#include <QListWidget>
#include <QLabel>
#include <QWebEngineView>
#include <QWebEngineSettings>
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
#include <QTimer>
#include <QAtomicInt>
#include <QDebug>
#include <queue>

// --- Data Structures ---

struct CrawlTask {
    QUrl url;
    int priority;
    int depth;
    // Priority queue is a max-heap; we invert the comparison so lower number = higher priority
    bool operator<(const CrawlTask &other) const { return priority > other.priority; }
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
    void setMaxSize(size_t s) { m_maxSize = s; }
private:
    std::priority_queue<CrawlTask> m_queue;
    QMutex m_mutex;
    size_t m_maxSize = 100000;
};

// --- Database Helper ---

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
            q.exec("CREATE TABLE IF NOT EXISTS assets (url TEXT PRIMARY KEY, parent_page TEXT, content TEXT)");
        }
        return db;
    }
};

// --- Background Worker ---

class FetchTask : public QRunnable {
public:
    FetchTask(const CrawlTask &task, ThreadSafeQueue *queue, QSet<QUrl> *visited, 
              QMutex *visitedMutex, QAtomicInt *activeWorkers, int maxDepth, QObject *notifier)
        : m_task(task), m_queue(queue), m_visited(visited), m_visitedMutex(visitedMutex), 
          m_activeWorkers(activeWorkers), m_maxDepth(maxDepth), m_notifier(notifier) {
        setAutoDelete(true);
    }

    void run() override {
        QString connName = QString("conn_%1").arg((quintptr)QThread::currentThreadId());
        QSqlDatabase db = DbHelper::openConnection(connName);
        
        QNetworkAccessManager nam;
        QEventLoop loop;
        QNetworkReply *reply = nam.get(QNetworkRequest(m_task.url));
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        
        QTimer timer;
        timer.setSingleShot(true);
        QObject::connect(&timer, &QTimer::timeout, [&]() { if (reply->isRunning()) reply->abort(); });
        timer.start(10000);
        loop.exec();

        if (reply->error() == QNetworkReply::NoError) {
            QByteArray body = reply->readAll();
            QString html = QString::fromUtf8(body);
            
            // 1. Save Page Data
            QSqlQuery q(db);
            q.prepare("INSERT OR REPLACE INTO pages (url, title, content, fetched_at) VALUES (?, ?, ?, ?)");
            q.addBindValue(m_task.url.toString());
            q.addBindValue(extractTitle(html));
            q.addBindValue(extractTextSnippet(html));
            q.addBindValue(QDateTime::currentSecsSinceEpoch());
            q.exec();

            // 2. Extract Links
            if (m_task.depth < m_maxDepth) {
                QRegularExpression re("href\\s*=\\s*['\"]([^'\"#>]+)['\"]", QRegularExpression::CaseInsensitiveOption);
                auto it = re.globalMatch(html);
                while (it.hasNext()) {
                    QString href = it.next().captured(1).trimmed();
                    QUrl u = m_task.url.resolved(QUrl::fromUserInput(href));
                    if (u.isValid() && (u.scheme() == "http" || u.scheme() == "https")) {
                        QMutexLocker lock(m_visitedMutex);
                        if (!m_visited->contains(u)) {
                            m_visited->insert(u);
                            int priority = (u.host().endsWith(".gov") || u.host().endsWith(".mil") || u.host().endsWith(".us")) ? 0 : 10;
                            m_queue->push({u, priority, m_task.depth + 1});
                        }
                    }
                }
            }
        }

        reply->deleteLater();
        db.close();
        QSqlDatabase::removeDatabase(connName);
        m_activeWorkers->fetchAndAddRelaxed(-1);
        QMetaObject::invokeMethod(m_notifier, "workerFinished", Qt::QueuedConnection);
    }

private:
    QString extractTitle(const QString &html) {
        QRegularExpression re("<title[^>]*>(.*?)</title>", QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
        auto m = re.match(html);
        return m.hasMatch() ? m.captured(1).trimmed() : "No Title";
    }
    QString extractTextSnippet(const QString &html) {
        QString s = html;
        s.remove(QRegularExpression("<script[^>]*>.*?</script>", QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption));
        s.remove(QRegularExpression("<style[^>]*>.*?</style>", QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption));
        s.replace(QRegularExpression("<[^>]+>"), " ");
        return s.simplified().left(5000);
    }

    CrawlTask m_task;
    ThreadSafeQueue *m_queue;
    QSet<QUrl> *m_visited;
    QMutex *m_visitedMutex;
    QAtomicInt *m_activeWorkers;
    int m_maxDepth;
    QObject *m_notifier;
};

// --- Main Window ---

class CrawlerWindow : public QMainWindow {
    Q_OBJECT
public:
    CrawlerWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setupUI();
        DbHelper::openConnection("main_setup").close();
        QSqlDatabase::removeDatabase("main_setup");

        m_maxWorkers = 8;
        m_activeWorkers.storeRelaxed(0);
        
        connect(m_startBtn, &QPushButton::clicked, this, &CrawlerWindow::startInitialCrawl);
        connect(&m_uiTimer, &QTimer::timeout, this, &CrawlerWindow::dispatchWork);
        m_uiTimer.start(500);
    }

public slots:
    void startInitialCrawl() {
        QUrl url = QUrl::fromUserInput(m_urlEdit->text());
        if (!url.isValid()) return;
        
        { QMutexLocker lock(&m_visitedMutex); m_visited.clear(); m_visited.insert(url); }
        m_queue.clear();
        m_urlListWidget->clear();
        m_log->clear();
        
        int priority = (url.host().endsWith(".gov") || url.host().endsWith(".mil")) ? 0 : 10;
        m_queue.push({url, priority, 0});
        dispatchWork();
    }

    void dispatchWork() {
        while (m_activeWorkers.loadRelaxed() < m_maxWorkers) {
            CrawlTask task;
            if (!m_queue.tryPop(task)) break;

            m_activeWorkers.fetchAndAddRelaxed(1);
            m_webView->load(task.url);
            m_urlListWidget->addItem(task.url.toString());
            m_urlListWidget->scrollToBottom();

            FetchTask *ft = new FetchTask(task, &m_queue, &m_visited, &m_visitedMutex, &m_activeWorkers, 2, this);
            QThreadPool::globalInstance()->start(ft);
        }
        updateProgress();
    }

    void workerFinished() { updateProgress(); }

private:
    void setupUI() {
        QWidget *central = new QWidget(this);
        QHBoxLayout *mainLayout = new QHBoxLayout(central);

        // Left Side: Web View
        QWidget *leftBox = new QWidget();
        QVBoxLayout *leftLayout = new QVBoxLayout(leftBox);
        m_currentUrlLabel = new QLineEdit();
        m_currentUrlLabel->setReadOnly(true);
        m_webView = new QWebEngineView();
        m_webView->setZoomFactor(0.25);
        m_webView->page()->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, false);
        
        leftLayout->addWidget(m_currentUrlLabel);
        leftLayout->addWidget(m_webView);

        // Right Side: Controls & Logs
        QWidget *rightBox = new QWidget();
        QVBoxLayout *rightLayout = new QVBoxLayout(rightBox);

        m_urlEdit = new QLineEdit();
        m_urlEdit->setPlaceholderText("Start URL...");
        m_startBtn = new QPushButton("Crawl");
        QHBoxLayout *h = new QHBoxLayout();
        h->addWidget(m_urlEdit); h->addWidget(m_startBtn);

        m_progressBar = new QProgressBar();
        m_urlListWidget = new QListWidget();
        m_log = new QPlainTextEdit();
        m_log->setReadOnly(true);

        rightLayout->addLayout(h);
        rightLayout->addWidget(m_progressBar);
        rightLayout->addWidget(new QLabel("Opened URLs:"));
        rightLayout->addWidget(m_urlListWidget, 2);
        rightLayout->addWidget(new QLabel("Logs:"));
        rightLayout->addWidget(m_log, 1);

        mainLayout->addWidget(leftBox, 3);
        mainLayout->addWidget(rightBox, 1);
        setCentralWidget(central);
        resize(1280, 720);
    }

    void updateProgress() {
        int vCount; { QMutexLocker lock(&m_visitedMutex); vCount = m_visited.size(); }
        m_progressBar->setMaximum(vCount + m_queue.size());
        m_progressBar->setValue(vCount);
        m_currentUrlLabel->setText(m_webView->url().toString());
        m_log->appendPlainText(QString("Active: %1 | Queue: %2").arg(m_activeWorkers.loadRelaxed()).arg(m_queue.size()));
    }

    QLineEdit *m_urlEdit, *m_currentUrlLabel;
    QPushButton *m_startBtn;
    QProgressBar *m_progressBar;
    QListWidget *m_urlListWidget;
    QPlainTextEdit *m_log;
    QWebEngineView *m_webView;
    ThreadSafeQueue m_queue;
    QSet<QUrl> m_visited;
    QMutex m_visitedMutex;
    QAtomicInt m_activeWorkers;
    int m_maxWorkers;
    QTimer m_uiTimer;
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QThreadPool::globalInstance()->setMaxThreadCount(16);
    CrawlerWindow w;
    w.show();
    return a.exec();
}

#include "crawler.moc"
