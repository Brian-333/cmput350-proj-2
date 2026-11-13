#pragma once

#include "ECSEngine.h"
#include "LocationComponent.h"

namespace ECSEngine
{

template <typename... Components>
class GravitySystem : public System<Components...> {
public:
	const std::string & GetName() const override { 
		static std::string name("GravitySystem"); 
		return name; 
	}
	
	tSystemReturn Run(EngineView<Components...> *engine) override
	{
		auto& em = engine->GetEntityManager();

		for (const Entity& entity : em) {
			auto id = entity.GetID();
			if (!em.IsValid(id)) {
				continue;
			}
			if (!em.template HasComponent<GravityComponent>(id) ||
				!em.template HasComponent<MovementComponent>(id)) {
				continue;
			}

			auto& gravity = em.template GetComponent<GravityComponent>(id);
			auto& movement = em.template GetComponent<MovementComponent>(id);

			if (gravity.disabledTimer <= 0) {
				movement.velocity += gravity.acceleration;
			} else {
				gravity.disabledTimer--;
			}
		}
		return kContinue;
	}
};


}



