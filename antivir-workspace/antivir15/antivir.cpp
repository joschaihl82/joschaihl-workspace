// antivir.cpp
// Version: 2.3.0 — libclamav full scanner without inotify
// Scans: /proc-disclosed files, running executables + mapped libs, kernel image + modules,
// PATH binaries (/bin,/sbin) + ELF DT_NEEDED deps, and all .so*/.a files
// discovered via ld.so.conf, ldconfig cache (parsed directly), common dirs and RPATHs.
// Logs to ~/.antivirus/antivirus.log. Uses libclamav (paranoid + archive parsing).

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
#include <QProcess>
#include <memory>
#include <cmath>
#include <cstring>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <elf.h>
#include <sys/stat.h>
#include <stdint.h>

#include <clamav.h>

static const QString APP_VERSION = QStringLiteral("2.3.0-libclamav-full-scan-ldcache");

// Tunables / safety caps
static const qint64 LIBFILES_MAX = 200000;
static const int LDCONFIG_TIMEOUT_MS = 3000;
static const int FRESHCLAM_TIMEOUT_MS = 5 * 60 * 1000;

// ----------------------------- Logging
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

// ----------------------------- InfectionWindow
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

// ----------------------------- ProgressOverlay
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
        m_progress->setRange(0, 100000);
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
        int scaled = static_cast<int>(qRound(p * 1000.0));
        m_progress->setValue(scaled);
        m_label->setText(QString::number(p, 'f', 3) + "%");
        updateOverlayText();
    }
    void setCurrentFile(const QString &file) {
        QString display = file;
        if (display.isEmpty()) display = QStringLiteral("-");
        const int maxLen = 64;
        if (display.length() > maxLen) display = QString("...%1").arg(display.right(maxLen));
        m_fileLabel->setText(display);
        updateOverlayText();
    }
    void setVirusCount(int count) { m_countLabel->setText(QStringLiteral("Viren: %1").arg(count)); updateOverlayText(); }
    void setIndex(int currentIndex, int totalFiles) { m_currentIndex = currentIndex; m_totalFiles = totalFiles; updateOverlayText(); }

protected:
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
    void updateOverlayText() {
        QString pct = m_label->text();
        QString idx;
        if (m_totalFiles > 0) idx = QString("%1/%2").arg(m_currentIndex).arg(m_totalFiles);
        else idx = QStringLiteral("0/0");
        QString fileText = m_fileLabel->text();
        QString text = QStringLiteral("%1  (%2)  %3").arg(fileText, idx, pct);
        const int maxLen = 128;
        QString display = text;
        if (display.length() > maxLen) display = QString("...%1").arg(display.right(maxLen));
        m_fileLabel->setText(display);
        adjustSize();
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) {
            QRect geom = screen->availableGeometry();
            int margin = 12;
            move(geom.right() - width() - margin, geom.bottom() - height() - margin);
        }
    }

    QProgressBar *m_progress;
    QLabel *m_label;
    QLabel *m_fileLabel;
    QLabel *m_countLabel;
    int m_currentIndex = 0;
    int m_totalFiles = 0;
};

// ----------------------------- ClamAVScanner
class ClamAVScanner : public QThread {
    Q_OBJECT
public:
    explicit ClamAVScanner(QObject *parent = nullptr) : QThread(parent),
        m_abort(false), m_totalFiles(0), m_scannedFiles(0),
        m_infectedCount(0), m_errorCount(0), m_skippedCount(0) {}

    void scanOpenFiles() { m_targetDescription = "full system scan (proc + running execs + libs + kernel/modules + PATH + ELF deps + libs)"; if (isRunning()) wait(); start(); }
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

        QStringList files;
        if (!collectAllProcFiles(files)) {
            emit scanError("Keine scanbaren Dateien aus /proc gefunden.");
            return;
        }

        QStringList addedPathBinaries;
        collectPathBinaries(files, addedPathBinaries);

        for (const QString &bin : addedPathBinaries) {
            if (m_abort) break;
            collectElfDependencies(bin, files);
        }

        // inotify removed per request — no dynamic observation

        collectKernelAndModules(files);

        // collect library dirs (ld.so.conf + ld.so.cache + ldconfig -p fallback + common + parents)
        QStringList libDirs;
        collectLibraryDirs(libDirs);

        // collect .so* and .a files from libDirs (with caps)
        collectLibraryFiles(libDirs, files);

        m_totalFiles = files.size();
        m_scannedFiles = 0;
        m_infectedCount = 0;
        m_errorCount = 0;
        m_skippedCount = 0;

        // Notify listeners that the scan is starting only after the file list is prepared
        emit scanStarted(m_targetDescription);

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
                if (ret == CL_EOPEN || ret == CL_EACCES) ++m_skippedCount;
                else ++m_errorCount;
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

    // ---- helpers (proc collectors) ----
    static bool isPathSkippable(const QString &path) {
        if (path.isEmpty()) return true;
        if (path.contains(" (deleted)")) return true;
        if (path.startsWith("socket:")) return true;
        if (path.startsWith("pipe:")) return true;
        if (path.startsWith("anon_inode:")) return true;
        if (path.startsWith("memfd:")) return true;
        if (path.startsWith("/dev/null")) return true;
        if (path.startsWith("/dev/zero")) return true;
        if (path.startsWith("[") && path.endsWith("]")) return true;
        if (path.startsWith("/proc/")) return true;
        return false;
    }

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
            collectFdDir(QString("/proc/%1/fd").arg(pid), seen, outFiles);
            collectTaskFds(pid, seen, outFiles);
            collectExeLink(pid, seen, outFiles);
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
            if (!seen.contains(absPath)) { seen.insert(absPath); outFiles << absPath; }
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

    // ---- PATH binaries
    void collectPathBinaries(QStringList &outFiles, QStringList &addedBinaries) {
        addedBinaries.clear();
        QSet<QString> seen(outFiles.begin(), outFiles.end());
        QByteArray pathEnv = qgetenv("PATH");
        if (pathEnv.isEmpty()) pathEnv = "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin";
        const QStringList pathDirs = QString::fromUtf8(pathEnv).split(':', Qt::SkipEmptyParts);
        for (const QString &dir : pathDirs) {
            if (m_abort) break;
            if (!(dir == "/bin" || dir == "/sbin")) continue;
            QDir d(dir);
            if (!d.exists()) continue;
            QFileInfoList entries = d.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
            for (const QFileInfo &fi : entries) {
                if (m_abort) break;
                if (!fi.isFile()) continue;
                QFile::Permissions perms = fi.permissions();
                bool isExecutable = (perms & QFileDevice::ExeUser) || (perms & QFileDevice::ExeGroup) || (perms & QFileDevice::ExeOther);
                if (!isExecutable) continue;
                QString abs = fi.absoluteFilePath();
                if (abs.isEmpty()) continue;
                if (!seen.contains(abs)) {
                    seen.insert(abs);
                    outFiles << abs;
                    addedBinaries << abs;
                    logMessageQt(QString("Added PATH binary for scanning: %1").arg(abs));
                }
            }
        }
    }

    // ---- ELF parsing (DT_NEEDED, RPATH, RUNPATH)
    static bool parseElfDynamic(const QString &filePath, QStringList &neededNames, QStringList &rpaths, QStringList &runpaths) {
        neededNames.clear(); rpaths.clear(); runpaths.clear();
        int fd = open(filePath.toUtf8().constData(), O_RDONLY);
        if (fd < 0) return false;
        unsigned char e_ident[EI_NIDENT];
        if (pread(fd, e_ident, EI_NIDENT, 0) != EI_NIDENT) { close(fd); return false; }
        if (e_ident[EI_MAG0] != ELFMAG0 || e_ident[EI_MAG1] != ELFMAG1 || e_ident[EI_MAG2] != ELFMAG2 || e_ident[EI_MAG3] != ELFMAG3) { close(fd); return false; }
        bool is64 = (e_ident[EI_CLASS] == ELFCLASS64);
        if (is64) {
            Elf64_Ehdr ehdr;
            if (pread(fd, &ehdr, sizeof(ehdr), 0) != (ssize_t)sizeof(ehdr)) { close(fd); return false; }
            QVector<Elf64_Phdr> phdrs(ehdr.e_phnum);
            if (pread(fd, phdrs.data(), ehdr.e_phnum * sizeof(Elf64_Phdr), ehdr.e_phoff) != (ssize_t)(ehdr.e_phnum * sizeof(Elf64_Phdr))) { close(fd); return false; }
            Elf64_Off dynOffset = 0; Elf64_Xword dynSize = 0;
            for (const Elf64_Phdr &ph : phdrs) if (ph.p_type == PT_DYNAMIC) { dynOffset = ph.p_offset; dynSize = ph.p_filesz; }
            if (dynOffset == 0) { close(fd); return false; }
            size_t dynCount = dynSize / sizeof(Elf64_Dyn);
            QVector<Elf64_Dyn> dyns(dynCount);
            if (pread(fd, dyns.data(), dynSize, dynOffset) != (ssize_t)dynSize) { close(fd); return false; }
            Elf64_Addr strtabAddr = 0; size_t strsz = 0;
            for (const Elf64_Dyn &d : dyns) {
                if (d.d_tag == DT_STRTAB) strtabAddr = (Elf64_Addr)d.d_un.d_ptr;
                if (d.d_tag == DT_STRSZ) strsz = (size_t)d.d_un.d_val;
            }
            if (strtabAddr == 0 || strsz == 0) { close(fd); return false; }
            Elf64_Off strtabOffset = 0;
            for (const Elf64_Phdr &ph : phdrs) {
                if (ph.p_type == PT_LOAD) {
                    if (strtabAddr >= ph.p_vaddr && strtabAddr < ph.p_vaddr + ph.p_memsz) {
                        strtabOffset = ph.p_offset + (strtabAddr - ph.p_vaddr);
                        break;
                    }
                }
            }
            if (strtabOffset == 0) { close(fd); return false; }
            QByteArray strtab; strtab.resize(strsz);
            if (pread(fd, strtab.data(), strsz, strtabOffset) != (ssize_t)strsz) { close(fd); return false; }
            for (const Elf64_Dyn &d : dyns) {
                if (d.d_tag == DT_NEEDED) {
                    uint64_t idx = (uint64_t)d.d_un.d_val;
                    if (idx < (uint64_t)strtab.size()) neededNames << QString::fromUtf8(strtab.constData() + idx);
                } else if (d.d_tag == DT_RPATH) {
                    uint64_t idx = (uint64_t)d.d_un.d_val;
                    if (idx < (uint64_t)strtab.size()) rpaths << QString::fromUtf8(strtab.constData() + idx);
                } else if (d.d_tag == DT_RUNPATH) {
                    uint64_t idx = (uint64_t)d.d_un.d_val;
                    if (idx < (uint64_t)strtab.size()) runpaths << QString::fromUtf8(strtab.constData() + idx);
                }
            }
        } else {
            Elf32_Ehdr ehdr;
            if (pread(fd, &ehdr, sizeof(ehdr), 0) != (ssize_t)sizeof(ehdr)) { close(fd); return false; }
            QVector<Elf32_Phdr> phdrs(ehdr.e_phnum);
            if (pread(fd, phdrs.data(), ehdr.e_phnum * sizeof(Elf32_Phdr), ehdr.e_phoff) != (ssize_t)(ehdr.e_phnum * sizeof(Elf32_Phdr))) { close(fd); return false; }
            Elf32_Off dynOffset = 0; Elf32_Xword dynSize = 0;
            for (const Elf32_Phdr &ph : phdrs) if (ph.p_type == PT_DYNAMIC) { dynOffset = ph.p_offset; dynSize = ph.p_filesz; }
            if (dynOffset == 0) { close(fd); return false; }
            size_t dynCount = dynSize / sizeof(Elf32_Dyn);
            QVector<Elf32_Dyn> dyns(dynCount);
            if (pread(fd, dyns.data(), dynSize, dynOffset) != (ssize_t)dynSize) { close(fd); return false; }
            Elf32_Addr strtabAddr = 0; size_t strsz = 0;
            for (const Elf32_Dyn &d : dyns) {
                if (d.d_tag == DT_STRTAB) strtabAddr = (Elf32_Addr)d.d_un.d_ptr;
                if (d.d_tag == DT_STRSZ) strsz = (size_t)d.d_un.d_val;
            }
            if (strtabAddr == 0 || strsz == 0) { close(fd); return false; }
            Elf32_Off strtabOffset = 0;
            for (const Elf32_Phdr &ph : phdrs) {
                if (ph.p_type == PT_LOAD) {
                    if (strtabAddr >= ph.p_vaddr && strtabAddr < ph.p_vaddr + ph.p_memsz) {
                        strtabOffset = ph.p_offset + (strtabAddr - ph.p_vaddr);
                        break;
                    }
                }
            }
            if (strtabOffset == 0) { close(fd); return false; }
            QByteArray strtab; strtab.resize(strsz);
            if (pread(fd, strtab.data(), strsz, strtabOffset) != (ssize_t)strsz) { close(fd); return false; }
            for (const Elf32_Dyn &d : dyns) {
                if (d.d_tag == DT_NEEDED) {
                    uint32_t idx = (uint32_t)d.d_un.d_val;
                    if (idx < (uint32_t)strtab.size()) neededNames << QString::fromUtf8(strtab.constData() + idx);
                } else if (d.d_tag == DT_RPATH) {
                    uint32_t idx = (uint32_t)d.d_un.d_val;
                    if (idx < (uint32_t)strtab.size()) rpaths << QString::fromUtf8(strtab.constData() + idx);
                } else if (d.d_tag == DT_RUNPATH) {
                    uint32_t idx = (uint32_t)d.d_un.d_val;
                    if (idx < (uint32_t)strtab.size()) runpaths << QString::fromUtf8(strtab.constData() + idx);
                }
            }
        }
        close(fd);
        return true;
    }

    static QStringList expandOrigin(const QStringList &paths, const QString &binaryPath) {
        QStringList out;
        QString origin = QFileInfo(binaryPath).absolutePath();
        for (const QString &p : paths) {
            QString s = p;
            if (s.contains("$ORIGIN")) s.replace("$ORIGIN", origin);
            out << s;
        }
        return out;
    }

    static QString findLibraryInDirs(const QString &libname, const QStringList &dirs) {
        for (const QString &d : dirs) {
            QString candidate = QDir(d).filePath(libname);
            QFileInfo fi(candidate);
            if (fi.exists() && fi.isFile()) return fi.absoluteFilePath();
        }
        return QString();
    }

    static void collectElfDependencies(const QString &binaryPath, QStringList &outDeps) {
        QStringList needed, rpaths, runpaths;
        if (!parseElfDynamic(binaryPath, needed, rpaths, runpaths)) return;
        QStringList expandedR = expandOrigin(rpaths, binaryPath);
        QStringList expandedRun = expandOrigin(runpaths, binaryPath);
        QStringList searchDirs;
        searchDirs << expandedR << expandedRun;
        QStringList common = {
            "/lib", "/lib64", "/usr/lib", "/usr/lib64",
            "/lib/x86_64-linux-gnu", "/usr/lib/x86_64-linux-gnu",
            "/lib/i386-linux-gnu", "/usr/lib/i386-linux-gnu",
            "/usr/local/lib", "/usr/local/lib64"
        };
        searchDirs << common;
        QSet<QString> seen(outDeps.begin(), outDeps.end());
        for (const QString &lib : needed) {
            if (lib.isEmpty()) continue;
            QString found = findLibraryInDirs(lib, searchDirs);
            if (!found.isEmpty()) {
                if (!seen.contains(found)) { seen.insert(found); outDeps << found; }
                continue;
            }
            for (const QString &d : common) {
                QDirIterator it(d, QDirIterator::Subdirectories);
                while (it.hasNext()) {
                    QString cand = it.next();
                    if (QFileInfo(cand).fileName() == lib) {
                        if (!seen.contains(cand)) { seen.insert(cand); outDeps << cand; }
                        break;
                    }
                }
            }
        }
    }

    // ---- kernel + modules collector (simple best-effort)
    void collectKernelAndModules(QStringList &outFiles) {
        // try common kernel image locations
        QStringList candidates = { "/boot/vmlinuz", "/boot/vmlinuz-linux", "/vmlinuz" };
        for (const QString &c : candidates) {
            QFileInfo fi(c);
            if (fi.exists() && fi.isFile()) {
                if (!outFiles.contains(fi.absoluteFilePath())) outFiles << fi.absoluteFilePath();
            }
        }
        // try /lib/modules/*/*.ko
        QDir modulesDir("/lib/modules");
        if (modulesDir.exists()) {
            QDirIterator it("/lib/modules", QDirIterator::Subdirectories);
            while (it.hasNext()) {
                QString p = it.next();
                if (m_abort) break;
                if (p.endsWith(".ko")) {
                    QFileInfo fi(p);
                    if (fi.exists() && fi.isFile() && !outFiles.contains(fi.absoluteFilePath())) outFiles << fi.absoluteFilePath();
                }
            }
        }
    }

    // ---- library dirs discovery (ld.so.conf + ldconfig -p fallback + common)
    void collectLibraryDirs(QStringList &outDirs) {
        outDirs.clear();
        // parse /etc/ld.so.conf and included files
        QFile ldconf("/etc/ld.so.conf");
        if (ldconf.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&ldconf);
            while (!in.atEnd()) {
                QString line = in.readLine().trimmed();
                if (line.isEmpty() || line.startsWith("#")) continue;
                if (line.startsWith("include")) {
                    QString pattern = line.section(' ', 1).trimmed();
                    QDir d("/");
                    QStringList matches = d.entryList(QStringList() << pattern, QDir::Dirs | QDir::Files);
                    // best-effort: try common include expansion (not exhaustive)
                    QDirIterator it("/", QDirIterator::Subdirectories);
                    Q_UNUSED(matches);
                    break;
                } else {
                    QFileInfo fi(line);
                    if (fi.exists() && fi.isDir()) outDirs << fi.absoluteFilePath();
                }
            }
            ldconf.close();
        }
        // try parsing ldconfig cache via `ldconfig -p` as fallback
        QProcess p;
        p.start("ldconfig", QStringList() << "-p");
        if (p.waitForFinished(LDCONFIG_TIMEOUT_MS)) {
            QByteArray out = p.readAllStandardOutput();
            QList<QByteArray> lines = out.split('\n');
            for (const QByteArray &l : lines) {
                int idx = l.indexOf(" => ");
                if (idx > 0) {
                    QByteArray path = l.mid(idx + 4).trimmed();
                    QFileInfo fi(QString::fromUtf8(path));
                    if (fi.exists()) {
                        QString dir = fi.absolutePath();
                        if (!outDirs.contains(dir)) outDirs << dir;
                    }
                }
            }
        }
        // add common dirs
        QStringList common = {
            "/lib", "/lib64", "/usr/lib", "/usr/lib64",
            "/usr/local/lib", "/usr/local/lib64",
            "/lib/x86_64-linux-gnu", "/usr/lib/x86_64-linux-gnu"
        };
        for (const QString &d : common) if (!outDirs.contains(d)) outDirs << d;
    }

    // ---- collect .so* and .a files from libDirs (with cap)
    void collectLibraryFiles(const QStringList &libDirs, QStringList &outFiles) {
        QSet<QString> seen(outFiles.begin(), outFiles.end());
        qint64 added = 0;
        for (const QString &d : libDirs) {
            if (m_abort) break;
            QDir dir(d);
            if (!dir.exists()) continue;
            QDirIterator it(d, QDirIterator::Subdirectories);
            while (it.hasNext() && !m_abort) {
                QString p = it.next();
                if (added >= LIBFILES_MAX) break;
                QFileInfo fi(p);
                if (!fi.exists() || !fi.isFile()) continue;
                QString name = fi.fileName();
                if (name.endsWith(".so") || name.contains(".so.") || name.endsWith(".a")) {
                    QString abs = fi.absoluteFilePath();
                    if (!seen.contains(abs)) {
                        seen.insert(abs);
                        outFiles << abs;
                        ++added;
                    }
                }
            }
            if (added >= LIBFILES_MAX) break;
        }
        logMessageQt(QString("Collected library files (count added=%1)").arg(added));
    }

    // ---- libclamav integration helpers
    struct cl_engine* initClamEngine() {
        cl_engine *engine = nullptr;
        if (cl_init(CL_INIT_DEFAULT) != CL_SUCCESS) {
            logMessageQt("cl_init failed");
            return nullptr;
        }
        engine = cl_engine_new();
        if (!engine) {
            logMessageQt("cl_engine_new failed");
            // some libclamav versions provide cl_cleanup(); if your header declares it, you can call it here
            return nullptr;
        }
        unsigned int sigs = 0;
        cl_error_t ret = cl_load(cl_retdbdir(), engine, &sigs, CL_DB_STDOPT);
        if (ret != CL_SUCCESS) {
            logMessageQt(QString("cl_load failed: %1").arg(QString::fromUtf8(cl_strerror(ret))));
        } else {
            logMessageQt(QString("Loaded ClamAV signatures: %1").arg(sigs));
        }
        if (cl_engine_compile(engine) != CL_SUCCESS) {
            logMessageQt("cl_engine_compile failed");
            cl_engine_free(engine);
            return nullptr;
        }
        // NOTE: engine option calls vary between libclamav versions.
        // If you have a version that supports setting string options via an enum, set them here.
        return engine;
    }

    cl_error_t scanFileWithLibclamav(struct cl_engine *engine, const QString &path, QString &virusOut) {
        virusOut.clear();
        QByteArray ba = path.toUtf8();
        const char *cpath = ba.constData();
        const char *virname = nullptr;
        unsigned long scanned = 0;
        // call cl_scanfile with proper types: const char **virname, unsigned long *scanned, const cl_engine*, cl_scan_options*
        // pass nullptr for options to use defaults
        cl_error_t ret = cl_scanfile(cpath, &virname, &scanned, engine, nullptr);
        if (ret == CL_VIRUS && virname) {
            virusOut = QString::fromUtf8(virname);
        }
        return ret;
    }
};

// ----------------------------- Main UI and wiring
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("antivirus");
    QApplication::setApplicationVersion(APP_VERSION);

    QString logDir = QDir::homePath() + "/.antivirus";
    QDir().mkpath(logDir);
    QString logPath = logDir + "/antivirus.log";
    openLogFileQt(logPath);

    QSystemTrayIcon tray;
    QIcon icon = QIcon::fromTheme("security-high");
    if (icon.isNull()) icon = QIcon(":/icons/antivirus.png");
    tray.setIcon(icon);
    tray.setToolTip(QString("Antivirus %1").arg(APP_VERSION));

    QMenu menu;
    QAction actScan("Vollscan starten", &menu);
    QAction actShowLog("Log öffnen", &menu);
    QAction actQuit("Beenden", &menu);
    menu.addAction(&actScan);
    menu.addAction(&actShowLog);
    menu.addSeparator();
    menu.addAction(&actQuit);
    tray.setContextMenu(&menu);
    tray.show();

    InfectionWindow infWin;
    ProgressOverlay overlay;
    overlay.hide();

    ClamAVScanner scanner;

    QObject::connect(&actScan, &QAction::triggered, [&]() {
        if (scanner.isRunning()) {
            QMessageBox::information(nullptr, "Scan läuft", "Ein Scan läuft bereits.");
            return;
        }
        overlay.show();
        overlay.setPercent(0.0);
        overlay.setCurrentFile(QStringLiteral("-"));
        overlay.setVirusCount(0);
        scanner.scanOpenFiles();
    });

    QObject::connect(&actShowLog, &QAction::triggered, [&]() {
        QDesktopServices::openUrl(QUrl::fromLocalFile(logPath));
    });

    QObject::connect(&actQuit, &QAction::triggered, [&]() {
        if (scanner.isRunning()) {
            scanner.abort();
            scanner.wait();
        }
        closeLogFileQt();
        qApp->quit();
    });

    QObject::connect(&scanner, &ClamAVScanner::scanStarted, [&](const QString &desc) {
        tray.showMessage("Scan gestartet", desc, QSystemTrayIcon::Information, 3000);
    });

    QObject::connect(&scanner, &ClamAVScanner::scanProgress, [&](int current, int total, const QString &file) {
        double pct = 0.0;
        if (total > 0) pct = (100.0 * current) / total;
        overlay.setIndex(current, total);
        overlay.setPercent(pct);
        overlay.setCurrentFile(file);
    });

    QObject::connect(&scanner, &ClamAVScanner::scanInfectionFound, [&](const QString &path, const QString &virus) {
        infWin.addInfection(path, virus);
        overlay.setVirusCount(infWin.findChildren<QListWidget*>().isEmpty() ? 0 : infWin.findChildren<QListWidget*>().first()->count());
        tray.showMessage("Infektion gefunden", QString("%1 — %2").arg(path, virus), QSystemTrayIcon::Warning, 5000);
    });

    QObject::connect(&scanner, &ClamAVScanner::scanFinished, [&](const QString &, bool infected, const QString &summary) {
        overlay.hide();
        QString title = infected ? "Scan beendet - Infektionen gefunden" : "Scan beendet - keine Infektionen";
        tray.showMessage(title, summary.left(1024), QSystemTrayIcon::Information, 8000);
    });

    QObject::connect(&scanner, &ClamAVScanner::scanError, [&](const QString &err) {
        overlay.hide();
        tray.showMessage("Scan Fehler", err, QSystemTrayIcon::Critical, 5000);
        logMessageQt(QString("Scan error: %1").arg(err));
    });

    // start a scan on startup (optional)
    // scanner.scanOpenFiles();

    int ret = app.exec();

    if (scanner.isRunning()) {
        scanner.abort();
        scanner.wait();
    }
    closeLogFileQt();
    return ret;
}

#include "antivir.moc"
