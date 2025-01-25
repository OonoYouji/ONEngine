#include "SpotLight.h"

/// std
#include <numbers>

/// external
#include "imgui.h"


SpotLight::SpotLight() {
	CreateTag(this);
}

SpotLight::~SpotLight() {}

void SpotLight::Initialize() {
	data_ = {
		.color     = Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		.position  = Vec3(0.0f, 10.0f, 0.0f),
		.intensity = 1.0f,
		.direction = Vec3(0.0f, -1.0f, 0.0f),
		.distance  = 10.0f,
		.decay     = 2.0f,
		.cosAngle  = std::cos(std::numbers::pi_v<float> / 3.0f),
		.cosFalloffStart = std::cos(std::numbers::pi_v<float> / 3.0f) + 0.25f,
		.active    = true
	};
}

void SpotLight::Update() {
	data_.position  = pTransform_->position;
	data_.direction = data_.direction.Normalize();
}


void SpotLight::Debug() {
	
	bool isActive = data_.active;
	ImGui::Checkbox("active", &isActive);
	data_.active = isActive;

	ImGui::ColorEdit3("color",            &data_.color.x);
	ImGui::DragFloat("intensity",         &data_.intensity,       0.1f,  0.0f, 10.0f);
	ImGui::DragFloat3("direction",        &data_.direction.x,     0.1f);
	ImGui::DragFloat("distance",          &data_.distance,        0.1f,  0.0f, 100.0f);
	ImGui::DragFloat("decay",             &data_.decay,           0.1f,  0.0f, 10.0f);
	ImGui::DragFloat("cos angle",         &data_.cosAngle,        0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("cos falloff start", &data_.cosFalloffStart, 0.01f, 0.0f, 1.0f);

}


void SpotLight::SetColor(const Vec4& _color) {
	data_.color = _color;
}

void SpotLight::SetIntensity(float _intensity) {
	data_.intensity = _intensity;
}

void SpotLight::SetDirection(const Vec3& _direction) {
	data_.direction = _direction;
}

void SpotLight::SetDecay(float _decay) {
	data_.decay = _decay;
}

void SpotLight::SetCosAngle(float _cosAngle) {
	data_.cosAngle = _cosAngle;
}

void SpotLight::SetCosFalloffStart(float _cosFalloffStart) {
	data_.cosFalloffStart = _cosFalloffStart;
}

void SpotLight::SetActive(bool _active) {
	data_.active = _active;
}

