// antivir.cpp
// ClamAV Quick Scan — vollständige, amalgamierte Implementierung
// Verhalten: startet immer "daemonisiert" im Sinne von: kein sichtbares Hauptfenster, läuft im Tray
// - kein Fork/Detach mehr (keine setsid/fork), damit Tray und Desktop‑Notifications zuverlässig funktionieren
// - schreibt PID‑Datei
// - beendet alte Instanzen (überspringt eigene PID und sehr frische Prozesse)
// - clamd‑Probe, throttled clamdscan, aggressive Priorisierung, Autostart XFCE

#include <QtWidgets>
#include <QAtomicInteger>
#include <QApplication>
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
#include <QMessageBox>
#include <QFileDialog>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QPainter>
#include <QTimer>
#include <QElapsedTimer>
#include <QCloseEvent>
#include <csignal>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#ifdef QT_DBUS_LIB
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusVariant>
#endif

// ---------------- Konfiguration ----------------
static const QString DEFAULT_LOGFILE = QDir::homePath() + "/.clamav-quickscan.log";
static const QString DEFAULT_PIDFILE = QDir::homePath() + "/.clamav-quickscan.pid";

// ---------------- Hilfsfunktionen ----------------
static bool writePidFile(const QString &path) {
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream ts(&f);
    ts << getpid() << "\n";
    f.close();
    return true;
}

static QString runWhich(const QString &cmd) {
    QProcess p; p.start("which", {cmd}); p.waitForFinished(500);
    return QString::fromUtf8(p.readAllStandardOutput()).trimmed();
}
static bool hasProgram(const QString &name) { return !runWhich(name).isEmpty(); }

static QStringList uniqueAbs(const QStringList &in) {
    QSet<QString> seen; QStringList out;
    for (const auto &p : in) {
        QFileInfo fi(p);
        const QString abs = fi.absoluteFilePath();
        if (!seen.contains(abs)) { seen.insert(abs); out << abs; }
    }
    return out;
}
static bool isRegularFile(const QString &p) { QFileInfo fi(p); return fi.isFile(); }
static bool isExecutableFile(const QString &p) { QFileInfo fi(p); return fi.isFile() && fi.isExecutable(); }

// ---------------- Prozessalter (für Schutz frisch gestarteter Prozesse) ----------------
static double getProcessAgeSeconds(pid_t pid) {
    QFile f(QString("/proc/%1/stat").arg(pid));
    if (!f.open(QIODevice::ReadOnly)) return -1.0;
    QList<QByteArray> parts = f.readAll().split(' ');
    f.close();
    if (parts.size() < 22) return -1.0;
    bool ok = false;
    long long starttime_jiffies = parts.at(21).toLongLong(&ok);
    if (!ok) return -1.0;
    QFile u("/proc/uptime");
    if (!u.open(QIODevice::ReadOnly)) return -1.0;
    double uptime = QString::fromUtf8(u.readLine().split(' ').first()).toDouble(&ok);
    u.close();
    if (!ok) return -1.0;
    long clk_tck = sysconf(_SC_CLK_TCK);
    if (clk_tck <= 0) return -1.0;
    return uptime - (double(starttime_jiffies) / double(clk_tck));
}

// ---------------- Kill-Prozesse, eigene PID + frische Prozesse schützen ----------------
static bool killProcessesByExactName(const QString &name, int waitMs = 4000) {
    if (name.isEmpty()) return true;
    const pid_t selfPid = getpid();
    const double freshThresholdSec = 3.0;

    auto shouldSkip = [&](pid_t pid) {
        if (pid == selfPid) return true; // eigene Instanz nie killen
        double age = getProcessAgeSeconds(pid);
        if (age >= 0.0 && age < freshThresholdSec) return true; // sehr frisch -> überspringen
        return false;
    };

    // Erst SIGTERM (pkill/killall falls vorhanden)
    if (hasProgram("pkill")) QProcess::execute("pkill", {"-x", name});
    else if (hasProgram("killall")) QProcess::execute("killall", {"-q", "-w", name});

    // Warte und prüfe
    int waited = 0;
    while (waited < waitMs) {
        bool anyRunning = false;
        if (hasProgram("pgrep")) {
            QProcess p; p.start("pgrep", {"-x", name}); p.waitForFinished(300);
            const auto lines = QString::fromUtf8(p.readAllStandardOutput()).split('\n', Qt::SkipEmptyParts);
            for (const auto &ln : lines) {
                bool ok = false; pid_t pid = ln.toInt(&ok);
                if (!ok || pid <= 0) continue;
                if (shouldSkip(pid)) continue;
                anyRunning = true; break;
            }
        } else {
            QDir proc("/proc");
            const auto entries = proc.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
            for (const QString &e : entries) {
                bool ok = false; int pidInt = e.toInt(&ok); if (!ok) continue;
                pid_t pid = static_cast<pid_t>(pidInt);
                QFile f(QString("/proc/%1/comm").arg(pid));
                if (!f.open(QIODevice::ReadOnly)) continue;
                const QString comm = QString::fromUtf8(f.readAll()).trimmed();
                f.close();
                if (comm != name) continue;
                if (shouldSkip(pid)) continue;
                anyRunning = true; break;
            }
        }
        if (!anyRunning) return true;
        QThread::msleep(200); waited += 200;
    }

    // Notfalls SIGKILL, aber eigene/frische PIDs überspringen
    if (hasProgram("pgrep")) {
        QProcess p; p.start("pgrep", {"-x", name}); p.waitForFinished(500);
        const auto lines = QString::fromUtf8(p.readAllStandardOutput()).split('\n', Qt::SkipEmptyParts);
        for (const auto &ln : lines) {
            bool ok = false; pid_t pid = ln.toInt(&ok);
            if (!ok || pid <= 0) continue;
            if (shouldSkip(pid)) continue;
            kill(pid, SIGKILL);
        }
    } else {
        QDir proc("/proc");
        const auto entries = proc.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &e : entries) {
            bool ok = false; int pidInt = e.toInt(&ok); if (!ok) continue;
            pid_t pid = static_cast<pid_t>(pidInt);
            QFile f(QString("/proc/%1/comm").arg(pid));
            if (!f.open(QIODevice::ReadOnly)) continue;
            const QString comm = QString::fromUtf8(f.readAll()).trimmed();
            f.close();
            if (comm != name) continue;
            if (shouldSkip(pid)) continue;
            kill(pid, SIGKILL);
        }
    }
    return true;
}

// ---------------- PID/Instance Hilfen ----------------
static pid_t readPidFromFile(const QString &pidFilePath) {
    QFile f(pidFilePath);
    if (!f.exists() || !f.open(QIODevice::ReadOnly | QIODevice::Text)) return 0;
    QTextStream ts(&f);
    const QString line = ts.readLine().trimmed();
    f.close();
    bool ok = false; qint64 p = line.toLongLong(&ok);
    if (!ok || p <= 0) return 0;
    return static_cast<pid_t>(p);
}
static bool pidIsRunning(pid_t pid) {
    if (pid <= 0) return false;
    int res = kill(pid, 0);
    if (res == 0) return true;
    if (errno == EPERM) return true;
    return false;
}
static bool terminatePidGraceful(pid_t pid, int waitMs = 3000) {
    if (pid <= 0 || !pidIsRunning(pid)) return true;
    kill(pid, SIGTERM);
    const int step = 200; int waited = 0;
    while (waited < waitMs) { if (!pidIsRunning(pid)) return true; QThread::msleep(step); waited += step; }
    if (pidIsRunning(pid)) {
        kill(pid, SIGKILL);
        waited = 0;
        while (waited < 2000) { if (!pidIsRunning(pid)) return true; QThread::msleep(step); waited += step; }
    }
    return !pidIsRunning(pid);
}
static void cleanupOldInstanceFiles(const QString &pidFilePath, const QString &localServerName) {
    QFile::remove(pidFilePath);
    QLocalServer::removeServer(localServerName);
}
static bool stopExistingInstanceAndWait(const QString &pidFilePath, const QString &localServerName, QStringList *log = nullptr) {
    pid_t pid = readPidFromFile(pidFilePath);
    if (log) log->append(QString("Gefundene PID aus %1: %2").arg(pidFilePath).arg(pid));
    if (pid == 0) { cleanupOldInstanceFiles(pidFilePath, localServerName); if (log) log->append("Keine gültige PID gefunden; entferne ggf. alten QLocalServer Socket."); return true; }
    if (!pidIsRunning(pid)) { if (log) log->append(QString("PID %1 läuft nicht mehr; entferne PID‑Datei/socket.").arg(pid)); cleanupOldInstanceFiles(pidFilePath, localServerName); return true; }
    if (log) log->append(QString("Versuche PID %1 mit SIGTERM zu beenden...").arg(pid));
    bool ok = terminatePidGraceful(pid, 4000);
    if (!ok) { if (log) log->append(QString("SIGTERM/SIGKILL für PID %1 schlug fehl.").arg(pid)); return false; }
    cleanupOldInstanceFiles(pidFilePath, localServerName);
    if (log) log->append(QString("Alte Instanz (PID %1) beendet und Dateien entfernt.").arg(pid));
    return true;
}

// ---------------- clamd socket probing ----------------
static QStringList commonClamdSocketPaths() {
    return {
        "/var/run/clamd.scan/clamd.sock",
        "/var/run/clamd.sock",
        "/var/run/clamd/clamd.sock",
        "/var/run/clamav/clamd.sock",
        "/var/run/clamav/clamd.scan.sock",
        "/run/clamd.scan/clamd.sock",
        "/run/clamd.sock",
        "/run/clamav/clamd.sock"
    };
}
static QString probeClamdSocket(const QString &socketPath, int timeoutMs = 1500) {
    QLocalSocket sock;
    sock.connectToServer(socketPath);
    if (!sock.waitForConnected(timeoutMs)) return QString();
    sock.write("PING\n");
    if (!sock.waitForBytesWritten(timeoutMs)) { sock.disconnectFromServer(); return QString(); }
    if (!sock.waitForReadyRead(timeoutMs)) { sock.disconnectFromServer(); return QString(); }
    sock.readLine();
    sock.write("VERSION\n");
    if (!sock.waitForBytesWritten(timeoutMs)) { sock.disconnectFromServer(); return QString(); }
    if (!sock.waitForReadyRead(timeoutMs)) { sock.disconnectFromServer(); return QString(); }
    QByteArray ver = sock.readLine().trimmed();
    sock.disconnectFromServer();
    if (ver.isEmpty()) return QString();
    return QString::fromUtf8(ver);
}
static QPair<QString, QString> findResponsiveClamdSocket() {
    const auto paths = commonClamdSocketPaths();
    for (const auto &p : paths) if (QFile::exists(p)) { QString ver = probeClamdSocket(p); if (!ver.isEmpty()) return {p, ver}; }
    const char *envp = getenv("CLAMD_SOCKET");
    if (envp) { QString p(envp); if (QFile::exists(p)) { QString ver = probeClamdSocket(p); if (!ver.isEmpty()) return {p, ver}; } }
    return {"", ""};
}

// ---------------- systemd helpers (best-effort) ----------------
static bool systemctlSetCpuQuota(const QString &unit, const QString &quota = "5%") {
    if (!hasProgram("systemctl")) return false;
    QProcess p; p.start("systemctl", {"set-property", unit, QString("CPUQuota=%1").arg(quota)});
    if (!p.waitForFinished(5000)) { p.kill(); return false; }
    return (p.exitCode() == 0);
}
static bool systemctlRestartUnit(const QString &unit, int timeoutMs = 8000) {
    if (!hasProgram("systemctl")) return false;
    QProcess p; p.start("systemctl", {"restart", unit});
    if (!p.waitForFinished(timeoutMs)) { p.kill(); return false; }
    return (p.exitCode() == 0);
}
static bool ensureClamdCpuQuota(int timeoutMs = 8000, const QString &quota = "5%") {
    const QStringList units = {"clamd.service","clamav-daemon.service","clamd@scan.service","clamd@.service"};
    for (const auto &u : units) {
        if (systemctlSetCpuQuota(u, quota)) { systemctlRestartUnit(u, timeoutMs); return true; }
    }
    return false;
}

// ---------------- Entropie / Heuristik / Collectors ----------------
// (gleich wie vorher — aus Platzgründen nicht kommentiert; unverändert implementiert)
static double fileEntropySample(const QString &path, qint64 maxBytes = 65536) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return 0.0;
    QByteArray data = f.read(maxBytes);
    f.close();
    if (data.isEmpty()) return 0.0;
    QVector<int> counts(256, 0);
    for (auto b : data) counts[quint8(b)]++;
    const double len = double(data.size());
    double ent = 0.0;
    for (int c : counts) { if (c == 0) continue; double p = double(c) / len; ent -= p * std::log2(p); }
    return ent;
}

static double scoreFileAggressive(const QString &path, const QSet<QString> &openSet, const QHash<QString,int> &execFreq, qint64 nowSecs) {
    const double W_OPEN=150.0, W_RECENT=120.0, W_EXEC=90.0, W_EXT=110.0, W_SIZE=-12.0, W_FREQ=40.0, W_ENTROPY=80.0, W_PATH=30.0;
    double score = 0.0; QFileInfo fi(path);
    if (openSet.contains(path)) score += W_OPEN;
    if (fi.exists()) {
        qint64 mtime = fi.lastModified().toSecsSinceEpoch();
        qint64 age = qMax<qint64>(1, nowSecs - mtime);
        double recent = 1.0 / std::log(double(age + 10));
        score += W_RECENT * recent;
    }
    if (fi.exists() && fi.isExecutable()) score += W_EXEC;
    static const QSet<QString> suspiciousExt = {"exe","dll","scr","bat","cmd","ps1","sh","jar","py","pl","php","js","run","bin","so","elf","apk"};
    const QString ext = fi.suffix().toLower();
    if (suspiciousExt.contains(ext)) score += W_EXT;
    qint64 size = fi.exists() ? fi.size() : 0;
    if (size > 0) { double sizeScore = 1.0 / (1.0 + std::log(double(size + 1024))); score += W_SIZE * sizeScore; }
    if (execFreq.contains(path)) { double freq = execFreq.value(path); score += W_FREQ * std::log(1.0 + freq); }
    if (path.startsWith("/tmp") || path.startsWith("/var/tmp")) score += W_PATH;
    if (path.startsWith(QDir::homePath())) score += W_PATH * 0.6;
    if (path.startsWith("/var/www") || path.contains("/www/") || path.contains("/html/")) score += W_PATH * 0.8;
    if (path.startsWith("/usr/bin") || path.startsWith("/usr/local/bin")) score += W_PATH * 0.4;
    double ent = fileEntropySample(path, 65536);
    if (ent > 7.0) { score += W_ENTROPY * ((ent - 7.0) / 1.5 + 1.0); }
    static const QSet<QString> scriptExt = {"sh","py","pl","rb","php","js","ps1"};
    if (scriptExt.contains(ext)) score += 8.0;
    return score;
}

namespace Collectors {
    QStringList collectOpenFiles(const QString &user, QString *errorOut) {
        QStringList files; QProcess proc; QStringList args = {"-F","n"}; if (!user.isEmpty()) args << "-u" << user;
        proc.start("lsof", args); proc.waitForFinished(-1);
        if (proc.exitCode() != 0) { if (errorOut) *errorOut = QString("lsof Fehler: %1").arg(QString::fromUtf8(proc.readAllStandardError()).trimmed()); return {}; }
        const auto lines = QString::fromUtf8(proc.readAllStandardOutput()).split('\n', Qt::SkipEmptyParts);
        for (const auto &line : lines) if (line.startsWith("n")) { const QString path = line.mid(1); if (path.startsWith("/") && isRegularFile(path)) files << path; }
        return uniqueAbs(files);
    }

    QStringList collectFrequentExecutables(const QString &historyFile, int topN) {
        QFile f(historyFile); if (!f.exists() || !f.open(QIODevice::ReadOnly | QIODevice::Text)) return {};
        QHash<QString,int> counter; QTextStream ts(&f);
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
        QStringList result; int n = std::min(topN, items.size());
        for (int i = 0; i < n; ++i) { const QString p = items[i].first; if (isExecutableFile(p)) result << p; }
        return uniqueAbs(result);
    }

    QStringList collectRecentFiles(const QString &root, int hours) {
        const qint64 cutoff = QDateTime::currentSecsSinceEpoch() - hours * 3600;
        static const QSet<QString> excludeDirs = {".cache","node_modules","venv",".venv",".git"};
        QStringList matches; QDirIterator it(root, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            const QString p = it.next(); QFileInfo fi(p);
            const auto parts = fi.absoluteFilePath().split('/');
            bool excluded = false;
            for (const auto &part : parts) { if (excludeDirs.contains(part)) { excluded = true; break; } }
            if (excluded) continue;
            if (!fi.exists() || !fi.isFile()) continue;
            const qint64 mtime = fi.lastModified().toSecsSinceEpoch();
            if (mtime >= cutoff) matches << fi.absoluteFilePath();
        }
        return uniqueAbs(matches);
    }

    QStringList collectAllFilesUnderRoot(const QString &root = "/") {
        static const QSet<QString> excludeTop = {"proc","sys","dev","run","mnt","media","tmp","lost+found"};
        QStringList matches; QDirIterator it(root, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            const QString p = it.next(); QFileInfo fi(p);
            if (!fi.exists() || !fi.isFile()) continue;
            const QString abs = fi.absoluteFilePath();
            const QStringList parts = abs.split('/', Qt::SkipEmptyParts);
            if (!parts.isEmpty()) {
                const QString top = parts.first();
                if (excludeTop.contains(top)) continue;
                if (parts.size() >= 2 && parts[0] == "var" && (parts[1] == "tmp" || parts[1] == "run")) continue;
            }
            matches << abs;
        }
        return uniqueAbs(matches);
    }
}

// ---------------- Alarmdialog ----------------
class AlarmDialog : public QDialog {
    Q_OBJECT
public:
    AlarmDialog(const QStringList &findings, bool defaultAutoClean, QWidget *parent = nullptr)
        : QDialog(parent) {
        setWindowTitle("Virenfund – Maßnahmen");
        resize(700, 400);
        auto *layout = new QVBoxLayout(this);
        layout->addWidget(new QLabel("Folgende Funde wurden gemeldet:", this));
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

// ---------------- clamdscan throttled starter ----------------
static void startClamdscanThrottled(QProcess &proc, const QStringList &clamdArgs, const QString &quota = "5%") {
    if (hasProgram("systemd-run")) {
        QStringList wrapper; wrapper << "--scope" << "-p" << QString("CPUQuota=%1").arg(quota) << "clamdscan";
        wrapper.append(clamdArgs);
        proc.start("systemd-run", wrapper);
        return;
    }
    if (hasProgram("cpulimit")) {
        QString qstr = quota; qstr.remove('%');
        bool ok = false; int q = qstr.toInt(&ok); if (!ok) q = 5;
        QStringList wrapper; wrapper << "-l" << QString::number(q) << "--" << "clamdscan";
        wrapper.append(clamdArgs);
        proc.start("cpulimit", wrapper);
        return;
    }
    proc.start("clamdscan", clamdArgs);
}

// ---------------- ScanWorker ----------------
class ScanWorker : public QObject {
    Q_OBJECT
public:
    ScanWorker(QStringList paths, bool doAutoClean)
        : paths_(std::move(paths)), autoClean_(doAutoClean), stop_(false) {}
public slots:
    void run() {
        QStringList infected, cleaned, errors;
        const int total = paths_.size();
        for (int i = 0; i < total; ++i) {
            if (stop_.loadRelaxed()) { errors << "Scan abgebrochen."; break; }
            const QString p = paths_.at(i);
            emit message(QString("Scanne: %1").arg(p));
            QStringList args; args << "--fdpass" << "--no-summary"; if (autoClean_) args << "--remove"; args << p;
            QProcess proc; startClamdscanThrottled(proc, args, "5%"); proc.waitForFinished(-1);
            const QString out = proc.readAllStandardOutput(); const QString err = proc.readAllStandardError(); const int rc = proc.exitCode();
            const auto lines = out.split('\n', Qt::SkipEmptyParts);
            for (const auto &line : lines) {
                if (line.endsWith("FOUND")) infected << line.trimmed();
                if (autoClean_ && (line.contains("Removed") || line.contains("removed"))) cleaned << line.trimmed();
            }
            if (rc != 0 && rc != 1) { if (!err.trimmed().isEmpty()) errors << QString("%1: %2").arg(p, err.trimmed()); }
            const int percent100000 = int(((i + 1) * 100000.0) / qMax(1, total));
            emit progressWithFile(percent100000, p);
        }
        emit finished(infected, cleaned, errors);
    }
    void stop() { stop_.storeRelaxed(true); }
signals:
    void progressWithFile(int percent100000, const QString &currentFile);
    void message(const QString &msg);
    void finished(const QStringList &infected, const QStringList &cleaned, const QStringList &errors);
private:
    QStringList paths_;
    bool autoClean_;
    QBasicAtomicInteger<bool> stop_;
};

// ---------------- MainWindow ----------------
class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(const QString &clamdSocketPath = QString(), const QString &clamdVersion = QString(), QWidget *parent = nullptr)
        : QWidget(parent), worker_(nullptr), tray_(nullptr), clamdSocketPath_(clamdSocketPath), clamdVersion_(clamdVersion)
    {
        setWindowTitle("ClamAV Quick Scan (Aggressiv)");
        resize(950, 650);

        auto *layout = new QVBoxLayout(this);

        auto *cfg = new QGroupBox("Einstellungen", this);
        auto *form = new QFormLayout(cfg);
        spinTopN_ = new QSpinBox(this); spinTopN_->setRange(5, 200); spinTopN_->setValue(50);
        spinHours_ = new QSpinBox(this); spinHours_->setRange(1, 168); spinHours_->setValue(48);
        chkClamd_ = new QCheckBox("clamdscan verwenden (immer aktiv)", this); chkClamd_->setChecked(true); chkClamd_->setEnabled(false);
        chkHomeOnly_ = new QCheckBox("Nur Home-Verzeichnis durchsuchen (nicht empfohlen für aggressiv)", this); chkHomeOnly_->setChecked(false);
        chkAutoCleanGlobal_ = new QCheckBox("Automatisch bereinigen (vorsichtig verwenden)", this);
        form->addRow("Top-N häufig ausgeführte Executables (History):", spinTopN_);
        form->addRow("Zeitfenster zuletzt geändert (Stunden):", spinHours_);
        form->addRow(chkClamd_);
        form->addRow(chkHomeOnly_);
        form->addRow(chkAutoCleanGlobal_);
        cfg->setLayout(form);

        auto *btnRow = new QHBoxLayout();
        btnOpen_   = new QPushButton("Geöffnete Dateien scannen", this);
        btnFreq_   = new QPushButton("Häufig ausgeführte Executables scannen", this);
        btnRecent_ = new QPushButton("Prioritätsscan / (Aggressiv)", this);
        btnCustom_ = new QPushButton("Eigenen Pfad/Ordner scannen…", this);
        btnStop_   = new QPushButton("Abbrechen", this); btnStop_->setEnabled(false);
        btnRow->addWidget(btnOpen_); btnRow->addWidget(btnFreq_); btnRow->addWidget(btnRecent_); btnRow->addWidget(btnCustom_); btnRow->addWidget(btnStop_);

        auto *progRow = new QHBoxLayout();
        progress_ = new QProgressBar(this); progress_->setRange(0, 100000);
        status_   = new QLabel("Bereit.", this);
        progRow->addWidget(progress_, 1); progRow->addWidget(status_);

        output_ = new QTextEdit(this); output_->setReadOnly(true);

        layout->addWidget(cfg);
        layout->addLayout(btnRow);
        layout->addLayout(progRow);
        layout->addWidget(output_, 1);

        connect(btnOpen_,   &QPushButton::clicked, this, &MainWindow::scanOpenFiles);
        connect(btnFreq_,   &QPushButton::clicked, this, &MainWindow::scanFrequentExecs);
        connect(btnRecent_, &QPushButton::clicked, this, &MainWindow::scanRootPriority);
        connect(btnCustom_, &QPushButton::clicked, this, &MainWindow::scanCustom);
        connect(btnStop_,   &QPushButton::clicked, this, &MainWindow::stopScan);

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
        actScanRecent_ = trayMenu_->addAction("Scan: Priorität / (Aggressiv)", this, &MainWindow::scanRootPriority);
        actStop_       = trayMenu_->addAction("Scan stoppen", this, &MainWindow::stopScan); actStop_->setEnabled(false);
        trayMenu_->addSeparator();
        actQuit_       = trayMenu_->addAction("Beenden", qApp, &QApplication::quit);

        tray_->setContextMenu(trayMenu_);
        tray_->setToolTip("ClamAV Quick Scan (Aggressiv)");
        tray_->setIcon(baseIcon_);
        tray_->show();

        connect(tray_, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason r){
            if (r == QSystemTrayIcon::Trigger) { this->showNormal(); this->raise(); this->activateWindow(); }
        });

        notifyTimer_.start();

        if (!clamdSocketPath_.isEmpty()) {
            output_->append(QString("Clamd socket: %1").arg(clamdSocketPath_));
            output_->append(QString("Clamd version: %1").arg(clamdVersion_));
            status_->setText(QString("Clamd: %1").arg(clamdVersion_));
        } else {
            output_->append("Kein clamd Socket gefunden oder clamd nicht erreichbar.");
            status_->setText("Clamd: nicht erreichbar");
        }

        // Startverhalten: Fenster nicht anzeigen, nur Tray (immer daemonisiert im Tray‑Sinn)
        this->hide();
        notify("ClamAV Quick Scan gestartet", "Läuft im Tray. Klicke das Icon für Optionen.");
        QTimer::singleShot(500, this, &MainWindow::scanRootPriority);
    }

    ~MainWindow() {
        stopScan();
        workerThread_.quit();
        workerThread_.wait();
    }

protected:
    void closeEvent(QCloseEvent *e) override {
        e->ignore();
        this->hide();
        if (tray_) tray_->showMessage("ClamAV Quick Scan", "Läuft im Tray weiter.", QSystemTrayIcon::Information, 2000);
    }

private slots:
    // (Methoden: scanRootPriority, scanOpenFiles, scanFrequentExecs, scanCustom, onWorkerProgressFile,
    //  onWorkerMessage, onWorkerFinished, stopScan — Implementierung wie zuvor, unverändert)
    void scanRootPriority();
    void scanOpenFiles();
    void scanFrequentExecs();
    void scanCustom();
    void onWorkerProgressFile(int percent100000, const QString &currentFile);
    void onWorkerMessage(const QString &msg);
    void onWorkerFinished(const QStringList &infected, const QStringList &cleaned, const QStringList &errors);
    void stopScan();

private:
    void startScanWithAlarm(const QStringList &paths);
    void autoCleanPathsFromFindings(const QStringList &findings);
    QStringList expandSelection(const QStringList &paths);
    QIcon makeProgressIcon(int percent);
    void notify(const QString &title, const QString &body, int iconTimeoutMs = 3000);

    // Widgets
    QTextEdit *output_;
    QLabel *status_;
    QProgressBar *progress_;
    QPushButton *btnOpen_;
    QPushButton *btnFreq_;
    QPushButton *btnRecent_;
    QPushButton *btnCustom_;
    QPushButton *btnStop_;
    QSpinBox *spinTopN_;
    QSpinBox *spinHours_;
    QCheckBox *chkClamd_;
    QCheckBox *chkHomeOnly_;
    QCheckBox *chkAutoCleanGlobal_;

    // Worker
    QThread workerThread_;
    ScanWorker *worker_;

    // Tray
    QSystemTrayIcon *tray_;
    QMenu *trayMenu_;
    QAction *actShow_;
    QAction *actScanOpen_;
    QAction *actScanFreq_;
    QAction *actScanRecent_;
    QAction *actStop_;
    QAction *actQuit_;
    QIcon baseIcon_;

    // Notifications
    int lastNotifyPercent_ = -1;
    QElapsedTimer notifyTimer_;

    // clamd info
    QString clamdSocketPath_;
    QString clamdVersion_;
};

// Implementations der MainWindow‑Methoden (aus Platzgründen kompakt, entsprechen vorheriger Logik)
void MainWindow::scanRootPriority() {
    QString lsofErr;
    QStringList openFiles = Collectors::collectOpenFiles(QString(), &lsofErr);
    if (openFiles.isEmpty()) openFiles = Collectors::collectOpenFiles(qgetenv("USER"), &lsofErr);

    const int hours = spinHours_->value();
    QStringList recentRoot = Collectors::collectRecentFiles("/", hours);
    QStringList allRoot = Collectors::collectAllFilesUnderRoot("/");

    QSet<QString> openSet; for (const auto &p : openFiles) openSet.insert(QFileInfo(p).absoluteFilePath());

    QHash<QString,int> execFreq;
    { const QString history = QDir::homePath() + "/.bash_history";
      QFile f(history);
      if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
          QTextStream ts(&f);
          while (!ts.atEnd()) {
              const QString line = ts.readLine().trimmed();
              if (line.isEmpty()) continue;
              const QString cmd = line.split(QRegularExpression("\\s+")).value(0);
              const QString exe = runWhich(cmd);
              if (!exe.isEmpty()) execFreq[exe] += 1;
          }
          f.close();
      } }

    QSet<QString> seen; QStringList master;
    auto appendUnique = [&](const QStringList &lst){
        for (const auto &p : lst) {
            const QString abs = QFileInfo(p).absoluteFilePath();
            if (!seen.contains(abs)) { seen.insert(abs); master << abs; }
        }
    };
    appendUnique(openFiles);
    appendUnique(recentRoot);
    appendUnique(allRoot);

    if (!lsofErr.isEmpty()) output_->append(QString("Hinweis (lsof): %1").arg(lsofErr));

    const qint64 nowSecs = QDateTime::currentSecsSinceEpoch();
    QVector<QPair<double, QString>> scored; scored.reserve(master.size());
    for (const auto &p : master) { double s = scoreFileAggressive(p, openSet, execFreq, nowSecs); scored.append({s, p}); }

    std::sort(scored.begin(), scored.end(), [](const QPair<double,QString> &a, const QPair<double,QString> &b){
        if (a.first == b.first) return a.second < b.second; return a.first > b.first;
    });

    QStringList ordered; ordered.reserve(scored.size());
    for (const auto &kv : scored) ordered << kv.second;

    const int showTop = 20;
    output_->append(QString("Aggressive Priorität — Top %1 Dateien:").arg(qMin(showTop, ordered.size())));
    for (int i = 0; i < qMin(showTop, ordered.size()); ++i)
        output_->append(QString("%1: %2").arg(i+1).arg(ordered.at(i)));

    startScanWithAlarm(ordered);
}

void MainWindow::scanOpenFiles() {
    QString err; auto paths = Collectors::collectOpenFiles(qgetenv("USER"), &err);
    if (!err.isEmpty()) { QMessageBox::warning(this, "Fehler", err); return; }
    startScanWithAlarm(paths);
}

void MainWindow::scanFrequentExecs() {
    auto history = QDir::homePath() + "/.bash_history";
    auto paths = Collectors::collectFrequentExecutables(history, spinTopN_->value());
    if (paths.isEmpty()) output_->append("Keine häufigen Executables ermittelt. Prüfe ~/.bash_history.");
    startScanWithAlarm(paths);
}

void MainWindow::scanCustom() {
    QFileDialog dlg(this, "Datei oder Ordner wählen");
    dlg.setFileMode(QFileDialog::ExistingFiles);
    dlg.setOption(QFileDialog::DontUseNativeDialog, true);
    if (dlg.exec()) {
        auto selected = dlg.selectedFiles();
        auto expanded = expandSelection(selected);
        startScanWithAlarm(expanded.isEmpty() ? selected : expanded);
    }
}

void MainWindow::onWorkerProgressFile(int percent100000, const QString &currentFile) {
    progress_->setValue(percent100000);
    const double pct = percent100000 / 1000.0;
    const QString pctText = QString::number(pct, 'f', 3) + "%";
    const QString fileName = QFileInfo(currentFile).fileName();
    const QString tip = QString("ClamAV Quick Scan — %1 (%2)").arg(pctText, fileName.isEmpty() ? currentFile : fileName);
    tray_->setIcon(makeProgressIcon(int(pct)));
    tray_->setToolTip(tip);

    const bool percentJump = (lastNotifyPercent_ < 0) || (percent100000 - lastNotifyPercent_ >= 5000);
    const bool timeOk = (notifyTimer_.elapsed() >= 1000);
    if (percentJump && timeOk) {
        notify(QString("Fortschritt: %1").arg(pctText), fileName.isEmpty() ? currentFile : fileName);
        lastNotifyPercent_ = percent100000;
        notifyTimer_.restart();
    }
}

void MainWindow::onWorkerMessage(const QString &msg) {
    output_->append(msg);
    if (msg.startsWith("Scanne: ")) {
        const QString path = msg.mid(QString("Scanne: ").size());
        const QString name = QFileInfo(path).fileName();
        const QString shortName = name.isEmpty() ? path : name;
        status_->setText(QString("Scanne: %1").arg(shortName));
        tray_->setToolTip(QString("Scanne: %1").arg(shortName));
        notify("Scan gestartet", shortName, 1500);
    } else {
        status_->setText(msg);
    }
}

void MainWindow::onWorkerFinished(const QStringList &infected, const QStringList &cleaned, const QStringList &errors) {
    status_->setText("Fertig."); btnStop_->setEnabled(false); actStop_->setEnabled(false);
    tray_->setIcon(baseIcon_); tray_->setToolTip("ClamAV Quick Scan — bereit"); lastNotifyPercent_ = -1;

    if (!infected.isEmpty()) { output_->append("\nFunde:"); for (const auto &line : infected) output_->append(line); }
    else { output_->append("\nKeine Infektionen gefunden (oder clamscan meldete keine)."); }

    if (!cleaned.isEmpty()) { output_->append("\nBereinigt:"); for (const auto &line : cleaned) output_->append(line); }
    if (!errors.isEmpty()) { output_->append("\nFehler/Hinweise:"); for (const auto &e : errors) output_->append(e); }

    notify("Scan abgeschlossen", infected.isEmpty() ? "Keine Funde." : QString("%1 Fund(e) erkannt").arg(infected.size()));

    if (!infected.isEmpty()) {
        AlarmDialog dlg(infected, chkAutoCleanGlobal_->isChecked(), this);
        if (dlg.exec() == QDialog::Accepted && dlg.autoClean()) {
            const auto ret = QMessageBox::warning(
                this, "Bestätigung",
                "Automatische Bereinigung löscht/entfernt infizierte Dateien.\nFortfahren?",
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No
            );
            if (ret == QMessageBox::Yes) {
                autoCleanPathsFromFindings(infected);
            }
        }
    }
}

void MainWindow::stopScan() {
    if (worker_) QMetaObject::invokeMethod(worker_, "stop", Qt::QueuedConnection);
    btnStop_->setEnabled(false); actStop_->setEnabled(false);
    tray_->setIcon(baseIcon_); tray_->setToolTip("ClamAV Quick Scan — gestoppt");
    lastNotifyPercent_ = -1;
    notify("Scan gestoppt", "Vorgang wurde abgebrochen.");
}

void MainWindow::startScanWithAlarm(const QStringList &paths) {
    QStringList uniq = uniqueAbs(paths);
    if (uniq.isEmpty()) { QMessageBox::information(this, "Hinweis", "Keine zu scannenden Dateien gefunden."); return; }
    output_->append(QString("Starte Scan (%1 Elemente)…").arg(uniq.size()));
    progress_->setValue(0); status_->setText("Scan läuft…"); btnStop_->setEnabled(true); actStop_->setEnabled(true);

    stopScan();

    const bool autoClean = chkAutoCleanGlobal_->isChecked();
    worker_ = new ScanWorker(uniq, autoClean);
    worker_->moveToThread(&workerThread_);

    connect(&workerThread_, &QThread::started, worker_, &ScanWorker::run);
    connect(worker_, &ScanWorker::message,          this, &MainWindow::onWorkerMessage);
    connect(worker_, &ScanWorker::finished,         this, &MainWindow::onWorkerFinished);
    connect(worker_, &ScanWorker::progressWithFile, this, &MainWindow::onWorkerProgressFile);

    connect(worker_, &ScanWorker::finished, &workerThread_, &QThread::quit);
    connect(&workerThread_, &QThread::finished, worker_, &QObject::deleteLater);

    workerThread_.start();
}

void MainWindow::autoCleanPathsFromFindings(const QStringList &findings) {
    QStringList targets;
    for (const auto &line : findings) {
        const int colon = line.indexOf(':');
        if (colon > 0) {
            QString path = line.left(colon).trimmed();
            if (QFileInfo(path).isFile()) targets << path;
        }
    }
    targets = uniqueAbs(targets);
    if (targets.isEmpty()) { QMessageBox::information(this, "Bereinigung", "Keine bereinigbaren Zielpfade erkannt."); return; }

    output_->append(QString("\nStarte Bereinigung (%1 Dateien)…").arg(targets.size()));
    QStringList cleaned, errors;

    for (const auto &p : targets) {
        QStringList args; args << "--fdpass" << "--remove" << "--no-summary" << p;
        QProcess proc; startClamdscanThrottled(proc, args, "5%"); proc.waitForFinished(-1);
        const QString out = proc.readAllStandardOutput(); const QString err = proc.readAllStandardError(); const int rc = proc.exitCode();
        if (!out.trimmed().isEmpty()) cleaned << out.trimmed();
        if (rc != 0 && rc != 1 && !err.trimmed().isEmpty()) errors << QString("%1: %2").arg(p, err.trimmed());
    }

    if (!cleaned.isEmpty()) { output_->append("\nBereinigungsergebnisse:"); for (const auto &line : cleaned) output_->append(line); }
    if (!errors.isEmpty())  { output_->append("\nBereinigungsfehler:");   for (const auto &e : errors)  output_->append(e); }

    QMessageBox::information(this, "Bereinigung abgeschlossen", QString("Bereinigung abgeschlossen. %1 Einträge.").arg(cleaned.size()));
}

QStringList MainWindow::expandSelection(const QStringList &paths) {
    QStringList out;
    for (const auto &p : paths) {
        QFileInfo fi(p);
        if (fi.isDir()) {
            QDirIterator it(fi.absoluteFilePath(), QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext()) out << it.next();
        } else if (fi.isFile()) {
            out << fi.absoluteFilePath();
        }
    }
    return uniqueAbs(out);
}

QIcon MainWindow::makeProgressIcon(int percent) {
    QPixmap pm = baseIcon_.pixmap(64, 64);
    if (pm.isNull()) { pm = QPixmap(64,64); pm.fill(Qt::transparent); }
    QPainter p(&pm); p.setRenderHint(QPainter::Antialiasing, true);
    QRect badgeRect(pm.width()-28, pm.height()-28, 26, 26);
    p.setBrush(QColor(20, 20, 20, 200)); p.setPen(Qt::NoPen);
    p.drawEllipse(badgeRect.center(), badgeRect.width()/2, badgeRect.height()/2);
    p.setPen(Qt::white);
    QFont f = p.font(); f.setBold(true); f.setPointSize(9); p.setFont(f);
    QString text = QString::number(std::max(0, std::min(100, percent))) + "%";
    p.drawText(badgeRect, Qt::AlignCenter, text);
    p.end();
    return QIcon(pm);
}

void MainWindow::notify(const QString &title, const QString &body, int iconTimeoutMs) {
    if (tray_ && QSystemTrayIcon::supportsMessages()) {
        tray_->showMessage(title, body, QSystemTrayIcon::Information, iconTimeoutMs);
    }
}

// ---------------- main ----------------
int main(int argc, char *argv[]) {
    // Immer "daemonisiert" starten: kein Fork, aber Fenster verstecken und Tray nutzen.
    QApplication app(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);

    // Schreibe PID-Datei (wichtig für externe Verwaltung)
    writePidFile(DEFAULT_PIDFILE);

    // Vorhandene Instanzen beenden (eigene PID wird geschützt)
    const QString targetInstanceName = "ClamAVQuickScan";
    qDebug() << "Versuche, Prozesse mit Name" << targetInstanceName << "zu beenden (eigene PID wird geschont)...";
    killProcessesByExactName(targetInstanceName, 4000);

    // Single instance via QLocalServer und PID-Datei
    const QString pidFile = DEFAULT_PIDFILE;
    const QString localServerName = "ClamAVQuickScan_Instance";
    QScopedPointer<QLocalServer> instanceServer;
    if (!ensureSingleInstance(localServerName, instanceServer)) {
        QStringList restartLog;
        restartLog << "Es scheint bereits eine Instanz zu laufen; versuche sie zu beenden...";
        bool stopped = stopExistingInstanceAndWait(pidFile, localServerName, &restartLog);
        for (const auto &l : restartLog) qDebug() << l;
        if (!stopped) {
            qDebug() << "Konnte laufende Instanz nicht beenden. Beende Start.";
            return 0;
        }
        QThread::msleep(500);
        if (!ensureSingleInstance(localServerName, instanceServer)) {
            qDebug() << "Nach Beenden der alten Instanz konnte keine neue SingleInstance erstellt werden.";
            return 0;
        }
    }

    // clamd Quota (best-effort)
    bool quotaApplied = ensureClamdCpuQuota(8000, "5%");
    if (quotaApplied) qDebug() << "clamd CPU quota set to 5% (best-effort via systemctl).";
    else qDebug() << "Could not set clamd CPU quota via systemctl; falling back to throttled clamdscan.";

    // clamd Socket finden
    QPair<QString, QString> sock = findResponsiveClamdSocket();
    QString socketPath = sock.first;
    QString clamdVersion = sock.second;
    if (socketPath.isEmpty()) qDebug() << "No clamd socket found or clamd not reachable.";
    else qDebug() << "Clamd Socket:" << socketPath << "Version:" << clamdVersion;

    // Autostart sicherstellen
    const QString execPath = QFileInfo(QCoreApplication::applicationFilePath()).absoluteFilePath();
    ensureXfceAutostartEntry("ClamAVQuickScan", execPath);

    // GUI-Modus aber versteckt: Tray + Notifications aktiv
    MainWindow w(socketPath, clamdVersion);
    // Fenster bleibt versteckt; Tray ist aktiv
    return app.exec();
}

#include "antivir.moc"

