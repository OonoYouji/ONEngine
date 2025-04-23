#include "PlayerWalkEffect.h"

#define NOMINMAX

/// engine
#include "Engine/ECS/Component/Component.h"
#include "Engine/Core/Utility/Utility.h"

/// game
#include "Game/Entity/Player/Player.h"


void PlayerWalkEffect::Initialize() {
	effect_ = AddComponent<Effect>();
	effect_->SetEmitTypeDistance(2, 4);
	effect_->SetMeshPath("Assets/Models/primitive/plane.obj");
	//effect->SetTexturePath("Packages/Textures/uvChecker.png");
	effect_->SetTexturePath("Assets/Textures/playerWalkEffect.png");

	//effect->SetUseBillboard(true);
	effect_->SetStartSpeed(0.1f);
	effect_->SetLifeLeftTime(0.4f);
	effect_->SetEmittedElementColor(Vector4::kWhite);

	effect_->SetElementUpdateFunc(
		[](Effect::Element* _element) {
			float lerpT = std::clamp(_element->lifeTime / 3.0f, 0.0f, 1.0f);
			_element->color.w = std::lerp(0.0f, 0.3f, lerpT);

			_element->transform.position.y += 0.1f * Time::DeltaTime();
			_element->transform.scale.z = std::lerp(3.0f, 0.5f, (lerpT));

			if (_element->transform.rotate.y == 0) {
				_element->transform.rotate.y = static_cast<float>(rand() % 360) / 180.0f;
			}

			_element->transform.Update();
		}
	);
}

void PlayerWalkEffect::Update() {

	Player* pPlayer = dynamic_cast<Player*>(GetParent());
	if (!pPlayer) {
		return;
	}

	auto vars = pPlayer->GetComponent<Variables>();

	if (vars->Get<float>("jumpPower") == 0) {
		effect_->SetIsCreateParticle(false);
	} else {
		effect_->SetIsCreateParticle(true);
	}
}
