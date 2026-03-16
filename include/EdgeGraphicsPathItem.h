#ifndef EDGEGRAPHICSPATHITEM_H
#define EDGEGRAPHICSPATHITEM_H

#include <QGraphicsPathItem>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QPainterPath>

class Edge;

/**
 * @brief Abstract base class for the visual representation of an Edge in the scene.
 *
 * EdgeGraphicsPathItem is a QGraphicsPathItem subclass that renders a connection
 * between two sockets. It manages source/destination positions, selection state,
 * and painting logic. Concrete subclasses must implement calcPath() to define
 * the specific curve or line style (e.g. bezier, direct).
 *
 * @see Edge
 * @see QGraphicsPathItem
 */
class EdgeGraphicsPathItem : public QGraphicsPathItem
{
public:

    /**
     * @brief Constructs the graphics item and links it to its logical Edge.
     *
     * Calls initAssets() and initUI() to set up pens and visual defaults.
     *
     * @param edge   The logical Edge this item represents. Must not be null.
     * @param parent Optional parent QGraphicsItem.
     */
    explicit EdgeGraphicsPathItem(Edge* edge, QGraphicsItem* parent = nullptr);

    /**
     * @brief Sets the source (start) position of the edge path in scene coordinates.
     * @param pos The position of the start socket.
     */
    void setSource(const QPointF& pos);

    /**
     * @brief Sets the destination (end) position of the edge path in scene coordinates.
     * @param pos The position of the end socket.
     */
    void setDestination(const QPointF& pos);

    /**
     * @brief Returns the current source position of the edge path.
     * @return The source position in scene coordinates.
     */
    QPointF getSource() const;

    /**
     * @brief Returns the current destination position of the edge path.
     * @return The destination position in scene coordinates.
     */
    QPointF getDestination() const;

    /**
     * @brief Returns the bounding rectangle of the edge path.
     *
     * Overridden to ensure Qt uses an accurate bounding box for culling
     * and hit testing, accounting for pen width and control points.
     *
     * @return A QRectF encompassing the full edge path.
     */
    QRectF boundingRect() const override;

    /**
     * @brief Returns the precise shape of the edge for hit testing.
     *
     * Overridden to provide a stroked path shape so that click detection
     * works accurately along the curve rather than just the bounding rect.
     *
     * @return A QPainterPath representing the clickable area of the edge.
     */
    QPainterPath shape() const override;

    /**
     * @brief Paints the edge using the appropriate pen based on selection state.
     *
     * Draws the path using m_penSelected when selected, m_pen otherwise.
     * Also handles the dragging visual state via penDragging.
     *
     * @param painter The QPainter to draw with.
     * @param option  Style options including selection state.
     * @param widget  The widget being painted on (may be null).
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    /**
     * @brief Handles mouse release events on the edge graphics item.
     *
     * Used to detect selection state changes and trigger visual updates
     * via onSelected() when the selection state has changed.
     *
     * @param event The mouse release event.
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    /**
     * @brief The roundness factor used for bezier control point offset calculations.
     *
     * A higher value produces more curved edges. Used in calcPath() implementations.
     */
    inline static const int EDGE_CP_ROUNDNESS = 100;

    /**
     * @brief Returns the logical Edge associated with this graphics item.
     * @return Pointer to the owning Edge.
     */
    Edge* getEdge() const { return edge; }

    /**
     * @brief Recalculates and updates the painter path from the current source and destination.
     *
     * Should be called whenever posSource or posDestination changes to
     * keep the rendered path in sync.
     */
    void updatePath();

    /**
     * @brief Sets the cached selection state used to detect selection changes.
     *
     * Stored so that mouseReleaseEvent can compare against the current
     * selection state and only call onSelected() when it has actually changed.
     *
     * @param s The selection state to cache.
     */
    void setLastSelectedState(bool s) { m_lastSelectedState = s; }

protected:

    /**
     * @brief Calculates and returns the painter path for this edge.
     *
     * Pure virtual — must be implemented by subclasses to define the
     * specific shape of the edge (e.g. cubic bezier, straight line).
     *
     * @return A QPainterPath from posSource to posDestination.
     */
    virtual QPainterPath calcPath() const = 0;

    /// @brief The position of the start socket in scene coordinates.
    QPointF posSource {0, 0};

    /// @brief The position of the end socket in scene coordinates.
    QPointF posDestination {200, 100};

    /// @brief Cached selection state from the previous frame, used to detect changes.
    bool m_lastSelectedState = false;

    /// @brief Pen used to draw the edge in its default (unselected) state.
    QPen m_pen;

    /// @brief Pen used to draw the edge when it is selected.
    QPen m_penSelected;

    /// @brief Pen used to draw the edge while it is being dragged.
    QPen penDragging;

    /// @brief Pointer to the logical Edge this item represents.
    Edge* edge;

private:

    /**
     * @brief Initialises pen colours, widths, and styles.
     *
     * Called once from the constructor to set up m_pen, m_penSelected,
     * and penDragging with their default appearance.
     */
    void initAssets();

    /**
     * @brief Initialises QGraphicsItem flags and settings.
     *
     * Sets up selectability, Z-value, and any other item-level
     * configuration required for correct behaviour in the scene.
     */
    void initUI();

    /**
     * @brief Called when the selection state of this item changes.
     *
     * Triggers a visual update (e.g. switching between m_pen and
     * m_penSelected) in response to the item being selected or deselected.
     */
    void onSelected();
};

#endif // EDGEGRAPHICSPATHITEM_H