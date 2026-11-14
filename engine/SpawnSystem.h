#pragma once

#include <random>
#include <cmath>
#include "ECSEngine.h"
#include "LocationComponent.h"
#include "SpawnComponent.h"
#include "CollisionComponent.h"
#include "SpriteComponent.h"

namespace ECSEngine {

const float kStarSpeed = 6.f;

template <typename... Components>
class SpawnSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("SpawnSystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override { 

        auto& em = engine->GetEntityManager();

        // only the spawners have the spawn component
        auto& spawners = em.template GetComponentStorage<SpawnComponent>();

        for (std::size_t i = 0; i < spawners.mStorage.size(); ++i) {
            if (spawners.valid(i) == false) {
                continue;
            }

            auto& spawner = spawners[i];
            auto spawnerId = spawner.entityID;

            if (spawner.totalSpawns <= 0) { // done spawning
                continue;
            }
            
            spawner.timeToNextSpawn--;

            if (spawner.timeToNextSpawn > 0) {
                continue;
            }

            // spawner will spawn
            spawner.timeToNextSpawn = spawner.timeBetweenSpawns;
            spawner.totalSpawns--;
            if (spawner.spawnType == "star" && 
                em.template HasComponent<LocationComponent>(spawnerId)) {
                auto& location = em.template GetComponent<LocationComponent>(spawnerId);
                
                Point2D starSpawn(location.loc);
                auto starId = em.CreateEntity("star");

                em.AddComponent(starId, LocationComponent{starSpawn});

                const float kTwoPi = 6.2831853f;
                std::uniform_real_distribution<float> angleDist(0.f, kTwoPi);
                float angle = angleDist(mRng);

                Point2D velocity(std::cos(angle) * kStarSpeed, std::sin(angle) * kStarSpeed);
                em.AddComponent(starId, MovementComponent{velocity});

                CollisionComponent starC{};
                starC.staticEntity = 0;
                starC.collisionBox = {{12, 13}, 40, 38};
                starC.previousCollisionBox = starC.collisionBox;
                starC.previousCollisionBox.topLeft += starSpawn;
                starC.collectable = 1;
                em.AddComponent(starId, starC);

                em.AddComponent(starId, GravityComponent(Point2D(0.f, 0.5f)));
                em.AddComponent(starId, SpriteComponent{
                    spawner.spawnTextureID, {{0, 0}, 64, 64}, true
                });
            }
        }

        return kContinue; 
    }

private:
    std::mt19937 mRng{std::random_device{}()};   // seeded once, from <random>
};


}  // namespace ECSEngine
