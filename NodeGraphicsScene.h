#ifndef NODEGRAPHICSSCENE_H
#define NODEGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QPen>
#include <QColor>

class Scene;

class NodeGraphicsScene : public QGraphicsScene {
    Q_OBJECT

public:
    NodeGraphicsScene(Scene* scene, QObject *parent = nullptr);
    void setGraphicsScene(int width, int height);
    Scene* getScene(){ return m_scene; };


protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    Scene* m_scene;
    int gridSize;
    int gridSquares;
    QColor colorBackground, colorLight, colorDark;
    QPen penLight, penDark;
    int sceneWidth, sceneHeight;
};

#endif // NODEGRAPHICSSCENE_H
