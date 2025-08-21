#ifndef NODEEDITORWIDGET_H
#define NODEEDITORWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

class NodeGraphicsScene;
class NodeEditorGraphicsView;
class Scene;
class Node;

class NodeEditorWidget : public QWidget {
    Q_OBJECT

public:
    NodeEditorWidget(QWidget *parent = nullptr);
    Scene* getScene() { return scene; }
    NodeEditorGraphicsView* getGraphicsView() { return view; }


private:
    QVBoxLayout *layout;
    NodeEditorGraphicsView *view;
    NodeGraphicsScene *graphicsScene;
    Scene *scene;
};

#endif // NODEEDITORWINDOW_H
