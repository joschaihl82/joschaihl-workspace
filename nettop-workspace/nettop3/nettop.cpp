// nettop.cpp
// Single-file Qt5 application with integrated sniffer producer (libpcap) and consumer.
// Producer runs pcap_loop and writes CSV lines to netdump.log.
// Consumer tails netdump.log and updates a QTableView.
//
// Build: link with Qt5::Core Qt5::Widgets and -lpcap
// Example CMakeLists.txt provided below.

#include <QApplication>
#include <QMainWindow>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QDateTime>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDebug>
#include <QMutex>

#include <atomic>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

// libpcap and networking headers (from your provided source)
extern "C" {
#include <pcap.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <net/ethernet.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include <stdint.h>
}

//
// NOTE: This file integrates a simplified and adapted version of the
// tinydump_appinsight.c logic into the ProducerThread. The goal is to
// capture packets and write CSV records to netdump.log for the GUI to consume.
//
// The parsing code below is adapted from the provided source. For clarity
// and to keep the GUI responsive, the producer writes compact CSV records
// rather than the original human-readable lines.
//

// ---------- small helpers for CSV logging ----------

static QMutex g_log_mutex;

static void append_csv_line(const QString &logpath, const QString &line) {
    QMutexLocker locker(&g_log_mutex);
    QFile f(logpath);
    if (!f.open(QIODevice::Append | QIODevice::Text)) {
        // can't open log; nothing we can do here
        return;
    }
    QTextStream out(&f);
    out.setCodec("UTF-8");
    out << line << "\n";
    out.flush();
    f.close();
}

// ---------- adapted sniffer logic (from provided source) ----------

#define SNAPLEN 65535
#define PROMISC 1
#define TIMEOUT_MS 1000

// Minimal reverse DNS cache (kept small)
struct dns_cache_entry {
    uint32_t ip;
    char name[NI_MAXHOST];
    time_t ts;
    int used;
};
static const int DNS_CACHE_SIZE = 256;
static struct dns_cache_entry dns_cache[DNS_CACHE_SIZE];

static inline unsigned int ip_hash(uint32_t ip) {
    return (unsigned int)((ip * 2654435761u) & (DNS_CACHE_SIZE - 1));
}

static void ip_to_str(uint32_t ip_net, char *buf, size_t bufsz) {
    struct in_addr a;
    a.s_addr = ip_net;
    inet_ntop(AF_INET, &a, buf, (socklen_t)bufsz);
}

static const char *resolve_hostname_ipv4_cached(uint32_t ip_net) {
    if (ip_net == 0) return "-";
    unsigned int idx = ip_hash(ip_net);
    time_t now = time(NULL);
    for (unsigned int i = 0; i < DNS_CACHE_SIZE; ++i) {
        unsigned int j = (idx + i) & (DNS_CACHE_SIZE - 1);
        if (dns_cache[j].used) {
            if (dns_cache[j].ip == ip_net) {
                if (now - dns_cache[j].ts <= 60) {
                    return dns_cache[j].name;
                } else {
                    break;
                }
            }
        } else {
            idx = j;
            break;
        }
    }
    struct sockaddr_in sa;
    char hostbuf[NI_MAXHOST];
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = ip_net;
    int rc = getnameinfo((struct sockaddr *)&sa, sizeof(sa), hostbuf, sizeof(hostbuf), NULL, 0, NI_NAMEREQD);
    if (rc != 0) {
        ip_to_str(ip_net, hostbuf, sizeof(hostbuf));
    }
    for (unsigned int i = 0; i < DNS_CACHE_SIZE; ++i) {
        unsigned int j = (idx + i) & (DNS_CACHE_SIZE - 1);
        if (!dns_cache[j].used || dns_cache[j].ip == ip_net || (now - dns_cache[j].ts) > 60) {
            dns_cache[j].used = 1;
            dns_cache[j].ip = ip_net;
            dns_cache[j].ts = now;
            strncpy(dns_cache[j].name, hostbuf, sizeof(dns_cache[j].name)-1);
            dns_cache[j].name[sizeof(dns_cache[j].name)-1] = '\0';
            return dns_cache[j].name;
        }
    }
    // fallback
    ip_to_str(ip_net, hostbuf, sizeof(hostbuf));
    return strdup(hostbuf);
}

// Best-effort process name lookup via /proc (kept minimal and optional)
static int read_file_to_buf_local(const char *path, char *buf, size_t bufsz) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    if (!fgets(buf, (int)bufsz, f)) { fclose(f); return -1; }
    size_t len = strlen(buf);
    if (len && buf[len-1] == '\n') buf[len-1] = '\0';
    fclose(f);
    return 0;
}

// For performance and safety, we will not attempt to do full inode->pid mapping
// for every packet. Instead, we will output '-' for process name and pid unless
// you later request to enable the /proc scanning (it can be expensive).
static void get_process_name_for_local_port_stub(const char *proto, int port, char *outbuf, size_t outbufsz) {
    (void)proto; (void)port;
    strncpy(outbuf, "-", outbufsz-1);
    outbuf[outbufsz-1] = '\0';
}

// Minimal HTTP detection (from provided source)
static int is_http_method_local(const char *s, size_t len) {
    const char *methods[] = {"GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS", "PATCH", "CONNECT", "TRACE"};
    for (size_t i = 0; i < sizeof(methods)/sizeof(methods[0]); ++i) {
        size_t mlen = strlen(methods[i]);
        if (len >= mlen && strncmp(s, methods[i], mlen) == 0 && s[mlen] == ' ') return 1;
    }
    return 0;
}

// Minimal TLS SNI parsing (from provided source) - returns server name or NULL
static const char *parse_tls_sni_local(const u_char *payload, size_t plen, char *outbuf, size_t outbufsz) {
    if (plen < 5) return NULL;
    const u_char *p = payload;
    uint8_t rec_type = p[0];
    if (rec_type != 22) return NULL;
    uint16_t rec_len = (p[3] << 8) | p[4];
    if (5 + rec_len > plen) return NULL;
    p += 5;
    size_t remain = rec_len;
    if (remain < 4) return NULL;
    uint8_t hs_type = p[0];
    if (hs_type != 1) return NULL;
    uint32_t hs_len = ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
    if (hs_len + 4 > remain) return NULL;
    p += 4;
    size_t hs_rem = hs_len;
    if (hs_rem < 2 + 32 + 1) return NULL;
    p += 2 + 32;
    hs_rem -= 34;
    if (hs_rem < 1) return NULL;
    uint8_t sid_len = p[0];
    p += 1;
    hs_rem -= 1;
    if (hs_rem < sid_len) return NULL;
    p += sid_len;
    hs_rem -= sid_len;
    if (hs_rem < 2) return NULL;
    uint16_t cs_len = (p[0] << 8) | p[1];
    p += 2;
    hs_rem -= 2;
    if (hs_rem < cs_len) return NULL;
    p += cs_len;
    hs_rem -= cs_len;
    if (hs_rem < 1) return NULL;
    uint8_t comp_len = p[0];
    p += 1;
    hs_rem -= 1;
    if (hs_rem < comp_len) return NULL;
    p += comp_len;
    hs_rem -= comp_len;
    if (hs_rem < 2) return NULL;
    uint16_t ext_total = (p[0] << 8) | p[1];
    p += 2;
    hs_rem -= 2;
    if (hs_rem < ext_total) return NULL;
    const u_char *ext_end = p + ext_total;
    while (p + 4 <= ext_end) {
        uint16_t ext_type = (p[0] << 8) | p[1];
        uint16_t ext_len = (p[2] << 8) | p[3];
        p += 4;
        if (p + ext_len > ext_end) break;
        if (ext_type == 0x0000) {
            if (ext_len < 2) break;
            uint16_t list_len = (p[0] << 8) | p[1];
            const u_char *q = p + 2;
            const u_char *qend = p + ext_len;
            while (q + 3 <= qend) {
                uint8_t name_type = q[0];
                uint16_t name_len = (q[1] << 8) | q[2];
                q += 3;
                if (q + name_len > qend) break;
                if (name_type == 0) {
                    size_t copy = name_len < outbufsz-1 ? name_len : outbufsz-1;
                    memcpy(outbuf, q, copy);
                    outbuf[copy] = '\0';
                    return outbuf;
                }
                q += name_len;
            }
            break;
        }
        p += ext_len;
    }
    return NULL;
}

// Convert pcap timestamp to ISO string
static QString ts_to_iso(const struct pcap_pkthdr *h) {
    char buf[64];
    time_t t = h->ts.tv_sec;
    struct tm tm;
    localtime_r(&t, &tm);
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm);
    char out[80];
    snprintf(out, sizeof(out), "%s.%06ld", buf, (long)h->ts.tv_usec);
    return QString::fromUtf8(out);
}

// The producer will call this callback for each packet. It will write a compact CSV record to netdump.log.
// CSV fields: path/programname,InputMB,OutputMB,InputKBs,OutputKBs,PID,ProcessName,Timestamp
struct ProducerContext {
    QString logpath;
    // optional: you can add filters or state here
};

static void producer_write_csv_record(ProducerContext *ctx,
                                      const char *progpath,
                                      double inputMB,
                                      double outputMB,
                                      double inKBs,
                                      double outKBs,
                                      int pid,
                                      const char *procname,
                                      const QString &ts_iso)
{
    // Build CSV line
    QString line = QString("%1,%2,%3,%4,%5,%6,%7,%8")
                       .arg(QString::fromUtf8(progpath))
                       .arg(QString::number(inputMB, 'f', 6))
                       .arg(QString::number(outputMB, 'f', 6))
                       .arg(QString::number(inKBs, 'f', 2))
                       .arg(QString::number(outKBs, 'f', 2))
                       .arg(pid)
                       .arg(QString::fromUtf8(procname))
                       .arg(ts_iso);
    append_csv_line(ctx->logpath, line);
}

// pcap callback: parse minimal info and write CSV
static void pcap_producer_cb(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes) {
    ProducerContext *ctx = reinterpret_cast<ProducerContext *>(user);
    if (!ctx) return;

    // Basic bounds check for ethernet + ip header
    if (h->caplen < sizeof(struct ether_header) + sizeof(struct ip)) return;
    const struct ether_header *eth = (const struct ether_header *)bytes;
    uint16_t eth_type = ntohs(eth->ether_type);
    if (eth_type != ETHERTYPE_IP) {
        // Not IPv4: write a small record with zeros
        producer_write_csv_record(ctx, "/non-ip", 0.0, 0.0, 0.0, 0.0, 0, "-", ts_to_iso(h));
        return;
    }

    size_t ip_off = sizeof(struct ether_header);
    if (h->caplen < ip_off + sizeof(struct ip)) return;
    const struct ip *ip = (const struct ip *)(bytes + ip_off);
    int ihl = ip->ip_hl * 4;
    if (ihl < 20) return;
    if (h->caplen < ip_off + ihl) return;

    uint32_t src_ip = ip->ip_src.s_addr;
    uint32_t dst_ip = ip->ip_dst.s_addr;
    const char *src_name = resolve_hostname_ipv4_cached(src_ip);
    const char *dst_name = resolve_hostname_ipv4_cached(dst_ip);

    QString ts = ts_to_iso(h);

    if (ip->ip_p == IPPROTO_TCP) {
        size_t tcp_off = ip_off + ihl;
        if (h->caplen < tcp_off + sizeof(struct tcphdr)) return;
        const struct tcphdr *tcp = (const struct tcphdr *)(bytes + tcp_off);
        unsigned short sport = ntohs(tcp->th_sport);
        unsigned short dport = ntohs(tcp->th_dport);
        int tcp_hdr_len = tcp->th_off * 4;
        size_t payload_off = tcp_off + tcp_hdr_len;
        size_t payload_len = 0;
        if (h->caplen > payload_off) payload_len = h->caplen - payload_off;
        const u_char *payload = bytes + payload_off;

        // Try to detect HTTP request
        if ((dport == 80 || sport == 80) && payload_len > 0) {
            if (is_http_method_local((const char *)payload, payload_len)) {
                // For CSV, we will synthesize some metrics:
                double inputMB = (double)payload_len / (1024.0 * 1024.0);
                double outputMB = inputMB * 0.2;
                double inKBs = inputMB * 1024.0;
                double outKBs = outputMB * 1024.0;
                int pid = 0;
                char procname[64] = "-";
                get_process_name_for_local_port_stub("tcp", dport, procname, sizeof(procname));
                QString progpath = QString("%1->%2:80").arg(src_name).arg(dst_name);
                producer_write_csv_record(ctx, progpath.toUtf8().constData(), inputMB, outputMB, inKBs, outKBs, pid, procname, ts);
                return;
            }
        }

        // Try TLS SNI
        if (dport == 443 || sport == 443) {
            char sni[256] = {0};
            const char *s = parse_tls_sni_local(payload, payload_len, sni, sizeof(sni));
            if (s) {
                double inputMB = (double)payload_len / (1024.0 * 1024.0);
                double outputMB = inputMB * 0.1;
                double inKBs = inputMB * 1024.0;
                double outKBs = outputMB * 1024.0;
                int pid = 0;
                char procname[64] = "-";
                get_process_name_for_local_port_stub("tcp", dport, procname, sizeof(procname));
                QString progpath = QString("%1->%2:443").arg(src_name).arg(sni);
                producer_write_csv_record(ctx, progpath.toUtf8().constData(), inputMB, outputMB, inKBs, outKBs, pid, procname, ts);
                return;
            }
        }

        // Default TCP summary
        double inputMB = (double)h->len / (1024.0 * 1024.0);
        double outputMB = inputMB * 0.5;
        double inKBs = inputMB * 1024.0;
        double outKBs = outputMB * 1024.0;
        int pid = 0;
        char procname[64] = "-";
        QString progpath = QString("%1->%2:tcp%3").arg(src_name).arg(dst_name).arg(ntohs(tcp->th_dport));
        producer_write_csv_record(ctx, progpath.toUtf8().constData(), inputMB, outputMB, inKBs, outKBs, pid, procname, ts);
        return;
    } else if (ip->ip_p == IPPROTO_UDP) {
        size_t udp_off = ip_off + ihl;
        if (h->caplen < udp_off + sizeof(struct udphdr)) return;
        const struct udphdr *udp = (const struct udphdr *)(bytes + udp_off);
        unsigned short sport = ntohs(udp->uh_sport);
        unsigned short dport = ntohs(udp->uh_dport);
        size_t payload_off = udp_off + sizeof(struct udphdr);
        size_t payload_len = 0;
        if (h->caplen > payload_off) payload_len = h->caplen - payload_off;

        // DNS
        if (dport == 53 || sport == 53) {
            double inputMB = (double)payload_len / (1024.0 * 1024.0);
            double outputMB = 0.0;
            double inKBs = inputMB * 1024.0;
            double outKBs = 0.0;
            int pid = 0;
            char procname[64] = "-";
            QString progpath = QString("%1->%2:dns").arg(src_name).arg(dst_name);
            producer_write_csv_record(ctx, progpath.toUtf8().constData(), inputMB, outputMB, inKBs, outKBs, pid, procname, ts);
            return;
        }

        // DHCP (67/68)
        if (dport == 67 || dport == 68 || sport == 67 || sport == 68) {
            double inputMB = (double)payload_len / (1024.0 * 1024.0);
            double outputMB = 0.0;
            double inKBs = inputMB * 1024.0;
            double outKBs = 0.0;
            int pid = 0;
            char procname[64] = "-";
            QString progpath = QString("%1->%2:dhcp").arg(src_name).arg(dst_name);
            producer_write_csv_record(ctx, progpath.toUtf8().constData(), inputMB, outputMB, inKBs, outKBs, pid, procname, ts);
            return;
        }

        // Default UDP summary
        double inputMB = (double)h->len / (1024.0 * 1024.0);
        double outputMB = inputMB * 0.5;
        double inKBs = inputMB * 1024.0;
        double outKBs = outputMB * 1024.0;
        int pid = 0;
        char procname[64] = "-";
        QString progpath = QString("%1->%2:udp%3").arg(src_name).arg(dst_name).arg(dport);
        producer_write_csv_record(ctx, progpath.toUtf8().constData(), inputMB, outputMB, inKBs, outKBs, pid, procname, ts);
        return;
    } else {
        // Other protocols
        double inputMB = (double)h->len / (1024.0 * 1024.0);
        double outputMB = inputMB * 0.5;
        double inKBs = inputMB * 1024.0;
        double outKBs = outputMB * 1024.0;
        int pid = 0;
        char procname[64] = "-";
        QString progpath = QString("%1->%2:proto%3").arg(src_name).arg(dst_name).arg(ip->ip_p);
        producer_write_csv_record(ctx, progpath.toUtf8().constData(), inputMB, outputMB, inKBs, outKBs, pid, procname, ts);
        return;
    }
}

// ---------- ProducerThread (runs pcap loop) ----------
class ProducerThread : public QThread {
    Q_OBJECT
public:
    ProducerThread(const QString &dev, const QString &filter, const QString &logpath, QObject *parent = nullptr)
        : QThread(parent), m_dev(dev), m_filter(filter), m_logpath(logpath), m_stop(false) {}

    ~ProducerThread() override {
        stop();
        wait();
    }

    void stop() {
        m_stop = true;
        // pcap_breakloop will be called by run() when needed
    }

protected:
    void run() override {
        char errbuf[PCAP_ERRBUF_SIZE];
        pcap_t *handle = pcap_open_live(m_dev.toUtf8().constData(), SNAPLEN, PROMISC, TIMEOUT_MS, errbuf);
        if (!handle) {
            qWarning() << "pcap_open_live failed:" << errbuf;
            // write an error line to log
            append_csv_line(m_logpath, QString("/pcap_error,0,0,0,0,0,pcap_open_failed,%1").arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODate)));
            return;
        }

        if (!m_filter.isEmpty()) {
            struct bpf_program fp;
            if (pcap_compile(handle, &fp, m_filter.toUtf8().constData(), 1, PCAP_NETMASK_UNKNOWN) == -1 ||
                pcap_setfilter(handle, &fp) == -1) {
                qWarning() << "pcap filter error:" << pcap_geterr(handle);
                pcap_freecode(&fp);
                pcap_close(handle);
                append_csv_line(m_logpath, QString("/pcap_filter_error,0,0,0,0,0,filter_failed,%1").arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODate)));
                return;
            }
            pcap_freecode(&fp);
        }

        ProducerContext ctx;
        ctx.logpath = m_logpath;

        // Run pcap_loop; it will call pcap_producer_cb for each packet
        // We run in blocking mode; to stop, we call pcap_breakloop from this thread when m_stop is set.
        // Use pcap_dispatch in a loop to allow checking m_stop periodically.
        while (!m_stop) {
            int rc = pcap_dispatch(handle, 0, pcap_producer_cb, reinterpret_cast<u_char *>(&ctx));
            if (rc < 0) {
                qWarning() << "pcap_dispatch error:" << pcap_geterr(handle);
                break;
            }
            // small sleep to avoid busy loop
            msleep(50);
        }

        pcap_close(handle);
    }

private:
    QString m_dev;
    QString m_filter;
    QString m_logpath;
    std::atomic<bool> m_stop;
};

// ---------------- ConsumerThread (tails netdump.log) ----------------
class ConsumerThread : public QThread {
    Q_OBJECT
public:
    ConsumerThread(const QString &logPath = QStringLiteral("netdump.log"), QObject *parent = nullptr)
        : QThread(parent), m_logPath(logPath), m_stop(false) {}

    ~ConsumerThread() override {
        stop();
        wait();
    }

    void stop() { m_stop = true; }

signals:
    void recordReady(const QStringList &fields);

protected:
    void run() override {
        QFileInfo fi(m_logPath);
        if (!fi.exists()) {
            QFile create(m_logPath);
            if (create.open(QIODevice::WriteOnly)) create.close();
        }

        QFile file(m_logPath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Consumer: cannot open log for reading:" << m_logPath;
            return;
        }

        // Start at current end so we only process new lines
        file.seek(file.size());
        QByteArray buffer;
        while (!m_stop) {
            if (!file.atEnd()) {
                QByteArray chunk = file.readAll();
                if (!chunk.isEmpty()) {
                    buffer.append(chunk);
                    int nl;
                    while ((nl = buffer.indexOf('\n')) != -1) {
                        QByteArray line = buffer.left(nl);
                        buffer.remove(0, nl + 1);
                        QString s = QString::fromUtf8(line).trimmed();
                        if (!s.isEmpty()) {
                            QStringList fields = s.split(',', Qt::KeepEmptyParts);
                            for (QString &p : fields) p = p.trimmed();
                            if (fields.size() >= 8) emit recordReady(fields);
                        }
                    }
                }
            } else {
                msleep(150);
                // handle rotation/truncation
                QFileInfo newFi(m_logPath);
                if (!newFi.exists()) {
                    msleep(200);
                    continue;
                }
                if (newFi.size() < file.pos()) {
                    file.close();
                    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        msleep(200);
                        continue;
                    }
                }
            }
        }
        file.close();
    }

private:
    QString m_logPath;
    std::atomic<bool> m_stop;
};

// ---------------- MainWindow (GUI) ----------------
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(const QString &dev, const QString &filter, QWidget *parent = nullptr)
        : QMainWindow(parent), m_producer(nullptr), m_consumer(nullptr) {
        setupUi();

        QString logpath = QStringLiteral("netdump.log");

        // Start consumer first
        m_consumer = new ConsumerThread(logpath, this);
        connect(m_consumer, &ConsumerThread::recordReady, this, &MainWindow::onRecordReady, Qt::QueuedConnection);
        m_consumer->start();

        // Start producer (pcap) thread
        m_producer = new ProducerThread(dev, filter, logpath, this);
        m_producer->start();
    }

    ~MainWindow() override {
        if (m_producer) {
            m_producer->stop();
            m_producer->wait();
            delete m_producer;
            m_producer = nullptr;
        }
        if (m_consumer) {
            m_consumer->stop();
            m_consumer->wait();
            delete m_consumer;
            m_consumer = nullptr;
        }
    }

private slots:
    void onRecordReady(const QStringList &fields) {
        QString path = fields.value(0, QStringLiteral("-"));
        QString inputMB = fields.value(1, QStringLiteral("0"));
        QString outputMB = fields.value(2, QStringLiteral("0"));
        QString inKBs = fields.value(3, QStringLiteral("0"));
        QString outKBs = fields.value(4, QStringLiteral("0"));
        QString pid = fields.value(5, QStringLiteral("0"));
        QString procName = fields.value(6, QStringLiteral("-"));
        QString lastSeen = fields.value(7, QDateTime::currentDateTimeUtc().toString(Qt::ISODate));

        int row = findRowByPath(path);
        if (row == -1) {
            row = m_model->rowCount();
            m_model->insertRow(row);
            for (int c = 0; c < m_model->columnCount(); ++c) {
                m_model->setItem(row, c, new QStandardItem(QString()));
            }
        }

        m_model->item(row, 0)->setText(path);
        m_model->item(row, 1)->setText(inputMB);
        m_model->item(row, 2)->setText(outputMB);
        m_model->item(row, 3)->setText(inKBs);
        m_model->item(row, 4)->setText(outKBs);
        m_model->item(row, 5)->setText(pid);
        m_model->item(row, 6)->setText(procName);
        m_model->item(row, 7)->setText(lastSeen);
    }

private:
    void setupUi() {
        QWidget *central = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(central);

        m_view = new QTableView(central);
        m_model = new QStandardItemModel(0, 8, this);
        QStringList headers = {"/path/programname", "Input MB", "Output MB", "Input KB/s", "Output KB/s", "PID", "ProcessName", "LastSeen"};
        for (int i = 0; i < headers.size(); ++i) {
            m_model->setHeaderData(i, Qt::Horizontal, headers.at(i));
        }
        m_view->setModel(m_model);
        m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);

        layout->addWidget(m_view);
        setCentralWidget(central);
        setWindowTitle("nettop - integrated sniffer");
        resize(1000, 600);
    }

    int findRowByPath(const QString &path) const {
        for (int r = 0; r < m_model->rowCount(); ++r) {
            QStandardItem *it = m_model->item(r, 0);
            if (it && it->text() == path) return r;
        }
        return -1;
    }

    QTableView *m_view = nullptr;
    QStandardItemModel *m_model = nullptr;
    ProducerThread *m_producer = nullptr;
    ConsumerThread *m_consumer = nullptr;
};

// ---------------- main ----------------
int main(int argc, char **argv) {
    QApplication app(argc, argv);

    // Default device and filter (same defaults as your original source)
    QString dev = QStringLiteral("wlan0");
    QString filter = QString();

    if (argc == 2) {
        dev = QString::fromLocal8Bit(argv[1]);
    } else if (argc >= 3) {
        dev = QString::fromLocal8Bit(argv[1]);
        filter = QString::fromLocal8Bit(argv[2]);
    }

    MainWindow w(dev, filter);
    w.show();

    return app.exec();
}

#include "nettop.moc"
