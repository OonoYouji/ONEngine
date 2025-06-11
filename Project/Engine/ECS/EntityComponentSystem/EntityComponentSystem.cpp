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

IEntity* EntityComponentSystem::GenerateEntity(const std::string& _name) {
	return entityCollection_->GenerateEntity(_name);
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

