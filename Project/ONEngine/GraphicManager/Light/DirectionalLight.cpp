#include "DirectionalLight.h"

/// engine
#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"
#include "ImGuiManager/ImGuiManager.h"
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"
#include "Objects/Camera/Manager/CameraManager.h"


void DirectionalLight::Initialize() {
#ifdef _DEBUG
	{	/// デバッグ用のアイコンを設定
		SpriteRenderer* renderer = AddComponent<SpriteRenderer>();
		renderer->SetTexture("LightIcon.png");
	}
#endif // _DEBUG


	data_ = {
		.color     = Vec4(1, 1, 1, 1),
		.direction = Vec3(-0.98f, 0.0f, 0.195f).Normalize(),
		.intensity = 1.0f,
		.active    = true
	};
}

void DirectionalLight::Update() {
#ifdef _DEBUG
	{	/// アイコンのビルボード
		BaseCamera* pCamera = CameraManager::GetInstance()->GetMainCamera();
		if (pCamera) {
			pTransform_->rotate = pCamera->GetRotate();
		}
	}
#endif // _DEBUG



	data_.direction = data_.direction.Normalize();
}

void DirectionalLight::Debug() {
	ImGui::ColorEdit3("color",     &data_.color.x);
	ImGui::DragFloat3("direction", &data_.direction.x, 0.01f);
	ImGui::DragFloat("intensity",  &data_.intensity,   0.01f, 0.0f, 100.0f);
}

void DirectionalLight::SetColor(const Vec4& _color) {
	data_.color = _color;
}

void DirectionalLight::SetDirection(const Vec3& _direction) {
	data_.direction = _direction;
}

void DirectionalLight::SetIntensity(float _intensity) {
	data_.intensity = _intensity;
}