#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUrl>
#include <QSet>
#include <QDebug>

struct CrawlTask {
    QUrl url;
    int priority;
    int depth;
};

class CrawlerWindow : public QMainWindow {
    Q_OBJECT

public:
    CrawlerWindow(QWidget *parent = nullptr) : QMainWindow(parent), m_isLoading(false) {
        setupUI();
        initDatabase();
        
        // Configure Browser Settings
        m_view->page()->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, false);
        m_view->page()->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);

        connect(m_view, &QWebEngineView::loadFinished, this, &CrawlerWindow::handleLoadFinished);
        connect(m_urlEdit, &QLineEdit::returnPressed, this, &CrawlerWindow::startInitialCrawl);
    }

private slots:
    void startInitialCrawl() {
        QUrl url = QUrl::fromUserInput(m_urlEdit->text());
        if (url.isValid()) {
            m_visited.clear();
            m_queue.clear();
            m_progressBar->setValue(0);
            addUrl(url, 0);
        }
    }

    void handleLoadFinished(bool success) {
        if (!success) {
            m_isLoading = false;
            processNext();
            return;
        }

        // Extract and Save
        m_view->page()->runJavaScript(
            R"(
                (function() {
                    return {
                        title: document.title,
                        text: document.body.innerText.substring(0, 5000),
                        links: Array.from(document.querySelectorAll('a[href]')).map(a => a.href)
                    };
                })()
            )",
            [this](const QVariant &result) {
                QVariantMap data = result.toMap();
                saveToDatabase(m_currentTask.url.toString(), data["title"].toString(), data["text"].toString());

                QStringList links = data["links"].toStringList();
                for (const QString &link : links) {
                    addUrl(QUrl(link), m_currentTask.depth + 1);
                }

                m_isLoading = false;
                processNext();
            }
        );
    }

private:
    void setupUI() {
        QWidget *central = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(central);

        // URL Bar Area
        QHBoxLayout *topLayout = new QHBoxLayout();
        m_urlEdit = new QLineEdit();
        m_urlEdit->setPlaceholderText("Enter URL to start crawling (e.g. https://usa.gov)...");
        QPushButton *btn = new QPushButton("Start Crawl");
        topLayout->addWidget(m_urlEdit);
        topLayout->addWidget(btn);
        connect(btn, &QPushButton::clicked, this, &CrawlerWindow::startInitialCrawl);

        // Browser View
        m_view = new QWebEngineView();
        
        // Progress Bar
        m_progressBar = new QProgressBar();
        m_progressBar->setFixedHeight(25);
        m_progressBar->setFormat("Visited: %v | Queue: %m");

        layout->addLayout(topLayout);
        layout->addWidget(m_view);
        layout->addWidget(m_progressBar);

        setCentralWidget(central);
        resize(1024, 768);
        setWindowTitle("Qt6 Priority Web Crawler (.gov / .mil / .us)");
    }

    void addUrl(const QUrl &url, int depth) {
        if (!url.isValid() || m_visited.contains(url) || depth > 2) return;
        
        int priority = (url.host().endsWith(".gov") || url.host().endsWith(".mil") || url.host().endsWith(".us")) ? 0 : 10;
        m_queue.append({url, priority, depth});

        // Sort by Priority
        std::sort(m_queue.begin(), m_queue.end(), [](const CrawlTask &a, const CrawlTask &b){
            return a.priority < b.priority;
        });

        // Update Progress Bar Range
        m_progressBar->setMaximum(m_visited.size() + m_queue.size());
        
        if (!m_isLoading) processNext();
    }

    void processNext() {
        if (m_queue.isEmpty()) return;

        m_isLoading = true;
        m_currentTask = m_queue.takeFirst();
        m_visited.insert(m_currentTask.url);
        
        m_progressBar->setValue(m_visited.size());
        m_urlEdit->setText(m_currentTask.url.toString()); // Show current URL
        m_view->load(m_currentTask.url);
    }

    void initDatabase() {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("crawler_results.db");
        if (db.open()) {
            QSqlQuery q;
            q.exec("CREATE TABLE IF NOT EXISTS pages (url TEXT PRIMARY KEY, title TEXT, content TEXT)");
        }
    }

    void saveToDatabase(const QString &url, const QString &title, const QString &content) {
        QSqlQuery q;
        q.prepare("INSERT OR REPLACE INTO pages (url, title, content) VALUES (?, ?, ?)");
        q.addBindValue(url); q.addBindValue(title); q.addBindValue(content);
        q.exec();
    }

    QLineEdit *m_urlEdit;
    QWebEngineView *m_view;
    QProgressBar *m_progressBar;
    QList<CrawlTask> m_queue;
    QSet<QUrl> m_visited;
    CrawlTask m_currentTask;
    bool m_isLoading;
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    CrawlerWindow w;
    w.show();
    return a.exec();
}

#include "crawler.moc"
