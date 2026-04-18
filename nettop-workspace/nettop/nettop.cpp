// nettop.cpp  - expanded, more working Qt5 + libpcap nettop proof-of-concept
// Build with Qt5 Widgets and libpcap. Run as root.
// See CMakeLists.txt provided earlier.

#include <QApplication>
#include <QTableView>
#include <QAbstractTableModel>
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QMap>
#include <QHash>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>

#include <pcap.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <atomic>
#include <algorithm>
#include <inttypes.h>

using std::string;
using std::unordered_map;

static inline QString readCmdline(pid_t pid){
    QString path = QString("/proc/%1/cmdline").arg(pid);
    QFile f(path);
    if(!f.open(QIODevice::ReadOnly)) return QString::number(pid);
    QByteArray data = f.readAll();
    if(data.isEmpty()) return QString::number(pid);
    for(int i=0;i<data.size();++i) if(data.at(i) == '\0') data[i] = ' ';
    return QString::fromLocal8Bit(data).trimmed();
}

// Flow key helper
static inline string flow_key(uint8_t proto, uint32_t saddr, uint16_t sport, uint32_t daddr, uint16_t dport){
    std::ostringstream ss;
    ss << int(proto) << ":" << saddr << ":" << sport << ":" << daddr << ":" << dport;
    return ss.str();
}

struct ProcStat {
    pid_t pid = 0;
    QString cmd;
    uint64_t inBytes = 0;
    uint64_t outBytes = 0;
    uint64_t inPrev = 0;
    uint64_t outPrev = 0;
    QDateTime lastSeen;
};

class NetModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit NetModel(QObject *parent=nullptr) : QAbstractTableModel(parent) {}
    QList<ProcStat> rows;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override { Q_UNUSED(parent); return rows.size(); }
    int columnCount(const QModelIndex &parent = QModelIndex()) const override { Q_UNUSED(parent); return 5; }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
        if(role != Qt::DisplayRole || orientation != Qt::Horizontal) return {};
        static QStringList h = {"Path/Name","Input MB","Output MB","Input KB/s","Output KB/s"};
        return h.value(section);
    }

    QVariant data(const QModelIndex &idx, int role) const override {
        if(role != Qt::DisplayRole) return {};
        if(!idx.isValid()) return {};
        const ProcStat &p = rows.at(idx.row());
        switch(idx.column()){
            case 0: return p.cmd;
            case 1: return QString::number(p.inBytes/1024.0/1024.0,'f',2);
            case 2: return QString::number(p.outBytes/1024.0/1024.0,'f',2);
            case 3: {
                double kb = double(p.inBytes - p.inPrev)/1024.0;
                return QString::number(kb,'f',2);
            }
            case 4: {
                double kb = double(p.outBytes - p.outPrev)/1024.0;
                return QString::number(kb,'f',2);
            }
        }
        return {};
    }

    // Efficient in-place update using a snapshot map
    void applySnapshot(const QMap<pid_t, ProcStat> &snapshot) {
        // Build pid->index map
        QHash<pid_t,int> idxMap;
        for(int i=0;i<rows.size();++i) idxMap.insert(rows[i].pid, i);

        QList<int> changedRows;
        QList<ProcStat> newRows;

        // Update existing and collect new
        for(auto it = snapshot.constBegin(); it != snapshot.constEnd(); ++it){
            pid_t pid = it.key();
            const ProcStat &ps = it.value();
            if(idxMap.contains(pid)){
                int i = idxMap.value(pid);
                rows[i].inPrev = rows[i].inBytes;
                rows[i].outPrev = rows[i].outBytes;
                rows[i].inBytes = ps.inBytes;
                rows[i].outBytes = ps.outBytes;
                rows[i].cmd = ps.cmd;
                rows[i].lastSeen = ps.lastSeen;
                changedRows.append(i);
            } else {
                ProcStat nr = ps;
                nr.inPrev = ps.inBytes;
                nr.outPrev = ps.outBytes;
                newRows.append(nr);
            }
        }

        // Append new rows
        if(!newRows.isEmpty()){
            beginInsertRows(QModelIndex(), rows.size(), rows.size() + newRows.size() - 1);
            for(const ProcStat &r : newRows) rows.append(r);
            endInsertRows();
            for(int i = rows.size() - newRows.size(); i < rows.size(); ++i) changedRows.append(i);
        }

        // Optionally prune rows not in snapshot (not done to avoid flicker)

        if(!changedRows.isEmpty()){
            int minRow = *std::min_element(changedRows.begin(), changedRows.end());
            int maxRow = *std::max_element(changedRows.begin(), changedRows.end());
            QModelIndex topLeft = index(minRow, 0);
            QModelIndex bottomRight = index(maxRow, columnCount() - 1);
            emit dataChanged(topLeft, bottomRight);
        }
    }

    // Helper to reset with test data (useful for debugging)
    void populateTest(){
        beginResetModel();
        rows.clear();
        ProcStat a; a.pid = 1234; a.cmd = "/usr/bin/exampleA"; a.inBytes = 5*1024*1024; a.outBytes = 2*1024*1024;
        ProcStat b; b.pid = 4321; b.cmd = "/usr/bin/exampleB"; b.inBytes = 1*1024*1024; b.outBytes = 8*1024*1024;
        rows.append(a); rows.append(b);
        endResetModel();
    }
};

class ProcMapper : public QThread {
    Q_OBJECT
public:
    std::atomic<bool> running{true};
    QMutex mapMutex;
    unordered_map<string, pid_t> flowToPid;

    void run() override {
        while(running){
            buildMapping();
            for(int i=0;i<20 && running; ++i) msleep(100);
        }
    }

    void stop(){ running = false; }

    void buildMapping(){
        unordered_map<string, string> inodeToFlow;
        const char *files[] = {"/proc/net/tcp", "/proc/net/udp"};
        for(int fi=0; fi<2; ++fi){
            QFile f(files[fi]);
            if(!f.open(QIODevice::ReadOnly | QIODevice::Text)) continue;
            QTextStream in(&f);
            QString header = in.readLine();
            while(!in.atEnd()){
                QString line = in.readLine().trimmed();
                if(line.isEmpty()) continue;
                QStringList parts = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
                if(parts.size() < 10) continue;
                QString local = parts[1];
                QString rem = parts[2];
                QString inode = parts[9];
                unsigned int lip=0, lport=0, rip=0, rport=0;
                sscanf(local.toStdString().c_str(), "%X:%X", &lip, &lport);
                sscanf(rem.toStdString().c_str(), "%X:%X", &rip, &rport);
                uint8_t proto = (fi==0?6:17);
                string fwd = flow_key(proto, lip, (uint16_t)lport, rip, (uint16_t)rport);
                string rev = flow_key(proto, rip, (uint16_t)rport, lip, (uint16_t)lport);
                inodeToFlow[inode.toStdString()] = fwd + "|" + rev;
            }
            f.close();
        }

        unordered_map<string, pid_t> inodePid;
        DIR *pdir = opendir("/proc");
        if(pdir){
            struct dirent *dent;
            while((dent = readdir(pdir))){
                if(dent->d_type != DT_DIR) continue;
                const char *dname = dent->d_name;
                bool isnum = true;
                for(const char *c = dname; *c; ++c) if(!isdigit(*c)){ isnum=false; break; }
                if(!isnum) continue;
                pid_t pid = atoi(dname);
                char fdpath[256];
                snprintf(fdpath, sizeof(fdpath), "/proc/%s/fd", dname);
                DIR *fdd = opendir(fdpath);
                if(!fdd) continue;
                struct dirent *fde;
                while((fde = readdir(fdd))){
                    if(fde->d_type != DT_LNK) continue;
                    char linkpath[512];
                    snprintf(linkpath, sizeof(linkpath), "%s/%s", fdpath, fde->d_name);
                    char target[512];
                    ssize_t r = readlink(linkpath, target, sizeof(target)-1);
                    if(r>0){ target[r]=0;
                        if(strncmp(target, "socket:[", 8)==0){
                            char inode[64];
                            if(sscanf(target, "socket:[%63[^]]]", inode)==1){
                                inodePid[string(inode)] = pid;
                            }
                        }
                    }
                }
                closedir(fdd);
            }
            closedir(pdir);
        }

        unordered_map<string, pid_t> newMap;
        for(auto &kv : inodeToFlow){
            const string &inode = kv.first;
            auto it = inodePid.find(inode);
            if(it==inodePid.end()) continue;
            pid_t pid = it->second;
            string both = kv.second;
            size_t sep = both.find('|');
            if(sep!=string::npos){
                string fwd = both.substr(0,sep);
                string rev = both.substr(sep+1);
                newMap[fwd] = pid;
                newMap[rev] = pid;
            }
        }

        {
            QMutexLocker locker(&mapMutex);
            flowToPid.swap(newMap);
        }
    }

    pid_t lookup(uint8_t proto, uint32_t saddr, uint16_t sport, uint32_t daddr, uint16_t dport){
        string k = flow_key(proto, saddr, sport, daddr, dport);
        QMutexLocker locker(&mapMutex);
        auto it = flowToPid.find(k);
        if(it!=flowToPid.end()) return it->second;
        return 0;
    }
};

class CaptureThread : public QThread {
    Q_OBJECT
public:
    ProcMapper *mapper;
    QMutex statsMutex;
    QMap<pid_t, ProcStat> stats;
    std::atomic<bool> running{true};

    CaptureThread(ProcMapper *m) : mapper(m) {}

    void run() override {
        char err[PCAP_ERRBUF_SIZE];
        pcap_t *p = pcap_open_live("any", 65536, 1, 500, err);
        if(!p){
            fprintf(stderr, "pcap_open_live failed: %s\n", err);
            return;
        }
        while(running){
            pcap_dispatch(p, 200, [](u_char *user, const struct pcap_pkthdr *h, const u_char *bytes){
                CaptureThread *self = reinterpret_cast<CaptureThread*>(user);
                if(h->caplen < 14 + 20) return;
                const u_char *ip = bytes + 14;
                if((ip[0] >> 4) != 4) return;
                int ihl = (ip[0] & 0x0F) * 4;
                if(h->caplen < 14 + ihl) return;
                uint8_t proto = ip[9];
                uint32_t saddr = 0, daddr = 0;
                memcpy(&saddr, ip+12, 4);
                memcpy(&daddr, ip+16, 4);
                uint16_t sport = 0, dport = 0;
                const u_char *l4 = ip + ihl;
                if(proto == 6 && h->caplen >= 14 + ihl + 20){
                    sport = ntohs(*(uint16_t*)(l4));
                    dport = ntohs(*(uint16_t*)(l4+2));
                } else if(proto == 17 && h->caplen >= 14 + ihl + 8){
                    sport = ntohs(*(uint16_t*)(l4));
                    dport = ntohs(*(uint16_t*)(l4+2));
                } else {
                    return;
                }

                pid_t pid = 0;
                if(self->mapper) pid = self->mapper->lookup(proto, saddr, sport, daddr, dport);

                uint64_t len = h->len;

                // Heuristic direction detection:
                // If pid found and source port is ephemeral (>1024) assume packet is outgoing from process (outBytes).
                // If pid found and dest port is ephemeral assume incoming.
                // This is imperfect but works reasonably for many client/server flows.
                QMutexLocker locker(&self->statsMutex);
                ProcStat &ps = self->stats[pid];
                if(ps.pid == 0){ ps.pid = pid; ps.cmd = (pid==0?QString("[kernel]"):readCmdline(pid)); }
                if(pid != 0){
                    if(sport > 1024 && dport <= 1024) ps.outBytes += len;
                    else if(dport > 1024 && sport <= 1024) ps.inBytes += len;
                    else {
                        // both ephemeral or both well-known: attribute to total (split)
                        ps.outBytes += len/2;
                        ps.inBytes += len - len/2;
                    }
                } else {
                    // unknown pid: attribute to kernel aggregate (pid 0)
                    ps.outBytes += len/2;
                    ps.inBytes += len - len/2;
                }
                ps.lastSeen = QDateTime::currentDateTimeUtc();
            }, (u_char*)this);
            msleep(20);
        }
        pcap_close(p);
    }

    void stop(){ running = false; }
};

int main(int argc, char **argv){
    QApplication app(argc, argv);

    QWidget mainWin;
    QVBoxLayout *layout = new QVBoxLayout(&mainWin);
    QLabel *title = new QLabel("---- nettop ----");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    NetModel model;
    QTableView *view = new QTableView;
    view->setModel(&model);
    view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSortingEnabled(false);
    layout->addWidget(view);

    mainWin.resize(1000, 600);
    mainWin.show();

    // threads
    ProcMapper mapper;
    mapper.start();

    CaptureThread capture(&mapper);
    capture.start();

    // UI timer (default 50 ms). Set to 10 for 10ms refresh if you accept CPU cost.
    int uiIntervalMs = 50;
    QTimer uiTimer;
    uiTimer.setInterval(uiIntervalMs);
    QObject::connect(&uiTimer, &QTimer::timeout, [&](){
        // snapshot
        QMap<pid_t, ProcStat> snapshot;
        {
            QMutexLocker locker(&capture.statsMutex);
            snapshot = capture.stats;
        }

        // compute rates: move prev counters if not present
        for(auto it = snapshot.begin(); it != snapshot.end(); ++it){
            if(it->cmd.isEmpty()) it->cmd = (it->pid==0?QString("[kernel]"):readCmdline(it->pid));
            if(!it->lastSeen.isValid()) it->lastSeen = QDateTime::currentDateTimeUtc();
        }

        // sort and keep top N (optional). We'll keep all but sort in model by total bytes by rebuilding a QMap keyed by pid.
        // For simplicity, model.applySnapshot will append new rows; we rely on view sorting visually by total bytes if needed.
        model.applySnapshot(snapshot);
        view->resizeColumnsToContents();
    });
    uiTimer.start();

    // initial test row so UI is visible
    {
        QMap<pid_t, ProcStat> t;
        ProcStat p; p.pid = 0; p.cmd = "[kernel]"; p.inBytes = 0; p.outBytes = 0; p.lastSeen = QDateTime::currentDateTimeUtc();
        t.insert(0, p);
        model.applySnapshot(t);
    }

    int ret = app.exec();

    // shutdown
    capture.stop();
    capture.wait();
    mapper.stop();
    mapper.wait();

    return ret;
}

#include "nettop.moc"
