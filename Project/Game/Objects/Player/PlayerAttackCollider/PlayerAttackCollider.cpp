#include "PlayerAttackCollider.h"

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "ComponentManager/Collider/BoxCollider.h"


PlayerAttackCollider::PlayerAttackCollider() {
	CreateTag(this);
}

PlayerAttackCollider::~PlayerAttackCollider() {}

void PlayerAttackCollider::Initialize() {
	
	Model* model = ModelManager::Load("PlayerAttackCollider");
	collider_ = AddComponent<BoxCollider>(model);
	collider_->isActive = false;

	damageValue_ = 10.0f;
}

void PlayerAttackCollider::Update() {

}

void PlayerAttackCollider::SetColliderActive(bool _isActive) {
	collider_->isActive = _isActive;
}

