#include "DirectionalLight.h"

#include <DxResourceCreator.h>

#include <ImGuiManager.h>


void DirectionalLight::Initialize() {
	constantBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(BufferData));
	constantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&bufferData_));

	bufferData_->color = Vec4(1, 1, 1, 1);
	bufferData_->direction = Vec3(-0.98f, 0.0f, 0.195f).Normalize();
	bufferData_->intencity = 1.0f;
}

void DirectionalLight::Update() {
	bufferData_->direction = bufferData_->direction.Normalize();
}

void DirectionalLight::Debug() {
	if(ImGui::TreeNodeEx("buffer", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::ColorEdit3("color", &bufferData_->color.x);
		ImGui::DragFloat3("direction", &bufferData_->direction.x, 0.01f);
		ImGui::DragFloat("intencity", &bufferData_->intencity, 0.01f);

		ImGui::TreePop();
	}
}



void DirectionalLight::BindToCommandList(UINT rootParamIndex, ID3D12GraphicsCommandList* commandList) {
	commandList->SetGraphicsRootConstantBufferView(rootParamIndex, constantBuffer_->GetGPUVirtualAddress());
}

void DirectionalLight::SetDirection(const Vec3& direction) {
	bufferData_->direction = direction;
}
