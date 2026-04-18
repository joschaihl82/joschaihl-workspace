// antivir.cpp
// Scannt rekursiv das Root-Dateisystem "/" und passt die Fortschrittsanzeige an die Anzahl der gelisteten Pfade an.
// Übergabe an clamscan via --file-list. Virenfunde werden in $(HOME)/rmvirus.sh gespeichert.

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QThread>
#include <QIcon>
#include <QTextStream>
#include <QDir>
#include <QProcess>
#include <QFileInfo>
#include <QFile>
#include <QDateTime>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QProcessEnvironment>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QMap>
#include <QDesktopServices>
#include <QUrl>
#include <QWidget>
#include <QScreen>
#include <QSet>
#include <QDirIterator>
#include <sys/statvfs.h>
#include <mntent.h>
#include <memory>

static const QString APP_VERSION = QStringLiteral("1.2.1");

// -----------------------------
// Logging (NO-OP: keine Konsolenausgaben)
static void openLogFileQt(const QString &) { /* no-op */ }
static void closeLogFileQt() { /* no-op */ }
static void logMessageQt(const QString &) { /* no-op */ }

// -----------------------------
// InfectionWindow
class InfectionWindow : public QDialog {
    Q_OBJECT
public:
    explicit InfectionWindow(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Virenfunde");
        resize(700, 400);

        QVBoxLayout *main = new QVBoxLayout(this);
        QHBoxLayout *top = new QHBoxLayout();
        m_list = new QListWidget(this);
        m_desc = new QTextEdit(this);
        m_desc->setReadOnly(true);
        m_desc->setMinimumWidth(320);

        top->addWidget(m_list, 2);
        top->addWidget(m_desc, 3);

        QHBoxLayout *buttons = new QHBoxLayout();
        QPushButton *closeBtn = new QPushButton("Schließen", this);
        QPushButton *openLogBtn = new QPushButton("Log öffnen", this);
        buttons->addStretch();
        buttons->addWidget(openLogBtn);
        buttons->addWidget(closeBtn);

        main->addLayout(top);
        main->addLayout(buttons);

        connect(m_list, &QListWidget::currentTextChanged, this, &InfectionWindow::onSelectionChanged);
        connect(closeBtn, &QPushButton::clicked, this, &InfectionWindow::close);
        connect(openLogBtn, &QPushButton::clicked, this, &InfectionWindow::onOpenLog);

        m_descriptions.insert("Eicar-Test-Signature", "EICAR-Testdatei: harmlose Signatur zur Funktionsprüfung.");
        m_descriptions.insert("Win.Trojan.Generic", "Generische Trojaner-Erkennung: Herkunft prüfen und ggf. isolieren.");
        m_descriptions.insert("HTML.Phishing", "Phishing-Inhalt erkannt: verdächtige Links/Formulare.");
        m_descriptions.insert("PUA", "Potentiell unerwünschte Anwendung: z. B. Adware.");
        m_descriptions.insert("Unknown", "Keine Details verfügbar. Logs und Herstellerressourcen prüfen.");
    }

    void addInfection(const QString &path, const QString &virus) {
        QString itemText = QString("%1 — %2").arg(path, virus);
        QList<QListWidgetItem*> found = m_list->findItems(itemText, Qt::MatchExactly);
        if (!found.isEmpty()) return;
        m_list->addItem(itemText);
        if (m_list->count() == 1) m_list->setCurrentRow(0);
        if (!isVisible()) show();
        raise();
        activateWindow();
    }

private slots:
    void onSelectionChanged(const QString &text) {
        QString virus = "Unknown";
        int sep = text.lastIndexOf(" — ");
        if (sep >= 0) virus = text.mid(sep + 3).trimmed();
        QString desc = m_descriptions.value(virus, m_descriptions.value("Unknown"));
        QString details = QString("<b>Signatur:</b> %1<br><br><b>Beschreibung:</b><br>%2").arg(virus.toHtmlEscaped(), desc.toHtmlEscaped());
        m_desc->setHtml(details);
    }

    void onOpenLog() {
        // Öffnet das rmvirus-Skript im Dateimanager / Standardprogramm
        QString scriptPath = QDir::homePath() + "/rmvirus.sh";
        QDesktopServices::openUrl(QUrl::fromLocalFile(scriptPath));
    }

private:
    QListWidget *m_list;
    QTextEdit *m_desc;
    QMap<QString, QString> m_descriptions;
};

// -----------------------------
// Scanner thread
class ClamAVScanner : public QThread {
    Q_OBJECT
public:
    explicit ClamAVScanner(QObject *parent = nullptr) : QThread(parent),
        m_abort(false), m_totalItems(0), m_scannedFiles(0),
        m_infectedCount(0), m_errorCount(0) {}

    void scanOpenResources() { m_targetDescription = "/"; if (isRunning()) wait(); start(); }
    void abort() { m_abort = true; }

signals:
    void scanStarted(const QString &path);
    void scanFinished(const QString &path, bool infected, const QString &summary);
    void scanError(const QString &error);
    void scanProgress(int current, int total, const QString &file);
    void scanLineOutput(const QString &line);
    void scanInfectionFound(const QString &path, const QString &virus);

    // Indexierungsfortschritt während des Erstellens der Datei-Liste
    void indexProgress(qint64 current, qint64 total, const QString &path);

protected:
    void run() override {
        m_abort = false;
        emit scanStarted(m_targetDescription);

        QString listFilePath;
        int listedCount = 0;
        if (!buildOpenResourcesList(listFilePath, listedCount)) {
            emit scanError("Dateiliste konnte nicht erstellt werden oder keine Dateien gefunden.");
            return;
        }

        m_totalItems = listedCount;
        m_scannedFiles = 0;
        m_infectedCount = 0;
        m_errorCount = 0;

        QString details;
        bool infected = false;

        if (!startAndParseClamscanWithList(listFilePath, details, infected)) {
            emit scanError("clamscan konnte mit der Datei-Liste nicht gestartet werden: " + listFilePath);
            QFile::remove(listFilePath);
            return;
        }

        if (m_scannedFiles < m_totalItems) {
            emit scanProgress(m_totalItems, m_totalItems, QString());
        }

        QString summary;
        summary += QString("Aufgelistete Ressourcen: %1\n").arg(m_totalItems);
        summary += QString("Verarbeitet: %1\n").arg(m_scannedFiles);
        summary += QString("Funde: %1\n").arg(m_infectedCount);
        summary += QString("Fehler: %1\n").arg(m_errorCount);
        if (!details.isEmpty()) summary += QString("\nDetails:\n") + details;

        emit scanFinished(m_targetDescription, infected, summary);

        QFile::remove(listFilePath);
    }

private:
    QString m_targetDescription;
    bool m_abort;
    int m_totalItems;
    int m_scannedFiles;
    int m_infectedCount;
    int m_errorCount;

    static bool isReadableRegularFile(const QString &path) {
        if (path.isEmpty()) return false;
        QFileInfo fi(path);
        if (!fi.exists() || !fi.isFile()) return false;
        return fi.isReadable();
    }

    static qint64 estimateTotalFilesUnderRoot() {
        FILE *mnt = setmntent("/proc/mounts", "r");
        if (!mnt) return 1;
        struct mntent *ent;
        qint64 total = 0;
        while ((ent = getmntent(mnt)) != nullptr) {
            const char *mp = ent->mnt_dir;
            if (!mp) continue;
            if (strcmp(mp, "/") != 0 && strncmp(mp, "/", 1) != 0) continue;
            struct statvfs st;
            if (statvfs(mp, &st) == 0) {
                if (st.f_files > 0) {
                    qint64 used = static_cast<qint64>(st.f_files) - static_cast<qint64>(st.f_ffree);
                    if (used > 0) total += used;
                }
            }
        }
        endmntent(mnt);
        if (total <= 0) total = 1;
        return total;
    }

    bool buildOpenResourcesList(QString &outPath, int &outCount) {
        outPath.clear();
        outCount = 0;

        QString tempPath = QDir::temp().absoluteFilePath(QString("openresources_%1_%2.lst")
                                .arg(QCoreApplication::applicationName().isEmpty() ? "antivir" : QCoreApplication::applicationName())
                                .arg(QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz")));
        QFile outFile(tempPath);
        if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return false;
        }
        QTextStream ts(&outFile);

        const QStringList excludePrefixes = {
            "/proc",
            "/sys",
            "/dev",
            "/run",
            "/var/run",
            "/tmp",
            "/var/tmp"
        };

        qint64 estimatedTotal = estimateTotalFilesUnderRoot();
        emit indexProgress(0, estimatedTotal, QString());

        QDirIterator it("/", QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
        qint64 processed = 0;
        const qint64 flushInterval = 1000;
        const qint64 progressInterval = 200;

        while (it.hasNext()) {
            if (m_abort) break;
            QString filePath = it.next();

            bool skip = false;
            for (const QString &pref : excludePrefixes) {
                if (filePath == pref || filePath.startsWith(pref + "/")) {
                    skip = true;
                    break;
                }
            }
            if (skip) continue;

            if (!isReadableRegularFile(filePath)) continue;

            ts << filePath << "\n";
            ++outCount;
            ++processed;

            if ((outCount % flushInterval) == 0) outFile.flush();

            if ((processed % progressInterval) == 0) {
                emit indexProgress(processed, estimatedTotal, filePath);
            }
        }

        emit indexProgress(processed, estimatedTotal, QString());

        outFile.flush();
        outFile.close();

        if (outCount == 0) {
            QFile::remove(tempPath);
            return false;
        }

        outPath = tempPath;
        return true;
    }

    bool startAndParseClamscanWithList(const QString &listFilePath, QString &details, bool &outInfected) {
        outInfected = false;
        QString program = "clamscan";

        QProcess proc;
        proc.setProcessChannelMode(QProcess::MergedChannels);

        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("TERM", "dumb");
        proc.setProcessEnvironment(env);

        QStringList args;
        args << "--no-summary" << "--stdout"
             << "--file-list=" + listFilePath;

        proc.start(program, args);
        if (!proc.waitForStarted(3000)) {
            ++m_errorCount;
            return false;
        }

        QByteArray buffer;
        bool foundInfected = false;
        bool sawUnexpected = false;

        QElapsedTimer timer;
        timer.start();

        auto bumpProgress = [&](const QString &filePath) {
            ++m_scannedFiles;
            emit scanProgress(m_scannedFiles, m_totalItems, filePath);
        };

        auto consumeLine = [&](const QString &sline) {
            emit scanLineOutput(sline);

            int colon = sline.indexOf(':');
            if (colon > 0) {
                QString maybePath = sline.left(colon).trimmed();
                QString rest = sline.mid(colon + 1).trimmed();

                if (rest.endsWith("FOUND")) {
                    QString virus = rest.mid(0, rest.size() - QString("FOUND").size()).trimmed();
                    details += QString("Infected file: %1 - Virus: %2\n").arg(maybePath, virus);
                    foundInfected = true;
                    ++m_infectedCount;
                    bumpProgress(maybePath);
                    emit scanInfectionFound(maybePath, virus);
                } else if (rest.endsWith("OK")) {
                    bumpProgress(maybePath);
                } else if (rest.startsWith("Error") || rest.startsWith("ERROR")) {
                    ++m_errorCount;
                    bumpProgress(maybePath);
                } else {
                    sawUnexpected = true;
                }
            } else {
                sawUnexpected = true;
            }
        };

        while (proc.state() == QProcess::Running) {
            if (proc.waitForReadyRead(200)) {
                buffer += proc.readAllStandardOutput();
                while (true) {
                    int nl = buffer.indexOf('\n');
                    if (nl < 0) break;
                    QByteArray line = buffer.left(nl);
                    buffer = buffer.mid(nl + 1);
                    QString sline = QString::fromUtf8(line).trimmed();
                    if (sline.isEmpty()) continue;
                    consumeLine(sline);
                }
            }

            if (timer.elapsed() > 24 * 60 * 60 * 1000) {
                proc.kill();
                proc.waitForFinished(2000);
                ++m_errorCount;
                return false;
            }

            if (m_abort) {
                proc.kill();
                proc.waitForFinished(2000);
                return false;
            }
        }

        buffer += proc.readAllStandardOutput();
        if (!buffer.isEmpty()) {
            QList<QByteArray> lines = buffer.split('\n');
            for (const QByteArray &line : lines) {
                QString sline = QString::fromUtf8(line).trimmed();
                if (sline.isEmpty()) continue;
                consumeLine(sline);
            }
        }

        if (sawUnexpected) ++m_errorCount;
        outInfected = foundInfected;
        return true;
    }
};

// -----------------------------
// Overlay window (nur Prozentanzeige vor Dateiname) mit weiß-transparenten Label-Hintergründen
class ProgressOverlay : public QWidget {
    Q_OBJECT
public:
    explicit ProgressOverlay(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_ShowWithoutActivating);
        setWindowOpacity(0.80);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(8,8,8,8);
        layout->setSpacing(6);

        m_displayLabel = new QLabel(QStringLiteral("000.000% -"), this);
        QFont dispFont = m_displayLabel->font();
        dispFont.setBold(true);
        dispFont.setPointSize(qMax(10, dispFont.pointSize()));
        m_displayLabel->setFont(dispFont);
        m_displayLabel->setStyleSheet(
            "color: black;"
            "background-color: rgba(255,255,255,153);"
            "padding: 6px;"
            "border-radius: 6px;"
        );
        m_displayLabel->setAlignment(Qt::AlignLeft);

        m_countLabel = new QLabel(QStringLiteral("Viren: 0"), this);
        QFont countFont = m_countLabel->font();
        countFont.setBold(true);
        m_countLabel->setFont(countFont);
        m_countLabel->setStyleSheet(
            "color: black;"
            "background-color: rgba(255,255,255,153);"
            "padding: 4px;"
            "border-radius: 6px;"
        );
        m_countLabel->setAlignment(Qt::AlignLeft);

        layout->addWidget(m_displayLabel);
        layout->addWidget(m_countLabel);

        setLayout(layout);
        adjustSize();

        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) {
            QRect geom = screen->availableGeometry();
            int margin = 12;
            move(geom.right() - width() - margin, geom.bottom() - height() - margin);
        }

        m_currentPercent = 0.0;
        m_currentFile.clear();
    }

public slots:
    void setPercent(double percent) {
        double p = qBound(0.0, percent, 100.0);
        m_currentPercent = p;
        updateDisplay();
    }

    void setCurrentFile(const QString &file) {
        m_currentFile = file;
        updateDisplay();
    }

    void setVirusCount(int count) {
        m_countLabel->setText(QStringLiteral("Viren: %1").arg(count));
    }

private:
    void updateDisplay() {
        QString num = QString::number(m_currentPercent, 'f', 3);
        num = QString("%1").arg(num, 7, QChar('0'));
        QString percentStr = num + "%";

        QString filePart = m_currentFile;
        if (filePart.isEmpty()) filePart = QStringLiteral("-");
        const int maxLen = 64;
        if (filePart.length() > maxLen) filePart = QString("...%1").arg(filePart.right(maxLen));

        QString combined = QString("%1  %2").arg(percentStr, filePart);
        m_displayLabel->setText(combined);
        adjustSize();
    }

    QLabel *m_displayLabel;
    QLabel *m_countLabel;
    double m_currentPercent;
    QString m_currentFile;
};

// -----------------------------
// Tray app + integration
class ClamAVTrayApp : public QObject {
    Q_OBJECT
public:
    explicit ClamAVTrayApp(const QString & /*logPath*/) :
        m_notifiedThisScan(false),
        m_lastIconPercent(-1.0),
        m_infectionWindow(nullptr),
        m_overlay(nullptr),
        m_showWarningBadge(false),
        m_infectionCount(0),
        m_scanningActive(false)
    {
        // openLogFileQt intentionally not used (no logging)

        trayIcon = new QSystemTrayIcon(this);
        QMenu *menu = new QMenu();
        QAction *scanOpenResourcesAction = new QAction("Offene Dateien/Programme/Libs scannen", menu);
        QAction *showInfectionsAction = new QAction("Virenfunde anzeigen", menu);
        QAction *quitAction = new QAction("Beenden", menu);
        menu->addAction(scanOpenResourcesAction);
        menu->addAction(showInfectionsAction);
        menu->addSeparator();
        menu->addAction(quitAction);
        trayIcon->setContextMenu(menu);

        buildInfo = QStringLiteral("%1 (built %2 %3)").arg(APP_VERSION, QString::fromUtf8(__DATE__), QString::fromUtf8(__TIME__));
        startTime = QDateTime::currentDateTime().toString(Qt::ISODate);
        baseTooltip = QStringLiteral("ClamAV Qt Scanner\nVersion: %1\nStarted: %2").arg(buildInfo, startTime);
        trayIcon->setToolTip(baseTooltip);

        baseIcon = QIcon::fromTheme("security-high");
        if (baseIcon.isNull()) baseIcon = QIcon::fromTheme("dialog-information");
        virusIcon = QIcon::fromTheme("emblem-danger");
        if (virusIcon.isNull()) virusIcon = QIcon::fromTheme("dialog-warning");

        trayIcon->setIcon(baseIcon);
        trayIcon->show();

        m_overlay = new ProgressOverlay();
        m_overlay->setWindowOpacity(0.80);
        m_overlay->show();
        m_overlay->setPercent(0.0);
        m_overlay->setCurrentFile(QString());
        m_overlay->setVirusCount(0);
        m_infectionCount = 0;

        scanner = new ClamAVScanner(this);

        connect(scanOpenResourcesAction, &QAction::triggered, this, &ClamAVTrayApp::onScanOpenResources);
        connect(showInfectionsAction, &QAction::triggered, this, &ClamAVTrayApp::onShowInfections);
        connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
        connect(scanner, &ClamAVScanner::scanStarted, this, &ClamAVTrayApp::onScanStarted);
        connect(scanner, &ClamAVScanner::scanFinished, this, &ClamAVTrayApp::onScanFinished);
        connect(scanner, &ClamAVScanner::scanError, this, &ClamAVTrayApp::onScanError);
        connect(scanner, &ClamAVScanner::scanProgress, this, &ClamAVTrayApp::onScanProgress);
        connect(scanner, &ClamAVScanner::scanLineOutput, this, &ClamAVTrayApp::onScanLineOutput);
        connect(scanner, &ClamAVScanner::scanInfectionFound, this, &ClamAVTrayApp::onScanInfectionFound);
        connect(scanner, &ClamAVScanner::indexProgress, this, &ClamAVTrayApp::onIndexProgress);

        trayIcon->showMessage("ClamAV Scan", "Initialer Scan des Root-Dateisystems startet", QSystemTrayIcon::Information, 3000);
        scanner->scanOpenResources();
    }

    ~ClamAVTrayApp() {
        if (m_overlay) {
            m_overlay->close();
            delete m_overlay;
            m_overlay = nullptr;
        }
        // closeLogFileQt intentionally not used
    }

public slots:
    void onScanOpenResources() {
        trayIcon->showMessage("ClamAV Scan", "Starte rekursiven Scan von /", QSystemTrayIcon::Information, 3000);
        scanner->scanOpenResources();
    }

    void onShowInfections() {
        if (!m_infectionWindow) {
            m_infectionWindow = new InfectionWindow();
            m_infectionWindow->setAttribute(Qt::WA_DeleteOnClose);
            connect(m_infectionWindow, &QObject::destroyed, this, [this]() { m_infectionWindow = nullptr; });
        }
        m_infectionWindow->show();
        m_infectionWindow->raise();
        m_infectionWindow->activateWindow();
    }

    void onScanStarted(const QString &path) {
        Q_UNUSED(path);
        m_notifiedThisScan = false;
        m_lastIconPercent = -1.0;
        m_showWarningBadge = false;
        m_scanningActive = true;
        m_infectionCount = 0;
        if (m_overlay) {
            m_overlay->setPercent(0.0);
            m_overlay->setCurrentFile(QString());
            m_overlay->setVirusCount(m_infectionCount);
        }
        trayIcon->setIcon(baseIcon);
    }

    void onScanProgress(int current, int total, const QString &file) {
        static double lastPct = -1.0;
        static qint64 lastUpdateMs = 0;

        QString shortName = QFileInfo(file).fileName();
        QString statusLine;

        if (total > 0) {
            double percent = (static_cast<double>(current) / static_cast<double>(total)) * 100.0;
            QString pctStr = QString::number(percent, 'f', 3);

            qint64 nowMs = QDateTime::currentMSecsSinceEpoch();
            bool pctChanged = (qAbs(percent - lastPct) >= 0.001);
            bool timeElapsed = (nowMs - lastUpdateMs) > 500;

            if (pctChanged || timeElapsed) {
                statusLine = QStringLiteral("Scanning: %1/%2 - %3 (%4%)").arg(current).arg(total).arg(shortName).arg(pctStr);
                QString newTooltip = baseTooltip + "\n" + statusLine;
                trayIcon->setToolTip(newTooltip);

                lastPct = percent;
                lastUpdateMs = nowMs;
            }

            if (m_overlay) {
                m_overlay->setPercent(percent);
                m_overlay->setCurrentFile(shortName);
                m_overlay->setVirusCount(m_infectionCount);
            }

            m_lastIconPercent = percent;
        } else {
            statusLine = QStringLiteral("Scanning: %1 - %2").arg(current).arg(shortName);
            QString newTooltip = baseTooltip + "\n" + statusLine;
            trayIcon->setToolTip(newTooltip);
            if (m_overlay) m_overlay->setCurrentFile(shortName);
        }
    }

    void onScanLineOutput(const QString &line) {
        QString preview = line;
        const int maxLen = 120;
        if (preview.length() > maxLen) preview = preview.left(maxLen - 3) + "...";
        QString newTooltip = baseTooltip + "\nLast: " + preview;
        trayIcon->setToolTip(newTooltip);
    }

    void onScanInfectionFound(const QString &path, const QString &virus) {
        if (!m_infectionWindow) {
            m_infectionWindow = new InfectionWindow();
            m_infectionWindow->setAttribute(Qt::WA_DeleteOnClose);
            connect(m_infectionWindow, &QObject::destroyed, this, [this]() { m_infectionWindow = nullptr; });
        }
        m_infectionWindow->addInfection(path, virus);

        ++m_infectionCount;
        if (m_overlay) {
            m_overlay->setVirusCount(m_infectionCount);
            m_overlay->setCurrentFile(QFileInfo(path).fileName());
        }

        m_showWarningBadge = true;
        trayIcon->setIcon(virusIcon);

        // Notification: 24 hours = 86400000 ms
        const int longNotifyMs = 24 * 60 * 60 * 1000;
        if (!m_notifiedThisScan) {
            QString balloonTitle = QStringLiteral("ClamAV - Infection");
            QString balloonText = QString("%1 — %2").arg(QFileInfo(path).fileName(), virus);
            trayIcon->showMessage(balloonTitle, balloonText, QSystemTrayIcon::Warning, longNotifyMs);
            m_notifiedThisScan = true;
        }

        // Schreibe Lösch-Skript-Eintrag in $(HOME)/rmvirus.sh
        writeRmScriptEntry(path);
    }

    void onScanFinished(const QString &path, bool infected, const QString &summary) {
        Q_UNUSED(path);
        if (infected && !m_notifiedThisScan) {
            const int longNotifyMs = 24 * 60 * 60 * 1000;
            trayIcon->showMessage("ClamAV Scan - Infection", "Infections found!\n" + summary, QSystemTrayIcon::Warning, longNotifyMs);
            m_notifiedThisScan = true;
        }
        m_scanningActive = false;
        if (!m_showWarningBadge) trayIcon->setIcon(baseIcon);

        if (m_overlay) {
            m_overlay->setPercent(100.0);
            m_overlay->setCurrentFile(QString());
            m_overlay->setVirusCount(m_infectionCount);
        }
    }

    void onScanError(const QString &error) {
        Q_UNUSED(error);
        // keine Konsolenausgabe
    }

    void onIndexProgress(qint64 current, qint64 total, const QString &path) {
        double percent = 0.0;
        if (total > 0) percent = (static_cast<double>(current) / static_cast<double>(total)) * 100.0;
        if (percent < 0.0) percent = 0.0;
        if (percent > 100.0) percent = 100.0;

        QString shortName = QFileInfo(path).fileName();
        if (shortName.isEmpty()) shortName = QStringLiteral("/");

        if (m_overlay) {
            m_overlay->setPercent(percent);
            m_overlay->setCurrentFile(shortName);
            m_overlay->setVirusCount(m_infectionCount);
        }

        QString pctStr = QString::number(percent, 'f', 3);
        pctStr = QString("%1").arg(pctStr, 7, QChar('0'));
        QString statusLine = QStringLiteral("Indexing: %1/%2 - %3 (%4%)").arg(current).arg(total).arg(shortName).arg(pctStr);
        trayIcon->setToolTip(baseTooltip + "\n" + statusLine);
    }

private:
    QSystemTrayIcon *trayIcon;
    ClamAVScanner *scanner;
    QString buildInfo;
    QString startTime;
    QString baseTooltip;

    bool m_notifiedThisScan;
    QIcon baseIcon;
    QIcon virusIcon;
    double m_lastIconPercent;
    InfectionWindow *m_infectionWindow;

    ProgressOverlay *m_overlay;

    bool m_showWarningBadge;
    int m_infectionCount;
    bool m_scanningActive;

    Q_DISABLE_COPY(ClamAVTrayApp)

    // Schreibt einen Eintrag in $HOME/rmvirus.sh, erstellt Datei falls nötig und setzt ausführbar
    void writeRmScriptEntry(const QString &infectedPath) {
        QString home = QDir::homePath();
        QString scriptPath = home + "/rmvirus.sh";
        QFile script(scriptPath);
        bool exists = script.exists();

        if (!script.open(QIODevice::Append | QIODevice::Text)) {
            return; // still: no console output
        }

        QTextStream ts(&script);
        if (!exists) {
            ts << "#!/bin/sh\n";
        }

        // sichere Quoting: einfache Anführungszeichen, vorhandene ' in Pfad ersetzen
        QString safePath = infectedPath;
        safePath.replace('\'', "'\"'\"'"); // closes, inserts escaped single quote, reopens
        ts << "rm -f '" << safePath << "'\n";
        script.flush();
        script.close();

        // set executable
        QFile::Permissions perms = script.permissions();
        perms |= QFile::ExeOwner | QFile::ExeGroup | QFile::ExeOther;
        script.setPermissions(perms);
    }
};

// -----------------------------
// main
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("antivir");
    app.setQuitOnLastWindowClosed(false);

    QString logDir = QDir::homePath() + "/.antivirus";
    QDir d(logDir);
    if (!d.exists()) d.mkpath(".");

    // kein Logging öffnen

    ClamAVTrayApp trayApp(QString());
    return app.exec();
}

#include "antivir.moc"

