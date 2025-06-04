#include "ComponentCollection.h"

ComponentCollection::ComponentCollection() {}

ComponentCollection::~ComponentCollection() {}

IComponent* ComponentCollection::AddComponent(const std::string& _name) {
	size_t hash = GetComponentHash(_name);

	if (arrayMap_.find(hash) == arrayMap_.end()) {
		//RegisterComponentFactory<Comp>();
		return nullptr;
	}

	return factoryMap_[hash]();
}




