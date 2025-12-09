#include "SystemCollection.h"

#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"

void SystemCollection::AddSystem(std::unique_ptr<ECSISystem> _system) {
	if (_system) {
		systems_.emplace_back(std::move(_system));
	}
}

void SystemCollection::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {
	for (auto& system : systems_) {
		if (system) {
			system->OutsideOfRuntimeUpdate(_ecs);
		}
	}
}

void SystemCollection::RuntimeUpdate(ECSGroup* _ecs) {
	for (auto& system : systems_) {
		if (system) {
			system->RuntimeUpdate(_ecs);
		}
	}
}
