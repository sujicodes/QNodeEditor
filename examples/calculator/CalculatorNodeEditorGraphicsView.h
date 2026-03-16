#include "NodeEditorGraphicsView.h"
#include "NodeSelectorPopupWidget.h"
 
class CalculatorNodeEditorGraphicsView : public NodeEditorGraphicsView
{
    Q_OBJECT
 
public:
    explicit CalculatorNodeEditorGraphicsView(NodeEditorGraphicsScene* grScene, QWidget* parent = nullptr)
        : NodeEditorGraphicsView(grScene, parent)
    {
        QStringList keys       = {"Input", "Output", "Add", "Subtract", "Multiply", "Divide"};
        QStringList classNames = {"InputNode", "OutputNode", "AddNode",
                                  "SubtractNode", "MultiplyNode", "DivideNode"};
 
        m_nodePopup = new NodeSelectorPopupWidget(this);
        m_nodePopup->setNodeList(keys, classNames);
        m_nodePopup->setScene(grScene);
    }
 
protected:
    void keyPressEvent(QKeyEvent* event) override
    {
        if (event->key() == Qt::Key_QuoteLeft) {
            QPoint pos = QCursor::pos();
            m_nodePopup->move(pos);
            m_nodePopup->show();
            m_nodePopup->raise();
            m_nodePopup->activateWindow();
            return;
        }
 
        NodeEditorGraphicsView::keyPressEvent(event);
    }
private:
    NodeSelectorPopupWidget* m_nodePopup;
};