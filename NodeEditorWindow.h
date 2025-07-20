#ifndef NODEEDITORWINDOW_H
#define NODEEDITORWINDOW_H

#include <QWidget>
#include <QGraphicsView>
#include <QVBoxLayout>
#include "NodeGraphicsScene.h"

class NodeEditorWindow : public QWidget {
    Q_OBJECT

public:
    NodeEditorWindow(QWidget *parent = nullptr);

private:
    QVBoxLayout *layout;
    QGraphicsView *view;
    NodeGraphicsScene *grScene;
};

#endif // NODEEDITORWINDOW_H
