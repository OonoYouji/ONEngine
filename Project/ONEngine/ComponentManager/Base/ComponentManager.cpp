#include "ComponentManager.h"

void ComponentManager::Update() {
	for(auto&& compArray : componentArrays_) {
		compArray.second->Update();
	}
}
