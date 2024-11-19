#include "PlayerAttackCollider.h"

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"

/// component
#include "ComponentManager/Collider/BoxCollider.h"

/// objects
#include "../../Player.h"

PlayerAttackCollider::PlayerAttackCollider(Player* _player) {
	CreateTag(this);

	pPlayer_ = _player;
}

PlayerAttackCollider::~PlayerAttackCollider() {}

void PlayerAttackCollider::Initialize() {
	
	Model* colliderModel = ModelManager::Load("Sphere");
	boxCollider_ = AddComponent<BoxCollider>(colliderModel);

	SetParent(pPlayer_->GetTransform());

	

}

void PlayerAttackCollider::Update() {

}

