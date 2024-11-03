#pragma once

#include <memory>
namespace EnemyBehaviorTree{ class Node; }

class Enemy;
class EnemyAI{
public:
	EnemyAI(Enemy* _enemy);

	void Setup();
	void Update();

private:
	Enemy* enemy_ = nullptr;
	std::unique_ptr<EnemyBehaviorTree::Node> rootNode_;
};