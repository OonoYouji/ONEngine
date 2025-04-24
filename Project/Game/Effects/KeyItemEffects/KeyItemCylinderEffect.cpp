#include "KeyItemCylinderEffect.h"

/// std
#include <numbers>

/// engine
#include "Engine/ECS/Component/Component.h"
#include "Engine/Core/Utility/Utility.h"

KeyItemCylinderEffect::KeyItemCylinderEffect() {}
KeyItemCylinderEffect::~KeyItemCylinderEffect() {}

void KeyItemCylinderEffect::Initialize() {

	Effect* effect = AddComponent<Effect>();
	effect->SetEmitShape(Vec3::kZero, 1.0f); // sphereで設定

	effect->SetMeshPath("Assets/Models/primitive/tube.obj");
	effect->SetTexturePath("Assets/Textures/gradationLine.png");

	effect->SetLifeLeftTime(0.5f);
	effect->SetStartColor(Color(1, 1, 1, 1.1f));
	effect->SetStartSize(Vec3::kOne * 5.0f, Vec3::kOne * 10.0f);
	effect->SetEmitTypeTime({ .emitTime = 0.1f, .emitInterval = 0.0f }, 2);

	effect->SetMaxEffectCount(64);
	particleElements_.resize(64);
	for (auto& element : particleElements_) {
		element.rotateSpeed = Random::Float(0.0f, 2.0f * std::numbers::pi_v<float>);
	}

	effect->SetElementUpdateFunc(
		[](Effect::Element* _element) {
			_element->transform.rotate.y += Time::DeltaTime() * 3.0f;
			_element->transform.position += Vec3::kUp * Time::DeltaTime();
		}
	);

}

void KeyItemCylinderEffect::Update() {
	// Update code here
}