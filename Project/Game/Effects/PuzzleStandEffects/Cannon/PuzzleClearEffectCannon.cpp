#include "PuzzleClearEffectCannon.h"

#include "Engine/ECS/Component/Component.h"

void PuzzleClearEffectCannon::Initialize() {

	MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetMeshPath("Assets/Models/objects/PuzzleClearEffect/Cannon.obj");
	meshRenderer->SetTexturePath("Packages/Textures/white.png");
	meshRenderer->SetColor(Color(0.5f, 0.5f, 0.5f, 1.0f));

	transform_->position.y = 2.2f;

}

void PuzzleClearEffectCannon::Update() {

}
