#include <QApplication>
#include <QMainWindow>
#include <QTcpSocket>
#include <QTabWidget>
#include <QTabBar>
#include <QTextEdit>
#include <QLineEdit>
#include <QMap>
#include <QListWidget>
#include <QVBoxLayout>
#include <QSplitter>
#include <QStringList>

class IrcClient : public QMainWindow {
    Q_OBJECT

public:
    IrcClient(QWidget *parWid = nullptr) : QMainWindow(parWid) {
        tabMain = new QTabWidget(this);
        setCentralWidget(tabMain);
        resize(800, 600);

        connect(tabMain, &QTabWidget::currentChanged, this, &IrcClient::resetTab);

        netCon = new QTcpSocket(this);
        connect(netCon, &QTcpSocket::readyRead, this, &IrcClient::readData);

        makeTab("status");

        // verbindung zu libera.chat
        netCon->connectToHost("irc.libera.chat", 6667);
        connect(netCon, &QTcpSocket::connected, this, [this]() {
            netCon->write("NICK joscha\r\n");
            netCon->write("USER joscha 0 * :joscha\r\n");
        });
    }

private slots:
    void readData() {
        while (netCon->canReadLine()) {
            QString strLine = QString::fromUtf8(netCon->readLine()).trimmed();
            parseLine(strLine);
        }
    }

    void sendData() {
        QLineEdit *txtInp = qobject_cast<QLineEdit*>(sender());
        if (!txtInp) return;

        QString strTxt = txtInp->text();
        QString strNam = txtInp->property("tabName").toString();
        txtInp->clear();

        if (strTxt.startsWith("/join ")) {
            QString strChn = strTxt.mid(6).trimmed();
            netCon->write("JOIN " + strChn.toUtf8() + "\r\n");
            makeTab(strChn);
            tabMain->setCurrentIndex(tabMain->count() - 1);
        } else if (strTxt.startsWith("/msg ")) {
            int idxSpc = strTxt.indexOf(' ', 5);
            if (idxSpc != -1) {
                QString strUsr = strTxt.mid(5, idxSpc - 5).trimmed();
                QString strMsg = strTxt.mid(idxSpc + 1);
                netCon->write("PRIVMSG " + strUsr.toUtf8() + " :" + strMsg.toUtf8() + "\r\n");
                makeTab(strUsr);
                showText(strUsr, "<joscha> " + strMsg);
                tabMain->setCurrentIndex(tabMain->count() - 1);
            }
        } else {
            if (strNam != "status") {
                netCon->write("PRIVMSG " + strNam.toUtf8() + " :" + strTxt.toUtf8() + "\r\n");
                showText(strNam, "<joscha> " + strTxt);
            } else {
                netCon->write(strTxt.toUtf8() + "\r\n");
            }
        }
    }

    void openQry(QListWidgetItem *itmUsr) {
        QString strUsr = itmUsr->text();
        if (strUsr.startsWith("@") || strUsr.startsWith("+")) {
            strUsr = strUsr.mid(1);
        }
        makeTab(strUsr);
        tabMain->setCurrentIndex(tabMain->count() - 1);
    }

    void resetTab(int idxTab) {
        tabMain->tabBar()->setTabTextColor(idxTab, Qt::black);
    }

private:
    QTcpSocket *netCon;
    QTabWidget *tabMain;
    QMap<QString, QTextEdit*> mapTxt;
    QMap<QString, QListWidget*> mapLst;
    QMap<QString, int> mapIdx;

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
            connect(lstUsr, &QListWidget::itemDoubleClicked, this, &IrcClient::openQry);
            splTop->addWidget(lstUsr);
            mapLst[strNam] = lstUsr;
        }

        boxLay->addWidget(splTop);

        QLineEdit *txtInp = new QLineEdit();
        txtInp->setProperty("tabName", strNam);
        connect(txtInp, &QLineEdit::returnPressed, this, &IrcClient::sendData);
        boxLay->addWidget(txtInp);

        int idxNew = tabMain->addTab(widPag, strNam);
        mapTxt[strNam] = txtBox;
        mapIdx[strNam] = idxNew;
    }

    void showText(const QString &strNam, const QString &strMsg) {
        if (mapTxt.contains(strNam)) {
            mapTxt[strNam]->append(strMsg);

            int idxTab = -1;
            for(int i = 0; i < tabMain->count(); ++i) {
                if(tabMain->tabText(i) == strNam) {
                    idxTab = i;
                    break;
                }
            }

            if (idxTab != -1 && idxTab != tabMain->currentIndex()) {
                tabMain->tabBar()->setTabTextColor(idxTab, Qt::red);
            }
        }
    }

    void parseLine(const QString &strLin) {
        showText("status", strLin);

        if (strLin.startsWith("PING ")) {
            QString strPng = strLin;
            strPng.replace(0, 4, "PONG");
            netCon->write(strPng.toUtf8() + "\r\n");
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
                    mapLst[strChn]->addItem(strNck);
                }
                showText(strChn, "*** " + strNck + " joined " + strChn);
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
                showText(strTab, "<" + strNck + "> " + strTxt);
            }
        }
        else if (strLin.contains(" 353 ")) {
            int idxHsh = strLin.indexOf('#');
            if (idxHsh != -1) {
                int idxCol = strLin.indexOf(':', idxHsh);
                if (idxCol != -1) {
                    QString strChn = strLin.mid(idxHsh, idxCol - idxHsh - 1).trimmed();
                    QString strNam = strLin.mid(idxCol + 1);
                    if (mapLst.contains(strChn)) {
                        QStringList lstNam = strNam.split(' ', Qt::SkipEmptyParts);
                        for (const QString &strU : lstNam) {
                            if (mapLst[strChn]->findItems(strU, Qt::MatchExactly).isEmpty()) {
                                mapLst[strChn]->addItem(strU);
                            }
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
    winApp.show();
    return app.exec();
}

#include "qirc1.moc"
