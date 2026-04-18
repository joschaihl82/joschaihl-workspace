
#include <QApplication>

#include <QMainWindow>

#include <QtCharts>

#include <QChartView>

#include <QLineSeries>

#include <QTimer>

#include <QDebug>

#include <QWidget>

#include <QGridLayout>

#include <QLabel>

#include <QProgressBar>

#include <QDateTime>

#include <QVector>

#include <QOpenGLWidget>

#include <QPainter>

#include <cmath>

#include <QRandomGenerator>

#include <vector>

#include <thread>

#include <mutex>

#include <condition_variable>

#include <QFile>

#include <QProcess>

#include <QPushButton>

#include <QGeoPositionInfoSource>

#include <QGeoPositionInfo>

#include <QNetworkAccessManager>

#include <QNetworkReply>

#include <QJsonDocument>

#include <QJsonObject>

#include <QWebEngineView>

#include <QWebEngineSettings>

#include <QStandardPaths>

#include <QScreen>

#include <QFileDialog>

#include <alsa/asoundlib.h>


QT_CHARTS_USE_NAMESPACE


class DosimeterWidget : public QWidget {

    Q_OBJECT

public:

    DosimeterWidget(QWidget *parent = nullptr);

    ~DosimeterWidget();

public slots:

    void updateData();

    void stopAlert();

    void generateReport();

    void updateLocation(const QGeoPositionInfo &info);

    void locationError(QGeoPositionInfoSource::Error error);

    //void locationNameReplyFinishedNominatim(QNetworkReply *reply);

    void updateMapPosition(const QGeoPositionInfo &info);

    void takeScreenshot();

private:

    QChart *chart;

    QLineSeries *series;

    QTimer *timer;

    int pointCount;

    double accumulatedDosage;

    QProgressBar *progressBar;

    QLabel *timeLabel, *dosageLabel, *locationLabel, *durationLabel;

    QVector<double> history;

    QOpenGLWidget *glWidget;

    snd_pcm_t *alertPcmHandle;

    QTimer *alertTimer;

    std::thread alertThread;

    std::mutex alertMutex;

    std::condition_variable alertCv;

    bool alertRequested, alertStopRequested, dataReady;

    QDateTime startTime;

    QPushButton *reportButton, *resetButton;

    QGeoPositionInfoSource *positionSource;

    QString locationString;

    QNetworkAccessManager *networkManager;

    QWebEngineView *mapView;

    QGeoCoordinate currentCoordinate;

    double translateNoiseToRad(int noiseType);

    void paintGL(QPainter *painter);

    void playAlsaClickSound();

    void playAlert();

    void alertThreadFunction();

    void getLocationName(const QGeoCoordinate &coordinate);

    void initializeMap();

    QString getMapHtmlPath();

    void updateDisplayLabels();

};


std::vector<short> generateSineWave(double f, double d, int r, double a);

std::vector<short> generateSquareWave(double f, double d, int r, double a);

std::vector<short> generateClick(double d, int r, double a);


DosimeterWidget::DosimeterWidget(QWidget *parent) : QWidget(parent), pointCount(0), accumulatedDosage(0), alertPcmHandle(nullptr), alertRequested(false), alertStopRequested(false), dataReady(false) {

    QGridLayout *layout = new QGridLayout(this);

    mapView = new QWebEngineView(this); layout->addWidget(mapView, 0, 0, 6, 1); initializeMap();

    progressBar = new QProgressBar(this); progressBar->setRange(0, 100); layout->addWidget(progressBar, 0, 1, 1, 2);

    timeLabel = new QLabel(this); layout->addWidget(timeLabel, 1, 1);

    dosageLabel = new QLabel(this); layout->addWidget(dosageLabel, 1, 2);

    locationLabel = new QLabel(this); layout->addWidget(locationLabel, 2, 1, 1, 2);

    durationLabel = new QLabel(this); layout->addWidget(durationLabel, 3, 1, 1, 2);

    chart = new QChart(); series = new QLineSeries(); chart->addSeries(series); chart->createDefaultAxes(); chart->setTitle("Microwave Readings (rad/m²)");

    QChartView *chartView = new QChartView(chart); chartView->setRenderHint(QPainter::Antialiasing); layout->addWidget(chartView, 4, 1, 1, 2);

    glWidget = new QOpenGLWidget(this); layout->addWidget(glWidget, 5, 1, 1, 2);

    timer = new QTimer(this); connect(timer, &QTimer::timeout, this, &DosimeterWidget::updateData); timer->start(100);

    alertTimer = new QTimer(this); connect(alertTimer, &QTimer::timeout, this, &DosimeterWidget::stopAlert);

    alertThread = std::thread(&DosimeterWidget::alertThreadFunction, this);

    reportButton = new QPushButton("Screenshot", this); layout->addWidget(reportButton, 6, 1, 1, 1); connect(reportButton, &QPushButton::clicked, this, &DosimeterWidget::takeScreenshot);

    resetButton = new QPushButton("Reset", this); layout->addWidget(resetButton, 6, 2, 1, 1); connect(resetButton, &QPushButton::clicked, [this](){accumulatedDosage = 0; series->clear(); history.clear(); pointCount = 0; startTime = QDateTime::currentDateTime(); updateDisplayLabels();});

    startTime = QDateTime::currentDateTime();

    positionSource = QGeoPositionInfoSource::createDefaultSource(this);

    if (positionSource) { connect(positionSource, &QGeoPositionInfoSource::positionUpdated, this, &DosimeterWidget::updateLocation); connect(positionSource, static_cast<void (QGeoPositionInfoSource::*)(QGeoPositionInfoSource::Error)>(&QGeoPositionInfoSource::error), this, &DosimeterWidget::locationError); positionSource->startUpdates(); }

    networkManager = new QNetworkAccessManager(this); updateDisplayLabels();

}


DosimeterWidget::~DosimeterWidget() {

    { std::unique_lock<std::mutex> lock(alertMutex); alertStopRequested = true; } alertCv.notify_one();

    if (alertThread.joinable()) alertThread.join(); if (alertPcmHandle) snd_pcm_close(alertPcmHandle); delete alertTimer;

    if (positionSource) positionSource->stopUpdates();

}


void DosimeterWidget::updateData() {

    double radValue = translateNoiseToRad(QRandomGenerator::global()->bounded(5)); accumulatedDosage += radValue;

    series->append(pointCount, radValue); history.append(radValue); if (history.size() > 150) history.removeFirst();

    pointCount++; progressBar->setValue(qMin(100, (int)(radValue * 10))); glWidget->update(); playAlsaClickSound();

    if (radValue > 3.0) playAlert(); dataReady = true; updateDisplayLabels();

}


void DosimeterWidget::updateDisplayLabels() {

    timeLabel->setText(QDateTime::currentDateTime().toString("hh:mm:ss.zzz"));

    dosageLabel->setText(QString("Dosage: %1 rad/m²").arg(accumulatedDosage));

    durationLabel->setText(QString("Duration: %1 sec").arg(startTime.secsTo(QDateTime::currentDateTime())));

    chart->axes(Qt::Horizontal)[0]->setRange(pointCount > 100 ? pointCount - 100 : 0, pointCount > 100 ? pointCount : 100);

}


double DosimeterWidget::translateNoiseToRad(int noiseType) {

    switch (noiseType) { case 0: return 0.1; case 1: return 0.5; case 2: return 1.0; case 3: return 2.0; case 4: return 5.0; default: return 0.0; }

}


void DosimeterWidget::paintGL(QPainter *painter) {

    painter->fillRect(glWidget->rect(), Qt::black); if (history.isEmpty()) return;

    int w = glWidget->width(), h = glWidget->height(), n = history.size(); painter->setPen(Qt::green);

    for (int i = 1; i < n; ++i) {

        int x1 = (i - 1) * w / n, y1 = h - (history[i - 1] / 5.0) * h, x2 = i * w / n, y2 = h - (history[i] / 5.0) * h;

        painter->drawLine(x1, y1, x2, y2);

    }

}


void DosimeterWidget::playAlsaClickSound() {

    snd_pcm_t *pcm; snd_pcm_hw_params_t *params; unsigned int rate = 44100, chans = 1, dur = 10;

    snd_pcm_uframes_t frames = rate * dur / 1000; short *buf = new short[frames]; double freq = 2000.0, amp = 32000.0;

    for (snd_pcm_uframes_t i = 0; i < frames; ++i) { double t = (double)i / rate; buf[i] = (short)(amp * sin(2.0 * M_PI * freq * t) * exp(-t * 100)); }

    if (snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) { qDebug() << "ALSA open error"; delete[] buf; return; }

    snd_pcm_hw_params_alloca(&params); snd_pcm_hw_params_any(pcm, params);

    snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED); snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_S16_LE);

    snd_pcm_hw_params_set_channels(pcm, params, chans); snd_pcm_hw_params_set_rate_near(pcm, params, &rate, nullptr);

    snd_pcm_hw_params_set_period_size_near(pcm, params, &frames, nullptr);

    if (snd_pcm_hw_params(pcm, params) < 0) { qDebug() << "ALSA set params error"; snd_pcm_close(pcm); delete[] buf; return; }

    if (snd_pcm_writei(pcm, buf, frames) < 0) qDebug() << "ALSA write error";

    snd_pcm_drain(pcm); snd_pcm_close(pcm); delete[] buf;

}


void DosimeterWidget::playAlert() { { std::unique_lock<std::mutex> lock(alertMutex); alertRequested = true; } alertCv.notify_one(); }


void DosimeterWidget::alertThreadFunction() {

    while (true) {

        std::unique_lock<std::mutex> lock(alertMutex); alertCv.wait(lock, [this] { return alertRequested || alertStopRequested; });

        if (alertStopRequested) break;

        if (alertRequested) {

            alertRequested = false; if (!dataReady) continue;

            if (!alertPcmHandle) {

                if (snd_pcm_open(&alertPcmHandle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) { qDebug() << "ALSA alert open error"; continue; }

                snd_pcm_hw_params_t *params; snd_pcm_hw_params_alloca(&params); snd_pcm_hw_params_any(alertPcmHandle, params);

                unsigned int rate = 44100; snd_pcm_hw_params_set_access(alertPcmHandle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

                snd_pcm_hw_params_set_format(alertPcmHandle, params, SND_PCM_FORMAT_S16_LE); snd_pcm_hw_params_set_channels(alertPcmHandle, params, 1);

                snd_pcm_hw_params_set_rate_near(alertPcmHandle, params, &rate, nullptr); snd_pcm_hw_params(alertPcmHandle, params);

            }

            double rad = history.isEmpty() ? 0 : history.back(), interval = 1.0 / (1.0 + std::log(1.0 + rad)); if (interval < 0.1) interval = 0.1;

            double dur = 0.2, amp = 32000.0, baseFreq = 440.0, clickFreq = 10; int rate = 44100;

            std::vector<short> base = generateSquareWave(baseFreq, dur, rate, amp), mod = generateSineWave(baseFreq * 1.5, dur, rate, amp * 0.5), clickTrack;

            for (double t = 0; t < dur; t += (1.0 / clickFreq)) { std::vector<short> click = generateClick(0.005, rate, amp); clickTrack.insert(clickTrack.end(), click.begin(), click.end()); }

            snd_pcm_writei(alertPcmHandle, base.data(), base.size()); snd_pcm_writei(alertPcmHandle, mod.data(), mod.size()); snd_pcm_writei(alertPcmHandle, clickTrack.data(), clickTrack.size());

            std::this_thread::sleep_for(std::chrono::duration<double>(interval));

        }

    }

}


void DosimeterWidget::stopAlert() { if (alertPcmHandle) { snd_pcm_drain(alertPcmHandle); alertTimer->stop(); } }


void DosimeterWidget::generateReport() { takeScreenshot(); }


void DosimeterWidget::takeScreenshot() {

    QScreen *screen = QGuiApplication::primaryScreen();

    if (screen) {

        QString file = QFileDialog::getSaveFileName(this, "Save Screenshot", "screenshot.png", "PNG (*.png);;JPEG (*.jpg *.jpeg)");

        if (!file.isEmpty()) screen->grabWindow(this->winId()).save(file);

    }

}


void DosimeterWidget::updateLocation(const QGeoPositionInfo &info) {

    if (info.isValid()) {

        currentCoordinate = info.coordinate(); getLocationName(currentCoordinate);

        locationString = QString("GPS: Lat: %1, Lon: %2").arg(currentCoordinate.latitude()).arg(currentCoordinate.longitude());

        locationLabel->setText(locationString); updateMapPosition(info);

    }

}


void DosimeterWidget::locationError(QGeoPositionInfoSource::Error error) {

    qDebug() << "Location error:" << error; locationString = "Location unavailable."; locationLabel->setText(locationString);

}


void DosimeterWidget::getLocationName(const QGeoCoordinate &coord) {

    QString url = QString("https://nominatim.openstreetmap.org/reverse?format=jsonv2&lat=%1&lon=%2").arg(coord.latitude()).arg(coord.longitude());

    QNetworkReply *reply = networkManager->get(QNetworkRequest(QUrl(url)));

    connect(reply, &QNetworkReply::finished, [this, reply](){

        if (reply->error() == QNetworkReply::NoError) {

            QJsonObject obj = QJsonDocument::fromJson(reply->readAll()).object();

            if (obj.contains("display_name")) { locationString = QString("%1 - %2").arg(obj["display_name"].toString()).arg(locationString); locationLabel->setText(locationString); }

        } else qDebug() << "Location name failed:" << reply->errorString();

        reply->deleteLater();

    });

}


void DosimeterWidget::initializeMap() {

    mapView->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);

#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)

    //mapView->settings()->setAttribute(QWebEngineSettings::GeolocationEnabled, true);

#endif

    mapView->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);

    mapView->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);

    mapView->setUrl(QUrl::fromLocalFile(getMapHtmlPath()));

}


void DosimeterWidget::updateMapPosition(const QGeoPositionInfo &info) {

    if (info.isValid()) {

        QGeoCoordinate coord = info.coordinate();

        mapView->page()->runJavaScript(QString("updateMarker(%1, %2);").arg(coord.latitude()).arg(coord.longitude()));

    }

}


QString DosimeterWidget::getMapHtmlPath() {

    QString html = R"(<!DOCTYPE html><html><head><title>Map</title><meta charset="utf-8" /><meta name="viewport" content="width=device-width, initial-scale=1.0"><link rel="stylesheet" href="https://unpkg.com/leaflet@1.7.1/dist/leaflet.css" /><script src="https://unpkg.com/leaflet@1.7.1/dist/leaflet.js"></script><style>#map { height: 400px; }</style></head><body><div id="map"></div><script>var map = L.map('map').setView([0, 0], 2);L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'}).addTo(map);var marker = L.marker([0, 0]).addTo(map);function updateMarker(lat, lng) {marker.setLatLng([lat, lng]);map.setView([lat, lng], 13);}</script></body></html>)";

    QString path = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/map.html";

    QFile file(path);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) { file.write(html.toUtf8()); file.close(); return path; }

    else { qDebug() << "Failed to create map.html"; return ""; }

}


int main(int argc, char *argv[]) {

    QApplication app(argc, argv);

    DosimeterWidget dosimeter;

    dosimeter.show();

    return app.exec();

}


std::vector<short> generateSineWave(double f, double d, int r, double a) {

    int n = static_cast<int>(d * r); std::vector<short> s(n);

    for (int i = 0; i < n; ++i) { double t = static_cast<double>(i) / r; s[i] = static_cast<short>(a * sin(2.0 * M_PI * f * t)); }

    return s;

}


std::vector<short> generateSquareWave(double f, double d, int r, double a) {

    int n = static_cast<int>(d * r); std::vector<short> s(n);

    for (int i = 0; i < n; ++i) { double t = static_cast<double>(i) / r; s[i] = static_cast<short>(a * ((sin(2.0 * M_PI * f * t) >= 0) ? 1.0 : -1.0)); }

    return s;

}


std::vector<short> generateClick(double d, int r, double a) {

    int n = static_cast<int>(d * r); std::vector<short> s(n); double freq = 10000.0;

    for (int i = 0; i < n; ++i) { double t = static_cast<double>(i) / r; s[i] = (short)(a * sin(2.0 * M_PI * freq * t) * exp(-t * 500)); }

    return s;

}


#include "main.moc"
