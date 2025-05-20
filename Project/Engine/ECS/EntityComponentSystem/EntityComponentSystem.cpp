#include "EntityComponentSystem.h"

/// std
#include <numbers>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/Entity/Camera/Camera.h"
#include "Engine/ECS/Entity/Camera/DebugCamera.h"
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

void IEntity::SetPosition(const Vector3& _v) {
	transform_->position = _v;
}

void IEntity::SetPositionX(float _x) {
	transform_->position.x = _x;
}

void IEntity::SetPositionY(float _y) {
	transform_->position.y = _y;
}

void IEntity::SetPositionZ(float _z) {
	transform_->position.z = _z;
}

void IEntity::SetRotate(const Vector3& _v) {
	transform_->rotate = _v;
}

void IEntity::SetRotateX(float _x) {
	transform_->rotate.x = _x;
}

void IEntity::SetRotateY(float _y) {
	transform_->rotate.y = _y;
}

void IEntity::SetRotateZ(float _z) {
	transform_->rotate.z = _z;
}

void IEntity::SetScale(const Vector3& _v) {
	transform_->scale = _v;
}

void IEntity::SetScaleX(float _x) {
	transform_->scale.x = _x;
}

void IEntity::SetScaleY(float _y) {
	transform_->scale.y = _y;
}

void IEntity::SetScaleZ(float _z) {
	transform_->scale.z = _z;
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

const Vector3& IEntity::GetLocalPosition() const {
	return transform_->position;
}

const Vector3& IEntity::GetLocalRotate() const {
	return transform_->rotate;
}

const Vector3& IEntity::GetLocalScale() const {
	return transform_->scale;
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

Transform* IEntity::GetTransform() const {
	return transform_;
}

const IEntity* IEntity::GetParent() const {
	return parent_;
}

IEntity* IEntity::GetParent() {
	return parent_;
}

const std::vector<IEntity*>& IEntity::GetChildren() const {
	return children_;
}

IEntity* IEntity::GetChild(size_t _index) {
	return children_[_index];
}

const std::unordered_map<size_t, IComponent*>& IEntity::GetComponents() const {
	return components_;
}

const std::string& IEntity::GetName() const {
	return name_;
}



EntityComponentSystem::EntityComponentSystem(DxManager* _pDxManager) : pDxManager_(_pDxManager) {}
EntityComponentSystem::~EntityComponentSystem() {}

void EntityComponentSystem::Initialize() {

	pDxDevice_ = pDxManager_->GetDxDevice();

	entities_.reserve(256);

	DebugCamera* debugCamera = GenerateCamera<DebugCamera>();
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

void EntityComponentSystem::SetMainCamera(Camera* _camera) {
	mainCamera_ = _camera;
}

void EntityComponentSystem::SetMainCamera(size_t _index) {
	mainCamera_ = cameras_[_index];
}

void EntityComponentSystem::SetMainCamera2D(Camera* _camera) {
	mainCamera2D_ = _camera;
}

void EntityComponentSystem::SetMainCamera2D(size_t _index) {
	mainCamera2D_ = cameras_[_index];
}

void EntityComponentSystem::SetDebugCamera(Camera* _camera) {
	debugCamera_ = _camera;
}

void EntityComponentSystem::SetDebugCamera(size_t _index) {
	debugCamera_ = cameras_[_index];
}

const std::vector<std::unique_ptr<IEntity>>& EntityComponentSystem::GetEntities() {
	return entities_;
}

const std::vector<Camera*>& EntityComponentSystem::GetCameras() {
	return cameras_;
}

const Camera* EntityComponentSystem::GetMainCamera() const {
	return mainCamera_;
}

Camera* EntityComponentSystem::GetMainCamera() {
	return mainCamera_;
}

const Camera* EntityComponentSystem::GetMainCamera2D() const {
	return mainCamera2D_;
}

Camera* EntityComponentSystem::GetMainCamera2D() {
	return mainCamera2D_;
}

const Camera* EntityComponentSystem::GetDebugCamera() const {
	return debugCamera_;
}

Camera* EntityComponentSystem::GetDebugCamera() {
	return debugCamera_;
}

