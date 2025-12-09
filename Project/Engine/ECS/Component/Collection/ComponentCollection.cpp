#include "ComponentCollection.h"

using namespace ONEngine;

/// engine
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"

ComponentCollection::ComponentCollection() {
	/// この二つだけ例外で最初に登録しておく
	RegisterComponentFactory<Script>();
	RegisterComponentFactory<Transform>();
}

ComponentCollection::~ComponentCollection() {}

IComponent* ComponentCollection::AddComponent(const std::string& _name) {
	size_t hash = GetComponentHash(_name);

	if (arrayMap_.find(hash) == arrayMap_.end()) {
		return nullptr;
	}

	IComponent* comp = arrayMap_[hash]->AddComponentUntyped();

	return comp;
}

void ComponentCollection::RemoveComponent(size_t _hash, size_t _id) {
	auto it = arrayMap_.find(_hash);
	if (it != arrayMap_.end()) {
		it->second->RemoveComponent(_id);
	}
}

void ComponentCollection::RemoveComponentAll(GameEntity* _entity) {
	for (auto& component : _entity->GetComponents()) {
		auto it = arrayMap_.find(component.first);
		if (it != arrayMap_.end()) {
			it->second->RemoveComponent(component.second->id);
		}
	}
}




