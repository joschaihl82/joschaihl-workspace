// ext4recover.cpp
//
// Single-file Qt6 application (Widgets + WebEngine) that scans an ext4 device/image
// and displays discovered directory entries in a table with columns:
//   Inode Nr. | Filename | Filesize | Create Date | Change Date
//
// This version implements stricter filtering before adding rows to the grid:
//  - Only entries with a non-zero inode number are considered.
//  - Filenames are validated using the same heuristics as the provided C scanner:
//      * UTF-8 printable (no NUL, no '/'), reasonable length (1..255).
//      * At least one filename-like character (alnum or . _ -).
//      * Reject names that are only punctuation or control characters.
//  - If a mountpoint was selected and an inode->path map was built, the entry is
//    only added if the inode number exists in that map (i.e., a "correct inode-nr").
//  - Entries named "." or ".." are ignored.
//
// Layout and other behavior follow the previously provided UI: three-pane splitter
// (grid | spacer | preview), progress bar reserved at 30px at bottom, top dropdown
// lists device nodes with ext4 mounts and an option to pick an image file.
//
// Build with Qt6 (Widgets + WebEngineWidgets).
//

#include <QApplication>
#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QTableView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QProgressBar>
#include <QSplitter>
#include <QWebEngineView>
#include <QFileDialog>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QThread>
#include <QFileInfo>
#include <QUrl>
#include <QTimer>
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QDateTime>
#include <QDirIterator>
#include <QMenu>
#include <QAction>
#include <QClipboard>
#include <QDesktopServices>
#include <QSet>
#include <QMutex>
#include <QMutexLocker>
#include <QByteArray>

#include <atomic>
#include <vector>
#include <string>
#include <cstring>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <sstream>
#include <iomanip>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

//
// Low-level helpers (POSIX)
//
static ssize_t pread_all(int fd, void *buf, size_t count, off_t offset) {
    size_t done = 0;
    while (done < count) {
        ssize_t r = pread(fd, (char*)buf + done, count - done, offset + done);
        if (r < 0) return -1;
        if (r == 0) break;
        done += r;
    }
    return done;
}
static inline uint16_t le16(const void *p) {
    const uint8_t *b = (const uint8_t*)p;
    return (uint16_t)b[0] | ((uint16_t)b[1] << 8);
}
static inline uint32_t le32(const void *p) {
    const uint8_t *b = (const uint8_t*)p;
    return (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
}

#pragma pack(push,1)
struct ext4_super {
    uint32_t s_inodes_count;
    uint32_t s_blocks_count_lo;
    uint32_t s_r_blocks_count_lo;
    uint32_t s_free_blocks_count_lo;
    uint32_t s_free_inodes_count;
    uint32_t s_first_data_block;
    uint32_t s_log_block_size;
    uint32_t s_log_cluster_size;
    uint32_t s_blocks_per_group;
    uint32_t s_clusters_per_group;
    uint32_t s_inodes_per_group;
    uint32_t s_mtime;
    uint32_t s_wtime;
    uint16_t s_mnt_count;
    uint16_t s_max_mnt_count;
    uint16_t s_magic;
    uint16_t s_state;
    uint16_t s_errors;
    uint16_t s_minor_rev_level;
    uint32_t s_lastcheck;
    uint32_t s_checkinterval;
    uint32_t s_creator_os;
    uint32_t s_rev_level;
    uint16_t s_def_resuid;
    uint16_t s_def_resgid;
    uint32_t s_first_ino;
    uint16_t s_inode_size;
    uint16_t s_block_group_nr;
    uint32_t s_feature_compat;
    uint32_t s_feature_incompat;
    uint32_t s_feature_ro_compat;
    uint8_t  s_uuid[16];
    char     s_volume_name[16];
    char     s_last_mounted[64];
};
struct ext4_inode {
    uint16_t i_mode;
    uint16_t i_uid;
    uint32_t i_size_lo;
    uint32_t i_atime;
    uint32_t i_ctime;
    uint32_t i_mtime;
    uint32_t i_dtime;
    uint16_t i_gid;
    uint16_t i_links_count;
    uint32_t i_blocks_lo;
    uint32_t i_flags;
    uint32_t i_osd1;
    uint32_t i_block[15];
    uint32_t i_generation;
    uint32_t i_file_acl_lo;
    uint32_t i_size_high;
    uint32_t i_obso_faddr;
    uint8_t  i_osd2[12];
};
struct ext2_dir_entry {
    uint32_t inode;
    uint16_t rec_len;
    uint8_t  name_len;
    uint8_t  file_type;
    // name follows
};
#pragma pack(pop)

//
// Filename validation helpers (C++ versions of the C heuristics)
//
static bool is_printable_utf8_filename(const QByteArray &ba) {
    const unsigned char *p = (const unsigned char*)ba.constData();
    size_t len = (size_t)ba.size();
    if (len == 0 || len > 255) return false;
    size_t i = 0;
    int printable_count = 0;
    while (i < len) {
        unsigned char c = p[i];
        if (c == 0) return false;
        if (c == '/') return false;
        if (c < 0x80) {
            if (iscntrl(c)) return false;
            if (isprint(c)) printable_count++;
            i++;
            continue;
        }
        size_t seq_len = 0;
        if ((c & 0xE0) == 0xC0) seq_len = 2;
        else if ((c & 0xF0) == 0xE0) seq_len = 3;
        else if ((c & 0xF8) == 0xF0) seq_len = 4;
        else return false;
        if (i + seq_len > len) return false;
        for (size_t k = 1; k < seq_len; ++k) if ((p[i + k] & 0xC0) != 0x80) return false;
        printable_count++;
        i += seq_len;
    }
    return printable_count > 0;
}

static bool name_has_filename_chars(const QByteArray &ba) {
    const unsigned char *p = (const unsigned char*)ba.constData();
    size_t len = (size_t)ba.size();
    for (size_t i = 0; i < len; ++i) {
        unsigned char c = p[i];
        if (isalnum(c)) return true;
        if (c == '.' || c == '_' || c == '-') return true;
    }
    return false;
}

static bool validate_filename_like(const QString &name) {
    if (name.isEmpty()) return false;
    if (name == "." || name == "..") return false;
    QByteArray ba = name.toUtf8();
    if (!is_printable_utf8_filename(ba)) return false;
    if (!name_has_filename_chars(ba)) return false;
    return true;
}

//
// Scanner: worker object that runs in a QThread
// Emits: entryFound(path, percent, pseudoInode, inodeNumber), progress(percent), finished(), logMessage(msg)
//
class Scanner : public QObject {
    Q_OBJECT
public:
    explicit Scanner(QObject *parent = nullptr) : QObject(parent), m_stop(false) {}
    ~Scanner() override { requestStop(); }

    void requestStop() { m_stop.store(true); }

public slots:
    void startScan(const QString &devicePath) {
        m_stop.store(false);
        scanDevice(devicePath);
    }

signals:
    void entryFound(const QString &path, double percent, quint64 pseudoInode, quint64 inodeNumber);
    void progress(double percent);
    void finished();
    void logMessage(const QString &msg);

private:
    std::atomic<bool> m_stop;

    void log(const QString &s) { emit logMessage(s); }

    bool readSuperblock(int fd, uint16_t &inodeSize, uint32_t &blockSize) {
        ext4_super sb;
        if (pread_all(fd, &sb, sizeof(sb), 1024) != (ssize_t)sizeof(sb)) return false;
        if (le16(&sb.s_magic) != 0xEF53) return false;
        uint16_t isz = le16(&sb.s_inode_size);
        if (isz == 0) isz = 128;
        uint32_t log_block = le32(&sb.s_log_block_size);
        uint32_t bsz = 1024U << log_block;
        inodeSize = isz;
        blockSize = bsz;
        return true;
    }

    bool validateDirEntry(const uint8_t *block, size_t blockSize, size_t offset) {
        if (offset + 8 > blockSize) return false;
        const uint8_t *p = block + offset;
        uint32_t inode = le32(p);
        uint16_t rec_len = le16(p + 4);
        uint8_t name_len = p[6];
        uint8_t file_type = p[7];
        if (rec_len < 8) return false;
        if (offset + rec_len > blockSize) return false;
        if (name_len == 0 || name_len > rec_len - 8) return false;
        if (name_len > 255) return false;
        if (inode == 0) return false;
        if (file_type > 7) return false;
        const uint8_t *namep = p + 8;
        int printable = 0;
        int alnum = 0;
        for (size_t i = 0; i < name_len; ++i) {
            uint8_t c = namep[i];
            if (c == 0 || c == '/') return false;
            if (c >= 0x20 && c < 0x7f) printable++;
            if (isalnum(c)) alnum++;
        }
        if (printable == 0) return false;
        if (alnum == 0 && name_len < 2) return false;
        return true;
    }

    void scanDevice(const QString &devicePath) {
        log(QString("Scanner: opening %1").arg(devicePath));
        int fd = open(devicePath.toLocal8Bit().constData(), O_RDONLY | O_CLOEXEC);
        if (fd < 0) {
            log(QString("Scanner: failed to open %1: %2").arg(devicePath, QString::fromUtf8(strerror(errno))));
            emit finished();
            return;
        }

        uint16_t inodeSize = 128;
        uint32_t blockSize = 4096;
        if (!readSuperblock(fd, inodeSize, blockSize)) {
            log("Scanner: superblock not found or not ext4; using defaults");
            inodeSize = 128;
            blockSize = 4096;
        } else {
            log(QString("Scanner: superblock read: inode_size=%1 block_size=%2").arg(inodeSize).arg(blockSize));
        }

        off_t end = lseek(fd, 0, SEEK_END);
        if (end == (off_t)-1) {
            log(QString("Scanner: lseek failed: %1").arg(QString::fromUtf8(strerror(errno))));
            close(fd);
            emit finished();
            return;
        }
        off_t totalBytes = end;
        log(QString("Scanner: device size %1 bytes").arg((long long)totalBytes));

        std::unique_ptr<uint8_t[]> inodeBuf(new (std::nothrow) uint8_t[inodeSize ? inodeSize : 128]);
        if (!inodeBuf) { log("Scanner: allocation failed"); close(fd); emit finished(); return; }

        off_t processedBytes = 0;
        for (off_t off = 0; off + inodeSize <= end; off += inodeSize) {
            if (m_stop.load()) {
                log("Scanner: stop requested");
                break;
            }

            ssize_t r = pread_all(fd, inodeBuf.get(), inodeSize, off);
            if (r != (ssize_t)inodeSize) {
                processedBytes = off + inodeSize;
                double pct = totalBytes > 0 ? (double)processedBytes / (double)totalBytes * 100.0 : 0.0;
                emit progress(pct);
                continue;
            }

            processedBytes = off;
            double pct = totalBytes > 0 ? (double)processedBytes / (double)totalBytes * 100.0 : 0.0;
            emit progress(pct);

            ext4_inode inode;
            memset(&inode, 0, sizeof(inode));
            memcpy(&inode, inodeBuf.get(), std::min((size_t)sizeof(inode), (size_t)inodeSize));
            uint16_t mode = le16(&inode.i_mode);
            uint16_t type = mode & 0xF000;
            if (type != 0x4000) continue; // not a directory

            uint64_t pseudoIno = (uint64_t)(off / inodeSize) + 1;

            for (int bi = 0; bi < 12; ++bi) {
                uint32_t blk = le32(&inode.i_block[bi]);
                if (blk == 0) continue;
                off_t blockOff = (off_t)blk * (off_t)blockSize;
                if (blockOff + blockSize > end) continue;
                std::unique_ptr<uint8_t[]> blockBuf(new (std::nothrow) uint8_t[blockSize]);
                if (!blockBuf) continue;
                if (pread_all(fd, blockBuf.get(), blockSize, blockOff) != (ssize_t)blockSize) continue;
                uint32_t pos = 0;
                while (pos + 8 < blockSize) {
                    if (!validateDirEntry(blockBuf.get(), blockSize, pos)) { pos += 4; continue; }
                    const uint8_t *p = blockBuf.get() + pos;
                    uint32_t inode_no = le32(p);
                    uint16_t rec_len = le16(p + 4);
                    uint8_t name_len = p[6];
                    if (rec_len < 8) break;
                    if (pos + rec_len > blockSize) break;
                    if (inode_no != 0 && name_len > 0 && name_len <= 255) {
                        std::string name((const char*)(p + 8), name_len);
                        char bufPath[1024];
                        snprintf(bufPath, sizeof(bufPath), "/[ino:%llu]/%s", (unsigned long long)pseudoIno, name.c_str());
                        QString qpath = QString::fromUtf8(bufPath);
                        emit entryFound(qpath, pct, (quint64)pseudoIno, (quint64)inode_no);
                    }
                    pos += rec_len;
                }
            }
        }

        emit progress(100.0);
        log("Scanner: finished");
        close(fd);
        emit finished();
    }
};

//
// MainWindow: UI and logic
//
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent),
        m_model(new QStandardItemModel(this)),
        m_proxy(new QSortFilterProxyModel(this)),
        m_scanner(nullptr),
        m_scanThread(nullptr)
    {
        setupUi();
        setupConnections();
        populateFsCombo();
    }

    ~MainWindow() override {
        stopScanner();
    }

private slots:
    void refreshFsList() { populateFsCombo(); }

    void onFsActivated(int index) {
        QVariant data = m_fsCombo->itemData(index);
        if (!data.isValid()) return;
        QString v = data.toString();
        if (v == "__choose_file__") {
            QString device = QFileDialog::getOpenFileName(this, "Select device or image to scan", QString(), "All files (*)");
            if (device.isEmpty()) return;
            // insert and select
            m_fsCombo->insertItem(1, device, device + "|");
            m_fsCombo->setCurrentIndex(1);
            startScanForCurrentSelection();
        } else {
            startScanForCurrentSelection();
        }
    }

    void onSearchClicked() {
        QString pattern = m_searchEdit->text();
        if (pattern.isEmpty()) m_proxy->setFilterRegularExpression(QRegularExpression());
        else {
            QRegularExpression re(pattern, QRegularExpression::CaseInsensitiveOption);
            if (re.isValid()) m_proxy->setFilterRegularExpression(re);
            else m_proxy->setFilterFixedString(pattern);
        }
    }

    void onStartClicked() { startScanForCurrentSelection(); }

    void onStopClicked() { stopScanner(); }

    // New: stricter filtering before adding to grid
    void onEntryFound(const QString &path, double percent, quint64 pseudoInode, quint64 inodeNumber) {
        Q_UNUSED(pseudoInode);
        // parse filename from path (after last '/')
        QString filename = path.section('/', -1);
        if (!validate_filename_like(filename)) {
            // filename fails validation heuristics; ignore
            return;
        }

        // require non-zero inode number
        if (inodeNumber == 0) return;

        // If we built an inode map from a mounted filesystem, require the inode to exist there
        if (!m_inodeMap.empty()) {
            auto it = m_inodeMap.find((unsigned long long)inodeNumber);
            if (it == m_inodeMap.end()) {
                // inode not found in mounted tree; skip
                return;
            }
        }

        QString inodeStr = QString::number((unsigned long long)inodeNumber);

        // deduplicate by inode+filename
        QString key = inodeStr + "|" + filename;
        if (m_seenKeys.contains(key)) return;
        m_seenKeys.insert(key);

        // lookup attributes if we have a mountpoint map
        QString sizeStr = "";
        QString createStr = "";
        QString changeStr = "";

        if (!m_inodeMap.empty()) {
            auto it = m_inodeMap.find((unsigned long long)inodeNumber);
            if (it != m_inodeMap.end()) {
                const FileAttrs &fa = it->second;
                sizeStr = QString::number((long long)fa.size);
                createStr = QDateTime::fromSecsSinceEpoch((qint64)fa.ctime).toString(Qt::ISODate);
                changeStr = QDateTime::fromSecsSinceEpoch((qint64)fa.mtime).toString(Qt::ISODate);
            }
        }

        QList<QStandardItem*> row;
        QStandardItem *inodeItem = new QStandardItem(inodeStr);
        QStandardItem *nameItem = new QStandardItem(filename);
        QStandardItem *sizeItem = new QStandardItem(sizeStr);
        QStandardItem *createItem = new QStandardItem(createStr);
        QStandardItem *changeItem = new QStandardItem(changeStr);

        inodeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        row << inodeItem << nameItem << sizeItem << createItem << changeItem;
        m_model->appendRow(row);
    }

    void onProgress(double percent) {
        int p = qBound(0, int(percent + 0.5), 100);
        m_progressBar->setValue(p);
    }

    void onScanFinished() {
        QMessageBox::information(this, "Scan finished", "Device scan finished.");
        m_startButton->setEnabled(true);
        m_stopButton->setEnabled(false);
    }

    void onTableSelectionChanged(const QModelIndex &current, const QModelIndex &previous) {
        Q_UNUSED(previous);
        if (!current.isValid()) return;
        QModelIndex src = m_proxy->mapToSource(current);
        QString inodeStr = m_model->item(src.row(), 0)->text();
        QString filename = m_model->item(src.row(), 1)->text();

        // If we have a mountpoint and inode map, try to open the real file for preview
        if (!m_selectedMountpoint.isEmpty()) {
            unsigned long long ino = inodeStr.toULongLong();
            auto it = m_inodeMap.find(ino);
            if (it != m_inodeMap.end()) {
                QString path = QString::fromUtf8(it->second.path.c_str());
                QFileInfo fi(path);
                if (fi.exists() && fi.isFile()) {
                    QUrl url = QUrl::fromLocalFile(fi.absoluteFilePath());
                    m_preview->load(url);
                    return;
                }
            }
        }
        // fallback: show filename text
        QString html = QString("<html><body><pre>%1</pre></body></html>").arg(filename.toHtmlEscaped());
        m_preview->setHtml(html);
    }

    void onTableContextMenuRequested(const QPoint &pos) {
        QModelIndex idx = m_tableView->indexAt(pos);
        if (!idx.isValid()) return;
        QModelIndex src = m_proxy->mapToSource(idx);
        QString inodeStr = m_model->item(src.row(), 0)->text();
        QString filename = m_model->item(src.row(), 1)->text();

        QMenu menu(this);
        QAction *copyAct = menu.addAction("Copy filename");
        QAction *openFolderAct = menu.addAction("Open containing folder (if available)");
        QAction *selected = menu.exec(m_tableView->viewport()->mapToGlobal(pos));
        if (selected == copyAct) {
            QApplication::clipboard()->setText(filename);
        } else if (selected == openFolderAct) {
            if (!m_selectedMountpoint.isEmpty()) {
                unsigned long long ino = inodeStr.toULongLong();
                auto it = m_inodeMap.find(ino);
                if (it != m_inodeMap.end()) {
                    QString path = QString::fromUtf8(it->second.path.c_str());
                    QFileInfo fi(path);
                    if (fi.exists()) QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absolutePath()));
                    else QMessageBox::information(this, "Open folder", "File not available on local filesystem.");
                } else {
                    QMessageBox::information(this, "Open folder", "No path known for this inode on the mounted filesystem.");
                }
            } else {
                QMessageBox::information(this, "Open folder", "No mountpoint selected.");
            }
        }
    }

private:
    // UI
    QComboBox *m_fsCombo = nullptr;
    QPushButton *m_refreshFsBtn = nullptr;
    QLineEdit *m_searchEdit = nullptr;
    QPushButton *m_searchButton = nullptr;
    QPushButton *m_startButton = nullptr;
    QPushButton *m_stopButton = nullptr;
    QTableView *m_tableView = nullptr;
    QStandardItemModel *m_model = nullptr;
    QSortFilterProxyModel *m_proxy = nullptr;
    QWebEngineView *m_preview = nullptr;
    QProgressBar *m_progressBar = nullptr;
    QLabel *m_statusLabel = nullptr;

    // scanner
    Scanner *m_scanner;
    QThread *m_scanThread;

    // inode -> attributes map (built by walking mountpoint)
    struct FileAttrs { unsigned long long ino; unsigned long long size; time_t ctime; time_t mtime; std::string path; };
    std::unordered_map<unsigned long long, FileAttrs> m_inodeMap;
    QString m_selectedMountpoint;
    QSet<QString> m_seenKeys;

    void setupUi() {
        QWidget *central = new QWidget(this);
        setCentralWidget(central);

        m_fsCombo = new QComboBox(this);
        m_fsCombo->setMinimumWidth(360);
        m_refreshFsBtn = new QPushButton("Refresh FS", this);

        m_searchEdit = new QLineEdit(this);
        m_searchEdit->setPlaceholderText("Filter filenames (regex or substring)");
        m_searchButton = new QPushButton("Search", this);

        m_startButton = new QPushButton("Start", this);
        m_stopButton = new QPushButton("Stop", this);
        m_stopButton->setEnabled(false);

        QHBoxLayout *topLayout = new QHBoxLayout;
        topLayout->addWidget(new QLabel("Filesystem:"));
        topLayout->addWidget(m_fsCombo);
        topLayout->addWidget(m_refreshFsBtn);
        topLayout->addSpacing(12);
        topLayout->addWidget(new QLabel("Filter:"));
        topLayout->addWidget(m_searchEdit);
        topLayout->addWidget(m_searchButton);
        topLayout->addSpacing(12);
        topLayout->addWidget(m_startButton);
        topLayout->addWidget(m_stopButton);

        // table columns: Inode Nr. | Filename | Filesize | Create Date | Change Date
        m_model->setColumnCount(5);
        m_model->setHeaderData(0, Qt::Horizontal, "Inode Nr.");
        m_model->setHeaderData(1, Qt::Horizontal, "Filename");
        m_model->setHeaderData(2, Qt::Horizontal, "Filesize");
        m_model->setHeaderData(3, Qt::Horizontal, "Create Date");
        m_model->setHeaderData(4, Qt::Horizontal, "Change Date");

        m_proxy->setSourceModel(m_model);
        m_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
        m_proxy->setFilterKeyColumn(1);

        m_tableView = new QTableView(this);
        m_tableView->setModel(m_proxy);
        m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        m_tableView->horizontalHeader()->setStretchLastSection(true);
        m_tableView->verticalHeader()->setVisible(false);
        m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);

        m_preview = new QWebEngineView(this);
        m_preview->setHtml("<html><body><h3>No preview</h3></body></html>");

        // --- Three-pane splitter: Grid | Spacer | Preview
        QWidget *gridContainer = new QWidget(this);
        QVBoxLayout *gridLayout = new QVBoxLayout(gridContainer);
        gridLayout->setContentsMargins(0,0,0,0);
        gridLayout->addWidget(m_tableView);

        QWidget *spacerWidget = new QWidget(this);
        spacerWidget->setMinimumWidth(8);
        spacerWidget->setMaximumWidth(400);
        spacerWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

        QWidget *previewContainer = new QWidget(this);
        QVBoxLayout *previewLayout = new QVBoxLayout(previewContainer);
        previewLayout->setContentsMargins(0,0,0,0);
        previewLayout->addWidget(m_preview);

        QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);
        mainSplitter->addWidget(gridContainer);
        mainSplitter->addWidget(spacerWidget);
        mainSplitter->addWidget(previewContainer);

        mainSplitter->setStretchFactor(0, 1); // grid
        mainSplitter->setStretchFactor(1, 0); // spacer
        mainSplitter->setStretchFactor(2, 2); // preview
        QList<int> sizes;
        sizes << 600 << 20 << 900;
        mainSplitter->setSizes(sizes);

        // Reserve 30px for the progress bar at the bottom
        m_progressBar = new QProgressBar(this);
        m_progressBar->setRange(0, 100);
        m_progressBar->setValue(0);
        m_progressBar->setFixedHeight(30);

        m_statusLabel = new QLabel("Ready", this);

        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addLayout(topLayout);
        mainLayout->addWidget(mainSplitter);
        mainLayout->addWidget(m_progressBar);
        mainLayout->addWidget(m_statusLabel);

        mainLayout->setStretchFactor(mainSplitter, 1);
        mainLayout->setStretchFactor(m_progressBar, 0);
        mainLayout->setStretchFactor(m_statusLabel, 0);

        central->setLayout(mainLayout);

        setWindowTitle("ext4recover");
        resize(1200, 800);
    }

    void setupConnections() {
        connect(m_refreshFsBtn, &QPushButton::clicked, this, &MainWindow::refreshFsList);
        connect(m_fsCombo, QOverload<int>::of(&QComboBox::activated), this, &MainWindow::onFsActivated);
        connect(m_searchButton, &QPushButton::clicked, this, &MainWindow::onSearchClicked);
        connect(m_searchEdit, &QLineEdit::returnPressed, this, &MainWindow::onSearchClicked);
        connect(m_startButton, &QPushButton::clicked, this, &MainWindow::onStartClicked);
        connect(m_stopButton, &QPushButton::clicked, this, &MainWindow::onStopClicked);
        connect(m_tableView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onTableSelectionChanged);
        connect(m_tableView, &QTableView::customContextMenuRequested, this, &MainWindow::onTableContextMenuRequested);
    }

    // Populate combo with device nodes that have ext4 mounts (device only)
    void populateFsCombo() {
        m_fsCombo->clear();
        m_fsCombo->addItem("Choose device (ext4) or image...", QString());

        QFile f("/proc/mounts");
        QSet<QString> devices;
        QHash<QString, QString> devToMount;
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&f);
            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList parts = line.split(' ', Qt::SkipEmptyParts);
                if (parts.size() < 3) continue;
                QString dev = parts.at(0);
                QString mnt = parts.at(1);
                QString fstype = parts.at(2);
                if (fstype == "ext4") {
                    if (dev.startsWith("/dev/")) {
                        devices.insert(dev);
                        if (!devToMount.contains(dev)) devToMount.insert(dev, mnt);
                    }
                }
            }
            f.close();
        }

        QList<QString> devList = devices.values();
        std::sort(devList.begin(), devList.end());
        for (const QString &d : devList) {
            QString mnt = devToMount.value(d, QString());
            QString data = d + "|" + mnt;
            m_fsCombo->addItem(d, data);
        }

        m_fsCombo->addItem("Select device/image file...", QString("__choose_file__"));
    }

    // Walk mountpoint and build inode->attributes map
    void buildInodeMapForMountpoint(const QString &mountpoint) {
        m_inodeMap.clear();
        m_selectedMountpoint = mountpoint;
        if (mountpoint.isEmpty()) return;

        QDirIterator it(mountpoint, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString path = it.next();
            struct stat st;
            if (stat(path.toLocal8Bit().constData(), &st) == 0) {
                unsigned long long ino = (unsigned long long)st.st_ino;
                FileAttrs fa;
                fa.ino = ino;
                fa.size = (unsigned long long)st.st_size;
                fa.ctime = st.st_ctime;
                fa.mtime = st.st_mtime;
                fa.path = path.toUtf8().toStdString();
                m_inodeMap[ino] = fa;
            }
        }
    }

    void startScanForCurrentSelection() {
        int idx = m_fsCombo->currentIndex();
        if (idx < 0) return;
        QVariant data = m_fsCombo->itemData(idx);
        QString device;
        QString mountpoint;
        if (data.isValid()) {
            QString v = data.toString();
            if (v == "__choose_file__") {
                device = QFileDialog::getOpenFileName(this, "Select device or image to scan", QString(), "All files (*)");
                if (device.isEmpty()) return;
            } else if (v.contains("|")) {
                device = v.section('|', 0, 0);
                mountpoint = v.section('|', 1, 1);
            } else {
                device = v;
            }
        } else {
            device = m_fsCombo->currentText();
        }
        if (device.isEmpty()) {
            QMessageBox::warning(this, "No device", "Please select a device or image to scan.");
            return;
        }

        // If mountpoint known, build inode map
        if (!mountpoint.isEmpty()) {
            m_statusLabel->setText(QString("Building inode map for %1 ...").arg(mountpoint));
            qApp->processEvents();
            buildInodeMapForMountpoint(mountpoint);
            m_statusLabel->setText(QString("Built inode map (%1 entries)").arg(m_inodeMap.size()));
        } else {
            m_inodeMap.clear();
            m_selectedMountpoint.clear();
        }

        // clear previous results
        m_model->removeRows(0, m_model->rowCount());
        m_seenKeys.clear();
        m_progressBar->setValue(0);

        // create scanner and thread
        stopScanner();

        m_scanner = new Scanner;
        m_scanThread = new QThread(this);
        m_scanner->moveToThread(m_scanThread);

        connect(m_scanThread, &QThread::started, [this, device]() { m_scanner->startScan(device); });
        connect(m_scanner, &Scanner::entryFound, this, &MainWindow::onEntryFound);
        connect(m_scanner, &Scanner::progress, this, &MainWindow::onProgress);
        connect(m_scanner, &Scanner::finished, this, &MainWindow::onScanFinished);
        connect(m_scanner, &Scanner::logMessage, this, [this](const QString &s){ m_statusLabel->setText(s); });
        connect(m_scanner, &Scanner::finished, m_scanThread, &QThread::quit);
        connect(m_scanThread, &QThread::finished, m_scanner, &QObject::deleteLater);
        connect(m_scanThread, &QThread::finished, m_scanThread, &QObject::deleteLater);

        m_scanThread->start();
        m_startButton->setEnabled(false);
        m_stopButton->setEnabled(true);
        m_statusLabel->setText(QString("Scanning %1 ...").arg(device));
    }

    void stopScanner() {
        if (m_scanner) {
            m_scanner->requestStop();
        }
        if (m_scanThread) {
            m_scanThread->quit();
            m_scanThread->wait(2000);
            m_scanThread = nullptr;
        }
        m_scanner = nullptr;
        m_startButton->setEnabled(true);
        m_stopButton->setEnabled(false);
        m_statusLabel->setText("Stopped");
    }
};

#include "ext4recover.moc"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    MainWindow w;
    w.show();

    return app.exec();
}

#include <QObject>
