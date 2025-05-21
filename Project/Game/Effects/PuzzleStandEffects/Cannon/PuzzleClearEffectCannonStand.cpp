#include "PuzzleClearEffectCannonStand.h"

/// engine
#include "Engine/ECS/Component/Component.h"

void PuzzleClearEffectCannonStand::Initialize() {

	MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetMeshPath("Assets/Models/objects/PuzzleClearEffect/CannonStand.obj");
	meshRenderer->SetTexturePath("Packages/Textures/white.png");
	meshRenderer->SetColor(Color(0.5f, 0.5f, 0.5f, 1.0f));
}

void PuzzleClearEffectCannonStand::Update() {

}
