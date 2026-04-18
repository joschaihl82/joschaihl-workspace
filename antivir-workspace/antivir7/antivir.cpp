#include <QApplication>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QAction>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QProgressBar>
#include <QThread>
#include <QDirIterator>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QCloseEvent>
#include <QMessageBox>
#include <QDateTime>
#include <cmath>

// ClamAV-Header
#include <clamav.h>

// --- Globale ClamAV-Ressourcen ---
// In einer echten Anwendung würde man diese in einer Manager-Klasse kapseln.
struct cl_engine *g_engine = nullptr;
struct cl_scan_options g_scan_options;

// --- Hilfsfunktionen für die Dateibewertung ---

double calculateEntropy(const QByteArray &data) {
    if (data.isEmpty()) return 0.0;

    QMap<char, int> frequencies;
    for (char byte : data) {
        frequencies[byte]++;
    }

    double entropy = 0.0;
    double data_size = data.size();
    for (int count : frequencies) {
        double probability = static_cast<double>(count) / data_size;
        entropy -= probability * std::log2(probability);
    }
    return entropy;
}

double scoreFileAggressive(const QFileInfo &fileInfo) {
    double score = 0.0;
    const QString path = fileInfo.absoluteFilePath();
    const QString suffix = fileInfo.suffix().toLower();

    // 1. Ausführbare Dateien (höchstes Risiko)
    if (fileInfo.isExecutable()) {
        score += 50.0;
    }

    // 2. Heuristik für Dateitypen
    if (suffix == "exe" || suffix == "dll" || suffix == "bat" || suffix == "cmd" || suffix == "scr" || suffix == "vbs" || suffix == "js") {
        score += 40.0;
    } else if (suffix == "zip" || suffix == "rar" || suffix == "7z" || suffix == "tar" || suffix == "gz") {
        score += 25.0; // Archive können Schadsoftware enthalten
    } else if (suffix == "doc" || suffix == "docx" || suffix == "xls" || suffix == "xlsx" || suffix == "pdf") {
        score += 15.0; // Dokumente mit Makros
    }

    // 3. Dateigröße (sehr kleine oder sehr große sind verdächtig)
    qint64 size = fileInfo.size();
    if (size > 0 && size < 1024) { // < 1KB
        score += 10.0;
    } else if (size > 100 * 1024 * 1024) { // > 100MB
        score += 5.0;
    }

    // 4. Dateialter (neue Dateien sind verdächtiger)
    qint64 daysOld = fileInfo.birthTime().daysTo(QDateTime::currentDateTime());
    if (daysOld < 7) { // Jünger als 7 Tage
        score += 20.0;
    } else if (daysOld < 30) { // Jünger als 30 Tage
        score += 10.0;
    }

    // 5. Entropie (verschlüsselte/packte Dateien haben hohe Entropie)
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.read(4096); // Nur die ersten 4KB für Performance
        if (!data.isEmpty()) {
            double entropy = calculateEntropy(data);
            if (entropy > 7.5) {
                score += 25.0;
            } else if (entropy > 7.0) {
                score += 15.0;
            }
        }
        file.close();
    }
    
    return score;
}


// --- ScanWorker (im selben Header deklariert) ---

class ScanWorker : public QObject {
    Q_OBJECT

public:
    explicit ScanWorker(const QString &rootPath, QObject *parent = nullptr);
    ~ScanWorker();

public slots:
    void run();
    void stop();

signals:
    void finished(int threatsFound);
    void progress(const QString &status, double progress);
    void message(const QString &message);

private:
    QString rootPath;
    bool stopRequested;
    QStringList filesToScan;
};

ScanWorker::ScanWorker(const QString &rootPath, QObject *parent)
    : QObject(parent), rootPath(rootPath), stopRequested(false) {}

ScanWorker::~ScanWorker() {}

void ScanWorker::stop() {
    stopRequested = true;
}

void ScanWorker::run() {
    emit message("Indizierung und Bewertung von Dateien gestartet...");
    emit progress("Indizierung", 0.0);

    // --- Schritt 1: Indizierung und Scoring ---
    QList<QPair<double, QString>> scoredFiles;
    qint64 totalFiles = 0;
    qint64 processedFiles = 0;

    // Zuerst die Gesamtzahl der Dateien ermitteln (für präzisen Fortschritt)
    QDirIterator it_count(rootPath, QDir::Files | QDir::Hidden | QDir::System, QDirIterator::Subdirectories);
    while (it_count.hasNext()) {
        it_count.next();
        totalFiles++;
    }

    // Jetzt durchgehen und bewerten
    QDirIterator it_scan(rootPath, QDir::Files | QDir::Hidden | QDir::System, QDirIterator::Subdirectories);
    while (it_scan.hasNext() && !stopRequested) {
        it_scan.next();
        QFileInfo fileInfo = it_scan.fileInfo();
        
        double score = scoreFileAggressive(fileInfo);
        scoredFiles.append(qMakePair(score, fileInfo.absoluteFilePath()));
        
        processedFiles++;
        double progressPercent = (static_cast<double>(processedFiles) / totalFiles) * 100.0;
        emit progress(QString("Indizierung: %1%").arg(progressPercent, 0, 'f', 2), progressPercent);
    }

    if (stopRequested) {
        emit message("Indizierung vom Benutzer abgebrochen.");
        emit finished(-1); // -1 signalisiert Abbruch
        return;
    }

    // --- Schritt 2: Sortieren nach Risiko ---
    emit message(QString("%1 Dateien indiziert. Sortiere nach Risiko...").arg(scoredFiles.size()));
    std::sort(scoredFiles.begin(), scoredFiles.end(), [](const QPair<double, QString> &a, const QPair<double, QString> &b) {
        return a.first > b.first; // Absteigend sortieren (höchster Score zuerst)
    });
    
    for (auto &file : scoredFiles) {
        filesToScan.append(file.second);
    }
    emit message("Sortierung abgeschlossen. Beginne Scan...");

    // --- Schritt 3: Scan mit libclamav ---
    int threatsFound = 0;
    int scannedCount = 0;
    int totalToScan = filesToScan.size();

    for (const QString &filePath : filesToScan) {
        if (stopRequested) {
            emit message("Scan vom Benutzer abgebrochen.");
            emit finished(-1);
            return;
        }

        const char *virname = nullptr;
        unsigned long int scanned_size = 0;
        
        // cl_scanfile ist thread-sicher
        int result = cl_scanfile(filePath.toUtf8().constData(), &virname, &scanned_size, g_engine, &g_scan_options);

        scannedCount++;
        double progressPercent = (static_cast<double>(scannedCount) / totalToScan) * 100.0;
        emit progress(QString("Scannen: %1%").arg(progressPercent, 0, 'f', 2), progressPercent);

        if (result == CL_VIRUS) {
            threatsFound++;
            QString msg = QString("FUND! [%1] in Datei: %2").arg(virname).arg(filePath);
            emit message(msg);
        } else if (result != CL_CLEAN) {
            emit message(QString("Fehler beim Scannen von %1: %2").arg(filePath).arg(cl_strerror(result)));
        }
    }

    emit message(QString("Scan abgeschlossen. %1 Fund(e).").arg(threatsFound));
    emit finished(threatsFound);
}


// --- MainWindow ---

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void startScan();
    void stopScan();
    void onScanFinished(int threatsFound);
    void onScanProgress(const QString &status, double progress);
    void onScanMessage(const QString &message);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void createTrayIcon();
    void createActions();

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QAction *startAction;
    QAction *stopAction;
    QAction *quitAction;

// --- MainWindow Implementierung (Fortsetzung) ---

    // UI-Elemente für das Debug-Fenster
    QWidget *centralWidget;
    QTextEdit *logTextEdit;
    QPushButton *startButton;
    QPushButton *stopButton;
    QProgressBar *progressBar;

    QThread *scanThread;
    ScanWorker *scanWorker;
    bool isScanning;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), isScanning(false), scanThread(nullptr), scanWorker(nullptr) {
    
    setWindowTitle("ClamAV Tray Scanner");
    setMinimumSize(500, 300);

    createActions();
    createTrayIcon();

    // UI für das Hauptfenster (zum Debuggen)
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    auto *layout = new QVBoxLayout(centralWidget);

    logTextEdit = new QTextEdit(this);
    logTextEdit->setReadOnly(true);
    logTextEdit->append("Bereit. Klicken Sie auf 'Scan starten' oder das Tray-Icon.");

    startButton = new QPushButton("Scan starten", this);
    stopButton = new QPushButton("Scan stoppen", this);
    stopButton->setEnabled(false);

    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 10000); // Für 2 Nachkommastellen

    layout->addWidget(logTextEdit);
    layout->addWidget(progressBar);
    layout->addWidget(startButton);
    layout->addWidget(stopButton);

    connect(startButton, &QPushButton::clicked, this, &MainWindow::startScan);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopScan);
}

MainWindow::~MainWindow() {
    if (isScanning) {
        stopScan();
    }
    if (scanThread && scanThread->isRunning()) {
        scanThread->quit();
        scanThread->wait();
    }
    delete scanWorker;
    delete scanThread;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // Wenn das Fenster geschlossen wird, ins Tray minimieren, nicht beenden
    if (trayIcon->isVisible()) {
        hide();
        event->ignore();
    }
}

void MainWindow::createActions() {
    startAction = new QAction(tr("Scan starten"), this);
    connect(startAction, &QAction::triggered, this, &MainWindow::startScan);

    stopAction = new QAction(tr("Scan stoppen"), this);
    stopAction->setEnabled(false);
    connect(stopAction, &QAction::triggered, this, &MainWindow::stopScan);

    quitAction = new QAction(tr("Beenden"), this);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
}

void MainWindow::createTrayIcon() {
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(startAction);
    trayIconMenu->addAction(stopAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    // Ersetzen Sie ":/icons/clamav-icon.png" durch den Pfad zu Ihrem Icon
    // Für dieses Beispiel verwenden wir ein Standard-Qt-Icon, falls kein Ressourcensystem eingerichtet ist.
    if (!QFile::exists(":/icons/clamav-icon.png")) {
        trayIcon->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    } else {
        trayIcon->setIcon(QIcon(":/icons/clamav-icon.png"));
    }
    
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setToolTip("Bereit");

    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
    
    trayIcon->show();
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
        setVisible(!isVisible());
    }
}

void MainWindow::startScan() {
    if (isScanning) return;

    isScanning = true;
    logTextEdit->clear();
    progressBar->setValue(0);

    startAction->setEnabled(false);
    stopAction->setEnabled(true);
    startButton->setEnabled(false);
    stopButton->setEnabled(true);

    scanThread = new QThread(this);
    scanWorker = new ScanWorker("/"); // Wurzelverzeichnis scannen
    scanWorker->moveToThread(scanThread);

    connect(scanWorker, &ScanWorker::finished, this, &MainWindow::onScanFinished);
    connect(scanWorker, &ScanWorker::progress, this, &MainWindow::onScanProgress);
    connect(scanWorker, &ScanWorker::message, this, &MainWindow::onScanMessage);
    connect(scanThread, &QThread::started, scanWorker, &ScanWorker::run);
    connect(scanWorker, &ScanWorker::finished, scanThread, &QThread::quit);
    connect(scanWorker, &ScanWorker::finished, scanWorker, &QObject::deleteLater);
    connect(scanThread, &QThread::finished, scanThread, &QObject::deleteLater);

    scanThread->start();
}

void MainWindow::stopScan() {
    if (!isScanning) return;
    
    logTextEdit->append("Stoppsignal gesendet...");
    if (scanWorker) {
        scanWorker->stop();
    }
}

void MainWindow::onScanFinished(int threatsFound) {
    isScanning = false;
    startAction->setEnabled(true);
    stopAction->setEnabled(false);
    startButton->setEnabled(true);
    stopButton->setEnabled(false);

    progressBar->setValue(10000);
    trayIcon->setToolTip("Scan abgeschlossen");

    QString title = "Scan abgeschlossen";
    QString msg;
    if (threatsFound >= 0) {
        msg = QString("Der Scan wurde erfolgreich beendet.\n%1 Fund(e).").arg(threatsFound);
    } else {
        msg = "Der Scan wurde vom Benutzer abgebrochen.";
    }
    
    logTextEdit->append(msg);
    trayIcon->showMessage(title, msg, QSystemTrayIcon::Information, 5000);
}

void MainWindow::onScanProgress(const QString &status, double progress) {
    progressBar->setValue(static_cast<int>(progress * 100));
    trayIcon->setToolTip(status);
}

void MainWindow::onScanMessage(const QString &message) {
    logTextEdit->append(message);
}

// --- Hauptfunktion ---

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, "ClamAV Tray Scanner", "Ich konnte kein System-Tray auf diesem System finden.");
        return 1;
    }

    QApplication::setQuitOnLastWindowClosed(false);

    // ClamAV initialisieren
    if (cl_init(CL_INIT_DEFAULT) != CL_SUCCESS) {
        QMessageBox::critical(nullptr, "ClamAV Fehler", "Konnte ClamAV nicht initialisieren.");
        return 1;
    }

    // Engine erstellen
    g_engine = cl_engine_new();
    if (!g_engine) {
        QMessageBox::critical(nullptr, "ClamAV Fehler", "Konnte ClamAV-Engine nicht erstellen.");
        return 1;
    }

    // Standard-Virusdatenbank laden
    const char *db_dir = cl_retdbdir();
    unsigned int sigs = 0;
    if (cl_load(db_dir, g_engine, &sigs, CL_DB_STDOPT) != CL_SUCCESS) {
        QMessageBox::critical(nullptr, "ClamAV Fehler", "Konnte ClamAV-Datenbank nicht laden.");
        cl_engine_free(g_engine);
        return 1;
    }
    qDebug() << "ClamAV Signatures geladen:" << sigs;

    // Engine kompilieren
    if (cl_engine_compile(g_engine) != CL_SUCCESS) {
        QMessageBox::critical(nullptr, "ClamAV Fehler", "Konnte ClamAV-Engine nicht kompilieren.");
        cl_engine_free(g_engine);
        return 1;
    }

    // Scan-Options Struktur vorbereiten
    memset(&g_scan_options, 0, sizeof(struct cl_scan_options));
    g_scan_options.parse = ~0; // Alle Dateitypen scannen
    g_scan_options.general = CL_SCAN_GENERAL_ALLMATCHES | CL_SCAN_GENERAL_COLLECT_METADATA;


    MainWindow window;
    // window.show(); // Auskommentieren, um die App ohne sichtbares Hauptfenster zu starten

    int result = app.exec();

    // Aufräumen
    cl_engine_free(g_engine);

    return result;
}

#include "antivir.moc" // Wichtig für die Q_OBJECT-Metadaten in einer Single-File-Implementierung
