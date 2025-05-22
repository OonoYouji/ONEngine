#include "LaserHitEffect.h"

/// engine
#include "Engine/ECS/Component/Component.h"

void LaserHitEffect::Initialize() {

	Effect* effect = AddComponent<Effect>();

	effect->SetBlendMode(Effect::BlendMode::Add);
	effect->SetMaxEffectCount(512);
	effect->SetMeshPath("Assets/Models/primitive/sphere.obj");
	effect->SetTexturePath("Assets/Textures/smoke.png");
	effect->SetLifeLeftTime(2.0f);
	//effect->SetUseBillboard(true);

	effect->SetStartSpeed(50.0f, 70.0f);
	effect->SetStartSize(Vector3::kOne, Vector3::kOne * 3);
	effect->SetStartColor(Color(0.75f, 0.75f, 0.75f, 0.2f));
	
	effect->SetEmitTypeTime({ .emitTime = 0.01f, .emitInterval = 0.0f }, 4);
	effect->SetEmitShape({ 0, 0, 0 }, 4.0f);
		
	effect->SetElementUpdateFunc(
		[this](Effect::Element* _element) {
			_element->color.a -= 0.005f;
		}
	);


}

void LaserHitEffect::Update() {


}



void LaserExplosionEffect::Initialize() {

	Effect* effect = AddComponent<Effect>();

	effect->SetBlendMode(Effect::BlendMode::Add);
	effect->SetMaxEffectCount(1024);
	effect->SetMeshPath("Assets/Models/primitive/sphere.obj");
	effect->SetTexturePath("Assets/Textures/smoke.png");
	effect->SetLifeLeftTime(0.25f);

	effect->SetStartSpeed(1.0f, 4.0f);
	effect->SetStartSize(Vector3::kOne * 10.0f);
	effect->SetStartColor(Color(0.75f, 0.1f, 0.1f, 0.2f));

	effect->SetEmitTypeTime({ .emitTime = 0.05f, .emitInterval = 0.0f }, 4);
	effect->SetEmitShape({ 0, 0, 0 }, 5.0f);

	effect->SetElementUpdateFunc(
		[this](Effect::Element* _element) {
			_element->color.a -= 0.005f;
		}
	);
}

void LaserExplosionEffect::Update() {}
