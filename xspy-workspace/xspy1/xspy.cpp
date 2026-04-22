// xspy.cpp
// Amalgamated prototype: X11 + AT-SPI + Qt6 -> QML
// Event-based _NET_ACTIVE_WINDOW + FocusIn/FocusOut + Map/Unmap handling
// Always-on-top transparent overlay in bottom-right quadrant showing generated QML source (80% opacity, 8pt monospace).
// All capture roles enabled by default; no UI controls.
// Build example:
// g++ xspy.cpp -std=c++17 `pkg-config --cflags --libs Qt6Widgets Qt6Concurrent x11 xcomposite xrender atspi-2 gobject-2.0 glib-2.0 dbus-1` -o xspy
//
// Requirements:
// - Qt6 (widgets, concurrent)
// - libx11-dev, libxcomposite-dev, libxrender-dev
// - libatspi2.0-dev, libglib2.0-dev, libdbus-1-dev
// - pkg-config

#include <QApplication>
#include <QWidget>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QImage>
#include <QDir>
#include <QStandardPaths>
#include <QProgressDialog>
#include <QFuture>
#include <QtConcurrent>
#include <QMutex>
#include <QMutexLocker>
#include <QRandomGenerator>
#include <QThread>
#include <QSocketNotifier>
#include <QScreen>
#include <QFont>

#include <glib.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xrender.h>

#include <atspi/atspi.h>

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <functional>
#include <set>
#include <atomic>

// -------------------- Data structures --------------------
struct Node {
    long x=0, y=0, w=0, h=0;
    QString role;
    QString text;
    QString toolkit;
    bool checked = false;
    bool hasChecked = false;
    double value = 0.0;
    bool hasValue = false;
    std::vector<std::shared_ptr<Node>> children;
    QString imagePath;
};

// -------------------- Utilities --------------------
static QString qmlEscape(const QString &s){
    QString r = s;
    r.replace("\\", "\\\\");
    r.replace("\"", "\\\"");
    r.replace("\n", "\\n");
    return r;
}

// Forward declarations (to ensure functions are visible where used)
static QString generateQml(const std::shared_ptr<Node> &root);
static QImage captureRegion(Display* dpy, Window rootWindow, int rx, int ry, int rw, int rh);
static QString saveImageToTemp(const QImage &img);

// -------------------- X11 helpers --------------------
static Display* openDisplay(){ return XOpenDisplay(nullptr); }

static Window getActiveWindow(Display* dpy){
    if(!dpy) return 0;
    Atom netActive = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", True);
    if(netActive == None) return 0;
    Atom actualType;
    int actualFormat;
    unsigned long nitems, bytesAfter;
    unsigned char *prop = nullptr;
    if(XGetWindowProperty(dpy, DefaultRootWindow(dpy), netActive, 0, (~0L), False, AnyPropertyType,
                          &actualType, &actualFormat, &nitems, &bytesAfter, &prop) == Success && prop){
        if(nitems >= 1){
            Window *win = (Window*)prop;
            Window w = win[0];
            XFree(prop);
            return w;
        }
        XFree(prop);
    }
    return 0;
}

static QString getWindowName(Display* dpy, Window w){
    if(!dpy || !w) return {};
    Atom netName = XInternAtom(dpy, "_NET_WM_NAME", False);
    Atom utf8 = XInternAtom(dpy, "UTF8_STRING", False);
    Atom actualType;
    int actualFormat;
    unsigned long nitems, bytesAfter;
    unsigned char *prop = nullptr;
    if(XGetWindowProperty(dpy, w, netName, 0, (~0L), False, utf8,
                          &actualType, &actualFormat, &nitems, &bytesAfter, &prop) == Success && prop){
        QString s = QString::fromUtf8((char*)prop);
        XFree(prop);
        return s;
    }
    XTextProperty tp;
    if(XGetWMName(dpy, w, &tp) && tp.value){
        QString s = QString::fromUtf8((char*)tp.value);
        if(tp.value) XFree(tp.value);
        return s;
    }
    return {};
}

static QString getWindowClass(Display* dpy, Window w){
    if(!dpy || !w) return {};
    XClassHint hint;
    if(XGetClassHint(dpy, w, &hint)){
        QString cls = QString::fromUtf8(hint.res_class ? hint.res_class : "");
        if(hint.res_name) XFree(hint.res_name);
        if(hint.res_class) XFree(hint.res_class);
        return cls;
    }
    return {};
}

static void traverseX(Display* dpy, Window w, std::shared_ptr<Node> node){
    if(!dpy || !w || !node) return;
    XWindowAttributes attr;
    if(!XGetWindowAttributes(dpy, w, &attr)) return;
    node->x = attr.x; node->y = attr.y; node->w = attr.width; node->h = attr.height;
    node->text = getWindowName(dpy, w);
    node->toolkit = getWindowClass(dpy, w);
    Window root, parent; Window *children = nullptr; unsigned int nchildren = 0;
    if(XQueryTree(dpy, w, &root, &parent, &children, &nchildren)){
        for(unsigned int i=0;i<nchildren;i++){
            auto childNode = std::make_shared<Node>();
            traverseX(dpy, children[i], childNode);
            node->children.push_back(childNode);
        }
        if(children) XFree(children);
    }
}

// -------------------- AT-SPI helpers --------------------
static bool atspiExtractStateAndValue(AtspiAccessible *acc, std::shared_ptr<Node> node){
    if(!acc || !node) return false;
    // role and name
    gchar *role_c = atspi_accessible_get_role_name(acc, nullptr);
    gchar *name_c = atspi_accessible_get_name(acc, nullptr);

    // component extents
    AtspiComponent *comp = ATSPI_COMPONENT(atspi_accessible_get_component(acc));
    AtspiRect *rect = atspi_component_get_extents(comp, ATSPI_COORD_TYPE_SCREEN, nullptr);

    bool hasRect = false;
    if(rect){
        node->x = rect->x;
        node->y = rect->y;
        node->w = rect->width;
        node->h = rect->height;
        hasRect = true;
        g_free(rect);
    }

    if(role_c){ node->role = QString::fromUtf8(role_c); g_free(role_c); }
    if(name_c){ node->text = QString::fromUtf8(name_c); g_free(name_c); }

    // states
    AtspiStateSet *states = atspi_accessible_get_state_set(acc);
    if(states){
        if(atspi_state_set_contains(states, ATSPI_STATE_CHECKED)){
            node->checked = true; node->hasChecked = true;
        } else if(atspi_state_set_contains(states, ATSPI_STATE_SELECTED)){
            node->checked = true; node->hasChecked = true;
        } else {
            node->hasChecked = false;
        }
        g_object_unref(states);
    }

    // value
    AtspiValue *val = atspi_accessible_get_value(acc);
    if(val){
        GError *errVal = nullptr;
        gdouble cur = atspi_value_get_current_value(val, &errVal);
        if(!errVal){
            node->value = (double)cur;
            node->hasValue = true;
        } else {
            g_clear_error(&errVal);
        }
    }

    // children
    int nchildren = atspi_accessible_get_child_count(acc, nullptr);
    for(int i=0;i<nchildren;i++){
        AtspiAccessible *child = atspi_accessible_get_child_at_index(acc, i, nullptr);
        if(child){
            auto childNode = std::make_shared<Node>();
            atspiExtractStateAndValue(child, childNode);
            node->children.push_back(childNode);
            g_object_unref(child);
        }
    }

    return hasRect;
}

static std::vector<std::shared_ptr<Node>> findAtspiNodesForRect(long x, long y, long w, long h){
    std::vector<std::shared_ptr<Node>> results;
    AtspiAccessible *desktop = atspi_get_desktop(0);
    if(!desktop) return results;
    int nApps = atspi_accessible_get_child_count(desktop, nullptr);
    for(int ai=0; ai<nApps; ++ai){
        AtspiAccessible *app = atspi_accessible_get_child_at_index(desktop, ai, nullptr);
        if(!app) continue;
        int nWin = atspi_accessible_get_child_count(app, nullptr);
        for(int wi=0; wi<nWin; ++wi){
            AtspiAccessible *win = atspi_accessible_get_child_at_index(app, wi, nullptr);
            if(!win) continue;
            auto rootNode = std::make_shared<Node>();
            if(atspiExtractStateAndValue(win, rootNode)){
                QRect r1(x,y,w,h);
                QRect r2(rootNode->x, rootNode->y, rootNode->w, rootNode->h);
                if(r1.intersects(r2)){
                    results.push_back(rootNode);
                }
            }
            g_object_unref(win);
        }
        g_object_unref(app);
    }
    g_object_unref(desktop);
    return results;
}

static void mergeAtspiIntoX(std::shared_ptr<Node> xnode, const std::vector<std::shared_ptr<Node>> &atspiRoots){
    if(!xnode) return;
    QRect xr(xnode->x, xnode->y, xnode->w, xnode->h);
    std::shared_ptr<Node> best;
    int bestArea = 0;
    std::function<void(const std::shared_ptr<Node>&)> search;
    search = [&](const std::shared_ptr<Node>& a){
        QRect ar(a->x, a->y, a->w, a->h);
        QRect inter = xr.intersected(ar);
        int area = inter.width() * inter.height();
        if(area > bestArea){
            bestArea = area;
            best = a;
        }
        for(auto &c: a->children) search(c);
    };
    for(auto &root: atspiRoots) search(root);
    if(best && bestArea>0){
        if(!best->role.isEmpty()) xnode->role = best->role.toLower();
        if(!best->text.isEmpty()) xnode->text = best->text;
        if(!best->toolkit.isEmpty()) xnode->toolkit = best->toolkit;
        if(best->hasChecked){ xnode->checked = best->checked; xnode->hasChecked = true; }
        if(best->hasValue){ xnode->value = best->value; xnode->hasValue = true; }
    }
    for(auto &c: xnode->children) mergeAtspiIntoX(c, atspiRoots);
}

// -------------------- Pixel capture helpers --------------------
static QString saveImageToTemp(const QImage &img){
    QString tmpDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    if(tmpDir.isEmpty()) tmpDir = "/tmp";
    QString filePath = tmpDir + "/xspy_capture_" + QString::number(QRandomGenerator::global()->generate()) + ".png";
    if(img.save(filePath, "PNG")) return filePath;
    return {};
}

static QImage ximageToQImage(XImage *ximg){
    if(!ximg) return QImage();
    int width = ximg->width;
    int height = ximg->height;
    QImage img(width, height, QImage::Format_ARGB32);
    unsigned long rmask = ximg->red_mask;
    unsigned long gmask = ximg->green_mask;
    unsigned long bmask = ximg->blue_mask;
    int rshift = 0, gshift = 0, bshift = 0;
    unsigned long tmp;
    tmp = rmask; while((tmp & 1) == 0 && tmp) { tmp >>= 1; ++rshift; }
    tmp = gmask; while((tmp & 1) == 0 && tmp) { tmp >>= 1; ++gshift; }
    tmp = bmask; while((tmp & 1) == 0 && tmp) { tmp >>= 1; ++bshift; }
    if(ximg->bits_per_pixel < 24) return QImage();
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            unsigned long pixel = XGetPixel(ximg, x, y);
            unsigned int r = (pixel & ximg->red_mask) >> rshift;
            unsigned int g = (pixel & ximg->green_mask) >> gshift;
            unsigned int b = (pixel & ximg->blue_mask) >> bshift;
            if(ximg->red_mask != 0 && ximg->red_mask != 0xFF) {
                unsigned long rmax = ximg->red_mask >> rshift;
                if(rmax) r = (r * 255) / rmax;
            }
            if(ximg->green_mask != 0 && ximg->green_mask != 0xFF) {
                unsigned long gmax = ximg->green_mask >> gshift;
                if(gmax) g = (g * 255) / gmax;
            }
            if(ximg->blue_mask != 0 && ximg->blue_mask != 0xFF) {
                unsigned long bmax = ximg->blue_mask >> bshift;
                if(bmax) b = (b * 255) / bmax;
            }
            img.setPixel(x, y, qRgba(r,g,b,0xFF));
        }
    }
    return img;
}

static QImage captureRegion(Display* dpy, Window rootWindow, int rx, int ry, int rw, int rh){
    if(!dpy || rw<=0 || rh<=0) return QImage();
    int event_base, error_base;
    if(XCompositeQueryExtension(dpy, &event_base, &error_base)){
        Pixmap pix = XCompositeNameWindowPixmap(dpy, rootWindow);
        if(pix){
            XImage *ximg = XGetImage(dpy, pix, rx, ry, rw, rh, AllPlanes, ZPixmap);
            if(ximg){
                QImage img = ximageToQImage(ximg);
                XDestroyImage(ximg);
                XFreePixmap(dpy, pix);
                return img;
            }
            XFreePixmap(dpy, pix);
        }
    }
    XImage *ximg = XGetImage(dpy, rootWindow, rx, ry, rw, rh, AllPlanes, ZPixmap);
    if(!ximg) return QImage();
    QImage img = ximageToQImage(ximg);
    XDestroyImage(ximg);
    return img;
}

// -------------------- QML generation --------------------
static QString generateTableModelQml(const std::shared_ptr<Node> &tableNode, const QString &modelName){
    QString q;
    q += "ListModel {\n";
    q += "  id: " + modelName + "\n";
    for(const auto &row : tableNode->children){
        q += "  ListElement {\n";
        int c = 0;
        for (const auto &cell : row->children){
            q += "    col" + QString::number(c) + ": \"" + qmlEscape(cell->text) + "\"\n";
            ++c;
        }
        q += "  }\n";
    }
    q += "}\n";
    return q;
}

static QString generateTreeModelQml_recursive(const std::shared_ptr<Node> &node){
    QString s;
    s += "{ \"text\": \"" + qmlEscape(node->text) + "\"";
    if(!node->children.empty()){
        s += ", \"children\": [";
        bool first = true;
        for(auto &c: node->children){
            if(!first) s += ", ";
            s += generateTreeModelQml_recursive(c);
            first = false;
        }
        s += "]";
    }
    s += " }";
    return s;
}

static QString generateTreeModelQml(const std::shared_ptr<Node> &treeNode, const QString &propName){
    QString arr = "[";
    bool first = true;
    for(auto &c: treeNode->children){
        if(!first) arr += ", ";
        arr += generateTreeModelQml_recursive(c);
        first = false;
    }
    arr += "]";
    QString q;
    q += "property var " + propName + " : " + arr + "\n";
    return q;
}

static QString nodeToQml(const std::shared_ptr<Node> &n, int indent=2){
    QString pad(indent, ' ');
    QString s;
    QString rx = QString::number(n->x);
    QString ry = QString::number(n->y);
    QString rw = QString::number(n->w);
    QString rh = QString::number(n->h);
    QString txt = qmlEscape(n->text);
    QString role = n->role.toLower();

    if(role.contains("push") || role.contains("button")) role = "button";
    else if(role.contains("menu")) role = "menu";
    else if(role.contains("menu item") || role.contains("menuitem")) role = "menuitem";
    else if(role.contains("text") && role.contains("editable")) role = "editable";
    else if(role.contains("text") || role.contains("label")) role = "text";
    else if(role.contains("check")) role = "checkbox";
    else if(role.contains("radio")) role = "radiobutton";
    else if(role.contains("combo") || role.contains("combobox")) role = "combobox";
    else if(role.contains("slider") || role.contains("scale")) role = "slider";
    else if(role.contains("table")) role = "table";
    else if(role.contains("tree")) role = "tree";
    else if(role.contains("image")) role = "image";
    else if(role.contains("progress")) role = "progressbar";
    else if(role.contains("toolbar")) role = "toolbar";
    else if(role.contains("tab")) role = "tab";

    if(!n->imagePath.isEmpty()){
        s += pad + "Image { x: " + rx + "; y: " + ry + "; width: " + rw + "; height: " + rh + "; source: \"file://" + n->imagePath + "\" }\n";
        return s;
    }

    if(role == "button"){
        s += pad + "Button { x: " + rx + "; y: " + ry + "; width: " + rw + "; height: " + rh + "; text: \"" + txt + "\" }\n";
    } else if(role == "menu"){
        s += pad + "Menu { x: " + rx + "; y: " + ry + " }\n";
        for(auto &c: n->children) s += nodeToQml(c, indent+2);
    } else if(role == "menuitem"){
        s += pad + "MenuItem { text: \"" + txt + "\" }\n";
    } else if(role == "editable"){
        s += pad + "TextField { x: " + rx + "; y: " + ry + "; width: " + rw + "; height: " + rh + "; text: \"" + txt + "\" }\n";
    } else if(role == "text"){
        s += pad + "Text { x: " + rx + "; y: " + ry + "; text: \"" + txt + "\" }\n";
    } else if(role == "checkbox"){
        QString checked = n->hasChecked ? (n->checked ? "true" : "false") : "false";
        s += pad + "CheckBox { x: " + rx + "; y: " + ry + "; text: \"" + txt + "\"; checked: " + checked + " }\n";
    } else if(role == "radiobutton"){
        QString checked = n->hasChecked ? (n->checked ? "true" : "false") : "false";
        s += pad + "RadioButton { x: " + rx + "; y: " + ry + "; text: \"" + txt + "\"; checked: " + checked + " }\n";
    } else if(role == "combobox"){
        s += pad + "ComboBox { x: " + rx + "; y: " + ry + "; width: " + rw + "; height: " + rh + " }\n";
    } else if(role == "slider"){
        QString val = n->hasValue ? QString::number(n->value) : "0";
        s += pad + "Slider { x: " + rx + "; y: " + ry + "; width: " + rw + "; value: " + val + " }\n";
    } else if(role == "table"){
        static int tableCounter = 0;
        QString modelName = "tableModel" + QString::number(tableCounter++);
        s += pad + "// Table model for detected table\n";
        s += pad + generateTableModelQml(n, modelName).replace("\n", "\n" + pad);
        s += pad + "TableView { x: " + rx + "; y: " + ry + "; width: " + rw + "; height: " + rh + "; model: " + modelName + " }\n";
    } else if(role == "tree"){
        static int treeCounter = 0;
        QString propName = "treeData" + QString::number(treeCounter++);
        s += pad + "// Tree model for detected tree\n";
        s += pad + generateTreeModelQml(n, propName).replace("\n", "\n" + pad);
        s += pad + "TreeView { x: " + rx + "; y: " + ry + "; width: " + rw + "; height: " + rh + "; model: " + propName + " }\n";
    } else if(role == "image"){
        s += pad + "Image { x: " + rx + "; y: " + ry + "; width: " + rw + "; height: " + rh + " }\n";
    } else if(role == "progressbar"){
        QString val = n->hasValue ? QString::number(n->value) : "0";
        s += pad + "ProgressBar { x: " + rx + "; y: " + ry + "; width: " + rw + "; height: " + rh + "; value: " + val + " }\n";
    } else if(role == "toolbar"){
        s += pad + "ToolBar { x: " + rx + "; y: " + ry + "; width: " + rw + "; height: " + rh + " }\n";
    } else if(role == "tab"){
        s += pad + "TabBar { x: " + rx + "; y: " + ry + "; width: " + rw + " }\n";
    } else {
        s += pad + "Rectangle { x: " + rx + "; y: " + ry + "; width: " + rw + "; height: " + rh + "; color: \"transparent\" }\n";
        if(!n->text.isEmpty()) s += pad + "Text { text: \"" + txt + "\" }\n";
        for(auto &c: n->children) s += nodeToQml(c, indent+2);
    }
    return s;
}

static QString generateQml(const std::shared_ptr<Node> &root){
    QString q;
    q += "import QtQuick 2.15\nimport QtQuick.Controls 2.15\n\n";
    q += "ApplicationWindow {\n";
    q += "  visible: true\n";
    q += "  width: " + QString::number(root->w) + "\n";
    q += "  height: " + QString::number(root->h) + "\n";
    q += "\n";
    for(auto &c: root->children) q += nodeToQml(c, 2);
    q += "}\n";
    return q;
}

// -------------------- Main capture & overlay logic --------------------
// All capture roles are enabled by default (no checkboxes).
// The overlay is a frameless, translucent, always-on-top window in the bottom-right quadrant
// that displays the generated QML source immediately when a new active window is detected.

class QmlOverlay : public QWidget {
public:
    QmlOverlay(QScreen *screen, QWidget *parent = nullptr) : QWidget(parent) {
        // Tool + StayOnTop + Bypass WM to reduce being sent to background
        setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_ShowWithoutActivating);
        setFocusPolicy(Qt::NoFocus);
        setAttribute(Qt::WA_AlwaysStackOnTop);

        text = new QPlainTextEdit(this);
        text->setReadOnly(true);
        text->setFrameStyle(QFrame::NoFrame);
        QFont f("Monospace");
        f.setStyleHint(QFont::Monospace);
        f.setPointSize(8);
        text->setFont(f);
        text->setStyleSheet("background: transparent; color: black;");
        setWindowOpacity(0.8); // 80% opacity
        screenRef = screen;
        adjustToScreen();
    }

    void adjustToScreen(){
        if(!screenRef) screenRef = QGuiApplication::primaryScreen();
        QRect g = screenRef->geometry();
        int w = g.width() / 2;   // quadrant width (half width)
        int h = g.height() / 2;  // quadrant height (half height)
        int x = g.x() + g.width() - w; // bottom-right quadrant
        int y = g.y() + g.height() - h;
        setGeometry(x, y, w, h);
        text->setGeometry(0, 0, w, h);
    }

    void showQmlText(const QString &qml){
        text->setPlainText(qml);
        adjustToScreen();
        show();
        raise();
    }

private:
    QPlainTextEdit *text;
    QScreen *screenRef = nullptr;
};

// Helper: collect all nodes for capture (all roles enabled)
static void collectCaptureCandidatesAll(const std::shared_ptr<Node> &node, std::vector<std::shared_ptr<Node>> &out, int minSize){
    if(!node) return;
    if(node->w >= minSize && node->h >= minSize){
        out.push_back(node);
    }
    for(auto &c: node->children) collectCaptureCandidatesAll(c, out, minSize);
}

static QString generateQmlForWindow(Display* dpy, Window targetWindow, QWidget *parent){
    if(!dpy || !targetWindow) return {};
    auto rootNode = std::make_shared<Node>();
    traverseX(dpy, targetWindow, rootNode);

    if(!atspi_init()){
        // continue with X11-only fallback
    }

    auto atspiRoots = findAtspiNodesForRect(rootNode->x, rootNode->y, rootNode->w, rootNode->h);
    mergeAtspiIntoX(rootNode, atspiRoots);

    // collect all candidates (no role filtering)
    std::vector<std::shared_ptr<Node>> candidates;
    collectCaptureCandidatesAll(rootNode, candidates, 12);

    // progress dialog (modal to parent if provided)
    QProgressDialog progress("Capturing controls...", "Abort", 0, (int)candidates.size(), parent);
    progress.setWindowModality(parent ? Qt::WindowModal : Qt::NonModal);
    progress.setMinimumDuration(200);
    progress.setValue(0);

    std::atomic<bool> cancelled(false);
    std::function<bool()> cancelCb = [&](){ return cancelled.load(); };

    QMutex writeMutex;

    auto progressCb = [&](int done, int total){
        if(parent){
            QMetaObject::invokeMethod(parent, [done,total,&progress](){
                progress.setMaximum(total);
                progress.setValue(done);
            }, Qt::QueuedConnection);
        } else {
            // If no parent, update progress directly (safe from worker via queued invocation)
            QMetaObject::invokeMethod(qApp, [done,total,&progress](){
                progress.setMaximum(total);
                progress.setValue(done);
            }, Qt::QueuedConnection);
        }
    };

    QFuture<void> future = QtConcurrent::run([&](){
        Display* workerDpy = XOpenDisplay(nullptr);
        if(!workerDpy) workerDpy = dpy;
        Window rootWin = DefaultRootWindow(workerDpy);
        int total = (int)candidates.size();
        for(int i=0;i<total;i++){
            if(cancelCb && cancelCb()) break;
            auto node = candidates[i];
            QImage img = captureRegion(workerDpy, rootWin, node->x, node->y, node->w, node->h);
            if(!img.isNull()){
                QString path = saveImageToTemp(img);
                if(!path.isEmpty()){
                    QMutexLocker locker(&writeMutex);
                    node->imagePath = path;
                }
            }
            progressCb(i+1, total);
        }
        if(workerDpy && workerDpy != dpy) XCloseDisplay(workerDpy);
    });

    QObject::connect(&progress, &QProgressDialog::canceled, [&](){
        cancelled.store(true);
    });

    while(!future.isFinished()){
        QApplication::processEvents();
        if(progress.wasCanceled()) break;
        QThread::msleep(50);
    }
    future.waitForFinished();

    QString qml = generateQml(rootNode);
    return qml;
}

// -------------------- Main --------------------
int main(int argc, char** argv){
    QApplication app(argc, argv);

    // Overlay only
    QmlOverlay overlay(QGuiApplication::primaryScreen());
    overlay.hide();

    // Open X display
    Display* dpy = openDisplay();
    if(!dpy){
        QMessageBox::critical(nullptr, "Error", "Cannot open X display. Is DISPLAY set?");
        return 1;
    }

    // Initialize AT-SPI (best-effort)
    if(!atspi_init()){
        // continue with X11-only fallback
    }

    // Track last active window
    Window lastActive = 0;
    QString lastQml;

    // Root window and atoms
    Window rootWin = DefaultRootWindow(dpy);
    Atom netActiveAtom = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False);

    // Select input on root window for property changes and substructure/focus events
    long eventMask = PropertyChangeMask | SubstructureNotifyMask | FocusChangeMask;
    XSelectInput(dpy, rootWin, eventMask);

    // QSocketNotifier to watch X connection fd
    int xfd = ConnectionNumber(dpy);
    QSocketNotifier *notifier = new QSocketNotifier(xfd, QSocketNotifier::Read, QApplication::instance());

    QObject::connect(notifier, &QSocketNotifier::activated, [&](int){
        while(XPending(dpy)){
            XEvent ev;
            XNextEvent(dpy, &ev);
            switch(ev.type){
                case PropertyNotify: {
                    XPropertyEvent *pe = (XPropertyEvent*)&ev;
                    if(pe->atom == netActiveAtom){
                        Window active = getActiveWindow(dpy);
                        if(active && active != lastActive){
                            lastActive = active;
                            QString qml = generateQmlForWindow(dpy, active, nullptr);
                            if(!qml.isEmpty()){
                                lastQml = qml;
                                overlay.showQmlText(qml);
                            } else {
                                overlay.hide();
                            }
                        }
                    }
                    break;
                }
                case MapNotify: {
                    XMapEvent *me = (XMapEvent*)&ev;
                    Window active = getActiveWindow(dpy);
                    if(active && (me->window == active) && active != lastActive){
                        lastActive = active;
                        QString qml = generateQmlForWindow(dpy, active, nullptr);
                        if(!qml.isEmpty()){
                            lastQml = qml;
                            overlay.showQmlText(qml);
                        } else {
                            overlay.hide();
                        }
                    }
                    break;
                }
                case UnmapNotify: {
                    XUnmapEvent *ue = (XUnmapEvent*)&ev;
                    Window active = getActiveWindow(dpy);
                    if(!active || ue->window == lastActive){
                        lastActive = 0;
                        overlay.hide();
                    }
                    break;
                }
                case FocusIn: {
                    Window active = getActiveWindow(dpy);
                    if(active && active != lastActive){
                        lastActive = active;
                        QString qml = generateQmlForWindow(dpy, active, nullptr);
                        if(!qml.isEmpty()){
                            lastQml = qml;
                            overlay.showQmlText(qml);
                        } else {
                            overlay.hide();
                        }
                    }
                    break;
                }
                case FocusOut: {
                    Window active = getActiveWindow(dpy);
                    if(!active){
                        lastActive = 0;
                        overlay.hide();
                    }
                    break;
                }
                default:
                    break;
            }
        }
    });

    notifier->setEnabled(true);

    int ret = app.exec();
    XCloseDisplay(dpy);
    return ret;
}

