#pragma once

#include <algorithm>
#include <cmath>

#include "CameraComponent.h"
#include "CollisionComponent.h"
#include "ECSEngine.h"
#include "LocationComponent.h"
#include "MathUtil.h"
#include "SpriteComponent.h"
#include "SystemManager.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace ECSEngine {

// Written with help of Cursor.
// Follows the lab 9 dynamic camera on x only: dead zone, soft ease, hard clamp.
// Prompt: implement CameraSystem from the lab 9 follower and the Project 2 camera rules.
/**
 * Moves each camera to follow its target entity and applies any camera shake.
 */
template <typename... Components>
class CameraSystem : public System<Components...> {
public:
    /**
     * Get the name of the system.
     * @return The name of the system.
     */
    const std::string& GetName() const override {
        static std::string name = "CameraSystem";
        return name;
    }

    /**
     * Update every camera's position from its followed entity and set the window view.
     * @param engine The engine view used to access the entity and window managers.
     * @return kContinue to keep the engine running.
     */
    tSystemReturn Run(EngineView<Components...>* engine) override {
        auto& entityManager = engine->GetEntityManager();
        auto& windowManager = engine->GetWindowManager();

        // Loop through all entities with CameraFollower component
        for (const Entity& entity : entityManager) {
            auto id = entity.GetID();
            if (!entityManager.IsValid(id)) continue;
            if (!entityManager.template HasComponent<CameraComponent>(id)) continue;
            if (!entityManager.template HasComponent<CameraFollower>(id)) continue;

            auto& camera = entityManager.template GetComponent<CameraComponent>(id);
            auto followerId = entityManager.template GetComponent<CameraFollower>(id).entityID;

            // SetWorldRect is the only call that sets both the view and the scale.
            // SetWorldCenter / SetWorldScale keep the stored rect, which starts uninitialized.
            auto place = [&](const Point2D& center) {
                Rect window = windowManager.GetWindowBounds();
                float viewW = window.width * camera.worldUnitsPerPixel;
                float viewH = window.height * camera.worldUnitsPerPixel;
                windowManager.SetWorldRect(
                    {{center.x - viewW * 0.5f, center.y - viewH * 0.5f}, viewW, viewH});
            };

            // Measure against the unshaken center. Follow never changes y.
            place(camera.position);

            if (!entityManager.IsValid(followerId) ||
                !entityManager.template HasComponent<LocationComponent>(followerId)) {
                continue;
            }

            const auto& location =
                entityManager.template GetComponent<LocationComponent>(followerId);
            Point2D playerWorld = location.loc;
            if (entityManager.template HasComponent<CollisionComponent>(followerId)) {
                const auto& box =
                    entityManager.template GetComponent<CollisionComponent>(followerId)
                        .collisionBox;
                playerWorld += box.topLeft + Point2D(box.width * 0.5f, box.height * 0.5f);
            }

            float playerX = windowManager.WorldToWindow(playerWorld).x;
            float deadMin = camera.softExclusion.topLeft.x;
            float deadMax = deadMin + camera.softExclusion.width;
            float hardMin = camera.hardExclusion.topLeft.x;
            float hardMax = hardMin + camera.hardExclusion.width;

            // Signed screen pixels from the player to the nearest point inside the zone.
            float correction = playerX - std::clamp(playerX, deadMin, deadMax);
            if (playerX < hardMin || playerX > hardMax) {
                float hardCorrection = playerX - std::clamp(playerX, hardMin, hardMax);
                correction = (std::abs(correction * camera.speed) < std::abs(hardCorrection))
                                 ? hardCorrection
                                 : correction * camera.speed;
            } else if (std::abs(correction) >= 0.5f) {
                correction *= camera.speed;
            }
            camera.position.x += windowManager.WindowToWorld(correction);

            Point2D drawCenter = camera.position;
            if (entityManager.template HasComponent<CameraShake>(followerId)) {
                auto& shake = entityManager.template GetComponent<CameraShake>(followerId);
                float sign = (shake.frameLimit % 2 == 0) ? 1.f : -1.f;
                drawCenter += sign * shake.offset;
                if (--shake.frameLimit <= 0) {
                    entityManager.template RemoveComponent<CameraShake>(followerId);
                }
            }
            place(drawCenter);
        }
        return kContinue;
    }
};

}  // namespace ECSEngine
