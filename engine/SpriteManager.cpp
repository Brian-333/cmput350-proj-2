#include "SpriteManager.h"

namespace ECSEngine {

/**
 * Register a sprite cut from a texture. Each texture file is only loaded once.
 * @param texturePath The path to the texture file.
 * @param sourceRect The area of the texture to use for the sprite, in pixels.
 * @return The ID of the new sprite.
 */
SpriteID SpriteManager::RegisterTexture(const std::string& texturePath, const Rect& sourceRect) {
    sf::IntRect intRect(
        {static_cast<int>(sourceRect.topLeft.x), static_cast<int>(sourceRect.topLeft.y)},
        {static_cast<int>(sourceRect.width), static_cast<int>(sourceRect.height)}
    );
    if (!mTextures.contains(texturePath)) {
        mTextures[texturePath] = sf::Texture(texturePath);
    }
    mSprites.emplace_back(mTextures[texturePath]);
    mSprites.back().setTextureRect(intRect);
    return mSprites.size() - 1;
}

/**
 * Get a sprite from the sprite manager.
 * @param id The ID of the sprite.
 * @return The sprite, or an empty fallback sprite if the ID is not registered.
 */
sf::Sprite& SpriteManager::GetSprite(SpriteID id) {
    if (id >= mSprites.size()) {
        return mFallbackSprite;
    }
    return mSprites[id];
}

}  // namespace ECSEngine
