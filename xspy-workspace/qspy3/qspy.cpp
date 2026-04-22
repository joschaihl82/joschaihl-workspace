Ich habe den Code so erweitert, dass **für jedes generierte QML-Widget die Eigenschaften `x`, `y`, `width` und `height`** aus den Bildschirm-Extents des jeweiligen Accessible eingefügt werden.  
Aus dem Originaldokument: "Right side contains a QML text output for the selected accessible (root component + children)."

```cpp
// qspy.cpp
// Qt6 + libatspi - Single-file AT-SPI Inspector with hierarchy, live preview, overlay (1s) and QML export
// Build: qmake6 (QT += widgets) or CMake (Qt6 Widgets) and link with pkg-config atspi-2
//
// Notes:
// - Selection in the tree does NOT send focus/highlight to the target application.
// - Overlay highlight is shown for 1 second when the "Highlight" toolbar button is pressed.
// - Right side contains a QML text output for the selected accessible (root component + children).
// - "Save" button opens a Save As dialog with a suggested filename like widgetname.qml.
// - MOC: file ends with #include "qspy.moc"

#include <QtWidgets>
#include <QRegularExpression>
#include <QGuiApplication>
#include <QScreen>
#include <QBuffer>
#include <QImage>
#include <atspi/atspi.h>
#include <map>
#include <vector>
#include <deque>

// ---- Helpers ----
static QString gstr(const char* s){ return s?QString::fromUtf8(s):QString(); }
static QString roleToStr(AtspiRole r){ const char* n = atspi_role_get_name(r); return gstr(n); }
static QString escapeQmlString(const QString &s){
    QString r = s;
    r.replace("\\", "\\\\");
    r.replace("\"", "\\\"");
    r.replace("\n", "\\n");
    return r;
}

// ---- OverlayWindow: translucent always-on-top highlight ----
class OverlayWindow : public QWidget {
    Q_OBJECT
public:
    OverlayWindow() {
        setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::BypassWindowManagerHint);
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_TranslucentBackground);
        hide();
    }
    void showRect(const QRect &r) {
        if (r.isEmpty()) { hide(); return; }
        QRect padded = r.adjusted(-4,-4,4,4);
        setGeometry(padded);
        show();
        raise();
        update();
    }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        QColor fill(0, 120, 215, 40);
        QColor stroke(0, 120, 215, 200);
        p.fillRect(rect(), fill);
        QPen pen(stroke, 3);
        p.setPen(pen);
        p.drawRect(rect().adjusted(1,1,-2,-2));
    }
};

// ---- AtspiManager: enumerate & actions, keep mapping id -> AtspiAccessible* ----
class AtspiManager : public QObject {
    Q_OBJECT
public:
    AtspiManager() { if (!atspi_is_initialized()) atspi_init(); }
    ~AtspiManager() {
        for (auto &p : idMap) g_object_unref(p.second);
    }

    struct Node {
        quintptr id;
        QString name;
        QString role;
        QRect geom;
        std::vector<Node> children;
    };

    // Build hierarchical structure and register accessibles
    std::vector<Node> buildHierarchy() {
        for (auto &p : idMap) g_object_unref(p.second);
        idMap.clear();

        std::vector<Node> out;
        AtspiAccessible *desktop = atspi_get_desktop(0);
        if (!desktop) return out;

        GError *err = nullptr;
        gint napps = atspi_accessible_get_child_count(desktop, &err);
        if (err) { g_error_free(err); err = nullptr; }

        for (gint i=0;i<napps;++i) {
            GError *err2 = nullptr;
            AtspiAccessible *app = atspi_accessible_get_child_at_index(desktop, i, &err2);
            if (err2) { g_error_free(err2); err2 = nullptr; }
            if (!app) continue;
            Node n = traverseNode(app);
            out.push_back(std::move(n));
            g_object_unref(app);
        }

        g_object_unref(desktop);
        return out;
    }

    AtspiAccessible* getById(quintptr id) {
        auto it = idMap.find(id);
        return (it!=idMap.end()) ? it->second : nullptr;
    }

    AtspiAccessible* accessibleById(quintptr id) { return getById(id); }

    bool setFocus(quintptr id) {
        AtspiAccessible* acc = getById(id); if (!acc) return false;
        AtspiAction *act = atspi_accessible_get_action(acc);
        if (act) {
            GError *err = nullptr;
            gboolean ok = atspi_action_do_action(act, 0, &err);
            if (err) { g_error_free(err); }
            return ok;
        }
        return false;
    }

    bool doDefaultAction(quintptr id) {
        AtspiAccessible* acc = getById(id); if (!acc) return false;
        AtspiAction *act = atspi_accessible_get_action(acc);
        if (!act) return false;
        GError *err = nullptr;
        gint n = atspi_action_get_n_actions(act, &err);
        if (err) { g_error_free(err); err = nullptr; }
        if (n<=0) return false;
        gboolean ok = atspi_action_do_action(act, 0, &err);
        if (err) { g_error_free(err); }
        (void)ok;
        return true;
    }

    QString readText(quintptr id) {
        AtspiAccessible* acc = getById(id);
        if (!acc) return {};
        AtspiText *t = ATSPI_TEXT(atspi_accessible_get_text(acc));
        if (!t) return {};
        GError *err = nullptr;
        gchar *s = atspi_text_get_text(t, 0, -1, &err);
        if (err) { g_error_free(err); err = nullptr; }
        QString r = gstr(s);
        if (s) g_free(s);
        return r;
    }

    QStringList readStates(quintptr id) {
        QStringList out;
        AtspiAccessible* acc = getById(id);
        if (!acc) return out;
        AtspiStateSet *ss = atspi_accessible_get_state_set(acc);
        if (!ss) return out;
        GArray *arr = atspi_state_set_get_states(ss);
        if (!arr) return out;
        AtspiStateType *types = (AtspiStateType*)arr->data;
        for (guint i=0;i<arr->len;++i) {
            const char *sname = nullptr;
            switch (types[i]) {
                case ATSPI_STATE_ACTIVE: sname = "active"; break;
                case ATSPI_STATE_FOCUSABLE: sname = "focusable"; break;
                case ATSPI_STATE_FOCUSED: sname = "focused"; break;
                case ATSPI_STATE_ENABLED: sname = "enabled"; break;
                case ATSPI_STATE_SENSITIVE: sname = "sensitive"; break;
                case ATSPI_STATE_VISIBLE: sname = "visible"; break;
                case ATSPI_STATE_SHOWING: sname = "showing"; break;
                case ATSPI_STATE_CHECKED: sname = "checked"; break;
                case ATSPI_STATE_EXPANDED: sname = "expanded"; break;
                case ATSPI_STATE_COLLAPSED: sname = "collapsed"; break;
                case ATSPI_STATE_BUSY: sname = "busy"; break;
                default: sname = nullptr; break;
            }
            if (!sname) {
                gchar *tmp = g_strdup_printf("state_%d", (int)types[i]);
                out << QString::fromUtf8(tmp);
                g_free(tmp);
            } else {
                out << QString::fromUtf8(sname);
            }
        }
        g_array_free(arr, TRUE);
        return out;
    }

    bool readValue(quintptr id, double &outValue, double &outMin, double &outMax) {
        AtspiAccessible* acc = getById(id);
        if (!acc) return false;
        AtspiValue *val = atspi_accessible_get_value(acc);
        if (!val) return false;

        GError *err = nullptr;
        double cur = atspi_value_get_current_value(val, &err);
        if (err) { g_error_free(err); err = nullptr; return false; }
        outValue = cur;

        GError *err2 = nullptr;
        double minv = atspi_value_get_minimum_value(val, &err2);
        if (err2) { g_error_free(err2); err2 = nullptr; minv = 0.0; }
        outMin = minv;

        GError *err3 = nullptr;
        double maxv = atspi_value_get_maximum_value(val, &err3);
        if (err3) { g_error_free(err3); err3 = nullptr; maxv = 100.0; }
        outMax = maxv;

        return true;
    }

    QVariantMap getInfo(quintptr id) {
        QVariantMap m;
        AtspiAccessible* acc = getById(id); if (!acc) return m;

        GError *err = nullptr;
        gchar *name = atspi_accessible_get_name(acc, &err);
        if (err) { g_error_free(err); err = nullptr; }

        GError *err2 = nullptr;
        AtspiRole role = atspi_accessible_get_role(acc, &err2);
        if (err2) { g_error_free(err2); err2 = nullptr; }

        GError *err3 = nullptr;
        gint childCount = atspi_accessible_get_child_count(acc, &err3);
        if (err3) { g_error_free(err3); err3 = nullptr; }

        AtspiComponent *comp = atspi_accessible_get_component(acc);
        long x=0,y=0,w=0,h=0;
        if (comp) {
            GError *err4 = nullptr;
            AtspiRect *r = atspi_component_get_extents(comp, ATSPI_COORD_TYPE_SCREEN, &err4);
            if (r) {
                x = r->x; y = r->y; w = r->width; h = r->height;
                g_free(r);
            }
            if (err4) { g_error_free(err4); }
            g_object_unref(comp);
        }

        m["name"] = gstr(name);
        m["role"] = roleToStr(role);
        m["children"] = (int)childCount;
        m["geom"] = QString("%1,%2 %3x%4").arg(x).arg(y).arg(w).arg(h);
        if (name) g_free(name);
        return m;
    }

    QRect getExtents(quintptr id) {
        AtspiAccessible* acc = getById(id);
        if (!acc) return {};
        AtspiComponent *comp = atspi_accessible_get_component(acc);
        if (!comp) return {};
        long x=0,y=0,w=0,h=0;
        GError *err = nullptr;
        AtspiRect *r = atspi_component_get_extents(comp, ATSPI_COORD_TYPE_SCREEN, &err);
        if (r) {
            x = r->x; y = r->y; w = r->width; h = r->height;
            g_free(r);
        }
        if (err) { g_error_free(err); }
        g_object_unref(comp);
        if (w<=0 || h<=0) return {};
        return QRect(x,y,w,h);
    }

    QString getAttributes(quintptr id) {
        AtspiAccessible* acc = getById(id);
        if (!acc) return {};
        GError *err = nullptr;
        GHashTable *attrs = atspi_accessible_get_attributes(acc, &err);
        if (err) { g_error_free(err); err = nullptr; }
        if (!attrs) return {};
        GHashTableIter iter;
        gpointer keyp, valp;
        g_hash_table_iter_init(&iter, attrs);
        QStringList parts;
        while (g_hash_table_iter_next(&iter, &keyp, &valp)) {
            const char *k = static_cast<const char*>(keyp);
            const char *v = static_cast<const char*>(valp);
            parts << QString("%1:%2").arg(QString::fromUtf8(k), QString::fromUtf8(v));
        }
        g_hash_table_unref(attrs);
        return parts.join(";");
    }

    QString detectWebControlType(quintptr id) {
        AtspiAccessible* acc = getById(id);
        if (!acc) return "generic";

        GError *err = nullptr;
        gchar *name_c = atspi_accessible_get_name(acc, &err);
        if (err) { g_error_free(err); err = nullptr; }
        QString name = gstr(name_c);
        if (name_c) g_free(name_c);

        GError *err2 = nullptr;
        AtspiRole role = atspi_accessible_get_role(acc, &err2);
        if (err2) { g_error_free(err2); err2 = nullptr; }
        QString roleStr = roleToStr(role);

        QString attrs = getAttributes(id);

        QString rn = roleStr.toLower();
        QString nn = name.toLower();
        QString aa = attrs.toLower();

        bool hasText = (atspi_accessible_get_text(acc) != nullptr);
        bool hasValue = (atspi_accessible_get_value(acc) != nullptr);
        bool hasAction = (atspi_accessible_get_action(acc) != nullptr);
        bool hasComponent = (atspi_accessible_get_component(acc) != nullptr);

        if (aa.contains("type=password") || aa.contains("password") || aa.contains("input[type=\"password\"]") || aa.contains("html-type:password")) {
            return "password";
        }
        if (aa.contains("textarea") || aa.contains("html-tag:textarea") || (rn.contains("text") && hasText && !aa.contains("input"))) {
            return "textarea";
        }
        if (aa.contains("input") && aa.contains("type=text")) {
            return "textinput";
        }
        if (aa.contains("role:searchbox") || aa.contains("search") || rn.contains("search")) {
            return "searchbox";
        }
        if (aa.contains("checkbox") || rn.contains("check") || nn.contains("checkbox")) {
            return "checkbox";
        }
        if (aa.contains("radio") || rn.contains("radio") || nn.contains("radio")) {
            return "radio";
        }
        if (aa.contains("combobox") || aa.contains("select") || rn.contains("combo") || rn.contains("select")) {
            return "select";
        }
        if (aa.contains("option") || rn.contains("option") || nn.contains("option")) {
            return "option";
        }
        if (aa.contains("button") || rn.contains("push") || nn.contains("button")) {
            return "button";
        }
        if (aa.contains("link") || rn.contains("link") || nn.startsWith("http") || nn.contains("href")) {
            return "link";
        }
        if (aa.contains("img") || aa.contains("image") || rn.contains("image") || rn.contains("graphic")) {
            return "image";
        }
        if (aa.contains("heading") || rn.contains("heading") || rn.startsWith("h1") || rn.startsWith("h2") || nn.startsWith("h1") ) {
            return "heading";
        }
        if (aa.contains("progress") || rn.contains("progress") || nn.contains("progress")) {
            return "progress";
        }
        if (aa.contains("slider") || rn.contains("slider") || nn.contains("range") || aa.contains("input[type=range]")) {
            return "slider";
        }
        if (rn.contains("table") || aa.contains("table") || nn.contains("table")) {
            return "table";
        }
        if (rn.contains("list") || aa.contains("list") || nn.contains("list")) {
            return "list";
        }
        if (rn.contains("menu") || aa.contains("menu") || nn.contains("menu")) {
            return "menu";
        }
        if (hasValue && hasText) {
            if (aa.contains("spin") || rn.contains("spin") || nn.contains("spin")) return "spin";
            if (aa.contains("date") || nn.contains("date")) return "date";
            return "textinput";
        }
        if (hasText && !hasAction) return "label";
        if (hasAction && !hasText) return "button";
        return "generic";
    }

    // Diagnostic helpers for frame traversal
    static QString atspiNameSafe(AtspiAccessible* a) {
        if (!a) return {};
        GError *err = nullptr;
        gchar *n = atspi_accessible_get_name(a, &err);
        if (err) { g_error_free(err); err = nullptr; }
        QString s = gstr(n);
        if (n) g_free(n);
        return s;
    }
    static QString atspiRoleSafe(AtspiAccessible* a) {
        if (!a) return {};
        GError *err = nullptr;
        AtspiRole r = atspi_accessible_get_role(a, &err);
        if (err) { g_error_free(err); err = nullptr; }
        return roleToStr(r);
    }

    AtspiAccessible* findWebDescendant(AtspiAccessible* root, int maxDepth = 6) {
        if (!root) return nullptr;
        std::deque<std::pair<AtspiAccessible*,int>> q;
        q.emplace_back(root, 0);

        while (!q.empty()) {
            auto [node, depth] = q.front(); q.pop_front();
            if (depth > maxDepth) continue;

            QString role = atspiRoleSafe(node).toLower();
            QString name = atspiNameSafe(node).toLower();

            if (role.contains("document") || role.contains("text") || role.contains("textbox") ||
                role.contains("entry") || role.contains("iframe") || role.contains("web") ||
                role.contains("link") || role.contains("image") || role.contains("table") ||
                name.contains("iframe") || name.contains("document") || name.contains("textbox")) {
                return node;
            }

            GError *err = nullptr;
            gint cnt = atspi_accessible_get_child_count(node, &err);
            if (err) { g_error_free(err); err = nullptr; cnt = 0; }
            for (gint i=0;i<cnt;++i) {
                GError *e2 = nullptr;
                AtspiAccessible *ch = atspi_accessible_get_child_at_index(node, i, &e2);
                if (e2) { g_error_free(e2); e2 = nullptr; }
                if (!ch) continue;
                q.emplace_back(ch, depth+1);
            }
        }
        return nullptr;
    }

private:
    std::map<quintptr, AtspiAccessible*> idMap;

    void registerAcc(AtspiAccessible* acc, quintptr id) {
        if (!acc) return;
        auto it = idMap.find(id);
        if (it==idMap.end()) g_object_ref(acc), idMap[id] = acc;
    }

    Node traverseNode(AtspiAccessible* acc) {
        Node n;
        if (!acc) return n;
        quintptr id = (quintptr)acc;

        GError *err = nullptr;
        gchar *name = atspi_accessible_get_name(acc, &err);
        if (err) { g_error_free(err); err = nullptr; }

        GError *err2 = nullptr;
        AtspiRole role = atspi_accessible_get_role(acc, &err2);
        if (err2) { g_error_free(err2); err2 = nullptr; }

        AtspiComponent *comp = atspi_accessible_get_component(acc);
        long x=0,y=0,w=0,h=0;
        if (comp) {
            GError *err3 = nullptr;
            AtspiRect *r = atspi_component_get_extents(comp, ATSPI_COORD_TYPE_SCREEN, &err3);
            if (r) {
                x = r->x; y = r->y; w = r->width; h = r->height;
                g_free(r);
            }
            if (err3) { g_error_free(err3); }
            g_object_unref(comp);
        }

        n.id = id;
        n.name = gstr(name);
        n.role = roleToStr(role);
        n.geom = QRect(x,y,w,h);
        if (name) g_free(name);

        auto it = idMap.find(id);
        if (it == idMap.end()) {
            g_object_ref(acc);
            idMap[id] = acc;
        }

        QString roleLower = n.role.toLower();
        if (roleLower.contains("frame") || roleLower.contains("embedded")) {
            AtspiAccessible *doc = findWebDescendant(acc, 8);
            if (doc && doc != acc) {
                quintptr did = (quintptr)doc;
                if (idMap.find(did) == idMap.end()) {
                    g_object_ref(doc);
                    idMap[did] = doc;
                }
                GError *e1 = nullptr;
                gchar *dname = atspi_accessible_get_name(doc, &e1);
                if (e1) { g_error_free(e1); e1 = nullptr; }
                GError *e2 = nullptr;
                AtspiRole drole = atspi_accessible_get_role(doc, &e2);
                if (e2) { g_error_free(e2); e2 = nullptr; }
                AtspiComponent *dcomp = atspi_accessible_get_component(doc);
                long dx=0,dy=0,dw=0,dh=0;
                if (dcomp) {
                    GError *e3 = nullptr;
                    AtspiRect *dr = atspi_component_get_extents(dcomp, ATSPI_COORD_TYPE_SCREEN, &e3);
                    if (dr) { dx=dr->x; dy=dr->y; dw=dr->width; dh=dr->height; g_free(dr); }
                    if (e3) { g_error_free(e3); }
                    g_object_unref(dcomp);
                }
                if (dname) { n.name = gstr(dname); g_free(dname); }
                n.role = roleToStr(drole);
                if (dw>0 && dh>0) n.geom = QRect(dx,dy,dw,dh);
            }
        }

        GError *err4 = nullptr;
        gint len = atspi_accessible_get_child_count(acc, &err4);
        if (err4) { g_error_free(err4); err4 = nullptr; }
        for (gint i=0;i<len;++i) {
            GError *err5 = nullptr;
            AtspiAccessible *child = atspi_accessible_get_child_at_index(acc, i, &err5);
            if (err5) { g_error_free(err5); err5 = nullptr; }
            if (child) {
                n.children.push_back(traverseNode(child));
                g_object_unref(child);
            }
        }
        return n;
    }
};

// ---- Role/type mapping and icon generation ----
static QString mapRoleToType(const QString &role, const QString &name) {
    QString r = role.toLower();
    QString n = name.toLower();
    if (r.contains("button") || n.contains("button") || r.contains("push")) return "button";
    if (r.contains("check") || r.contains("checkbox") || n.contains("check")) return "check";
    if (r.contains("radio") || n.contains("radio")) return "radio";
    if (r.contains("slider") || r.contains("scale")) return "slider";
    if (r.contains("progress") || r.contains("meter") || r.contains("progressbar")) return "progress";
    if (r.contains("label") || r.contains("static") || r.contains("text")) {
        if (r.contains("editable") || n.contains("edit") || r.contains("entry") || r.contains("textbox")) return "edit";
        return "label";
    }
    if (r.contains("combo") || r.contains("select") || r.contains("dropdown")) return "combo";
    if (r.contains("list") || r.contains("listbox")) return "list";
    if (r.contains("table") || r.contains("grid")) return "table";
    if (r.contains("tree") || r.contains("treeview")) return "tree";
    if (r.contains("image") || r.contains("icon") || r.contains("graphic") || n.contains("icon")) return "image";
    if (r.contains("menu") || r.contains("menuitem") || r.contains("menubar")) return "menu";
    if (r.contains("toolbar") || r.contains("toolbutton")) return "toolbar";
    if (r.contains("canvas") || r.contains("drawing") || r.contains("graphic")) return "canvas";
    if (r.contains("frame") || r.contains("group") || r.contains("panel") || r.contains("box") || r.contains("container")) return "container";
    if (r.contains("spin") || n.contains("spin")) return "spin";
    return "generic";
}

static QIcon createRoleIcon(const QString &type, const QString &role, const QString &name) {
    QPixmap pm(16,16);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    if (type == "button") {
        p.setBrush(QColor(70,130,180));
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(1,1,14,14,3,3);
        p.setPen(Qt::white);
        p.drawText(pm.rect(), Qt::AlignCenter, "B");
    } else if (type == "check") {
        p.setPen(QPen(Qt::black,1));
        p.drawRect(2,2,12,12);
        p.drawLine(4,8,7,11);
        p.drawLine(7,11,12,5);
    } else if (type == "radio") {
        p.setPen(QPen(Qt::black,1));
        p.drawEllipse(2,2,12,12);
        p.setBrush(Qt::black);
        p.drawEllipse(6,6,4,4);
    } else if (type == "slider") {
        p.setPen(QPen(QColor(80,80,80),2));
        p.drawLine(2,8,14,8);
        p.setBrush(QColor(200,200,200));
        p.drawEllipse(6,5,4,4);
    } else if (type == "progress") {
        p.setBrush(QColor(0,150,0));
        p.setPen(Qt::NoPen);
        p.drawRect(1,6,12,4);
        p.setPen(QPen(Qt::black,1));
        p.drawRect(1,6,12,4);
    } else if (type == "image") {
        p.setBrush(QColor(220,220,220));
        p.setPen(QPen(Qt::black,1));
        p.drawRect(1,1,14,14);
        p.drawLine(3,11,7,6);
        p.drawEllipse(10,3,3,3);
    } else if (type == "table") {
        p.setPen(QPen(Qt::black,1));
        for (int i=0;i<3;i++) p.drawLine(2,4+i*4,14,4+i*4);
        for (int c=0;c<3;c++) p.drawLine(4+c*4,2,4+c*4,14);
    } else if (type == "tree") {
        p.setPen(QPen(Qt::black,1));
        p.drawRect(2,2,12,12);
        p.drawLine(4,6,12,6);
        p.drawLine(4,10,12,10);
    } else if (type == "combo") {
        p.setPen(QPen(Qt::black,1));
        p.drawRect(2,3,10,10);
        p.drawPolygon(QPolygon() << QPoint(13,6) << QPoint(15,9) << QPoint(11,9));
    } else if (type == "list") {
        p.setPen(QPen(Qt::black,1));
        p.drawRect(2,2,12,12);
        p.drawRect(4,4,8,2);
        p.drawRect(4,8,8,2);
    } else if (type == "menu") {
        p.setPen(QPen(Qt::black,1));
        p.drawRect(2,3,12,3);
        p.drawRect(2,8,12,3);
    } else if (type == "toolbar") {
        p.setPen(QPen(Qt::black,1));
        p.drawRect(2,3,12,3);
        p.drawRect(2,8,4,4);
        p.drawRect(8,8,4,4);
    } else if (type == "edit") {
        p.setPen(QPen(Qt::black,1));
        p.drawRect(2,5,12,6);
        p.drawText(pm.rect(), Qt::AlignCenter, "T");
    } else if (type == "generic") {
        p.setBrush(QColor(120,120,120));
        p.setPen(Qt::NoPen);
        p.drawRect(2,2,12,12);
        p.setPen(Qt::white);
        p.drawText(pm.rect(), Qt::AlignCenter, "?");
    } else {
        p.setBrush(QColor(150,150,150));
        p.setPen(Qt::NoPen);
        p.drawRect(2,2,12,12);
    }
    p.end();
    return QIcon(pm);
}

// ---- MainWindow: Tree UI + actions + overlay integration + live preview pane + QML output ----
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent=nullptr) : QMainWindow(parent) {
        mgr = new AtspiManager();

        // Left: tree
        tree = new QTreeWidget;
        tree->setColumnCount(3);
        tree->setHeaderLabels({"Name","Role","Geom"});
        tree->setUniformRowHeights(true);
        tree->setSelectionMode(QAbstractItemView::SingleSelection);

        // Middle: preview area (kept small)
        previewRoot = new QWidget;
        QVBoxLayout *pvLayout = new QVBoxLayout(previewRoot);
        pvLayout->setContentsMargins(8,8,8,8);
        previewIcon = new QLabel;
        previewIcon->setFixedSize(64,64);
        previewIcon->setAlignment(Qt::AlignCenter);
        pvLayout->addWidget(previewIcon, 0, Qt::AlignHCenter);

        previewTitle = new QLabel;
        QFont f = previewTitle->font();
        f.setBold(true);
        previewTitle->setFont(f);
        previewTitle->setAlignment(Qt::AlignCenter);
        pvLayout->addWidget(previewTitle);

        previewStack = new QStackedWidget;
        demoEmpty = new QWidget;
        demoButton = new QPushButton;
        demoCheck = new QCheckBox;
        demoRadio = new QRadioButton;
        demoProgress = new QProgressBar;
        demoProgress->setRange(0,100);
        demoSlider = new QSlider(Qt::Horizontal);
        demoSlider->setRange(0,100);
        demoEdit = new QLineEdit;
        demoText = new QTextEdit;
        demoCombo = new QComboBox;
        demoList = new QListWidget;
        demoTable = new QTableWidget(0,3);
        demoTable->setHorizontalHeaderLabels(QStringList{"A","B","C"});
        demoTree = new QTreeWidget;
        demoTree->setHeaderHidden(true);
        demoImage = new QLabel;
        demoImage->setAlignment(Qt::AlignCenter);
        demoImage->setMinimumSize(120,80);
        demoMenuPreview = new QListWidget;
        demoToolbarPreview = new QWidget;
        QHBoxLayout *tbL = new QHBoxLayout(demoToolbarPreview);
        tbL->setContentsMargins(0,0,0,0);
        tbL->addWidget(new QPushButton("A"));
        tbL->addWidget(new QPushButton("B"));
        tbL->addWidget(new QPushButton("C"));

        previewStack->addWidget(demoEmpty);
        previewStack->addWidget(demoButton);
        previewStack->addWidget(demoCheck);
        previewStack->addWidget(demoRadio);
        previewStack->addWidget(demoProgress);
        previewStack->addWidget(demoSlider);
        previewStack->addWidget(demoEdit);
        previewStack->addWidget(demoText);
        previewStack->addWidget(demoCombo);
        previewStack->addWidget(demoList);
        previewStack->addWidget(demoTable);
        previewStack->addWidget(demoTree);
        previewStack->addWidget(demoImage);
        previewStack->addWidget(demoMenuPreview);
        previewStack->addWidget(demoToolbarPreview);

        pvLayout->addWidget(previewStack, 1);
        previewRoot->setLayout(pvLayout);

        // Right: QML text output + Save button
        qmlRoot = new QWidget;
        QVBoxLayout *qmlLayout = new QVBoxLayout(qmlRoot);
        qmlLayout->setContentsMargins(6,6,6,6);

        QHBoxLayout *saveRow = new QHBoxLayout;
        saveButton = new QPushButton("Save QML");
        saveRow->addWidget(saveButton);
        saveRow->addStretch();
        qmlLayout->addLayout(saveRow);

        qmlText = new QTextEdit;
        qmlText->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
        qmlLayout->addWidget(qmlText, 1);
        qmlRoot->setLayout(qmlLayout);

        connect(saveButton, &QPushButton::clicked, this, &MainWindow::onSaveQml);

        // Splitter: tree | preview | qml
        QSplitter *splitMain = new QSplitter;
        splitMain->addWidget(tree);
        splitMain->addWidget(previewRoot);
        splitMain->addWidget(qmlRoot);
        splitMain->setStretchFactor(0, 3);
        splitMain->setStretchFactor(1, 1);
        splitMain->setStretchFactor(2, 2);
        setCentralWidget(splitMain);

        // Toolbar
        auto tb = addToolBar("Main");
        tb->addAction("Refresh", this, &MainWindow::refresh);
        tb->addAction("Focus", this, &MainWindow::focusSelected);
        tb->addAction("Invoke", this, &MainWindow::invokeSelected);
        tb->addAction("GetText", this, &MainWindow::getText);
        tb->addAction("Info", this, &MainWindow::showInfo);

        // Highlight button: show overlay for 1 second
        highlightAction = tb->addAction("Highlight (1s)");
        connect(highlightAction, &QAction::triggered, this, [this](){
            quintptr id = selectedIid();
            if (!id) return;
            QRect r = mgr->getExtents(id);
            if (r.isEmpty()) return;
            overlay->showRect(r);
            QTimer::singleShot(1000, overlay, &OverlayWindow::hide);
        });

        // Overlay timer not used for automatic updates; overlay shown only on demand
        overlay = new OverlayWindow();

        // Selection: only update preview and QML (do NOT focus or highlight)
        connect(tree, &QTreeWidget::itemSelectionChanged, this, [this](){
            updatePreviewForSelection();
            updateQmlForSelection();
        });

        tree->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(tree, &QTreeWidget::customContextMenuRequested, this, &MainWindow::onContextMenu);

        resize(1400,820);
        refresh();
    }
    ~MainWindow() override {
        delete overlay;
        delete mgr;
    }

private slots:
    void refresh() {
        tree->clear();
        auto roots = mgr->buildHierarchy();
        for (auto &r : roots) {
            QTreeWidgetItem *it = makeTreeItem(r);
            tree->addTopLevelItem(it);
        }
        tree->expandToDepth(0);
        previewStack->setCurrentWidget(demoEmpty);
        previewIcon->clear();
        previewTitle->setText("");
        qmlText->clear();
    }

    QTreeWidgetItem* makeTreeItem(const AtspiManager::Node &n) {
        QTreeWidgetItem *it = new QTreeWidgetItem();
        it->setText(0, n.name.isEmpty() ? QString("[no name]") : n.name);
        it->setText(1, n.role);
        it->setText(2, QString("%1,%2 %3x%4").arg(n.geom.x()).arg(n.geom.y()).arg(n.geom.width()).arg(n.geom.height()));
        it->setData(0, Qt::UserRole, QVariant::fromValue((qulonglong)n.id));
        QString type = mapRoleToType(n.role, n.name);

        QString nodeNameLower = n.name.toLower();
        if (nodeNameLower.contains("firefox") || nodeNameLower.contains("chromium") || nodeNameLower.contains("chrome") || nodeNameLower.contains("brave") || nodeNameLower.contains("edge")) {
            QString webType = mgr->detectWebControlType(n.id);
            if (!webType.isEmpty()) type = webType;
        }

        it->setIcon(0, createRoleIcon(type, n.role, n.name));
        for (auto &c : n.children) it->addChild(makeTreeItem(c));
        return it;
    }

    quintptr selectedIid() {
        auto items = tree->selectedItems();
        if (items.isEmpty()) return 0;
        QVariant v = items.first()->data(0, Qt::UserRole);
        return (quintptr)v.toULongLong();
    }

    void focusSelected() {
        quintptr id = selectedIid();
        if (!id) return;
        if (!mgr->setFocus(id)) QMessageBox::warning(this,"Focus","Failed to set focus");
    }
    void invokeSelected() {
        quintptr id = selectedIid();
        if (!id) return;
        if (!mgr->doDefaultAction(id)) QMessageBox::warning(this,"Invoke","No default action / failed");
    }
    void getText() {
        quintptr id = selectedIid();
        if (!id) return;
        QString s = mgr->readText(id);
        QMessageBox::information(this,"Text", s.isEmpty() ? "(no text)" : s);
    }
    void showInfo() {
        quintptr id = selectedIid();
        if (!id) return;
        auto info = mgr->getInfo(id);
        QString out;
        for (auto it = info.begin(); it!=info.end(); ++it) out += QString("%1: %2\n").arg(it.key(), it.value().toString());
        QMessageBox::information(this,"Info", out);
    }

    void updateOverlayForSelection() {
        // Not used for automatic updates in this variant.
    }

    // Preview update (no focus)
    void updatePreviewForSelection() {
        auto items = tree->selectedItems();
        if (items.isEmpty()) {
            previewStack->setCurrentWidget(demoEmpty);
            previewIcon->clear();
            previewTitle->setText("");
            return;
        }
        QTreeWidgetItem *it = items.first();
        QString name = it->text(0);
        QString role = it->text(1);
        quintptr iid = (quintptr)it->data(0, Qt::UserRole).toULongLong();

        QString type = mapRoleToType(role, name);
        bool isBrowser = false;
        AtspiAccessible *acc = mgr->accessibleById(iid);
        if (acc) {
            AtspiAccessible *cur = acc;
            while (cur) {
                GError *e = nullptr;
                AtspiRole r = atspi_accessible_get_role(cur, &e);
                if (e) { g_error_free(e); e = nullptr; }
                GError *e2 = nullptr;
                gchar *aname = atspi_accessible_get_name(cur, &e2);
                if (e2) { g_error_free(e2); e2 = nullptr; }
                QString an = gstr(aname).toLower();
                if (aname) g_free(aname);
                if (an.contains("firefox") || an.contains("chromium") || an.contains("chrome") || an.contains("brave") || an.contains("edge")) { isBrowser = true; break; }
                GError *e3 = nullptr;
                AtspiAccessible *parent = atspi_accessible_get_parent(cur, &e3);
                if (e3) { g_error_free(e3); e3 = nullptr; parent = nullptr; }
                if (cur != acc) g_object_unref(cur);
                cur = parent;
            }
            if (cur && cur != acc) g_object_unref(cur);
        }
        if (isBrowser) {
            QString webType = mgr->detectWebControlType(iid);
            if (!webType.isEmpty()) type = webType;
        }

        previewTitle->setText(name.isEmpty() ? role : QString("%1 — %2").arg(name, role));
        previewIcon->setPixmap(createRoleIcon(type, role, name).pixmap(64,64));

        if (!acc) { previewStack->setCurrentWidget(demoEmpty); return; }

        double curv=0.0, minv=0.0, maxv=100.0;
        bool hasValue = mgr->readValue(iid, curv, minv, maxv);
        QString text = mgr->readText(iid);
        QStringList states = mgr->readStates(iid);

        GError *err = nullptr;
        gint childCount = atspi_accessible_get_child_count(acc, &err);
        if (err) { g_error_free(err); err = nullptr; childCount = 0; }

        if (hasValue && (type == "progress" || type == "slider" || type == "spin")) {
            double span = (maxv - minv);
            double pct = (span > 0.0) ? ((curv - minv) / span * 100.0) : 0.0;
            int ipct = qBound(0, (int)qRound(pct), 100);
            demoProgress->setRange(0,100);
            demoProgress->setValue(ipct);
            demoSlider->setRange(0,100);
            demoSlider->setValue(ipct);
            if (type == "progress") previewStack->setCurrentWidget(demoProgress);
            else previewStack->setCurrentWidget(demoSlider);
            return;
        }

        if (!text.isEmpty() && (type == "edit" || type == "label" || type == "generic" || type == "password" || type == "textarea")) {
            if (type == "password") {
                demoEdit->setEchoMode(QLineEdit::Password);
                demoEdit->setText(text);
                previewStack->setCurrentWidget(demoEdit);
            } else if (type == "edit") {
                demoEdit->setEchoMode(QLineEdit::Normal);
                demoEdit->setText(text);
                previewStack->setCurrentWidget(demoEdit);
            } else if (type == "textarea") {
                demoText->setPlainText(text);
                previewStack->setCurrentWidget(demoText);
            } else {
                demoText->setPlainText(text);
                previewStack->setCurrentWidget(demoText);
            }
            return;
        }

        if (type == "combo" || type == "list" || type == "menu" || type == "select") {
            if (type == "combo" || type == "select") {
                demoCombo->clear();
                for (gint i=0;i<childCount;++i) {
                    GError *e2 = nullptr;
                    AtspiAccessible *ch = atspi_accessible_get_child_at_index(acc, i, &e2);
                    if (e2) { g_error_free(e2); e2 = nullptr; }
                    if (!ch) continue;
                    GError *e3 = nullptr;
                    gchar *cname = atspi_accessible_get_name(ch, &e3);
                    if (e3) { g_error_free(e3); e3 = nullptr; }
                    demoCombo->addItem(gstr(cname));
                    if (cname) g_free(cname);
                    g_object_unref(ch);
                }
                previewStack->setCurrentWidget(demoCombo);
                return;
            } else {
                demoList->clear();
                demoMenuPreview->clear();
                for (gint i=0;i<childCount;++i) {
                    GError *e2 = nullptr;
                    AtspiAccessible *ch = atspi_accessible_get_child_at_index(acc, i, &e2);
                    if (e2) { g_error_free(e2); e2 = nullptr; }
                    if (!ch) continue;
                    GError *e3 = nullptr;
                    gchar *cname = atspi_accessible_get_name(ch, &e3);
                    if (e3) { g_error_free(e3); e3 = nullptr; }
                    QString s = gstr(cname);
                    if (type == "list") demoList->addItem(s);
                    else demoMenuPreview->addItem(s);
                    if (cname) g_free(cname);
                    g_object_unref(ch);
                }
                previewStack->setCurrentWidget(type == "list" ? (QWidget*)demoList : (QWidget*)demoMenuPreview);
                return;
            }
        }

        if (type == "table") {
            demoTable->clearContents();
            int rows = qMin(8, childCount);
            demoTable->setRowCount(rows);
            for (int r=0;r<rows;++r) {
                GError *e2 = nullptr;
                AtspiAccessible *rowAcc = atspi_accessible_get_child_at_index(acc, r, &e2);
                if (e2) { g_error_free(e2); e2 = nullptr; }
                if (!rowAcc) continue;
                GError *e3 = nullptr;
                gint cols = atspi_accessible_get_child_count(rowAcc, &e3);
                if (e3) { g_error_free(e3); e3 = nullptr; }
                for (gint c=0;c<cols && c<demoTable->columnCount();++c) {
                    GError *e4 = nullptr;
                    AtspiAccessible *cell = atspi_accessible_get_child_at_index(rowAcc, c, &e4);
                    if (e4) { g_error_free(e4); e4 = nullptr; }
                    if (!cell) continue;
                    GError *e5 = nullptr;
                    gchar *cname = atspi_accessible_get_name(cell, &e5);
                    if (e5) { g_error_free(e5); e5 = nullptr; }
                    demoTable->setItem(r, c, new QTableWidgetItem(gstr(cname)));
                    if (cname) g_free(cname);
                    g_object_unref(cell);
                }
                g_object_unref(rowAcc);
            }
            previewStack->setCurrentWidget(demoTable);
            return;
        }

        if (type == "tree") {
            demoTree->clear();
            for (gint i=0;i<childCount;++i) {
                GError *e2 = nullptr;
                AtspiAccessible *ch = atspi_accessible_get_child_at_index(acc, i, &e2);
                if (e2) { g_error_free(e2); e2 = nullptr; }
                if (!ch) continue;
                GError *e3 = nullptr;
                gchar *cname = atspi_accessible_get_name(ch, &e3);
                if (e3) { g_error_free(e3); e3 = nullptr; }
                QTreeWidgetItem *ti = new QTreeWidgetItem(QStringList{gstr(cname)});
                demoTree->addTopLevelItem(ti);
                if (cname) g_free(cname);
                g_object_unref(ch);
            }
            previewStack->setCurrentWidget(demoTree);
            return;
        }

        if (type == "image") {
            QIcon theme = QIcon::fromTheme("image-x-generic");
            QPixmap pm = theme.pixmap(120,80);
            if (pm.isNull()) {
                pm = QPixmap(120,80);
                pm.fill(Qt::lightGray);
                QPainter p(&pm);
                p.drawText(pm.rect(), Qt::AlignCenter, text.isEmpty() ? "IMG" : text);
            }
            demoImage->setPixmap(pm.scaled(demoImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            previewStack->setCurrentWidget(demoImage);
            return;
        }

        QString summary;
        if (!text.isEmpty()) summary += text + "\n";
        if (!states.isEmpty()) summary += QString("States: %1").arg(states.join(", "));
        if (summary.isEmpty()) summary = QString("Role: %1").arg(role);
        demoText->setPlainText(summary);
        previewStack->setCurrentWidget(demoText);
    }

    // Build QML representation for selected accessible and its children
    void updateQmlForSelection() {
        quintptr iid = selectedIid();
        if (!iid) { qmlText->clear(); return; }
        AtspiAccessible *acc = mgr->accessibleById(iid);
        if (!acc) { qmlText->clear(); return; }

        QString qml = generateQmlForAccessible(acc, 0);
        qmlText->setPlainText(qml);
    }

    // Capture the screen extents for an accessible, scale to 1/10 and return base64 PNG (no prefix)
    QString captureScaledImageDataUrl(AtspiAccessible *acc) {
        if (!acc) return QString();
        quintptr id = (quintptr)acc;
        QRect r = mgr->getExtents(id);
        if (r.isEmpty()) return QString();

        QScreen *screen = QGuiApplication::primaryScreen();
        if (!screen) return QString();

        // grabWindow with global coordinates
        QPixmap pm = screen->grabWindow(0, r.x(), r.y(), r.width(), r.height());
        if (pm.isNull()) return QString();

        int sw = qMax(1, r.width() / 10);
        int sh = qMax(1, r.height() / 10);
        QPixmap scaled = pm.scaled(sw, sh, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QImage img = scaled.toImage();

        QBuffer buf;
        buf.open(QIODevice::WriteOnly);
        img.save(&buf, "PNG");
        QByteArray ba = buf.data().toBase64();
        return QString::fromLatin1(ba);
    }

    // Generate a simple QML component tree from an AtspiAccessible*
    // Now includes x, y, width, height for each generated widget when available.
    QString generateQmlForAccessible(AtspiAccessible *acc, int depth) {
        if (!acc) return QString();
        if (depth > 12) return QString();

        GError *e = nullptr;
        gchar *name_c = atspi_accessible_get_name(acc, &e);
        if (e) { g_error_free(e); e = nullptr; }
        QString name = gstr(name_c);
        if (name_c) g_free(name_c);

        GError *e2 = nullptr;
        AtspiRole role = atspi_accessible_get_role(acc, &e2);
        if (e2) { g_error_free(e2); e2 = nullptr; }
        QString roleStr = roleToStr(role);

        quintptr id = (quintptr)acc;
        QString type = mgr->detectWebControlType(id);
        if (type.isEmpty()) type = mapRoleToType(roleStr, name);

        // Get extents for geometry properties
        QRect geom = mgr->getExtents(id);
        bool hasGeom = !geom.isEmpty();
        QString geomProps;
        if (hasGeom) {
            geomProps = QString("x: %1\n    y: %2\n    width: %3\n    height: %4")
                        .arg(geom.x()).arg(geom.y()).arg(geom.width()).arg(geom.height());
        }

        QString element;
        QString props;

        QString idName = name.trimmed();
        if (idName.isEmpty()) idName = roleStr;
        QString safeId = idName;
        safeId.replace(QRegularExpression("[^A-Za-z0-9_]"), "_");
        if (safeId.isEmpty()) safeId = QString("comp_%1").arg((qulonglong)id);

        QString label = escapeQmlString(name.isEmpty() ? roleStr : name);

        if (type == "button") {
            element = "Button";
            props = QString("text: \"%1\"").arg(label);
        } else if (type == "check") {
            element = "CheckBox";
            props = QString("text: \"%1\"").arg(label);
        } else if (type == "radio") {
            element = "RadioButton";
            props = QString("text: \"%1\"").arg(label);
        } else if (type == "edit" || type == "textinput") {
            element = "TextField";
            props = QString("text: \"%1\"").arg(label);
        } else if (type == "password") {
            element = "TextField";
            props = QString("echoMode: TextInput.Password\n            text: \"%1\"").arg(label);
        } else if (type == "textarea") {
            element = "TextArea";
            props = QString("text: \"%1\"").arg(label);
        } else if (type == "combo" || type == "select") {
            element = "ComboBox";
            props = QString("model: [\"%1\"]").arg(label);
        } else if (type == "list") {
            element = "ListView";
            props = QString("model: [\"%1\"]\n            delegate: Text { text: modelData }").arg(label);
        } else if (type == "image") {
            element = "Image";
            // Try to capture and embed a 10x smaller image as data URL
            QString b64 = captureScaledImageDataUrl(acc);
            if (!b64.isEmpty()) {
                props = QString("source: \"data:image/png;base64,%1\" // scaled 10x smaller").arg(b64);
            } else {
                props = QString("source: \"\" // image placeholder; accessible name: \"%1\"").arg(label);
            }
        } else if (type == "table") {
            element = "TableView";
            props = QString("// Table with accessible name: \"%1\"").arg(label);
        } else if (type == "progress") {
            element = "ProgressBar";
            props = QString("value: 0 // accessible name: \"%1\"").arg(label);
        } else if (type == "slider") {
            element = "Slider";
            props = QString("from: 0; to: 100; value: 0 // accessible name: \"%1\"").arg(label);
        } else if (type == "link") {
            element = "Text";
            props = QString("text: \"%1\"; color: \"blue\"; font.underline: true").arg(label);
        } else if (type == "heading") {
            element = "Text";
            props = QString("text: \"%1\"; font.bold: true; font.pointSize: 16").arg(label);
        } else {
            element = "Item";
            props = QString("// accessible role: %1; name: \"%2\"").arg(roleStr, label);
        }

        // Merge geometry props into props if present
        QString mergedProps;
        if (!geomProps.isEmpty()) {
            if (!props.isEmpty()) mergedProps = geomProps + "\n    " + props;
            else mergedProps = geomProps;
        } else {
            mergedProps = props;
        }

        QString childrenQml;
        GError *err = nullptr;
        gint cnt = atspi_accessible_get_child_count(acc, &err);
        if (err) { g_error_free(err); err = nullptr; cnt = 0; }
        for (gint i=0;i<cnt;++i) {
            GError *e3 = nullptr;
            AtspiAccessible *ch = atspi_accessible_get_child_at_index(acc, i, &e3);
            if (e3) { g_error_free(e3); e3 = nullptr; }
            if (!ch) continue;
            QString childQ = generateQmlForAccessible(ch, depth+1);
            if (!childQ.isEmpty()) {
                // indent child QML by 4 spaces
                QStringList lines = childQ.split('\n');
                for (QString &ln : lines) ln = QString("    ") + ln;
                childrenQml += "\n" + lines.join("\n");
            }
            g_object_unref(ch);
        }

        QString qml;
        if (element == "Item") {
            if (mergedProps.isEmpty())
                qml = QString("Item {\n    id: %1\n%2\n}").arg(safeId, childrenQml);
            else
                qml = QString("Item {\n    id: %1\n    %2\n%3\n}").arg(safeId, mergedProps, childrenQml);
        } else {
            if (mergedProps.isEmpty())
                qml = QString("%1 {\n    id: %2\n%3\n}").arg(element, safeId, childrenQml);
            else
                qml = QString("%1 {\n    id: %2\n    %3\n%4\n}").arg(element, safeId, mergedProps, childrenQml);
        }

        return qml;
    }

    void onContextMenu(const QPoint &pt) {
        QTreeWidgetItem *it = tree->itemAt(pt);
        if (!it) return;
        quintptr id = (quintptr)it->data(0, Qt::UserRole).toULongLong();
        QMenu m;
        QAction *aFocus = m.addAction("Focus");
        QAction *aInvoke = m.addAction("Invoke");
        QAction *aText = m.addAction("Get Text");
        QAction *aInfo = m.addAction("Info");
        QAction *sel = m.exec(tree->viewport()->mapToGlobal(pt));
        if (!sel) return;
        if (sel == aFocus) { if (!mgr->setFocus(id)) QMessageBox::warning(this,"Focus","Failed"); }
        else if (sel == aInvoke) { if (!mgr->doDefaultAction(id)) QMessageBox::warning(this,"Invoke","Failed"); }
        else if (sel == aText) { QString s = mgr->readText(id); QMessageBox::information(this,"Text", s.isEmpty() ? "(no text)" : s); }
        else if (sel == aInfo) { auto info = mgr->getInfo(id); QString out; for (auto it = info.begin(); it!=info.end(); ++it) out += QString("%1: %2\n").arg(it.key(), it.value().toString()); QMessageBox::information(this,"Info", out); }
    }

    void onSaveQml() {
        quintptr iid = selectedIid();
        if (!iid) {
            QMessageBox::information(this, "Save QML", "No item selected.");
            return;
        }
        AtspiAccessible *acc = mgr->accessibleById(iid);
        if (!acc) {
            QMessageBox::information(this, "Save QML", "Selected accessible not available.");
            return;
        }
        GError *e = nullptr;
        gchar *name_c = atspi_accessible_get_name(acc, &e);
        if (e) { g_error_free(e); e = nullptr; }
        QString name = gstr(name_c);
        if (name_c) g_free(name_c);
        if (name.trimmed().isEmpty()) {
            GError *e2 = nullptr;
            AtspiRole r = atspi_accessible_get_role(acc, &e2);
            if (e2) { g_error_free(e2); e2 = nullptr; }
            name = roleToStr(r);
        }
        QString safe = name.trimmed();
        safe.replace(QRegularExpression("[^A-Za-z0-9_]"), "_");
        if (safe.isEmpty()) safe = QString("widget_%1").arg((qulonglong)iid);
        QString suggested = safe + ".qml";

        QString fileName = QFileDialog::getSaveFileName(this, "Save QML", suggested, "QML Files (*.qml);;All Files (*)");
        if (fileName.isEmpty()) return;

        QString qml = qmlText->toPlainText();
        QFile f(fileName);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Save QML", "Failed to open file for writing.");
            return;
        }
        QTextStream out(&f);
        out << qml;
        f.close();
        QMessageBox::information(this, "Save QML", QString("Saved to %1").arg(fileName));
    }

private:
    AtspiManager *mgr;
    QTreeWidget *tree;
    OverlayWindow *overlay;
    QAction *highlightAction;

    // preview
    QWidget *previewRoot;
    QLabel *previewIcon;
    QLabel *previewTitle;
    QStackedWidget *previewStack;
    QWidget *demoEmpty;
    QPushButton *demoButton;
    QCheckBox *demoCheck;
    QRadioButton *demoRadio;
    QProgressBar *demoProgress;
    QSlider *demoSlider;
    QLineEdit *demoEdit;
    QTextEdit *demoText;
    QComboBox *demoCombo;
    QListWidget *demoList;
    QTableWidget *demoTable;
    QTreeWidget *demoTree;
    QLabel *demoImage;
    QListWidget *demoMenuPreview;
    QWidget *demoToolbarPreview;

    // QML output
    QWidget *qmlRoot;
    QTextEdit *qmlText;
    QPushButton *saveButton;
};

// ---- main ----
int main(int argc, char **argv) {
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("qspy - Qt AT-SPI Inspector (live) - QML Export");
    w.show();
    return a.exec();
}

#include "qspy.moc"
