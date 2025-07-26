#ifndef NODEEDITORWINDOW_H
#define NODEEDITORWINDOW_H

#include <QWidget>
#include <QVBoxLayout>

class NodeGraphicsScene;
class NodeEditorGraphicsView;
class Scene;

class NodeEditorWindow : public QWidget {
    Q_OBJECT

public:
    NodeEditorWindow(QWidget *parent = nullptr);

private:
    QVBoxLayout *layout;
    NodeEditorGraphicsView *view;
    NodeGraphicsScene *graphicsScene;
    Scene *scene;
};

#endif // NODEEDITORWINDOW_H
