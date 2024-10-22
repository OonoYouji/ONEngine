#include"BossBulletShot.h"
//behavior
#include"Objects/BossBehavior/BossRoot.h"
#include"Objects/BossBehavior/BossChasePlayer.h"
//emgine
#include"FrameManager/Time.h"
//obj
#include"Objects/Ground/Ground.h"
#include"Objects/Player/Player.h"
#include"Objects/boss/boss.h"
#include"Objects/Boss/BossBulletLump.h"

//function
#include"Easing/EasingFunction.h"
#include"HormingFunction/Horming.h"
//初期化

BossBulletShot::BossBulletShot(Boss* boss)
	: BaseBossBehavior("BossBulletShot", boss) {
	pBoss_->BulletShotInit();
	//値初期化
	easingTimeMax_ = 0.1f;
	easeDirection_ = 1.0f;	//予備動作

	anticipationTime_ = 0.0f;
	isAnticipationed_ = false;
	isStop_ = false;
}

BossBulletShot ::~BossBulletShot() {

}

//更新
void BossBulletShot::Update() {
	if (!isAnticipationed_) {
		//予備動作
		// イージングタイムを更新
		anticipationTime_ += Time::TimeRateDeltaTime();
		easingTime_ += Time::TimeRateDeltaTime() * easeDirection_; // 方向に応じて時間を増減

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
		pBoss_->SetScale(EaseInCubic<Vec3>(Vec3(2.0f, 2.0f, 2.0f), Vec3(2.4f, 2.4f, 2.4f), easingTime_, easingTimeMax_));

		//距離と向き
		auto [distance, direction] = CalculateDistanceAndDirection(pBoss_->GetPlayer()->GetPosition(), pBoss_->GetPosition(), Ground::groundScale_ + 1);

		// 現在の回転をオイラー角に変換
		Vec3 euler = QuaternionToEulerAngles(pBoss_->GetPivotQuaternion());

		// プレイヤーの方向を向くための回転を計算
		 inter_ = ToQuaternion({ euler.x, euler.y, -direction });

		// 回転を更新
		pBoss_->SetPivotQuaternion(inter_);
		
		//クールタイム終わったら弾発射
		if (anticipationTime_ >= kAnticipationTime_) {
			pBoss_->SetIsBuildingKill(true);//建物たち殺す
			isAnticipationed_ = true;
			Quaternion direction = pBoss_->GetPivotQuaternion() * pBoss_->GetQuaternion();			
			// 弾の生成
			BossbulletLump_ = new BossBulletLump();
			BossbulletLump_->Initialize();
			BossbulletLump_->SetDirection(direction);
		}
	}
	//弾発射
	if (isAnticipationed_) {

		//弾更新
		if (BossbulletLump_) {
			BossbulletLump_->Update();
			//弾死んだらスタン
			if (BossbulletLump_->GetIsDeath()) {
				BossbulletLump_->Destory();//デストロイ
				BossbulletLump_ = nullptr;
				isStop_ = true;
			}
		}
		if(isStop_){
			stopTime_ += Time::TimeRateDeltaTime();
			//振る舞い切り替え
			if (stopTime_ >= kStopTime_) {
				pBoss_->SetIsBuildingKill(false);
				pBoss_->ChangeState(std::make_unique<BossChasePlayer>(pBoss_));
			}
		}
	}
}

