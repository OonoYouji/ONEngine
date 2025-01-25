#include "PointLight.h"

/// external
#include <imgui.h>

/// engine
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"
#include "Objects/Camera/Manager/CameraManager.h"


void PointLight::Initialize() {
#ifdef _DEBUG
	{	/// デバッグ用のアイコンを設定
		SpriteRenderer* renderer = AddComponent<SpriteRenderer>();
		renderer->SetTexture("LightIcon.png");
	}
#endif // _DEBUG



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
#ifdef _DEBUG
	{	/// アイコンのビルボード
		BaseCamera* pCamera = CameraManager::GetInstance()->GetMainCamera();
		if (pCamera) {
			pTransform_->rotate = pCamera->GetRotate();
		}
	}
#endif // _DEBUG



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

void PointLight::SetColor(const Vec4& _color) {
	data_.color = _color;
}

void PointLight::SetIntensity(float _intensity) {
	data_.intensity = _intensity;
}

void PointLight::SetRadius(float _radius) {
	data_.radius = _radius;
}

void PointLight::SetDecay(float _decay) {
	data_.decay = _decay;
}

void PointLight::SetActive(bool _active) {
	data_.active = _active;
}
