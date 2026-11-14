#pragma once

#include "ECSEngine.h"
#include "LocationComponent.h"

namespace ECSEngine {

/**
 * Moves entities by their velocity.
 */
template <typename... Components>
class MovementSystem : public System<Components...> {
public:
    /**
     * Get the name of the system.
     * @return The name of the system.
     */
    const std::string& GetName() const override {
        static std::string name("MovementSystem");
        return name;
    }

    /**
     * Add each entity's velocity to its location.
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
            if (!em.template HasComponent<MovementComponent>(id) ||
                !em.template HasComponent<LocationComponent>(id)) {
                continue;
            }

            auto& movement = em.template GetComponent<MovementComponent>(id);
            auto& location = em.template GetComponent<LocationComponent>(id);

            location.loc += movement.velocity;
        }
        return kContinue;
    }
};

}  // namespace ECSEngine
