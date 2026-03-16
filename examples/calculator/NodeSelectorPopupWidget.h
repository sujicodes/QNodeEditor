#ifndef NODEPOPUPWIDGET_H
#define NODEPOPUPWIDGET_H

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QKeyEvent>

class NodeEditorGraphicsScene;  // forward declare

class NodeSelectorPopupWidget : public QDialog
{
    Q_OBJECT
public:
    explicit NodeSelectorPopupWidget(QWidget *parent = nullptr);

    void setNodeList(const QStringList &displayNames, const QStringList &classNames);
    void setScene(NodeEditorGraphicsScene *scene);          // ← new

signals:
    void nodeChosen(const QString &className);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void filterList(const QString &text);
    void onItemActivated(QListWidgetItem *item);

private:
    QLineEdit *m_searchBox;
    QListWidget *m_listWidget;
    NodeEditorGraphicsScene *m_scene = nullptr;
};

#endif // NODEEDITORWINDOW_H