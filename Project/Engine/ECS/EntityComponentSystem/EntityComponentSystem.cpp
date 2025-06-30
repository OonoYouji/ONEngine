#include "EntityComponentSystem.h"

/// std
#include <numbers>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Editor/EditorManager.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentEditCommands.h"

/// ecs
#include "../Component/Component.h"
#include "AddECSSystemFunction.h"
#include "AddECSComponentFactoryFunction.h"

#include "Engine/ECS/Entity/Entities/Camera/DebugCamera.h"

namespace {
	EntityComponentSystem* gECS = nullptr;
}

void SetEntityComponentSystemPtr(EntityComponentSystem* _ecs) {
	gECS = _ecs;
}

EntityComponentSystem* GetEntityComponentSystemPtr() {
	return gECS;
}

uint64_t InternalAddComponent(uint32_t _entityId, MonoString* _monoTypeName) {
	std::string typeName = mono_string_to_utf8(_monoTypeName);
	IEntity* entity = gECS->GetEntity(_entityId);
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

uint64_t InternalGetComponent(uint32_t _entityId, MonoString* _monoTypeName) {
	/// idからentityを取得
	IEntity* entity = gECS->GetEntity(_entityId);
	if (!entity) {
		Console::Log("Entity not found for ID: " + std::to_string(_entityId));
		return 0;
	}

	/// componentを取得
	std::string typeName = mono_string_to_utf8(_monoTypeName);
	IComponent* component = entity->GetComponent(typeName);

	return reinterpret_cast<uint64_t>(component);
}

MonoString* InternalGetName(uint32_t _entityId) {
	IEntity* entity = gECS->GetEntity(_entityId);
	if (!entity) {
		Console::Log("Entity not found for ID: " + std::to_string(_entityId));
		return nullptr;
	}

	std::string name = entity->GetName();
	return mono_string_new(mono_domain_get(), name.c_str());
}

void InternalSetName(uint32_t _entityId, MonoString* _name) {
	std::string name = mono_string_to_utf8(_name);
	IEntity* entity = gECS->GetEntity(_entityId);

	if (!entity) {
		Console::Log("Entity not found for ID: " + std::to_string(_entityId));
		return;
	}

	entity->SetName(name);
}

uint32_t InternalGetChildId(uint32_t _entityId, uint32_t _childIndex) {
	IEntity* entity = gECS->GetEntity(_entityId);
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
	return static_cast<uint32_t>(child->GetId());
}

bool InternalContainsEntity(uint32_t _entityId) {
	IEntity* entity = gECS->GetEntity(_entityId);
	if (entity) {
		return true;
	}

	return false;
}


EntityComponentSystem::EntityComponentSystem(DxManager* _pDxManager)
	: pDxManager_(_pDxManager) {}
EntityComponentSystem::~EntityComponentSystem() {}

void EntityComponentSystem::Initialize() {

	pDxDevice_ = pDxManager_->GetDxDevice();

	entityCollection_ = std::make_unique<EntityCollection>(this, pDxManager_);
	componentCollection_ = std::make_unique<ComponentCollection>();


	AddECSSystemFunction(this, pDxManager_);
	AddComponentFactoryFunction(componentCollection_.get());

	DebugCamera* debugCamera = GenerateCamera<DebugCamera>();
	debugCamera->SetPosition(Vector3(0.0f, 20.0f, -25.0f));
	debugCamera->SetRotate(Vector3(std::numbers::pi_v<float> / 5.0f, 0.0f, 0.0f));
	SetDebugCamera(debugCamera);

}


void EntityComponentSystem::Update() {

	entityCollection_->UpdateEntities();

	for (auto& system : systemMap_) {
		system->Update(this);
	}
}

IEntity* EntityComponentSystem::GenerateEntity(const std::string& _name, bool _isInit) {
	return entityCollection_->GenerateEntity(_name, _isInit);
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

void EntityComponentSystem::SetDebugCamera(Camera* _camera) {
	entityCollection_->SetDebugCamera(_camera);
}

void EntityComponentSystem::SetDebugCamera(size_t _index) {
	entityCollection_->SetDebugCamera(_index);
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
	return entityCollection_->GetDebugCamera();
}

Camera* EntityComponentSystem::GetDebugCamera() {
	return entityCollection_->GetDebugCamera();
}

