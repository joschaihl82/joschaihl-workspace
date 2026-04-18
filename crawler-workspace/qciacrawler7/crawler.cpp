#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QTextEdit>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QThreadPool>
#include <QRunnable>
#include <QCryptographicHash>
#include <QSplitter>
#include <QDateTime>
#include <QEventLoop>
#include <QTimer>
#include <QAtomicInt>

// --- HIGH SPEED CONFIG ---
namespace FastConfig {
    const int MAX_PAGES = 3000000;
    const int THREADS = 64; // Massive concurrency since we aren't rendering
    const int TIMEOUT = 8000; // Fast timeout for dead sites
    const char* DB_NAME = "ultra_fast_crawler.db";
}

// --- LIGHTWEIGHT WORKER (NO BROWSER ENGINE) ---
class FastWorkerRelay : public QObject {
    Q_OBJECT
signals:
    void rawDataReady(const QString &source, const QString &url);
    void finished();
};

class FastCrawlTask : public QRunnable {
public:
    FastCrawlTask(QString url, QAtomicInt *active, FastWorkerRelay *relay)
        : m_url(url), m_active(active), m_relay(relay) { setAutoDelete(true); }

    void run() override {
        QString connName = "fast_conn_" + QString::number((quintptr)QThread::currentThreadId());
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
            db.setDatabaseName(FastConfig::DB_NAME);
            if (!db.open()) return;

            QNetworkAccessManager manager;
            QNetworkRequest req((QUrl(m_url)));
            
            // Speed optimization: Request compressed data only
            req.setRawHeader("User-Agent", "FastCrawler/5.0 (No-Image; Headless)");
            req.setRawHeader("Accept-Encoding", "gzip, deflate");

            QEventLoop loop;
            QTimer timer; timer.setSingleShot(true);
            QNetworkReply *reply = manager.get(req);

            QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
            QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
            timer.start(FastConfig::TIMEOUT);
            loop.exec();

            if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {
                // Check Mime-Type: Only process text/html
                QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
                if (contentType.contains("text/html") || contentType.contains("text/plain")) {
                    QString html = QString::fromUtf8(reply->readAll());
                    
                    // Update UI with raw data (very fast)
                    emit m_relay->rawDataReady(html.left(10000), m_url); 

                    processLinks(html, db);
                }
            }
            updateStatus(db);
        }
        QSqlDatabase::removeDatabase(connName);
        m_active->fetchAndAddRelaxed(-1);
        emit m_relay->finished();
    }

private:
    void processLinks(const QString &html, QSqlDatabase &db) {
        db.transaction();
        QRegularExpression re("href=\"(http[s]?://[^\"]+)\"");
        auto it = re.globalMatch(html);
        int limit = 0;
        while (it.hasNext() && limit++ < 50) {
            QString link = it.next().captured(1);
            QSqlQuery q(db);
            q.prepare("INSERT OR IGNORE INTO queue (hash, url, status) VALUES (?, ?, 0)");
            q.addBindValue(QCryptographicHash::hash(link.toUtf8(), QCryptographicHash::Sha1));
            q.addBindValue(link);
            q.exec();
        }
        db.commit();
    }

    void updateStatus(QSqlDatabase &db) {
        QSqlQuery q(db);
        q.prepare("UPDATE queue SET status = 1 WHERE url = ?");
        q.addBindValue(m_url);
        q.exec();
    }

    QString m_url;
    QAtomicInt *m_active;
    FastWorkerRelay *m_relay;
};

// --- MAIN ENGINE ---
class UltraFastEngine : public QMainWindow {
    Q_OBJECT
public:
    UltraFastEngine() {
        m_relay = new FastWorkerRelay();
        connect(m_relay, &FastWorkerRelay::rawDataReady, this, &UltraFastEngine::showRawContent);
        connect(m_relay, &FastWorkerRelay::finished, this, &UltraFastEngine::dispatch);

        setupSpeedUI();
        initDB();
        QThreadPool::globalInstance()->setMaxThreadCount(FastConfig::THREADS);
    }

public slots:
    void showRawContent(const QString &source, const QString &url) {
        // Fast update: Text only, no rendering engine
        m_sourceView->setPlainText(source);
        m_currentUrlLabel->setText("Source: " + url);
    }

    void start() {
        QString seed = m_input->text();
        QSqlDatabase db = QSqlDatabase::database("fast_service");
        QSqlQuery q(db);
        q.prepare("INSERT OR IGNORE INTO queue (hash, url, status) VALUES (?, ?, 0)");
        q.addBindValue(QCryptographicHash::hash(seed.toUtf8(), QCryptographicHash::Sha1));
        q.addBindValue(seed);
        q.exec();
        dispatch();
    }

    void dispatch() {
        if (m_running.loadRelaxed() >= FastConfig::THREADS) return;

        QSqlDatabase db = QSqlDatabase::database("fast_service");
        QSqlQuery q(db);
        q.prepare("SELECT url FROM queue WHERE status = 0 LIMIT ?");
        q.addBindValue(FastConfig::THREADS - m_running.loadRelaxed());

        if (q.exec()) {
            while (q.next()) {
                QString url = q.value(0).toString();
                m_running.fetchAndAddRelaxed(1);
                QThreadPool::globalInstance()->start(new FastCrawlTask(url, &m_running, m_relay));
            }
        }
        updateStats();
    }

private:
    void setupSpeedUI() {
        QWidget *central = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout(central);
        QSplitter *split = new QSplitter(Qt::Horizontal);

        // LEFT: RAW SOURCE (FAST)
        QWidget *left = new QWidget();
        QVBoxLayout *lBox = new QVBoxLayout(left);
        m_currentUrlLabel = new QLabel("Ready...");
        m_sourceView = new QTextEdit();
        m_sourceView->setReadOnly(true);
        m_sourceView->setFontFamily("Courier");
        m_sourceView->setStyleSheet("background: #0c0c0c; color: #00ff41;");
        lBox->addWidget(m_currentUrlLabel);
        lBox->addWidget(m_sourceView);

        // RIGHT: CONTROLS
        QWidget *right = new QWidget();
        right->setMaximumWidth(400);
        QVBoxLayout *rBox = new QVBoxLayout(right);
        m_input = new QLineEdit("https://www.wikipedia.org");
        m_btn = new QPushButton("START TURBO CRAWL");
        m_btn->setStyleSheet("background: #c0392b; color: white; font-weight: bold; height: 40px;");
        m_stats = new QLabel("Stats: 0 visited");
        
        rBox->addWidget(new QLabel("Seed URL:"));
        rBox->addWidget(m_input);
        rBox->addWidget(m_btn);
        rBox->addWidget(m_stats);
        rBox->addStretch();

        split->addWidget(left);
        split->addWidget(right);
        split->setStretchFactor(0, 3);

        layout->addWidget(split);
        setCentralWidget(central);
        resize(1200, 800);
        setWindowTitle("UltraFast Headless Crawler 5.0 - [No Images / No Video]");

        connect(m_btn, &QPushButton::clicked, this, &UltraFastEngine::start);
    }

    void initDB() {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "fast_service");
        db.setDatabaseName(FastConfig::DB_NAME);
        db.open();
        QSqlQuery q(db);
        q.exec("PRAGMA journal_mode = WAL;");
        q.exec("CREATE TABLE IF NOT EXISTS queue (hash BLOB PRIMARY KEY, url TEXT, status INTEGER)");
    }

    void updateStats() {
        QSqlQuery q(QSqlDatabase::database("fast_service"));
        q.exec("SELECT COUNT(*) FROM queue WHERE status = 1"); q.next();
        m_stats->setText(QString("Visited: %1 | Active: %2").arg(q.value(0).toInt()).arg(m_running.loadRelaxed()));
    }

    QTextEdit *m_sourceView;
    QLabel *m_currentUrlLabel;
    QLineEdit *m_input;
    QPushButton *m_btn;
    QLabel *m_stats;
    QAtomicInt m_running = 0;
    FastWorkerRelay *m_relay;
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    UltraFastEngine w;
    w.show();
    return a.exec();
}

#include "crawler.moc"
