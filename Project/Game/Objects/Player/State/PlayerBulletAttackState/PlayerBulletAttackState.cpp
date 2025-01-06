#include "PlayerBulletAttackState.h"

/// engine
#include "Math/Random.h"
#include "FrameManager/Time.h"

/// user
#include "../../Player.h"
#include "../../PlayerBullet/PlayerBullet.h"
#include "Objects/Enemy/Enemy.h"


PlayerBulletAttackState::PlayerBulletAttackState(Player* _player, PlayerStateManager* _stateManager)
	: IPlayerState(_player, _stateManager) {

}

PlayerBulletAttackState::~PlayerBulletAttackState() {}

void PlayerBulletAttackState::Start() {
	
	forwardEnemyList_ = pPlayer_->GetForwardEnemyList();
	if(forwardEnemyList_.size() == 0) {
		isEnd_ = true;
		return;
	}

	maxTime_     = 0.5f;
	currentTime_ = 0.0f;
	isEnd_ = false;

	creationBulletNum_ = static_cast<int>(forwardEnemyList_.size());

	CreateBullets();
	SpreadToTheLeftAndRight();

}

void PlayerBulletAttackState::Update() {

	if(currentTime_ > maxTime_) {
		isEnd_ = true;
		return;
	}

	/// current timeが max time以下になるまで以下の処理を行う

	currentTime_ += Time::DeltaTime();
	float lerpT = std::clamp(currentTime_ / maxTime_, 0.0f, 1.0f);

	if(lerpT == 1.0f) {
		FireBullets();
	}

}

void PlayerBulletAttackState::Exit() {

}


bool PlayerBulletAttackState::IsEnd() {
	return isEnd_;
}

int PlayerBulletAttackState::NextStateIndex() {
	return PlayerStateOrder_Root;
}


void PlayerBulletAttackState::Debug() {

}

void PlayerBulletAttackState::CreateBullets() {
	for(int i = 0; i < creationBulletNum_; ++i) {
		PlayerBullet* bullet = new PlayerBullet();
		bullet->Initialize();
		bullets_.push_back(bullet);
	}
}

void PlayerBulletAttackState::FireBullets() {
	std::list<Enemy*> enemyList = forwardEnemyList_;

	for(int i = 0; i < creationBulletNum_; ++i) {
		PlayerBullet* bullet = *std::next(bullets_.begin(), i);
		Enemy*        enemy  = *std::next(enemyList.begin(), i);

		bullet->Fire(enemy);
	}

	bullets_.clear();
}

void PlayerBulletAttackState::SpreadToTheLeftAndRight() {

	/// listを半分に分ける
	auto itr = bullets_.begin();
	std::advance(itr, bullets_.size() / 2);

	rightBulletList_.clear();
	leftBulletList_.clear();

	rightBulletList_.insert(rightBulletList_.begin(), bullets_.begin(), itr);
	leftBulletList_.insert(leftBulletList_.begin(), itr, bullets_.end());

	int index = 0;
	for(auto& bullet : leftBulletList_) {
		bullet->SetPosition(pPlayer_->GetPosition() + Vec3(-2.0f, 3.0f, 0.0f));
		bullet->SetIdelPosition(
			pPlayer_->GetPosition() + Vec3(-index * 2.0f, Random::Float(2.0f, 4.0f), Random::Float(-1.0f, 1.0f))
		);
		index++;
	}
	
	index = 0;
	for(auto& bullet : rightBulletList_) {
		bullet->SetPosition(pPlayer_->GetPosition() + Vec3(-2.0f, 3.0f, 0.0f));
		bullet->SetIdelPosition(
			pPlayer_->GetPosition() + Vec3(index * 2.0f, Random::Float(2.0f, 4.0f), Random::Float(-1.0f, 1.0f))
		);
		index++;
	}

}

