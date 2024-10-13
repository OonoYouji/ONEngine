#include"BossChasePlayer.h"
//behavior
#include"Objects/BossBehavior/BossRoot.h"
#include"Objects/BossBehavior/BossSlurp.h"
//obj
#include"Objects/Ground/Ground.h"
#include"Objects/boss/boss.h"
#include"Objects/Player/Player.h"
//初期化
BossChasePlayer::BossChasePlayer(Boss* boss)
	: BaseBossBehavior("ChasePlayer", boss) {

	pBoss_->ChaseInit();
	//パラメータ初期化
	chaseSpeedMax_ = 5.0f;
	chaseSpeedNormal_ = 0.01f;
	chaseMinPos_ = 4.0f;
	chaseMaxPos_ = 28.0f;
}

BossChasePlayer::~BossChasePlayer() {

}

//更新
void BossChasePlayer::Update() {

	
	std::pair<float, float> distanceAndDirection =pBoss_->CalculateDistanceAndDirection(pBoss_->GetPlayer()->GetPosition(), pBoss_->GetPosition(), Ground::groundScale_ + 1.0f);
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



Vec3 BossChasePlayer::QuaternionToEulerAngles(const Quaternion& q) {
	Vec3 euler;

	// X軸（Roll）の計算
	float sinX = 2.0f * (q.y * q.z - q.x * q.w);
	float absSinX = std::abs(sinX);
	const float epsilon = 0.001f;

	// 特定の範囲内でsinXをゼロにする（計算の安定性のため）
	if (absSinX < epsilon) {
		sinX = 0.0f;
	}

	euler.x = std::asin(-sinX);  // Roll角度（X軸回転）

	// X軸のcosを計算
	float cosX = std::cos(euler.x);

	// Y軸（Pitch）の計算
	float sinY = (2.0f * (q.x * q.z + q.y * q.w)) / cosX;
	float cosY = (2.0f * (q.w * q.w + q.z * q.z) - 1.0f) / cosX;
	euler.y = std::atan2(sinY, cosY);  // Pitch角度（Y軸回転）

	// Z軸（Yaw）の計算
	float sinZ = (2.0f * (q.x * q.y + q.z * q.w)) / cosX;
	float cosZ = (2.0f * (q.w * q.w + q.y * q.y) - 1.0f) / cosX;
	euler.z = std::atan2(sinZ, cosZ);  // Yaw角度（Z軸回転）

	return euler;
}

//Vector3からQuaternion変換
Quaternion BossChasePlayer::ToQuaternion(const Vec3& eulerAngles) {

	float halfRoll = eulerAngles.z * 0.5f;   // Roll
	float halfPitch = eulerAngles.x * 0.5f;  // Pitch
	float halfYaw = eulerAngles.y * 0.5f;    // Yaw

	float cosRoll = std::cos(halfRoll);
	float sinRoll = std::sin(halfRoll);
	float cosPitch = std::cos(halfPitch);
	float sinPitch = std::sin(halfPitch);
	float cosYaw = std::cos(halfYaw);
	float sinYaw = std::sin(halfYaw);

	Quaternion q;
	q.x = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw; // x
	q.y = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw; // y
	q.z = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw; // z
	q.w = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw; // w

	return Quaternion::Normalize(q); // 正規化して返す
}

