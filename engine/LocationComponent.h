#pragma once

#include <bitset>
#include <unordered_map>

#include "MathUtil.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace ECSEngine {

/**
 * The top-left location of an entity. All other stored locations are relative to this
 * location, so moving it moves anything relative to it, such as:
 * - sprite bounding boxes
 * - collision bounding boxes
 */
struct LocationComponent {
    Point2D loc;
};

/**
 * The velocity of an entity, added to its location every frame.
 */
struct MovementComponent {
    Point2D velocity;  // Velocity in pixels per second
};

/**
 * Gravity acting on an entity.
 */
struct GravityComponent {
    /**
     * Create a gravity component.
     * @param acceleration The acceleration added to velocity each frame.
     * @param disabledTimer The number of frames gravity is turned off for.
     */
    GravityComponent(Point2D acceleration, int disabledTimer = 0)
        : acceleration(acceleration), disabledTimer(disabledTimer) {}
    Point2D acceleration;  // Acceleration in pixels per second squared
    int disabledTimer;     // Frames left with gravity turned off (e.g. during a dash)
    bool wallSliding =
        false;  // was the entity sliding down a wall last frame (used to play the slide sound once)
};

// location for respawning
struct RespawnComponent {
    Point2D loc;
};

}  // namespace ECSEngine
