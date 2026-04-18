// main.cpp
// Qt5 single-file: verbesserte Layoutstruktur für Remote Controller
// Startet ohne Proxy-Umgebungseinträge und lädt https://copilot.microsoft.com
// Build: qmake (QT += widgets webenginewidgets webenginecore webchannel) oder CMake

#include <QApplication>
#include <QtWebEngine/QtWebEngine>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWebEngineWidgets/QWebEnginePage>
#include <QWebChannel>
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QProgressBar>
#include <QStatusBar>
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QNetworkProxy>
#include <QDebug>

#ifdef Q_OS_UNIX
  #include <stdlib.h> // unsetenv
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

class Bridge : public QObject {
    Q_OBJECT
public:
    explicit Bridge(QObject *parent = nullptr) : QObject(parent) {}
    Q_INVOKABLE void reportStatus(const QString &status) { emit statusChanged(status); }
    Q_INVOKABLE void reportProgress(int percent) { emit progressReported(percent); }

signals:
    void statusChanged(const QString &status);
    void progressReported(int percent);
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
          bridge(new Bridge(this)),
          channel(new QWebChannel(this))
    {
        // Grundlayout: zentrales Widget mit vertikalem Layout
        QWidget *central = new QWidget(this);
        setCentralWidget(central);
        QVBoxLayout *mainLayout = new QVBoxLayout(central);
        mainLayout->setContentsMargins(8,8,8,8);
        mainLayout->setSpacing(6);

        // --- Toolbar (oben) ---
        QWidget *toolbar = new QWidget(this);
        toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        QHBoxLayout *tbLayout = new QHBoxLayout(toolbar);
        tbLayout->setContentsMargins(0,0,0,0);
        tbLayout->setSpacing(8);

        QLabel *lbl = new QLabel(tr("Input:"), this);
        lbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        tbLayout->addWidget(lbl);

        inputBox->setPlaceholderText(tr("Nachricht an die Seite"));
        inputBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        inputBox->setMinimumWidth(300);
        tbLayout->addWidget(inputBox);

        sendBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        tbLayout->addWidget(sendBtn);

        endlessCb->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        tbLayout->addWidget(endlessCb);

        // Spacer rechts, damit Toolbar-Elemente links bleiben
        QSpacerItem *sp = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
        tbLayout->addItem(sp);

        mainLayout->addWidget(toolbar);

        // --- WebView (zentrale, flexible Fläche) ---
        view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        mainLayout->addWidget(view, /*stretch=*/1);

        // --- Statusbar (unten) ---
        QStatusBar *sb = new QStatusBar(this);
        setStatusBar(sb);

        statusLabel->setText(tr("Ready"));
        sb->addWidget(statusLabel, /*stretch=*/1);

        progressBar->setRange(0,100);
        progressBar->setValue(0);
        progressBar->setTextVisible(true);
        progressBar->setFixedWidth(220);
        sb->addPermanentWidget(progressBar);

        // WebChannel Bridge
        channel->registerObject(QStringLiteral("bridge"), bridge);
        view->page()->setWebChannel(channel);

        // Verbindungen
        connect(sendBtn, &QPushButton::clicked, this, &MainWindow::onSendClicked);
        connect(view, &QWebEngineView::loadFinished, this, &MainWindow::onLoadFinished);
        connect(view, &QWebEngineView::loadProgress, this, &MainWindow::onPageLoadProgress);
        connect(endlessCb, &QCheckBox::stateChanged, this, &MainWindow::onEndlessChanged);

        connect(bridge, &Bridge::statusChanged, this, &MainWindow::onBridgeStatusChanged);
        connect(bridge, &Bridge::progressReported, this, &MainWindow::onBridgeProgressReported);

        // Default window properties
        setWindowTitle(tr("Remote Controller"));
        resize(1100, 760);

        // Ensure Qt network uses no proxy
        QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);

        // Load initial URL (replaced qt.pro -> copilot.microsoft.com)
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

    void onBridgeStatusChanged(const QString &status) {
        if (status == QLatin1String("busy")) {
            statusLabel->setText(tr("Server: responding..."));
            progressBar->setValue(40);
        } else if (status == QLatin1String("idle")) {
            statusLabel->setText(tr("Server: done"));
            progressBar->setValue(100);
            QTimer::singleShot(800, [this](){ progressBar->setValue(0); statusLabel->setText(tr("Ready")); });
        } else {
            statusLabel->setText(status);
        }
    }

    void onBridgeProgressReported(int percent) {
        progressBar->setValue(percent);
        statusLabel->setText(tr("Server: %1%").arg(percent));
    }

private:
    QWebEngineView *view;
    QLineEdit *inputBox;
    QPushButton *sendBtn;
    QCheckBox *endlessCb;
    QProgressBar *progressBar;
    QLabel *statusLabel;

    Bridge *bridge;
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
        const QString js = QStringLiteral(R"JS(
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

  window.copilot_set_input = function(text){
    const inp = findInput();
    if(!inp) return false;
    inp.focus();
    if ('value' in inp) {
      inp.value = text;
      inp.dispatchEvent(new Event('input', {bubbles:true}));
      inp.dispatchEvent(new Event('change', {bubbles:true}));
    } else {
      inp.innerText = text;
      try { inp.dispatchEvent(new InputEvent('input', {bubbles:true})); }
      catch(e){ var ev = document.createEvent('Event'); ev.initEvent('input', true, true); inp.dispatchEvent(ev); }
    }
    return true;
  };

  window.copilot_click_send = function(){
    const btn = findSend();
    if(!btn) return false;
    btn.click();
    return true;
  };

  function hasSpinner(){
    if(document.querySelector('[role="progressbar"]')) return true;
    if(document.querySelector('[class*="spinner"], [class*="loading"], [class*="progress"]')) return true;
    return false;
  }

  function ensureWebChannelAndStart(){
    if(window.QWebChannel) { startBridge(); return; }
    var script = document.createElement('script');
    script.src = 'qrc:/qtwebchannel/qwebchannel.js';
    script.onload = function(){ startBridge(); };
    script.onerror = function(){ startBridge(); };
    document.head.appendChild(script);
  }

  function startBridge(){
    try {
      if(window.QWebChannel && !window._copilot_webchannel_initialized){
        window._copilot_webchannel_initialized = true;
        new QWebChannel(qt.webChannelTransport, function(channel){
          window.bridge = channel.objects.bridge;
        });
      }
    } catch(e) {}

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

  ensureWebChannelAndStart();
})();
)JS");
        view->page()->runJavaScript(js);
    }
};

int main(int argc, char *argv[]) {
    // Clear proxy env before QApplication
    clearProxyEnvironment();

    QApplication app(argc, argv);

    // Ensure Qt network uses no proxy
    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);

    MainWindow w;
    w.show();
    return app.exec();
}

#include "main.moc"

