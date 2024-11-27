#define NOMINMAX
#include "GameStartEffect.h"

/// engine
#include "VariableManager/VariableManager.h"
#include "FrameManager/Time.h"

/// game
#include "ComponentManager/AudioSource/AudioSource.h"
#include "Objects/Enemy/Enemy.h"
#include "Objects/GameManagerObject/GameManagerObject.h"
#include "Objects/TrackingCamera/TrackingCamera.h"


GameStartEffect::GameStartEffect(std::vector<BaseGameObject*>& _gameObjectVector)
	: gameObjectVector_(_gameObjectVector) {
	CreateTag(this);
}

GameStartEffect::~GameStartEffect() {}

void GameStartEffect::Initialize() {

	se_ = AddComponent<AudioSource>();

	/// 演出を見せるために一旦すべてのオブジェクトのアクティブを切る
	camera_ = nullptr;
	enemy_ = nullptr;
	for(auto& obj : gameObjectVector_) {
		obj->isActive = false;

		if(obj->GetTag() == "Enemy") {
			enemy_ = static_cast<Enemy*>(obj);
			continue;
		}
		
		if(obj->GetTag() == "TrackingCamera") {
			camera_ = static_cast<TrackingCamera*>(obj);
			continue;
		}

	}

	maxEffectTime_ = 7.5f;
	seTriggerTime_ = 2.0f;
	transitionTime_ = 2.0f;

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson(
		"./Resources/Parameters/Objects", GetTag()
	);
	ApplyVariables();


	isRestart_ = GameManagerObject::GetFlag("isGameRestart");

}

void GameStartEffect::Update() {
	
	isFinish_.Update();
	isPlaySE_.Update();

	//ApplyVariables();
	transitionTime_ -= Time::DeltaTime();
	if(transitionTime_ > 0.0f) {
		return;
	}

	/// 演出の時間経過
	currentEffectTime_ += Time::DeltaTime();
	float lerpT = std::min(currentEffectTime_ / maxEffectTime_, 1.0F);

	if(lerpT == 1.0f) {
		isFinish_.current = true;
	}

	if(!isPlaySE_.Press() && !isRestart_.Press()) {
		if(seTriggerTime_ <= enemy_->GetBodyCurrentAnimationTime()) {
			isPlaySE_.current = true;
			se_->PlayOneShot("EnemyStart.wav", 0.1f);
			camera_->StartShake(0.3f, 1.0f, maxEffectTime_ - currentEffectTime_ - 1.3f);
			camera_->isActive = true;
			camera_->isInput = false;
		}
	}


	/// 演出が終わった瞬間
	if(isFinish_.Trigger()) {
		StartGame();
		camera_->isInput = true;
	}

}

void GameStartEffect::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "maxEffectTime", maxEffectTime_);
	vm->AddValue(groupName, "seTriggerTime", seTriggerTime_);
}

void GameStartEffect::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	maxEffectTime_ = vm->GetValue<float>(groupName, "maxEffectTime");
	seTriggerTime_ = vm->GetValue<float>(groupName, "seTriggerTime");
}

void GameStartEffect::StartGame() {
	currentEffectTime_ = maxEffectTime_;

	for(auto& obj : gameObjectVector_) {
		obj->isActive = true;
	}
}

void GameStartEffect::SetMaxEffectTime(float _time) {
	maxEffectTime_ = _time;
}

