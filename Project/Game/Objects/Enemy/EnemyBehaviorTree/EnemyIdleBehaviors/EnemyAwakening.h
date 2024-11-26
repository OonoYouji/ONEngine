#pragma once

#include <functional>
#include <memory>
#include <string>

#include "../Node.h"

class AnimationRenderer;
namespace EnemyBehaviorTree{
	class EnemyAwakeningAction
		:public Action{
	public:
		EnemyAwakeningAction(Enemy* enemy);
		~EnemyAwakeningAction(){}

		Status tick()override;
	private:
		std::function<Status()>currentUpdate_;
		Status Init();
		Status Update();

		float leftTime_;
	};

	class EnemyLoadAwakeAnimation
		:public Action{
	public:
		EnemyLoadAwakeAnimation(Enemy* enemy);
		~EnemyLoadAwakeAnimation(){}

		Status tick()override;
	private:
		std::string animation_[4];
	};

	class EnemyAwakening
		:public Sequence{
	public:
		EnemyAwakening(Enemy* enemy);
		~EnemyAwakening()override{}
	};

}