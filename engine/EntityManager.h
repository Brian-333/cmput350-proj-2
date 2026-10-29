#pragma once

#include <cassert>
#include <iostream>
#include <limits>
#include <type_traits>

#include "Component.h"
#include "Entity.h"
#include "Pack.h"

namespace ECSEngine {

template <typename... Components>
class EntityManager {
public:
    static constexpr size_t kNullComponent = std::numeric_limits<size_t>::max();
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

    void RemoveEntity(EntityID entity) {
        assert(ValidEntity(entity));
        mIsFree[entity] = true;
        mFreeEntityIndices.push_back(entity);
        for (size_t i = 0; i < numComponents; ++i) {
            mComponentIndices[entity][i] = kNullComponent;
        }
    }

    template <typename T>
    void AddComponent(EntityID entity, T component) {
        assert(!HasComponent<T>(entity));

        static constexpr auto COMPONENT_TYPE_INDEX = Pack<Components...>::template index<T>;
        mComponentIndices[entity][COMPONENT_TYPE_INDEX] =
            std::get<COMPONENT_TYPE_INDEX>(mComponentStorages).store(std::move(component));
    }

    template <typename T>
    void RemoveComponent(EntityID entity) {
        assert(HasComponent<T>(entity));

        static constexpr auto COMPONENT_TYPE_INDEX = Pack<Components...>::template index<T>;
        std::get<COMPONENT_TYPE_INDEX>(mComponentStorages).remove(mComponentIndices[entity][COMPONENT_TYPE_INDEX]);
        mComponentIndices[entity][COMPONENT_TYPE_INDEX] = kNullComponent;
    }

    template <typename T>
    bool HasComponent(EntityID entity) const {
        assert(ValidEntity(entity));
        static_assert(Pack<Components...>::template contains<T>);
        static constexpr auto COMPONENT_TYPE_INDEX = Pack<Components...>::template index<T>;
        return mComponentIndices[entity][COMPONENT_TYPE_INDEX] != kNullComponent;
    }

    template <typename T>
    T& GetComponent(EntityID entity) {
        assert(HasComponent<T>(entity));
        static constexpr auto COMPONENT_TYPE_INDEX = Pack<Components...>::template index<T>;
        return std::get<COMPONENT_TYPE_INDEX>(mComponentStorages)[mComponentIndices[entity][COMPONENT_TYPE_INDEX]];
    }

    template <typename T>
    ComponentStorage<T>& GetComponentStorage() {
        // Re-write code
        return std::get<ComponentStorage<T>>(mComponentStorages);
    }

private:
    [[nodiscard]] bool ValidEntity(EntityID id) const {
        return id < mIsFree.size() && !mIsFree[id];
    }
    
    static constexpr size_t numComponents = sizeof...(Components);

    std::vector<Entity> mEntities;
    std::vector<bool> mIsFree;
    std::vector<EntityID> mFreeEntityIndices;

    std::vector<std::array<size_t, numComponents>> mComponentIndices;
    std::tuple<ComponentStorage<Components>...> mComponentStorages;

};

}  // namespace ECSEngine
