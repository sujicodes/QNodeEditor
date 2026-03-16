#include <QApplication>
#include <QCursor>
#include <QDebug>
#include "NodeSelectorPopupWidget.h"
#include "NodeEditorGraphicsScene.h"
#include "UndoCommands.h"

NodeSelectorPopupWidget::NodeSelectorPopupWidget(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::Popup);
    setFocusPolicy(Qt::StrongFocus);

    m_searchBox = new QLineEdit(this);
    m_searchBox->setPlaceholderText("Search nodes...");
    m_searchBox->setFocus();

    m_listWidget = new QListWidget(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->addWidget(m_searchBox);
    layout->addWidget(m_listWidget);
    setLayout(layout);

    connect(m_searchBox,  &QLineEdit::textChanged,
            this,          &NodeSelectorPopupWidget::filterList);
    connect(m_listWidget, &QListWidget::itemClicked,
            this,          &NodeSelectorPopupWidget::onItemActivated);
}

void NodeSelectorPopupWidget::setScene(NodeEditorGraphicsScene *scene)
{
    m_scene = scene;
}

void NodeSelectorPopupWidget::setNodeList(const QStringList &displayNames,
                                          const QStringList &classNames)
{
    m_listWidget->clear();
    for (int i = 0; i < displayNames.size(); ++i) {
        auto *item = new QListWidgetItem(displayNames[i], m_listWidget);
        item->setData(Qt::UserRole, classNames.value(i));
    }
}

void NodeSelectorPopupWidget::filterList(const QString &text)
{
    for (int i = 0; i < m_listWidget->count(); ++i) {
        QListWidgetItem *item = m_listWidget->item(i);
        item->setHidden(!item->text().contains(text, Qt::CaseInsensitive));
    }
}

void NodeSelectorPopupWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QListWidgetItem *current = m_listWidget->currentItem();
        if (current) onItemActivated(current);
    } else {
        QDialog::keyPressEvent(event);
    }
}

void NodeSelectorPopupWidget::onItemActivated(QListWidgetItem *item)
{
    const QString className = item->data(Qt::UserRole).toString();
    emit nodeChosen(className);

    if (m_scene) {
        // Map the popup's top-left screen position into scene coordinates
        QPoint popupScreenPos = this->mapToGlobal(QPoint(0, 0));
        QPoint viewportPos    = m_scene->getView()->mapFromGlobal(popupScreenPos);
        QPointF spawnPos      = m_scene->getView()->mapToScene(viewportPos);

        m_scene->getHistory()->push(
            new CreateNodeCommand(m_scene, className, spawnPos)
        );
    }

    close();
}