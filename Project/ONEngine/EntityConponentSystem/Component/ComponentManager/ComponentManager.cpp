#include "ComponentManager.h"

void ComponentManager::Initialize() {}

void ComponentManager::Update() {
	for(auto& compArray : componentArrays_) {
		compArray.second->Update();
	}
}
