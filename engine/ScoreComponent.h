#pragma once

#include <unordered_map>

#include "MathUtil.h"
#include "SpriteManager.h"

namespace ECSEngine {

/**
 * A score counter displayed as three digit entities.
 */
class ScoreComponent {
public:
    int score;
    SpriteID digitTextureID[10];  // Texture for each digit 0-9
    EntityID digitID[3];          // Entities showing the ones, tens, and hundreds digits

private:
};

}  // namespace ECSEngine
