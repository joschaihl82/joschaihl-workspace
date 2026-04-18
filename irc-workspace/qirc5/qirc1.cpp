// irc_qt_users_with_modes.cpp
//
// Single-file Qt5 IRC client (POSIX C library + Qt GUI).
// Preserves operator/voice status in channel user lists.
//
// Build:
//   g++ -std=c++11 irc_qt_users_with_modes.cpp -o irc_qt_users_with_modes $(pkg-config --cflags --libs Qt5Widgets) -pthread
//
// Note: This targets POSIX (Linux/macOS). Windows needs Winsock changes.

#define _POSIX_C_SOURCE 200112L

extern "C" {

/* ---------------------------
   Minimal C IRC library (amalgamated)
   --------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stddef.h>
#include <sys/time.h>
#include <sys/select.h>

typedef struct {
    int sockfd;
    char *server;
    char *port;
    char *nick;
    char *user;
} IRCConnection;

int irc_connect(IRCConnection *conn, const char *host, const char *port);
int irc_register(IRCConnection *conn, const char *nick, const char *user);
int irc_send_raw(IRCConnection *conn, const char *line);
int irc_join(IRCConnection *conn, const char *channel);
int irc_privmsg(IRCConnection *conn, const char *target, const char *message);
ssize_t irc_read_once(IRCConnection *conn, char *buf, size_t buflen);
void irc_disconnect(IRCConnection *conn);

static int create_and_connect(const char *host, const char *port) {
    struct addrinfo hints, *res, *rp;
    int sfd = -1;
    int rc;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    rc = getaddrinfo(host, port, &hints, &res);
    if (rc != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
        return -1;
    }
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) continue;
        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) == 0) break;
        close(sfd);
        sfd = -1;
    }
    freeaddrinfo(res);
    return sfd;
}

int irc_connect(IRCConnection *conn, const char *host, const char *port) {
    if (!conn || !host || !port) return -1;
    int sfd = create_and_connect(host, port);
    if (sfd < 0) return -1;
    conn->sockfd = sfd;
    conn->server = strdup(host);
    conn->port = strdup(port);
    conn->nick = NULL;
    conn->user = NULL;
    return 0;
}

int irc_register(IRCConnection *conn, const char *nick, const char *user) {
    if (!conn || conn->sockfd < 0 || !nick || !user) return -1;
    conn->nick = strdup(nick);
    conn->user = strdup(user);
    char buf[512];
    snprintf(buf, sizeof(buf), "NICK %s", nick);
    if (irc_send_raw(conn, buf) < 0) return -1;
    snprintf(buf, sizeof(buf), "USER %s 0 * :%s", user, user);
    if (irc_send_raw(conn, buf) < 0) return -1;
    return 0;
}

int irc_send_raw(IRCConnection *conn, const char *line) {
    if (!conn || conn->sockfd < 0 || !line) return -1;
    size_t len = strlen(line);
    char *out = (char*)malloc(len + 3);
    if (!out) return -1;
    memcpy(out, line, len);
    out[len] = '\r';
    out[len+1] = '\n';
    out[len+2] = '\0';
    ssize_t sent = send(conn->sockfd, out, (ssize_t)(len + 2), 0);
    free(out);
    if (sent < 0) {
        perror("send");
        return -1;
    }
    return 0;
}

int irc_join(IRCConnection *conn, const char *channel) {
    if (!conn || !channel) return -1;
    char buf[512];
    snprintf(buf, sizeof(buf), "JOIN %s", channel);
    return irc_send_raw(conn, buf);
}

int irc_privmsg(IRCConnection *conn, const char *target, const char *message) {
    if (!conn || !target || !message) return -1;
    char buf[512];
    snprintf(buf, sizeof(buf), "PRIVMSG %s :%s", target, message);
    return irc_send_raw(conn, buf);
}

static void handle_line(IRCConnection *conn, const char *line) {
    if (strncmp(line, "PING :", 6) == 0) {
        const char *token = line + 6;
        char pong[512];
        snprintf(pong, sizeof(pong), "PONG :%s", token);
        irc_send_raw(conn, pong);
        fprintf(stderr, "-> %s\n", pong);
    } else {
        fprintf(stderr, "%s\n", line);
        fflush(stderr);
    }
}

ssize_t irc_read_once(IRCConnection *conn, char *buf, size_t buflen) {
    if (!conn || conn->sockfd < 0 || !buf) return -1;
    ssize_t n = recv(conn->sockfd, buf, (ssize_t)(buflen - 1), 0);
    if (n <= 0) return n;
    buf[n] = '\0';
    char *start = buf;
    char *p;
    while ((p = strstr(start, "\r\n")) != NULL) {
        *p = '\0';
        handle_line(conn, start);
        start = p + 2;
    }
    if (*start) handle_line(conn, start);
    return n;
}

void irc_disconnect(IRCConnection *conn) {
    if (!conn) return;
    if (conn->sockfd >= 0) {
        close(conn->sockfd);
        conn->sockfd = -1;
    }
    free(conn->server); conn->server = NULL;
    free(conn->port); conn->port = NULL;
    free(conn->nick); conn->nick = NULL;
    free(conn->user); conn->user = NULL;
}

} // extern "C"

/* ---------------------------
   Qt GUI with improved user-list merging and modes
   --------------------------- */

#include <QApplication>
#include <QMainWindow>
#include <QSocketNotifier>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QMap>
#include <QMessageBox>
#include <QDateTime>
#include <QSplitter>
#include <QListWidget>
#include <QDebug>

class ChatTab : public QWidget {
    Q_OBJECT
public:
    explicit ChatTab(const QString &target, QWidget *parent = nullptr)
        : QWidget(parent), targetName(target)
    {
        QHBoxLayout *h = new QHBoxLayout(this);

        QVBoxLayout *left = new QVBoxLayout;
        view = new QTextEdit(this);
        view->setReadOnly(true);
        input = new QLineEdit(this);
        left->addWidget(view);
        left->addWidget(input);

        userList = new QListWidget(this);
        userList->setMaximumWidth(220);

        h->addLayout(left);
        h->addWidget(userList);
        setLayout(h);
    }

    void appendLine(const QString &line) {
        QString ts = QDateTime::currentDateTime().toString("hh:mm:ss");
        view->append(QString("[%1] %2").arg(ts, line));
    }

    // Set user list from a map nick -> modePrefix ("" or "@" or "+", etc.)
    void setUserListWithModes(const QMap<QString, QString> &usersWithModes) {
        userList->clear();
        // Build a list of display strings with mode prefix
        QStringList display;
        for (auto it = usersWithModes.constBegin(); it != usersWithModes.constEnd(); ++it) {
            QString displayName = it.value() + it.key(); // e.g., "@" + "alice" => "@alice"
            display.append(displayName);
        }
        display.sort(Qt::CaseInsensitive);
        for (const QString &d : display) {
            QListWidgetItem *it = new QListWidgetItem(d, userList);
            // Optionally style operators/voices
            if (d.startsWith('@')) {
                it->setForeground(QBrush(QColor("#d14"))); // red-ish for ops
                it->setFont(QFont("Monospace", 10, QFont::Bold));
            } else if (d.startsWith('+')) {
                it->setForeground(QBrush(QColor("#1a7"))); // green-ish for voice
            }
        }
    }

    // Add or update a single user with mode
    void addOrUpdateUser(const QString &nick, const QString &mode) {
        // find existing item
        QList<QListWidgetItem*> items = userList->findItems(nick, Qt::MatchContains);
        // We search by exact nick ignoring mode prefix; simpler to rebuild map externally.
        // For convenience, if exact match with prefix exists, remove it and re-add.
        for (QListWidgetItem *it : userList->findItems(QString(), Qt::MatchContains)) {
            QString text = it->text();
            QString plain = text;
            // strip leading mode chars
            while (!plain.isEmpty() && (plain[0] == '@' || plain[0] == '+' || plain[0] == '%' || plain[0] == '&' || plain[0] == '~')) {
                plain = plain.mid(1);
            }
            if (plain == nick) {
                delete userList->takeItem(userList->row(it));
                break;
            }
        }
        QString display = mode + nick;
        QListWidgetItem *it = new QListWidgetItem(display, userList);
        if (display.startsWith('@')) {
            it->setForeground(QBrush(QColor("#d14")));
            it->setFont(QFont("Monospace", 10, QFont::Bold));
        } else if (display.startsWith('+')) {
            it->setForeground(QBrush(QColor("#1a7")));
        }
    }

    // Remove a user by nick
    void removeUser(const QString &nick) {
        QList<QListWidgetItem*> items = userList->findItems(QString(), Qt::MatchContains);
        for (QListWidgetItem *it : items) {
            QString text = it->text();
            QString plain = text;
            while (!plain.isEmpty() && (plain[0] == '@' || plain[0] == '+' || plain[0] == '%' || plain[0] == '&' || plain[0] == '~')) {
                plain = plain.mid(1);
            }
            if (plain == nick) {
                delete userList->takeItem(userList->row(it));
                break;
            }
        }
    }

    QString target() const { return targetName; }
    QLineEdit *inputField() const { return input; }

private:
    QTextEdit *view;
    QLineEdit *input;
    QListWidget *userList;
    QString targetName;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent), connected(false), notifier(nullptr)
    {
        QWidget *central = new QWidget(this);
        setCentralWidget(central);

        serverEdit = new QLineEdit("irc.libera.chat", this);
        portEdit = new QLineEdit("6667", this);
        nickEdit = new QLineEdit("QtBot", this);
        channelEdit = new QLineEdit("#testchannel", this);

        connectBtn = new QPushButton("Connect", this);
        joinBtn = new QPushButton("Join", this);

        connect(connectBtn, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
        connect(joinBtn, &QPushButton::clicked, this, &MainWindow::onJoinClicked);

        QHBoxLayout *topRow = new QHBoxLayout;
        topRow->addWidget(new QLabel("Server:"));
        topRow->addWidget(serverEdit);
        topRow->addWidget(new QLabel("Port:"));
        topRow->addWidget(portEdit);
        topRow->addWidget(connectBtn);

        QHBoxLayout *midRow = new QHBoxLayout;
        midRow->addWidget(new QLabel("Nick:"));
        midRow->addWidget(nickEdit);
        midRow->addWidget(new QLabel("Channel:"));
        midRow->addWidget(channelEdit);
        midRow->addWidget(joinBtn);

        tabs = new QTabWidget(this);
        tabs->setTabsClosable(true);
        connect(tabs, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);

        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addLayout(topRow);
        mainLayout->addLayout(midRow);
        mainLayout->addWidget(tabs);

        central->setLayout(mainLayout);
        setWindowTitle("Qt IRC Client - Users with Modes");
        resize(900, 700);

        memset(&conn, 0, sizeof(conn));
        conn.sockfd = -1;
    }

    ~MainWindow() override {
        cleanupConnection();
    }

private slots:
    void onConnectClicked() {
        if (connected) {
            cleanupConnection();
            appendSystem("Disconnected");
            connectBtn->setText("Connect");
            return;
        }
        QString server = serverEdit->text().trimmed();
        QString port = portEdit->text().trimmed();
        QString nick = nickEdit->text().trimmed();
        if (server.isEmpty() || port.isEmpty() || nick.isEmpty()) {
            QMessageBox::warning(this, "Input", "Server, port and nick are required.");
            return;
        }
        if (irc_connect(&conn, server.toLocal8Bit().constData(), port.toLocal8Bit().constData()) != 0) {
            appendSystem("Failed to connect");
            return;
        }
        if (irc_register(&conn, nick.toLocal8Bit().constData(), nick.toLocal8Bit().constData()) != 0) {
            appendSystem("Failed to register nick");
            irc_disconnect(&conn);
            return;
        }
        notifier = new QSocketNotifier(conn.sockfd, QSocketNotifier::Read, this);
        connect(notifier, &QSocketNotifier::activated, this, &MainWindow::onSocketActivated);
        connected = true;
        connectBtn->setText("Disconnect");
        appendSystem(QString("Connected to %1:%2").arg(server, port));
        createOrSelectTab("status");
    }

    void onJoinClicked() {
        if (!connected) {
            QMessageBox::information(this, "Not connected", "Connect first.");
            return;
        }
        QString channel = channelEdit->text().trimmed();
        if (channel.isEmpty()) {
            QMessageBox::warning(this, "Input", "Channel required.");
            return;
        }
        // Do not create tab here; wait for server JOIN/NAMES replies
        if (irc_join(&conn, channel.toLocal8Bit().constData()) == 0) {
            appendSystem(QString("Requested JOIN %1").arg(channel));
        } else {
            appendSystem("JOIN failed");
        }
    }

    void onSocketActivated(int fd) {
        Q_UNUSED(fd);
        if (!connected) return;
        char buf[8192];
        ssize_t n = irc_read_once(&conn, buf, sizeof(buf));
        if (n <= 0) {
            appendSystem("Connection closed by server");
            cleanupConnection();
            connectBtn->setText("Connect");
            return;
        }
        QString s = QString::fromLocal8Bit(buf, (int)n);
        QStringList lines = s.split("\r\n", Qt::SkipEmptyParts);
        for (const QString &line : lines) dispatchIrcLine(line);
    }

    void onTabCloseRequested(int index) {
        QWidget *w = tabs->widget(index);
        if (!w) return;
        ChatTab *ct = qobject_cast<ChatTab*>(w);
        if (ct) {
            QString t = ct->target();
            targetToIndex.remove(t);
            pendingNames.remove(t);
            channelUserModes.remove(t);
        }
        tabs->removeTab(index);
        delete w;
    }

    void onTabInputReturnPressed() {
        QLineEdit *input = qobject_cast<QLineEdit*>(sender());
        if (!input) return;
        ChatTab *ct = qobject_cast<ChatTab*>(input->parent());
        if (!ct) return;
        QString text = input->text();
        if (text.isEmpty()) return;
        QString target = ct->target();
        if (irc_privmsg(&conn, target.toLocal8Bit().constData(), text.toLocal8Bit().constData()) == 0) {
            ct->appendLine(QString("<%1> %2").arg(QString::fromLocal8Bit(conn.nick ? conn.nick : "me"), text));
            input->clear();
        } else {
            ct->appendLine("Failed to send message");
        }
    }

private:
    QLineEdit *serverEdit;
    QLineEdit *portEdit;
    QLineEdit *nickEdit;
    QLineEdit *channelEdit;
    QPushButton *connectBtn;
    QPushButton *joinBtn;
    QTabWidget *tabs;

    IRCConnection conn;
    bool connected;
    QSocketNotifier *notifier;

    QMap<QString,int> targetToIndex; // target -> tab index

    // Accumulate NAMES chunks per channel: channel -> map(nick -> modePrefix)
    QMap<QString, QMap<QString, QString>> pendingNames;

    // Current channel user modes (applied): channel -> map(nick -> modePrefix)
    QMap<QString, QMap<QString, QString>> channelUserModes;

    void appendSystem(const QString &msg) {
        ChatTab *ct = createOrSelectTab("status");
        ct->appendLine(QString("[SYSTEM] %1").arg(msg));
    }

    ChatTab* createOrSelectTab(const QString &target) {
        QString key = target;
        if (key.isEmpty()) key = "status";
        if (targetToIndex.contains(key)) {
            int idx = targetToIndex.value(key);
            tabs->setCurrentIndex(idx);
            return qobject_cast<ChatTab*>(tabs->widget(idx));
        }
        ChatTab *ct = new ChatTab(key, this);
        connect(ct->inputField(), &QLineEdit::returnPressed, this, &MainWindow::onTabInputReturnPressed);
        int idx = tabs->addTab(ct, key);
        tabs->setCurrentIndex(idx);
        targetToIndex.insert(key, idx);
        return ct;
    }

    QString prefixNick(const QString &prefix) {
        if (!prefix.startsWith(':')) return QString();
        QString p = prefix.mid(1);
        int excl = p.indexOf('!');
        if (excl >= 0) return p.left(excl);
        int at = p.indexOf('@');
        if (at >= 0) return p.left(at);
        return p;
    }

    // Clean a single token from NAMES and return pair (modePrefix, nick)
    QPair<QString, QString> cleanNameToken(const QString &token) {
        QString t = token.trimmed();
        QString mode;
        // collect leading mode chars (only first is usually meaningful)
        while (!t.isEmpty() && (t[0] == '@' || t[0] == '+' || t[0] == '%' || t[0] == '&' || t[0] == '~')) {
            mode += t[0];
            t = t.mid(1);
        }
        // strip leading colon if present
        if (t.startsWith(':')) t = t.mid(1);
        t = t.trimmed();
        // sanity checks: ignore fragments that look like "End", "of", "/NAMES", "list."
        QString up = t.toUpper();
        if (t.isEmpty() || up == "END" || up == "OF" || up == "NAMES" || up == "LIST." || t.contains('/')) {
            return qMakePair(QString(), QString());
        }
        return qMakePair(mode, t);
    }

    void dispatchIrcLine(const QString &line) {
        QString raw = line;
        if (raw.startsWith("PING :")) {
            appendSystem(raw);
            return;
        }

        QString prefix;
        QString rest = raw;
        if (raw.startsWith(':')) {
            int sp = raw.indexOf(' ');
            if (sp > 0) {
                prefix = raw.left(sp);
                rest = raw.mid(sp + 1);
            }
        }

        QString command;
        QString params;
        int sp2 = rest.indexOf(' ');
        if (sp2 >= 0) {
            command = rest.left(sp2);
            params = rest.mid(sp2 + 1);
        } else {
            command = rest;
            params = "";
        }

        bool isNumeric = false;
        int numeric = 0;
        if (!command.isEmpty() && command[0].isDigit()) {
            isNumeric = true;
            numeric = command.toInt();
        }

        // RPL_NAMREPLY (353) - accumulate tokens into pendingNames[channel]
        if (isNumeric && numeric == 353) {
            int trailingPos = params.indexOf(" :");
            QString namesPart;
            QString before;
            if (trailingPos >= 0) {
                before = params.left(trailingPos);
                namesPart = params.mid(trailingPos + 2);
            } else {
                int col = params.indexOf(':');
                if (col >= 0) {
                    before = params.left(col);
                    namesPart = params.mid(col + 1);
                } else {
                    appendSystem(QString("Ignored malformed 353: %1").arg(params));
                    return;
                }
            }
            QStringList beforeParts = before.split(' ', Qt::SkipEmptyParts);
            QString channel;
            if (!beforeParts.isEmpty()) channel = beforeParts.last();
            if (channel.isEmpty()) channel = "#unknown";

            QStringList tokens = namesPart.split(' ', Qt::SkipEmptyParts);
            for (QString tok : tokens) {
                QPair<QString, QString> p = cleanNameToken(tok);
                if (p.second.isEmpty()) continue;
                QString mode = p.first;
                QString nick = p.second;
                // If multiple mode chars present, keep only the first (server semantics)
                QString modePrefix = mode.isEmpty() ? QString() : QString(mode[0]);
                // store into pendingNames[channel][nick] = modePrefix (do not overwrite stronger mode with weaker)
                QMap<QString, QString> &mapRef = pendingNames[channel];
                if (!mapRef.contains(nick)) {
                    mapRef.insert(nick, modePrefix);
                } else {
                    // if existing has no mode but new has, update
                    if (mapRef.value(nick).isEmpty() && !modePrefix.isEmpty()) {
                        mapRef[nick] = modePrefix;
                    }
                }
            }
            // debug
            ChatTab *status = createOrSelectTab("status");
            status->appendLine(QString("*** Received NAMES chunk for %1").arg(channel));
            return;
        }

        // RPL_ENDOFNAMES (366) - finalize pendingNames[channel] into channelUserModes and update tab
        if (isNumeric && numeric == 366) {
            int trailingPos = params.indexOf(" :");
            QString before = (trailingPos >= 0) ? params.left(trailingPos) : params;
            QStringList parts = before.split(' ', Qt::SkipEmptyParts);
            QString channel;
            if (parts.size() >= 2) channel = parts[1];
            if (channel.isEmpty()) channel = "#unknown";

            QMap<QString, QString> namesMap = pendingNames.value(channel);
            pendingNames.remove(channel);

            // Merge into channelUserModes: replace or merge
            QMap<QString, QString> &applied = channelUserModes[channel];
            // Replace with namesMap (server authoritative)
            applied = namesMap;

            // Update tab UI
            ChatTab *ct = createOrSelectTab(channel);
            ct->setUserListWithModes(applied);

            ct->appendLine(QString("*** NAMES for %1: %2").arg(channel, QStringList(applied.keys()).join(' ')));
            ct->appendLine(QString("*** End of NAMES for %1").arg(channel));
            return;
        }

        // PRIVMSG handling
        if (command == "PRIVMSG") {
            int colon = params.indexOf(" :");
            QString target = (colon >= 0) ? params.left(colon) : params;
            QString message = (colon >= 0) ? params.mid(colon + 2) : "";
            QString sender = prefixNick(prefix);
            QString ourNick = QString::fromLocal8Bit(conn.nick ? conn.nick : "");
            if (target == ourNick) {
                ChatTab *ct = createOrSelectTab(sender);
                ct->appendLine(QString("<%1> %2").arg(sender, message));
                // ensure sender present in private tab user list (no mode)
                channelUserModes[sender].insert(sender, QString());
                ct->setUserListWithModes(channelUserModes[sender]);
            } else {
                ChatTab *ct = createOrSelectTab(target);
                ct->appendLine(QString("<%1> %2").arg(sender, message));
                // ensure sender present in channel user map (if missing, add without mode)
                if (!channelUserModes[target].contains(sender)) {
                    channelUserModes[target].insert(sender, QString());
                    ct->setUserListWithModes(channelUserModes[target]);
                }
            }
            return;
        }

        // JOIN handling
        if (command == "JOIN") {
            QString channel = params;
            if (channel.startsWith(':')) channel = channel.mid(1);
            QString who = prefixNick(prefix);
            ChatTab *ct = createOrSelectTab(channel);
            ct->appendLine(QString("*** %1 has joined %2").arg(who, channel));
            // If someone joined, add to channelUserModes with no mode (server will later send NAMES or MODE)
            channelUserModes[channel].insert(who, QString());
            ct->setUserListWithModes(channelUserModes[channel]);
            // If we joined ourselves, request NAMES to ensure we get full list
            QString ourNick = QString::fromLocal8Bit(conn.nick ? conn.nick : "");
            if (who == ourNick) {
                irc_send_raw(&conn, QString("NAMES %1").arg(channel).toLocal8Bit().constData());
            }
            return;
        }

        // PART handling
        if (command == "PART") {
            int colon = params.indexOf(" :");
            QString channel = (colon >= 0) ? params.left(colon) : params;
            QString who = prefixNick(prefix);
            ChatTab *ct = createOrSelectTab(channel);
            ct->appendLine(QString("*** %1 has left %2").arg(who, channel));
            // remove from channelUserModes and update UI
            if (channelUserModes.contains(channel)) {
                channelUserModes[channel].remove(who);
                ct->setUserListWithModes(channelUserModes[channel]);
            }
            return;
        }

        // MODE changes (simple handling: update mode prefix for a nick in a channel)
        if (command == "MODE") {
            // params example: "#chan +o nick" or "nick +o target"
            QStringList parts = params.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 3) {
                QString target = parts[0];
                QString modeStr = parts[1];
                QString who = parts[2];
                // Only handle simple +o/-o and +v/-v for channel user modes
                if (target.startsWith('#') && (modeStr.contains('o') || modeStr.contains('v'))) {
                    QString modePrefix;
                    if (modeStr.startsWith('+')) {
                        if (modeStr.contains('o')) modePrefix = "@";
                        else if (modeStr.contains('v')) modePrefix = "+";
                    } else if (modeStr.startsWith('-')) {
                        // remove mode
                        modePrefix = QString();
                    }
                    if (channelUserModes.contains(target)) {
                        if (modePrefix.isEmpty()) channelUserModes[target].remove(who);
                        else channelUserModes[target].insert(who, modePrefix);
                        ChatTab *ct = createOrSelectTab(target);
                        ct->setUserListWithModes(channelUserModes[target]);
                        ct->appendLine(QString("*** MODE %1 %2 %3").arg(target, modeStr, who));
                    }
                }
            }
            return;
        }

        // default: show in status
        ChatTab *status = createOrSelectTab("status");
        status->appendLine(raw);
    }

    void cleanupConnection() {
        if (notifier) {
            delete notifier;
            notifier = nullptr;
        }
        if (connected) {
            irc_send_raw(&conn, "QUIT :Qt client exiting");
        }
        irc_disconnect(&conn);
        connected = false;
        targetToIndex.clear();
        pendingNames.clear();
        channelUserModes.clear();
        while (tabs->count() > 0) {
            QWidget *w = tabs->widget(0);
            tabs->removeTab(0);
            delete w;
        }
    }
};

#include "qirc1.moc" // placeholder for moc when using qmake

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

