#define NOMINMAX
#include "Player.h"

/// std
#include <algorithm>
#include <cassert>

/// engine
#include "VariableManager/VariableManager.h"
#include "Input/Input.h"
#include "ImGuiManager/ImGuiManager.h"
#include "FrameManager/Time.h"


#include "Math/LerpShortAngle.h"

/// game
#include "Objects/Camera/GameCamera.h"
#include "Mesh/PlayerMesh.h"


Player::Player(GameCamera* _gameCamera)
	: pGameCamera_(_gameCamera) {
	CreateTag(this);
}

Player::~Player() {}

void Player::Initialize() {

	/// ===================================================
	/// setting
	/// ===================================================

	/// component setting
	mesh_ = new PlayerMesh();
	mesh_->Initialize();
	mesh_->SetParent(pTransform_);

	/// behavior
	behaviorManager_.reset(new PlayerBehaviorManager(this));
	behaviorManager_->Initialize();
	

	/// ===================================================
	/// json variable io
	/// ===================================================

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson(
		"./Resources/Parameters/Objects", GetTag()
	);
	ApplyVariables();
}

void Player::Update() {
	ApplyVariables();

	behaviorManager_->Update();

	const MotionKeyframe& keyframe = behaviorManager_->GetMotion()->GetMotionKeyframe();
	pTransform_->position = currentCommonData_.position + keyframe.position;
	pTransform_->rotate   = keyframe.rotate;
	pTransform_->scale    = keyframe.scale;
}

void Player::Debug() {
	behaviorManager_->Debugging();
}


/// ===================================================
/// variable io
/// ===================================================

void Player::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "movementSpeed", currentCommonData_.movementSpeed);
	vm->AddValue(groupName, "useGravity",    currentCommonData_.useGravity);

}

void Player::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	currentCommonData_.movementSpeed = vm->GetValue<float>(groupName, "movementSpeed");
	currentCommonData_.useGravity    = vm->GetValue<bool>(groupName, "useGravity");

}



void Player::LoadingBehaviors() {

}

