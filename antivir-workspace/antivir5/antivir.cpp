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

// Kombinierte Heuristik: ausführbar oder ELF-Header
static bool isBinaryFile(const QString &p) {
    if (!isRegularFile(p)) return false;
    if (isExecutableFile(p)) return true;
    // Manche Binaries könnten nicht ausführbar sein (z. B. mounted FS), prüfen wir ELF-Magic
    return isElfBinary(p);
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
            // Fortschritt lokal: Prozent dieses Workers (0..100000 skaliert)
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
        chkHomeOnly_->setChecked(true);

        chkAutoCleanGlobal_ = new QCheckBox("Automatisch bereinigen (vorsichtig verwenden)", this);

        // Neu: Intervall für Endlosschleife (Minuten)
        spinLoopMinutes_ = new QSpinBox(this);
        spinLoopMinutes_->setRange(1, 1440); // 1 Minute .. 24 Stunden
        spinLoopMinutes_->setValue(5);       // Standard: alle 5 Minuten

        // Neu: Concurrency (Anzahl paralleler Worker)
        spinConcurrency_ = new QSpinBox(this);
        spinConcurrency_->setRange(1, 64);
        spinConcurrency_->setValue(std::max(1, QThread::idealThreadCount()));

        // Neu: Whitelist aktivieren + Button zum Bearbeiten
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
        connect(btnEditWhitelist_, &QPushButton::clicked, this, &MainWindow::editWhitelist);

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

        // Whitelist initial laden
        whitelistPath_ = QDir::homePath() + "/.clamav-quickscan.whitelist";
        loadWhitelist();

        // Fenster „close to tray“
        this->hide();
        notify("Automatischer Scan", "Zuletzt geänderte Dateien werden geprüft …");

        // Timer für Endlosschleife: scanRecentFiles() in konfigurierbarem Intervall
        loopTimer_ = new QTimer(this);
        connect(loopTimer_, &QTimer::timeout, this, [this]() {
            // Keine Überlappung: nur starten, wenn kein Scan läuft
            if (!scanRunning_.load()) {
                this->scanRecentFiles();
            } else {
                output_->append("Vorheriger Scan läuft noch — nächster Durchlauf übersprungen.");
            }
        });
        loopTimer_->start(spinLoopMinutes_->value() * 60 * 1000);

        // Wenn der Benutzer das Intervall ändert, Timer anpassen
        connect(spinLoopMinutes_, qOverload<int>(&QSpinBox::valueChanged), this, [this](int v){
            if (loopTimer_->isActive()) loopTimer_->start(v * 60 * 1000);
        });

        // Sofortigen ersten Durchlauf starten
        QTimer::singleShot(500, this, [this](){ this->scanRecentFiles(); });
    }

    ~MainWindow() {
        if (loopTimer_) loopTimer_->stop();
        stopScan();
        // Threads stoppen
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
        // Datei anlegen, falls nicht vorhanden
        if (!QFile::exists(whitelistPath_)) {
            QFile f(whitelistPath_);
            if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream ts(&f);
                ts << "# Eine Zeile pro Pfad, absolute Pfade empfohlen\n";
                ts << "# Beispiel: /home/josh/bin/special-binary\n";
                f.close();
            }
        }
        // Öffne mit Standard-Editor (xdg-open)
        QProcess::startDetached("xdg-open", {whitelistPath_});
        QMessageBox::information(this, "Whitelist", QString("Whitelist geöffnet: %1\nSpeichere und schließe den Editor, dann wird die Whitelist beim nächsten Scan neu geladen.").arg(whitelistPath_));
    }

    // Lokaler Worker-Fortschritt (pro Worker)
    void onWorkerProgressLocal(int percent100000_local, const QString &currentFile) {
        Q_UNUSED(percent100000_local);
        Q_UNUSED(currentFile);
        // Globaler Fortschritt wird in onSubWorkerFinished aktualisiert
    }

    void onWorkerMessage(const QString &msg) {
        output_->append(msg);
        if (msg.startsWith("Scanne: ")) {
            const QString path = msg.mid(QString("Scanne: ").size());
            const QString name = QFileInfo(path).fileName();
            notify("Scan gestartet", name.isEmpty() ? path : name);
        }
    }

    // Wenn ein Sub-Worker fertig ist, aggregieren wir Ergebnisse
    void onSubWorkerFinished(const QStringList &infected, const QStringList &cleaned, const QStringList &errors, int processedCount) {
        QMutexLocker lk(&aggMutex_);
        aggregatedInfected_ += infected;
        aggregatedCleaned_ += cleaned;
        aggregatedErrors_ += errors;
        processedCount_.fetchAndAddRelaxed(processedCount);

        // Update globaler Fortschritt
        const int total = totalFiles_.load();
        const int processed = processedCount_.load();
        const int percent100000 = total > 0 ? int((processed * 100000.0) / total) : 100000;
        progress_->setValue(percent100000);

        const double pct = percent100000 / 1000.0;
        const QString pctText = QString::number(pct, 'f', 3) + "%";
        tray_->setIcon(makeProgressIcon(int(pct)));
        tray_->setToolTip(QString("ClamAV Quick Scan — %1").arg(pctText));

        // Wenn alle Worker fertig sind, abschließen
        activeWorkers_--;
        if (activeWorkers_ <= 0) {
            // Reset Flag
            scanRunning_.store(false);
            // Status & UI
            status_->setText("Fertig.");
            btnStop_->setEnabled(false);
            actStop_->setEnabled(false);
            tray_->setIcon(baseIcon_);
            tray_->setToolTip("ClamAV Quick Scan — bereit");
            lastNotifyPercent_ = -1;

            // Ausgabe aggregierter Ergebnisse
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

            notify("Scan abgeschlossen", aggregatedInfected_.isEmpty() ? "Keine Funde." : QString("%1 Fund(e) erkannt").arg(aggregatedInfected_.size()));

            // Alarmdialog falls Funde
            if (!aggregatedInfected_.isEmpty()) {
                AlarmDialog dlg(aggregatedInfected_, chkAutoCleanGlobal_->isChecked(), this);
                if (dlg.exec() == QDialog::Accepted && dlg.autoClean()) {
                    const auto ret = QMessageBox::warning(
                        this, "Bestätigung",
                        "Automatische Bereinigung löscht/entfernt infizierte Dateien.\nFortfahren?",
                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No
                    );
                    if (ret == QMessageBox::Yes) {
                        autoCleanPathsFromFindings(aggregatedInfected_);
                    }
                }
            }

            // Aufräumen: Threads löschen
            for (auto t : workerThreads_) {
                if (t->isRunning()) {
                    t->quit();
                    t->wait();
                }
                delete t;
            }
            workerThreads_.clear();
            workers_.clear();

            // Reset aggregated lists
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
        // Stoppe alle Worker
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
    void startParallelScanWithFilters(const QStringList &paths) {
        // Filter: nur Binaries + Whitelist
        QStringList uniq = uniqueAbs(paths);
        if (uniq.isEmpty()) {
            output_->append("Keine zu scannenden Dateien gefunden.");
            return;
        }

        // Lade Whitelist (aktuell)
        if (chkUseWhitelist_->isChecked()) loadWhitelist();

        QStringList filtered;
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
            filtered << p;
        }

        output_->append(QString("Gefundene Elemente: %1; Binaries zum Scannen: %2; Whitelist übersprungen: %3; Nicht-Binaries übersprungen: %4")
                        .arg(uniq.size()).arg(filtered.size()).arg(skippedWhitelist.size()).arg(skippedNotBinary.size()));

        if (!skippedWhitelist.isEmpty()) {
            output_->append("Beispiel Whitelist-Übersprungen:");
            for (int i = 0; i < std::min(3, skippedWhitelist.size()); ++i) output_->append("  " + skippedWhitelist.at(i));
        }
        if (!skippedNotBinary.isEmpty()) {
            output_->append("Beispiel Nicht-Binaries:");
            for (int i = 0; i < std::min(3, skippedNotBinary.size()); ++i) output_->append("  " + skippedNotBinary.at(i));
        }

        startParallelScan(filtered);
    }

    void startParallelScan(const QStringList &paths) {
        QStringList uniq = uniqueAbs(paths);
        if (uniq.isEmpty()) {
            output_->append("Keine zu scannenden Dateien gefunden.");
            return;
        }

        // Stoppe evtl. laufende Scans
        if (scanRunning_.load()) {
            output_->append("Scan läuft bereits. Neuer Scan wird nicht gestartet.");
            return;
        }

        // UI vorbereiten
        output_->append(QString("Starte parallelen Scan (%1 Elemente)…").arg(uniq.size()));
        progress_->setValue(0);
        status_->setText("Scan läuft…");
        btnStop_->setEnabled(true);
        actStop_->setEnabled(true);

        // Aggregation vorbereiten
        aggregatedInfected_.clear();
        aggregatedCleaned_.clear();
        aggregatedErrors_.clear();
        processedCount_.store(0);
        totalFiles_.store(uniq.size());

        // Concurrency
        int concurrency = spinConcurrency_->value();
        concurrency = std::max(1, std::min(concurrency, uniq.size()));

        // Split in N Teile
        QVector<QStringList> chunks(concurrency);
        for (int i = 0; i < uniq.size(); ++i) {
            chunks[i % concurrency] << uniq.at(i);
        }

        // Start Worker-Threads
        activeWorkers_ = concurrency;
        scanRunning_.store(true);

        const bool useClamd = chkClamd_->isChecked();
        const bool autoClean = false;

        for (int i = 0; i < concurrency; ++i) {
            if (chunks[i].isEmpty()) {
                activeWorkers_--;
                continue;
            }
            ScanWorker *w = new ScanWorker(chunks[i], useClamd, autoClean);
            QThread *t = new QThread();
            w->moveToThread(t);

            connect(t, &QThread::started, w, &ScanWorker::run);
            connect(w, &ScanWorker::message, this, &MainWindow::onWorkerMessage);
            connect(w, &ScanWorker::progressWithFileLocal, this, &MainWindow::onWorkerProgressLocal);
            connect(w, &ScanWorker::finishedWorker, this, &MainWindow::onSubWorkerFinished);

            // Wenn Worker fertig, Thread beenden und Worker löschen (deleteLater)
            connect(w, &ScanWorker::finishedWorker, t, &QThread::quit);
            connect(t, &QThread::finished, w, &QObject::deleteLater);

            workers_.append(w);
            workerThreads_.append(t);
            t->start();
        }
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

    // Whitelist: Laden & Prüfen
    void loadWhitelist() {
        whitelist_.clear();
        QFile f(whitelistPath_);
        if (!f.exists()) return;
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
        QTextStream ts(&f);
        while (!ts.atEnd()) {
            QString line = ts.readLine().trimmed();
            if (line.isEmpty()) continue;
            if (line.startsWith('#')) continue;
            // Nur absolute Pfade akzeptieren
            QFileInfo fi(line);
            if (fi.isRelative()) continue;
            whitelist_.insert(fi.absoluteFilePath());
        }
        f.close();
    }

    bool isWhitelisted(const QString &path) const {
        if (whitelist_.contains(path)) return true;
        // zusätzlich: falls whitelist enthält ein Verzeichnis, dann alle Unterpfade überspringen
        for (const auto &w : whitelist_) {
            if (w.endsWith('/')) {
                if (path.startsWith(w)) return true;
            } else {
                // Verzeichnis-Check
                if (QFileInfo(w).isDir()) {
                    const QString dir = QFileInfo(w).absoluteFilePath();
                    if (path.startsWith(dir + "/")) return true;
                }
            }
        }
        return false;
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

    // Whitelist UI
    QCheckBox *chkUseWhitelist_;
    QPushButton *btnEditWhitelist_;
    QString whitelistPath_;
    QSet<QString> whitelist_;

    // Neu: Endlosschleifen-Timer & Intervall + Concurrency
    QSpinBox *spinLoopMinutes_;
    QSpinBox *spinConcurrency_;
    QTimer *loopTimer_;

    // Worker-Management
    QVector<QThread*> workerThreads_;
    QVector<ScanWorker*> workers_;
    QAtomicInteger<int> activeWorkers_{0};
    QAtomicInteger<bool> scanRunning_{false};

    // Aggregation
    QStringList aggregatedInfected_;
    QStringList aggregatedCleaned_;
    QStringList aggregatedErrors_;
    QMutex aggMutex_;
    QAtomicInteger<int> processedCount_{0};
    QAtomicInteger<int> totalFiles_{0};

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

