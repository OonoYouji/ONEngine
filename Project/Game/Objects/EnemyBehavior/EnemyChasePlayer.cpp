#include"EnemyChasePlayer.h"
//behavior
#include"Objects/EnemyBehavior/EnemyRoot.h"
//obj
#include"Objects/Enemy/Enemy.h"
#include"Objects/Ground/Ground.h"
#include"Objects/boss/boss.h"
#include"Objects/Player/Player.h"
//std
#include<numbers>
//function
#include"HormingFunction/Horming.h"
#include"FrameManager/Time.h"
//初期化
EnemyChasePlayer::EnemyChasePlayer(Enemy* enemy)
	: BaseEnemyBehavior("ChasePlayer", enemy) {

	pEnemy_->ChaseInit();
	pEnemy_->SetScale({ 2.0f,2.0f,2.0f });
	//パラメータ初期化
	chaseSpeedMax_ = 5.0f;
	chaseSpeedNormal_ = 0.01f;
	chaseMinPos_ = 10.0f;
	chaseMaxPos_ = 25.0f;
}

EnemyChasePlayer::~EnemyChasePlayer() {

}

//更新
void EnemyChasePlayer::Update() {

	// 距離と方向を計算
	std::pair<float, float> distanceAndDirection = CalculateDistanceAndDirection(
		pEnemy_->GetPlayer()->GetPosition(), pEnemy_->GetPosition(), Ground::groundScale_ + 1.0f);

	// 現在の回転をオイラー角に変換
	Vec3 euler = QuaternionToEulerAngles(pEnemy_->GetPivotQuaternion());

	// プレイヤーの方向を向くための回転を計算
	Quaternion targetRotation = ToQuaternion({ euler.x, euler.y, -distanceAndDirection.second });

	// 現在の回転
	Quaternion currentRotation = pEnemy_->GetPivotQuaternion();

	// 回転をスムーズに補間 (Slerpを使用)
	float rotationSpeed = 6.0f; // 回転速度、必要に応じて調整
	Quaternion interpolatedRotation = Slerp(currentRotation, targetRotation, rotationSpeed * Time::DeltaTime());

	// ホーミング移動のスピードを設定
	Quaternion move = ToQuaternion({ pEnemy_->GetSpeedParamater() * Time::DeltaTime(), 0, 0 });

	// 回転を更新
	pEnemy_->SetPivotQuaternion(interpolatedRotation);
	pEnemy_->SetPivotSubtraction(move ); // 移動もスムーズに

	// 一定距離で通常状態に遷移
	if (distanceAndDirection.first >= chaseMaxPos_) {
		pEnemy_->ChangeState(std::make_unique<EnemyRoot>(pEnemy_));
	}
}


