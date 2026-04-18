// main.cpp
// Qt6 Node Editor with toolbar and shader-editor nodes
// Updated: shader nodes render an embedded, editable QPlainTextEdit via QGraphicsProxyWidget.
// - Editor uses a fixed-width font at 12pt
// - Editor area shows up to 10 lines and 80 characters per line (desired size)
// - Node size is adjusted so the editor area does not overlap slot labels
// Build with CMake (AUTOMOC enabled).

#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsObject>
#include <QGraphicsPathItem>
#include <QGraphicsProxyWidget>
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QPainterPathStroker>
#include <QRubberBand>
#include <QTimer>
#include <QKeyEvent>
#include <QScrollBar>
#include <QDialog>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QRegularExpression>
#include <QMessageBox>
#include <QObject>
#include <QFontMetrics>
#include <QFontDatabase>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QScrollBar>
#include <cmath>
#include <algorithm>
#include <vector>

 // ----------------------------- Forward declarations ----------------------
class NodeItem;
class EdgeItem;
class NodeView;

// ----------------------------- NodeItem --------------------------------
class NodeItem : public QGraphicsObject {
    Q_OBJECT
public:
    struct Slot {
        QString name;
        bool isOutput; // true => right side (output), false => left side (input)
    };

    NodeItem(int id, const QString &title, QGraphicsItem* parent = nullptr)
        : QGraphicsObject(parent), m_id(id), m_title(title)
    {
        // default size; shader nodes will adjust when editor text is set
        m_size = QSizeF(200, 100);
        setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
        setAcceptHoverEvents(true);
        m_snapToGrid = true;

        // default editor font (monospace)
        m_editorFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        m_editorFont.setPointSize(12);

        m_editorWidget = nullptr;
        m_editorProxy = nullptr;
    }

    ~NodeItem() override {
        // Proxy is child of this QGraphicsItem; deleting it explicitly is safe.
        if (m_editorProxy) {
            // The proxy owns the widget; delete proxy will delete widget.
            delete m_editorProxy;
            m_editorProxy = nullptr;
            m_editorWidget = nullptr;
        }
    }

    QRectF boundingRect() const override {
        return QRectF(QPointF(0,0), m_size).adjusted(-8,-8,8,8);
    }

    void paint(QPainter* p, const QStyleOptionGraphicsItem* option, QWidget*) override {
        Q_UNUSED(option);
        p->setRenderHint(QPainter::Antialiasing);
        QColor bg = isSelected() ? QColor(80,120,200) : QColor(60,60,60);
        p->setBrush(bg);
        p->setPen(QPen(QColor(30,30,30), 2));
        p->drawRoundedRect(QRectF(QPointF(0,0), m_size), 6, 6);

        // Title bar
        QRectF titleRect(0,0,m_size.width(), 24);
        p->setBrush(isSelected() ? QColor(70,100,180) : QColor(45,45,45));
        p->setPen(Qt::NoPen);
        p->drawRoundedRect(titleRect, 6, 6);
        p->setPen(Qt::white);
        p->setFont(QFont("Arial", 10, QFont::Bold));
        p->drawText(titleRect.adjusted(6,0,0,0), Qt::AlignVCenter | Qt::AlignLeft, m_title);

        // Draw slots
        const qreal r = 6.0;
        int inputs = 0, outputs = 0;
        for (auto &s : m_slots) { if (!s.isOutput) ++inputs; else ++outputs; }

        // draw input slots
        if (inputs > 0) {
            for (int i = 0, idx = 0; i < (int)m_slots.size(); ++i) {
                if (!m_slots[i].isOutput) {
                    qreal y = slotYPosition(idx, inputs);
                    p->setBrush(QColor(200,80,80));
                    p->setPen(Qt::NoPen);
                    p->drawEllipse(QPointF(0, y), r, r);
                    p->setPen(Qt::white);
                    p->setFont(QFont("Arial", 8));
                    p->drawText(QRectF(8, y-8, m_size.width()/2 - 8, 16), Qt::AlignLeft | Qt::AlignVCenter, m_slots[i].name);
                    ++idx;
                }
            }
        }

        // draw output slots
        if (outputs > 0) {
            for (int i = 0, idx = 0; i < (int)m_slots.size(); ++i) {
                if (m_slots[i].isOutput) {
                    qreal y = slotYPosition(idx, outputs);
                    p->setBrush(QColor(80,200,80));
                    p->setPen(Qt::NoPen);
                    p->drawEllipse(QPointF(m_size.width(), y), r, r);
                    p->setPen(Qt::white);
                    p->setFont(QFont("Arial", 8));
                    p->drawText(QRectF(m_size.width()/2, y-8, m_size.width()/2 - 8, 16), Qt::AlignRight | Qt::AlignVCenter, m_slots[i].name);
                    ++idx;
                }
            }
        }

        // Optionally draw a subtle border for the editor area (the actual editor is a widget)
        if (m_isShaderNode) {
            // compute editor rectangle centered horizontally, below title bar
            QFont fmFont = m_editorFont;
            QFontMetrics fm(fmFont);
            int charWidth = fm.horizontalAdvance(QLatin1Char('M'));
            int lineHeight = fm.lineSpacing();

            qreal editorWidth = charWidth * 80.0;
            qreal editorHeight = lineHeight * 10.0;

            const qreal marginX = 12.0;
            const qreal marginY = 8.0;

            qreal availW = m_size.width() - marginX * 2.0;
            qreal availH = m_size.height() - 24.0 - marginY * 2.0;
            qreal drawW = std::min(editorWidth, availW);
            qreal drawH = std::min(editorHeight, availH);

            qreal x = (m_size.width() - drawW) * 0.5;
            qreal y = 24.0 + marginY;

            QRectF editorRect(x, y, drawW, drawH);

            p->setPen(QPen(QColor(80,80,80), 1.0));
            p->setBrush(Qt::NoBrush);
            p->drawRect(editorRect);
        }

        // hovered border
        if (m_hovered) {
            p->setPen(QPen(QColor(200,200,200,80), 1.5));
            p->setBrush(Qt::NoBrush);
            p->drawRoundedRect(QRectF(QPointF(0,0), m_size), 6, 6);
        }
    }

    int id() const { return m_id; }

    QPointF slotScenePos(int slotIndex) const {
        if (slotIndex < 0 || slotIndex >= (int)m_slots.size()) return mapToScene(boundingRect().center());
        bool isOut = m_slots[slotIndex].isOutput;
        int count = 0;
        for (int i = 0; i < slotIndex; ++i) if (m_slots[i].isOutput == isOut) ++count;
        int total = 0;
        for (auto &s : m_slots) if (s.isOutput == isOut) ++total;
        qreal y = slotYPosition(count, total);
        QPointF local = QPointF(isOut ? m_size.width() : 0, y);
        return mapToScene(local);
    }

    std::pair<bool,int> slotAtScenePos(const QPointF& scenePos, qreal radius = 10.0) const {
        for (int i = 0; i < (int)m_slots.size(); ++i) {
            QPointF s = slotScenePos(i);
            if (QLineF(s, scenePos).length() <= radius) return {true, i};
        }
        return {false, -1};
    }

    // Parse GLSL and set slots; also store editor text and mark as shader node
    void setEditorCodeAndSlots(const QString& glslCode, bool isShaderNode) {
        m_isShaderNode = isShaderNode;
        m_editorCode = glslCode;
        // parse 'in' and 'out'
        QRegularExpression re(R"(^\s*(in|out)\s+\w+\s+([A-Za-z_]\w*)\s*;)", QRegularExpression::MultilineOption);
        QRegularExpressionMatchIterator it = re.globalMatch(glslCode);

        std::vector<Slot> newSlots;
        while (it.hasNext()) {
            QRegularExpressionMatch m = it.next();
            QString dir = m.captured(1);
            QString name = m.captured(2);
            Slot s;
            s.name = name;
            s.isOutput = (dir == "out");
            newSlots.push_back(s);
        }

        if (!newSlots.empty()) {
            m_slots.clear();
            for (auto &s : newSlots) m_slots.push_back(s);
        }

        // create editor widget if shader node
        if (m_isShaderNode) {
            createEmbeddedEditorWidget();
            if (m_editorWidget) m_editorWidget->setPlainText(m_editorCode);
        }

        // adjust node size to fit editor area and slots
        adjustSizeForEditor();
        update();
        emit positionChanged(this);
    }

    void setEditorCode(const QString& glslCode) {
        m_editorCode = glslCode;
        if (m_isShaderNode) {
            createEmbeddedEditorWidget();
            if (m_editorWidget) m_editorWidget->setPlainText(m_editorCode);
        }
        adjustSizeForEditor();
        update();
        emit positionChanged(this);
    }

    void addSlot(const QString& name, bool isOutput) {
        Slot s; s.name = name; s.isOutput = isOutput;
        m_slots.push_back(s);
        adjustSizeForEditor();
        update();
        emit positionChanged(this);
    }

    // Renamed to avoid conflict with Qt 'slots' keyword
    const std::vector<Slot>& slotList() const { return m_slots; }

Q_SIGNALS:
    void doubleClicked(NodeItem*);
    void positionChanged(NodeItem*);

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* ev) override {
        Q_UNUSED(ev);
        emit doubleClicked(this);
    }

    void hoverEnterEvent(QGraphicsSceneHoverEvent* ev) override {
        Q_UNUSED(ev);
        m_hovered = true;
        update();
    }
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* ev) override {
        Q_UNUSED(ev);
        m_hovered = false;
        update();
    }

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override {
        if (change == ItemPositionHasChanged) {
            emit positionChanged(this);
        } else if (change == ItemPositionChange && m_snapToGrid) {
            QPointF newPos = value.toPointF();
            const qreal grid = 8.0;
            qreal x = std::round(newPos.x() / grid) * grid;
            qreal y = std::round(newPos.y() / grid) * grid;
            return QPointF(x, y);
        }
        return QGraphicsObject::itemChange(change, value);
    }

private:
    qreal slotYPosition(int index, int total) const {
        qreal top = 28.0;
        qreal bottom = m_size.height() - 8.0;
        if (total <= 1) return (top + bottom) * 0.5;
        qreal span = bottom - top;
        return top + (index + 0.5) * (span / total);
    }

    // Compute editor metrics and adjust node size so editor area (80 chars x 10 lines) fits
    void adjustSizeForEditor() {
        if (!m_isShaderNode) {
            // keep default or slot-driven size
            int inputs = 0, outputs = 0;
            for (auto &s : m_slots) { if (!s.isOutput) ++inputs; else ++outputs; }
            int maxSlots = std::max(1, std::max(inputs, outputs));
            m_size.setHeight(std::max(88.0, 24.0 + maxSlots * 20.0));
            m_size.setWidth(std::max(160.0, m_size.width()));
            // update embedded editor geometry if present
            if (m_editorProxy) updateEmbeddedEditorGeometry();
            return;
        }

        // Editor desired dimensions: 80 chars x 10 lines using fixed-width font at 12pt
        QFont fmFont = m_editorFont;
        QFontMetrics fm(fmFont);
        int charWidth = fm.horizontalAdvance(QLatin1Char('M')); // approx monospace width
        int lineHeight = fm.lineSpacing();

        qreal editorWidth = charWidth * 80.0;
        qreal editorHeight = lineHeight * 10.0;

        // margins inside node
        const qreal marginX = 12.0;
        const qreal marginY = 8.0;
        qreal contentWidth = editorWidth + marginX * 2.0;
        qreal contentHeight = editorHeight + marginY * 2.0 + 24.0; // include title bar

        // ensure space for slot labels on left/right: reserve half width for labels if many slots
        qreal slotLabelReserve = 120.0; // safe reserve for labels and slot circles
        qreal minWidth = contentWidth + slotLabelReserve;

        // also ensure a minimum width
        minWidth = std::max(minWidth, 240.0);

        m_size.setWidth(minWidth);
        m_size.setHeight(std::max(contentHeight, 120.0));

        // if editor widget exists, update its geometry to match new node size
        if (m_editorProxy) updateEmbeddedEditorGeometry();
    }

    // Create the embedded editor widget and proxy (if not already created)
    void createEmbeddedEditorWidget() {
        if (m_editorProxy) return; // already created

        // create plain text editor
        m_editorWidget = new QPlainTextEdit();
        m_editorWidget->setPlainText(m_editorCode);
        m_editorWidget->setFont(m_editorFont);
        m_editorWidget->setLineWrapMode(QPlainTextEdit::NoWrap);
        m_editorWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        m_editorWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // optional: small margins and background to match painted style
        m_editorWidget->setStyleSheet(
            "QPlainTextEdit { background: #141414; color: #c8c8c8; border: none; padding: 4px; }"
        );

        // embed into scene as proxy widget and parent it to this NodeItem
        m_editorProxy = new QGraphicsProxyWidget(this);
        m_editorProxy->setWidget(m_editorWidget);
        m_editorProxy->setZValue(1); // above node background
        m_editorProxy->setAcceptedMouseButtons(Qt::AllButtons);

        // ensure the proxy does not accept focus for the scene item itself,
        // but the widget will accept focus when clicked.
        m_editorProxy->setFlag(QGraphicsItem::ItemIsSelectable, false);
        m_editorProxy->setFlag(QGraphicsItem::ItemIsMovable, false);

        // connect text changes back to node state
        connect(m_editorWidget, &QPlainTextEdit::textChanged, this, [this]() {
            m_editorCode = m_editorWidget->toPlainText();
            emit positionChanged(this); // notify edges to update if needed
        });

        // initial geometry will be set in updateEmbeddedEditorGeometry()
        updateEmbeddedEditorGeometry();
    }

    // Update proxy geometry/position to match node size and margins
    void updateEmbeddedEditorGeometry() {
        if (!m_editorProxy) return;

        QFontMetrics fm(m_editorFont);
        int charWidth = fm.horizontalAdvance(QLatin1Char('M'));
        int lineHeight = fm.lineSpacing();

        // desired editor size (80 chars x 10 lines) but clamp to node size
        qreal desiredW = charWidth * 80.0;
        qreal desiredH = lineHeight * 10.0;

        const qreal marginX = 12.0;
        const qreal marginY = 8.0;

        qreal availW = m_size.width() - marginX * 2.0;
        qreal availH = m_size.height() - 24.0 - marginY * 2.0;

        qreal w = std::min(desiredW, availW);
        qreal h = std::min(desiredH, availH);

        qreal x = (m_size.width() - w) * 0.5;
        qreal y = 24.0 + marginY;

        // set proxy geometry in local coordinates
        m_editorProxy->setPos(x, y);
        // setGeometry is robust across Qt versions
        m_editorProxy->setGeometry(QRectF(x, y, w, h));
        if (m_editorWidget) {
            m_editorWidget->setFixedSize(int(std::max<qreal>(1.0, w)), int(std::max<qreal>(1.0, h)));
        }
    }

    int m_id;
    QString m_title;
    QSizeF m_size;
    bool m_hovered = false;
    bool m_snapToGrid = true;
    std::vector<Slot> m_slots;

    // shader/editor related
    bool m_isShaderNode = false;
    QString m_editorCode;
    QFont m_editorFont;

    // embedded editor widget
    QPlainTextEdit* m_editorWidget = nullptr;
    QGraphicsProxyWidget* m_editorProxy = nullptr;
};

// ----------------------------- EdgeItem --------------------------------
// EdgeItem must be a QObject to use Qt signals/slots with NodeItem
class EdgeItem : public QObject, public QGraphicsPathItem {
    Q_OBJECT
public:
    EdgeItem(NodeItem* a, int aSlotIndex, NodeItem* b, int bSlotIndex, QGraphicsItem* parent = nullptr)
        : QObject(nullptr), QGraphicsPathItem(parent),
          m_a(a), m_b(b), m_aSlotIndex(aSlotIndex), m_bSlotIndex(bSlotIndex)
    {
        setZValue(-1);
        setPen(QPen(QColor(200,200,120), 2.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        if (m_a) QObject::connect(m_a, &NodeItem::positionChanged, this, &EdgeItem::updatePath);
        if (m_b) QObject::connect(m_b, &NodeItem::positionChanged, this, &EdgeItem::updatePath);
        updatePath();
    }

public Q_SLOTS:
    void updatePath() {
        if (!m_a || !m_b) return;
        QPointF p1 = m_a->slotScenePos(m_aSlotIndex);
        QPointF p2 = m_b->slotScenePos(m_bSlotIndex);

        QPainterPath path(p1);
        qreal dx = (p2.x() - p1.x());
        qreal ctrl = std::max(std::abs(dx) * 0.5, 30.0);
        bool aOut = m_a->slotList()[m_aSlotIndex].isOutput;
        bool bOut = m_b->slotList()[m_bSlotIndex].isOutput;
        QPointF c1 = p1 + QPointF(aOut ? ctrl : -ctrl, 0);
        QPointF c2 = p2 + QPointF(bOut ? ctrl : -ctrl, 0);
        path.cubicTo(c1, c2, p2);
        setPath(path);
    }

    bool containsPoint(const QPointF& scenePos, qreal maxDist) const {
        QPainterPathStroker stroker;
        stroker.setWidth(maxDist*2.0);
        QPainterPath stroked = stroker.createStroke(path());
        return stroked.contains(scenePos);
    }

    NodeItem* aNode() const { return m_a; }
    NodeItem* bNode() const { return m_b; }

private:
    NodeItem* m_a = nullptr;
    NodeItem* m_b = nullptr;
    int m_aSlotIndex = -1;
    int m_bSlotIndex = -1;
};

// ----------------------------- ShaderEditorDialog -----------------------
class ShaderEditorDialog : public QDialog {
    Q_OBJECT
public:
    ShaderEditorDialog(const QString& initialCode = QString(), QWidget* parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle("GLSL Shader Editor");
        setModal(true);
        resize(700, 500);
        QVBoxLayout* v = new QVBoxLayout(this);
        m_editor = new QPlainTextEdit(this);
        m_editor->setPlainText(initialCode);
        m_editor->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
        m_editor->setLineWrapMode(QPlainTextEdit::NoWrap);
        v->addWidget(new QLabel("Edit GLSL code. 'in' and 'out' declarations will create slots automatically."));
        v->addWidget(m_editor);
        QHBoxLayout* h = new QHBoxLayout();
        QPushButton* ok = new QPushButton("Save", this);
        QPushButton* cancel = new QPushButton("Cancel", this);
        QPushButton* parse = new QPushButton("Parse & Preview", this);
        h->addWidget(parse);
        h->addStretch();
        h->addWidget(cancel);
        h->addWidget(ok);
        v->addLayout(h);
        connect(ok, &QPushButton::clicked, this, &ShaderEditorDialog::accept);
        connect(cancel, &QPushButton::clicked, this, &ShaderEditorDialog::reject);
        connect(parse, &QPushButton::clicked, this, &ShaderEditorDialog::onParsePreview);
    }

    QString code() const { return m_editor->toPlainText(); }

private Q_SLOTS:
    void onParsePreview() {
        QString code = m_editor->toPlainText();
        QRegularExpression re(R"(^\s*(in|out)\s+\w+\s+([A-Za-z_]\w*)\s*;)", QRegularExpression::MultilineOption);
        QRegularExpressionMatchIterator it = re.globalMatch(code);
        QStringList ins, outs;
        while (it.hasNext()) {
            QRegularExpressionMatch m = it.next();
            QString dir = m.captured(1);
            QString name = m.captured(2);
            if (dir == "in") ins << name;
            else outs << name;
        }
        QString msg = QString("Found %1 inputs and %2 outputs.\n\nInputs:\n%3\n\nOutputs:\n%4")
            .arg(ins.size()).arg(outs.size()).arg(ins.join("\n")).arg(outs.join("\n"));
        QMessageBox::information(this, "Parse Preview", msg);
    }

private:
    QPlainTextEdit* m_editor;
};

// ----------------------------- NodeView --------------------------------
class NodeView : public QGraphicsView {
    Q_OBJECT
public:
    NodeView(QGraphicsScene* s, QWidget* parent = nullptr)
        : QGraphicsView(s, parent), m_scene(s)
    {
        setRenderHint(QPainter::Antialiasing);
        setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        setDragMode(QGraphicsView::NoDrag);
        setBackgroundBrush(QColor(30,30,30));
        setMouseTracking(true);

        m_rubber = new QRubberBand(QRubberBand::Rectangle, viewport());
        m_tempEdge = new QGraphicsPathItem();
        m_tempEdge->setZValue(-1);
        m_tempEdge->setPen(QPen(QColor(180,180,180,200), 2.0, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
        m_scene->addItem(m_tempEdge);
        m_tempEdge->hide();

        QTimer* t = new QTimer(this);
        connect(t, &QTimer::timeout, this, &NodeView::updateAllEdges);
        t->start(100);
    }

    ~NodeView() {
        if (m_tempEdge) {
            m_scene->removeItem(m_tempEdge);
            delete m_tempEdge;
        }
        delete m_rubber;
    }

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override {
        Q_UNUSED(rect);
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, false);

        const qreal grid = 20.0;
        QRectF viewRect = mapToScene(viewport()->rect()).boundingRect();

        qreal left = std::floor(viewRect.left() / grid) * grid;
        qreal top  = std::floor(viewRect.top()  / grid) * grid;

        QPen pen1(QColor(70,70,70));
        pen1.setWidth(1);
        QPen pen2(QColor(50,50,50));
        pen2.setWidth(1);

        for (qreal x = left; x < viewRect.right(); x += grid) {
            painter->setPen(((int)(x/grid) % 5 == 0) ? pen1 : pen2);
            painter->drawLine(QPointF(x, viewRect.top()), QPointF(x, viewRect.bottom()));
        }
        for (qreal y = top; y < viewRect.bottom(); y += grid) {
            painter->setPen(((int)(y/grid) % 5 == 0) ? pen1 : pen2);
            painter->drawLine(QPointF(viewRect.left(), y), QPointF(viewRect.right(), y));
        }
        painter->restore();
    }

    void wheelEvent(QWheelEvent* event) override {
        constexpr qreal factor = 1.25;
        if (event->angleDelta().y() > 0) {
            scale(factor, factor);
        } else {
            scale(1.0 / factor, 1.0 / factor);
        }
    }

    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::MiddleButton) {
            m_panning = true;
            m_lastPan = event->pos();
            setCursor(Qt::ClosedHandCursor);
            event->accept();
            return;
        }

        if (event->button() == Qt::LeftButton) {
            QPointF scenePos = mapToScene(event->pos());
            NodeItem* node = nodeAtScenePos(scenePos);
            if (node) {
                auto [found, slotIndex] = node->slotAtScenePos(scenePos, 10.0);
                if (found) {
                    m_draggingEdge = true;
                    m_edgeFromNode = node;
                    m_edgeFromSlot = slotIndex;
                    QPainterPath p(node->slotScenePos(slotIndex));
                    m_tempEdge->setPath(p);
                    m_tempEdge->show();
                    event->accept();
                    return;
                }
            }
        }

        if (event->button() == Qt::LeftButton) {
            QPointF scenePos = mapToScene(event->pos());
            QGraphicsItem* it = m_scene->itemAt(scenePos, transform());
            if (!it || it == m_tempEdge) {
                m_rubberOrigin = event->pos();
                m_rubber->setGeometry(QRect(m_rubberOrigin, QSize()));
                m_rubber->show();
                m_rubberActive = true;
                event->accept();
                return;
            }
        }

        QGraphicsView::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if (m_panning) {
            QPoint delta = event->pos() - m_lastPan;
            m_lastPan = event->pos();
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
            verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
            return;
        }

        if (m_draggingEdge && m_edgeFromNode) {
            QPointF p1 = m_edgeFromNode->slotScenePos(m_edgeFromSlot);
            QPointF p2 = mapToScene(event->pos());
            QPainterPath path(p1);
            qreal dx = (p2.x() - p1.x());
            qreal ctrl = std::max(std::abs(dx) * 0.5, 30.0);
            // fixed: use slotList() instead of slots()
            bool aOut = m_edgeFromNode->slotList()[m_edgeFromSlot].isOutput;
            QPointF c1 = p1 + QPointF(aOut ? ctrl : -ctrl, 0);
            QPointF c2 = p2 + QPointF((aOut ? 1 : -1) * ctrl, 0);
            path.cubicTo(c1, c2, p2);
            m_tempEdge->setPath(path);
            event->accept();
            return;
        }

        if (m_rubberActive) {
            QRect r(m_rubberOrigin, event->pos());
            m_rubber->setGeometry(r.normalized());
            event->accept();
            return;
        }

        QGraphicsView::mouseMoveEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        if (event->button() == Qt::MiddleButton && m_panning) {
            m_panning = false;
            setCursor(Qt::ArrowCursor);
            event->accept();
            return;
        }

        if (event->button() == Qt::LeftButton && m_draggingEdge) {
            QPointF scenePos = mapToScene(event->pos());
            NodeItem* targetNode = nodeAtScenePos(scenePos);
            if (targetNode) {
                auto [found, targetSlot] = targetNode->slotAtScenePos(scenePos, 10.0);
                if (found) {
                    EdgeItem* e = new EdgeItem(m_edgeFromNode, m_edgeFromSlot, targetNode, targetSlot);
                    m_scene->addItem(e);
                    e->updatePath();
                }
            }
            m_draggingEdge = false;
            m_edgeFromNode = nullptr;
            m_tempEdge->hide();
            event->accept();
            return;
        }

        if (event->button() == Qt::LeftButton && m_rubberActive) {
            m_rubber->hide();
            m_rubberActive = false;
            QRect rubberRect = m_rubber->geometry();
            QRectF sceneRect = mapToScene(rubberRect).boundingRect();
            QList<QGraphicsItem*> items = m_scene->items(sceneRect, Qt::IntersectsItemShape);
            if (!(QApplication::keyboardModifiers() & Qt::ShiftModifier)) {
                for (QGraphicsItem* it : m_scene->selectedItems()) it->setSelected(false);
            }
            for (QGraphicsItem* it : items) {
                if (dynamic_cast<NodeItem*>(it)) it->setSelected(true);
            }
            event->accept();
            return;
        }

        QGraphicsView::mouseReleaseEvent(event);
    }

    void keyPressEvent(QKeyEvent* event) override {
        if (event->key() == Qt::Key_Delete) {
            auto selected = m_scene->selectedItems();
            for (QGraphicsItem* it : selected) {
                if (auto node = dynamic_cast<NodeItem*>(it)) {
                    QList<QGraphicsItem*> all = m_scene->items();
                    for (QGraphicsItem* ai : all) {
                        if (auto edge = dynamic_cast<EdgeItem*>(ai)) {
                            if (edge->aNode() == node || edge->bNode() == node) {
                                m_scene->removeItem(edge);
                                delete edge;
                            }
                        }
                    }
                    m_scene->removeItem(node);
                    delete node;
                } else if (auto edge = dynamic_cast<EdgeItem*>(it)) {
                    m_scene->removeItem(edge);
                    delete edge;
                } else {
                    m_scene->removeItem(it);
                    delete it;
                }
            }
            event->accept();
            return;
        }

        if (event->key() == Qt::Key_Home) {
            QRectF itemsRect;
            bool first = true;
            for (QGraphicsItem* it : m_scene->items()) {
                if (dynamic_cast<NodeItem*>(it)) {
                    if (first) { itemsRect = it->sceneBoundingRect(); first = false; }
                    else itemsRect = itemsRect.united(it->sceneBoundingRect());
                }
            }
            if (!first) {
                fitInView(itemsRect.adjusted(-40,-40,40,40), Qt::KeepAspectRatio);
            }
            event->accept();
            return;
        }

        QGraphicsView::keyPressEvent(event);
    }

public Q_SLOTS:
    void onNodeDoubleClicked(NodeItem* node) {
        if (!m_lastClickedNode) {
            m_lastClickedNode = node;
            node->setSelected(true);
        } else if (m_lastClickedNode == node) {
            m_lastClickedNode = nullptr;
            node->setSelected(false);
        } else {
            int aSlot = -1, bSlot = -1;
            const auto &as = m_lastClickedNode->slotList();
            for (int i = 0; i < (int)as.size(); ++i) if (as[i].isOutput) { aSlot = i; break; }
            const auto &bs = node->slotList();
            for (int i = 0; i < (int)bs.size(); ++i) if (!bs[i].isOutput) { bSlot = i; break; }
            if (aSlot != -1 && bSlot != -1) {
                EdgeItem* e = new EdgeItem(m_lastClickedNode, aSlot, node, bSlot);
                m_scene->addItem(e);
                e->updatePath();
            } else {
                EdgeItem* e = new EdgeItem(m_lastClickedNode, 0, node, 0);
                m_scene->addItem(e);
                e->updatePath();
            }
            m_lastClickedNode->setSelected(false);
            m_lastClickedNode = nullptr;
        }
    }

    void updateAllEdges() {
        for (QGraphicsItem* it : m_scene->items()) {
            if (auto edge = dynamic_cast<EdgeItem*>(it)) {
                edge->updatePath();
            }
        }
    }

    NodeItem* createMicrophoneNode(const QPointF& scenePos) {
        NodeItem* n = new NodeItem(nextNodeId(), "Microphone Input");
        n->addSlot("audio", true);
        n->setPos(scenePos);
        m_scene->addItem(n);
        connect(n, &NodeItem::doubleClicked, this, &NodeView::onNodeDoubleClicked);
        return n;
    }

    NodeItem* createShaderNode(const QString& title, const QString& glslCode, const QPointF& scenePos) {
        NodeItem* n = new NodeItem(nextNodeId(), title);
        n->setEditorCodeAndSlots(glslCode, true);
        n->setPos(scenePos);
        m_scene->addItem(n);
        connect(n, &NodeItem::doubleClicked, this, &NodeView::onNodeDoubleClicked);
        return n;
    }

private:
    NodeItem* nodeAtScenePos(const QPointF& scenePos) const {
        QList<QGraphicsItem*> items = m_scene->items(scenePos);
        for (QGraphicsItem* it : items) {
            if (auto node = dynamic_cast<NodeItem*>(it)) return node;
        }
        return nullptr;
    }

    int nextNodeId() { static int id = 1; return id++; }

private:
    QGraphicsScene* m_scene;
    bool m_panning = false;
    QPoint m_lastPan;

    QRubberBand* m_rubber = nullptr;
    QPoint m_rubberOrigin;
    bool m_rubberActive = false;

    QGraphicsPathItem* m_tempEdge = nullptr;
    bool m_draggingEdge = false;
    NodeItem* m_edgeFromNode = nullptr;
    int m_edgeFromSlot = -1;

    NodeItem* m_lastClickedNode = nullptr;
};

// ----------------------------- MainWindow --------------------------------
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        m_scene = new QGraphicsScene(this);
        m_scene->setSceneRect(-4000, -4000, 8000, 8000);

        m_view = new NodeView(m_scene, this);
        setCentralWidget(m_view);
        setWindowTitle("Qt6 Node Editor - Shader Nodes Example");
        resize(1200, 800);

        createToolbar();

        NodeItem* n1 = m_view->createMicrophoneNode(QPointF(-300, -50));
        NodeItem* n2 = m_view->createShaderNode("Fragment Shader", defaultFragmentTemplate(), QPointF(0, 0));
        NodeItem* n3 = m_view->createShaderNode("Vertex Shader", defaultVertexTemplate(), QPointF(300, -50));
    }

private:
    void createToolbar() {
        QToolBar* tb = addToolBar("Create");
        QAction* actMic = tb->addAction("Add Microphone");
        QAction* actFrag = tb->addAction("Add Fragment Shader");
        QAction* actVert = tb->addAction("Add Vertex Shader");

        connect(actMic, &QAction::triggered, this, [this]() {
            QPointF center = m_view->mapToScene(m_view->viewport()->rect().center());
            m_view->createMicrophoneNode(center + QPointF(-200, -100));
        });

        connect(actFrag, &QAction::triggered, this, [this]() {
            ShaderEditorDialog dlg(defaultFragmentTemplate(), this);
            if (dlg.exec() == QDialog::Accepted) {
                QString code = dlg.code();
                QPointF center = m_view->mapToScene(m_view->viewport()->rect().center());
                m_view->createShaderNode("Fragment Shader", code, center + QPointF(-100, -50));
            }
        });

        connect(actVert, &QAction::triggered, this, [this]() {
            ShaderEditorDialog dlg(defaultVertexTemplate(), this);
            if (dlg.exec() == QDialog::Accepted) {
                QString code = dlg.code();
                QPointF center = m_view->mapToScene(m_view->viewport()->rect().center());
                m_view->createShaderNode("Vertex Shader", code, center + QPointF(100, -50));
            }
        });
    }

    // simple shader templates
    QString defaultFragmentTemplate() const {
        return QString(
            "#version 330 core\n"
            "in vec2 uv;\n"
            "out vec4 fragColor;\n\n"
            "void main() {\n"
            "    fragColor = vec4(uv, 0.5, 1.0);\n"
            "}\n"
        );
    }

    QString defaultVertexTemplate() const {
        return QString(
            "#version 330 core\n"
            "layout(location = 0) in vec3 position;\n"
            "layout(location = 1) in vec2 texcoord;\n"
            "out vec2 uv;\n\n"
            "void main() {\n"
            "    uv = texcoord;\n"
            "    gl_Position = vec4(position, 1.0);\n"
            "}\n"
        );
    }

private:
    QGraphicsScene* m_scene;
    NodeView* m_view;
};

// ----------------------------- main --------------------------------
int main(int argc, char** argv) {
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}

#include "main.moc"
