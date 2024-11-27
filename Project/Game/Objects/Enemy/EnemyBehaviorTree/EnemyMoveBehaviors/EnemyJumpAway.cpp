#include "EnemyJumpAway.h"

#include <algorithm>
#include <numbers>
#include <cmath>

#include "Game/Objects/Enemy/BehaviorWorker/EnemyBehaviorWorkers.h"
#include "Game/Objects/Enemy/Enemy.h"
#include "Game/Objects/Enemy/EnemyBehaviorTree/EnemyBasicActions.h"
#include "Game/Objects/Player/Player.h"
#include "Math/Easing.h"

#include "FrameManager/Time.h"

float ParabolicEase(float t){
	if(t < 0.0f) t = 0.0f; // t を範囲内に制限
	if(t > 1.0f) t = 1.0f;

	return -4.0f * (t - 0.5f) * (t - 0.5f) + 1.0f;
}

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
								   float activeTime,
								   float distance)
		: Action(enemy){
		jumpSpeed_ = jumpSpeed;
		activeTime_ = activeTime;
		leftTime_ = 0.0f;
		distance_ = distance;

		currentUpdate_ = [this](){return InitTargetPoint(); };
	}

	Status JumpAwayAction::tick(){
		return currentUpdate_();
	}

	Status JumpAwayAction::InitTargetPoint(){
		beforePos_ = enemy_->GetPosition();

		Vector3 playerDirectionFromCenter = enemy_->GetPlayer()->GetPosition() * -1.0f;
		Vector2 targetPosXZ = {playerDirectionFromCenter.x,playerDirectionFromCenter.z};

		targetPosXZ = targetPosXZ.Normalize() * distance_;

		targetPoint_ = {targetPosXZ.x,0.0f,targetPosXZ.y};
		currentUpdate_ = [this](){return Jump(); };
		return Status::RUNNING;
	}

	Status JumpAwayAction::Jump(){
		Vector3 currentEnemyPos = enemy_->GetPosition();

		leftTime_ += Time::DeltaTime();
		float t = leftTime_ / activeTime_;
		currentEnemyPos.y = std::lerp(0.0f,jumpSpeed_,ParabolicEase(t));
		currentEnemyPos.x = std::lerp(beforePos_.x,targetPoint_.x,t);
		currentEnemyPos.z = std::lerp(beforePos_.z,targetPoint_.z,t);

		enemy_->SetPosition(currentEnemyPos);

		if(t >= 1.0f){
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
		addChild(std::make_unique<JumpAwayAction>(enemy,worker->jumpSpeed_,worker->motionTimes_.activeTime_,worker->distance_));

		addChild(std::make_unique<TransitionAnimation>(enemy,"Boss_Jump_3",worker->motionTimes_.endLagTime_,false));
		addChild(std::make_unique<Landing>(enemy,worker->motionTimes_.endLagTime_));
	}
}