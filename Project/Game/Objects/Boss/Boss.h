#pragma once

#include "GameObjectManager/GameObjectManager.h"

#include <ComponentManager/AudioSource/AudioSource.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>

//std
#include<memory>
//behavior
#include"Objects/BossBehavior/BaseBossBehavior.h"
#include"Objects/BossBehavior/BossRoot.h"
#include"Objects/BossBehavior/BossChasePlayer.h"

class Player;
class Boss : public BaseGameObject {
public:

	Boss() { CreateTag(this); }
	~Boss() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	//通常更新
	void RootInit();
	void RootUpdate();
	//ストーカー
	void ChaseInit();
	void ChaseUpdate();
	//建物吸引
	void SlurpInit();
	void SlurpUpdate();
	

	Vec3 QuaternionToEulerAngles(const Quaternion& q);
	Quaternion ToQuaternion(const Vec3& eulerAngles);
	
	//プレイヤーセット
	void SetPlayer(Player*player);
	//状態変更
	void ChangeState(std::unique_ptr<BaseBossBehavior>behavior);
	//getter
	float GetChaseSpeedParamater()const {return SpeedParamater_; }
	Quaternion GetPivotQuaternion()const { return pivot_.quaternion; }
	Player* GetPlayer() { return pPlayer_; }

	//setter
	void SetPivotQuaternion(Quaternion pivot) { pivot_.quaternion = pivot; }
	void SetPivotSubtraction(Quaternion pivot) { pivot_.quaternion *= pivot; }
	
private:
	
private:
	//プレイヤーポインタ
	Player* pPlayer_=nullptr;
	////状態
	std::unique_ptr<BaseBossBehavior>behavior_=nullptr;
	//ピボット
	Transform pivot_;
	//パラメータ調節用追従すピーⅮ
	float SpeedParamater_;
};