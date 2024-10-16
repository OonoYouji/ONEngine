#pragma once

#include"Objects/BossBehavior/BaseBossBehavior.h"

class BossAttack : public BaseBossBehavior {
public:
	//コンストラクタ
	BossAttack(Boss* boss);
	~BossAttack();
	void Update()override;

private:

	float attackWaitTime_;
	const float kAttackWaitTime_ = 1.0f;
	bool isSwingDown_;
};
