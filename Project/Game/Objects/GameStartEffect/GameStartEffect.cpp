#define NOMINMAX
#include "GameStartEffect.h"

/// engine
#include "VariableManager/VariableManager.h"
#include "FrameManager/Time.h"


GameStartEffect::GameStartEffect(std::vector<BaseGameObject*>& _gameObjectVector)
	: gameObjectVector_(_gameObjectVector) {
	CreateTag(this);
}

GameStartEffect::~GameStartEffect() {}

void GameStartEffect::Initialize() {

	/// 演出を見せるために一旦すべてのオブジェクトのアクティブを切る
	for(auto& obj : gameObjectVector_) {
		obj->isActive = false;
	}

	maxEffectTime_ = 10.0f;
	AddVariables();
	ApplyVariables();

}

void GameStartEffect::Update() {
	
	isFinish_.Update();

	ApplyVariables();


	/// 演出の時間経過
	currentEffectTime_ += Time::DeltaTime();
	float lerpT = std::min(currentEffectTime_ / maxEffectTime_, 1.0F);

	if(lerpT == 1.0f) {
		isFinish_.current = true;
	}


	/// 演出が終わった瞬間
	if(isFinish_.Trigger()) {
		StartGame();
	}

}

void GameStartEffect::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "maxEffectTime", maxEffectTime_);
}

void GameStartEffect::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	maxEffectTime_ = vm->GetValue<float>(groupName, "maxEffectTime");
}

void GameStartEffect::StartGame() {
	for(auto& obj : gameObjectVector_) {
		obj->isActive = true;
	}
}

void GameStartEffect::SetMaxEffectTime(float _time) {
	maxEffectTime_ = _time;
}

