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

class IrcClient : public QMainWindow {
    Q_OBJECT

public:
    IrcClient(QWidget *parent = nullptr) : QMainWindow(parent) {
        tabCtr = new QTabWidget(this);
        setCentralWidget(tabCtr);

        connect(tabCtr, &QTabWidget::currentChanged, this, &IrcClient::resTab);

        netCon = new QTcpSocket(this);
        connect(netCon, &QTcpSocket::readyRead, this, &IrcClient::readDat);

        makeTab("status");

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
        tabCtr->tabBar()->setTabTextColor(idxTab, Qt::black);
    }

private:
    QTcpSocket *netCon;
    QTabWidget *tabCtr;
    QMap<QString, QTextEdit*> mapTxt;
    QMap<QString, QListWidget*> mapLst;

    void makeTab(const QString &strNam) {
        if (mapTxt.contains(strNam)) return;

        QWidget *widPag = new QWidget();
        QVBoxLayout *boxLay = new QVBoxLayout(widPag);
        QSplitter *splTop = new QSplitter(Qt::Horizontal);

        QTextEdit *txtBox = new QTextEdit();
        txtBox->setReadOnly(true);
        splTop->addWidget(txtBox);

        if (strNam.startsWith("#")) {
            QListWidget *lstUsr = new QListWidget();
            lstUsr->setFixedWidth(120);
            // aktiviert die automatische Sortierung
            lstUsr->setSortingEnabled(true);
            connect(lstUsr, &QListWidget::itemDoubleClicked, this, &IrcClient::opnQry);
            splTop->addWidget(lstUsr);
            mapLst[strNam] = lstUsr;
        }

        boxLay->addWidget(splTop);

        QLineEdit *txtInp = new QLineEdit();
        txtInp->setProperty("tabname", strNam);
        connect(txtInp, &QLineEdit::returnPressed, this, &IrcClient::sndDat);
        boxLay->addWidget(txtInp);

        tabCtr->addTab(widPag, strNam);
        mapTxt[strNam] = txtBox;
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

        if (strLin.startsWith("PING ")) {
            QString strPng = strLin;
            strPng.replace(0, 4, "PONG");
            netCon->write((strPng + "\r\n").toUtf8());
            return;
        }

        if (strLin.contains(" 001 joscha")) {
            netCon->write("JOIN #testchannel\r\n");
            makeTab("#testchannel");
        }

        QString strNck = "";
        if (strLin.startsWith(":")) {
            int idxS1 = strLin.indexOf(' ');
            if (idxS1 != -1) {
                QString strFul = strLin.mid(1, idxS1 - 1);
                strNck = strFul.split('!').first();
            }
        }

        if (strLin.contains(" JOIN ")) {
            int idxHsh = strLin.indexOf('#');
            if (idxHsh != -1) {
                QString strChn = strLin.mid(idxHsh).trimmed();
                if (strChn.startsWith(":")) strChn = strChn.mid(1);

                if (mapLst.contains(strChn)) {
                    QList<QListWidgetItem*> lstFnd = mapLst[strChn]->findItems(strNck, Qt::MatchEndsWith);
                    bool bExists = false;
                    for (auto *itmTst : lstFnd) {
                        if (itmTst->text() == strNck || itmTst->text() == "@" + strNck || itmTst->text() == "+" + strNck) {
                            bExists = true; break;
                        }
                    }
                    if (!bExists) mapLst[strChn]->addItem(strNck);
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
        else if (strLin.contains(" 353 ")) {
            int idxHsh = strLin.indexOf('#');
            if (idxHsh != -1) {
                int idxCol = strLin.indexOf(':', idxHsh);
                if (idxCol != -1) {
                    QString strChn = strLin.mid(idxHsh, idxCol - idxHsh - 1).trimmed();
                    QString strRaw = strLin.mid(idxCol + 1);
                    if (mapLst.contains(strChn)) {
                        QStringList lstNam = strRaw.split(' ', Qt::SkipEmptyParts);
                        for (const QString &strNam : lstNam) {
                            QString strBas = strNam;
                            if (strBas.startsWith('@') || strBas.startsWith('+')) strBas = strBas.mid(1);

                            QList<QListWidgetItem*> lstOld = mapLst[strChn]->findItems(strBas, Qt::MatchEndsWith);
                            for (auto *itmRem : lstOld) {
                                QString strTst = itmRem->text();
                                if (strTst == strBas || strTst == "@" + strBas || strTst == "+" + strBas) {
                                    delete mapLst[strChn]->takeItem(mapLst[strChn]->row(itmRem));
                                }
                            }
                            mapLst[strChn]->addItem(strNam);
                        }
                    }
                }
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
