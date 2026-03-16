#ifndef NODEEDITORGRAPHICSVIEW_H
#define NODEEDITORGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWidget>
#include <QMap>
#include <QSet>

class Edge;
class SocketItem;
class NodeEditorGraphicsScene;
class NodeItem;

/**
 * @brief The main viewport for the node editor, handling user interaction with the scene.
 *
 * NodeEditorGraphicsView extends QGraphicsView to provide node-editor-specific
 * interaction: zooming via the mouse wheel, panning via the middle mouse button,
 * edge dragging between sockets, node drag-and-drop from external sources,
 * multi-node move tracking for undo/redo, and selection change handling.
 *
 * Subclass this to extend or override interaction behaviour (e.g. CalculatorNodeEditorGraphicsView).
 *
 * @see NodeEditorGraphicsScene
 * @see NodeItem
 * @see Edge
 * @see SocketItem
 */
class NodeEditorGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:

    /**
     * @brief Constructs the view and links it to the given scene.
     *
     * Calls initUI() to configure rendering hints, scroll bar policy,
     * zoom defaults, and scene assignment.
     *
     * @param grScene The scene this view will display. Must not be null.
     * @param parent  Optional parent widget.
     */
    NodeEditorGraphicsView(NodeEditorGraphicsScene* grScene, QWidget* parent = nullptr);

    /**
     * @brief Deletes all currently selected nodes and edges from the scene.
     *
     * Pushes a compound undo command onto the scene's history stack so
     * the deletion can be undone.
     */
    void deleteSelected();

    /**
     * @brief Returns the last known mouse position in scene coordinates.
     *
     * Updated on every mouseMoveEvent. Useful for spawning nodes or
     * other items at the cursor's scene position.
     *
     * @return The last scene-space mouse position as a QPointF.
     */
    QPointF getLastSceneMousePosition() const { return m_lastSceneMousePosition; }

    /**
     * @brief Registers a callback to be invoked when a drag-enter event occurs on the view.
     *
     * Multiple listeners can be registered and are all notified in order.
     *
     * @param callback A callable with signature `void(QDragEnterEvent*)`.
     */
    void addDragEnterListener(std::function<void(QDragEnterEvent*)> callback);

    /**
     * @brief Registers a callback to be invoked when a drop event occurs on the view.
     *
     * Multiple listeners can be registered and are all notified in order.
     *
     * @param callback A callable with signature `void(QDropEvent*)`.
     */
    void addDropListener(std::function<void(QDropEvent*)> callback);

signals:

    /**
     * @brief Emitted whenever the mouse moves over the scene.
     *
     * @param x The current mouse X position in scene coordinates.
     * @param y The current mouse Y position in scene coordinates.
     */
    void scenePosChanged(int x, int y);

    /**
     * @brief Emitted when a drag-enter event is received by the view.
     *
     * @param event The drag enter event, forwarded from dragEnterEvent().
     */
    void nodeDragEntered(QDragEnterEvent* event);

    /**
     * @brief Emitted when a drop event is received by the view.
     *
     * @param event The drop event, forwarded from dropEvent().
     */
    void nodeDropped(QDropEvent* event);

protected:

    /**
     * @brief Initialises the view's rendering settings, scroll bars, and zoom parameters.
     *
     * Called once from the constructor. Sets antialiasing, drag mode,
     * viewport update mode, and default zoom values.
     */
    void initUI();

    /**
     * @brief Dispatches mouse press events to the appropriate handler based on button.
     *
     * Routes to leftMouseButtonPress(), middleMouseButtonPress(), or
     * rightMouseButtonPress() depending on the event's button.
     *
     * @param event The mouse press event.
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief Dispatches mouse release events to the appropriate handler based on button.
     *
     * Routes to leftMouseButtonRelease(), middleMouseButtonRelease(), or
     * rightMouseButtonRelease() depending on the event's button.
     *
     * @param event The mouse release event.
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

    /**
     * @brief Handles mouse wheel events to zoom the view in or out.
     *
     * Applies zoom in steps of m_zoomStep, clamped to m_zoomRange.
     * Zooms toward the current mouse cursor position.
     *
     * @param event The wheel event.
     */
    void wheelEvent(QWheelEvent* event) override;

    /**
     * @brief Begins panning the view when the middle mouse button is pressed.
     *
     * Temporarily switches drag mode to ScrollHandDrag and stores the
     * synthetic left-button event needed to initiate Qt's built-in panning.
     *
     * @param event The original middle mouse button press event.
     */
    void middleMouseButtonPress(QMouseEvent* event);

    /**
     * @brief Ends panning when the middle mouse button is released.
     *
     * Restores the previous drag mode and releases the synthetic press.
     *
     * @param event The original middle mouse button release event.
     */
    void middleMouseButtonRelease(QMouseEvent* event);

    /**
     * @brief Tracks mouse movement for scene position updates and edge dragging.
     *
     * Updates m_lastSceneMousePosition, emits scenePosChanged(), and if
     * in MODE_EDGE_DRAG, updates the drag edge's endpoint to follow the cursor.
     *
     * @param event The mouse move event.
     */
    void mouseMoveEvent(QMouseEvent* event);

    /**
     * @brief Handles left mouse button press for selection and edge drag initiation.
     *
     * If clicking on a SocketItem, begins an edge drag via edgeDragStart().
     * Otherwise defers to QGraphicsView for standard selection behaviour.
     * Records m_lastLeftClickScenePos for distance threshold checking.
     *
     * @param event The mouse press event.
     */
    void leftMouseButtonPress(QMouseEvent* event);

    /**
     * @brief Handles left mouse button release for edge drag completion and node move recording.
     *
     * If in MODE_EDGE_DRAG and the drag distance exceeds EDGE_DRAG_START_THRESHOLD,
     * calls edgeDragEnd(). Also finalises multi-node move commands for undo/redo.
     *
     * @param event The mouse release event.
     */
    void leftMouseButtonRelease(QMouseEvent* event);

    /**
     * @brief Handles right mouse button press events.
     *
     * Reserved for context menu or other right-click interactions.
     *
     * @param event The mouse press event.
     */
    void rightMouseButtonPress(QMouseEvent* event);

    /**
     * @brief Handles right mouse button release events.
     *
     * @param event The mouse release event.
     */
    void rightMouseButtonRelease(QMouseEvent* event);

    /**
     * @brief Returns the topmost QGraphicsItem at the position of a mouse event.
     *
     * @param event The mouse event whose position is used for hit testing.
     * @return The topmost item under the cursor, or nullptr if none.
     */
    QGraphicsItem* getItemAtClick(QMouseEvent* event);

    /**
     * @brief Begins dragging a new edge from the given socket.
     *
     * Creates a temporary Edge, sets m_dragEdge, m_dragStartSocket, and
     * switches m_mode to MODE_EDGE_DRAG.
     *
     * @param item The SocketItem from which the edge drag originates.
     */
    void edgeDragStart(SocketItem* item);

    /**
     * @brief Attempts to complete an edge drag onto the given graphics item.
     *
     * If the item is a compatible SocketItem, finalises the connection and
     * pushes a CreateEdgeCommand onto the undo stack. Otherwise discards
     * the drag edge and restores any previous edge.
     *
     * @param item The item under the cursor at release time.
     * @return True if the edge was successfully connected, false if it was discarded.
     */
    bool edgeDragEnd(QGraphicsItem* item);

    /**
     * @brief Returns true if the distance between the last click and the current release exceeds the drag threshold.
     *
     * Used to distinguish an intentional edge drag from an accidental
     * slight movement during a click.
     *
     * @param event The mouse release event to compare against m_lastLeftClickScenePos.
     * @return True if the distance exceeds EDGE_DRAG_START_THRESHOLD.
     */
    bool distanceBetweenClickAndReleaseIsOff(QMouseEvent* event);

    /**
     * @brief Called when the scene's selection changes.
     *
     * Forwards selection change notifications to the scene via
     * onItemSelected() or onItemsDeselected() as appropriate.
     */
    void onSelectionChanged();

    /**
     * @brief Accepts incoming drag-enter events and notifies registered listeners.
     *
     * Calls each callback in m_dragEnterListeners and emits nodeDragEntered().
     *
     * @param event The drag enter event.
     */
    void dragEnterEvent(QDragEnterEvent* event) override;

    /**
     * @brief Handles drop events and notifies registered listeners.
     *
     * Calls each callback in m_dropListeners and emits nodeDropped().
     *
     * @param event The drop event.
     */
    void dropEvent(QDropEvent* event) override;

private:

    /// @brief Interaction mode: no active special operation.
    static const int MODE_NOOP = 1;

    /// @brief Interaction mode: an edge is currently being dragged.
    static const int MODE_EDGE_DRAG = 2;

    /// @brief Minimum pixel distance between click and release to qualify as an edge drag.
    static const int EDGE_DRAG_START_THRESHOLD = 10;

    /// @brief The scene this view is displaying.
    NodeEditorGraphicsScene* m_grScene;

    /// @brief Current interaction mode. One of MODE_NOOP or MODE_EDGE_DRAG.
    int m_mode = MODE_NOOP;

    /// @brief The zoom multiplier applied per zoom step.
    float m_zoomInFactor;

    /// @brief The number of zoom levels to increment or decrement per wheel tick.
    int m_zoomStep;

    /// @brief The current zoom level, expressed as an integer step count.
    int m_zoom;

    /// @brief Whether zoom should be clamped to m_zoomRange.
    bool m_zoomClamp;

    /// @brief The minimum and maximum allowed zoom levels.
    std::pair<int, int> m_zoomRange;

    /// @brief The scene position of the last left mouse button press.
    QPointF m_lastLeftClickScenePos;

    /// @brief The scene position of the most recent mouse move event.
    QPointF m_lastSceneMousePosition;

    /// @brief The temporary edge being dragged from a socket. Null when not dragging.
    Edge* m_dragEdge = nullptr;

    /// @brief The edge that was previously connected to the drag start socket, if any.
    Edge* m_previousEdge = nullptr;

    /// @brief The socket from which the current edge drag originated.
    SocketItem* m_dragStartSocket = nullptr;

    /// @brief Stores the start and end positions of nodes being moved, keyed by node ID.
    /// Used to build MoveNodeCommand entries for undo/redo after a drag completes.
    QMap<qint64, QPair<QPointF, QPointF>> m_moveData;

    /// @brief The list of nodes currently being dragged in a multi-node move.
    QList<NodeItem*> m_draggedNodes;

    /// @brief True while one or more nodes are being actively dragged.
    bool m_nodesAreDragging = false;

    /// @brief The set of node IDs present in the scene before the last interaction began.
    QSet<qint64> m_previousNodeIds;

    /// @brief The set of edge IDs present in the scene before the last interaction began.
    QSet<qint64> m_previousEdgeIds;

    /// @brief Registered callbacks invoked on drag-enter events.
    QList<std::function<void(QDragEnterEvent*)>> m_dragEnterListeners;

    /// @brief Registered callbacks invoked on drop events.
    QList<std::function<void(QDropEvent*)>> m_dropListeners;
};

#endif // NODEEDITORGRAPHICSVIEW_H