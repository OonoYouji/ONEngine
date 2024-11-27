#define NOMINMAX
#include "EnemyKilledEffect.h"

/// engine
#include "VariableManager/VariableManager.h"
#include "FrameManager/Time.h"

#include "ComponentManager/AudioSource/AudioSource.h"
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"

/// game
#include "Objects/Enemy/Enemy.h"
#include "Objects/TrackingCamera/TrackingCamera.h"
#include "Objects/Player/Player.h"


EnemyKilledEffect::EnemyKilledEffect(const std::vector<BaseGameObject*>& _objects)
	: objectVector_(_objects) {
	CreateTag(this);
}

EnemyKilledEffect::~EnemyKilledEffect() {}

void EnemyKilledEffect::Initialize() {

	currentTime_ = 0.f;
	maxTime_ = 5.0f;

	for(auto& obj : objectVector_) {
		obj->isActive = false;
	}



	AddVariables();
	ApplyVariables();

	TrackingCamera* camera = nullptr;
	Enemy* enemy = nullptr;
	Player* player = nullptr;
	for(auto& obj : objectVector_) {
		if(obj->GetTag() == "Enemy") {
			enemy = static_cast<Enemy*>(obj);
			continue;
		}

		if(obj->GetTag() == "TrackingCamera") {
			camera = static_cast<TrackingCamera*>(obj);
			continue;
		}
		
		if(obj->GetTag() == "Player") {
			player = static_cast<Player*>(obj);
			continue;
		}

	}

	if(enemy) {
		enemy->SetAnimationRender("Boss_Dead");
		enemy->SetAnimationTotalTime(maxTime_);
		enemy->SetAnimationFlags(ANIMATION_FLAG_NOLOOP);
	}

	if(camera) {
		camera->StartShake(0.3f, 1.0f, maxTime_ - 0.5f);
		camera->isActive = true;
	}

	if(player) {
		player->SetAnimationFlags(1);
	}

	AudioSource* se = AddComponent<AudioSource>();
	se->volume = 0.1f;
	se->PlayOneShot("EnemyDead.wav", se->volume);


}

void EnemyKilledEffect::Update() {

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

void EnemyKilledEffect::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "maxTime", maxTime_);
}

void EnemyKilledEffect::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	maxTime_ = vm->GetValue<float>(groupName, "maxTime");
}

