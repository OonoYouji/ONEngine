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
#include"Objects/BossBehavior/BossBulletShot.h"

///////////////////////////////////////////////////////////////////////////////////////////
// BossTube
////////////////////////////////////////////////////////////////////////////////////////////

class Boss;
class BossTubu : public BaseGameObject {
public:

	BossTubu() { CreateTag(this); }
	~BossTubu() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void ParamaterInit();
	//getter
	Boss* GetBoss()const { return pBoss_; }
	//setter
	void SetBoss(Boss* boss);

private:
	float floatingCycle_;
	float floatingAmplitude_;
	float floatingParameter_;
	const float easeTimeMax_ = 0.5f;

private:
	Boss* pBoss_;
};

///////////////////////////////////////////////////////////////////////////////////////////
// Bosshead
////////////////////////////////////////////////////////////////////////////////////////////
class BossTubu;
class BossHead : public BaseGameObject {
public:

	BossHead() { CreateTag(this); }
	~BossHead() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;
	//パラメータ


	//パラメータ初期化
	void ParamaterInit();

	void SetBossTube(BossTubu* bossTube);

private:

private:
	BossTubu* pBossTube_;
	float floatingCycle_;
	float floatingAmplitude_;
	float floatingParameter_;
	const float easeTimeMax_ = 0.5f;
	bool isInit_;
};