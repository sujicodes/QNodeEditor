#include "NodeGraphicsScene.h"
#include <QPainter>
#include <cmath>

NodeGraphicsScene::NodeGraphicsScene(QObject *parent)
    : QGraphicsScene(parent),
    gridSize(20), gridSquares(5),
    colorBackground("#393939"), colorLight("#2f2f2f"), colorDark("#292929"),
    penLight(colorLight), penDark(colorDark),
    sceneWidth(64000), sceneHeight(64000)
{
    penLight.setWidth(1);
    penDark.setWidth(2);

    setSceneRect(-sceneWidth/2, -sceneHeight/2, sceneWidth, sceneHeight);
    setBackgroundBrush(colorBackground);
}

void NodeGraphicsScene::drawBackground(QPainter *painter, const QRectF &rect) {
    QGraphicsScene::drawBackground(painter, rect);

    int left = std::floor(rect.left());
    int right = std::ceil(rect.right());
    int top = std::floor(rect.top());
    int bottom = std::ceil(rect.bottom());

    int firstLeft = left - (left % gridSize);
    int firstTop = top - (top % gridSize);

    QVector<QLine> linesLight, linesDark;

    for (int x = firstLeft; x < right; x += gridSize) {
        if (x % (gridSize * gridSquares) != 0)
            linesLight.append(QLine(x, top, x, bottom));
        else
            linesDark.append(QLine(x, top, x, bottom));
    }

    for (int y = firstTop; y < bottom; y += gridSize) {
        if (y % (gridSize * gridSquares) != 0)
            linesLight.append(QLine(left, y, right, y));
        else
            linesDark.append(QLine(left, y, right, y));
    }

    painter->setPen(penLight);
    painter->drawLines(linesLight);

    painter->setPen(penDark);
    painter->drawLines(linesDark);
}
