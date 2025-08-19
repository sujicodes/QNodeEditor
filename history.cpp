#include "history.h"
#include "Scene.h"
#include <QDebug>

History::History(Scene* scene)
    : scene(scene), historyCurrentStep(-1), historyLimit(32) {}

void History::undo() {
    qDebug() << "UNDO";

    if (historyCurrentStep > 0) {
        historyCurrentStep -= 1;
        restoreHistory();
    }
}

void History::redo() {
    qDebug() << "REDO";

    if (historyCurrentStep + 1 < historyStack.size()) {
        historyCurrentStep += 1;
        restoreHistory();
    }
}

void History::restoreHistory() {
    qDebug() << "Restoring history"
             << ".... current_step: @" << historyCurrentStep
             << "(" << historyStack.size() << ")";

    restoreHistoryStamp(historyStack[historyCurrentStep]);
}

void History::storeHistory(const QString& desc) {
    qDebug() << "Storing history" << "\"" + desc + "\""
             << ".... current_step: @" << historyCurrentStep
             << "(" << historyStack.size() << ")";

    // truncate history if not at the end
    if (historyCurrentStep + 1 < historyStack.size()) {
        historyStack.resize(historyCurrentStep + 1);
    }

    // enforce history limit
    if (historyCurrentStep + 1 >= historyLimit) {
        historyStack.pop_front();
        historyCurrentStep -= 1;
    }

    QString hs = createHistoryStamp(desc);
    historyStack.append(hs);
    historyCurrentStep += 1;

    qDebug() << "  -- setting step to:" << historyCurrentStep;
}

QString History::createHistoryStamp(const QString& desc) {
    // Selection object
    QJsonObject selObj;
    QJsonArray nodeArray;
    QJsonArray edgeArray;

    // Iterate over selected items in the scene
    for (QGraphicsItem* item : scene->getGrScene()->selectedItems()) {
        // If item has a Node
        if (auto nodeItem = dynamic_cast<NodeGraphicsItem*>(item)) {
            if (nodeItem->getNode()) {
                nodeArray.append(QString::number(nodeItem->getNode()->getId()));
            }
        }
        // If item is an Edge
        else if (auto edgeItem = dynamic_cast<EdgeGraphicsPathItem*>(item)) {
            if (edgeItem->getEdge()) {
                edgeArray.append(QString::number(edgeItem->getEdge()->getId()));
            }
        }
    }

    selObj["nodes"] = nodeArray;
    selObj["edges"] = edgeArray;

    // Create history stamp
    QJsonObject historyStamp;
    historyStamp["desc"] = desc;
    historyStamp["snapshot"] = scene->serialize();   // assuming Scene::serialize() returns QJsonObject
    historyStamp["selection"] = selObj;

    return historyStamp;
}

void History::restoreHistoryStamp(const QString& historyStamp) {
    qDebug() << "RHS:" << historyStamp;
}