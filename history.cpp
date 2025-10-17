#include "history.h"
#include "Edge.h"
#include "EdgeGraphicsPathItem.h"
#include "Node.h"
#include "NodeGraphicsItem.h"
#include "Scene.h"
#include <QDebug>
#include <qjsonarray.h>

History::History(Scene* scene)
    : scene(scene), historyCurrentStep(-1), historyLimit(100) {}

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

    QJsonObject hs = createHistoryStamp(desc);
    historyStack.append(hs);
    historyCurrentStep += 1;

    qDebug() << "  -- setting step to:" << historyCurrentStep;
}

QJsonObject History::createHistoryStamp(const QString& desc) {
    // Selection object
    QJsonObject selObj;
    QJsonArray nodeArray;
    QJsonArray edgeArray;

    // Iterate over selected items in the scene
    for (QGraphicsItem* item : scene->graphicsScene()->selectedItems()) {
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

void History::restoreHistoryStamp(const QJsonObject &historyStamp)
{
    // Debug printing
    qDebug() << "RHS:" << historyStamp;
    qDebug() << "RHS desc:" << historyStamp["desc"].toString();

    // Restore scene from snapshot
    QJsonObject snapshot = historyStamp["snapshot"].toObject();
    if (scene) {
        std::unordered_map<qint64, Serializable*> hashmap = {};
        scene->deserialize(snapshot, hashmap);
    }

    // Restore edge selection
    QJsonArray selEdges = historyStamp["selection"].toObject()["edges"].toArray();
    for (auto it = selEdges.begin(); it != selEdges.end(); ++it) {
        const QJsonValue &edgeIdVal = *it;
        qint64 edgeId = edgeIdVal.toVariant().toLongLong();
        for (Edge* edge : scene->getEdges()) {
            if (edge->getId() == edgeId) {
                if (edge->getEdgeGraphicsItem()) {
                    edge->getEdgeGraphicsItem()->setSelected(true);
                }
                break;
            }
        }
    }

    // Restore node selection
    QJsonArray selNodes = historyStamp["selection"].toObject()["nodes"].toArray();
    for (auto it = selNodes.begin(); it != selNodes.end(); ++it) {
        const QJsonValue &nodeIdVal = *it;
        qint64 nodeId = nodeIdVal.toVariant().toLongLong();
        for (Node* node : scene->getNodes()) {
            if (node->getId() == nodeId) {
                if (node->getNodeGraphicsItem()) {
                    node->getNodeGraphicsItem()->setSelected(true);
                }
                break;
            }
        }
    }
}
