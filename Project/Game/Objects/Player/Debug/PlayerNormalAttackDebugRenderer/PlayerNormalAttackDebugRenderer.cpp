#include "PlayerNormalAttackDebugRenderer.h"

/// engine
#include "ComponentManager/MeshInstancingRenderer/MeshInstancingRenderer.h"

/// user
#include "Objects/Player/Player.h"
#include "../../State/Manager/PlayerStateManager.h"
#include "../../State/PlayerNormalAttack/PlayerNormalAttack.h"


PlayerNormalAttackDebugRenderer::PlayerNormalAttackDebugRenderer(Player* _playerPtr) 
	: pPlayer_(_playerPtr) {
	CreateTag(this);
}

PlayerNormalAttackDebugRenderer::~PlayerNormalAttackDebugRenderer() {}

void PlayerNormalAttackDebugRenderer::Initialize() {

	meshInstancingRenderer_ = AddComponent<MeshInstancingRenderer>(128u);

	SetParent(pPlayer_->GetTransform());

	pNormalAttakState_ = static_cast<PlayerNormalAttack*>(pPlayer_->GetStateManager()->GetState(PlayerStateOrder_NormalAttack));

}

void PlayerNormalAttackDebugRenderer::Update() {


}

