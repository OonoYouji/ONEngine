#include "Player.h"

#include "ComponentManager/MeshRenderer/MeshRenderer.h"

void Player::Initialize(){
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Sphere");
}

void Player::Update(){}
