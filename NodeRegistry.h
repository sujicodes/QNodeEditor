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

    void registerType(const QString& className, NodeFactory factory)
    {
        if (factories.find(className) != factories.end()) {
            std::cerr << "Node type already registered: " << className.toStdString() << "\n";
            return;
        }
        factories[className] = factory;
    }

    Node* createNode(const QString& className, Scene* scene) const
    {
        auto it = factories.find(className);
        if (it == factories.end()) {
            qWarning() << "Node not registered:" << className;
            return nullptr;
        }
        return it->second(scene);
    }

    QStringList allClassNamesSorted() const
    {
        QStringList keys;
        for (const auto& pair : factories)
            keys << pair.first;
        keys.sort();
        return keys;
    }

private:
    NodeRegistry() = default;
    std::unordered_map<QString, NodeFactory> factories;
};

#define REGISTER_NODE(NodeClass, TypeName)                          \
     inline bool _##NodeClass##_registered = []() {                  \
        NodeRegistry::instance().registerType(                      \
            TypeName,                                               \
            [](Scene* scene) -> Node* {                             \
                NodeClass* node = new NodeClass(scene);             \
                return node;                                        \
            });                                                     \
        return true;                                                \
    }();


#endif // NODEREGISTRY_H
