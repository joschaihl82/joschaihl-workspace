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
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QRegularExpression>
#include <QDateTime>
#include <QTimer>
#include <QAtomicInt>
#include <QThreadPool>
#include <QRunnable>
#include <QMutex>
#include <QEventLoop>
#include <QFont>
#include <QListWidgetItem>
#include <QSplitter>
#include <QSizePolicy>

// Maximale Seiten (3x erhöht)
const int MAX_PAGES = 3000000;

// Prioritätsberechnung: kleinere Werte = höhere Priorität
static int computePriority(const QUrl &u) {
    QString host = u.host().toLower();
    if (host.endsWith(".gov")) return -100; // starker Boost
    if (host.endsWith(".mil")) return -100; // starker Boost
    if (host.endsWith(".us"))  return 1;    // hohe Priorität, aber hinter gov/mil
    return 10; // Standard
}

// --- Datenbank & Queue Logik ---
class CrawlerDB {
public:
    static bool init() {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "queue_connection");
        db.setDatabaseName("crawler_huge.db");
        if (!db.open()) return false;
        QSqlQuery q(db);
        q.exec("PRAGMA journal_mode = WAL;");
        q.exec("PRAGMA synchronous = OFF;");
        // Status: 0 = Pending, 1 = Visited, 2 = Error, 3 = InProgress
        q.exec("CREATE TABLE IF NOT EXISTS queue (url TEXT PRIMARY KEY, status INTEGER, priority INTEGER, depth INTEGER)");
        q.exec("CREATE TABLE IF NOT EXISTS data (url TEXT PRIMARY KEY, title TEXT, content TEXT)");
        // Kontakte-Tabelle: contact + type ('email'/'phone') + source_url
        q.exec("CREATE TABLE IF NOT EXISTS contacts (contact TEXT PRIMARY KEY, type TEXT, source_url TEXT)");
        q.exec("CREATE INDEX IF NOT EXISTS idx_status ON queue(status, priority)");
        return true;
    }

    // priority = -1 -> computePriority wird verwendet
    static void addUrl(const QUrl &url, int depth, int priority = -1) {
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

    static void addContact(const QString &contact, const QString &type, const QString &sourceUrl) {
        QSqlDatabase db = QSqlDatabase::database("queue_connection");
        QSqlQuery q(db);
        q.prepare("INSERT OR IGNORE INTO contacts (contact, type, source_url) VALUES (?, ?, ?)");
        q.addBindValue(contact);
        q.addBindValue(type);
        q.addBindValue(sourceUrl);
        q.exec();
    }
};

// --- Worker ---
class BigFetchTask : public QRunnable {
public:
    BigFetchTask(const QString &url, int depth, QAtomicInt *active, QObject *notifier)
        : m_url(url), m_depth(depth), m_active(active), m_notifier(notifier) { setAutoDelete(true); }

    void run() override {
        QString conn = QString("thread_%1").arg((quintptr)QThread::currentThreadId());
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", conn);
            db.setDatabaseName("crawler_huge.db");
            if (!db.open()) return;

            QNetworkAccessManager nam;
            QEventLoop loop;
            QNetworkReply *reply = nam.get(QNetworkRequest(QUrl(m_url)));
            QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

            QTimer timer; timer.setSingleShot(true);
            QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
            timer.start(15000);
            loop.exec();

            if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {
                QString html = QString::fromUtf8(reply->readAll());
                QSqlQuery q(db);
                q.prepare("INSERT OR REPLACE INTO data (url, title, content) VALUES (?, ?, ?)");
                q.addBindValue(m_url);
                q.addBindValue(html.section("<title>", 1, 1).section("</title>", 0, 0).left(255 * 3));
                q.addBindValue(html.left(2000 * 3));
                q.exec();

                q.prepare("UPDATE queue SET status = 1 WHERE url = ?");
                q.addBindValue(m_url);
                q.exec();

                // E-Mail-Adressen extrahieren
                QRegularExpression emailRe(R"(([\w\.\-+%]+@[\w\.\-]+\.[A-Za-z]{2,}))", QRegularExpression::CaseInsensitiveOption);
                auto emailIt = emailRe.globalMatch(html);
                while (emailIt.hasNext()) {
                    QString email = emailIt.next().captured(1).trimmed();
                    if (!email.isEmpty()) {
                        QString norm = email.toLower();
                        CrawlerDB::addContact(norm, "email", m_url);
                    }
                }

                // Telefonnummern extrahieren (einfache Heuristik)
                QRegularExpression phoneRe(R"((\+?\d[\d\-\s().]{5,}\d))", QRegularExpression::CaseInsensitiveOption);
                auto phoneIt = phoneRe.globalMatch(html);
                while (phoneIt.hasNext()) {
                    QString raw = phoneIt.next().captured(1).trimmed();
                    QString cleaned;
                    for (QChar c : raw) {
                        if (c.isDigit() || c == QChar('+')) cleaned.append(c);
                    }
                    QString digitsOnly;
                    for (QChar c : cleaned) if (c.isDigit()) digitsOnly.append(c);
                    if (digitsOnly.length() >= 7) {
                        QString tel = cleaned;
                        CrawlerDB::addContact(tel, "phone", m_url);
                    }
                }

                // Links extrahieren und priorisiert einfügen
                if (m_depth < 5) {
                    QRegularExpression re("href\\s*=\\s*['\"]([^'\"#>]+)['\"]", QRegularExpression::CaseInsensitiveOption);
                    auto it = re.globalMatch(html);
                    db.transaction();
                    while (it.hasNext()) {
                        QString href = it.next().captured(1);
                        QUrl u = QUrl(m_url).resolved(QUrl::fromUserInput(href));
                        if (u.isValid() && u.scheme().startsWith("http")) {
                            int prio = computePriority(u);
                            QSqlQuery ins(db);
                            ins.prepare("INSERT OR IGNORE INTO queue (url, status, priority, depth) VALUES (?, 0, ?, ?)");
                            ins.addBindValue(u.toString());
                            ins.addBindValue(prio);
                            ins.addBindValue(m_depth + 1);
                            ins.exec();
                        }
                    }
                    db.commit();
                }
            } else {
                QSqlQuery q(db);
                q.prepare("UPDATE queue SET status = 2 WHERE url = ?");
                q.addBindValue(m_url);
                q.exec();
            }
        }
        QSqlDatabase::removeDatabase(conn);
        m_active->fetchAndAddRelaxed(-1);
        QMetaObject::invokeMethod(m_notifier, "dispatch", Qt::QueuedConnection);
    }

private:
    QString m_url;
    int m_depth;
    QAtomicInt *m_active;
    QObject *m_notifier;
};

// --- Hauptfenster ---
class MegaCrawler : public QMainWindow {
    Q_OBJECT
public:
    MegaCrawler() : m_activeWorkers(0) {
        setupUI();
        CrawlerDB::init();
        m_maxWorkers = QThread::idealThreadCount() * 12;
        connect(m_startBtn, &QPushButton::clicked, this, &MegaCrawler::start);
    }

public slots:
    void start() {
        QUrl startUrl = QUrl::fromUserInput(m_urlEdit->text());
        CrawlerDB::addUrl(startUrl, 0, -1);
        dispatch();
    }

    void dispatch() {
        QSqlDatabase db = QSqlDatabase::database("queue_connection");
        QSqlQuery q(db);
        q.prepare("SELECT url, depth FROM queue WHERE status = 0 ORDER BY priority ASC, depth ASC LIMIT ?");
        q.addBindValue(m_maxWorkers);
        q.exec();

        while (q.next() && m_activeWorkers.loadRelaxed() < m_maxWorkers) {
            QString url = q.value(0).toString();
            int depth = q.value(1).toInt();

            QSqlQuery update(db);
            update.prepare("UPDATE queue SET status = 3 WHERE url = ?");
            update.addBindValue(url);
            update.exec();

            m_activeWorkers.fetchAndAddRelaxed(1);

            m_webView->load(QUrl(url));

            if (m_urlList->count() > 300) delete m_urlList->takeItem(0);
            m_urlList->addItem(url);

            QThreadPool::globalInstance()->start(new BigFetchTask(url, depth, &m_activeWorkers, this));
        }
        updateStats();
        refreshContacts();
    }

private:
    void setupUI() {
        QWidget *c = new QWidget();
        QHBoxLayout *mainLayout = new QHBoxLayout(c);

        // Splitter zwischen WebView und rechter Sidebar
        QSplitter *splitter = new QSplitter(Qt::Horizontal, c);

        // WebView (linker Bereich)
        m_webView = new QWebEngineView();
        m_webView->setZoomFactor(0.75);
        m_webView->page()->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, false);

        QWidget *leftContainer = new QWidget();
        QVBoxLayout *leftLayout = new QVBoxLayout(leftContainer);
        leftLayout->addWidget(m_webView);
        leftLayout->setContentsMargins(0,0,0,0);

        splitter->addWidget(leftContainer);

        // Rechte Sidebar
        QWidget *r = new QWidget();
        QVBoxLayout *rl = new QVBoxLayout(r);
        rl->setContentsMargins(8,8,8,8);
        rl->setSpacing(8);

        // Rechts: Schrift überall auf 12 Punkt setzen
        QFont rightFont;
        rightFont.setPointSize(12);

        // URL / Start
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

        // URL-Liste (Übersicht)
        m_urlList = new QListWidget();
        m_urlList->setFont(rightFont);
        m_urlList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_urlList->setMaximumHeight(200);

        // E-Mail- und Telefon-Listen
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

        // Sidebar zusammenbauen
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

        // Setze vernünftige Stretch-Faktoren: WebView größer, Sidebar schmaler
        splitter->setStretchFactor(0, 3);
        splitter->setStretchFactor(1, 1);

        // Begrenze maximale Breite der Sidebar, damit Fenster nicht zu breit wirkt
        r->setMaximumWidth(700);

        mainLayout->addWidget(splitter);
        setCentralWidget(c);

        // Angemessene Standardfenstergröße
        resize(1280, 800);
        setWindowTitle("MegaCrawler - Fixed Layout (12pt right)");
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

    // Kontakte aus DB lesen und als klickbare Links in die Listen einfügen
    void refreshContacts() {
        QSqlDatabase db = QSqlDatabase::database("queue_connection");
        QSqlQuery q(db);

        // E-Mails
        m_emailsList->clear();
        q.prepare("SELECT contact FROM contacts WHERE type = 'email' ORDER BY contact ASC");
        q.exec();
        while (q.next()) {
            QString email = q.value(0).toString();
            QLabel *lbl = new QLabel();
            lbl->setTextFormat(Qt::RichText);
            lbl->setTextInteractionFlags(Qt::TextBrowserInteraction);
            lbl->setOpenExternalLinks(true);
            QString safe = email.toHtmlEscaped();
            lbl->setText(QString("<a href=\"mailto:%1\">%1</a>").arg(safe));
            QListWidgetItem *item = new QListWidgetItem(m_emailsList);
            item->setSizeHint(lbl->sizeHint());
            m_emailsList->addItem(item);
            m_emailsList->setItemWidget(item, lbl);
        }

        // Telefonnummern
        m_phonesList->clear();
        q.prepare("SELECT contact FROM contacts WHERE type = 'phone' ORDER BY contact ASC");
        q.exec();
        while (q.next()) {
            QString phone = q.value(0).toString();
            QString telUri;
            for (QChar c : phone) {
                if (c.isDigit() || c == QChar('+')) telUri.append(c);
            }
            if (telUri.isEmpty()) continue;
            QLabel *lbl = new QLabel();
            lbl->setTextFormat(Qt::RichText);
            lbl->setTextInteractionFlags(Qt::TextBrowserInteraction);
            lbl->setOpenExternalLinks(true);
            QString display = phone.toHtmlEscaped();
            lbl->setText(QString("<a href=\"tel:%1\">%2</a>").arg(telUri, display));
            QListWidgetItem *item = new QListWidgetItem(m_phonesList);
            item->setSizeHint(lbl->sizeHint());
            m_phonesList->addItem(item);
            m_phonesList->setItemWidget(item, lbl);
        }
    }

    QWebEngineView *m_webView;
    QLineEdit *m_urlEdit;
    QPushButton *m_startBtn;
    QProgressBar *m_progress;
    QListWidget *m_urlList;
    QLabel *m_statLabel;
    QListWidget *m_emailsList;
    QListWidget *m_phonesList;
    QAtomicInt m_activeWorkers;
    int m_maxWorkers;
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    MegaCrawler w;
    w.show();
    return a.exec();
}

#include "crawler.moc"

