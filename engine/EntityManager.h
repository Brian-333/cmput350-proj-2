#pragma once

#include <cassert>
#include <iostream>
#include <limits>
#include <type_traits>

#include "Component.h"
#include "Entity.h"
#include "Pack.h"

namespace ECSEngine {

/**
 * Creates and removes entities and stores the components attached to them.
 */
template <typename... Components>
class EntityManager {
public:
    static constexpr size_t kNullComponent = std::numeric_limits<size_t>::max();

    /**
     * Create a new entity with no components, reusing a freed ID if one exists.
     * @param name The type name of the entity.
     * @return The ID of the new entity.
     */
    [[nodiscard]] EntityID CreateEntity(const std::string& name) {
        if (mFreeEntityIndices.empty()) {
            EntityID id = mEntities.size();
            mIsFree.push_back(false);
            mEntities.push_back(Entity(id, name));
            mComponentIndices.push_back(std::array<size_t, numComponents>());
            for (size_t i = 0; i < numComponents; ++i) {
                mComponentIndices.back()[i] = kNullComponent;
            }
            return id;
        } else {
            EntityID id = mFreeEntityIndices.back();
            mIsFree[id] = false;
            mFreeEntityIndices.pop_back();
            mEntities[id].mType = name;
            for (size_t i = 0; i < numComponents; ++i) {
                mComponentIndices[id][i] = kNullComponent;
            }
            return id;
        }
    }

    /**
     * Remove an entity and all of its components. Its ID may be reused later.
     * @param entity The ID of the entity.
     */
    void RemoveEntity(EntityID entity) {
        assert(ValidEntity(entity));
        // Remove all components from the entity
        (TryRemoveComponent<Components>(entity), ...);
        for (size_t i = 0; i < numComponents; ++i) {
            mComponentIndices[entity][i] = kNullComponent;
        }

        mIsFree[entity] = true;
        mFreeEntityIndices.push_back(entity);
    }

    /**
     * Add a component to an entity. The entity must not already have a component of this type.
     * @param entity The ID of the entity.
     * @param component The component to add.
     */
    template <typename T>
    void AddComponent(EntityID entity, T component) {
        assert(!HasComponent<T>(entity));

        static constexpr auto COMPONENT_TYPE_INDEX = Pack<Components...>::template index<T>;
        mComponentIndices[entity][COMPONENT_TYPE_INDEX] =
            std::get<COMPONENT_TYPE_INDEX>(mComponentStorages).store(std::move(component));
    }

    /**
     * Remove a component from an entity. The entity must have a component of this type.
     * @param entity The ID of the entity.
     */
    template <typename T>
    void RemoveComponent(EntityID entity) {
        assert(HasComponent<T>(entity));

        static constexpr auto COMPONENT_TYPE_INDEX = Pack<Components...>::template index<T>;
        std::get<COMPONENT_TYPE_INDEX>(mComponentStorages)
            .remove(mComponentIndices[entity][COMPONENT_TYPE_INDEX]);
        mComponentIndices[entity][COMPONENT_TYPE_INDEX] = kNullComponent;
    }

    /**
     * Check whether an entity has a component of a type.
     * @param entity The ID of the entity.
     * @return True if the entity has the component.
     */
    template <typename T>
    bool HasComponent(EntityID entity) const {
        assert(ValidEntity(entity));
        static_assert(Pack<Components...>::template contains<T>);
        static constexpr auto COMPONENT_TYPE_INDEX = Pack<Components...>::template index<T>;
        return mComponentIndices[entity][COMPONENT_TYPE_INDEX] != kNullComponent;
    }

    /**
     * Get a component of an entity. The entity must have a component of this type.
     * @param entity The ID of the entity.
     * @return The component.
     */
    template <typename T>
    T& GetComponent(EntityID entity) {
        assert(HasComponent<T>(entity));
        static constexpr auto COMPONENT_TYPE_INDEX = Pack<Components...>::template index<T>;
        return std::get<COMPONENT_TYPE_INDEX>(
            mComponentStorages)[mComponentIndices[entity][COMPONENT_TYPE_INDEX]];
    }

    /**
     * Get the storage holding every component of a type.
     * @return The component storage.
     */
    template <typename T>
    ComponentStorage<T>& GetComponentStorage() {
        // Re-write code
        return std::get<ComponentStorage<T>>(mComponentStorages);
    }

    /**
     * Check whether an entity ID refers to a live entity.
     * @param id The ID of the entity.
     * @return True if the entity exists and has not been removed.
     */
    bool IsValid(EntityID id) const { return ValidEntity(id); }

    // TODO: Implement iterators for the entity manager
    using tEntity = std::vector<Entity>;
    using const_iterator = tEntity::const_iterator;
    const_iterator cbegin() const { return mEntities.begin(); }
    const_iterator cend() const { return mEntities.end(); }
    const_iterator begin() { return mEntities.begin(); }
    const_iterator end() { return mEntities.end(); }

private:
    /**
     * Check whether an entity ID refers to a live entity.
     * @param id The ID of the entity.
     * @return True if the ID is in range and not free.
     */
    [[nodiscard]] bool ValidEntity(EntityID id) const {
        return id < mIsFree.size() && !mIsFree[id];
    }

    /**
     * Remove a component from an entity if it has one.
     * @param entity The ID of the entity.
     */
    template <typename T>
    void TryRemoveComponent(EntityID entity) {
        if (HasComponent<T>(entity)) {
            RemoveComponent<T>(entity);
        }
    }

    static constexpr size_t numComponents = sizeof...(Components);

    std::vector<Entity> mEntities;
    std::vector<bool> mIsFree;
    std::vector<EntityID> mFreeEntityIndices;

    std::vector<std::array<size_t, numComponents>> mComponentIndices;
    std::tuple<ComponentStorage<Components>...> mComponentStorages;
};

}  // namespace ECSEngine
