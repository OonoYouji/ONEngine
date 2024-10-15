#pragma once

#include"Objects/BossBehavior/BaseBossBehavior.h"


class BossSlurp : public BaseBossBehavior {
public:
	//コンストラクタ
	BossSlurp(Boss* boss);
	~BossSlurp();
	void Update()override;

private:
	float easingTime_;
	float easingTimeMax_;
	float easeDirection_;
	float MinDistance_;
};
