#pragma once

#include "ECSEngine.h"
#include "ScoreComponent.h"
#include "SpriteComponent.h"

namespace ECSEngine {

template <typename... Components>
class ScoreSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("ScoreSystem");
        return name;
    }

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
