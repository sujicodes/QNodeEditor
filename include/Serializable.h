#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <QJsonObject>
#include <unordered_map>

class Serializable
{

    public:

        Serializable()
        {
            // Generate a unique ID using the object's pointer address
            id = reinterpret_cast<qint64>(this);
        }

        virtual ~Serializable() = default;

        // Pure virtual methods (must be implemented by subclasses)
        virtual QJsonObject serialize() const = 0;

        virtual void deserialize(
            const QJsonObject & data,
            std::unordered_map<qint64, Serializable*>& hashmap,
            bool restoreId = true
            ) = 0;

        qint64 getId() const { return id; }

    protected:

        qint64 id;  // unique ID (like Python's id(self))
};

#endif // SERIALIZABLE_H