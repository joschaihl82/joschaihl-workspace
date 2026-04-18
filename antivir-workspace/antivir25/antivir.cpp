// antivir.cpp — kompakte Einzeldatei (Qt5, libclamav) mit using namespace std, korrekte cl_scandesc-Signatur
#include <QApplication>
#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QDialog>
#include <QLabel>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>
#include <QThread>
#include <QStringList>
#include <QHeaderView>
#include <clamav.h>
using namespace std;

enum class Mode { Full, List };
struct Finding { QString path, status, virus; };

class Findings : public QWidget {
    Q_OBJECT
public:
    Findings() {
        setWindowTitle("Virenfunde"); resize(700,300);
        auto *lay = new QVBoxLayout(this);
        table = new QTableWidget(this); table->setColumnCount(3);
        table->setHorizontalHeaderLabels({"Pfad","Status","Virus"});
        table->horizontalHeader()->setStretchLastSection(true);
        lay->addWidget(table);
    }
    void add(const Finding& f) {
        int r = table->rowCount(); table->insertRow(r);
        table->setItem(r,0,new QTableWidgetItem(f.path));
        table->setItem(r,1,new QTableWidgetItem(f.status));
        table->setItem(r,2,new QTableWidgetItem(f.virus));
    }
private: QTableWidget* table=nullptr;
};

class Overlay : public QDialog {
    Q_OBJECT
public:
    Overlay() {
        setWindowTitle("Fortschritt"); setModal(false); setWindowFlag(Qt::Tool); resize(420,110);
        auto *lay=new QVBoxLayout(this); label=new QLabel("Bereit",this); label->setWordWrap(true); lay->addWidget(label);
    }
    void showMsg(const QString& s) { label->setText(s); if(!isVisible()) show(); qApp->processEvents(); }
private: QLabel* label=nullptr;
};

class Scanner : public QThread {
    Q_OBJECT
public:
    Mode mode=Mode::Full; QStringList excludes; QString logFiles, logResults;
signals:
    void started(const QString& m); void progress(const QString& p,const QString& s);
    void finding(const Finding& f); void finished();
protected:
    void run() override {
        emit started(mode==Mode::Full?"Vollscan":"Dateiliste-Scan");
        if (cl_init(CL_INIT_DEFAULT)!=CL_SUCCESS) { emit progress("Engine","Init-Fehler"); emit finished(); return; }
        cl_engine* eng = cl_engine_new(); if(!eng){ emit progress("Engine","Engine-Fehler"); emit finished(); return; }
        unsigned int sigs = 0;
        if (cl_load("/var/lib/clamav", eng, &sigs, CL_DB_STDOPT)!=CL_SUCCESS) { cl_engine_free(eng); emit progress("Engine","DB-Load-Fehler"); emit finished(); return; }
        if (cl_engine_compile(eng)!=CL_SUCCESS) { cl_engine_free(eng); emit progress("Engine","Compile-Fehler"); emit finished(); return; }

        auto scanOne = [&](const QString& path){
            emit progress(path, "Prüfe...");
            QFile f(path);
            if(!f.open(QIODevice::ReadOnly)) { writeResult({path,"SKIPPED",""}); return; }
            const QByteArray fname = path.toUtf8();
            const char* vname = nullptr; unsigned long scanned = 0;
            // neuere libclamav Signatur: cl_scandesc(int, const char*, const char**, unsigned long*, const cl_engine*, const cl_scan_options*)
            int r = cl_scandesc(f.handle(), fname.constData(), &vname, &scanned, eng, nullptr);
            Finding out{path, r==CL_VIRUS?"INFECTED":(r==CL_CLEAN?"OK":"ERROR"),
                        r==CL_VIRUS?(vname?QString::fromUtf8(vname):"Unknown"):(r==CL_CLEAN?"":"Code "+QString::number(r))};
            writeFiles(path); writeResult(out); if(out.status=="INFECTED") emit finding(out);
        };

        if (mode==Mode::Full) {
            QDirIterator it("/", QDir::NoDotAndDotDot|QDir::AllEntries, QDirIterator::Subdirectories);
            while(it.hasNext()){
                QString p = it.next();
                if (isExcluded(p)) continue;
                QFileInfo fi(p);
                if (!fi.exists() || !fi.isFile()) continue;
                QFile test(p);
                if (!test.open(QIODevice::ReadOnly)) continue;
                test.close();
                scanOne(p);
            }
        } else {
            QFile f(logFiles);
            if (f.open(QIODevice::ReadOnly)) {
                QTextStream in(&f);
                while(!in.atEnd()){
                    QString p = in.readLine().trimmed();
                    if (p.isEmpty() || isExcluded(p)) continue;
                    QFileInfo fi(p);
                    if (!fi.isFile()) continue;
                    QFile test(p);
                    if (!test.open(QIODevice::ReadOnly)) continue;
                    test.close();
                    scanOne(p);
                }
            }
        }
        cl_engine_free(eng); emit finished();
    }
private:
    bool isExcluded(const QString& p) const { for(const auto& ex: excludes) if(p.startsWith(ex)) return true; return false; }
    void writeFiles(const QString& p){ QFile f(logFiles); if(f.open(QIODevice::Append)){ QTextStream o(&f); o<<p<<"\n"; } }
    void writeResult(const Finding& r){ QFile f(logResults); if(f.open(QIODevice::Append)){ QTextStream o(&f); o<<r.path<<"\t"<<r.status<<"\t"<<r.virus<<"\n"; } }
};

class App : public QObject {
    Q_OBJECT
public:
    App(){
        dir = QDir::homePath()+"/.antivirus"; lf=dir+"/antivirus.log"; lr=dir+"/scan_results.log";
        QDir d(dir); if(!d.exists()) d.mkpath(".");
        { QFile a(lf); if(!a.exists()){ a.open(QIODevice::WriteOnly); a.close(); } }
        { QFile b(lr); if(!b.exists()){ b.open(QIODevice::WriteOnly); b.close(); } }
        tray.setIcon(QIcon::fromTheme("security-high")); tray.setToolTip("Antivirus (ClamAV)");
        menu.addAction("Vollscan starten", this, &App::fullScan);
        menu.addAction("Dateiliste scannen", this, &App::listScan);
        menu.addSeparator(); menu.addAction("Logs öffnen", this, &App::openLogs);
        menu.addSeparator(); menu.addAction("Beenden", []{ qApp->quit(); });
        tray.setContextMenu(&menu); tray.show(); tray.showMessage("Antivirus","Bereit.",QSystemTrayIcon::Information,2500);
        findings = new Findings(); overlay = new Overlay();
    }
private slots:
    void fullScan(){ start(Mode::Full); }
    void listScan(){ start(Mode::List); }
    void openLogs(){ QDesktopServices::openUrl(QUrl::fromLocalFile(lr)); }
    void onStarted(const QString& m){ overlay->showMsg("Scan gestartet ("+m+")"); }
    void onProgress(const QString& p,const QString& s){ overlay->showMsg(p+"\n"+s); }
    void onFinding(const Finding& f){ findings->add(f); if(!findings->isVisible()) findings->show(); }
    void onFinished(){
        overlay->showMsg("Scan abgeschlossen"); tray.showMessage("Antivirus","Scan abgeschlossen.",QSystemTrayIcon::Information,2500);
        scanner->quit(); scanner->wait(); scanner->deleteLater(); scanner=nullptr;
    }
private:
    void start(Mode m){
        if(scanner) return;
        scanner = new Scanner(); scanner->mode=m;
        scanner->excludes = QStringList{ "/proc","/sys","/dev","/run","/var/cache","/var/tmp","/tmp", QDir::homePath()+"/.cache" };
        scanner->logFiles=lf; scanner->logResults=lr;
        connect(scanner,&Scanner::started,this,&App::onStarted);
        connect(scanner,&Scanner::progress,this,&App::onProgress);
        connect(scanner,&Scanner::finding,this,&App::onFinding);
        connect(scanner,&Scanner::finished,this,&App::onFinished);
        scanner->start();
    }
    QString dir, lf, lr;
    QSystemTrayIcon tray; QMenu menu;
    Findings* findings=nullptr; Overlay* overlay=nullptr; Scanner* scanner=nullptr;
};

int main(int argc, char** argv){
    QApplication app(argc, argv);
    QApplication::setApplicationName("Antivirus QT");
    App antivir; return app.exec();
}

#include "antivir.moc"

