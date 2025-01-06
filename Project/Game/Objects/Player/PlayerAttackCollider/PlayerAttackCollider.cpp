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

}

void PlayerAttackCollider::Update() {

}

