// qirc.cpp
// Single-file Qt6 IRC client (minimal, mIRC-like UI, notifications, server/channel/private tabs)
// Build with CMake or qmake; requires Qt6 Widgets and Network modules.

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

//
// IrcConnection: minimal IRC protocol handling (connect, send raw, basic parsing)
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
                QString server = line.section(' ', 1).trimmed();
                sendRaw(QString("PONG %1").arg(server));
            }

            // PRIVMSG parsing: :nick!user@host PRIVMSG target :message
            QRegularExpression re(R"(^:([^!]+)![^ ]+ PRIVMSG ([^ ]+) :(.+)$)");
            auto match = re.match(line);
            if (match.hasMatch()) {
                QString from = match.captured(1);
                QString target = match.captured(2);
                QString message = match.captured(3);
                emit privmsgReceived(from, target, message);
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
        setWindowTitle("qIRC (Qt6) - Single-file Example");
        resize(900, 600);

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
        m_tabs->addTab(serverTab, "Server");
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
        QString host = m_serverHost->text();
        quint16 port = m_serverPort->text().toUShort();
        m_currentNick = m_nickEdit->text().trimmed();
        if (m_currentNick.isEmpty()) return;

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
        m_serverLog->append(line);
    }

    void onPrivmsg(const QString &from, const QString &target, const QString &message)
    {
        bool isPrivate = (target.compare(m_currentNick, Qt::CaseInsensitive) == 0);

        if (isPrivate) {
            ensurePrivateTab(from);
            for (int i=0;i<m_tabs->count();++i) {
                if (m_tabs->tabText(i) == from) {
                    QWidget *tab = m_tabs->widget(i);
                    QTextEdit *view = tab->findChild<QTextEdit*>("view");
                    if (view) {
                        QString ts = QDateTime::currentDateTime().toString("hh:mm");
                        view->append(QString("<b>%1</b> [%2]: %3").arg(from).arg(ts).arg(message.toHtmlEscaped()));
                    }
                    notifyUser(QString("Private from %1").arg(from), message);
                    markTabUnread(tab);
                    break;
                }
            }
        } else {
            QString chan = target;
            for (int i=0;i<m_tabs->count();++i) {
                if (m_tabs->tabText(i) == chan) {
                    QWidget *tab = m_tabs->widget(i);
                    QTextEdit *view = tab->findChild<QTextEdit*>("view");
                    if (view) {
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
                    return;
                }
            }
            // If channel tab not found, create it (optional)
            // createChannelTab(chan); // commented to avoid auto-creating on every message
        }
    }

    void sendMessage()
    {
        QString text = m_input->text().trimmed();
        if (text.isEmpty()) return;

        int idx = m_tabs->currentIndex();
        if (idx < 0) return;
        QString target = m_tabs->tabText(idx);
        if (target.startsWith("* ")) target = target.mid(2);

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
        if (!chan.startsWith("#")) chan = "#" + chan;
        m_irc.sendRaw(QString("JOIN %1").arg(chan));
        QWidget *tab = createChannelTab(chan);
        m_tabs->addTab(tab, chan);
    }

    void onTrayActivated(QSystemTrayIcon::ActivationReason reason)
    {
        if (reason == QSystemTrayIcon::Trigger) {
            showNormal();
            activateWindow();
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
        users->setMaximumWidth(160);

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
        for (int i=0;i<m_tabs->count();++i) {
            if (m_tabs->tabText(i) == nick) return;
        }
        QWidget *tab = createPrivateTab(nick);
        m_tabs->addTab(tab, nick);
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
};

#include "qirc1.moc"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("qIRC Single-file");
    MainWindow w;
    w.show();
    return a.exec();
}

#include <QObject>
#include <QtGlobal>
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#error "Qt6 is required"
#endif

