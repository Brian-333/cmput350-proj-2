#pragma once

#include <unordered_map>

#include "MathUtil.h"
#include "SpriteManager.h"

namespace ECSEngine {

/**
 * A spawner that creates entities at its location on a timer.
 * Spawning only happens if the entity also has a LocationComponent.
 */
class SpawnComponent {
public:
    EntityID entityID;        // The entity that this spawn component is attached to
    std::string spawnType;    // The kind of entity to spawn (e.g. "star")
    SpriteID spawnTextureID;  // The texture given to spawned entities
    int timeToNextSpawn;      // in frames
    int timeBetweenSpawns;    // in frames
    int totalSpawns;          // Spawns remaining

private:
};

}  // namespace ECSEngine
