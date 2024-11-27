#pragma once

#include <functional>

#include "../Node.h"

#include "Math/Vector3.h"

class WorkJumpAwayAction;
namespace EnemyBehaviorTree{

	class JumpAwayStartup
		:public EnemyBehaviorTree::Action{
	public:
		JumpAwayStartup(Enemy* enemy
						,float startupTime);
		~JumpAwayStartup()override{}

		Status tick()override;
	private:
		float leftTime_;
	};

	class JumpAwayAction
		:public EnemyBehaviorTree::Action{
	public:
		JumpAwayAction(Enemy* enemy,
					   float jumpSpeed,
					   float activeTime,
					   float distance);

		Status tick()override;
	private:
		std::function<Status()> currentUpdate_;
		Status InitTargetPoint();
		Status Jump();
	private:
		Vector3 beforePos_;
		Vector3 targetPoint_;
		float jumpSpeed_;
		float leftTime_;
		float activeTime_;
		float distance_;
	};

	class Landing
		:public EnemyBehaviorTree::Action{
	public:
		Landing(Enemy* enemy,
				float endLagTime);
		~Landing()override{}

		Status tick()override;
	private:
		float leftTime_;
	};

	class JumpAway
		:public EnemyBehaviorTree::Sequence{
	public:
		JumpAway(Enemy* enemy,WorkJumpAwayAction* worker);
		~JumpAway()override{}
	};
}