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
	void ChaseUpdate(const float& speed);
	

	Vec3 QuaternionToEulerAngles(const Quaternion& q);
	Quaternion ToQuaternion(const Vec3& eulerAngles);
	/*Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t);*/

	//プレイヤーセット
	void SetPlayer(Player*player);

	//状態変更
	void ChangeState(std::unique_ptr<BaseBossBehavior>behavior);
	//getter
	float GetChaseSpeedParamater()const {return SpeedParamater_; }
	
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