#pragma once

#include "CollisionComponent.h"
#include "ECSEngine.h"
#include "InputComponent.h"
#include "Keyboard.h"
#include "LocationComponent.h"

namespace ECSEngine {

const float kFastFallMultiplier = 5.f;
const float kFallMultiplier = 1.5f;
const float kWallSlideSpeed = 2.f;  // max fall speed while sliding down a wall

template <typename... Components>
class GravitySystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("GravitySystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override {
        auto& em = engine->GetEntityManager();

        for (const Entity& entity : em) {
            auto id = entity.GetID();
            if (!em.IsValid(id)) {
                continue;
            }
            if (!em.template HasComponent<GravityComponent>(id) ||
                !em.template HasComponent<MovementComponent>(id)) {
                continue;
            }

            bool isPlayer = em.template HasComponent<InputComponent>(id);

            auto& gravity = em.template GetComponent<GravityComponent>(id);
            auto& movement = em.template GetComponent<MovementComponent>(id);

            Point2D accel = gravity.acceleration;

            if (movement.velocity.y > 0) {
                accel.y *= kFallMultiplier;
            }

            if (isPlayer) {
                auto& input = em.template GetComponent<InputComponent>(id);

                if (input.keydown[Key::Down] && movement.velocity.y > 0) {
                    accel.y = accel.y * kFastFallMultiplier;
                }
            }

            if (gravity.disabledTimer <= 0) {
                movement.velocity += accel;
            } else {
                gravity.disabledTimer--;
            }

            // AI assistance (Claude) was used for the wall slide below
            // wall slide: falling while pushing into a wall caps the fall speed and plays a sound
            if (isPlayer && em.template HasComponent<CollisionComponent>(id)) {
                auto& input = em.template GetComponent<InputComponent>(id);
                auto& collision = em.template GetComponent<CollisionComponent>(id);
                bool sliding = false;

                if (movement.velocity.y > kWallSlideSpeed && collision.onGround == 0 &&
                    collision.onLeftWall == 1 && input.keydown[Key::Left]) {
                    movement.velocity.y = kWallSlideSpeed;
                    sliding = true;
                } else if (movement.velocity.y > kWallSlideSpeed && collision.onGround == 0 &&
                           collision.onRightWall == 1 && input.keydown[Key::Right]) {
                    movement.velocity.y = kWallSlideSpeed;
                    sliding = true;
                }

                // only play the sound when a slide starts, not every frame of it
                if (sliding && !gravity.wallSliding) {
                    engine->GetSoundManager().GetSound("wallSlide").play();
                }
                gravity.wallSliding = sliding;
            }

            if (movement.velocity.y > 15) {
                movement.velocity.y = 15;
            }
        }
        return kContinue;
    }
};

}  // namespace ECSEngine
