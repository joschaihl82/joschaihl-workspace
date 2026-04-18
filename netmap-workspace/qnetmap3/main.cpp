// main.cpp
// Build:
//   g++ -O2 -Wall -std=c++17 main.cpp -o qnetmap \
//       -lpcap -lmaxminddb \
//       -lQt5WebEngineWidgets -lQt5Widgets -lQt5Gui -lQt5Core -pthread
//
// Run:
//   sudo ./qnetmap
//
// Requirements:
//   - GeoLite2-City.mmdb in the same directory as ./qnetmap
//   - Qt5 with WebEngine and Network
//   - libpcap, libmaxminddb

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
#include <pcap.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <net/ethernet.h>
#include <maxminddb.h>
#include <unistd.h>
#include <limits.h>
#include <netdb.h>
#include <sys/socket.h>

// pcap capture parameters
static const int SNAPLEN    = 65535;
static const int PROMISC    = 1;
static const int TIMEOUT_MS = 1000;

// ---------------- GUI ----------------
class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent=nullptr) : QMainWindow(parent) {
        view = new QWebEngineView(this);
        setCentralWidget(view);
        setWindowTitle("Live IP Geo Map (with hostnames and home marker)");

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
var ipMarkers = {};
var homeMarker = null;

function addIPMarker(ip, lat, lon, label) {
  if (!lat || !lon) return;
  if (ipMarkers[ip]) return;
  var m = L.circleMarker([lat, lon], {
    radius: 6, color: '#ff4444', weight: 1,
    fillColor: '#ff4444', fillOpacity: 0.8
  }).addTo(map);
  var popupText = '';
  if (label && label.length) popupText = '<div class="label">' + label + '</div><div>' + ip + '</div>';
  else popupText = ip;
  m.bindPopup(popupText);
  ipMarkers[ip] = m;
}

function addHomeMarker(ip, lat, lon, label) {
  if (!lat || !lon) return;
  // remove previous home marker if any
  if (homeMarker) {
    map.removeLayer(homeMarker);
    homeMarker = null;
  }
  // blue marker with larger radius
  homeMarker = L.circleMarker([lat, lon], {
    radius: 8, color: '#2b7cff', weight: 2,
    fillColor: '#2b7cff', fillOpacity: 0.9
  }).addTo(map);
  var popupText = '';
  if (label && label.length) popupText = '<div class="label">' + label + '</div><div>' + ip + '</div>';
  else popupText = ip;
  homeMarker.bindPopup(popupText);
  // center map on home marker (optional)
  map.setView([lat, lon], 6);
}
</script>
</body>
</html>
)HTML";
        view->setHtml(html, QUrl("https://example.local/"));
    }

    void addMarker(const QString& ip, double lat, double lon, const QString& label) {
        QString ipEsc = ip;
        ipEsc.replace("'", "\\'");
        QString labelEsc = label;
        labelEsc.replace("'", "\\'");

        QString js = QString("addIPMarker('%1', %2, %3, '%4');")
                         .arg(ipEsc)
                         .arg(lat, 0, 'f', 6)
                         .arg(lon, 0, 'f', 6)
                         .arg(labelEsc);
        view->page()->runJavaScript(js);
    }

    void addHomeMarker(const QString& ip, double lat, double lon, const QString& label) {
        QString ipEsc = ip;
        ipEsc.replace("'", "\\'");
        QString labelEsc = label;
        labelEsc.replace("'", "\\'");

        QString js = QString("addHomeMarker('%1', %2, %3, '%4');")
                         .arg(ipEsc)
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
    // Try to get hostname for IPv4 string; returns empty string if none
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

// ---------------- Capture thread ----------------
static std::atomic<bool> stopFlag{false};

static void captureLoop(MainWindow* win, GeoIPResolver* geo, ReverseDNS* rdns) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* handle = pcap_open_live("wlan0", SNAPLEN, PROMISC, TIMEOUT_MS, errbuf);
    if (!handle) { std::cerr<<"pcap_open_live failed: "<<errbuf<<std::endl; return; }
    std::unordered_set<std::string> seen;
    while (!stopFlag.load()) {
        struct pcap_pkthdr* hdr; const u_char* data;
        int rc = pcap_next_ex(handle,&hdr,&data);
        if (rc == 1 && hdr && data) {
            std::string src,dst;
            if (parse_ipv4(data,hdr->caplen,src,dst)) {
                for (const std::string& ip : {src,dst}) {
                    if (seen.find(ip) != seen.end()) continue;

                    // First try reverse DNS (hostname)
                    std::string hostname = rdns->lookup(ip);

                    // Then try GeoIP
                    double lat=0.0, lon=0.0;
                    std::string geoLabel;
                    bool geoOk = geo->lookup(ip, lat, lon, geoLabel);

                    // If we have either hostname or geo coords, add marker
                    if (!hostname.empty() && geoOk) {
                        std::string label = hostname + " (" + geoLabel + ")";
                        seen.insert(ip);
                        QString qip = QString::fromStdString(ip);
                        QString qlabel = QString::fromStdString(label);
                        QMetaObject::invokeMethod(win, [win,qip,lat,lon,qlabel](){
                            win->addMarker(qip, lat, lon, qlabel);
                        }, Qt::QueuedConnection);
                    } else if (!hostname.empty() && !geoOk) {
                        seen.insert(ip);
                    } else if (hostname.empty() && geoOk) {
                        std::string label = geoLabel;
                        seen.insert(ip);
                        QString qip = QString::fromStdString(ip);
                        QString qlabel = QString::fromStdString(label);
                        QMetaObject::invokeMethod(win, [win,qip,lat,lon,qlabel](){
                            win->addMarker(qip, lat, lon, qlabel);
                        }, Qt::QueuedConnection);
                    } else {
                        seen.insert(ip);
                    }
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

    // Fetch public IP using a simple HTTP GET (ipify). This runs in the GUI thread.
    QNetworkAccessManager *mgr = new QNetworkAccessManager(&w);
    QNetworkRequest req(QUrl("https://api.ipify.org?format=json"));
    QNetworkReply *reply = mgr->get(req);
    QObject::connect(reply, &QNetworkReply::finished, [&w, reply, &geo]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray body = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(body);
            if (!doc.isNull() && doc.isObject()) {
                QJsonObject obj = doc.object();
                QString ip = obj.value("ip").toString();
                if (!ip.isEmpty()) {
                    // Lookup GeoIP for public IP
                    double lat=0.0, lon=0.0;
                    std::string label;
                    bool ok = geo.lookup(ip.toStdString(), lat, lon, label);
                    if (ok) {
                        QString qlabel = QString::fromStdString("Public IP: " + label);
                        // Add blue home marker
                        QMetaObject::invokeMethod(&w, [=,&w]() {
                            w.addHomeMarker(ip, lat, lon, qlabel);
                        }, Qt::QueuedConnection);
                    } else {
                        // If GeoIP fails, still show IP as label but no marker (no coords)
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

