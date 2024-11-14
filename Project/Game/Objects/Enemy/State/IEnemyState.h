#pragma once

#include <memory>

#include "Game/Objects/Enemy/BehaviorTree/Node.h"

class Enemy;
class IEnemyState{
public:
	IEnemyState(Enemy* enemy);
	virtual ~IEnemyState(){}

	virtual void Initialize() = 0;
	virtual void Update()     = 0;

protected:
	Enemy* enemy_;

	std::unique_ptr<EnemyBehaviorTree::Node> rootNode_;
};