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
	name_ = typeid(*this).name();
	name_.erase(0, 6);
	variables_->LoadJson("./Assets/Jsons/" + name_ + ".json");
}

void IEntity::UpdateTransform() {
	transform_->matWorld = Matrix4x4::MakeAffine(transform_->scale, transform_->rotate, transform_->position);

	if (parent_) {

		if ((transform_->matrixCalcFlags & Transform::kAll) == Transform::kAll) {
			transform_->matWorld *= parent_->transform_->GetMatWorld();
			return;
		}

		Matrix4x4 matCancel = Matrix4x4::kIdentity;
		if (transform_->matrixCalcFlags & Transform::kScale) {
			matCancel = Matrix4x4::MakeScale(parent_->transform_->scale);
		}

		if (transform_->matrixCalcFlags & Transform::kRotate) {
			matCancel *= Matrix4x4::MakeRotate(parent_->transform_->rotate);
		}

		if (transform_->matrixCalcFlags & Transform::kPosition) {
			matCancel *= Matrix4x4::MakeTranslate(parent_->transform_->position);
		}

		transform_->matWorld *= matCancel;
	}
}

void IEntity::SetParent(IEntity* _parent) {
	_parent->children_.push_back(this);
	parent_ = _parent;
}

void IEntity::RemoveParent() {
	if (parent_) {
		auto itr = std::remove_if(parent_->children_.begin(), parent_->children_.end(),
			[this](IEntity* child) {
				return child == this;
			}
		);
		parent_->children_.erase(itr, parent_->children_.end());
		parent_ = nullptr;
	}
}

Vector3 IEntity::GetWorldPosition() {
	return Matrix4x4::Transform(GetLocalPosition(), transform_->GetMatWorld());
}

Vector3 IEntity::GetWorldRotate() {
	if (!parent_) {
		return transform_->rotate;
	}

	// 親のワールド行列を取得  
	//const Matrix4x4& parentWorldMatrix = parent_->transform_->GetMatWorld();

	// 親の回転を抽出  
	//Vector3 parentRotation = parent_->GetWorldRotate();

	// 自身のローカル回転を加算  
	return parent_->GetWorldRotate() + transform_->rotate;
}

Vector3 IEntity::GetWorldScale() {
	return Vector3();
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
		entity->UpdateTransform();
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
