#include "KeyItemRiseParticle.h"

/// engine
#include "Engine/ECS/Component/Component.h"
#include "Engine/Core/Utility/Utility.h"

KeyItemRiseParticle::KeyItemRiseParticle() {}
KeyItemRiseParticle::~KeyItemRiseParticle() {}

void KeyItemRiseParticle::Initialize() {

	Effect* effect = AddComponent<Effect>();
	effect->SetMeshPath("Assets/Models/primitive/frontToPlane.obj");
	effect->SetTexturePath("Assets/Textures/circle.png");

	effect->SetEmitTypeTime({ .emitTime = 0.2f, .emitInterval = 0.0f }, 4);
	effect->SetLifeLeftTime(2.0f);
	effect->SetStartSpeed(0.0f);
	effect->SetMaxEffectCount(64);
	effect->SetEmittedElementColor(Vector4(1.0f, 0x66 / 255.0f, 0.0f, 1.0f));
	effect->SetBlendMode(Effect::BlendMode::Add);
	effect->SetEmitShape({ 0, 0, 0 }, Vec3::kOne * 3.0f);

	effect->SetElementUpdateFunc(
		[](Effect::Element* _element) {
			float lerpT = _element->lifeTime / 2.0f;
			_element->color.w = std::lerp(0.0f, 1.0f, lerpT);
			_element->transform.scale = {
				0.5f, 5.0f, 0.5f
			};

			_element->transform.position.y += 3.0f + 1.0f * Time::DeltaTime();
			_element->transform.Update();
		}
	);

}

void KeyItemRiseParticle::Update() {

}