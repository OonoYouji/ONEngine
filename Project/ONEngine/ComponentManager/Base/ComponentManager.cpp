#include "ComponentManager.h"

void ComponentManager::Finalize() {
	componentArrays_.clear();
}

void ComponentManager::Update() {
	for(auto&& compArray : componentArrays_) {
		compArray.second->Update();
	}
}
