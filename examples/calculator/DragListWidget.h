#ifndef DRAGLISTWIDGET_H
#define DRAGLISTWIDGET_H

#include <QListWidget>
#include <QSize>

class DragListWidget : public QListWidget
{
    Q_OBJECT

    public:
        explicit DragListWidget(QWidget* parent = nullptr);

        void addCustomItems(const QStringList& keys,
                            const QStringList& paths,
                            const QStringList& classNames);

    protected:
        void startDrag(Qt::DropActions supportedActions) override;

    private:
        void initUI();
        void addIconItem(const QString& name,
                         const QString& iconPath = QString(),
                         const QString& className = QString());
};

#endif // DRAGLISTWIDGET_H