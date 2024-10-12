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

	//プレイヤーセット
	void SetPlayer(Player*player);

	//状態変更
	void ChangeState(std::unique_ptr<BaseBossBehavior>behavior);
	
private:
	
private:
	//プレイヤーポインタ
	Player* pPlayer_=nullptr;
	////状態
	std::unique_ptr<BaseBossBehavior>behavior_=nullptr;
	//ピボット
	Transform pivot_;
	//ストーカースピード(速い)
	float chaseSpeedMax_;
	//ストーカースピード(遅い)
	float chaseSpeedNormal_;
};