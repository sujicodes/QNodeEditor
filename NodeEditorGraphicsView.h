#ifndef NODEEDITORGRAPHICSVIEW_H
#define NODEEDITORGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWidget>

class NodeEditorGraphicsView : public QGraphicsView  {
    Q_OBJECT

public:
    NodeEditorGraphicsView(QGraphicsScene* grScene, QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:

    QGraphicsScene* m_grScene;
    double zoomInFactor = 1.25;
    bool zoomClamp = true;
    int zoom = 10;
    int zoomStep = 1;
    std::pair<int, int> zoomRange = {0, 20};

    void initUI();

    void middleMouseButtonPress(QMouseEvent* event);
    void middleMouseButtonRelease(QMouseEvent* event);
    void leftMouseButtonPress(QMouseEvent* event);
    void leftMouseButtonRelease(QMouseEvent* event);
    void rightMouseButtonPress(QMouseEvent* event);
    void rightMouseButtonRelease(QMouseEvent* event);

};

#endif // NODEEDITORGRAPHICSVIEW_H
