#include "DirectionalLightObject.h"

/// engine
#include "Engine/ECS/Component/Component.h"

void DirectionalLightObject::Initialize() {
	/// DirectionalLightの生成
	DirectionalLight* directionalLight = AddComponent<DirectionalLight>();
	directionalLight->SetColor(Color::kWhite);
	directionalLight->SetDirection(Vector3(0.0f, -1.0f, 0.0f));
	directionalLight->SetIntensity(1.0f);
}

void DirectionalLightObject::Update() {

	Vector3& direction = variables_->Get<Vector3>("direction");
	direction = Vector3::Normalize(direction);

	/// DirectionalLightの更新
	if (DirectionalLight* directionalLight = GetComponent<DirectionalLight>()) {
		directionalLight->SetDirection(direction);
	}
}