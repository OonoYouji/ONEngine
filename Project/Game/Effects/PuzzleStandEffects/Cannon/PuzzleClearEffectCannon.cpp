#include "PuzzleClearEffectCannon.h"

/// std
#include <numbers>

/// engine
#include "Engine/ECS/Component/Component.h"

void PuzzleClearEffectCannon::Initialize() {

	MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetMeshPath("Assets/Models/objects/PuzzleClearEffect/Cannon.obj");
	meshRenderer->SetTexturePath("Packages/Textures/white.png");
	meshRenderer->SetColor(Color(0.5f, 0.5f, 0.5f, 1.0f));

	transform_->position.y = 2.2f;

}

void PuzzleClearEffectCannon::Update() {
	float& cannonAnimeTime = variables_->Get<float>("animeTime");

	if (cannonAnimeTime < 2.0f) {
		cannonAnimeTime += Time::DeltaTime();

		transform_->rotate.x = std::lerp(
			std::numbers::pi_v<float> / 5.0f, 0.0f,
			cannonAnimeTime / 2.0f
		);

	}

}
