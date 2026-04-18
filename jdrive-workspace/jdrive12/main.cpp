// main.cpp
// DriveSync - Ordnerstruktur rekonstruieren und alle Dateien herunterladen (Qt6)
// Zwei-Wege-Synchronisation (Dropbox-ähnlich), automatischer Start, Tray-Icon und kleines Statusfenster
// Enthält zusätzliche Debug-Ausgaben zur Pfadrekonstruktion und Queue-Bildung.

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QFile>
#include <QDir>
#include <QFileSystemWatcher>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QUrlQuery>
#include <QMutex>
#include <QDateTime>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QDesktopServices>
#include <QFileInfo>
#include <QTextEdit>
#include <QElapsedTimer>
#include <QIcon>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <QAbstractOAuth>
#include <QThread>
#include <QEventLoop>
#include <QMetaType>

    // Worker: MetadataFetcher
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
            req.setHeader(QNetworkRequest::UserAgentHeader, "DriveSync-Qt");

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

        emit metadataFound(allFiles);
    }

signals:
    void metadataFound(const QJsonArray &files);
    void fetchError(const QString &err);

private:
    QNetworkAccessManager *manager = nullptr;
};

// Worker: DownloadWorker (seriell, jede Datei komplett, dann nächste)
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

        qint64 downloadedBytes = 0; // kumulativ über alle Dateien
        int index = 0;
        QElapsedTimer timer;
        timer.start();

        for (const QJsonValue &v : queue) {
            QJsonObject meta = v.toObject();
            QString id = meta.value("__id").toString();
            if (id.isEmpty()) id = meta.value("id").toString();
            QString name = meta.value("name").toString();
            bool okSize = false;
            qint64 size = meta.value("size").toString().toLongLong(&okSize);

            qint64 downloadedForFile = 0;

            QString safeRel = name;
            safeRel.replace('\\', '_');
            QFileInfo fi(QDir(syncPath).filePath(safeRel));
            QDir dir;
            QString dirPath = fi.path();
            if (!dir.exists(dirPath)) dir.mkpath(dirPath);
            QString outPath = QDir(syncPath).filePath(safeRel);

            QUrl url(QString("https://www.googleapis.com/drive/v3/files/%1").arg(id));
            QUrlQuery q; q.addQueryItem("alt", "media"); url.setQuery(q);

            QNetworkRequest req(url);
            req.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());
            req.setHeader(QNetworkRequest::UserAgentHeader, "DriveSync-Qt");

            QNetworkReply *reply = manager->get(req);
            QFile outFile(outPath + ".part");
            if (!outFile.open(QIODevice::WriteOnly)) {
                emit downloadStatus(QStringLiteral("Fehler beim Öffnen: %1").arg(outPath));
                reply->abort();
                reply->deleteLater();
                continue;
            }

            // readyRead handler for this file
            auto readyReadConn = connect(reply, &QNetworkReply::readyRead, this, [this, reply, &outFile, &downloadedBytes, &downloadedForFile, name, size, &timer, totalBytes]() mutable {
                QByteArray chunk = reply->readAll();
                if (!chunk.isEmpty()) {
                    outFile.write(chunk);
                    downloadedForFile += chunk.size();
                    downloadedBytes += chunk.size();

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
                disconnect(readyReadConn);
                continue;
            }

            outFile.flush();
            outFile.close();

            QFile::remove(outPath);
            QFile::rename(outPath + ".part", outPath);

            index++;
            int overallPercent = 0;
            if (totalBytes > 0) overallPercent = qBound(0, int((downloadedBytes * 100) / totalBytes), 100);
            else overallPercent = qMin(99, index * 100 / qMax(1, queue.size()));

            emit overallProgress(overallPercent);
            emit downloadStatus(QStringLiteral("Heruntergeladen: %1").arg(name));

            reply->deleteLater();
            disconnect(readyReadConn);
        }

        emit overallProgress(100);
        emit etaUpdated(QStringLiteral("0s"));
        emit downloadsFinished();
    }

signals:
    void fileProgress(const QString &name, qint64 rec, qint64 tot);
    void overallProgress(int percent);
    void downloadStatus(const QString &s);
    void etaUpdated(const QString &eta);
    void downloadsFinished();

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

// DriveSync (Hauptklasse)
class DriveSync : public QObject {
    Q_OBJECT
public:
    explicit DriveSync(QObject *parent = nullptr) : QObject(parent) {
        setupLocalDirectory();
        setupOAuth();
        setupWatcher();

        connect(google, &QOAuth2AuthorizationCodeFlow::granted, this, &DriveSync::startDropboxSync);

        QString accessToken, refreshToken;
        qint64 expiresAt = 0;
        if (loadTokens(accessToken, refreshToken, expiresAt)) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
            if (!refreshToken.isEmpty()) google->setRefreshToken(refreshToken);
            if (!accessToken.isEmpty()) google->setToken(accessToken);
#else
            if (!refreshToken.isEmpty()) google->setProperty("refreshToken", refreshToken);
            if (!accessToken.isEmpty()) google->setProperty("accessToken", accessToken);
#endif
            if (!refreshToken.isEmpty()) {
                QMetaObject::invokeMethod(google, "refreshTokens", Qt::QueuedConnection);
            }
            QMetaObject::invokeMethod(this, "startDropboxSync", Qt::QueuedConnection);
        } else {
            google->grant();
        }
    }

signals:
    void overallProgress(int percent);
    void fileProgress(const QString &fileName, qint64 received, qint64 total);
    void status(const QString &s);
    void etaUpdated(const QString &eta);

public slots:
    void startDropboxSync() {
        emit overallProgress(0);
        emit status(QStringLiteral("Starte Zwei-Wege-Synchronisation..."));
        scanLocalState();

        QString token = currentAccessToken();
        if (token.isEmpty()) {
            emit status(QStringLiteral("Kein Access-Token. Bitte anmelden."));
            return;
        }

        MetadataFetcher *fetcher = new MetadataFetcher();
        QThread *t = new QThread();
        fetcher->moveToThread(t);

        connect(t, &QThread::started, fetcher, [fetcher, token]() { fetcher->fetchAll(token); });
        connect(fetcher, &MetadataFetcher::metadataFound, this, &DriveSync::onMetadataFetched);
        connect(fetcher, &MetadataFetcher::fetchError, this, [this](const QString &e){ emit status(QStringLiteral("Fehler beim Metadatenabruf: %1").arg(e)); });
        connect(fetcher, &MetadataFetcher::metadataFound, fetcher, &QObject::deleteLater);
        connect(fetcher, &MetadataFetcher::metadataFound, t, &QThread::quit);
        connect(t, &QThread::finished, t, &QThread::deleteLater);

        t->start();
    }

private slots:
    void onMetadataFetched(const QJsonArray &files) {
        idIndex.clear();
        for (const QJsonValue &v : files) {
            if (!v.isObject()) continue;
            QJsonObject meta = v.toObject();
            QString id = meta.value("id").toString();
            if (!id.isEmpty()) idIndex.insert(id, meta);
        }
        emit status(QStringLiteral("Metadaten vollständig (%1 Objekte)").arg(idIndex.size()));
        buildLocalFoldersFromIndex();
        reconcileAndSync();
    }

    void onDownloadStatus(const QString &s) { emit status(s); }
    void onDownloadFileProgress(const QString &name, qint64 rec, qint64 tot) { emit fileProgress(name, rec, tot); }
    void onDownloadOverall(int p) { emit overallProgress(p); }
    void onDownloadEta(const QString &eta) { emit etaUpdated(eta); }
    void onDownloadsFinished() { emit status(QStringLiteral("Alle Dateien heruntergeladen")); }

private:
    QOAuth2AuthorizationCodeFlow *google = nullptr;
    QFileSystemWatcher *watcher = nullptr;
    QString syncPath;
    QString tokenFilePath;
    QMutex tokenMutex;
    QHash<QString, QJsonObject> idIndex;
    QHash<QString, QJsonObject> localIndex;

    void setupLocalDirectory() {
        syncPath = QDir::homePath() + "/google_drive";
        QDir d(syncPath);
        if (!d.exists()) d.mkpath(".");
        QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
        if (configDir.isEmpty()) configDir = QDir::homePath() + "/.config";
        QDir().mkpath(configDir + "/DriveSync");
        tokenFilePath = configDir + "/DriveSync/tokens.json";
    }

    void setupOAuth() {
        google = new QOAuth2AuthorizationCodeFlow(this);
        google->setAuthorizationUrl(QUrl("https://accounts.google.com/o/oauth2/v2/auth"));
        google->setTokenUrl(QUrl("https://oauth2.googleapis.com/token"));

        // Client ID/Secret (user-provided)
        google->setClientIdentifier(QStringLiteral("487363744399-g0h02noep3qgc2q7b5co0undhdoau0ir.apps.googleusercontent.com"));
        google->setClientIdentifierSharedKey(QStringLiteral("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"));

        auto replyHandler = new QOAuthHttpServerReplyHandler(1337, this);
        google->setReplyHandler(replyHandler);

        // Use requestedScopeTokens property and ensure 'scope' is present
        QVariant scopeVar = QVariant::fromValue(QStringList{ QStringLiteral("https://www.googleapis.com/auth/drive.readonly") });
        google->setProperty("requestedScopeTokens", scopeVar);

        google->setModifyParametersFunction([](QAbstractOAuth::Stage stage, QMultiMap<QString, QVariant> *parameters) {
            if (!parameters) return;
            if (stage == QAbstractOAuth::Stage::RequestingAuthorization) {
                parameters->insert("access_type", "offline");
                parameters->insert("prompt", "consent");
                if (!parameters->contains("scope")) {
                    parameters->insert("scope", QStringLiteral("https://www.googleapis.com/auth/drive.readonly"));
                }
            }
        });

        connect(google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, [](const QUrl &url) {
            qDebug() << "Authorization URL:" << url.toString();
            QDesktopServices::openUrl(url);
        });

        connect(google,
                QOverload<QAbstractOAuth::Error>::of(&QAbstractOAuth::requestFailed),
                this,
                [this](QAbstractOAuth::Error err) {
                    Q_UNUSED(err);
                    emit status(QStringLiteral("OAuth Fehler"));
                    qWarning() << "OAuth Fehler, code:" << int(err);
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
            qint64 expiresAt = QDateTime::currentSecsSinceEpoch() + 3600;
            saveTokens(at, rt, expiresAt);
            emit status(QStringLiteral("OAuth: Zugriff gewährt"));
        });

        connect(google, &QOAuth2AuthorizationCodeFlow::tokenChanged, this, [this]() {
            qDebug() << "Token changed, length:" << google->token().length();
        });
    }

    void setupWatcher() {
        watcher = new QFileSystemWatcher(this);
        QDir d(syncPath);
        if (!d.exists()) d.mkpath(".");
        watcher->addPath(syncPath);
        connect(watcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString&) {
            emit status(QStringLiteral("Lokale Änderung erkannt, starte Reconciliation"));
            QMetaObject::invokeMethod(this, "startDropboxSync", Qt::QueuedConnection);
        });
    }

    void saveTokens(const QString &accessToken, const QString &refreshToken, qint64 expiresAt) {
        QJsonObject obj;
        obj["access_token"] = accessToken;
        obj["refresh_token"] = refreshToken;
        obj["expires_at"] = QString::number(expiresAt);
        QFile f(tokenFilePath);
        if (f.open(QIODevice::WriteOnly)) {
            f.write(QJsonDocument(obj).toJson(QJsonDocument::Compact));
            f.close();
            emit status(QStringLiteral("Tokens gespeichert"));
        } else {
            emit status(QStringLiteral("Fehler beim Speichern der Tokens"));
        }
    }

    bool loadTokens(QString &accessToken, QString &refreshToken, qint64 &expiresAt) {
        QFile f(tokenFilePath);
        if (!f.exists()) return false;
        if (!f.open(QIODevice::ReadOnly)) return false;
        QByteArray data = f.readAll();
        f.close();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) return false;
        QJsonObject obj = doc.object();
        accessToken = obj.value("access_token").toString();
        refreshToken = obj.value("refresh_token").toString();
        expiresAt = obj.value("expires_at").toString().toLongLong();
        return true;
    }

    QString currentAccessToken() {
        QMutexLocker locker(&tokenMutex);
        if (google) return google->token();
        QString accessToken, refreshToken;
        qint64 expiresAt = 0;
        if (loadTokens(accessToken, refreshToken, expiresAt)) return accessToken;
        return QString();
    }

    void scanLocalState() {
        localIndex.clear();
        QDir root(syncPath);
        if (!root.exists()) {
            emit status(QStringLiteral("Sync-Ordner existiert nicht, erstelle..."));
            QDir().mkpath(syncPath);
        }
        QList<QDir> stack;
        stack.append(root);
        while (!stack.isEmpty()) {
            QDir d = stack.takeLast();
            QFileInfoList list = d.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
            for (const QFileInfo &fi : list) {
                QString rel = QDir(syncPath).relativeFilePath(fi.absoluteFilePath());
                QJsonObject meta;
                meta["isDir"] = fi.isDir();
                meta["size"] = QString::number(fi.size());
                meta["mtime"] = QString::number(fi.lastModified().toSecsSinceEpoch());
                localIndex.insert(rel, meta);
                if (fi.isDir()) stack.append(QDir(fi.absoluteFilePath()));
            }
        }
        emit status(QStringLiteral("Lokaler Zustand gescannt (%1 Einträge)").arg(localIndex.size()));
    }

    // Rekonstruiert den vollständigen Pfad für eine gegebene Datei-ID
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

            if (!idIndex.contains(cur)) {
                break;
            }
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
        qDebug() << "buildFullPathForId:" << id << "->" << fullPath;
        return fullPath;
    }

    void buildLocalFoldersFromIndex() {
        emit status(QStringLiteral("Erzeuge lokale Ordnerstruktur..."));
        for (auto it = idIndex.constBegin(); it != idIndex.constEnd(); ++it) {
            QJsonObject meta = it.value();
            QString mime = meta.value("mimeType").toString();
            if (mime == QStringLiteral("application/vnd.google-apps.folder")) {
                QString rel = meta.value("name").toString();
                if (rel.isEmpty()) continue;
                QString localDir = QDir(syncPath).filePath(rel);
                QDir d;
                if (!d.exists(localDir)) d.mkpath(localDir);
            }
        }
        emit status(QStringLiteral("Ordnerstruktur erstellt"));
    }

    // reconcileAndSync mit Pfadrekonstruktion, Duplikatvermeidung und Debug-Ausgaben
    void reconcileAndSync() {
        emit status(QStringLiteral("Vergleiche lokal/remote und synchronisiere..."));

        QHash<QString, QJsonObject> remotePathIndex;
        for (auto it = idIndex.constBegin(); it != idIndex.constEnd(); ++it) {
            QString id = it.key();
            QJsonObject meta = it.value();
            QString fullPath = buildFullPathForId(id);
            if (fullPath.isEmpty()) {
                fullPath = meta.value("name").toString();
                if (fullPath.isEmpty()) fullPath = id;
            }
            meta["__id"] = id;
            QString compositeKey = QStringLiteral("%1::%2").arg(fullPath, id);
            remotePathIndex.insert(compositeKey, meta);
            qDebug() << "remotePathIndex insert:" << compositeKey;
        }

        QSet<QString> allPaths;
        for (const QString &p : localIndex.keys()) allPaths.insert(p);
        for (const QString &composite : remotePathIndex.keys()) {
            QString pathPart = composite.section("::", 0, 0);
            allPaths.insert(pathPart);
        }

        QJsonArray downloadQueue;
        qint64 downloadTotalBytes = 0;
        QSet<QString> queuedIds;

        for (const QString &rel : allPaths) {
            QList<QJsonObject> remotesForPath;
            for (auto it = remotePathIndex.constBegin(); it != remotePathIndex.constEnd(); ++it) {
                QString compositeKey = it.key();
                QString pathPart = compositeKey.section("::", 0, 0);
                if (pathPart == rel) remotesForPath.append(it.value());
            }

            bool hasLocal = localIndex.contains(rel);
            bool hasRemote = !remotesForPath.isEmpty();

            if (hasLocal && !hasRemote) {
                QJsonObject lmeta = localIndex.value(rel);
                if (lmeta.value("isDir").toBool()) {
                    emit status(QStringLiteral("Remote-Ordner erstellen (Platzhalter): %1").arg(rel));
                    continue;
                }
                emit status(QStringLiteral("Upload lokal->remote (Platzhalter): %1").arg(rel));
            } else if (!hasLocal && hasRemote) {
                for (const QJsonObject &rmeta : remotesForPath) {
                    QString rid = rmeta.value("__id").toString();
                    if (rid.isEmpty()) rid = rmeta.value("id").toString();
                    if (rid.isEmpty()) continue;
                    if (queuedIds.contains(rid)) {
                        qDebug() << "Skipping already queued id:" << rid << "path:" << rel;
                        continue;
                    }

                    qDebug() << "Queueing id:" << rid << "path:" << rel;
                    emit status(QStringLiteral("Download remote->lokal: %1 (id=%2)").arg(rel, rid));
                    QJsonObject copy = rmeta;
                    if (!copy.contains("size")) copy["size"] = QString("0");
                    downloadQueue.append(copy);
                    queuedIds.insert(rid);

                    bool ok = false;
                    qint64 s = copy.value("size").toString().toLongLong(&ok);
                    if (ok) downloadTotalBytes += s;
                }
            } else if (hasLocal && hasRemote) {
                QJsonObject lmeta = localIndex.value(rel);

                QJsonObject newestRemote;
                qint64 newestTime = 0;
                for (const QJsonObject &rmeta : remotesForPath) {
                    qint64 rmtime = 0;
                    if (rmeta.contains("modifiedTime")) {
                        rmtime = QDateTime::fromString(rmeta.value("modifiedTime").toString(), Qt::ISODate).toSecsSinceEpoch();
                    }
                    if (rmtime > newestTime) {
                        newestTime = rmtime;
                        newestRemote = rmeta;
                    }
                }
                if (newestRemote.isEmpty()) continue;

                if (lmeta.value("isDir").toBool() || newestRemote.value("mimeType").toString() == QStringLiteral("application/vnd.google-apps.folder")) {
                    continue;
                }

                qint64 lmtime = lmeta.value("mtime").toString().toLongLong();
                qint64 rmtime = 0;
                if (newestRemote.contains("modifiedTime")) {
                    rmtime = QDateTime::fromString(newestRemote.value("modifiedTime").toString(), Qt::ISODate).toSecsSinceEpoch();
                }

                if (lmtime > rmtime + 2) {
                    emit status(QStringLiteral("Konflikt: lokale Version neuer, upload (Platzhalter): %1").arg(rel));
                } else if (rmtime > lmtime + 2) {
                    QString rid = newestRemote.value("__id").toString();
                    if (rid.isEmpty()) rid = newestRemote.value("id").toString();
                    if (!queuedIds.contains(rid)) {
                        qDebug() << "Queueing (conflict) id:" << rid << "path:" << rel;
                        emit status(QStringLiteral("Konflikt: remote Version neuer, download: %1 (id=%2)").arg(rel, rid));
                        QJsonObject copy = newestRemote;
                        if (!copy.contains("size")) copy["size"] = QString("0");
                        downloadQueue.append(copy);
                        queuedIds.insert(rid);
                        bool ok = false;
                        qint64 s = copy.value("size").toString().toLongLong(&ok);
                        if (ok) downloadTotalBytes += s;
                    } else {
                        qDebug() << "Skipping already queued conflict id:" << rid << "path:" << rel;
                    }
                } else {
                    // nothing
                }
            }
        }

        if (downloadQueue.isEmpty()) {
            emit status(QStringLiteral("Keine Downloads erforderlich"));
            emit overallProgress(100);
            emit etaUpdated(QStringLiteral("0s"));
            return;
        }

        DownloadWorker *dw = new DownloadWorker();
        QThread *dt = new QThread();
        dw->moveToThread(dt);

        connect(dt, &QThread::started, dw, [dw, downloadQueue, this]() {
            QString token = currentAccessToken();
            dw->startDownloads(downloadQueue, token, syncPath);
        });

        connect(dw, &DownloadWorker::downloadStatus, this, &DriveSync::onDownloadStatus);
        connect(dw, &DownloadWorker::fileProgress, this, &DriveSync::onDownloadFileProgress);
        connect(dw, &DownloadWorker::overallProgress, this, &DriveSync::onDownloadOverall);
        connect(dw, &DownloadWorker::etaUpdated, this, &DriveSync::onDownloadEta);
        connect(dw, &DownloadWorker::downloadsFinished, this, &DriveSync::onDownloadsFinished);

        connect(dw, &DownloadWorker::downloadsFinished, dw, &QObject::deleteLater);
        connect(dw, &DownloadWorker::downloadsFinished, dt, &QThread::quit);
        connect(dt, &QThread::finished, dt, &QThread::deleteLater);

        dt->start();
    }

    void uploadLocalChange(const QString &relPath) {
        QString abs = QDir::homePath() + "/google_drive";
        abs = QDir(abs).filePath(relPath);
        QFile f(abs);
        if (!f.exists()) {
            emit status(QStringLiteral("Upload fehlgeschlagen, Datei nicht gefunden: %1").arg(relPath));
            return;
        }
        emit status(QStringLiteral("Upload (Platzhalter) für %1 abgeschlossen").arg(relPath));
    }
};

// LogWindow (UI)
class LogWindow : public QWidget {
    Q_OBJECT
public:
    explicit LogWindow(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle(QStringLiteral("DriveSync"));
        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
        setFixedSize(480, 320);

        QVBoxLayout *l = new QVBoxLayout(this);

        QHBoxLayout *topRow = new QHBoxLayout();
        overallBar = new QProgressBar(this);
        overallBar->setRange(0, 100);
        overallBar->setValue(0);
        overallBar->setTextVisible(true);
        etaLabel = new QLabel(QStringLiteral("ETA: -"), this);
        topRow->addWidget(overallBar, 3);
        topRow->addWidget(etaLabel, 1);

        fileLabel = new QLabel(QStringLiteral("Keine Datei aktiv"), this);
        fileBar = new QProgressBar(this);
        fileBar->setRange(0, 100);
        fileBar->setValue(0);

        logEdit = new QTextEdit(this);
        logEdit->setReadOnly(true);
        logEdit->setLineWrapMode(QTextEdit::NoWrap);
        logEdit->setAcceptRichText(false);

        l->addLayout(topRow);
        l->addWidget(fileLabel);
        l->addWidget(fileBar);
        l->addWidget(logEdit, 1);
    }

public slots:
    void onOverallProgress(int p) { overallBar->setValue(p); }
    void onFileProgress(const QString &name, qint64 rec, qint64 tot) {
        fileLabel->setText(name);
        if (tot > 0) fileBar->setValue(int((rec * 100) / tot));
        else fileBar->setValue((fileBar->value() + 10) % 100);
    }
    void onStatus(const QString &s) {
        QString ts = QDateTime::currentDateTime().toString(Qt::ISODate);
        logEdit->append(QString("[%1] %2").arg(ts, s));
        QTextCursor c = logEdit->textCursor();
        c.movePosition(QTextCursor::End);
        logEdit->setTextCursor(c);
    }
    void onEtaUpdated(const QString &eta) {
        etaLabel->setText(QStringLiteral("ETA: %1").arg(eta));
    }

private:
    QProgressBar *overallBar;
    QLabel *etaLabel;
    QLabel *fileLabel;
    QProgressBar *fileBar;
    QTextEdit *logEdit;
};

// Helper functions
static QString computeConfigDir() {
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    if (configDir.isEmpty()) configDir = QDir::homePath() + "/.config";
    return QDir(configDir).filePath("DriveSync");
}
static bool removePathRecursive(const QString &path) {
    QDir d(path);
    if (!d.exists()) return true;
    return d.removeRecursively();
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    qRegisterMetaType<QJsonArray>("QJsonArray");
    qRegisterMetaType<QJsonObject>("QJsonObject");
    qRegisterMetaType<QList<QJsonObject>>("QList<QJsonObject>");

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("DriveSync"));
    parser.addHelpOption();
    QCommandLineOption resetOpt(QStringList() << "r" << "reset-config",
                                QStringLiteral("Löscht die Konfigurationsdateien (tokens.json) und beendet das Programm."));
    QCommandLineOption wipeOpt(QStringList() << "w" << "wipe-all",
                               QStringLiteral("Löscht Konfiguration und den Sync-Ordner (~/google_drive) und beendet das Programm."));
    parser.addOption(resetOpt);
    parser.addOption(wipeOpt);
    parser.process(a);

    if (parser.isSet(resetOpt) || parser.isSet(wipeOpt)) {
        QString cfgDir = computeConfigDir();
        QString tokenFile = QDir(cfgDir).filePath("tokens.json");
        bool ok = true;
        if (QFile::exists(tokenFile)) ok &= QFile::remove(tokenFile);
        ok &= removePathRecursive(cfgDir);
        if (parser.isSet(wipeOpt)) {
            QString syncPath = QDir::homePath() + "/google_drive";
            ok &= removePathRecursive(syncPath);
        }
        if (ok) qInfo() << "DriveSync: Konfigurationsdaten erfolgreich gelöscht.";
        else qWarning() << "DriveSync: Fehler beim Löschen einiger Dateien/Ordner. Prüfen Sie Berechtigungen.";
        return 0;
    }

    DriveSync *sync = new DriveSync(nullptr);

    LogWindow *win = new LogWindow();
    win->show();

    QSystemTrayIcon *tray = new QSystemTrayIcon(QIcon::fromTheme("folder-sync"));
    QMenu *menu = new QMenu();
    QAction *showAction = new QAction(QStringLiteral("Status anzeigen"), menu);
    QAction *hideAction = new QAction(QStringLiteral("Fenster verbergen"), menu);
    QAction *quitAction = new QAction(QStringLiteral("Beenden"), menu);
    menu->addAction(showAction);
    menu->addAction(hideAction);
    menu->addSeparator();
    menu->addAction(quitAction);
    tray->setContextMenu(menu);
    tray->setToolTip(QStringLiteral("DriveSync - synchronisiert automatisch"));
    tray->show();

    QObject::connect(showAction, &QAction::triggered, win, [win]() { win->show(); win->raise(); win->activateWindow(); });
    QObject::connect(hideAction, &QAction::triggered, win, [win]() { win->hide(); });
    QObject::connect(quitAction, &QAction::triggered, &a, [&a]() { a.quit(); });

    QObject::connect(sync, &DriveSync::overallProgress, win, &LogWindow::onOverallProgress);
    QObject::connect(sync, &DriveSync::fileProgress, win, &LogWindow::onFileProgress);
    QObject::connect(sync, &DriveSync::status, win, &LogWindow::onStatus);
    QObject::connect(sync, &DriveSync::etaUpdated, win, &LogWindow::onEtaUpdated);

    QObject::connect(sync, &DriveSync::status, &a, [&](const QString &s) {
        if (tray->isVisible()) tray->showMessage(QStringLiteral("DriveSync"), s, QSystemTrayIcon::Information, 3000);
    });

    int ret = a.exec();
    return ret;
}

#include "main.moc"
