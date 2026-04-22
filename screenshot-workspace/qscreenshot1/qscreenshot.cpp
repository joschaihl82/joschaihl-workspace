// qscreenshot.cpp
// Qt6 Screenshot Cropper single-file with OpenCV edge detection suggestions
// Includes reordered to avoid OpenCV <-> X11 macro/name conflicts.

#include <QApplication>
#include <QScreen>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QGuiApplication>
#include <QPixmap>
#include <QTimer>
#include <QLabel>
#include <QCursor>
#include <QWindow>
#include <QStyle>
#include <QKeyEvent>
#include <QDateTime>
#include <QDir>
#include <QObject>
#include <QList>
#include <QVector>

// --- OpenCV must be included BEFORE X11 headers to avoid macro/name conflicts ---
#include <opencv2/opencv.hpp>

#ifdef Q_OS_UNIX
// Include X11 only after OpenCV to avoid the "Status" / True/False conflicts.
// Keep X11 usage limited to the functions that need it.
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <unistd.h>
#endif

class CropOverlay : public QWidget {
    Q_OBJECT
public:
    explicit CropOverlay(QPixmap screenPixmap, bool useActiveWindow = false, QWidget *parent = nullptr)
        : QWidget(parent), screen(screenPixmap), useActive(useActiveWindow)
    {
        setWindowFlag(Qt::FramelessWindowHint);
        setWindowFlag(Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_DeleteOnClose);

        QScreen *scr = QGuiApplication::screenAt(QCursor::pos());
        if (!scr) scr = QGuiApplication::primaryScreen();
        QRect geom = scr->geometry();
        setGeometry(geom);

        left = width() * 0.10;
        right = width() * 0.90;
        top = height() * 0.10;
        bottom = height() * 0.90;

        saveBtn = new QPushButton(tr("Speichern (Enter)"), this);
        cancelBtn = new QPushButton(tr("Abbrechen (Esc)"), this);
        autoResetBtn = new QPushButton(tr("Aktives Fenster"), this);
        detectBtn = new QPushButton(tr("Auto Detect"), this);
        nextBtn = new QPushButton(tr("Nächstes"), this);
        infoLabel = new QLabel(tr("Ziehe Kanten oder verschiebe das Rechteck. Tab/Nächstes: Vorschläge."), this);
        dimsLabel = new QLabel(this);

        saveBtn->setFixedWidth(140);
        cancelBtn->setFixedWidth(100);
        autoResetBtn->setFixedWidth(140);
        detectBtn->setFixedWidth(120);
        nextBtn->setFixedWidth(90);

        infoLabel->setStyleSheet("color: white; background: rgba(0,0,0,0.45); padding:4px;");
        dimsLabel->setStyleSheet("color: white; background: rgba(0,0,0,0.6); padding:4px;");

        connect(saveBtn, &QPushButton::clicked, this, &CropOverlay::saveAndClose);
        connect(cancelBtn, &QPushButton::clicked, this, &CropOverlay::close);
        connect(autoResetBtn, &QPushButton::clicked, this, &CropOverlay::resetToActiveWindow);
        connect(detectBtn, &QPushButton::clicked, this, &CropOverlay::runEdgeDetection);
        connect(nextBtn, &QPushButton::clicked, this, &CropOverlay::selectNextSuggestion);

        QHBoxLayout *h = new QHBoxLayout;
        h->addWidget(infoLabel);
        h->addStretch();
        h->addWidget(autoResetBtn);
        h->addWidget(detectBtn);
        h->addWidget(nextBtn);
        h->addWidget(saveBtn);
        h->addWidget(cancelBtn);
        h->setContentsMargins(10,10,10,10);

        QWidget *container = new QWidget(this);
        container->setLayout(h);
        container->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        container->move(10,10);
        container->show();

        dimsLabel->move(10, 60);
        dimsLabel->show();

        if (useActive) {
            QTimer::singleShot(10, this, &CropOverlay::resetToActiveWindow);
        }
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.drawPixmap(0,0, screen.scaled(size()));

        QColor maskColor(0,0,0,140);
        p.setBrush(maskColor);
        p.setPen(Qt::NoPen);

        QRect selRect = selectionRect();

        p.drawRect(0,0,width(), selRect.top());
        p.drawRect(0, selRect.bottom(), width(), height() - selRect.bottom());
        p.drawRect(0, selRect.top(), selRect.left(), selRect.height());
        p.drawRect(selRect.right(), selRect.top(), width() - selRect.right(), selRect.height());

        QPen pen(Qt::yellow);
        pen.setWidth(3);
        p.setPen(pen);
        p.setBrush(Qt::NoBrush);
        p.drawRect(selRect);

        QPen linePen(Qt::cyan);
        linePen.setWidth(2);
        p.setPen(linePen);
        p.drawLine(left, selRect.top(), left, selRect.bottom());
        p.drawLine(right, selRect.top(), right, selRect.bottom());
        p.drawLine(selRect.left(), top, selRect.right(), top);
        p.drawLine(selRect.left(), bottom, selRect.right(), bottom);

        const int hs = 10;
        p.fillRect(left - hs/2, top - hs/2, hs, hs, Qt::cyan);
        p.fillRect(right - hs/2, top - hs/2, hs, hs, Qt::cyan);
        p.fillRect(left - hs/2, bottom - hs/2, hs, hs, Qt::cyan);
        p.fillRect(right - hs/2, bottom - hs/2, hs, hs, Qt::cyan);

        // draw suggestions (if any)
        for (int i = 0; i < suggestions.size(); ++i) {
            QRect r = suggestions[i];
            QRect mapped = mapFromScreenRect(r);
            QPen spen(i == currentSuggestion ? Qt::green : Qt::magenta);
            spen.setWidth(i == currentSuggestion ? 3 : 2);
            p.setPen(spen);
            p.setBrush(Qt::NoBrush);
            p.drawRect(mapped);
        }

        dimsLabel->setText(tr("W x H: %1 x %2   Pos: %3,%4")
                           .arg(selRect.width()).arg(selRect.height())
                           .arg(selRect.left()).arg(selRect.top()));
    }

    void mousePressEvent(QMouseEvent *ev) override {
        QPoint pos = ev->pos();
        lastMouse = pos;
        dragging = detectDragZone(pos);
        if (dragging == DragMoveWhole) moveOffset = pos;
    }

    void mouseMoveEvent(QMouseEvent *ev) override {
        QPoint pos = ev->pos();
        int dx = pos.x() - lastMouse.x();
        int dy = pos.y() - lastMouse.y();
        lastMouse = pos;

        switch (dragging) {
            case DragLeft:
                left = qBound(0, left + dx, right - minWidth());
                break;
            case DragRight:
                right = qBound(left + minWidth(), right + dx, width());
                break;
            case DragTop:
                top = qBound(0, top + dy, bottom - minHeight());
                break;
            case DragBottom:
                bottom = qBound(top + minHeight(), bottom + dy, height());
                break;
            case DragMoveWhole: {
                int w = right - left;
                int h = bottom - top;
                int nx = qBound(0, left + dx, width() - w);
                int ny = qBound(0, top + dy, height() - h);
                left = nx; right = nx + w;
                top = ny; bottom = ny + h;
                break;
            }
            default:
                break;
        }
        update();
    }

    void mouseReleaseEvent(QMouseEvent *) override {
        dragging = DragNone;
    }

    void keyPressEvent(QKeyEvent *ev) override {
        int step = (ev->modifiers() & Qt::ShiftModifier) ? 10 : 1;
        bool handled = true;
        switch (ev->key()) {
            case Qt::Key_Return:
            case Qt::Key_Enter:
                saveAndClose();
                break;
            case Qt::Key_Escape:
                close();
                break;
            case Qt::Key_Tab:
                selectNextSuggestion();
                break;
            case Qt::Key_Left:
                left = qBound(0, left - step, right - minWidth());
                break;
            case Qt::Key_Right:
                right = qBound(left + minWidth(), right + step, width());
                break;
            case Qt::Key_Up:
                top = qBound(0, top - step, bottom - minHeight());
                break;
            case Qt::Key_Down:
                bottom = qBound(top + minHeight(), bottom + step, height());
                break;
            case Qt::Key_Space:
                toggleFull();
                break;
            default:
                handled = false;
                break;
        }
        if (handled) update();
        else QWidget::keyPressEvent(ev);
    }

private:
    enum DragZone { DragNone, DragLeft, DragRight, DragTop, DragBottom, DragMoveWhole };

    QRect selectionRect() const {
        return QRect(QPoint(left, top), QPoint(right, bottom)).normalized();
    }

    int minWidth() const { return 40; }
    int minHeight() const { return 40; }

    DragZone detectDragZone(const QPoint &p) const {
        const int tol = 12;
        QRect sel = selectionRect();
        QRect leftZone(sel.left() - tol, sel.top(), tol*2, sel.height());
        QRect rightZone(sel.right() - tol, sel.top(), tol*2, sel.height());
        QRect topZone(sel.left(), sel.top() - tol, sel.width(), tol*2);
        QRect bottomZone(sel.left(), sel.bottom() - tol, sel.width(), tol*2);
        QRect center(sel.left()+tol, sel.top()+tol, sel.width()-2*tol, sel.height()-2*tol);

        if (leftZone.contains(p)) return DragLeft;
        if (rightZone.contains(p)) return DragRight;
        if (topZone.contains(p)) return DragTop;
        if (bottomZone.contains(p)) return DragBottom;
        if (center.contains(p)) return DragMoveWhole;
        return DragNone;
    }

    // Map a QRect in screen coordinates (as produced by detection) to widget coords
    QRect mapFromScreenRect(const QRect &r) const {
        QSize screenSize = screen.size();
        QSize widgetSize = size();
        double sx = double(widgetSize.width()) / screenSize.width();
        double sy = double(widgetSize.height()) / screenSize.height();
        return QRect(int((r.left()) * sx), int((r.top()) * sy),
                     int(r.width() * sx), int(r.height() * sy));
    }

    // Map widget selection to original screen rect
    QRect mapSelectionToScreenRect() const {
        QRect sel = selectionRect();
        QSize screenSize = screen.size();
        QSize widgetSize = size();
        double sx = double(screenSize.width()) / widgetSize.width();
        double sy = double(screenSize.height()) / widgetSize.height();
        return QRect(int(sel.left() * sx), int(sel.top() * sy),
                     int(sel.width() * sx), int(sel.height() * sy));
    }

    void saveAndClose() {
        QRect srcRect = mapSelectionToScreenRect();
        QPixmap cropped = screen.copy(srcRect);

        QString defaultName = QDir::homePath() + "/screenshot-" + QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss") + ".jpg";
        QString fileName = QFileDialog::getSaveFileName(this, tr("Speichern als JPEG"),
                                                        defaultName,
                                                        tr("JPEG Files (*.jpg *.jpeg)"));
        if (fileName.isEmpty()) return;

        QImage img = cropped.toImage();
        img.save(fileName, "JPG", 90);
        close();
    }

    void toggleFull() {
        left = 0; top = 0; right = width(); bottom = height();
    }

    void resetToActiveWindow() {
#ifdef Q_OS_UNIX
        // Use X11 only here; X11 headers are included after OpenCV to avoid conflicts.
        Display *d = XOpenDisplay(NULL);
        if (!d) return;
        Atom a_net_active = XInternAtom(d, "_NET_ACTIVE_WINDOW", True);
        Atom actual;
        int format;
        unsigned long nitems, bytes;
        unsigned char *prop = nullptr;
        Window active = 0;
        if (a_net_active != None) {
            if (XGetWindowProperty(d, DefaultRootWindow(d), a_net_active, 0, (~0L), False,
                                   AnyPropertyType, &actual, &format, &nitems, &bytes, &prop) == Success) {
                if (prop) {
                    // XGetWindowProperty returns data as array of Window (unsigned long)
                    // Cast carefully: prop points to unsigned char*, so reinterpret as Window*
                    active = *(Window*)prop;
                    XFree(prop);
                }
            }
        }
        if (active) {
            XWindowAttributes attr;
            if (XGetWindowAttributes(d, active, &attr)) {
                int wx = attr.x;
                int wy = attr.y;
                Window child;
                int rx, ry;
                Window root = DefaultRootWindow(d);
                if (XTranslateCoordinates(d, active, root, 0, 0, &rx, &ry, &child)) {
                    wx = rx; wy = ry;
                }
                QRect geom = geometry();
                int nx = wx - geom.left();
                int ny = wy - geom.top();
                left = qBound(0, nx, width()-10);
                top = qBound(0, ny, height()-10);
                right = qBound(left+minWidth(), left + attr.width, width());
                bottom = qBound(top+minHeight(), top + attr.height, height());
                update();
            }
        }
        XCloseDisplay(d);
#else
        left = width()*0.25;
        right = width()*0.75;
        top = height()*0.25;
        bottom = height()*0.75;
        update();
#endif
    }

    // --- Edge detection using OpenCV ---
    // Convert QPixmap/QImage to cv::Mat (BGR)
    static cv::Mat qimageToMat(const QImage &img) {
        QImage conv = img.convertToFormat(QImage::Format_RGB888);
        cv::Mat mat(conv.height(), conv.width(), CV_8UC3, (void*)conv.constBits(), conv.bytesPerLine());
        cv::Mat matBGR;
        cv::cvtColor(mat, matBGR, cv::COLOR_RGB2BGR);
        return matBGR.clone();
    }

    // Run Canny + findContours, produce vector<QRect> in screen coordinates
    void runEdgeDetection() {
        suggestions.clear();
        currentSuggestion = -1;

        QImage qimg = screen.toImage();
        cv::Mat mat = qimageToMat(qimg);

        cv::Mat gray;
        cv::cvtColor(mat, gray, cv::COLOR_BGR2GRAY);
        cv::Mat blurred;
        cv::GaussianBlur(gray, blurred, cv::Size(5,5), 1.5);
        cv::Mat edges;
        cv::Canny(blurred, edges, 50, 150);

        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
        cv::dilate(edges, edges, kernel);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        struct RectArea { QRect r; int area; };
        std::vector<RectArea> rects;
        for (auto &c : contours) {
            cv::Rect br = cv::boundingRect(c);
            int area = br.width * br.height;
            if (area < 1000) continue;
            QRect qr(br.x, br.y, br.width, br.height);
            rects.push_back({qr, area});
        }

        std::sort(rects.begin(), rects.end(), [](const RectArea &a, const RectArea &b){ return a.area > b.area; });

        int N = std::min<int>(6, rects.size());
        for (int i=0;i<N;i++){
            QRect r = rects[i].r;
            r = r.intersected(QRect(0,0, screen.width(), screen.height()));
            suggestions.append(r);
            QRect e = r.adjusted(-10,-10,10,10).intersected(QRect(0,0, screen.width(), screen.height()));
            suggestions.append(e);
        }

        if (suggestions.isEmpty()) {
            QRect centerRect(screen.width()/4, screen.height()/4, screen.width()/2, screen.height()/2);
            suggestions.append(centerRect);
        }

        currentSuggestion = 0;
        applySuggestion(currentSuggestion);
        update();
    }

    void applySuggestion(int idx) {
        if (idx < 0 || idx >= suggestions.size()) return;
        QRect r = suggestions[idx];
        QSize screenSize = screen.size();
        QSize widgetSize = size();
        double sx = double(widgetSize.width()) / screenSize.width();
        double sy = double(widgetSize.height()) / screenSize.height();
        left = int(r.left() * sx);
        top = int(r.top() * sy);
        right = left + int(r.width() * sx);
        bottom = top + int(r.height() * sy);
        update();
    }

    void selectNextSuggestion() {
        if (suggestions.isEmpty()) return;
        currentSuggestion = (currentSuggestion + 1) % suggestions.size();
        applySuggestion(currentSuggestion);
    }

    // members
    QPixmap screen;
    bool useActive = false;
    int left, right, top, bottom;
    QPoint lastMouse;
    QPoint moveOffset;
    DragZone dragging = DragNone;

    QPushButton *saveBtn;
    QPushButton *cancelBtn;
    QPushButton *autoResetBtn;
    QPushButton *detectBtn;
    QPushButton *nextBtn;
    QLabel *infoLabel;
    QLabel *dimsLabel;

    QVector<QRect> suggestions;
    int currentSuggestion = -1;
};

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    bool captureActiveWindow = false;
    for (int i=1;i<argc;i++){
        QString a = QString::fromLocal8Bit(argv[i]);
        if (a == "--active" || a == "-a") captureActiveWindow = true;
    }

    QScreen *scr = QGuiApplication::screenAt(QCursor::pos());
    if (!scr) scr = QGuiApplication::primaryScreen();

    QPixmap pix;
    if (captureActiveWindow) {
#ifdef Q_OS_UNIX
        // X11 usage kept local here
        Display *d = XOpenDisplay(NULL);
        if (d) {
            Atom a_net_active = XInternAtom(d, "_NET_ACTIVE_WINDOW", True);
            Atom actual;
            int format;
            unsigned long nitems, bytes;
            unsigned char *prop = nullptr;
            Window active = 0;
            if (a_net_active != None) {
                if (XGetWindowProperty(d, DefaultRootWindow(d), a_net_active, 0, (~0L), False,
                                       AnyPropertyType, &actual, &format, &nitems, &bytes, &prop) == Success) {
                    if (prop) {
                        active = *(Window*)prop;
                        XFree(prop);
                    }
                }
            }
            if (active) {
                WId wid = (WId)active;
                pix = scr->grabWindow(wid);
            } else {
                pix = scr->grabWindow(0);
            }
            XCloseDisplay(d);
        } else {
            pix = scr->grabWindow(0);
        }
#else
        pix = scr->grabWindow(0);
#endif
    } else {
        pix = scr->grabWindow(0);
    }

    CropOverlay *overlay = new CropOverlay(pix, captureActiveWindow);
    overlay->showFullScreen();
    overlay->activateWindow();
    overlay->raise();

    return app.exec();
}

#include "qscreenshot.moc"

