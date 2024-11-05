#pragma once
#include "IEnemyState.h"

class EnemyChaseState :
	public IEnemyState{
public:
	EnemyChaseState(Enemy* enemy,float gaugeLength):IEnemyState(enemy),gaugeLength_(gaugeLength){}
	~EnemyChaseState(){}

	void Initialize()override;
	void Update()override;
private:
	float gaugeLength_;
	float maxTime_ = 3.0f;
	float currentTime_;
};