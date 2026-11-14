#pragma once

#include <bitset>
#include <unordered_map>

#include "MathUtil.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace ECSEngine {

/**
 * Collision data for an entity. The collision box is relative to the entity's LocationComponent.
 */
struct CollisionComponent {
    uint8_t staticEntity : 1;  // Never moves (e.g. tiles)
    uint8_t doesDamage : 1;    // Respawns entities that touch it (e.g. spikes)
    uint8_t onGround : 1;
    uint8_t onCeiling : 1;
    uint8_t onLeftWall : 1;
    uint8_t onRightWall : 1;
    uint8_t collectable : 1;    // Can be collected by an entity with a ScoreComponent
    Rect collisionBox;          // The bounding box for collision detection
    Rect previousCollisionBox;  // The previous bounding box for collision detection
};

}  // namespace ECSEngine
