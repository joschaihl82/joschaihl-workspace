// antivir.cpp
// Version: 2.3.0 — libclamav full scanner scanning / (no /proc, no ld.so.conf/ldconfig .so collection)
// Scans: all regular files under / (recursively), excluding virtual filesystems like /proc, /sys, /dev, /run, /tmp, /var/tmp.
// Logs to ~/.antivirus/antivirus.log. Uses libclamav (paranoid + archive parsing).
// This variant shows only a horizontal indexing progress bar positioned at one third from the right edge above the bottom-right screen corner.
// Includes debug logging for indexing.

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
#include <sys/statvfs.h>
#include <stdint.h>

#include <clamav.h>

static const QString APP_VERSION = QStringLiteral("2.3.0-libclamav-full-scan-root-hbar");

// Tunables / safety caps
static const qint64 LIBFILES_MAX = 20000000; // cap for collected files (also used to cap estimated total for UI)

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

// ----------------------------- ProgressOverlay (horizontal only, positioned 1/3 from right bottom)
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
        fileFont.setPointSize(qMax(8, fileFont.pointSize()));
        m_fileLabel->setFont(fileFont);
        m_fileLabel->setStyleSheet("color: black;");
        m_fileLabel->setAlignment(Qt::AlignLeft);

        // Only horizontal progress bar (indexing)
        m_indexProgress = new QProgressBar(this);
        m_indexProgress->setOrientation(Qt::Horizontal);
        m_indexProgress->setRange(0, 1000);
        m_indexProgress->setValue(0);
        m_indexProgress->setTextVisible(true);
        m_indexProgress->setFormat("Indexing: %p%");

        layout->addWidget(m_fileLabel);
        layout->addWidget(m_indexProgress);
        setLayout(layout);

        // initial sizing
        resize(480, 64);
        adjustSize();
        positionAtOneThirdFromRightBottom();
    }

public slots:
    void setIndexProgress(int currentIndex, int totalFiles, const QString &file) {
        int total = static_cast<int>(qMax<qint64>(1, totalFiles));
        int cur = qBound(0, currentIndex, total);
        int scaled = static_cast<int>((1000.0 * cur) / total);
        m_indexProgress->setRange(0, 1000);
        m_indexProgress->setValue(scaled);
        setCurrentFile(file);
        // debug log to confirm overlay received the call
        logMessageQt(QString("DEBUG: overlay.setIndexProgress called: %1 / %2  file=%3").arg(cur).arg(total).arg(file));
    }

    void setCurrentFile(const QString &file) {
        QString display = file;
        if (display.isEmpty()) display = QStringLiteral("-");
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
    void positionAtOneThirdFromRightBottom() {
        QScreen *screen = QGuiApplication::primaryScreen();
        if (!screen) return;
        QRect geom = screen->availableGeometry();
        int margin = 12;
        // place the overlay so that its right edge is offset by one third of the screen width from the right edge
        // i.e., move left by screen.width()/3 from the right edge
        int offsetFromRight = geom.width() / 3;
        int x = geom.right() - width() - margin - offsetFromRight;
        int y = geom.bottom() - height() - margin;
        // ensure we don't go off-screen on the left
        if (x < geom.left() + margin) x = geom.left() + margin;
        move(x, y);
    }

    QLabel *m_fileLabel;
    QProgressBar *m_indexProgress;
};

// ----------------------------- ClamAVScanner
class ClamAVScanner : public QThread {
    Q_OBJECT
public:
    explicit ClamAVScanner(QObject *progressReceiver = nullptr, QObject *parent = nullptr)
        : QThread(parent),
          m_progressReceiver(progressReceiver),
          m_abort(false), m_totalFiles(0), m_scannedFiles(0),
          m_infectedCount(0), m_errorCount(0), m_skippedCount(0) {}

    void scanOpenFiles() { m_targetDescription = "full root scan (/) excluding /proc and ldconfig/ld.so.conf .so collection"; if (isRunning()) wait(); start(); }
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

        // estimate total files using statvfs("/")
        qint64 estimatedTotal = 0;
        struct statvfs sv;
        if (statvfs("/", &sv) == 0) {
            unsigned long long total_inodes = sv.f_files;
            unsigned long long free_inodes = sv.f_ffree;
            if (total_inodes > free_inodes) {
                unsigned long long used = total_inodes - free_inodes;
                if (used > static_cast<unsigned long long>(LIBFILES_MAX)) estimatedTotal = LIBFILES_MAX;
                else estimatedTotal = static_cast<qint64>(used);
            }
        } else {
            logMessageQt(QString("statvfs('/') failed: %1").arg(strerror(errno)));
        }

        QStringList files;
        collectAllFilesFromRoot(files, estimatedTotal);

        if (files.isEmpty()) {
            emit scanError("Keine Dateien zum Scannen unter / gefunden (oder Zugriff verweigert).");
            return;
        }

        if (estimatedTotal > 0) m_totalFiles = estimatedTotal;
        else m_totalFiles = files.size();

        if (m_totalFiles < files.size()) m_totalFiles = files.size();

        m_scannedFiles = 0;
        m_infectedCount = 0;
        m_errorCount = 0;
        m_skippedCount = 0;

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
        summary += QString("Total files to scan (UI total): %1\n").arg(m_totalFiles);
        summary += QString("Collected files scanned: %1\n").arg(m_scannedFiles);
        summary += QString("Infected: %1\n").arg(m_infectedCount);
        summary += QString("Errors: %1\n").arg(m_errorCount);
        summary += QString("Skipped (no read): %1\n").arg(m_skippedCount);

        logMessageQt(QString("Scan summary for %1:\n%2").arg(m_targetDescription, summary));
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

    void collectAllFilesFromRoot(QStringList &outFiles, qint64 totalEstimate) {
        outFiles.clear();
        QSet<QString> seen;
        qint64 added = 0;

        if (m_progressReceiver) {
            const QMetaObject *mo = m_progressReceiver->metaObject();
            int idx = mo->indexOfMethod("setIndexProgress(int,int,QString)");
            if (idx < 0) {
                logMessageQt(QString("DEBUG: progressReceiver does NOT have setIndexProgress(int,int,QString) (metaindex=%1)").arg(idx));
            } else {
                logMessageQt(QString("DEBUG: progressReceiver has setIndexProgress (metaindex=%1)").arg(idx));
            }
        } else {
            logMessageQt("DEBUG: m_progressReceiver is NULL");
        }

        const QStringList excludePrefixes = {
            "/proc/", "/proc", "/sys/", "/sys", "/dev/", "/dev", "/run/", "/run",
            "/tmp/", "/tmp", "/var/tmp/", "/var/tmp"
        };

        QDirIterator it("/", QDirIterator::Subdirectories);
        while (it.hasNext() && !m_abort && added < LIBFILES_MAX) {
            QString p = it.next();
            if (p.isEmpty()) {
                logMessageQt("DEBUG: QDirIterator returned empty path");
                continue;
            }
            QFileInfo fi(p);
            if (!fi.exists()) continue;
            if (!fi.isFile()) continue;
            QString abs = fi.absoluteFilePath();

            bool skip = false;
            for (const QString &pref : excludePrefixes) {
                if (abs == pref || abs.startsWith(pref + "/") || abs.startsWith(pref)) {
                    skip = true;
                    break;
                }
            }
            if (skip) continue;

            if (seen.contains(abs)) continue;
            seen.insert(abs);
            outFiles << abs;
            ++added;

            if (added <= 10 || (added % 1000) == 0) {
                logMessageQt(QString("DEBUG: indexed %1 files, last: %2").arg(added).arg(abs));
            }

            if (m_progressReceiver) {
                int totalForUI = static_cast<int>(qMax<qint64>(1, qMin<qint64>(totalEstimate > 0 ? totalEstimate : LIBFILES_MAX, LIBFILES_MAX)));
                bool invoked = QMetaObject::invokeMethod(m_progressReceiver, "setIndexProgress",
                                                          Qt::QueuedConnection,
                                                          Q_ARG(int, static_cast<int>(added)),
                                                          Q_ARG(int, totalForUI),
                                                          Q_ARG(QString, abs));
                if (!invoked) {
                    logMessageQt(QString("DEBUG: invokeMethod FAILED for file: %1  (count=%2 totalForUI=%3)").arg(abs).arg(added).arg(totalForUI));
                    QMetaObject::invokeMethod(QCoreApplication::instance(), [this, added, totalForUI, abs]() {
                        if (m_progressReceiver) {
                            bool ok = QMetaObject::invokeMethod(m_progressReceiver, "setIndexProgress",
                                                                Qt::QueuedConnection,
                                                                Q_ARG(int, static_cast<int>(added)),
                                                                Q_ARG(int, totalForUI),
                                                                Q_ARG(QString, abs));
                            if (!ok) {
                                logMessageQt(QString("DEBUG: fallback invokeMethod also FAILED for file: %1").arg(abs));
                            }
                        }
                    }, Qt::QueuedConnection);
                }
            }
        }

        if (m_progressReceiver) {
            int totalForUI = static_cast<int>(qMax<qint64>(1, qMin<qint64>(totalEstimate > 0 ? totalEstimate : added, LIBFILES_MAX)));
            bool invoked = QMetaObject::invokeMethod(m_progressReceiver, "setIndexProgress",
                                                     Qt::QueuedConnection,
                                                     Q_ARG(int, static_cast<int>(added)),
                                                     Q_ARG(int, totalForUI),
                                                     Q_ARG(QString, QStringLiteral("-")));
            if (!invoked) {
                logMessageQt(QString("DEBUG: final invokeMethod FAILED (count=%1 totalForUI=%2)").arg(added).arg(totalForUI));
            }
        }

        logMessageQt(QString("Collected files from / (count=%1, cap=%2)").arg(outFiles.size()).arg(LIBFILES_MAX));
    }

    // libclamav helpers
    struct cl_engine* initClamEngine() {
        cl_engine *engine = nullptr;
        if (cl_init(CL_INIT_DEFAULT) != CL_SUCCESS) {
            logMessageQt("cl_init failed");
            return nullptr;
        }
        engine = cl_engine_new();
        if (!engine) {
            logMessageQt("cl_engine_new failed");
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
        return engine;
    }

    cl_error_t scanFileWithLibclamav(struct cl_engine *engine, const QString &path, QString &virusOut) {
        virusOut.clear();
        QByteArray ba = path.toUtf8();
        const char *cpath = ba.constData();
        const char *virname = nullptr;
        unsigned long scanned = 0;
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

    ClamAVScanner scanner(&overlay);

    QObject::connect(&actScan, &QAction::triggered, [&]() {
        if (scanner.isRunning()) {
            QMessageBox::information(nullptr, "Scan läuft", "Ein Scan läuft bereits.");
            return;
        }
        overlay.show();
        overlay.setIndexProgress(0, 1, QStringLiteral("-"));
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
        // We only show indexing bar; update overlay label to reflect scanning file as well
        overlay.setIndexProgress(current, total, file);
    });

    QObject::connect(&scanner, &ClamAVScanner::scanInfectionFound, [&](const QString &path, const QString &virus) {
        infWin.addInfection(path, virus);
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

    int ret = app.exec();

    if (scanner.isRunning()) {
        scanner.abort();
        scanner.wait();
    }
    closeLogFileQt();
    return ret;
}

#include "antivir.moc"
