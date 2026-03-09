#ifndef NODEEDITORWINDOW_H
#define NODEEDITORWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QLabel>

class NodeEditorWidget;  // forward declare

class NodeEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit NodeEditorWindow(QWidget *parent = nullptr);

private slots:
    void onScenePosChanged(int x, int y);

    virtual void onFileNew();
    virtual void onFileOpen();
    virtual bool onFileSave();
    virtual bool onFileSaveAs();
    void onEditCut();
    void onEditCopy();
    void onEditPaste();
    void onEditUndo();
    void onEditRedo();
    void onEditDelete();

    void setTitle();

protected:
    void closeEvent(QCloseEvent* event) override;
    virtual NodeEditorWidget* getCurrentNodeEditorWidget() const;
    QLabel* statusMousePos;
    QString filename;
    bool isModified() const;
    bool maybeSave();

    void createActions();
    void createMenus();

    // Actions
    QAction *actNew;
    QAction *actOpen;
    QAction *actSave;
    QAction *actSaveAs;
    QAction *actExit;

    QAction *actUndo;
    QAction *actRedo;
    QAction *actCut;
    QAction *actCopy;
    QAction *actPaste;
    QAction *actDelete;

private:
    QAction* createAct(const QString &name,
                       const QString &shortcut,
                       const QString &tooltip,
                       const QObject* receiver,
                       const char* member);

    void initUI();
    NodeEditorWidget* nodeEditorWidget = nullptr;
    
};

#endif // NODEEDITORWINDOW_H
