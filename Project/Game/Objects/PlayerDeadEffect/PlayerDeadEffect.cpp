#define NOMINMAX
#include "PlayerDeadEffect.h"


/// engine
#include "FrameManager/Time.h"
#include "ComponentManager/AudioSource/AudioSource.h"
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"

/// game
#include "Objects/Player/Player.h"




PlayerDeadEffect::PlayerDeadEffect(std::vector<BaseGameObject*>& _objects)
	: objectVector_(_objects) {
	CreateTag(this);
}

PlayerDeadEffect::~PlayerDeadEffect() {}

void PlayerDeadEffect::Initialize() {

	currentTime_ = 0.f;
	maxTime_ = 5.0f;

	for(auto& obj : objectVector_) {
		obj->isActive = false;
	}



	//AddVariables();
	//ApplyVariables();

	//TrackingCamera* camera = nullptr;
	Player* player = nullptr;
	for(auto& obj : objectVector_) {
		if(obj->GetTag() == "Player") {
			player = static_cast<Player*>(obj);
			continue;
		}

		/*if(obj->GetTag() == "TrackingCamera") {
			camera = static_cast<TrackingCamera*>(obj);
			continue;
		}*/
	}

	if(player) {
		player->SetAnimationModel("Player_Dead");
		player->SetAnimationTotalTime(maxTime_);
		player->SetAnimationFlags(ANIMATION_FLAG_NOLOOP);
		player->SetIsActiveWeapon(false);
	}

	/*if(camera) {
		camera->StartShake(0.3f, 1.0f, maxTime_ - 0.5f);
		camera->isActive = true;
	}*/

	AudioSource* se = AddComponent<AudioSource>();
	se->volume = 0.1f;
	se->PlayOneShot("EnemyDead.wav", se->volume);


}

void PlayerDeadEffect::Update() {

	isFinish_.Update();

	currentTime_ += Time::DeltaTime();
	float lerpT = std::min(currentTime_ / maxTime_, 1.0f);

	if(lerpT == 1.0f) {
		isFinish_.current = true;
	}


	/// 終了した
	if(isFinish_.Trigger()) {

	}

}

