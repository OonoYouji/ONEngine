#include "PuzzleClearEffectCannonStand.h"

/// engine
#include "Engine/ECS/Component/Component.h"

void PuzzleClearEffectCannonStand::Initialize() {

	MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetMeshPath("Assets/Models/objects/PuzzleClearEffect/CannonStand.obj");
	meshRenderer->SetTexturePath("Packages/Textures/white.png");

}

void PuzzleClearEffectCannonStand::Update() {

}
