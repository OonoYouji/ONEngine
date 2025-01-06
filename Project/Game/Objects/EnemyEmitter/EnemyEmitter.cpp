#include "EnemyEmitter.h"

/// std
#include <algorithm>

/// external
#include <imgui.h>

/// engine
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "VariableManager/VariableManager.h"
#include "FrameManager/Time.h"
#include "Math/Easing.h"
#include "Math/Random.h"

/// user
#include "../Enemy/Enemy.h"


EnemyEmitter::EnemyEmitter(class Player* _player)
	: pPlayer_(_player) {
	CreateTag(this);
}

EnemyEmitter::~EnemyEmitter() {}

void EnemyEmitter::Initialize() {
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("EnemyEmitter");

	config_ = {};
	config_.emitEnemyNum = 5;
	config_.radius       = 5.0f;

	isStart_          = false;
	animationTime_    = 0.0f;
	maxAnimationTime_ = 2.0f;

	minScale_ = 1.0f;

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson(
		"./Resources/Parameters/Objects", GetTag()
	);
	ApplyVariables();

}

void EnemyEmitter::Update() {

	ApplyVariables();


	/// startしてなければ終了
	if(!isStart_) { 
		config_.activionTime -= Time::DeltaTime();
		if(config_.activionTime <= 0.0f) {
			Start();
		}
		return; 
	} 

	if(isEnd_) { return; }    /// endしていたら終了

	animationTime_ += Time::DeltaTime();
	float lerpT = std::clamp(animationTime_ / maxAnimationTime_, 0.0f, 1.0f);
	
	pTransform_->scale = Vec3::Lerp(
		{ minScale_, minScale_, minScale_ }, 
		Vec3::kOne * config_.radius,
		Ease::InOut::Expo(lerpT)
	);

	if(lerpT == 1.0f) {
		isEnd_ = true;
		Emit();
	}

}

void EnemyEmitter::Debug() {

	if(ImGui::Button("start")) {
		Start();
	}

}

void EnemyEmitter::Start() {
	isStart_           = true;
	animationTime_     = 0.0f;
	pTransform_->scale = { minScale_, minScale_, minScale_ };
}

void EnemyEmitter::Emit() {

	for(size_t i = 0; i < config_.emitEnemyNum; ++i) {
		/// emit enemy
		Enemy* enemy = new Enemy(pPlayer_);
		enemy->Initialize();

		Vec3 position = pTransform_->position;
		position += Random::Vec3(-Vec3::kOne * config_.radius, Vec3::kOne * config_.radius);
		position.y = 0.0f;

		enemy->SetPosition(position);
		enemyList_.push_back(enemy);
	}
}

void EnemyEmitter::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "minScale",         minScale_);
	vm->AddValue(groupName, "maxAnimationTime", maxAnimationTime_);
}

void EnemyEmitter::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	minScale_         = vm->GetValue<float>(groupName, "minScale");
	maxAnimationTime_ = vm->GetValue<float>(groupName, "maxAnimationTime");
}

