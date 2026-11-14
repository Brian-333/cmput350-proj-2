#pragma once

#include <bitset>
#include <unordered_map>

#include "MathUtil.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Keyboard.hpp>

namespace ECSEngine {

/**
 * Keyboard state for a player-controlled entity.
 */
struct InputComponent {
    /**
     * Create an input component.
     * @param whichEntity The entity this input controls.
     */
    InputComponent(EntityID whichEntity) : entity(whichEntity), lastDash(0) {}
    EntityID entity;
    int lastDash;                                          // Frames since the last dash
    std::bitset<sf::Keyboard::ScancodeCount> keydown;      // Keys held this frame
    std::bitset<sf::Keyboard::ScancodeCount> lastKeydown;  // Keys held last frame
};

}  // namespace ECSEngine
