#ifndef NODEEDITORWIDGET_H
#define NODEEDITORWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QString>
#include <QGraphicsItem>

class NodeGraphicsScene;
class NodeEditorGraphicsView;
class NodeEditorGraphicsScene;
class NodeItem;

class NodeEditorWidget : public QWidget
{
    Q_OBJECT

    public:

        NodeEditorWidget(QWidget *parent = nullptr);
        NodeEditorGraphicsScene* getScene() { return scene; }
        NodeEditorGraphicsView* getGraphicsView() { return view; }
        bool isModified() const;
        bool isFilenameSet() const;
        QString getUserFriendlyFilename() const;
        QString getFilename() const;
        void fileNew();
        bool fileLoad(const QString& name);
        bool fileSave(const QString& name = QString());
        QList<QGraphicsItem*> getSelectedItems() const;
        bool hasSelectedItems() const;
        bool canUndo() const;
        bool canRedo() const;

    private:

        QVBoxLayout *layout;
        NodeEditorGraphicsView* view;
        NodeEditorGraphicsScene* scene;
        QString filename;
};

#endif // NODEEDITORWINDOW_H
