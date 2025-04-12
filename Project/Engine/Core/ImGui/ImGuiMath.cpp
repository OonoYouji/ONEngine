#include "ImGuiMath.h"

/// std
#include <numbers>

/// engine
#include "Engine/ECS/Component/Component.h"


namespace {

	float rotateSpeed = std::numbers::pi_v<float> / 100.0f;

}	/// unnamed namespace


void TransformDebug(Transform* _transform) {
	if (!_transform) {
		return;
	}

	ImGui::DragFloat3("position", &_transform->position.x, 0.1f);
	ImGui::DragFloat3("rotate",   &_transform->rotate.x,   rotateSpeed);
	ImGui::DragFloat3("scale",    &_transform->scale.x,    0.1f);
}

void DirectionalLightDebug(DirectionalLight* _light) {
	if (!_light) {
		return;
	}

	/// param get
	float intensity = _light->GetIntensity();
	Vec4  color     = _light->GetColor();
	Vec3  direction = _light->GetDirection();
	
	/// edit
	if (ImGui::ColorEdit4("color", &color.x)) {
		_light->SetColor(color);
	}

	if (ImGui::DragFloat3("direction", &direction.x, 0.1f)) {
		_light->SetDirection(Vec3::Normalize(direction));
	}

	if (ImGui::DragFloat("intensity", &intensity, 0.1f)) {
		_light->SetIntensity(intensity);
	}
	
}
