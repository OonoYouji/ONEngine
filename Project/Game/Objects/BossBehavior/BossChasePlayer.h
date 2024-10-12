#pragma once
#include"Objects/BossBehavior/BaseBossBehavior.h"


class BossChasePlayer : public BaseBossBehavior {
public:
	//コンストラクタ
	BossChasePlayer(Boss* boss);
	~BossChasePlayer();
	void Update()override;
};
