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
//   - Qt5 with WebEngine
//   - libpcap, libmaxminddb

#include <QApplication>
#include <QMainWindow>
#include <QWebEngineView>
#include <QMetaObject>
#include <QUrl>
#include <thread>
#include <atomic>
#include <unordered_set>
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
        setWindowTitle("Live IP Geo Map");

        const char html[] = R"HTML(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8"/>
  <title>IP Map</title>
  <link rel="stylesheet" href="https://unpkg.com/leaflet/dist/leaflet.css"/>
  <script src="https://unpkg.com/leaflet/dist/leaflet.js"></script>
  <style>html,body,#map{height:100%;margin:0;}</style>
</head>
<body>
<div id="map"></div>
<script>
var map = L.map('map').setView([20,0], 2);
L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png',
 { attribution: '© OpenStreetMap contributors'}).addTo(map);
var ipMarkers = {};
function addIPMarker(ip, lat, lon, label) {
  if (!lat || !lon) return;
  if (ipMarkers[ip]) return;
  var m = L.circleMarker([lat, lon], {
    radius: 6, color: '#ff4444', weight: 1,
    fillColor: '#ff4444', fillOpacity: 0.8
  }).addTo(map);
  m.bindPopup(label ? label : ip);
  ipMarkers[ip] = m;
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

// ---------------- pcap ----------------
static bool parse_ipv4(const u_char* bytes, size_t caplen, std::string& src, std::string& dst) {
    if (caplen < sizeof(ether_header)) return false;
    const struct ether_header* eth = (const struct ether_header*)bytes;
    if (ntohs(eth->ether_type) != ETHERTYPE_IP) return false;
    if (caplen < sizeof(ether_header)+sizeof(struct ip)) return false;
    const struct ip* iphdr = (const struct ip*)(bytes+sizeof(ether_header));
    char ssrc[INET_ADDRSTRLEN], sdst[INET_ADDRSTRLEN];
    inet_ntop(AF_INET,&iphdr->ip_src,ssrc,sizeof(ssrc));
    inet_ntop(AF_INET,&iphdr->ip_dst,sdst,sizeof(sdst));
    src=ssrc; dst=sdst; return true;
}

// ---------------- Capture thread ----------------
static std::atomic<bool> stopFlag{false};

static void captureLoop(MainWindow* win, GeoIPResolver* geo) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* handle = pcap_open_live("wlan0", SNAPLEN, PROMISC, TIMEOUT_MS, errbuf);
    if (!handle) { std::cerr<<"pcap_open_live failed: "<<errbuf<<std::endl; return; }
    std::unordered_set<std::string> seen;
    while (!stopFlag.load()) {
        struct pcap_pkthdr* hdr; const u_char* data;
        int rc = pcap_next_ex(handle,&hdr,&data);
        if (rc==1) {
            std::string src,dst;
            if (parse_ipv4(data,hdr->caplen,src,dst)) {
                for (auto& ip: {src,dst}) {
                    if (seen.find(ip)!=seen.end()) continue;
                    double lat,lon; std::string label;
                    if (geo->lookup(ip,lat,lon,label)) {
                        seen.insert(ip);
                        QString qip=QString::fromStdString(ip);
                        QString qlabel=QString::fromStdString(label);
                        QMetaObject::invokeMethod(win,[win,qip,lat,lon,qlabel](){
                                win->addMarker(qip,lat,lon,qlabel);
                            },Qt::QueuedConnection);
                    }
                }
            }
        }
    }
    pcap_close(handle);
}

// ---------------- main ----------------
int main(int argc,char* argv[]) {
    QApplication app(argc,argv);
    MainWindow w; w.resize(1200,800); w.show();

    // Resolve binary path for mmdb
    char exe[PATH_MAX]; ssize_t len=readlink("/proc/self/exe",exe,sizeof(exe)-1);
    if (len==-1) { std::cerr<<"cannot resolve exe path"<<std::endl; return 1; }
    exe[len]='\0';
    std::string path(exe);
    auto pos=path.find_last_of('/');
    std::string mmdbPath = (pos==std::string::npos)? "GeoLite2-City.mmdb" : path.substr(0,pos+1)+"GeoLite2-City.mmdb";

    GeoIPResolver geo;
    if (!geo.open(mmdbPath)) return 2;

    std::thread th(captureLoop,&w,&geo);

    int rc=app.exec();
    stopFlag.store(true);
    th.join();
    return rc;
}
