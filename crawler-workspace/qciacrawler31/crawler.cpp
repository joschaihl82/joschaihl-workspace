#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QPlainTextEdit>
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
#include <QEventLoop>
#include <QLabel>
#include <queue>

struct CrawlTask {
    QUrl url;
    int priority; // Lower number = higher priority (0 is top)
    int depth;
    // The priority queue returns the "largest" element, 
    // so we invert the comparison to make lower priority numbers come out first.
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
            q.exec("CREATE TABLE IF NOT EXISTS pages (url TEXT PRIMARY KEY, title TEXT, content TEXT, priority INTEGER, fetched_at INTEGER)");
        }
        return db;
    }
};

class FetchTask : public QRunnable {
public:
    FetchTask(const CrawlTask &task, ThreadSafeQueue *queue, QSet<QUrl> *visited, QMutex *visitedMutex, QAtomicInt *activeWorkers, int maxDepth, QObject *parentNotifier)
        : m_task(task), m_queue(queue), m_visited(visited), m_visitedMutex(visitedMutex), m_activeWorkers(activeWorkers), m_maxDepth(maxDepth), m_notifier(parentNotifier) {
        setAutoDelete(true);
    }

    void run() override {
        QString connName = QString("conn_%1").arg((quintptr)QThread::currentThreadId());
        QSqlDatabase db = DbHelper::openConnection(connName);
        QSqlQuery insertQuery(db);
        insertQuery.prepare("INSERT OR REPLACE INTO pages (url, title, content, priority, fetched_at) VALUES (?, ?, ?, ?, ?)");

        QNetworkAccessManager nam;
        QNetworkRequest request(m_task.url);
        request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) QtCrawler/1.2");

        QEventLoop loop;
        QNetworkReply *reply = nam.get(request);

        QObject::connect(reply, &QNetworkReply::metaDataChanged, [reply]() {
            QVariant contentType = reply->header(QNetworkRequest::ContentTypeHeader);
            if (contentType.isValid() && !contentType.toString().toLower().contains("text/html")) {
                reply->abort();
            }
        });

        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        QTimer timer;
        timer.setSingleShot(true);
        QObject::connect(&timer, &QTimer::timeout, [&]() { if (reply->isRunning()) reply->abort(); });
        timer.start(8000); 
        loop.exec();

        if (reply->error() == QNetworkReply::NoError) {
            QString html = QString::fromUtf8(reply->readAll());
            
            insertQuery.addBindValue(m_task.url.toString());
            insertQuery.addBindValue(extractTitle(html));
            insertQuery.addBindValue(extractTextSnippet(html));
            insertQuery.addBindValue(m_task.priority);
            insertQuery.addBindValue(QDateTime::currentSecsSinceEpoch());
            insertQuery.exec();

            if (m_task.depth < m_maxDepth) {
                QRegularExpression re("href\\s*=\\s*['\"]([^'\"#>]+)['\"]", QRegularExpression::CaseInsensitiveOption);
                auto it = re.globalMatch(html);
                while (it.hasNext()) {
                    QUrl link = m_task.url.resolved(QUrl::fromUserInput(it.next().captured(1).trimmed()));
                    if (!link.isValid() || (link.scheme() != "http" && link.scheme() != "https")) continue;
                    
                    {
                        QMutexLocker lock(m_visitedMutex);
                        if (m_visited->contains(link)) continue;
                        m_visited->insert(link);
                    }

                    // RANKING LOGIC: .gov and .mil get priority 0, others get 10
                    QString host = link.host().toLower();
                    int p = (host.endsWith(".gov") || host.endsWith(".mil")) ? 0 : 10;
                    m_queue->push({link, p, m_task.depth + 1});
                }
            }
        }

        reply->deleteLater();
        db.close();
        QSqlDatabase::removeDatabase(connName);
        m_activeWorkers->fetchAndAddRelaxed(-1);
        
        QString logEntry = QString("[%1] %2").arg(m_task.priority == 0 ? "HIGH" : "LOW").arg(m_task.url.toString());
        QMetaObject::invokeMethod(m_notifier, "logUrl", Qt::QueuedConnection, Q_ARG(QString, logEntry));
    }

private:
    static QString extractTitle(const QString &html) {
        QRegularExpression re("<title[^>]*>(.*?)</title>", QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
        auto m = re.match(html);
        return m.hasMatch() ? m.captured(1).trimmed() : "Untitled";
    }
    static QString extractTextSnippet(const QString &html) {
        QString s = html;
        s.remove(QRegularExpression("<script[^>]*>.*?</script>", QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption));
        s.remove(QRegularExpression("<style[^>]*>.*?</style>", QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption));
        s.replace(QRegularExpression("<[^>]+>"), " ");
        return s.simplified().left(1000);
    }

    CrawlTask m_task;
    ThreadSafeQueue *m_queue;
    QSet<QUrl> *m_visited;
    QMutex *m_visitedMutex;
    QAtomicInt *m_activeWorkers;
    int m_maxDepth;
    QObject *m_notifier;
};

class CrawlerWindow : public QMainWindow {
    Q_OBJECT
public:
    CrawlerWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setupUI();
        initDatabase();
        m_activeWorkers.storeRelaxed(0);
        connect(m_startBtn, &QPushButton::clicked, this, &CrawlerWindow::startInitialCrawl);
        connect(&m_uiTimer, &QTimer::timeout, this, &CrawlerWindow::dispatchWork);
        m_uiTimer.start(150);
    }

public slots:
    void logUrl(const QString &logText) {
        m_log->appendPlainText(logText);
        updateProgress();
    }

    void startInitialCrawl() {
        QUrl url = QUrl::fromUserInput(m_urlEdit->text());
        if (!url.isValid()) return;
        {
            QMutexLocker lock(&m_visitedMutex);
            m_visited.clear();
            m_visited.insert(url);
        }
        m_queue.clear();
        m_log->clear();
        m_log->appendPlainText("--- Priority Crawl Started (.gov/.mil ranked highest) ---");
        
        QString host = url.host().toLower();
        int p = (host.endsWith(".gov") || host.endsWith(".mil")) ? 0 : 10;
        m_queue.push({url, p, 0});
    }

    void dispatchWork() {
        while (m_activeWorkers.loadRelaxed() < 12) {
            CrawlTask task;
            if (!m_queue.tryPop(task)) break;
            m_activeWorkers.fetchAndAddRelaxed(1);
            m_currentUrlLabel->setText(task.url.toString());
            m_webView->load(task.url);
            QThreadPool::globalInstance()->start(new FetchTask(task, &m_queue, &m_visited, &m_visitedMutex, &m_activeWorkers, 2, this));
        }
    }

private:
    void setupUI() {
        QWidget *central = new QWidget(this);
        QHBoxLayout *mainLayout = new QHBoxLayout(central);

        QWidget *leftWidget = new QWidget();
        QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
        m_webView = new QWebEngineView();
        m_webView->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, false);
        m_webView->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, false);
        
        m_currentUrlLabel = new QLineEdit();
        m_currentUrlLabel->setReadOnly(true);
        leftLayout->addWidget(new QLabel("Live Preview:"));
        leftLayout->addWidget(m_currentUrlLabel);
        leftLayout->addWidget(m_webView, 1);

        QWidget *rightWidget = new QWidget();
        QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
        m_urlEdit = new QLineEdit();
        m_startBtn = new QPushButton("Start High-Priority Crawl");
        m_progressBar = new QProgressBar();
        m_log = new QPlainTextEdit();
        m_log->setReadOnly(true);

        rightLayout->addWidget(new QLabel("Target URL:"));
        rightLayout->addWidget(m_urlEdit);
        rightLayout->addWidget(m_startBtn);
        rightLayout->addWidget(m_progressBar);
        rightLayout->addWidget(new QLabel("Ranked Stream ([HIGH] = .gov/.mil):"));
        rightLayout->addWidget(m_log, 1);

        mainLayout->addWidget(leftWidget, 3);
        mainLayout->addWidget(rightWidget, 1);
        setCentralWidget(central);
        resize(1200, 800);
        setWindowTitle("Ranked Crawler - High Value Targets: .gov & .mil");
    }

    void initDatabase() {
        QSqlDatabase db = DbHelper::openConnection("main_conn");
        db.close();
        QSqlDatabase::removeDatabase("main_conn");
    }

    void updateProgress() {
        QMutexLocker lock(&m_visitedMutex);
        int v = m_visited.size();
        int q = m_queue.size();
        m_progressBar->setMaximum(v + q);
        m_progressBar->setValue(v);
    }

private:
    QLineEdit *m_urlEdit, *m_currentUrlLabel;
    QPushButton *m_startBtn;
    QProgressBar *m_progressBar;
    QPlainTextEdit *m_log;
    QWebEngineView *m_webView;
    ThreadSafeQueue m_queue;
    QSet<QUrl> m_visited;
    QMutex m_visitedMutex;
    QAtomicInt m_activeWorkers;
    QTimer m_uiTimer;
};

#include "crawler.moc"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QThreadPool::globalInstance()->setMaxThreadCount(16);
    CrawlerWindow w;
    w.show();
    return a.exec();
}
