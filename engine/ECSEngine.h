#pragma once

#include <vector>

#include "CameraComponent.h"
#include "CollisionComponent.h"
#include "EntityManager.h"
#include "InputComponent.h"
#include "LocationComponent.h"
#include "MathUtil.h"
#include "ScoreComponent.h"
#include "SoundManager.h"
#include "SpawnComponent.h"
#include "SpriteComponent.h"
#include "SpriteManager.h"
#include "SystemManager.h"
#include "WindowManager.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace ECSEngine {

template <typename... Components>
class EngineView {
public:
    virtual SoundManager& GetSoundManager() = 0;
    virtual SpriteManager& GetSpriteManager() = 0;
    virtual EntityManager<Components...>& GetEntityManager() = 0;
    virtual WindowManager& GetWindowManager() = 0;
};

template <typename... Components>
class ECSEngine : public EngineView<Components...> {
public:
    ECSEngine(unsigned int width, unsigned int height, const std::string& name);
    ~ECSEngine();

    void Run();

    SoundManager& GetSoundManager() override { return mSoundManager; }

    SpriteManager& GetSpriteManager() override { return mSpriteManager; }

    EntityManager<Components...>& GetEntityManager() override { return mEntityManager; }

    WindowManager& GetWindowManager() override { return mWindowManager; }

    SystemManager<Components...>& GetSystemManager() { return mSystemManager; }

private:
    bool ProcessEvents();
    void DebugDraw();

    EntityManager<Components...> mEntityManager;
    SystemManager<Components...> mSystemManager;
    SpriteManager mSpriteManager;
    SoundManager mSoundManager;
    WindowManager mWindowManager;

    // For debugging - pause and step the engine
    bool running, step;
    int debug;
};

template <typename... Components>
ECSEngine<Components...>::ECSEngine(unsigned int width, unsigned int height,
                                    const std::string& name)
    : mWindowManager(width, height, name), running(true), step(false), debug(0) {
    std::shared_ptr<sf::RenderWindow> window = mWindowManager.GetWindow();
    window->setFramerateLimit(60);
}

template <typename... Components>
ECSEngine<Components...>::~ECSEngine() {}

// Written with help of Cursor
template <typename... Components>
void ECSEngine<Components...>::Run() {
    std::shared_ptr<sf::RenderWindow> window = mWindowManager.GetWindow();
    while (window->isOpen()) {
        if (!ProcessEvents()) break;
        if (!running && !step) continue;

        step = false;
        window->clear();
        if (!mSystemManager.Run(this)) break;
        if (debug) DebugDraw();
        window->display();
    }
}

template <typename... Components>
bool ECSEngine<Components...>::ProcessEvents() {
    std::shared_ptr<sf::RenderWindow> window = mWindowManager.GetWindow();
    auto& inputs = mEntityManager.template GetComponentStorage<InputComponent>();

    auto setKey = [&](sf::Keyboard::Scancode scancode, bool down) {
        const auto idx = static_cast<std::size_t>(scancode);
        for (std::size_t i = 0; i < inputs.mStorage.size(); ++i) {
            if (!inputs.valid(i)) continue;
            inputs[i].keydown[idx] = down;
        }
    };
    while (const std::optional event = window->pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window->close();
            return false;
        }
        if (const auto* pressed = event->getIf<sf::Event::KeyPressed>()) {
            setKey(pressed->scancode, true);
            // optional: pause / step / debug toggles here
            if (pressed->scancode == sf::Keyboard::Scan::Grave) {
                debug = 1 - debug;
            }
        }
        if (const auto* released = event->getIf<sf::Event::KeyReleased>()) {
            setKey(released->scancode, false);
        }
    }

    return true;
}

// Written with help of Cursor
template <typename... Components>
void ECSEngine<Components...>::DebugDraw() {
    for (const Entity& entity : mEntityManager) {
        auto id = entity.GetID();
        if (!mEntityManager.IsValid(id)) continue;
        if (!mEntityManager.template HasComponent<CollisionComponent>(id)) continue;
        if (!mEntityManager.template HasComponent<LocationComponent>(id)) continue;
        const auto& collision = mEntityManager.template GetComponent<CollisionComponent>(id);
        const auto& location = mEntityManager.template GetComponent<LocationComponent>(id);
        Rect box = collision.collisionBox;
        box.topLeft += location.loc;
        // optional if camera is set:
        box = mWindowManager.WorldToWindow(box);
        mWindowManager.DebugFrameRect(box, 255, 255, 255, 200);
    }
}

}  // namespace ECSEngine
