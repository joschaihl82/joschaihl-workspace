// main.cpp
// DriveSync - Download aller Dateien (seitenweise, streaming) mit Qt6
// Kompilieren: qmake/CMake mit Qt6 (Network, NetworkAuth, Widgets)
// qmake: QT += core gui network networkauth widgets

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QDesktopServices>
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
#include <functional>

class DriveSync : public QObject {
    Q_OBJECT
public:
    explicit DriveSync(QObject *parent = nullptr) : QObject(parent) {
        manager = new QNetworkAccessManager(this);
        setupLocalDirectory();
        setupOAuth();
        setupWatcher();
    }

    Q_INVOKABLE void startDownloadAll() { fetchFileListAndDownloadAll(); }

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

    void setupLocalDirectory() {
        syncPath = QDir::homePath() + "/google_drive";
        QDir d(syncPath);
        if (!d.exists()) d.mkpath(".");
        QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
        if (configDir.isEmpty()) configDir = QDir::homePath() + "/.config";
        QDir().mkpath(configDir + "/DriveSync");
        tokenFilePath = configDir + "/DriveSync/tokens.json";
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

    void setupOAuth() {
        google = new QOAuth2AuthorizationCodeFlow(this);
        google->setAuthorizationUrl(QUrl("https://accounts.google.com/o/oauth2/v2/auth"));
        google->setTokenUrl(QUrl("https://oauth2.googleapis.com/token"));

        // Client-ID und Secret anpassen
        google->setClientIdentifier("487363744399-of79l9lg2rqigjvgq9meqlreo1iqn9a3.apps.googleusercontent.com");
        google->setClientIdentifierSharedKey("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");

        // Verwende requestedScopeTokens statt setScope (vermeidet Deprecation)
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
            emit status(QStringLiteral("Login erfolgreich"));
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

        QString accessToken, refreshToken;
        qint64 expiresAt = 0;
        if (loadTokens(accessToken, refreshToken, expiresAt)) {
            if (!refreshToken.isEmpty()) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
                google->setRefreshToken(refreshToken);
#else
                google->setProperty("refreshToken", refreshToken);
#endif
                // Versuche Token-Refresh (invokeMethod als kompatibler Aufruf)
                QMetaObject::invokeMethod(google, "refreshTokens", Qt::QueuedConnection);
                emit status(QStringLiteral("Erneuere Access-Token"));
                return;
            } else if (!accessToken.isEmpty()) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
                google->setToken(accessToken);
#else
                google->setProperty("accessToken", accessToken);
#endif
                emit status(QStringLiteral("Access-Token gesetzt"));
                return;
            }
        }

        google->grant();
        emit status(QStringLiteral("Starte Login"));
    }

    void setupWatcher() {
        watcher = new QFileSystemWatcher(this);
        watcher->addPath(syncPath);
        connect(watcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString&) {
            emit status(QStringLiteral("Lokale Änderung erkannt"));
            QTimer::singleShot(300, this, &DriveSync::fetchFileListAndDownloadAll);
        });
    }

    QString currentAccessToken() {
        QMutexLocker locker(&tokenMutex);
        if (google) return google->token();
        QString accessToken, refreshToken;
        qint64 expiresAt = 0;
        if (loadTokens(accessToken, refreshToken, expiresAt)) return accessToken;
        return QString();
    }

    // Hauptfunktion: Seite für Seite abrufen und jede Seite sofort herunterladen
    void fetchFileListAndDownloadAll() {
        emit overallProgress(0);
        emit status(QStringLiteral("Hole Dateiliste von Google Drive..."));
        QString token = currentAccessToken();
        if (token.isEmpty()) {
            emit status(QStringLiteral("Kein Access-Token. Bitte anmelden."));
            return;
        }

        QUrl base("https://www.googleapis.com/drive/v3/files");
        QUrlQuery q;
        q.addQueryItem("pageSize", "500"); // moderate Seiten-Größe
        q.addQueryItem("fields", "nextPageToken, files(id, name, size)");
        base.setQuery(q);

        // Lambda: ruft eine Seite ab, lädt die Dateien dieser Seite herunter, ruft dann die nächste Seite auf
        std::function<void(const QUrl&)> fetchPage;
        fetchPage = [this, token, &fetchPage](const QUrl &pageUrl) mutable {
            QNetworkRequest req(pageUrl);
            req.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());
            req.setHeader(QNetworkRequest::UserAgentHeader, "DriveSync-Qt");

            QNetworkReply *reply = manager->get(req);
            connect(reply, &QNetworkReply::finished, this, [this, reply, &fetchPage]() mutable {
                if (reply->error() != QNetworkReply::NoError) {
                    emit status(QStringLiteral("Fehler beim Abrufen der Dateiliste: ") + reply->errorString());
                    reply->deleteLater();
                    emit overallProgress(0);
                    return;
                }

                QByteArray data = reply->readAll();
                reply->deleteLater();

                QJsonDocument doc = QJsonDocument::fromJson(data);
                if (!doc.isObject()) {
                    emit status(QStringLiteral("Ungültige API-Antwort"));
                    emit overallProgress(0);
                    return;
                }
                QJsonObject obj = doc.object();
                QJsonArray files = obj.value("files").toArray();

                // Konvertiere Seite in QList<QJsonObject>
                QList<QJsonObject> pageFiles;
                for (const QJsonValue &v : files) if (v.isObject()) pageFiles.append(v.toObject());

                // Wenn keine Dateien auf dieser Seite: direkt nächste Seite prüfen
                if (pageFiles.isEmpty()) {
                    QString nextPageToken = obj.value("nextPageToken").toString();
                    if (!nextPageToken.isEmpty()) {
                        QUrl next("https://www.googleapis.com/drive/v3/files");
                        QUrlQuery q;
                        q.addQueryItem("pageSize", "500");
                        q.addQueryItem("fields", "nextPageToken, files(id, name, size)");
                        q.addQueryItem("pageToken", nextPageToken);
                        next.setQuery(q);
                        QTimer::singleShot(150, this, [next, &fetchPage]() { fetchPage(next); });
                    } else {
                        emit status(QStringLiteral("Keine Dateien gefunden"));
                        emit overallProgress(100);
                    }
                    return;
                }

                // Lade die Dateien dieser Seite herunter; wenn fertig -> nächste Seite
                downloadFilesSequentially(pageFiles, [this, obj, &fetchPage]() {
                    QString nextPageToken = obj.value("nextPageToken").toString();
                    if (!nextPageToken.isEmpty()) {
                        QUrl next("https://www.googleapis.com/drive/v3/files");
                        QUrlQuery q;
                        q.addQueryItem("pageSize", "500");
                        q.addQueryItem("fields", "nextPageToken, files(id, name, size)");
                        q.addQueryItem("pageToken", nextPageToken);
                        next.setQuery(q);
                        QTimer::singleShot(150, this, [next, &fetchPage]() { fetchPage(next); });
                    } else {
                        emit status(QStringLiteral("Alle Seiten verarbeitet"));
                        emit overallProgress(100);
                    }
                });
            });
        };

        fetchPage(base);
    }

    // Download einer Seite (Liste von Dateien) sequentiell; ruft finishedCallback auf, wenn die Seite fertig ist.
    void downloadFilesSequentially(const QList<QJsonObject> &files, std::function<void()> finishedCallback) {
        if (files.isEmpty()) { finishedCallback(); return; }

        // Gesamtanzahl der Dateien dieser Seite (für Schätzung)
        const int totalCount = files.size();

        // Gesamtgröße der Seite (falls vorhanden) für grobe Fortschrittsabschätzung
        qint64 pageTotal = 0;
        for (const QJsonObject &f : files) {
            bool ok = false;
            qint64 s = f.value("size").toString().toLongLong(&ok);
            if (ok) pageTotal += s;
        }
        bool haveSizes = (pageTotal > 0);
        qint64 downloadedBytes = 0;
        int index = 0;

        QList<QJsonObject> queue = files;
        std::function<void()> next;
        next = [this, &queue, &downloadedBytes, pageTotal, haveSizes, &index, &next, finishedCallback, totalCount]() mutable {
            if (queue.isEmpty()) { finishedCallback(); return; }

            QJsonObject fileObj = queue.takeFirst();
            QString id = fileObj.value("id").toString();
            QString name = fileObj.value("name").toString();
            qint64 size = fileObj.value("size").toString().toLongLong();

            QString safeName = name;
            safeName.replace('/', '_');
            QString outPath = QDir(syncPath).filePath(safeName);

            QUrl url(QString("https://www.googleapis.com/drive/v3/files/%1").arg(id));
            QUrlQuery q; q.addQueryItem("alt", "media"); url.setQuery(q);

            QString token = currentAccessToken();
            if (token.isEmpty()) { emit status(QStringLiteral("Kein Access-Token")); finishedCallback(); return; }

            QNetworkRequest req(url);
            req.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());
            req.setHeader(QNetworkRequest::UserAgentHeader, "DriveSync-Qt");

            QNetworkReply *reply = manager->get(req);

            // Datei öffnen und streamen
            QFile *outFile = new QFile(outPath + ".part");
            if (!outFile->open(QIODevice::WriteOnly)) {
                emit status(QStringLiteral("Fehler beim Öffnen: ") + outPath);
                delete outFile;
                reply->abort();
                reply->deleteLater();
                QTimer::singleShot(50, this, next);
                return;
            }

            // readyRead: schreibe kontinuierlich in Datei
            connect(reply, &QNetworkReply::readyRead, this, [reply, outFile]() {
                QByteArray chunk = reply->readAll();
                if (!chunk.isEmpty()) outFile->write(chunk);
            });

            // downloadProgress: Einzeldatei-Fortschritt melden
            connect(reply, &QNetworkReply::downloadProgress, this, [this, name](qint64 rec, qint64 tot) {
                emit fileProgress(name, rec, tot);
            });

            // finished: close, rename, update counters, weiter zur nächsten Datei
            connect(reply, &QNetworkReply::finished, this, [this, reply, outFile, outPath, &downloadedBytes, size, haveSizes, pageTotal, &index, &next, name, totalCount]() mutable {
                if (reply->error() != QNetworkReply::NoError) {
                    emit status(QStringLiteral("Fehler beim Herunterladen: ") + name + " - " + reply->errorString());
                    outFile->close();
                    outFile->remove();
                    delete outFile;
                    reply->deleteLater();
                    QTimer::singleShot(50, this, next);
                    return;
                }

                // Keine großen readAll() Aufrufe mehr; readyRead hat bereits geschrieben.
                outFile->flush();
                outFile->close();

                // Atomarer Rename
                QString finalPath = outPath;
                QFile::remove(finalPath); // überschreiben falls vorhanden
                QFile::rename(outPath + ".part", finalPath);

                if (haveSizes && size > 0) downloadedBytes += size;
                index++;
                int pagePercent = 0;
                if (haveSizes && pageTotal > 0) pagePercent = qBound(0, int((downloadedBytes * 100) / pageTotal), 100);
                else pagePercent = qMin(99, index * 100 / totalCount); // Schätzung basierend auf Anzahl

                emit overallProgress(pagePercent);
                emit status(QStringLiteral("Heruntergeladen: ") + name);

                reply->deleteLater();
                delete outFile;
                QTimer::singleShot(50, this, next);
            });
        };

        next();
    }
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle(QStringLiteral("DriveSync - Download alle Dateien"));

        QWidget *c = new QWidget(this);
        QVBoxLayout *l = new QVBoxLayout(c);

        statusLabel = new QLabel(QStringLiteral("Bereit"), this);
        overallBar = new QProgressBar(this);
        overallBar->setRange(0, 100);
        overallBar->setValue(0);

        fileLabel = new QLabel(QStringLiteral("Keine Datei aktiv"), this);
        fileBar = new QProgressBar(this);
        fileBar->setRange(0, 100);
        fileBar->setValue(0);

        QPushButton *start = new QPushButton(QStringLiteral("Alle Dateien herunterladen / Login"), this);
        QPushButton *simulate = new QPushButton(QStringLiteral("Lokale Änderung simulieren"), this);

        l->addWidget(statusLabel);
        l->addWidget(overallBar);
        l->addWidget(fileLabel);
        l->addWidget(fileBar);
        l->addWidget(start);
        l->addWidget(simulate);

        setCentralWidget(c);
        resize(520, 200);

        sync = new DriveSync(this);

        connect(start, &QPushButton::clicked, this, [this]() {
            overallBar->setValue(0);
            fileBar->setValue(0);
            statusLabel->setText(QStringLiteral("Starte Download..."));
            sync->startDownloadAll();
        });

        connect(simulate, &QPushButton::clicked, this, [this]() {
            statusLabel->setText(QStringLiteral("Simuliere lokale Änderung"));
            QTimer::singleShot(100, sync, [this]() {
                QString testFile = QDir::homePath() + "/google_drive/.drivesync_test";
                QFile f(testFile);
                if (f.open(QIODevice::WriteOnly)) {
                    f.write(QDateTime::currentDateTime().toString().toUtf8());
                    f.close();
                }
            });
        });

        connect(sync, &DriveSync::overallProgress, this, &MainWindow::onOverallProgress);
        connect(sync, &DriveSync::fileProgress, this, &MainWindow::onFileProgress);
        connect(sync, &DriveSync::status, this, &MainWindow::onStatus);
    }

private slots:
    void onOverallProgress(int p) { overallBar->setValue(p); }
    void onFileProgress(const QString &name, qint64 rec, qint64 tot) {
        fileLabel->setText(name);
        if (tot > 0) fileBar->setValue(int((rec * 100) / tot));
        else fileBar->setValue((fileBar->value() + 10) % 100);
    }
    void onStatus(const QString &s) { statusLabel->setText(s); }

private:
    DriveSync *sync = nullptr;
    QProgressBar *overallBar = nullptr;
    QLabel *statusLabel = nullptr;
    QLabel *fileLabel = nullptr;
    QProgressBar *fileBar = nullptr;
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

#include "main.moc"
