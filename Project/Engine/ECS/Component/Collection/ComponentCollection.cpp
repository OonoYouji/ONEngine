#include "ComponentCollection.h"

/// engine
#include "Engine/ECS/Entity/Interface/IEntity.h"
#include "../Component.h"

ComponentCollection::ComponentCollection() {
	RegisterComponentFactory<Script>();
	RegisterComponentFactory<Transform>();
}

ComponentCollection::~ComponentCollection() {}

IComponent* ComponentCollection::AddComponent(const std::string& _name) {
	size_t hash = GetComponentHash(_name);

	if (arrayMap_.find(hash) == arrayMap_.end()) {
		//RegisterComponentFactory();
		return nullptr;
	}

	return factoryMap_[hash]();
}

void ComponentCollection::RemoveComponent(size_t _hash, size_t _id) {
	auto it = arrayMap_.find(_hash);
	if (it != arrayMap_.end()) {
		it->second->RemoveComponent(_id);
	}
}

void ComponentCollection::RemoveComponentAll(IEntity* _entity) {
	for (auto& component : _entity->GetComponents()) {
		auto it = arrayMap_.find(component.first);
		if (it != arrayMap_.end()) {
			it->second->RemoveComponent(component.second->id);
		}
	}
}




