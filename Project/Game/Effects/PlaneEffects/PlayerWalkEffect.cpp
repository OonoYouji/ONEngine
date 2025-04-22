#include "PlayerWalkEffect.h"

#define NOMINMAX
#include "Engine/ECS/Component/Component.h"

void PlayerWalkEffect::Initialize() {
	Effect* effect = AddComponent<Effect>();
	effect->SetEmitTypeDistance(10.0f, 4);
	effect->SetMeshPath("Assets/Models/primitive/plane.obj");
	effect->SetTexturePath("Packages/Textures/uvChecker.png");

	effect->SetStartSpeed(0.1f);
	effect->SetLifeLeftTime(0.2f);

	effect->SetElementUpdateFunc(
		[](Effect::Element* _element) {
			_element->color.w = std::max(0.0f, _element->lifeTime / 3.0f);

			_element->transform.scale.z = 3.0f * (1.0f + (1.0f - _element->lifeTime / 0.2f));

			if (_element->transform.rotate.y == 0) {
				_element->transform.rotate.y = static_cast<float>(rand() % 360) / 180.0f;
			}

			_element->transform.Update();
		}
	);
}

void PlayerWalkEffect::Update() {

}
