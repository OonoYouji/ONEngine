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
	const float kAttackWaitTime_ = 1.2f;
	float attackFixationTime_;
	const float kAttackFixationTime_ = 1.8f;
	bool isSwingDown_;
	bool isFixation_;
	Quaternion inter_;



};
