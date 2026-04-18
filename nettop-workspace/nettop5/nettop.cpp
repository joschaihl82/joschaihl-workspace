// nettop.cpp
//
// nettop - single-file Qt5 application with embedded libpcap capture,
// OpenStreetMap tile background, SPSC ring buffer for high-performance
// capture-to-model handoff, and a diagnostics panel to tune and observe
// batching/ring behavior.
//
// Build example:
//   cmake .. && make
// or
//   g++ -std=c++17 nettop.cpp -o nettop `pkg-config --cflags --libs Qt5Widgets Qt5Network Qt5Concurrent` -lpcap
//
// Run (capture requires privileges):
//   sudo ./nettop
//
// ----------------------------------------------------------------------------

#include <pcap.h>
#include <QApplication>
#include <QMainWindow>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>
#include <QMutexLocker>
#include <QTimer>
#include <QDateTime>
#include <QPainter>
#include <QPaintEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QSplitter>
#include <QRandomGenerator>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QElapsedTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QDir>
#include <QCache>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QSpinBox>
#include <cmath>
#include <map>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <netdb.h>

Q_DECLARE_METATYPE(QVector<QVariant>)

// ----------------------------- Data structures -----------------------------

struct ConnectionEvent {
    qint64 ts_us = 0;
    QString proto;
    QString src_ip;
    int src_port = 0;
    QString dst_ip;
    int dst_port = 0;
    QString proc;
    QString event;
    int len = 0;
    QJsonObject meta;
};

struct CountryInfo {
    QString iso;    // ISO alpha-2 code, e.g., "DE"
    double lat = 0; // centroid latitude
    double lon = 0; // centroid longitude
    bool valid = false;
};

struct Node {
    QString ip;
    QString label;
    QString proc;
    CountryInfo country;
    bool hasCountry = false;
    QPointF mapPos;        // world coords relative to map center (pixels)
    QColor color;
    qint64 last_seen_us = 0;
    int aggregated_bytes = 0;
};

// ----------------------------- Simple built-in centroid table -------------
// A small table for common countries. Extend as needed.

static const std::map<QString, CountryInfo> kBuiltInCentroids = {
    {"US", {"US", 39.8283, -98.5795, true}},
    {"DE", {"DE", 51.1657, 10.4515, true}},
    {"GB", {"GB", 55.3781, -3.4360, true}},
    {"FR", {"FR", 46.2276, 2.2137, true}},
    {"CN", {"CN", 35.8617, 104.1954, true}},
    {"RU", {"RU", 61.5240, 105.3188, true}},
    {"IN", {"IN", 20.5937, 78.9629, true}},
    {"JP", {"JP", 36.2048, 138.2529, true}},
    {"BR", {"BR", -14.2350, -51.9253, true}},
    {"CA", {"CA", 56.1304, -106.3468, true}},
    {"AU", {"AU", -25.2744, 133.7751, true}},
    {"NL", {"NL", 52.1326, 5.2913, true}},
    {"IT", {"IT", 41.8719, 12.5674, true}},
    {"ES", {"ES", 40.4637, -3.7492, true}},
    {"SE", {"SE", 60.1282, 18.6435, true}},
    {"CH", {"CH", 46.8182, 8.2275, true}},
    {"PL", {"PL", 51.9194, 19.1451, true}},
    {"BE", {"BE", 50.5039, 4.4699, true}},
    {"AT", {"AT", 47.5162, 14.5501, true}}
};

// ----------------------------- GeoResolver -------------------------------

class GeoResolver : public QObject {
    Q_OBJECT
public:
    GeoResolver(QObject *parent = nullptr) : QObject(parent) {}

    void resolveAsync(const QString &ip) {
        {
            QMutexLocker lk(&m_cacheMutex);
            if (m_cache.contains(ip)) {
                emit resolved(ip, m_cache[ip]);
                return;
            }
            if (m_inProgress.contains(ip)) return;
            m_inProgress.insert(ip);
        }
        QtConcurrent::run([this, ip]() {
            CountryInfo ci;
            struct in_addr addr;
            if (inet_pton(AF_INET, ip.toUtf8().constData(), &addr) == 1) {
                struct sockaddr_in sa;
                sa.sin_family = AF_INET;
                sa.sin_addr = addr;
                char hostbuf[NI_MAXHOST] = {0};
                int rc = getnameinfo((struct sockaddr *)&sa, sizeof(sa), hostbuf, sizeof(hostbuf), NULL, 0, NI_NAMEREQD);
                if (rc == 0) {
                    QString host = QString::fromUtf8(hostbuf);
                    int lastDot = host.lastIndexOf('.');
                    if (lastDot != -1 && lastDot + 1 < host.length()) {
                        QString tld = host.mid(lastDot + 1).toLower();
                        if (tld.length() == 2) {
                            QString iso = tld.toUpper();
                            auto it = kBuiltInCentroids.find(iso);
                            if (it != kBuiltInCentroids.end()) {
                                ci = it->second;
                                ci.valid = true;
                            } else {
                                ci.iso = iso;
                                ci.valid = false;
                            }
                        }
                    }
                }
            }
            if (!ci.valid) {
                bool assigned = false;
                QStringList parts = ip.split('.');
                if (parts.size() == 4) {
                    int first = parts[0].toInt();
                    if (first == 10 || (first == 192 && parts[1].toInt() == 168) || (first == 172 && (parts[1].toInt() >= 16 && parts[1].toInt() <= 31))) {
                        ci = kBuiltInCentroids.at("DE");
                        assigned = true;
                    }
                }
                if (!assigned) ci.valid = false;
            }
            if (!ci.valid && !ci.iso.isEmpty()) {
                auto it = kBuiltInCentroids.find(ci.iso);
                if (it != kBuiltInCentroids.end()) {
                    ci = it->second;
                    ci.valid = true;
                }
            }
            {
                QMutexLocker lk(&m_cacheMutex);
                m_cache[ip] = ci;
                m_inProgress.remove(ip);
            }
            emit resolved(ip, ci);
        });
    }

signals:
    void resolved(const QString &ip, const CountryInfo &ci);

private:
    QMutex m_cacheMutex;
    QHash<QString, CountryInfo> m_cache;
    QSet<QString> m_inProgress;
};

// ----------------------------- SPSC ring buffer ---------------------------

template<typename T>
class SpscRing {
public:
    explicit SpscRing(size_t size_pow2 = 4096)
        : m_size(size_pow2), m_mask(size_pow2 - 1), m_buf(size_pow2) {
        if ((size_pow2 & (size_pow2 - 1)) != 0) {
            throw std::invalid_argument("SpscRing size must be power of two");
        }
        m_head.store(0, std::memory_order_relaxed);
        m_tail.store(0, std::memory_order_relaxed);
    }

    bool push(const T &item) {
        size_t head = m_head.load(std::memory_order_relaxed);
        size_t next = (head + 1) & m_mask;
        if (next == m_tail.load(std::memory_order_acquire)) return false;
        m_buf[head] = item;
        m_head.store(next, std::memory_order_release);
        return true;
    }

    bool push(T &&item) {
        size_t head = m_head.load(std::memory_order_relaxed);
        size_t next = (head + 1) & m_mask;
        if (next == m_tail.load(std::memory_order_acquire)) return false;
        m_buf[head] = std::move(item);
        m_head.store(next, std::memory_order_release);
        return true;
    }

    bool pop(T &out) {
        size_t tail = m_tail.load(std::memory_order_relaxed);
        if (tail == m_head.load(std::memory_order_acquire)) return false;
        out = std::move(m_buf[tail]);
        m_tail.store((tail + 1) & m_mask, std::memory_order_release);
        return true;
    }

    size_t capacity() const { return m_size - 1; }

    size_t size() const {
        size_t head = m_head.load(std::memory_order_acquire);
        size_t tail = m_tail.load(std::memory_order_acquire);
        if (head >= tail) return head - tail;
        return (m_size + head - tail);
    }

    bool empty() const { return m_tail.load(std::memory_order_acquire) == m_head.load(std::memory_order_acquire); }

private:
    const size_t m_size;
    const size_t m_mask;
    std::vector<T> m_buf;
    std::atomic<size_t> m_head;
    std::atomic<size_t> m_tail;
};

// ----------------------------- CaptureThread (SPSC variant) --------------

class CaptureThread : public QThread {
    Q_OBJECT
public:
    CaptureThread(const QString &dev = QString(), const QString &bpf = QString(), QObject *parent = nullptr)
        : QThread(parent),
          m_dev(dev),
          m_bpf(bpf),
          m_handle(nullptr),
          m_stop(false),
          m_ring(1 << 12), // 4096 slots
          m_dropped(0),
          m_batchMax(256),
          m_batchMs(50),
          m_totalBatches(0),
          m_totalBatchItems(0)
    {}

    ~CaptureThread() override {
        stopCapture();
    }

    void stopCapture() {
        m_stop.store(true);
        if (m_handle) pcap_breakloop(m_handle);
        wait(500);
    }

    // tuning setters
    void setBatchMax(int v) { m_batchMax.store(qMax(1, v), std::memory_order_relaxed); }
    void setBatchMs(int v)  { m_batchMs.store(qMax(1, v), std::memory_order_relaxed); }

    // diagnostics getters
    uint64_t getDropped() const { return m_dropped.load(std::memory_order_relaxed); }
    size_t getRingOccupancy() const { return m_ring.size(); }
    size_t getRingCapacity() const { return m_ring.capacity(); }
    double getAvgBatchSize() const {
        uint64_t batches = m_totalBatches.load(std::memory_order_relaxed);
        if (batches == 0) return 0.0;
        uint64_t items = m_totalBatchItems.load(std::memory_order_relaxed);
        return double(items) / double(batches);
    }

signals:
    void newEvent(const ConnectionEvent &ev); // optional
    void newBatch(const QVector<ConnectionEvent> &batch);

protected:
    void run() override {
        char errbuf[PCAP_ERRBUF_SIZE];
        const char *devc = nullptr;
        if (!m_dev.isEmpty()) {
            devc = m_dev.toUtf8().constData();
        } else {
            pcap_if_t *alldevs = nullptr;
            if (pcap_findalldevs(&alldevs, errbuf) == 0 && alldevs != nullptr) {
                pcap_if_t *d = alldevs;
                while (d && !d->name) d = d->next;
                if (d && d->name) devc = d->name;
                pcap_freealldevs(alldevs);
            }
            if (!devc) {
                qWarning("No pcap device found (pcap_findalldevs failed or returned none)");
                return;
            }
        }

        m_handle = pcap_open_live(devc, 65535, 1, 1000, errbuf);
        if (!m_handle) {
            qWarning("pcap_open_live failed: %s", errbuf);
            return;
        }

        if (!m_bpf.isEmpty()) {
            struct bpf_program fp;
            if (pcap_compile(m_handle, &fp, m_bpf.toUtf8().constData(), 1, PCAP_NETMASK_UNKNOWN) == 0) {
                if (pcap_setfilter(m_handle, &fp) != 0) {
                    qWarning("pcap_setfilter failed: %s", pcap_geterr(m_handle));
                }
                pcap_freecode(&fp);
            } else {
                qWarning("pcap_compile failed: %s", pcap_geterr(m_handle));
            }
        }

        QVector<ConnectionEvent> batch;
        batch.reserve(512);
        qint64 lastFlush = QDateTime::currentMSecsSinceEpoch();

        while (!m_stop.load()) {
            int rc = pcap_dispatch(m_handle, 128, &CaptureThread::pcapCallback, reinterpret_cast<u_char*>(this));
            if (rc < 0) {
                qWarning("pcap_dispatch error: %s", pcap_geterr(m_handle));
                break;
            }

            int BATCH_MAX = m_batchMax.load(std::memory_order_relaxed);
            int BATCH_MS  = m_batchMs.load(std::memory_order_relaxed);

            ConnectionEvent ev;
            while ((int)batch.size() < BATCH_MAX && m_ring.pop(ev)) {
                batch.append(ev);
            }

            qint64 now = QDateTime::currentMSecsSinceEpoch();
            if (!batch.isEmpty() && (batch.size() >= BATCH_MAX || now - lastFlush >= BATCH_MS)) {
                emit newBatch(batch);
                m_totalBatches.fetch_add(1, std::memory_order_relaxed);
                m_totalBatchItems.fetch_add(batch.size(), std::memory_order_relaxed);
                batch.clear();
                lastFlush = now;
            }

            msleep(2);
        }

        // drain remaining
        ConnectionEvent ev;
        while (m_ring.pop(ev)) {
            batch.append(ev);
            if ((int)batch.size() >= m_batchMax.load(std::memory_order_relaxed)) {
                emit newBatch(batch);
                m_totalBatches.fetch_add(1, std::memory_order_relaxed);
                m_totalBatchItems.fetch_add(batch.size(), std::memory_order_relaxed);
                batch.clear();
            }
        }
        if (!batch.isEmpty()) {
            emit newBatch(batch);
            m_totalBatches.fetch_add(1, std::memory_order_relaxed);
            m_totalBatchItems.fetch_add(batch.size(), std::memory_order_relaxed);
            batch.clear();
        }

        if (m_handle) {
            pcap_close(m_handle);
            m_handle = nullptr;
        }
    }

private:
    static void pcapCallback(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes) {
        CaptureThread *self = reinterpret_cast<CaptureThread*>(user);
        if (!self) return;
        ConnectionEvent ev;
        if (!self->parsePacketToEvent(h, bytes, ev)) return;
        if (!self->m_ring.push(std::move(ev))) {
            self->m_dropped.fetch_add(1, std::memory_order_relaxed);
        }
    }

    bool parsePacketToEvent(const struct pcap_pkthdr *h, const u_char *bytes, ConnectionEvent &ev) {
        if (!h || !bytes) return false;
        if (h->caplen < sizeof(struct ether_header)) return false;
        const struct ether_header *eth = reinterpret_cast<const struct ether_header*>(bytes);
        uint16_t eth_type = ntohs(eth->ether_type);
        if (eth_type != ETHERTYPE_IP) return false;

        size_t ip_off = sizeof(struct ether_header);
        if (h->caplen < ip_off + sizeof(struct ip)) return false;
        const struct ip *ip = reinterpret_cast<const struct ip*>(bytes + ip_off);
        int ihl = ip->ip_hl * 4;
        if (ihl < 20) return false;
        if (h->caplen < ip_off + ihl) return false;

        char srcbuf[INET_ADDRSTRLEN] = {0};
        char dstbuf[INET_ADDRSTRLEN] = {0};
        inet_ntop(AF_INET, &ip->ip_src, srcbuf, sizeof(srcbuf));
        inet_ntop(AF_INET, &ip->ip_dst, dstbuf, sizeof(dstbuf));
        ev.src_ip = QString::fromUtf8(srcbuf);
        ev.dst_ip = QString::fromUtf8(dstbuf);
        ev.proto = (ip->ip_p == IPPROTO_TCP) ? "TCP" : (ip->ip_p == IPPROTO_UDP) ? "UDP" : "IP";
        ev.src_port = 0; ev.dst_port = 0;
        ev.event = "data";
        ev.len = 0;
        ev.ts_us = qint64(h->ts.tv_sec) * 1000000LL + qint64(h->ts.tv_usec);
        ev.proc = "-";

        if (ip->ip_p == IPPROTO_TCP) {
            size_t tcp_off = ip_off + ihl;
            if (h->caplen < tcp_off + sizeof(struct tcphdr)) return false;
            const struct tcphdr *tcp = reinterpret_cast<const struct tcphdr*>(bytes + tcp_off);
            ev.src_port = ntohs(tcp->th_sport);
            ev.dst_port = ntohs(tcp->th_dport);
            int thlen = tcp->th_off * 4;
            if (thlen < 20) thlen = 20;
            size_t payload_off = tcp_off + thlen;
            if (h->caplen > payload_off) ev.len = h->caplen - payload_off;
            bool syn = (tcp->th_flags & TH_SYN);
            bool fin = (tcp->th_flags & TH_FIN);
            if (syn && !fin) ev.event = "connect";
            else if (fin) ev.event = "close";
        } else if (ip->ip_p == IPPROTO_UDP) {
            size_t udp_off = ip_off + ihl;
            if (h->caplen < udp_off + sizeof(struct udphdr)) return false;
            const struct udphdr *udp = reinterpret_cast<const struct udphdr*>(bytes + udp_off);
            ev.src_port = ntohs(udp->uh_sport);
            ev.dst_port = ntohs(udp->uh_dport);
            size_t payload_off = udp_off + sizeof(struct udphdr);
            if (h->caplen > payload_off) ev.len = h->caplen - payload_off;
        }
        return true;
    }

    QString m_dev;
    QString m_bpf;
    pcap_t *m_handle;
    std::atomic<bool> m_stop;

    SpscRing<ConnectionEvent> m_ring;
    std::atomic<uint64_t> m_dropped;

    std::atomic<int> m_batchMax;
    std::atomic<int> m_batchMs;

    std::atomic<uint64_t> m_totalBatches;
    std::atomic<uint64_t> m_totalBatchItems;
};

// ----------------------------- GraphModel --------------------------------

class GraphModel : public QObject {
    Q_OBJECT
public:
    GraphModel(GeoResolver *resolver, QObject *parent = nullptr)
        : QObject(parent), m_resolver(resolver) {
        connect(m_resolver, &GeoResolver::resolved, this, &GraphModel::onResolved, Qt::QueuedConnection);
    }

public slots:
    void onNewEvent(const ConnectionEvent &ev) {
        QMutexLocker lk(&m_mutex);
        qint64 now = ev.ts_us ? ev.ts_us : QDateTime::currentMSecsSinceEpoch() * 1000LL;
        if (!m_nodes.contains(ev.src_ip)) {
            Node n;
            n.ip = ev.src_ip;
            n.label = ev.src_ip;
            n.proc = ev.proc;
            n.color = colorForIp(ev.src_ip);
            n.last_seen_us = now;
            m_nodes[ev.src_ip] = n;
            m_resolver->resolveAsync(ev.src_ip);
        } else {
            m_nodes[ev.src_ip].last_seen_us = now;
            m_nodes[ev.src_ip].aggregated_bytes += ev.len;
        }
        if (!m_nodes.contains(ev.dst_ip)) {
            Node n;
            n.ip = ev.dst_ip;
            n.label = ev.dst_ip;
            n.proc = ev.proc;
            n.color = colorForIp(ev.dst_ip);
            n.last_seen_us = now;
            m_nodes[ev.dst_ip] = n;
            m_resolver->resolveAsync(ev.dst_ip);
        } else {
            m_nodes[ev.dst_ip].last_seen_us = now;
            m_nodes[ev.dst_ip].aggregated_bytes += ev.len;
        }
        m_recent.prepend(ev);
        while (m_recent.size() > 300) m_recent.removeLast();
        emit graphUpdated();
    }

    void onNewBatch(const QVector<ConnectionEvent> &batch) {
        if (batch.isEmpty()) return;
        QMutexLocker lk(&m_mutex);
        qint64 now = QDateTime::currentMSecsSinceEpoch() * 1000LL;
        for (const ConnectionEvent &ev : batch) {
            if (!m_nodes.contains(ev.src_ip)) {
                Node n;
                n.ip = ev.src_ip;
                n.label = ev.src_ip;
                n.proc = ev.proc;
                n.color = colorForIp(ev.src_ip);
                n.last_seen_us = ev.ts_us ? ev.ts_us : now;
                m_nodes[ev.src_ip] = n;
                m_resolver->resolveAsync(ev.src_ip);
            } else {
                Node &ns = m_nodes[ev.src_ip];
                ns.last_seen_us = ev.ts_us ? ev.ts_us : now;
                ns.aggregated_bytes += ev.len;
            }
            if (!m_nodes.contains(ev.dst_ip)) {
                Node n;
                n.ip = ev.dst_ip;
                n.label = ev.dst_ip;
                n.proc = ev.proc;
                n.color = colorForIp(ev.dst_ip);
                n.last_seen_us = ev.ts_us ? ev.ts_us : now;
                m_nodes[ev.dst_ip] = n;
                m_resolver->resolveAsync(ev.dst_ip);
            } else {
                Node &nd = m_nodes[ev.dst_ip];
                nd.last_seen_us = ev.ts_us ? ev.ts_us : now;
                nd.aggregated_bytes += ev.len;
            }
            m_recent.prepend(ev);
            if (m_recent.size() > 300) m_recent.removeLast();
        }
        emit graphUpdated();
    }

    void onResolved(const QString &ip, const CountryInfo &ci) {
        QMutexLocker lk(&m_mutex);
        if (!m_nodes.contains(ip)) return;
        Node &n = m_nodes[ip];
        n.country = ci;
        n.hasCountry = ci.valid;
        emit graphUpdated();
    }

    void snapshotNodes(std::vector<Node> &out) {
        QMutexLocker lk(&m_mutex);
        out.clear();
        out.reserve(m_nodes.size());
        for (auto it = m_nodes.constBegin(); it != m_nodes.constEnd(); ++it) out.push_back(it.value());
    }

    QList<ConnectionEvent> recentEvents() {
        QMutexLocker lk(&m_mutex);
        return m_recent;
    }

signals:
    void graphUpdated();

private:
    static QColor colorForIp(const QString &ip) {
        uint32_t h = 2166136261u;
        for (QChar c : ip) {
            h ^= (uint32_t)c.unicode();
            h *= 16777619u;
        }
        int hue = int(h % 360);
        return QColor::fromHsl(hue, 200, 160);
    }

    QMutex m_mutex;
    QMap<QString, Node> m_nodes;
    QList<ConnectionEvent> m_recent;
    GeoResolver *m_resolver;
};

// ----------------------------- TileManager (OSM) --------------------------

class TileManager : public QObject {
    Q_OBJECT
public:
    TileManager(QObject *parent = nullptr)
        : QObject(parent), m_net(new QNetworkAccessManager(this)), m_memCache(512) {
        QString cacheBase = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        if (cacheBase.isEmpty()) cacheBase = QDir::homePath() + "/.cache/nettop";
        m_cacheDir = cacheBase + "/tiles";
        QDir().mkpath(m_cacheDir);
        m_memCache.setMaxCost(512);
    }

    void requestTile(int z, int x, int y) {
        QString key = keyFor(z,x,y);
        if (m_memCache.contains(key)) {
            QImage *cached = m_memCache.object(key);
            if (cached) emit tileReady(z,x,y, *cached);
            return;
        }
        if (m_pending.contains(key)) return;

        QString diskPath = diskPathFor(z,x,y);
        if (QFile::exists(diskPath)) {
            QImage img(diskPath);
            if (!img.isNull()) {
                QImage *ptr = new QImage(img);
                m_memCache.insert(key, ptr);
                emit tileReady(z,x,y, img);
                return;
            }
        }

        if (m_pending.size() >= 8) return;

        m_pending.insert(key);
        QString url = m_tileServer;
        url.replace("{z}", QString::number(z));
        url.replace("{x}", QString::number(x));
        url.replace("{y}", QString::number(y));

        QNetworkRequest req{ QUrl(url) };
        req.setRawHeader("User-Agent", QByteArray("nettop/1.0 (contact@example.com)"));
        QNetworkReply *reply = m_net->get(req);
        connect(reply, &QNetworkReply::finished, this, [this, reply, z, x, y, key, diskPath]() {
            m_pending.remove(key);
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray data = reply->readAll();
                QImage img;
                img.loadFromData(data);
                if (!img.isNull()) {
                    QImage *ptr = new QImage(img);
                    m_memCache.insert(key, ptr);
                    ensureDirFor(diskPath);
                    img.save(diskPath, "PNG");
                    emit tileReady(z, x, y, img);
                }
            }
            reply->deleteLater();
        });
    }

    void setTileServer(const QString &server) { m_tileServer = server; }

signals:
    void tileReady(int z, int x, int y, const QImage &img);

private:
    static QString keyFor(int z, int x, int y) { return QString("%1/%2/%3").arg(z).arg(x).arg(y); }
    QString diskPathFor(int z, int x, int y) {
        return QString("%1/%2/%3/%4.png").arg(m_cacheDir).arg(z).arg(x).arg(y);
    }
    static void ensureDirFor(const QString &path) {
        QFileInfo fi(path);
        QDir dir = fi.dir();
        if (!dir.exists()) dir.mkpath(".");
    }

    QNetworkAccessManager *m_net;
    QString m_cacheDir;
    QString m_tileServer = "https://tile.openstreetmap.org/{z}/{x}/{y}.png";
    QSet<QString> m_pending;
    QCache<QString, QImage> m_memCache; // stores QImage* internally
};

// ----------------------------- GLWidget (map + nodes only) ---------------

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    GLWidget(GraphModel *model, TileManager *tiles, QWidget *parent = nullptr)
        : QOpenGLWidget(parent), m_model(model), m_tiles(tiles), m_zoomLevel(3), m_zoomScale(1.0) {
        setMouseTracking(true);
        setFocusPolicy(Qt::StrongFocus);
        connect(m_model, &GraphModel::graphUpdated, this, &GLWidget::onModelUpdated, Qt::QueuedConnection);
        connect(m_tiles, &TileManager::tileReady, this, &GLWidget::onTileReady, Qt::QueuedConnection);
        m_lastTick.start();
        m_centerLat = 52.52;
        m_centerLon = 13.405;
    }

    void setMapMode(bool on) { m_mapMode = on; update(); }
    void setTileZoom(int z) { m_zoomLevel = qBound(0, z, 19); update(); }

protected:
    void initializeGL() override {
        initializeOpenGLFunctions();
        glClearColor(0.06f, 0.06f, 0.07f, 1.0f);
    }

    void resizeGL(int w, int h) override { m_viewSize = QSize(w, h); }

    void paintGL() override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.fillRect(rect(), QColor(10,10,12));

        drawMap(p);

        std::vector<Node> nodes;
        m_model->snapshotNodes(nodes);

        for (Node &n : nodes) {
            QPointF screen;
            bool placed = false;
            if (n.hasCountry) {
                QPointF global = latLonToGlobalPixel(n.country.lat, n.country.lon, m_zoomLevel);
                QPointF world = globalPixelToWorld(global);
                float jitter = 6.0f + std::min(20.0f, float(n.aggregated_bytes) / 2000.0f);
                float rx = (float(QRandomGenerator::global()->bounded(10000)) / 10000.0f - 0.5f) * 2.0f * jitter;
                float ry = (float(QRandomGenerator::global()->bounded(10000)) / 10000.0f - 0.5f) * 2.0f * jitter;
                screen = QPointF(width()/2.0 + world.x() + rx, height()/2.0 + world.y() + ry);
                placed = true;
            } else {
                int idx = (m_unknownIndex++ % 1000);
                float x = width() - 120.0f;
                float y = 40.0f + (idx % 30) * 18.0f;
                screen = QPointF(x, y);
                placed = true;
            }
            if (placed) drawNodeAt(p, n, screen);
        }

        p.setPen(QColor(200,200,200,180));
        QFont f = p.font();
        f.setPointSize(9);
        p.setFont(f);
        p.drawText(8, height() - 10, "© OpenStreetMap contributors");
    }

    void wheelEvent(QWheelEvent *ev) override {
        float delta = ev->angleDelta().y() / 120.0f;
        if (ev->modifiers() & Qt::ControlModifier) {
            m_zoomLevel = qBound(0, m_zoomLevel + int(delta), 19);
            update();
            return;
        }
        float factor = std::pow(1.15f, delta);
        QPointF cursor = ev->position();
        QPointF center(width()/2.0, height()/2.0);
        QPointF worldBefore = (cursor - center - m_pan) / m_zoomScale;
        float newZoom = m_zoomScale * factor;
        newZoom = qBound(0.25f, newZoom, 8.0f);
        m_pan = cursor - center - worldBefore * newZoom;
        m_zoomScale = newZoom;
        update();
    }

    void mousePressEvent(QMouseEvent *ev) override {
        if (ev->button() == Qt::LeftButton) {
            m_dragging = true;
            m_lastMouse = ev->pos();
        }
    }

    void mouseMoveEvent(QMouseEvent *ev) override {
        if (m_dragging) {
            QPoint delta = ev->pos() - m_lastMouse;
            m_lastMouse = ev->pos();
            m_pan += delta;
            update();
        }
    }

    void mouseReleaseEvent(QMouseEvent *ev) override {
        if (ev->button() == Qt::LeftButton) m_dragging = false;
    }

private slots:
    void onModelUpdated() { update(); }
    void onTileReady(int z, int x, int y, const QImage &img) { Q_UNUSED(z); Q_UNUSED(x); Q_UNUSED(y); Q_UNUSED(img); update(); }

private:
    static QPointF latLonToGlobalPixel(double lat, double lon, int z) {
        double sinLat = sin(lat * M_PI / 180.0);
        double n = pow(2.0, z);
        double x = (lon + 180.0) / 360.0 * n * 256.0;
        double y = (0.5 - log((1 + sinLat) / (1 - sinLat)) / (4 * M_PI)) * n * 256.0;
        return QPointF(x, y);
    }

    QPointF globalPixelToWorld(const QPointF &globalPx) const {
        QPointF centerGlobal = latLonToGlobalPixel(m_centerLat, m_centerLon, m_zoomLevel);
        QPointF offset = globalPx - centerGlobal;
        return QPointF(offset.x() * m_zoomScale + m_pan.x(), offset.y() * m_zoomScale + m_pan.y());
    }

    void drawMap(QPainter &p) {
        QSize wsize = size();
        double halfW = (wsize.width() / 2.0) / m_zoomScale;
        double halfH = (wsize.height() / 2.0) / m_zoomScale;
        QPointF centerGlobal = latLonToGlobalPixel(m_centerLat, m_centerLon, m_zoomLevel);
        double leftGlobal = centerGlobal.x() - halfW;
        double rightGlobal = centerGlobal.x() + halfW;
        double topGlobal = centerGlobal.y() - halfH;
        double bottomGlobal = centerGlobal.y() + halfH;

        int tileLeft = int(floor(leftGlobal / 256.0));
        int tileRight = int(floor(rightGlobal / 256.0));
        int tileTop = int(floor(topGlobal / 256.0));
        int tileBottom = int(floor(bottomGlobal / 256.0));

        int maxTiles = 1 << m_zoomLevel;

        for (int tx = tileLeft; tx <= tileRight; ++tx) {
            for (int ty = tileTop; ty <= tileBottom; ++ty) {
                int wrapX = tx;
                while (wrapX < 0) wrapX += maxTiles;
                while (wrapX >= maxTiles) wrapX -= maxTiles;
                if (ty < 0 || ty >= maxTiles) continue;
                m_tiles->requestTile(m_zoomLevel, wrapX, ty);
                QPointF tileGlobal(tx * 256.0, ty * 256.0);
                QPointF world = globalPixelToWorld(tileGlobal);
                QRectF target(world.x() + width()/2.0, world.y() + height()/2.0, 256.0 * m_zoomScale, 256.0 * m_zoomScale);
                p.fillRect(target, QColor(30,30,34));
            }
        }
    }

    void drawNodeAt(QPainter &p, const Node &n, const QPointF &screen) {
        float baseW = 110.0f;
        float baseH = 36.0f;
        QRectF r(screen.x() - baseW/2.0, screen.y() - baseH/2.0, baseW, baseH);
        QColor fill = n.color;
        fill.setAlpha(220);
        QColor border = fill.darker(120);
        p.setPen(QPen(border, 1.2));
        p.setBrush(fill);
        p.drawRoundedRect(r, 6, 6);
        p.setPen(QColor(240,240,240));
        QFont f = p.font();
        f.setPointSize(9);
        f.setBold(true);
        p.setFont(f);
        QString label = n.label;
        if (n.hasCountry && !n.country.iso.isEmpty()) label += " (" + n.country.iso + ")";
        p.drawText(r.adjusted(6,2,-6,-2), Qt::AlignLeft | Qt::AlignVCenter, label);
    }

    GraphModel *m_model;
    TileManager *m_tiles;
    QSize m_viewSize;
    int m_zoomLevel;
    float m_zoomScale;
    double m_centerLat;
    double m_centerLon;
    QPointF m_pan = QPointF(0,0);
    bool m_mapMode = true;
    bool m_dragging = false;
    QPoint m_lastMouse;
    QElapsedTimer m_lastTick;
    int m_unknownIndex = 0;
};

// ----------------------------- MainWindow ---------------------------------

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(const QString &dev, const QString &bpf, QWidget *parent = nullptr)
        : QMainWindow(parent) {
        setWindowTitle("nettop - map view (nodes placed by country/state)");
        resize(1200, 800);

        m_resolver = new GeoResolver(this);
        m_model = new GraphModel(m_resolver, this);
        m_tiles = new TileManager(this);

        m_gl = new GLWidget(m_model, m_tiles, this);

        QWidget *topBar = new QWidget(this);
        QHBoxLayout *topLayout = new QHBoxLayout(topBar);
        topLayout->setContentsMargins(6,6,6,6);
        QLabel *lbl = new QLabel("Map Mode (tiles: OpenStreetMap)", topBar);
        topLayout->addWidget(lbl);
        topLayout->addStretch();
        QPushButton *centerBtn = new QPushButton("Center: Berlin", topBar);
        topLayout->addWidget(centerBtn);
        connect(centerBtn, &QPushButton::clicked, [this]() {
            m_gl->setMapMode(true);
            m_gl->update();
        });

        QWidget *rightPanel = new QWidget(this);
        QVBoxLayout *rpLayout = new QVBoxLayout(rightPanel);
        rpLayout->setContentsMargins(6,6,6,6);
        QLabel *tailLabel = new QLabel("Recent events", rightPanel);
        m_tailList = new QListWidget(rightPanel);
        rpLayout->addWidget(tailLabel);
        rpLayout->addWidget(m_tailList);

        // Diagnostics panel
        QWidget *diagWidget = new QWidget(rightPanel);
        QVBoxLayout *diagLayout = new QVBoxLayout(diagWidget);
        diagLayout->setContentsMargins(6,6,6,6);
        QLabel *diagTitle = new QLabel("<b>Diagnostics</b>", diagWidget);
        diagLayout->addWidget(diagTitle);

        m_lblDropped = new QLabel("Dropped: 0", diagWidget);
        m_lblOccupancy = new QLabel("Ring: 0 / 0", diagWidget);
        m_lblAvgBatch = new QLabel("Avg batch: 0.0", diagWidget);
        diagLayout->addWidget(m_lblDropped);
        diagLayout->addWidget(m_lblOccupancy);
        diagLayout->addWidget(m_lblAvgBatch);

        QHBoxLayout *h1 = new QHBoxLayout();
        QLabel *lblBatchSize = new QLabel("Batch size:", diagWidget);
        m_spinBatchSize = new QSpinBox(diagWidget);
        m_spinBatchSize->setRange(1, 8192);
        m_spinBatchSize->setValue(256);
        h1->addWidget(lblBatchSize);
        h1->addWidget(m_spinBatchSize);
        diagLayout->addLayout(h1);

        QHBoxLayout *h2 = new QHBoxLayout();
        QLabel *lblBatchMs = new QLabel("Flush ms:", diagWidget);
        m_spinBatchMs = new QSpinBox(diagWidget);
        m_spinBatchMs->setRange(1, 2000);
        m_spinBatchMs->setValue(50);
        h2->addWidget(lblBatchMs);
        h2->addWidget(m_spinBatchMs);
        diagLayout->addLayout(h2);

        QPushButton *applyDiag = new QPushButton("Apply", diagWidget);
        diagLayout->addWidget(applyDiag);
        rpLayout->addWidget(diagWidget);

        QSplitter *split = new QSplitter(this);
        split->addWidget(m_gl);
        split->addWidget(rightPanel);
        split->setStretchFactor(0, 4);
        split->setStretchFactor(1, 1);

        QWidget *central = new QWidget(this);
        QVBoxLayout *mainLayout = new QVBoxLayout(central);
        mainLayout->setContentsMargins(0,0,0,0);
        mainLayout->addWidget(topBar);
        mainLayout->addWidget(split);
        setCentralWidget(central);

        m_capture = new CaptureThread(dev, bpf, this);
        connect(m_capture, &CaptureThread::newBatch, m_model, &GraphModel::onNewBatch, Qt::QueuedConnection);
        connect(m_capture, &CaptureThread::newEvent, m_model, &GraphModel::onNewEvent, Qt::QueuedConnection);
        connect(m_capture, &CaptureThread::newBatch, this, [this](const QVector<ConnectionEvent> &batch){
            for (const ConnectionEvent &ev : batch) {
                QString s = QString("[%1] %2 %3:%4 -> %5:%6 %7 %8B")
                    .arg(QDateTime::fromMSecsSinceEpoch(ev.ts_us/1000).toString("hh:mm:ss"))
                    .arg(ev.proto)
                    .arg(ev.src_ip).arg(ev.src_port)
                    .arg(ev.dst_ip).arg(ev.dst_port)
                    .arg(ev.event)
                    .arg(ev.len);
                m_tailList->insertItem(0, s);
            }
            while (m_tailList->count() > 300) delete m_tailList->takeItem(m_tailList->count()-1);
        }, Qt::QueuedConnection);

        connect(m_capture, &CaptureThread::newEvent, this, &MainWindow::onNewEvent, Qt::QueuedConnection);

        connect(applyDiag, &QPushButton::clicked, this, [this]() {
            int bs = m_spinBatchSize->value();
            int ms = m_spinBatchMs->value();
            if (m_capture) {
                m_capture->setBatchMax(bs);
                m_capture->setBatchMs(ms);
            }
        });

        QTimer *diagTimer = new QTimer(this);
        connect(diagTimer, &QTimer::timeout, this, &MainWindow::updateDiagnostics);
        diagTimer->start(1000);

        m_capture->start();
    }

    ~MainWindow() override {
        if (m_capture) {
            m_capture->stopCapture();
            m_capture->wait(500);
        }
    }

private slots:
    void onNewEvent(const ConnectionEvent &ev) {
        QString s = QString("[%1] %2 %3:%4 -> %5:%6 %7 %8B")
            .arg(QDateTime::fromMSecsSinceEpoch(ev.ts_us/1000).toString("hh:mm:ss"))
            .arg(ev.proto)
            .arg(ev.src_ip).arg(ev.src_port)
            .arg(ev.dst_ip).arg(ev.dst_port)
            .arg(ev.event)
            .arg(ev.len);
        m_tailList->insertItem(0, s);
        while (m_tailList->count() > 300) delete m_tailList->takeItem(m_tailList->count()-1);
    }

    void updateDiagnostics() {
        if (!m_capture) return;
        uint64_t dropped = m_capture->getDropped();
        size_t occ = m_capture->getRingOccupancy();
        size_t cap = m_capture->getRingCapacity();
        double avg = m_capture->getAvgBatchSize();
        m_lblDropped->setText(QString("Dropped: %1").arg(dropped));
        m_lblOccupancy->setText(QString("Ring: %1 / %2").arg(occ).arg(cap));
        m_lblAvgBatch->setText(QString("Avg batch: %1").arg(QString::number(avg, 'f', 2)));
    }

private:
    GeoResolver *m_resolver;
    GraphModel *m_model;
    TileManager *m_tiles;
    GLWidget *m_gl;
    CaptureThread *m_capture;
    QListWidget *m_tailList;

    QLabel *m_lblDropped = nullptr;
    QLabel *m_lblOccupancy = nullptr;
    QLabel *m_lblAvgBatch = nullptr;
    QSpinBox *m_spinBatchSize = nullptr;
    QSpinBox *m_spinBatchMs = nullptr;
};

// ----------------------------- main --------------------------------------
int main(int argc, char **argv) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    QString dev;
    QString bpf;
    if (argc >= 2) dev = QString::fromUtf8(argv[1]);
    if (argc >= 3) bpf = QString::fromUtf8(argv[2]);

    // If no device was provided on the command line, default to wlan0
    if (dev.isEmpty()) dev = QStringLiteral("wlan0");

    MainWindow w(dev, bpf);
    w.show();
    return app.exec();
}


#include "nettop.moc"
