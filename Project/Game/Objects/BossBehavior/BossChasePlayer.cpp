#include"BossChasePlayer.h"
//behavior
#include"Objects/BossBehavior/BossRoot.h"
#include"Objects/BossBehavior/BossSlurp.h"
//obj
#include"Objects/Ground/Ground.h"
#include"Objects/boss/boss.h"
#include"Objects/Player/Player.h"
//std
#include<numbers>
//function
#include"HormingFunction/Horming.h"
//初期化
BossChasePlayer::BossChasePlayer(Boss* boss)
	: BaseBossBehavior("ChasePlayer", boss) {

	pBoss_->ChaseInit();
	pBoss_->SetScale({ 2.0f,2.0f,2.0f });
	//パラメータ初期化
	chaseSpeedMax_ = 5.0f;
	chaseSpeedNormal_ = 0.01f;
	chaseMinPos_ = 2.0f;
	chaseMaxPos_ = 20.0f;
}

BossChasePlayer::~BossChasePlayer() {

}

//更新
void BossChasePlayer::Update() {

	std::pair<float, float> distanceAndDirection =CalculateDistanceAndDirection(pBoss_->GetPlayer()->GetPosition(), pBoss_->GetPosition(), Ground::groundScale_ + 1.0f);
	if (distanceAndDirection.first <= chaseMinPos_) {
		return;
	}
	// 現在の回転をオイラー角に変換
	Vec3 euler = QuaternionToEulerAngles(pBoss_->GetPivotQuaternion());

	// プレイヤーの方向を向くための回転を計算
	Quaternion inter = ToQuaternion({ euler.x, euler.y, -distanceAndDirection.second });

	// ホーミング移動のスピードを設定
	Quaternion move = ToQuaternion({ pBoss_->GetChaseSpeedParamater(), 0, 0 });

	// 回転を更新
	pBoss_->SetPivotQuaternion(inter);
	pBoss_->SetPivotSubtraction(move);

	if (distanceAndDirection.first >= chaseMaxPos_) {
		pBoss_->ChangeState(std::make_unique<BossSlurp>(pBoss_));
	}
}

