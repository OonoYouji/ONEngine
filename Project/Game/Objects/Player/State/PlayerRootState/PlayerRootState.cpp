#include "PlayerRootState.h"

#include "Math/LerpShortAngle.h"
#include "../../Player.h"

PlayerRootState::PlayerRootState(Player* _player) : IPlayerState(_player) {}
PlayerRootState::~PlayerRootState() {}

void PlayerRootState::Start() {

}

void PlayerRootState::Update() {

	/// 回転の更新
	RotateUpdate();

	/// 座標の更新
	MoveUpdate();

}

void PlayerRootState::Exit() {

}

bool PlayerRootState::IsEnd() {
	return pPlayer_->GetFlag(PlayerFlag_IsDush).Enter();
}

int PlayerRootState::NextStateIndex() {
	if(pPlayer_->GetFlag(PlayerFlag_IsDush).Enter()) {
		return PlayerStateOrder_Dush;
	}

	return PlayerStateOrder_Root;
}



void PlayerRootState::RotateUpdate() {
	const Vec3& direction = pPlayer_->GetDirection();
	const Vec3& lastDir   = pPlayer_->GetLastDirection();

	if(direction.x != 0 || direction.z != 0) {
		pPlayer_->SetLastDirection(direction);
	}

	Vector3 rotate = pPlayer_->GetMesh()->GetRotate();
	rotate.y = LerpShortAngle(rotate.y, std::atan2(lastDir.x, lastDir.z), 0.1f);
	pPlayer_->SetMeshRotate(rotate);
}

void PlayerRootState::MoveUpdate() {
	Transform* playerTransform = pPlayer_->GetTransform();
	pPlayer_->SetVelocity(pPlayer_->GetDirection() * pPlayer_->GetMoveSpeed());
	playerTransform->position += pPlayer_->GetDirection() * pPlayer_->GetMoveSpeed();
}
