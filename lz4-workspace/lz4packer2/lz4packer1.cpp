// lz4_winzip_clone.cpp
// Qt6 single-file WinZip-like UI for .lz4 archives
// - Implements toolbar buttons: New, Open, Favorites, Add, Extract, View, CheckOut, Wizard
// - Uses a simple container format "PCK2" and lz4 CLI fallback
// Build: use your CMakeLists.txt (AUTOMOC ON) and link Qt6::Widgets
// Save as lz4_winzip_clone.cpp

#include <QtWidgets>
#include <QProcess>
#include <QDirIterator>
#include <QTemporaryFile>

static QString humanSize(qint64 bytes) {
    if (bytes < 0) return QString();
    const char* units[] = {"B","KB","MB","GB","TB"};
    double b = double(bytes);
    int i = 0;
    while (b >= 1024.0 && i < 4) { b /= 1024.0; ++i; }
    return QString::asprintf("%.2f %s", b, units[i]);
}

static void appendLog(QTextEdit* log, const QString& line) {
    QString ts = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    log->append(QString("[%1] %2").arg(ts, line));
}

// Simple container parser/writer helpers (PCK2)
// Format per entry:
// 4 bytes "PCK2"
// 1 byte type (0=file,1=dir)
// u16 pathlen
// path bytes (UTF-8)
// u64 filesize
// file bytes (only for files)

struct ArchiveEntry {
    QString relPath;
    bool isDir;
    quint64 size;
};

class ArchiveIO {
public:
    // Create container file (uncompressed) from list of absolute paths (files and dirs)
    static bool createContainer(const QStringList& absPaths, const QString& containerPath, QTextEdit* log) {
        QFile out(containerPath);
        if (!out.open(QIODevice::WriteOnly)) { appendLog(log, "Cannot create container file."); return false; }
        for (const QString& p : absPaths) {
            QFileInfo fi(p);
            QString rel = QDir::current().relativeFilePath(p);
            QByteArray pathUtf8 = rel.toUtf8();
            out.write("PCK2",4);
            unsigned char type = fi.isDir() ? 1 : 0;
            out.write(reinterpret_cast<const char*>(&type),1);
            quint16 plen = quint16(pathUtf8.size());
            out.write(reinterpret_cast<const char*>(&plen), sizeof(plen));
            out.write(pathUtf8);
            quint64 fsize = fi.isFile() ? (quint64)fi.size() : 0;
            out.write(reinterpret_cast<const char*>(&fsize), sizeof(fsize));
            if (fi.isFile()) {
                QFile in(p);
                if (!in.open(QIODevice::ReadOnly)) { appendLog(log, QString("Cannot open %1").arg(p)); out.close(); return false; }
                while (!in.atEnd()) {
                    QByteArray chunk = in.read(1<<16);
                    out.write(chunk);
                }
                in.close();
            }
            appendLog(log, QString("Added to container: %1").arg(rel));
        }
        out.close();
        return true;
    }

    // Parse container file (uncompressed) and return entries (without extracting file bytes)
    static bool parseContainer(const QString& containerPath, QList<ArchiveEntry>& entries, QTextEdit* log) {
        QFile in(containerPath);
        if (!in.open(QIODevice::ReadOnly)) { appendLog(log, "Cannot open container for reading."); return false; }
        QByteArray all = in.readAll();
        const char* ptr = all.constData();
        qint64 remaining = all.size();
        while (remaining > 0) {
            if (remaining < 4) { appendLog(log, "Corrupt container (short magic)"); return false; }
            if (memcmp(ptr, "PCK2", 4) != 0) { appendLog(log, "Invalid container magic"); return false; }
            ptr += 4; remaining -= 4;
            unsigned char type = (unsigned char)*ptr; ptr += 1; remaining -= 1;
            quint16 plen;
            if (remaining < (int)sizeof(plen)) { appendLog(log, "Corrupt container (short plen)"); return false; }
            memcpy(&plen, ptr, sizeof(plen)); ptr += sizeof(plen); remaining -= sizeof(plen);
            if (remaining < plen) { appendLog(log, "Corrupt container (short path)"); return false; }
            QString rel = QString::fromUtf8(ptr, plen);
            ptr += plen; remaining -= plen;
            if (remaining < (int)sizeof(quint64)) { appendLog(log, "Corrupt container (short fsize)"); return false; }
            quint64 fsize;
            memcpy(&fsize, ptr, sizeof(fsize)); ptr += sizeof(fsize); remaining -= sizeof(fsize);
            // skip file bytes
            if ((qint64)fsize > remaining) { appendLog(log, "Corrupt container (short file data)"); return false; }
            ptr += fsize; remaining -= fsize;
            ArchiveEntry e; e.relPath = rel; e.isDir = (type==1); e.size = fsize;
            entries << e;
        }
        return true;
    }
};

// Worker to compress/decompress using lz4 CLI (simple, runs in background)
class Lz4Worker : public QObject, public QRunnable {
    Q_OBJECT
public:
    enum Mode { Compress, Decompress };
    Lz4Worker(const QString& src, const QString& dst, Mode mode, QTextEdit* log)
        : m_src(src), m_dst(dst), m_mode(mode), m_log(log) { setAutoDelete(true); }

signals:
    void progress(int percent, const QString& message);
    void finished(bool ok, const QString& message);

protected:
    void run() override {
        emit progress(0, "Starting lz4 process");
        QStringList args;
        if (m_mode == Compress) {
            args << m_src << m_dst;
        } else {
            args << "-d" << m_src << m_dst;
        }
        QProcess proc;
        proc.start("lz4", args);
        if (!proc.waitForStarted(3000)) {
            appendLog(m_log, "Failed to start lz4. Is it installed?");
            emit finished(false, "lz4 not available");
            return;
        }
        while (proc.state() == QProcess::Running) {
            QThread::msleep(150);
            emit progress(50, "Processing...");
        }
        proc.waitForFinished();
        if (proc.exitCode() != 0) {
            QString err = proc.readAllStandardError();
            appendLog(m_log, QString("lz4 failed: %1").arg(err));
            emit finished(false, "lz4 failed");
            return;
        }
        emit progress(100, "lz4 finished");
        emit finished(true, "lz4 finished");
    }

private:
    QString m_src;
    QString m_dst;
    Mode m_mode;
    QTextEdit* m_log;
};

// MainWindow: WinZip-like UI and full button behavior
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow() {
        setWindowTitle("LZ4 Packer — WinZip Style");
        resize(1100, 720);

        QWidget* central = new QWidget;
        setCentralWidget(central);

        // Ribbon toolbar (large icons)
        QToolBar* ribbon = new QToolBar;
        ribbon->setIconSize(QSize(32,32));
        ribbon->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        addToolBar(Qt::TopToolBarArea, ribbon);

        QAction* aNew = ribbon->addAction(QIcon::fromTheme("document-new"), "New");
        QAction* aOpen = ribbon->addAction(QIcon::fromTheme("document-open"), "Open");
        QAction* aFavorites = ribbon->addAction(QIcon::fromTheme("starred"), "Favorites");
        ribbon->addSeparator();
        QAction* aAdd = ribbon->addAction(QIcon::fromTheme("list-add"), "Add");
        QAction* aExtract = ribbon->addAction(QIcon::fromTheme("document-save-as"), "Extract");
        QAction* aView = ribbon->addAction(QIcon::fromTheme("document-preview"), "View");
        QAction* aCheckOut = ribbon->addAction(QIcon::fromTheme("document-edit"), "CheckOut");
        ribbon->addSeparator();
        QAction* aWizard = ribbon->addAction(QIcon::fromTheme("help-browser"), "Wizard");

        // Left favorites pane
        QFrame* leftFrame = new QFrame;
        leftFrame->setFrameShape(QFrame::StyledPanel);
        leftFrame->setMinimumWidth(200);
        QVBoxLayout* leftLayout = new QVBoxLayout(leftFrame);
        QLabel* lblFav = new QLabel("<b>Favorites</b>");
        leftLayout->addWidget(lblFav);
        m_favList = new QListWidget;
        m_favList->addItems(QStringList() << "Desktop" << "Documents" << "Downloads" << "Home");
        leftLayout->addWidget(m_favList);
        leftLayout->addStretch();

        // Center file table
        m_table = new QTableWidget;
        m_table->setColumnCount(5);
        m_table->setHorizontalHeaderLabels(QStringList() << "Name" << "Type" << "Modified" << "Size" << "Ratio");
        m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        for (int c=1;c<5;++c) m_table->horizontalHeader()->setSectionResizeMode(c, QHeaderView::ResizeToContents);
        m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_table->setContextMenuPolicy(Qt::CustomContextMenu);

        // Right preview/details
        QFrame* rightFrame = new QFrame;
        rightFrame->setFrameShape(QFrame::StyledPanel);
        rightFrame->setMinimumWidth(320);
        QVBoxLayout* rightLayout = new QVBoxLayout(rightFrame);
        QLabel* lblPreview = new QLabel("<b>Preview</b>");
        rightLayout->addWidget(lblPreview);
        m_preview = new QTextEdit; m_preview->setReadOnly(true);
        rightLayout->addWidget(m_preview);
        QLabel* lblDetails = new QLabel("<b>Details</b>");
        rightLayout->addWidget(lblDetails);
        m_details = new QTextEdit; m_details->setReadOnly(true); m_details->setMaximumHeight(140);
        rightLayout->addWidget(m_details);

        // Bottom progress + log
        m_progress = new QProgressBar; m_progress->setRange(0,100);
        m_log = new QTextEdit; m_log->setReadOnly(true); m_log->setMaximumHeight(120);

        // Layout composition
        QHBoxLayout* topLayout = new QHBoxLayout;
        topLayout->addWidget(leftFrame, 0);
        topLayout->addWidget(m_table, 1);
        topLayout->addWidget(rightFrame, 0);

        QVBoxLayout* mainLayout = new QVBoxLayout;
        mainLayout->addLayout(topLayout);
        mainLayout->addWidget(m_progress);
        mainLayout->addWidget(m_log);
        central->setLayout(mainLayout);

        statusBar()->showMessage("Ready");

        // Connect actions
        connect(aNew, &QAction::triggered, this, &MainWindow::onNewArchive);
        connect(aOpen, &QAction::triggered, this, &MainWindow::onOpenArchive);
        connect(aFavorites, &QAction::triggered, this, &MainWindow::onToggleFavorites);
        connect(aAdd, &QAction::triggered, this, &MainWindow::onAdd);
        connect(aExtract, &QAction::triggered, this, &MainWindow::onExtract);
        connect(aView, &QAction::triggered, this, &MainWindow::onView);
        connect(aCheckOut, &QAction::triggered, this, &MainWindow::onCheckOut);
        connect(aWizard, &QAction::triggered, this, &MainWindow::onWizard);

        connect(m_table, &QTableWidget::customContextMenuRequested, this, &MainWindow::onTableContextMenu);
        connect(m_table, &QTableWidget::itemSelectionChanged, this, &MainWindow::onSelectionChanged);
        connect(m_favList, &QListWidget::itemDoubleClicked, this, &MainWindow::onFavoriteActivated);

        setAcceptDrops(true);
        appendLog(m_log, "UI ready");
    }

protected:
    void dragEnterEvent(QDragEnterEvent* e) override {
        if (e->mimeData()->hasUrls()) e->acceptProposedAction();
    }
    void dropEvent(QDropEvent* e) override {
        const QList<QUrl> urls = e->mimeData()->urls();
        for (const QUrl& u : urls) {
            QString path = u.toLocalFile();
            QFileInfo fi(path);
            if (fi.isDir()) addDirectory(path);
            else if (fi.isFile()) addFile(path);
        }
    }

private slots:
    // New: clear current list and reset state
    void onNewArchive() {
        m_table->setRowCount(0);
        m_currentArchive.clear();
        appendLog(m_log, "New archive (empty) created in UI.");
        statusBar()->showMessage("New archive");
    }

    // Open: decompress archive (.lz4) to temp container and parse entries
    void onOpenArchive() {
        QString file = QFileDialog::getOpenFileName(this, "Open archive", QDir::currentPath(), "LZ4 Archive (*.lz4)");
        if (file.isEmpty()) return;
        appendLog(m_log, QString("Opening archive: %1").arg(file));
        // decompress to temp
        QString tmp = QDir::temp().filePath(QString("lz4_unpack_%1.bin").arg(QDateTime::currentMSecsSinceEpoch()));
        Lz4Worker* w = new Lz4Worker(file, tmp, Lz4Worker::Decompress, m_log);
        connect(w, &Lz4Worker::progress, this, &MainWindow::onWorkerProgress);
        connect(w, &Lz4Worker::finished, this, [=](bool ok, const QString& msg){
            if (!ok) { appendLog(m_log, "Failed to decompress archive."); return; }
            QList<ArchiveEntry> entries;
            if (!ArchiveIO::parseContainer(tmp, entries, m_log)) { appendLog(m_log, "Failed to parse container."); QFile::remove(tmp); return; }
            populateTableFromEntries(entries);
            QFile::remove(tmp);
            m_currentArchive = file;
            appendLog(m_log, QString("Opened archive: %1").arg(file));
        });
        QThreadPool::globalInstance()->start(w);
    }

    // Toggle favorites pane visibility
    void onToggleFavorites() {
        bool vis = m_favList->isVisible();
        m_favList->setVisible(!vis);
        appendLog(m_log, QString("Favorites %1").arg(!vis ? "shown" : "hidden"));
    }

    // Add: add files or directories
    void onAdd() {
        QStringList files = QFileDialog::getOpenFileNames(this, "Add files");
        for (const QString& f : files) addFile(f);
        QString dir = QFileDialog::getExistingDirectory(this, "Add directory (optional)");
        if (!dir.isEmpty()) addDirectory(dir);
    }

    // Extract: extract selected entries to chosen folder
    void onExtract() {
        QString dest = QFileDialog::getExistingDirectory(this, "Select destination folder");
        if (dest.isEmpty()) return;
        appendLog(m_log, QString("Extracting selected items to %1").arg(dest));
        // If current archive is set, decompress and extract; otherwise, extract selected files directly from disk (if they are local)
        if (!m_currentArchive.isEmpty()) {
            // decompress archive to temp and parse, then extract matching entries
            QString tmp = QDir::temp().filePath(QString("lz4_unpack_%1.bin").arg(QDateTime::currentMSecsSinceEpoch()));
            Lz4Worker* w = new Lz4Worker(m_currentArchive, tmp, Lz4Worker::Decompress, m_log);
            connect(w, &Lz4Worker::finished, this, [=](bool ok, const QString&){
                if (!ok) { appendLog(m_log, "Failed to decompress for extraction."); QFile::remove(tmp); return; }
                // parse and extract
                QFile in(tmp);
                if (!in.open(QIODevice::ReadOnly)) { appendLog(m_log, "Cannot open temp container."); QFile::remove(tmp); return; }
                QByteArray all = in.readAll();
                const char* ptr = all.constData();
                qint64 remaining = all.size();
                while (remaining > 0) {
                    if (remaining < 4) break;
                    if (memcmp(ptr,"PCK2",4)!=0) break;
                    ptr+=4; remaining-=4;
                    unsigned char type = (unsigned char)*ptr; ptr+=1; remaining-=1;
                    quint16 plen; memcpy(&plen, ptr, sizeof(plen)); ptr+=sizeof(plen); remaining-=sizeof(plen);
                    QString rel = QString::fromUtf8(ptr, plen); ptr+=plen; remaining-=plen;
                    quint64 fsize; memcpy(&fsize, ptr, sizeof(fsize)); ptr+=sizeof(fsize); remaining-=sizeof(fsize);
                    if (type==1) {
                        QDir(dest).mkpath(rel);
                    } else {
                        QByteArray data(ptr, (qint64)fsize);
                        QString outPath = QDir(dest).filePath(rel);
                        QDir().mkpath(QFileInfo(outPath).absolutePath());
                        QFile out(outPath);
                        if (out.open(QIODevice::WriteOnly)) out.write(data);
                        ptr += fsize; remaining -= fsize;
                    }
                }
                in.close();
                QFile::remove(tmp);
                appendLog(m_log, "Extraction finished.");
            });
            QThreadPool::globalInstance()->start(w);
        } else {
            // extract selected local files (copy)
            auto rows = selectedRows();
            for (int r : rows) {
                QString path = m_table->item(r,0)->data(Qt::UserRole).toString();
                QFileInfo fi(path);
                if (fi.isFile()) {
                    QString destPath = QDir(dest).filePath(fi.fileName());
                    QFile::copy(path, destPath);
                } else if (fi.isDir()) {
                    QDir().mkpath(QDir(dest).filePath(fi.fileName()));
                }
            }
            appendLog(m_log, "Copied selected items to destination.");
        }
    }

    // View: preview selected file (text/image)
    void onView() {
        auto rows = selectedRows();
        if (rows.isEmpty()) { appendLog(m_log, "No selection to view."); return; }
        int r = rows.first();
        QString path = m_table->item(r,0)->data(Qt::UserRole).toString();
        QFileInfo fi(path);
        if (fi.isFile()) {
            if (fi.size() > 200000) { appendLog(m_log, "File too large to preview."); return; }
            QFile f(path);
            if (f.open(QIODevice::ReadOnly)) {
                QByteArray data = f.readAll();
                QString text = QString::fromUtf8(data);
                m_preview->setPlainText(text.left(100000));
                appendLog(m_log, QString("Previewed: %1").arg(fi.fileName()));
                f.close();
            } else appendLog(m_log, "Cannot open file for preview.");
        } else appendLog(m_log, "Selected item is not a file.");
    }

    // CheckOut: extract selected file to temp and open in default editor; mark as checked out
    void onCheckOut() {
        auto rows = selectedRows();
        if (rows.isEmpty()) { appendLog(m_log, "No selection to check out."); return; }
        int r = rows.first();
        QString path = m_table->item(r,0)->data(Qt::UserRole).toString();
        QFileInfo fi(path);
        if (!fi.exists()) { appendLog(m_log, "File not found."); return; }
        QString tmp = QDir::temp().filePath(fi.fileName());
        if (fi.isFile()) {
            QFile::copy(path, tmp);
            QDesktopServices::openUrl(QUrl::fromLocalFile(tmp));
            appendLog(m_log, QString("Checked out: %1 -> %2").arg(fi.fileName()).arg(tmp));
            // mark in UI
            //m_table->item(r,0)->setBackground((QColor(255, 250, 200));
        } else appendLog(m_log, "CheckOut only supports files.");
    }

    // Wizard: show a simple QWizard for common tasks
    void onWizard() {
        QWizard wizard(this);
        wizard.setWindowTitle("Archive Wizard");
        QWizardPage* p1 = new QWizardPage; p1->setTitle("Step 1: Choose action");
        QVBoxLayout* l1 = new QVBoxLayout(p1);
        QRadioButton* rbPack = new QRadioButton("Create a new archive and add files");
        QRadioButton* rbOpen = new QRadioButton("Open an existing archive");
        rbPack->setChecked(true);
        l1->addWidget(rbPack); l1->addWidget(rbOpen);
        QWizardPage* p2 = new QWizardPage; p2->setTitle("Step 2: Files");
        QVBoxLayout* l2 = new QVBoxLayout(p2);
        QPushButton* btnAdd = new QPushButton("Add files...");
        l2->addWidget(btnAdd);
        connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAdd);
        wizard.addPage(p1); wizard.addPage(p2);
        wizard.exec();
    }

    // Table context menu
    void onTableContextMenu(const QPoint& pt) {
        QTableWidgetItem* it = m_table->itemAt(pt);
        QMenu m;
        QAction* open = m.addAction("Open location");
        QAction* extract = m.addAction("Extract");
        QAction* view = m.addAction("View");
        QAction* remove = m.addAction("Remove");
        QAction* act = m.exec(m_table->viewport()->mapToGlobal(pt));
        if (!act) return;
        int row = it ? it->row() : -1;
        if (act == open && row >= 0) {
            QString path = m_table->item(row,0)->data(Qt::UserRole).toString();
            QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(path).absolutePath()));
        } else if (act == extract) onExtract();
        else if (act == view) onView();
        else if (act == remove && row >= 0) { m_table->removeRow(row); updateStatus(); }
    }

    void onSelectionChanged() {
        auto rows = selectedRows();
        if (rows.isEmpty()) { m_preview->clear(); m_details->clear(); return; }
        int r = rows.first();
        QString path = m_table->item(r,0)->data(Qt::UserRole).toString();
        QFileInfo fi(path);
        m_details->setPlainText(QString("Path: %1\nType: %2\nSize: %3\nModified: %4")
                                .arg(fi.absoluteFilePath())
                                .arg(fi.isDir() ? "Directory" : "File")
                                .arg(humanSize(fi.size()))
                                .arg(fi.lastModified().toString()));
    }

    void onFavoriteActivated(QListWidgetItem* it) {
        QString name = it->text();
        QString path;
        if (name == "Desktop") path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        else if (name == "Documents") path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        else if (name == "Downloads") path = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        else path = QDir::homePath();
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }

    void onWorkerProgress(int percent, const QString& message) {
        m_progress->setValue(percent);
        statusBar()->showMessage(message);
    }

    // Helper: add single file row
    void addFile(const QString& path) {
        QFileInfo fi(path);
        if (!fi.exists() || !fi.isFile()) return;
        int row = m_table->rowCount();
        m_table->insertRow(row);
        QTableWidgetItem* name = new QTableWidgetItem(fi.fileName());
        name->setData(Qt::UserRole, fi.absoluteFilePath());
        QTableWidgetItem* type = new QTableWidgetItem(fi.suffix().isEmpty() ? "File" : fi.suffix().toUpper());
        QTableWidgetItem* mod = new QTableWidgetItem(fi.lastModified().toString("yyyy-MM-dd HH:mm"));
        QTableWidgetItem* size = new QTableWidgetItem(humanSize(fi.size()));
        QTableWidgetItem* ratio = new QTableWidgetItem("-");
        m_table->setItem(row,0,name); m_table->setItem(row,1,type); m_table->setItem(row,2,mod);
        m_table->setItem(row,3,size); m_table->setItem(row,4,ratio);
        updateStatus();
        appendLog(m_log, QString("Added file: %1").arg(fi.absoluteFilePath()));
    }

    // Helper: add directory (flat: add dir entry + files)
    void addDirectory(const QString& dirPath) {
        QFileInfo fi(dirPath);
        if (!fi.exists() || !fi.isDir()) return;
        int row = m_table->rowCount();
        m_table->insertRow(row);
        QTableWidgetItem* name = new QTableWidgetItem(fi.fileName());
        name->setData(Qt::UserRole, fi.absoluteFilePath());
        QTableWidgetItem* type = new QTableWidgetItem("Directory");
        QTableWidgetItem* mod = new QTableWidgetItem(fi.lastModified().toString("yyyy-MM-dd HH:mm"));
        QTableWidgetItem* size = new QTableWidgetItem("");
        QTableWidgetItem* ratio = new QTableWidgetItem("");
        m_table->setItem(row,0,name); m_table->setItem(row,1,type); m_table->setItem(row,2,mod);
        m_table->setItem(row,3,size); m_table->setItem(row,4,ratio);
        // add files recursively (flat)
        QDirIterator it(dirPath, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) { it.next(); addFile(it.filePath()); }
        updateStatus();
        appendLog(m_log, QString("Added directory: %1").arg(fi.absoluteFilePath()));
    }

    // Populate table from parsed entries (from container)
    void populateTableFromEntries(const QList<ArchiveEntry>& entries) {
        m_table->setRowCount(0);
        for (const ArchiveEntry& e : entries) {
            int row = m_table->rowCount();
            m_table->insertRow(row);
            QTableWidgetItem* name = new QTableWidgetItem(QFileInfo(e.relPath).fileName());
            name->setData(Qt::UserRole, e.relPath); // store relative path for archive view
            QTableWidgetItem* type = new QTableWidgetItem(e.isDir ? "Directory" : "File");
            QTableWidgetItem* mod = new QTableWidgetItem("");
            QTableWidgetItem* size = new QTableWidgetItem(e.isDir ? "" : humanSize((qint64)e.size));
            QTableWidgetItem* ratio = new QTableWidgetItem("-");
            m_table->setItem(row,0,name); m_table->setItem(row,1,type); m_table->setItem(row,2,mod);
            m_table->setItem(row,3,size); m_table->setItem(row,4,ratio);
        }
        updateStatus();
    }

    QList<int> selectedRows() const {
        QList<int> rows;
        for (QTableWidgetItem* it : m_table->selectedItems()) {
            if (!rows.contains(it->row())) rows << it->row();
        }
        std::sort(rows.begin(), rows.end());
        return rows;
    }

    void updateStatus() {
        int count = m_table->rowCount();
        qint64 total = 0;
        for (int r=0;r<count;++r) {
            QVariant v = m_table->item(r,0)->data(Qt::UserRole);
            QString path = v.toString();
            QFileInfo fi(path);
            if (fi.exists() && fi.isFile()) total += fi.size();
        }
        statusBar()->showMessage(QString("%1 items — %2").arg(count).arg(humanSize(total)));
    }

private:
    QTableWidget* m_table = nullptr;
    QListWidget* m_favList = nullptr;
    QTextEdit* m_preview = nullptr;
    QTextEdit* m_details = nullptr;
    QProgressBar* m_progress = nullptr;
    QTextEdit* m_log = nullptr;
    QString m_currentArchive;
};

// main
int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("lz4_winzip_clone");
    QCoreApplication::setOrganizationName("ExampleOrg");
    MainWindow w;
    w.show();
    return app.exec();
}

#include "lz4packer1.moc"
