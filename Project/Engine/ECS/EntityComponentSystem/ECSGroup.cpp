#include "ECSGroup.h"

/// engine
#include "AddECSComponentFactoryFunction.h"

ECSGroup::ECSGroup(DxManager* _dxm) {
	/// インスタンスの生成
	entityCollection_ = std::make_unique<EntityCollection>(this, _dxm);
	componentCollection_ = std::make_unique<ComponentCollection>();
	systemCollection_ = std::make_unique<SystemCollection>();
}

ECSGroup::~ECSGroup() {}

void ECSGroup::Initialize(const std::string& _groupName) {
	/// このECSGroupの名前を決める
	groupName_ = _groupName;

	AddComponentFactoryFunction(componentCollection_.get());
}

void ECSGroup::Update() {}

GameEntity* ECSGroup::GenerateEntity(const Guid& _guid, bool _isRuntime) {
	return entityCollection_->GenerateEntity(_guid, _isRuntime);
}

GameEntity* ECSGroup::GenerateEntityFromPrefab(const std::string& _prefabName, bool _isRuntime) {
	return entityCollection_->GenerateEntityFromPrefab(_prefabName, _isRuntime);
}


GameEntity* ECSGroup::GetEntityFromGuid(const Guid& _guid) {
	/// 例外チェック(無効値なら nullptr を返す)
	if (!_guid.CheckValid()) {
		Console::LogError("ECSGroup::GetEntityFromGuid: Invalid Guid provided.");
		return nullptr;
	}

	const auto& entities = entityCollection_->GetEntities();
	for (const auto& entity : entities) {
		if (entity->GetGuid() == _guid) {
			return entity.get();
		}
	}

	return nullptr;
}

void ECSGroup::RemoveEntity(GameEntity* _entity, bool _deleteChildren) {
	/// 例外チェック
	if (_entity == nullptr) {
		Console::LogError("ECSGroup::RemoveEntity: Null entity provided.");
		return;
	}

	entityCollection_->RemoveEntity(_entity, _deleteChildren);
}

void ECSGroup::RemoveEntityAll() {
	entityCollection_->RemoveEntityAll();
}

void ECSGroup::AddDoNotDestroyEntity(GameEntity* _entity) {
	if (_entity == nullptr) {
		Console::LogError("ECSGroup::AddDoNotDestroyEntity: Null entity provided.");
		return;
	}

	entityCollection_->AddDoNotDestroyEntity(_entity);
}

void ECSGroup::RemoveDoNotDestroyEntity(GameEntity* _entity) {
	if (_entity == nullptr) {
		Console::LogError("ECSGroup::RemoveDoNotDestroyEntity: Null entity provided.");
		return;
	}

	entityCollection_->RemoveDoNotDestroyEntity(_entity);
}

uint32_t ECSGroup::GetEntityId(const std::string& _name) {
	return entityCollection_->GetEntityId(_name);
}

uint32_t ECSGroup::CountEntity(const std::string& _name) {
	const auto& entities = entityCollection_->GetEntities();
	return static_cast<uint32_t>(std::count_if(entities.begin(), entities.end(),
		[&_name](const std::unique_ptr<GameEntity>& entity) {
			std::string name = entity->GetName();
			/// 後ろから"_"を検索、"_"を含む場合はその前までを比較する
			if (name.find_last_of('_') != std::string::npos) {
				return name.substr(0, name.find_last_of('_')) == _name;
			}

			return name == _name;
		}
	));
}

IComponent* ECSGroup::AddComponent(const std::string& _compName) {
	return componentCollection_->AddComponent(_compName);
}

void ECSGroup::RemoveComponent(size_t _hash, uint32_t _compId) {
	componentCollection_->RemoveComponent(_hash, _compId);
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

void ECSGroup::SetMainCamera(CameraComponent* _camera) {
	entityCollection_->SetMainCamera(_camera);
}

void ECSGroup::SetMainCamera2D(CameraComponent* _camera) {
	entityCollection_->SetMainCamera2D(_camera);
}

EntityCollection* ECSGroup::GetEntityCollection() {
	return entityCollection_.get();
}

const std::vector<std::unique_ptr<GameEntity>>& ECSGroup::GetEntities() const {
	return entityCollection_->GetEntities();
}

GameEntity* ECSGroup::GetEntity(size_t _id) const {
	return entityCollection_->GetEntity(_id);
}

const CameraComponent* ECSGroup::GetMainCamera() const {
	return entityCollection_->GetMainCamera();
}

CameraComponent* ECSGroup::GetMainCamera() {
	return entityCollection_->GetMainCamera();
}

const CameraComponent* ECSGroup::GetMainCamera2D() const {
	return entityCollection_->GetMainCamera2D();
}

CameraComponent* ECSGroup::GetMainCamera2D() {
	return entityCollection_->GetMainCamera2D();
}

const std::string& ECSGroup::GetGroupName() const {
	return groupName_;
}
