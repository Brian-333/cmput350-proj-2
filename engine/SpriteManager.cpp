#include "SpriteManager.h"

namespace ECSEngine {

SpriteID SpriteManager::RegisterTexture(const std::string& texturePath, const Rect& sourceRect) {
    sf::IntRect intRect(
        {static_cast<int>(sourceRect.topLeft.x), static_cast<int>(sourceRect.topLeft.y)},
        {static_cast<int>(sourceRect.width), static_cast<int>(sourceRect.height)});
    if (!mTextures.contains(texturePath)) {
        mTextures[texturePath] = sf::Texture(texturePath);
    }
    mSprites.emplace_back(mTextures[texturePath]);
    mSprites.back().setTextureRect(intRect);
    return mSprites.size() - 1;
}

sf::Sprite& SpriteManager::GetSprite(SpriteID id) {
    if (id >= mSprites.size()) {
        return mFallbackSprite;
    }
    return mSprites[id];
}

}  // namespace ECSEngine
