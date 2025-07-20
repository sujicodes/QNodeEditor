#ifndef NODEEDITORGRAPHICSVIEW_H
#define NODEEDITORGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWidget>

class NodeEditorGraphicsView : public QGraphicsView  {
    Q_OBJECT

public:
    NodeEditorGraphicsView(QGraphicsScene* grScene, QWidget* parent = nullptr);

private:
QGraphicsScene* m_grScene;

void initUI();
};

#endif // NODEEDITORGRAPHICSVIEW_H
