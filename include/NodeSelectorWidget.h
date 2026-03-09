#ifndef NODEPOPUPWIDGET_H
#define NODEPOPUPWIDGET_H

#include <QMainWindow>
#include <QDialog>
#include <QListWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QKeyEvent>

class NodePopupWidget : public QDialog
{
    Q_OBJECT

    public:

        explicit NodePopupWidget(QWidget *parent = nullptr);

        void setNodeList(const QStringList &nodes);
        QString selectedNode() const { return selected; }

    signals:

        void nodeChosen(const QString &name);

    protected:

        void keyPressEvent(QKeyEvent *event) override;

    private slots:

        void filterList(const QString &text);
        void onItemActivated(QListWidgetItem *item);

    private:

        QLineEdit *searchBox;
        QListWidget *listWidget;
        QString selected;
};

#endif // NODEEDITORWINDOW_H