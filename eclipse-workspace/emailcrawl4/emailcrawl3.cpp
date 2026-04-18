// emailcrawl3.cpp
#include <QApplication>
#include <QObject>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QRegularExpression>
#include <QTimer>
#include <QDebug>
#include <QSet>
#include <QQueue>
#include <QHash>
#include <QUrl>
#include <QElapsedTimer>

class Crawler : public QObject {
    Q_OBJECT

public:
    struct Item { QString url; int depth; };

    explicit Crawler(const QUrl &start, int maxDepth, int maxPages, bool followExternal, int politenessMs, QObject *parent = nullptr)
        : QObject(parent),
          maxDepth(maxDepth), maxPages(maxPages),
          followExternal(followExternal), politenessMs(politenessMs)
    {
        page = new QWebEnginePage(QWebEngineProfile::defaultProfile(), this);
        page->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, false);
        page->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);

        connect(page, &QWebEnginePage::loadFinished, this, &Crawler::onLoadFinished);

        startHost = start.host();
        enqueueResolved(start, 0);
        QTimer::singleShot(0, this, &Crawler::processNext);
    }

signals:
    void finished();

private slots:
    void onLoadFinished(bool ok) {
        requestTimer.invalidate();
        if (ok) {
            page->toHtml([this](const QString &html) {
                extractEmails(html);
                extractLinks(html);
                pagesVisitedCount++;
                qDebug() << "[PROGRESS] visited" << pagesVisitedCount << "/" << maxPages << "queue" << queue.size();

                if (pagesVisitedCount >= maxPages) {
                    finish();
                    return;
                }

                QTimer::singleShot(politenessMs, this, &Crawler::processNext);
            });
        } else {
            QTimer::singleShot(politenessMs, this, &Crawler::processNext);
        }
    }

private:
    void processNext() {
        if (queue.isEmpty()) { finish(); return; }
        Item it = queue.dequeue();

        if (visitedUrls.contains(it.url)) {
            QTimer::singleShot(0, this, &Crawler::processNext);
            return;
        }

        if (it.depth > maxDepth) {
            QTimer::singleShot(0, this, &Crawler::processNext);
            return;
        }

        visitedUrls.insert(it.url);
        visitedDepths.insert(it.url, it.depth);
        currentBase = QUrl(it.url);
        qDebug() << "[CRAWL]" << it.url << "depth" << it.depth;

        QUrl req = QUrl::fromUserInput(it.url);
        if (req.scheme().isEmpty()) req.setScheme("https");

        requestTimer.restart();
        page->load(req);
    }

    void enqueueResolved(const QUrl &base, int depth) {
        QString s = base.toString(QUrl::RemoveFragment);
        if (s.isEmpty()) return;
        if (!visitedUrls.contains(s) && !enqueuedUrls.contains(s)) {
            enqueuedUrls.insert(s);
            queue.enqueue({s, depth});
            visitedDepths.insert(s, depth);
        }
    }

    void extractEmails(const QString &html) {
        QRegularExpression rx(R"(([\w\.-]+@[\w\.-]+\.[A-Za-z]{2,}))");
        QRegularExpressionMatchIterator it = rx.globalMatch(html);
        while (it.hasNext()) {
            QRegularExpressionMatch m = it.next();
            QString e = m.captured(0).trimmed();
            if (!e.isEmpty() && !foundEmails.contains(e)) {
                foundEmails.insert(e);
                qDebug() << "[EMAIL]" << e;
            }
        }
        QRegularExpression mailtoRx(R"((?i)mailto:([^\s"'<>]+))");
        it = mailtoRx.globalMatch(html);
        while (it.hasNext()) {
            QString e = it.next().captured(1).trimmed();
            if (!e.isEmpty() && !foundEmails.contains(e)) {
                foundEmails.insert(e);
                qDebug() << "[EMAIL]" << e;
            }
        }
    }

    void extractLinks(const QString &html) {
        QRegularExpression hrefRx(R"((?i)\bhref\s*=\s*(?:\"([^\"]*)\"|'([^']*)'|([^\s>]+)))");
        QRegularExpressionMatchIterator it = hrefRx.globalMatch(html);

        while (it.hasNext()) {
            QRegularExpressionMatch m = it.next();
            QString raw;
            if (!m.captured(1).isEmpty()) raw = m.captured(1);
            else if (!m.captured(2).isEmpty()) raw = m.captured(2);
            else raw = m.captured(3);

            raw = raw.trimmed();
            if (raw.isEmpty()) continue;
            if (raw.startsWith("javascript:", Qt::CaseInsensitive)) continue;
            if (raw.startsWith("#")) continue;

            QUrl resolved = currentBase.resolved(QUrl(raw));
            if (!resolved.isValid()) continue;
            resolved.setFragment(QString());

            if (resolved.scheme().toLower() == "mailto") {
                QString mail = resolved.path();
                if (!mail.isEmpty() && !foundEmails.contains(mail)) {
                    foundEmails.insert(mail);
                    qDebug() << "[EMAIL]" << mail;
                }
                continue;
            }

            if (!(resolved.scheme() == "http" || resolved.scheme() == "https")) continue;

            QString normalized = resolved.toString(QUrl::RemoveUserInfo | QUrl::StripTrailingSlash);
            if (visitedUrls.contains(normalized) || enqueuedUrls.contains(normalized)) continue;

            if (!followExternal) {
                if (!sameHost(normalized, startHost)) continue;
            }

            int parentDepth = currentDepthForUrl(currentBase.toString());
            int nextDepth = parentDepth + 1;
            if (nextDepth <= maxDepth) {
                enqueuedUrls.insert(normalized);
                queue.enqueue({normalized, nextDepth});
                visitedDepths.insert(normalized, nextDepth);
            }
        }
    }

    bool sameHost(const QString &urlStr, const QString &host) const {
        QUrl u(urlStr);
        return !host.isEmpty() && u.host().endsWith(host, Qt::CaseInsensitive);
    }

    int currentDepthForUrl(const QString &u) const {
        return visitedDepths.value(u, 0);
    }

    void finish() {
        qDebug() << "[DONE] pages visited" << pagesVisitedCount << "found emails" << foundEmails.size();
        qDebug() << "=== Emails ===";
        for (const QString &e : foundEmails) qDebug() << e;
        emit finished();
        QCoreApplication::quit();
    }

    QWebEnginePage *page = nullptr;
    QQueue<Item> queue;
    QSet<QString> visitedUrls;
    QSet<QString> enqueuedUrls;
    QSet<QString> foundEmails;
    QHash<QString,int> visitedDepths;
    QUrl currentBase;
    int pagesVisitedCount = 0;
    int maxDepth;
    int maxPages;
    bool followExternal;
    int politenessMs;
    QElapsedTimer requestTimer;
    QString startHost;
};

int main(int argc, char **argv) {
    if (argc < 2) {
        qWarning() << "Usage: emailcrawl_recursive <start-url> [maxDepth] [maxPages] [followExternal 0|1] [politenessMs]";
        return 1;
    }

    QApplication app(argc, argv);

    QString input = QString::fromUtf8(argv[1]).trimmed();
    QUrl start = QUrl::fromUserInput(input);
    if (start.scheme().isEmpty()) start.setScheme("https");
    if (!start.isValid()) { qWarning() << "Invalid start URL"; return 1; }

    int maxDepth = (argc > 2) ? QString(argv[2]).toInt() : 2;
    int maxPages = (argc > 3) ? QString(argv[3]).toInt() : 200;
    bool followExternal = (argc > 4) ? (QString(argv[4]) != "0") : true;
    int politenessMs = (argc > 5) ? QString(argv[5]).toInt() : 500;

    QWebEngineProfile::defaultProfile()->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
    QWebEngineProfile::defaultProfile()->setHttpCacheType(QWebEngineProfile::MemoryHttpCache);

    Crawler *c = new Crawler(start, maxDepth, maxPages, followExternal, politenessMs);
    QObject::connect(c, &Crawler::finished, &app, &QCoreApplication::quit);
    return app.exec();
}

#include "emailcrawl3.moc"
