#include"EnemyRoot.h"
//behavior
#include"objects/EnemyBehavior/EnemyChasePlayer.h"
//obj
#include"Objects/Enemy/Enemy.h"
#include"Objects/Ground/Ground.h"
#include"Objects/boss/boss.h"
#include"Objects/Player/Player.h"
//std
#include<numbers>
//function
#include"HormingFunction/Horming.h"
#include"Easing/EasingFunction.h"
#include"FrameManager/Time.h"
//初期化

EnemyRoot::EnemyRoot(Enemy* boss)
	: BaseEnemyBehavior("Root", boss) {
	pEnemy_->RootInit();
	/*easeT_ = 0.0f;
	easeTimeMax_ = 0.7f;
	amplitude_ = 0.2f;
	period_ = 0.2f;
	perceptionDistance_ = 10.0f;
	isChase_ = false;*/
}

EnemyRoot::~EnemyRoot() {

}

//更新
void EnemyRoot::Update() {
	pEnemy_->RootUpdate();
	//// 距離と方向を計算
	//std::pair<float, float> distanceAndDirection = CalculateDistanceAndDirection(
	//	pEnemy_->GetPlayer()->GetPosition(), pEnemy_->GetPosition(), Ground::groundScale_ + 1.0f);

	//if (distanceAndDirection.first<= perceptionDistance_&&!isChase_) {
	//	isChase_ = true;
	//}
	//if (isChase_) {//チェイスモードに以降

	//	// 現在の回転をオイラー角に変換
	//	Vec3 euler = QuaternionToEulerAngles(pEnemy_->GetPivotQuaternion());

	//	// プレイヤーの方向を向くための回転を計算
	//	Quaternion targetRotation = ToQuaternion({ euler.x, euler.y, -distanceAndDirection.second });

	//	// 現在の回転
	//	Quaternion currentRotation = pEnemy_->GetPivotQuaternion();

	//	// 回転をスムーズに補間 (Slerpを使用)
	//	float rotationSpeed = 10.0f; // 回転速度、必要に応じて調整
	//	Quaternion interpolatedRotation = Slerp(currentRotation, targetRotation, rotationSpeed * Time::DeltaTime());

	//	// 回転を更新
	//	pEnemy_->SetPivotQuaternion(interpolatedRotation);

	//	easeT_ += Time::DeltaTime();
	//	if (easeT_ >= easeTimeMax_) {
	//	easeT_=	easeTimeMax_;
	//	pEnemy_->ChangeState(std::make_unique<EnemyChasePlayer>(pEnemy_));
	//	}
	//	pEnemy_->SetScale(EaseAmplitudeScale(Vec3(1.0f, 1.0f, 1.0f), easeT_, easeTimeMax_, amplitude_, period_));

	//}
}


