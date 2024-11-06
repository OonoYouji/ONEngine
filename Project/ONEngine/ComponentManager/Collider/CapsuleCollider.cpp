#include "CapsuleCollider.h"

/// std
#include <format>
#include <numbers>

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

	/// 当たり判定の可視化用
	sphere_ = ModelManager::Load("Sphere");
	tube_   = ModelManager::Load("Tube");

	transform_.reset(new Transform());
	transform_->Initialize();
	transform_->SetName("Transform" + std::format("##{:p}", reinterpret_cast<void*>(transform_.get())));
	transform_->rotateOrder = QUATERNION;
	transform_->scale = {
		1.0f, radius_, radius_
	};

	for(auto& transform : transformArray_) {
		transform.Initialize();
		transform.SetName("Transform" + std::format("##{:p}", reinterpret_cast<void*>(&transform)));

		transform.scale = Vec3::kOne * radius_;
	}

	/// quaternionを正常に計算するためのデフォルト値
	defaultYQuaternion_ = Quaternion::MakeFromAxis(Vec3::kUp, 0.5f * std::numbers::pi_v<float>);

}

void CapsuleCollider::Update() {
	
	/// カプセルの端から端までの方向
	direction_ = GetEndPosition() - GetStartPosition();
	lenght_    = direction_.Len();
	direction_ = direction_.Normalize();

	/// 端から端の中心に置く
	transform_->position = (transformArray_[1].position + transformArray_[0].position) * 0.5f;

	/// 回転の計算
	transform_->quaternion = defaultYQuaternion_ * Quaternion::LockAt(
		transformArray_[1].position, transformArray_[0].position
	);

	/// Scaleの計算
	transform_->scale = {
		radius_, radius_, 
		lenght_ * 0.5f
	};

	UpdateMatrix();
	for(size_t i = 0; i < 2; ++i) {
		Transform* transform = &transformArray_[i];
		transform->position  = *positionArray_[i];
		transform->scale     = Vec3::kOne * radius_;

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

		ImGui::Text(std::format("direction : {:.2f}, {:.2f}, {:.2f}", direction_.x, direction_.y, direction_.z).c_str());
		ImGui::Spacing();
		ImGui::Text(std::format("start position : {:.2f}, {:.2f}, {:.2f}", startPosition.x, startPosition.y, startPosition.z).c_str());
		ImGui::Text(std::format("end   position : {:.2f}, {:.2f}, {:.2f}", endPosition_.x, endPosition_.y, endPosition_.z).c_str());
		ImGui::DragFloat("radius", &radius_, 0.01f);
		
		ImGui::TreePop();
	}
}



void CapsuleCollider::CreateCollider(const Model* model) {}

void CapsuleCollider::SetPositionArray(const std::array<Vec3*, 2>& _positionArray) {
	positionArray_ = _positionArray;
}

Vec3 CapsuleCollider::GetStartPosition() {
	startPosition = Mat4::Transform(*positionArray_[0], transform_->matTransform);
	return startPosition;
}

Vec3 CapsuleCollider::GetEndPosition() {
	endPosition_ = Mat4::Transform(*positionArray_[1], transform_->matTransform);
	return endPosition_;
}

void CapsuleCollider::SetRadius(float _radius) {
	radius_ = _radius;
}


