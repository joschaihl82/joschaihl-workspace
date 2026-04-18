// main.cpp
// Qt5 application: nettop-style table with columns:
// Path/Name | Input MB | Output MB | Input KB/s | Output KB/s | Last seen
// Updates every second. Uses libpcap for packet capture and basic parsing.

#include <QApplication>
#include <QMainWindow>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QDateTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDebug>

#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <map>
#include <QString>
#include <QMap>
#include <QList>
#include <atomic>
#include <cstring>

// ---------- Config ----------
static const char *DEFAULT_IFACE = "wlan0";
static const char *DEFAULT_FILTER = "";
static const int UPDATE_INTERVAL_MS = 1000; // 1 second
static const int DNS_CACHE_TTL = 60;

// ---------- Helpers ----------
static QString ipToString(uint32_t ip_net) {
    struct in_addr a; a.s_addr = ip_net;
    char buf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &a, buf, sizeof(buf));
    return QString::fromUtf8(buf);
}

// ---------- Connection key & stats ----------
struct ConnKey {
    QString pathName;   // application or path (e.g., process name)
    QString src;        // "ip:port"
    QString dst;        // "ip:port"
    bool operator<(const ConnKey &o) const {
        if (pathName != o.pathName) return pathName < o.pathName;
        if (src != o.src) return src < o.src;
        return dst < o.dst;
    }
};

struct ConnStats {
    qint64 bytes_in = 0;
    qint64 bytes_out = 0;
    qint64 last_bytes_in = 0;
    qint64 last_bytes_out = 0;
    double mb_in = 0.0;
    double mb_out = 0.0;
    double kbs_in = 0.0;  // KB/s
    double kbs_out = 0.0; // KB/s
    qint64 last_seen = 0;
};

// ---------- Best-effort process lookup (same approach as earlier) ----------
static bool readFileLine(const QString &path, QString &out) {
    FILE *f = fopen(path.toUtf8().constData(), "r");
    if (!f) return false;
    char buf[512];
    if (!fgets(buf, sizeof(buf), f)) { fclose(f); return false; }
    fclose(f);
    size_t len = strlen(buf);
    if (len && buf[len-1] == '\n') buf[len-1] = '\0';
    out = QString::fromUtf8(buf);
    return true;
}

static bool findInodeForPort(const char *proto, int port, QString &inodeOut) {
    char path[64];
    snprintf(path, sizeof(path), "/proc/net/%s", proto);
    FILE *f = fopen(path, "r");
    if (!f) return false;
    char line[1024];
    if (!fgets(line, sizeof(line), f)) { fclose(f); return false; }
    while (fgets(line, sizeof(line), f)) {
        unsigned long inode = 0;
        char local_addr[64];
        int items = sscanf(line, "%*d: %63s %*s %*s %*s %*s %*s %*s %*s %lu", local_addr, &inode);
        if (items >= 2) {
            char *colon = strchr(local_addr, ':');
            if (!colon) continue;
            unsigned int p;
            if (sscanf(colon + 1, "%x", &p) != 1) continue;
            if ((int)p == port) {
                inodeOut = QString::number(inode);
                fclose(f);
                return true;
            }
        }
    }
    fclose(f);
    return false;
}

static bool findProcessByInode(const QString &inode, QString &procNameOut) {
    DIR *pdir = opendir("/proc");
    if (!pdir) return false;
    struct dirent *dent;
    while ((dent = readdir(pdir)) != NULL) {
        char *endptr;
        long pid = strtol(dent->d_name, &endptr, 10);
        if (*endptr != '\0') continue;
        char fdpath[PATH_MAX];
        snprintf(fdpath, sizeof(fdpath), "/proc/%ld/fd", pid);
        DIR *fddir = opendir(fdpath);
        if (!fddir) continue;
        struct dirent *fdent;
        while ((fdent = readdir(fddir)) != NULL) {
            if (strcmp(fdent->d_name, ".") == 0 || strcmp(fdent->d_name, "..") == 0) continue;
            char linkpath[PATH_MAX];
            char target[PATH_MAX];
            snprintf(linkpath, sizeof(linkpath), "%s/%s", fdpath, fdent->d_name);
            ssize_t r = readlink(linkpath, target, sizeof(target)-1);
            if (r <= 0) continue;
            target[r] = '\0';
            char want[64];
            snprintf(want, sizeof(want), "socket:[%s]", inode.toUtf8().constData());
            if (strcmp(target, want) == 0) {
                char commpath[PATH_MAX];
                snprintf(commpath, sizeof(commpath), "/proc/%ld/comm", pid);
                QString comm;
                if (readFileLine(QString::fromUtf8(commpath), comm)) {
                    procNameOut = comm;
                    closedir(fddir);
                    closedir(pdir);
                    return true;
                } else {
                    char cmdpath[PATH_MAX];
                    snprintf(cmdpath, sizeof(cmdpath), "/proc/%ld/cmdline", pid);
                    FILE *cf = fopen(cmdpath, "r");
                    if (cf) {
                        char buf[512];
                        if (fgets(buf, sizeof(buf), cf)) {
                            for (size_t i = 0; i < sizeof(buf); ++i) if (buf[i] == '\0') buf[i] = ' ';
                            procNameOut = QString::fromUtf8(buf);
                            fclose(cf);
                            closedir(fddir);
                            closedir(pdir);
                            return true;
                        }
                        fclose(cf);
                    }
                    closedir(fddir);
                    closedir(pdir);
                    return false;
                }
            }
        }
        closedir(fddir);
    }
    closedir(pdir);
    return false;
}

static QString getProcessNameForLocalPort(const char *proto, int port) {
    QString inode;
    if (findInodeForPort(proto, port, inode)) {
        QString pname;
        if (findProcessByInode(inode, pname)) return pname;
    }
    return QString("-");
}

// ---------- Packet capture worker ----------
class PcapWorker : public QThread {
    Q_OBJECT
public:
    PcapWorker(const QString &iface, const QString &filterExpr)
        : iface_(iface), filterExpr_(filterExpr), handle_(nullptr) {}
    void stop() {
        if (handle_) pcap_breakloop(handle_);
    }

signals:
    void packetParsed(const ConnKey &key, qint64 bytes_in, qint64 bytes_out, qint64 ts);

    void error(const QString &msg);

protected:
    void run() override {
        char errbuf[PCAP_ERRBUF_SIZE];
        handle_ = pcap_open_live(iface_.toUtf8().constData(), 65535, 1, 1000, errbuf);
        if (!handle_) { emit error(QString("pcap_open_live failed: %1").arg(errbuf)); return; }
        if (!filterExpr_.isEmpty()) {
            struct bpf_program fp;
            if (pcap_compile(handle_, &fp, filterExpr_.toUtf8().constData(), 1, PCAP_NETMASK_UNKNOWN) == -1 ||
                pcap_setfilter(handle_, &fp) == -1) {
                emit error(QString("filter error: %1").arg(pcap_geterr(handle_)));
                pcap_freecode(&fp);
                pcap_close(handle_);
                handle_ = nullptr;
                return;
            }
            pcap_freecode(&fp);
        }
        pcap_loop(handle_, -1, &PcapWorker::pcapCallback, reinterpret_cast<u_char*>(this));
        if (handle_) { pcap_close(handle_); handle_ = nullptr; }
    }

private:
    QString iface_;
    QString filterExpr_;
    pcap_t *handle_;

    static void pcapCallback(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes) {
        PcapWorker *self = reinterpret_cast<PcapWorker*>(user);
        if (!self) return;
        self->processPacket(h, bytes);
    }

    void processPacket(const struct pcap_pkthdr *h, const u_char *bytes) {
        if (!h || !bytes) return;
        if (h->caplen < sizeof(struct ether_header)) return;
        const struct ether_header *eth = (const struct ether_header*)bytes;
        uint16_t eth_type = ntohs(eth->ether_type);
        if (eth_type != ETHERTYPE_IP) return;
        size_t ip_off = sizeof(struct ether_header);
        if (h->caplen < ip_off + sizeof(struct ip)) return;
        const struct ip *ip = (const struct ip*)(bytes + ip_off);
        int ihl = ip->ip_hl * 4;
        if (ihl < 20) return;
        if (h->caplen < ip_off + ihl) return;
        uint32_t src_ip = ip->ip_src.s_addr;
        uint32_t dst_ip = ip->ip_dst.s_addr;
        qint64 ts = (qint64)h->ts.tv_sec;

        if (ip->ip_p == IPPROTO_TCP) {
            size_t tcp_off = ip_off + ihl;
            if (h->caplen < tcp_off + sizeof(struct tcphdr)) return;
            const struct tcphdr *tcp = (const struct tcphdr*)(bytes + tcp_off);
            int tcp_hdr_len = tcp->th_off * 4;
            if (tcp_hdr_len < 20) return;
            if (h->caplen < tcp_off + tcp_hdr_len) return;
            size_t payload_off = tcp_off + tcp_hdr_len;
            size_t payload_len = (h->caplen > payload_off) ? (h->caplen - payload_off) : 0;
            unsigned short sport = ntohs(tcp->th_sport);
            unsigned short dport = ntohs(tcp->th_dport);

            // best-effort process name
            QString proc = getProcessNameForLocalPort("tcp", dport);
            bool localIsDst = (proc != "-");
            if (!localIsDst) {
                proc = getProcessNameForLocalPort("tcp", sport);
                localIsDst = (proc != "-");
            }
            QString src = ipToString(src_ip) + ":" + QString::number(sport);
            QString dst = ipToString(dst_ip) + ":" + QString::number(dport);
            ConnKey key{proc, src, dst};
            qint64 bin = localIsDst ? (qint64)payload_len : 0;
            qint64 bout = localIsDst ? 0 : (qint64)payload_len;
            emit packetParsed(key, bin, bout, ts);
        } else if (ip->ip_p == IPPROTO_UDP) {
            size_t udp_off = ip_off + ihl;
            if (h->caplen < udp_off + sizeof(struct udphdr)) return;
            const struct udphdr *udp = (const struct udphdr*)(bytes + udp_off);
            size_t payload_off = udp_off + sizeof(struct udphdr);
            size_t payload_len = (h->caplen > payload_off) ? (h->caplen - payload_off) : 0;
            unsigned short sport = ntohs(udp->uh_sport);
            unsigned short dport = ntohs(udp->uh_dport);

            QString proc = getProcessNameForLocalPort("udp", dport);
            bool localIsDst = (proc != "-");
            if (!localIsDst) {
                proc = getProcessNameForLocalPort("udp", sport);
                localIsDst = (proc != "-");
            }
            QString src = ipToString(src_ip) + ":" + QString::number(sport);
            QString dst = ipToString(dst_ip) + ":" + QString::number(dport);
            ConnKey key{proc, src, dst};
            qint64 bin = localIsDst ? (qint64)payload_len : 0;
            qint64 bout = localIsDst ? 0 : (qint64)payload_len;
            emit packetParsed(key, bin, bout, ts);
        }
    }
};

// ---------- MainWindow ----------
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("nettop_qt");
        resize(800, 400);

        QWidget *central = new QWidget(this);
        QVBoxLayout *vbox = new QVBoxLayout(central);

        QHBoxLayout *hbox = new QHBoxLayout();
        hbox->addWidget(new QLabel("Interface:"));
        ifaceEdit_ = new QLineEdit(DEFAULT_IFACE);
        hbox->addWidget(ifaceEdit_);
        hbox->addWidget(new QLabel("Filter:"));
        filterEdit_ = new QLineEdit(DEFAULT_FILTER);
        hbox->addWidget(filterEdit_);
        startBtn_ = new QPushButton("Start");
        stopBtn_ = new QPushButton("Stop");
        stopBtn_->setEnabled(false);
        hbox->addWidget(startBtn_);
        hbox->addWidget(stopBtn_);
        vbox->addLayout(hbox);

        tableView_ = new QTableView(this);
        model_ = new QStandardItemModel(this);
        QStringList headers = {"Path/Name", "Input MB", "Output MB", "Input KB/s", "Output KB/s", "Last seen"};
        model_->setHorizontalHeaderLabels(headers);
        tableView_->setModel(model_);
        tableView_->setSortingEnabled(true);
        tableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        vbox->addWidget(tableView_);

        setCentralWidget(central);

        // pre-populate with [kernel] row (zeros)
        QList<QStandardItem*> items;
        items << new QStandardItem("[kernel]");
        items << new QStandardItem(QString::number(0.0, 'f', 2));
        items << new QStandardItem(QString::number(0.0, 'f', 2));
        items << new QStandardItem(QString::number(0.0, 'f', 2));
        items << new QStandardItem(QString::number(0.0, 'f', 2));
        items << new QStandardItem(QDateTime::currentDateTime().toString("HH:mm:ss"));
        model_->appendRow(items);

        uiTimer_ = new QTimer(this);
        uiTimer_->setInterval(UPDATE_INTERVAL_MS);
        connect(uiTimer_, &QTimer::timeout, this, &MainWindow::onUiTimer);

        connect(startBtn_, &QPushButton::clicked, this, &MainWindow::onStart);
        connect(stopBtn_, &QPushButton::clicked, this, &MainWindow::onStop);

        worker_ = nullptr;
    }

    ~MainWindow() { stopWorker(); }

public slots:
    void onStart() {
        if (worker_) return;
        QString iface = ifaceEdit_->text().trimmed();
        if (iface.isEmpty()) iface = DEFAULT_IFACE;
        QString filter = filterEdit_->text();
        worker_ = new PcapWorker(iface, filter);
        connect(worker_, &PcapWorker::packetParsed, this, &MainWindow::onPacketParsed, Qt::QueuedConnection);
        connect(worker_, &PcapWorker::error, this, &MainWindow::onWorkerError, Qt::QueuedConnection);
        worker_->start();
        startBtn_->setEnabled(false);
        stopBtn_->setEnabled(true);
        uiTimer_->start();
    }

    void onStop() {
        stopWorker();
        startBtn_->setEnabled(true);
        stopBtn_->setEnabled(false);
        uiTimer_->stop();
    }

    void onWorkerError(const QString &msg) { qWarning() << "Worker error:" << msg; }

    void onPacketParsed(const ConnKey &key, qint64 bin, qint64 bout, qint64 ts) {
        QMutexLocker locker(&mapMutex_);
        ConnStats &s = stats_[key];
        s.bytes_in += bin;
        s.bytes_out += bout;
        s.last_seen = ts;
    }

    void onUiTimer() {
        qint64 now = QDateTime::currentSecsSinceEpoch();
        QMutexLocker locker(&mapMutex_);
        model_->removeRows(0, model_->rowCount());
        // ensure [kernel] row exists if no data
        if (stats_.isEmpty()) {
            QList<QStandardItem*> items;
            items << new QStandardItem("[kernel]");
            items << new QStandardItem(QString::number(0.0, 'f', 2));
            items << new QStandardItem(QString::number(0.0, 'f', 2));
            items << new QStandardItem(QString::number(0.0, 'f', 2));
            items << new QStandardItem(QString::number(0.0, 'f', 2));
            items << new QStandardItem(QDateTime::currentDateTime().toString("HH:mm:ss"));
            model_->appendRow(items);
            return;
        }
        for (auto it = stats_.begin(); it != stats_.end(); ++it) {
            const ConnKey &k = it.key();
            ConnStats &s = it.value();
            s.mb_in = s.bytes_in / (1024.0 * 1024.0);
            s.mb_out = s.bytes_out / (1024.0 * 1024.0);
            qint64 delta_in = s.bytes_in - s.last_bytes_in;
            qint64 delta_out = s.bytes_out - s.last_bytes_out;
            s.kbs_in = (delta_in / 1024.0) * (1000.0 / UPDATE_INTERVAL_MS);
            s.kbs_out = (delta_out / 1024.0) * (1000.0 / UPDATE_INTERVAL_MS);
            s.last_bytes_in = s.bytes_in;
            s.last_bytes_out = s.bytes_out;

            QList<QStandardItem*> items;
            items << new QStandardItem(k.pathName.isEmpty() ? "-" : k.pathName);
            items << new QStandardItem(QString::number(s.mb_in, 'f', 2));
            items << new QStandardItem(QString::number(s.mb_out, 'f', 2));
            items << new QStandardItem(QString::number(s.kbs_in, 'f', 2));
            items << new QStandardItem(QString::number(s.kbs_out, 'f', 2));
            items << new QStandardItem(QDateTime::fromSecsSinceEpoch(s.last_seen).toString("HH:mm:ss"));
            model_->appendRow(items);
        }
    }

private:
    void stopWorker() {
        if (worker_) {
            worker_->stop();
            worker_->wait(2000);
            delete worker_;
            worker_ = nullptr;
        }
    }

    QLineEdit *ifaceEdit_;
    QLineEdit *filterEdit_;
    QPushButton *startBtn_;
    QPushButton *stopBtn_;
    QTableView *tableView_;
    QStandardItemModel *model_;
    QTimer *uiTimer_;
    PcapWorker *worker_;
    QMutex mapMutex_;
    QMap<ConnKey, ConnStats> stats_;
};

// ---------- main ----------
int main(int argc, char **argv) {
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}

#include "nettop.moc"
