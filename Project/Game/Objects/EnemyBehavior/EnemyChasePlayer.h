#pragma once
#include"Objects/EnemyBehavior/BaseEnemyBehavior.h"
//std



class EnemyChasePlayer : public BaseEnemyBehavior {
public:
	//コンストラクタ
	EnemyChasePlayer(Enemy* boss);
	~EnemyChasePlayer();


	void Update()override;
private:
	//ストーカースピード(速い)
	float chaseSpeedMax_;
	//ストーカースピード(遅い)
	float chaseSpeedNormal_;
	//追っかける最小距離
	float chaseMinPos_;
	//追っかける限界距離
	float chaseMaxPos_;

};
