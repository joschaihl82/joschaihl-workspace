// antivir.cpp
// Single-binary: GUI mode (default). Uses clamscan to scan the list of currently open files
// and writes logs to ~/.antivirus/antivirus.log.
// Scans ONLY currently opened files (from /proc/*/fd). Prints each clamscan output line to console (tee) and logs it.
// Tray icon: uses an unchanged shield icon for normal/active states and switches to a virus icon when infections are found.
// Overlay window (always on top, semi-transparent) shows detailed progress: percent label with three decimals,
// filename (no path), and infection count. The vertical progress bar was removed and replaced by a single status label:
// "Viren: <count> - <percent%> - <filename>" positioned bottom-right.

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QThread>
#include <QIcon>
#include <QTimer>
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
#include <QPainter>
#include <QFontMetrics>
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
#include <QLinearGradient>
#include <QProgressBar>
#include <QWidget>
#include <QScreen>
#include <QTemporaryFile>
#include <QSet>
#include <memory>
#include <cmath>

static const QString APP_VERSION = QStringLiteral("1.1.0");

// -----------------------------
// Logging (RAII)
static std::unique_ptr<QFile> g_logFilePtr;
static std::unique_ptr<QTextStream> g_logStreamPtr;
static bool g_logOpen = false;

static void openLogFileQt(const QString &logPath) {
    if (g_logOpen) return;
    g_logFilePtr = std::make_unique<QFile>(logPath);
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

        m_descriptions.insert("Eicar-Test-Signature", "EICAR test file: harmless test signature used to verify antivirus functionality.");
        m_descriptions.insert("Win.Trojan.Generic", "Generic Trojan detection: may indicate a trojanous executable. Investigate file origin and quarantine.");
        m_descriptions.insert("HTML.Phishing", "Phishing HTML content detected. Contains suspicious links or forms attempting credential theft.");
        m_descriptions.insert("PUA", "Potentially Unwanted Application: may be adware or software with unwanted behavior.");
        m_descriptions.insert("Unknown", "No detailed description available for this signature. Check logs and vendor resources.");
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
        QString details = QString("<b>Signature:</b> %1<br><br><b>Beschreibung:</b><br>%2").arg(virus.toHtmlEscaped(), desc.toHtmlEscaped());
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
        m_abort(false), m_totalFiles(0), m_scannedFiles(0),
        m_infectedCount(0), m_errorCount(0), m_skippedCount(0) {}

    void scanOpenFiles() { m_targetDescription = "open files"; if (isRunning()) wait(); start(); }
    void abort() { m_abort = true; }

signals:
    void scanStarted(const QString &path);
    void scanFinished(const QString &path, bool infected, const QString &summary);
    void scanError(const QString &error);
    void scanProgress(int current, int total, const QString &file);
    void scanLineOutput(const QString &line);
    void scanInfectionFound(const QString &path, const QString &virus);

protected:
    void run() override {
        m_abort = false;
        emit scanStarted(m_targetDescription);

        QString listFilePath;
        int listedCount = 0;
        if (!buildOpenFilesList(listFilePath, listedCount)) {
            emit scanError("Keine geöffneten Dateien gefunden oder Liste konnte nicht erstellt werden.");
            return;
        }

        m_totalFiles = listedCount;
        m_scannedFiles = 0;
        m_infectedCount = 0;
        m_errorCount = 0;
        m_skippedCount = 0;

        QString details;
        bool infected = false;

        if (!startAndParseClamscanWithList(listFilePath, details, infected)) {
            emit scanError("clamscan konnte mit der Datei-Liste nicht gestartet werden: " + listFilePath);
            QFile::remove(listFilePath);
            return;
        }

        QString summary;
        summary += QString("Listed open files: %1\n").arg(m_totalFiles);
        summary += QString("Scanned files: %1\n").arg(m_scannedFiles);
        summary += QString("Infected: %1\n").arg(m_infectedCount);
        summary += QString("Errors: %1\n").arg(m_errorCount);
        summary += QString("Skipped (no read): %1\n").arg(m_skippedCount);
        if (!details.isEmpty()) summary += QString("\nDetails:\n") + details;

        logMessageQt(QString("Scan summary for %1:\n%2").arg(m_targetDescription, summary));
        emit scanFinished(m_targetDescription, infected, summary);

        QFile::remove(listFilePath);
    }

private:
    QString m_targetDescription;
    bool m_abort;
    int m_totalFiles;
    int m_scannedFiles;
    int m_infectedCount;
    int m_errorCount;
    int m_skippedCount;

    // Build a list of currently open regular files using /proc/*/fd symlinks.
    // Writes unique absolute file paths to a temporary file and returns the path and count.
    bool buildOpenFilesList(QString &outPath, int &outCount) {
        outPath.clear();
        outCount = 0;

        // Create a temp file path in /tmp (we need a named file for clamscan --file-list).
        QString tempPath = QDir::temp().absoluteFilePath(QString("openfiles_%1_%2.lst")
                                .arg(QCoreApplication::applicationName().isEmpty() ? "antivir" : QCoreApplication::applicationName())
                                .arg(QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz")));
        QFile outFile(tempPath);
        if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            logMessageQt(QString("Failed to open temp list file: %1").arg(tempPath));
            return false;
        }
        QTextStream ts(&outFile);

        QSet<QString> seen;
        QDir procDir("/proc");
        QStringList procEntries = procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &entry : procEntries) {
            if (m_abort) break;
            bool isPid = false;
            entry.toInt(&isPid);
            if (!isPid) continue;

            QString fdDirPath = "/proc/" + entry + "/fd";
            QDir fdDir(fdDirPath);
            if (!fdDir.exists()) continue;

            QFileInfoList fdInfos = fdDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
            for (const QFileInfo &fdInfo : fdInfos) {
                if (m_abort) break;

                // fd entries are symlinks to targets
                if (!fdInfo.isSymLink()) continue;
                QString target = fdInfo.symLinkTarget();

                // Filter out non-file targets (pipes, sockets, anon-inode, deleted markers)
                if (target.isEmpty()) continue;
                if (target.startsWith("socket:") || target.startsWith("pipe:")
                    || target.startsWith("anon_inode:")
                    || target == "/dev/null" || target.startsWith("/proc/")
                    || target.startsWith("inotify")
                    || target.contains("(deleted)")) {
                    continue;
                }

                QFileInfo real(target);
                if (!real.exists() || !real.isFile()) continue;

                QString absPath = real.absoluteFilePath();
                if (absPath.isEmpty()) continue;

                if (!seen.contains(absPath)) {
                    seen.insert(absPath);
                    ts << absPath << "\n";
                    ++outCount;
                }
            }
        }

        outFile.flush();
        outFile.close();

        if (outCount == 0) {
            QFile::remove(tempPath);
            logMessageQt("No open files collected from /proc.");
            return false;
        }

        outPath = tempPath;
        logMessageQt(QString("Prepared open files list: %1 (count=%2)").arg(outPath).arg(outCount));
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

        // Use --file-list to scan exactly the collected open files.
        QStringList args;
        args << "--no-summary" << "--stdout" << "--suppress-ok-results"
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

                    emit scanLineOutput(sline);
                    fprintf(stdout, "%s\n", sline.toLocal8Bit().constData());
                    fflush(stdout);
                    logMessageQt(QString("clamscan: %1").arg(sline));

                    int colon = sline.indexOf(':');
                    if (colon > 0) {
                        QString maybePath = sline.left(colon).trimmed();
                        QString rest = sline.mid(colon + 1).trimmed();
                        if (rest.endsWith("FOUND")) {
                            QString virus = rest;
                            virus = virus.replace("FOUND", "").trimmed();
                            details += QString("Infected file: %1 - Virus: %2\n").arg(maybePath, virus);
                            foundInfected = true;
                            ++m_infectedCount;
                            ++m_scannedFiles;
                            emit scanProgress(m_scannedFiles, m_totalFiles, maybePath);
                            emit scanInfectionFound(maybePath, virus);
                        } else if (rest.endsWith("OK")) {
                            ++m_scannedFiles;
                            emit scanProgress(m_scannedFiles, m_totalFiles, maybePath);
                        } else {
                            // clamscan may also print lines like "ERROR: ..."
                            if (rest.startsWith("Error") || rest.startsWith("ERROR")) {
                                ++m_errorCount;
                            } else {
                                logMessageQt(QString("clamscan unexpected output: %1").arg(sline));
                                sawUnexpected = true;
                            }
                        }
                    } else {
                        logMessageQt(QString("clamscan unexpected output (no colon): %1").arg(sline));
                        sawUnexpected = true;
                    }
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

        buffer += proc.readAllStandardOutput();
        if (!buffer.isEmpty()) {
            QList<QByteArray> lines = buffer.split('\n');
            for (const QByteArray &line : lines) {
                QString sline = QString::fromUtf8(line).trimmed();
                if (sline.isEmpty()) continue;

                emit scanLineOutput(sline);
                fprintf(stdout, "%s\n", sline.toLocal8Bit().constData());
                fflush(stdout);
                logMessageQt(QString("clamscan: %1").arg(sline));
                int colon = sline.indexOf(':');
                if (colon > 0) {
                    QString maybePath = sline.left(colon).trimmed();
                    QString rest = sline.mid(colon + 1).trimmed();
                    if (rest.endsWith("FOUND")) {
                        QString virus = rest;
                        virus = virus.replace("FOUND", "").trimmed();
                        details += QString("Infected file: %1 - Virus: %2\n").arg(maybePath, virus);
                        foundInfected = true;
                        ++m_infectedCount;
                        ++m_scannedFiles;
                        emit scanProgress(m_scannedFiles, m_totalFiles, maybePath);
                        emit scanInfectionFound(maybePath, virus);
                    } else if (rest.endsWith("OK")) {
                        ++m_scannedFiles;
                        emit scanProgress(m_scannedFiles, m_totalFiles, maybePath);
                    } else {
                        if (rest.startsWith("Error") || rest.startsWith("ERROR")) {
                            ++m_errorCount;
                        } else {
                            logMessageQt(QString("clamscan unexpected output: %1").arg(sline));
                            sawUnexpected = true;
                        }
                    }
                } else {
                    logMessageQt(QString("clamscan unexpected output (no colon): %1").arg(sline));
                    sawUnexpected = true;
                }
            }
        }

        if (sawUnexpected) ++m_errorCount;
        outInfected = foundInfected;
        return true;
    }
};

// -----------------------------
// Overlay window with combined status label (no progressbar)
// Shows: "Viren: <count> - <percent%> - <filename>" in bottom-right
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

        // Single combined status label
        m_statusLabel = new QLabel(QStringLiteral("Viren: 0 - 0.000% - -"), this);
        QFont statusFont = m_statusLabel->font();
        statusFont.setBold(true);
        statusFont.setPointSize(qMax(10, statusFont.pointSize()));
        m_statusLabel->setFont(statusFont);
        m_statusLabel->setStyleSheet("color: black; background: rgba(255,255,255,200); padding:4px; border-radius:6px;");
        m_statusLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_statusLabel->setTextInteractionFlags(Qt::NoTextInteraction);
        m_statusLabel->setWordWrap(false);
        m_statusLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

        layout->addWidget(m_statusLabel);
        setLayout(layout);
        adjustSize();

        // Position bottom-right on primary screen
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) {
            QRect geom = screen->availableGeometry();
            int margin = 12;
            move(geom.right() - width() - margin, geom.bottom() - height() - margin);
        }
    }

public slots:
    // percent in 0..100.0
    void setPercent(double percent) {
        m_percent = qBound(0.0, percent, 100.0);
        updateStatusText();
    }

    // show the currently scanned filename (no path)
    void setCurrentFile(const QString &file) {
        QString display = QFileInfo(file).fileName();
        if (display.isEmpty()) display = QStringLiteral("-");
        const int maxLen = 64;
        if (display.length() > maxLen) {
            display = QString("...%1").arg(display.right(maxLen));
        }
        m_currentFile = display;
        updateStatusText();
    }

    // show number of infections found so far
    void setVirusCount(int count) {
        m_virusCount = count;
        updateStatusText();
    }

protected:
    // ensure overlay stays bottom-right if screen geometry changes or on show
    void showEvent(QShowEvent *ev) override {
        QWidget::showEvent(ev);
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) {
            QRect geom = screen->availableGeometry();
            int margin = 12;
            move(geom.right() - width() - margin, geom.bottom() - height() - margin);
        }
    }

private:
    void updateStatusText() {
        QString pct = QString::number(m_percent, 'f', 3) + "%";
        QString text = QStringLiteral("Viren: %1 - %2 - %3").arg(m_virusCount).arg(pct, m_currentFile);
        m_statusLabel->setText(text);
        adjustSize();
        // keep bottom-right position after resize
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) {
            QRect geom = screen->availableGeometry();
            int margin = 12;
            move(geom.right() - width() - margin, geom.bottom() - height() - margin);
        }
    }

    QLabel *m_statusLabel;
    double m_percent = 0.0;
    QString m_currentFile = QStringLiteral("-");
    int m_virusCount = 0;
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
        QAction *scanOpenFilesAction = new QAction("Geöffnete Dateien scannen", menu);
        QAction *showInfectionsAction = new QAction("Virenfunde anzeigen", menu);
        QAction *quitAction = new QAction("Quit", menu);
        menu->addAction(scanOpenFilesAction);
        menu->addAction(showInfectionsAction);
        menu->addSeparator();
        menu->addAction(quitAction);
        trayIcon->setContextMenu(menu);

        buildInfo = QStringLiteral("%1 (built %2 %3)").arg(APP_VERSION, QString::fromUtf8(__DATE__), QString::fromUtf8(__TIME__));
        startTime = QDateTime::currentDateTime().toString(Qt::ISODate);
        baseTooltip = QStringLiteral("ClamAV Qt Scanner\nVersion: %1\nStarted: %2").arg(buildInfo, startTime);
        trayIcon->setToolTip(baseTooltip);

        // Load icons from theme (do not modify them)
        baseIcon = QIcon::fromTheme("security-high");
        if (baseIcon.isNull()) baseIcon = QIcon::fromTheme("dialog-information"); // fallback
        virusIcon = QIcon::fromTheme("emblem-danger");
        if (virusIcon.isNull()) virusIcon = QIcon::fromTheme("dialog-warning"); // fallback

        trayIcon->setIcon(baseIcon);
        trayIcon->show();

        // Create overlay window once (semi-transparent)
        m_overlay = new ProgressOverlay();
        m_overlay->setWindowOpacity(0.80); // 80% opaque (20% transparent)
        m_overlay->show();
        // initialize overlay
        m_overlay->setPercent(0.0);
        m_overlay->setCurrentFile(QString());
        m_overlay->setVirusCount(0);
        m_infectionCount = 0;

        scanner = new ClamAVScanner(this);

        connect(scanOpenFilesAction, &QAction::triggered, this, &ClamAVTrayApp::onScanOpenFiles);
        connect(showInfectionsAction, &QAction::triggered, this, &ClamAVTrayApp::onShowInfections);
        connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
        connect(scanner, &ClamAVScanner::scanStarted, this, &ClamAVTrayApp::onScanStarted);
        connect(scanner, &ClamAVScanner::scanFinished, this, &ClamAVTrayApp::onScanFinished);
        connect(scanner, &ClamAVScanner::scanError, this, &ClamAVTrayApp::onScanError);
        connect(scanner, &ClamAVScanner::scanProgress, this, &ClamAVTrayApp::onScanProgress);
        connect(scanner, &ClamAVScanner::scanLineOutput, this, &ClamAVTrayApp::onScanLineOutput);
        connect(scanner, &ClamAVScanner::scanInfectionFound, this, &ClamAVTrayApp::onScanInfectionFound);

        trayIcon->showMessage("ClamAV Scan", "Initialer Scan der geöffneten Dateien startet", QSystemTrayIcon::Information, 3000);
        logMessageQt(QString("Starting initial scan of open files"));
        scanner->scanOpenFiles();
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
    void onScanOpenFiles() {
        trayIcon->showMessage("ClamAV Scan", "Starte Scan der geöffneten Dateien", QSystemTrayIcon::Information, 3000);
        logMessageQt(QString("User started scan of open files"));
        scanner->scanOpenFiles();
    }

    void onShowInfections() {
        if (!m_infectionWindow) {
            m_infectionWindow = new InfectionWindow(); // parentless
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
        // reset overlay and counters
        m_infectionCount = 0;
        if (m_overlay) {
            m_overlay->setPercent(0.0);
            m_overlay->setCurrentFile(QString());
            m_overlay->setVirusCount(m_infectionCount);
        }
        // Keep the tray icon unchanged (base shield) for active scan
        trayIcon->setIcon(baseIcon);
    }

    void onScanProgress(int current, int total, const QString &file) {
        static double lastPct = -1.0;
        static qint64 lastUpdateMs = 0;

        QString shortName = QFileInfo(file).fileName();
        QString statusLine;

        if (total > 0) {
            double percent = (static_cast<double>(current) / static_cast<double>(total)) * 100.0;
            QString pctStr = QString::number(percent, 'f', 3); // three decimal places

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

            // update overlay progress and current filename (no path)
            if (m_overlay) {
                m_overlay->setPercent(percent);
                m_overlay->setCurrentFile(QFileInfo(file).fileName());
                m_overlay->setVirusCount(m_infectionCount);
            }

            m_lastIconPercent = percent;
        } else {
            statusLine = QStringLiteral("Scanning: %1 - %2").arg(current).arg(shortName);
            QString newTooltip = baseTooltip + "\n" + statusLine;
            trayIcon->setToolTip(newTooltip);
            if (m_overlay) m_overlay->setCurrentFile(QFileInfo(file).fileName());
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

    void onScanInfectionFound(const QString &path, const QString &virus) {
        if (!m_infectionWindow) {
            m_infectionWindow = new InfectionWindow();
            m_infectionWindow->setAttribute(Qt::WA_DeleteOnClose);
            connect(m_infectionWindow, &QObject::destroyed, this, [this]() { m_infectionWindow = nullptr; });
        }
        m_infectionWindow->addInfection(path, virus);

        // increment infection counter (UI-thread)
        ++m_infectionCount;
        if (m_overlay) {
            m_overlay->setVirusCount(m_infectionCount);
            m_overlay->setCurrentFile(QFileInfo(path).fileName()); // show filename only
        }

        // switch to virus icon (do not redraw or modify base icon)
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
        // If infections were found, ensure virus icon remains; otherwise restore base icon
        if (m_infectionCount > 0 || infected) {
            trayIcon->setIcon(virusIcon);
            if (!m_notifiedThisScan) {
                trayIcon->showMessage("ClamAV Scan - Infection", QString("Infections found!\n%1").arg(summary), QSystemTrayIcon::Warning, 6000);
                m_notifiedThisScan = true;
            }
        } else {
            trayIcon->setIcon(baseIcon);
            trayIcon->showMessage("ClamAV Scan", "Scan completed: no infections found", QSystemTrayIcon::Information, 3000);
        }

        m_scanningActive = false;

        // Update overlay to final state (100% if we had a percent, otherwise leave)
        if (m_overlay) {
            // If we had a last percent, keep it; otherwise set to 100.000%
            if (m_lastIconPercent >= 0.0) {
                m_overlay->setPercent(m_lastIconPercent);
            } else {
                m_overlay->setPercent(100.0);
            }
            m_overlay->setVirusCount(m_infectionCount);
            // keep current file as "-" to indicate finished
            m_overlay->setCurrentFile(QString());
        }
    }

    void onScanError(const QString &error) {
        logMessageQt(QString("Scan error: %1").arg(error));
        trayIcon->showMessage("ClamAV Scan - Error", error, QSystemTrayIcon::Critical, 5000);
        m_scanningActive = false;
        if (m_overlay) {
            m_overlay->setCurrentFile(QString());
        }
    }

private:
    QSystemTrayIcon *trayIcon;
    QIcon baseIcon;
    QIcon virusIcon;
    QString buildInfo;
    QString startTime;
    QString baseTooltip;

    ClamAVScanner *scanner;
    bool m_notifiedThisScan;
    double m_lastIconPercent;
    InfectionWindow *m_infectionWindow;
    ProgressOverlay *m_overlay;
    bool m_showWarningBadge;
    int m_infectionCount;
    bool m_scanningActive;
};

// -----------------------------
// main
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("antivir"));
    QCoreApplication::setApplicationVersion(APP_VERSION);

    // Ensure log directory exists
    QString logDirPath = QDir::homePath() + "/.antivirus";
    QDir logDir(logDirPath);
    if (!logDir.exists()) {
        logDir.mkpath(".");
    }
    QString logPath = logDirPath + "/antivirus.log";

    ClamAVTrayApp *trayApp = new ClamAVTrayApp(logPath);

    int ret = app.exec();

    delete trayApp;
    return ret;
}

#include "antivir.moc"

