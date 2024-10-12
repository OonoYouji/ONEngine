#pragma once
#include"Objects/BossBehavior/BaseBossBehavior.h"


class BossChasePlayer : public BaseBossBehavior {
public:
	//コンストラクタ
	BossChasePlayer(Boss* boss);
	~BossChasePlayer();
	void Update()override;
private:
	//ストーカースピード(速い)
	float chaseSpeedMax_;
	//ストーカースピード(遅い)
	float chaseSpeedNormal_;
};
