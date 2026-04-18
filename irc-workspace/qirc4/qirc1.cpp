// main.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QIntValidator>
#include <QTcpSocket>
#include <QTimer>
#include <QDateTime>
#include <QPair>
#include <QList>

//
// Single-file Qt6 IRC client
// - Hard-coded servers with ports
// - Hard-coded nickname "joscha" (read-only in UI)
// - 5 second timeout per server attempt
// - Auto-connects on startup (multi-server sequential attempts)
// Build with the provided CMakeLists.txt
//

class IrcConnection : public QObject
{
    Q_OBJECT
public:
    explicit IrcConnection(QObject *parent = nullptr)
        : QObject(parent),
          m_socket(new QTcpSocket(this)),
          m_attemptTimer(new QTimer(this)),
          m_currentIndex(-1),
          m_timeoutMs(5000),
          m_joined(false)
    {
        m_attemptTimer->setSingleShot(true);

        connect(m_socket, &QTcpSocket::readyRead, this, &IrcConnection::onReadyRead);
        connect(m_socket, &QTcpSocket::connected, this, &IrcConnection::onConnected);
        connect(m_socket, &QTcpSocket::disconnected, this, &IrcConnection::onDisconnected);
        connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
                this, &IrcConnection::onErrorOccurred);
        connect(m_attemptTimer, &QTimer::timeout, this, &IrcConnection::onAttemptTimeout);
    }

    // servers: list of (host, port) pairs
    void tryConnectToServers(const QList<QPair<QString, quint16>> &servers,
                             const QString &nick, const QString &user = QString())
    {
        if (servers.isEmpty()) {
            emit errorOccurred(QStringLiteral("No servers provided"));
            return;
        }
        m_servers = servers;
        m_nick = nick.trimmed();
        m_user = user.isEmpty() ? m_nick : user.trimmed();
        m_currentIndex = -1;
        m_joined = false;
        m_buffer.clear();

        if (m_socket->state() == QAbstractSocket::ConnectedState ||
            m_socket->state() == QAbstractSocket::ConnectingState) {
            m_socket->abort();
        }
        tryNextServer();
    }

    void joinChannel(const QString &channel)
    {
        const QString ch = channel.trimmed();
        if (!ch.isEmpty()) {
            sendRaw(QStringLiteral("JOIN %1").arg(ch));
        }
    }

    void sendMessage(const QString &target, const QString &text)
    {
        const QString t = target.trimmed();
        if (t.isEmpty() || text.isEmpty())
            return;
        sendRaw(QStringLiteral("PRIVMSG %1 :%2").arg(t, text));
    }

    void quit(const QString &reason = QStringLiteral("Leaving"))
    {
        sendRaw(QStringLiteral("QUIT :%1").arg(reason));
        m_socket->disconnectFromHost();
    }

    void setTimeoutMs(int ms) { m_timeoutMs = ms; }
    int timeoutMs() const { return m_timeoutMs; }

signals:
    void connectedToServer(const QString &server, quint16 port);
    void connected();
    void disconnected();
    void errorOccurred(const QString &error);
    void rawLineReceived(const QString &line);
    void messageReceived(const QString &prefix,
                         const QString &command,
                         const QStringList &params);
    void attemptStatus(const QString &status);

private slots:
    void onReadyRead()
    {
        m_buffer.append(m_socket->readAll());

        while (true) {
            int idx = m_buffer.indexOf("\r\n");
            if (idx == -1)
                break;
            QByteArray rawLine = m_buffer.left(idx);
            m_buffer.remove(0, idx + 2);
            QString line = QString::fromUtf8(rawLine);
            emit rawLineReceived(line);
            parseLine(line);
        }
    }

    void onConnected()
    {
        if (m_attemptTimer->isActive())
            m_attemptTimer->stop();

        if (m_nick.isEmpty())
            m_nick = QStringLiteral("joscha");

        sendRaw(QStringLiteral("NICK %1").arg(m_nick));
        sendRaw(QStringLiteral("USER %1 0 * :%2").arg(m_user, QStringLiteral("Qt IRC Client")));

        emit connected();
        if (m_currentIndex >= 0 && m_currentIndex < m_servers.size()) {
            const auto &p = m_servers.at(m_currentIndex);
            emit connectedToServer(p.first, p.second);
        }
    }

    void onDisconnected()
    {
        emit disconnected();
    }

    void onErrorOccurred(QAbstractSocket::SocketError)
    {
        QString err = m_socket->errorString();
        emit errorOccurred(err);
        emit attemptStatus(QStringLiteral("Error on %1:%2 : %3").arg(currentHost()).arg(currentPort()).arg(err));
        if (m_attemptTimer->isActive())
            m_attemptTimer->stop();
        tryNextServer();
    }

    void onAttemptTimeout()
    {
        QString srv = currentHost();
        quint16 port = currentPort();
        emit attemptStatus(QStringLiteral("Timeout connecting to %1:%2").arg(srv).arg(port));
        if (m_socket->state() == QAbstractSocket::ConnectingState ||
            m_socket->state() == QAbstractSocket::ConnectedState) {
            m_socket->abort();
        }
        tryNextServer();
    }

private:
    void tryNextServer()
    {
        m_currentIndex++;
        if (m_currentIndex >= m_servers.size()) {
            emit attemptStatus(QStringLiteral("All servers exhausted"));
            emit errorOccurred(QStringLiteral("Could not connect to any server"));
            return;
        }

        QString srv = currentHost();
        quint16 port = currentPort();
        if (srv.isEmpty()) {
            tryNextServer();
            return;
        }

        emit attemptStatus(QStringLiteral("Attempting %1:%2 (timeout %3 ms)")
                           .arg(srv).arg(port).arg(m_timeoutMs));

        m_socket->abort();
        m_socket->connectToHost(srv, port);
        m_attemptTimer->start(m_timeoutMs);
    }

    QString currentHost() const
    {
        if (m_currentIndex >= 0 && m_currentIndex < m_servers.size())
            return m_servers.at(m_currentIndex).first;
        return QString();
    }

    quint16 currentPort() const
    {
        if (m_currentIndex >= 0 && m_currentIndex < m_servers.size())
            return m_servers.at(m_currentIndex).second;
        return 0;
    }

    void sendRaw(const QString &line)
    {
        if (!m_socket)
            return;
        if (m_socket->state() != QAbstractSocket::ConnectedState) {
            emit errorOccurred(QStringLiteral("Not connected: cannot send"));
            return;
        }
        QByteArray data = line.toUtf8();
        data.append("\r\n");
        qint64 written = m_socket->write(data);
        if (written == -1) {
            emit errorOccurred(QStringLiteral("Failed to write to socket"));
        }
    }

    void parseLine(const QString &line)
    {
        QString rest = line;
        QString prefix;
        if (rest.startsWith(':')) {
            int space = rest.indexOf(' ');
            if (space == -1)
                return;
            prefix = rest.mid(1, space - 1);
            rest = rest.mid(space + 1);
        }

        int space = rest.indexOf(' ');
        QString command;
        if (space == -1) {
            command = rest;
            rest.clear();
        } else {
            command = rest.left(space);
            rest = rest.mid(space + 1);
        }

        QStringList params;
        while (!rest.isEmpty()) {
            if (rest.startsWith(':')) {
                params << rest.mid(1);
                break;
            }
            int nextSpace = rest.indexOf(' ');
            if (nextSpace == -1) {
                params << rest;
                break;
            }
            params << rest.left(nextSpace);
            rest = rest.mid(nextSpace + 1);
        }

        if (command.compare(QStringLiteral("PING"), Qt::CaseInsensitive) == 0 && !params.isEmpty()) {
            sendRaw(QStringLiteral("PONG :%1").arg(params.first()));
        }

        emit messageReceived(prefix, command, params);

        bool isNumeric = false;
        int numeric = command.toInt(&isNumeric);
        if (isNumeric && numeric == 1 && !m_joined) {
            m_joined = true;
        }
    }

    QTcpSocket *m_socket;
    QTimer *m_attemptTimer;
    QByteArray m_buffer;
    QList<QPair<QString, quint16>> m_servers;
    int m_currentIndex;
    QString m_nick;
    QString m_user;
    int m_timeoutMs;
    bool m_joined;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent),
          m_irc(new IrcConnection(this))
    {
        setupUi();

        connect(m_irc, &IrcConnection::rawLineReceived, this, &MainWindow::onIrcRawLine);
        connect(m_irc, &IrcConnection::messageReceived, this, &MainWindow::onIrcMessage);
        connect(m_irc, &IrcConnection::errorOccurred, this, &MainWindow::onIrcError);
        connect(m_irc, &IrcConnection::connected, this, &MainWindow::onIrcConnected);
        connect(m_irc, &IrcConnection::disconnected, this, &MainWindow::onIrcDisconnected);
        connect(m_irc, &IrcConnection::attemptStatus, this, &MainWindow::onAttemptStatus);
        connect(m_irc, &IrcConnection::connectedToServer, this, &MainWindow::onConnectedToServer);

        // fixed timeout 5 seconds
        m_irc->setTimeoutMs(5000);

        // Hard-coded servers with ports (edit here if you want different servers/ports)
        // Note: availability depends on network and server policies.
        m_hardCodedServers = {
            qMakePair(QStringLiteral("irc.libera.chat"), (quint16)6667),
            qMakePair(QStringLiteral("irc.libera.chat"), (quint16)6697), // TLS port (not using TLS here)
            qMakePair(QStringLiteral("irc.oftc.net"), (quint16)6667),
            qMakePair(QStringLiteral("irc.rizon.net"), (quint16)6667),
            qMakePair(QStringLiteral("irc.ircnet.com"), (quint16)6667)
        };

        // Hard-coded nickname "joscha"
        m_nickEdit->setText(QStringLiteral("joscha"));
        m_nickEdit->setReadOnly(true);

        // Start auto-connect shortly after startup so the UI is visible first
        QTimer::singleShot(150, this, &MainWindow::startAutoConnect);
    }

private slots:
    // Called automatically on startup
    void startAutoConnect()
    {
        const QString channel = m_channelEdit->text().trimmed();
        if (m_hardCodedServers.isEmpty()) {
            appendText(QStringLiteral("*** No hard-coded servers available"));
            return;
        }
        appendText(QStringLiteral("*** Auto-starting multi-server connect attempts (5s timeout each)"));
        m_desiredChannel = channel;
        m_irc->tryConnectToServers(m_hardCodedServers, QStringLiteral("joscha"));
    }

    // Multi-Connect: tries the hard-coded servers sequentially with 5s timeout each
    void onMultiConnectClicked()
    {
        const QString channel = m_channelEdit->text().trimmed();

        if (m_hardCodedServers.isEmpty()) {
            appendText(QStringLiteral("*** No hard-coded servers available"));
            return;
        }

        appendText(QStringLiteral("*** Starting multi-server connect attempts (5s timeout each)"));
        m_desiredChannel = channel;
        m_irc->tryConnectToServers(m_hardCodedServers, QStringLiteral("joscha"));
    }

    // Single Connect: attempts only the first hard-coded server (host+port)
    void onConnectClicked()
    {
        const QString channel = m_channelEdit->text().trimmed();

        if (m_hardCodedServers.isEmpty()) {
            appendText(QStringLiteral("*** No hard-coded servers available"));
            return;
        }

        const auto first = m_hardCodedServers.first();
        appendText(QStringLiteral("*** Connecting to %1:%2 as joscha")
                   .arg(first.first).arg(first.second));

        m_desiredChannel = channel;
        m_irc->tryConnectToServers(QList<QPair<QString, quint16>>{ first }, QStringLiteral("joscha"));
    }

    void onSendClicked()
    {
        const QString text = m_inputEdit->text();
        if (text.isEmpty())
            return;

        const QString channel = m_channelEdit->text().trimmed();
        if (!channel.isEmpty()) {
            m_irc->sendMessage(channel, text);
            appendText(QStringLiteral("<%1> %2").arg(QStringLiteral("joscha"), text));
        } else {
            appendText(QStringLiteral("*** No channel set"));
        }
        m_inputEdit->clear();
    }

    void onIrcRawLine(const QString &line)
    {
        appendText(QStringLiteral(">> %1").arg(line));
    }

    void onIrcMessage(const QString &prefix,
                      const QString &command,
                      const QStringList &params)
    {
        Q_UNUSED(prefix);

        bool isNumeric = false;
        int numeric = command.toInt(&isNumeric);
        if (isNumeric && numeric == 1) {
            if (!m_desiredChannel.isEmpty()) {
                m_irc->joinChannel(m_desiredChannel);
                appendText(QStringLiteral("*** Joining %1").arg(m_desiredChannel));
            }
        }

        if (command.compare(QStringLiteral("PRIVMSG"), Qt::CaseInsensitive) == 0 && params.size() >= 2) {
            QString from = prefix;
            int ex = from.indexOf('!');
            if (ex != -1)
                from = from.left(ex);
            const QString target = params.at(0);
            const QString msg = params.at(1);
            appendText(QStringLiteral("[%1] <%2> %3").arg(target, from, msg));
        }

        if (command.compare(QStringLiteral("NOTICE"), Qt::CaseInsensitive) == 0 && params.size() >= 2) {
            const QString target = params.at(0);
            const QString msg = params.at(1);
            appendText(QStringLiteral("[NOTICE %1] %2").arg(target, msg));
        }
    }

    void onIrcError(const QString &error)
    {
        appendText(QStringLiteral("*** Error: %1").arg(error));
    }

    void onIrcConnected()
    {
        appendText(QStringLiteral("*** Connected (handshake sent)"));
    }

    void onIrcDisconnected()
    {
        appendText(QStringLiteral("*** Disconnected"));
    }

    void onAttemptStatus(const QString &status)
    {
        appendText(QStringLiteral("*** %1").arg(status));
    }

    void onConnectedToServer(const QString &server, quint16 port)
    {
        appendText(QStringLiteral("*** Successfully connected to %1:%2").arg(server).arg(port));
    }

private:
    void setupUi()
    {
        auto *central = new QWidget(this);
        auto *mainLayout = new QVBoxLayout;

        auto *connLayout = new QHBoxLayout;
        m_serverEdit = new QLineEdit;
        m_portEdit   = new QLineEdit;
        m_portEdit->setValidator(new QIntValidator(1, 65535, m_portEdit));
        m_nickEdit   = new QLineEdit;
        m_channelEdit = new QLineEdit;
        m_connectButton = new QPushButton(tr("Connect (first hard-coded)"));
        m_multiConnectButton = new QPushButton(tr("Multi-Connect (5s timeout)"));

        m_serverEdit->setPlaceholderText("server field ignored (servers hard-coded)");
        m_portEdit->setPlaceholderText("port field ignored for multi-connect");
        m_nickEdit->setPlaceholderText("joscha");
        m_channelEdit->setPlaceholderText("#qt");

        connLayout->addWidget(new QLabel("Server:"));
        connLayout->addWidget(m_serverEdit);
        connLayout->addWidget(new QLabel("Port:"));
        connLayout->addWidget(m_portEdit);
        connLayout->addWidget(new QLabel("Nick:"));
        connLayout->addWidget(m_nickEdit);
        connLayout->addWidget(new QLabel("Channel:"));
        connLayout->addWidget(m_channelEdit);
        connLayout->addWidget(m_connectButton);
        connLayout->addWidget(m_multiConnectButton);

        m_chatView = new QTextEdit;
        m_chatView->setReadOnly(true);

        auto *inputLayout = new QHBoxLayout;
        m_inputEdit = new QLineEdit;
        m_sendButton = new QPushButton(tr("Send"));
        inputLayout->addWidget(m_inputEdit);
        inputLayout->addWidget(m_sendButton);

        mainLayout->addLayout(connLayout);
        mainLayout->addWidget(m_chatView);
        mainLayout->addLayout(inputLayout);

        central->setLayout(mainLayout);
        setCentralWidget(central);
        setWindowTitle("Qt6 IRC Client (hard-coded servers+ports, nick joscha)");

        connect(m_connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
        connect(m_multiConnectButton, &QPushButton::clicked, this, &MainWindow::onMultiConnectClicked);
        connect(m_sendButton, &QPushButton::clicked, this, &MainWindow::onSendClicked);
        connect(m_inputEdit, &QLineEdit::returnPressed, this, &MainWindow::onSendClicked);
    }

    void appendText(const QString &text)
    {
        const QString ts = QDateTime::currentDateTime().toString("HH:mm:ss");
        m_chatView->append(QStringLiteral("[%1] %2").arg(ts, text));
    }

    IrcConnection *m_irc;
    QList<QPair<QString, quint16>> m_hardCodedServers;
    QString m_desiredChannel;

    QLineEdit *m_serverEdit = nullptr;
    QLineEdit *m_portEdit = nullptr;
    QLineEdit *m_nickEdit = nullptr;
    QLineEdit *m_channelEdit = nullptr;
    QPushButton *m_multiConnectButton = nullptr;

    QTextEdit  *m_chatView = nullptr;
    QLineEdit *m_inputEdit = nullptr;
    QPushButton *m_connectButton = nullptr;
    QPushButton *m_sendButton = nullptr;
};

#include "qirc1.moc"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.resize(1000, 640);
    w.show();
    return app.exec();
}
