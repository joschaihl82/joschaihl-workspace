// main.cpp
// DriveSync - Google Drive OAuth2 (compatible with older Qt6 NetworkAuth APIs)
// Kompilieren: qmake/CMake mit Qt6 (Network, NetworkAuth)
// qmake: QT += core gui network networkauth widgets

#include <QApplication>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QDesktopServices>
#include <QNetworkReply>
#include <QFileSystemWatcher>
#include <QDir>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QFile>
#include <QTimer>
#include <QDateTime>

class DriveSync : public QObject {
    Q_OBJECT
public:
    DriveSync(QObject *parent = nullptr) : QObject(parent) {
        setupLocalDirectory();
        setupOAuth();
        setupFileWatcher();
    }

private:
    QOAuth2AuthorizationCodeFlow *google = nullptr;
    QFileSystemWatcher *watcher = nullptr;
    QString syncPath;
    QString tokenFilePath;

    void setupLocalDirectory() {
        syncPath = QDir::homePath() + "/google_drive";
        QDir dir(syncPath);
        if (!dir.exists()) {
            dir.mkpath(".");
        }

        // Token-Datei im config-Verzeichnis
        QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
        if (configDir.isEmpty()) {
            // Fallback auf home
            configDir = QDir::homePath() + "/.config";
        }
        QDir().mkpath(configDir + "/DriveSync");
        tokenFilePath = configDir + "/DriveSync/tokens.json";
    }

    void saveTokens(const QString &accessToken, const QString &refreshToken, qint64 expiresAt) {
        QJsonObject obj;
        obj["access_token"] = accessToken;
        obj["refresh_token"] = refreshToken;
        obj["expires_at"] = QString::number(expiresAt);

        QJsonDocument doc(obj);
        QFile f(tokenFilePath);
        if (f.open(QIODevice::WriteOnly)) {
            f.write(doc.toJson(QJsonDocument::Compact));
            f.close();
            qDebug() << "Tokens gespeichert:" << tokenFilePath;
        } else {
            qWarning() << "Konnte Token-Datei nicht schreiben:" << tokenFilePath;
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

        // Google OAuth 2.0 Endpunkte
        google->setAuthorizationUrl(QUrl("https://accounts.google.com/o/oauth2/v2/auth"));
        google->setTokenUrl(QUrl("https://oauth2.googleapis.com/token"));

        // --- Hier die Client-ID und das Client-Secret setzen ---
        // Neue Werte vom Nutzer:
        // Client-ID: 487363744399-of79l9lg2rqigjvgq9meqlreo1iqn9a3.apps.googleusercontent.com
        // Client Secret: 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
        google->setClientIdentifier("487363744399-of79l9lg2rqigjvgq9meqlreo1iqn9a3.apps.googleusercontent.com");
        google->setClientIdentifierSharedKey("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");

        // Hinweis: Manche Qt-Versionen setzen PKCE intern automatisch für native desktop clients.
        // Um Kompatibilitätsfehler zu vermeiden, rufen wir keine explizite PKCE-API auf.

        // Scope setzen (deprecated in neueren Qt, aber in vielen älteren Versionen vorhanden)
        google->setScope("https://www.googleapis.com/auth/drive");

        // Reply Handler auf Port 1337
        auto replyHandler = new QOAuthHttpServerReplyHandler(1337, this);
        google->setReplyHandler(replyHandler);

        // Google-spezifische Parameter (offline access -> Refresh Token)
        google->setModifyParametersFunction([](QAbstractOAuth::Stage stage, QMultiMap<QString, QVariant> *parameters) {
            if (!parameters) return;
            if (stage == QAbstractOAuth::Stage::RequestingAuthorization) {
                parameters->insert("access_type", "offline");
                parameters->insert("prompt", "consent");
            }
        });

        // Browser öffnen
        connect(google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, [](const QUrl &url) {
            qDebug() << "Öffne Browser für Login...";
            QDesktopServices::openUrl(url);
        });

        // Wenn Token erhalten wurden
        connect(google, &QOAuth2AuthorizationCodeFlow::granted, this, [this]() {
            qDebug() << "Login erfolgreich! Zugriff auf Google Drive gewährt.";

            // Access/Refresh Token auslesen und speichern
            QString accessToken = google->token();
            QString refreshToken;

#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
            refreshToken = google->refreshToken();
#else
            QVariant rt = google->property("refreshToken");
            if (rt.isValid()) refreshToken = rt.toString();
#endif

            // Ablaufzeit: fallback 3600s
            qint64 expiresAt = QDateTime::currentSecsSinceEpoch() + 3600;

            saveTokens(accessToken, refreshToken, expiresAt);

            // Beispiel: jetzt Datei-Liste holen
            fetchFileList();
        });

        // Versuche gespeicherte Tokens zu laden und ggf. zu refreshen
        QString accessToken, refreshToken;
        qint64 expiresAt = 0;
        if (loadTokens(accessToken, refreshToken, expiresAt)) {
            qDebug() << "Gefundene Tokens geladen.";
            if (!refreshToken.isEmpty()) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
                google->setRefreshToken(refreshToken);
#else
                google->setProperty("refreshToken", refreshToken);
#endif
                // Verwende die ältere Refresh-API, die in vielen Qt-Installationen vorhanden ist
                QTimer::singleShot(0, google, &QOAuth2AuthorizationCodeFlow::refreshAccessToken);
                qDebug() << "Versuche Access-Token mit Refresh-Token zu erneuern...";
                return;
            } else if (!accessToken.isEmpty()) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
                google->setToken(accessToken);
#else
                google->setProperty("accessToken", accessToken);
#endif
                qDebug() << "Access-Token gesetzt (aus Datei).";
                // Prüfe Ablaufzeit und ggf. erneuern
                if (QDateTime::currentSecsSinceEpoch() > expiresAt - 60) {
                    if (!refreshToken.isEmpty()) {
                        QTimer::singleShot(0, google, &QOAuth2AuthorizationCodeFlow::refreshAccessToken);
                    } else {
                        google->grant();
                    }
                }
                return;
            }
        }

        // Wenn keine Tokens vorhanden oder kein Refresh möglich: starte interaktiven Flow
        google->grant();
    }

    void setupFileWatcher() {
        watcher = new QFileSystemWatcher(this);
        watcher->addPath(syncPath);
        connect(watcher, &QFileSystemWatcher::directoryChanged, this, [](const QString &path) {
            qDebug() << "Lokale Änderung erkannt im Pfad:" << path;
        });
    }

    void fetchFileList() {
        QUrl url("https://www.googleapis.com/drive/v3/files");

        // Ältere API: benutze get(), die in vielen Qt-Versionen vorhanden ist
        QNetworkReply *reply = google->get(url);
        connect(reply, &QNetworkReply::finished, this, [reply]() {
            if (reply->error() != QNetworkReply::NoError) {
                qDebug() << "API Fehler:" << reply->errorString();
                reply->deleteLater();
                return;
            }

            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (!doc.isObject()) {
                qDebug() << "Ungültige API-Antwort";
                reply->deleteLater();
                return;
            }
            QJsonArray files = doc.object().value("files").toArray();

            qDebug() << "--- Deine Dateien im Google Drive ---";
            for (const QJsonValue &value : files) {
                qDebug() << "Datei:" << value.toObject().value("name").toString();
            }
            reply->deleteLater();
        });
    }
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    DriveSync syncApp;
    return a.exec();
}

#include "main.moc"
