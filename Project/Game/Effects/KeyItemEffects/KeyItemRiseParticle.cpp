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

	effect->SetEmitTypeTime({ .emitTime = 0.1f, .emitInterval = 0.0f }, 2);
	effect->SetLifeLeftTime(1.0f);
	effect->SetStartSpeed(0.0f);
	effect->SetMaxEffectCount(64);
	effect->SetBlendMode(Effect::BlendMode::Add);
	effect->SetEmitShape({ 0, 0, 0 }, Vec3::kOne * 10.0f);
	effect->SetColorStartType(Effect::StartData::Constant);
	effect->SetColorStart(Color(0xF8650AFF));


	effect->SetElementUpdateFunc(
		[](Effect::Element* _element) {
			float lerpT = _element->lifeTime / 2.0f;
			_element->color.a = std::lerp(0.0f, 1.0f, lerpT);
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