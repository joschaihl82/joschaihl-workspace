// antivir.cpp
// Scannt rekursiv das Root-Dateisystem "/" und passt die Fortschrittsanzeige an die Anzahl der gelisteten Pfade an.
// Übergabe an clamscan via --file-list. Log nach ~/.antivirus/antivirus.log.

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QThread>
#include <QIcon>
#include <QTextStream>
#include <QDebug>
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
#include <QProgressBar>
#include <QWidget>
#include <QScreen>
#include <QSet>
#include <QDirIterator>
#include <sys/statvfs.h>
#include <mntent.h>
#include <memory>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

static const QString APP_VERSION = QStringLiteral("1.2.1");

// -----------------------------
// Logging (RAII)
static std::unique_ptr<QFile> g_logFilePtr;
static std::unique_ptr<QTextStream> g_logStreamPtr;
static bool g_logOpen = false;

static void openLogFileQt(const QString &logPath) {
    if (g_logOpen) return;
    g_logFilePtr = std::make_unique<QFile>(logPath);
    QDir dir = QFileInfo(logPath).absoluteDir();
    if (!dir.exists()) dir.mkpath(".");
    if (!g_logFilePtr->open(QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Failed to open log file:" << logPath;
        g_logFilePtr.reset();
        g_logStreamPtr.reset();
        g_logOpen = false;
        return;
    }
    g_logStreamPtr = std::make_unique<QTextStream>(g_logFilePtr.get());
    *g_logStreamPtr << QDateTime::currentDateTime().toString(Qt::ISODate)
                    << " - antivirus started (version " << APP_VERSION << ")\n";
    g_logStreamPtr->flush();
    g_logOpen = true;
}

static void closeLogFileQt() {
    if (!g_logOpen) return;
    if (g_logStreamPtr) {
        *g_logStreamPtr << QDateTime::currentDateTime().toString(Qt::ISODate)
                        << " - antivirus exiting\n";
        g_logStreamPtr->flush();
        g_logStreamPtr.reset();
    }
    if (g_logFilePtr) {
        g_logFilePtr->close();
        g_logFilePtr.reset();
    }
    g_logOpen = false;
}

static void logMessageQt(const QString &msg) {
    QString line = QDateTime::currentDateTime().toString(Qt::ISODate) + " - " + msg;
    fprintf(stdout, "%s\n", line.toLocal8Bit().constData());
    fflush(stdout);
    if (g_logOpen && g_logStreamPtr) {
        *g_logStreamPtr << line << "\n";
        g_logStreamPtr->flush();
    }
}

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
        QString logPath = QDir::homePath() + "/.antivirus/antivirus.log";
        QDesktopServices::openUrl(QUrl::fromLocalFile(logPath));
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

    // Angepasst: scanOpenResources startet Scan mit Zielbeschreibung "/"
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

        // Die Gesamtzahl für die Progressbar ist die Anzahl der Zeilen in der Liste.
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

        // Falls aus irgendeinem Grund weniger Zeilen verarbeitet wurden, auf die Gesamtanzahl kappen.
        if (m_scannedFiles < m_totalItems) {
            emit scanProgress(m_totalItems, m_totalItems, QString());
        }

        QString summary;
        summary += QString("Aufgelistete Ressourcen: %1\n").arg(m_totalItems);
        summary += QString("Verarbeitet: %1\n").arg(m_scannedFiles);
        summary += QString("Funde: %1\n").arg(m_infectedCount);
        summary += QString("Fehler: %1\n").arg(m_errorCount);
        if (!details.isEmpty()) summary += QString("\nDetails:\n") + details;

        logMessageQt(QString("Scan summary for %1:\n%2").arg(m_targetDescription, summary));
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

    // Schätzt die Anzahl genutzter Inodes unter "/" durch Summieren der (f_files - f_ffree)
    static qint64 estimateTotalFilesUnderRoot() {
        FILE *mnt = setmntent("/proc/mounts", "r");
        if (!mnt) return 1;
        struct mntent *ent;
        qint64 total = 0;
        while ((ent = getmntent(mnt)) != nullptr) {
            const char *mp = ent->mnt_dir;
            if (!mp) continue;
            // Wir interessieren uns nur für Mountpoints, die unter "/" liegen
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

    // Rekursive Implementierung: erstellt eine temporäre Liste mit allen regulären, lesbaren Dateien unter "/"
    bool buildOpenResourcesList(QString &outPath, int &outCount) {
        outPath.clear();
        outCount = 0;

        QString tempPath = QDir::temp().absoluteFilePath(QString("openresources_%1_%2.lst")
                                .arg(QCoreApplication::applicationName().isEmpty() ? "antivir" : QCoreApplication::applicationName())
                                .arg(QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz")));
        QFile outFile(tempPath);
        if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            logMessageQt(QString("Failed to open temp list file: %1").arg(tempPath));
            return false;
        }
        QTextStream ts(&outFile);

        // Ausschlussliste für virtuelle Dateisysteme und Orte, die typischerweise nicht gescannt werden sollten
        const QStringList excludePrefixes = {
            "/proc",
            "/sys",
            "/dev",
            "/run",
            "/var/run",
            "/tmp",
            "/var/tmp"
        };

        // Schätze Gesamtanzahl vorab
        qint64 estimatedTotal = estimateTotalFilesUnderRoot();
        logMessageQt(QString("Estimated total files under / : %1").arg(estimatedTotal));
        emit indexProgress(0, estimatedTotal, QString());

        // QDirIterator rekursiv über "/"
        QDirIterator it("/", QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
        qint64 processed = 0;
        const qint64 flushInterval = 1000;
        const qint64 progressInterval = 200; // wie oft indexProgress senden

        while (it.hasNext()) {
            if (m_abort) break;
            QString filePath = it.next();

            // Überspringe Pfade mit ausgeschlossenen Präfixen
            bool skip = false;
            for (const QString &pref : excludePrefixes) {
                if (filePath == pref || filePath.startsWith(pref + "/")) {
                    skip = true;
                    break;
                }
            }
            if (skip) continue;

            // Nur reguläre, lesbare Dateien hinzufügen
            if (!isReadableRegularFile(filePath)) continue;

            ts << filePath << "\n";
            ++outCount;
            ++processed;

            if ((outCount % flushInterval) == 0) outFile.flush();

            // Sende Indexierungsfortschritt in regelmäßigen Abständen
            if ((processed % progressInterval) == 0) {
                emit indexProgress(processed, estimatedTotal, filePath);
            }
        }

        emit indexProgress(processed, estimatedTotal, QString());

        outFile.flush();
        outFile.close();

        if (outCount == 0) {
            QFile::remove(tempPath);
            logMessageQt("No regular readable files collected from / (after exclusions).");
            return false;
        }

        outPath = tempPath;
        logMessageQt(QString("Prepared recursive file list for '/': %1 (count=%2)").arg(outPath).arg(outCount));
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

        // WICHTIG: keine --suppress-ok-results, damit jede Zeile (OK/FOUND/ERROR) den Fortschritt liefert.
        QStringList args;
        args << "--no-summary" << "--stdout"
             << "--file-list=" + listFilePath;

        proc.start(program, args);
        if (!proc.waitForStarted(3000)) {
            QString err = QString("clamscan failed to start with --file-list: %1").arg(listFilePath);
            logMessageQt(err);
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
            fprintf(stdout, "%s\n", sline.toLocal8Bit().constData());
            fflush(stdout);
            logMessageQt(QString("clamscan: %1").arg(sline));

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
                    bumpProgress(maybePath); // Fehler zählt als verarbeitet
                } else {
                    logMessageQt(QString("clamscan unexpected output: %1").arg(sline));
                    sawUnexpected = true;
                }
            } else {
                logMessageQt(QString("clamscan unexpected output (no colon): %1").arg(sline));
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

            // Safety timeout: 24h
            if (timer.elapsed() > 24 * 60 * 60 * 1000) {
                proc.kill();
                proc.waitForFinished(2000);
                QString err = QString("clamscan overall timeout for list: %1").arg(listFilePath);
                logMessageQt(err);
                ++m_errorCount;
                return false;
            }

            if (m_abort) {
                proc.kill();
                proc.waitForFinished(2000);
                logMessageQt(QString("Scan aborted by user for list: %1").arg(listFilePath));
                return false;
            }
        }

        // Reste konsumieren
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
// Overlay window (nur Prozentanzeige vor Dateiname)
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

        // Anzeige-Label: enthält "PPP.ppp%  filename"
        m_displayLabel = new QLabel(QStringLiteral("000.000% -"), this);
        QFont dispFont = m_displayLabel->font();
        dispFont.setBold(true);
        dispFont.setPointSize(qMax(10, dispFont.pointSize()));
        m_displayLabel->setFont(dispFont);
        m_displayLabel->setStyleSheet("color: black;");
        m_displayLabel->setAlignment(Qt::AlignLeft);

        // Optional: Virencount als kleine Zeile darunter
        m_countLabel = new QLabel(QStringLiteral("Viren: 0"), this);
        QFont countFont = m_countLabel->font();
        countFont.setBold(true);
        m_countLabel->setFont(countFont);
        m_countLabel->setStyleSheet("color: black;");
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
    // percent in [0..100]
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
        // Format: 3 Nachkommastellen, links mit Nullen aufgefüllt, Gesamtbreite vor Dezimalpunkt inklusive Dezimalstellen:
        // Wir wollen z.B. "005.123%" oder "100.000%"
        QString num = QString::number(m_currentPercent, 'f', 3); // e.g. "5.123" or "100.000"
        // Pad left with zeros to at least 7 characters (e.g. "005.123")
        // 7 = 3 digits before dot + dot + 3 decimals => "000.000" length 7
        num = QString("%1").arg(num, 7, QChar('0'));
        QString percentStr = num + "%";

        QString filePart = m_currentFile;
        if (filePart.isEmpty()) filePart = QStringLiteral("-");
        // Kürze Dateiname falls zu lang
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
    explicit ClamAVTrayApp(const QString &logPath) :
        m_notifiedThisScan(false),
        m_lastIconPercent(-1.0),
        m_infectionWindow(nullptr),
        m_overlay(nullptr),
        m_showWarningBadge(false),
        m_infectionCount(0),
        m_scanningActive(false)
    {
        openLogFileQt(logPath);

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
        logMessageQt(QString("Starting initial recursive scan of /"));
        scanner->scanOpenResources();
    }

    ~ClamAVTrayApp() {
        if (m_overlay) {
            m_overlay->close();
            delete m_overlay;
            m_overlay = nullptr;
        }
        closeLogFileQt();
    }

public slots:
    void onScanOpenResources() {
        trayIcon->showMessage("ClamAV Scan", "Starte rekursiven Scan von /", QSystemTrayIcon::Information, 3000);
        logMessageQt(QString("User started recursive scan of /"));
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
        logMessageQt(QString("Scan started for: %1").arg(path));
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

        logMessageQt(QString("Progress %1/%2 - %3").arg(current).arg(total).arg(file));
    }

    void onScanLineOutput(const QString &line) {
        QString preview = line;
        const int maxLen = 120;
        if (preview.length() > maxLen) preview = preview.left(maxLen - 3) + "...";
        QString newTooltip = baseTooltip + "\nLast: " + preview;
        trayIcon->setToolTip(newTooltip);
        logMessageQt(QString("Subprocess line: %1").arg(line));
    }

    // Vollständig integrierte, sichere Version: schreibt nur Pfade in ~/.antivirus/infected_files.txt
    void onScanInfectionFound(const QString &path, const QString &virus) {
        if (!m_infectionWindow) {
            m_infectionWindow = new InfectionWindow();
            m_infectionWindow->setAttribute(Qt::WA_DeleteOnClose);
            connect(m_infectionWindow, &QObject::destroyed, this, [this]() { m_infectionWindow = nullptr; });
        }
        m_infectionWindow->addInfection(path, virus);

        // --- Beginn: sichere Protokollierung des infizierten Pfads ---
        {
            // Stelle sicher, dass ~/.antivirus existiert
            QString home = QDir::homePath();
            QDir adir(home + "/.antivirus");
            if (!adir.exists()) {
                if (!adir.mkpath(".")) {
                    logMessageQt(QString("Failed to create directory: %1").arg(adir.path()));
                }
            }

            // Datei zum Anhängen der infizierten Pfade (newline-separiert)
            QString infectedList = adir.filePath("infected_files.txt");
            QFile f(infectedList);
            if (f.open(QIODevice::Append | QIODevice::Text)) {
                QTextStream ts(&f);
                ts << path << "\n";
                f.close();
                // Setze restriktive Berechtigungen: nur Besitzer lesen/schreiben
                QFile::setPermissions(infectedList, QFileDevice::ReadOwner | QFileDevice::WriteOwner);
            } else {
                logMessageQt(QString("Could not open infected list for append: %1").arg(infectedList));
            }
        }
        // --- Ende: sichere Protokollierung des infizierten Pfads ---

        ++m_infectionCount;
        if (m_overlay) {
            m_overlay->setVirusCount(m_infectionCount);
            m_overlay->setCurrentFile(QFileInfo(path).fileName());
        }

        m_showWarningBadge = true;
        trayIcon->setIcon(virusIcon);

        if (!m_notifiedThisScan) {
            QString balloonTitle = QStringLiteral("ClamAV - Infection");
            QString balloonText = QString("%1 — %2").arg(QFileInfo(path).fileName(), virus);
            trayIcon->showMessage(balloonTitle, balloonText, QSystemTrayIcon::Warning, 4000);
            m_notifiedThisScan = true;
        }
    }

    void onScanFinished(const QString &path, bool infected, const QString &summary) {
        Q_UNUSED(path);
        trayIcon->setToolTip(baseTooltip);
        logMessageQt(QString("Final summary for %1:\n%2").arg(path, summary));
        if (infected && !m_notifiedThisScan) {
            trayIcon->showMessage("ClamAV Scan - Infection", "Infections found!\n" + summary, QSystemTrayIcon::Warning, 4000);
            m_notifiedThisScan = true;
        }
        m_scanningActive = false;
        if (!m_showWarningBadge) trayIcon->setIcon(baseIcon);

        m_lastIconPercent = -1.0;

        if (m_overlay) {
            m_overlay->setPercent(100.0);
            m_overlay->setCurrentFile(QString());
            m_overlay->setVirusCount(m_infectionCount);
        }
    }

    void onScanError(const QString &error) {
        trayIcon->showMessage("ClamAV Scan Error", error, QSystemTrayIcon::Critical, 5000);
        logMessageQt(QString("Scan error: %1").arg(error));
    }

    // Indexierungsfortschritt-Slot: zeigt Prozent (3 Nachkommastellen, mit Nullen aufgefüllt) vor Dateiname
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
        QString newTooltip = baseTooltip + "\n" + statusLine;
        trayIcon->setToolTip(newTooltip);

        logMessageQt(QString("Index progress %1/%2 - %3").arg(current).arg(total).arg(path));
    }

private:
    QSystemTrayIcon *trayIcon;
    QIcon baseIcon;
    QIcon virusIcon;
    QString buildInfo;
    QString startTime;
    QString baseTooltip;

    InfectionWindow *m_infectionWindow;
    ProgressOverlay *m_overlay;
    ClamAVScanner *scanner;

    bool m_notifiedThisScan;
    double m_lastIconPercent;
    bool m_showWarningBadge;
    int m_infectionCount;
    bool m_scanningActive;
};

// -----------------------------
// Utilities: PID file and autostart creation (sichere Varianten)

// PID file location under user's home
static QString pidFilePath() {
    return QDir::homePath() + "/.antivirus/antivir.pid";
}

static bool pidExists(pid_t pid) {
    if (pid <= 0) return false;
    // kill(pid, 0) prüft nur Existenz/Erreichbarkeit des Prozesses
    if (kill(pid, 0) == 0) return true;
    return false;
}

static bool readPidFile(pid_t &outPid) {
    QFile f(pidFilePath());
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QTextStream ts(&f);
    qint64 p = 0;
    ts >> p;
    outPid = static_cast<pid_t>(p);
    return (p > 0);
}

static bool writePidFile() {
    QDir dir = QFileInfo(pidFilePath()).absoluteDir();
    if (!dir.exists()) dir.mkpath(".");
    QFile f(pidFilePath());
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) return false;
    QTextStream ts(&f);
    ts << getpid() << "\n";
    f.close();
    QFile::setPermissions(pidFilePath(), QFileDevice::ReadOwner | QFileDevice::WriteOwner);
    return true;
}

static void removePidFile() {
    QFile::remove(pidFilePath());
}

// Create XDG autostart .desktop in ~/.config/autostart
static void ensureAutostartDesktop(const QString &execPath) {
    QString cfgDirPath = QDir::homePath() + "/.config/autostart";
    QDir cfgDir(cfgDirPath);
    if (!cfgDir.exists()) cfgDir.mkpath(".");

    QString desktopPath = cfgDir.filePath("antivir.desktop");
    QFile f(desktopPath);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream ts(&f);
        ts << "[Desktop Entry]\n";
        ts << "Type=Application\n";
        ts << "Name=Antivir\n";
        ts << "Exec=" << execPath << "\n";
        ts << "X-GNOME-Autostart-enabled=true\n";
        ts << "Comment=Startet Antivir Tray App beim Login\n";
        f.close();
        QFile::setPermissions(desktopPath, QFileDevice::ReadOwner | QFileDevice::WriteOwner);
        logMessageQt(QString("Wrote autostart desktop file: %1").arg(desktopPath));
    } else {
        logMessageQt(QString("Could not write autostart desktop file: %1").arg(desktopPath));
    }
}

// Signal handler to ensure PID file removal on termination
static void handleSignal(int sig) {
    Q_UNUSED(sig);
    removePidFile();
    // restore default and re-raise to allow normal termination if needed
    signal(sig, SIG_DFL);
    raise(sig);
}

// -----------------------------
// main
int main(int argc, char *argv[]) {
    // Minimal pre-check: if PID file exists and process is alive, exit (do not kill)
    pid_t existing = 0;
    if (readPidFile(existing) && pidExists(existing)) {
        fprintf(stderr, "Another instance is already running (pid=%d). Exiting.\n", existing);
        return 0;
    }

    // Fork for background/daemon-like behavior (optional)
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        // Parent exits, child continues
        exit(EXIT_SUCCESS);
    }

    // Child continues
    if (setsid() < 0) {
        // non-fatal, continue
    }
    // Optional: change working dir to home instead of "/" to avoid permission surprises for Qt
    chdir(QDir::homePath().toUtf8().constData());
    umask(0);

    // Write PID file for this instance
    if (!writePidFile()) {
        fprintf(stderr, "Could not write PID file. Exiting.\n");
        exit(EXIT_FAILURE);
    }

    // Install signal handlers to remove PID file on termination
    signal(SIGTERM, handleSignal);
    signal(SIGINT, handleSignal);
    signal(SIGHUP, handleSignal);

    // Start Qt application
    QApplication app(argc, argv);
    app.setApplicationName("antivir");
    app.setQuitOnLastWindowClosed(false);

    // Ensure autostart .desktop exists (safe, idempotent)
    ensureAutostartDesktop(QCoreApplication::applicationFilePath());

    // Ensure PID file is removed on normal exit
    QObject::connect(&app, &QCoreApplication::aboutToQuit, [](){
        removePidFile();
    });

    QString logPath = QDir::homePath() + "/.antivirus/antivirus.log";
    ClamAVTrayApp trayApp(logPath);

    int ret = app.exec();

    // Cleanup (redundant due to aboutToQuit handler)
    removePidFile();
    return ret;
}

#include "antivir.moc"

