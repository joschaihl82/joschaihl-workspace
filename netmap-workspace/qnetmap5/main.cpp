// main.cpp
// Build:
// g++ -O2 -Wall -std=c++17 main.cpp -o qnetmap \
//   -lpcap -lmaxminddb \
//   -lQt5WebEngineWidgets -lQt5Widgets -lQt5Gui -lQt5Core -pthread
//
// Run:
// sudo ./qnetmap
//
// Requirements:
// - GeoLite2-City.mmdb next to the binary
// - Qt5 with WebEngine and Network
// - libpcap, libmaxminddb
// - root or CAP_NET_RAW for raw sockets

#define _GNU_SOURCE
#include <QApplication>
#include <QMainWindow>
#include <QWebEngineView>
#include <QMetaObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

#include <thread>
#include <atomic>
#include <unordered_set>
#include <unordered_map>
#include <mutex>
#include <string>
#include <iostream>
#include <vector>
#include <chrono>
#include <functional>

#include <pcap.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <net/ethernet.h>
#include <maxminddb.h>
#include <unistd.h>
#include <limits.h>
#include <netdb.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

// pcap capture parameters
static const int SNAPLEN    = 65535;
static const int PROMISC    = 1;
static const int TIMEOUT_MS = 1000;

// traceroute parameters
static const int TR_MAX_HOPS = 30;
static const int TR_PROBE_TIMEOUT_MS = 1500;
static const int TR_BASE_PORT = 33434; // standard traceroute base

// ---------------- GUI ----------------
class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent=nullptr) : QMainWindow(parent) {
        view = new QWebEngineView(this);
        setCentralWidget(view);
        setWindowTitle("Live IP Geo Map (traceroute hops + polylines)");

        const char html[] = R"HTML(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8"/>
  <title>IP Map</title>
  <link rel="stylesheet" href="https://unpkg.com/leaflet/dist/leaflet.css"/>
  <script src="https://unpkg.com/leaflet/dist/leaflet.js"></script>
  <style>html,body,#map{height:100%;margin:0;} .label { font-weight: bold; }</style>
</head>
<body>
<div id="map"></div>
<script>
var map = L.map('map').setView([20,0], 2);
L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png',
 { attribution: '© OpenStreetMap contributors'}).addTo(map);

var ipMarkers = {};      // red remote IPs
var tracerMarkers = {};  // yellow traceroute hop markers keyed by hop ip
var tracerPaths = {};    // per-destination polyline and last coords
var homeMarker = null;

function addIPMarker(ip, lat, lon, label) {
  if (!lat || !lon) return;
  if (ipMarkers[ip]) return;
  var m = L.circleMarker([lat, lon], {
    radius: 6, color: '#ff4444', weight: 1,
    fillColor: '#ff4444', fillOpacity: 0.8
  }).addTo(map);
  var popupText = (label && label.length) ? ('<div class="label">'+label+'</div><div>'+ip+'</div>') : ip;
  m.bindPopup(popupText);
  ipMarkers[ip] = m;
}

function addHomeMarker(ip, lat, lon, label) {
  if (!lat || !lon) return;
  if (homeMarker) { map.removeLayer(homeMarker); homeMarker = null; }
  homeMarker = L.circleMarker([lat, lon], {
    radius: 8, color: '#2b7cff', weight: 2,
    fillColor: '#2b7cff', fillOpacity: 0.9
  }).addTo(map);
  var popupText = (label && label.length) ? ('<div class="label">'+label+'</div><div>'+ip+'</div>') : ip;
  homeMarker.bindPopup(popupText);
  map.setView([lat, lon], 6);
}

// destIp: the traceroute destination (string)
// hopIp: the hop IP to add
// lat, lon: coordinates for hop
// label: popup label
function addTracerHop(destIp, hopIp, lat, lon, label) {
  if (!lat || !lon) return;
  // add hop marker if not present
  if (!tracerMarkers[hopIp]) {
    var m = L.circleMarker([lat, lon], {
      radius: 6, color: '#ffcc33', weight: 1,
      fillColor: '#ffcc33', fillOpacity: 0.9
    }).addTo(map);
    var popupText = (label && label.length) ? ('<div class="label">'+label+'</div><div>'+hopIp+'</div>') : hopIp;
    m.bindPopup(popupText);
    tracerMarkers[hopIp] = m;
  }
  // ensure a path exists for this destination
  if (!tracerPaths[destIp]) {
    tracerPaths[destIp] = {
      poly: L.polyline([], {color: '#2b7cff', weight: 3, opacity: 0.8}).addTo(map),
      seen: new Set()
    };
  }
  // avoid adding same hop twice to the polyline for this destination
  if (!tracerPaths[destIp].seen.has(hopIp)) {
    tracerPaths[destIp].poly.addLatLng([lat, lon]);
    tracerPaths[destIp].seen.add(hopIp);
  }
}
</script>
</body>
</html>
)HTML";
        view->setHtml(html, QUrl("https://example.local/"));
    }

    void addMarker(const QString& ip, double lat, double lon, const QString& label) {
        QString ipEsc = ip; ipEsc.replace("'", "\\'");
        QString labelEsc = label; labelEsc.replace("'", "\\'");
        QString js = QString("addIPMarker('%1', %2, %3, '%4');")
                         .arg(ipEsc)
                         .arg(lat, 0, 'f', 6)
                         .arg(lon, 0, 'f', 6)
                         .arg(labelEsc);
        view->page()->runJavaScript(js);
    }

    void addHomeMarker(const QString& ip, double lat, double lon, const QString& label) {
        QString ipEsc = ip; ipEsc.replace("'", "\\'");
        QString labelEsc = label; labelEsc.replace("'", "\\'");
        QString js = QString("addHomeMarker('%1', %2, %3, '%4');")
                         .arg(ipEsc)
                         .arg(lat, 0, 'f', 6)
                         .arg(lon, 0, 'f', 6)
                         .arg(labelEsc);
        view->page()->runJavaScript(js);
    }

    // New: add traceroute hop and extend blue polyline for destination
    void addTracerHop(const QString& destIp, const QString& hopIp, double lat, double lon, const QString& label) {
        QString destEsc = destIp; destEsc.replace("'", "\\'");
        QString hopEsc = hopIp; hopEsc.replace("'", "\\'");
        QString labelEsc = label; labelEsc.replace("'", "\\'");
        QString js = QString("addTracerHop('%1', '%2', %3, %4, '%5');")
                         .arg(destEsc)
                         .arg(hopEsc)
                         .arg(lat, 0, 'f', 6)
                         .arg(lon, 0, 'f', 6)
                         .arg(labelEsc);
        view->page()->runJavaScript(js);
    }

private:
    QWebEngineView* view;
};

// ---------------- GeoIP ----------------
class GeoIPResolver {
public:
    GeoIPResolver() : db_open(false) {}
    ~GeoIPResolver() { close(); }

    bool open(const std::string& path) {
        int status = MMDB_open(path.c_str(), MMDB_MODE_MMAP, &mmdb);
        if (status != MMDB_SUCCESS) {
            std::cerr << "MMDB_open failed: " << MMDB_strerror(status) << std::endl;
            return false;
        }
        db_open = true;
        return true;
    }
    void close() { if (db_open) { MMDB_close(&mmdb); db_open=false; } }

    bool lookup(const std::string& ip, double& lat, double& lon, std::string& label) {
        lat=lon=0.0; label.clear();
        if (!db_open) return false;
        int gai_error=0, mmdb_error=0;
        auto result = MMDB_lookup_string(&mmdb, ip.c_str(), &gai_error, &mmdb_error);
        if (gai_error!=0 || mmdb_error!=MMDB_SUCCESS || !result.found_entry) return false;
        MMDB_entry_data_s data;
        if (MMDB_get_value(&result.entry,&data,"location","latitude",NULL)==MMDB_SUCCESS &&
            data.has_data && data.type==MMDB_DATA_TYPE_DOUBLE) lat=data.double_value;
        else return false;
        if (MMDB_get_value(&result.entry,&data,"location","longitude",NULL)==MMDB_SUCCESS &&
            data.has_data && data.type==MMDB_DATA_TYPE_DOUBLE) lon=data.double_value;
        else return false;
        if (MMDB_get_value(&result.entry,&data,"city","names","en",NULL)==MMDB_SUCCESS &&
            data.has_data && data.type==MMDB_DATA_TYPE_UTF8_STRING)
            label.assign(data.utf8_string,data.data_size);
        else label=ip;
        return true;
    }
private:
    MMDB_s mmdb{};
    bool db_open;
};

// ---------------- reverse DNS cache ----------------
class ReverseDNS {
public:
    ReverseDNS() {}
    std::string lookup(const std::string& ip) {
        {
            std::lock_guard<std::mutex> lk(mu);
            auto it = cache.find(ip);
            if (it != cache.end()) return it->second;
        }
        struct sockaddr_in sa;
        memset(&sa, 0, sizeof(sa));
        sa.sin_family = AF_INET;
        if (inet_pton(AF_INET, ip.c_str(), &sa.sin_addr) != 1) {
            store(ip, "");
            return "";
        }
        char host[NI_MAXHOST] = {0};
        int rc = getnameinfo((struct sockaddr*)&sa, sizeof(sa), host, sizeof(host), NULL, 0, NI_NAMEREQD);
        std::string res;
        if (rc == 0) res = host;
        else res = "";
        store(ip, res);
        return res;
    }
private:
    void store(const std::string& ip, const std::string& name) {
        std::lock_guard<std::mutex> lk(mu);
        cache[ip] = name;
    }
    std::unordered_map<std::string,std::string> cache;
    std::mutex mu;
};

// ---------------- pcap parsing ----------------
static bool parse_ipv4(const u_char* bytes, size_t caplen, std::string& src, std::string& dst) {
    if (caplen < sizeof(ether_header)) return false;
    const struct ether_header* eth = (const struct ether_header*)bytes;
    uint16_t etype = ntohs(eth->ether_type);
    size_t off = sizeof(ether_header);

    // handle simple VLAN tag (0x8100)
    if (etype == 0x8100) {
        if (caplen < off + 4 + 2) return false;
        off += 4;
        etype = ntohs(*(uint16_t*)(bytes + off));
        off += 2;
    }

    if (etype != ETHERTYPE_IP) return false;
    if (caplen < off + sizeof(struct ip)) return false;
    const struct ip* iphdr = (const struct ip*)(bytes + off);
    char ssrc[INET_ADDRSTRLEN], sdst[INET_ADDRSTRLEN];
    if (!inet_ntop(AF_INET, &iphdr->ip_src, ssrc, sizeof(ssrc))) return false;
    if (!inet_ntop(AF_INET, &iphdr->ip_dst, sdst, sizeof(sdst))) return false;
    src = ssrc; dst = sdst;
    return true;
}

// ---------------- traceroute implementation ----------------
// Send UDP probe with given TTL to dest_ip:port and wait for ICMP reply.
// Returns the responding IP (empty on timeout/error).
static std::string traceroute_probe(const std::string& dest_ip, int ttl, int probe_port, int timeout_ms) {
    int sendfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sendfd < 0) return "";
    if (setsockopt(sendfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
        close(sendfd);
        return "";
    }
    struct sockaddr_in dest{};
    dest.sin_family = AF_INET;
    dest.sin_port = htons(probe_port);
    if (inet_pton(AF_INET, dest_ip.c_str(), &dest.sin_addr) != 1) {
        close(sendfd);
        return "";
    }

    int recvfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (recvfd < 0) {
        close(sendfd);
        return "";
    }
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    setsockopt(recvfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    const char payload[] = "trprobe";
    (void)sendto(sendfd, payload, sizeof(payload), 0, (struct sockaddr*)&dest, sizeof(dest));

    char buf[1500];
    struct sockaddr_in from{};
    socklen_t fromlen = sizeof(from);
    ssize_t len = recvfrom(recvfd, buf, sizeof(buf), 0, (struct sockaddr*)&from, &fromlen);
    std::string responder;
    if (len > 0) {
        char addrbuf[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &from.sin_addr, addrbuf, sizeof(addrbuf))) {
            responder = addrbuf;
        }
    }

    close(sendfd);
    close(recvfd);
    return responder;
}

// Run full traceroute to dest_ip; for each hop found, call the callback with hop IP.
static void run_traceroute(const std::string& dest_ip,
                           std::function<void(const std::string&)> hop_callback) {
    for (int ttl = 1; ttl <= TR_MAX_HOPS; ++ttl) {
        int port = TR_BASE_PORT + ttl;
        std::string hop = traceroute_probe(dest_ip, ttl, port, TR_PROBE_TIMEOUT_MS);
        if (!hop.empty()) {
            hop_callback(hop);
            if (hop == dest_ip) break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}

// ---------------- Capture thread ----------------
static std::atomic<bool> stopFlag{false};

static void captureLoop(MainWindow* win, GeoIPResolver* geo, ReverseDNS* rdns) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* handle = pcap_open_live("wlan0", SNAPLEN, PROMISC, TIMEOUT_MS, errbuf);
    if (!handle) { std::cerr<<"pcap_open_live failed: "<<errbuf<<std::endl; return; }
    std::unordered_set<std::string> seen;
    std::mutex seen_m;

    while (!stopFlag.load()) {
        struct pcap_pkthdr* hdr; const u_char* data;
        int rc = pcap_next_ex(handle,&hdr,&data);
        if (rc == 1 && hdr && data) {
            std::string src,dst;
            if (parse_ipv4(data,hdr->caplen,src,dst)) {
                for (const std::string& ip : {src,dst}) {
                    {
                        std::lock_guard<std::mutex> lk(seen_m);
                        if (seen.find(ip) != seen.end()) continue;
                        seen.insert(ip);
                    }

                    // Reverse DNS + GeoIP for the remote IP
                    std::string hostname = rdns->lookup(ip);
                    double lat=0.0, lon=0.0;
                    std::string geoLabel;
                    bool geoOk = geo->lookup(ip, lat, lon, geoLabel);

                    if (geoOk) {
                        QString qip = QString::fromStdString(ip);
                        QString qlabel = QString::fromStdString(hostname.empty() ? geoLabel : (hostname + " (" + geoLabel + ")"));
                        QMetaObject::invokeMethod(win, [win,qip,lat,lon,qlabel](){
                            win->addMarker(qip, lat, lon, qlabel);
                        }, Qt::QueuedConnection);
                    }

                    // Start traceroute in a detached thread for this IP
                    std::thread([ip, win, geo, rdns]() {
                        run_traceroute(ip, [win, geo, rdns, ip](const std::string& hopip) {
                            if (hopip.empty()) return;
                            double hlat=0.0, hlon=0.0;
                            std::string hlabel;
                            bool ok = geo->lookup(hopip, hlat, hlon, hlabel);
                            std::string host = rdns->lookup(hopip);
                            std::string label = host.empty() ? hlabel : (host + " (" + hlabel + ")");
                            if (ok) {
                                QString qdest = QString::fromStdString(ip);
                                QString qhop = QString::fromStdString(hopip);
                                QString qlabel = QString::fromStdString(label);
                                QMetaObject::invokeMethod(win, [win,qdest,qhop,hlat,hlon,qlabel](){
                                    win->addTracerHop(qdest, qhop, hlat, hlon, qlabel);
                                }, Qt::QueuedConnection);
                            }
                        });
                    }).detach();
                }
            }
        } else if (rc == 0) {
            continue;
        } else if (rc == -1) {
            std::cerr << "pcap_next_ex error: " << pcap_geterr(handle) << std::endl;
            break;
        } else if (rc == -2) {
            break;
        }
    }
    pcap_close(handle);
}

// ---------------- main ----------------
int main(int argc,char* argv[]) {
    signal(SIGPIPE, SIG_IGN);

    QApplication app(argc,argv);
    MainWindow w; w.resize(1200,800); w.show();

    // Resolve binary path for mmdb
    char exe[PATH_MAX]; ssize_t len = readlink("/proc/self/exe", exe, sizeof(exe)-1);
    if (len == -1) { std::cerr<<"cannot resolve exe path"<<std::endl; return 1; }
    exe[len] = '\0';
    std::string path(exe);
    auto pos = path.find_last_of('/');
    std::string mmdbPath = (pos==std::string::npos) ? "GeoLite2-City.mmdb" : path.substr(0,pos+1) + "GeoLite2-City.mmdb";

    GeoIPResolver geo;
    if (!geo.open(mmdbPath)) {
        std::cerr << "Failed to open GeoLite2 DB at: " << mmdbPath << std::endl;
        return 2;
    }

    ReverseDNS rdns;

    // Start capture thread
    std::thread th(captureLoop, &w, &geo, &rdns);

    // Fetch public IP and add home marker
    QNetworkAccessManager mgr;
    QNetworkRequest req(QUrl("https://api.ipify.org?format=json"));
    QNetworkReply* reply = mgr.get(req);
    QObject::connect(reply, &QNetworkReply::finished, [&w, reply, &geo]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray body = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(body);
            if (!doc.isNull() && doc.isObject()) {
                QJsonObject obj = doc.object();
                QString ip = obj.value("ip").toString();
                if (!ip.isEmpty()) {
                    double lat=0.0, lon=0.0;
                    std::string label;
                    if (geo.lookup(ip.toStdString(), lat, lon, label)) {
                        QString qlabel = QString::fromStdString("Public IP: " + label);
                        QMetaObject::invokeMethod(&w, [=,&w]() {
                            w.addHomeMarker(ip, lat, lon, qlabel);
                        }, Qt::QueuedConnection);
                    } else {
                        std::cerr << "GeoIP lookup failed for public IP: " << ip.toStdString() << std::endl;
                    }
                }
            }
        } else {
            std::cerr << "Failed to fetch public IP: " << reply->errorString().toStdString() << std::endl;
        }
        reply->deleteLater();
    });

    int rc = app.exec();
    stopFlag.store(true);
    th.join();
    return rc;
}

