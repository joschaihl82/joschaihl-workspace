#include <QApplication>
#include <QMainWindow>
#include <QTcpSocket>
#include <QTabWidget>
#include <QTabBar>
#include <QTextEdit>
#include <QLineEdit>
#include <QMap>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QSplitter>
#include <QStringList>
#include <QList>
#include <QTableWidget>
#include <QHeaderView>
#include <QFontDatabase>
#include <QRegularExpression>
#include <QColor>
#include <QPushButton>
#include <QHBoxLayout>

class IrcClient : public QMainWindow {
    Q_OBJECT

public:
    IrcClient(QWidget *parent = nullptr) : QMainWindow(parent) {
        // fixed-width console font
        QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        if (fixedFont.family().isEmpty()) {
            fixedFont = QFont("Monospace");
            fixedFont.setStyleHint(QFont::Monospace);
        }
        fixedFont.setPointSize(10);
        consoleFont = fixedFont;

        tabCtr = new QTabWidget(this);
        setCentralWidget(tabCtr);
        tabCtr->setTabsClosable(true);
        connect(tabCtr, &QTabWidget::currentChanged, this, &IrcClient::resTab);
        connect(tabCtr, &QTabWidget::tabCloseRequested, this, &IrcClient::closeTab);

        netCon = new QTcpSocket(this);
        connect(netCon, &QTcpSocket::readyRead, this, &IrcClient::readDat);

        // LIST request bookkeeping
        listRequestCounter = 0;
        activeListRequestId = 0;

        makeTab("status");

        channelListTabCreated = false;
        channelTable = nullptr;

        netCon->connectToHost("irc.libera.chat", 6667);
        connect(netCon, &QTcpSocket::connected, this, [this]() {
            const QString nickCmd = "NICK joscha";
            const QString userCmd = "USER joscha 0 * :joscha";
            netCon->write((nickCmd + "\r\n").toUtf8());
            appendStatus(nickCmd, Qt::red);
            netCon->write((userCmd + "\r\n").toUtf8());
            appendStatus(userCmd, Qt::red);
        });
    }

private slots:
    void readDat() {
        while (netCon->canReadLine()) {
            QString strLin = QString::fromUtf8(netCon->readLine()).trimmed();
            parLin(strLin);
        }
    }

    void sndDat() {
        QLineEdit *txtInp = qobject_cast<QLineEdit*>(sender());
        if (!txtInp) return;
        QString strTxt = txtInp->text();
        QString strNam = txtInp->property("tabname").toString();
        txtInp->clear();

        if (strTxt.startsWith("/join ")) {
            QString strChn = strTxt.mid(6).trimmed();
            const QString cmd = "JOIN " + strChn;
            netCon->write((cmd + "\r\n").toUtf8());
            appendStatus(cmd, Qt::red);
            makeTab(strChn);
            tabCtr->setCurrentIndex(tabCtr->count() - 1);
        } else if (strTxt.startsWith("/msg ")) {
            int idxSpc = strTxt.indexOf(' ', 5);
            if (idxSpc != -1) {
                QString strUsr = strTxt.mid(5, idxSpc - 5).trimmed();
                QString strMsg = strTxt.mid(idxSpc + 1);
                const QString cmd = "PRIVMSG " + strUsr + " :" + strMsg;
                netCon->write((cmd + "\r\n").toUtf8());
                appendStatus(cmd, Qt::red);
                makeTab(strUsr);
                showTxt(strUsr, "<" + strUsr + "> " + strMsg);
                tabCtr->setCurrentIndex(tabCtr->count() - 1);
            }
        } else {
            if (strNam != "status") {
                const QString cmd = "PRIVMSG " + strNam + " :" + strTxt;
                netCon->write((cmd + "\r\n").toUtf8());
                appendStatus(cmd, Qt::red);
                showTxt(strNam, "<joscha> " + strTxt);
            } else {
                netCon->write((strTxt + "\r\n").toUtf8());
                appendStatus(strTxt, Qt::red);
            }
        }
    }

    void opnQry(QListWidgetItem *itmUsr) {
        QString strUsr = itmUsr->text();
        if (strUsr.startsWith("@") || strUsr.startsWith("+")) strUsr = strUsr.mid(1);
        makeTab(strUsr);
        tabCtr->setCurrentIndex(tabCtr->count() - 1);
    }

    void resTab(int idxTab) {
        if (idxTab >= 0 && idxTab < tabCtr->count())
            tabCtr->tabBar()->setTabTextColor(idxTab, Qt::black);
    }

    void onChannelTableCellDoubleClicked(int row, int /*column*/) {
        if (!channelTable) return;
        if (row < 0 || row >= channelTable->rowCount()) return;
        QTableWidgetItem *item = channelTable->item(row, 0);
        if (!item) return;
        QString chn = item->text().trimmed();
        if (chn.isEmpty() || !chn.startsWith("#")) return;

        const QString cmd = "JOIN " + chn;
        netCon->write((cmd + "\r\n").toUtf8());
        appendStatus(cmd, Qt::red);
        makeTab(chn);

        for (int i = 0; i < tabCtr->count(); ++i) {
            if (tabCtr->tabText(i) == chn) { tabCtr->setCurrentIndex(i); break; }
        }
    }

    void closeTab(int index) {
        if (index < 0 || index >= tabCtr->count()) return;
        QString tabName = tabCtr->tabText(index);
        if (tabName == "status") return;

        if (tabName == "Channel List") {
            QWidget *w = tabCtr->widget(index);
            tabCtr->removeTab(index);
            if (w) delete w;
            // keep channelUserCount/channelTopic/pending maps intact
            channelListTabCreated = false;
            channelTable = nullptr;
            return;
        }

        mapTxt.remove(tabName);
        mapLst.remove(tabName);
        channelsToQuery.remove(tabName);
        channelsNamesReceived.remove(tabName);
        QWidget *w = tabCtr->widget(index);
        tabCtr->removeTab(index);
        if (w) delete w;
    }

    // Refresh button handler: start a new LIST request
    void sendListRefresh() {
        // increment request counter and mark active request
        ++listRequestCounter;
        activeListRequestId = listRequestCounter;

        // clear pending buffers for this new request
        pendingChannelUserCount.clear();
        pendingChannelTopic.clear();

        const QString cmd = "LIST";
        netCon->write((cmd + "\r\n").toUtf8());
        appendStatus(cmd, Qt::red);
    }

private:
    QTcpSocket *netCon;
    QTabWidget *tabCtr;
    QMap<QString, QTextEdit*> mapTxt;
    QMap<QString, QListWidget*> mapLst;

    // main LIST data (last completed LIST)
    QMap<QString,int> channelUserCount;
    QMap<QString,QString> channelTopic;

    // pending data for the currently active LIST request
    QMap<QString,int> pendingChannelUserCount;
    QMap<QString,QString> pendingChannelTopic;

    QSet<QString> channelsToQuery;
    QSet<QString> channelsNamesReceived;

    // request id bookkeeping
    quint64 listRequestCounter;      // increments for each LIST request
    quint64 activeListRequestId;     // id of the request we're currently collecting (0 = none)

    bool channelListTabCreated;
    QTableWidget *channelTable;
    QFont consoleFont;

    void appendStatus(const QString &text, const QColor &color) {
        if (!mapTxt.contains("status")) return;
        QTextEdit *status = mapTxt["status"];
        if (!status) return;
        const QString escaped = text.toHtmlEscaped();
        const QString html = QString("<span style=\"color:%1;\">%2</span>").arg(color.name(), escaped);
        status->append(html);
    }

    QString colorizeUserTags(const QString &plain) const {
        static const QRegularExpression re(R"(<([^>\s]+)>)");
        QString result;
        result.reserve(plain.size() * 2);
        int lastIndex = 0;
        QRegularExpressionMatchIterator it = re.globalMatch(plain);
        while (it.hasNext()) {
            QRegularExpressionMatch m = it.next();
            int start = m.capturedStart(0);
            int end = m.capturedEnd(0);
            result += plain.mid(lastIndex, start - lastIndex).toHtmlEscaped();
            QString username = m.captured(1);
            uint hval = qHash(username);
            int hue = int(hval % 360);
            int sat = 220;
            int val = 220;
            QColor col = QColor::fromHsv(hue, sat, val);
            QString hex = col.name();
            QString escUser = username.toHtmlEscaped();
            result += QString("<span style=\"color:%1; font-weight:bold;\">&lt;%2&gt;</span>").arg(hex, escUser);
            lastIndex = end;
        }
        if (lastIndex < plain.size()) result += plain.mid(lastIndex).toHtmlEscaped();
        return result;
    }

    void makeTab(const QString &strNam) {
        if (mapTxt.contains(strNam)) return;
        QWidget *widPag = new QWidget();
        QVBoxLayout *boxLay = new QVBoxLayout(widPag);
        QSplitter *splTop = new QSplitter(Qt::Horizontal);
        QTextEdit *txtBox = new QTextEdit();
        txtBox->setReadOnly(true);
        txtBox->setFont(consoleFont);
        splTop->addWidget(txtBox);

        if (strNam.startsWith("#")) {
            QListWidget *lstUsr = new QListWidget();
            lstUsr->setFixedWidth(120);
            lstUsr->setFont(consoleFont);
            lstUsr->setSortingEnabled(true);
            connect(lstUsr, &QListWidget::itemDoubleClicked, this, &IrcClient::opnQry);
            splTop->addWidget(lstUsr);
            mapLst[strNam] = lstUsr;
            channelsToQuery.insert(strNam);
        }

        boxLay->addWidget(splTop);
        QLineEdit *txtInp = new QLineEdit();
        txtInp->setProperty("tabname", strNam);
        txtInp->setFont(consoleFont);
        connect(txtInp, &QLineEdit::returnPressed, this, &IrcClient::sndDat);
        boxLay->addWidget(txtInp);
        tabCtr->addTab(widPag, strNam);
        mapTxt[strNam] = txtBox;
    }

    void ensureChannelListTab() {
        if (channelListTabCreated) return;
        QWidget *wid = new QWidget();
        QVBoxLayout *lay = new QVBoxLayout(wid);

        // top row: refresh button (left) and spacer
        QHBoxLayout *topRow = new QHBoxLayout();
        QPushButton *btnRefresh = new QPushButton("Refresh");
        btnRefresh->setFont(consoleFont);
        connect(btnRefresh, &QPushButton::clicked, this, &IrcClient::sendListRefresh);
        topRow->addWidget(btnRefresh);
        topRow->addStretch();
        lay->addLayout(topRow);

        channelTable = new QTableWidget();
        channelTable->setColumnCount(3);
        channelTable->setHorizontalHeaderLabels(QStringList() << "Channel" << "Users" << "Topic");
        channelTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        channelTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        channelTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        channelTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        channelTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        channelTable->setSelectionMode(QAbstractItemView::SingleSelection);
        channelTable->setFont(consoleFont);
        lay->addWidget(channelTable);

        tabCtr->addTab(wid, "Channel List");
        channelListTabCreated = true;

        connect(channelTable, &QTableWidget::cellDoubleClicked, this, &IrcClient::onChannelTableCellDoubleClicked);
    }

    void populateChannelListTab() {
        ensureChannelListTab();

        // Use the completed main maps (channelUserCount/channelTopic)
        QList<QPair<QString,int>> list;
        for (auto it = channelUserCount.constBegin(); it != channelUserCount.constEnd(); ++it)
            list.append(qMakePair(it.key(), it.value()));
        std::sort(list.begin(), list.end(), [](const QPair<QString,int> &a, const QPair<QString,int> &b){
            return a.second > b.second;
        });

        channelTable->setRowCount(list.size());
        int row = 0;
        for (const auto &p : list) {
            const QString &chn = p.first;
            int users = p.second;
            QString topic = channelTopic.value(chn, QString());
            QTableWidgetItem *itChn = new QTableWidgetItem(chn);
            QTableWidgetItem *itUsers = new QTableWidgetItem(QString::number(users));
            QTableWidgetItem *itTopic = new QTableWidgetItem(topic);
            itChn->setFlags(itChn->flags() & ~Qt::ItemIsEditable);
            itUsers->setFlags(itUsers->flags() & ~Qt::ItemIsEditable);
            itTopic->setFlags(itTopic->flags() & ~Qt::ItemIsEditable);
            itChn->setFont(consoleFont);
            itUsers->setFont(consoleFont);
            itTopic->setFont(consoleFont);
            channelTable->setItem(row, 0, itChn);
            channelTable->setItem(row, 1, itUsers);
            channelTable->setItem(row, 2, itTopic);
            ++row;
        }

        // Do NOT force-select the Channel List tab here.
    }

    void showTxt(const QString &strNam, const QString &strMsg) {
        if (!mapTxt.contains(strNam)) return;
        QTextEdit *ed = mapTxt[strNam];
        if (!ed) return;
        if (strNam == "status") {
            ed->append(strMsg.toHtmlEscaped());
        } else {
            QString html = colorizeUserTags(strMsg);
            ed->append(html);
        }
        int idxTab = -1;
        for (int i = 0; i < tabCtr->count(); ++i) {
            if (tabCtr->tabText(i) == strNam) { idxTab = i; break; }
        }
        if (idxTab != -1 && idxTab != tabCtr->currentIndex()) tabCtr->tabBar()->setTabTextColor(idxTab, Qt::red);
    }

    void parLin(const QString &strLin) {
        // show incoming server line in status (black)
        showTxt("status", strLin);

        // PING/PONG
        if (strLin.startsWith("PING ")) {
            QString strPng = strLin;
            strPng.replace(0, 4, "PONG");
            netCon->write((strPng + "\r\n").toUtf8());
            appendStatus(strPng, Qt::red);
            return;
        }

        // after login join test channel
        if (strLin.contains(" 001 joscha")) {
            const QString cmd = "JOIN #testchannel";
            netCon->write((cmd + "\r\n").toUtf8());
            appendStatus(cmd, Qt::red);
            makeTab("#testchannel");
        }

        // extract nick of sender
        QString strNck = "";
        if (strLin.startsWith(":")) {
            int idxS1 = strLin.indexOf(' ');
            if (idxS1 != -1) {
                QString strFul = strLin.mid(1, idxS1 - 1);
                strNck = strFul.split('!').first();
            }
        }

        // --- 322: RPL_LIST (only process if we have an active LIST request)
        if (strLin.contains(" 322 ") && activeListRequestId != 0) {
            int idxHash = strLin.indexOf('#');
            if (idxHash != -1) {
                int idxSpaceAfterChannel = strLin.indexOf(' ', idxHash);
                if (idxSpaceAfterChannel != -1) {
                    QString chn = strLin.mid(idxHash, idxSpaceAfterChannel - idxHash).trimmed();
                    int idxAfterChannel = idxSpaceAfterChannel + 1;
                    int idxSpaceAfterCount = strLin.indexOf(' ', idxAfterChannel);
                    if (idxSpaceAfterCount != -1) {
                        QString countStr = strLin.mid(idxAfterChannel, idxSpaceAfterCount - idxAfterChannel).trimmed();
                        bool ok = false;
                        int count = countStr.toInt(&ok);
                        if (ok) pendingChannelUserCount[chn] = count;
                        int idxColon = strLin.indexOf(':', idxSpaceAfterCount);
                        if (idxColon != -1) pendingChannelTopic[chn] = strLin.mid(idxColon + 1).trimmed();
                        else pendingChannelTopic[chn] = QString();
                    }
                }
            }
        }

        // --- 323: End of LIST (only finalize if it belongs to the active request)
        if (strLin.contains(" 323 ") && activeListRequestId != 0) {
            // commit pending maps to main maps (atomic swap)
            channelUserCount = pendingChannelUserCount;
            channelTopic = pendingChannelTopic;

            // populate the table from the committed data
            populateChannelListTab();

            // mark that no LIST request is currently active
            activeListRequestId = 0;

            // keep the committed data in memory (do not clear)
        }

        // --- 353: NAMES reply
        if (strLin.contains(" 353 ")) {
            int idxHsh = strLin.indexOf('#');
            if (idxHsh != -1) {
                int idxCol = strLin.indexOf(':', idxHsh);
                if (idxCol != -1) {
                    QString strChn = strLin.mid(idxHsh, idxCol - idxHsh).trimmed();
                    QString strRaw = strLin.mid(idxCol + 1);
                    if (mapLst.contains(strChn)) {
                        QStringList lstnam = strRaw.split(' ', Qt::SkipEmptyParts);
                        for (const QString &strnam : lstnam) {
                            QString strbas = strnam;
                            if (strbas.startsWith('@') || strbas.startsWith('+')) strbas = strbas.mid(1);
                            QList<QListWidgetItem*> lstold = mapLst[strChn]->findItems(strbas, Qt::MatchEndsWith);
                            for (auto *itmrem : lstold) {
                                QString strtst = itmrem->text();
                                if (strtst == strbas || strtst == "@" + strbas || strtst == "+" + strbas) {
                                    delete mapLst[strChn]->takeItem(mapLst[strChn]->row(itmrem));
                                }
                            }
                            mapLst[strChn]->addItem(strnam);
                        }
                    }
                    channelsNamesReceived.insert(strChn);
                    bool allReceived = true;
                    for (const QString &c : channelsToQuery) {
                        if (!channelsNamesReceived.contains(c)) { allReceived = false; break; }
                    }
                    if (allReceived && !channelsToQuery.isEmpty()) {
                        // start a fresh LIST request (clear pending buffers first)
                        ++listRequestCounter;
                        activeListRequestId = listRequestCounter;
                        pendingChannelUserCount.clear();
                        pendingChannelTopic.clear();

                        const QString cmd = "LIST";
                        netCon->write((cmd + "\r\n").toUtf8());
                        appendStatus(cmd, Qt::red);

                        channelsToQuery.clear();
                        channelsNamesReceived.clear();
                    }
                }
            }
        }

        // JOIN processing (user joined)
        if (strLin.contains(" JOIN ")) {
            int idxHsh = strLin.indexOf('#');
            if (idxHsh != -1) {
                QString strChn = strLin.mid(idxHsh).trimmed();
                if (strChn.startsWith(":")) strChn = strChn.mid(1);
                if (mapLst.contains(strChn)) {
                    QList<QListWidgetItem*> lstfnd = mapLst[strChn]->findItems(strNck, Qt::MatchEndsWith);
                    bool bexists = false;
                    for (auto *itmtst : lstfnd) {
                        if (itmtst->text() == strNck || itmtst->text() == "@" + strNck || itmtst->text() == "+" + strNck) {
                            bexists = true; break;
                        }
                    }
                    if (!bexists) mapLst[strChn]->addItem(strNck);
                }
                showTxt(strChn, "*** " + strNck + " joined");
            }
        }
        else if (strLin.contains(" PRIVMSG ")) {
            int idxS1 = strLin.indexOf(' ');
            int idxS2 = strLin.indexOf(' ', idxS1 + 1);
            int idxCol = strLin.indexOf(':', idxS2);
            if (idxS1 != -1 && idxS2 != -1 && idxCol != -1) {
                QString strTgt = strLin.mid(idxS2 + 1, idxCol - idxS2 - 2).trimmed();
                QString strTxt = strLin.mid(idxCol + 1);
                QString strTab = (strTgt == "joscha") ? strNck : strTgt;
                makeTab(strTab);
                showTxt(strTab, "<" + strNck + "> " + strTxt);
            }
        }
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    IrcClient winApp;
    winApp.showMaximized();
    return app.exec();
}

#include "qirc1.moc"
