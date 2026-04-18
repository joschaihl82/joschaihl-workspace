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

class IrcClient : public QMainWindow {
    Q_OBJECT

public:
    IrcClient(QWidget *parent = nullptr) : QMainWindow(parent) {
        // choose a fixed-width console font
        QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        if (fixedFont.family().isEmpty()) {
            fixedFont = QFont("Monospace");
            fixedFont.setStyleHint(QFont::Monospace);
        }
        fixedFont.setPointSize(10);

        tabCtr = new QTabWidget(this);
        setCentralWidget(tabCtr);

        // Allow closing tabs with an "x"
        tabCtr->setTabsClosable(true);
        connect(tabCtr, &QTabWidget::currentChanged, this, &IrcClient::resTab);
        connect(tabCtr, &QTabWidget::tabCloseRequested, this, &IrcClient::closeTab);

        netCon = new QTcpSocket(this);
        connect(netCon, &QTcpSocket::readyRead, this, &IrcClient::readDat);

        // store font for later use
        consoleFont = fixedFont;

        makeTab("status");

        // Channel-Liste Tab initial noch nicht erstellt
        channelListTabCreated = false;
        channelTable = nullptr;

        netCon->connectToHost("irc.libera.chat", 6667);
        connect(netCon, &QTcpSocket::connected, this, [this]() {
            netCon->write("NICK joscha\r\n");
            netCon->write("USER joscha 0 * :joscha\r\n");
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
            netCon->write(("JOIN " + strChn + "\r\n").toUtf8());
            makeTab(strChn);
            tabCtr->setCurrentIndex(tabCtr->count() - 1);
        } else if (strTxt.startsWith("/msg ")) {
            int idxSpc = strTxt.indexOf(' ', 5);
            if (idxSpc != -1) {
                QString strUsr = strTxt.mid(5, idxSpc - 5).trimmed();
                QString strMsg = strTxt.mid(idxSpc + 1);
                netCon->write(("PRIVMSG " + strUsr + " :" + strMsg + "\r\n").toUtf8());
                makeTab(strUsr);
                showTxt(strUsr, "<joscha> " + strMsg);
                tabCtr->setCurrentIndex(tabCtr->count() - 1);
            }
        } else {
            if (strNam != "status") {
                netCon->write(("PRIVMSG " + strNam + " :" + strTxt + "\r\n").toUtf8());
                showTxt(strNam, "<joscha> " + strTxt);
            } else {
                netCon->write((strTxt + "\r\n").toUtf8());
            }
        }
    }

    void opnQry(QListWidgetItem *itmUsr) {
        QString strUsr = itmUsr->text();
        if (strUsr.startsWith("@") || strUsr.startsWith("+")) {
            strUsr = strUsr.mid(1);
        }
        makeTab(strUsr);
        tabCtr->setCurrentIndex(tabCtr->count() - 1);
    }

    void resTab(int idxTab) {
        if (idxTab >= 0 && idxTab < tabCtr->count())
            tabCtr->tabBar()->setTabTextColor(idxTab, Qt::black);
    }

    // Slot für Doppelklick in der Channel-Tabelle
    void onChannelTableCellDoubleClicked(int row, int /*column*/) {
        if (!channelTable) return;
        if (row < 0 || row >= channelTable->rowCount()) return;
        QTableWidgetItem *item = channelTable->item(row, 0);
        if (!item) return;
        QString chn = item->text().trimmed();
        if (chn.isEmpty()) return;

        // Nur Channels mit führendem '#' joinen
        if (!chn.startsWith("#")) return;

        // JOIN senden und Tab anlegen/öffnen
        netCon->write(("JOIN " + chn + "\r\n").toUtf8());
        makeTab(chn);

        // Wechsel zum Channel-Tab
        for (int i = 0; i < tabCtr->count(); ++i) {
            if (tabCtr->tabText(i) == chn) { tabCtr->setCurrentIndex(i); break; }
        }
    }

    // Slot zum Schließen eines Tabs (per "x")
    void closeTab(int index) {
        if (index < 0 || index >= tabCtr->count()) return;

        QString tabName = tabCtr->tabText(index);

        // Verhindere das Schließen des Status-Tabs (optional)
        if (tabName == "status") return;

        // Wenn Channel List Tab geschlossen wird, aufräumen
        if (tabName == "Channel List") {
            QWidget *w = tabCtr->widget(index);
            tabCtr->removeTab(index);
            if (w) {
                delete w;
            }
            channelListTabCreated = false;
            channelTable = nullptr;
            return;
        }

        // Entferne Einträge aus mapTxt und mapLst
        if (mapTxt.contains(tabName)) {
            mapTxt.remove(tabName);
        }
        if (mapLst.contains(tabName)) {
            mapLst.remove(tabName);
        }

        // Entferne Tracking-Sets, falls vorhanden
        channelsToQuery.remove(tabName);
        channelsNamesReceived.remove(tabName);
        channelUserCount.remove(tabName);
        channelTopic.remove(tabName);

        // Entferne und lösche das Tab-Widget
        QWidget *w = tabCtr->widget(index);
        tabCtr->removeTab(index);
        if (w) {
            delete w;
        }
    }

private:
    QTcpSocket *netCon;
    QTabWidget *tabCtr;
    QMap<QString, QTextEdit*> mapTxt;
    QMap<QString, QListWidget*> mapLst;

    // für LIST-Auswertung
    QMap<QString,int> channelUserCount;    // channel -> user count (aus 322)
    QMap<QString,QString> channelTopic;    // channel -> topic (aus 322)
    QSet<QString> channelsToQuery;         // channels, die wir erwarten (bei makeTab hinzugefügt)
    QSet<QString> channelsNamesReceived;   // channels, für die wir 353 erhalten haben

    // Channel-Liste UI
    bool channelListTabCreated;
    QTableWidget *channelTable;

    // stored console font
    QFont consoleFont;

    void makeTab(const QString &strNam) {
        if (mapTxt.contains(strNam)) return;

        QWidget *widPag = new QWidget();
        QVBoxLayout *boxLay = new QVBoxLayout(widPag);
        QSplitter *splTop = new QSplitter(Qt::Horizontal);

        QTextEdit *txtBox = new QTextEdit();
        txtBox->setReadOnly(true);
        txtBox->setFont(consoleFont);               // apply fixed-width font
        splTop->addWidget(txtBox);

        if (strNam.startsWith("#")) {
            QListWidget *lstUsr = new QListWidget();
            lstUsr->setFixedWidth(120);
            lstUsr->setFont(consoleFont);          // fixed-width for user list
            // aktiviert die automatische Sortierung
            lstUsr->setSortingEnabled(true);
            connect(lstUsr, &QListWidget::itemDoubleClicked, this, &IrcClient::opnQry);
            splTop->addWidget(lstUsr);
            mapLst[strNam] = lstUsr;

            // Tracken, dass wir für diesen Channel NAMES/Join erwarten
            channelsToQuery.insert(strNam);
        }

        boxLay->addWidget(splTop);

        QLineEdit *txtInp = new QLineEdit();
        txtInp->setProperty("tabname", strNam);
        txtInp->setFont(consoleFont);               // fixed-width for input
        connect(txtInp, &QLineEdit::returnPressed, this, &IrcClient::sndDat);
        boxLay->addWidget(txtInp);

        tabCtr->addTab(widPag, strNam);
        mapTxt[strNam] = txtBox;
    }

    void ensureChannelListTab() {
        if (channelListTabCreated) return;

        QWidget *wid = new QWidget();
        QVBoxLayout *lay = new QVBoxLayout(wid);

        channelTable = new QTableWidget();
        channelTable->setColumnCount(3);
        channelTable->setHorizontalHeaderLabels(QStringList() << "Channel" << "Users" << "Topic");
        channelTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        channelTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        channelTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        channelTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        channelTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        channelTable->setSelectionMode(QAbstractItemView::SingleSelection);
        channelTable->setFont(consoleFont);         // fixed-width for table cells

        lay->addWidget(channelTable);
        tabCtr->addTab(wid, "Channel List");
        channelListTabCreated = true;

        // Verbindung: Doppelklick auf eine Zeile -> Kanal betreten
        connect(channelTable, &QTableWidget::cellDoubleClicked, this, &IrcClient::onChannelTableCellDoubleClicked);
    }

    void populateChannelListTab() {
        ensureChannelListTab();

        // Sammle Paare und sortiere nach Anzahl absteigend
        QList<QPair<QString,int>> list;
        for (auto it = channelUserCount.constBegin(); it != channelUserCount.constEnd(); ++it) {
            list.append(qMakePair(it.key(), it.value()));
        }
        std::sort(list.begin(), list.end(), [](const QPair<QString,int> &a, const QPair<QString,int> &b){
            return a.second > b.second; // absteigend
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

        // Optional: Tab hervorheben / auswählen
        int idx = tabCtr->indexOf(channelTable->parentWidget());
        if (idx != -1) tabCtr->setCurrentIndex(idx);
    }

    void showTxt(const QString &strNam, const QString &strMsg) {
        if (mapTxt.contains(strNam)) {
            mapTxt[strNam]->append(strMsg);

            int idxTab = -1;
            for (int i = 0; i < tabCtr->count(); ++i) {
                if (tabCtr->tabText(i) == strNam) {
                    idxTab = i;
                    break;
                }
            }

            if (idxTab != -1 && idxTab != tabCtr->currentIndex()) {
                tabCtr->tabBar()->setTabTextColor(idxTab, Qt::red);
            }
        }
    }

    void parLin(const QString &strLin) {
        showTxt("status", strLin);

        // PING/PONG
        if (strLin.startsWith("PING ")) {
            QString strPng = strLin;
            strPng.replace(0, 4, "PONG");
            netCon->write((strPng + "\r\n").toUtf8());
            return;
        }

        // Nach erfolgreichem Login automatisch #testchannel joinen
        if (strLin.contains(" 001 joscha")) {
            netCon->write("JOIN #testchannel\r\n");
            makeTab("#testchannel");
        }

        // Nick des Senders extrahieren
        QString strNck = "";
        if (strLin.startsWith(":")) {
            int idxS1 = strLin.indexOf(' ');
            if (idxS1 != -1) {
                QString strFul = strLin.mid(1, idxS1 - 1);
                strNck = strFul.split('!').first();
            }
        }

        // --- 322: RPL_LIST (Format: ":server 322 nick #channel users :topic")
        if (strLin.contains(" 322 ")) {
            int idxHash = strLin.indexOf('#');
            if (idxHash != -1) {
                // channel name
                int idxSpaceAfterChannel = strLin.indexOf(' ', idxHash);
                if (idxSpaceAfterChannel != -1) {
                    QString chn = strLin.mid(idxHash, idxSpaceAfterChannel - idxHash).trimmed();

                    // user count steht nach channel, vor dem nächsten ':'
                    int idxAfterChannel = idxSpaceAfterChannel + 1;
                    int idxSpaceAfterCount = strLin.indexOf(' ', idxAfterChannel);
                    if (idxSpaceAfterCount != -1) {
                        QString countStr = strLin.mid(idxAfterChannel, idxSpaceAfterCount - idxAfterChannel).trimmed();
                        bool ok = false;
                        int count = countStr.toInt(&ok);
                        if (ok) {
                            channelUserCount[chn] = count;
                        }

                        // topic ist alles nach dem ':' nach der count
                        int idxColon = strLin.indexOf(':', idxSpaceAfterCount);
                        if (idxColon != -1) {
                            QString topic = strLin.mid(idxColon + 1).trimmed();
                            channelTopic[chn] = topic;
                        } else {
                            channelTopic[chn] = QString();
                        }
                    }
                }
            }
        }

        // --- 323: End of LIST -> sortieren und anzeigen
        if (strLin.contains(" 323 ")) {
            // populate table und anzeigen
            populateChannelListTab();

            // optional: clear collected data, falls später erneut benötigt
            channelUserCount.clear();
            channelTopic.clear();
        }

        // --- NAMES reply (353) Verarbeitung
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

                    // Markiere, dass wir für diesen Channel NAMES erhalten haben
                    channelsNamesReceived.insert(strChn);

                    // wenn wir für alle erwarteten Channels NAMES erhalten haben -> LIST anfordern
                    bool allReceived = true;
                    for (const QString &c : channelsToQuery) {
                        if (!channelsNamesReceived.contains(c)) { allReceived = false; break; }
                    }
                    if (allReceived && !channelsToQuery.isEmpty()) {
                        // Liste vom Server anfordern
                        netCon->write("LIST\r\n");

                        // wir können die Sets zurücksetzen, damit bei späteren joins erneut getriggert werden kann
                        channelsToQuery.clear();
                        channelsNamesReceived.clear();
                    }
                }
            }
        }

        // JOIN Verarbeitung (Benutzer join)
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
