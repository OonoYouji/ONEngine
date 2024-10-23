#pragma once

#include "GameObjectManager/GameObjectManager.h"

#include <ComponentManager/AudioSource/AudioSource.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>

//std
#include<memory>
//behavior
#include"Objects/EnemyBehavior/EnemyRoot.h"
#include"Objects/EnemyBehavior/EnemyChasePlayer.h"


class Player;
class BuildingManager;
//class BossHead;
class Enemy : public BaseGameObject {
public:

	Enemy() { CreateTag(this); }
	~Enemy() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	//通常更新
	void RootInit();
	void RootUpdate();
	//ストーカー
	void ChaseInit();
	void ChaseUpdate();

	
	//プレイヤーセット
	void SetPlayer(Player*player);
	void SetBuildingaManager(BuildingManager* player);
	

	//状態変更
	void ChangeState(std::unique_ptr<BaseEnemyBehavior>behavior);
	//getter
	Quaternion GetPivotQuaternion()const { return pivot_.quaternion; }
	Player* GetPlayer() { return pPlayer_; }
	const std::pair<float, float>& GetPos() const { return pos_; }
	bool GetIsInTornado()const { return isInTornado_; }
	float GetSpeedParamater()const { return speedParamager_; }
	
	void OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision)override;

	//setter
	void SetPos(const std::pair<float, float>& pos) { pos_ = pos; }//緯度
	void SetPivotQuaternion(Quaternion pivot) { pivot_.quaternion = pivot; }
	void SetPivotSubtraction(Quaternion pivot) { pivot_.quaternion *= pivot; }

private:
	
private:
	//プレイヤーポインタ
	Player* pPlayer_=nullptr;
	BuildingManager* pBuildingManager_ = nullptr;
	
	//ライト
	class EarthRenderer* er_ = nullptr;
	 float radius_ = 1.0f;
	 Vec4  paintOutColor_ = { 1,1,1,1 };
	////状態
	std::unique_ptr<BaseEnemyBehavior>behavior_=nullptr;
	//座標
	std::pair<float, float> pos_;
	//ピボット
	Transform pivot_;
	
	bool isInTornado_;

	////////////////////////////////////////////////////////////////////////////////////////////
	//  パラメータ
	////////////////////////////////////////////////////////////////////////////////////////////
	float speedParamager_;

	//イージング
	float easeT_;
	float easeTimeMax_;
	float amplitude_;
	float period_;
	//距離
	float perceptionDistance_;
	bool isChase_;

	/// audio source
	class AudioSource* audioSource_ = nullptr;
};