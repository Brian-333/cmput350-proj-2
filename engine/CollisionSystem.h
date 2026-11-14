#pragma once

#include "ECSEngine.h"
#include "LocationComponent.h"
#include "CollisionComponent.h"
#include "InputComponent.h"
#include "ScoreComponent.h"
#include "CameraComponent.h"
#include <cmath>

namespace ECSEngine {

const float kStarBounce = 0.7f;
const float kShakeScale = 0.15f;
const int kShakeFrames = 6;

template <typename... Components>
class CollisionSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("CollisionSystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override { 
        auto& em = engine->GetEntityManager();
        auto& collisions = em.template GetComponentStorage<CollisionComponent>();

        // Getting only dynamic ids
        for (const Entity& entity : em) {
            auto id = entity.GetID();

            if (!em.IsValid(id)) {
                continue;
            }
            if (!em.template HasComponent<LocationComponent>(id) ||
                !em.template HasComponent<CollisionComponent>(id) ||
                !em.template HasComponent<MovementComponent>(id)) {
                continue;
            }

            auto& collisionC = em.template GetComponent<CollisionComponent>(id);
            auto& locationC = em.template GetComponent<LocationComponent>(id);
            auto& movement = em.template GetComponent<MovementComponent>(id);

            if (collisionC.staticEntity == 1) {
                continue;
            } 
            bool lastFrameGround = collisionC.onGround == 1 ? true : false;
            bool lastFrameLeftWall = collisionC.onLeftWall == 1 ? true : false;
            bool lastFrameRightWall = collisionC.onRightWall == 1 ? true : false;
            bool lastFrameCeiling = collisionC.onCeiling == 1 ? true : false;
            bool isPlayer = em.template HasComponent<InputComponent>(id);

            collisionC.onGround = 0;
            collisionC.onCeiling = 0;
            collisionC.onLeftWall = 0;
            collisionC.onRightWall = 0;

            // processing dynamic with static entity collisions
            for (std::size_t i = 0; i < collisions.mStorage.size(); ++i) {
                if (collisions.valid(i) == false) {
                    continue;
                }

                auto& tile = collisions[i];

                if (tile.staticEntity == 0) {
                    continue;
                }

                // static tiles can use previousCollisionBox since their box never changes
                // and the box is already in world coords
                Rect tileBox = tile.previousCollisionBox;
                Rect playerBox = collisionC.collisionBox;
                playerBox.topLeft += locationC.loc;
            
                Rect overlapBox = tileBox;
                overlapBox &= playerBox;

                // check real collision
                if (overlapBox.width <= 0 || overlapBox.height <= 0) {
                    continue;
                }
                // check spike
                if (tile.doesDamage == 1 && em.template HasComponent<RespawnComponent>(id)) {
                    auto& respawn = em.template GetComponent<RespawnComponent>(id);

                    locationC.loc = respawn.loc;
                    movement.velocity = {0, 0};
                    break;
                }

                // AI assistance (Claude) was used for this side detection
                // which side of the tile were we on last frame? (0.01f absorbs float rounding when flush)
                const Rect& prev = collisionC.previousCollisionBox;
                bool wasAbove = prev.topLeft.y + prev.height <= tileBox.topLeft.y + 0.01f;
                bool wasBelow = prev.topLeft.y >= tileBox.topLeft.y + tileBox.height - 0.01f;
                bool wasLeft = prev.topLeft.x + prev.width <= tileBox.topLeft.x + 0.01f;
                bool wasRight = prev.topLeft.x >= tileBox.topLeft.x + tileBox.width - 0.01f;

                // we came at the tile diagonally (e.g. jumping while pushing into a wall, where the wall tile
                // above us looks like a ceiling), so resolve along the axis we overlap the least
                if ((wasAbove || wasBelow) && (wasLeft || wasRight)) {
                    if (overlapBox.width < overlapBox.height) {
                        wasAbove = false;
                        wasBelow = false;
                    } else {
                        wasLeft = false;
                        wasRight = false;
                    }
                }

                // check onGround
                if (wasAbove) {
                    locationC.loc.y -= overlapBox.height;

                    if (isPlayer && lastFrameGround == 0 && 
                        std::abs(movement.velocity.y) >= 3.f &&
                        !em.template HasComponent<CameraShake>(id)) {
                        em.AddComponent(id, CameraShake{
                            kShakeFrames, {0.f, movement.velocity.y * kShakeScale}
                        });
                    }

                    if (movement.velocity.y > 0) {
                        movement.velocity.y = collisionC.collectable ? -movement.velocity.y * kStarBounce : 0;
                        if (std::abs(movement.velocity.y) < 0.5f) { // prevent star from jittering on the ground
                            movement.velocity.y = 0;
                        } 
                    }
                    
                    collisionC.onGround = 1;
                }
                // AI assistance (Claude) was used for part of the ceiling, wall and fallback checks below
                // check onCeiling: last frame our top was at or below the tile's bottom
                else if (wasBelow) {
                    locationC.loc.y += overlapBox.height;

                    if (isPlayer && lastFrameCeiling == 0 && 
                        std::abs(movement.velocity.y) >= 3.f &&
                        !em.template HasComponent<CameraShake>(id)) {
                        em.AddComponent(id, CameraShake{
                            kShakeFrames, {0.f, movement.velocity.y * kShakeScale}
                        });
                    }

                    if (movement.velocity.y < 0) {
                        movement.velocity.y = collisionC.collectable ? -movement.velocity.y * kStarBounce : 0;
                    }
                    collisionC.onCeiling = 1;
                }
                // check onRightWall: last frame our right side was left of the tile, so the wall is on our right
                else if (wasLeft) {
                    locationC.loc.x -= overlapBox.width;

                    if (isPlayer && lastFrameRightWall == 0 && 
                        std::abs(movement.velocity.x) >= 3.f &&
                        !em.template HasComponent<CameraShake>(id)) {
                        em.AddComponent(id, CameraShake{
                            kShakeFrames, {movement.velocity.x * kShakeScale, 0.f}
                        });
                    }

                    if (movement.velocity.x > 0) {
                        movement.velocity.x = collisionC.collectable ? -movement.velocity.x * kStarBounce : 0;
                    }
                    collisionC.onRightWall = 1;
                }
                // check onLeftWall: last frame our left side was right of the tile, so the wall is on our left
                else if (wasRight) {
                    locationC.loc.x += overlapBox.width;

                    if (isPlayer && lastFrameLeftWall == 0 && 
                        std::abs(movement.velocity.x) >= 3.f &&
                        !em.template HasComponent<CameraShake>(id)) {
                        em.AddComponent(id, CameraShake{
                            kShakeFrames, {movement.velocity.x * kShakeScale, 0.f}
                        });
                    }

                    if (movement.velocity.x < 0) {
                        movement.velocity.x = collisionC.collectable ? -movement.velocity.x * kStarBounce : 0;
                    }
                    collisionC.onLeftWall = 1;
                }
                // fallback: we were already overlapping last frame (e.g. spawned inside a tile),
                // so push out along whichever overlap is smaller, away from the tile's center
                else {
                    float playerCenterX = playerBox.topLeft.x + playerBox.width / 2;
                    float playerCenterY = playerBox.topLeft.y + playerBox.height / 2;
                    float tileCenterX = tileBox.topLeft.x + tileBox.width / 2;
                    float tileCenterY = tileBox.topLeft.y + tileBox.height / 2;

                    if (overlapBox.width < overlapBox.height) {
                        locationC.loc.x += playerCenterX < tileCenterX ? -overlapBox.width : overlapBox.width;
                    } else {
                        locationC.loc.y += playerCenterY < tileCenterY ? -overlapBox.height : overlapBox.height;
                    }
                }
            }

            // processing dyanmic with dynamic collisions
            for (const Entity& other : em) {
                auto otherId = other.GetID();
                if (otherId <= id) {    // this will skip duplicate checks and avoid checking collision with self
                    continue;
                }

                if (!em.IsValid(otherId)) {
                    continue;
                }
                
                if (!em.template HasComponent<MovementComponent>(otherId) ||
                    !em.template HasComponent<LocationComponent>(otherId) ||
                    !em.template HasComponent<CollisionComponent>(otherId)) {
                    continue;
                }

                auto& collisionOtherC = em.template GetComponent<CollisionComponent>(otherId);
                if (collisionOtherC.staticEntity == 1) {    // we only want to check with dynamic entities
                    continue;
                }

                auto& movementOtherC = em.template GetComponent<MovementComponent>(otherId);
                auto& locationOtherC = em.template GetComponent<LocationComponent>(otherId);

                Rect firstBox = collisionC.collisionBox;
                firstBox.topLeft += locationC.loc;
                Rect otherBox = collisionOtherC.collisionBox;
                otherBox.topLeft += locationOtherC.loc;

                Rect overlapBox = otherBox;
                overlapBox &= firstBox;

                // AI assistance (Claude) was used for the collect and star vs star checks below
                // check real collision
                if (overlapBox.width <= 0 || overlapBox.height <= 0) {
                    continue;
                }

                // check collect: one entity keeps score and the other is a star
                bool firstCollects = em.template HasComponent<ScoreComponent>(id) && collisionOtherC.collectable == 1;
                bool otherCollects = em.template HasComponent<ScoreComponent>(otherId) && collisionC.collectable == 1;
                if (firstCollects || otherCollects) {
                    EntityID collectorId = firstCollects ? id : otherId;
                    EntityID starId = firstCollects ? otherId : id;

                    em.template GetComponent<ScoreComponent>(collectorId).score++;
                    engine->GetSoundManager().GetSound("starCollect").play();
                    em.RemoveEntity(starId);

                    if (starId == id) {
                        break;  // our own entity was removed, so its components can't be used anymore
                    }
                    continue;
                }

                // check star vs star: swap velocities (a bounce between equal masses), lose some speed,
                // and push both apart along the smaller overlap so they don't stick together
                if (collisionC.collectable == 1 && collisionOtherC.collectable == 1) {
                    Point2D firstVelocity = movement.velocity;
                    movement.velocity = kStarBounce * movementOtherC.velocity;
                    movementOtherC.velocity = kStarBounce * firstVelocity;

                    float firstCenterX = firstBox.topLeft.x + firstBox.width / 2;
                    float firstCenterY = firstBox.topLeft.y + firstBox.height / 2;
                    float otherCenterX = otherBox.topLeft.x + otherBox.width / 2;
                    float otherCenterY = otherBox.topLeft.y + otherBox.height / 2;

                    if (overlapBox.width < overlapBox.height) {
                        float push = firstCenterX < otherCenterX ? overlapBox.width / 2 : -overlapBox.width / 2;
                        locationC.loc.x -= push;
                        locationOtherC.loc.x += push;
                    } else {
                        float push = firstCenterY < otherCenterY ? overlapBox.height / 2 : -overlapBox.height / 2;
                        locationC.loc.y -= push;
                        locationOtherC.loc.y += push;
                    }
                }
            }

            // our entity may have been collected (removed) above, so stop here if it's gone
            if (!em.IsValid(id)) {
                continue;
            }

            // check if player landed
            if (collisionC.onGround == 1 && !lastFrameGround && isPlayer) {
                engine->GetSoundManager().GetSound("playerLand").play();
            }
        }
        return kContinue; 
    }
};

}  // namespace ECSEngine
