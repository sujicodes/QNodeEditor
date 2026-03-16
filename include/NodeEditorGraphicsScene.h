#ifndef NODEEDITORGRAPHICSSCENE_H
#define NODEEDITORGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QObject>
#include <QVector>
#include <QUndoStack>
#include <QPen>
#include <qevent.h>
#include "Serializable.h"

class NodeItem;
class Edge;

/**
 * @brief The core scene class for the node editor, managing nodes, edges, history, and serialization.
 *
 * NodeEditorGraphicsScene extends QGraphicsScene with node-editor-specific logic:
 * node and edge lifetime management, undo/redo history, clipboard operations,
 * selection tracking, background grid rendering, and save/load functionality.
 *
 * It also implements Serializable to support full scene serialization to JSON,
 * enabling file persistence and clipboard-based copy/paste workflows.
 *
 * @see NodeItem
 * @see Edge
 * @see NodeEditorGraphicsView
 */
class NodeEditorGraphicsScene : public QGraphicsScene, public Serializable
{
    Q_OBJECT
    friend class NodeEditorGraphicsView;

public:

    /**
     * @brief Constructs the scene and initialises grid settings, history stack, and default colours.
     */
    NodeEditorGraphicsScene();

    /**
     * @brief Sets the logical dimensions of the scene.
     *
     * @param width  The scene width in scene units.
     * @param height The scene height in scene units.
     */
    void setGraphicsScene(int width, int height);

    /**
     * @brief Creates and adds a new node of the given type to the scene.
     *
     * Looks up the type in the NodeRegistry and delegates construction to
     * the registered factory. The node is added to the scene automatically.
     *
     * @param type The registered class name of the node to create (e.g. "AddNode").
     * @return Pointer to the newly created NodeItem, or nullptr if the type is unknown.
     */
    NodeItem* createNode(QString type);

    /**
     * @brief Adds an existing NodeItem to the scene.
     *
     * Appends the node to m_nodes and adds its graphics item to the QGraphicsScene.
     *
     * @param node The node to add. Must not be null.
     */
    void addNode(NodeItem* node);

    /**
     * @brief Adds an existing Edge to the scene.
     *
     * Appends the edge to m_edges and adds its graphics item to the QGraphicsScene.
     *
     * @param edge The edge to add. Must not be null.
     */
    void addEdge(Edge* edge);

    /**
     * @brief Removes a NodeItem from the scene without deleting it.
     *
     * Removes the node from m_nodes and from the QGraphicsScene item list.
     *
     * @param node The node to remove.
     */
    void removeNode(NodeItem* node);

    /**
     * @brief Removes an Edge from the scene without deleting it.
     *
     * Removes the edge from m_edges and from the QGraphicsScene item list.
     *
     * @param edge The edge to remove.
     */
    void removeEdge(Edge* edge);

    /**
     * @brief Returns all edges currently in the scene.
     * @return A const reference to the vector of Edge pointers.
     */
    const QVector<Edge*>& getEdges() const { return m_edges; }

    /**
     * @brief Returns all nodes currently in the scene.
     * @return A const reference to the vector of NodeItem pointers.
     */
    const QVector<NodeItem*>& getNodes() const { return m_nodes; }

    /**
     * @brief Returns the undo/redo history stack for this scene.
     * @return Pointer to the scene's QUndoStack.
     */
    QUndoStack* getHistory() const { return m_history; }

    /**
     * @brief Clears all nodes and edges from the scene.
     *
     * Removes and deletes all NodeItem and Edge objects, and clears
     * the underlying QGraphicsScene. Does not reset the undo history.
     */
    void clearScene();

    /**
     * @brief Loads the scene state from a JSON file.
     *
     * Deserializes nodes and edges from the given file path, replacing
     * the current scene contents.
     *
     * @param filename The path to the JSON file to load.
     * @param errorMsg If non-null, populated with a description of any error that occurred.
     * @return True if loading succeeded, false otherwise.
     */
    bool loadFromFile(const QString& filename, QString* errorMsg);

    /**
     * @brief Saves the current scene state to a JSON file.
     *
     * Serializes all nodes and edges to the given file path.
     *
     * @param filename The path to the JSON file to write.
     * @return True if saving succeeded, false otherwise.
     */
    bool saveToFile(const QString& filename);

    /**
     * @brief Serializes the entire scene to a JSON object.
     *
     * Captures all nodes and edges, including their positions,
     * connections, and internal state.
     *
     * @return A QJsonObject representing the full scene.
     */
    QJsonObject serialize() const override;

    /**
     * @brief Deserializes the scene from a JSON object.
     *
     * Reconstructs all nodes and edges from the provided data,
     * using the hashmap to resolve cross-references between objects.
     *
     * @param data       The JSON object containing serialized scene data.
     * @param hashmap    A map of IDs to Serializable pointers for resolving references.
     * @param restoreId  If true, restores original unique IDs for all objects.
     */
    void deserialize(
        const QJsonObject& data,
        std::unordered_map<qint64, Serializable*>& hashmap,
        bool restoreId = true
    ) override;

    /**
     * @brief Serializes only the currently selected nodes and edges to a JSON object.
     *
     * Used for clipboard copy/cut operations. Only edges where both endpoints
     * are within the selection are included.
     *
     * @param del If true, removes the selected items from the scene after serializing (cut).
     * @return A QJsonObject containing the serialized selected items.
     */
    QJsonObject serializeSelected(bool del = true);

    /**
     * @brief Deserializes and pastes nodes and edges from clipboard JSON data into the scene.
     *
     * Creates new instances of all nodes and edges described in the data,
     * assigning fresh IDs to avoid conflicts with existing scene objects.
     *
     * @param data The JSON object produced by serializeSelected().
     */
    void deserializeFromClipboard(const QJsonObject& data);

    /**
     * @brief Finds and returns a node by its unique ID.
     *
     * @param id The unique qint64 identifier of the node.
     * @return Pointer to the matching NodeItem, or nullptr if not found.
     */
    NodeItem* getNodeById(qint64 id) const;

    /**
     * @brief Finds and returns an edge by its unique ID.
     *
     * @param id The unique qint64 identifier of the edge.
     * @return Pointer to the matching Edge, or nullptr if not found.
     */
    Edge* getEdgeById(qint64 id) const;

    /**
     * @brief Returns whether the scene has unsaved modifications.
     * @return True if the scene has been modified since last save.
     */
    bool hasBeenModified() const;

    /**
     * @brief Sets the modified state of the scene.
     *
     * When set to true, notifies all registered hasBeenModified listeners.
     *
     * @param value The new modified state.
     */
    void setHasBeenModified(bool value);

    /**
     * @brief Registers a callback to be invoked when the scene's modified state changes.
     *
     * Useful for updating window titles or enabling save buttons in response
     * to scene edits.
     *
     * @param callback A callable with signature `void()`.
     */
    void addHasBeenModifiedListener(const std::function<void()>& callback);

    /**
     * @brief Registers a callback to be invoked when any item in the scene is selected.
     *
     * @param callback A callable with signature `void()`.
     */
    void addItemSelectedListener(const std::function<void()>& callback);

    /**
     * @brief Registers a callback to be invoked when all items in the scene are deselected.
     *
     * @param callback A callable with signature `void()`.
     */
    void addItemsDeselectedListener(const std::function<void()>& callback);

    /**
     * @brief Returns the list of currently selected graphics items.
     * @return A QList of selected QGraphicsItem pointers.
     */
    QList<QGraphicsItem*> getSelectedItems() const { return selectedItems(); }

    /**
     * @brief Returns the list of items that were selected in the previous selection event.
     *
     * Used to detect selection changes and drive visual state updates (e.g. edge highlight).
     *
     * @return A QList of QGraphicsItem pointers from the last selection.
     */
    QList<QGraphicsItem*> getLastSelectedItems() { return m_lastSelectedItems; }

    /**
     * @brief Called when one or more items become selected in the scene.
     *
     * Updates m_lastSelectedItems and notifies all itemSelected listeners.
     */
    void onItemSelected();

    /**
     * @brief Called when all items in the scene are deselected.
     *
     * Notifies all itemsDeselected listeners and resets last selected item tracking.
     */
    void onItemsDeselected();

    /**
     * @brief Resets the cached selection state on all graphics items.
     *
     * Iterates over m_lastSelectedItems and clears their last-selected flags,
     * typically called before processing a new selection change.
     */
    void resetLastSelectedStates();

    /**
     * @brief Returns the first view currently displaying this scene.
     *
     * Convenience wrapper around QGraphicsScene::views().first().
     *
     * @return Pointer to the first QGraphicsView, or nullptr if none are attached.
     */
    QGraphicsView* getView();

signals:

    /**
     * @brief Emitted when one or more items in the scene become selected.
     */
    void itemSelected();

    /**
     * @brief Emitted when all items in the scene are deselected.
     */
    void itemsDeselected();

protected:

    /**
     * @brief Draws the scene background grid.
     *
     * Renders a two-level grid (light and dark lines) using m_penLight and m_penDark,
     * sized according to m_gridSize and m_gridSquares.
     *
     * @param painter The QPainter to draw with.
     * @param rect    The visible rectangle to draw within.
     */
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    /**
     * @brief Accepts drag move events to allow drops onto the scene.
     *
     * @param event The drag move event.
     */
    void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;

private:

    /// @brief All nodes currently managed by this scene.
    QVector<NodeItem*> m_nodes;

    /// @brief All edges currently managed by this scene.
    QVector<Edge*> m_edges;

    /// @brief The logical width of the scene in scene units.
    int m_sceneWidth;

    /// @brief The logical height of the scene in scene units.
    int m_sceneHeight;

    /// @brief The undo/redo stack for all undoable operations in this scene.
    QUndoStack* m_history;

    /// @brief Tracks whether the scene has unsaved changes.
    bool m_hasBeenModified = false;

    /// @brief The items that were selected during the most recent selection event.
    QList<QGraphicsItem*> m_lastSelectedItems;

    /// @brief Callbacks invoked when an item becomes selected.
    std::vector<std::function<void()>> m_itemSelectedListeners;

    /// @brief Callbacks invoked when all items are deselected.
    std::vector<std::function<void()>> m_itemsDeselectedListeners;

    /// @brief Callbacks invoked when the scene's modified state changes.
    std::vector<std::function<void()>> m_hasBeenModifiedListeners;

    /// @brief The size of each small grid cell in scene units.
    int m_gridSize;

    /// @brief The number of small grid cells per large grid square.
    int m_gridSquares;

    /// @brief Background fill colour of the scene.
    QColor m_colorBackground;

    /// @brief Colour used for minor (light) grid lines.
    QColor m_colorLight;

    /// @brief Colour used for major (dark) grid lines.
    QColor m_colorDark;

    /// @brief Pen used to draw minor grid lines.
    QPen m_penLight;

    /// @brief Pen used to draw major grid lines.
    QPen m_penDark;
};

#endif // NODEEDITORGRAPHICSSCENE_H