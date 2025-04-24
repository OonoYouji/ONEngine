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
	effect_->SetEmitTypeDistance(2, 4);
	effect_->SetMeshPath("Assets/Models/primitive/frontToPlane.obj");
	effect_->SetTexturePath("Assets/Textures/circle.png");

	effect_->SetUseBillboard(true);
	effect_->SetStartSpeed(0.1f);
	effect_->SetLifeLeftTime(0.4f);
	//effect_->SetEmittedElementColor(Vector4::kWhite);
	effect_->SetStartScale(Vector3(0.5f, 0.5f, 0.5f)); 

	effect_->SetElementUpdateFunc(
		[](Effect::Element* _element) {
			float lerpT = std::clamp(_element->lifeTime / 3.0f, 0.0f, 1.0f);
			_element->color.a = std::lerp(0.0f, 0.3f, lerpT);

			_element->transform.position.y += 0.1f * Time::DeltaTime();
			_element->transform.scale.z = std::lerp(3.0f, 0.5f, (lerpT));

			if (_element->transform.rotate.y == 0) {
				_element->transform.rotate.y = Random::Float(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);
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
	effect_->SetIsCreateParticle(vars->Get<bool>("onGround"));
}
