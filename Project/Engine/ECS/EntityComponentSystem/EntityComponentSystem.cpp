#include "EntityComponentSystem.h"

/// std
#include <numbers>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/Entity/Camera/Camera.h"

IEntity::IEntity() {}

void IEntity::CommonInitialize() {
	transform_ = AddComponent<Transform>();
}

void IEntity::SetParent(IEntity* _parent) {
	transform_->SetParent(_parent->GetTransform());
}



EntityComponentSystem::EntityComponentSystem(DxManager* _pDxManager) : pDxManager_(_pDxManager) {}
EntityComponentSystem::~EntityComponentSystem() {}

void EntityComponentSystem::Initialize() {

	entities_.reserve(256);
	
	Camera* mainCamera = GenerateCamera();
	mainCamera->SetPosition(Vector3(0.0f, 20.0f, -25.0f));
	mainCamera->SetRotate(Vector3(std::numbers::pi_v<float> / 5.0f, 0.0f, 0.0f));
	SetMainCamera(mainCamera);
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
	std::unique_ptr<Camera> camera = std::make_unique<Camera>(pDxManager_->GetDxDevice());
	camera->pEntityComponentSystem_ = this;
	camera->CommonInitialize();
	camera->Initialize();

	Camera* cameraPtr = camera.get();
	entities_.push_back(std::move(camera));
	cameras_.push_back(cameraPtr);

	return cameraPtr;
}

Camera2D* EntityComponentSystem::GenerateCamera2D() {
	std::unique_ptr<Camera2D> camera = std::make_unique<Camera2D>(pDxManager_->GetDxDevice());
	camera->pEntityComponentSystem_ = this;
	camera->CommonInitialize();
	camera->Initialize();

	Camera2D* cameraPtr = camera.get();
	entities_.push_back(std::move(camera));
	camera2ds_.push_back(cameraPtr);

	return cameraPtr;
}
