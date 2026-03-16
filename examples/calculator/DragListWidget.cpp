#include "DragListWidget.h"

#include <QListWidgetItem>
#include <QPixmap>
#include <QIcon>
#include <QAbstractItemView>
#include <QDebug>
#include <QtCore/qresource.h>
#include <qcborcommon.h>
#include <QSize>
#include <QDataStream>
#include <QByteArray>

#include <QDataStream>
#include <QByteArray>
#include <QIODevice>
#include <QDrag>
#include <QMimeData>

DragListWidget::DragListWidget(QWidget* parent)
    : QListWidget(parent)
{
    initUI();
}

void DragListWidget::initUI()
{
    setIconSize(QSize(32, 32));
    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragEnabled(true);
}

void DragListWidget::addCustomItems(const QStringList& keys,
                                    const QStringList& paths,
                                    const QStringList& classNames)
{
    if(keys.size() != paths.size() || paths.size() != classNames.size())
    {
        qWarning() << "Input items are not of the same size";
        return;
    }

    for(int i = 0; i < keys.size(); ++i)
    {
        addIconItem(keys[i], paths[i], classNames[i]);
    }
}

void DragListWidget::addIconItem(const QString& name,
                               const QString& iconPath,
                               const QString& className)
{
    auto* item = new QListWidgetItem(name, this);

    QPixmap pixmap;
    if(!iconPath.isEmpty())
    {
        if(!pixmap.load(iconPath))
        {
            qWarning() << "Failed to load icon:" << iconPath;
        }
    }
    else
    {
        pixmap = QPixmap(32, 32);
        pixmap.fill(Qt::transparent);
    }

    item->setIcon(QIcon(pixmap));
    item->setSizeHint(QSize(32, 32));

    item->setFlags(Qt::ItemIsEnabled |
                   Qt::ItemIsSelectable |
                   Qt::ItemIsDragEnabled);

    // Match PyQt data roles
    item->setData(Qt::UserRole, pixmap);
    item->setData(Qt::UserRole + 1, className);
}

void DragListWidget::startDrag(Qt::DropActions /*supportedActions*/)
{
    auto* item = currentItem();
    if(!item) return;

    // Retrieve the operation code
    QString className = item->data(Qt::UserRole + 1).toString();

    QPixmap pixmap = qvariant_cast<QPixmap>(item->data(Qt::UserRole));

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    qDebug() << "text length:" << QString(item->text()).length();

    dataStream << QString(item->text());
    dataStream << QString(className);
    dataStream << item->data(Qt::UserRole).value<QPixmap>();

    QMimeData* mimeData = new QMimeData;
    mimeData->setData("application/x-item", itemData);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));

    drag->exec(Qt::MoveAction);
}