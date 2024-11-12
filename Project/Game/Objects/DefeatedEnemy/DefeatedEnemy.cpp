#include "DefeatedEnemy.h"

/// std
#include <numbers>

/// engine
#include "FrameManager/Time.h"

/// engine
#include "VariableManager/VariableManager.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// scenes
#include "Scenes/Scene_Game.h"

DefeatedEnemy::DefeatedEnemy() {
	CreateTag(this);
}

DefeatedEnemy::~DefeatedEnemy() {}

void DefeatedEnemy::Initialize() {

	drawLayerId = SCENE_GAME_LAYER_DEFEATED_ENEMY;
	
	
	/// ---------------------------------------------------
	/// json保存の保存、読み込み
	/// ---------------------------------------------------

	VariableManager* vm = VariableManager::GetInstance();
	
	const std::string& groupName = GetTag();
	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "rotate",   pTransform_->rotate);
	vm->AddValue(groupName, "scale",    pTransform_->scale);

	vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", groupName);

	ApplyVariables();


	/// ---------------------------------------------------
	/// mesh renderer initialize
	/// ---------------------------------------------------

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Enemy");
	meshRenderer_->SetMaterial("white2x2");
	meshRenderer_->SetIsLighting(false);


	isActive = false;
}

void DefeatedEnemy::Update() {

	activeLifeTime_ -= Time::DeltaTime();
	
	pTransform_->rotate.y = std::fmod(
		pTransform_->rotate.y + (0.5f * Time::DeltaTime()),
		2.0f * std::numbers::pi_v<float>
	);


	if(activeLifeTime_ < 0.0f) {
		meshRenderer_->isActive = false;
	}

}



void DefeatedEnemy::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();

	const std::string& groupName = GetTag();
	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->rotate   = vm->GetValue<Vec3>(groupName, "rotate");
	pTransform_->scale    = vm->GetValue<Vec3>(groupName, "scale");
}

void DefeatedEnemy::StartRotate() {
	meshRenderer_->isActive = true;
	activeLifeTime_ = 2.0f;
}

