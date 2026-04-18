// antivir.cpp
// GUI: scans all /proc-disclosed files plus running kernel image and kernel modules
// Uses libclamav directly (paranoid heuristics + archive parsing).
// Logs to ~/.antivirus/antivirus.log. Tray + overlay UI included.

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
#include <QFileInfo>
#include <QFile>
#include <QDateTime>
#include <QCoreApplication>
#include <QElapsedTimer>
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
#include <memory>
#include <cmath>
#include <cstring> // for memset

// libclamav
#include <clamav.h>

static const QString APP_VERSION = QStringLiteral("1.8.0-libclamav-proc-kernel-modules");

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
// Overlay window (semi-transparent, always on top)
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

        m_fileLabel = new QLabel(QStringLiteral("-"), this);
        QFont fileFont = m_fileLabel->font();
        fileFont.setPointSize(qMax(8, fileFont.pointSize()));
        m_fileLabel->setFont(fileFont);
        m_fileLabel->setStyleSheet("color: black;");
        m_fileLabel->setAlignment(Qt::AlignLeft);

        QHBoxLayout *h = new QHBoxLayout();
        h->setSpacing(8);

        m_progress = new QProgressBar(this);
        m_progress->setOrientation(Qt::Vertical);
        m_progress->setRange(0, 100000); // 0.000%..100.000%
        m_progress->setValue(0);
        m_progress->setTextVisible(false);
        m_progress->setFixedSize(25, 200);

        m_label = new QLabel("0.000%", this);
        QFont pctFont = m_label->font();
        pctFont.setBold(true);
        pctFont.setPointSize(qMax(10, pctFont.pointSize()));
        m_label->setFont(pctFont);
        m_label->setStyleSheet("color: black;");
        m_label->setAlignment(Qt::AlignCenter);

        h->addWidget(m_progress, 0, Qt::AlignVCenter);
        h->addWidget(m_label, 0, Qt::AlignVCenter);

        m_countLabel = new QLabel(QStringLiteral("Viren: 0"), this);
        QFont countFont = m_countLabel->font();
        countFont.setBold(true);
        m_countLabel->setFont(countFont);
        m_countLabel->setStyleSheet("color: black;");
        m_countLabel->setAlignment(Qt::AlignLeft);

        layout->addWidget(m_fileLabel);
        layout->addLayout(h);
        layout->addWidget(m_countLabel);

        setLayout(layout);
        adjustSize();

        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) {
            QRect geom = screen->availableGeometry();
            int margin = 12;
            move(geom.right() - width() - margin, geom.bottom() - height() - margin);
        }
    }

public slots:
    void setPercent(double percent) {
        double p = qBound(0.0, percent, 100.0);
        int scaled = static_cast<int>(qRound(p * 1000.0)); // 12.345% -> 12345
        m_progress->setValue(scaled);
        m_label->setText(QString::number(p, 'f', 3) + "%");
    }

    void setCurrentFile(const QString &file) {
        QString display = file;
        if (display.isEmpty()) display = QStringLiteral("-");
        const int maxLen = 64;
        if (display.length() > maxLen) display = QString("...%1").arg(display.right(maxLen));
        m_fileLabel->setText(display);
    }

    void setVirusCount(int count) {
        m_countLabel->setText(QStringLiteral("Viren: %1").arg(count));
    }

private:
    QProgressBar *m_progress;
    QLabel *m_label;
    QLabel *m_fileLabel;
    QLabel *m_countLabel;
};

// -----------------------------
// Scanner thread using libclamav
class ClamAVScanner : public QThread {
    Q_OBJECT
public:
    explicit ClamAVScanner(QObject *parent = nullptr) : QThread(parent),
        m_abort(false), m_totalFiles(0), m_scannedFiles(0),
        m_infectedCount(0), m_errorCount(0), m_skippedCount(0) {}

    void scanOpenFiles() { m_targetDescription = "running executables + their .so libraries + all /proc files + kernel+modules"; if (isRunning()) wait(); start(); }
    void abort() { m_abort = true; }

signals:
    void scanStarted(const QString &path);
    void scanFinished(const QString &path, bool infected, const QString &summary);
    void scanError(const QString &error);
    void scanProgress(int current, int total, const QString &file);
    void scanInfectionFound(const QString &path, const QString &virus);

protected:
    void run() override {
        m_abort = false;
        emit scanStarted(m_targetDescription);

        QStringList files;
        if (!collectAllProcFiles(files)) {
            emit scanError("Keine scanbaren Dateien aus /proc gefunden.");
            return;
        }

        // Add kernel image + modules
        collectKernelAndModules(files);

        m_totalFiles = files.size();
        m_scannedFiles = 0;
        m_infectedCount = 0;
        m_errorCount = 0;
        m_skippedCount = 0;

        struct cl_engine *engine = initClamEngine();
        if (!engine) {
            emit scanError("ClamAV Engine konnte nicht initialisiert werden.");
            return;
        }

        QElapsedTimer timer;
        timer.start();
        for (const QString &file : files) {
            if (m_abort) break;

            QString virusName;
            cl_error_t ret = scanFileWithLibclamav(engine, file, virusName);

            if (ret == CL_VIRUS) {
                ++m_infectedCount;
                ++m_scannedFiles;
                emit scanProgress(m_scannedFiles, m_totalFiles, file);
                emit scanInfectionFound(file, virusName);
                logMessageQt(QString("Infection: %1 — %2").arg(file, virusName));
            } else if (ret == CL_CLEAN) {
                ++m_scannedFiles;
                emit scanProgress(m_scannedFiles, m_totalFiles, file);
            } else {
                if (ret == CL_EOPEN || ret == CL_EACCES) {
                    ++m_skippedCount;
                } else {
                    ++m_errorCount;
                }
                ++m_scannedFiles;
                emit scanProgress(m_scannedFiles, m_totalFiles, file);
                logMessageQt(QString("Scan error on %1: %2").arg(file, QString::fromUtf8(cl_strerror(ret))));
            }

            if (timer.elapsed() > 24 * 60 * 60 * 1000) {
                logMessageQt("Overall scan timeout reached (24h). Aborting.");
                break;
            }
        }

        cl_engine_free(engine);

        QString summary;
        summary += QString("Total files to scan: %1\n").arg(m_totalFiles);
        summary += QString("Scanned files: %1\n").arg(m_scannedFiles);
        summary += QString("Infected: %1\n").arg(m_infectedCount);
        summary += QString("Errors: %1\n").arg(m_errorCount);
        summary += QString("Skipped (no read): %1\n").arg(m_skippedCount);

        logMessageQt(QString("Scan summary for %1:\n%2").arg(m_targetDescription, summary));
        emit scanFinished(m_targetDescription, m_infectedCount > 0, summary);
    }

private:
    QString m_targetDescription;
    bool m_abort;
    int m_totalFiles;
    int m_scannedFiles;
    int m_infectedCount;
    int m_errorCount;
    int m_skippedCount;

    // ---- Helpers ----
    static bool isPathSkippable(const QString &path) {
        if (path.isEmpty()) return true;
        if (path.contains(" (deleted)")) return true;
        if (path.startsWith("socket:")) return true;
        if (path.startsWith("pipe:")) return true;
        if (path.startsWith("anon_inode:")) return true;
        if (path.startsWith("memfd:")) return true;
        if (path.startsWith("/dev/null")) return true;
        if (path.startsWith("/dev/zero")) return true;
        if (path.startsWith("[") && path.endsWith("]")) return true; // [vdso], [heap], ...
        if (path.startsWith("/proc/")) return true; // virtual
        return false;
    }

    // Collect all real files disclosed by /proc (fd, task fds, exe, maps)
    bool collectAllProcFiles(QStringList &outFiles) {
        outFiles.clear();
        QSet<QString> seen;

        QDir procDir("/proc");
        QStringList procEntries = procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

        for (const QString &entry : procEntries) {
            if (m_abort) break;
            bool isPid = false;
            entry.toInt(&isPid);
            if (!isPid) continue;

            const QString pid = entry;

            // 1) /proc/PID/fd
            collectFdDir(QString("/proc/%1/fd").arg(pid), seen, outFiles);

            // 2) /proc/PID/task/*/fd
            collectTaskFds(pid, seen, outFiles);

            // 3) /proc/PID/exe (running executable)
            collectExeLink(pid, seen, outFiles);

            // 4) /proc/PID/maps (mapped files: libraries, binaries, other regular files)
            collectMapsFile(pid, seen, outFiles);
        }

        if (outFiles.isEmpty()) {
            logMessageQt("No scanable files collected from /proc.");
            return false;
        }

        logMessageQt(QString("Prepared /proc file list (count=%1)").arg(outFiles.size()));
        return true;
    }

    void collectFdDir(const QString &fdDirPath, QSet<QString> &seen, QStringList &outFiles) {
        QDir fdDir(fdDirPath);
        if (!fdDir.exists()) return;
        QFileInfoList fdInfos = fdDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
        for (const QFileInfo &fdInfo : fdInfos) {
            if (m_abort) break;
            if (!fdInfo.isSymLink()) continue;
            QString target = fdInfo.symLinkTarget();
            if (isPathSkippable(target)) continue;

            QFileInfo real(target);
            if (!real.exists() || !real.isFile()) continue;
            QString absPath = real.absoluteFilePath();
            if (absPath.isEmpty()) continue;

            if (!seen.contains(absPath)) {
                seen.insert(absPath);
                outFiles << absPath;
            }
        }
    }

    void collectTaskFds(const QString &pid, QSet<QString> &seen, QStringList &outFiles) {
        QDir taskDir(QString("/proc/%1/task").arg(pid));
        if (!taskDir.exists()) return;
        QStringList tids = taskDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &tid : tids) {
            if (m_abort) break;
            bool isTid = false;
            tid.toInt(&isTid);
            if (!isTid) continue;
            collectFdDir(QString("/proc/%1/task/%2/fd").arg(pid, tid), seen, outFiles);
        }
    }

    void collectExeLink(const QString &pid, QSet<QString> &seen, QStringList &outFiles) {
        QString exeLink = QString("/proc/%1/exe").arg(pid);
        QFileInfo exeInfo(exeLink);
        if (!exeInfo.isSymLink()) return;
        QString exePath = exeInfo.symLinkTarget();
        if (isPathSkippable(exePath)) return;

        QFileInfo real(exePath);
        if (!real.exists() || !real.isFile()) return;
        QString absPath = real.absoluteFilePath();
        if (absPath.isEmpty() || seen.contains(absPath)) return;

        seen.insert(absPath);
        outFiles << absPath;
    }

    void collectMapsFile(const QString &pid, QSet<QString> &seen, QStringList &outFiles) {
        QString mapsPath = QString("/proc/%1/maps").arg(pid);
        QFile maps(mapsPath);
        if (!maps.open(QIODevice::ReadOnly | QIODevice::Text)) return;

        while (!maps.atEnd() && !m_abort) {
            QByteArray line = maps.readLine();
            int pathStart = line.indexOf('/');
            if (pathStart < 0) continue;

            QString path = QString::fromUtf8(line.mid(pathStart)).trimmed();
            if (isPathSkippable(path)) continue;

            QFileInfo fi(path);
            if (!fi.exists() || !fi.isFile()) continue;

            QString abs = fi.absoluteFilePath();
            if (abs.isEmpty() || seen.contains(abs)) continue;

            seen.insert(abs);
            outFiles << abs;
        }
        maps.close();
    }

    // ---- Kernel + modules collection ----
    void collectKernelAndModules(QStringList &outFiles) {
        QSet<QString> seen(outFiles.begin(), outFiles.end());

        // 1) Read kernel release
        QString release;
        QFile relFile("/proc/sys/kernel/osrelease");
        if (relFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            release = QString::fromUtf8(relFile.readAll()).trimmed();
            relFile.close();
        }

        // 2) Candidate kernel image paths
        QStringList kernelCandidates;
        if (!release.isEmpty()) {
            kernelCandidates << QString("/boot/vmlinuz-%1").arg(release);
            kernelCandidates << QString("/lib/modules/%1/vmlinuz").arg(release); // sometimes present
        }
        kernelCandidates << "/boot/vmlinuz";
        kernelCandidates << "/vmlinuz";
        kernelCandidates << "/boot/kernel"; // fallback

        for (const QString &kpath : kernelCandidates) {
            if (m_abort) break;
            QFileInfo kfi(kpath);
            if (kfi.exists() && kfi.isFile()) {
                QString abs = kfi.absoluteFilePath();
                if (!seen.contains(abs)) {
                    seen.insert(abs);
                    outFiles << abs;
                    logMessageQt(QString("Added kernel image for scanning: %1").arg(abs));
                }
            }
        }

        // 3) Modules: prefer installed modules under /lib/modules/<release>
        QString modulesBase;
        if (!release.isEmpty()) modulesBase = QString("/lib/modules/%1").arg(release);
        if (modulesBase.isEmpty() || !QDir(modulesBase).exists()) {
            // fallback: scan /lib/modules
            modulesBase = "/lib/modules";
        }

        if (QDir(modulesBase).exists()) {
            QDirIterator it(modulesBase, QStringList() << "*.ko" << "*.ko.xz" << "*.ko.gz", QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext() && !m_abort) {
                QString modPath = it.next();
                QFileInfo mfi(modPath);
                if (!mfi.exists() || !mfi.isFile()) continue;
                QString abs = mfi.absoluteFilePath();
                if (!seen.contains(abs)) {
                    seen.insert(abs);
                    outFiles << abs;
                }
            }
            logMessageQt(QString("Added kernel modules from %1").arg(modulesBase));
        }

        // 4) Also try to map loaded modules from /proc/modules and match to files under /lib/modules/<release>
        QFile procModules("/proc/modules");
        if (procModules.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QByteArray content = procModules.readAll();
            procModules.close();
            QStringList lines = QString::fromUtf8(content).split('\n', Qt::SkipEmptyParts);
            for (const QString &ln : lines) {
                if (m_abort) break;
                // /proc/modules format: name size instances deps state offset
                QStringList parts = ln.split(' ', Qt::SkipEmptyParts);
                if (parts.isEmpty()) continue;
                QString modName = parts[0].trimmed();
                if (modName.isEmpty()) continue;

                // Search for files named like modName.ko* under modulesBase
                if (QDir(modulesBase).exists()) {
                    QDirIterator mit(modulesBase, QDirIterator::Subdirectories);
                    while (mit.hasNext() && !m_abort) {
                        QString candidate = mit.next();
                        QFileInfo cfi(candidate);
                        if (!cfi.exists() || !cfi.isFile()) continue;
                        QString base = cfi.completeBaseName(); // name without compression suffix
                        // match module name prefix (module names may be like 'xt_conntrack' and file 'xt_conntrack.ko')
                        if (cfi.suffix().startsWith("ko") || cfi.fileName().contains(".ko")) {
                            // check if filename starts with module name
                            if (cfi.fileName().startsWith(modName)) {
                                QString abs = cfi.absoluteFilePath();
                                if (!seen.contains(abs)) {
                                    seen.insert(abs);
                                    outFiles << abs;
                                    logMessageQt(QString("Added loaded module %1 -> %2").arg(modName, abs));
                                }
                            }
                        } else {
                            // also check compressed .ko.xz/.gz names
                            QString fname = cfi.fileName();
                            if (fname.startsWith(modName) && (fname.contains(".ko") || fname.endsWith(".xz") || fname.endsWith(".gz"))) {
                                QString abs = cfi.absoluteFilePath();
                                if (!seen.contains(abs)) {
                                    seen.insert(abs);
                                    outFiles << abs;
                                    logMessageQt(QString("Added loaded module (compressed) %1 -> %2").arg(modName, abs));
                                }
                            }
                        }
                    }
                }
            }
        }

        // 5) Deduplicate and log count
        logMessageQt(QString("Kernel+modules added: %1").arg(seen.size() - outFiles.size() + outFiles.size())); // best-effort log
    }

    // ---- libclamav: engine init ----
    static struct cl_engine* initClamEngine() {
        cl_error_t ret = cl_init(CL_INIT_DEFAULT);
        if (ret != CL_SUCCESS) {
            qWarning() << "cl_init failed:" << cl_strerror(ret);
            return nullptr;
        }

        struct cl_engine *engine = cl_engine_new();
        if (!engine) {
            qWarning() << "cl_engine_new failed";
            return nullptr;
        }

        unsigned int sigs = 0;
        ret = cl_load(cl_retdbdir(), engine, &sigs, CL_DB_STDOPT);
        if (ret != CL_SUCCESS) {
            qWarning() << "cl_load failed:" << cl_strerror(ret);
            cl_engine_free(engine);
            return nullptr;
        }

        ret = cl_engine_compile(engine);
        if (ret != CL_SUCCESS) {
            qWarning() << "cl_engine_compile failed:" << cl_strerror(ret);
            cl_engine_free(engine);
            return nullptr;
        }

        qDebug() << "libclamav signatures loaded:" << sigs;
        return engine;
    }

    // ---- libclamav: scan single file (paranoid + archive) ----
    static cl_error_t scanFileWithLibclamav(struct cl_engine *engine,
                                            const QString &path,
                                            QString &virusName) {
        const char *virname = nullptr;

#ifdef CL_SCAN_STDOPT
        cl_error_t ret = (cl_error_t)cl_scanfile(path.toUtf8().constData(),
                                                 &virname,
                                                 nullptr,
                                                 engine,
                                                 CL_SCAN_STDOPT);
#else
        struct cl_scan_options opts;
        std::memset(&opts, 0, sizeof(opts));

        // Paranoid + Heuristics + All matches
        opts.general |= CL_SCAN_GENERAL_HEURISTICS;
        opts.general |= CL_SCAN_GENERAL_ALLMATCHES;
        //opts.general |= CL_SCAN_GENERAL_PARANOID;

        // Archive parsing
        opts.parse |= CL_SCAN_PARSE_ARCHIVE;

        size_t scanned = 0;
        cl_error_t ret = cl_scanfile(path.toUtf8().constData(),
                                     &virname,
                                     &scanned,
                                     engine,
                                     &opts);
#endif
        if (ret == CL_VIRUS) {
            virusName = virname ? QString::fromUtf8(virname) : QStringLiteral("Unknown");
        }
        return ret;
    }
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
        QAction *scanOpenFilesAction = new QAction("Alle via /proc bekannten Dateien scannen (inkl. laufender Kernel + Module)", menu);
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

        connect(scanOpenFilesAction, &QAction::triggered, this, &ClamAVTrayApp::onScanOpenFiles);
        connect(showInfectionsAction, &QAction::triggered, this, &ClamAVTrayApp::onShowInfections);
        connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

        connect(scanner, &ClamAVScanner::scanStarted, this, &ClamAVTrayApp::onScanStarted);
        connect(scanner, &ClamAVScanner::scanFinished, this, &ClamAVTrayApp::onScanFinished);
        connect(scanner, &ClamAVScanner::scanError, this, &ClamAVTrayApp::onScanError);
        connect(scanner, &ClamAVScanner::scanProgress, this, &ClamAVTrayApp::onScanProgress);
        connect(scanner, &ClamAVScanner::scanInfectionFound, this, &ClamAVTrayApp::onScanInfectionFound);

        trayIcon->showMessage("ClamAV Scan", "Initialer Scan aller via /proc bekannten Dateien startet", QSystemTrayIcon::Information, 3000);
        logMessageQt(QString("Starting initial scan of all /proc-disclosed files (including running kernel and modules)"));
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
        trayIcon->showMessage("ClamAV Scan", "Starte Scan aller via /proc bekannten Dateien", QSystemTrayIcon::Information, 3000);
        logMessageQt(QString("User started scan of /proc-disclosed files"));
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
};

// -----------------------------
// main
int main(int argc, char *argv[]) {
    QString logDir = QDir::homePath() + "/.antivirus";
    QDir().mkpath(logDir);
    QString logPath = logDir + "/antivirus.log";

    QApplication app(argc, argv);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, "ClamAV Scanner", "System tray is not available on this system.");
        return 1;
    }

    QApplication::setQuitOnLastWindowClosed(false);

    ClamAVTrayApp trayApp(logPath);

    return app.exec();
}

#include "antivir.moc"

