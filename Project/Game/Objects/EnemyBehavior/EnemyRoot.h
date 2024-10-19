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


};
