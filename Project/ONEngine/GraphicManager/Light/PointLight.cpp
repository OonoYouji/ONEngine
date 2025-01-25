#include "PointLight.h"

/// external
#include <imgui.h>

void PointLight::Initialize() {

	data_ = {
		.color     = Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		.position  = Vec3(0.0f, 0.0f, 0.0f),
		.intensity = 1.0f,
		.radius    = 10.0f,
		.decay     = 1.0f,
		.active    = true
	};


}

void PointLight::Update() {

	data_.position = pTransform_->position;

}

void PointLight::Debug() {

	bool active = data_.active;
	ImGui::Checkbox("active", &active);
	data_.active = active;

	ImGui::ColorEdit3("color",    &data_.color.x);
	ImGui::DragFloat("intensity", &data_.intensity, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("radius",    &data_.radius,    0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("decay",     &data_.decay,     0.01f, 0.0f, 10.0f);

}
