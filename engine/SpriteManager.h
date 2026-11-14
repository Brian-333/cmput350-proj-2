#pragma once

#include <unordered_map>
#include <vector>

#include "MathUtil.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace ECSEngine {

using SpriteID = size_t;

/**
 * Loads textures and owns the sprites cut from them, looked up by SpriteID.
 */
class SpriteManager {
public:
    SpriteManager() = default;
    ~SpriteManager() = default;
    [[nodiscard]] SpriteID RegisterTexture(const std::string& texturePath, const Rect& sourceRect);
    sf::Sprite& GetSprite(SpriteID id);

private:
    std::unordered_map<std::string, sf::Texture> mTextures;
    std::vector<sf::Sprite> mSprites;
    // Owned here, not as function statics, so they are destroyed with the engine
    // while SFML's graphics context still exists.
    sf::Texture mFallbackTexture;
    sf::Sprite mFallbackSprite{mFallbackTexture};
};

}  // namespace ECSEngine
