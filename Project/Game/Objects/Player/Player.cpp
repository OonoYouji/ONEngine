#include "Player.h"

/// engine
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

Player::Player(GameCamera* _gameCameraPtr) {
	CreateTag(this);
}

Player::~Player() {}

void Player::Initialize() {

	playerMesh_ = new PlayerMesh();
	playerMesh_->Initialize();
	playerMesh_->SetParent(pTransform_);

}

void Player::Update() {

}

