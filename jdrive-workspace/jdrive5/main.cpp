// main.cpp
// DriveSync - Ordnerstruktur rekonstruieren und alle Dateien herunterladen (Qt6)
// Zwei-Wege-Synchronisation (Dropbox-ähnlich, automatischer Start, keine Buttons / keine Benutzerinteraktion)
// Kompilieren: qmake/CMake mit Qt6 (Network, NetworkAuth, Widgets)
// qmake: QT += core gui network networkauth widgets

#include <QApplication>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QFile>
#include <QDir>
#include <QFileSystemWatcher>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QTimer>
#include <QUrlQuery>
#include <QMutex>
#include <QDebug>
#include <QDateTime>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QDesktopServices>
#include <QFileInfo>

class DriveSync : public QObject {
    Q_OBJECT
public:
    explicit DriveSync(QObject *parent = nullptr) : QObject(parent) {
        manager = new QNetworkAccessManager(this);
        setupLocalDirectory();
        setupOAuth();
        setupWatcher();

        // Wenn Login erfolgreich, startet automatisch der Sync
        connect(google, &QOAuth2AuthorizationCodeFlow::granted, this, &DriveSync::startDropboxSync);

        // Wenn bereits Token vorhanden, sofort starten
        QString accessToken, refreshToken;
        qint64 expiresAt = 0;
        if (loadTokens(accessToken, refreshToken, expiresAt)) {
            emitStatus(QStringLiteral("Gefundene Tokens geladen, starte Sync automatisch"));
            // set tokens into google object if possible
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
            if (!refreshToken.isEmpty()) google->setRefreshToken(refreshToken);
            if (!accessToken.isEmpty()) google->setToken(accessToken);
#else
            if (!refreshToken.isEmpty()) google->setProperty("refreshToken", refreshToken);
            if (!accessToken.isEmpty()) google->setProperty("accessToken", accessToken);
#endif
            // Try to refresh tokens if refresh token exists
            if (!refreshToken.isEmpty()) {
                QMetaObject::invokeMethod(google, "refreshTokens", Qt::QueuedConnection);
                emitStatus(QStringLiteral("Erneuere Access-Token"));
            }
            QTimer::singleShot(0, this, &DriveSync::startDropboxSync);
        } else {
            // No tokens: start OAuth flow (this will open browser once)
            emitStatus(QStringLiteral("Keine Tokens gefunden, starte Login (nur einmalige Interaktion erforderlich)"));
            google->grant();
        }
    }

signals:
    void overallProgress(int percent);
    void fileProgress(const QString &fileName, qint64 received, qint64 total);
    void status(const QString &s);

private:
    QOAuth2AuthorizationCodeFlow *google = nullptr;
    QNetworkAccessManager *manager = nullptr;
    QFileSystemWatcher *watcher = nullptr;
    QString syncPath;
    QString tokenFilePath;
    QMutex tokenMutex;

    // Metadata index
    QHash<QString, QJsonObject> idIndex;

    // Local index: relative path -> meta (isDir, size, mtime)
    QHash<QString, QJsonObject> localIndex;

    // Download queue state (member-based)
    QList<QJsonObject> downloadQueue;
    qint64 downloadTotalBytes = 0;
    qint64 downloadDownloadedBytes = 0;
    bool downloadHaveSizes = false;
    int downloadIndex = 0;
    int downloadTotalCount = 0;
    QHash<QString, QString> pathMemo;

    // --- Setup methods ---
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

        // Client-ID und Secret anpassen
        google->setClientIdentifier("487363744399-of79l9lg2rqigjvgq9meqlreo1iqn9a3.apps.googleusercontent.com");
        google->setClientIdentifierSharedKey("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");

        // Scope: drive.readonly (setProperty vermeidet Deprecation)
        QVariant scopeVar = QVariant::fromValue(QStringList{QStringLiteral("https://www.googleapis.com/auth/drive.readonly")});
        google->setProperty("requestedScopeTokens", scopeVar);

        auto replyHandler = new QOAuthHttpServerReplyHandler(1337, this);
        google->setReplyHandler(replyHandler);

        google->setModifyParametersFunction([](QAbstractOAuth::Stage stage, QMultiMap<QString, QVariant> *parameters) {
            if (!parameters) return;
            if (stage == QAbstractOAuth::Stage::RequestingAuthorization) {
                parameters->insert("access_type", "offline");
                parameters->insert("prompt", "consent");
            }
        });

        connect(google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, [](const QUrl &url) {
            QDesktopServices::openUrl(url);
        });

        connect(google, &QOAuth2AuthorizationCodeFlow::granted, this, [this]() {
            emitStatus(QStringLiteral("Login erfolgreich"));
            QString accessToken = google->token();
            QString refreshToken;
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
            refreshToken = google->refreshToken();
#else
            QVariant rt = google->property("refreshToken");
            if (rt.isValid()) refreshToken = rt.toString();
#endif
            qint64 expiresAt = QDateTime::currentSecsSinceEpoch() + 3600;
            saveTokens(accessToken, refreshToken, expiresAt);
        });
    }

    void setupWatcher() {
        watcher = new QFileSystemWatcher(this);
        // watch the root sync folder for changes; if it doesn't exist yet, create it
        QDir d(syncPath);
        if (!d.exists()) d.mkpath(".");
        watcher->addPath(syncPath);
        connect(watcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString&) {
            emitStatus(QStringLiteral("Lokale Änderung erkannt, starte Reconciliation"));
            // Debounce and trigger full reconciliation
            QTimer::singleShot(300, this, &DriveSync::startDropboxSync);
        });
    }

    // --- Token storage ---
    void saveTokens(const QString &accessToken, const QString &refreshToken, qint64 expiresAt) {
        QJsonObject obj;
        obj["access_token"] = accessToken;
        obj["refresh_token"] = refreshToken;
        obj["expires_at"] = QString::number(expiresAt);
        QFile f(tokenFilePath);
        if (f.open(QIODevice::WriteOnly)) {
            f.write(QJsonDocument(obj).toJson(QJsonDocument::Compact));
            f.close();
            emitStatus(QStringLiteral("Tokens gespeichert"));
        } else {
            emitStatus(QStringLiteral("Fehler beim Speichern der Tokens"));
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

    // --- Dropbox-like two-way sync entry point ---
    void startDropboxSync() {
        emit overallProgress(0);
        emitStatus(QStringLiteral("Starte Zwei-Wege-Synchronisation..."));
        // 1) Scan local state
        scanLocalState();
        // 2) Fetch remote metadata (asynchronously). After fetch completes, reconcileAndSync() will be called.
        fetchAllMetadataAndDownload();
    }

    // --- Metadata retrieval (member-based recursion) ---
    void fetchAllMetadataAndDownload() {
        emit overallProgress(0);
        emitStatus(QStringLiteral("Hole Metadaten von Google Drive..."));
        QString token = currentAccessToken();
        if (token.isEmpty()) {
            emitStatus(QStringLiteral("Kein Access-Token. Bitte anmelden."));
            return;
        }

        idIndex.clear();

        QUrl base("https://www.googleapis.com/drive/v3/files");
        QUrlQuery q;
        q.addQueryItem("pageSize", "1000");
        // include modifiedTime and md5Checksum to improve conflict detection
        q.addQueryItem("fields", "nextPageToken, files(id, name, mimeType, parents, size, modifiedTime, md5Checksum)");
        base.setQuery(q);

        fetchMetadataPage(base, token);
    }

    void fetchMetadataPage(const QUrl &pageUrl, const QString &token) {
        QNetworkRequest req(pageUrl);
        req.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());
        req.setHeader(QNetworkRequest::UserAgentHeader, "DriveSync-Qt");

        QNetworkReply *reply = manager->get(req);
        connect(reply, &QNetworkReply::finished, this, [this, reply, token]() {
            if (reply->error() != QNetworkReply::NoError) {
                emitStatus(QStringLiteral("Fehler beim Abrufen der Metadaten: ") + reply->errorString());
                reply->deleteLater();
                emit overallProgress(0);
                return;
            }

            QByteArray data = reply->readAll();
            reply->deleteLater();

            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (!doc.isObject()) {
                emitStatus(QStringLiteral("Ungültige API-Antwort (Metadaten)"));
                emit overallProgress(0);
                return;
            }
            QJsonObject obj = doc.object();
            QJsonArray files = obj.value("files").toArray();

            for (const QJsonValue &v : files) {
                if (!v.isObject()) continue;
                QJsonObject meta = v.toObject();
                QString id = meta.value("id").toString();
                if (!id.isEmpty()) idIndex.insert(id, meta);
            }

            QString nextPageToken = obj.value("nextPageToken").toString();
            if (!nextPageToken.isEmpty()) {
                QUrl next("https://www.googleapis.com/drive/v3/files");
                QUrlQuery q;
                q.addQueryItem("pageSize", "1000");
                q.addQueryItem("fields", "nextPageToken, files(id, name, mimeType, parents, size, modifiedTime, md5Checksum)");
                q.addQueryItem("pageToken", nextPageToken);
                next.setQuery(q);

                QTimer::singleShot(150, this, [this, next, token]() { fetchMetadataPage(next, token); });
            } else {
                emitStatus(QStringLiteral("Metadaten vollständig (%1 Objekte)").arg(idIndex.size()));
                buildLocalFoldersFromIndex();
                // After metadata fully loaded, reconcile local and remote
                QTimer::singleShot(0, this, &DriveSync::reconcileAndSync);
            }
        });
    }

    // --- Build paths and folders ---
    QString buildPathForId(const QString &id) {
        if (pathMemo.contains(id)) return pathMemo.value(id);
        if (!idIndex.contains(id)) {
            pathMemo.insert(id, QString());
            return QString();
        }
        QJsonObject meta = idIndex.value(id);
        QString name = meta.value("name").toString();
        QJsonArray parents = meta.value("parents").toArray();

        if (parents.isEmpty()) {
            pathMemo.insert(id, name);
            return name;
        }

        QString parentId = parents.first().toString();
        if (parentId.isEmpty() || parentId == "root") {
            pathMemo.insert(id, name);
            return name;
        }

        if (parentId == id) {
            pathMemo.insert(id, name);
            return name;
        }

        QString parentPath = buildPathForId(parentId);
        QString full;
        if (parentPath.isEmpty()) full = name;
        else full = parentPath + "/" + name;
        pathMemo.insert(id, full);
        return full;
    }

    void buildLocalFoldersFromIndex() {
        emitStatus(QStringLiteral("Erzeuge lokale Ordnerstruktur..."));

        pathMemo.clear();
        QList<QString> folderIds;
        for (auto it = idIndex.constBegin(); it != idIndex.constEnd(); ++it) {
            QJsonObject meta = it.value();
            QString mime = meta.value("mimeType").toString();
            if (mime == QStringLiteral("application/vnd.google-apps.folder")) {
                folderIds.append(it.key());
            }
        }

        for (const QString &fid : folderIds) {
            QString relPath = buildPathForId(fid);
            if (relPath.isEmpty()) continue;
            QString localDir = QDir(syncPath).filePath(relPath);
            QDir d;
            if (!d.exists(localDir)) d.mkpath(localDir);
        }

        emitStatus(QStringLiteral("Ordnerstruktur erstellt"));
    }

    // --- Scan local filesystem into localIndex ---
    void scanLocalState() {
        localIndex.clear();
        QDir root(syncPath);
        if (!root.exists()) {
            emitStatus(QStringLiteral("Sync-Ordner existiert nicht, erstelle..."));
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
        emitStatus(QStringLiteral("Lokaler Zustand gescannt (%1 Einträge)").arg(localIndex.size()));
    }

    // --- Reconcile local and remote and schedule uploads/downloads ---
    void reconcileAndSync() {
        emitStatus(QStringLiteral("Vergleiche lokal/remote und synchronisiere..."));

        // Build remote path index: relPath -> meta
        QHash<QString, QJsonObject> remotePathIndex;
        pathMemo.clear();
        for (auto it = idIndex.constBegin(); it != idIndex.constEnd(); ++it) {
            QJsonObject meta = it.value();
            QString id = it.key();
            QString rel = buildPathForId(id);
            if (rel.isEmpty()) continue;
            rel.replace('\\', '/');
            meta["__id"] = id;
            remotePathIndex.insert(rel, meta);
        }

        // Collect all paths
        QSet<QString> allPaths;
        for (const QString &p : localIndex.keys()) allPaths.insert(p);
        for (const QString &p : remotePathIndex.keys()) allPaths.insert(p);

        // Clear download queue; we'll append needed downloads
        downloadQueue.clear();

        for (const QString &rel : allPaths) {
            bool hasLocal = localIndex.contains(rel);
            bool hasRemote = remotePathIndex.contains(rel);

            if (hasLocal && !hasRemote) {
                // Local only -> upload (placeholder)
                QJsonObject lmeta = localIndex.value(rel);
                if (lmeta.value("isDir").toBool()) {
                    // create remote folder: not implemented (placeholder)
                    emitStatus(QStringLiteral("Remote-Ordner erstellen (Platzhalter): %1").arg(rel));
                    continue;
                }
                emitStatus(QStringLiteral("Upload lokal->remote (Platzhalter): %1").arg(rel));
                uploadLocalChange(rel);
            } else if (!hasLocal && hasRemote) {
                // Remote only -> download
                QJsonObject rmeta = remotePathIndex.value(rel);
                emitStatus(QStringLiteral("Download remote->lokal: %1").arg(rel));
                if (!rmeta.contains("size")) rmeta["size"] = QString("0");
                downloadQueue.append(rmeta);
            } else if (hasLocal && hasRemote) {
                // Both exist -> conflict check
                QJsonObject lmeta = localIndex.value(rel);
                QJsonObject rmeta = remotePathIndex.value(rel);
                if (lmeta.value("isDir").toBool() || rmeta.value("mimeType").toString() == QStringLiteral("application/vnd.google-apps.folder")) {
                    continue; // directories: nothing to do
                }
                qint64 lmtime = lmeta.value("mtime").toString().toLongLong();
                qint64 rmtime = 0;
                if (rmeta.contains("modifiedTime")) {
                    rmtime = QDateTime::fromString(rmeta.value("modifiedTime").toString(), Qt::ISODate).toSecsSinceEpoch();
                }
                if (lmtime > rmtime + 2) {
                    // local newer -> upload
                    emitStatus(QStringLiteral("Konflikt: lokale Version neuer, upload (Platzhalter): %1").arg(rel));
                    uploadLocalChange(rel);
                } else if (rmtime > lmtime + 2) {
                    // remote newer -> download
                    emitStatus(QStringLiteral("Konflikt: remote Version neuer, download: %1").arg(rel));
                    downloadQueue.append(rmeta);
                } else {
                    // roughly equal -> nothing
                }
            }
        }

        // Start downloads if any
        if (!downloadQueue.isEmpty()) {
            downloadTotalCount = downloadQueue.size();
            processNextDownload();
        } else {
            emitStatus(QStringLiteral("Keine Downloads erforderlich"));
            emit overallProgress(100);
        }
    }

    // --- Upload placeholder (resumable upload recommended for production) ---
    void uploadLocalChange(const QString &relPath) {
        QString abs = QDir(syncPath).filePath(relPath);
        QFile f(abs);
        if (!f.exists()) {
            emitStatus(QStringLiteral("Upload fehlgeschlagen, Datei nicht gefunden: %1").arg(relPath));
            return;
        }
        // TODO: Implement resumable upload to Drive (https://developers.google.com/drive/api/v3/manage-uploads)
        // For now: log and treat as no-op (or implement a simple metadata-only placeholder)
        emitStatus(QStringLiteral("Upload (Platzhalter) für %1 abgeschlossen").arg(relPath));
    }

    // applyRemoteChange: append to download queue and start if idle
    void applyRemoteChange(const QJsonObject &meta) {
        QJsonObject m = meta;
        if (!m.contains("size")) m["size"] = QString("0");
        downloadQueue.append(m);
        if (downloadQueue.size() == 1) processNextDownload();
    }

    // --- Member-based download processing (no local std::function captures) ---
    void processNextDownload() {
        if (downloadQueue.isEmpty()) {
            emitStatus(QStringLiteral("Alle Dateien heruntergeladen"));
            emit overallProgress(100);
            return;
        }

        QJsonObject meta = downloadQueue.takeFirst();
        QString id = meta.value("id").toString();
        QString name = meta.value("name").toString();
        bool okSize = false;
        qint64 size = meta.value("size").toString().toLongLong(&okSize);

        // Determine relative path
        QString relPath;
        QJsonArray parents = meta.value("parents").toArray();
        if (!parents.isEmpty()) {
            QString parentId = parents.first().toString();
            if (!parentId.isEmpty() && parentId != "root") {
                QString parentRel = buildPathForId(parentId);
                if (!parentRel.isEmpty()) relPath = parentRel + "/" + name;
                else relPath = name;
            } else {
                relPath = name;
            }
        } else {
            relPath = name;
        }

        // Sanitize and ensure directories
        QString safeRel = relPath;
        safeRel.replace('\\', '_');
        QFileInfo fi(QDir(syncPath).filePath(safeRel));
        QDir dir;
        QString dirPath = fi.path();
        if (!dir.exists(dirPath)) dir.mkpath(dirPath);

        QString outPath = QDir(syncPath).filePath(safeRel);

        QUrl url(QString("https://www.googleapis.com/drive/v3/files/%1").arg(id));
        QUrlQuery q; q.addQueryItem("alt", "media"); url.setQuery(q);

        QString token = currentAccessToken();
        if (token.isEmpty()) { emitStatus(QStringLiteral("Kein Access-Token")); return; }

        QNetworkRequest req(url);
        req.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());
        req.setHeader(QNetworkRequest::UserAgentHeader, "DriveSync-Qt");

        QNetworkReply *reply = manager->get(req);

        QFile *outFile = new QFile(outPath + ".part");
        if (!outFile->open(QIODevice::WriteOnly)) {
            emitStatus(QStringLiteral("Fehler beim Öffnen: ") + outPath);
            delete outFile;
            reply->abort();
            reply->deleteLater();
            // schedule next
            QTimer::singleShot(50, this, &DriveSync::processNextDownload);
            return;
        }

        // readyRead: stream to file
        connect(reply, &QNetworkReply::readyRead, this, [reply, outFile]() {
            QByteArray chunk = reply->readAll();
            if (!chunk.isEmpty()) outFile->write(chunk);
        });

        // progress
        connect(reply, &QNetworkReply::downloadProgress, this, [this, name](qint64 rec, qint64 tot) {
            emit fileProgress(name, rec, tot);
        });

        // finished
        connect(reply, &QNetworkReply::finished, this, [this, reply, outFile, outPath, okSize, size, name]() mutable {
            if (reply->error() != QNetworkReply::NoError) {
                emitStatus(QStringLiteral("Fehler beim Herunterladen: ") + name + " - " + reply->errorString());
                outFile->close();
                outFile->remove();
                delete outFile;
                reply->deleteLater();
                QTimer::singleShot(50, this, &DriveSync::processNextDownload);
                return;
            }

            outFile->flush();
            outFile->close();

            QString finalPath = outPath;
            QFile::remove(finalPath);
            QFile::rename(outPath + ".part", finalPath);

            if (okSize && size > 0) downloadDownloadedBytes += size;
            downloadIndex++;
            int overallPercent = 0;
            if (downloadHaveSizes && downloadTotalBytes > 0) overallPercent = qBound(0, int((downloadDownloadedBytes * 100) / downloadTotalBytes), 100);
            else overallPercent = qMin(99, downloadIndex * 100 / downloadTotalCount);

            emit overallProgress(overallPercent);
            emitStatus(QStringLiteral("Heruntergeladen: ") + name);

            reply->deleteLater();
            delete outFile;
            QTimer::singleShot(50, this, &DriveSync::processNextDownload);
        });
    }

    // Helper to emit status and qDebug
    void emitStatus(const QString &s) {
        qDebug().noquote() << s;
        emit status(s);
    }
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // Create DriveSync instance; it will run automatically without UI
    DriveSync sync;

    // Keep application running; DriveSync uses signals/slots and QNetworkAccessManager
    return a.exec();
}

#include "main.moc"
