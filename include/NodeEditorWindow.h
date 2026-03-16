#ifndef NODEEDITORWINDOW_H
#define NODEEDITORWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QLabel>

class NodeEditorWidget;

/**
 * @brief The main application window for the node editor.
 *
 * NodeEditorWindow provides a QMainWindow shell around a NodeEditorWidget,
 * wiring up a full menu bar and status bar with standard File and Edit actions.
 * It handles the application lifecycle: new/open/save/save-as file operations,
 * clipboard cut/copy/paste, undo/redo, deletion, and unsaved-changes prompting
 * on close.
 *
 * Subclass this and override the virtual slots and getCurrentNodeEditorWidget()
 * to support MDI layouts or application-specific editor variants.
 *
 * @see NodeEditorWidget
 */
class NodeEditorWindow : public QMainWindow
{
    Q_OBJECT

public:

    /**
     * @brief Constructs the main window, building the UI, menus, and actions.
     *
     * Calls initUI(), createActions(), and createMenus(). Also creates and
     * embeds a default NodeEditorWidget as the central widget.
     *
     * @param parent Optional parent widget.
     */
    explicit NodeEditorWindow(QWidget* parent = nullptr);

signals:

    /**
     * @brief Emitted after the current document is successfully saved.
     *
     * @param filename The full file path that was written to.
     */
    void onFileSaved(const QString& filename);

private slots:

    /**
     * @brief Updates the status bar mouse position label.
     *
     * Connected to NodeEditorGraphicsView::scenePosChanged().
     *
     * @param x The current mouse X position in scene coordinates.
     * @param y The current mouse Y position in scene coordinates.
     */
    void onScenePosChanged(int x, int y);

    /**
     * @brief Creates a new empty document.
     *
     * Prompts to save if the current scene is modified, then resets
     * the scene via NodeEditorWidget::fileNew().
     */
    virtual void onFileNew();

    /**
     * @brief Opens a file dialog and loads a document.
     *
     * Prompts to save if modified, shows a file dialog, then delegates
     * to NodeEditorWidget::fileLoad().
     */
    virtual void onFileOpen();

    /**
     * @brief Saves the current document to its existing file path.
     *
     * If no filename is set, falls through to onFileSaveAs().
     *
     * @return True if the save succeeded, false if cancelled or failed.
     */
    virtual bool onFileSave();

    /**
     * @brief Prompts for a file path and saves the current document.
     *
     * Shows a save file dialog, then delegates to NodeEditorWidget::fileSave().
     *
     * @return True if the save succeeded, false if cancelled or failed.
     */
    virtual bool onFileSaveAs();

    /**
     * @brief Cuts the selected items to the clipboard.
     *
     * Serializes the selection, copies to clipboard, then removes the
     * selected items from the scene.
     */
    void onEditCut();

    /**
     * @brief Copies the selected items to the clipboard.
     *
     * Serializes the selection to clipboard without removing items from
     * the scene.
     */
    void onEditCopy();

    /**
     * @brief Pastes items from the clipboard into the scene.
     *
     * Deserializes clipboard data and inserts new node/edge instances
     * into the current scene at an offset from their original positions.
     */
    void onEditPaste();

    /**
     * @brief Undoes the last action on the scene's undo stack.
     */
    void onEditUndo();

    /**
     * @brief Redoes the last undone action on the scene's undo stack.
     */
    void onEditRedo();

    /**
     * @brief Deletes all currently selected items from the scene.
     *
     * Delegates to NodeEditorGraphicsView::deleteSelected().
     */
    void onEditDelete();

    /**
     * @brief Updates the window title to reflect the current filename and modified state.
     *
     * Typically called after file operations or when the scene's modified
     * flag changes.
     */
    void setTitle();

protected:

    /**
     * @brief Intercepts the window close event to prompt for unsaved changes.
     *
     * Calls maybeSave(). If the user cancels, the event is ignored and
     * the window remains open.
     *
     * @param event The close event.
     */
    void closeEvent(QCloseEvent* event) override;

    /**
     * @brief Returns the currently active NodeEditorWidget.
     *
     * The base implementation returns m_nodeEditorWidget. Override in
     * subclasses that manage multiple editors (e.g. MDI) to return the
     * currently focused widget instead.
     *
     * @return Pointer to the active NodeEditorWidget, or nullptr if none.
     */
    virtual NodeEditorWidget* getCurrentNodeEditorWidget() const;

    /// @brief Status bar label displaying the current mouse scene coordinates.
    QLabel* m_statusMousePos;

    /**
     * @brief Returns whether the current document has unsaved modifications.
     *
     * Delegates to NodeEditorWidget::isModified() on the current widget.
     *
     * @return True if the scene has been modified since last save.
     */
    bool isModified() const;

    /**
     * @brief Prompts the user to save if the scene has unsaved changes.
     *
     * Shows a message box with Save / Discard / Cancel options.
     * Calls onFileSave() if the user chooses to save.
     *
     * @return True if it is safe to proceed (saved or discarded),
     *         false if the user cancelled.
     */
    bool maybeSave();

    /**
     * @brief Creates all QAction instances and connects them to their slots.
     *
     * Sets up keyboard shortcuts, tooltips, and signal-slot connections
     * for all File and Edit menu actions.
     */
    void createActions();

    /**
     * @brief Builds the menu bar using the actions created by createActions().
     *
     * Constructs the File and Edit menus and populates them with the
     * appropriate actions and separators.
     */
    void createMenus();

    /// @brief Action for creating a new document (File > New).
    QAction* m_actNew;

    /// @brief Action for opening an existing document (File > Open).
    QAction* m_actOpen;

    /// @brief Action for saving the current document (File > Save).
    QAction* m_actSave;

    /// @brief Action for saving the document to a new path (File > Save As).
    QAction* m_actSaveAs;

    /// @brief Action for quitting the application (File > Exit).
    QAction* m_actExit;

    /// @brief Action for undoing the last operation (Edit > Undo).
    QAction* m_actUndo;

    /// @brief Action for redoing the last undone operation (Edit > Redo).
    QAction* m_actRedo;

    /// @brief Action for cutting the selection to the clipboard (Edit > Cut).
    QAction* m_actCut;

    /// @brief Action for copying the selection to the clipboard (Edit > Copy).
    QAction* m_actCopy;

    /// @brief Action for pasting from the clipboard (Edit > Paste).
    QAction* m_actPaste;

    /// @brief Action for deleting the current selection (Edit > Delete).
    QAction* m_actDelete;

private:

    /**
     * @brief Convenience factory for creating a QAction with shortcut, tooltip, and connection.
     *
     * @param name     The display text of the action.
     * @param shortcut The keyboard shortcut string (e.g. "Ctrl+S").
     * @param tooltip  The tooltip string shown on hover.
     * @param receiver The object to connect the triggered() signal to.
     * @param member   The slot to invoke when the action is triggered.
     * @return A fully configured QAction pointer.
     */
    QAction* createAct(const QString& name,
                        const QString& shortcut,
                        const QString& tooltip,
                        const QObject* receiver,
                        const char* member);

    /**
     * @brief Initialises the main window geometry, status bar, and central widget.
     *
     * Called once from the constructor before createActions() and createMenus().
     */
    void initUI();

    /// @brief The node editor widget embedded as the central widget.
    NodeEditorWidget* m_nodeEditorWidget = nullptr;
};

#endif // NODEEDITORWINDOW_H