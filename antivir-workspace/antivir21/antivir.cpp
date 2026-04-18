// antivir.cpp
// Version: 2.3.6 — Qt5 antivirus with libclamav streaming scan and filelist scanner
// - File-only logfile (~/.antivirus/antivirus.log) contains only filenames (one per line).
// - New action "Dateiliste scannen" reads that logfile and scans each listed file with libclamav,
//   writing results to ~/.antivirus/scan_results.log in the format: path<TAB>STATUS<TAB>VIRUS_OR_EMPTY
// - Only files readable by the current user are scanned and shown in the infection list.
// - No console/stdout logging; logfile contains only filenames as requested.

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QThread>
#include <QIcon>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QDateTime>
#include <QCoreApplication>
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
#include <QDirIterator>
#include <memory>
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <pwd.h>

#include <clamav.h>

static const QString APP_VERSION = QStringLiteral("2.3.6-filelist-scan");

// ----------------------------- File-only logging helpers (filenames only)
static std::unique_ptr<QFile> g_logFilePtr;
static bool g_logOpen = false;

static void writeFilenameLine(const QString &filename) {
    if (!g_logOpen || !g_logFilePtr) return;
    QByteArray data = filename.toUtf8();
    data.append('\n');
    g_logFilePtr->write(data);
    g_logFilePtr->flush();
}

static void openLogFile(const QString &logPath) {
    if (g_logOpen) return;
    g_logFilePtr = std::make_unique<QFile>(logPath);
    if (!g_logFilePtr->open(QIODevice::Append | QIODevice::Text)) {
        g_logFilePtr.reset();
        g_logOpen = false;
        return;
    }
    g_logOpen = true;
    // Per user's request: do not write startup lines; logfile contains only filenames.
}

static void closeLogFile() {
    if (!g_logOpen) return;
    if (g_logFilePtr) {
        g_logFilePtr->flush();
        g_logFilePtr->close();
        g_logFilePtr.reset();
    }
    g_logOpen = false;
}

static void logFilename(const QString &path) {
    writeFilenameLine(path);
}

// ----------------------------- Helper: check readability for current user
static bool isReadableByCurrentUser(const QString &path) {
    QFileInfo fi(path);
    if (!fi.exists() || !fi.isFile()) return false;
    if (!fi.isReadable()) return false;
    QByteArray ba = path.toUtf8();
    const char *cpath = ba.constData();
    if (access(cpath, R_OK) != 0) return false;
    return true;
}

// ----------------------------- Helper: postEvent-based starter (no timers, works on older Qt)
class StartScanEvent : public QEvent {
public:
    static QEvent::Type type() {
        static int t = QEvent::registerEventType();
        return static_cast<QEvent::Type>(t);
    }
    StartScanEvent() : QEvent(type()) {}
};

class ScanStarter : public QObject {
    Q_OBJECT
public:
    explicit ScanStarter(QAction *action, QObject *parent = nullptr) : QObject(parent), m_action(action) {}
    bool event(QEvent *ev) override {
        if (ev->type() == StartScanEvent::type()) {
            if (m_action) m_action->trigger();
            deleteLater();
            return true;
        }
        return QObject::event(ev);
    }
private:
    QAction *m_action;
};

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
        if (!isReadableByCurrentUser(path)) {
            // do not add unreadable files to the UI list
            return;
        }
        QString itemText = QString("%1 — %2").arg(path, virus);
        QList<QListWidgetItem*> found = m_list->findItems(itemText, Qt::MatchExactly);
        if (!found.isEmpty()) return;
        m_list->addItem(itemText);
        if (m_list->count() == 1) m_list->setCurrentRow(0);
        if (!isVisible()) show();
        raise();
        activateWindow();
        // per user's request, also append filename to the main logfile (only filenames)
        logFilename(path);
    }

private slots:
    void onSelectionChanged(const QString &text) {
        QString virus = "Unknown";
        int sep = text.lastIndexOf(" — ");
        if (sep >= 0) virus = text.mid(sep + 3).trimmed();
        QString desc = m_descriptions.value(virus, m_descriptions.value("Unknown"));
        QString details = QString("<b>Signature:</b> %1<br><br><b>Beschreibung:</b><br>%2")
                          .arg(virus.toHtmlEscaped(), desc.toHtmlEscaped());
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
        setWindowOpacity(0.90);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(8,8,8,8);
        layout->setSpacing(6);

        m_fileLabel = new QLabel(QStringLiteral("-"), this);
        QFont fileFont = m_fileLabel->font();
        fileFont.setPointSize(qMax(9, fileFont.pointSize()));
        m_fileLabel->setFont(fileFont);
        m_fileLabel->setStyleSheet("color: black;");
        m_fileLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        m_indexProgress = new QProgressBar(this);
        m_indexProgress->setOrientation(Qt::Horizontal);
        m_indexProgress->setRange(0, 1000);
        m_indexProgress->setValue(0);
        m_indexProgress->setTextVisible(true);
        m_indexProgress->setFormat("Indexing: %p%");
        m_indexProgress->setStyleSheet(
            "QProgressBar {"
            "  background-color: rgba(255,255,255,0.0);"
            "  border: 1px solid rgba(0,0,0,0.15);"
            "  border-radius: 6px;"
            "  padding: 2px;"
            "  color: black;"
            "}"
            "QProgressBar::chunk {"
            "  background-color: rgba(30,144,255,0.75);"
            "  border-radius: 6px;"
            "}"
        );

        m_statusLabel = new QLabel(QStringLiteral("-"), this);
        QFont statusFont = m_statusLabel->font();
        statusFont.setPointSize(qMax(8, statusFont.pointSize()));
        m_statusLabel->setFont(statusFont);
        m_statusLabel->setStyleSheet("color: black;");
        m_statusLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_statusLabel->setWordWrap(false);
        m_statusLabel->setMinimumWidth(320);

        layout->addWidget(m_fileLabel);
        layout->addWidget(m_indexProgress);
        layout->addWidget(m_statusLabel);
        setLayout(layout);

        resize(560, 80);
        adjustSize();
        positionAtOneThirdFromRightBottom();
    }

public slots:
    void setIndexProgress(int currentIndex, int totalFiles, const QString &file) {
        int total = (totalFiles <= 0) ? qMax(1, currentIndex) : totalFiles;
        int cur = qBound(0, currentIndex, total);
        int scaled = static_cast<int>((1000.0 * cur) / (double)total);
        m_indexProgress->setRange(0, 1000);
        m_indexProgress->setValue(scaled);

        setCurrentFile(file);

        QString truncated = truncatePathToDepth(file, 3);
        m_statusLabel->setText(truncated);

        // per user's request: log only the filename/path
        logFilename(file);
    }

    void setPercent(double percent) {
        double p = qBound(0.0, percent, 100.0);
        m_indexProgress->setFormat(QString("Scan: %1%").arg(QString::number(p, 'f', 1)));
    }

    void setVirusCount(int count) {
        setToolTip(QString("Viren: %1").arg(count));
    }

    void setIndex(int currentIndex, int totalFiles) {
        m_indexProgress->setFormat(QString("%1/%2").arg(currentIndex).arg(totalFiles));
    }

    void setCurrentFile(const QString &file) {
        QString display = file.isEmpty() ? QStringLiteral("-") : file;
        const int maxLen = 80;
        if (display.length() > maxLen) display = QString("...%1").arg(display.right(maxLen));
        m_fileLabel->setText(display);
    }

protected:
    void showEvent(QShowEvent *ev) override {
        QWidget::showEvent(ev);
        positionAtOneThirdFromRightBottom();
    }

private:
    QLabel *m_fileLabel;
    QProgressBar *m_indexProgress;
    QLabel *m_statusLabel;

    void positionAtOneThirdFromRightBottom() {
        QScreen *screen = QGuiApplication::primaryScreen();
        if (!screen) return;
        QRect geom = screen->availableGeometry();
        int margin = 12;
        int offsetFromRight = geom.width() / 3;
        int x = geom.right() - width() - margin - offsetFromRight;
        int y = geom.bottom() - height() - margin;
        if (x < geom.left() + margin) x = geom.left() + margin;
        move(x, y);
    }

    static QString truncatePathToDepth(const QString &path, int depth) {
        if (path.isEmpty()) return QStringLiteral("-");
        QString p = QDir::cleanPath(path);
        bool isAbs = p.startsWith('/');
        QStringList parts = p.split('/', Qt::SkipEmptyParts);
        if (parts.isEmpty()) return isAbs ? QStringLiteral("/") : QStringLiteral(".");
        if (parts.size() <= depth) {
            return (isAbs ? QStringLiteral("/") + parts.join('/') : parts.join('/'));
        }
        QStringList first = parts.mid(0, depth);
        QString out = (isAbs ? QStringLiteral("/") + first.join('/') : first.join('/'));
        out += "/...";
        return out;
    }
};

// ----------------------------- ClamAVScanner (streaming scan of filesystem)
class ClamAVScanner : public QThread {
    Q_OBJECT
public:
    explicit ClamAVScanner(QObject *progressReceiver = nullptr, QObject *parent = nullptr)
        : QThread(parent),
          m_progressReceiver(progressReceiver),
          m_abort(false), m_totalFiles(0),
          m_scannedFiles(0), m_infectedCount(0), m_errorCount(0), m_skippedCount(0) {}

    void scanOpenFiles() { m_targetDescription = "full root scan (/) excluding /proc and ucf cache"; if (isRunning()) wait(); start(); }
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

        qint64 estimatedTotal = 0;
        struct statvfs sv;
        if (statvfs("/", &sv) == 0) {
            unsigned long long total_inodes = sv.f_files;
            unsigned long long free_inodes = sv.f_ffree;
            if (total_inodes > free_inodes) {
                unsigned long long used = total_inodes - free_inodes;
                estimatedTotal = static_cast<qint64>(used);
            }
        }

        m_totalFiles = (estimatedTotal > 0) ? estimatedTotal : 0;
        emit scanStarted(m_targetDescription);

        struct cl_engine *engine = initClamEngine();
        if (!engine) {
            emit scanError("ClamAV Engine konnte nicht initialisiert werden.");
            return;
        }

        // Engine self-test
        {
            QString test = "/etc/hostname";
            if (QFileInfo::exists(test) && QFileInfo(test).isFile()) {
                QString dummy;
                cl_error_t testret = scanFileWithLibclamav(engine, test, dummy);
                if (testret != CL_CLEAN && testret != CL_VIRUS) {
                    emit scanError(QString("ClamAV Engine Self-Test fehlgeschlagen: %1")
                                   .arg(QString::fromUtf8(cl_strerror(testret))));
                    cl_engine_free(engine);
                    return;
                }
            }
        }

        qint64 indexed = 0;
        qint64 infected = 0;
        qint64 errors = 0;
        qint64 skipped = 0;

        const QStringList excludePrefixes = {
            "/proc/", "/proc", "/sys/", "/sys", "/dev/", "/dev", "/run/", "/run",
            "/tmp/", "/tmp", "/var/tmp/", "/var/tmp", "/var/lib/ucf/cache", "/var/lib/ucf/cache/"
        };

        QDirIterator it("/", QDirIterator::Subdirectories);
        while (it.hasNext() && !m_abort) {
            QString p = it.next();
            if (p.isEmpty()) continue;

            if (p.contains(':')) {
                ++skipped;
                logFilename(p);
                continue;
            }

            QFileInfo fi(p);
            if (!fi.exists()) { ++skipped; continue; }
            if (!fi.isFile()) { continue; }

            QString abs = fi.absoluteFilePath();
            if (!isReadableByCurrentUser(abs)) {
                ++skipped;
                logFilename(abs);
                continue;
            }

            bool skip = false;
            for (const QString &pref : excludePrefixes) {
                if (abs == pref || abs.startsWith(pref + "/") || abs.startsWith(pref)) { skip = true; break; }
            }
            if (skip) { ++skipped; continue; }

            QFile testFile(abs);
            if (!testFile.open(QIODevice::ReadOnly)) {
                ++skipped;
                logFilename(abs);
                continue;
            }
            testFile.close();

            if (m_progressReceiver) {
                QMetaObject::invokeMethod(m_progressReceiver, "setIndexProgress",
                                          Qt::QueuedConnection,
                                          Q_ARG(int, static_cast<int>(indexed)),
                                          Q_ARG(int, static_cast<int>(m_totalFiles)),
                                          Q_ARG(QString, abs));
            }
            emit scanProgress(static_cast<int>(indexed), static_cast<int>(m_totalFiles), abs);

            QString virusName;
            cl_error_t ret = scanFileWithLibclamav(engine, abs, virusName);

            ++indexed;
            if (ret == CL_VIRUS) {
                ++infected;
                emit scanInfectionFound(abs, virusName);
                logFilename(abs);
            } else if (ret == CL_CLEAN) {
                // nothing to log
            } else {
                if (ret == CL_EOPEN || ret == CL_EACCES) {
                    ++skipped;
                } else {
                    ++errors;
                }
                logFilename(abs);
            }

            if (m_progressReceiver) {
                QMetaObject::invokeMethod(m_progressReceiver, "setIndexProgress",
                                          Qt::QueuedConnection,
                                          Q_ARG(int, static_cast<int>(indexed)),
                                          Q_ARG(int, static_cast<int>(m_totalFiles)),
                                          Q_ARG(QString, abs));
            }
            emit scanProgress(static_cast<int>(indexed), static_cast<int>(m_totalFiles), abs);
        }

        if (m_progressReceiver) {
            QMetaObject::invokeMethod(m_progressReceiver, "setIndexProgress",
                                      Qt::QueuedConnection,
                                      Q_ARG(int, static_cast<int>(indexed)),
                                      Q_ARG(int, static_cast<int>(m_totalFiles)),
                                      Q_ARG(QString, QStringLiteral("-")));
        }
        emit scanProgress(static_cast<int>(indexed), static_cast<int>(m_totalFiles), QStringLiteral("-"));

        m_scannedFiles = static_cast<int>(indexed);
        m_infectedCount = static_cast<int>(infected);
        m_errorCount = static_cast<int>(errors);
        m_skippedCount = static_cast<int>(skipped);

        cl_engine_free(engine);

        QString summary;
        summary += QString("Total files scanned (streamed): %1\n").arg(m_scannedFiles);
        summary += QString("Infected: %1\n").arg(m_infectedCount);
        summary += QString("Errors: %1\n").arg(m_errorCount);
        summary += QString("Skipped (no read or excluded): %1\n").arg(m_skippedCount);

        emit scanFinished(m_targetDescription, m_infectedCount > 0, summary);
    }

private:
    QString m_targetDescription;
    QObject *m_progressReceiver;
    bool m_abort;
    qint64 m_totalFiles;
    int m_scannedFiles;
    int m_infectedCount;
    int m_errorCount;
    int m_skippedCount;

    // ---- libclamav helpers (robust)
    struct cl_engine* initClamEngine() {
        if (cl_init(CL_INIT_DEFAULT) != CL_SUCCESS) {
            return nullptr;
        }

        cl_engine *engine = cl_engine_new();
        if (!engine) {
            return nullptr;
        }

        unsigned int sigs = 0;
        const char *dbdir = cl_retdbdir();
        if (!dbdir) {
            cl_engine_free(engine);
            return nullptr;
        }

        cl_error_t ret = cl_load(dbdir, engine, &sigs, CL_DB_STDOPT);
        if (ret != CL_SUCCESS) {
            cl_engine_free(engine);
            return nullptr;
        }

        ret = cl_engine_compile(engine);
        if (ret != CL_SUCCESS) {
            cl_engine_free(engine);
            return nullptr;
        }
        return engine;
    }

    cl_error_t scanFileWithLibclamav(struct cl_engine *engine, const QString &path, QString &virusOut) {
        virusOut.clear();
        if (!engine) return CL_EARG;

        QByteArray ba = path.toUtf8();
        const char *cpath = ba.constData();
        const char *virname = nullptr;
        unsigned long scanned = 0;

        struct stat st;
        if (stat(cpath, &st) != 0) {
            return CL_EOPEN;
        }
        if (!S_ISREG(st.st_mode)) {
            return CL_EOPEN;
        }
        if (st.st_size == 0) {
            return CL_CLEAN;
        }

        struct cl_scan_options opts;
        memset(&opts, 0, sizeof(opts));
        opts.general = CL_SCAN_GENERAL_ALLMATCHES;

        cl_error_t ret = cl_scanfile(cpath, &virname, &scanned, engine, &opts);
        if (ret == CL_VIRUS && virname) {
            virusOut = QString::fromUtf8(virname);
            return ret;
        }
        if (ret == CL_CLEAN) return ret;

        if (ret == CL_EARG || ret == CL_EUNKNOWN || ret == CL_EFORMAT) {
            FILE *f = fopen(cpath, "rb");
            if (!f) return CL_EOPEN;
            virname = nullptr;
            scanned = 0;
            cl_error_t ret2 = cl_scandesc(f, &virname, &scanned, engine, &opts);
            fclose(f);
            if (ret2 == CL_VIRUS && virname) virusOut = QString::fromUtf8(virname);
            return ret2;
        }

        return ret;
    }
};

// ----------------------------- FileListScanner (reads logfile of filenames and scans each)
class FileListScanner : public QThread {
    Q_OBJECT
public:
    explicit FileListScanner(const QString &filelistPath, const QString &resultsPath, QObject *progressReceiver = nullptr, QObject *parent = nullptr)
        : QThread(parent), m_filelistPath(filelistPath), m_resultsPath(resultsPath), m_progressReceiver(progressReceiver), m_abort(false) {}

    void abort() { m_abort = true; }

signals:
    void filelistScanStarted(const QString &path);
    void filelistScanProgress(int index, int total, const QString &file);
    void filelistScanFinished(const QString &path, bool anyInfected, const QString &summary);
    void filelistInfectionFound(const QString &path, const QString &virus);

protected:
    void run() override {
        m_abort = false;
        QFile f(m_filelistPath);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            emit filelistScanFinished(m_filelistPath, false, QString("Cannot open filelist"));
            return;
        }

        // read all lines
        QTextStream in(&f);
        QStringList paths;
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.isEmpty()) continue;
            // expand ~ if present
            if (line.startsWith("~")) {
                QString home = QDir::homePath();
                line = home + line.mid(1);
            }
            paths.append(line);
        }
        f.close();

        int total = paths.size();
        emit filelistScanStarted(m_filelistPath);

        // prepare results file (append)
        QFile outFile(m_resultsPath);
        if (!outFile.open(QIODevice::Append | QIODevice::Text)) {
            emit filelistScanFinished(m_filelistPath, false, QString("Cannot open results file"));
            return;
        }
        QTextStream out(&outFile);

        // init engine
        QString err;
        cl_engine *engine = initClamEngine(err);
        if (!engine) {
            out << "#ENGINE_ERROR\t" << err << "\n";
            out.flush();
            outFile.close();
            emit filelistScanFinished(m_filelistPath, false, QString("Engine init failed"));
            return;
        }

        bool anyInfected = false;
        for (int i = 0; i < total && !m_abort; ++i) {
            QString path = paths.at(i);
            // check readability
            if (!isReadableByCurrentUser(path)) {
                out << path << '\t' << "ERR_NOT_READABLE" << '\t' << '\n';
                out.flush();
                emit filelistScanProgress(i+1, total, path);
                continue;
            }

            // scan
            QString virus;
            cl_error_t r = scanPathWithEngine(engine, path, virus);
            if (r == CL_CLEAN) {
                out << path << '\t' << "CLEAN" << '\t' << '\n';
            } else if (r == CL_VIRUS) {
                anyInfected = true;
                out << path << '\t' << "INFECTED" << '\t' << virus.toUtf8().constData() << '\n';
                emit filelistInfectionFound(path, virus);
            } else {
                out << path << '\t' << "ERR_" << int(r) << '\t' << '\n';
            }
            out.flush();
            emit filelistScanProgress(i+1, total, path);
        }

        cl_engine_free(engine);
        outFile.close();

        QString summary = QString("Scanned %1 entries, infected=%2").arg(total).arg(anyInfected ? 1 : 0);
        emit filelistScanFinished(m_filelistPath, anyInfected, summary);
    }

private:
    QString m_filelistPath;
    QString m_resultsPath;
    QObject *m_progressReceiver;
    bool m_abort;

    // reuse engine helpers similar to ClamAVScanner
    struct cl_engine* initClamEngine(QString &errOut) {
        if (cl_init(CL_INIT_DEFAULT) != CL_SUCCESS) {
            errOut = "cl_init failed";
            return nullptr;
        }
        cl_engine *engine = cl_engine_new();
        if (!engine) {
            errOut = "cl_engine_new failed";
            return nullptr;
        }
        unsigned int sigs = 0;
        const char *dbdir = cl_retdbdir();
        if (!dbdir) {
            errOut = "cl_retdbdir returned NULL";
            cl_engine_free(engine);
            return nullptr;
        }
        cl_error_t ret = cl_load(dbdir, engine, &sigs, CL_DB_STDOPT);
        if (ret != CL_SUCCESS) {
            errOut = QString::fromUtf8(cl_strerror(ret));
            cl_engine_free(engine);
            return nullptr;
        }
        ret = cl_engine_compile(engine);
        if (ret != CL_SUCCESS) {
            errOut = QString::fromUtf8(cl_strerror(ret));
            cl_engine_free(engine);
            return nullptr;
        }
        return engine;
    }

    cl_error_t scanPathWithEngine(cl_engine *engine, const QString &path, QString &virusOut) {
        virusOut.clear();
        if (!engine) return CL_EARG;
        QByteArray ba = path.toUtf8();
        const char *cpath = ba.constData();
        const char *virname = nullptr;
        unsigned long scanned = 0;

        struct stat st;
        if (stat(cpath, &st) != 0) return CL_EOPEN;
        if (!S_ISREG(st.st_mode)) return CL_EOPEN;
        if (st.st_size == 0) return CL_CLEAN;

        struct cl_scan_options opts;
        memset(&opts, 0, sizeof(opts));
        opts.general = CL_SCAN_GENERAL_ALLMATCHES;

        cl_error_t ret = cl_scanfile(cpath, &virname, &scanned, engine, &opts);
        if (ret == CL_VIRUS && virname) {
            virusOut = QString::fromUtf8(virname);
            return ret;
        }
        if (ret == CL_CLEAN) return ret;

        if (ret == CL_EARG || ret == CL_EUNKNOWN || ret == CL_EFORMAT) {
            FILE *f = fopen(cpath, "rb");
            if (!f) return CL_EOPEN;
            virname = nullptr;
            scanned = 0;
            cl_error_t ret2 = cl_scandesc(f, &virname, &scanned, engine, &opts);
            fclose(f);
            if (ret2 == CL_VIRUS && virname) virusOut = QString::fromUtf8(virname);
            return ret2;
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
    openLogFile(logPath);

    QSystemTrayIcon tray;
    QIcon icon = QIcon::fromTheme("security-high");
    if (icon.isNull()) icon = QIcon(":/icons/antivirus.png");
    tray.setIcon(icon);
    tray.setToolTip(QString("Antivirus %1").arg(APP_VERSION));

    QMenu menu;
    QAction actScan("Vollscan starten", &menu);
    QAction actScanFilelist("Dateiliste scannen", &menu);
    QAction actShowLog("Log öffnen", &menu);
    QAction actQuit("Beenden", &menu);
    menu.addAction(&actScan);
    menu.addAction(&actScanFilelist);
    menu.addAction(&actShowLog);
    menu.addSeparator();
    menu.addAction(&actQuit);
    tray.setContextMenu(&menu);
    tray.show();

    InfectionWindow infWin;
    ProgressOverlay overlay;
    overlay.hide();

    ClamAVScanner scanner(&overlay);

    QObject::connect(&actScan, &QAction::triggered, [&]() {
        if (scanner.isRunning()) {
            QMessageBox::information(nullptr, "Scan läuft", "Ein Scan läuft bereits.");
            return;
        }
        overlay.show();
        overlay.setIndexProgress(0, 0, QStringLiteral("-"));
        scanner.scanOpenFiles();
    });

    QObject::connect(&actScanFilelist, &QAction::triggered, [&]() {
        QString filelistPath = QDir::homePath() + "/.antivirus/antivirus.log";
        QString resultsPath = QDir::homePath() + "/.antivirus/scan_results.log";
        FileListScanner *fls = new FileListScanner(filelistPath, resultsPath, &overlay);
        QObject::connect(fls, &FileListScanner::filelistScanStarted, &overlay, [&](const QString &p) {
            overlay.show();
            overlay.setIndexProgress(0, 1, QStringLiteral("-"));
        }, Qt::QueuedConnection);
        QObject::connect(fls, &FileListScanner::filelistScanProgress, &overlay, [&](int idx, int total, const QString &file) {
            overlay.setIndex(idx, total);
            overlay.setIndexProgress(idx, total, file);
        }, Qt::QueuedConnection);
        QObject::connect(fls, &FileListScanner::filelistInfectionFound, &infWin, [&](const QString &path, const QString &virus) {
            infWin.addInfection(path, virus);
            tray.showMessage("Infektion gefunden", QString("%1 — %2").arg(path, virus), QSystemTrayIcon::Warning, 5000);
        }, Qt::QueuedConnection);
        QObject::connect(fls, &FileListScanner::filelistScanFinished, &overlay, [&](const QString &, bool infected, const QString &summary) {
            overlay.hide();
            QString title = infected ? "Dateiliste gescannt - Infektionen gefunden" : "Dateiliste gescannt - keine Infektionen";
            tray.showMessage(title, summary, QSystemTrayIcon::Information, 8000);
            fls->deleteLater();
        }, Qt::QueuedConnection);
        fls->start();
    });

    QObject::connect(&actShowLog, &QAction::triggered, [&]() {
        QDesktopServices::openUrl(QUrl::fromLocalFile(logPath));
    });

    QObject::connect(&actQuit, &QAction::triggered, [&]() {
        if (scanner.isRunning()) {
            scanner.abort();
            scanner.wait();
        }
        closeLogFile();
        qApp->quit();
    });

    QObject::connect(&scanner, &ClamAVScanner::scanStarted, &overlay, [&](const QString &desc) {
        overlay.show();
        tray.showMessage("Scan gestartet", desc, QSystemTrayIcon::Information, 3000);
    }, Qt::QueuedConnection);

    QObject::connect(&scanner, &ClamAVScanner::scanProgress, &overlay, [&](int current, int total, const QString &file) {
        overlay.setIndex(current, total);
        overlay.setIndexProgress(current, total, file);
    }, Qt::QueuedConnection);

    QObject::connect(&scanner, &ClamAVScanner::scanInfectionFound, &infWin, [&](const QString &path, const QString &virus) {
        infWin.addInfection(path, virus);
        tray.showMessage("Infektion gefunden", QString("%1 — %2").arg(path, virus), QSystemTrayIcon::Warning, 5000);
    }, Qt::QueuedConnection);

    QObject::connect(&scanner, &ClamAVScanner::scanFinished, &overlay, [&](const QString &, bool infected, const QString &summary) {
        overlay.hide();
        QString title = infected ? "Scan beendet - Infektionen gefunden" : "Scan beendet - keine Infektionen";
        tray.showMessage(title, summary.left(1024), QSystemTrayIcon::Information, 8000);
    }, Qt::QueuedConnection);

    QObject::connect(&scanner, &ClamAVScanner::scanError, &overlay, [&](const QString &err) {
        overlay.hide();
        tray.showMessage("Scan Fehler", err, QSystemTrayIcon::Critical, 5000);
    }, Qt::QueuedConnection);

    // schedule auto-start scan via posted event (works on older Qt)
    overlay.show();
    overlay.setIndexProgress(0, 0, QStringLiteral("-"));
    ScanStarter *starter = new ScanStarter(&actScan, &app);
    QCoreApplication::postEvent(starter, new StartScanEvent());

    int ret = app.exec();

    if (scanner.isRunning()) {
        scanner.abort();
        scanner.wait();
    }
    closeLogFile();
    return ret;
}

#include "antivir.moc"

