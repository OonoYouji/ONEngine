#include "EnemyJumpAway.h"

#include <algorithm>
#include <cmath>

#include "Game/Objects/Enemy/BehaviorWorker/EnemyBehaviorWorkers.h"
#include "Game/Objects/Enemy/Enemy.h"
#include "Game/Objects/Enemy/EnemyBehaviorTree/EnemyBasicActions.h"
#include "Game/Objects/Player/Player.h"
#include "Math/Easing.h"

#include "FrameManager/Time.h"

namespace EnemyBehaviorTree{

#pragma region"Startup"
	JumpAwayStartup::JumpAwayStartup(Enemy* enemy,float startupTime)
		:Action(enemy){
		leftTime_ = startupTime;
	}

	Status JumpAwayStartup::tick(){
		leftTime_ -= Time::DeltaTime();

		if(leftTime_ <= 0.0f){
			return Status::SUCCESS;
		}
		return Status::RUNNING;
	}
#pragma endregion

#pragma region"JumpAway"
	JumpAwayAction::JumpAwayAction(Enemy* enemy,
								   float jumpSpeed,
								   float mass,
								   float distance)
		: Action(enemy){
		jumpSpeed_ = jumpSpeed;
		mass_ = mass;
		distance_ = distance;

		currentUpdate_ = [this](){return InitTargetPoint(); };
	}

	Status JumpAwayAction::tick(){
		return currentUpdate_();
	}

	Status JumpAwayAction::InitTargetPoint(){
		Vector3 playerDirectionFromCenter = enemy_->GetPlayer()->GetPosition() * -1.0f;
		Vector2 targetPosXZ = {playerDirectionFromCenter.x,playerDirectionFromCenter.z};

		targetPosXZ = targetPosXZ.Normalize() * distance_;

		targetPoint_ = {targetPosXZ.x,0.0f,targetPosXZ.y};
		currentUpdate_ = [this](){return Jump(); };
		return Status::RUNNING;
	}

	Status JumpAwayAction::Jump(){
		const float kGravity = 9.8f;
		Vector3 currentEnemyPos = enemy_->GetPosition();

		jumpSpeed_ -= (kGravity * mass_) * Time::DeltaTime();
		currentEnemyPos.y += jumpSpeed_;
		currentEnemyPos.x = std::lerp(currentEnemyPos.x,targetPoint_.x,0.1f);
		currentEnemyPos.z = std::lerp(currentEnemyPos.z,targetPoint_.z,0.1f);

		enemy_->SetPosition(currentEnemyPos);

		if(enemy_->GetPosition().y <= 1.0f && jumpSpeed_ <= -0.1f){
			enemy_->SetPositionY(0.0f);
			return Status::SUCCESS;
		}
		return Status::RUNNING;
	}
#pragma endregion

#pragma region"EndLag"
	Landing::Landing(Enemy* enemy,float endLagTime)
		:Action(enemy){
		leftTime_ = endLagTime;
	}

	Status Landing::tick(){
		leftTime_ -= Time::DeltaTime();

		if(leftTime_ <= 0.0f){
			return Status::SUCCESS;
		}
		return Status::RUNNING;
	}

#pragma endregion

	JumpAway::JumpAway(Enemy* enemy,WorkJumpAwayAction* worker)
		:Sequence(enemy){

		addChild(std::make_unique<TransitionAnimation>(enemy,"Boss_Jump_1",worker->motionTimes_.startupTime_,false));
		addChild(std::make_unique<JumpAwayStartup>(enemy,worker->motionTimes_.startupTime_));

		addChild(std::make_unique<TransitionAnimation>(enemy,"Boss_Jump_2",-1.0f,false));
		addChild(std::make_unique<JumpAwayAction>(enemy,worker->jumpSpeed_,worker->mass_,worker->distance_));

		addChild(std::make_unique<TransitionAnimation>(enemy,"Boss_Jump_3",worker->motionTimes_.endLagTime_,false));
		addChild(std::make_unique<Landing>(enemy,worker->motionTimes_.endLagTime_));
	}
}