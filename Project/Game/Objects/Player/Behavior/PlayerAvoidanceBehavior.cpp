#include "PlayerAvoidanceBehavior.h"

#include "PlayerRootBehavior.h"

#include "VariableManager/VariableManager.h"
#include "FrameManager/Time.h"
#include "Math/Vector3.h"
#include "Math/Easing.h"
#include "Math/LerpShortAngle.h"

/// component
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"

/// objects
#include "Objects/Enemy/Enemy.h"


PlayerAvoidanceBehavior::PlayerAvoidanceBehavior(Player* _host):
	IPlayerBehavior(_host),
	workInBehavior_(host_->GetWorkAvoidanceBehavior()),
	workRootBehavior_(host_->GetWorkRootBehavior()) {

	currentTime_ = 0.0f;

	jastAvoidanceTime_ = host_->GetWorkAvoidanceBehavior().jastAvoidanceTime_;

	currentUpdate_ = [this](){StartupUpdate(); };
}

void PlayerAvoidanceBehavior::Update(){
	currentTime_ += Time::DeltaTime();
	currentUpdate_();
}

void PlayerAvoidanceBehavior::StartupUpdate(){

	{	// Rotate Update
		Vector3 rotate = host_->GetRotate();
		const Vec2& lastDir = host_->GetLastDirection();
		rotate.y = LerpShortAngle(rotate.y, std::atan2(lastDir.x, lastDir.y), workRootBehavior_.rotateLerpSensitivity_);
		host_->SetRotate(rotate);
	}


	if(currentTime_ >= workInBehavior_.motionTimes_.startupTime_){
		currentTime_ = 0.0f;

		beforePos_ = host_->GetPosition();
		Vector3 v3Direction = {0.0f,0.0f,1.0f};
		v3Direction = Matrix4x4::Transform(v3Direction,Matrix4x4::MakeRotateY(host_->GetRotate().y));
		afterPos_ = beforePos_ + (workInBehavior_.moveDistance_ * v3Direction);

		
		{	/// beforeとafterの間にボスがいるのか得る

			Enemy* pEnemy = host_->GetEnemy();

			Vec3  toEnemyVec         = pEnemy->GetPosition() - host_->GetPosition();
			float projectionValue    = Vec3::Dot((afterPos_ - beforePos_).Normalize(), toEnemyVec);

			if(projectionValue > 0.0f && projectionValue < (afterPos_ - beforePos_).Len()) {

				Vec3  projectionPosition = beforePos_ + (afterPos_ - beforePos_).Normalize() * projectionValue;

				/// TODO: 当たり判定がピッタリすぎて回避できないときがある
				Vec3  projectionPosToEnemy = pEnemy->GetPosition() - projectionPosition;
				float radius = host_->GetColliderRadius() + pEnemy->GetColliderRadius();

				/// 当たってたら
				if(projectionPosToEnemy.Len() < radius) {
					Vec3 afterPos = projectionPosition;
					afterPos += projectionPosToEnemy.Normalize() * (radius - projectionPosToEnemy.Len());;
					afterPos_ = afterPos;
				}
			}
		}

		// 無敵状態に 
		host_->SetIsInvisible(true);

		/// アニメーションを変更する
		host_->SetIsActiveWeapon(false);
		host_->SetAnimationModel("Player_Avoidance");
		host_->SetAnimationTotalTime(workInBehavior_.motionTimes_.activeTime_);
		host_->SetAnimationFlags(ANIMATION_FLAG_NOLOOP);

		currentUpdate_ = [this](){Avoidance(); };
		Avoidance();
		return;
	}
}

void PlayerAvoidanceBehavior::Avoidance(){
	float t = currentTime_ / workInBehavior_.motionTimes_.activeTime_;
	jastAvoidanceTime_ -= Time::DeltaTime();

	host_->SetPosition(Vector3::Lerp(
		beforePos_, afterPos_,
		Ease::Out::Sine(t)
	));

	if(currentTime_ >= workInBehavior_.motionTimes_.activeTime_){
		currentTime_ = 0.0f;

		// 無敵状態 を 解除
		host_->SetIsInvisible(false);

		currentUpdate_ = [this](){EndLagUpdate(); };
		EndLagUpdate();
		return;
	}
}

void PlayerAvoidanceBehavior::EndLagUpdate(){
	if(currentTime_ >= workInBehavior_.motionTimes_.endLagTime_){
		host_->TransitionBehavior(std::make_unique<PlayerRootBehavior>(host_));
		return;
	}
}