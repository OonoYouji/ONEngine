#include "PlayerWalkEffect.h"

#define NOMINMAX

/// std
#include <numbers>

/// engine
#include "Engine/ECS/Component/Component.h"
#include "Engine/Core/Utility/Utility.h"

/// game
#include "Game/Entity/Player/Player.h"


void PlayerWalkEffect::Initialize() {
	effect_ = AddComponent<Effect>();
	effect_->SetEmitTypeDistance(1.0f, 12);
	effect_->SetMeshPath("Assets/Models/primitive/frontToPlane.obj");
	effect_->SetTexturePath("Assets/Textures/smoke.png");

	effect_->SetBlendMode(Effect::BlendMode::Add);
	effect_->SetEmitShape(Vec3::kZero, Vec3::kOne * 2.0f);
	effect_->SetStartSpeed(0.0f);
	effect_->SetLifeLeftTime(1.8f);
	effect_->SetStartSize(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.05f, 0.05f, 0.05f));
	effect_->SetStartRotate(Vector3::kZero, Vec3(0.0f, 0.0f, 2.0f * std::numbers::pi_v<float>));
	effect_->SetStartColor(Color(0.0f, 0.0f, 0.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f));
	effect_->SetMaxEffectCount(1000);



	effect_->SetElementUpdateFunc(
		[](Effect::Element* _element) {
			float lerpT = std::clamp(_element->lifeTime / 1.8f, 0.0f, 1.0f);
			//_element->color.a = std::lerp(1.0f, 1.0f, lerpT);

			_element->transform.position.y += 1.5f * Time::DeltaTime();
			//_element->transform.scale.y += 2.0f * Time::DeltaTime();
			_element->transform.rotate.y += (1.0f / 12.0f) * Time::DeltaTime();

			_element->transform.Update();
		}
	);
}

void PlayerWalkEffect::Update() {

	/*Player* pPlayer = dynamic_cast<Player*>(GetParent());
	if (!pPlayer) {
		return;
	}

	auto vars = pPlayer->GetComponent<Variables>();
	effect_->SetIsCreateParticle(vars->Get<bool>("onGround"));*/
}
