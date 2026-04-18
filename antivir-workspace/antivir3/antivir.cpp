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

// Hintergrund/Daemon
#include <csignal>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
    // Strg+C / HUP ignorieren
    std::signal(SIGINT, SIG_IGN);
    std::signal(SIGHUP, SIG_IGN);

    // Erster Fork
    pid_t pid = fork();
    if (pid < 0) return;
    if (pid > 0) _exit(0); // Elternprozess beenden

    // Neue Session
    if (setsid() < 0) return;

    // Zweiter Fork (kein Session-Leader)
    pid = fork();
    if (pid < 0) return;
    if (pid > 0) _exit(0);

    umask(027);
    chdir("/");

    // TTY-FDs schließen
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // stdout/stderr in Logdatei
    int fd = ::open(logFile.toUtf8().constData(), O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd >= 0) {
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        // close(fd); // optional
    }

    // PID-Datei
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
}

// ---------- Alarmdialog ----------
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

            // Fortschritt als Prozent mit 3 Nachkommastellen skaliert: 0..100000 (0..100.000%)
            const int percent100000 = int(((i + 1) * 100000.0) / total);
            emit progressWithFile(percent100000, p);
        }
        emit finished(infected, cleaned, errors);
    }

    void stop() { stop_.store(true); }

signals:
    void progressWithFile(int percent100000, const QString &currentFile);
    void message(const QString &msg);
    void finished(const QStringList &infected, const QStringList &cleaned, const QStringList &errors);

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
    MainWindow() : worker_(nullptr), tray_(nullptr) {
        setWindowTitle("ClamAV Quick Scan");
        resize(950, 650);

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
        chkHomeOnly_->setChecked(true);

        chkAutoCleanGlobal_ = new QCheckBox("Automatisch bereinigen (vorsichtig verwenden)", this);

        form->addRow("Top-N häufig ausgeführte Executables:", spinTopN_);
        form->addRow("Zeitfenster zuletzt geändert (Stunden):", spinHours_);
        form->addRow(chkClamd_);
        form->addRow(chkHomeOnly_);
        form->addRow(chkAutoCleanGlobal_);
        cfg->setLayout(form);

        // Buttons
        auto *btnRow = new QHBoxLayout();
        btnOpen_   = new QPushButton("Geöffnete Dateien scannen", this);
        btnFreq_   = new QPushButton("Häufig ausgeführte Executables scannen", this);
        btnRecent_ = new QPushButton("Zuletzt geänderte Dateien scannen", this);
        btnCustom_ = new QPushButton("Eigenen Pfad/Ordner scannen…", this);
        btnStop_   = new QPushButton("Abbrechen", this);
        btnStop_->setEnabled(false);

        btnRow->addWidget(btnOpen_);
        btnRow->addWidget(btnFreq_);
        btnRow->addWidget(btnRecent_);
        btnRow->addWidget(btnCustom_);
        btnRow->addWidget(btnStop_);

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

        // Verbindungen
        connect(btnOpen_,   &QPushButton::clicked, this, &MainWindow::scanOpenFiles);
        connect(btnFreq_,   &QPushButton::clicked, this, &MainWindow::scanFrequentExecs);
        connect(btnRecent_, &QPushButton::clicked, this, &MainWindow::scanRecentFiles);
        connect(btnCustom_, &QPushButton::clicked, this, &MainWindow::scanCustom);
        connect(btnStop_,   &QPushButton::clicked, this, &MainWindow::stopScan);

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

        // Fenster „close to tray“
        this->hide();
        notify("Automatischer Scan", "Zuletzt geänderte Dateien werden geprüft …");
        QTimer::singleShot(500, this, &MainWindow::scanRecentFiles); // Priorität: zuletzt geänderte Dateien
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
    void scanOpenFiles() {
        QString err;
        auto paths = Collectors::collectOpenFiles(qgetenv("USER"), &err);
        if (!err.isEmpty()) {
            QMessageBox::warning(this, "Fehler", err);
            return;
        }
        startScanWithAlarm(paths);
    }

    void scanFrequentExecs() {
        auto history = QDir::homePath() + "/.bash_history";
        auto paths = Collectors::collectFrequentExecutables(history, spinTopN_->value());
        if (paths.isEmpty()) output_->append("Keine häufigen Executables ermittelt. Prüfe ~/.bash_history.");
        startScanWithAlarm(paths);
    }

    void scanRecentFiles() {
        QString root = chkHomeOnly_->isChecked() ? QDir::homePath() : "/";
        auto paths = Collectors::collectRecentFiles(root, spinHours_->value());
        startScanWithAlarm(paths);
    }

    void scanCustom() {
        QFileDialog dlg(this, "Datei oder Ordner wählen");
        dlg.setFileMode(QFileDialog::ExistingFiles);
        dlg.setOption(QFileDialog::DontUseNativeDialog, true);
        if (dlg.exec()) {
            auto selected = dlg.selectedFiles();
            auto expanded = expandSelection(selected);
            startScanWithAlarm(expanded.isEmpty() ? selected : expanded);
        }
    }

    // Fortschritt mit Dateiname und 0..100000 Anzeige
    void onWorkerProgressFile(int percent100000, const QString &currentFile) {
        progress_->setValue(percent100000);

        const double pct = percent100000 / 1000.0;
        const QString pctText = QString::number(pct, 'f', 3) + "%";
        const QString fileName = QFileInfo(currentFile).fileName();
        const QString tip = QString("ClamAV Quick Scan — %1 (%2)").arg(pctText, fileName.isEmpty() ? currentFile : fileName);

        tray_->setIcon(makeProgressIcon(int(pct))); // Overlay nutzt Ganzzahl-Prozent
        tray_->setToolTip(tip);

        const bool percentJump = (lastNotifyPercent_ < 0) || (percent100000 - lastNotifyPercent_ >= 5000); // +5%
        const bool timeOk = (notifyTimer_.elapsed() >= 1000);
        if (percentJump && timeOk) {
            notify(QString("Fortschritt: %1").arg(pctText), fileName.isEmpty() ? currentFile : fileName);
            lastNotifyPercent_ = percent100000;
            notifyTimer_.restart();
        }
    }

    void onWorkerMessage(const QString &msg) {
        output_->append(msg);
        if (msg.startsWith("Scanne: ")) {
            const QString path = msg.mid(QString("Scanne: ").size());
            const QString name = QFileInfo(path).fileName();
            notify("Scan gestartet", name.isEmpty() ? path : name);
        }
    }

    void onWorkerFinished(const QStringList &infected, const QStringList &cleaned, const QStringList &errors) {
        status_->setText("Fertig.");
        btnStop_->setEnabled(false);
        actStop_->setEnabled(false);
        tray_->setIcon(baseIcon_);
        tray_->setToolTip("ClamAV Quick Scan — bereit");
        lastNotifyPercent_ = -1;

        if (!infected.isEmpty()) {
            output_->append("\nFunde:");
            for (const auto &line : infected) output_->append(line);
        } else {
            output_->append("\nKeine Infektionen gefunden (oder clamscan meldete keine).");
        }

        if (!cleaned.isEmpty()) {
            output_->append("\nBereinigt:");
            for (const auto &line : cleaned) output_->append(line);
        }

        if (!errors.isEmpty()) {
            output_->append("\nFehler/Hinweise:");
            for (const auto &e : errors) output_->append(e);
        }

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

    void stopScan() {
        if (worker_) QMetaObject::invokeMethod(worker_, "stop", Qt::QueuedConnection);
        btnStop_->setEnabled(false);
        actStop_->setEnabled(false);
        tray_->setIcon(baseIcon_);
        tray_->setToolTip("ClamAV Quick Scan — gestoppt");
        lastNotifyPercent_ = -1;
        notify("Scan gestoppt", "Vorgang wurde abgebrochen.");
    }

private:
    void startScanWithAlarm(const QStringList &paths) {
        QStringList uniq = uniqueAbs(paths);
        if (uniq.isEmpty()) {
            QMessageBox::information(this, "Hinweis", "Keine zu scannenden Dateien gefunden.");
            return;
        }
        output_->append(QString("Starte Scan (%1 Elemente)…").arg(uniq.size()));
        progress_->setValue(0);
        status_->setText("Scan läuft…");
        btnStop_->setEnabled(true);
        actStop_->setEnabled(true);

        stopScan();

        const bool useClamd = chkClamd_->isChecked();
        const bool autoClean = false; // Erst nach Alarmdialog
        worker_ = new ScanWorker(uniq, useClamd, autoClean);
        worker_->moveToThread(&workerThread_);

        connect(&workerThread_, &QThread::started, worker_, &ScanWorker::run);
        connect(worker_, &ScanWorker::message,          this, &MainWindow::onWorkerMessage);
        connect(worker_, &ScanWorker::finished,         this, &MainWindow::onWorkerFinished);
        connect(worker_, &ScanWorker::progressWithFile, this, &MainWindow::onWorkerProgressFile);

        connect(worker_, &ScanWorker::finished, &workerThread_, &QThread::quit);
        connect(&workerThread_, &QThread::finished, worker_, &QObject::deleteLater);

        workerThread_.start();
    }

    void autoCleanPathsFromFindings(const QStringList &findings) {
        QStringList targets;
        for (const auto &line : findings) {
            const int colon = line.indexOf(':');
            if (colon > 0) {
                QString path = line.left(colon).trimmed();
                if (QFileInfo(path).isFile()) targets << path;
            }
        }
        targets = uniqueAbs(targets);
        if (targets.isEmpty()) {
            QMessageBox::information(this, "Bereinigung", "Keine bereinigbaren Zielpfade erkannt.");
            return;
        }

        output_->append(QString("\nStarte Bereinigung (%1 Dateien)…").arg(targets.size()));
        const bool useClamd = chkClamd_->isChecked();
        QStringList cleaned, errors;

        for (const auto &p : targets) {
            const QString program = useClamd ? "clamdscan" : "clamscan";
            QStringList args;
            if (useClamd) args << "--fdpass" << "--remove" << "--no-summary" << p;
            else          args << "--remove" << "--no-summary" << p;

            QProcess proc;
            proc.start(program, args);
            proc.waitForFinished(-1);
            const QString out = proc.readAllStandardOutput();
            const QString err = proc.readAllStandardError();
            const int rc = proc.exitCode();

            if (!out.trimmed().isEmpty()) cleaned << out.trimmed();
            if (rc != 0 && rc != 1 && !err.trimmed().isEmpty()) errors << QString("%1: %2").arg(p, err.trimmed());
        }

        if (!cleaned.isEmpty()) {
            output_->append("\nBereinigungsergebnisse:");
            for (const auto &line : cleaned) output_->append(line);
        }
        if (!errors.isEmpty()) {
            output_->append("\nBereinigungsfehler:");
            for (const auto &e : errors) output_->append(e);
        }

        QMessageBox::information(this, "Bereinigung abgeschlossen",
                                 QString("Bereinigung abgeschlossen. %1 Einträge.")
                                 .arg(cleaned.size()));
    }

    QStringList expandSelection(const QStringList &paths) {
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

    QIcon makeProgressIcon(int percent) {
        QPixmap pm = baseIcon_.pixmap(64, 64);
        if (pm.isNull()) { pm = QPixmap(64,64); pm.fill(Qt::transparent); }
        QPainter p(&pm);
        p.setRenderHint(QPainter::Antialiasing, true);

        QRect badgeRect(pm.width()-28, pm.height()-28, 26, 26);
        p.setBrush(QColor(20, 20, 20, 200));
        p.setPen(Qt::NoPen);
        p.drawEllipse(badgeRect.center(), badgeRect.width()/2, badgeRect.height()/2);

        p.setPen(Qt::white);
        QFont f = p.font(); f.setBold(true); f.setPointSize(9);
        p.setFont(f);
        QString text = QString::number(std::max(0, std::min(100, percent))) + "%";
        p.drawText(badgeRect, Qt::AlignCenter, text);
        p.end();

        return QIcon(pm);
    }

    void notify(const QString &title, const QString &body, int iconTimeoutMs = 3000) {
        if (tray_ && QSystemTrayIcon::supportsMessages()) {
            tray_->showMessage(title, body, QSystemTrayIcon::Information, iconTimeoutMs);
        }
    }

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
    int lastNotifyPercent_ = -1;      // skaliert: 0..100000
    QElapsedTimer notifyTimer_;
};

// ---------- main ----------
int main(int argc, char *argv[]) {
    // Sofort Hintergrund: keine Reaktion auf Strg+C im Terminal
    backgroundize();

    QApplication app(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false); // läuft ohne Fenster weiter

    // Einzelinstanz
    QScopedPointer<QLocalServer> instanceServer;
    if (!ensureSingleInstance("ClamAVQuickScan_Instance", instanceServer)) {
        return 0;
    }

    // Autostart (XFCE) sicherstellen
    const QString execPath = QFileInfo(QCoreApplication::applicationFilePath()).absoluteFilePath();
    ensureXfceAutostartEntry("ClamAVQuickScan", execPath);

    MainWindow w;
    // Absichtlich kein w.show() — Tray & Auto-Scan übernehmen
    return app.exec();
}

#include "antivir.moc"
