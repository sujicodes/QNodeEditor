#ifndef NODEGRAPHICSSCENE_H
#define NODEGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QPen>
#include <QColor>
#include <QGraphicsItem>

class Scene;

class NodeGraphicsScene : public QGraphicsScene {
    Q_OBJECT
    friend class NodeEditorGraphicsView;
    friend class Scene;

public:
    NodeGraphicsScene(Scene* scene, QObject *parent = nullptr);
    void setGraphicsScene(int width, int height);
    Scene* getScene(){ return m_scene; }
    QList<QGraphicsItem*> getLastSelectedItems(){return lastSelectedItems;}

signals:
    void itemSelected();
    void itemsDeselected();


protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;

private:
    Scene* m_scene;
    int gridSize;
    int gridSquares;
    QColor colorBackground, colorLight, colorDark;
    QPen penLight, penDark;
    int sceneWidth, sceneHeight;
    QList<QGraphicsItem*> lastSelectedItems;
};

#endif // NODEGRAPHICSSCENE_H
