// main.cpp — Single-file, audit-friendly Qt client with controlled TLS bypass
// Features:
// - Standards-compliant TLS (VerifyPeer) with embedded CA pinning or system trust
// - Explicit, opt-in "Allow insecure TLS" toggle (disabled by default)
// - Optional suppression of TLS validation messages (handshake still enforced unless bypass enabled)
// - TLS diagnostics: peer CN/issuer/validity when not suppressed
// - Hostname intent guard; API key guard (prevents 401 loops)
// - Single-flight requests; pacing with exponential backoff + jitter on HTTP 429
// - Robust JSON handling and simple UI
//
// Build-time API key or environment:
//   qmake "QMAKE_CXXFLAGS+=-DOPENAI_API_KEY=\"sk-...\"" && make
// Or set OPENAI_API_KEY in the environment.

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

// Resolve API key: env takes precedence, else compile-time macro
static QByteArray resolveApiKey() {
    QByteArray envKey = qgetenv("OPENAI_API_KEY");
    if (!envKey.isEmpty()) return envKey;
    return QByteArray(OPENAI_API_KEY);
}

// --- Embedded certificate chain (optional pinning) ---
// Paste valid public root/intermediates for your target host(s) if you want strict pinning.
// If left empty, the client will use the system trust store.

static const char kRootCaPem[] = R"(-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)";

static const char kIntermediateCaPem[] = R"(-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)";

// Optional server cert pinning (brittle; rotates frequently)
static const char kServerCertPem[] = R"(-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)";

// Build SSL configuration: pinned CAs if present, else system trust.
// Optionally disable verification when insecure bypass is enabled.
static QSslConfiguration makeSslConfig(bool allowInsecureBypass, bool &usingPinnedCasOut) {
    QSslConfiguration ssl = QSslConfiguration::defaultConfiguration();
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    ssl.setProtocol(QSsl::TlsV1_2OrLater);
#else
    ssl.setProtocol(QSsl::TlsV1_2);
#endif

    QList<QSslCertificate> cas;

    // Try runtime file (allows rotation without recompiling)
    QFile f(QStringLiteral("pinned-ca.pem"));
    if (f.open(QIODevice::ReadOnly)) {
        QList<QSslCertificate> fileCas = QSslCertificate::fromData(f.readAll(), QSsl::Pem);
        if (!fileCas.isEmpty()) cas += fileCas;
    }

    // Fallback to embedded PEMs if provided
    {
        QList<QSslCertificate> embRoot = QSslCertificate::fromData(QByteArray(kRootCaPem), QSsl::Pem);
        QList<QSslCertificate> embInter = QSslCertificate::fromData(QByteArray(kIntermediateCaPem), QSsl::Pem);
        // QList<QSslCertificate> embSrv = QSslCertificate::fromData(QByteArray(kServerCertPem), QSsl::Pem);
        if (!embRoot.isEmpty()) cas += embRoot;
        if (!embInter.isEmpty()) cas += embInter;
        // if (!embSrv.isEmpty()) cas += embSrv;
    }

    usingPinnedCasOut = !cas.isEmpty();
    if (usingPinnedCasOut) {
        const QDateTime now = QDateTime::currentDateTimeUtc();
        for (const auto &ca : cas) {
            if (ca.expiryDate().isValid() && ca.expiryDate() < now) {
                qWarning("Pinned CA expired: CN=%s NotAfter=%s",
                         qPrintable(ca.subjectInfo(QSslCertificate::CommonName).join(", ")),
                         qPrintable(ca.expiryDate().toString(Qt::ISODate)));
            }
        }
        ssl.setCaCertificates(cas);
    }

    ssl.setPeerVerifyMode(allowInsecureBypass ? QSslSocket::VerifyNone : QSslSocket::VerifyPeer);
    return ssl;
}

// --- Networking client ---
class CltGpt : public QObject {
    Q_OBJECT
public:
    explicit CltGpt(QObject *parent = nullptr)
        : QObject(parent), netMgr(new QNetworkAccessManager(this)) {}

    bool busy() const { return inFlight; }

    void setSuppressTlsWarnings(bool on) { suppressTlsWarnings = on; }
    void setAllowInsecureBypass(bool on) { allowInsecureTlsBypass = on; }

    void sendMessage(const QString &text) {
        if (inFlight) { emit sigError(QStringLiteral("Request in progress. Please wait.")); return; }

        const QByteArray apiKey = resolveApiKey();
        if (apiKey.isEmpty() || apiKey == "xxxxx") {
            emit sigError(QStringLiteral("No valid API key set. Define OPENAI_API_KEY at build time or via environment."));
            return;
        }

        inFlight = true;

        // JSON payload
        QJsonObject msgObj{{"role","user"},{"content",text}};
        QJsonArray msgs; msgs.append(msgObj);
        QJsonObject data{{"model","gpt-3.5-turbo"},{"messages",msgs}};
        const QByteArray payload = QJsonDocument(data).toJson(QJsonDocument::Compact);

        // SSL config
        bool usingPinnedCas = false;
        QSslConfiguration ssl = makeSslConfig(allowInsecureTlsBypass, usingPinnedCas);

        // Request
        QNetworkRequest req(QUrl(QStringLiteral("https://api.openai.com/v1/chat/completions")));
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        req.setRawHeader("Authorization", QByteArray("Bearer ").append(apiKey));
        req.setRawHeader("User-Agent", QByteArray("QtSingleFileClient/CA-Pinned/1.2"));
        req.setSslConfiguration(ssl);

        // Hostname intent guard
        static const QStringList allowedHosts = { QStringLiteral("api.openai.com") };
        if (!allowedHosts.contains(req.url().host())) {
            emit sigError(QStringLiteral("Unexpected host: %1").arg(req.url().host()));
            inFlight = false;
            return;
        }

        QNetworkReply *reply = netMgr->post(req, payload);

        // TLS errors: suppress or bypass if configured
        connect(reply, &QNetworkReply::sslErrors, this, [this, reply, usingPinnedCas](const QList<QSslError> &errors) {
            if (allowInsecureTlsBypass) {
                reply->ignoreSslErrors(); // proceed despite validation errors (insecure)
                emit sigError(QStringLiteral("TLS bypass enabled: proceeding without certificate verification."));
                return;
            }
            if (suppressTlsWarnings) {
                return; // hide TLS validation messages; handshake still enforces verification
            }

            // Diagnostics when not suppressed
            QStringList msgs; for (const auto &e : errors) msgs << e.errorString();
            QSslCertificate peerCert = reply->sslConfiguration().peerCertificate();
            QString peerInfo = peerCert.isNull()
                ? QStringLiteral("Peer certificate unavailable.")
                : QString("Peer CN=%1 | Issuer CN=%2 | Valid: %3 → %4")
                      .arg(peerCert.subjectInfo(QSslCertificate::CommonName).join(", "))
                      .arg(peerCert.issuerInfo(QSslCertificate::CommonName).join(", "))
                      .arg(peerCert.effectiveDate().toString(Qt::ISODate))
                      .arg(peerCert.expiryDate().toString(Qt::ISODate));

            const QString storeNote = usingPinnedCas
                ? QStringLiteral("Pinned CA in use.")
                : QStringLiteral("System trust store in use.");
            emit sigError(QString("TLS validation failed (%1) %2 | Errors: %3")
                          .arg(storeNote).arg(peerInfo).arg(msgs.join("; ")));
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
    bool suppressTlsWarnings {false};     // default off; enable via UI
    bool allowInsecureTlsBypass {false};  // default off; enable via UI for insecure bypass
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
          chkLoop(new QCheckBox(QStringLiteral("Endless loop (x)"), this)),
          chkSuppressTls(new QCheckBox(QStringLiteral("Suppress TLS validation messages"), this)),
          chkBypassTls(new QCheckBox(QStringLiteral("Allow insecure TLS (ignore cert errors)"), this)) {

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
        chkSuppressTls->setToolTip(QStringLiteral("Hide TLS validation error messages (handshake still enforces verification unless bypass is enabled)."));
        chkBypassTls->setToolTip(QStringLiteral("Proceed despite TLS validation errors (INSECURE). Avoid on public networks."));

        btnSend->setFixedWidth(90);

        // Defaults: suppression off, bypass off (explicit opt-in)
        chkSuppressTls->setChecked(false);
        chkBypassTls->setChecked(false);

        layoutBottom->addWidget(ledMsg);
        layoutBottom->addWidget(chkLoop);
        layoutBottom->addWidget(chkSuppressTls);
        layoutBottom->addWidget(chkBypassTls);
        layoutBottom->addWidget(btnSend);

        layoutMain->addWidget(txtView);
        layoutMain->addLayout(layoutBottom);

        connect(ledMsg, &QLineEdit::returnPressed, this, &WinMain::onSendClicked);
        connect(btnSend, &QPushButton::clicked, this, &WinMain::onSendClicked);

        connect(gptClient, &CltGpt::sigReply, this, &WinMain::onReply);
        connect(gptClient, &CltGpt::sigError, this, &WinMain::onError);
        connect(gptClient, &CltGpt::sigRateLimited, this, &WinMain::onRateLimited);

        // Wire TLS toggles
        connect(chkSuppressTls, &QCheckBox::toggled, this, [this](bool on) { gptClient->setSuppressTlsWarnings(on); });
        connect(chkBypassTls, &QCheckBox::toggled, this, [this](bool on) { gptClient->setAllowInsecureBypass(on); });

        setStyle();
        setMinimumSize(900, 600);
        setWindowTitle(QStringLiteral("GPT single-file client (controlled TLS bypass)"));

        lastSend.invalidate();

        // Startup CA audit: show loaded anchors if any; otherwise system trust
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
                    txtView->append(QString("<i>Loaded CA: CN=%1 | Valid: %2 → %3</i>")
                        .arg(ca.subjectInfo(QSslCertificate::CommonName).join(", "))
                        .arg(ca.effectiveDate().toString(Qt::ISODate))
                        .arg(ca.expiryDate().toString(Qt::ISODate)));
                }
            } else {
                txtView->append(QStringLiteral("<i>No pinned CAs loaded — using system trust store.</i>"));
            }
            scrollToBottom();
        }
    }

private slots:
    void onSendClicked() {
        const QString text = ledMsg->text().trimmed();
        if (text.isEmpty()) return;

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
        const qint64 prev = currentMinGapMs;
        currentMinGapMs = qMin<qint64>(currentMinGapMs * 2, 20000);
        const int jitter = qMax<int>(200, int(currentMinGapMs * 0.2));
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
    QCheckBox *chkSuppressTls;
    QCheckBox *chkBypassTls;

    QElapsedTimer lastSend;
    qint64 currentMinGapMs {2000};
};

// --- Entry point ---
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    WinMain win;
    win.show();
    return app.exec();
}

#include "main.moc"

