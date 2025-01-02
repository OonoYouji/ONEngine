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

	SetParent(pPlayer_->GetMesh()->GetTransform());

	pNormalAttakState_ = static_cast<PlayerNormalAttack*>(pPlayer_->GetStateManager()->GetState(PlayerStateOrder_NormalAttack));

}

void PlayerNormalAttackDebugRenderer::Update() {

	if(!pNormalAttakState_) {
		return;
	}

	const std::vector<Vec3>& bouncePositions = pNormalAttakState_->GetBouncePositions();
	bouncePositionTransforms_.resize(bouncePositions.size());

	for(size_t i = 0; i < bouncePositionTransforms_.size(); ++i) {
		bouncePositionTransforms_[i].SetParent(pPlayer_->GetMesh()->GetTransform());
		bouncePositionTransforms_[i].position = bouncePositions[i];
		bouncePositionTransforms_[i].Update();
	}

	/// バウンドのTransformを追加
	meshInstancingRenderer_->ResetTransformArray();
	for(auto& transform : bouncePositionTransforms_) {
		meshInstancingRenderer_->AddTransform(&transform);
	}

}

