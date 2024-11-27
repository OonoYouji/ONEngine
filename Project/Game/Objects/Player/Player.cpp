#include "Player.h"

#include "ComponentManager/MeshRenderer/MeshRenderer.h"

Player::Player() {
	CreateTag(this);
}

Player::~Player() {}

void Player::Initialize() {

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Player");

}

void Player::Update() {

}

