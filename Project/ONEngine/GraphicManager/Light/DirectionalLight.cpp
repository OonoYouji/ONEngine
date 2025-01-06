#include "DirectionalLight.h"

#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"
#include "ImGuiManager/ImGuiManager.h"


void DirectionalLight::Initialize() {
	data_ = {
		.color     = Vec4(1, 1, 1, 1),
		.direction = Vec3(-0.98f, 0.0f, 0.195f).Normalize(),
		.intencity = 1.0f,
		.active    = true
	};
}

void DirectionalLight::Update() {
	data_.direction = data_.direction.Normalize();
}

void DirectionalLight::Debug() {
	if(ImGui::TreeNodeEx("buffer", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::ColorEdit3("color", &data_.color.x);
		ImGui::DragFloat3("direction", &data_.direction.x, 0.01f);
		ImGui::DragFloat("intencity", &data_.intencity, 0.01f);

		ImGui::TreePop();
	}
}


void DirectionalLight::SetDirection(const Vec3& direction) {
	data_.direction = direction;
}
