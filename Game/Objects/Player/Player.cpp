#include "Player.h"

Player::Player() {
	CreateTag(this);
}

void Player::Initialize() {
	model_ = ModelManager::CreateCube();
	CreateBoxCollider(model_);
}

void Player::Update() {

}

void Player::Draw() {
	model_->Draw(&transform_);
}
