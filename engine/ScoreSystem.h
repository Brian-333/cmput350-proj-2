#pragma once

#include "ECSEngine.h"
#include "ScoreComponent.h"
#include "SpriteComponent.h"

namespace ECSEngine {

/**
 * Updates the digit sprites of each score to show its current value.
 */
template <typename... Components>
class ScoreSystem : public System<Components...> {
public:
    /**
     * Get the name of the system.
     * @return The name of the system.
     */
    const std::string& GetName() const override {
        static std::string name("ScoreSystem");
        return name;
    }

    /**
     * Set the texture of each digit entity to match the score.
     * @param engine The engine view used to access the entity manager.
     * @return kContinue to keep the engine running.
     */
    tSystemReturn Run(EngineView<Components...>* engine) override {
        auto& em = engine->GetEntityManager();

        // we only need the score component
        auto& scores = em.template GetComponentStorage<ScoreComponent>();

        for (std::size_t i = 0; i < scores.mStorage.size(); ++i) {
            if (scores.valid(i) == false) {
                continue;
            }

            auto& score = scores[i];

            for (int j = 0; j < 3; ++j) {
                if (!em.template HasComponent<SpriteComponent>(score.digitID[j])) {
                    continue;
                }

                auto& sprite = em.template GetComponent<SpriteComponent>(score.digitID[j]);
                sprite.textureID = j == 2   ? score.digitTextureID[(score.score / 100) % 10]
                                   : j == 1 ? score.digitTextureID[(score.score / 10) % 10]
                                            : score.digitTextureID[score.score % 10];
            }
        }

        return kContinue;
    }
};

}  // namespace ECSEngine
