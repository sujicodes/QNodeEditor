#ifndef NODEREGISTRY_H
#define NODEREGISTRY_H

#include <QString>
#include <QJsonObject>
#include <functional>
#include <iostream> 
#include <unordered_map>
#include "Node.h"
#include "Scene.h"


class NodeRegistry
{
public:
    using NodeFactory = std::function<Node*(Scene*)>;

    static NodeRegistry& instance()
    {
        static NodeRegistry inst;
        return inst;
    }

    void registerType(const QString& typeName, NodeFactory factory)
    {
        if (factories.find(typeName) != factories.end()) {
            std::cerr << "Node type already registered: " << typeName.toStdString() << "\n";
            return;
        }
        factories[typeName] = factory;
    }

    Node* createNode(const QString& typeName, Scene* scene) const
    {
        qDebug() << typeName;
        auto it = factories.find(typeName);
        if (it == factories.end()){ 
            qWarning() << typeName << " is not registered";
            return nullptr;
        }
        return it->second(scene);
    }

private:
    NodeRegistry() = default;
    std::unordered_map<QString, NodeFactory> factories;
};

#define REGISTER_NODE(NodeClass, TypeName)                          \
    static bool _##NodeClass##_registered = []() {                  \
        NodeRegistry::instance().registerType(                      \
            TypeName,                                               \
            [](Scene* scene) -> Node* {                             \
                NodeClass* node = new NodeClass(scene);             \
                return node;                                        \
            });                                                     \
        return true;                                                \
    }();


#endif // NODEREGISTRY_H
