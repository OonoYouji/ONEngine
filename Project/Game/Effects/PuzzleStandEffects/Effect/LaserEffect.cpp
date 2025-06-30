#include "LaserEffect.h"

#include "Engine/ECS/Component/Component.h"

void LaserEffect::Initialize() {

	Effect* effect = AddComponent<Effect>();
	effect->SetBlendMode(Effect::BlendMode::Add);
	effect->SetMaxEffectCount(1024);
	effect->SetMeshPath("./Assets/Models/objects/PuzzleClearEffect/PuzzleClearEffectLaser.obj");
	effect->SetTexturePath("./Assets/Textures/circle.png");
	effect->SetLifeLeftTime(0.5f);

	effect->SetStartSpeed(100.0f);
	effect->SetStartSize(Vector3::kOne);
	effect->SetStartColor(Color(0.75f, 0.75f, 0.75f, 0.2f));

	effect->SetEmitTypeTime({ .emitTime = 0.01f, .emitInterval = 0.0f }, 7);
	effect->SetEmitShape({ 0, 0, 0 }, Vec3::kZero);

	effect->SetElementUpdateFunc(
		[this](Effect::Element* _element) {
			Vector3& direction = variables_->Get<Vector3>("effectDirection");
			_element->transform.position += Matrix4x4::Transform(direction, transform_->matWorld) * Time::DeltaTime();
			_element->color.a -= 0.005f;
		}
	);


	transform_->position.y = 1.2f;
}

void LaserEffect::Update() {

}



void LaserShootEffect::Initialize() {
	Effect* effect = AddComponent<Effect>();
	effect->SetMeshPath("./Assets/Models/primitive/plane.obj");
	effect->SetTexturePath("./Assets/Textures/ring.png");

	effect->SetEmitTypeTime({ .emitTime = 0.25f, .emitInterval = 0.0f }, 2);
	effect->SetLifeLeftTime(1.0f);
	effect->SetStartSpeed(0.0f);
	effect->SetMaxEffectCount(64);
	effect->SetStartColor(Color(0.5f, 1.0f, 1.0f, 0.2f), Color(1.0f, 1.0f, 1.0f, 0.3f));
	effect->SetEmitShape({ 0, 0, 0 }, Vec3(2.0f, 0.0f, 2.0f));
	effect->SetBlendMode(Effect::BlendMode::Add);

	effect->SetElementUpdateFunc(
		[](Effect::Element* _element) {
			float lerpT = _element->lifeTime / 2.0f;
			_element->color.a = std::lerp(0.0f, 1.0f, lerpT);

			_element->transform.scale.x += 200.0f * Time::DeltaTime();
			_element->transform.scale.z += 200.0f * Time::DeltaTime();
		}
	);

	variables_->Add<float>("animeTime", 0.0f);

	transform_->position.y = 1.2f;
}

void LaserShootEffect::Update() {

	/*float& animeTime = variables_->Get<float>("animeTime");
	animeTime += Time::DeltaTime();

	if (animeTime > 0.5f) {
		SetActive(false);
	}*/

}
