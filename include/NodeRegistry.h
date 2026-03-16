#ifndef NODEREGISTRY_H
#define NODEREGISTRY_H

#include <QString>
#include <QJsonObject>
#include <functional>
#include <iostream>
#include <unordered_map>
#include "NodeItem.h"
#include "NodeEditorGraphicsScene.h"

/**
 * @brief Singleton registry for creating NodeItem subclasses by type name.
 *
 * NodeRegistry maps string type names to factory functions, allowing nodes
 * to be created dynamically at runtime without the caller needing to know
 * the concrete class. This enables scene serialization/deserialization and
 * popup-driven node creation to work purely by type name string.
 *
 * Node types are registered at static initialisation time using the
 * REGISTER_NODE macro, which should be placed in the node's header or
 * source file. The registry is a Meyer's singleton and is thread-safe
 * for reads after all static initialisers have run.
 *
 * @par Example usage:
 * @code
 * // Register a node type (in AddNode.h):
 * REGISTER_NODE(AddNode, "AddNode")
 *
 * // Create a node by type name:
 * NodeItem* node = NodeRegistry::instance().createNode("AddNode", scene);
 * @endcode
 *
 * @see NodeItem
 * @see REGISTER_NODE
 */
class NodeRegistry
{
public:

    /**
     * @brief The factory function signature used to create NodeItem instances.
     *
     * Takes the owning scene and returns a heap-allocated NodeItem subclass.
     * The registry takes no ownership — the scene or caller is responsible
     * for the node's lifetime.
     */
    using NodeFactory = std::function<NodeItem*(NodeEditorGraphicsScene*)>;

    /**
     * @brief Returns the single global instance of the NodeRegistry.
     *
     * Uses a local static (Meyer's singleton) for thread-safe initialisation.
     *
     * @return Reference to the global NodeRegistry instance.
     */
    static NodeRegistry& instance()
    {
        static NodeRegistry inst;
        return inst;
    }

    /**
     * @brief Registers a factory function for a given node class name.
     *
     * If the class name is already registered, a warning is printed and
     * the existing factory is preserved. This prevents accidental double
     * registration from multiple translation units.
     *
     * @param className The unique string identifier for the node type
     *                  (e.g. "AddNode"). Must match the string used in
     *                  scene serialization and the REGISTER_NODE macro.
     * @param factory   A callable that constructs and returns a new instance
     *                  of the node subclass given a scene pointer.
     */
    void registerType(const QString& className, NodeFactory factory)
    {
        qDebug() << "NodeRegistry registerType for " << className;
        qDebug() << "m_factories before registration : " << allClassNamesSorted();

        if (m_factories.find(className) != m_factories.end())
        {
            std::cerr << "Node type already registered: " << className.toStdString() << "\n";
            return;
        }

        m_factories[className] = factory;
        qDebug() << "m_factories after registration : " << allClassNamesSorted();
    }

    /**
     * @brief Creates and fully initialises a node of the given type.
     *
     * Looks up the factory for @p className, invokes it to construct the node,
     * then calls NodeItem::initNode() to build the node's visual child items.
     * Returns nullptr and logs a warning if the type is not registered.
     *
     * @param className The registered type name of the node to create.
     * @param scene     The scene the new node will belong to. Must not be null.
     * @return A fully initialised NodeItem subclass instance, or nullptr if
     *         the type name is not found in the registry.
     */
    NodeItem* createNode(const QString& className, NodeEditorGraphicsScene* scene) const
    {
        qDebug() << "NodeRegistry createNode, m_factories classes : " << allClassNamesSorted();

        auto it = m_factories.find(className);
        if (it == m_factories.end())
        {
            qWarning() << "Node not registered:" << className;
            return nullptr;
        }

        auto node = it->second(scene);
        node->initNode();
        return node;
    }

    /**
     * @brief Returns a sorted list of all registered node class names.
     *
     * Useful for populating node selector UI widgets and for debug logging.
     *
     * @return A QStringList of all registered type names in alphabetical order.
     */
    QStringList allClassNamesSorted() const
    {
        QStringList keys;
        for (const auto& pair : m_factories)
        {
            keys << pair.first;
        }
        keys.sort();
        return keys;
    }

private:

    /// @brief Private constructor — use instance() to access the singleton.
    NodeRegistry() = default;

    /// @brief Map from type name string to the corresponding node factory function.
    std::unordered_map<QString, NodeFactory> m_factories;
};

/**
 * @def REGISTER_NODE(NodeClass, TypeName)
 * @brief Registers a NodeItem subclass with the NodeRegistry at static initialisation time.
 *
 * Place this macro in the header or source file of the node subclass to automatically
 * register it when the translation unit is loaded. The macro creates a file-scoped
 * inline boolean variable whose initialiser calls NodeRegistry::registerType(), ensuring
 * registration happens before main() runs.
 *
 * @param NodeClass The C++ class name of the node subclass (e.g. AddNode).
 * @param TypeName  A string literal used as the registry key (e.g. "AddNode").
 *                  This string must match what is stored in serialized scene files.
 *
 * @par Example:
 * @code
 * // In AddNode.h, after the class definition:
 * REGISTER_NODE(AddNode, "AddNode")
 * @endcode
 *
 * @warning Each (NodeClass, TypeName) pair must be unique across the application.
 *          Duplicate registrations are detected and silently ignored, preserving
 *          the first registered factory.
 *
 * @see NodeRegistry::registerType()
 */
#define REGISTER_NODE(NodeClass, TypeName)                              \
    inline bool _##NodeClass##_registered = []() {                      \
        NodeRegistry::instance().registerType(                          \
            TypeName,                                                   \
            [](NodeEditorGraphicsScene* scene) -> NodeItem* {           \
                NodeClass* node = new NodeClass(scene);                 \
                return node;                                            \
            });                                                         \
        return true;                                                    \
    }();

#endif // NODEREGISTRY_H