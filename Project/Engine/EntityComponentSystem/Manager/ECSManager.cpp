
#include "ECSManager.h"


size_t ECSManager::nextCompArrayId_ = 0;

void ECSManager::Initialize() {
	nextEntityId_ = 0;
}

void ECSManager::Update() {

	for(auto& system : systems_) {
		for(auto& entity : entities_) {
			system->Execute(entity, this);
		}
	}

}

size_t ECSManager::GenerateEntity() {
	size_t resultEntity = ++nextEntityId_;

	entities_.push_back(resultEntity);
	return resultEntity;
}

void ECSManager::RemoveEntity(size_t _entity) {

}

void ECSManager::AddSystem(ISystem* _system) {
	std::unique_ptr<ISystem> add(_system);
	systems_.push_back(std::move(add));
}
