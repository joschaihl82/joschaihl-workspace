// antivir.cpp
// Build (qmake): qmake && make   with antivir.pro (QT += widgets network)
// Build (g++): g++ antivir.cpp -o ClamAVQuickScan `pkg-config --cflags --libs Qt5Widgets Qt5Network`

#include <QtWidgets>
#include <QAtomicInteger>
#include <QApplication>
#include <QCoreApplication>
#include <QLocalServer>
#include <QLocalSocket>
#include <QScopedPointer>
#include <QProcess>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QTextStream>
#include <QDateTime>
#include <QRegularExpression>
#include <QSet>
#include <QHash>
#include <QPair>
#include <QStringList>
#include <QMessageBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QSpinBox>
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QPainter>
#include <QTimer>
#include <QElapsedTimer>
#include <QCloseEvent>
#include <QMutex>
#include <QMutexLocker>
#include <QVector>
#include <QThread>
#include <QInputDialog>
#include <QLineEdit>
#include <cmath>

// Hintergrund/Daemon
#include <csignal>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// Für kill-other-instances
#include <dirent.h>
#include <ctype.h>

// ---------- Hintergrund ohne Strg+C (Daemonisierung) ----------
static bool writePidFile(const QString &path) {
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream ts(&f);
    ts << getpid() << "\n";
    f.close();
    return true;
}

static void backgroundize(const QString &logFile = QDir::homePath() + "/.clamav-quickscan.log",
                          const QString &pidFile = QDir::homePath() + "/.clamav-quickscan.pid") {
    std::signal(SIGINT, SIG_IGN);
    std::signal(SIGHUP, SIG_IGN);

    pid_t pid = fork();
    if (pid < 0) return;
    if (pid > 0) _exit(0);

    if (setsid() < 0) return;

    pid = fork();
    if (pid < 0) return;
    if (pid > 0) _exit(0);

    umask(027);
    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    int fd = ::open(logFile.toUtf8().constData(), O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd >= 0) {
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
    }

    writePidFile(pidFile);
}

// ---------- Einzelinstanz ----------
static bool ensureSingleInstance(const QString &key, QScopedPointer<QLocalServer> &serverOut) {
    QLocalServer::removeServer(key);
    serverOut.reset(new QLocalServer());
    if (!serverOut->listen(key)) {
        QLocalSocket probe;
        probe.connectToServer(key);
        if (probe.waitForConnected(100)) return false;
        if (!serverOut->listen(key)) return false;
    }
    return true;
}

// ---------- Xfce Autostart ----------
static bool ensureXfceAutostartEntry(const QString &name, const QString &execPath) {
    const QString autostartDir = QDir::homePath() + "/.config/autostart";
    QDir().mkpath(autostartDir);
    const QString desktopFile = autostartDir + "/" + name + ".desktop";
    if (QFile::exists(desktopFile)) return true;

    QFile f(desktopFile);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream ts(&f);
    ts << "[Desktop Entry]\n";
    ts << "Type=Application\n";
    ts << "Name=" << name << "\n";
    ts << "Comment=Starte " << name << " beim Login\n";
    ts << "Exec=" << execPath << "\n";
    ts << "OnlyShowIn=XFCE;\n";
    ts << "Terminal=false\n";
    ts << "Categories=Utility;Security;\n";
    f.close();
    return true;
}

// ---------- Utilities ----------
static QString runWhich(const QString &cmd) {
    QProcess p;
    p.start("which", {cmd});
    p.waitForFinished(-1);
    return QString::fromUtf8(p.readAllStandardOutput()).trimmed();
}

static QStringList uniqueAbs(const QStringList &in) {
    QSet<QString> seen;
    QStringList out;
    for (const auto &p : in) {
        QFileInfo fi(p);
        const QString abs = fi.absoluteFilePath();
        if (!seen.contains(abs)) {
            seen.insert(abs);
            out << abs;
        }
    }
    return out;
}

static bool isRegularFile(const QString &p) {
    QFileInfo fi(p);
    return fi.isFile();
}

static bool isExecutableFile(const QString &p) {
    QFileInfo fi(p);
    return fi.isFile() && fi.isExecutable();
}

// Prüft, ob Datei ein ELF-Binary ist (Linux/Unix)
static bool isElfBinary(const QString &p) {
    QFile f(p);
    if (!f.open(QIODevice::ReadOnly)) return false;
    const QByteArray head = f.read(4);
    f.close();
    return head.size() == 4 && head[0] == 0x7f && head[1] == 'E' && head[2] == 'L' && head[3] == 'F';
}

// Prüft auf Windows PE (MZ) Header
static bool isPeBinary(const QString &p) {
    QFile f(p);
    if (!f.open(QIODevice::ReadOnly)) return false;
    const QByteArray head = f.read(2);
    f.close();
    return head.size() == 2 && head[0] == 'M' && head[1] == 'Z';
}

// Erweiterte Heuristik: ELF, PE, ausführbar oder bekannte Erweiterungen (.so, .dll, .exe, .bin)
static bool isBinaryFile(const QString &p) {
    if (!isRegularFile(p)) return false;
    // quick checks
    if (isExecutableFile(p)) return true;
    const QString lower = p.toLower();
    static const QStringList exts = {".so", ".dll", ".exe", ".bin", ".o", ".a"};
    for (const auto &e : exts) if (lower.endsWith(e)) return true;
    // magic checks
    if (isElfBinary(p)) return true;
    if (isPeBinary(p)) return true;
    return false;
}

// ---------- Kill other instances (root required) ----------
// Versuche, als root neu zu starten (pkexec, sonst sudo).
// Gibt true zurück, wenn wir bereits root sind und weiterlaufen sollen.
// Wenn ein Re-exec gestartet wurde, beendet diese Funktion den aktuellen Prozess.
static bool ensureRunningAsRootWithPassword(QWidget *parent = nullptr) {
    if (geteuid() == 0) return true; // bereits root

    // Passwortdialog (modal)
    bool ok = false;
    QString pwd = QInputDialog::getText(parent ? parent : nullptr,
                                        "Root Passwort erforderlich",
                                        "Bitte Passwort für sudo eingeben:",
                                        QLineEdit::Password,
                                        QString(), &ok);
    if (!ok) {
        qWarning("Benutzer hat Passwortabfrage abgebrochen.");
        return false;
    }
    if (pwd.isEmpty()) {
        qWarning("Leeres Passwort eingegeben.");
        return false;
    }

    // Pfad zur eigenen Binary
    const QString exe = QCoreApplication::applicationFilePath();
    QStringList sudoArgs;
    sudoArgs << "-S" << "-p" << "" << exe;

    QProcess p;
    p.start("sudo", sudoArgs);
    if (!p.waitForStarted(3000)) {
        qWarning("Konnte sudo nicht starten.");
        return false;
    }

    QByteArray passBytes = (pwd + "\n").toUtf8();
    p.write(passBytes);
    p.waitForBytesWritten(2000);

    bool finished = p.waitForFinished(5000);

    if (finished && p.exitStatus() == QProcess::NormalExit && p.exitCode() == 0) {
        _exit(0);
        return false; // never reached
    }

    QByteArray stderrOut = p.readAllStandardError();
    QByteArray stdoutOut = p.readAllStandardOutput();
    qWarning() << "sudo fehlgeschlagen. stdout:" << QString::fromUtf8(stdoutOut)
               << " stderr:" << QString::fromUtf8(stderrOut)
               << " exitCode:" << p.exitCode();
    return false;
}

// Beendet andere Prozesse mit gleichem Programmnamen (Dateiname), außer der eigenen PID.
// Gibt Anzahl beendeter Prozesse zurück.
// graceSeconds: Wartezeit nach SIGTERM bevor SIGKILL gesendet wird.
static int killOtherInstancesByName(const QString &progName, int graceSeconds = 2) {
    const pid_t self = getpid();
    int killed = 0;

    DIR *d = opendir("/proc");
    if (!d) return 0;

    struct dirent *entry;
    while ((entry = readdir(d)) != nullptr) {
        // nur numerische Verzeichnisse sind PIDs
        bool isNum = true;
        for (char *c = entry->d_name; *c; ++c) if (!isdigit(*c)) { isNum = false; break; }
        if (!isNum) continue;

        pid_t pid = static_cast<pid_t>(atoi(entry->d_name));
        if (pid <= 0 || pid == self) continue;

        // Versuche /proc/<pid>/comm (kurzer Prozessname) zuerst
        QString name;
        QString commPath = QString("/proc/%1/comm").arg(pid);
        QFile f(commPath);
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            name = QString::fromUtf8(f.readAll()).trimmed();
            f.close();
        } else {
            // Fallback: /proc/<pid>/cmdline (erstes Token)
            QString cmdPath = QString("/proc/%1/cmdline").arg(pid);
            QFile f2(cmdPath);
            if (f2.open(QIODevice::ReadOnly)) {
                QByteArray cmd = f2.readAll();
                f2.close();
                if (!cmd.isEmpty()) {
                    int pos = cmd.indexOf('\0');
                    QByteArray first = pos > 0 ? cmd.left(pos) : cmd;
                    name = QFileInfo(QString::fromUtf8(first)).fileName();
                }
            }
        }

        if (name.isEmpty()) continue;

        // Vergleiche Dateiname (ohne Pfad) mit progName (auch Dateiname)
        QString targetBase = QFileInfo(progName).fileName();
        if (name == targetBase) {
            // Versuche SIGTERM, warte kurz, dann SIGKILL falls noch da
            if (kill(pid, SIGTERM) == 0) {
                int waited = 0;
                while (waited < graceSeconds) {
                    if (!QFile::exists(QString("/proc/%1").arg(pid))) break;
                    sleep(1);
                    waited++;
                }
                if (QFile::exists(QString("/proc/%1").arg(pid))) {
                    kill(pid, SIGKILL);
                }
                killed++;
            } else {
                // Falls SIGTERM fehlschlägt, versuche SIGKILL direkt
                kill(pid, SIGKILL);
                killed++;
            }
        }
    }
    closedir(d);
    return killed;
}

// ---------- Sammler ----------
namespace Collectors {
    QStringList collectOpenFiles(const QString &user, QString *errorOut) {
        QStringList files;
        QProcess proc;
        proc.start("lsof", {"-F", "n", "-u", user});
        proc.waitForFinished(-1);
        if (proc.exitCode() != 0) {
            if (errorOut) *errorOut = QString("lsof Fehler: %1")
                .arg(QString::fromUtf8(proc.readAllStandardError()).trimmed());
            return {};
        }
        const auto lines = QString::fromUtf8(proc.readAllStandardOutput())
                               .split('\n', Qt::SkipEmptyParts);
        for (const auto &line : lines) {
            if (line.startsWith("n")) {
                const QString path = line.mid(1);
                if (path.startsWith("/") && isRegularFile(path)) files << path;
            }
        }
        return uniqueAbs(files);
    }

    QStringList collectFrequentExecutables(const QString &historyFile, int topN) {
        QFile f(historyFile);
        if (!f.exists() || !f.open(QIODevice::ReadOnly | QIODevice::Text)) return {};
        QHash<QString,int> counter;
        QTextStream ts(&f);
        static const QSet<QString> ignore = {"ls","cd","pwd","echo","exit","fg","jobs","history","alias"};
        while (!ts.atEnd()) {
            QString line = ts.readLine().trimmed();
            if (line.isEmpty() || line.startsWith('#')) continue;
            const QString cmd = line.split(QRegularExpression("\\s+")).value(0);
            if (ignore.contains(cmd)) continue;
            const QString exe = runWhich(cmd);
            if (!exe.isEmpty()) counter[exe] += 1;
        }
        f.close();
        QList<QPair<QString,int>> items;
        for (auto it = counter.begin(); it != counter.end(); ++it) items.append({it.key(), it.value()});
        std::sort(items.begin(), items.end(), [](auto &a, auto &b){ return a.second > b.second; });

        QStringList result;
        int n = std::min(topN, items.size());
        for (int i = 0; i < n; ++i) {
            const QString p = items[i].first;
            if (isExecutableFile(p)) result << p;
        }
        return uniqueAbs(result);
    }

    QStringList collectRecentFiles(const QString &root, int hours) {
        const qint64 cutoff = QDateTime::currentSecsSinceEpoch() - hours * 3600;
        static const QSet<QString> excludeDirs = {".cache","node_modules","venv",".venv",".git"};
        QStringList matches;
        QDirIterator it(root, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            const QString p = it.next();
            QFileInfo fi(p);
            const auto parts = fi.absoluteFilePath().split('/');
            bool excluded = false;
            for (const auto &part : parts) {
                if (excludeDirs.contains(part)) { excluded = true; break; }
            }
            if (excluded) continue;
            if (!fi.exists() || !fi.isFile()) continue;
            const qint64 mtime = fi.lastModified().toSecsSinceEpoch();
            if (mtime >= cutoff) matches << fi.absoluteFilePath();
        }
        return uniqueAbs(matches);
    }

    // Neuer Collector: komplette Wurzel durchsuchen und nur Binaries zurückgeben
    QStringList collectAllBinaries(const QString &root) {
        QStringList matches;
        static const QSet<QString> excludeDirs = {"/proc", "/sys", "/dev", "/run", "/var/lib", "/var/run"}; // vermeiden problematischer Pseudo-FS
        QDirIterator it(root, QDir::Files, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
        while (it.hasNext()) {
            const QString p = it.next();
            // Skip excluded top-level mounts
            bool skip = false;
            for (const auto &ex : excludeDirs) {
                if (p.startsWith(ex + "/") || p == ex) { skip = true; break; }
            }
            if (skip) continue;
            QFileInfo fi(p);
            if (!fi.exists() || !fi.isFile()) continue;
            // Heuristik: nur Binaries
            if (isBinaryFile(p)) matches << fi.absoluteFilePath();
        }
        return uniqueAbs(matches);
    }
}

// ---------- Alarmdialog (wird nicht für automatische Löschung verwendet) ----------
class AlarmDialog : public QDialog {
    Q_OBJECT
public:
    AlarmDialog(const QStringList &findings, bool defaultAutoClean, QWidget *parent = nullptr)
        : QDialog(parent) {
        setWindowTitle("Virenfund – Maßnahmen");
        resize(700, 400);
        auto *layout = new QVBoxLayout(this);

        auto *label = new QLabel("Folgende Funde wurden gemeldet:", this);
        layout->addWidget(label);

        view_ = new QListWidget(this);
        for (const auto &line : findings) view_->addItem(line);
        layout->addWidget(view_);

        auto *options = new QGroupBox("Optionen", this);
        auto *form = new QFormLayout(options);
        chkAutoClean_ = new QCheckBox("Automatisch bereinigen (löscht/entfernt infizierte Dateien)", this);
        chkAutoClean_->setChecked(defaultAutoClean);
        form->addRow(chkAutoClean_);
        layout->addWidget(options);

        auto *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        connect(btns, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(btns, &QDialogButtonBox::rejected, this, &QDialog::reject);
        layout->addWidget(btns);
    }

    bool autoClean() const { return chkAutoClean_->isChecked(); }

private:
    QListWidget *view_;
    QCheckBox *chkAutoClean_;
};

// ---------- Scan‑Worker ----------
class ScanWorker : public QObject {
    Q_OBJECT
public:
    ScanWorker(QStringList paths, bool useClamd, bool doAutoClean)
        : paths_(std::move(paths)), useClamd_(useClamd), autoClean_(doAutoClean), stop_(false) {}

public slots:
    void run() {
        QStringList infected;
        QStringList cleaned;
        QStringList errors;
        const int total = paths_.size();
        int processed = 0;

        for (int i = 0; i < total; ++i) {
            if (stop_.load()) { errors << "Scan abgebrochen."; break; }
            const QString p = paths_.at(i);
            const QString program = useClamd_ ? "clamdscan" : "clamscan";
            QStringList args;
            if (useClamd_) {
                args << "--fdpass" << "--no-summary";
                if (autoClean_) args << "--remove";
                args << p;
            } else {
                args << "--infected" << "--no-summary";
                if (autoClean_) args << "--remove";
                args << p;
            }

            emit message(QString("Scanne: %1").arg(p));
            QProcess proc;
            proc.start(program, args);
            proc.waitForFinished(-1);

            const QString out = proc.readAllStandardOutput();
            const QString err = proc.readAllStandardError();
            const int rc = proc.exitCode();

            const auto lines = out.split('\n', Qt::SkipEmptyParts);
            for (const auto &line : lines) {
                if (line.endsWith("FOUND")) infected << line.trimmed();
                if (autoClean_ && (line.contains("Removed") || line.contains("removed"))) {
                    cleaned << line.trimmed();
                }
            }

            if (rc != 0 && rc != 1) {
                if (!err.trimmed().isEmpty()) errors << QString("%1: %2").arg(p, err.trimmed());
            }

            processed++;
            const int percent100000_local = int((processed * 100000.0) / total);
            emit progressWithFileLocal(percent100000_local, p);
        }
        emit finishedWorker(infected, cleaned, errors, processed);
    }

    void stop() { stop_.store(true); }

signals:
    void progressWithFileLocal(int percent100000_local, const QString &currentFile);
    void message(const QString &msg);
    void finishedWorker(const QStringList &infected, const QStringList &cleaned, const QStringList &errors, int processedCount);

private:
    QStringList paths_;
    bool useClamd_;
    bool autoClean_;
    QAtomicInteger<bool> stop_;
};

// ---------- Hauptfenster ----------
class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow() : tray_(nullptr), loopTimer_(nullptr), spinLoopMinutes_(nullptr) {
        setWindowTitle("ClamAV Quick Scan");
        resize(980, 700);

        auto *layout = new QVBoxLayout(this);

        // Einstellungen
        auto *cfg = new QGroupBox("Einstellungen", this);
        auto *form = new QFormLayout(cfg);

        spinTopN_ = new QSpinBox(this);
        spinTopN_->setRange(5, 100);
        spinTopN_->setValue(20);

        spinHours_ = new QSpinBox(this);
        spinHours_->setRange(1, 168);
        spinHours_->setValue(24);

        chkClamd_ = new QCheckBox("clamdscan verwenden (schneller, benötigt clamd)", this);
        chkHomeOnly_ = new QCheckBox("Nur Home-Verzeichnis durchsuchen (empfohlen)", this);
        // Default: false, weil der Benutzer explizit '/' scannen möchte
        chkHomeOnly_->setChecked(false);

        // **Automatisches Löschen aktivieren** — wenn gesetzt, werden infizierte Dateien automatisch entfernt (ohne Nachfrage)
        chkAutoCleanGlobal_ = new QCheckBox("Automatisch löschen (ohne Nachfrage)", this);
        chkAutoCleanGlobal_->setChecked(true); // Standard: aktiv

        // Neu: Intervall für Endlosschleife (Minuten)
        spinLoopMinutes_ = new QSpinBox(this);
        spinLoopMinutes_->setRange(1, 1440); // 1 Minute .. 24 Stunden
        spinLoopMinutes_->setValue(60);      // Default: 60 Minuten

        // Neu: Concurrency (Anzahl paralleler Worker)
        spinConcurrency_ = new QSpinBox(this);
        spinConcurrency_->setRange(1, 64);
        spinConcurrency_->setValue(std::max(1, QThread::idealThreadCount()));

        // Neu: Whitelist aktivieren + Button zum Bearbeiten (Button bleibt, aber UI otherwise minimal)
        chkUseWhitelist_ = new QCheckBox("Whitelist verwenden (Pfad: ~/.clamav-quickscan.whitelist)", this);
        btnEditWhitelist_ = new QPushButton("Whitelist bearbeiten", this);

        form->addRow("Top-N häufig ausgeführte Executables:", spinTopN_);
        form->addRow("Zeitfenster zuletzt geändert (Stunden):", spinHours_);
        form->addRow(chkClamd_);
        form->addRow(chkHomeOnly_);
        form->addRow(chkAutoCleanGlobal_);
        form->addRow("Scan-Intervall (Minuten):", spinLoopMinutes_);
        form->addRow("Parallel-Worker (schnellerer Scan):", spinConcurrency_);
        form->addRow(chkUseWhitelist_, btnEditWhitelist_);
        cfg->setLayout(form);

        // Keine sichtbaren Buttons: Convention over configuration
        auto *btnRow = new QHBoxLayout();
        auto *autoLabel = new QLabel("Automatische Scans aktiv (Konvention statt Buttons)", this);
        btnRow->addWidget(autoLabel);
        btnStop_ = new QPushButton(this);
        btnStop_->setVisible(false);
        btnStop_->setEnabled(false);

        // Progress/Status
        auto *progRow = new QHBoxLayout();
        progress_ = new QProgressBar(this);
        progress_->setRange(0, 100000); // 0 .. 100.000%
        status_   = new QLabel("Bereit.", this);
        progRow->addWidget(progress_, 1);
        progRow->addWidget(status_);

        // Output
        output_ = new QTextEdit(this);
        output_->setReadOnly(true);

        layout->addWidget(cfg);
        layout->addLayout(btnRow);
        layout->addLayout(progRow);
        layout->addWidget(output_, 1);

        // Verbindungen (nur notwendige)
        connect(btnEditWhitelist_, &QPushButton::clicked, this, &MainWindow::editWhitelist);
        // Tray actions remain for manual control if needed
        // Autostart sicherstellen
        const QString execPath = QFileInfo(QCoreApplication::applicationFilePath()).absoluteFilePath();
        ensureXfceAutostartEntry("ClamAVQuickScan", execPath);

        // Trayicon + Menü
        tray_ = new QSystemTrayIcon(this);
        baseIcon_ = QIcon::fromTheme("security-high");
        if (baseIcon_.isNull()) {
            QPixmap pm(64,64); pm.fill(Qt::transparent);
            QPainter p(&pm); p.setRenderHint(QPainter::Antialiasing);
            p.setBrush(QColor("#2d6cdf")); p.setPen(Qt::NoPen);
            p.drawRoundedRect(4,4,56,56,8,8);
            p.setPen(Qt::white); QFont f = p.font(); f.setBold(true); f.setPointSize(11); p.setFont(f);
            p.drawText(pm.rect(), Qt::AlignCenter, "AV");
            p.end();
            baseIcon_ = QIcon(pm);
        }
        trayMenu_ = new QMenu(this);

        actShow_       = trayMenu_->addAction("Fenster öffnen", this, [this]{ this->showNormal(); this->raise(); this->activateWindow(); });
        trayMenu_->addSeparator();
        actScanOpen_   = trayMenu_->addAction("Scan: Geöffnete Dateien", this, &MainWindow::scanOpenFiles);
        actScanFreq_   = trayMenu_->addAction("Scan: Häufige Executables", this, &MainWindow::scanFrequentExecs);
        actScanRecent_ = trayMenu_->addAction("Scan: Zuletzt geändert", this, &MainWindow::scanRecentFiles);
        actScanAllRoot_ = trayMenu_->addAction("Scan: Alle Binaries unter /", this, &MainWindow::scanAllBinaries);
        actStop_       = trayMenu_->addAction("Scan stoppen", this, &MainWindow::stopScan);
        actStop_->setEnabled(false);
        trayMenu_->addSeparator();
        actQuit_       = trayMenu_->addAction("Beenden", qApp, &QApplication::quit);

        tray_->setContextMenu(trayMenu_);
        tray_->setToolTip("ClamAV Quick Scan");
        tray_->setIcon(baseIcon_);
        tray_->show();

        // Linksklick: Fenster öffnen
        connect(tray_, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason r){
            if (r == QSystemTrayIcon::Trigger) {
                this->showNormal(); this->raise(); this->activateWindow();
            }
        });

        // Notifications: Timer initialisieren
        notifyTimer_.start();

        // Whitelist initial laden
        whitelistPath_ = QDir::homePath() + "/.clamav-quickscan.whitelist";
        loadWhitelist();

        // Fenster „close to tray“
        this->hide();
        notify("Automatischer Scan", "Starte initialen Root-Binary-Scan …");

        // Timer für Endlosschleife: scanAllBinaries() in konfigurierbarem Intervall
        loopTimer_ = new QTimer(this);
        connect(loopTimer_, &QTimer::timeout, this, [this]() {
            if (!scanRunning_.load()) {
                this->scanAllBinaries();
            } else {
                output_->append("Vorheriger Scan läuft noch — nächster Durchlauf übersprungen.");
            }
        });
        loopTimer_->start(spinLoopMinutes_->value() * 60 * 1000);

        // Wenn der Benutzer das Intervall ändert, Timer anpassen
        connect(spinLoopMinutes_, qOverload<int>(&QSpinBox::valueChanged), this, [this](int v){
            if (loopTimer_->isActive()) loopTimer_->start(v * 60 * 1000);
        });

        // Sofortigen ersten Durchlauf starten (Root-Binaries)
        QTimer::singleShot(500, this, [this](){ this->scanAllBinaries(); });
    }

    ~MainWindow() {
        if (loopTimer_) loopTimer_->stop();
        stopScan();
        for (auto t : workerThreads_) {
            if (t->isRunning()) {
                t->quit();
                t->wait();
            }
            delete t;
        }
    }

protected:
    void closeEvent(QCloseEvent *e) override {
        e->ignore();
        this->hide();
        if (tray_) tray_->showMessage("ClamAV Quick Scan", "Läuft im Tray weiter.", QSystemTrayIcon::Information, 2000);
    }

private slots:
    void scanOpenFiles() {
        QString err;
        auto paths = Collectors::collectOpenFiles(qgetenv("USER"), &err);
        if (!err.isEmpty()) {
            QMessageBox::warning(this, "Fehler", err);
            return;
        }
        startParallelScanWithFilters(paths);
    }

    void scanFrequentExecs() {
        auto history = QDir::homePath() + "/.bash_history";
        auto paths = Collectors::collectFrequentExecutables(history, spinTopN_->value());
        if (paths.isEmpty()) output_->append("Keine häufigen Executables ermittelt. Prüfe ~/.bash_history.");
        startParallelScanWithFilters(paths);
    }

    void scanRecentFiles() {
        QString root = chkHomeOnly_->isChecked() ? QDir::homePath() : "/";
        auto paths = Collectors::collectRecentFiles(root, spinHours_->value());
        startParallelScanWithFilters(paths);
    }

    // Neuer Slot: kompletter Root-Binary-Scan
    void scanAllBinaries() {
        output_->append("Starte vollständigen Scan: Suche nach Binaries unter '/' (dies kann lange dauern) …");
        auto paths = Collectors::collectAllBinaries("/");
        startParallelScanWithFilters(paths);
    }

    void scanCustom() {
        QFileDialog dlg(this, "Datei oder Ordner wählen");
        dlg.setFileMode(QFileDialog::ExistingFiles);
        dlg.setOption(QFileDialog::DontUseNativeDialog, true);
        if (dlg.exec()) {
            auto selected = dlg.selectedFiles();
            auto expanded = expandSelection(selected);
            startParallelScanWithFilters(expanded.isEmpty() ? selected : expanded);
        }
    }

    void editWhitelist() {
        if (!QFile::exists(whitelistPath_)) {
            QFile f(whitelistPath_);
            if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream ts(&f);
                ts << "# Eine Zeile pro Pfad, absolute Pfade empfohlen\n";
                ts << "# Beispiel: /usr/local/bin/special-binary\n";
                f.close();
            }
        }
        QProcess::startDetached("xdg-open", {whitelistPath_});
        QMessageBox::information(this, "Whitelist", QString("Whitelist geöffnet: %1\nSpeichere und schließe den Editor, dann wird die Whitelist beim nächsten Scan neu geladen.").arg(whitelistPath_));
    }

    // Lokaler Worker-Fortschritt (pro Worker)
    void onWorkerProgressLocal(int percent100000_local, const QString &currentFile) {
        Q_UNUSED(percent100000_local);
        Q_UNUSED(currentFile);
    }

    void onWorkerMessage(const QString &msg) {
        output_->append(msg);
        if (msg.startsWith("Scanne: ")) {
            const QString path = msg.mid(QString("Scanne: ").size());
            const QString name = QFileInfo(path).fileName();
            notify("Scan gestartet", name.isEmpty() ? path : name);
        }
    }

    void onSubWorkerFinished(const QStringList &infected, const QStringList &cleaned, const QStringList &errors, int processedCount) {
        QMutexLocker lk(&aggMutex_);
        aggregatedInfected_ += infected;
        aggregatedCleaned_ += cleaned;
        aggregatedErrors_ += errors;
        processedCount_.fetchAndAddRelaxed(processedCount);

        const int total = totalFiles_.load();
        const int processed = processedCount_.load();
        const int percent100000 = total > 0 ? int((processed * 100000.0) / total) : 100000;
        progress_->setValue(percent100000);

        const double pct = percent100000 / 1000.0;
        const QString pctText = QString::number(pct, 'f', 3) + "%";
        updateTrayIconWithCount(aggregatedInfected_.size());
        tray_->setToolTip(QString("ClamAV Quick Scan — %1").arg(pctText));

        activeWorkers_--;
        if (activeWorkers_ <= 0) {
            scanRunning_.store(false);
            status_->setText("Fertig.");
            btnStop_->setEnabled(false);
            actStop_->setEnabled(false);
            // If infections found, tray icon already updated to danger by updateTrayIconWithCount
            if (aggregatedInfected_.isEmpty()) {
                tray_->setIcon(baseIcon_);
                tray_->setToolTip("ClamAV Quick Scan — bereit");
            }

            if (!aggregatedInfected_.isEmpty()) {
                output_->append("\nFunde:");
                for (const auto &line : aggregatedInfected_) output_->append(line);
            } else {
                output_->append("\nKeine Infektionen gefunden (oder clamscan meldete keine).");
            }

            if (!aggregatedCleaned_.isEmpty()) {
                output_->append("\nBereinigt:");
                for (const auto &line : aggregatedCleaned_) output_->append(line);
            }

            if (!aggregatedErrors_.isEmpty()) {
                output_->append("\nFehler/Hinweise:");
                for (const auto &e : aggregatedErrors_) output_->append(e);
            }

            // AUTOMATISCHES LÖSCHEN: wenn aktiviert, bereinige gefundene Dateien automatisch (ohne Nachfrage)
            if (!aggregatedInfected_.isEmpty() && chkAutoCleanGlobal_->isChecked()) {
                output_->append("\nAutomatische Bereinigung aktiviert: starte Bereinigung der gefundenen Dateien...");
                autoCleanPathsFromFindings(aggregatedInfected_);
            }

            for (auto t : workerThreads_) {
                if (t->isRunning()) {
                    t->quit();
                    t->wait();
                }
                delete t;
            }
            workerThreads_.clear();
            workers_.clear();

            aggregatedInfected_.clear();
            aggregatedCleaned_.clear();
            aggregatedErrors_.clear();
            processedCount_.store(0);
            totalFiles_.store(0);

            // Reload whitelist so edits take effect for next run
            loadWhitelist();
        }
    }

    void stopScan() {
        if (!scanRunning_.load()) return;
        for (auto w : workers_) {
            if (w) QMetaObject::invokeMethod(w, "stop", Qt::QueuedConnection);
        }
        btnStop_->setEnabled(false);
        actStop_->setEnabled(false);
        tray_->setIcon(baseIcon_);
        tray_->setToolTip("ClamAV Quick Scan — gestoppt");
        lastNotifyPercent_ = -1;
        notify("Scan gestoppt", "Vorgang wurde abgebrochen.");
        scanRunning_.store(false);
    }

private:
    // Implementierungen ergänzt weiter unten
    void startParallelScanWithFilters(const QStringList &paths);
    void startParallelScan(const QStringList &paths);
    QIcon makeProgressIcon(int pct);
    void notify(const QString &title, const QString &msg);
    void loadWhitelist();
    bool isWhitelisted(const QString &path) const;
    QStringList expandSelection(const QStringList &sel) const;
    void autoCleanPathsFromFindings(const QStringList &findings);

    // New: update tray icon to show danger and count badge
    void updateTrayIconWithCount(int count) {
        // If no infections, restore base icon
        if (count <= 0) {
            tray_->setIcon(baseIcon_);
            return;
        }
        // Create a danger icon with badge
        const int size = 64;
        QPixmap pm(size, size);
        pm.fill(Qt::transparent);
        QPainter p(&pm);
        p.setRenderHint(QPainter::Antialiasing);

        // Draw red rounded square as danger background
        p.setBrush(QColor("#c62828")); // red
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(0, 0, size, size, 10, 10);

        // Draw white "!" or "AV" to indicate danger
        p.setPen(Qt::white);
        QFont f = p.font();
        f.setBold(true);
        f.setPointSize(20);
        p.setFont(f);
        p.drawText(pm.rect(), Qt::AlignCenter, "!");

        // Draw badge with count in top-right corner
        QString cnt = QString::number(count);
        int badgeSize = 28;
        QRect badgeRect(size - badgeSize - 4, 4, badgeSize, badgeSize);
        p.setBrush(QColor("#ffeb3b")); // yellow badge
        p.setPen(Qt::NoPen);
        p.drawEllipse(badgeRect);

        p.setPen(Qt::black);
        QFont bf = p.font();
        bf.setBold(true);
        bf.setPointSize(10);
        p.setFont(bf);
        p.drawText(badgeRect, Qt::AlignCenter, cnt);

        p.end();
        tray_->setIcon(QIcon(pm));
        tray_->setToolTip(QString("ClamAV Quick Scan — %1 Fund(e)").arg(count));
    }

    // UI Elemente
    QSpinBox *spinTopN_;
    QSpinBox *spinHours_;
    QCheckBox *chkClamd_;
    QCheckBox *chkHomeOnly_;
    QCheckBox *chkAutoCleanGlobal_;
    QSpinBox *spinLoopMinutes_;
    QSpinBox *spinConcurrency_;
    QCheckBox *chkUseWhitelist_;
    QPushButton *btnEditWhitelist_;

    QPushButton *btnOpen_;
    QPushButton *btnFreq_;
    QPushButton *btnRecent_;
    QPushButton *btnCustom_;
    QPushButton *btnStop_;

    QProgressBar *progress_;
    QLabel *status_;
    QTextEdit *output_;

    QSystemTrayIcon *tray_;
    QMenu *trayMenu_;
    QIcon baseIcon_;
    QAction *actShow_;
    QAction *actScanOpen_;
    QAction *actScanFreq_;
    QAction *actScanRecent_;
    QAction *actScanAllRoot_;
    QAction *actStop_;
    QAction *actQuit_;

    QTimer *loopTimer_;
    QElapsedTimer notifyTimer_;

    QString whitelistPath_;

    // Aggregation
    QStringList aggregatedInfected_;
    QStringList aggregatedCleaned_;
    QStringList aggregatedErrors_;
    QMutex aggMutex_;
    QAtomicInteger<int> processedCount_;
    QAtomicInteger<int> totalFiles_;
    QAtomicInteger<bool> scanRunning_{false};

    QVector<QThread*> workerThreads_;
    QVector<ScanWorker*> workers_;
    int activeWorkers_ = 0;
    int lastNotifyPercent_ = -1;
};

// ----------------- "AI" Heuristik (lokal, deterministisch) -----------------------------------
// Diese Funktion bewertet Pfade nach einfachen Merkmalen (Größe, Alter, ausführbar, Pfadmuster).
static double aiScorePath(const QString &path) {
    QFileInfo fi(path);
    if (!fi.exists() || !fi.isFile()) return 0.0;

    double score = 0.0;

    if (isBinaryFile(path)) score += 50.0;

    qint64 size = fi.size();
    if (size > 0) {
        score += std::min(20.0, std::log(double(size + 1)) * 2.0);
    }

    qint64 ageSec = QDateTime::currentSecsSinceEpoch() - fi.lastModified().toSecsSinceEpoch();
    if (ageSec < 3600) score += 15.0;
    else if (ageSec < 86400) score += 8.0;
    else if (ageSec < 7*86400) score += 3.0;

    const QString p = fi.absoluteFilePath().toLower();
    if (p.contains("/tmp/") || p.contains("/var/tmp/")) score += 10.0;
    if (p.contains("/downloads/") || p.contains("/download/")) score += 8.0;
    if (p.contains("node_modules") || p.contains(".cache") || p.contains("venv")) score += 4.0;

    if (p.endsWith(".sh") || p.endsWith(".py") || p.endsWith(".pl") || p.endsWith(".jar")) score += 6.0;

    if (p.contains("update") || p.contains("install") || p.contains("setup") || p.contains("run")) score += 3.0;

    return score;
}

// ----------------- Ergänzende Implementierungen für MainWindow -----------------

// Whitelist: einfache Implementierung mit statischem Container
static QSet<QString> s_whitelist_cache;

void MainWindow::loadWhitelist() {
    s_whitelist_cache.clear();
    const QString path = whitelistPath_;
    QFile f(path);
    if (!f.exists()) return;
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream ts(&f);
    while (!ts.atEnd()) {
        QString line = ts.readLine().trimmed();
        if (line.isEmpty()) continue;
        if (line.startsWith('#')) continue;
        QFileInfo fi(line);
        s_whitelist_cache.insert(fi.absoluteFilePath());
    }
    f.close();
    output_->append(QString("Whitelist geladen: %1 Einträge").arg(s_whitelist_cache.size()));
}

bool MainWindow::isWhitelisted(const QString &path) const {
    if (path.isEmpty()) return false;
    QFileInfo fi(path);
    return s_whitelist_cache.contains(fi.absoluteFilePath());
}

// Expand selection: wenn ein Eintrag ein Verzeichnis ist, rekursiv Dateien sammeln
QStringList MainWindow::expandSelection(const QStringList &sel) const {
    QStringList out;
    for (const auto &p : sel) {
        QFileInfo fi(p);
        if (!fi.exists()) continue;
        if (fi.isDir()) {
            QDirIterator it(p, QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext()) {
                out << it.next();
            }
        } else if (fi.isFile()) {
            out << fi.absoluteFilePath();
        }
    }
    return uniqueAbs(out);
}

// Versucht, aus den clamscan-Fundzeilen Pfade zu extrahieren und zu löschen.
// Erwartetes Format: "/path/to/file: MalwareName FOUND" oder "Removed: /path"
void MainWindow::autoCleanPathsFromFindings(const QStringList &findings) {
    QStringList cleaned;
    QStringList failed;
    for (const auto &line : findings) {
        QString path;
        int colon = line.indexOf(':');
        if (colon > 0) {
            QString maybePath = line.left(colon).trimmed();
            if (maybePath.startsWith('/')) path = maybePath;
        }
        if (path.isEmpty()) {
            QRegularExpression re("Removed[: ]+(.+)");
            auto m = re.match(line);
            if (m.hasMatch()) {
                path = m.captured(1).trimmed();
            }
        }
        if (path.isEmpty() && line.startsWith('/')) {
            int sp = line.indexOf(' ');
            path = (sp > 0) ? line.left(sp) : line;
        }

        if (path.isEmpty()) continue;
        QFileInfo fi(path);
        if (!fi.exists()) {
            failed << QString("%1 (nicht gefunden)").arg(path);
            continue;
        }
        // Versuche zu löschen
        if (QFile::remove(path)) {
            cleaned << path;
            output_->append(QString("Automatisch gelöscht: %1").arg(path));
        } else {
            failed << QString("%1 (Löschen fehlgeschlagen)").arg(path);
            output_->append(QString("Fehler beim Löschen: %1").arg(path));
        }
    }

    if (!cleaned.isEmpty()) {
        aggregatedCleaned_ += cleaned;
    }
    if (!failed.isEmpty()) {
        aggregatedErrors_ += failed;
    }
}

// Einfacher Start-Wrapper: Filter angewendet, dann eigentlicher Start
void MainWindow::startParallelScanWithFilters(const QStringList &paths) {
    QStringList uniq = uniqueAbs(paths);
    if (uniq.isEmpty()) {
        output_->append("Keine zu scannenden Dateien gefunden.");
        return;
    }

    if (chkUseWhitelist_->isChecked()) loadWhitelist();

    struct Item { QString path; double score; };
    QVector<Item> candidates;
    QStringList skippedWhitelist;
    QStringList skippedNotBinary;
    for (const auto &p : uniq) {
        if (chkUseWhitelist_->isChecked() && isWhitelisted(p)) {
            skippedWhitelist << p;
            continue;
        }
        if (!isBinaryFile(p)) {
            skippedNotBinary << p;
            continue;
        }
        double s = aiScorePath(p);
        candidates.append({p, s});
    }

    std::sort(candidates.begin(), candidates.end(), [](const Item &a, const Item &b){
        if (a.score == b.score) return a.path < b.path;
        return a.score > b.score;
    });

    int limit = std::max(100, spinTopN_->value() * 10);
    QStringList filtered;
    for (int i = 0; i < candidates.size() && i < limit; ++i) filtered << candidates[i].path;

    output_->append(QString("Gefundene Elemente: %1; Binaries priorisiert: %2; Whitelist übersprungen: %3; Nicht-Binaries übersprungen: %4")
                    .arg(uniq.size()).arg(filtered.size()).arg(skippedWhitelist.size()).arg(skippedNotBinary.size()));

    if (!skippedWhitelist.isEmpty()) {
        output_->append("Beispiel Whitelist-Übersprungen:");
        for (int i = 0; i < std::min(3, skippedWhitelist.size()); ++i) output_->append("  " + skippedWhitelist.at(i));
    }
    if (!skippedNotBinary.isEmpty()) {
        output_->append("Beispiel Nicht-Binaries:");
        for (int i = 0; i < std::min(3, skippedNotBinary.size()); ++i) output_->append("  " + skippedNotBinary.at(i));
    }

    if (filtered.isEmpty()) {
        output_->append("Keine priorisierten Binaries zum Scannen gefunden.");
        return;
    }

    startParallelScan(filtered);
}

// Einfache (seriell arbeitende) Start-Implementierung.
// Für jetzt wird alles in einem Worker-Thread verarbeitet.
void MainWindow::startParallelScan(const QStringList &paths) {
    if (paths.isEmpty()) {
        output_->append("Keine zu scannenden Dateien gefunden.");
        return;
    }
    if (scanRunning_.load()) {
        output_->append("Scan läuft bereits. Neuer Scan wird nicht gestartet.");
        return;
    }

    scanRunning_.store(true);
    btnStop_->setEnabled(true);
    actStop_->setEnabled(true);
    status_->setText("Scan läuft...");
    output_->append(QString("Starte Scan für %1 Datei(en)...").arg(paths.size()));

    totalFiles_.store(paths.size());
    processedCount_.store(0);

    QThread *t = new QThread();
    ScanWorker *w = new ScanWorker(paths, chkClamd_->isChecked(), chkAutoCleanGlobal_->isChecked());
    workers_.append(w);
    workerThreads_.append(t);

    w->moveToThread(t);
    connect(t, &QThread::started, w, &ScanWorker::run);
    connect(w, &ScanWorker::progressWithFileLocal, this, &MainWindow::onWorkerProgressLocal);
    connect(w, &ScanWorker::message, this, &MainWindow::onWorkerMessage);
    connect(w, &ScanWorker::finishedWorker, this, &MainWindow::onSubWorkerFinished);
    connect(w, &ScanWorker::finishedWorker, t, &QThread::quit);
    connect(t, &QThread::finished, t, &QThread::deleteLater);
    connect(t, &QThread::finished, w, &QObject::deleteLater);

    activeWorkers_ = 1;
    t->start();
}

// Erzeugt ein kleines Icon mit Prozentanzeige (Overlay auf baseIcon_)
QIcon MainWindow::makeProgressIcon(int pct) {
    const int size = 64;
    QPixmap pm(size, size);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);

    if (!baseIcon_.isNull()) {
        QPixmap base = baseIcon_.pixmap(size, size);
        p.drawPixmap(0, 0, base);
    } else {
        p.setBrush(QColor("#2d6cdf"));
        p.setPen(Qt::NoPen);
        p.drawEllipse(0, 0, size, size);
    }

    QRectF rect(4, 4, size-8, size-8);
    QPen pen(QColor(255,255,255,200));
    pen.setWidth(6);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    const qreal span = (360.0 * pct) / 100.0;
    p.drawArc(rect, 90 * 16, -span * 16);

    p.setPen(Qt::white);
    QFont f = p.font();
    f.setBold(true);
    f.setPointSize(10);
    p.setFont(f);
    p.drawText(pm.rect(), Qt::AlignCenter, QString::number(pct) + "%");
    p.end();
    return QIcon(pm);
}

// Notification helper: zeigt Tray-Nachricht und schreibt in Output
void MainWindow::notify(const QString &title, const QString &msg) {
    if (tray_) {
        tray_->showMessage(title, msg, QSystemTrayIcon::Information, 3000);
    }
    if (output_) {
        output_->append(QString("[%1] %2").arg(title, msg));
    }
}

// MOC benötigt für Q_OBJECT
#include "antivir.moc"

// ---------- main ----------
int main(int argc, char **argv) {
    QApplication a(argc, argv);

    // Passwortabfrage und Relaunch als root (grafisch)
    if (!ensureRunningAsRootWithPassword(nullptr)) {
        return 1;
    }

    // Danach läuft die neue Instanz als root; die alte wurde beendet.
    backgroundize();

    const QString prog = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
    int killed = killOtherInstancesByName(prog);
    qDebug() << "Beendete andere Instanzen:" << killed;

    QScopedPointer<QLocalServer> server;
    if (!ensureSingleInstance(prog, server)) {
        qWarning("Eine andere Instanz ist aktiv (QLocalServer). Beende.");
        return 1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}

