#ifndef NODEREGISTRY_H
#define NODEREGISTRY_H

#include <QString>
#include <QJsonObject>
#include <functional>
#include <iostream> 
#include <unordered_map>
#include "NodeItem.h"
#include "Scene.h"

class NodeRegistry
{
    public:

        using NodeFactory = std::function<NodeItem*(Scene*)>;

        static NodeRegistry& instance()
        {
            static NodeRegistry inst;
            return inst;
        }

        void registerType(const QString& className, NodeFactory factory)
        {
            qDebug() << "NodeRegistry registerType for " << className;
            qDebug() << "Factories before registration : " << allClassNamesSorted();
            if(factories.find(className) != factories.end())
            {
                std::cerr << "Node type already registered: " << className.toStdString() << "\n";
                return;
            }

            factories[className] = factory;
            qDebug() << "Factories after registration : " << allClassNamesSorted();
        }

        NodeItem* createNode(const QString& className, Scene* scene) const
        {
            qDebug() << "NodeRegistry createNode, factories classes : " << allClassNamesSorted();
            auto it = factories.find(className);
            if(it == factories.end())
            {
                qWarning() << "Node not registered:" << className;
                return nullptr;
            }

            auto node = it->second(scene);
            node->initNode();
            return node;
        }

        QStringList allClassNamesSorted() const
        {
            QStringList keys;
            for(const auto& pair : factories)
            {
                keys << pair.first;
            }

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
            [](Scene* scene) -> NodeItem* {                         \
                NodeClass* node = new NodeClass(scene);             \
                return node;                                        \
            });                                                     \
        return true;                                                \
    }();

#endif // NODEREGISTRY_H
