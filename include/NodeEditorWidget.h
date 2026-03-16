#ifndef NODEEDITORWIDGET_H
#define NODEEDITORWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QString>
#include <QGraphicsItem>

class NodeGraphicsScene;
class NodeEditorGraphicsView;
class NodeEditorGraphicsScene;
class NodeItem;

/**
 * @brief A self-contained widget that hosts the node editor scene and view.
 *
 * NodeEditorWidget combines a NodeEditorGraphicsScene and a NodeEditorGraphicsView
 * into a single reusable QWidget. It provides a high-level API for file operations
 * (new, load, save), selection queries, and undo/redo state — making it suitable
 * for embedding in main windows, MDI sub-windows, or dock widgets.
 *
 * The view can be replaced at runtime via setGraphicsView() to support
 * application-specific subclasses (e.g. CalculatorNodeEditorGraphicsView).
 *
 * @see NodeEditorGraphicsScene
 * @see NodeEditorGraphicsView
 */
class NodeEditorWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * @brief Constructs the widget, creating a default scene and view.
     *
     * Initialises m_scene and m_view, adds the view to the layout,
     * and sets a default geometry. The view can be replaced after
     * construction via setGraphicsView().
     *
     * @param parent Optional parent widget.
     */
    NodeEditorWidget(QWidget* parent = nullptr);

    /**
     * @brief Returns the scene managed by this widget.
     * @return Pointer to the NodeEditorGraphicsScene.
     */
    NodeEditorGraphicsScene* getScene() { return m_scene; }

    /**
     * @brief Returns the view managed by this widget.
     * @return Pointer to the NodeEditorGraphicsView currently in use.
     */
    NodeEditorGraphicsView* getGraphicsView() { return m_view; }

    /**
     * @brief Replaces the current view with a new one.
     *
     * Swaps the view in the layout and deletes the old view. Use this
     * to install an application-specific view subclass after construction.
     *
     * @param view The new view to install. Must not be null.
     */
    void setGraphicsView(NodeEditorGraphicsView* view);

    /**
     * @brief Returns whether the scene has unsaved modifications.
     *
     * Delegates to NodeEditorGraphicsScene::hasBeenModified().
     *
     * @return True if there are unsaved changes, false otherwise.
     */
    bool isModified() const;

    /**
     * @brief Returns whether a filename has been associated with this document.
     *
     * A filename is set after a successful fileLoad() or fileSave() with a
     * non-empty path. Used to determine whether "Save" should prompt for a path.
     *
     * @return True if m_filename is non-empty, false otherwise.
     */
    bool isFilenameSet() const;

    /**
     * @brief Returns a display-friendly version of the current filename.
     *
     * Strips the directory path and returns just the file's base name,
     * suitable for use in window titles or tab labels.
     *
     * @return The base filename, or a placeholder string if no file is set.
     */
    QString getUserFriendlyFilename() const;

    /**
     * @brief Returns the full file path associated with this document.
     * @return The full filename including path, or an empty string if not set.
     */
    QString getFilename() const;

    /**
     * @brief Resets the scene to a new empty state.
     *
     * Clears all nodes and edges, resets the modified flag, and clears
     * the undo/redo history. Also clears m_filename.
     */
    void fileNew();

    /**
     * @brief Loads a scene from the given file path.
     *
     * Delegates to NodeEditorGraphicsScene::loadFromFile(). On success,
     * stores the filename and resets the modified state.
     *
     * @param name The full path to the file to load.
     * @return True if loading succeeded, false if an error occurred.
     */
    bool fileLoad(const QString& name);

    /**
     * @brief Saves the scene to a file.
     *
     * If @p name is provided, saves to that path and updates m_filename.
     * If @p name is empty and m_filename is already set, saves to the
     * existing path. If neither is set, returns false without saving.
     *
     * @param name Optional file path to save to. Defaults to an empty string.
     * @return True if saving succeeded, false otherwise.
     */
    bool fileSave(const QString& name = QString());

    /**
     * @brief Returns the list of currently selected graphics items in the scene.
     *
     * Delegates to NodeEditorGraphicsScene::getSelectedItems().
     *
     * @return A QList of selected QGraphicsItem pointers.
     */
    QList<QGraphicsItem*> getSelectedItems() const;

    /**
     * @brief Returns whether any items are currently selected in the scene.
     * @return True if one or more items are selected, false otherwise.
     */
    bool hasSelectedItems() const;

    /**
     * @brief Returns whether there are any actions available to undo.
     *
     * Delegates to the scene's QUndoStack::canUndo().
     *
     * @return True if an undo operation is available.
     */
    bool canUndo() const;

    /**
     * @brief Returns whether there are any actions available to redo.
     *
     * Delegates to the scene's QUndoStack::canRedo().
     *
     * @return True if a redo operation is available.
     */
    bool canRedo() const;

private:

    /// @brief The vertical layout containing the graphics view.
    QVBoxLayout* m_layout;

    /// @brief The view currently displaying the scene.
    NodeEditorGraphicsView* m_view;

    /// @brief The scene containing all nodes and edges.
    NodeEditorGraphicsScene* m_scene;

    /// @brief The full file path of the currently loaded or saved document. Empty if unset.
    QString m_filename;
};

#endif // NODEEDITORWINDOW_H