#include "NodeEditorGraphicsView.h"
#include <QPainter>

NodeEditorGraphicsView::NodeEditorGraphicsView(QGraphicsScene* grScene, QWidget* parent)
    : QGraphicsView(parent), m_grScene(grScene) {
    initUI();
    setScene(m_grScene);
}

void NodeEditorGraphicsView::initUI() {
    // Enable high-quality rendering
    setRenderHints(QPainter::Antialiasing |
                   QPainter::TextAntialiasing |
                   QPainter::SmoothPixmapTransform);
    //QPainter::HighQualityAntialiasing might need in qt5

    // Ensure the entire viewport updates
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // Disable scrollbars
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}
