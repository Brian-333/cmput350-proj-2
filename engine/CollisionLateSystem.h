#pragma once

// also known as collisionsystemupdate

#include "CollisionComponent.h"
#include "ECSEngine.h"
#include "LocationComponent.h"

namespace ECSEngine {

/**
 * Stores each entity's world-space collision box for use as the previous box next frame.
 */
template <typename... Components>
class CollisionLateSystem : public System<Components...> {
public:
    /**
     * Get the name of the system.
     * @return The name of the system.
     */
    const std::string& GetName() const override {
        static std::string name("CollisionLateSystem");
        return name;
    }

    /**
     * Copy each entity's collision box, offset by its location, into previousCollisionBox.
     * @param engine The engine view used to access the entity manager.
     * @return kContinue to keep the engine running.
     */
    tSystemReturn Run(EngineView<Components...>* engine) override {
        auto& em = engine->GetEntityManager();

        for (const Entity& entity : em) {
            auto id = entity.GetID();
            if (!em.IsValid(id)) {
                continue;
            }
            if (!em.template HasComponent<LocationComponent>(id) ||
                !em.template HasComponent<CollisionComponent>(id)) {
                continue;
            }

            auto& collision = em.template GetComponent<CollisionComponent>(id);
            auto& location = em.template GetComponent<LocationComponent>(id);

            collision.previousCollisionBox = collision.collisionBox;
            collision.previousCollisionBox.topLeft += location.loc;
        }

        return kContinue;
    }
};

}  // namespace ECSEngine
