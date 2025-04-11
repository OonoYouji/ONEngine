#include "EntityComponentSystem.h"

EntityComponentSystem::EntityComponentSystem(DxManager* _pDxManager) : pDxManager_(_pDxManager) {}
EntityComponentSystem::~EntityComponentSystem() {}

void EntityComponentSystem::Initialize() {

	entities_.reserve(256);

}

void EntityComponentSystem::Update() {
	for (auto& entity : entities_) {
		entity->Update();
	}
}

void EntityComponentSystem::RemoveEntity(IEntity* _entity) {
	/// entityの削除
	auto itr = std::remove_if(entities_.begin(), entities_.end(),
		[_entity](const std::unique_ptr<IEntity>& entity) {
			return entity.get() == _entity;
		}
	);

	entities_.erase(itr, entities_.end());
}

Camera* EntityComponentSystem::GenerateCamera() {
	return nullptr;
}

Camera2D* EntityComponentSystem::GenerateCamera2D() {
	return nullptr;
}
