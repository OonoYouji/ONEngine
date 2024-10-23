#include"BossChasePlayer.h"
//behavior
#include"Objects/BossBehavior/BossRoot.h"
#include"Objects/BossBehavior/BossSlurp.h"
#include"Objects/BossBehavior/BossAttack.h"
//obj
#include"Objects/Ground/Ground.h"
#include"Objects/boss/boss.h"
#include"Objects/Player/Player.h"
//std
#include<numbers>
//function
#include"HormingFunction/Horming.h"
#include"FrameManager/Time.h"
//初期化
BossChasePlayer::BossChasePlayer(Boss* boss)
	: BaseBossBehavior("ChasePlayer", boss) {

	pBoss_->ChaseInit();
	pBoss_->SetScale({ 2.0f,2.0f,2.0f });
	//パラメータ初期化
	chaseSpeedMax_ = 5.0f;
	chaseSpeedNormal_ = 0.01f;
	AttackPos_ = 10.0f;/// ボス
	SlurpPos_ = 25.0f;
	
	attackCoolTime_ = 1.0f;
}

BossChasePlayer::~BossChasePlayer() {

}

//更新
void BossChasePlayer::Update() {
	if (!pBoss_->GetIsHitBack()) {
		attackCoolTime_ -= Time::TimeRateDeltaTime();//	攻撃クールタイムを減算
		// 距離と方向を計算
		std::pair<float, float> distanceAndDirection = CalculateDistanceAndDirection(
			pBoss_->GetPlayer()->GetPosition(), pBoss_->GetPosition(), Ground::groundScale_ + 1.0f);

		// 一定距離で攻撃に遷移
		if (distanceAndDirection.first <= AttackPos_) {
			if (attackCoolTime_ <= 0) {
				pBoss_->ChangeState(std::make_unique<BossAttack>(pBoss_));
			}
			return;
		}

		// 現在の回転をオイラー角に変換
		Vec3 euler = QuaternionToEulerAngles(pBoss_->GetPivotQuaternion());

		// プレイヤーの方向を向くための回転を計算
		Quaternion targetRotation = ToQuaternion({ euler.x, euler.y, -distanceAndDirection.second });

		// 現在の回転
		Quaternion currentRotation = pBoss_->GetPivotQuaternion();

		// 回転をスムーズに補間 (Slerpを使用)
		float rotationSpeed = 6.0f; // 回転速度、必要に応じて調整
		Quaternion interpolatedRotation = Slerp(currentRotation, targetRotation, rotationSpeed * Time::TimeRateDeltaTime());

		// ホーミング移動のスピードを設定
		Quaternion move = ToQuaternion({ pBoss_->GetChaseSpeedParamater() * Time::TimeRateDeltaTime(), 0, 0 });

		// 回転を更新
		pBoss_->SetPivotQuaternion(interpolatedRotation);
		pBoss_->SetPivotSubtraction(move); // 移動もスムーズに

		// 一定距離で吸い込み状態に遷移
		if (distanceAndDirection.first >= SlurpPos_) {
			pBoss_->ChangeState(std::make_unique<BossSlurp>(pBoss_));
		}
	}
}


