// cloneprj.cpp
// Single-file helper to add "Clone Project" to the Projects tree context menu
// No coreplugin / extensionsystem / projectexplorer headers required.
// Call ClonePrj::install(mainWindow) from your plugin initialize().

#include <QtCore>
#include <QtWidgets>
#include <QProcess>

class ClonePrj : public QObject {
    Q_OBJECT
public:
    // Install the context-menu action into Qt Creator's Projects tree.
    // mainWindow: pointer to Qt Creator main window (e.g., ICore::mainWindow()).
    static void install(QWidget *mainWindow) {
        if (!mainWindow) return;
        // Create a single shared instance that lives as a child of mainWindow
        // so it is cleaned up automatically when Qt Creator exits.
        ClonePrj *inst = new ClonePrj(mainWindow);
        inst->setupForMainWindow(mainWindow);
    }

private:
    explicit ClonePrj(QWidget *parent = nullptr) : QObject(parent) {}

    // Try a few common object names for the Projects tree and attach context action.
    void setupForMainWindow(QWidget *mainWindow) {
        // Delay a bit to allow Qt Creator to finish constructing UI
        QTimer::singleShot(500, this, [this, mainWindow]() {
            QTreeView *projTree = findProjectTree(mainWindow);
            if (!projTree) {
                // If not found, try again a couple times
                for (int i = 0; i < 5 && !projTree; ++i) {
                    QThread::msleep(100);
                    projTree = findProjectTree(mainWindow);
                }
            }
            if (!projTree) {
                qWarning() << "ClonePrj: Projects tree not found. Context action not installed.";
                return;
            }

            QAction *cloneAction = new QAction(tr("Clone Project"), this);
            connect(cloneAction, &QAction::triggered, this, [this, projTree]() {
                onCloneTriggered(projTree);
            });

            // Add action to the tree's context menu by intercepting contextMenuEvent
            projTree->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(projTree, &QWidget::customContextMenuRequested, this,
                    [this, projTree, cloneAction](const QPoint &pt) {
                QModelIndex idx = projTree->indexAt(pt);
                if (!idx.isValid()) return;
                QFileSystemModel *fsModel = qobject_cast<QFileSystemModel*>(projTree->model());
                QString path;
                if (fsModel) {
                    path = fsModel->filePath(idx);
                } else {
                    // fallback: try to read "filePath" role or data
                    QVariant v = idx.data(Qt::UserRole + 1);
                    if (v.isValid() && v.type() == QVariant::String) path = v.toString();
                    if (path.isEmpty()) path = idx.data(Qt::DisplayRole).toString();
                }
                QFileInfo fi(path);
                if (!fi.exists() || !fi.isDir()) return; // only for directories

                QMenu menu;
                menu.addAction(cloneAction);
                // show menu and map action to the selected index by temporarily storing it
                m_lastSelectedPath = path;
                menu.exec(projTree->viewport()->mapToGlobal(pt));
                m_lastSelectedPath.clear();
            });
        });
    }

    // Heuristic: search for a QTreeView that looks like the Projects tree.
    QTreeView* findProjectTree(QWidget *root) {
        // Common object names used by Qt Creator may vary by version.
        // Try several likely names and fall back to scanning for a QTreeView
        // that contains "Projects" in its accessibleName or windowTitle.
        const QStringList names = {
            "ProjectTree", "projectTree", "projectsTree", "ProjectExplorerTree"
        };
        for (const QString &n : names) {
            QTreeView *tv = root->findChild<QTreeView*>(n, Qt::FindChildrenRecursively);
            if (tv) return tv;
        }

        // Scan all QTreeView children and pick the one that looks like Projects
        QList<QTreeView*> all = root->findChildren<QTreeView*>(QString(), Qt::FindChildrenRecursively);
        for (QTreeView *tv : all) {
            QString an = tv->accessibleName().toLower();
            QString tt = tv->toolTip().toLower();
            QString wn = tv->windowTitle().toLower();
            if (an.contains("project") || tt.contains("project") || wn.contains("project")) return tv;
        }

        // As a last resort, return the first QTreeView (risky)
        if (!all.isEmpty()) return all.first();
        return nullptr;
    }

    void onCloneTriggered(QTreeView *projTree) {
        QString srcPath = m_lastSelectedPath;
        if (srcPath.isEmpty()) {
            // Try to get current index
            QModelIndex idx = projTree->currentIndex();
            if (!idx.isValid()) return;
            QFileSystemModel *fsModel = qobject_cast<QFileSystemModel*>(projTree->model());
            if (fsModel) srcPath = fsModel->filePath(idx);
            else srcPath = idx.data(Qt::DisplayRole).toString();
        }

        if (srcPath.isEmpty()) {
            QMessageBox::warning(nullptr, tr("Clone Project"), tr("No folder selected."));
            return;
        }

        QDir srcDir(srcPath);
        if (!srcDir.exists()) {
            QMessageBox::warning(nullptr, tr("Clone Project"), tr("Selected folder does not exist."));
            return;
        }

        QDir parent = srcDir;
        if (!parent.cdUp()) {
            QMessageBox::warning(nullptr, tr("Clone Project"), tr("Cannot determine parent folder."));
            return;
        }

        QString baseName = srcDir.dirName();
        QString newName = nextAvailableName(parent, baseName);
        QString dstPath = parent.filePath(newName);

        bool ok = copyRecursively(srcDir, QDir(dstPath));
        if (!ok) {
            QMessageBox::critical(nullptr, tr("Clone Project"), tr("Failed to copy project to:\n%1").arg(dstPath));
            return;
        }

        // Try to find a .pro or .pri file in the new folder
        QString proFile;
        QDir d(dstPath);
        QStringList proFiles = d.entryList(QStringList() << "*.pro" << "*.pri", QDir::Files | QDir::NoDotAndDotDot);
        if (!proFiles.isEmpty()) proFile = d.filePath(proFiles.first());

        // Inform the user and offer to open the new project in a new Qt Creator instance
        QString msg = tr("Project cloned to:\n%1").arg(dstPath);
        if (!proFile.isEmpty()) msg += tr("\n\nFound project file: %1\nQt Creator will be launched to open it.").arg(proFile);
        QMessageBox::information(nullptr, tr("Clone Project"), msg);

        if (!proFile.isEmpty()) {
            // Attempt to open the cloned project in a new Qt Creator instance.
            // This uses the system 'qtcreator' executable on PATH. If not available,
            // the user can open the project manually.
            QProcess::startDetached("qtcreator", QStringList() << proFile);
        }

        // Try to refresh the Projects tree view (best-effort)
        // If the tree uses QFileSystemModel, we can try to refresh its parent directory.
        QFileSystemModel *fsModel = qobject_cast<QFileSystemModel*>(projTree->model());
        if (fsModel) {
            QModelIndex parentIndex = fsModel->index(parent.absolutePath());
           // fsModel->refresh(parentIndex);
        }
    }

    // Recursively copy directory contents (including hidden files)
    static bool copyRecursively(const QDir &srcDir, const QDir &dstDir) {
        if (!srcDir.exists()) return false;
        if (!dstDir.exists() && !QDir().mkpath(dstDir.absolutePath())) return false;

        QFileInfoList entries = srcDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries | QDir::Hidden | QDir::System);
        for (const QFileInfo &entry : entries) {
            QString srcPath = entry.absoluteFilePath();
            QString dstPath = dstDir.filePath(entry.fileName());

            if (entry.isDir()) {
                QDir nextDst(dstPath);
                if (!copyRecursively(QDir(srcPath), nextDst)) return false;
            } else {
                // Ensure destination directory exists
                QFileInfo dstInfo(dstPath);
                QDir().mkpath(dstInfo.path());

                // Try QFile::copy first
                if (!QFile::copy(srcPath, dstPath)) {
                    QFile in(srcPath);
                    QFile out(dstPath);
                    if (!in.open(QIODevice::ReadOnly) || !out.open(QIODevice::WriteOnly)) {
                        return false;
                    }
                    out.write(in.readAll());
                    in.close();
                    out.close();
                }
                QFile::setPermissions(dstPath, entry.permissions());
            }
        }
        return true;
    }

    static QString nextAvailableName(const QDir &parent, const QString &baseName) {
        if (!parent.exists(baseName)) return baseName;
        int n = 1;
        QString candidate;
        do {
            candidate = QString("%1_%2").arg(baseName).arg(n++);
        } while (parent.exists(candidate));
        return candidate;
    }

    // last selected path during context menu invocation
    QString m_lastSelectedPath;
};

// No moc file required beyond this include
#include "main.moc"

