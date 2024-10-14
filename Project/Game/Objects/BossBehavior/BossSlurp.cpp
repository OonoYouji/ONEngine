#include"BossSlurp.h"
#include"Objects/BossBehavior/BossRoot.h"
#include"Objects/boss/boss.h"
#include"FrameManager/Time.h"

//obj
#include"Objects/Ground/Ground.h"
#include"Objects/Player/Player.h"

#include"Easing/EasingFunction.h"
//初期化
BossSlurp::BossSlurp(Boss* boss)
	: BaseBossBehavior("BossSlurp", boss) {
	pBoss_->SlurpInit();
	//値初期化
	easingTimeMax_ = 0.4f;
	MinDistance_ = 10.0f;
}

BossSlurp ::~BossSlurp() {

}

//更新
void BossSlurp::Update() {

	//吸い込み処理更新
	pBoss_->SlurpUpdate();

	// イージングタイムを更新
	easingTime_ += Time::DeltaTime() * easeDirection_; // 方向に応じて時間を増減

	// タイムが1を超えたら逆方向に、0未満になったら進む方向に変更
	if (easingTime_ >= easingTimeMax_) {
		easingTime_ = easingTimeMax_;
		easeDirection_ = -1.0f; // 逆方向に切り替え
	}
	else if (easingTime_ <= 0.0f) {
		easingTime_ = 0.0f;
		easeDirection_ = 1.0f; // 進む方向に切り替え
	}
	//ボスのスケール切り替え
	pBoss_->SetScale(EaseInCubic<Vec3>(Vec3(1.0f,1.0f,1.0f), Vec3(1.4f, 1.4f, 1.4f),easingTime_,easingTimeMax_));

	//一定距離で追従に戻す
	std::pair<float, float> distanceAndDirection = pBoss_->CalculateDistanceAndDirection(pBoss_->GetPlayer()->GetPosition(), pBoss_->GetPosition(), Ground::groundScale_ + 1.0f);
	if (distanceAndDirection.first <= MinDistance_) {
		if (!pBoss_->GetIsSlurping()) {
			pBoss_->ChangeState(std::make_unique<BossChasePlayer>(pBoss_));
		}
	}
}

std::pair<float, float> BossSlurp::CalculateDistanceAndDirection(const Vec3& targetPos, const Vec3& bossPosition, const float& radius) {
	// ボスとプレイヤーの位置を3次元座標から球面座標に変換
	float latitude1 = std::asin(bossPosition.y / Vector3::Length(bossPosition)); // ボスの緯度
	float longitude1 = std::atan2(bossPosition.z, bossPosition.x); // ボスの経度

	float latitude2 = std::asin(targetPos.y / Vector3::Length(targetPos)); // プレイヤーの緯度
	float longitude2 = std::atan2(targetPos.z, targetPos.x); // プレイヤーの経度

	// 中心角を計算
	float deltaSigma = std::acos(
		std::sin(latitude1) * std::sin(latitude2) +
		std::cos(latitude1) * std::cos(latitude2) * std::cos(longitude2 - longitude1)
	);

	// 球面上の距離を計算
	float sphereRadius = radius; // 半径
	float distance = sphereRadius * deltaSigma;

	// 方位角を計算
	float deltaLon = longitude2 - longitude1;
	float y = std::sin(deltaLon) * std::cos(latitude2);
	float x = std::cos(latitude1) * std::sin(latitude2) - std::sin(latitude1) * std::cos(latitude2) * std::cos(deltaLon);
	float direction = std::atan2(y, x);

	return { distance, direction }; // 距離と方位角を返す

}
