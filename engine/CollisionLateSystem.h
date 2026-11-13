#pragma once

// also known as collisionsystemupdate

#include "ECSEngine.h"
#include "LocationComponent.h"
#include "CollisionComponent.h"

namespace ECSEngine {

template <typename... Components>
class CollisionLateSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("CollisionLateSystem");
        return name;
    }

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
