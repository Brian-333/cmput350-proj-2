#pragma once

#include <bitset>
#include <unordered_map>

#include "MathUtil.h"
#include "SpriteManager.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace ECSEngine {

/**
 * A sprite drawn for an entity. The bounds are relative to the entity location,
 * so the sprite is only drawn if the entity also has a LocationComponent.
 */
class SpriteComponent {
public:
    SpriteID textureID;
    Rect bounds;
    bool worldSpace;  // If false, this sprite is drawn in screen space instead of world space
private:
};

}  // namespace ECSEngine
