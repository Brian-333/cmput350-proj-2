#pragma once

#include <vector>
// #include "ECSEngine.h"

namespace ECSEngine {

enum tSystemReturn { kContinue, kExit };

template <typename... Components>
class EngineView;

/**
 * The base class for all systems. A system runs once per frame.
 */
template <typename... Components>
class System {
public:
    virtual ~System() = default;

    /**
     * Run the system for one frame.
     * @param scene The engine view used to access the managers.
     * @return kExit to stop the engine, kContinue otherwise.
     */
    virtual tSystemReturn Run(EngineView<Components...>* scene) = 0;

    /**
     * Get the name of the system.
     * @return The name of the system.
     */
    virtual const std::string& GetName() const = 0;
};

/**
 * Owns the systems and runs them in the order they were added.
 */
template <typename... Components>
class SystemManager {
public:
    /**
     * Run every system once, stopping early if a system requests exit.
     * @param scene The engine view passed to each system.
     * @return False if a system requested exit, true otherwise.
     */
    bool Run(EngineView<Components...>* scene) {
        for (auto& system : mSystems) {
            if (system->Run(scene) == kExit)  // system wants to exit
            {
                std::cout << system->GetName() << " requested exit from system manager\n";
                return false;
            }
        }
        return true;
    }
    /**
     * Add a system to run after the existing systems.
     * @param system The system to add.
     */
    void AddSystem(std::unique_ptr<System<Components...>>&& system) {
        mSystems.push_back(std::move(system));
    }

    /**
     * Remove all systems.
     */
    void ClearSystems() { mSystems.clear(); }

private:
    std::vector<std::unique_ptr<System<Components...>>> mSystems;
};

}  // namespace ECSEngine
