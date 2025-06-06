#include "KeyItemRipplesEffect.h"

/// engine
#include "Engine/ECS/Component/Component.h"
#include "Engine/Core/Utility/Utility.h"

KeyItemRipplesEffect::KeyItemRipplesEffect() {}
KeyItemRipplesEffect::~KeyItemRipplesEffect() {}

void KeyItemRipplesEffect::Initialize() {
	Effect* effect = AddComponent<Effect>();
	effect->SetMeshPath("Assets/Models/primitive/plane.obj");
	effect->SetTexturePath("Assets/Textures/ring.png");

	effect->SetEmitTypeTime({ .emitTime = 0.5f, .emitInterval = 0.0f }, 6);
	effect->SetLifeLeftTime(2.0f);
	effect->SetStartSpeed(0.0f);
	effect->SetMaxEffectCount(64);
	effect->SetStartColor(Color(0.5f, 1.0f, 1.0f, 0.2f), Color(1.0f, 1.0f, 1.0f, 0.3f));
	effect->SetEmitShape({ 0, 0, 0 }, Vec3(2.0f, 0.0f, 2.0f));
	effect->SetBlendMode(Effect::BlendMode::Add);

	effect->SetElementUpdateFunc(
		[](Effect::Element* _element) {
			float lerpT = _element->lifeTime / 2.0f;
			_element->color.a = std::lerp(0.0f, 1.0f, lerpT);

			_element->transform.scale.x += 10.0f * Time::DeltaTime();
			_element->transform.scale.z += 10.0f * Time::DeltaTime();
			//_element->transform.Update();
		}
	);
}

void KeyItemRipplesEffect::Update() {

}
