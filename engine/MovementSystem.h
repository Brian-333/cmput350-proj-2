#pragma once

#include "ECSEngine.h"
#include "LocationComponent.h"

namespace ECSEngine {

template <typename... Components>
class MovementSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("MovementSystem");
        return name;
    }

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
