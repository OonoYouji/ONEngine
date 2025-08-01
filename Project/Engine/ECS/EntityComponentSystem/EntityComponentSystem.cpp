#include "EntityComponentSystem.h"

/// std
#include <numbers>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Editor/EditorManager.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentEditCommands.h"
#include "Engine/ECS/Entity/EntityJsonConverter.h"

/// ecs
#include "../Component/Component.h"
#include "AddECSSystemFunction.h"
#include "AddECSComponentFactoryFunction.h"

#include "Engine/ECS/Entity/Entities/Camera/DebugCamera.h"
#include "Engine/ECS/Entity/Entities/Grid/Grid.h"
#include "Engine/ECS/Entity/Entities/EmptyEntity/EmptyEntity.h"

namespace {
	EntityComponentSystem* gECS = nullptr;
}

void SetEntityComponentSystemPtr(EntityComponentSystem* _ecs) {
	gECS = _ecs;
}

EntityComponentSystem* GetEntityComponentSystemPtr() {
	return gECS;
}


EntityComponentSystem::EntityComponentSystem(DxManager* _pDxManager)
	: pDxManager_(_pDxManager) {}
EntityComponentSystem::~EntityComponentSystem() {}

void EntityComponentSystem::Initialize(GraphicsResourceCollection* _graphicsResourceCollection) {

	pGraphicsResourceCollection_ = _graphicsResourceCollection;
	pDxDevice_ = pDxManager_->GetDxDevice();

	entityCollection_ = std::make_unique<EntityCollection>(this, pDxManager_);
	componentCollection_ = std::make_unique<ComponentCollection>();


	AddECSSystemFunction(this, pDxManager_, pGraphicsResourceCollection_);
	AddComponentFactoryFunction(componentCollection_.get());


	debugCamera_ = entityCollection_->GetFactory()->Generate("DebugCamera");
	debugCamera_->pEntityComponentSystem_ = this;
	debugCamera_->CommonInitialize();
	debugCamera_->Initialize();

	/// gridの初期化
	gridEntity_ = entityCollection_->GetFactory()->Generate("Grid");
	gridEntity_->pEntityComponentSystem_ = this;
	gridEntity_->CommonInitialize();
	gridEntity_->Initialize();


}


void EntityComponentSystem::Update() {

	entityCollection_->UpdateEntities();


	auto entities = GetActiveEntities();
	UpdateSystems(entities);
}

void EntityComponentSystem::DebuggingUpdate() {
	debugCamera_->Update();
	debugCamera_->UpdateTransform();

	gridEntity_->Update();
	gridEntity_->UpdateTransform();
}

IEntity* EntityComponentSystem::GenerateEntity(const std::string& _name, bool _isInit) {
	return entityCollection_->GenerateEntity(_name, _isInit);
}

IEntity* EntityComponentSystem::GenerateEntityFromPrefab(const std::string& _prefabName, bool _isRuntime) {
	return entityCollection_->GenerateEntityFromPrefab(_prefabName, _isRuntime);
}

void EntityComponentSystem::RemoveEntity(IEntity* _entity, bool _deleteChildren) {
	return entityCollection_->RemoveEntity(_entity, _deleteChildren);
}

Camera* EntityComponentSystem::GenerateCamera() {
	return entityCollection_->GenerateCamera();
}

void EntityComponentSystem::RemoveEntityAll() {
	entityCollection_->RemoveEntityAll();
}

void EntityComponentSystem::AddDoNotDestroyEntity(IEntity* _entity) {
	entityCollection_->AddDoNotDestroyEntity(_entity);
}

void EntityComponentSystem::RemoveDoNotDestroyEntity(IEntity* _entity) {
	entityCollection_->RemoveDoNotDestroyEntity(_entity);
}

void EntityComponentSystem::SetFactoryRegisterFunc(std::function<void(EntityFactory*)> _func) {
	entityCollection_->SetFactoryRegisterFunc(_func);
}

uint32_t EntityComponentSystem::GetEntityId(const std::string& _name) {
	return entityCollection_->GetEntityId(_name);
}

std::vector<IEntity*> EntityComponentSystem::GetActiveEntities() const {
	std::vector<IEntity*> result;
	result.reserve(entityCollection_->GetEntities().size());

	for (const auto& entity : entityCollection_->GetEntities()) {
		if (entity->GetActive()) {
			result.push_back(entity.get());
		}
	}

	return result;
}

IComponent* EntityComponentSystem::AddComponent(const std::string& _name) {
	return componentCollection_->AddComponent(_name);
}

void EntityComponentSystem::RemoveComponent(size_t _hash, size_t _id) {
	componentCollection_->RemoveComponent(_hash, _id);
}

void EntityComponentSystem::LoadComponent(IEntity* _entity) {
	componentInputCommand_.SetEntity(_entity);
	componentInputCommand_.Execute();
}

void EntityComponentSystem::RemoveComponentAll(IEntity* _entity) {
	componentCollection_->RemoveComponentAll(_entity);
}

void EntityComponentSystem::UpdateSystems(const std::vector<IEntity*>& _entities) {
	for (auto& system : systems_) {
		system->Update(this, _entities);
	}
}

void EntityComponentSystem::ReloadPrefab(const std::string& _prefabName) {
	entityCollection_->ReloadPrefab(_prefabName);
}

IEntity* EntityComponentSystem::GetGridEntity() const {
	return gridEntity_.get();
}

IEntity* EntityComponentSystem::GetPrefabEntity() const {
	return prefabEntity_.get();
}

IEntity* EntityComponentSystem::GeneratePrefabEntity(const std::string& _name) {

	/// 同じエンティティが生成されているかチェック
	if (prefabEntity_ && prefabEntity_->GetName() == _name) {
		return prefabEntity_.get();
	}



	std::string prefabName = Mathf::FileNameWithoutExtension(_name);

	/// prefabが存在するかチェック
	EntityPrefab* prefab = entityCollection_->GetPrefab(_name);
	if (!prefab) {
		return nullptr;
	}

	/// entityを生成する
	std::unique_ptr<IEntity> entity = entityCollection_->GetFactory()->Generate("EmptyEntity");

	/// 違うエンティティが生成されている場合は削除して生成
	if (prefabEntity_) {
		prefabEntity_->RemoveComponentAll();
		entityCollection_->RemoveEntityId(prefabEntity_->GetId());
	}

	prefabEntity_ = std::move(entity);


	if (prefabEntity_) {
		prefabEntity_->pEntityComponentSystem_ = this;
		prefabEntity_->id_ = entityCollection_->NewEntityID(true);
		prefabEntity_->CommonInitialize();

		prefabEntity_->SetName(prefabName);
		prefabEntity_->SetPrefabName(_name);

		EntityJsonConverter::FromJson(prefab->GetJson(), prefabEntity_.get());

		prefabEntity_->Initialize();
	}


	return prefabEntity_.get();
}

void EntityComponentSystem::SetMainCamera(Camera* _camera) {
	entityCollection_->SetMainCamera(_camera);
}

void EntityComponentSystem::SetMainCamera(size_t _index) {
	entityCollection_->SetMainCamera(_index);
}

void EntityComponentSystem::SetMainCamera2D(Camera* _camera) {
	entityCollection_->SetMainCamera2D(_camera);
}

void EntityComponentSystem::SetMainCamera2D(size_t _index) {
	entityCollection_->SetMainCamera2D(_index);
}

const std::vector<std::unique_ptr<IEntity>>& EntityComponentSystem::GetEntities() {
	return entityCollection_->GetEntities();
}

IEntity* EntityComponentSystem::GetEntity(size_t _id) {
	/// idを検索
	auto itr = std::find_if(
		entityCollection_->GetEntities().begin(), entityCollection_->GetEntities().end(),
		[_id](const std::unique_ptr<IEntity>& entity) {
			return entity->GetId() == _id;
		}
	);

	if (itr != entityCollection_->GetEntities().end()) {
		return (*itr).get();
	}

	return nullptr;
}

const std::vector<Camera*>& EntityComponentSystem::GetCameras() {
	return entityCollection_->GetCameras();
}

const Camera* EntityComponentSystem::GetMainCamera() const {
	return entityCollection_->GetMainCamera();
}

Camera* EntityComponentSystem::GetMainCamera() {
	return entityCollection_->GetMainCamera();
}

const Camera* EntityComponentSystem::GetMainCamera2D() const {
	return entityCollection_->GetMainCamera2D();
}

Camera* EntityComponentSystem::GetMainCamera2D() {
	return entityCollection_->GetMainCamera2D();
}

const Camera* EntityComponentSystem::GetDebugCamera() const {
	return static_cast<Camera*>(debugCamera_.get());
}

Camera* EntityComponentSystem::GetDebugCamera() {
	return static_cast<Camera*>(debugCamera_.get());
}



/// ====================================================
/// internal methods
/// ====================================================

IEntity* GetEntityById(int32_t _entityId) {
	IEntity* entity = gECS->GetEntity(_entityId);
	if (!entity) {
		/// prefab entityじゃないかチェック
		if (gECS->GetPrefabEntity() && gECS->GetPrefabEntity()->GetId() == _entityId) {
			entity = gECS->GetPrefabEntity();
		}

		/// それでも違ったらエラーを出力
		if (!entity) {
			Console::Log("Entity not found for ID: " + std::to_string(_entityId));
			return nullptr;
		}
	}

	return entity;
}

uint64_t InternalAddComponent(int32_t _entityId, MonoString* _monoTypeName) {
	std::string typeName = mono_string_to_utf8(_monoTypeName);
	IEntity* entity = GetEntityById(_entityId);
	if (!entity) {
		Console::Log("Entity not found for ID: " + std::to_string(_entityId));
		return 0;
	}

	IComponent* component = entity->AddComponent(typeName);
	if (!component) {
		Console::Log("Failed to add component: " + typeName);
		return 0;
	}

	return reinterpret_cast<uint64_t>(component);
}

uint64_t InternalGetComponent(int32_t _entityId, MonoString* _monoTypeName) {
	/// idからentityを取得
	IEntity* entity = GetEntityById(_entityId);
	if (!entity) {
		Console::Log("Entity not found for ID: " + std::to_string(_entityId));
		return 0;
	}

	/// componentを取得
	std::string typeName = mono_string_to_utf8(_monoTypeName);
	IComponent* component = entity->GetComponent(typeName);

	return reinterpret_cast<uint64_t>(component);
}

const char* InternalGetName(int32_t _entityId) {
	IEntity* entity = GetEntityById(_entityId);
	if (!entity) {
		Console::Log("Entity not found for ID: " + std::to_string(_entityId));
		return nullptr;
	}

	return entity->GetName().c_str();
}

void InternalSetName(int32_t _entityId, MonoString* _name) {
	std::string name = mono_string_to_utf8(_name);
	IEntity* entity = GetEntityById(_entityId);

	if (!entity) {
		Console::Log("Entity not found for ID: " + std::to_string(_entityId));
		return;
	}

	entity->SetName(name);
}

int32_t InternalGetChildId(int32_t _entityId, uint32_t _childIndex) {
	IEntity* entity = GetEntityById(_entityId);
	if (!entity) {
		Console::Log("Entity not found for ID: " + std::to_string(_entityId));
		return 0;
	}

	const auto& children = entity->GetChildren();
	if (_childIndex >= children.size()) {
		Console::Log("Child index out of range for entity ID: " + std::to_string(_entityId));
		return 0;
	}

	IEntity* child = children[_childIndex];
	return child->GetId();
}

int32_t InternalGetParentId(int32_t _entityId) {
	IEntity* entity = GetEntityById(_entityId);
	if (!entity) {
		Console::Log("Entity not found for ID: " + std::to_string(_entityId));
		return 0;
	}

	IEntity* parent = entity->GetParent();
	if (parent) {
		return parent->GetId();
	}

	return 0;
}

void InternalSetParent(int32_t _entityId, int32_t _parentId) {
	IEntity* entity = GetEntityById(_entityId);
	IEntity* parent = GetEntityById(_parentId);

	/// nullptr チェック
	if (!entity || !parent) {
		Console::LogError("Entity or parent not found for IDs: " + std::to_string(_entityId) + ", " + std::to_string(_parentId));
		return;
	}

	/// idが同じ場合は何もしない
	if (entity->GetId() == parent->GetId()) {
		Console::LogError("Cannot set entity as its own parent: " + std::to_string(_entityId));
		return;
	}

	/// 既存の親を削除
	if (entity->GetParent()) {
		entity->RemoveParent();
	}

	/// 新しい親を設定
	entity->SetParent(parent);
}

void InternalAddScript(int32_t _entityId, MonoString* _scriptName) {
	std::string scriptName = mono_string_to_utf8(_scriptName);
	IEntity* entity = GetEntityById(_entityId);
	if (!entity) {
		Console::Log("Entity not found for ID: " + std::to_string(_entityId));
		return;
	}

	/// 追加されていなければスクリプトを追加
	Script* script = entity->AddComponent<Script>();
	if (!script->Contains(scriptName)) {
		script->AddScript(scriptName);
	}
}


bool InternalGetScript(int32_t _entityId, MonoString* _scriptName) {
	IEntity* entity = GetEntityById(_entityId);
	if (!entity) {
		Console::LogError("Entity not found for ID: " + std::to_string(_entityId));
		return false;
	}

	Script* script = entity->GetComponent<Script>();
	if (!script) {
		Console::LogError("Script component not found for Entity ID: " + std::to_string(_entityId));
		return false;
	}

	char* cstr = mono_string_to_utf8(_scriptName);
	std::string scriptName(cstr);
	mono_free(cstr);

	if (script->Contains(scriptName)) {
		Console::Log(std::format("Script {} found for Entity ID: {}", scriptName, _entityId));
		return true;
	}

	return false;
}

bool InternalContainsEntity(int32_t _entityId) {
	IEntity* entity = GetEntityById(_entityId);
	if (entity) {
		return true;
	}

	return false;
}

int32_t InternalGetEntityId(MonoString* _name) {
	std::string name = mono_string_to_utf8(_name);
	return gECS->GetEntityId(name);
}

int32_t InternalCreateEntity(MonoString* _name) {
	/// prefabを検索
	std::string name = mono_string_to_utf8(_name);
	IEntity* entity = gECS->GenerateEntityFromPrefab(name + ".prefab");
	if (!entity) {
		entity = gECS->GenerateEntity(name);
		if (!entity) {
			return 0;
		}
	}

	Script* script = entity->GetComponent<Script>();
	if (script) {
		/// スクリプトの初期化を行う
		script->CallAwakeMethodAll();
		script->CallInitMethodAll();
	}

	return entity->GetId();
}

bool InternalContainsPrefabEntity(int32_t _entityId) {
	IEntity* entity = gECS->GetPrefabEntity();
	if (entity) {
		if (entity->GetId() == _entityId) {
			return true;
		}
	}

	return false;
}

void InternalDestroyEntity(int32_t _entityId) {
	IEntity* entity = GetEntityById(_entityId);
	if (!entity) {
		Console::LogError("Entity not found for ID: " + std::to_string(_entityId));
		return;
	}

	gECS->RemoveEntity(entity, true);
	Console::Log("Entity destroyed: " + entity->GetName() + " (ID: " + std::to_string(_entityId) + ")");

}

