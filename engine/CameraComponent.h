#pragma once

#include <unordered_map>

#include "Entity.h"
#include "LocationComponent.h"
#include "MathUtil.h"
#include "SpriteComponent.h"
#include "SystemManager.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace ECSEngine {

/**
 * A camera that scrolls the view to keep a followed entity inside its exclusion zones.
 */
struct CameraComponent {
    Point2D position;  // [world coordinates] position in world coordinates
    float worldUnitsPerPixel;
    Rect hardExclusion;  // [screen coordinates] Area where the camera is not allowed to go
                         // (negative of this rect)
    Rect softExclusion;  // [screen coordinates] Area where the camera is allowed to go (but will
                         // scroll out)
    float speed;
};

/**
 * Marks a camera entity as following another entity.
 */
struct CameraFollower {
    EntityID entityID;  // The entity that this camera follows
};

/**
 * A short camera shake applied to the followed entity, alternating the offset sign each frame.
 */
struct CameraShake {
    int frameLimit;  // Frames of shake remaining
    Point2D offset;  // [world coordinates] Shake offset from the camera center
};

}  // namespace ECSEngine
