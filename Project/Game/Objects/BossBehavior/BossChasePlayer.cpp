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
	chaseMinPos_ = 10.0f;
	chaseMaxPos_ = 25.0f;
}

BossChasePlayer::~BossChasePlayer() {

}

//更新
void BossChasePlayer::Update() {

	// 距離と方向を計算
	std::pair<float, float> distanceAndDirection = CalculateDistanceAndDirection(
		pBoss_->GetPlayer()->GetPosition(), pBoss_->GetPosition(), Ground::groundScale_ + 1.0f);

	// 一定距離で攻撃に遷移
	if (distanceAndDirection.first <= chaseMinPos_) {
		pBoss_->ChangeState(std::make_unique<BossAttack>(pBoss_));
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
	Quaternion interpolatedRotation = Slerp(currentRotation, targetRotation, rotationSpeed * Time::DeltaTime());

	// ホーミング移動のスピードを設定
	Quaternion move = ToQuaternion({ pBoss_->GetChaseSpeedParamater(), 0, 0 });

	// 回転を更新
	pBoss_->SetPivotQuaternion(interpolatedRotation);
	pBoss_->SetPivotSubtraction(move); // 移動もスムーズに

	// 一定距離で吸い込み状態に遷移
	if (distanceAndDirection.first >= chaseMaxPos_) {
		pBoss_->ChangeState(std::make_unique<BossSlurp>(pBoss_));
	}
}


// QuaternionのLerp関数
Quaternion BossChasePlayer::Lerp(const Quaternion& start, const Quaternion& end, float t) {
	// 線形補間を実行
	Quaternion result = (start * (1.0f - t)) + (end * t);

	// 結果を正規化して返す（回転なので正規化は重要）
	return Quaternion::Normalize(result);
}

// Slerp関数（最短距離で補間）
Quaternion BossChasePlayer::Slerp(const Quaternion& start, Quaternion end, float t) {

	float dot = Quaternion::Dot(start,end);

	// 内積が負の場合、最短経路を取るためにendを反転
	if (dot < 0.0f) {
		end = end * -1.0f;
		dot = -dot;
	}

	const float DOT_THRESHOLD = 0.9995f;
	if (dot > DOT_THRESHOLD) {
		return Lerp(start, end, t);  // 線形補間にフォールバック
	}

	float theta_0 = std::acos(dot);
	float theta = theta_0 * t;

	float sin_theta = std::sin(theta);
	float sin_theta_0 = std::sin(theta_0);

	float s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;
	float s1 = sin_theta / sin_theta_0;

	return (start * s0) + (end * s1);
}