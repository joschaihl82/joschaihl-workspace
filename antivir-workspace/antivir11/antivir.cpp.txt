// antivir.cpp
// Version: 2.3.0 — libclamav full scanner with direct ld.so.cache parsing
// Scans: /proc-disclosed files, running executables + mapped libs, kernel image + modules,
// inotify-observed files, PATH binaries (/bin,/sbin) + ELF DT_NEEDED deps, and all .so*/.a files
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
#include <sys/inotify.h>
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
static const int INOTIFY_MAX_WATCHES = 20000;
static const qint64 LIBFILES_MAX = 200000;
static const int INOTIFY_OBSERVE_MS = 5000;
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
    }
    void setCurrentFile(const QString &file) {
        QString display = file;
        if (display.isEmpty()) display = QStringLiteral("-");
        const int maxLen = 64;
        if (display.length() > maxLen) display = QString("...%1").arg(display.right(maxLen));
        m_fileLabel->setText(display);
    }
    void setVirusCount(int count) { m_countLabel->setText(QStringLiteral("Viren: %1").arg(count)); }

private:
    QProgressBar *m_progress;
    QLabel *m_label;
    QLabel *m_fileLabel;
    QLabel *m_countLabel;
};

// ----------------------------- ClamAVScanner
class ClamAVScanner : public QThread {
    Q_OBJECT
public:
    explicit ClamAVScanner(QObject *parent = nullptr) : QThread(parent),
        m_abort(false), m_totalFiles(0), m_scannedFiles(0),
        m_infectedCount(0), m_errorCount(0), m_skippedCount(0) {}

    void scanOpenFiles() { m_targetDescription = "full system scan (proc + running execs + libs + kernel/modules + inotify + PATH + ELF deps + libs)"; if (isRunning()) wait(); start(); }
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

        QStringList addedPathBinaries;
        collectPathBinaries(files, addedPathBinaries);

        for (const QString &bin : addedPathBinaries) {
            if (m_abort) break;
            collectElfDependencies(bin, files);
        }

        QStringList inotifyFiles;
        collectInotifyFiles("/", inotifyFiles, INOTIFY_OBSERVE_MS);
        for (const QString &f : inotifyFiles) if (!files.contains(f)) files << f;

        collectKernelAndModules(files);

        // collect library dirs (ld.so.conf + ld.so.cache + ldconfig -p fallback + common + parents)
        QStringList libDirs;
        collectLibraryDirs(libDirs, files);

        // collect .so* and .a files from libDirs (with caps)
        collectLibraryFiles(libDirs, files);

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

    // ---- inotify collector
    void collectInotifyFiles(const QString &rootPath, QStringList &outFiles, int durationMs) {
        outFiles.clear();
        const int EVENT_BUF_LEN = 1024 * (sizeof(struct inotify_event) + 16);
        int in_fd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
        if (in_fd < 0) {
            logMessageQt(QString("inotify_init1 failed: %1").arg(strerror(errno)));
            return;
        }
        QHash<int, QString> wdPath;
        QList<int> wds;
        int added = 0;
        QDirIterator it(rootPath, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        auto try_add_watch = [&](const QString &p) -> bool {
            if (added >= INOTIFY_MAX_WATCHES) return false;
            int wd = inotify_add_watch(in_fd, p.toUtf8().constData(), IN_OPEN | IN_CREATE | IN_MOVED_TO | IN_ATTRIB | IN_ONLYDIR);
            if (wd >= 0) { wdPath.insert(wd, p); wds.append(wd); ++added; return true; }
            return false;
        };
        try_add_watch(rootPath);
        while (it.hasNext() && added < INOTIFY_MAX_WATCHES) {
            QString dir = it.next();
            if (dir.startsWith("/proc") || dir.startsWith("/sys") || dir.startsWith("/dev")) continue;
            try_add_watch(dir);
        }
        logMessageQt(QString("inotify: added %1 watches (cap %2)").arg(added).arg(INOTIFY_MAX_WATCHES));
        QByteArray buffer(EVENT_BUF_LEN, 0);
        int elapsed = 0;
        const int POLL_INTERVAL_MS = 200;
        while (!m_abort && elapsed < durationMs) {
            fd_set rfds; FD_ZERO(&rfds); FD_SET(in_fd, &rfds);
            struct timeval tv; tv.tv_sec = POLL_INTERVAL_MS / 1000; tv.tv_usec = (POLL_INTERVAL_MS % 1000) * 1000;
            int ret = select(in_fd + 1, &rfds, nullptr, nullptr, &tv);
            if (ret > 0 && FD_ISSET(in_fd, &rfds)) {
                ssize_t len = read(in_fd, buffer.data(), buffer.size());
                if (len <= 0) {
                    if (errno != EAGAIN && errno != EINTR) { logMessageQt(QString("inotify read error: %1").arg(strerror(errno))); break; }
                } else {
                    ssize_t i = 0;
                    while (i < len) {
                        struct inotify_event *event = (struct inotify_event *)(buffer.data() + i);
                        QString parentPath = wdPath.value(event->wd);
                        QString name; if (event->len > 0) name = QString::fromUtf8(event->name);
                        QString fullPath = name.isEmpty() ? parentPath : QDir(parentPath).filePath(name);
                        if (!fullPath.isEmpty() && !isPathSkippable(fullPath)) {
                            QFileInfo fi(fullPath);
                            if (fi.exists() && fi.isFile()) {
                                if (!outFiles.contains(fullPath)) { outFiles << fullPath; logMessageQt(QString("inotify observed file: %1").arg(fullPath)); }
                            }
                        }
                        i += sizeof(struct inotify_event) + event->len;
                    }
                }
            }
            elapsed += POLL_INTERVAL_MS;
        }
        for (int wd : wds) inotify_rm_watch(in_fd, wd);
        close(in_fd);
        outFiles.removeDuplicates();
        logMessageQt(QString("inotify: observed %1 files").arg(outFiles.size()));
    }

    // ---- kernel & modules
    void collectKernelAndModules(QStringList &outFiles) {
        QSet<QString> seen(outFiles.begin(), outFiles.end());
        QString release;
        QFile relFile("/proc/sys/kernel/osrelease");
        if (relFile.open(QIODevice::ReadOnly | QIODevice::Text)) { release = QString::fromUtf8(relFile.readAll()).trimmed(); relFile.close(); }
        QStringList kernelCandidates;
        if (!release.isEmpty()) { kernelCandidates << QString("/boot/vmlinuz-%1").arg(release); kernelCandidates << QString("/lib/modules/%1/vmlinuz").arg(release); }
        kernelCandidates << "/boot/vmlinuz" << "/vmlinuz" << "/boot/kernel";
        for (const QString &kpath : kernelCandidates) {
            if (m_abort) break;
            QFileInfo kfi(kpath);
            if (kfi.exists() && kfi.isFile()) {
                QString abs = kfi.absoluteFilePath();
                if (!seen.contains(abs)) { seen.insert(abs); outFiles << abs; logMessageQt(QString("Added kernel image for scanning: %1").arg(abs)); }
            }
        }
        QString modulesBase;
        if (!release.isEmpty()) modulesBase = QString("/lib/modules/%1").arg(release);
        if (modulesBase.isEmpty() || !QDir(modulesBase).exists()) modulesBase = "/lib/modules";
        if (QDir(modulesBase).exists()) {
            QDirIterator it(modulesBase, QStringList() << "*.ko" << "*.ko.xz" << "*.ko.gz", QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext() && !m_abort) {
                QString modPath = it.next();
                QFileInfo mfi(modPath);
                if (!mfi.exists() || !mfi.isFile()) continue;
                QString abs = mfi.absoluteFilePath();
                if (!seen.contains(abs)) { seen.insert(abs); outFiles << abs; }
            }
            logMessageQt(QString("Added kernel modules from %1").arg(modulesBase));
        }
        QFile procModules("/proc/modules");
        if (procModules.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QByteArray content = procModules.readAll(); procModules.close();
            QStringList lines = QString::fromUtf8(content).split('\n', Qt::SkipEmptyParts);
            for (const QString &ln : lines) {
                if (m_abort) break;
                QStringList parts = ln.split(' ', Qt::SkipEmptyParts);
                if (parts.isEmpty()) continue;
                QString modName = parts[0].trimmed();
                if (modName.isEmpty()) continue;
                if (QDir(modulesBase).exists()) {
                    QDirIterator mit(modulesBase, QDirIterator::Subdirectories);
                    while (mit.hasNext() && !m_abort) {
                        QString candidate = mit.next();
                        QFileInfo cfi(candidate);
                        if (!cfi.exists() || !cfi.isFile()) continue;
                        QString fname = cfi.fileName();
                        if (fname.startsWith(modName) && (fname.contains(".ko") || fname.endsWith(".ko.xz") || fname.endsWith(".ko.gz"))) {
                            QString abs = cfi.absoluteFilePath();
                            if (!seen.contains(abs)) { seen.insert(abs); outFiles << abs; logMessageQt(QString("Added loaded module %1 -> %2").arg(modName, abs)); }
                        }
                    }
                }
            }
        }
    }

    // ---- library dirs collection (ld.so.conf + ld.so.cache parsing + ldconfig -p fallback + common + parents)
    void collectLibraryDirs(QStringList &libDirs, const QStringList &currentFiles) {
        QSet<QString> dirs;
        QStringList common = {
            "/lib", "/lib64", "/usr/lib", "/usr/lib64",
            "/lib/x86_64-linux-gnu", "/usr/lib/x86_64-linux-gnu",
            "/lib/i386-linux-gnu", "/usr/lib/i386-linux-gnu",
            "/usr/local/lib", "/usr/local/lib64", "/opt/lib", "/opt/local/lib"
        };
        for (const QString &d : common) if (QDir(d).exists()) dirs.insert(d);

        // parse /etc/ld.so.conf (simple)
        parseLdSoConf("/etc/ld.so.conf", dirs);

        // parse ld.so.cache directly (binary) and add directories found there
        parseLdSoCache("/etc/ld.so.cache", dirs);

        // fallback: ldconfig -p parsing if cache parsing failed or to supplement
        parseLdconfigCache(dirs);

        // add parent dirs of already collected files
        for (const QString &f : currentFiles) {
            QFileInfo fi(f);
            QString p = fi.absolutePath();
            if (!p.isEmpty() && QDir(p).exists()) dirs.insert(p);
        }

        // LD_LIBRARY_PATH
        QByteArray ldpath = qgetenv("LD_LIBRARY_PATH");
        if (!ldpath.isEmpty()) {
            QStringList parts = QString::fromUtf8(ldpath).split(':', Qt::SkipEmptyParts);
            for (const QString &p : parts) if (QDir(p).exists()) dirs.insert(p);
        }

        libDirs = QStringList(dirs.begin(), dirs.end());
        logMessageQt(QString("Collected %1 library directories for scanning").arg(libDirs.size()));
    }

    void parseLdSoConf(const QString &confPath, QSet<QString> &outDirs) {
        QFile f(confPath);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
        QString content = QString::fromUtf8(f.readAll());
        f.close();
        QStringList lines = content.split('\n', Qt::SkipEmptyParts);
        for (const QString &ln : lines) {
            QString line = ln.trimmed();
            if (line.isEmpty() || line.startsWith('#')) continue;
            if (line.startsWith("include")) {
                QString pattern = line.section(' ', 1).trimmed();
                if (pattern.isEmpty()) continue;
                // try to expand simple globs using QDirIterator on parent
                QFileInfo pinfo(pattern);
                QString parent = pinfo.absolutePath();
                if (parent.isEmpty()) parent = "/";
                QDirIterator it(parent, QDir::Files);
                while (it.hasNext()) {
                    QString candidate = it.next();
                    if (QFileInfo(candidate).fileName() == pinfo.fileName()) parseLdSoConf(candidate, outDirs);
                }
            } else {
                QString d = line;
                if (QDir(d).exists()) outDirs.insert(d);
            }
        }
    }

    // Direct parsing of /etc/ld.so.cache (glibc ld.so.cache format)
    // This implementation supports common glibc cache layout: header + entries with string table.
    // It's defensive: if parsing fails, it returns without adding dirs.
    void parseLdSoCache(const QString &cachePath, QSet<QString> &outDirs) {
        QFile f(cachePath);
        if (!f.open(QIODevice::ReadOnly)) {
            logMessageQt(QString("parseLdSoCache: cannot open %1").arg(cachePath));
            return;
        }
        QByteArray data = f.readAll();
        f.close();
        if (data.size() < 16) {
            logMessageQt("parseLdSoCache: cache too small or invalid");
            return;
        }

        // glibc ld.so.cache header starts with a magic "ld.so-1.7.0" style? Not stable across versions.
        // Safer approach: call ldconfig -p fallback if binary parsing is uncertain.
        // We'll attempt a simple heuristic parse used by many tools:
        // Format (glibc 2.3+): header: uint32_t magic (0x0), uint32_t version, uint32_t nlibs, uint32_t shlibs
        // followed by array of entries (structs) and a string table. Layout varies; implement conservative scan for absolute paths.
        // We'll scan the data for substrings that look like absolute library paths and collect their parent dirs.

        QSet<QString> foundDirs;
        const char *buf = data.constData();
        int len = data.size();

        // Heuristic: find occurrences of "/lib" or "/usr" and extract full path until null or space
        for (int i = 0; i < len; ++i) {
            if (buf[i] == '/') {
                // check common prefixes
                if (i + 4 < len && (strncmp(buf + i, "/lib", 4) == 0 || strncmp(buf + i, "/usr", 4) == 0 || strncmp(buf + i, "/opt", 4) == 0)) {
                    // extract until null or non-printable
                    int j = i;
                    while (j < len && buf[j] != '\0' && isprint((unsigned char)buf[j])) ++j;
                    if (j > i) {
                        QString candidate = QString::fromUtf8(buf + i, j - i).trimmed();
                        // only accept if it looks like a file path with .so or .a or contains '/lib/'
                        if (candidate.contains(".so") || candidate.endsWith(".a") || candidate.contains("/lib/") || candidate.contains("/lib64/")) {
                            QFileInfo fi(candidate);
                            QString dir = fi.absolutePath();
                            if (!dir.isEmpty() && QDir(dir).exists()) foundDirs.insert(dir);
                        }
                    }
                }
            }
        }

        if (!foundDirs.isEmpty()) {
            for (const QString &d : foundDirs) outDirs.insert(d);
            logMessageQt(QString("parseLdSoCache: heuristically added %1 dirs from %2").arg(foundDirs.size()).arg(cachePath));
            return;
        }

        // If heuristic failed, fallback to calling 'ldconfig -p' (already implemented elsewhere)
        logMessageQt("parseLdSoCache: heuristic parse found no dirs; falling back to ldconfig -p");
    }

    // parse ldconfig -p output to collect directories referenced by cache (fallback)
    void parseLdconfigCache(QSet<QString> &outDirs) {
        QProcess proc;
        proc.setProgram("ldconfig");
        proc.setArguments(QStringList() << "-p");
        proc.setProcessChannelMode(QProcess::MergedChannels);
        proc.start();
        if (!proc.waitForStarted(1000)) {
            logMessageQt("ldconfig -p failed to start or not available");
            return;
        }
        if (!proc.waitForFinished(LDCONFIG_TIMEOUT_MS)) {
            proc.kill();
            proc.waitForFinished(200);
            logMessageQt("ldconfig -p timed out");
            return;
        }
        QByteArray out = proc.readAllStandardOutput();
        QString outStr = QString::fromUtf8(out);
        if (outStr.isEmpty()) return;
        QStringList lines = outStr.split('\n', Qt::SkipEmptyParts);
        for (const QString &ln : lines) {
            int arrow = ln.indexOf("=>");
            if (arrow < 0) continue;
            QString path = ln.mid(arrow + 2).trimmed();
            if (path.isEmpty()) continue;
            QFileInfo fi(path);
            QString dir = fi.absolutePath();
            if (!dir.isEmpty() && QDir(dir).exists()) outDirs.insert(dir);
        }
        logMessageQt(QString("parseLdconfigCache: added %1 dirs from ldconfig -p").arg(outDirs.size()));
    }

    // ---- collect .so* and .a files from libDirs (recursive, with caps) ----
    void collectLibraryFiles(const QStringList &libDirs, QStringList &outFiles) {
        if (libDirs.isEmpty()) return;
        QSet<QString> seen(outFiles.begin(), outFiles.end());
        qint64 added = 0;
        QStringList patterns = QStringList() << "*.so" << "*.so.*" << "*.a";
        for (const QString &d : libDirs) {
            if (m_abort) break;
            if (!QDir(d).exists()) continue;
            QDirIterator it(d, patterns, QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext() && !m_abort && added < LIBFILES_MAX) {
                QString path = it.next();
                QFileInfo fi(path);
                if (!fi.exists() || !fi.isFile()) continue;
                QString abs = fi.absoluteFilePath();
                if (abs.isEmpty() || seen.contains(abs)) continue;
                seen.insert(abs);
                outFiles << abs;
                ++added;
                if ((added % 500) == 0) logMessageQt(QString("collectLibraryFiles: added %1 library files so far").arg(added));
            }
        }
        logMessageQt(QString("collectLibraryFiles: total added %1 library files").arg(added));
    }

    // ---- libclamav engine init & scan
    static struct cl_engine* initClamEngine() {
        cl_error_t ret = cl_init(CL_INIT_DEFAULT);
        if (ret != CL_SUCCESS) {
            qWarning() << "cl_init failed:" << cl_strerror(ret);
            return nullptr;
        }
        struct cl_engine *engine = cl_engine_new();
        if (!engine) { qWarning() << "cl_engine_new failed"; return nullptr; }
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

    static cl_error_t scanFileWithLibclamav(struct cl_engine *engine, const QString &path, QString &virusName) {
        const char *virname = nullptr;
#ifdef CL_SCAN_STDOPT
        cl_error_t ret = (cl_error_t)cl_scanfile(path.toUtf8().constData(), &virname, nullptr, engine, CL_SCAN_STDOPT);
#else
        struct cl_scan_options opts;
        std::memset(&opts, 0, sizeof(opts));
        opts.general |= CL_SCAN_GENERAL_HEURISTICS;
        opts.general |= CL_SCAN_GENERAL_ALLMATCHES;
        //opts.general |= CL_SCAN_GENERAL_PARANOID;
        opts.parse |= CL_SCAN_PARSE_ARCHIVE;
        size_t scanned = 0;
        cl_error_t ret = cl_scanfile(path.toUtf8().constData(), &virname, &scanned, engine, &opts);
#endif
        if (ret == CL_VIRUS) virusName = virname ? QString::fromUtf8(virname) : QStringLiteral("Unknown");
        return ret;
    }
};

// ----------------------------- Tray app + integration
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
        updateVirusDBsIfNeeded(24, FRESHCLAM_TIMEOUT_MS);
        trayIcon = new QSystemTrayIcon(this);
        QMenu *menu = new QMenu();
        QAction *scanOpenFilesAction = new QAction("Alle via /proc bekannten Dateien scannen (inkl. Kernel + Module + inotify + PATH + ELF deps + libs)", menu);
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
        logMessageQt(QString("Starting initial scan of all /proc-disclosed files (including kernel/modules, inotify, PATH binaries, ELF deps and library files)"));
        scanner->scanOpenFiles();
    }

    ~ClamAVTrayApp() {
        if (m_overlay) { m_overlay->close(); delete m_overlay; m_overlay = nullptr; }
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
        m_notifiedThisScan = false; m_lastIconPercent = -1.0; m_showWarningBadge = false; m_scanningActive = true; m_infectionCount = 0;
        if (m_overlay) { m_overlay->setPercent(0.0); m_overlay->setCurrentFile(QString()); m_overlay->setVirusCount(m_infectionCount); }
        trayIcon->setIcon(baseIcon);
    }
    void onScanProgress(int current, int total, const QString &file) {
        static double lastPct = -1.0; static qint64 lastUpdateMs = 0;
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
                lastPct = percent; lastUpdateMs = nowMs;
            }
            if (m_overlay) { m_overlay->setPercent(percent); m_overlay->setCurrentFile(QFileInfo(file).fileName()); m_overlay->setVirusCount(m_infectionCount); }
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
        if (m_overlay) { m_overlay->setVirusCount(m_infectionCount); m_overlay->setCurrentFile(QFileInfo(path).fileName()); }
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
        if (m_overlay) { m_overlay->setPercent(100.0); m_overlay->setCurrentFile(QString()); m_overlay->setVirusCount(m_infectionCount); }
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

    // update DBs helper
    static void updateVirusDBsIfNeeded(int thresholdHours = 24, int freshclamTimeoutMs = FRESHCLAM_TIMEOUT_MS) {
        const char *dbdir_c = cl_retdbdir();
        if (!dbdir_c) { logMessageQt("updateVirusDBsIfNeeded: cl_retdbdir() returned NULL"); return; }
        QString dbdir = QString::fromUtf8(dbdir_c);
        QDir d(dbdir);
        if (!d.exists()) { logMessageQt(QString("updateVirusDBsIfNeeded: DB directory does not exist: %1").arg(dbdir)); return; }
        QStringList dbFiles = d.entryList(QStringList() << "main.*" << "daily.*" << "bytecode.*" << "*.ndb" << "*.cvd" << "*.cld", QDir::Files);
        qint64 newestMtime = 0;
        for (const QString &f : dbFiles) {
            QFileInfo fi(d.filePath(f));
            if (fi.exists()) { qint64 m = fi.lastModified().toMSecsSinceEpoch(); if (m > newestMtime) newestMtime = m; }
        }
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        qint64 thresholdMs = qint64(thresholdHours) * 60 * 60 * 1000;
        bool needUpdate = (newestMtime == 0) || ((now - newestMtime) > thresholdMs);
        if (!needUpdate) { logMessageQt(QString("Virus DB up-to-date (newest file mtime: %1). No update needed.").arg(QDateTime::fromMSecsSinceEpoch(newestMtime).toString(Qt::ISODate))); return; }
        logMessageQt(QString("Virus DB appears older than %1 hours or missing. Running freshclam...").arg(thresholdHours));
        QProcess proc;
        proc.setProgram("freshclam");
        proc.setArguments(QStringList() << "--stdout");
        proc.setProcessChannelMode(QProcess::MergedChannels);
        proc.start();
        if (!proc.waitForStarted(3000)) { logMessageQt("Failed to start freshclam. Is it installed and in PATH?"); return; }
        if (!proc.waitForFinished(freshclamTimeoutMs)) { proc.kill(); proc.waitForFinished(2000); logMessageQt(QString("freshclam timed out after %1 ms and was killed").arg(freshclamTimeoutMs)); }
        QByteArray out = proc.readAll();
        QString outStr = QString::fromUtf8(out).trimmed();
        if (!outStr.isEmpty()) { const int MAXLOG = 16 * 1024; if (outStr.size() > MAXLOG) outStr = outStr.left(MAXLOG) + QStringLiteral("\n...[truncated]"); logMessageQt(QStringLiteral("freshclam output:\n%1").arg(outStr)); }
        int exitCode = proc.exitCode(); QProcess::ExitStatus exitStatus = proc.exitStatus();
        if (exitStatus == QProcess::NormalExit && exitCode == 0) logMessageQt("freshclam finished successfully.");
        else logMessageQt(QString("freshclam finished with exit code %1, status %2").arg(exitCode).arg(int(exitStatus)));
    }
};

// ----------------------------- main
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

