#include "ECSGroup.h"

ECSGroup::ECSGroup() {}
ECSGroup::~ECSGroup() {}

void ECSGroup::Initialize() {
	/// インスタンスの生成
	entityCollection_    = std::make_unique<EntityCollection>();
	componentCollection_ = std::make_unique<ComponentCollection>();
	systemCollection_    = std::make_unique<SystemCollection>();
}

void ECSGroup::Update() {
}

GameEntity* ECSGroup::GenerateEntity(bool _isRuntime) {
	return entityCollection_->GenerateEntity(_isRuntime);
}

GameEntity* ECSGroup::GenerateEntityFromPrefab(const std::string& _prefabName, bool _isRuntime) {
	return entityCollection_->GenerateEntityFromPrefab(_prefabName, _isRuntime);
}

void ECSGroup::RemoveEntity(GameEntity* _entity, bool _deleteChildren) {
	/// 例外チェック
	if (_entity == nullptr) {
		return;
	}

	entityCollection_->RemoveEntity(_entity, _deleteChildren);
}

void ECSGroup::RemoveEntityAll() {
	entityCollection_->RemoveEntityAll();
}

void ECSGroup::AddDoNotDestroyEntity(GameEntity* _entity) {
	if (_entity == nullptr) {
		return;
	}

	entityCollection_->AddDoNotDestroyEntity(_entity);
}

void ECSGroup::RemoveDoNotDestroyEntity(GameEntity* _entity) {
	if (_entity == nullptr) {
		return;
	}

	entityCollection_->RemoveDoNotDestroyEntity(_entity);
}

uint32_t ECSGroup::GetEntityId(const std::string& _name) {
	return entityCollection_->GetEntityId(_name);
}

IComponent* ECSGroup::AddComponent(const std::string& _compName) {
	return componentCollection_->AddComponent(_compName);
}

void ECSGroup::RemoveComponentAll(GameEntity* _entity) {
	if (_entity == nullptr) {
		return;
	}

	componentCollection_->RemoveComponentAll(_entity);
}

void ECSGroup::LoadComponent(GameEntity* _entity) {
	componentInputCommand_.SetEntity(_entity);
	componentInputCommand_.Execute();
}

void ECSGroup::OutsideOfRuntimeUpdateSystems() {
	systemCollection_->OutsideOfRuntimeUpdate(this);
}

void ECSGroup::RuntimeUpdateSystems() {
	systemCollection_->RuntimeUpdate(this);
}
