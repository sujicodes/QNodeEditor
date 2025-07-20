#ifndef NODEGRAPHICSSCENE_H
#define NODEGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QPen>
#include <QColor>

class NodeGraphicsScene : public QGraphicsScene {
    Q_OBJECT

public:
    NodeGraphicsScene(QObject *parent = nullptr);

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    int gridSize;
    int gridSquares;
    QColor colorBackground, colorLight, colorDark;
    QPen penLight, penDark;
    int sceneWidth, sceneHeight;
};

#endif // NODEGRAPHICSSCENE_H
