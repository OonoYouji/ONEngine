#pragma once
#include"Objects/EnemyBehavior/BaseEnemyBehavior.h"
//std



class EnemyRoot : public BaseEnemyBehavior {
public:
	//コンストラクタ
	EnemyRoot(Enemy* boss);
	~EnemyRoot();


	void Update()override;
private:
	//イージング
	float easeT_;
	float easeTimeMax_;
	float amplitude_;
	float period_;
	//距離
	float perceptionDistance_;
	bool isChase_;

};
