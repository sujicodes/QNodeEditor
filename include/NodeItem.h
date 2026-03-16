#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QGraphicsProxyWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QPen>
#include <QBrush>
#include <QFont>

#include "Serializable.h"

class NodeEditorGraphicsScene;
class SocketItem;
class Edge;

/**
 * @brief Base class for all nodes in the node editor.
 *
 * NodeItem represents a single node in the graph — the fundamental building
 * block of the editor. It manages input and output sockets, connected edges,
 * a title bar, an embedded QWidget via QGraphicsProxyWidget, and a dirty/invalid
 * evaluation state system for dataflow graphs.
 *
 * Subclass NodeItem to implement custom node logic by overriding eval(),
 * onInputChanged(), nodeType(), and setItemWidget().
 *
 * NodeItem instances are created through NodeRegistry and should not be
 * constructed directly in application code.
 *
 * @see SocketItem
 * @see Edge
 * @see NodeEditorGraphicsScene
 * @see NodeRegistry
 */
class NodeItem : public QGraphicsItem, public Serializable
{
public:

    /**
     * @brief Constructs a NodeItem and registers it with the given scene.
     *
     * Sets up default geometry, title, and socket counts. Does not create
     * child items — call initNode() (via NodeRegistry) to fully initialise
     * the visual structure.
     *
     * @param scene The scene this node belongs to. Must not be null.
     * @param title The display title shown in the node's title bar.
     * @param in    The number of input sockets to create.
     * @param outs  The number of output sockets to create.
     */
    NodeItem(NodeEditorGraphicsScene* scene,
             const QString& title = "Undefined Node",
             const int& in = 0,
             const int& outs = 0);

    /**
     * @brief Adds an input socket to this node.
     *
     * Appends the socket to m_inputs and sets this node as its parent item.
     *
     * @param input The SocketItem to add as an input. Must not be null.
     */
    void addInput(SocketItem* input);

    /**
     * @brief Adds an output socket to this node.
     *
     * Appends the socket to m_outputs and sets this node as its parent item.
     *
     * @param output The SocketItem to add as an output. Must not be null.
     */
    void addOutput(SocketItem* output);

    /**
     * @brief Calculates the scene position of a socket.
     *
     * Returns the (x, y) offset from the node's origin for a socket at the
     * given index, position slot, and type (input/output). Used by SocketItem
     * and edge rendering to anchor connection endpoints.
     *
     * @param index    The socket's index within its list (0-based).
     * @param position The positional alignment constant (e.g. LEFT_CENTER, RIGHT_CENTER).
     * @param type     The socket type (INPUT or OUTPUT).
     * @return A pair of floats representing the (x, y) local offset.
     */
    std::pair<float, float> getSocketPosition(int index, int position, int type);

    /**
     * @brief Returns the position slot used for input sockets.
     * @return The input socket position constant.
     */
    int getInputSocketPosition() const;

    /**
     * @brief Returns the position slot used for output sockets.
     * @return The output socket position constant.
     */
    int getOutputSocketPosition() const;

    /**
     * @brief Sets the position slot for all input sockets.
     * @param value The new position constant to apply.
     */
    void setInputSocketPosition(int value);

    /**
     * @brief Sets the position slot for all output sockets.
     * @param value The new position constant to apply.
     */
    void setOutputSocketPosition(int value);

    /**
     * @brief Updates the positions of all edges connected to this node's sockets.
     *
     * Calls Edge::updatePositions() on every connected edge, keeping
     * the rendered paths in sync after the node is moved or resized.
     */
    void updateConnectedEdges();

    /**
     * @brief Recalculates and repositions all input and output socket items.
     *
     * Should be called after changing the node's size or socket count
     * to keep socket positions consistent with the node geometry.
     */
    void updateSockets();

    /**
     * @brief Returns all edges connected to any socket of this node.
     * @return A flat QList of Edge pointers covering all input and output sockets.
     */
    QList<Edge*> getConnectedEdges();

    /**
     * @brief Returns the direct child nodes connected to this node's output sockets.
     * @return A vector of NodeItem pointers for each downstream neighbour.
     */
    std::vector<NodeItem*> getChildrenNodes() const;

    /**
     * @brief Returns the node connected to the first edge on the given input socket.
     *
     * @param index The index of the input socket to query (0-based).
     * @return Pointer to the connected upstream NodeItem, or nullptr if unconnected.
     */
    NodeItem* getInput(int index = 0);

    /**
     * @brief Returns all nodes connected to the given input socket.
     *
     * @param index The index of the input socket to query (0-based).
     * @return A QList of upstream NodeItem pointers.
     */
    QList<NodeItem*> getInputNodes(int index = 0);

    /**
     * @brief Returns all nodes connected to the given output socket.
     *
     * @param index The index of the output socket to query (0-based).
     * @return A QList of downstream NodeItem pointers.
     */
    QList<NodeItem*> getOutputNodes(int index = 0);

    /**
     * @brief Returns all input socket items on this node.
     * @return A QList of input SocketItem pointers.
     */
    QList<SocketItem*> getInputSockets() { return m_inputs; }

    /**
     * @brief Returns all output socket items on this node.
     * @return A QList of output SocketItem pointers.
     */
    QList<SocketItem*> getOutputSockets() { return m_outputs; }

    /**
     * @brief Called when an edge is connected or disconnected from any socket.
     *
     * Override to respond to topology changes in the graph. The base
     * implementation does nothing.
     *
     * @param edge The edge that was added or removed.
     */
    virtual void onEdgeConnectionChanged(Edge* edge);

    /**
     * @brief Called when an upstream input value changes.
     *
     * Override to trigger re-evaluation or propagate dirty state when
     * connected input data changes. The base implementation does nothing.
     *
     * @param edge The edge whose source value has changed.
     */
    virtual void onInputChanged(Edge* edge);

    /**
     * @brief Evaluates this node and returns its computed output value.
     *
     * Override in subclasses to implement node-specific computation.
     * The base implementation returns an invalid QVariant.
     *
     * @return A QVariant containing the evaluated output, or an invalid
     *         QVariant if the node has no meaningful output.
     */
    virtual QVariant eval();

    /**
     * @brief Triggers evaluation on all downstream child nodes.
     *
     * Propagates evaluation through the graph by calling eval() on
     * each node in getChildrenNodes().
     */
    void evalChildren();

    /**
     * @brief Returns the type identifier string for this node.
     *
     * Override in subclasses to return a unique type name that matches
     * the string used to register the node in NodeRegistry.
     *
     * @return A QString identifying the node type (e.g. "AddNode").
     */
    virtual QString nodeType() const { return "Node"; }

    /**
     * @brief Serializes this node to a JSON object.
     *
     * Captures position, title, socket configuration, and subclass-specific
     * state for save/load and clipboard operations.
     *
     * @return A QJsonObject representing the full node state.
     */
    QJsonObject serialize() const override;

    /**
     * @brief Deserializes this node from a JSON object.
     *
     * Restores position, title, and socket state. Socket edge references
     * are resolved via the provided hashmap.
     *
     * @param data       The JSON object containing serialized node data.
     * @param hashmap    A map of IDs to Serializable pointers for resolving references.
     * @param restoreId  If true, restores the original unique ID of the node.
     */
    void deserialize(const QJsonObject& data,
                     std::unordered_map<qint64, Serializable*>& hashmap,
                     bool restoreId = true) override;

    /**
     * @brief Sets the display title of the node and updates the title graphics item.
     * @param title The new title string.
     */
    void setTitle(const QString& title);

    /**
     * @brief Returns the current display title of the node.
     * @return The title string.
     */
    QString title() const { return m_title; }

    /**
     * @brief Creates and returns the QWidget to embed inside this node.
     *
     * Override in subclasses to provide a custom widget (e.g. a spin box,
     * label, or preview). The base implementation returns nullptr.
     *
     * @return A heap-allocated QWidget, or nullptr for no embedded widget.
     */
    virtual QWidget* setItemWidget() const;

    /**
     * @brief Returns the currently embedded widget, if any.
     * @return Pointer to the embedded QWidget, or nullptr if none.
     */
    QWidget* getItemWidget() const { return m_itemWidget; }

    /**
     * @brief Returns whether this node is marked as dirty (needs re-evaluation).
     * @return True if the node's output is stale and requires recomputation.
     */
    bool isDirty() const;

    /**
     * @brief Sets the dirty state of this node.
     * @param newValue True to mark as dirty, false to mark as clean.
     */
    void markDirty(bool newValue = true);

    /**
     * @brief Marks all direct child nodes as dirty.
     * @param newValue True to mark children dirty, false to mark clean.
     */
    void markChildrenDirty(bool newValue = true);

    /**
     * @brief Recursively marks all downstream descendants as dirty.
     * @param newValue True to mark descendants dirty, false to mark clean.
     */
    void markDescendantsDirty(bool newValue = true);

    /**
     * @brief Returns whether this node is marked as invalid.
     *
     * An invalid node has encountered an error during evaluation (e.g.
     * type mismatch, missing input) and cannot produce a valid output.
     *
     * @return True if the node is in an invalid state.
     */
    bool isInvalid() const;

    /**
     * @brief Sets the invalid state of this node.
     * @param newValue True to mark as invalid, false to clear the invalid state.
     */
    void markInvalid(bool newValue = true);

    /**
     * @brief Called when this node is marked invalid.
     *
     * Override to trigger visual or logical responses to an invalid state.
     * The base implementation does nothing.
     */
    void onMarkedInvalid() { return; }

    /**
     * @brief Marks all direct child nodes as invalid.
     * @param newValue True to mark children invalid, false to clear.
     */
    void markChildrenInvalid(bool newValue = true);

    /**
     * @brief Recursively marks all downstream descendants as invalid.
     * @param newValue True to mark descendants invalid, false to clear.
     */
    void markDescendantsInvalid(bool newValue = true);

    /**
     * @brief Returns the bounding rectangle of the node in local coordinates.
     *
     * Used by Qt for culling, hit testing, and dirty region updates.
     *
     * @return A QRectF encompassing the full node body including title bar.
     */
    QRectF boundingRect() const override;

    /**
     * @brief Paints the node body, title bar, and selection highlight.
     *
     * Draws the background rounded rectangle, title area, and switches
     * between m_penDefault and m_penSelected based on selection state.
     *
     * @param painter The QPainter to draw with.
     * @param option  Style options including selection state.
     * @param widget  The widget being painted on (may be null).
     */
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

    /**
     * @brief Handles mouse release on the node to detect selection state changes.
     *
     * Compares the current selection state against m_lastSelectedState and
     * updates the visual appearance if the state has changed.
     *
     * @param event The mouse release event.
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    /**
     * @brief Returns the width of the node in scene units.
     * @return Node width as a float.
     */
    float getWidth() const { return m_width; }

    /**
     * @brief Returns the height of the node in scene units.
     * @return Node height as a float.
     */
    float getHeight() const { return m_height; }

    /**
     * @brief Returns the height of the title bar area in scene units.
     * @return Title bar height as a float.
     */
    float getTitleHeight() const { return m_titleHeight; }

    /**
     * @brief Returns the corner roundness radius used when painting the node body.
     * @return Edge roundness as an integer.
     */
    int getEdgeRoundness() const { return m_edgeRoundness; }

    /**
     * @brief Returns the inner padding between the node border and its content area.
     * @return Edge padding as an integer.
     */
    int getEdgePadding() const { return m_edgePadding; }

    /**
     * @brief Returns the horizontal padding applied to the title text item.
     * @return Title horizontal padding as an integer.
     */
    int getTitleHorizontalPadding() const { return m_titleHorizontalPadding; }

    /**
     * @brief Returns the vertical padding applied to the title text item.
     * @return Title vertical padding as an integer.
     */
    int getTitleVerticalPadding() const { return m_titleVerticalPadding; }

    /**
     * @brief Sets the width of the node.
     * @param w New width in scene units.
     */
    void setWidth(float w) { m_width = w; }

    /**
     * @brief Sets the height of the node.
     * @param h New height in scene units.
     */
    void setHeight(float h) { m_height = h; }

    /**
     * @brief Sets the height of the title bar area.
     * @param t New title height in scene units.
     */
    void setTitleHeight(float t) { m_titleHeight = t; }

    /**
     * @brief Caches the current selection state for change detection on the next mouse release.
     * @param s The selection state to store.
     */
    void setLastSelectedState(bool s) { m_lastSelectedState = s; }

    /**
     * @brief Sets the corner roundness radius for painting the node body.
     * @param value New roundness value.
     */
    void setEdgeRoundness(int value) { m_edgeRoundness = value; }

    /**
     * @brief Sets the inner padding between the node border and its content.
     * @param value New padding value in scene units.
     */
    void setEdgePadding(int value) { m_edgePadding = value; }

    /**
     * @brief Sets the horizontal padding for the title text item.
     * @param value New horizontal padding value in scene units.
     */
    void setTitleHorizontalPadding(int value) { m_titleHorizontalPadding = value; }

    /**
     * @brief Sets the vertical padding for the title text item.
     * @param value New vertical padding value in scene units.
     */
    void setTitleVerticalPadding(int value) { m_titleVerticalPadding = value; }

    /**
     * @brief Called when this node has been moved in the scene.
     *
     * Triggers updateConnectedEdges() to keep all attached edge paths
     * in sync with the new node position.
     */
    void onNodeMoved();

    /**
     * @brief Removes this node from the scene, disconnecting all edges first.
     *
     * Calls removeFromSockets() on all connected edges, removes this item
     * from the scene, and schedules deletion.
     */
    void remove();

private:

    /// @brief The scene this node belongs to.
    NodeEditorGraphicsScene* scene;

    /// @brief The display title shown in the node's title bar.
    QString m_title;

    /// @brief All input sockets attached to this node.
    QList<SocketItem*> m_inputs;

    /// @brief All output sockets attached to this node.
    QList<SocketItem*> m_outputs;

    /// @brief The text item rendering the node title in the title bar.
    QGraphicsTextItem* m_titleItem = nullptr;

    /// @brief The proxy widget embedding m_itemWidget into the graphics scene.
    QGraphicsProxyWidget* m_graphicsProxyWidget = nullptr;

    /// @brief The embedded application widget displayed inside the node body.
    QWidget* m_itemWidget = nullptr;

    /// @brief Width of the node in scene units.
    float m_width = 180;

    /// @brief Height of the node in scene units.
    float m_height = 240;

    /// @brief Height of the title bar area in scene units.
    float m_titleHeight = 24.0f;

    /// @brief Corner roundness radius used when drawing the node's rounded rectangle.
    int m_edgeRoundness = 0;

    /// @brief Inner padding between the node border and its content area.
    int m_edgePadding = 0;

    /// @brief Horizontal padding applied to the title text item.
    int m_titleHorizontalPadding = 8;

    /// @brief Vertical padding applied to the title text item.
    int m_titleVerticalPadding = 10;

    /// @brief The position slot constant used to place input sockets.
    int m_inputSocketPosition;

    /// @brief The position slot constant used to place output sockets.
    int m_outputSocketPosition;

    /// @brief Cached selection state from the previous event, used to detect selection changes.
    bool m_lastSelectedState = false;

    /// @brief Whether this node's output is stale and requires re-evaluation.
    bool m_isDirty = false;

    /// @brief Whether this node is in an error/invalid state and cannot produce valid output.
    bool m_isInvalid = false;

    /// @brief Vertical spacing between consecutive sockets in scene units.
    float m_socketSpacing = 22.0f;

    /// @brief Whether initNode() has been called and the node is fully constructed.
    bool m_initialized = false;

    /// @brief The number of input sockets to create during initialisation.
    int m_inputSize;

    /// @brief The number of output sockets to create during initialisation.
    int m_outputSize;

    /// @brief Pen used to draw the node outline in its default (unselected) state.
    QPen m_penDefault = QPen(QColor("#7F000000"));

    /// @brief Pen used to draw the node outline when the node is selected.
    QPen m_penSelected;

    /// @brief Brush used to fill the title bar area.
    QBrush m_brushTitle;

    /// @brief Brush used to fill the node body background.
    QBrush m_brushBackground;

    /// @brief NodeRegistry is a friend so it can call initNode() after construction.
    friend class NodeRegistry;

    /**
     * @brief Fully initialises the node's visual child items.
     *
     * Creates the title QGraphicsTextItem, embeds the widget via
     * QGraphicsProxyWidget, and instantiates all input and output
     * SocketItems. Called by NodeRegistry immediately after construction.
     */
    void initNode();
};

#endif