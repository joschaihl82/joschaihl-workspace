// main.cpp
// Qt6 single-file example: QWebEngineView + QWebChannel bridge + progress bar + "endless" checkbox
// - Clears common proxy environment variables before QApplication is created
// - Uses a LoggingPage to forward JS console messages to the app log
// - Attempts to inject qwebchannel.js from Qt resource (:/qtwebchannel/qwebchannel.js) as inline script
// - Loads https://copilot.microsoft.com
//
// Build with CMake (example CMakeLists.txt below). Requires Qt6 Widgets, WebEngineWidgets, WebChannel.

#include <QApplication>
//#include <QtWebEngine/QtWebEngine>
#include <QtWebEngineWidgets/QWebEngineView>
///#include <QtWebEngineWidgets/QWebEnginePage>
#include <QtWebEngineCore/qtwebengineglobal.h> // optional, nur wenn du WebEngine-Globals brauchst
#include <QWebChannel>
#include <QFile>
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QProgressBar>
#include <QStatusBar>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QNetworkProxy>
#include <QDebug>

#ifdef Q_OS_UNIX
# include <stdlib.h> // unsetenv
#endif

static void clearProxyEnvironment()
{
    const char *vars[] = {
        "HTTP_PROXY", "http_proxy",
        "HTTPS_PROXY", "https_proxy",
        "ALL_PROXY", "all_proxy",
        nullptr
    };
    for (const char **p = vars; *p; ++p) {
        qputenv(*p, QByteArray());
#ifdef Q_OS_UNIX
        unsetenv(*p);
#endif
    }
    qDebug() << "Proxy environment cleared.";
}

// LoggingPage: forwards JS console messages to qDebug
class LoggingPage : public QWebEnginePage {
    Q_OBJECT
public:
    explicit LoggingPage(QObject *parent = nullptr) : QWebEnginePage(parent) {}
protected:
    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level,
                                  const QString &message,
                                  int lineNumber,
                                  const QString &sourceID) override
    {
        Q_UNUSED(level);
        qDebug().noquote() << "JS console:" << message << "line" << lineNumber << "src" << sourceID;
        QWebEnginePage::javaScriptConsoleMessage(level, message, lineNumber, sourceID);
    }
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent),
          view(new QWebEngineView(this)),
          inputBox(new QLineEdit(this)),
          sendBtn(new QPushButton(tr("Send to page"), this)),
          endlessCb(new QCheckBox(tr("endless (o)"), this)),
          progressBar(new QProgressBar(this)),
          statusLabel(new QLabel(this)),
          channel(new QWebChannel(this))
    {
        // Central widget + layout
        QWidget *central = new QWidget(this);
        setCentralWidget(central);
        QVBoxLayout *mainLayout = new QVBoxLayout(central);
        mainLayout->setContentsMargins(8,8,8,8);
        mainLayout->setSpacing(6);

        // Top toolbar
        QWidget *toolbar = new QWidget(this);
        QHBoxLayout *tbLayout = new QHBoxLayout(toolbar);
        tbLayout->setContentsMargins(0,0,0,0);
        tbLayout->setSpacing(8);

        QLabel *lbl = new QLabel(tr("Input:"), this);
        tbLayout->addWidget(lbl);

        inputBox->setPlaceholderText(tr("Nachricht an die Seite"));
        inputBox->setMinimumWidth(300);
        inputBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        tbLayout->addWidget(inputBox);

        sendBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        tbLayout->addWidget(sendBtn);

        endlessCb->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        tbLayout->addWidget(endlessCb);

        tbLayout->addItem(new QSpacerItem(10,10, QSizePolicy::Expanding, QSizePolicy::Minimum));
        mainLayout->addWidget(toolbar);

        // Web view
        view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        mainLayout->addWidget(view, 1);

        // Status bar
        QStatusBar *sb = new QStatusBar(this);
        setStatusBar(sb);
        statusLabel->setText(tr("Ready"));
        sb->addWidget(statusLabel, 1);

        progressBar->setRange(0,100);
        progressBar->setValue(0);
        progressBar->setFixedWidth(220);
        sb->addPermanentWidget(progressBar);

        // Use LoggingPage to capture JS console messages
        LoggingPage *page = new LoggingPage(this);
        view->setPage(page);

        // Register bridge object in QWebChannel
        bridge = new QObject(this); // minimal placeholder if you want to expand
        channel->registerObject(QStringLiteral("bridge"), bridge);
        page->setWebChannel(channel);

        // Connections
        connect(sendBtn, &QPushButton::clicked, this, &MainWindow::onSendClicked);
        connect(view, &QWebEngineView::loadFinished, this, &MainWindow::onLoadFinished);
        connect(view, &QWebEngineView::loadProgress, this, &MainWindow::onPageLoadProgress);
        connect(endlessCb, &QCheckBox::stateChanged, this, &MainWindow::onEndlessChanged);

        setWindowTitle(tr("Remote Controller"));
        resize(1100, 760);

        // Ensure Qt network uses no proxy
        QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);

        // Load initial URL (copilot)
        view->load(QUrl(QStringLiteral("https://copilot.microsoft.com")));
    }

private slots:
    void onSendClicked() {
        const QString text = inputBox->text();
        const QString escaped = jsEscape(text);
        const QString setJs = QStringLiteral("if(window.copilot_set_input) window.copilot_set_input('%1');").arg(escaped);
        const QString clickJs = QStringLiteral("if(window.copilot_click_send) window.copilot_click_send();");
        view->page()->runJavaScript(setJs);
        QTimer::singleShot(150, [this, clickJs]() { view->page()->runJavaScript(clickJs); });
    }

    void onLoadFinished(bool ok) {
        if (ok) {
            statusLabel->setText(tr("Page loaded"));
            injectHelperJs();
        } else {
            statusLabel->setText(tr("Load failed"));
        }
    }

    void onPageLoadProgress(int progress) {
        progressBar->setValue(progress);
        if (progress < 100) statusLabel->setText(tr("Loading... %1%").arg(progress));
        else statusLabel->setText(tr("Page loaded"));
    }

    void onEndlessChanged(int state) {
        const bool val = (state == Qt::Checked);
        view->page()->runJavaScript(QStringLiteral("window._endless = %1;").arg(val ? "true" : "false"));
    }

private:
    QWebEngineView *view;
    QLineEdit *inputBox;
    QPushButton *sendBtn;
    QCheckBox *endlessCb;
    QProgressBar *progressBar;
    QLabel *statusLabel;

    QObject *bridge;
    QWebChannel *channel;

    QString jsEscape(const QString &s) const {
        QString out = s;
        out.replace("\\", "\\\\");
        out.replace("'", "\\'");
        out.replace("\r", "");
        out.replace("\n", "\\n");
        return out;
    }

    void injectHelperJs() {
        // Try to read qwebchannel.js from resources (recommended: add qwebchannel.js to resources.qrc under prefix /qtwebchannel)
        QString qwebchannelCode;
        QFile f(":/qtwebchannel/qwebchannel.js");
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qwebchannelCode = QString::fromUtf8(f.readAll());
            f.close();
        }

        // Helper JS: safe operations (no .at, inline qwebchannel if available)
        QString helper = R"JS(
(function(){
  if(window._copilot_helper_installed) return;
  window._copilot_helper_installed = true;
  window._endless = false;

  const inputSelector = 'textarea#userInput, textarea[data-testid="composer-input"], [contenteditable="true"]';
  const sendSelector = 'button[data-testid="submit-button"], button[title="Nachricht übermitteln"], button[type="submit"]';

  function findInput(){ return document.querySelector(inputSelector); }
  function findSend(){ return document.querySelector(sendSelector); }

  function reportStatus(s){
    if(window.bridge && typeof window.bridge.reportStatus === 'function'){
      try { window.bridge.reportStatus(s); } catch(e) {}
    }
  }
  function reportProgress(p){
    if(window.bridge && typeof window.bridge.reportProgress === 'function'){
      try { window.bridge.reportProgress(p); } catch(e) {}
    }
  }

  function hasSpinner(){
    if(document.querySelector('[role="progressbar"]')) return true;
    if(document.querySelector('[class*="spinner"], [class*="loading"], [class*="progress"]')) return true;
    return false;
  }

  function startBridge(){
    try {
      if(window.QWebChannel && !window._copilot_webchannel_initialized){
        window._copilot_webchannel_initialized = true;
        new QWebChannel(qt.webChannelTransport, function(channel){
          window.bridge = channel.objects.bridge;
        });
      }
    } catch(e) { console.error('startBridge error', e); }

    var busy = false;
    var progressCounter = 0;
    var observer = new MutationObserver(function(){
      var spinner = hasSpinner();
      if(spinner && !busy){
        busy = true;
        progressCounter = 0;
        reportStatus('busy');
      }
      if(busy){
        progressCounter = Math.min(95, progressCounter + Math.floor(Math.random()*15)+5);
        reportProgress(progressCounter);
      }
      if(!spinner && busy){
        busy = false;
        reportProgress(100);
        reportStatus('idle');
        if(window._endless){
          setTimeout(function(){
            const btn = findSend();
            if(btn) btn.click();
          }, 400);
        }
        setTimeout(function(){ reportProgress(0); }, 800);
      }
    });
    observer.observe(document.body, {childList:true, subtree:true, attributes:true});
  }

  // If qwebchannel.js was inlined by the host, QWebChannel may already be available.
  if(window.QWebChannel){
    startBridge();
  } else {
    // If the host injected qwebchannel.js inline via resource, it will be available after DOM ready.
    // Otherwise we attempt to load a blob fallback (best-effort).
    document.addEventListener('DOMContentLoaded', function(){
      if(window.QWebChannel){
        startBridge();
        return;
      }
      // Try to fetch /qtwebchannel/qwebchannel.js (server-provided) and load as blob
      try {
        var xhr = new XMLHttpRequest();
        xhr.open('GET', '/qtwebchannel/qwebchannel.js', true);
        xhr.onreadystatechange = function(){
          if(xhr.readyState === 4){
            if(xhr.status >= 200 && xhr.status < 400){
              var blob = new Blob([xhr.responseText], {type: 'text/javascript'});
              var blobUrl = URL.createObjectURL(blob);
              var s = document.createElement('script');
              s.src = blobUrl;
              s.onload = function(){ URL.revokeObjectURL(blobUrl); startBridge(); };
              s.onerror = function(){ startBridge(); };
              document.head.appendChild(s);
            } else {
              startBridge();
            }
          }
        };
        xhr.send();
      } catch(e){
        startBridge();
      }
    });
  }
})();
)JS";

        // If we have qwebchannel.js in resources, inject it inline first (avoids Trusted Types issues)
        if (!qwebchannelCode.isEmpty()) {
            // Escape the code for embedding into a JS string literal
            QString escaped = qwebchannelCode;
            escaped.replace("\\", "\\\\");
            escaped.replace("'", "\\'");
            escaped.replace("\r", "");
            escaped.replace("\n", "\\n");

            QString inlineLoader = QStringLiteral(R"(
(function(){
  try {
    var s = document.createElement('script');
    s.type = 'text/javascript';
    s.text = '%1';
    document.head.appendChild(s);
    console.log('qwebchannel.js injected inline from resource');
  } catch(e) {
    console.error('inline injection failed', e);
  }
})();
)").arg(escaped);

            view->page()->runJavaScript(inlineLoader);
        }

        // Finally inject helper (safe) script
        view->page()->runJavaScript(helper);
    }
};

int main(int argc, char *argv[])
{
    // Clear proxy env before QApplication is created
    clearProxyEnvironment();

    // Optional debug flag (only for temporary debugging; remove for production)
    // qputenv("QTWEBENGINE_CHROMIUM_FLAGS", QByteArray("--ignore-certificate-errors"));

    QApplication app(argc, argv);

    // Ensure Qt network uses no proxy
    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);

    MainWindow w;
    w.show();
    return app.exec();
}

#include "main.moc"

