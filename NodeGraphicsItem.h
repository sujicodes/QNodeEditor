#ifndef NODEGRAPHICSITEM_H
#define NODEGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>
#include <QFont>

class NodeGraphicsItem : public QGraphicsItem {
public:
    NodeGraphicsItem(QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void setTitle(const QString &title);
    QString title() const;
    QWidget* content;
    QWidget* initContent() const;

private:
    void initUI();
    void initTitle();

private:
    QGraphicsTextItem *titleItem;

    QString _title;
    QColor _titleColor = Qt::white;
    QFont _titleFont = QFont("Ubuntu", 10);

    float width = 180;
    float height = 240;
    float edgeSize = 10.0;
    float titleHeight = 24.0;
    float padding = 4.0;

    QPen penDefault = QPen(QColor("#7F000000"));
    QPen penSelected = QPen(QColor("#FFFFA637"));

    QBrush brushTitle = QBrush(QColor("#FF313131"));
    QBrush brushBackground = QBrush(QColor("#E3212121"));
};

#endif // NODEGRAPHICSITEM_H
