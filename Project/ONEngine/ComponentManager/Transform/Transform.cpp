#include "Transform.h"

#include "ImGuiManager/ImGuiManager.h"

#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"

#include "GameObjectManager/BaseGameObject.h"

/// ===================================================
/// 初期化
/// ===================================================
void Transform::Initialize() {
	transformBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Mat4));
	transformBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mapingData_));
	//UpdateMatrix();
}

void Transform::Update() {
	UpdateMatrix();
}


void Transform::Debug() {
	if(ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat3("position",	&position.x,	0.1f);

		if(rotateOrder != QUATERNION) {
			ImGui::DragFloat3("rotate",		&rotate.x,		0.05f);
		} else {
			ImGui::DragFloat4("quaternion",	&quaternion.x,	0.05f);
		}

		ImGui::DragFloat3("scale",		&scale.x,		0.1f);

		ImGui::Separator();


		const char* rotateOrderItems[] = {
			"XZY", "XYZ",
			"YXZ", "YZX",
			"ZYX", "ZXY",
			"QUATERNION"
		};

		int currentIndex = static_cast<int>(rotateOrder);
		if(ImGui::Combo("Rotation Order", &currentIndex, rotateOrderItems, IM_ARRAYSIZE(rotateOrderItems))) {
			rotateOrder  = static_cast<ROTATE_ORDER>(currentIndex);
		}


		UpdateMatrix();
		ImGui::TreePop();
	}
}


/// ===================================================
/// 行列の更新
/// ===================================================
void Transform::UpdateMatrix() {
	matTransform =
		Mat4::MakeScale(scale) *
		MakeRotate(rotateOrder) *
		Mat4::MakeTranslate(position);

	Transform* parent = GetParent();
	if(parent) {
		matTransform *= parent->matTransform;
	}

	*mapingData_ = matTransform;
}

void Transform::BindTransform(ID3D12GraphicsCommandList* commandList, UINT rootParamIndex) {
	*mapingData_ = matTransform;
	commandList->SetGraphicsRootConstantBufferView(rootParamIndex, transformBuffer_->GetGPUVirtualAddress());
}

void Transform::SetParent(Transform* parent) {
	parent_ = parent;
	if (parent) {
		parent->AddChild(this);
	}
}

void Transform::AddChild(Transform* child) {
	childs_.push_back(child);
}

Mat4 Transform::MakeRotate(uint32_t order) {
	switch(order) {
	case ROTATE_ORDER::XYZ:
		return Mat4::MakeRotateX(rotate.x) * Mat4::MakeRotateY(rotate.y) * Mat4::MakeRotateZ(rotate.z);
	case ROTATE_ORDER::XZY:
		return Mat4::MakeRotateX(rotate.x) * Mat4::MakeRotateZ(rotate.z) * Mat4::MakeRotateY(rotate.y);
	case ROTATE_ORDER::YXZ:
		return Mat4::MakeRotateY(rotate.y) * Mat4::MakeRotateX(rotate.x) * Mat4::MakeRotateZ(rotate.z);
	case ROTATE_ORDER::YZX:
		return Mat4::MakeRotateY(rotate.y) * Mat4::MakeRotateZ(rotate.z) * Mat4::MakeRotateX(rotate.x);
	case ROTATE_ORDER::ZXY:
		return Mat4::MakeRotateZ(rotate.z) * Mat4::MakeRotateX(rotate.x) * Mat4::MakeRotateY(rotate.y);
	case ROTATE_ORDER::ZYX:
		return Mat4::MakeRotateZ(rotate.z) * Mat4::MakeRotateY(rotate.y) * Mat4::MakeRotateX(rotate.x);
	case ROTATE_ORDER::QUATERNION:
		quaternion = Quaternion::Normalize(quaternion);
		return Mat4::MakeRotateQuaternion(quaternion);
	}
	return Mat4::MakeRotate(rotate);
}
