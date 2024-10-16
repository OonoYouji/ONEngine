#pragma once

#include"Objects/BossBehavior/BaseBossBehavior.h"


class BossAttack : public BaseBossBehavior {
public:
	//コンストラクタ
	BossAttack(Boss* boss);
	~BossAttack();
	void Update()override;

private:
	float easingTime_;
	float easingTimeMax_;
	float easeDirection_;
	float MinDistance_;
};
