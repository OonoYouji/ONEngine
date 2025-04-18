#include "EntityComponentSystem.h"

/// std
#include <numbers>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/Entity/Camera/Camera.h"
#include "../Component/Component.h"
#include "AddECSSystemFunction.h"

IEntity::IEntity() {}

void IEntity::CommonInitialize() {
	transform_ = AddComponent<Transform>();
	variables_ = AddComponent<Variables>();
	std::string name = typeid(*this).name();
	name.erase(0, 6);
	variables_->LoadJson("./Assets/Jsons/" + name + ".json");
}

void IEntity::SetParent(IEntity* _parent) {
	transform_->SetParent(_parent->GetTransform());
}



EntityComponentSystem::EntityComponentSystem(DxManager* _pDxManager) : pDxManager_(_pDxManager) {}
EntityComponentSystem::~EntityComponentSystem() {}

void EntityComponentSystem::Initialize() {

	entities_.reserve(256);

	Camera* debugCamera = GenerateCamera();
	debugCamera->SetPosition(Vector3(0.0f, 20.0f, -25.0f));
	debugCamera->SetRotate(Vector3(std::numbers::pi_v<float> / 5.0f, 0.0f, 0.0f));
	SetDebugCamera(debugCamera);

	AddECSSystemFunction(this, pDxManager_);
}

void EntityComponentSystem::Update() {
	for (auto& entity : entities_) {
		entity->Update();
		entity->transform_->Update();
	}

	for (auto& system : systemMap_) {
		system->Update(this);
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
	std::unique_ptr<Camera> camera = std::make_unique<Camera>(pDxManager_->GetDxDevice());
	camera->pEntityComponentSystem_ = this;
	camera->CommonInitialize();
	camera->Initialize();

	Camera* cameraPtr = camera.get();
	entities_.push_back(std::move(camera));
	cameras_.push_back(cameraPtr);

	return cameraPtr;
}

//Camera2D* EntityComponentSystem::GenerateCamera2D() {
//	std::unique_ptr<Camera2D> camera = std::make_unique<Camera2D>(pDxManager_->GetDxDevice());
//	camera->pEntityComponentSystem_ = this;
//	camera->CommonInitialize();
//	camera->Initialize();
//
//	Camera2D* cameraPtr = camera.get();
//	entities_.push_back(std::move(camera));
//	camera2ds_.push_back(cameraPtr);
//
//	return cameraPtr;
//}
//
