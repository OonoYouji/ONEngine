#include "CapsuleCollider.h"

/// std
#include <format>

/// externals
#include <imgui.h>

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "GameObjectManager/BaseGameObject.h"

CapsuleCollider::CapsuleCollider() {}
CapsuleCollider::~CapsuleCollider() {}


void CapsuleCollider::Initialize() {

	/// コライダーの形状
	colliderType_ = COLLIDER_TYPE_CAPSULE;

	transform_.reset(new Transform());
	transform_->Initialize();
	transform_->SetName("Transform" + std::format("##{:p}", reinterpret_cast<void*>(transform_.get())));
	transform_->rotateOrder = QUATERNION;
	transform_->scale = {
		1.0f, capsuleScale_, capsuleScale_
	};

	for(auto& transform : transformArray_) {
		transform.Initialize();
		transform.SetName("Transform" + std::format("##{:p}", reinterpret_cast<void*>(&transform)));

		transform.scale = Vec3::kOne * capsuleScale_;
	}

	sphere_ = ModelManager::Load("Sphere");
	tube_   = ModelManager::Load("Tube");
}

void CapsuleCollider::Update() {
	
	/// カプセルの端から端までの方向
	direction_ = transformArray_[1].position - transformArray_[0].position;
	lenght_    = direction_.Len();
	direction_ = direction_.Normalize();

	/// 端から端の中心に置く
	transform_->position = (transformArray_[1].position + transformArray_[0].position) * 0.5f;

	/// 回転の計算
	transform_->quaternion = Quaternion::LockAt(
		transformArray_[1].position, transformArray_[0].position
	);

	/// Scaleの計算
	transform_->scale = {
		capsuleScale_, capsuleScale_, 
		lenght_ * 0.5f
	};

	UpdateMatrix();
	for(size_t i = 0; i < 2; ++i) {
		Transform* transform = &transformArray_[i];
		transform->position  = *positionArray_[i];
		transform->scale     = Vec3::kOne * capsuleScale_;

		transform->Update();
		transform->matTransform *= GetOwner()->GetMatTransform();
	}

}

void CapsuleCollider::Draw() {

	tube_->Draw(transform_.get(), kWireFrame);
	for(auto& transform : transformArray_) {
		sphere_->Draw(&transform, kWireFrame);
	}
}

void CapsuleCollider::Debug() {
	if(ImGui::TreeNodeEx(GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

		transform_->Debug();

		ImGui::DragFloat("capsule scale", &capsuleScale_, 0.01f);
		
		ImGui::TreePop();
	}
}



void CapsuleCollider::CreateCollider(const Model* model) {}

void CapsuleCollider::SetPositionArray(const std::array<Vec3*, 2>& _positionArray) {
	positionArray_ = _positionArray;
}


