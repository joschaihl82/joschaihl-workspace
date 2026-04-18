// jbrowser.cpp
#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QtWebEngineCore/qwebengineurlrequestinfo.h>
#include <QtWebEngineCore/qwebengineurlrequestinterceptor.h>
#include <QToolBar>
#include <QLineEdit>
#include <QAction>
#include <QProgressBar>
#include <QListWidget>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardPaths>
#include <QNetworkProxy>
#include <QDialog>
#include <QFormLayout>
#include <QSpinBox>
#include <QLabel>
#include <QElapsedTimer>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFileInfo>
#include <QFileDialog>
#include <QPushButton>
#include <QUrlQuery>
#include <QDateTime>

// Utility: common downloadable extensions
static const QStringList downloadExtensions = {
    "zip","exe","msi","dmg","tar","gz","tgz","rar","7z","pdf","doc","docx","xls","xlsx","ppt","pptx","iso","apk","bin","csv","mp3","mp4","mkv","avi"
};

// Heuristic: check if URL looks like a downloadable resource
static bool looksLikeDownload(const QUrl &url) {
    QString path = url.path().toLower();
    for (const QString &ext : downloadExtensions) {
        if (path.endsWith("." + ext)) return true;
    }
    // query-based filenames: ?file=... or download=...
    QUrlQuery q(url);
    if (q.hasQueryItem("download") || q.hasQueryItem("file") || q.hasQueryItem("attachment")) return true;
    return false;
}

// Custom WebEnginePage to intercept navigation requests
class InterceptingPage : public QWebEnginePage {
    Q_OBJECT
public:
    InterceptingPage(QWebEngineProfile *profile, QObject *parent = nullptr)
        : QWebEnginePage(profile, parent) {}

signals:
    void downloadRequestedByPage(const QUrl &url);

protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override {
        // If the URL looks like a download or is not an HTML resource, treat it as a download
        // (This is heuristic; some sites may require different handling.)
        if (looksLikeDownload(url)) {
            emit downloadRequestedByPage(url);
            return false; // prevent navigation
        }
        // allow normal navigation
        return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
    }
};

// DownloadTask: performs a download using QNetworkAccessManager and reports progress
class DownloadTask : public QObject {
    Q_OBJECT
public:
    DownloadTask(const QUrl &url, const QString &path, QNetworkAccessManager *mgr, QObject *parent = nullptr)
        : QObject(parent), m_url(url), m_path(path), m_mgr(mgr), m_reply(nullptr) {
        m_file.setFileName(m_path);
        if (!m_file.open(QIODevice::WriteOnly)) {
            emit errorOccurred(QStringLiteral("Cannot open file for writing: %1").arg(m_path));
            return;
        }
        start();
    }

    ~DownloadTask() {
        if (m_reply) m_reply->deleteLater();
        if (m_file.isOpen()) m_file.close();
    }

    QString filePath() const { return m_path; }
    QUrl url() const { return m_url; }

signals:
    void progress(qint64 received, qint64 total);
    void finished();
    void errorOccurred(const QString &msg);

private slots:
    void onReadyRead() {
        if (!m_reply) return;
        QByteArray chunk = m_reply->readAll();
        m_file.write(chunk);
    }

    void onDownloadProgress(qint64 received, qint64 total) {
        emit progress(received, total);
    }

    void onFinishedInternal() {
        if (!m_reply) return;
        QVariant statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if (m_reply->error() != QNetworkReply::NoError) {
            QString err = m_reply->errorString();
            m_file.close();
            m_file.remove();
            emit errorOccurred(err);
        } else {
            m_file.flush();
            m_file.close();
            emit finished();
        }
        m_reply->deleteLater();
        m_reply = nullptr;
    }

    void onSslErrors(const QList<QSslError> &errors) {
        Q_UNUSED(errors);
        // For simplicity, ignore SSL errors (not recommended for production)
        if (m_reply) m_reply->ignoreSslErrors();
    }

private:
    void start() {
        QNetworkRequest req(m_url);
        // set a reasonable user-agent
        req.setHeader(QNetworkRequest::UserAgentHeader,
                      QStringLiteral("JBrowser/1.0 (QtWebEngine)"));
        m_reply = m_mgr->get(req);
        connect(m_reply, &QNetworkReply::readyRead, this, &DownloadTask::onReadyRead);
        connect(m_reply, &QNetworkReply::downloadProgress, this, &DownloadTask::onDownloadProgress);
        connect(m_reply, &QNetworkReply::finished, this, &DownloadTask::onFinishedInternal);
        connect(m_reply, &QNetworkReply::sslErrors, this, &DownloadTask::onSslErrors);
        m_startTime.start();
    }

    QUrl m_url;
    QString m_path;
    QNetworkAccessManager *m_mgr;
    QNetworkReply *m_reply;
    QFile m_file;
    QElapsedTimer m_startTime;
};

// UI row for a download: progress bar, elapsed, ETA, cancel button
class DownloadRow : public QWidget {
    Q_OBJECT
public:
    DownloadRow(DownloadTask *task, QWidget *parent = nullptr) : QWidget(parent), m_task(task) {
        auto *h = new QHBoxLayout(this);
        m_label = new QLabel(QFileInfo(task->filePath()).fileName());
        m_progress = new QProgressBar; m_progress->setRange(0,100);
        m_elapsed = new QLabel("00:00:00");
        m_eta = new QLabel("--:--:--");
        m_cancel = new QPushButton("Cancel");
        h->addWidget(m_label, 2);
        h->addWidget(m_progress, 3);
        h->addWidget(m_elapsed);
        h->addWidget(m_eta);
        h->addWidget(m_cancel);

        connect(m_task, &DownloadTask::progress, this, &DownloadRow::onProgress);
        connect(m_task, &DownloadTask::finished, this, &DownloadRow::onFinished);
        connect(m_task, &DownloadTask::errorOccurred, this, &DownloadRow::onError);
        connect(m_cancel, &QPushButton::clicked, this, &DownloadRow::onCancel);

        m_timer.start();
    }

signals:
    void requestCancel();

private slots:
    void onProgress(qint64 received, qint64 total) {
        if (total > 0) {
            int p = int((received * 100) / total);
            m_progress->setValue(p);
            qint64 elapsedMs = m_timer.elapsed();
            QString elapsedStr = QTime(0,0).addMSecs(elapsedMs).toString("hh:mm:ss");
            m_elapsed->setText(elapsedStr);
            double speed = (elapsedMs > 0) ? (received / (elapsedMs / 1000.0)) : 0.0;
            if (speed > 0.0) {
                qint64 remain = qRound((total - received) / speed);
                m_eta->setText(QTime(0,0).addSecs(remain).toString("hh:mm:ss"));
            } else {
                m_eta->setText("--:--:--");
            }
        } else {
            m_progress->setValue(0);
        }
    }

    void onFinished() {
        m_progress->setValue(100);
        m_cancel->setEnabled(false);
        m_eta->setText("00:00:00");
    }

    void onError(const QString &msg) {
        m_label->setText(QStringLiteral("%1 (error)").arg(m_label->text()));
        m_cancel->setEnabled(false);
        Q_UNUSED(msg);
    }

    void onCancel() {
        emit requestCancel();
        m_cancel->setEnabled(false);
    }

private:
    DownloadTask *m_task;
    QLabel *m_label;
    QProgressBar *m_progress;
    QLabel *m_elapsed;
    QLabel *m_eta;
    QPushButton *m_cancel;
    QElapsedTimer m_timer;
};

// Main browser window
class BrowserWindow : public QMainWindow {
    Q_OBJECT
public:
    BrowserWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        // Optional: set a global proxy before creating WebEngine objects
        // QNetworkProxy proxy(QNetworkProxy::HttpProxy, "127.0.0.1", 8080);
        // QNetworkProxy::setApplicationProxy(proxy);

        // Profile for cache and persistence
        QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(base);
        m_profile = new QWebEngineProfile("JBrowserProfile", this);
        m_profile->setCachePath(base + "/cache");
        m_profile->setPersistentStoragePath(base + "/storage");
        m_profile->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);

        // Network manager for downloads
        m_netMgr = new QNetworkAccessManager(this);

        // UI: tabs
        m_tabs = new QTabWidget;
        setCentralWidget(m_tabs);

        createToolBar();
        createSideBar();

        // Add initial tab
        addTab(QUrl("https://www.qt.io"));

        resize(1200, 800);
    }

private:
    QWebEngineProfile *m_profile;
    QTabWidget *m_tabs;
    QLineEdit *m_urlEdit;
    QProgressBar *m_pageProgress;
    QListWidget *m_bookmarks;
    QWidget *m_downloadPanel;
    QVBoxLayout *m_downloadLayout;
    QString m_bookmarksFile;
    QNetworkAccessManager *m_netMgr;

    QWebEngineView* currentView() {
        return qobject_cast<QWebEngineView*>(m_tabs->currentWidget());
    }

    void createToolBar() {
        auto *tb = addToolBar("Navigation");
        QAction *newTabA = tb->addAction("New Tab");
        connect(newTabA, &QAction::triggered, this, [this](){ addTab(QUrl("about:blank")); });

        QAction *proxyA = tb->addAction("Proxy");
        connect(proxyA, &QAction::triggered, this, &BrowserWindow::showProxyDialog);

        m_urlEdit = new QLineEdit;
        m_pageProgress = new QProgressBar; m_pageProgress->setRange(0,100);
        tb->addWidget(m_urlEdit);
        tb->addWidget(m_pageProgress);

        connect(m_urlEdit, &QLineEdit::returnPressed, this, [this](){
            QUrl u = QUrl::fromUserInput(m_urlEdit->text());
            if (currentView()) currentView()->load(u);
        });
    }

    void createSideBar() {
        auto *dock = new QDockWidget("Side", this);
        auto *container = new QWidget;
        auto *v = new QVBoxLayout(container);

        m_bookmarks = new QListWidget;
        v->addWidget(new QLabel("Bookmarks"));
        v->addWidget(m_bookmarks);

        QPushButton *addBm = new QPushButton("Add Bookmark");
        v->addWidget(addBm);
        connect(addBm, &QPushButton::clicked, this, &BrowserWindow::addBookmark);

        v->addWidget(new QLabel("Downloads"));
        m_downloadPanel = new QWidget;
        m_downloadLayout = new QVBoxLayout(m_downloadPanel);
        v->addWidget(m_downloadPanel);

        container->setLayout(v);
        dock->setWidget(container);
        addDockWidget(Qt::LeftDockWidgetArea, dock);

        m_bookmarksFile = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/bookmarks.json";
        loadBookmarks();

        connect(m_bookmarks, &QListWidget::itemActivated, this, [this](QListWidgetItem *it){
            if (!it) return;
            QUrl u(it->data(Qt::UserRole).toString());
            addTab(u);
        });
    }

    void addTab(const QUrl &url) {
        // Create page and view
        InterceptingPage *page = new InterceptingPage(m_profile, this);
        QWebEngineView *view = new QWebEngineView;
        view->setPage(page);

        int idx = m_tabs->addTab(view, "New");
        m_tabs->setCurrentIndex(idx);

        connect(view, &QWebEngineView::titleChanged, this, [this, idx](const QString &t){
            m_tabs->setTabText(idx, t);
        });
        connect(view, &QWebEngineView::urlChanged, this, [this, idx](const QUrl &u){
            if (m_tabs->currentIndex() == idx) m_urlEdit->setText(u.toString());
        });
        connect(view, &QWebEngineView::loadProgress, m_pageProgress, &QProgressBar::setValue);

        // Intercept download requests from page
        connect(page, &InterceptingPage::downloadRequestedByPage, this, &BrowserWindow::handleDownloadRequest);

        view->load(url);
    }

    // Called when page signals a download candidate
    void handleDownloadRequest(const QUrl &url) {
        // Ask user where to save
        QString suggested = QFileInfo(url.path()).fileName();
        if (suggested.isEmpty()) suggested = QStringLiteral("download");
        QString path = QFileDialog::getSaveFileName(this, QStringLiteral("Save As"), QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/" + suggested);
        if (path.isEmpty()) return;

        // Create task
        DownloadTask *task = new DownloadTask(url, path, m_netMgr, this);

        // UI row
        DownloadRow *row = new DownloadRow(task);
        m_downloadLayout->addWidget(row);

        // Connect cancel: we will abort the underlying reply by deleting the task's reply indirectly
        connect(row, &DownloadRow::requestCancel, this, [task, row](){
            // There is no direct access to reply from outside; we can delete the task to abort
            task->deleteLater();
            row->deleteLater();
        });

        // Clean up when finished or error
        connect(task, &DownloadTask::finished, this, [task, row](){
            // leave row visible but disable cancel
            QTimer::singleShot(5000, row, &QObject::deleteLater); // auto-remove after a while
            task->deleteLater();
        });
        connect(task, &DownloadTask::errorOccurred, this, [task, row](const QString &msg){
            Q_UNUSED(msg);
            QTimer::singleShot(10000, row, &QObject::deleteLater);
            task->deleteLater();
        });
    }

    void addBookmark() {
        QWebEngineView *v = currentView();
        if (!v) return;
        QUrl u = v->url();
        QJsonObject obj; obj["title"] = v->title(); obj["url"] = u.toString();
        QFile f(m_bookmarksFile);
        QJsonArray arr;
        if (f.open(QIODevice::ReadOnly)) {
            arr = QJsonDocument::fromJson(f.readAll()).array();
            f.close();
        }
        arr.append(obj);
        if (f.open(QIODevice::WriteOnly)) {
            f.write(QJsonDocument(arr).toJson());
            f.close();
        }
        loadBookmarks();
    }

    void loadBookmarks() {
        m_bookmarks->clear();
        QFile f(m_bookmarksFile);
        if (!f.open(QIODevice::ReadOnly)) return;
        QJsonArray arr = QJsonDocument::fromJson(f.readAll()).array();
        for (auto v : arr) {
            auto o = v.toObject();
            QListWidgetItem *it = new QListWidgetItem(o["title"].toString());
            it->setData(Qt::UserRole, o["url"].toString());
            m_bookmarks->addItem(it);
        }
    }

    void showProxyDialog() {
        QDialog d(this);
        QFormLayout fl(&d);
        QLineEdit host; QSpinBox port; QLineEdit user; QLineEdit pass;
        fl.addRow("Host", &host); fl.addRow("Port", &port); fl.addRow("User", &user); fl.addRow("Pass", &pass);
        port.setRange(0,65535);
        QPushButton ok("Apply");
        fl.addRow(&ok);
        connect(&ok, &QPushButton::clicked, &d, &QDialog::accept);
        if (d.exec() == QDialog::Accepted) {
            if (host.text().isEmpty()) {
                QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
            } else {
                QNetworkProxy p(QNetworkProxy::HttpProxy, host.text(), port.value());
                QNetworkProxy::setApplicationProxy(p);
            }
        }
    }
};

int main(int argc, char **argv) {
    QApplication a(argc, argv);

    // Ensure Qt WebEngine is initialized properly by creating a profile early
    BrowserWindow w;
    w.show();

    return a.exec();
}

#include "main.moc"
