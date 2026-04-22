// ide.cpp
#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QDockWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>
#include <QPainter>
#include <QMouseEvent>
#include <QSplitter>
#include <QListWidget>
#include <QHeaderView>
#include <QTimer>
#include <QTreeWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QSet>
#include <QMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QInputDialog>
#include <QDir>
#include <QKeyEvent>
#include <QVector>
#include <QFile>
#include <QDesktopServices>
#include <QUrl>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QDirIterator>
#include <QTextStream>
#include <QPlainTextEdit>

    // =====================
    // Simple C++ Highlighter
    // =====================
    class CppHighlighter : public QSyntaxHighlighter {
public:
    CppHighlighter(QTextDocument *parent=nullptr):QSyntaxHighlighter(parent){
        QTextCharFormat kw; kw.setForeground(Qt::blue); kw.setFontWeight(QFont::Bold);
        QStringList kws={"int","float","double","return","if","else","for","while","class","public","private","protected","include","void","const","auto","struct","namespace","switch","case","break","continue","template","typename","QString"};
        for(auto&s:kws) rules.push_back({QRegularExpression("\\b"+s+"\\b"),kw});

        QTextCharFormat str; str.setForeground(Qt::darkGreen);
        rules.push_back({QRegularExpression("\".*\""),str});

        QTextCharFormat com; com.setForeground(Qt::gray);
        rules.push_back({QRegularExpression("//[^\n]*"),com});
    }
protected:
    void highlightBlock(const QString &text) override{
        for(auto&r:rules){
            auto it=r.rx.globalMatch(text);
            while(it.hasNext()){
                auto m=it.next();
                setFormat(m.capturedStart(),m.capturedLength(),r.fmt);
            }
        }
    }
private:
    struct R{QRegularExpression rx; QTextCharFormat fmt;};
    QVector<R> rules;
};

// ============================================================
// SYMBOL + AST (CDT Indexer) - minimal
// ============================================================
struct Symbol { QString name; QString type; int line; QString file; };

class CDTIndexer : public QObject {
    Q_OBJECT
public:
    QMap<QString, Symbol> symbols;
    QMap<QString, QVector<Symbol>> fileSymbols;
    QStringList outline;

    void index(const QString &file, const QString &code){
        fileSymbols[file].clear();
        outline.clear();
        QStringList lines = code.split('\n');
        QRegularExpression func(R"((\w[\w0-9_]*)\s*\([^)]*\)\s*\{)");
        QRegularExpression var(R"((int|float|double|auto|QString|void)\s+(\w+))");
        for(int i=0;i<lines.size();++i){
            auto f = func.match(lines[i]);
            if(f.hasMatch()){
                Symbol s{f.captured(1),"function",i+1,file};
                symbols[s.name]=s;
                fileSymbols[file].push_back(s);
                outline.push_back("fn: "+s.name);
            }
            auto v = var.match(lines[i]);
            if(v.hasMatch()){
                Symbol s{v.captured(2),v.captured(1),i+1,file};
                symbols[s.name]=s;
                fileSymbols[file].push_back(s);
                outline.push_back("var: "+s.name);
            }
        }
    }

    Symbol find(const QString &name){ return symbols.value(name); }
    QVector<Symbol> all(){ return symbols.values().toVector(); }
};

// =====================
// FileAndDirectoryListBoxTreeView: tree + list + preview + context menus
// =====================
class FileAndDirectoryListBoxTreeView : public QWidget {
    Q_OBJECT
public:
    FileAndDirectoryListBoxTreeView(QWidget *parent = nullptr) : QWidget(parent) {
        fsModel = new QFileSystemModel(this);
        fsModel->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::AllEntries);
        fsModel->setRootPath("");

        dirTree = new QTreeView(this);
        dirTree->setModel(fsModel);
        for(int c=1;c<fsModel->columnCount();++c) dirTree->hideColumn(c);
        dirTree->setHeaderHidden(true);
        dirTree->setAnimated(true);
        dirTree->setExpandsOnDoubleClick(false);
        dirTree->setContextMenuPolicy(Qt::CustomContextMenu);

        fileList = new QListWidget(this);
        fileList->setSelectionMode(QAbstractItemView::SingleSelection);
        fileList->setContextMenuPolicy(Qt::CustomContextMenu);

        preview = new QPlainTextEdit(this);
        preview->setReadOnly(true);
        preview->setLineWrapMode(QPlainTextEdit::NoWrap);

        auto rightSplitter = new QSplitter(Qt::Vertical, this);
        rightSplitter->addWidget(fileList);
        rightSplitter->addWidget(preview);
        rightSplitter->setStretchFactor(0, 3);
        rightSplitter->setStretchFactor(1, 2);

        auto splitter = new QSplitter(Qt::Horizontal, this);
        splitter->addWidget(dirTree);
        splitter->addWidget(rightSplitter);
        splitter->setStretchFactor(0, 1);
        splitter->setStretchFactor(1, 2);

        auto lay = new QHBoxLayout(this);
        lay->setContentsMargins(0,0,0,0);
        lay->addWidget(splitter);
        setLayout(lay);

        connect(dirTree, &QTreeView::clicked, this, &FileAndDirectoryListBoxTreeView::onDirClicked);
        connect(dirTree, &QTreeView::customContextMenuRequested, this, &FileAndDirectoryListBoxTreeView::onDirContextMenu);
        connect(fileList, &QListWidget::itemDoubleClicked, this, &FileAndDirectoryListBoxTreeView::onFileDoubleClicked);
        connect(fileList, &QListWidget::itemClicked, this, &FileAndDirectoryListBoxTreeView::onFileClicked);
        connect(fileList, &QListWidget::customContextMenuRequested, this, &FileAndDirectoryListBoxTreeView::onFileContextMenu);
    }

    void setRootPath(const QString &path){
        QModelIndex idx = fsModel->index(path);
        if(idx.isValid()){
            dirTree->setRootIndex(idx);
            populateFileList(path);
            currentRoot = path;
        }
    }

    void setSuffixFilter(const QStringList &suffixes){ suffixFilter = suffixes; }

signals:
    void fileActivated(const QString &filePath);
    void directoryActivated(const QString &dirPath);
    void directoryContextSetAsProject(const QString &dirPath);
    void filePreviewRequested(const QString &filePath);
    void fileContextRenameRequested(const QString &oldPath);
    void fileContextDeleteRequested(const QString &path);
    void fileContextRevealRequested(const QString &path);
    void fileContextSwitchHeaderSourceRequested(const QString &path);

public slots:
    void refresh(){
        QString rp = fsModel->rootPath();
        fsModel->setRootPath("");
        fsModel->setRootPath(rp);
        QModelIndex idx = dirTree->currentIndex();
        if(idx.isValid()){
            QString path = fsModel->filePath(idx);
            populateFileList(path);
        } else if(!currentRoot.isEmpty()){
            populateFileList(currentRoot);
        }
    }

private slots:
    void onDirClicked(const QModelIndex &idx){
        QString path = fsModel->filePath(idx);
        populateFileList(path);
        emit directoryActivated(path);
    }

    void onDirContextMenu(const QPoint &pt){
        QModelIndex idx = dirTree->indexAt(pt);
        if(!idx.isValid()) return;
        QString path = fsModel->filePath(idx);
        QMenu m;
        QAction *setProject = m.addAction("Als Projekt setzen");
        QAction *refresh = m.addAction("Aktualisieren");
        QAction *act = m.exec(dirTree->viewport()->mapToGlobal(pt));
        if(act==setProject) emit directoryContextSetAsProject(path);
        else if(act==refresh) populateFileList(path);
    }

    void onFileDoubleClicked(QListWidgetItem *it){
        QString p = it->data(Qt::UserRole).toString();
        emit fileActivated(p);
    }

    void onFileClicked(QListWidgetItem *it){
        QString p = it->data(Qt::UserRole).toString();
        emit filePreviewRequested(p);
        showPreview(p);
    }

    void onFileContextMenu(const QPoint &pt){
        auto it = fileList->itemAt(pt);
        if(!it) return;
        QString path = it->data(Qt::UserRole).toString();
        QMenu m;
        QAction *open = m.addAction("Öffnen");
        QAction *rename = m.addAction("Umbenennen");
        QAction *del = m.addAction("Löschen");
        QAction *reveal = m.addAction("Im Dateimanager zeigen");
        QAction *switchHS = m.addAction("Header/Source wechseln");
        QAction *act = m.exec(fileList->mapToGlobal(pt));
        if(act==open) emit fileActivated(path);
        else if(act==rename) emit fileContextRenameRequested(path);
        else if(act==del) emit fileContextDeleteRequested(path);
        else if(act==reveal) emit fileContextRevealRequested(path);
        else if(act==switchHS) emit fileContextSwitchHeaderSourceRequested(path);
    }

private:
    QFileSystemModel *fsModel;
    QTreeView *dirTree;
    QListWidget *fileList;
    QPlainTextEdit *preview;
    QStringList suffixFilter;
    QString currentRoot;

    void populateFileList(const QString &dirPath){
        fileList->clear();
        QDir d(dirPath);
        if(!d.exists()) return;
        QFileInfoList entries = d.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name);
        for(const QFileInfo &fi : entries){
            if(fi.isFile() && !suffixFilter.isEmpty()){
                QString suf = fi.suffix();
                if(!suffixFilter.contains(suf, Qt::CaseInsensitive)) continue;
            }
            QListWidgetItem *it = new QListWidgetItem();
            if(fi.isDir()){
                it->setText(QString("[Dir] %1").arg(fi.fileName()));
                it->setForeground(Qt::darkBlue);
            } else {
                it->setText(fi.fileName());
            }
            it->setData(Qt::UserRole, fi.absoluteFilePath());
            fileList->addItem(it);
        }
    }

    void showPreview(const QString &path){
        QFile f(path);
        if(!f.open(QIODevice::ReadOnly)){
            preview->setPlainText(QString("Kann %1 nicht öffnen").arg(path));
            return;
        }
        QByteArray data = f.read(1024*64); // preview up to 64KB
        preview->setPlainText(QString::fromUtf8(data));
    }
};

// =====================
// CodeEditor (with indexer + breakpoints)
// =====================
class CodeEditor : public QPlainTextEdit {
    Q_OBJECT
public:
    QString filePath;
    QSet<int> breakpoints;
    int currentDebugLine = -1;
    CDTIndexer *indexer = nullptr;

    CodeEditor(const QString &path=""):filePath(path){
        new CppHighlighter(document());
        setLineWrapMode(NoWrap);
        if(!path.isEmpty()) load(path);
    }

    void setIndexer(CDTIndexer *i){ indexer = i; if(!filePath.isEmpty()) indexer->index(filePath, toPlainText()); }

    void load(const QString &p){
        QFile f(p);
        if(f.open(QIODevice::ReadOnly)){
            setPlainText(f.readAll());
            filePath=p;
            if(indexer) indexer->index(filePath, toPlainText());
        }
    }

    void save(){
        if(filePath.isEmpty()) return;
        QFile f(filePath);
        if(f.open(QIODevice::WriteOnly))
            f.write(toPlainText().toUtf8());
        if(indexer) indexer->index(filePath, toPlainText());
    }

signals:
    void breakpointToggled(int line, bool set);

protected:
    void keyPressEvent(QKeyEvent *e) override {
        QPlainTextEdit::keyPressEvent(e);
        if(indexer && !filePath.isEmpty()) indexer->index(filePath, toPlainText());
        if(!e->text().isEmpty() && e->text().endsWith('.')){
            if(!indexer) return;
            QStringList suggestions;
            for(auto &s : indexer->all()) suggestions << s.name;
            if(suggestions.isEmpty()) return;
            bool ok=false;
            QString item = QInputDialog::getItem(this,"Autocomplete","Symbol:",suggestions,0,false,&ok);
            if(ok && !item.isEmpty()) insertPlainText(item);
        }
    }

    void mousePressEvent(QMouseEvent *e) override {
        if(e->button()==Qt::LeftButton && e->pos().x()<20){
            int line = cursorForPosition(e->pos()).blockNumber()+1;
            bool set=false;
            if(breakpoints.contains(line)) { breakpoints.remove(line); set=false; }
            else { breakpoints.insert(line); set=true; }
            viewport()->update();
            emit breakpointToggled(line, set);
            return;
        }
        QPlainTextEdit::mousePressEvent(e);
    }

    void paintEvent(QPaintEvent *e) override {
        QPlainTextEdit::paintEvent(e);
        QPainter p(viewport());
        for(int bp:breakpoints){
            QTextBlock b=document()->findBlockByNumber(bp-1);
            QRect r=blockBoundingGeometry(b).translated(contentOffset()).toRect();
            p.setBrush(Qt::red);
            p.drawEllipse(2,r.top()+2,8,8);
        }
        if(currentDebugLine>0){
            QTextBlock b=document()->findBlockByNumber(currentDebugLine-1);
            QRect r=blockBoundingGeometry(b).translated(contentOffset()).toRect();
            p.fillRect(r, QColor(255,255,0,80));
        }
    }
};

// =====================
// Outline & Navigator (minimal)
// =====================
class Outline : public QListWidget { Q_OBJECT public: CDTIndexer *indexer=nullptr; void refresh(){ clear(); if(!indexer) return; for(auto &s:indexer->all()) addItem(s.type+": "+s.name); } };
class Navigator : public QListWidget { Q_OBJECT public: void showRefs(const QVector<Symbol>&syms){ clear(); for(auto &s:syms) addItem(s.file+":"+QString::number(s.line)+" -> "+s.name); } };

// =====================
// GDB MI Controller (minimal)
// =====================
class GDB : public QObject {
    Q_OBJECT
public:
    QProcess proc;
    bool collectingBreakList = false;
    QString breakListBuffer;
    GDB(){
        connect(&proc,&QProcess::readyReadStandardOutput,this,[this](){
            QString out=proc.readAllStandardOutput();
            emit raw(out);
            emit consoleOutput(out);
            if(collectingBreakList){
                breakListBuffer += out;
                if(breakListBuffer.contains("\n^done") || breakListBuffer.contains("\r^done") || breakListBuffer.contains("^done")){
                    collectingBreakList = false;
                    emit breakListReady(breakListBuffer);
                    breakListBuffer.clear();
                }
            }
            if(out.contains("*stopped")){
                QRegularExpression re("file=\"([^\"]+)\",line=\"(\\d+)\"");
                auto m=re.match(out);
                if(m.hasMatch()) emit stopped(m.captured(1), m.captured(2).toInt(), 0);
                send("-stack-list-variables --simple-values");
            }
            QRegularExpression reBkpt(R"(bkpt=\{[^}]*number=\"(\d+)\"[^}]*file=\"([^\"]+)\"[^}]*line=\"(\d+)\"[^}]*\})");
            auto it = reBkpt.globalMatch(out);
            while(it.hasNext()){
                auto m = it.next();
                int id = m.captured(1).toInt();
                QString file = m.captured(2);
                int line = m.captured(3).toInt();
                emit breakpointSet(file, line, id);
            }
            QRegularExpression reDel(R"(=breakpoint-deleted,id=\"(\d+)\")");
            auto it2 = reDel.globalMatch(out);
            while(it2.hasNext()){
                auto m = it2.next();
                int id = m.captured(1).toInt();
                emit breakpointDeleted(id);
            }
            if(out.contains("variables=[")) emit locals(out);
        });
        connect(&proc,&QProcess::readyReadStandardError,this,[this](){
            QString err=proc.readAllStandardError();
            emit raw(err);
            emit consoleOutput(err);
        });
        connect(&proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, [this](int code, QProcess::ExitStatus st){ Q_UNUSED(code); Q_UNUSED(st); emit exited(); });
    }
    void start(){ if(proc.state()!=QProcess::NotRunning) proc.kill(); proc.start("gdb", {"--interpreter=mi2"}); }
    void send(QString cmd){ if(proc.state()==QProcess::NotRunning){ emit error("GDB not running"); return; } proc.write((cmd+"\n").toUtf8()); }
    void requestBreakList(){ if(proc.state()==QProcess::NotRunning){ emit error("GDB not running (cannot request break-list)"); return; } collectingBreakList = true; breakListBuffer.clear(); proc.write(QByteArray("-break-list\n")); }
signals:
    void raw(QString); void stopped(QString file,int line,int thread); void locals(QString); void consoleOutput(QString);
    void breakpointSet(QString file,int line,int id); void breakpointDeleted(int id); void breakListReady(QString);
    void error(QString); void exited();
};

// =====================
// Build system detection + IDE with view switching
// =====================
enum class BuildSystem { None, Make, Autotools, CMake, QMake, QMake6, Meson, Bazel, Other };

static QString buildSystemToString(BuildSystem b){
    switch(b){
    case BuildSystem::Make: return "Make";
    case BuildSystem::Autotools: return "Autotools";
    case BuildSystem::CMake: return "CMake";
    case BuildSystem::QMake: return "qmake";
    case BuildSystem::QMake6: return "qmake6";
    case BuildSystem::Meson: return "Meson";
    case BuildSystem::Bazel: return "Bazel";
    default: return "None";
    }
}

enum class ViewMode { Code, Debugger, All };

class IDE : public QMainWindow {
    Q_OBJECT
public:
    IDE(){
        resize(1800,1000);
        setWindowTitle("Hybrid IDE");

        fsModel=new QFileSystemModel();
        fsModel->setRootPath("");

        projView = new FileAndDirectoryListBoxTreeView();
        projView->setSuffixFilter(QStringList() << "cpp" << "h" << "hpp" << "c" << "txt" << "md");

        tabs=new QTabWidget();

        console=new QPlainTextEdit(); console->setReadOnly(true);
        gdbConsole=new QPlainTextEdit(); gdbConsole->setReadOnly(true);
        localsView=new QListWidget();

        framesView = new QListWidget();
        varsTree = new QTreeWidget();
        varsTree->setHeaderLabels({"Name","Value","Type"});

        outline=new Outline();
        outline->indexer=&indexer;

        nav=new Navigator();

        problems=new QListWidget();
        search=new QListWidget();

        gdb=new GDB();

        loadPersistedBreakpoints();

        setupUI();
        setupConnections();

        QTimer *t=new QTimer(this);
        connect(t,&QTimer::timeout,this,[this](){ outline->refresh(); });
        t->start(500);

        // default view
        setViewMode(ViewMode::Code);
    }

    ~IDE(){ savePersistedBreakpoints(); }

private:
    QFileSystemModel *fsModel;
    FileAndDirectoryListBoxTreeView *projView;
    QTabWidget *tabs;
    QPlainTextEdit *console;
    QPlainTextEdit *gdbConsole;
    QListWidget *localsView;
    QListWidget *framesView;
    QTreeWidget *varsTree;

    CDTIndexer indexer;
    Outline *outline;
    Navigator *nav;
    QListWidget *problems;
    QListWidget *search;

    GDB *gdb;

    QMap<CodeEditor*, QMap<int,int>> bpMap;
    QMap<QString, QSet<int>> savedBps;

    // docks (kept for view switching)
    QDockWidget *dockExplorer=nullptr;
    QDockWidget *dockOutput=nullptr;
    QDockWidget *dockDebug=nullptr;
    QDockWidget *dockOutline=nullptr;
    QDockWidget *dockNav=nullptr;
    QDockWidget *dockProblems=nullptr;
    QDockWidget *dockSearch=nullptr;

    // project root and detected build system
    QString currentProjectRoot;
    BuildSystem currentBuildSystem = BuildSystem::None;

    ViewMode currentView = ViewMode::All;

    CodeEditor* currentEditor(){ return dynamic_cast<CodeEditor*>(tabs->currentWidget()); }

    QString persistedFilePath(){
        QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(dir);
        return dir + "/simpleqtide_breakpoints.json";
    }

    void loadPersistedBreakpoints(){
        QString p = persistedFilePath();
        QFile f(p);
        if(!f.exists()) return;
        if(!f.open(QIODevice::ReadOnly)) return;
        QByteArray data = f.readAll();
        f.close();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if(!doc.isObject()) return;
        QJsonObject obj = doc.object();
        for(auto it = obj.begin(); it != obj.end(); ++it){
            QString file = it.key();
            QJsonArray arr = it.value().toArray();
            QSet<int> s;
            for(auto v: arr) s.insert(v.toInt());
            savedBps[file] = s;
        }
    }

    void savePersistedBreakpoints(){
        QJsonObject obj;
        for(auto it = bpMap.begin(); it != bpMap.end(); ++it){
            CodeEditor* ed = it.key();
            if(!ed) continue;
            QString file = ed->filePath;
            if(file.isEmpty()) continue;
            QSet<int> set = savedBps.value(file);
            for(int line: ed->breakpoints) set.insert(line);
            savedBps[file] = set;
        }
        for(auto it = savedBps.begin(); it != savedBps.end(); ++it){
            QJsonArray arr;
            for(int line: it.value()) arr.append(line);
            obj.insert(it.key(), arr);
        }
        QJsonDocument doc(obj);
        QString p = persistedFilePath();
        QFile f(p);
        if(f.open(QIODevice::WriteOnly)){ f.write(doc.toJson()); f.close(); }
    }

    void setupUI(){
        setCentralWidget(tabs);

        projView->setRootPath(QDir::currentPath());

        dockExplorer = new QDockWidget("Explorer");
        dockExplorer->setWidget(projView);
        addDockWidget(Qt::LeftDockWidgetArea, dockExplorer);

        auto bottom=new QTabWidget();
        bottom->addTab(console,"Program Output");
        bottom->addTab(gdbConsole,"GDB");

        dockOutput = new QDockWidget("Output");
        dockOutput->setWidget(bottom);
        addDockWidget(Qt::BottomDockWidgetArea,dockOutput);

        auto rightTabs = new QTabWidget();
        rightTabs->addTab(localsView, "Locals");
        rightTabs->addTab(framesView, "Frames");
        rightTabs->addTab(varsTree, "Variables");

        dockDebug = new QDockWidget("Debug");
        dockDebug->setWidget(rightTabs);
        addDockWidget(Qt::RightDockWidgetArea,dockDebug);

        dockOutline = new QDockWidget("Outline");
        dockOutline->setWidget(outline);
        addDockWidget(Qt::RightDockWidgetArea,dockOutline);

        dockNav = new QDockWidget("Navigation");
        dockNav->setWidget(nav);
        addDockWidget(Qt::RightDockWidgetArea,dockNav);

        dockProblems = new QDockWidget("Problems");
        dockProblems->setWidget(problems);
        addDockWidget(Qt::BottomDockWidgetArea,dockProblems);

        dockSearch = new QDockWidget("Search");
        dockSearch->setWidget(search);
        addDockWidget(Qt::BottomDockWidgetArea,dockSearch);

        // Menüs
        auto file=menuBar()->addMenu("Datei");
        file->addAction("Öffnen...",this,&IDE::openFile);
        file->addAction("Ordner öffnen...",this,&IDE::openFolder);
        file->addAction("Speichern",this,&IDE::save);
        file->addSeparator();
        file->addAction("Neue C Anwendung", this, &IDE::createNewCApplication);
        file->addAction("Neue C++ Anwendung", this, &IDE::createNewCppApplication);
        file->addAction("Neue Qt Widgets Anwendung", this, &IDE::createNewQtWidgetsApplication);

        auto viewMenu = menuBar()->addMenu("Ansicht");
        viewMenu->addAction("Code Ansicht", this, [this](){ setViewMode(ViewMode::Code); });
        viewMenu->addAction("Debugger Ansicht", this, [this](){ setViewMode(ViewMode::Debugger); });
        viewMenu->addAction("Alle anzeigen", this, [this](){ setViewMode(ViewMode::All); });

        auto projectMenu = menuBar()->addMenu("Projekt");
        projectMenu->addAction("Projekt-Root setzen...", this, &IDE::setProjectRootDialog);
        projectMenu->addAction("Buildsystem erkennen", this, &IDE::detectAndShowBuildSystem);

        auto buildMenu = menuBar()->addMenu("Build");
        buildMenu->addAction("Konfigurieren", this, &IDE::configureProject);
        buildMenu->addAction("Build", this, &IDE::buildProject);
        buildMenu->addAction("Clean", this, &IDE::cleanProject);
        buildMenu->addAction("Custom Command...", this, &IDE::runCustomCommand);

        auto run=menuBar()->addMenu("Ausführen");
        run->addAction("Ausführen",this,&IDE::run);
        run->addAction("Debug",this,&IDE::debugStart);

        // Toolbar with build buttons and run/debug/new project buttons
        auto tb=addToolBar("Main");
        QAction *actNewC = tb->addAction("Neu C");
        QAction *actNewCpp = tb->addAction("Neu C++");
        QAction *actNewQt = tb->addAction("Neu Qt");
        QAction *actRun = tb->addAction("Ausführen");
        QAction *actDebug = tb->addAction("Debug");
        QAction *actBuild = tb->addAction("Build");
        QAction *actClean = tb->addAction("Clean");
        QAction *actConfig = tb->addAction("Konfig");
        QAction *actCustom = tb->addAction("Cmd...");
        connect(actNewC, &QAction::triggered, this, &IDE::createNewCApplication);
        connect(actNewCpp, &QAction::triggered, this, &IDE::createNewCppApplication);
        connect(actNewQt, &QAction::triggered, this, &IDE::createNewQtWidgetsApplication);
        connect(actRun, &QAction::triggered, this, &IDE::run);
        connect(actDebug, &QAction::triggered, this, &IDE::debugStart);
        connect(actBuild, &QAction::triggered, this, &IDE::buildProject);
        connect(actClean, &QAction::triggered, this, &IDE::cleanProject);
        connect(actConfig, &QAction::triggered, this, &IDE::configureProject);
        connect(actCustom, &QAction::triggered, this, &IDE::runCustomCommand);

        auto refreshAct = new QAction("Explorer aktualisieren", this);
        menuBar()->addAction(refreshAct);
        connect(refreshAct, &QAction::triggered, projView, &FileAndDirectoryListBoxTreeView::refresh);

        statusBar()->showMessage("Bereit");
    }

    void setupConnections(){
        connect(projView, &FileAndDirectoryListBoxTreeView::fileActivated, this, [this](const QString &path){
            if(QFileInfo(path).isFile()){
                for(int i=0;i<tabs->count();++i){
                    auto ed = dynamic_cast<CodeEditor*>(tabs->widget(i));
                    if(ed && ed->filePath == path){ tabs->setCurrentIndex(i); return; }
                }
                auto ed = createEditor(path);
                tabs->addTab(ed, QFileInfo(path).fileName());
                tabs->setCurrentWidget(ed);
            }
        });

        connect(projView, &FileAndDirectoryListBoxTreeView::directoryContextSetAsProject, this, [this](const QString &dir){
            setProjectRoot(dir);
        });

        connect(projView, &FileAndDirectoryListBoxTreeView::filePreviewRequested, this, [this](const QString &path){
            statusBar()->showMessage(QString("Vorschau: %1").arg(QFileInfo(path).fileName()), 2000);
        });

        connect(projView, &FileAndDirectoryListBoxTreeView::fileContextRenameRequested, this, [this](const QString &oldPath){
            bool ok=false;
            QString newName = QInputDialog::getText(this,"Umbenennen","Neuer Name:",QLineEdit::Normal,QFileInfo(oldPath).fileName(),&ok);
            if(!ok || newName.isEmpty()) return;
            QString newPath = QFileInfo(oldPath).absolutePath() + "/" + newName;
            if(QFile::rename(oldPath, newPath)){ projView->refresh(); statusBar()->showMessage("Umbenannt",2000); }
            else QMessageBox::warning(this,"Fehler","Umbenennen fehlgeschlagen.");
        });

        connect(projView, &FileAndDirectoryListBoxTreeView::fileContextDeleteRequested, this, [this](const QString &path){
            if(QMessageBox::question(this,"Löschen","Löschen "+path+"?")!=QMessageBox::Yes) return;
            if(QFileInfo(path).isDir()){ QDir dir(path); if(!dir.removeRecursively()) QMessageBox::warning(this,"Fehler","Löschen fehlgeschlagen."); }
            else { if(!QFile::remove(path)) QMessageBox::warning(this,"Fehler","Löschen fehlgeschlagen."); }
            projView->refresh();
        });

        connect(projView, &FileAndDirectoryListBoxTreeView::fileContextRevealRequested, this, [this](const QString &path){
            QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(path).absolutePath()));
        });

        connect(projView, &FileAndDirectoryListBoxTreeView::fileContextSwitchHeaderSourceRequested, this, [this](const QString &path){
            QString other = switchHeaderSource(path);
            if(other.isEmpty()){ QMessageBox::information(this,"Header/Source","Kein passendes Gegenstück gefunden."); return; }
            for(int i=0;i<tabs->count();++i){ auto ed = dynamic_cast<CodeEditor*>(tabs->widget(i)); if(ed && ed->filePath == other){ tabs->setCurrentIndex(i); return; } }
            auto ed = createEditor(other);
            tabs->addTab(ed, QFileInfo(other).fileName());
            tabs->setCurrentWidget(ed);
        });

        connect(gdb,&GDB::raw,this,[this](QString s){ gdbConsole->appendPlainText(s); });
        connect(gdb,&GDB::consoleOutput,this,[this](QString s){ gdbConsole->appendPlainText(s); });

        connect(gdb,&GDB::stopped,this,[this](QString file,int line,int thread){ openFileAtLine(file, line); });

        connect(gdb,&GDB::locals,this,[this](QString txt){
            localsView->clear();
            QRegularExpression re("name=\"([^\"]+)\",value=\"([^\"]*)\"");
            auto it=re.globalMatch(txt);
            while(it.hasNext()){ auto m=it.next(); localsView->addItem(m.captured(1)+" = "+m.captured(2)); }
        });

        connect(gdb,&GDB::breakpointSet,this,[this](QString file,int line,int id){
            for(int i=0;i<tabs->count();++i){
                auto ed=dynamic_cast<CodeEditor*>(tabs->widget(i));
                if(!ed) continue;
                if(ed->filePath == file || QFileInfo(ed->filePath).fileName() == QFileInfo(file).fileName()){
                    bpMap[ed][line] = id;
                    if(!ed->breakpoints.contains(line)) ed->breakpoints.insert(line);
                    ed->viewport()->update();
                    gdbConsole->appendPlainText(QString("Breakpoint %1 gesetzt bei %2:%3").arg(id).arg(file).arg(line));
                    savedBps[ed->filePath].insert(line);
                }
            }
        });

        connect(gdb,&GDB::breakpointDeleted,this,[this](int id){
            for(auto it = bpMap.begin(); it != bpMap.end(); ++it){
                CodeEditor* ed = it.key();
                auto &map = it.value();
                QList<int> toRemove;
                for(auto mit = map.begin(); mit != map.end(); ++mit) if(mit.value() == id) toRemove.append(mit.key());
                for(int line: toRemove){ map.remove(line); ed->breakpoints.remove(line); ed->viewport()->update(); if(savedBps.contains(ed->filePath)) savedBps[ed->filePath].remove(line); }
            }
        });

        connect(gdb, &GDB::breakListReady, this, [this](QString mi){
            QMap<QString, QMap<int,int>> listMap;
            QRegularExpression reBkpt(R"(bkpt=\{[^}]*number=\"(\d+)\"[^}]*file=\"([^\"]+)\"[^}]*line=\"(\d+)\"[^}]*\})");
            auto it = reBkpt.globalMatch(mi);
            while(it.hasNext()){ auto m = it.next(); int id = m.captured(1).toInt(); QString file = m.captured(2); int line = m.captured(3).toInt(); listMap[file][line] = id; }
            for(int i=0;i<tabs->count();++i){
                auto ed=dynamic_cast<CodeEditor*>(tabs->widget(i));
                if(!ed) continue;
                QString filePath = ed->filePath;
                QString base = QFileInfo(filePath).fileName();
                QList<int> savedLines = savedBps.value(filePath).values();
                for(int line : savedLines){
                    bool hasId = bpMap.contains(ed) && bpMap[ed].contains(line);
                    if(!hasId){
                        int id = -1;
                        if(listMap.contains(filePath) && listMap[filePath].contains(line)) id = listMap[filePath][line];
                        else {
                            for(auto lmIt = listMap.begin(); lmIt != listMap.end(); ++lmIt){
                                if(QFileInfo(lmIt.key()).fileName() == base && lmIt.value().contains(line)){ id = lmIt.value()[line]; break; }
                            }
                        }
                        if(id>0){ gdb->send(QString("-break-delete %1").arg(id)); savedBps[filePath].remove(line); }
                    }
                }
            }
        });
    }

    // ---------- view switching ----------
    void setViewMode(ViewMode m){
        currentView = m;
        switch(m){
        case ViewMode::Code:
            if(dockExplorer) dockExplorer->show();
            if(dockOutline) dockOutline->show();
            if(dockOutput) dockOutput->hide();
            if(dockDebug) dockDebug->hide();
            if(dockNav) dockNav->hide();
            if(dockProblems) dockProblems->hide();
            if(dockSearch) dockSearch->hide();
            statusBar()->showMessage("Ansicht: Code", 2000);
            break;
        case ViewMode::Debugger:
            if(dockExplorer) dockExplorer->hide();
            if(dockOutline) dockOutline->hide();
            if(dockOutput) dockOutput->show();
            if(dockDebug) dockDebug->show();
            if(dockNav) dockNav->show();
            if(dockProblems) dockProblems->show();
            if(dockSearch) dockSearch->show();
            statusBar()->showMessage("Ansicht: Debugger", 2000);
            break;
        case ViewMode::All:
        default:
            if(dockExplorer) dockExplorer->show();
            if(dockOutline) dockOutline->show();
            if(dockOutput) dockOutput->show();
            if(dockDebug) dockDebug->show();
            if(dockNav) dockNav->show();
            if(dockProblems) dockProblems->show();
            if(dockSearch) dockSearch->show();
            statusBar()->showMessage("Ansicht: Alle", 2000);
            break;
        }
    }

    // ---------- project root & build detection ----------
    void setProjectRoot(const QString &root){
        if(root.isEmpty() || !QFileInfo(root).isDir()) return;
        currentProjectRoot = QDir(root).absolutePath();
        projView->setRootPath(currentProjectRoot);
        detectBuildSystem();
        statusBar()->showMessage(QString("Projekt-Root: %1 — Build: %2").arg(currentProjectRoot).arg(buildSystemToString(currentBuildSystem)), 5000);
    }

    void setProjectRootDialog(){
        QString d = QFileDialog::getExistingDirectory(this, "Projekt-Root wählen", QDir::currentPath());
        if(!d.isEmpty()) setProjectRoot(d);
    }

    void detectAndShowBuildSystem(){ detectBuildSystem(); QMessageBox::information(this,"Buildsystem", QString("Erkannt: %1").arg(buildSystemToString(currentBuildSystem))); }

    void detectBuildSystem(){
        if(currentProjectRoot.isEmpty()){ currentBuildSystem = BuildSystem::None; statusBar()->showMessage("Kein Projekt-Root gesetzt"); return; }
        BuildSystem detected = BuildSystem::None;
        QDir root(currentProjectRoot);
        if(root.exists("CMakeLists.txt")) detected = BuildSystem::CMake;
        else if(root.exists("configure.ac") || root.exists("configure")) detected = BuildSystem::Autotools;
        else if(root.exists("Makefile") || root.exists("makefile")) detected = BuildSystem::Make;
        else {
            QStringList pro = root.entryList(QStringList() << "*.pro", QDir::Files);
            if(!pro.isEmpty()) detected = BuildSystem::QMake;
            if(root.exists("meson.build")) detected = BuildSystem::Meson;
            if(root.exists("WORKSPACE") || root.exists("BUILD")) detected = BuildSystem::Bazel;
        }
        currentBuildSystem = detected;
        statusBar()->showMessage(QString("Projekt: %1 — Build: %2").arg(currentProjectRoot.isEmpty() ? "<none>" : currentProjectRoot).arg(buildSystemToString(currentBuildSystem)), 4000);
    }

    // ---------- build command helpers ----------
    QString configureCommandFor(BuildSystem b){
        switch(b){
        case BuildSystem::Autotools: return "autoreconf -i && ./configure";
        case BuildSystem::CMake: return "cmake -S . -B build";
        case BuildSystem::Meson: return "meson setup build";
        case BuildSystem::Bazel: return "";
        case BuildSystem::QMake: return "qmake";
        case BuildSystem::QMake6: return "qmake6";
        case BuildSystem::Make: return "";
        default: return "";
        }
    }

    QString buildCommandFor(BuildSystem b){
        switch(b){
        case BuildSystem::Autotools: return "make";
        case BuildSystem::CMake: return "cmake --build build";
        case BuildSystem::Meson: return "ninja -C build";
        case BuildSystem::Bazel: return "bazel build //...";
        case BuildSystem::QMake: return "make";
        case BuildSystem::QMake6: return "make";
        case BuildSystem::Make: return "make";
        default: return "";
        }
    }

    QString cleanCommandFor(BuildSystem b){
        switch(b){
        case BuildSystem::Autotools: return "make clean";
        case BuildSystem::CMake: return "cmake --build build --target clean";
        case BuildSystem::Meson: return "ninja -C build clean";
        case BuildSystem::Bazel: return "bazel clean";
        case BuildSystem::QMake: return "make clean";
        case BuildSystem::QMake6: return "make clean";
        case BuildSystem::Make: return "make clean";
        default: return "";
        }
    }

    // run a shell command in project root, stream output to console
    void runShellCommand(const QString &cmd){
        if(currentProjectRoot.isEmpty()){ QMessageBox::warning(this,"Kein Projekt","Bitte zuerst Projekt-Root setzen."); return; }
        console->appendPlainText(QString("=== RUN: %1 === (in %2)").arg(cmd).arg(currentProjectRoot));
        QProcess *p = new QProcess(this);
        p->setWorkingDirectory(currentProjectRoot);
#ifdef Q_OS_WIN
        p->start("cmd.exe", QStringList() << "/C" << cmd);
#else
        p->start("/bin/sh", QStringList() << "-lc" << cmd);
#endif
        connect(p, &QProcess::readyReadStandardOutput, this, [this,p](){ console->appendPlainText(QString::fromUtf8(p->readAllStandardOutput())); });
        connect(p, &QProcess::readyReadStandardError, this, [this,p](){ console->appendPlainText(QString::fromUtf8(p->readAllStandardError())); });
        connect(p, QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished), this, [this,p](int code, QProcess::ExitStatus){ console->appendPlainText(QString("=== PROCESS EXIT %1 ===").arg(code)); p->deleteLater(); });
    }

    // ---------- UI actions ----------
    void configureProject(){ QString cmd = configureCommandFor(currentBuildSystem); if(cmd.isEmpty()){ QMessageBox::information(this,"Konfigurieren","Kein Configure-Schritt für dieses Buildsystem."); return; } runShellCommand(cmd); }
    void buildProject(){ QString cmd = buildCommandFor(currentBuildSystem); if(cmd.isEmpty()){ QMessageBox::information(this,"Build","Kein Build-Befehl erkannt; nutze 'Custom Command'."); return; } runShellCommand(cmd); }
    void cleanProject(){ QString cmd = cleanCommandFor(currentBuildSystem); if(cmd.isEmpty()){ QMessageBox::information(this,"Clean","Kein Clean-Befehl erkannt."); return; } runShellCommand(cmd); }
    void runCustomCommand(){ bool ok=false; QString cmd = QInputDialog::getText(this,"Befehl ausführen","Shell-Befehl:",QLineEdit::Normal,"", &ok); if(ok && !cmd.isEmpty()) runShellCommand(cmd); }

    // ---------- project skeleton creation ----------
    bool writeFile(const QString &path, const QString &content){
        QFile f(path);
        if(!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;
        QTextStream ts(&f);
        ts << content;
        f.close();
        return true;
    }

    void createNewCApplication(){
        QString dir = currentProjectRoot;
        if(dir.isEmpty()){
            dir = QFileDialog::getExistingDirectory(this, "Verzeichnis für neues C-Projekt wählen", QDir::currentPath());
            if(dir.isEmpty()) return;
        }
        QString name = QInputDialog::getText(this,"Projektname","Name:",QLineEdit::Normal,"my_c_app");
        if(name.isEmpty()) return;
        QDir d(dir);
        if(!d.exists()) d.mkpath(".");
        QString projDir = d.filePath(name);
        QDir().mkpath(projDir);
        QString mainc = R"(#include <stdio.h>

int main(int argc, char **argv){
    printf("Hello from C application\n");
    return 0;
}
)";
        writeFile(projDir + "/main.c", mainc);
        QString make = QString("CC = gcc\nCFLAGS = -O2 -Wall\nTARGET = %1\nSRCS = main.c\n\nall: $(TARGET)\n\n$(TARGET): $(SRCS)\n\t$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)\n\nclean:\n\trm -f $(TARGET)\n").arg(name);
        writeFile(projDir + "/Makefile", make);
        QString cmake = QString("cmake_minimum_required(VERSION 3.0)\nproject(%1 C)\nadd_executable(%1 main.c)\n").arg(name);
        writeFile(projDir + "/CMakeLists.txt", cmake);
        QMessageBox::information(this,"Erstellt","C-Projekt erstellt: " + projDir);
        setProjectRoot(projDir);
        projView->refresh();
    }

    void createNewCppApplication(){
        QString dir = currentProjectRoot;
        if(dir.isEmpty()){
            dir = QFileDialog::getExistingDirectory(this, "Verzeichnis für neues C++-Projekt wählen", QDir::currentPath());
            if(dir.isEmpty()) return;
        }
        QString name = QInputDialog::getText(this,"Projektname","Name:",QLineEdit::Normal,"my_cpp_app");
        if(name.isEmpty()) return;
        QDir d(dir);
        if(!d.exists()) d.mkpath(".");
        QString projDir = d.filePath(name);
        QDir().mkpath(projDir);
        QString maincpp = R"(#include <iostream>

int main(int argc, char **argv){
    std::cout << "Hello from C++ application" << std::endl;
    return 0;
}
)";
        writeFile(projDir + "/main.cpp", maincpp);
        QString make = QString("CXX = g++\nCXXFLAGS = -O2 -Wall -std=c++17\nTARGET = %1\nSRCS = main.cpp\n\nall: $(TARGET)\n\n$(TARGET): $(SRCS)\n\t$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)\n\nclean:\n\trm -f $(TARGET)\n").arg(name);
        writeFile(projDir + "/Makefile", make);
        QString cmake = QString("cmake_minimum_required(VERSION 3.0)\nproject(%1 CXX)\nset(CMAKE_CXX_STANDARD 17)\nadd_executable(%1 main.cpp)\n").arg(name);
        writeFile(projDir + "/CMakeLists.txt", cmake);
        QMessageBox::information(this,"Erstellt","C++-Projekt erstellt: " + projDir);
        setProjectRoot(projDir);
        projView->refresh();
    }

    void createNewQtWidgetsApplication(){
        QString dir = currentProjectRoot;
        if(dir.isEmpty()){
            dir = QFileDialog::getExistingDirectory(this, "Verzeichnis für neues Qt-Projekt wählen", QDir::currentPath());
            if(dir.isEmpty()) return;
        }
        QString name = QInputDialog::getText(this,"Projektname","Name:",QLineEdit::Normal,"my_qt_app");
        if(name.isEmpty()) return;
        QDir d(dir);
        if(!d.exists()) d.mkpath(".");
        QString projDir = d.filePath(name);
        QDir().mkpath(projDir);
        QString maincpp = R"(#include <QApplication>
#include <QPushButton>

int main(int argc, char **argv){
    QApplication a(argc, argv);
    QPushButton b("Hello Qt");
    b.show();
    return a.exec();
}
)";
        writeFile(projDir + "/main.cpp", maincpp);
        QString pro = QString("QT += widgets\nCONFIG += c++17\nTEMPLATE = app\nTARGET = %1\nSOURCES += main.cpp\n").arg(name);
        writeFile(projDir + "/" + name + ".pro", pro);
        QString cmake = QString("cmake_minimum_required(VERSION 3.0)\nproject(%1)\nfind_package(Qt5 COMPONENTS Widgets REQUIRED)\nadd_executable(%1 main.cpp)\ntarget_link_libraries(%1 Qt5::Widgets)\n").arg(name);
        writeFile(projDir + "/CMakeLists.txt", cmake);
        QMessageBox::information(this,"Erstellt","Qt Widgets Projekt erstellt: " + projDir);
        setProjectRoot(projDir);
        projView->refresh();
    }

    // ---------- editors & helpers ----------
    CodeEditor* createEditor(const QString &path){
        auto ed = new CodeEditor(path);
        ed->setIndexer(&indexer);
        if(savedBps.contains(path)) for(int line: savedBps[path]) ed->breakpoints.insert(line);
        connect(ed, &CodeEditor::breakpointToggled, this, [this, ed](int line, bool set){
            if(set){ QString cmd = QString("-break-insert %1:%2").arg(ed->filePath).arg(line); gdb->send(cmd); savedBps[ed->filePath].insert(line); savePersistedBreakpoints(); }
            else {
                if(bpMap.contains(ed) && bpMap[ed].contains(line)){ int id = bpMap[ed][line]; gdb->send(QString("-break-delete %1").arg(id)); savedBps[ed->filePath].remove(line); savePersistedBreakpoints(); }
                else { gdbConsole->appendPlainText(QString("Breakpoint id für %1:%2 unbekannt; fordere -break-list an.").arg(ed->filePath).arg(line)); savedBps[ed->filePath].remove(line); savePersistedBreakpoints(); gdb->requestBreakList(); }
            }
        });
        connect(ed, &QPlainTextEdit::textChanged, this, [this, ed](){ if(!ed->filePath.isEmpty()){ indexer.index(ed->filePath, ed->toPlainText()); outline->refresh(); } });
        return ed;
    }

    void openFileAtLine(const QString &file, int line){
        if(file.isEmpty() || line<=0) return;
        QString target = file;
        if(!QFileInfo(target).isAbsolute()){
            for(int i=0;i<tabs->count();++i){ auto ed = dynamic_cast<CodeEditor*>(tabs->widget(i)); if(ed && QFileInfo(ed->filePath).fileName() == QFileInfo(target).fileName()){ target = ed->filePath; break; } }
        }
        for(int i=0;i<tabs->count();++i){
            auto ed = dynamic_cast<CodeEditor*>(tabs->widget(i));
            if(ed && ed->filePath == target){
                tabs->setCurrentIndex(i);
                QTextCursor cur = ed->textCursor();
                QTextBlock b = ed->document()->findBlockByNumber(line-1);
                cur.setPosition(b.position());
                ed->setTextCursor(cur);
                ed->currentDebugLine = line;
                ed->viewport()->update();
                return;
            }
        }
        if(QFileInfo::exists(target)){
            auto ed = createEditor(target);
            tabs->addTab(ed, QFileInfo(target).fileName());
            tabs->setCurrentWidget(ed);
            QTimer::singleShot(50, this, [ed,line](){ QTextCursor cur = ed->textCursor(); QTextBlock b = ed->document()->findBlockByNumber(line-1); cur.setPosition(b.position()); ed->setTextCursor(cur); ed->currentDebugLine = line; ed->viewport()->update(); });
        }
    }

    QString switchHeaderSource(const QString &path){
        if(path.isEmpty()) return QString();
        QString base = QFileInfo(path).completeBaseName();
        QString dir = QFileInfo(path).absolutePath();
        QStringList candidates = {dir + "/" + base + ".h", dir + "/" + base + ".hpp", dir + "/" + base + ".hh", dir + "/" + base + ".cpp", dir + "/" + base + ".c", dir + "/" + base + ".cc"};
        for(const QString &c: candidates) if(QFileInfo::exists(c) && c != path) return c;
        QString root = currentProjectRoot.isEmpty() ? QDir::currentPath() : currentProjectRoot;
        QDirIterator it(root, QStringList() << "*.*", QDir::Files, QDirIterator::Subdirectories);
        while(it.hasNext()){ QString p = it.next(); if(QFileInfo(p).completeBaseName() == base && p != path) return p; }
        return QString();
    }

    // ===== actions: open/save/build/run etc. =====
    void openFile(){ QString f=QFileDialog::getOpenFileName(this); if(!f.isEmpty()){ for(int i=0;i<tabs->count();++i){ auto ed = dynamic_cast<CodeEditor*>(tabs->widget(i)); if(ed && ed->filePath == f){ tabs->setCurrentIndex(i); return; } } auto ed=createEditor(f); tabs->addTab(ed,QFileInfo(f).fileName()); tabs->setCurrentWidget(ed); } }
    void openFolder(){ QString d=QFileDialog::getExistingDirectory(this); if(!d.isEmpty()) projView->setRootPath(d); }
    void save(){ if(auto ed=currentEditor()) ed->save(); }
    void build(){ buildProject(); }
    void run(){ console->appendPlainText("=== AUSFÜHREN ==="); QProcess *p=new QProcess(this); connect(p,&QProcess::readyReadStandardOutput,this,[this,p](){ console->appendPlainText(p->readAllStandardOutput()); }); p->start("./a.out"); }
    void debugStart(){ for(int i=0;i<tabs->count();++i){ auto ed=dynamic_cast<CodeEditor*>(tabs->widget(i)); if(ed) ed->save(); } gdb->start(); gdb->send("-file-exec-and-symbols ./a.out"); for(int i=0;i<tabs->count();++i){ auto ed=dynamic_cast<CodeEditor*>(tabs->widget(i)); if(!ed) continue; for(int bp: ed->breakpoints){ if(bpMap.contains(ed) && bpMap[ed].contains(bp)) continue; gdb->send(QString("-break-insert %1:%2").arg(ed->filePath).arg(bp)); } } gdb->send("-exec-run"); }
};

// =====================
// main
// =====================
int main(int argc,char**argv){
    QApplication app(argc,argv);
    IDE ide;
    ide.show();
    return app.exec();
}

#include "ide.moc"
