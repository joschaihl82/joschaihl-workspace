// main.cpp
// Qt5 GUI proxy checker with progress, file watch, CSV export and "Launch Chromium with best proxy"
// Added: after protocol-level success, perform an HTTP GET to http://www.google.com/ and require HTTP 200 OK.
#include <QtWidgets>
#include <atomic>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <cerrno>
#include <ctime>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <arpa/inet.h>

#define DEFAULT_TIMEOUT_MS 3000
#define WORKER_COUNT 50

    enum ProtoMode { MODE_HTTP, MODE_HTTPS, MODE_SOCKS4, MODE_SOCKS5 };

struct Job {
    QString host;
    QString port;
    ProtoMode mode;
};

struct Result {
    QString host;
    QString port;
    ProtoMode mode;
    bool ok;
    long tcp_ms;
    long proto_ms;
    long http_ms;
    QString statusText;
};

static long timespec_diff_ms(const struct timespec *a, const struct timespec *b) {
    long s = a->tv_sec - b->tv_sec;
    long ns = a->tv_nsec - b->tv_nsec;
    return s * 1000 + ns / 1000000;
}

static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static int wait_fd(int fd, int events, int timeout_ms) {
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = events;
    int r;
    do { r = poll(&pfd, 1, timeout_ms); } while (r < 0 && errno == EINTR);
    return r;
}

static int connect_nonblocking_addr(struct addrinfo *ai, int timeout_ms, struct timespec *t_start, struct timespec *t_end, int *out_sock) {
    int s = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (s < 0) return -1;
    set_nonblocking(s);
    clock_gettime(CLOCK_MONOTONIC, t_start);
    int rc = connect(s, ai->ai_addr, ai->ai_addrlen);
    if (rc == 0) {
        clock_gettime(CLOCK_MONOTONIC, t_end);
        *out_sock = s;
        return 0;
    } else if (errno == EINPROGRESS) {
        int pol = wait_fd(s, POLLOUT, timeout_ms);
        if (pol > 0) {
            int err = 0;
            socklen_t len = sizeof(err);
            if (getsockopt(s, SOL_SOCKET, SO_ERROR, &err, &len) < 0) err = errno;
            clock_gettime(CLOCK_MONOTONIC, t_end);
            if (err == 0) {
                *out_sock = s;
                return 0;
            } else {
                close(s);
                errno = err;
                return -1;
            }
        } else if (pol == 0) {
            close(s);
            errno = ETIMEDOUT;
            return -1;
        } else {
            close(s);
            return -1;
        }
    } else {
        close(s);
        return -1;
    }
}

static int tcp_connect_to(const char *host, const char *port, int timeout_ms, int *out_sock, struct timespec *t_start, struct timespec *t_end) {
    struct addrinfo hints, *res, *rp;
    int gai;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    if ((gai = getaddrinfo(host, port, &hints, &res)) != 0) {
        errno = EINVAL;
        return -1;
    }
    int ok = -1;
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        if (connect_nonblocking_addr(rp, timeout_ms, t_start, t_end, out_sock) == 0) {
            ok = 0;
            break;
        }
    }
    freeaddrinfo(res);
    return ok;
}

static ssize_t send_all_timeout(int fd, const void *buf, size_t len, int timeout_ms) {
    size_t sent = 0;
    const char *p = (const char*)buf;
    while (sent < len) {
        int pol = wait_fd(fd, POLLOUT, timeout_ms);
        if (pol <= 0) return -1;
        ssize_t w = send(fd, p + sent, len - sent, 0);
        if (w < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        sent += w;
    }
    return (ssize_t)sent;
}

static ssize_t recv_some_timeout(int fd, void *buf, size_t len, int timeout_ms) {
    int pol = wait_fd(fd, POLLIN, timeout_ms);
    if (pol <= 0) return -1;
    ssize_t r = recv(fd, buf, len, 0);
    return r;
}

static int do_http_connect(int sock, const char *t_host, const char *t_port, int timeout_ms) {
    char req[512];
    snprintf(req, sizeof(req),
             "CONNECT %s:%s HTTP/1.1\r\nHost: %s:%s\r\nProxy-Connection: Keep-Alive\r\n\r\n",
             t_host, t_port, t_host, t_port);
    if (send_all_timeout(sock, req, strlen(req), timeout_ms) < 0) return -1;
    char buf[1024];
    size_t got = 0;
    while (1) {
        ssize_t r = recv_some_timeout(sock, buf + got, sizeof(buf) - got - 1, timeout_ms);
        if (r <= 0) return -1;
        got += r;
        buf[got] = '\0';
        if (strstr(buf, "\r\n\r\n")) break;
        if (got >= sizeof(buf) - 1) break;
    }
    if (strstr(buf, "200") != NULL) return 0;
    return -1;
}

static int do_socks4(int sock, const char *t_host, const char *t_port, int timeout_ms) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(t_host, t_port, &hints, &res) != 0) return -1;
    struct sockaddr_in *sin = (struct sockaddr_in *)res->ai_addr;
    uint16_t portn = sin->sin_port; // network order
    uint32_t ipn = sin->sin_addr.s_addr;
    freeaddrinfo(res);

    unsigned char req[9];
    req[0] = 0x04;
    req[1] = 0x01;
    memcpy(&req[2], &portn, 2);
    memcpy(&req[4], &ipn, 4);
    if (send_all_timeout(sock, req, 8, timeout_ms) < 0) return -1;
    unsigned char zero = 0x00;
    if (send_all_timeout(sock, &zero, 1, timeout_ms) < 0) return -1;
    unsigned char resp[8];
    ssize_t r = recv_some_timeout(sock, resp, sizeof(resp), timeout_ms);
    if (r < 8) return -1;
    if (resp[1] == 0x5A) return 0;
    return -1;
}

static int do_socks5(int sock, const char *t_host, const char *t_port, int timeout_ms) {
    unsigned char greet[3] = {0x05, 0x01, 0x00};
    if (send_all_timeout(sock, greet, 3, timeout_ms) < 0) return -1;
    unsigned char sel[2];
    ssize_t r = recv_some_timeout(sock, sel, 2, timeout_ms);
    if (r != 2) return -1;
    if (sel[0] != 0x05 || sel[1] == 0xFF) return -1;

    unsigned char req[512];
    size_t reqlen = 0;
    req[0] = 0x05; req[1] = 0x01; req[2] = 0x00;
    struct in6_addr in6;
    struct in_addr in4;
    if (inet_pton(AF_INET, t_host, &in4) == 1) {
        req[3] = 0x01;
        memcpy(&req[4], &in4, 4);
        reqlen = 4 + 4;
    } else if (inet_pton(AF_INET6, t_host, &in6) == 1) {
        req[3] = 0x04;
        memcpy(&req[4], &in6, 16);
        reqlen = 4 + 16;
    } else {
        req[3] = 0x03;
        size_t hlen = strlen(t_host);
        if (hlen > 255) return -1;
        req[4] = (unsigned char)hlen;
        memcpy(&req[5], t_host, hlen);
        reqlen = 5 + hlen;
    }
    uint16_t portn = htons((uint16_t)atoi(t_port));
    memcpy(&req[reqlen], &portn, 2);
    reqlen += 2;
    if (send_all_timeout(sock, req, 3 + reqlen, timeout_ms) < 0) return -1;

    unsigned char header[4];
    r = recv_some_timeout(sock, header, 4, timeout_ms);
    if (r != 4) return -1;
    if (header[0] != 0x05 || header[1] != 0x00) return -1;
    unsigned char atyp = header[3];
    size_t toread = 0;
    if (atyp == 0x01) toread = 4 + 2;
    else if (atyp == 0x04) toread = 16 + 2;
    else if (atyp == 0x03) {
        unsigned char lenb;
        r = recv_some_timeout(sock, &lenb, 1, timeout_ms);
        if (r != 1) return -1;
        toread = (size_t)lenb + 2;
    } else return -1;
    unsigned char *rest = (unsigned char*)malloc(toread);
    if (!rest) return -1;
    r = recv_some_timeout(sock, rest, toread, timeout_ms);
    free(rest);
    if (r != (ssize_t)toread) return -1;
    return 0;
}

// Perform an HTTP GET to given host/path over an already-established TCP socket.
// Returns 0 on HTTP 200 OK, -1 otherwise. Measures time in ms via http_ms_out if non-null.
static int perform_http_get_check(int sock, const char *host, const char *path, int timeout_ms, long *http_ms_out) {
    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);

    char req[512];
    // Use HTTP/1.1 with Host header and Connection: close
    snprintf(req, sizeof(req),
             "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: qproxyperf/1.0\r\nConnection: close\r\n\r\n",
             path, host);
    if (send_all_timeout(sock, req, strlen(req), timeout_ms) < 0) return -1;

    // Read response headers (we only need status line)
    char buf[2048];
    size_t got = 0;
    bool header_done = false;
    while (!header_done) {
        ssize_t r = recv_some_timeout(sock, buf + got, sizeof(buf) - got - 1, timeout_ms);
        if (r <= 0) return -1;
        got += r;
        buf[got] = '\0';
        if (strstr(buf, "\r\n\r\n")) header_done = true;
        if (got >= sizeof(buf) - 1) break;
    }

    clock_gettime(CLOCK_MONOTONIC, &t_end);
    if (http_ms_out) *http_ms_out = timespec_diff_ms(&t_end, &t_start);

    // parse status line
    // status line is first line up to \r\n
    char *line_end = strstr(buf, "\r\n");
    if (!line_end) return -1;
    *line_end = '\0';
    // Example: "HTTP/1.1 200 OK"
    if (strstr(buf, "200") != NULL) return 0;
    return -1;
}

// Thread-safe queue
class JobQueue {
public:
    void push(const Job &j) {
        std::unique_lock<std::mutex> lk(m);
        q.push(j);
        cv.notify_one();
    }
    bool pop(Job &out) {
        std::unique_lock<std::mutex> lk(m);
        while (q.empty() && !closed) cv.wait(lk);
        if (q.empty()) return false;
        out = q.front(); q.pop();
        return true;
    }
    void close() {
        std::unique_lock<std::mutex> lk(m);
        closed = true;
        cv.notify_all();
    }
private:
    std::queue<Job> q;
    std::mutex m;
    std::condition_variable cv;
    bool closed = false;
};

// Worker thread that performs checks and emits results via a Qt signal
class Worker : public QObject {
    Q_OBJECT
public:
    Worker(JobQueue *queue, int timeoutMs) : q(queue), timeout(timeoutMs) {}
    void start() {
        thr = std::thread([this]{ run(); });
    }
    void join() {
        if (thr.joinable()) thr.join();
    }
signals:
    void resultReady(const Result &res);
private:
    JobQueue *q;
    int timeout;
    std::thread thr;

    void run() {
        Job job;
        while (q->pop(job)) {
            Result r;
            r.host = job.host;
            r.port = job.port;
            r.mode = job.mode;
            r.ok = false;
            r.tcp_ms = -1;
            r.proto_ms = -1;
            r.http_ms = -1;
            r.statusText = "UNKNOWN";

            struct timespec t_tcp_start, t_tcp_end, t_proto_end;
            int sock = -1;
            if (tcp_connect_to(job.host.toUtf8().constData(), job.port.toUtf8().constData(), timeout, &sock, &t_tcp_start, &t_tcp_end) != 0) {
                r.statusText = QString("TCP_ERR %1").arg(strerror(errno));
                emit resultReady(r);
                continue;
            }
            r.tcp_ms = timespec_diff_ms(&t_tcp_end, &t_tcp_start);

            // choose target for protocol handshake (kept from original: example.com)
            const char *t_host = "example.com";
            const char *t_port = (job.mode == MODE_HTTPS) ? "443" : "80";

            bool proto_ok = false;
            if (job.mode == MODE_HTTP || job.mode == MODE_HTTPS) {
                if (do_http_connect(sock, t_host, t_port, timeout) == 0) {
                    clock_gettime(CLOCK_MONOTONIC, &t_proto_end);
                    proto_ok = true;
                } else {
                    r.statusText = QString("PROTO_ERR http-connect");
                    close(sock);
                    emit resultReady(r);
                    continue;
                }
            } else if (job.mode == MODE_SOCKS4) {
                if (do_socks4(sock, t_host, t_port, timeout) == 0) {
                    clock_gettime(CLOCK_MONOTONIC, &t_proto_end);
                    proto_ok = true;
                } else {
                    r.statusText = QString("PROTO_ERR socks4");
                    close(sock);
                    emit resultReady(r);
                    continue;
                }
            } else if (job.mode == MODE_SOCKS5) {
                if (do_socks5(sock, t_host, t_port, timeout) == 0) {
                    clock_gettime(CLOCK_MONOTONIC, &t_proto_end);
                    proto_ok = true;
                } else {
                    r.statusText = QString("PROTO_ERR socks5");
                    close(sock);
                    emit resultReady(r);
                    continue;
                }
            }

            if (!proto_ok) {
                r.statusText = QString("PROTO_ERR");
                close(sock);
                emit resultReady(r);
                continue;
            }
            r.proto_ms = timespec_diff_ms(&t_proto_end, &t_tcp_end);

            // Now perform the HTTP GET to google.com over the established connection.
            // For HTTP proxies we used CONNECT above, so the socket is a direct tunnel to the target.
            // For SOCKS4/5 the handshake already connected to the target.
            // We'll request "GET /" on host "www.google.com" and expect HTTP 200 in response headers.
            long http_ms = -1;
            int http_ok = perform_http_get_check(sock, "www.google.com", "/", timeout, &http_ms);
            r.http_ms = http_ms;
            if (http_ok == 0) {
                r.ok = true;
                r.statusText = QString("OK 200");
            } else {
                r.ok = false;
                r.statusText = QString("HTTP_ERR");
            }

            close(sock);
            emit resultReady(r);
        }
    }
};

class LatencySortProxyModel : public QSortFilterProxyModel {
public:
    LatencySortProxyModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {}
protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override {
        QVariant lStatus = sourceModel()->index(left.row(), 5).data();
        QVariant rStatus = sourceModel()->index(right.row(), 5).data();
        bool lOk = (lStatus.toString().startsWith("OK"));
        bool rOk = (rStatus.toString().startsWith("OK"));
        if (lOk != rOk) return lOk; // OK rows come before non-OK
        if (lOk && rOk) {
            // read numeric values stored in Qt::UserRole
            long lTcp = sourceModel()->index(left.row(), 3).data(Qt::UserRole).toLongLong();
            long lProto = sourceModel()->index(left.row(), 4).data(Qt::UserRole).toLongLong();
            long lHttp = sourceModel()->index(left.row(), 3).data(Qt::UserRole + 1).toLongLong(); // fallback if stored differently
            Q_UNUSED(lHttp);
            long rTcp = sourceModel()->index(right.row(), 3).data(Qt::UserRole).toLongLong();
            long rProto = sourceModel()->index(right.row(), 4).data(Qt::UserRole).toLongLong();
            long lTotal = (lTcp < 0 ? 0 : lTcp) + (lProto < 0 ? 0 : lProto);
            long rTotal = (rTcp < 0 ? 0 : rTcp) + (rProto < 0 ? 0 : rProto);
            return lTotal < rTotal;
        }
        // fallback to lexicographic by host
        QString lHost = sourceModel()->index(left.row(), 0).data().toString();
        QString rHost = sourceModel()->index(right.row(), 0).data().toString();
        return QString::localeAwareCompare(lHost, rHost) < 0;
    }
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        resize(1000, 650);

        // Model and view
        model = new QStandardItemModel(this);
        model->setHorizontalHeaderLabels(QStringList() << "Host" << "Port" << "Mode" << "TCP ms" << "PROTO ms" << "Status");

        proxy = new LatencySortProxyModel(this);
        proxy->setSourceModel(model);
        proxy->setSortCaseSensitivity(Qt::CaseInsensitive);

        view = new QTableView(this);
        view->setModel(proxy);
        view->setSortingEnabled(true);
        view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        setCentralWidget(view);

        // Toolbar actions
        QToolBar *tb = addToolBar("Main");
        QAction *exportAct = tb->addAction("Export OK to CSV");
        connect(exportAct, &QAction::triggered, this, &MainWindow::exportOkToCsv);

        QAction *reloadAct = tb->addAction("Reload Now");
        connect(reloadAct, &QAction::triggered, this, &MainWindow::manualReload);

        launchChromiumAction = tb->addAction("Launch Chromium (best proxy)");
        connect(launchChromiumAction, &QAction::triggered, this, &MainWindow::launchChromiumWithBestProxy);

        // Status widgets
        QWidget *statusWidget = new QWidget(this);
        QHBoxLayout *statusLayout = new QHBoxLayout(statusWidget);
        statusLayout->setContentsMargins(4,4,4,4);
        totalLabel = new QLabel("Total 0", this);
        processedLabel = new QLabel("Processed 0", this);
        okLabel = new QLabel("OK 0", this);
        failedLabel = new QLabel("Failed 0", this);
        progressBar = new QProgressBar(this);
        progressBar->setMinimum(0);
        progressBar->setValue(0);
        progressBar->setTextVisible(true);
        progressBar->setMinimumWidth(200);
        statusLayout->addWidget(totalLabel);
        statusLayout->addWidget(processedLabel);
        statusLayout->addWidget(okLabel);
        statusLayout->addWidget(failedLabel);
        statusLayout->addWidget(progressBar);
        statusLayout->addStretch();
        statusWidget->setLayout(statusLayout);
        statusBar()->addPermanentWidget(statusWidget, 1);

        // Worker pool
        queue = new JobQueue();
        for (int i = 0; i < WORKER_COUNT; ++i) {
            Worker *w = new Worker(queue, DEFAULT_TIMEOUT_MS);
            workers.push_back(w);
            connect(w, &Worker::resultReady, this, &MainWindow::onResultReady, Qt::QueuedConnection);
            w->start();
        }

        // File watcher
        watcher = new QFileSystemWatcher(this);
        connect(watcher, &QFileSystemWatcher::fileChanged, this, &MainWindow::onFileChanged);

        // Load initial jobs
        dataFilePath = QDir::current().filePath("data.txt");
        loadJobsFromFile(dataFilePath);

        // watch file if exists
        if (QFile::exists(dataFilePath)) {
            watcher->addPath(dataFilePath);
        }
    }

    ~MainWindow() {
        queue->close();
        for (Worker *w : workers) {
            w->join();
            delete w;
        }
        delete queue;
    }

private slots:
    void onResultReady(const Result &r) {
        // Append a row
        QList<QStandardItem*> row;
        row << new QStandardItem(r.host)
            << new QStandardItem(r.port)
            << new QStandardItem(modeToString(r.mode))
            << new QStandardItem(r.tcp_ms >= 0 ? QString::number(r.tcp_ms) : QString())
            << new QStandardItem(r.proto_ms >= 0 ? QString::number(r.proto_ms) : QString())
            << new QStandardItem(r.statusText);

        // store numeric values for sorting using explicit qlonglong QVariant
        row[3]->setData(QVariant::fromValue((qlonglong)r.tcp_ms), Qt::UserRole);
        row[4]->setData(QVariant::fromValue((qlonglong)r.proto_ms), Qt::UserRole);
        // store http_ms in a secondary role on column 3 (not used for sorting currently)
        row[3]->setData(QVariant::fromValue((qlonglong)r.http_ms), Qt::UserRole + 1);

        model->appendRow(row);

        // update counts and progress
        processedCount++;
        if (r.ok) okCount++; else failedCount++;
        updateStatusWidgets();
    }

    void onFileChanged(const QString &path) {
        Q_UNUSED(path);
        // small delay to allow writer to finish
        QTimer::singleShot(200, this, [this](){
            // re-add watch (some editors replace file)
            if (QFile::exists(dataFilePath)) {
                if (!watcher->files().contains(dataFilePath)) watcher->addPath(dataFilePath);
            }
            appendNewLinesFromFile(dataFilePath);
        });
    }

    void exportOkToCsv() {
        QString fn = QFileDialog::getSaveFileName(this, "Export OK to CSV", QDir::current().filePath("ok_proxies.csv"), "CSV files (*.csv);;All files (*)");
        if (fn.isEmpty()) return;
        QFile f(fn);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Export Failed", QString("Cannot write file: %1").arg(f.errorString()));
            return;
        }
        QTextStream out(&f);
        out << "Host,Port,Mode,TCP ms,PROTO ms,HTTP ms,Status\n";
        // iterate source model rows and write OK rows
        for (int r = 0; r < model->rowCount(); ++r) {
            QString status = model->index(r, 5).data().toString();
            if (!status.startsWith("OK")) continue;
            QString host = model->index(r, 0).data().toString();
            QString port = model->index(r, 1).data().toString();
            QString mode = model->index(r, 2).data().toString();
            QString tcp = model->index(r, 3).data().toString();
            QString proto = model->index(r, 4).data().toString();
            // http_ms stored in UserRole+1 of column 3
            qlonglong httpms = model->item(r, 3)->data(Qt::UserRole + 1).toLongLong();
            QString http = httpms >= 0 ? QString::number(httpms) : QString();
            // simple CSV escaping
            auto esc = [](const QString &s)->QString {
                QString t = s;
                if (t.contains('"')) t.replace("\"", "\"\"");
                if (t.contains(',') || t.contains('"') || t.contains('\n')) t = "\"" + t + "\"";
                return t;
            };
            out << esc(host) << "," << esc(port) << "," << esc(mode) << "," << esc(tcp) << "," << esc(proto) << "," << esc(http) << "," << esc(status) << "\n";
        }
        f.close();
        statusBar()->showMessage(QString("Exported OK rows to %1").arg(fn), 5000);
    }

    void manualReload() {
        // reload entire file and queue any unseen lines
        loadJobsFromFile(dataFilePath);
    }

    void launchChromiumWithBestProxy() {
        // Find first OK row in the source model (fastest OK)
        int rows = model->rowCount();
        int foundRow = -1;
        for (int r = 0; r < rows; ++r) {
            QString status = model->index(r, 5).data().toString();
            if (status.startsWith("OK")) { foundRow = r; break; }
        }

        if (foundRow < 0) {
            QMessageBox::information(this, "No proxy", "No successful (OK) proxy available to launch Chromium.");
            return;
        }

        QString host = model->index(foundRow, 0).data().toString();
        QString port = model->index(foundRow, 1).data().toString();
        QString mode = model->index(foundRow, 2).data().toString();

        QString proxySpec;
        if (mode.compare("SOCKS5", Qt::CaseInsensitive) == 0) {
            proxySpec = QString("socks5://%1:%2").arg(host, port);
        } else if (mode.compare("SOCKS4", Qt::CaseInsensitive) == 0) {
            proxySpec = QString("socks4://%1:%2").arg(host, port);
        } else {
            // HTTP/HTTPS proxies use http:// scheme for Chromium proxy-server
            proxySpec = QString("http://%1:%2").arg(host, port);
        }

        // Build arguments
        QStringList args;
        args << QString("--proxy-server=%1").arg(proxySpec);
        args << QString("--proxy-bypass-list=<-loopback>");
        QString tmpProfile = QDir::temp().filePath(QString("qproxy_chrome_profile_%1").arg(QCoreApplication::applicationPid()));
        args << QString("--user-data-dir=%1").arg(tmpProfile);

        // Try common binary names
        QStringList candidates = { "chromium", "chromium-browser", "google-chrome", "chrome" };
        bool started = false;
        for (const QString &bin : candidates) {
            QString exe = QStandardPaths::findExecutable(bin);
            if (exe.isEmpty()) continue;
            bool ok = QProcess::startDetached(exe, args);
            if (ok) { started = true; break; }
        }

        if (!started) {
            // fallback: try starting "chromium" directly (may still fail)
            bool ok = QProcess::startDetached("chromium", args);
            if (!ok) {
                QMessageBox::warning(this, "Launch failed",
                                     QString("Could not start Chromium. Ensure Chromium/Chrome is installed and on PATH.\nProxy: %1").arg(proxySpec));
                return;
            }
        }

        statusBar()->showMessage(QString("Launched Chromium with proxy %1").arg(proxySpec), 5000);
    }

private:
    QTableView *view;
    QStandardItemModel *model;
    LatencySortProxyModel *proxy;
    JobQueue *queue;
    std::vector<Worker*> workers;
    QFileSystemWatcher *watcher;
    QString dataFilePath;

    // status widgets
    QLabel *totalLabel;
    QLabel *processedLabel;
    QLabel *okLabel;
    QLabel *failedLabel;
    QProgressBar *progressBar;

    // toolbar action
    QAction *launchChromiumAction;

    // counters and seen set
    std::atomic<int> processedCount{0};
    std::atomic<int> okCount{0};
    std::atomic<int> failedCount{0};
    std::atomic<int> totalCount{0};
    QSet<QString> seenLines;
    std::mutex seenMutex;

    static QString modeToString(ProtoMode m) {
        switch (m) {
        case MODE_HTTP: return "HTTP";
        case MODE_HTTPS: return "HTTPS";
        case MODE_SOCKS4: return "SOCKS4";
        case MODE_SOCKS5: return "SOCKS5";
        }
        return "UNKNOWN";
    }

    static int parse_line_to_job(const QString &line, Job &out) {
        QString s = line.trimmed();
        if (s.isEmpty()) return 0;
        ProtoMode mode = MODE_HTTP;
        QString p = s;
        if (p.startsWith("socks5://", Qt::CaseInsensitive)) { mode = MODE_SOCKS5; p = p.mid(9); }
        else if (p.startsWith("socks4://", Qt::CaseInsensitive)) { mode = MODE_SOCKS4; p = p.mid(9); }
        else if (p.startsWith("http://", Qt::CaseInsensitive)) { mode = MODE_HTTP; p = p.mid(7); }
        else if (p.startsWith("https://", Qt::CaseInsensitive)) { mode = MODE_HTTPS; p = p.mid(8); }
        int colon = p.lastIndexOf(':');
        if (colon < 0) return -1;
        QString host = p.left(colon);
        QString port = p.mid(colon + 1);
        if (host.isEmpty() || port.isEmpty()) return -1;
        out.host = host;
        out.port = port;
        out.mode = mode;
        return 1;
    }

    void updateStatusWidgets() {
        totalLabel->setText(QString("Total %1").arg(totalCount.load()));
        processedLabel->setText(QString("Processed %1").arg(processedCount.load()));
        okLabel->setText(QString("OK %1").arg(okCount.load()));
        failedLabel->setText(QString("Failed %1").arg(failedCount.load()));
        int tot = totalCount.load();
        int proc = processedCount.load();
        if (tot > 0) {
            progressBar->setMaximum(tot);
            progressBar->setValue(proc);
        } else {
            progressBar->setMaximum(1);
            progressBar->setValue(0);
        }
    }

    void loadJobsFromFile(const QString &filename) {
        QFile f(filename);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            // file may not exist yet; set counts to zero
            totalCount = 0;
            updateStatusWidgets();
            return;
        }
        QTextStream in(&f);
        int newCount = 0;
        std::lock_guard<std::mutex> lk(seenMutex);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QString key = line.trimmed();
            if (key.isEmpty()) continue;
            if (seenLines.contains(key)) continue;
            Job job;
            int r = parse_line_to_job(line, job);
            if (r == 1) {
                queue->push(job);
                seenLines.insert(key);
                newCount++;
            } else {
                qWarning() << "Skipping malformed line:" << line;
            }
        }
        f.close();
        totalCount += newCount;
        updateStatusWidgets();
        // ensure watcher watches file
        if (!watcher->files().contains(filename)) {
            watcher->addPath(filename);
        }
    }

    void appendNewLinesFromFile(const QString &filename) {
        QFile f(filename);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
        QTextStream in(&f);
        // read all lines but only queue unseen ones
        int newCount = 0;
        std::lock_guard<std::mutex> lk(seenMutex);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QString key = line.trimmed();
            if (key.isEmpty()) continue;
            if (seenLines.contains(key)) continue;
            Job job;
            int r = parse_line_to_job(line, job);
            if (r == 1) {
                queue->push(job);
                seenLines.insert(key);
                newCount++;
            } else {
                qWarning() << "Skipping malformed line:" << line;
            }
        }
        f.close();
        totalCount += newCount;
        updateStatusWidgets();
    }
};

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}

#include "main.moc"
