#ifndef HISTORY_H
#define HISTORY_H

#include "NodeEditorGraphicsScene.h"
#include <QString>
#include <QVector>
#include <qjsonobject.h>

class Scene;

class History
{
    public:

        History(NodeEditorGraphicsScene* scene);
        void undo();
        void redo();
        void restoreHistory();
        void storeHistory(const QString& desc);
        QVector<QJsonObject> getStack() { return historyStack; }
        int getCurrentStep() { return historyCurrentStep; }

    private:

        QJsonObject createHistoryStamp(const QString& desc);
        void restoreHistoryStamp(const QJsonObject& historyStamp);

        NodeEditorGraphicsScene* scene;
        QVector<QJsonObject> historyStack;
        int historyCurrentStep;
        int historyLimit;
};

#endif // HISTORY_H
