#include <QApplication>
#include <QCursor>
#include <QDebug>
#include "NodeSelectorWidget.h"

NodePopupWidget::NodePopupWidget(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::Popup);
    setFocusPolicy(Qt::StrongFocus);

    searchBox = new QLineEdit(this);
    searchBox->setPlaceholderText("Search nodes...");
    listWidget = new QListWidget(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->addWidget(searchBox);
    layout->addWidget(listWidget);
    setLayout(layout);

    connect(searchBox, &QLineEdit::textChanged, this, &NodePopupWidget::filterList);
    connect(listWidget, &QListWidget::itemActivated, this, &NodePopupWidget::onItemActivated);
}

void NodePopupWidget::setNodeList(const QStringList &nodes)
{
    listWidget->clear();
    listWidget->addItems(nodes);
}

void NodePopupWidget::filterList(const QString &text)
{
    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem *item = listWidget->item(i);
        item->setHidden(!item->text().contains(text, Qt::CaseInsensitive));
    }
}

void NodePopupWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QListWidgetItem *current = listWidget->currentItem();
        if (current) onItemActivated(current);
    } else {
        QDialog::keyPressEvent(event);
    }
}

void NodePopupWidget::onItemActivated(QListWidgetItem *item)
{
    selected = item->text();
    emit nodeChosen(selected);
    close();
}
