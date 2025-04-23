#include "PlayerWalkEffect.h"

#define NOMINMAX
#include "Engine/ECS/Component/Component.h"
#include "Engine/Core/Utility/Utility.h"

void PlayerWalkEffect::Initialize() {
	Effect* effect = AddComponent<Effect>();
	effect->SetEmitTypeDistance(2, 4);
	effect->SetMeshPath("Assets/Models/primitive/plane.obj");
	//effect->SetTexturePath("Packages/Textures/uvChecker.png");
	effect->SetTexturePath("Assets/Textures/playerWalkEffect.png");

	//effect->SetUseBillboard(true);
	effect->SetStartSpeed(0.1f);
	effect->SetLifeLeftTime(0.4f);

	effect->SetElementUpdateFunc(
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

}
