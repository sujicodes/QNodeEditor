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

    void onFileNew();
    void onFileOpen();
    bool onFileSave();
    bool onFileSaveAs();
    void onEditCut();
    void onEditCopy();
    void onEditPaste();
    void onEditUndo();
    void onEditRedo();
    void onEditDelete();

    void changeTitle();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    QAction* createAct(const QString &name,
                       const QString &shortcut,
                       const QString &tooltip,
                       const QObject* receiver,
                       const char* member);

    void initUI();

    QString filename;
    QLabel* statusMousePos;
    NodeEditorWidget* nodeEditorWidget;
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
};

#endif // NODEEDITORWINDOW_H
