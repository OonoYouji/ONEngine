#pragma once
#include"Objects/BossBehavior/BaseBossBehavior.h"

class BossRoot : public BaseBossBehavior {
public:
	//コンストラクタ
	BossRoot(Boss* boss);
	~BossRoot();
	void Update()override;
};
