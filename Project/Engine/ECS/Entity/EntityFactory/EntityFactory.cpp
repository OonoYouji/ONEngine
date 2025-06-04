#include "EntityFactory.h"

EntityFactory::EntityFactory(EntityComponentSystem* _ecs)
	: pECS_(_ecs) {}

EntityFactory::~EntityFactory() {}

void EntityFactory::Register(const std::string& _entityName, GenerateFunc _generateFunc) {
	factoryMap_[_entityName] = _generateFunc;
}

std::unique_ptr<IEntity> EntityFactory::Generate(const std::string& _entityName) {
	auto it = factoryMap_.find(_entityName);
	if (it != factoryMap_.end()) {
		return it->second();
	}
	return nullptr; // or throw an exception
}
