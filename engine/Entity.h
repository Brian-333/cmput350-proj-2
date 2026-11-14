#pragma once

#include <string>
#include <array>
#include <tuple>

// An entity can have any component it is templated with,
// but does not have these installed by default.
// This is a base class for all entities in the ECS system.

namespace ECSEngine {

using EntityID = size_t;

class Entity {
public:
    /**
     * Get the ID of the entity.
     * @return The entity ID.
     */
    EntityID GetID() const { return mID; }

private:
    Entity() = default;

    /**
     * Create an entity. Only EntityManager can construct entities.
     * @param eid The ID of the entity.
     * @param name The type name of the entity.
     */
    Entity(EntityID eid, const std::string& name) : mID(eid), mType(name) {}
    EntityID mID;
    std::string mType;

    template <typename... T>
    friend class EntityManager;
};

}  // namespace ECSEngine
