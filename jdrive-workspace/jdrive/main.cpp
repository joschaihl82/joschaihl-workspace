#include <QApplication> // Zwingend dieser Header!
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
        // Lokalen Pfad $(HOME)/google_drive definieren
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

        // OAuth-Parameter für Google konfigurieren
        // ACHTUNG: Hier deine echten Zugangsdaten eintragen!
        google->setAuthorizationUrl(QUrl("https://accounts.google.com/o/oauth2/auth"));
        google->setAccessTokenUrl(QUrl("https://oauth2.googleapis.com/token"));
        google->setClientIdentifier("DEINE_CLIENT_ID.apps.googleusercontent.com");
        google->setClientIdentifierSharedKey("DEIN_CLIENT_SECRET");

        // Scope für vollen Google Drive Zugriff
        google->setScope("https://www.googleapis.com/auth/drive");

        // Lokalen HTTP-Server starten, um den Redirect von Google abzufangen
        auto replyHandler = new QOAuthHttpServerReplyHandler(1337, this);
        google->setReplyHandler(replyHandler);

        // Wenn Google den Browser öffnen will
        connect(google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
                &QDesktopServices::openUrl);

        // Wenn die Authentifizierung erfolgreich war
        connect(google, &QOAuth2AuthorizationCodeFlow::granted, this, [this]() {
            qDebug() << "Authentifizierung erfolgreich! Token erhalten.";
            fetchFileList(); // Test-Aufruf, um zu prüfen, ob die API antwortet
        });

        // Authentifizierung starten
        qDebug() << "Starte Authentifizierung...";
        google->grant();
    }

    void setupFileWatcher() {
        watcher = new QFileSystemWatcher(this);
        watcher->addPath(syncPath);

        // Wird aufgerufen, wenn sich im Ordner etwas ändert (Datei neu/gelöscht)
        connect(watcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString &path) {
            qDebug() << "Lokale Änderung im Verzeichnis erkannt:" << path;
            // Hier würde die Logik starten:
            // 1. Prüfen, WELCHE Datei sich geändert hat (z.B. Vergleich mit lokalem Cache/Datenbank).
            // 2. Google Drive API aufrufen, um die Datei hochzuladen.
        });
    }

    // Beispiel-Funktion: Ruft die neuesten Dateien aus dem Google Drive ab
    void fetchFileList() {
        QUrl url("https://www.googleapis.com/drive/v3/files");

        auto reply = google->get(url);
        connect(reply, &QNetworkReply::finished, this, [reply]() {
            if (reply->error() != QNetworkReply::NoError) {
                qDebug() << "API Fehler:" << reply->errorString();
                reply->deleteLater();
                return;
            }

            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject json = doc.object();
            QJsonArray files = json["files"].toArray();

            qDebug() << "Dateien in deinem Google Drive:";
            for (const QJsonValue &value : files) {
                QJsonObject fileObj = value.toObject();
                qDebug() << "-" << fileObj["name"].toString() << "(ID:" << fileObj["id"].toString() << ")";
            }

            reply->deleteLater();
        });
    }
};

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    DriveSync syncApp;

    return a.exec();
}

#include "main.moc"
