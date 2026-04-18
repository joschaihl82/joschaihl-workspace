// main.cpp
// Qt6 amalgamated Node Editor with useful extensions:
// - Rubber-band selection (left-drag on empty space)
// - Drag-to-create edges from slots with temporary preview edge
// - Slot hit-tests (left/right slots, two per side)
// - Snap-to-grid when moving nodes
// - Edges automatically update when connected nodes move
// - Delete key removes selected nodes and edges
// - Zoom (wheel) and pan (middle mouse)
// Build with CMake (AUTOMOC enabled) or qmake.

#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsObject>
#include <QGraphicsPathItem>
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QPainterPathStroker>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QRubberBand>
#include <QTimer>
#include <QKeyEvent>
#include <QScrollBar>
#include <QObject>
#include <cmath>
#include <algorithm>

// ----------------------------- NodeItem --------------------------------
class NodeItem : public QGraphicsObject {
    Q_OBJECT
public:
    NodeItem(int id, const QString &title, QGraphicsItem* parent = nullptr)
        : QGraphicsObject(parent), m_id(id), m_title(title)
    {
        m_size = QSizeF(160, 88);
        setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
        setAcceptHoverEvents(true);
        m_snapToGrid = true;
    }

    QRectF boundingRect() const override {
        return QRectF(QPointF(0,0), m_size).adjusted(-6,-6,6,6);
    }

    void paint(QPainter* p, const QStyleOptionGraphicsItem* option, QWidget*) override {
        Q_UNUSED(option);
        // Background
        QColor bg = isSelected() ? QColor(80,120,200) : QColor(60,60,60);
        p->setRenderHint(QPainter::Antialiasing);
        p->setBrush(bg);
        p->setPen(QPen(QColor(30,30,30), 2));
        p->drawRoundedRect(QRectF(QPointF(0,0), m_size), 6, 6);

        // Title
        p->setPen(Qt::white);
        p->setFont(QFont("Arial", 10, QFont::Bold));
        p->drawText(QRectF(8,6,m_size.width()-16,20), Qt::AlignLeft | Qt::AlignVCenter, m_title);

        // Slots (visual only)
        const qreal r = 6.0;
        // left input slots
        p->setBrush(QColor(200,80,80));
        p->setPen(Qt::NoPen);
        p->drawEllipse(QPointF(0, m_size.height()*0.28), r, r);
        p->drawEllipse(QPointF(0, m_size.height()*0.72), r, r);
        // right output slots
        p->setBrush(QColor(80,200,80));
        p->drawEllipse(QPointF(m_size.width(), m_size.height()*0.28), r, r);
        p->drawEllipse(QPointF(m_size.width(), m_size.height()*0.72), r, r);

        // optional border when hovered
        if (m_hovered) {
            p->setPen(QPen(QColor(200,200,200,80), 1.5));
            p->setBrush(Qt::NoBrush);
            p->drawRoundedRect(QRectF(QPointF(0,0), m_size), 6, 6);
        }
    }

    int id() const { return m_id; }

    // Return scene position of a slot. right==true -> right side (outputs), index 0..1
    QPointF slotScenePos(bool right, int index) const {
        qreal y = (index==0) ? m_size.height()*0.28 : m_size.height()*0.72;
        QPointF local = QPointF(right ? m_size.width() : 0, y);
        return mapToScene(local);
    }

    // Hit-test: returns (found, right, index)
    std::tuple<bool,bool,int> slotAtScenePos(const QPointF& scenePos, qreal radius = 8.0) const {
        for (int side = 0; side < 2; ++side) { // 0 = left, 1 = right
            bool right = (side == 1);
            for (int idx = 0; idx < 2; ++idx) {
                QPointF s = slotScenePos(right, idx);
                qreal d2 = QLineF(s, scenePos).length();
                if (d2 <= radius) {
                    return { true, right, idx };
                }
            }
        }
        return { false, false, -1 };
    }

signals:
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
            // Snap to grid while dragging
            QPointF newPos = value.toPointF();
            const qreal grid = 8.0; // snap resolution
            qreal x = std::round(newPos.x() / grid) * grid;
            qreal y = std::round(newPos.y() / grid) * grid;
            return QPointF(x, y);
        }
        return QGraphicsObject::itemChange(change, value);
    }

private:
    int m_id;
    QString m_title;
    QSizeF m_size;
    bool m_hovered = false;
    bool m_snapToGrid = true;
};

// ----------------------------- EdgeItem --------------------------------
// EdgeItem must be a QObject to use Qt signals/slots with NodeItem
class EdgeItem : public QObject, public QGraphicsPathItem {
    Q_OBJECT
public:
    EdgeItem(NodeItem* a, bool aRight, int aIndex, NodeItem* b, bool bRight, int bIndex, QGraphicsItem* parent = nullptr)
        : QObject(nullptr), QGraphicsPathItem(parent),
          m_a(a), m_b(b), m_aRight(aRight), m_bRight(bRight), m_aIndex(aIndex), m_bIndex(bIndex)
    {
        setZValue(-1); // behind nodes
        setPen(QPen(QColor(200,200,120), 2.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        // connect to node movement (NodeItem is QObject)
        if (m_a) QObject::connect(m_a, &NodeItem::positionChanged, this, &EdgeItem::updatePath);
        if (m_b) QObject::connect(m_b, &NodeItem::positionChanged, this, &EdgeItem::updatePath);
        updatePath();
    }

public slots:
    void updatePath() {
        if (!m_a || !m_b) return;
        QPointF p1 = m_a->slotScenePos(m_aRight, m_aIndex);
        QPointF p2 = m_b->slotScenePos(m_bRight, m_bIndex);

        QPainterPath path(p1);
        qreal dx = (p2.x() - p1.x());
        qreal ctrl = std::max(std::abs(dx) * 0.5, 30.0);
        QPointF c1 = p1 + QPointF(m_aRight ? ctrl : -ctrl, 0);
        QPointF c2 = p2 + QPointF(m_bRight ? ctrl : -ctrl, 0);
        path.cubicTo(c1, c2, p2);
        setPath(path);
    }

    bool containsPoint(const QPointF& scenePos, qreal maxDist) const {
        QPainterPathStroker stroker;
        stroker.setWidth(maxDist*2.0);
        QPainterPath stroked = stroker.createStroke(path());
        return stroked.contains(scenePos);
    }

private:
    NodeItem* m_a = nullptr;
    NodeItem* m_b = nullptr;
    bool m_aRight = true;
    bool m_bRight = false;
    int m_aIndex = 0;
    int m_bIndex = 0;
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

        // Timer to update edges occasionally (safety)
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
        // Middle mouse -> pan
        if (event->button() == Qt::MiddleButton) {
            m_panning = true;
            m_lastPan = event->pos();
            setCursor(Qt::ClosedHandCursor);
            event->accept();
            return;
        }

        // Left mouse: check for slot hit to start edge drag
        if (event->button() == Qt::LeftButton) {
            QPointF scenePos = mapToScene(event->pos());
            NodeItem* node = nodeAtScenePos(scenePos);
            if (node) {
                auto [found, right, idx] = node->slotAtScenePos(scenePos, 10.0);
                if (found) {
                    // start dragging an edge from this slot
                    m_draggingEdge = true;
                    m_edgeFromNode = node;
                    m_edgeFromRight = right;
                    m_edgeFromIndex = idx;
                    QPainterPath p(node->slotScenePos(right, idx));
                    m_tempEdge->setPath(p);
                    m_tempEdge->show();
                    event->accept();
                    return;
                }
            }
        }

        // Left mouse: start rubberband if clicked on empty space
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
            // update temp edge to mouse
            QPointF p1 = m_edgeFromNode->slotScenePos(m_edgeFromRight, m_edgeFromIndex);
            QPointF p2 = mapToScene(event->pos());
            QPainterPath path(p1);
            qreal dx = (p2.x() - p1.x());
            qreal ctrl = std::max(std::abs(dx) * 0.5, 30.0);
            QPointF c1 = p1 + QPointF(m_edgeFromRight ? ctrl : -ctrl, 0);
            QPointF c2 = p2 + QPointF((m_edgeFromRight ? 1 : -1) * ctrl, 0);
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
            // finish edge drag: check for slot under mouse
            QPointF scenePos = mapToScene(event->pos());
            NodeItem* targetNode = nodeAtScenePos(scenePos);
            if (targetNode) {
                auto [found, right, idx] = targetNode->slotAtScenePos(scenePos, 10.0);
                if (found) {
                    // create edge between m_edgeFromNode and targetNode
                    EdgeItem* e = new EdgeItem(m_edgeFromNode, m_edgeFromRight, m_edgeFromIndex,
                                               targetNode, right, idx);
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
            // select items inside rect (intersect)
            QList<QGraphicsItem*> items = m_scene->items(sceneRect, Qt::IntersectsItemShape);
            // clear previous selection unless Shift is held
            if (!(QApplication::keyboardModifiers() & Qt::ShiftModifier)) {
                for (QGraphicsItem* it : m_scene->selectedItems()) it->setSelected(false);
            }
            for (QGraphicsItem* it : items) {
                // only select NodeItem
                if (dynamic_cast<NodeItem*>(it)) it->setSelected(true);
            }
            event->accept();
            return;
        }

        QGraphicsView::mouseReleaseEvent(event);
    }

    void keyPressEvent(QKeyEvent* event) override {
        if (event->key() == Qt::Key_Delete) {
            // delete selected nodes and edges
            auto selected = m_scene->selectedItems();
            for (QGraphicsItem* it : selected) {
                if (auto node = dynamic_cast<NodeItem*>(it)) {
                    // remove edges connected to this node
                    QList<QGraphicsItem*> all = m_scene->items();
                    for (QGraphicsItem* ai : all) {
                        if (auto edge = dynamic_cast<EdgeItem*>(ai)) {
                            // crude check: remove edges whose path start or end is near node center
                            QPainterPath p = edge->path();
                            if (p.elementCount() > 0) {
                                QPainterPath::Element e0 = p.elementAt(0);
                                QPointF start(e0.x, e0.y);
                                QPointF c = node->mapToScene(node->boundingRect().center());
                                if (QLineF(start, c).length() < 1.0) {
                                    m_scene->removeItem(edge);
                                    delete edge;
                                } else {
                                    // also check last element
                                    QPainterPath::Element el = p.elementAt(p.elementCount()-1);
                                    QPointF last(el.x, el.y);
                                    if (QLineF(last, c).length() < 1.0) {
                                        m_scene->removeItem(edge);
                                        delete edge;
                                    }
                                }
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
            // zoom to fit all nodes
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

public slots:
    void onNodeDoubleClicked(NodeItem* node) {
        // double-click behavior: if first click stored, create edge; else store
        if (!m_lastClickedNode) {
            m_lastClickedNode = node;
            node->setSelected(true);
        } else if (m_lastClickedNode == node) {
            node->setSelected(false);
            m_lastClickedNode = nullptr;
        } else {
            // create edge between centers (choose right/left heuristics)
            bool aRight = true;
            bool bRight = false;
            EdgeItem* e = new EdgeItem(m_lastClickedNode, aRight, 0, node, bRight, 0);
            m_scene->addItem(e);
            e->updatePath();
            m_lastClickedNode->setSelected(false);
            m_lastClickedNode = nullptr;
        }
    }

    void updateAllEdges() {
        // iterate edges and update
        for (QGraphicsItem* it : m_scene->items()) {
            if (auto edge = dynamic_cast<EdgeItem*>(it)) {
                edge->updatePath();
            }
        }
    }

private:
    NodeItem* nodeAtScenePos(const QPointF& scenePos) const {
        QList<QGraphicsItem*> items = m_scene->items(scenePos);
        for (QGraphicsItem* it : items) {
            if (auto node = dynamic_cast<NodeItem*>(it)) return node;
        }
        return nullptr;
    }

private:
    QGraphicsScene* m_scene;
    bool m_panning = false;
    QPoint m_lastPan;

    // Rubber band selection
    QRubberBand* m_rubber = nullptr;
    QPoint m_rubberOrigin;
    bool m_rubberActive = false;

    // Temporary edge while dragging
    QGraphicsPathItem* m_tempEdge = nullptr;
    bool m_draggingEdge = false;
    NodeItem* m_edgeFromNode = nullptr;
    bool m_edgeFromRight = false;
    int m_edgeFromIndex = 0;

    // double-click edge creation helper
    NodeItem* m_lastClickedNode = nullptr;
};

// ----------------------------- main --------------------------------
int main(int argc, char** argv) {
    QApplication app(argc, argv);

    QGraphicsScene scene;
    scene.setSceneRect(-4000, -4000, 8000, 8000);

    // Create view
    NodeView view(&scene);
    view.setWindowTitle("Qt6 Node Editor - Enhanced Example");
    view.resize(1100, 760);

    // Create some nodes
    NodeItem* n1 = new NodeItem(1, "Input A");
    NodeItem* n2 = new NodeItem(2, "Process B");
    NodeItem* n3 = new NodeItem(3, "Output C");
    NodeItem* n4 = new NodeItem(4, "Filter D");

    n1->setPos(-320, -40);
    n2->setPos(-20, 120);
    n3->setPos(320, -40);
    n4->setPos(0, -200);

    scene.addItem(n1);
    scene.addItem(n2);
    scene.addItem(n3);
    scene.addItem(n4);

    // Connect double-click signals to view slot
    QObject::connect(n1, &NodeItem::doubleClicked, &view, &NodeView::onNodeDoubleClicked);
    QObject::connect(n2, &NodeItem::doubleClicked, &view, &NodeView::onNodeDoubleClicked);
    QObject::connect(n3, &NodeItem::doubleClicked, &view, &NodeView::onNodeDoubleClicked);
    QObject::connect(n4, &NodeItem::doubleClicked, &view, &NodeView::onNodeDoubleClicked);

    // Show view
    view.show();

    return app.exec();
}

#include "main.moc"
