#include "SystemCollection.h"

void SystemCollection::Update(EntityComponentSystem* _ecs) {
	for (auto& system : systems_) {
		if (system) {
			//system->RuntimeUpdate(_ecs);
		}
	}
}
