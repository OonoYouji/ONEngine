#include "Player.h"

#include <Component/MeshRenderer/MeshRenderer.h>


void Player::Initialize() {
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("GameScreen");
}
