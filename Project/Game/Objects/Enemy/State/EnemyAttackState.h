#pragma once
#include "IEnemyState.h"

class EnemyAttackState :
    public IEnemyState{
public:
	EnemyAttackState(Enemy* enemy):IEnemyState(enemy){}
	~EnemyAttackState(){}

	void Initialize()override;
	void Update()override;
private:

};