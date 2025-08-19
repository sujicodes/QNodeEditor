#ifndef HISTORY_H
#define HISTORY_H

#include <QString>
#include <QVector>

class Scene;
class History
{
public:
    History(Scene* scene);
    void undo();
    void redo();
    void restoreHistory();
    void storeHistory(const QString& desc);
    QVector<QString> getStack() { return historyStack; }
    int getCurrentStep() { return historyCurrentStep; }

private:
    QString createHistoryStamp(const QString& desc);
    void restoreHistoryStamp(const QString& historyStamp);

    Scene* scene;
    QVector<QString> historyStack;
    int historyCurrentStep;
    int historyLimit;
};

#endif // HISTORY_H
