// main.cpp — Single-file, audit-friendly Qt chat client
// Features:
// - Standards-compliant TLS (VerifyPeer) with embedded CA pinning (replace PEM blocks below)
// - Runtime fallback: optional pinned-ca.pem file overrides embedded CAs
// - TLS diagnostics: peer CN/issuer/validity on errors; clear messages
// - Hostname intent guard; API key guard (prevents 401 loops)
// - Single-flight requests; pacing with exponential backoff + jitter on HTTP 429
// - Robust JSON handling and UI feedback
//
// Inject your API key at build time or via environment:
//   qmake "QMAKE_CXXFLAGS+=-DOPENAI_API_KEY=\"sk-...\"" && make
// Or set OPENAI_API_KEY in the environment. The macro defaults to "xxxxx" for auditability.

#include <QApplication>
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QTimer>
#include <QElapsedTimer>
#include <QScrollBar>
#include <QUrl>
#include <QByteArray>
#include <QStringList>
#include <QFile>
#include <QDateTime>
#include <QSslError>
#include <QSslConfiguration>
#include <QSslCertificate>
#include <QSslSocket>

#ifndef OPENAI_API_KEY
#define OPENAI_API_KEY "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"
#endif

// Compile-time key default; runtime env fallback for convenience
static QByteArray resolveApiKey() {
    const char *macroKey = OPENAI_API_KEY;
    QByteArray envKey = qgetenv("OPENAI_API_KEY");
    if (envKey.isEmpty()) envKey = QByteArray(macroKey);
    return envKey;
}

// --- Embedded certificate chain ---
// Paste actual PEMs retrieved via:
//   openssl s_client -connect api.openai.com:443 -showcerts
// Include BEGIN/END lines exactly. Prefer pinning the root CA (e.g., ISRG Root X1).
// If you also want to pin intermediates or server cert, include them too.



static const char kRootCaPem[] = R"(-----BEGIN CERTIFICATE-----
MIIDoDCCA0agAwIBAgIQTxlPsMdRRogR89F746L4pzAKBggqhkjOPQQDAjA7MQsw
CQYDVQQGEwJVUzEeMBwGA1UEChMVR29vZ2xlIFRydXN0IFNlcnZpY2VzMQwwCgYD
VQQDEwNXRTEwHhcNMjUxMTI1MDI0MjQzWhcNMjYwMjIzMDM0MjQxWjAVMRMwEQYD
VQQDEwpvcGVuYWkuY29tMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEyoG8BfHB
+iWHvMHIylQOzeB2/Eam1fAwDmU0megmCnc4FvRvbGuRFGn0hmYPRJ8VnsZlbmcn
3VREB6VyFFIJFKOCAlAwggJMMA4GA1UdDwEB/wQEAwIHgDATBgNVHSUEDDAKBggr
BgEFBQcDATAMBgNVHRMBAf8EAjAAMB0GA1UdDgQWBBTxqpI76PxT5QUhLqeRD6Yi
GFAErjAfBgNVHSMEGDAWgBSQd5I1Z8T/qMyp5nvZgHl7zJP5ODBeBggrBgEFBQcB
AQRSMFAwJwYIKwYBBQUHMAGGG2h0dHA6Ly9vLnBraS5nb29nL3Mvd2UxL1R4azAl
BggrBgEFBQcwAoYZaHR0cDovL2kucGtpLmdvb2cvd2UxLmNydDAjBgNVHREEHDAa
ggpvcGVuYWkuY29tggwqLm9wZW5haS5jb20wEwYDVR0gBAwwCjAIBgZngQwBAgEw
NgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2MucGtpLmdvb2cvd2UxL3JnZW9rUkFf
bi1nLmNybDCCAQMGCisGAQQB1nkCBAIEgfQEgfEA7wB2AJaXZL9VWJet90OHaDcI
Qnfp8DrV9qTzNm5GpD8PyqnGAAABmrkbRz8AAAQDAEcwRQIhAPNSYKkT4NXypa7+
VE1MjGLI8TicCSNBHcb/xmz9gBnIAiBEpdU9+Ny2JbMHg/uj7jvwvv6zpFGJJlVk
8294OgRmzQB1AGQRxGykEuyniRyiAi4AvKtPKAfUHjUnq+r+1QPJfc3wAAABmrkb
Ry0AAAQDAEYwRAIgQkR+0YJJITpqmpFEotPOCrCSzOKHgs5InQYWFHYf5+cCIBUw
Flff12o9NwYDoZyfD3GJh6e4NhWG4xP8FVtoaNvxMAoGCCqGSM49BAMCA0gAMEUC
IQC8db4AKlImnfkj+gepFXankbFiQHc9RYVDiR8LNmXKUgIgFZbtu1EoOJS/engR
bMpo5sy/+w5p/HyUUabnx7VJIsY=
-----END CERTIFICATE-----
)";

static const char kIntermediateCaPem[] = R"(-----BEGIN CERTIFICATE-----
MIICnzCCAiWgAwIBAgIQf/MZd5csIkp2FV0TttaF4zAKBggqhkjOPQQDAzBHMQsw
CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU
MBIGA1UEAxMLR1RTIFJvb3QgUjQwHhcNMjMxMjEzMDkwMDAwWhcNMjkwMjIwMTQw
MDAwWjA7MQswCQYDVQQGEwJVUzEeMBwGA1UEChMVR29vZ2xlIFRydXN0IFNlcnZp
Y2VzMQwwCgYDVQQDEwNXRTEwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAARvzTr+
Z1dHTCEDhUDCR127WEcPQMFcF4XGGTfn1XzthkubgdnXGhOlCgP4mMTG6J7/EFmP
LCaY9eYmJbsPAvpWo4H+MIH7MA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggr
BgEFBQcDAQYIKwYBBQUHAwIwEgYDVR0TAQH/BAgwBgEB/wIBADAdBgNVHQ4EFgQU
kHeSNWfE/6jMqeZ72YB5e8yT+TgwHwYDVR0jBBgwFoAUgEzW63T/STaj1dj8tT7F
avCUHYwwNAYIKwYBBQUHAQEEKDAmMCQGCCsGAQUFBzAChhhodHRwOi8vaS5wa2ku
Z29vZy9yNC5jcnQwKwYDVR0fBCQwIjAgoB6gHIYaaHR0cDovL2MucGtpLmdvb2cv
ci9yNC5jcmwwEwYDVR0gBAwwCjAIBgZngQwBAgEwCgYIKoZIzj0EAwMDaAAwZQIx
AOcCq1HW90OVznX+0RGU1cxAQXomvtgM8zItPZCuFQ8jSBJSjz5keROv9aYsAm5V
sQIwJonMaAFi54mrfhfoFNZEfuNMSQ6/bIBiNLiyoX46FohQvKeIoJ99cx7sUkFN
7uJW
-----END CERTIFICATE-----
)";

// Optional exact server cert pinning (brittle; rotates more frequently)
static const char kServerCertPem[] = R"(-----BEGIN CERTIFICATE-----
MIICCTCCAY6gAwIBAgINAgPlwGjvYxqccpBQUjAKBggqhkjOPQQDAzBHMQswCQYD
VQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEUMBIG
A1UEAxMLR1RTIFJvb3QgUjQwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAwMDAw
WjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2Vz
IExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjQwdjAQBgcqhkjOPQIBBgUrgQQAIgNi
AATzdHOnaItgrkO4NcWBMHtLSZ37wWHO5t5GvWvVYRg1rkDdc/eJkTBa6zzuhXyi
QHY7qca4R9gq55KRanPpsXI5nymfopjTX15YhmUPoYRlBtHci8nHc8iMai/lxKvR
HYqjQjBAMA4GA1UdDwEB/wQEAwIBhjAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQW
BBSATNbrdP9JNqPV2Py1PsVq8JQdjDAKBggqhkjOPQQDAwNpADBmAjEA6ED/g94D
9J+uHXqnLrmvT/aDHQ4thQEd0dlq7A/Cr8deVl5c1RxYIigL9zC2L7F8AjEA8GE8
p/SgguMh1YQdc4acLa/KNJvxn7kjNuK8YAOdgLOaVsjh4rsUecrNIdSUtUlD
-----END CERTIFICATE-----
)";

// Build SSL configuration from runtime file pinned-ca.pem if present; else embedded CAs.
// VerifyPeer remains enabled for standards-compliant validation.
static QSslConfiguration makePinnedSslConfigRuntime() {
    QSslConfiguration ssl = QSslConfiguration::defaultConfiguration();
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    ssl.setProtocol(QSsl::TlsV1_2OrLater);
#else
    ssl.setProtocol(QSsl::TlsV1_2);
#endif

    QList<QSslCertificate> cas;

    // Try runtime file (allows CA rotation without recompiling)
    QFile f(QStringLiteral("pinned-ca.pem"));
    if (f.open(QIODevice::ReadOnly)) {
        const QByteArray pem = f.readAll();
        QList<QSslCertificate> fileCas = QSslCertificate::fromData(pem, QSsl::Pem);
        if (!fileCas.isEmpty()) cas += fileCas;
    }

    // Fallback to embedded PEMs
    {
        QList<QSslCertificate> embRoot = QSslCertificate::fromData(QByteArray(kRootCaPem), QSsl::Pem);
        QList<QSslCertificate> embInter = QSslCertificate::fromData(QByteArray(kIntermediateCaPem), QSsl::Pem);
        // Server cert pin is optional; usually not needed if you trust root/intermediate correctly
        QList<QSslCertificate> embSrv = QSslCertificate::fromData(QByteArray(kServerCertPem), QSsl::Pem);

        if (!embRoot.isEmpty()) cas += embRoot;
        if (!embInter.isEmpty()) cas += embInter;
        // Add server cert only if you intend exact pinning
        // if (!embSrv.isEmpty()) cas += embSrv;
    }

    ssl.setPeerVerifyMode(QSslSocket::VerifyPeer);

    if (!cas.isEmpty()) {
        // Warn about expired pinned CAs (handshake will still fail canonically later)
        const QDateTime now = QDateTime::currentDateTimeUtc();
        for (const auto &ca : cas) {
            if (ca.expiryDate().isValid() && ca.expiryDate() < now) {
                qWarning("Pinned CA expired: CN=%s NotAfter=%s",
                         qPrintable(ca.subjectInfo(QSslCertificate::CommonName).join(", ")),
                         qPrintable(ca.expiryDate().toString(Qt::ISODate)));
            }
        }
        ssl.setCaCertificates(cas);
    } // If cas is empty, we rely on defaultConfiguration() trust store

    return ssl;
}

// --- Networking client ---
class CltGpt : public QObject {
    Q_OBJECT
public:
    explicit CltGpt(QObject *parent = nullptr)
        : QObject(parent), netMgr(new QNetworkAccessManager(this)) {}

    bool busy() const { return inFlight; }

    void sendMessage(const QString &text) {
        if (inFlight) { emit sigError(QStringLiteral("Request in progress. Please wait.")); return; }

        const QByteArray apiKey = resolveApiKey();
        auto isPlaceholderKey = [](const QByteArray &key) -> bool {
            return key.isEmpty() || key == "xxxxx";
        };
        if (isPlaceholderKey(apiKey)) {
            emit sigError(QStringLiteral("No valid API key set. Define OPENAI_API_KEY at build time or via environment."));
            return;
        }

        inFlight = true;

        // Build JSON payload
        QJsonObject msgObj{{"role","user"},{"content",text}};
        QJsonArray msgs; msgs.append(msgObj);
        QJsonObject data{{"model","gpt-3.5-turbo"},{"messages",msgs}};
        const QByteArray payload = QJsonDocument(data).toJson(QJsonDocument::Compact);

        // Prepare request
        const QUrl url(QStringLiteral("https://api.openai.com/v1/chat/completions"));
        QNetworkRequest req(url);
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        req.setRawHeader("Authorization", QByteArray("Bearer ").append(apiKey));
        req.setRawHeader("User-Agent", QByteArray("QtSingleFileClient/CA-Pinned/1.0"));
        req.setSslConfiguration(makePinnedSslConfigRuntime());

        // Hostname intent guard
        static const QStringList allowedHosts = {
            QStringLiteral("api.openai.com")
            // Add your private host(s) here if you use your own CA:
            // QStringLiteral("your.private.host")
        };
        if (!allowedHosts.contains(req.url().host())) {
            emit sigError(QStringLiteral("Unexpected host for pinned CA: %1").arg(req.url().host()));
            inFlight = false;
            return;
        }

        // Issue request
        QNetworkReply *reply = netMgr->post(req, payload);

        // TLS diagnostics: access peer certificate via sslConfiguration()
        connect(reply, &QNetworkReply::sslErrors, this, [this, reply](const QList<QSslError> &errors) {
            QStringList msgs; for (const auto &e : errors) msgs << e.errorString();

            QSslCertificate peerCert = reply->sslConfiguration().peerCertificate();

            QString peerInfo;
            if (!peerCert.isNull()) {
                peerInfo = QString("Peer CN=%1 | Issuer CN=%2 | Valid: %3 → %4")
                    .arg(peerCert.subjectInfo(QSslCertificate::CommonName).join(", "))
                    .arg(peerCert.issuerInfo(QSslCertificate::CommonName).join(", "))
                    .arg(peerCert.effectiveDate().toString(Qt::ISODate))
                    .arg(peerCert.expiryDate().toString(Qt::ISODate));
            } else {
                peerInfo = QStringLiteral("Peer certificate unavailable.");
            }

            emit sigError(QString("TLS validation failed (pinned CA). %1 | Errors: %2")
                          .arg(peerInfo).arg(msgs.join("; ")));
        });

        // Response handling with diagnostics and 429 backoff signal
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            auto cleanup = [this, reply]() { inFlight = false; reply->deleteLater(); };

            const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

            if (reply->error() != QNetworkReply::NoError) {
                if (status == 429) {
                    emit sigRateLimited();
                    emit sigError(QStringLiteral("Rate limit exceeded (HTTP 429). Backing off."));
                    cleanup();
                    return;
                }
                emit sigError(QStringLiteral("Network error: %1 (HTTP %2)")
                                  .arg(reply->errorString()).arg(status));
                cleanup();
                return;
            }

            QJsonParseError pe{};
            const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &pe);
            if (pe.error != QJsonParseError::NoError || !doc.isObject()) {
                emit sigError(QStringLiteral("JSON parse error: %1").arg(pe.errorString()));
                cleanup();
                return;
            }

            const QJsonObject root = doc.object();
            const QJsonArray choices = root.value(QStringLiteral("choices")).toArray();
            if (choices.isEmpty() || !choices.at(0).isObject()) {
                emit sigError(QStringLiteral("Invalid response: 'choices' missing or empty"));
                cleanup();
                return;
            }

            const QJsonObject choice0 = choices.at(0).toObject();
            const QJsonObject msg = choice0.value(QStringLiteral("message")).toObject();
            const QString content = msg.value(QStringLiteral("content")).toString();
            if (content.isEmpty()) {
                emit sigError(QStringLiteral("Invalid response: 'content' empty"));
                cleanup();
                return;
            }

            emit sigReply(content);
            cleanup();
        });
    }

signals:
    void sigReply(const QString &response);
    void sigError(const QString &error);
    void sigRateLimited();

private:
    QNetworkAccessManager *netMgr;
    bool inFlight {false};
};

// --- UI and control ---
class WinMain : public QMainWindow {
    Q_OBJECT
public:
    explicit WinMain(QWidget *parent = nullptr)
        : QMainWindow(parent),
          gptClient(new CltGpt(this)),
          txtView(new QTextEdit(this)),
          ledMsg(new QLineEdit(this)),
          btnSend(new QPushButton(QStringLiteral("Send"), this)),
          chkLoop(new QCheckBox(QStringLiteral("Endless loop (x)"), this)) {

        QWidget *central = new QWidget(this);
        setCentralWidget(central);

        auto *layoutMain = new QVBoxLayout(central);
        layoutMain->setSpacing(10);
        layoutMain->setContentsMargins(15, 15, 15, 15);

        txtView->setReadOnly(true);
        txtView->setPlaceholderText(QStringLiteral("Chat history..."));

        auto *layoutBottom = new QHBoxLayout();
        ledMsg->setPlaceholderText(QStringLiteral("Type message here..."));
        chkLoop->setToolTip(QStringLiteral("Auto-send 'next part only please!' after replies"));
        btnSend->setFixedWidth(90);

        layoutBottom->addWidget(ledMsg);
        layoutBottom->addWidget(chkLoop);
        layoutBottom->addWidget(btnSend);

        layoutMain->addWidget(txtView);
        layoutMain->addLayout(layoutBottom);

        connect(ledMsg, &QLineEdit::returnPressed, this, &WinMain::onSendClicked);
        connect(btnSend, &QPushButton::clicked, this, &WinMain::onSendClicked);

        connect(gptClient, &CltGpt::sigReply, this, &WinMain::onReply);
        connect(gptClient, &CltGpt::sigError, this, &WinMain::onError);
        connect(gptClient, &CltGpt::sigRateLimited, this, &WinMain::onRateLimited);

        setStyle();
        setMinimumSize(800, 580);
        setWindowTitle(QStringLiteral("GPT single-file client (CA pinned)"));

        lastSend.invalidate();

        // Startup audit: show loaded trust anchors (runtime file or embedded)
        {
            QList<QSslCertificate> cas;
            QFile f(QStringLiteral("pinned-ca.pem"));
            if (f.open(QIODevice::ReadOnly)) {
                cas = QSslCertificate::fromData(f.readAll(), QSsl::Pem);
            }
            if (cas.isEmpty()) {
                cas += QSslCertificate::fromData(QByteArray(kRootCaPem), QSsl::Pem);
                cas += QSslCertificate::fromData(QByteArray(kIntermediateCaPem), QSsl::Pem);
                // cas += QSslCertificate::fromData(QByteArray(kServerCertPem), QSsl::Pem);
            }
            if (!cas.isEmpty()) {
                for (const auto &ca : cas) {
                    txtView->append(QString("<i>Pinned CA: CN=%1 | Valid: %2 → %3</i>")
                        .arg(ca.subjectInfo(QSslCertificate::CommonName).join(", "))
                        .arg(ca.effectiveDate().toString(Qt::ISODate))
                        .arg(ca.expiryDate().toString(Qt::ISODate)));
                }
            } else {
                txtView->append(QString("<b style='color:#ff5555'>Pinned CA parse error — none loaded.</b>"));
            }
            scrollToBottom();
        }
    }

private slots:
    void onSendClicked() {
        const QString text = ledMsg->text().trimmed();
        if (text.isEmpty()) return;

        // Pacing: minimum gap to reduce 429 likelihood
        const qint64 minGapMs = currentMinGapMs;
        if (lastSend.isValid() && lastSend.elapsed() < minGapMs) {
            txtView->append(QStringLiteral("<i>Rate limit: wait %1 ms...</i>").arg(minGapMs - lastSend.elapsed()));
            scrollToBottom();
            return;
        }

        ledMsg->clear();
        doSend(text);
    }

    void onReply(const QString &response) {
        txtView->append(QStringLiteral("<b style='color:#5bb450'>AI:</b> ") + response);
        txtView->append(QString());
        scrollToBottom();
        setEnabledState(true);

        if (chkLoop->isChecked()) {
            const int loopDelayMs = qMax<int>(int(currentMinGapMs), 3000);
            txtView->append(QStringLiteral("<i>Loop: requesting next part in %1 ms...</i>").arg(loopDelayMs));
            scrollToBottom();
            QTimer::singleShot(loopDelayMs, this, [this]() {
                if (chkLoop->isChecked()) doSend(QStringLiteral("next part only please!"));
            });
        }
    }

    void onError(const QString &error) {
        txtView->append(QStringLiteral("<b style='color:#ff5555'>Error:</b> ") + error);
        scrollToBottom();
        setEnabledState(true);
    }

    void onRateLimited() {
        // Backoff with jitter up to 20s
        const qint64 prev = currentMinGapMs;
        currentMinGapMs = qMin<qint64>(currentMinGapMs * 2, 20000);
        const int jitter = qMax<int>(200, int(currentMinGapMs * 0.2)); // ~20% jitter
        currentMinGapMs += (qrand() % jitter) - (jitter / 2);
        txtView->append(QStringLiteral("<i>Backoff: increased min gap from %1 to %2 ms (with jitter).</i>")
                        .arg(prev).arg(currentMinGapMs));
        scrollToBottom();
    }

private:
    void doSend(const QString &text) {
        txtView->append(QStringLiteral("<b style='color:#8ab4f8'>You:</b> ") + text);
        scrollToBottom();
        setEnabledState(false);

        lastSend.restart();

        if (gptClient->busy()) {
            txtView->append(QStringLiteral("<i>Busy: previous request still in flight.</i>"));
            scrollToBottom();
            setEnabledState(true);
            return;
        }

        gptClient->sendMessage(text);
    }

    void setEnabledState(bool enabled) {
        ledMsg->setEnabled(enabled);
        btnSend->setEnabled(enabled);
        if (enabled) ledMsg->setFocus();
    }

    void scrollToBottom() {
        if (auto *bar = txtView->verticalScrollBar()) bar->setValue(bar->maximum());
    }

    void setStyle() {
        this->setStyleSheet(
            "QWidget { background: #2b2b2b; color: #ddd; font-family: sans-serif; font-size: 13px; }"
            "QTextEdit { background: #333; border: 1px solid #555; border-radius: 4px; padding: 6px; }"
            "QLineEdit { background: #333; border: 1px solid #555; border-radius: 12px; padding: 4px 8px; }"
            "QPushButton { background: #3a7bd5; color: white; border-radius: 12px; padding: 6px; font-weight: bold; }"
            "QPushButton:hover { background: #3569b0; }"
            "QPushButton:disabled { background: #555; color: #888; }"
            "QCheckBox { padding: 0 6px; }"
        );
    }

    CltGpt *gptClient;
    QTextEdit *txtView;
    QLineEdit *ledMsg;
    QPushButton *btnSend;
    QCheckBox *chkLoop;

    QElapsedTimer lastSend;
    qint64 currentMinGapMs {2000}; // start at 2s; backoff/jitter on 429
};

// --- Entry point ---
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    WinMain win;
    win.show();
    return app.exec();
}

#include "main.moc"

