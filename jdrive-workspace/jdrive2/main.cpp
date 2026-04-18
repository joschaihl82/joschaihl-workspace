#include <QApplication>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QDesktopServices>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFileSystemWatcher>
#include <QDir>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QProcess>

class DriveSync : public QObject {
    Q_OBJECT
public:
    DriveSync(QObject *parent = nullptr) : QObject(parent) {
        setupLocalDirectory();
        setupOAuth();
        setupFileWatcher();
    }

private:
    QOAuth2AuthorizationCodeFlow *google;
    QFileSystemWatcher *watcher;
    QString syncPath;

    void setupLocalDirectory() {
        syncPath = QDir::homePath() + "/google_drive";
        QDir dir(syncPath);
        if (!dir.exists()) {
            dir.mkpath(".");
            qDebug() << "Verzeichnis erstellt:" << syncPath;
        } else {
            qDebug() << "Verzeichnis existiert bereits:" << syncPath;
        }
    }

    void setupOAuth() {
        google = new QOAuth2AuthorizationCodeFlow(this);

        google->setAuthorizationUrl(QUrl("https://accounts.google.com/o/oauth2/auth"));
        google->setAccessTokenUrl(QUrl("https://oauth2.googleapis.com/token"));

        // DEINE CLIENT-ID (aktualisiert)
        google->setClientIdentifier("487363744399-g0h02noep3qgc2q7b5co0undhdoau0ir.apps.googleusercontent.com");

        // ACHTUNG: Hier musst du dein Client Secret aus der Google Console eintragen!
        google->setClientIdentifierSharedKey("DEIN_CLIENT_SECRET_HIER_EINTRAGEN");

        google->setScope("https://www.googleapis.com/auth/drive");

        auto replyHandler = new QOAuthHttpServerReplyHandler(1337, this);
        google->setReplyHandler(replyHandler);

        // Direkter Aufruf von xdg-open, um GUI-Probleme zu umgehen
        connect(google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, [](const QUrl &url) {
            qDebug() << "Öffne Browser für Google Login...";
            QProcess::startDetached("xdg-open", {url.toString()});
        });

        connect(google, &QOAuth2AuthorizationCodeFlow::granted, this, [this]() {
            qDebug() << "Authentifizierung erfolgreich! Verbindung zu Google Drive steht.";
            fetchFileList();
        });

        qDebug() << "Starte Authentifizierungsvorgang...";
        google->grant();
    }

    void setupFileWatcher() {
        watcher = new QFileSystemWatcher(this);
        watcher->addPath(syncPath);

        connect(watcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString &path) {
            qDebug() << "Lokale Änderung im Ordner erkannt:" << path;
        });
    }

    void fetchFileList() {
        QUrl url("https://www.googleapis.com/drive/v3/files");
        auto reply = google->get(url);
        connect(reply, &QNetworkReply::finished, this, [reply]() {
            if (reply->error() != QNetworkReply::NoError) {
                qDebug() << "Fehler beim Laden der Dateiliste:" << reply->errorString();
                reply->deleteLater();
                return;
            }

            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject json = doc.object();
            QJsonArray files = json["files"].toArray();

            qDebug() << "Gefundene Dateien in deinem Google Drive:";
            for (const QJsonValue &value : files) {
                QJsonObject fileObj = value.toObject();
                qDebug() << " - " << fileObj["name"].toString();
            }
            reply->deleteLater();
        });
    }
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false); // Programm läuft weiter, auch ohne Fenster

    DriveSync syncApp;

    return a.exec();
}

#include "main.moc"
