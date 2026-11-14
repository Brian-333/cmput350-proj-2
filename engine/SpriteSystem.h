#pragma once

#include "ECSEngine.h"
#include "LocationComponent.h"
#include "MathUtil.h"
#include "SpriteComponent.h"
#include "SpriteManager.h"
#include "SystemManager.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace ECSEngine {

// Written with help of Cursor.
// Scales each texture rect into its destination box, as in the lab 8 sprite transform.
// Prompt: implement SpriteSystem from the lab 8 sprite exercise and the Project 2 spec.
template <typename... Components>
class SpriteSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("SpriteSystem");
        return name;
    }

    // ECSEngine::Run owns clear() and display(), so DebugDraw can draw after this system.
    tSystemReturn Run(EngineView<Components...>* engine) override {
        auto& entityManager = engine->GetEntityManager();
        auto& spriteManager = engine->GetSpriteManager();
        auto& windowManager = engine->GetWindowManager();
        auto window = windowManager.GetWindow();

        for (const Entity& entity : entityManager) {
            auto id = entity.GetID();
            if (!entityManager.IsValid(id)) continue;
            if (!entityManager.template HasComponent<SpriteComponent>(id)) continue;
            if (!entityManager.template HasComponent<LocationComponent>(id)) continue;

            const auto& spriteC = entityManager.template GetComponent<SpriteComponent>(id);
            const auto& location = entityManager.template GetComponent<LocationComponent>(id);

            sf::Sprite& sprite = spriteManager.GetSprite(spriteC.textureID);
            auto texSize = sprite.getTextureRect().size;
            if (texSize.x == 0 || texSize.y == 0) continue;

            Rect dst = spriteC.bounds;
            dst.topLeft += location.loc;
            if (spriteC.worldSpace) dst = windowManager.WorldToWindow(dst);

            sprite.setScale({dst.width / static_cast<float>(texSize.x),
                             dst.height / static_cast<float>(texSize.y)});
            sprite.setPosition({dst.topLeft.x, dst.topLeft.y});
            window->draw(sprite);
        }
        return kContinue;
    }
};

}  // namespace ECSEngine
