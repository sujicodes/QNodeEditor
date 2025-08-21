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
    void onFileSave();
    void onFileSaveAs();
    void onEditUndo();
    void onEditRedo();
    void onEditDelete();

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
};

#endif // NODEEDITORWINDOW_H
