#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <QJsonObject>
#include <unordered_map>

/**
 * @brief Abstract base class for all objects that can be serialized to and from JSON.
 *
 * Serializable provides a common interface for save/load, clipboard, and undo/redo
 * workflows throughout the node editor. Every Serializable instance is assigned a
 * unique 64-bit ID on construction, derived from its memory address. This ID is
 * used as a stable key for cross-referencing objects (e.g. resolving socket-to-edge
 * relationships) during deserialization.
 *
 * Subclasses must implement serialize() and deserialize() to define how their
 * state is written to and read from a QJsonObject.
 *
 * @note IDs are pointer-based and are therefore only unique within a single
 *       process lifetime. When deserializing, restoreId should be set to true
 *       to reinstate the original ID from the JSON data so that cross-references
 *       between objects (stored by ID) can be resolved correctly.
 *
 * @see NodeItem
 * @see Edge
 * @see SocketItem
 */
class Serializable
{
public:

    /**
     * @brief Constructs a Serializable and assigns it a unique ID.
     *
     * The ID is derived from the object's memory address via reinterpret_cast,
     * guaranteeing uniqueness within the current process for the object's lifetime.
     */
    Serializable()
    {
        m_id = reinterpret_cast<qint64>(this);
    }

    /**
     * @brief Virtual destructor. Ensures correct cleanup of subclass instances.
     */
    virtual ~Serializable() = default;

    /**
     * @brief Serializes this object's state to a JSON object.
     *
     * Subclasses must implement this to write all state necessary to fully
     * reconstruct the object, including m_id so that cross-references can
     * be resolved on deserialization.
     *
     * @return A QJsonObject representing the full serialized state.
     */
    virtual QJsonObject serialize() const = 0;

    /**
     * @brief Deserializes this object's state from a JSON object.
     *
     * Subclasses must implement this to restore all state from @p data.
     * The @p hashmap is used to resolve references to other Serializable
     * objects by their stored ID (e.g. a socket referencing its connected edges).
     *
     * @param data       The JSON object containing the serialized state.
     * @param hashmap    A map of original IDs to live Serializable pointers,
     *                   populated as objects are deserialized, used to resolve
     *                   cross-references between objects.
     * @param restoreId  If true, restores m_id from the serialized data so that
     *                   other objects can locate this instance by its original ID.
     *                   Set to false when creating fresh copies (e.g. paste).
     */
    virtual void deserialize(
        const QJsonObject& data,
        std::unordered_map<qint64, Serializable*>& hashmap,
        bool restoreId = true
    ) = 0;

    /**
     * @brief Returns the unique ID of this object.
     *
     * Used as a key in serialized JSON and in the deserialization hashmap
     * to establish cross-references between objects.
     *
     * @return The unique qint64 identifier for this instance.
     */
    qint64 getId() const { return m_id; }

protected:

    /// @brief The unique identifier for this instance, set on construction from its memory address.
    qint64 m_id;
};

#endif // SERIALIZABLE_H