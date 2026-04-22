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
// SYMBOL + AST (CDT Indexer)
// ============================================================
struct Symbol {
    QString name;
    QString type;
    int line;
    QString file;
};

class CDTIndexer : public QObject {
    Q_OBJECT
public:
    QMap<QString, Symbol> symbols;
    QMap<QString, QVector<Symbol>> fileSymbols;
    QStringList outline;

    void index(const QString &file, const QString &code){
        // simple re-index: clear fileSymbols[file] and re-scan
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

    Symbol find(const QString &name){
        return symbols.value(name);
    }

    QVector<Symbol> all(){ return symbols.values().toVector(); }
};

// =====================
// FileAndDirectoryListBoxTreeView: kombiniert Verzeichnisbaum + Dateiliste
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
        // show only the column with the name
        for(int c=1;c<fsModel->columnCount();++c) dirTree->hideColumn(c);
        dirTree->setHeaderHidden(true);
        dirTree->setAnimated(true);
        dirTree->setExpandsOnDoubleClick(false);

        fileList = new QListWidget(this);
        fileList->setSelectionMode(QAbstractItemView::SingleSelection);

        // layout
        auto splitter = new QSplitter(Qt::Horizontal, this);
        splitter->addWidget(dirTree);
        splitter->addWidget(fileList);
        splitter->setStretchFactor(0, 1);
        splitter->setStretchFactor(1, 2);

        auto lay = new QHBoxLayout(this);
        lay->setContentsMargins(0,0,0,0);
        lay->addWidget(splitter);
        setLayout(lay);

        // connections
        connect(dirTree, &QTreeView::clicked, this, &FileAndDirectoryListBoxTreeView::onDirClicked);
        connect(fileList, &QListWidget::itemDoubleClicked, this, &FileAndDirectoryListBoxTreeView::onFileDoubleClicked);
    }

    // set root directory for the tree
    void setRootPath(const QString &path){
        QModelIndex idx = fsModel->index(path);
        if(idx.isValid()){
            dirTree->setRootIndex(idx);
            // populate file list for root
            populateFileList(path);
        }
    }

    // optional: set file suffix filter (e.g., {"cpp","h"})
    void setSuffixFilter(const QStringList &suffixes){
        suffixFilter = suffixes;
    }

signals:
    void fileActivated(const QString &filePath);
    void directoryActivated(const QString &dirPath);

public slots:
    void refresh(){
        // refresh model and current list
        QString rp = fsModel->rootPath();
        fsModel->setRootPath(""); // force refresh
        fsModel->setRootPath(rp);
        QModelIndex idx = dirTree->currentIndex();
        if(idx.isValid()){
            QString path = fsModel->filePath(idx);
            populateFileList(path);
        }
    }

private slots:
    void onDirClicked(const QModelIndex &idx){
        QString path = fsModel->filePath(idx);
        populateFileList(path);
        emit directoryActivated(path);
    }

    void onFileDoubleClicked(QListWidgetItem *it){
        QString p = it->data(Qt::UserRole).toString();
        emit fileActivated(p);
    }

private:
    QFileSystemModel *fsModel;
    QTreeView *dirTree;
    QListWidget *fileList;
    QStringList suffixFilter;

    void populateFileList(const QString &dirPath){
        fileList->clear();
        QDir d(dirPath);
        if(!d.exists()) return;
        // list files and directories
        QFileInfoList entries = d.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name);
        for(const QFileInfo &fi : entries){
            // apply suffix filter for files if set
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
};

// =====================
// Editor with Breakpoints + current line + simple autocomplete + indexer
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
        // call base to insert text etc.
        QPlainTextEdit::keyPressEvent(e);

        // re-index on edits
        if(indexer && !filePath.isEmpty()){
            indexer->index(filePath, toPlainText());
        }

        // SIMPLE AUTOCOMPLETE: trigger on dot
        if(!e->text().isEmpty() && e->text().endsWith('.')){
            if(!indexer) return;
            QStringList suggestions;
            for(auto &s : indexer->all())
                suggestions << s.name;
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
// Outline (list of symbols)
// =====================
class Outline : public QListWidget {
    Q_OBJECT
public:
    CDTIndexer *indexer=nullptr;

    void refresh(){
        clear();
        if(!indexer) return;
        for(auto &s:indexer->all())
            addItem(s.type+": "+s.name);
    }
};

// =====================
// Navigator (references)
// =====================
class Navigator : public QListWidget {
    Q_OBJECT
public:
    void showRefs(const QVector<Symbol>&syms){
        clear();
        for(auto &s:syms)
            addItem(s.file+":"+QString::number(s.line)+" -> "+s.name);
    }
};

// =====================
// Minimal GDB MI Controller (mit -break-list handling)
// =====================
class GDB : public QObject {
    Q_OBJECT
public:
    QProcess proc;

    // buffer for collecting -break-list response
    bool collectingBreakList = false;
    QString breakListBuffer;

    GDB(){
        connect(&proc,&QProcess::readyReadStandardOutput,this,[this](){
            QString out=proc.readAllStandardOutput();
            emit raw(out);
            emit consoleOutput(out);

            // if we are collecting a break-list response, append and check for ^done
            if(collectingBreakList){
                breakListBuffer += out;
                if(breakListBuffer.contains("\n^done") || breakListBuffer.contains("\r^done") || breakListBuffer.contains("^done")){
                    collectingBreakList = false;
                    emit breakListReady(breakListBuffer);
                    breakListBuffer.clear();
                }
            }

            // crude parsing for stopped
            if(out.contains("*stopped")){
                QRegularExpression re("file=\"([^\"]+)\",line=\"(\\d+)\"");
                auto m=re.match(out);
                if(m.hasMatch()){
                    emit stopped(m.captured(1), m.captured(2).toInt(), 0);
                }
                send("-stack-list-variables --simple-values");
            }

            // parse bkpt responses (e.g. ^done,bkpt={number="1",...file="main.cpp",line="42",...})
            QRegularExpression reBkpt(R"(bkpt=\{[^}]*number=\"(\d+)\"[^}]*file=\"([^\"]+)\"[^}]*line=\"(\d+)\"[^}]*\})");
            auto it = reBkpt.globalMatch(out);
            while(it.hasNext()){
                auto m = it.next();
                int id = m.captured(1).toInt();
                QString file = m.captured(2);
                int line = m.captured(3).toInt();
                emit breakpointSet(file, line, id);
            }

            // parse =breakpoint-deleted notifications
            QRegularExpression reDel(R"(=breakpoint-deleted,id=\"(\d+)\")");
            auto it2 = reDel.globalMatch(out);
            while(it2.hasNext()){
                auto m = it2.next();
                int id = m.captured(1).toInt();
                emit breakpointDeleted(id);
            }

            // parse variables list
            if(out.contains("variables=[")){
                emit locals(out);
            }
        });

        connect(&proc,&QProcess::readyReadStandardError,this,[this](){
            QString err=proc.readAllStandardError();
            emit raw(err);
            emit consoleOutput(err);
        });

        connect(&proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, [this](int code, QProcess::ExitStatus st){
                    Q_UNUSED(code); Q_UNUSED(st);
                    emit exited();
                });
    }

    void start(){
        if(proc.state()!=QProcess::NotRunning) proc.kill();
        proc.start("gdb", {"--interpreter=mi2"});
    }

    void send(QString cmd){
        if(proc.state()==QProcess::NotRunning){
            emit error("GDB not running");
            return;
        }
        proc.write((cmd+"\n").toUtf8());
    }

    // request break-list and emit breakListReady when complete
    void requestBreakList(){
        if(proc.state()==QProcess::NotRunning){
            emit error("GDB not running (cannot request break-list)");
            return;
        }
        collectingBreakList = true;
        breakListBuffer.clear();
        proc.write(QByteArray("-break-list\n"));
    }

signals:
    void raw(QString);
    void stopped(QString file,int line,int thread);
    void locals(QString);
    void consoleOutput(QString);
    void breakpointSet(QString file,int line,int id);
    void breakpointDeleted(int id);
    void breakListReady(QString);
    void error(QString);
    void exited();
};

// =====================
// IDE (merged features: project view, indexer, outline, navigator, gdb, breakpoints, persistence)
// =====================
class IDE : public QMainWindow {
    Q_OBJECT
public:
    IDE(){
        resize(1800,1000);
        setWindowTitle("Single-File Qt IDE (Hybrid, with GDB, Indexer, Explorer)");

        fsModel=new QFileSystemModel();
        fsModel->setRootPath("");

        projView = new FileAndDirectoryListBoxTreeView();

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

        // load persisted breakpoints from disk
        loadPersistedBreakpoints();

        setupUI();
        setupConnections();

        // live outline refresh timer
        QTimer *t=new QTimer(this);
        connect(t,&QTimer::timeout,this,[this](){ outline->refresh(); });
        t->start(500);
    }

    ~IDE(){
        savePersistedBreakpoints();
    }

private:
    // models & views
    QFileSystemModel *fsModel;
    FileAndDirectoryListBoxTreeView *projView;
    QTabWidget *tabs;
    QPlainTextEdit *console;
    QPlainTextEdit *gdbConsole;
    QListWidget *localsView;
    QListWidget *framesView;
    QTreeWidget *varsTree;

    // indexer & outline
    CDTIndexer indexer;
    Outline *outline;
    Navigator *nav;
    QListWidget *problems;
    QListWidget *search;

    // gdb
    GDB *gdb;

    // breakpoint map: editor -> (line -> breakpoint-id)
    QMap<CodeEditor*, QMap<int,int>> bpMap;

    // persisted breakpoints: absolute file path -> set of lines
    QMap<QString, QSet<int>> savedBps;

    CodeEditor* currentEditor(){
        return dynamic_cast<CodeEditor*>(tabs->currentWidget());
    }

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
        // include savedBps plus any in-memory editors not yet persisted
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
        if(f.open(QIODevice::WriteOnly)){
            f.write(doc.toJson());
            f.close();
        }
    }

    void setupUI(){
        // Central
        setCentralWidget(tabs);

        // Explorer (Projektansicht) mit FileAndDirectoryListBoxTreeView
        projView->setSuffixFilter(QStringList() << "cpp" << "h" << "hpp" << "c" << "txt" << "md");
        projView->setRootPath(QDir::currentPath());

        auto dockL = new QDockWidget("Explorer");
        dockL->setWidget(projView);
        addDockWidget(Qt::LeftDockWidgetArea, dockL);

        // Bottom: Console + GDB
        auto bottom=new QTabWidget();
        bottom->addTab(console,"Program Output");
        bottom->addTab(gdbConsole,"GDB");

        auto dockB=new QDockWidget("Output");
        dockB->setWidget(bottom);
        addDockWidget(Qt::BottomDockWidgetArea,dockB);

        // Right: Debug (Locals, Frames, Variables)
        auto rightTabs = new QTabWidget();
        rightTabs->addTab(localsView, "Locals");
        rightTabs->addTab(framesView, "Frames");
        rightTabs->addTab(varsTree, "Variables");

        auto dockR=new QDockWidget("Debug");
        dockR->setWidget(rightTabs);
        addDockWidget(Qt::RightDockWidgetArea,dockR);

        // Outline dock
        auto dockO=new QDockWidget("Outline");
        dockO->setWidget(outline);
        addDockWidget(Qt::RightDockWidgetArea,dockO);

        // Navigation dock
        auto dockN=new QDockWidget("Navigation");
        dockN->setWidget(nav);
        addDockWidget(Qt::RightDockWidgetArea,dockN);

        // Problems & Search
        auto dockP=new QDockWidget("Problems");
        dockP->setWidget(problems);
        addDockWidget(Qt::BottomDockWidgetArea,dockP);

        auto dockS=new QDockWidget("Search");
        dockS->setWidget(search);
        addDockWidget(Qt::BottomDockWidgetArea,dockS);

        // Menu
        auto file=menuBar()->addMenu("File");
        file->addAction("Open File",this,&IDE::openFile);
        file->addAction("Open Folder",this,&IDE::openFolder);
        file->addAction("Save",this,&IDE::save);

        auto build=menuBar()->addMenu("Project");
        build->addAction("Build",this,&IDE::build);

        auto run=menuBar()->addMenu("Run");
        run->addAction("Run",this,&IDE::run);

        auto debug=menuBar()->addMenu("Debug");
        debug->addAction("Start",this,&IDE::debugStart);
        debug->addAction("Continue",this,[this]{ gdb->send("-exec-continue"); });
        debug->addAction("Next",this,[this]{ gdb->send("-exec-next"); });
        debug->addAction("Step",this,[this]{ gdb->send("-exec-step"); });
        debug->addAction("Backtrace",this,[this]{ gdb->send("-stack-list-frames"); });
        debug->addAction("Attach to PID",this,[this](){
            bool ok=false;
            int pid = QInputDialog::getInt(this,"Attach to PID","PID:",0,0,INT_MAX,1,&ok);
            if(ok) {
                gdb->start();
                gdb->send(QString("-target-attach %1").arg(pid));
            }
        });

        // Toolbar
        auto tb=addToolBar("Main");
        tb->addAction("▶",this,&IDE::run);
        tb->addAction("🐞",this,&IDE::debugStart);

        // optional refresh action for explorer
        auto refreshAct = new QAction("Refresh Explorer", this);
        menuBar()->addAction(refreshAct);
        connect(refreshAct, &QAction::triggered, projView, &FileAndDirectoryListBoxTreeView::refresh);

        statusBar()->showMessage("Hybrid IDE: GDB + CDT Indexer + Explorer");
    }

    void setupConnections(){
        // project view file open
        connect(projView, &FileAndDirectoryListBoxTreeView::fileActivated, this, [this](const QString &path){
            if(QFileInfo(path).isFile()){
                // if already opened, focus tab
                for(int i=0;i<tabs->count();++i){
                    auto ed = dynamic_cast<CodeEditor*>(tabs->widget(i));
                    if(ed && ed->filePath == path){
                        tabs->setCurrentIndex(i);
                        return;
                    }
                }
                auto ed = createEditor(path);
                tabs->addTab(ed, QFileInfo(path).fileName());
                tabs->setCurrentWidget(ed);
            }
        });

        // tree double click (legacy model) - keep for compatibility
        connect(fsModel, &QFileSystemModel::directoryLoaded, this, [](const QString&){ /* noop */ });

        // GDB raw output
        connect(gdb,&GDB::raw,this,[this](QString s){
            gdbConsole->appendPlainText(s);
        });

        connect(gdb,&GDB::consoleOutput,this,[this](QString s){
            gdbConsole->appendPlainText(s);
        });

        connect(gdb,&GDB::stopped,this,[this](QString file,int line,int thread){
            for(int i=0;i<tabs->count();++i){
                auto ed=dynamic_cast<CodeEditor*>(tabs->widget(i));
                if(ed && QFileInfo(ed->filePath).fileName()==QFileInfo(file).fileName()){
                    tabs->setCurrentIndex(i);
                    ed->currentDebugLine=line;
                    ed->viewport()->update();
                }
            }
        });

        connect(gdb,&GDB::locals,this,[this](QString txt){
            localsView->clear();
            QRegularExpression re("name=\"([^\"]+)\",value=\"([^\"]*)\"");
            auto it=re.globalMatch(txt);
            while(it.hasNext()){
                auto m=it.next();
                localsView->addItem(m.captured(1)+" = "+m.captured(2));
            }
        });

        // breakpoint set: update mapping and UI
        connect(gdb,&GDB::breakpointSet,this,[this](QString file,int line,int id){
            for(int i=0;i<tabs->count();++i){
                auto ed=dynamic_cast<CodeEditor*>(tabs->widget(i));
                if(!ed) continue;
                if(ed->filePath == file || QFileInfo(ed->filePath).fileName() == QFileInfo(file).fileName()){
                    bpMap[ed][line] = id;
                    if(!ed->breakpoints.contains(line)) ed->breakpoints.insert(line);
                    ed->viewport()->update();
                    gdbConsole->appendPlainText(QString("Breakpoint %1 set at %2:%3").arg(id).arg(file).arg(line));
                    // persist
                    savedBps[ed->filePath].insert(line);
                }
            }
        });

        // breakpoint deleted: remove mapping and UI marker
        connect(gdb,&GDB::breakpointDeleted,this,[this](int id){
            for(auto it = bpMap.begin(); it != bpMap.end(); ++it){
                CodeEditor* ed = it.key();
                auto &map = it.value();
                QList<int> toRemove;
                for(auto mit = map.begin(); mit != map.end(); ++mit){
                    if(mit.value() == id){
                        toRemove.append(mit.key());
                    }
                }
                for(int line: toRemove){
                    map.remove(line);
                    ed->breakpoints.remove(line);
                    ed->viewport()->update();
                    gdbConsole->appendPlainText(QString("Breakpoint %1 removed (editor %2 line %3)").arg(id).arg(ed->filePath).arg(line));
                    if(savedBps.contains(ed->filePath)) savedBps[ed->filePath].remove(line);
                }
            }
        });

        // when break-list response arrives, try to resolve pending delete requests
        connect(gdb, &GDB::breakListReady, this, [this](QString mi){
            QMap<QString, QMap<int,int>> listMap;
            QRegularExpression reBkpt(R"(bkpt=\{[^}]*number=\"(\d+)\"[^}]*file=\"([^\"]+)\"[^}]*line=\"(\d+)\"[^}]*\})");
            auto it = reBkpt.globalMatch(mi);
            while(it.hasNext()){
                auto m = it.next();
                int id = m.captured(1).toInt();
                QString file = m.captured(2);
                int line = m.captured(3).toInt();
                listMap[file][line] = id;
            }
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
                                if(QFileInfo(lmIt.key()).fileName() == base && lmIt.value().contains(line)){
                                    id = lmIt.value()[line];
                                    break;
                                }
                            }
                        }
                        if(id>0){
                            gdb->send(QString("-break-delete %1").arg(id));
                            savedBps[filePath].remove(line);
                        }
                    }
                }
            }
        });

        // project tree double click: open file (legacy)
        connect(projView, &FileAndDirectoryListBoxTreeView::directoryActivated, this, [this](const QString&){ /* noop */ });

        // navigation menu actions
        auto navMenu = menuBar()->addMenu("Navigate");
        navMenu->addAction("Open Declaration",this,[this](){
            bool ok;
            QString s=QInputDialog::getText(this,"Open Declaration","Symbol:",QLineEdit::Normal,"",&ok);
            if(!ok) return;
            Symbol sym=indexer.find(s);
            nav->showRefs({sym});
        });

        navMenu->addAction("Global Search",this,[this](){
            bool ok;
            QString s=QInputDialog::getText(this,"Search","Query:",QLineEdit::Normal,"",&ok);
            if(!ok) return;
            search->clear();
            for(auto &sym:indexer.all())
                if(sym.name.contains(s))
                    search->addItem(sym.file+":"+sym.name);
        });
    }

    // helper to centralize editor creation and connect breakpoint signal
    CodeEditor* createEditor(const QString &path){
        auto ed = new CodeEditor(path);
        ed->setIndexer(&indexer);
        // if persisted breakpoints exist for this file, apply to editor UI
        if(savedBps.contains(path)){
            for(int line: savedBps[path]) ed->breakpoints.insert(line);
        }
        connect(ed, &CodeEditor::breakpointToggled, this, [this, ed](int line, bool set){
            if(set){
                QString cmd = QString("-break-insert %1:%2").arg(ed->filePath).arg(line);
                gdb->send(cmd);
                savedBps[ed->filePath].insert(line);
                savePersistedBreakpoints();
            } else {
                if(bpMap.contains(ed) && bpMap[ed].contains(line)){
                    int id = bpMap[ed][line];
                    gdb->send(QString("-break-delete %1").arg(id));
                    savedBps[ed->filePath].remove(line);
                    savePersistedBreakpoints();
                } else {
                    gdbConsole->appendPlainText(QString("Breakpoint id for %1:%2 unknown; requesting -break-list to resolve.").arg(ed->filePath).arg(line));
                    savedBps[ed->filePath].remove(line);
                    savePersistedBreakpoints();
                    gdb->requestBreakList();
                }
            }
        });

        // when editor content changes, update indexer and outline
        connect(ed, &QPlainTextEdit::textChanged, this, [this, ed](){
            if(!ed->filePath.isEmpty()){
                indexer.index(ed->filePath, ed->toPlainText());
                outline->refresh();
            }
        });

        return ed;
    }

    // ===== actions =====
    void openFile(){
        QString f=QFileDialog::getOpenFileName(this);
        if(!f.isEmpty()){
            // if already opened, focus
            for(int i=0;i<tabs->count();++i){
                auto ed = dynamic_cast<CodeEditor*>(tabs->widget(i));
                if(ed && ed->filePath == f){
                    tabs->setCurrentIndex(i);
                    return;
                }
            }
            auto ed=createEditor(f);
            tabs->addTab(ed,QFileInfo(f).fileName());
            tabs->setCurrentWidget(ed);
        }
    }

    void openFolder(){
        QString d=QFileDialog::getExistingDirectory(this);
        if(!d.isEmpty()) {
            projView->setRootPath(d);
        }
    }

    void save(){
        if(auto ed=currentEditor()) ed->save();
    }

    void build(){
        console->appendPlainText("=== BUILD ===");
        QProcess::execute("make");
    }

    void run(){
        console->appendPlainText("=== RUN ===");
        QProcess *p=new QProcess(this);
        connect(p,&QProcess::readyReadStandardOutput,this,[this,p](){
            console->appendPlainText(p->readAllStandardOutput());
        });
        p->start("./a.out");
    }

    void debugStart(){
        // save all editors
        for(int i=0;i<tabs->count();++i){
            auto ed=dynamic_cast<CodeEditor*>(tabs->widget(i));
            if(ed) ed->save();
        }

        gdb->start();
        gdb->send("-file-exec-and-symbols ./a.out");

        // set breakpoints from all open editors (if not already set)
        for(int i=0;i<tabs->count();++i){
            auto ed=dynamic_cast<CodeEditor*>(tabs->widget(i));
            if(!ed) continue;
            for(int bp: ed->breakpoints){
                if(bpMap.contains(ed) && bpMap[ed].contains(bp)) continue;
                gdb->send(QString("-break-insert %1:%2").arg(ed->filePath).arg(bp));
            }
        }
        gdb->send("-exec-run");
    }
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
