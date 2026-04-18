// qirc.cpp
// Single-file Qt6 IRC client (mIRC-like UI) with robust channel user lists, NAMES parsing,
// slash commands, tab normalization, and other fixes.
//
// Build: requires Qt6 Widgets and Network modules.

#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QListWidget>
#include <QSystemTrayIcon>
#include <QTcpSocket>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDateTime>
#include <QFont>
#include <QRegularExpression>
#include <QTimer>
#include <QMap>
#include <QSet>
#include <QVariant>

//
// IrcConnection: minimal IRC protocol handling (connect, send raw, parsing)
// - tolerant NAMES parsing
// - emits signals for PRIVMSG, NAMES (353), NAMES_END (366), JOIN, PART, NICK
//
class IrcConnection : public QObject
{
    Q_OBJECT
public:
    explicit IrcConnection(QObject *parent = nullptr)
        : QObject(parent)
    {
        connect(&m_socket, &QTcpSocket::readyRead, this, &IrcConnection::onReadyRead);
        connect(&m_socket, &QTcpSocket::connected, this, &IrcConnection::onConnected);
        connect(&m_socket, &QTcpSocket::disconnected, this, &IrcConnection::onDisconnected);
        connect(&m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
                this, &IrcConnection::onErrorOccurred);
    }

    void connectToServer(const QString &host, quint16 port) { m_socket.connectToHost(host, port); }
    void sendRaw(const QString &line)
    {
        if (m_socket.state() == QAbstractSocket::ConnectedState) {
            QByteArray data = line.toUtf8();
            if (!data.endsWith("\r\n")) data += "\r\n";
            m_socket.write(data);
        }
    }
    void setNick(const QString &nick) { m_nick = nick; }
    QString nick() const { return m_nick; }

signals:
    void rawLineReceived(const QString &line);
    void privmsgReceived(const QString &from, const QString &target, const QString &message);
    void connected();
    void disconnected();
    void errorOccurred(const QString &err);

    // Names and userlist signals
    void namesReceived(const QString &channel, const QStringList &names);
    void namesEnd(const QString &channel); // 366
    void joinReceived(const QString &channel, const QString &nick);
    void partReceived(const QString &channel, const QString &nick);
    void nickChanged(const QString &oldNick, const QString &newNick);

private slots:
    void onReadyRead()
    {
        m_buffer += QString::fromUtf8(m_socket.readAll());
        while (m_buffer.contains("\r\n")) {
            int idx = m_buffer.indexOf("\r\n");
            QString line = m_buffer.left(idx);
            m_buffer = m_buffer.mid(idx + 2);
            emit rawLineReceived(line);

            // PING -> PONG
            if (line.startsWith("PING ")) {
                // server may send "PING :server" or "PING server"
                QString server = line.section(' ', 1).trimmed();
                if (server.startsWith(":")) server = server.mid(1);
                sendRaw(QString("PONG %1").arg(server));
            }

            // PRIVMSG parsing: :nick!user@host PRIVMSG target :message
            QRegularExpression rePriv(R"(^:([^!]+)![^ ]+ PRIVMSG ([^ ]+) :(.+)$)");
            auto mPriv = rePriv.match(line);
            if (mPriv.hasMatch()) {
                QString from = mPriv.captured(1);
                QString target = mPriv.captured(2);
                QString message = mPriv.captured(3);
                emit privmsgReceived(from, target, message);
                continue;
            }

            // JOIN: :nick!user@host JOIN :#channel  OR :nick!user@host JOIN #channel
            QRegularExpression reJoin(R"(^:([^!]+)![^ ]+ JOIN :?([^ ]+)$)");
            auto mJoin = reJoin.match(line);
            if (mJoin.hasMatch()) {
                QString nick = mJoin.captured(1);
                QString channel = mJoin.captured(2);
                emit joinReceived(channel, nick);
                continue;
            }

            // PART: :nick!user@host PART #channel :reason
            QRegularExpression rePart(R"(^:([^!]+)![^ ]+ PART ([^ ]+))");
            auto mPart = rePart.match(line);
            if (mPart.hasMatch()) {
                QString nick = mPart.captured(1);
                QString channel = mPart.captured(2);
                emit partReceived(channel, nick);
                continue;
            }

            // NICK change: :oldNick!user@host NICK :newNick
            QRegularExpression reNick(R"(^:([^!]+)![^ ]+ NICK :?(.+)$)");
            auto mNick = reNick.match(line);
            if (mNick.hasMatch()) {
                QString oldNick = mNick.captured(1);
                QString newNick = mNick.captured(2);
                emit nickChanged(oldNick, newNick);
                continue;
            }

            // Numeric replies: handle 353 (NAMES) and 366 (end of names)
            // Use tolerant parsing: split by spaces, find numeric token "353" or "366"
            QStringList parts = line.split(' ');
            if (parts.size() >= 2) {
                // find numeric token
                for (int i = 0; i < parts.size(); ++i) {
                    if (parts[i] == "353") {
                        // channel is usually at parts[i+3] or parts[i+4] depending on server
                        // find the ':' that starts the names list (the trailing parameter)
                        int colonIndex = line.indexOf(" :");
                        if (colonIndex >= 0) {
                            QString namesStr = line.mid(colonIndex + 2).trimmed();
                            // channel: try to extract token before the colon (search backwards)
                            // Example: ":server 353 mynick = #chan :nick1 nick2"
                            // We'll search for the token that starts with '#' or '&' near the numeric token
                            QString channel;
                            for (int j = i+1; j < parts.size(); ++j) {
                                if (parts[j].startsWith("#") || parts[j].startsWith("&")) {
                                    channel = parts[j];
                                    break;
                                }
                            }
                            if (channel.isEmpty()) {
                                // fallback: try parts[i+3] if exists
                                if (parts.size() > i+3) channel = parts[i+3];
                            }
                            if (!channel.isEmpty()) {
                                QStringList raw = namesStr.split(' ', Qt::SkipEmptyParts);
                                QStringList cleaned;
                                for (const QString &n : raw) {
                                    QString c = n;
                                    while (!c.isEmpty() && (c[0] == '@' || c[0] == '+' || c[0] == '%' || c[0] == '&' || c[0] == '~'))
                                        c.remove(0,1);
                                    if (!c.isEmpty()) cleaned << c;
                                }
                                if (!cleaned.isEmpty()) emit namesReceived(channel, cleaned);
                            }
                        }
                        break;
                    } else if (parts[i] == "366") {
                        // end of names for a channel; channel token usually at parts[i+2] or i+3
                        QString channel;
                        for (int j = i+1; j < parts.size(); ++j) {
                            if (parts[j].startsWith("#") || parts[j].startsWith("&")) {
                                channel = parts[j];
                                break;
                            }
                        }
                        if (channel.isEmpty() && parts.size() > i+2) channel = parts[i+2];
                        if (!channel.isEmpty()) emit namesEnd(channel);
                        break;
                    }
                }
            }
        }
    }
    void onConnected() { emit connected(); }
    void onDisconnected() { emit disconnected(); }
    void onErrorOccurred(QAbstractSocket::SocketError)
    {
        emit errorOccurred(m_socket.errorString());
    }

private:
    QTcpSocket m_socket;
    QString m_buffer;
    QString m_nick;
};

//
// MainWindow: UI and glue logic
//
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
    {
        setWindowTitle("qIRC (Qt6) - Patched Single-file");
        resize(920, 620);

        QWidget *central = new QWidget(this);
        setCentralWidget(central);
        QVBoxLayout *mainLayout = new QVBoxLayout(central);

        // Top controls
        QHBoxLayout *top = new QHBoxLayout();
        m_serverHost = new QLineEdit("irc.libera.chat");
        m_serverPort = new QLineEdit("6667");
        m_nickEdit = new QLineEdit("QtUser");
        QPushButton *connectBtn = new QPushButton("Connect");
        connect(connectBtn, &QPushButton::clicked, this, &MainWindow::connectToServer);

        m_channelEdit = new QLineEdit("#test");
        QPushButton *joinBtn = new QPushButton("Join");
        connect(joinBtn, &QPushButton::clicked, this, &MainWindow::joinChannel);

        top->addWidget(new QLabel("Host:"));
        top->addWidget(m_serverHost);
        top->addWidget(new QLabel("Port:"));
        top->addWidget(m_serverPort);
        top->addWidget(new QLabel("Nick:"));
        top->addWidget(m_nickEdit);
        top->addWidget(connectBtn);
        top->addSpacing(20);
        top->addWidget(new QLabel("Channel:"));
        top->addWidget(m_channelEdit);
        top->addWidget(joinBtn);
        mainLayout->addLayout(top);

        // Tabs
        m_tabs = new QTabWidget();
        m_tabs->setTabsClosable(true);
        connect(m_tabs, &QTabWidget::tabCloseRequested, [this](int index){
            QWidget *w = m_tabs->widget(index);
            m_tabs->removeTab(index);
            w->deleteLater();
        });

        // Server tab
        QWidget *serverTab = createServerTab();
        int serverIndex = m_tabs->addTab(serverTab, "Server");
        m_tabs->setTabData(serverIndex, QString("server")); // canonical id
        mainLayout->addWidget(m_tabs);

        // Input area
        QHBoxLayout *inputLayout = new QHBoxLayout();
        m_input = new QLineEdit();
        QPushButton *sendBtn = new QPushButton("Send");
        connect(sendBtn, &QPushButton::clicked, this, &MainWindow::sendMessage);
        connect(m_input, &QLineEdit::returnPressed, this, &MainWindow::sendMessage);
        inputLayout->addWidget(m_input);
        inputLayout->addWidget(sendBtn);
        mainLayout->addLayout(inputLayout);

        // Tray icon for notifications
        m_tray = new QSystemTrayIcon(this);
        m_tray->setIcon(windowIcon());
        m_tray->show();
        connect(m_tray, &QSystemTrayIcon::activated, this, &MainWindow::onTrayActivated);

        // Connect IrcConnection signals
        connect(&m_irc, &IrcConnection::rawLineReceived, this, &MainWindow::onRawLine);
        connect(&m_irc, &IrcConnection::privmsgReceived, this, &MainWindow::onPrivmsg);
        connect(&m_irc, &IrcConnection::connected, this, &MainWindow::onConnected);
        connect(&m_irc, &IrcConnection::disconnected, this, &MainWindow::onDisconnected);
        connect(&m_irc, &IrcConnection::errorOccurred, this, [this](const QString &err){
            m_serverLog->append(QString("[Error] %1").arg(err));
        });

        // User list signals
        connect(&m_irc, &IrcConnection::namesReceived, this, &MainWindow::onNamesReceived);
        connect(&m_irc, &IrcConnection::namesEnd, this, &MainWindow::onNamesEnd);
        connect(&m_irc, &IrcConnection::joinReceived, this, &MainWindow::onJoinReceived);
        connect(&m_irc, &IrcConnection::partReceived, this, &MainWindow::onPartReceived);
        connect(&m_irc, &IrcConnection::nickChanged, this, &MainWindow::onNickChanged);

        // Styling: monospaced font for mIRC-like feel
        QFont mono;
        mono.setFamily("Courier New");
        mono.setPointSize(10);
        setFont(mono);
    }

    ~MainWindow() override = default;

private slots:
    void connectToServer()
    {
        QString host = m_serverHost->text().trimmed();
        quint16 port = m_serverPort->text().toUShort();
        m_currentNick = m_nickEdit->text().trimmed();
        if (m_currentNick.isEmpty()) {
            m_serverLog->append("[Error] Nick cannot be empty.");
            return;
        }

        m_serverLog->append(QString("[%1] Connecting to %2:%3 ...")
                            .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                            .arg(host).arg(port));
        m_irc.setNick(m_currentNick);
        m_irc.connectToServer(host, port);
    }

    void onConnected()
    {
        m_serverLog->append("[Connected]");
        m_irc.sendRaw(QString("NICK %1").arg(m_currentNick));
        m_irc.sendRaw(QString("USER %1 0 * :%2").arg(m_currentNick).arg("qIRC Qt6 Client"));
    }

    void onDisconnected()
    {
        m_serverLog->append("[Disconnected]");
    }

    void onRawLine(const QString &line)
    {
        // Show raw server lines for debugging
        m_serverLog->append(line);
    }

    void onPrivmsg(const QString &from, const QString &target, const QString &message)
    {
        bool isPrivate = (target.compare(m_currentNick, Qt::CaseInsensitive) == 0);

        if (isPrivate) {
            ensurePrivateTab(from);
            QWidget *tab = findTabById(from);
            if (!tab) return;
            QTextEdit *view = tab->findChild<QTextEdit*>("view");
            if (view) {
                QString ts = QDateTime::currentDateTime().toString("hh:mm");
                view->append(QString("<b>%1</b> [%2]: %3").arg(from).arg(ts).arg(message.toHtmlEscaped()));
            }
            notifyUser(QString("Private from %1").arg(from), message);
            markTabUnread(tab);
        } else {
            QString chan = target;
            QWidget *tab = findTabById(chan);
            if (!tab) {
                // If channel tab missing, create it and request names
                QWidget *newTab = createChannelTab(chan);
                int idx = m_tabs->addTab(newTab, chan);
                m_tabs->setTabData(idx, chan);
                m_irc.sendRaw(QString("NAMES %1").arg(chan));
                tab = newTab;
            }
            QTextEdit *view = tab->findChild<QTextEdit*>("view");
            if (!view) return;
            QString ts = QDateTime::currentDateTime().toString("hh:mm");
            bool mention = message.contains(QRegularExpression("\\b" + QRegularExpression::escape(m_currentNick) + "\\b", QRegularExpression::CaseInsensitiveOption));
            QString formatted = QString("<span style='color:#7fbf7f'><b>%1</b></span> [%2]: %3")
                    .arg(from).arg(ts).arg(message.toHtmlEscaped());
            if (mention) {
                formatted = "<span style='background-color: #333333; color: #ffd700;'>" + formatted + "</span>";
                notifyUser(QString("Mention in %1").arg(chan), QString("%1: %2").arg(from).arg(message));
                markTabUnread(tab);
            }
            view->append(formatted);
        }
    }

    // Process slash commands like /join, /part, /nick, /msg, /quit
    void processCommand(const QString &line)
    {
        if (m_currentNick.isEmpty()) {
            m_serverLog->append("[Error] Not connected. Connect first.");
            return;
        }

        QString cmdLine = line.mid(1).trimmed(); // remove leading '/'
        QString cmd = cmdLine.section(' ', 0, 0).toLower();
        QString args = cmdLine.section(' ', 1).trimmed();

        if (cmd == "join") {
            if (args.isEmpty()) {
                m_serverLog->append("[Usage] /join #channel");
                return;
            }
            QString chan = args;
            if (!chan.startsWith("#") && !chan.startsWith("&")) chan = "#" + chan;
            m_irc.sendRaw(QString("JOIN %1").arg(chan));
            // create tab immediately and request names
            QWidget *tab = createChannelTab(chan);
            int idx = m_tabs->addTab(tab, chan);
            m_tabs->setTabData(idx, chan);
            m_irc.sendRaw(QString("NAMES %1").arg(chan));
            return;
        }

        if (cmd == "part") {
            QString chan = args;
            if (chan.isEmpty()) {
                int idx = m_tabs->currentIndex();
                if (idx >= 0) chan = tabCanonicalId(idx);
            }
            if (chan.isEmpty()) {
                m_serverLog->append("[Usage] /part #channel");
                return;
            }
            m_irc.sendRaw(QString("PART %1").arg(chan));
            QWidget *tab = findTabById(chan);
            if (tab) {
                int idx = m_tabs->indexOf(tab);
                if (idx >= 0) { m_tabs->removeTab(idx); tab->deleteLater(); }
            }
            return;
        }

        if (cmd == "nick") {
            if (args.isEmpty()) {
                m_serverLog->append("[Usage] /nick newnick");
                return;
            }
            m_irc.sendRaw(QString("NICK %1").arg(args));
            m_currentNick = args;
            return;
        }

        if (cmd == "msg") {
            QString target = args.section(' ', 0, 0);
            QString msg = args.section(' ', 1).trimmed();
            if (target.isEmpty() || msg.isEmpty()) {
                m_serverLog->append("[Usage] /msg nick message");
                return;
            }
            m_irc.sendRaw(QString("PRIVMSG %1 :%2").arg(target).arg(msg));
            ensurePrivateTab(target);
            QWidget *tab = findTabById(target);
            if (tab) {
                QTextEdit *view = tab->findChild<QTextEdit*>("view");
                if (view) {
                    QString ts = QDateTime::currentDateTime().toString("hh:mm");
                    view->append(QString("<span style='color:#9fbfff'><b>%1</b></span> [%2]: %3")
                                 .arg(m_currentNick).arg(ts).arg(msg.toHtmlEscaped()));
                }
            }
            return;
        }

        if (cmd == "quit") {
            QString reason = args;
            if (reason.isEmpty()) reason = "qIRC client quitting";
            m_irc.sendRaw(QString("QUIT :%1").arg(reason));
            return;
        }

        // Unknown command: send raw (without leading '/')
        m_irc.sendRaw(line.mid(1));
    }

    void sendMessage()
    {
        QString text = m_input->text().trimmed();
        if (text.isEmpty()) return;

        if (text.startsWith('/')) {
            processCommand(text);
            m_input->clear();
            return;
        }

        int idx = m_tabs->currentIndex();
        if (idx < 0) return;
        QString target = tabCanonicalId(idx);

        if (target == "server") {
            m_serverLog->append("[Error] Cannot send PRIVMSG to Server tab.");
            m_input->clear();
            return;
        }

        m_irc.sendRaw(QString("PRIVMSG %1 :%2").arg(target).arg(text));

        QWidget *tab = m_tabs->widget(idx);
        if (tab) {
            QTextEdit *view = tab->findChild<QTextEdit*>("view");
            if (view) {
                QString ts = QDateTime::currentDateTime().toString("hh:mm");
                view->append(QString("<span style='color:#9fbfff'><b>%1</b></span> [%2]: %3")
                             .arg(m_currentNick).arg(ts).arg(text.toHtmlEscaped()));
            }
        }
        m_input->clear();
    }

    void joinChannel()
    {
        QString chan = m_channelEdit->text().trimmed();
        if (chan.isEmpty()) return;
        if (!chan.startsWith("#") && !chan.startsWith("&")) chan = "#" + chan;
        m_irc.sendRaw(QString("JOIN %1").arg(chan));
        QWidget *tab = createChannelTab(chan);
        int idx = m_tabs->addTab(tab, chan);
        m_tabs->setTabData(idx, chan);
        m_irc.sendRaw(QString("NAMES %1").arg(chan));
    }

    void onTrayActivated(QSystemTrayIcon::ActivationReason reason)
    {
        if (reason == QSystemTrayIcon::Trigger) {
            showNormal();
            activateWindow();
        }
    }

    // --- User list handlers ---

    // Normalize tab label: strip "* " unread marker and trim
    QString normalizeTabLabel(const QString &label) const
    {
        QString s = label;
        if (s.startsWith("* ")) s = s.mid(2);
        return s.trimmed();
    }

    // Return canonical id stored in tabData (channel or nick)
    QString tabCanonicalId(int tabIndex) const
    {
        QVariant d = m_tabs->tabData(tabIndex);
        if (d.isValid() && d.canConvert<QString>()) return d.toString();
        // fallback to normalized label
        return normalizeTabLabel(m_tabs->tabText(tabIndex));
    }

    // Find tab by canonical id (channel or nick)
    QWidget* findTabById(const QString &id)
    {
        QString idNorm = id.trimmed();
        for (int i = 0; i < m_tabs->count(); ++i) {
            QVariant d = m_tabs->tabData(i);
            if (d.isValid() && d.canConvert<QString>()) {
                if (QString::compare(d.toString(), idNorm, Qt::CaseInsensitive) == 0)
                    return m_tabs->widget(i);
            } else {
                if (QString::compare(normalizeTabLabel(m_tabs->tabText(i)), idNorm, Qt::CaseInsensitive) == 0)
                    return m_tabs->widget(i);
            }
        }
        return nullptr;
    }

    // Called when a NAMES (353) reply arrives
    void onNamesReceived(const QString &channel, const QStringList &names)
    {
        // Debug log
        m_serverLog->append(QString("[debug] NAMES %1: %2").arg(channel).arg(names.join(", ")));

        QWidget *tab = findTabById(channel);
        if (!tab) {
            // create tab if missing
            QWidget *newTab = createChannelTab(channel);
            int idx = m_tabs->addTab(newTab, channel);
            m_tabs->setTabData(idx, channel);
            tab = newTab;
        }

        QListWidget *users = tab->findChild<QListWidget*>("users");
        if (!users) return;

        // If we haven't seen names for this channel before, treat this as authoritative and clear
        if (!m_namesSeen.contains(channel)) {
            users->clear();
            users->addItems(names);
            users->sortItems(Qt::CaseInsensitive);
            m_namesSeen.insert(channel);
            return;
        }

        // Otherwise merge avoiding duplicates
        QSet<QString> existing;
        for (int i = 0; i < users->count(); ++i) existing.insert(users->item(i)->text());
        bool added = false;
        for (const QString &n : names) {
            if (!existing.contains(n)) {
                users->addItem(n);
                added = true;
            }
        }
        if (added) users->sortItems(Qt::CaseInsensitive);
    }

    // Called when server signals end of names (366)
    void onNamesEnd(const QString &channel)
    {
        Q_UNUSED(channel)
        // For now we don't need special handling; namesReceived already populated.
        // Keep this hook for future improvements (e.g., show "end of list" marker).
    }

    void onJoinReceived(const QString &channel, const QString &nick)
    {
        // Debug log
        m_serverLog->append(QString("[debug] JOIN %1: %2").arg(channel).arg(nick));

        QWidget *tab = findTabById(channel);
        if (!tab) {
            // If tab missing, create it
            QWidget *newTab = createChannelTab(channel);
            int idx = m_tabs->addTab(newTab, channel);
            m_tabs->setTabData(idx, channel);
            tab = newTab;
        }
        QListWidget *users = tab->findChild<QListWidget*>("users");
        if (!users) return;
        QList<QListWidgetItem*> found = users->findItems(nick, Qt::MatchExactly);
        if (found.isEmpty()) users->addItem(nick);
        users->sortItems(Qt::CaseInsensitive);

        QTextEdit *view = tab->findChild<QTextEdit*>("view");
        if (view) {
            QString ts = QDateTime::currentDateTime().toString("hh:mm");
            view->append(QString("<i>%1 joined [%2]</i>").arg(nick).arg(ts));
        }
    }

    void onPartReceived(const QString &channel, const QString &nick)
    {
        // Debug log
        m_serverLog->append(QString("[debug] PART %1: %2").arg(channel).arg(nick));

        QWidget *tab = findTabById(channel);
        if (!tab) return;
        QListWidget *users = tab->findChild<QListWidget*>("users");
        if (!users) return;
        QList<QListWidgetItem*> found = users->findItems(nick, Qt::MatchExactly);
        for (QListWidgetItem *it : found) delete it;

        QTextEdit *view = tab->findChild<QTextEdit*>("view");
        if (view) {
            QString ts = QDateTime::currentDateTime().toString("hh:mm");
            view->append(QString("<i>%1 left [%2]</i>").arg(nick).arg(ts));
        }
    }

    void onNickChanged(const QString &oldNick, const QString &newNick)
    {
        // Debug log
        m_serverLog->append(QString("[debug] NICK %1 -> %2").arg(oldNick).arg(newNick));

        // update all channel lists where oldNick exists
        for (int t=0; t<m_tabs->count(); ++t) {
            QWidget *tab = m_tabs->widget(t);
            QListWidget *users = tab->findChild<QListWidget*>("users");
            if (!users) continue;
            QList<QListWidgetItem*> found = users->findItems(oldNick, Qt::MatchExactly);
            for (QListWidgetItem *it : found) it->setText(newNick);
            users->sortItems(Qt::CaseInsensitive);
        }

        // update private tab name if exists
        QWidget *privateTab = findTabById(oldNick);
        if (privateTab) {
            int idx = m_tabs->indexOf(privateTab);
            if (idx >= 0) {
                m_tabs->setTabText(idx, newNick);
                m_tabs->setTabData(idx, newNick);
            }
        }
    }

private:
    QWidget* createServerTab()
    {
        QWidget *w = new QWidget();
        QVBoxLayout *l = new QVBoxLayout(w);
        m_serverLog = new QTextEdit();
        m_serverLog->setReadOnly(true);
        l->addWidget(m_serverLog);
        return w;
    }

    QWidget* createChannelTab(const QString &name)
    {
        QWidget *w = new QWidget();
        QVBoxLayout *l = new QVBoxLayout(w);

        QTextEdit *view = new QTextEdit();
        view->setReadOnly(true);
        view->setObjectName("view");
        view->setStyleSheet("QTextEdit { background: #0b0b0b; color: #e6e6e6; }");

        QListWidget *users = new QListWidget();
        users->setObjectName("users");
        users->setMaximumWidth(200);

        // Double-click user to open private tab
        connect(users, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item){
            if (!item) return;
            QString nick = item->text();
            ensurePrivateTab(nick);
            QWidget *tab = findTabById(nick);
            if (tab) m_tabs->setCurrentWidget(tab);
        });

        QHBoxLayout *h = new QHBoxLayout();
        h->addWidget(view);
        h->addWidget(users);
        l->addLayout(h);

        return w;
    }

    QWidget* createPrivateTab(const QString &nick)
    {
        QWidget *w = new QWidget();
        QVBoxLayout *l = new QVBoxLayout(w);
        QTextEdit *view = new QTextEdit();
        view->setReadOnly(true);
        view->setObjectName("view");
        view->setStyleSheet("QTextEdit { background: #101010; color: #dcdcdc; }");
        l->addWidget(view);
        return w;
    }

    void ensurePrivateTab(const QString &nick)
    {
        if (findTabById(nick)) return;
        QWidget *tab = createPrivateTab(nick);
        int idx = m_tabs->addTab(tab, nick);
        m_tabs->setTabData(idx, nick);
    }

    void notifyUser(const QString &title, const QString &message)
    {
        if (m_tray && m_tray->isVisible()) {
            m_tray->showMessage(title, message, QSystemTrayIcon::Information, 5000);
        }
    }

    void markTabUnread(QWidget *tabWidget)
    {
        int idx = m_tabs->indexOf(tabWidget);
        if (idx < 0) return;
        QString text = m_tabs->tabText(idx);
        if (!text.startsWith("* ")) {
            m_tabs->setTabText(idx, "* " + text);
            QTimer::singleShot(0, [this, idx](){
                QWidget *w = m_tabs->widget(idx);
                if (w) w->setStyleSheet("background-color: #1a1a1a;");
            });
        }
    }

private:
    QTabWidget *m_tabs = nullptr;
    QTextEdit *m_serverLog = nullptr;
    QLineEdit *m_input = nullptr;
    QLineEdit *m_serverHost = nullptr;
    QLineEdit *m_serverPort = nullptr;
    QLineEdit *m_nickEdit = nullptr;
    QLineEdit *m_channelEdit = nullptr;
    IrcConnection m_irc;
    QSystemTrayIcon *m_tray = nullptr;
    QString m_currentNick;

    // Track which channels we've seen NAMES for (so first NAMES clears list)
    QSet<QString> m_namesSeen;
};

#include "qirc1.moc"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("qIRC Patched Single-file");
    MainWindow w;
    w.show();
    return a.exec();
}

#include <QObject>
#include <QtGlobal>
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#error "Qt6 is required"
#endif
