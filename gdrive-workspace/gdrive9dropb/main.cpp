// main.cpp
// Qt6 single-file: GUI account selection + serial Drive download + two-way sync (Dropbox-like)
// Requires Qt6 Core, Widgets, Network, NetworkAuth, Gui

#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QProcess>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QEventLoop>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QDateTime>
#include <QStandardPaths>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QFileInfo>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QProgressBar>
#include <QTimer>
#include <QElapsedTimer>
#include <QScreen>
#include <QGuiApplication>
#include <QStyle>
#include <QMimeDatabase>

// --- Helper: human readable bytes ---
static QString humanReadableBytes(qint64 bytes) {
    if (bytes < 0) return QStringLiteral("-");
    const char *units[] = { "B", "KB", "MB", "GB", "TB" };
    double v = double(bytes);
    int u = 0;
    while (v >= 1024.0 && u < 4) {
        v /= 1024.0;
        ++u;
    }
    return QString::number(v, 'f', 3) + " " + QString::fromUtf8(units[u]);
}

// --- MetadataFetcher ---
class MetadataFetcher : public QObject {
    Q_OBJECT
public:
    explicit MetadataFetcher(QObject *parent = nullptr) : QObject(parent) {
        manager = new QNetworkAccessManager(this);
    }
public slots:
    void fetchAll(const QString &token) {
        QJsonArray allFiles;
        QUrl base("https://www.googleapis.com/drive/v3/files");
        QUrlQuery q;
        q.addQueryItem("pageSize", "1000");
        q.addQueryItem("fields", "nextPageToken, files(id, name, mimeType, parents, size, modifiedTime, md5Checksum)");
        base.setQuery(q);

        QUrl page = base;
        while (true) {
            QNetworkRequest req(page);
            req.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());
            req.setHeader(QNetworkRequest::UserAgentHeader, "DriveSync-Qt6");

            QNetworkReply *reply = manager->get(req);
            QEventLoop loop;
            connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
            loop.exec();

            if (reply->error() != QNetworkReply::NoError) {
                emit fetchError(reply->errorString());
                reply->deleteLater();
                return;
            }

            QByteArray data = reply->readAll();
            reply->deleteLater();

            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (!doc.isObject()) {
                emit fetchError(QStringLiteral("Ungültige API-Antwort (Metadaten)"));
                return;
            }
            QJsonObject obj = doc.object();
            QJsonArray files = obj.value("files").toArray();
            for (const QJsonValue &v : files) allFiles.append(v);

            QString nextPageToken = obj.value("nextPageToken").toString();
            if (nextPageToken.isEmpty()) break;

            QUrl next("https://www.googleapis.com/drive/v3/files");
            QUrlQuery nq;
            nq.addQueryItem("pageSize", "1000");
            nq.addQueryItem("fields", "nextPageToken, files(id, name, mimeType, parents, size, modifiedTime, md5Checksum)");
            nq.addQueryItem("pageToken", nextPageToken);
            next.setQuery(nq);
            page = next;
        }

        // compute totals
        qint64 totalBytes = 0;
        int fileCount = 0;
        for (const QJsonValue &v : allFiles) {
            if (!v.isObject()) continue;
            QJsonObject o = v.toObject();
            bool ok = false;
            qint64 s = o.value("size").toString().toLongLong(&ok);
            if (ok) totalBytes += s;
            if (o.value("mimeType").toString() != QStringLiteral("application/vnd.google-apps.folder")) ++fileCount;
        }

        emit metadataFound(allFiles);
        emit totalsComputed(totalBytes, fileCount);
    }

signals:
    void metadataFound(const QJsonArray &files);
    void fetchError(const QString &err);
    void totalsComputed(qint64 totalBytes, int fileCount);

private:
    QNetworkAccessManager *manager = nullptr;
};

// --- DownloadWorker (seriell) ---
class DownloadWorker : public QObject {
    Q_OBJECT
public:
    explicit DownloadWorker(QObject *parent = nullptr) : QObject(parent) {
        manager = new QNetworkAccessManager(this);
    }

public slots:
    void startDownloads(const QJsonArray &queue, const QString &token, const QString &syncPath) {
        qint64 totalBytes = 0;
        for (const QJsonValue &v : queue) {
            QJsonObject o = v.toObject();
            bool ok = false;
            qint64 s = o.value("size").toString().toLongLong(&ok);
            if (ok) totalBytes += s;
        }

        qint64 downloadedBytes = 0;
        QElapsedTimer timer;
        timer.start();

        for (const QJsonValue &v : queue) {
            QJsonObject meta = v.toObject();
            QString id = meta.value("id").toString();
            QString relPath = meta.value("path").toString();
            QString name = meta.value("name").toString();
            bool okSize = false;
            qint64 size = meta.value("size").toString().toLongLong(&okSize);

            QString safeRel = relPath.isEmpty() ? name : relPath;
            safeRel.replace('\\', '_');

            QFileInfo fi(QDir(syncPath).filePath(safeRel));
            QDir dir;
            QString dirPath = fi.path();
            if (!dir.exists(dirPath)) dir.mkpath(dirPath);
            QString outPath = QDir(syncPath).filePath(safeRel);

            // Determine local file size
            qint64 localSize = 0;
            QFile localFile(outPath);
            if (localFile.exists()) localSize = QFileInfo(localFile).size();

            // Skip if already complete
            if (okSize && localSize == size) {
                emit downloadStatus(QStringLiteral("Überspringe (bereits vollständig): %1").arg(name));
                downloadedBytes += localSize;
                emit bytesTransferred(localSize);
                emit overallProgress( totalBytes > 0 ? int((downloadedBytes * 100000) / totalBytes) : 0 );
                continue;
            }

            // If local larger than remote, remove and download fresh
            if (okSize && localSize > size) {
                emit downloadStatus(QStringLiteral("Lokale Datei größer als Remote, entferne und lade neu: %1").arg(name));
                QFile::remove(outPath);
                localSize = 0;
            }

            // Prepare request
            QUrl url(QString("https://www.googleapis.com/drive/v3/files/%1").arg(id));
            QUrlQuery q; q.addQueryItem("alt", "media"); url.setQuery(q);

            QNetworkRequest req(url);
            req.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());
            req.setHeader(QNetworkRequest::UserAgentHeader, "DriveSync-Qt6");

            QNetworkReply *reply = manager->get(req);
            QFile outFile(outPath + ".part");
            if (!outFile.open(QIODevice::WriteOnly)) {
                emit downloadStatus(QStringLiteral("Fehler beim Öffnen: %1").arg(outPath));
                reply->abort();
                reply->deleteLater();
                continue;
            }

            qint64 downloadedForFile = 0;

            emit downloadStatus(QStringLiteral("Starte Download: %1").arg(name));
            emit fileProgress(name, 0, (size>0?size:0));

            connect(reply, &QNetworkReply::readyRead, this, [this, reply, &outFile, &downloadedBytes, &downloadedForFile, name, size, &timer, totalBytes]() mutable {
                QByteArray chunk = reply->readAll();
                if (!chunk.isEmpty()) {
                    outFile.write(chunk);
                    downloadedForFile += chunk.size();
                    downloadedBytes += chunk.size();

                    emit bytesTransferred(chunk.size());
                    qint64 reportedTotal = (size > 0) ? size : downloadedForFile;
                    emit fileProgress(name, downloadedForFile, reportedTotal);

                    qint64 elapsedMs = timer.elapsed();
                    QString etaStr = QStringLiteral("unbekannt");
                    if (elapsedMs > 0 && totalBytes > 0) {
                        double rate = double(downloadedBytes) / (elapsedMs / 1000.0);
                        if (rate > 0.0) {
                            qint64 remaining = totalBytes - downloadedBytes;
                            qint64 etaSec = qMax<qint64>(0, qint64(remaining / rate));
                            etaStr = formatSeconds(etaSec);
                        }
                    }
                    emit etaUpdated(etaStr);

                    if (totalBytes > 0) {
                        int scaled = int((downloadedBytes * 100000) / totalBytes);
                        scaled = qBound(0, scaled, 100000);
                        emit overallProgress(scaled);
                    } else {
                        emit overallProgress(0);
                    }
                }
            });

            QEventLoop loop;
            connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
            loop.exec();

            if (reply->error() != QNetworkReply::NoError) {
                emit downloadStatus(QStringLiteral("Fehler beim Herunterladen: %1 - %2").arg(name, reply->errorString()));
                outFile.close();
                outFile.remove();
                reply->deleteLater();
                continue;
            }

            outFile.flush();
            outFile.close();

            QFile::remove(outPath);
            QFile::rename(outPath + ".part", outPath);

            emit downloadStatus(QStringLiteral("Heruntergeladen: %1").arg(name));
            reply->deleteLater();
        }

        emit overallProgress(100000);
        emit etaUpdated(QStringLiteral("0s"));
        emit downloadsFinished();
    }

signals:
    void fileProgress(const QString &name, qint64 rec, qint64 tot);
    void overallProgress(int percentScaled);
    void downloadStatus(const QString &s);
    void etaUpdated(const QString &eta);
    void downloadsFinished();
    void bytesTransferred(qint64 bytes);

private:
    QNetworkAccessManager *manager = nullptr;

    QString formatSeconds(qint64 s) {
        if (s < 60) return QStringLiteral("%1s").arg(s);
        qint64 h = s / 3600;
        qint64 m = (s % 3600) / 60;
        qint64 sec = s % 60;
        if (h > 0) return QStringLiteral("%1h %2m %3s").arg(h).arg(m).arg(sec);
        return QStringLiteral("%1m %2s").arg(m).arg(sec);
    }
};

// --- UploadWorker (seriell) using multipart simple upload (suitable for small/medium files) ---
class UploadWorker : public QObject {
    Q_OBJECT
public:
    explicit UploadWorker(QObject *parent = nullptr) : QObject(parent) {
        manager = new QNetworkAccessManager(this);
    }

public slots:
    void startUploads(const QJsonArray &queue, const QString &token, const QString &syncPath) {
        qint64 totalBytes = 0;
        for (const QJsonValue &v : queue) {
            QJsonObject o = v.toObject();
            QString localPath = o.value("localPath").toString();
            QFileInfo fi(localPath);
            if (fi.exists()) totalBytes += fi.size();
        }

        qint64 uploadedBytes = 0;
        QElapsedTimer timer;
        timer.start();

        for (const QJsonValue &v : queue) {
            QJsonObject meta = v.toObject();
            QString localPath = meta.value("localPath").toString();
            QString relPath = meta.value("path").toString();
            QString name = QFileInfo(localPath).fileName();

            QFile f(localPath);
            if (!f.exists()) {
                emit uploadStatus(QStringLiteral("Lokale Datei nicht gefunden: %1").arg(localPath));
                continue;
            }
            if (!f.open(QIODevice::ReadOnly)) {
                emit uploadStatus(QStringLiteral("Fehler beim Öffnen: %1").arg(localPath));
                continue;
            }
            qint64 fileSize = f.size();

            // Build multipart/related body for simple upload
            QByteArray boundary = "----DriveSyncBoundary";
            QByteArray metaJson;
            QJsonObject j;
            j.insert("name", name);
            // try to set parents if path contains folders - for simplicity we upload to root or create folders elsewhere
            metaJson = QJsonDocument(j).toJson(QJsonDocument::Compact);

            QByteArray body;
            body.append("--" + boundary + "\r\n");
            body.append("Content-Type: application/json; charset=UTF-8\r\n\r\n");
            body.append(metaJson);
            body.append("\r\n");
            body.append("--" + boundary + "\r\n");

            // detect mime
            QMimeDatabase mdb;
            QString mime = mdb.mimeTypeForFile(localPath).name();
            if (mime.isEmpty()) mime = "application/octet-stream";
            body.append("Content-Type: " + mime.toUtf8() + "\r\n\r\n");

            QByteArray fileData = f.readAll();
            body.append(fileData);
            body.append("\r\n");
            body.append("--" + boundary + "--\r\n");

            f.close();

            QUrl url("https://www.googleapis.com/upload/drive/v3/files?uploadType=multipart");
            QNetworkRequest req(url);
            req.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());
            req.setHeader(QNetworkRequest::UserAgentHeader, "DriveSync-Qt6");
            req.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/related; boundary=" + boundary);

            emit uploadStatus(QStringLiteral("Starte Upload: %1").arg(name));
            emit fileUploadProgress(name, 0, fileSize);

            QNetworkReply *reply = manager->post(req, body);

            // track progress via readyRead chunks (we already have full body sent, but we can track reply)
            qint64 receivedForFile = 0;
            connect(reply, &QNetworkReply::readyRead, this, [this, reply, &receivedForFile, &uploadedBytes, &fileSize, name, &timer, totalBytes]() mutable {
                QByteArray chunk = reply->readAll();
                if (!chunk.isEmpty()) {
                    // response body doesn't reflect upload progress; instead we update uploadedBytes when finished
                    receivedForFile += chunk.size();
                }
            });

            QEventLoop loop;
            connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
            loop.exec();

            if (reply->error() != QNetworkReply::NoError) {
                emit uploadStatus(QStringLiteral("Fehler beim Upload: %1 - %2").arg(name, reply->errorString()));
                reply->deleteLater();
                continue;
            }

            // assume full file uploaded
            uploadedBytes += fileSize;
            emit bytesTransferred(fileSize);
            emit fileUploadProgress(name, fileSize, fileSize);

            qint64 elapsedMs = timer.elapsed();
            QString etaStr = QStringLiteral("unbekannt");
            if (elapsedMs > 0 && totalBytes > 0) {
                double rate = double(uploadedBytes) / (elapsedMs / 1000.0);
                if (rate > 0.0) {
                    qint64 remaining = totalBytes - uploadedBytes;
                    qint64 etaSec = qMax<qint64>(0, qint64(remaining / rate));
                    etaStr = formatSeconds(etaSec);
                }
            }
            emit etaUpdated(etaStr);

            if (totalBytes > 0) {
                int scaled = int((uploadedBytes * 100000) / totalBytes);
                scaled = qBound(0, scaled, 100000);
                emit overallProgress(scaled);
            } else {
                emit overallProgress(0);
            }

            emit uploadStatus(QStringLiteral("Hochgeladen: %1").arg(name));
            reply->deleteLater();
        }

        emit overallProgress(100000);
        emit etaUpdated(QStringLiteral("0s"));
        emit uploadsFinished();
    }

signals:
    void fileUploadProgress(const QString &name, qint64 rec, qint64 tot);
    void overallProgress(int percentScaled);
    void uploadStatus(const QString &s);
    void etaUpdated(const QString &eta);
    void uploadsFinished();
    void bytesTransferred(qint64 bytes);

private:
    QNetworkAccessManager *manager = nullptr;

    QString formatSeconds(qint64 s) {
        if (s < 60) return QStringLiteral("%1s").arg(s);
        qint64 h = s / 3600;
        qint64 m = (s % 3600) / 60;
        qint64 sec = s % 60;
        if (h > 0) return QStringLiteral("%1h %2m %3s").arg(h).arg(m).arg(sec);
        return QStringLiteral("%1m %2s").arg(m).arg(sec);
    }
};

// --- SummaryWindow ---
class SummaryWindow : public QWidget {
    Q_OBJECT
public:
    explicit SummaryWindow(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle(QStringLiteral("DriveSync - Zusammenfassung"));
        setFixedSize(360, 100);
        QVBoxLayout *l = new QVBoxLayout(this);
        filesLabel = new QLabel(QStringLiteral("Dateien: 0"), this);
        bytesLabel = new QLabel(QStringLiteral("Gesamtgröße: 0 B"), this);
        l->addWidget(filesLabel);
        l->addWidget(bytesLabel);
    }

public slots:
    void updateTotals(qint64 bytes, int files) {
        filesLabel->setText(QStringLiteral("Dateien: %1").arg(files));
        bytesLabel->setText(QStringLiteral("Gesamtgröße: %1").arg(humanReadableBytes(bytes)));
    }

private:
    QLabel *filesLabel;
    QLabel *bytesLabel;
};

// --- ProgressWindow ---
class ProgressWindow : public QWidget {
    Q_OBJECT
public:
    explicit ProgressWindow(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle(QStringLiteral("DriveSync - Fortschritt"));
        setFixedSize(560, 200);
        QVBoxLayout *l = new QVBoxLayout(this);

        fileNameLabel = new QLabel(QStringLiteral("Aktuelle Datei: -"), this);
        filePercentLabel = new QLabel(QStringLiteral("Datei: 0.000 %"), this);
        fileProgress = new QProgressBar(this);
        fileProgress->setRange(0, 100000); // percent * 1000

        overallPercentLabel = new QLabel(QStringLiteral("Gesamt: 0.000 %"), this);
        overallProgress = new QProgressBar(this);
        overallProgress->setRange(0, 100000); // percent * 1000

        etaLabel = new QLabel(QStringLiteral("ETA: -"), this);
        statusLabel = new QLabel(QStringLiteral("Status: -"), this);

        l->addWidget(fileNameLabel);
        l->addWidget(filePercentLabel);
        l->addWidget(fileProgress);
        l->addWidget(overallPercentLabel);
        l->addWidget(overallProgress);
        l->addWidget(etaLabel);
        l->addWidget(statusLabel);
    }

public slots:
    void onFileProgress(const QString &name, qint64 rec, qint64 tot) {
        fileNameLabel->setText(QStringLiteral("Aktuelle Datei: %1").arg(name));
        double percent = 0.0;
        if (tot > 0) percent = (double(rec) / double(tot)) * 100.0;
        int scaled = int(qBound(0.0, percent * 1000.0, 100000.0));
        fileProgress->setValue(scaled);
        filePercentLabel->setText(QStringLiteral("Datei: %1 %").arg(QString::number(percent, 'f', 3)));
    }
    void onOverallProgress(int scaled) {
        overallProgress->setValue(qBound(0, scaled, 100000));
        double percent = double(qBound(0, scaled, 100000)) / 1000.0;
        overallPercentLabel->setText(QStringLiteral("Gesamt: %1 %").arg(QString::number(percent, 'f', 3)));
    }
    void onEtaUpdated(const QString &eta) { etaLabel->setText(QStringLiteral("ETA: %1").arg(eta)); }
    void onStatus(const QString &s) { statusLabel->setText(QStringLiteral("Status: %1").arg(s)); }

private:
    QLabel *fileNameLabel;
    QLabel *filePercentLabel;
    QProgressBar *fileProgress;
    QLabel *overallPercentLabel;
    QProgressBar *overallProgress;
    QLabel *etaLabel;
    QLabel *statusLabel;
};

// --- DriveSync (Hauptklasse) mit two-way sync ---
class DriveSync : public QObject {
    Q_OBJECT
public:
    explicit DriveSync(const QString &preferredAccount = QString(), QObject *parent = nullptr) : QObject(parent), preferredAccountEmail(preferredAccount) {
        setupLocalDirectory();
        setupUi();
        setupOAuth();
        connect(this, &DriveSync::requestStartSync, this, &DriveSync::startSync, Qt::QueuedConnection);
        connect(google, &QOAuth2AuthorizationCodeFlow::granted, this, [this]() { emit requestStartSync(); });
    }

    ~DriveSync() {
        if (trayIcon) trayIcon->hide();
    }

signals:
    void requestStartSync();

public slots:
    void startSync() {
        qDebug() << "[status] Start sync";
        QString token = currentAccessToken();
        if (token.isEmpty()) {
            qDebug() << "[status] Kein Access-Token. Bitte anmelden.";
            return;
        }

        MetadataFetcher *fetcher = new MetadataFetcher();
        QThread *t = new QThread();
        fetcher->moveToThread(t);

        connect(t, &QThread::started, fetcher, [fetcher, token]() { fetcher->fetchAll(token); });
        connect(fetcher, &MetadataFetcher::metadataFound, this, &DriveSync::onMetadataFetched);
        connect(fetcher, &MetadataFetcher::fetchError, this, [this](const QString &e){ qWarning() << "Fehler beim Metadatenabruf:" << e; QCoreApplication::quit(); });
        connect(fetcher, &MetadataFetcher::metadataFound, fetcher, &QObject::deleteLater);
        connect(fetcher, &MetadataFetcher::metadataFound, t, &QThread::quit);
        connect(fetcher, &MetadataFetcher::totalsComputed, this, &DriveSync::onTotalsComputed);
        connect(t, &QThread::finished, t, &QThread::deleteLater);

        t->start();
    }

private slots:
    void onTotalsComputed(qint64 totalBytes, int fileCount) {
        QString sizeStr = humanReadableBytes(totalBytes);
        QString tooltip = QStringLiteral("DriveSync: %1 Dateien, %2").arg(fileCount).arg(sizeStr);
        if (trayIcon) trayIcon->setToolTip(tooltip);
        if (summaryWindow) summaryWindow->updateTotals(totalBytes, fileCount);
    }

    void onMetadataFetched(const QJsonArray &files) {
        qDebug() << "[status] Metadaten erhalten:" << files.size();
        idIndex.clear();
        for (const QJsonValue &v : files) {
            if (!v.isObject()) continue;
            QJsonObject meta = v.toObject();
            QString id = meta.value("id").toString();
            if (!id.isEmpty()) idIndex.insert(id, meta);
        }
        buildPathIndexAndQueues();
    }

    void onDownloadStatus(const QString &s) {
        qDebug() << s;
        if (progressWindow) progressWindow->onStatus(s);
        if (trayIcon) trayIcon->setToolTip(s);
    }
    void onUploadStatus(const QString &s) {
        qDebug() << s;
        if (progressWindow) progressWindow->onStatus(s);
        if (trayIcon) trayIcon->setToolTip(s);
    }
    void onDownloadFileProgress(const QString &name, qint64 rec, qint64 tot) {
        qDebug() << "[file]" << name << rec << "/" << tot;
        if (progressWindow) progressWindow->onFileProgress(name, rec, tot);
    }
    void onUploadFileProgress(const QString &name, qint64 rec, qint64 tot) {
        qDebug() << "[upload]" << name << rec << "/" << tot;
        if (progressWindow) progressWindow->onFileProgress(name, rec, tot);
    }
    void onOverallProgress(int p) {
        qDebug() << "[overall scaled]" << p;
        if (progressWindow) progressWindow->onOverallProgress(p);
    }
    void onEtaUpdated(const QString &eta) {
        qDebug() << "[eta]" << eta;
        if (progressWindow) progressWindow->onEtaUpdated(eta);
    }
    void onTransfersFinished() {
        qDebug() << "[status] Alle Transfers abgeschlossen. Beende.";
        if (trayIcon) trayIcon->showMessage(QStringLiteral("DriveSync"), QStringLiteral("Alle Transfers abgeschlossen."));
        QCoreApplication::quit();
    }

    void showSummaryWindow() {
        if (summaryWindow) summaryWindow->show();
        summaryWindow->raise();
        summaryWindow->activateWindow();
    }
    void showProgressWindow() {
        if (progressWindow) progressWindow->show();
        progressWindow->raise();
        progressWindow->activateWindow();
    }
    void quitApp() {
        QCoreApplication::quit();
    }

private:
    QOAuth2AuthorizationCodeFlow *google = nullptr;
    QString syncPath;
    QString tokenFilePath;
    QMutex tokenMutex;
    QHash<QString, QJsonObject> idIndex;
    QHash<QString, QJsonObject> pathIndex;
    QString preferredAccountEmail;

    // UI
    QSystemTrayIcon *trayIcon = nullptr;
    SummaryWindow *summaryWindow = nullptr;
    ProgressWindow *progressWindow = nullptr;

    // Queues
    QJsonArray downloadQueue;
    QJsonArray uploadQueue;

    // Overall accounting
    qint64 totalBytesToTransfer = 0;
    qint64 bytesTransferredSoFar = 0;
    QMutex bytesMutex;

    void setupLocalDirectory() {
        syncPath = QDir::homePath() + "/google_drive";
        QDir d(syncPath);
        if (!d.exists()) d.mkpath(".");
        QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
        if (configDir.isEmpty()) configDir = QDir::homePath() + "/.config";
        QDir().mkpath(configDir + "/DriveSync");
        tokenFilePath = configDir + "/DriveSync/token.txt";
    }

    void setupUi() {
        // Tray icon
        QIcon icon = QIcon::fromTheme("folder-download");
        if (icon.isNull()) icon = QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton);
        trayIcon = new QSystemTrayIcon(icon, this);
        QMenu *menu = new QMenu();
        QAction *openSummary = new QAction(QStringLiteral("Zusammenfassung anzeigen"), menu);
        QAction *openProgress = new QAction(QStringLiteral("Fortschritt anzeigen"), menu);
        QAction *quitAct = new QAction(QStringLiteral("Beenden"), menu);
        menu->addAction(openSummary);
        menu->addAction(openProgress);
        menu->addSeparator();
        menu->addAction(quitAct);
        trayIcon->setContextMenu(menu);
        connect(openSummary, &QAction::triggered, this, &DriveSync::showSummaryWindow);
        connect(openProgress, &QAction::triggered, this, &DriveSync::showProgressWindow);
        connect(quitAct, &QAction::triggered, this, &DriveSync::quitApp);
        trayIcon->show();

        // Windows
        summaryWindow = new SummaryWindow();
        progressWindow = new ProgressWindow();

        // Position top-right and show immediately
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) {
            QRect g = screen->availableGeometry();
            const int margin = 12;
            summaryWindow->move(g.right() - summaryWindow->width() - margin, g.top() + margin);
            summaryWindow->show();
            progressWindow->move(g.right() - progressWindow->width() - margin, g.top() + summaryWindow->height() + margin*2);
            progressWindow->show();
        } else {
            summaryWindow->show();
            progressWindow->show();
        }

        // Double click tray to show progress
        connect(trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason r){
            if (r == QSystemTrayIcon::DoubleClick) showProgressWindow();
        });
    }

    void setupOAuth() {
        google = new QOAuth2AuthorizationCodeFlow(this);
        google->setAuthorizationUrl(QUrl("https://accounts.google.com/o/oauth2/v2/auth"));
        google->setTokenUrl(QUrl("https://oauth2.googleapis.com/token"));

        // TODO: Replace with your own client id/secret
        google->setClientIdentifier(QStringLiteral("487363744399-of79l9lg2rqigjvgq9meqlreo1iqn9a3.apps.googleusercontent.com"));
        google->setClientIdentifierSharedKey(QStringLiteral("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"));

        auto replyHandler = new QOAuthHttpServerReplyHandler(1337, this);
        google->setReplyHandler(replyHandler);

        QVariant scopeVar = QVariant::fromValue(QStringList{ QStringLiteral("https://www.googleapis.com/auth/drive") , QStringLiteral("https://www.googleapis.com/auth/userinfo.email") });
        google->setProperty("requestedScopeTokens", scopeVar);

        QString hint = preferredAccountEmail;
        google->setModifyParametersFunction([hint](QAbstractOAuth::Stage stage, QMultiMap<QString, QVariant> *parameters) {
            if (!parameters) return;
            if (stage == QAbstractOAuth::Stage::RequestingAuthorization) {
                parameters->insert("access_type", "offline");
                parameters->insert("prompt", "select_account consent");
                if (!parameters->contains("scope")) {
                    parameters->insert("scope", QStringLiteral("https://www.googleapis.com/auth/drive https://www.googleapis.com/auth/userinfo.email"));
                }
                if (!hint.isEmpty()) parameters->insert("login_hint", hint);
            }
        });

        connect(google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, [](const QUrl &url) {
            qDebug() << "Authorization URL:" << url.toString();
            bool started = QProcess::startDetached(QStringLiteral("xdg-open"), QStringList{ url.toString() });
            if (!started) {
                qDebug() << "Bitte öffne die folgende URL manuell im Browser:";
                qDebug() << url.toString();
            }
        });

        connect(google,
                QOverload<QAbstractOAuth::Error>::of(&QAbstractOAuth::requestFailed),
                this,
                [this](QAbstractOAuth::Error err) {
                    Q_UNUSED(err);
                    qWarning() << "OAuth Fehler";
                });

        connect(google, &QOAuth2AuthorizationCodeFlow::granted, this, [this]() {
            QString at = google->token();
            QString rt;
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
            rt = google->refreshToken();
#else
            QVariant v = google->property("refreshToken");
            if (v.isValid()) rt = v.toString();
#endif
            QFile f(tokenFilePath);
            if (f.open(QIODevice::WriteOnly)) {
                f.write(at.toUtf8());
                f.close();
                qDebug() << "[status] Token gespeichert in" << tokenFilePath;
            }
            // Query userinfo
            QNetworkAccessManager nam;
            QNetworkRequest req(QUrl("https://www.googleapis.com/oauth2/v2/userinfo"));
            req.setRawHeader("Authorization", QString("Bearer %1").arg(at).toUtf8());
            QEventLoop loop;
            QNetworkReply *r = nam.get(req);
            connect(r, &QNetworkReply::finished, &loop, &QEventLoop::quit);
            loop.exec();
            if (r->error() == QNetworkReply::NoError) {
                QByteArray data = r->readAll();
                QJsonDocument doc = QJsonDocument::fromJson(data);
                if (doc.isObject()) {
                    QJsonObject obj = doc.object();
                    QString email = obj.value("email").toString();
                    qDebug() << "[status] Autorisiert für Account:" << email;
                    if (!preferredAccountEmail.isEmpty() && preferredAccountEmail != email) {
                        qWarning() << "[warn] Gewünschtes Konto" << preferredAccountEmail << "stimmt nicht überein mit autorisiertem Konto" << email;
                        qWarning() << "Fortfahren mit dem autorisierten Konto.";
                    }
                }
            } else {
                qWarning() << "Fehler beim Abfragen der Userinfo:" << r->errorString();
            }
            r->deleteLater();
        });

        QFile f(tokenFilePath);
        if (f.exists() && f.open(QIODevice::ReadOnly)) {
            QByteArray at = f.readAll().trimmed();
            f.close();
            if (!at.isEmpty()) {
                google->setToken(QString::fromUtf8(at));
                QMetaObject::invokeMethod(this, "startSync", Qt::QueuedConnection);
                return;
            }
        }

        google->grant();
    }

    QString currentAccessToken() {
        QMutexLocker locker(&tokenMutex);
        if (google) return google->token();
        return QString();
    }

    QString buildFullPathForId(const QString &id) {
        if (id.isEmpty()) return QString();
        QStringList parts;
        QSet<QString> visited;
        QString cur = id;
        while (!cur.isEmpty()) {
            if (visited.contains(cur)) {
                qWarning() << "Zyklus in Elternkette für id:" << cur;
                break;
            }
            visited.insert(cur);
            if (!idIndex.contains(cur)) break;
            QJsonObject meta = idIndex.value(cur);
            QString name = meta.value("name").toString();
            if (!name.isEmpty()) parts.prepend(name);
            QJsonArray parents = meta.value("parents").toArray();
            if (parents.isEmpty()) break;
            QString nextParent = parents.first().toString();
            if (nextParent.isEmpty()) break;
            cur = nextParent;
        }
        QString fullPath = parts.join('/');
        return fullPath;
    }

    // Build both download and upload queues using a Dropbox-like conflict resolution:
    // - If remote exists and local missing -> download
    // - If local exists and remote missing -> upload
    // - If both exist: compare modifiedTime (remote vs local). Newer wins (upload or download).
    // - If sizes equal and mtimes similar -> skip
    void buildPathIndexAndQueues() {
        qDebug() << "[status] Erzeuge Pfadindex und Queues...";
        pathIndex.clear();
        for (auto it = idIndex.constBegin(); it != idIndex.constEnd(); ++it) {
            QString id = it.key();
            QJsonObject meta = it.value();
            QString fullPath = buildFullPathForId(id);
            if (fullPath.isEmpty()) {
                fullPath = meta.value("name").toString();
                if (fullPath.isEmpty()) fullPath = id;
            }
            QJsonObject copy = meta;
            copy.insert("path", fullPath);
            pathIndex.insert(fullPath, copy);
        }

        // Scan local
        QHash<QString, QJsonObject> localIndex;
        QDir root(syncPath);
        if (!root.exists()) root.mkpath(".");
        std::function<void(const QString&, const QString&)> scanDir = [&](const QString &base, const QString &relBase) {
            QDir d(base);
            QFileInfoList list = d.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
            for (const QFileInfo &fi : list) {
                QString rel = relBase.isEmpty() ? fi.fileName() : relBase + "/" + fi.fileName();
                QJsonObject meta;
                meta.insert("isDir", fi.isDir());
                meta.insert("size", QString::number(fi.size()));
                meta.insert("mtime", QString::number(fi.lastModified().toSecsSinceEpoch()));
                meta.insert("localPath", fi.absoluteFilePath());
                localIndex.insert(rel, meta);
                if (fi.isDir()) scanDir(fi.absoluteFilePath(), rel);
            }
        };
        scanDir(root.absolutePath(), QString());

        downloadQueue = QJsonArray();
        uploadQueue = QJsonArray();
        totalBytesToTransfer = 0;
        bytesTransferredSoFar = 0;

        QSet<QString> processed;

        // iterate union of keys
        QSet<QString> allKeys;
        for (auto it = pathIndex.constBegin(); it != pathIndex.constEnd(); ++it) allKeys.insert(it.key());
        for (auto it = localIndex.constBegin(); it != localIndex.constEnd(); ++it) allKeys.insert(it.key());

        for (const QString &rel : allKeys) {
            if (processed.contains(rel)) continue;
            processed.insert(rel);

            bool remoteExists = pathIndex.contains(rel);
            bool localExists = localIndex.contains(rel);

            if (remoteExists) {
                QJsonObject rmeta = pathIndex.value(rel);
                bool isFolder = rmeta.value("mimeType").toString() == QStringLiteral("application/vnd.google-apps.folder");
                if (isFolder) {
                    // ensure local folder exists
                    QString localDir = QDir(syncPath).filePath(rel);
                    QDir().mkpath(localDir);
                    continue;
                }
            }

            if (remoteExists && !localExists) {
                // download
                QJsonObject rmeta = pathIndex.value(rel);
                downloadQueue.append(rmeta);
                bool ok = false;
                qint64 s = rmeta.value("size").toString().toLongLong(&ok);
                if (ok) totalBytesToTransfer += s;
                qDebug() << "[queue] download (remote only)" << rel;
                continue;
            }

            if (!remoteExists && localExists) {
                // upload
                QJsonObject lmeta = localIndex.value(rel);
                QJsonObject umeta;
                umeta.insert("path", rel);
                umeta.insert("localPath", lmeta.value("localPath").toString());
                uploadQueue.append(umeta);
                bool ok = false;
                qint64 s = lmeta.value("size").toString().toLongLong(&ok);
                if (ok) totalBytesToTransfer += s;
                qDebug() << "[queue] upload (local only)" << rel;
                continue;
            }

            if (remoteExists && localExists) {
                QJsonObject rmeta = pathIndex.value(rel);
                QJsonObject lmeta = localIndex.value(rel);
                bool rOk = false, lOk = false;
                qint64 rsize = rmeta.value("size").toString().toLongLong(&rOk);
                qint64 lsize = lmeta.value("size").toString().toLongLong(&lOk);
                qint64 rmtime = 0;
                if (rmeta.contains("modifiedTime")) rmtime = QDateTime::fromString(rmeta.value("modifiedTime").toString(), Qt::ISODate).toSecsSinceEpoch();
                qint64 lmtime = lmeta.value("mtime").toString().toLongLong();

                // If sizes equal and mtimes within 2 seconds -> skip
                if (rOk && lOk && rsize == lsize && qAbs(rmtime - lmtime) <= 2) {
                    continue;
                }

                // Newer wins (Dropbox-like): if remote newer -> download, if local newer -> upload
                if (rmtime > lmtime + 2) {
                    downloadQueue.append(rmeta);
                    if (rOk) totalBytesToTransfer += rsize;
                    qDebug() << "[queue] download (remote newer)" << rel;
                } else if (lmtime > rmtime + 2) {
                    QJsonObject umeta;
                    umeta.insert("path", rel);
                    umeta.insert("localPath", lmeta.value("localPath").toString());
                    uploadQueue.append(umeta);
                    if (lOk) totalBytesToTransfer += lsize;
                    qDebug() << "[queue] upload (local newer)" << rel;
                } else {
                    // sizes differ but mtimes similar -> prefer download
                    downloadQueue.append(rmeta);
                    if (rOk) totalBytesToTransfer += rsize;
                    qDebug() << "[queue] download (size mismatch)" << rel;
                }
            }
        }

        // If nothing to do
        if (downloadQueue.isEmpty() && uploadQueue.isEmpty()) {
            qDebug() << "[status] Keine Transfers erforderlich. Beende.";
            QCoreApplication::quit();
            return;
        }

        // Start workers and combine progress
        startWorkers();
    }

    void startWorkers() {
        QString token = currentAccessToken();

        // Download worker
        if (!downloadQueue.isEmpty()) {
            DownloadWorker *dw = new DownloadWorker();
            QThread *dt = new QThread();
            dw->moveToThread(dt);

            connect(dt, &QThread::started, dw, [dw, this, token]() {
                dw->startDownloads(downloadQueue, token, syncPath);
            });

            connect(dw, &DownloadWorker::downloadStatus, this, &DriveSync::onDownloadStatus);
            connect(dw, &DownloadWorker::fileProgress, this, &DriveSync::onDownloadFileProgress);
            connect(dw, &DownloadWorker::overallProgress, this, &DriveSync::onOverallProgress);
            connect(dw, &DownloadWorker::etaUpdated, this, &DriveSync::onEtaUpdated);
            connect(dw, &DownloadWorker::downloadsFinished, this, &DriveSync::onWorkerFinished);
            connect(dw, &DownloadWorker::bytesTransferred, this, &DriveSync::onBytesTransferred);

            connect(dw, &DownloadWorker::downloadsFinished, dw, &QObject::deleteLater);
            connect(dw, &DownloadWorker::downloadsFinished, dt, &QThread::quit);
            connect(dt, &QThread::finished, dt, &QThread::deleteLater);

            dt->start();
        }

        // Upload worker
        if (!uploadQueue.isEmpty()) {
            UploadWorker *uw = new UploadWorker();
            QThread *ut = new QThread();
            uw->moveToThread(ut);

            connect(ut, &QThread::started, uw, [uw, this, token]() {
                uw->startUploads(uploadQueue, token, syncPath);
            });

            connect(uw, &UploadWorker::uploadStatus, this, &DriveSync::onUploadStatus);
            connect(uw, &UploadWorker::fileUploadProgress, this, &DriveSync::onUploadFileProgress);
            connect(uw, &UploadWorker::overallProgress, this, &DriveSync::onOverallProgress);
            connect(uw, &UploadWorker::etaUpdated, this, &DriveSync::onEtaUpdated);
            connect(uw, &UploadWorker::uploadsFinished, this, &DriveSync::onWorkerFinished);
            connect(uw, &UploadWorker::bytesTransferred, this, &DriveSync::onBytesTransferred);

            connect(uw, &UploadWorker::uploadsFinished, uw, &QObject::deleteLater);
            connect(uw, &UploadWorker::uploadsFinished, ut, &QThread::quit);
            connect(ut, &QThread::finished, ut, &QThread::deleteLater);

            ut->start();
        }
    }

    // Called when either worker finishes; when both finished, exit
    void onWorkerFinished() {
        static int finishedCount = 0;
        finishedCount++;
        if (finishedCount >= ((downloadQueue.isEmpty() ? 0 : 1) + (uploadQueue.isEmpty() ? 0 : 1))) {
            emit onTransfersFinished();
        }
    }

    // update overall accounting when workers report bytes transferred
    void onBytesTransferred(qint64 bytes) {
        QMutexLocker locker(&bytesMutex);
        bytesTransferredSoFar += bytes;
        int scaled = 0;
        if (totalBytesToTransfer > 0) scaled = int((bytesTransferredSoFar * 100000) / totalBytesToTransfer);
        scaled = qBound(0, scaled, 100000);
        emit onOverallProgress(scaled);
    }
};

// --- Simple dialog to ask for preferred account email ---
class AccountDialog : public QDialog {
    Q_OBJECT
public:
    explicit AccountDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle(QStringLiteral("Konto auswählen"));
        setModal(true);
        setFixedSize(420, 120);

        QLabel *lbl = new QLabel(QStringLiteral("Gib die gewünschte Google-Account-E-Mail ein (optional):"), this);
        emailEdit = new QLineEdit(this);
        emailEdit->setPlaceholderText(QStringLiteral("user@example.com"));

        QPushButton *ok = new QPushButton(QStringLiteral("OK"), this);
        QPushButton *cancel = new QPushButton(QStringLiteral("Abbrechen"), this);

        QHBoxLayout *btnRow = new QHBoxLayout();
        btnRow->addStretch();
        btnRow->addWidget(ok);
        btnRow->addWidget(cancel);

        QVBoxLayout *main = new QVBoxLayout(this);
        main->addWidget(lbl);
        main->addWidget(emailEdit);
        main->addLayout(btnRow);

        connect(ok, &QPushButton::clicked, this, &AccountDialog::accept);
        connect(cancel, &QPushButton::clicked, this, &AccountDialog::reject);
    }

    QString email() const { return emailEdit->text().trimmed(); }

private:
    QLineEdit *emailEdit;
};

// --- main ---
int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("DriveSync");
    QApplication::setApplicationVersion("1.0");

    // Show account selection dialog
    AccountDialog dlg;
    if (dlg.exec() != QDialog::Accepted) {
        qDebug() << "Abgebrochen durch Benutzer.";
        return 0;
    }
    QString preferred = dlg.email();
    qDebug() << "Gewähltes Konto (login_hint):" << (preferred.isEmpty() ? QStringLiteral("<keines>") : preferred);

    // Start DriveSync with chosen account
    DriveSync ds(preferred);
    return app.exec();
}

#include "main.moc"
