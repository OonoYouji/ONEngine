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
class BossHeadEr;
class BossHead : public BaseGameObject {
public:

	BossHead() { CreateTag(this); }
	~BossHead() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	//パラメータ初期化
	void ParamaterInit();
	void ParentBoss();
	void ParentTubu();

	bool GetIsAttackCollision()const { return isAttackCollision_; }

	void RootInit();
	void AttackInit();
	void AttackUpdate();

	void SetIsAttackCollision(bool is) { isAttackCollision_ = is; }
	void SetBossTube(BossTubu* bossTube);

private:

private:

	bool isAttackCollision_;

	BossTubu* pBossTube_;
	float floatingCycle_;
	float floatingAmplitude_;
	float floatingParameter_;
	const float easeTimeMax_ = 0.5f;
	bool isRootinit_;
	bool isAttackInit_;

	BossHeadEr* bossHeadEr_=nullptr;
};

class BossHeadEr : public BaseGameObject {
public:

	BossHeadEr() { CreateTag(this); }
	~BossHeadEr() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void RootInit();
	void AttackInit();

	//void SetBoss(Boss* bossTube);

private:
	Boss* pBoss_;


	
	//ライト
	class EarthRenderer* er_ = nullptr;
	float radius_ = 1.0f;
	Vec4  paintOutColor_ = { 1,1,1,1 };
};