#include "CapsuleCollider.h"

/// externals
#include <imgui.h>

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "GameObjectManager/BaseGameObject.h"

CapsuleCollider::CapsuleCollider() {}
CapsuleCollider::~CapsuleCollider() {}


void CapsuleCollider::Initialize() {

	transform_.reset(new Transform());
	transform_->Initialize();
	transform_->rotateOrder = QUATERNION;

	sphere_ = ModelManager::Load("Sphere");
	tube_   = ModelManager::Load("Tube");
}

void CapsuleCollider::Update() {
	
	/// カプセルの端から端までの方向
	direction_ = transformArray_[1]->position - transformArray_[0]->position;
	lenght_    = direction_.Len();
	direction_ = direction_.Normalize();

	/// 端から端の中心に置く
	transform_->position = (transformArray_[1]->position + transformArray_[0]->position) * 0.5f;

	/// 回転の計算
	transform_->quaternion = Quaternion::LockAt(
		transform_->position, direction_ * (lenght_),
		upDirection_.Normalize()
	);

	/// Scaleの計算
	transform_->scale.z = lenght_ * 0.5f;


	UpdateMatrix();
	for(auto& transform : transformArray_) {
		transform->Update();
		transform->matTransform *= GetOwner()->GetMatTransform();
	}
}

void CapsuleCollider::Draw() {

	tube_->Draw(transform_.get(), kWireFrame);
	for(auto& transform : transformArray_) {
		sphere_->Draw(transform, kWireFrame);
	}
}

void CapsuleCollider::Debug() {
	if(ImGui::TreeNodeEx(GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat3("up direction", &upDirection_.x, 0.01f);
		if(ImGui::IsItemEdited()) {
			upDirection_ = upDirection_.Normalize();
		}

		ImGui::TreePop();
	}
}



void CapsuleCollider::CreateCollider(const Model* model) {}

void CapsuleCollider::SetTransformArray(const std::array<Transform*, 2>& _transformArray) {
	transformArray_ = _transformArray;
}

