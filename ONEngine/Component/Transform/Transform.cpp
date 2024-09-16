#include "Transform.h"

#include <ImGuiManager.h>

#include <DxResourceCreator.h>

/// ===================================================
/// 初期化
/// ===================================================
void Transform::Initialize() {
	transformBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Mat4));
	transformBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mapingData_));
	UpdateMatrix();
}

void Transform::Update() {
	UpdateMatrix();
}


void Transform::Debug() {
	if(ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat3("position",	&position.x,	0.1f);
		ImGui::DragFloat3("rotate",	&rotate.x,	0.05f);
		ImGui::DragFloat3("scale",		&scale.x,	0.1f);

		ImGui::TreePop();
	}
}


/// ===================================================
/// 行列の更新
/// ===================================================
void Transform::UpdateMatrix() {
	matTransform = Mat4::MakeAffine(scale, rotate, position);
	*mapingData_ = matTransform;
}

void Transform::BindTransform(ID3D12GraphicsCommandList* commandList, UINT rootParamIndex) {
	*mapingData_ = matTransform;
	commandList->SetGraphicsRootConstantBufferView(rootParamIndex, transformBuffer_->GetGPUVirtualAddress());
}
