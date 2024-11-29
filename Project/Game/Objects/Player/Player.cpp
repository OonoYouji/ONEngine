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

/// graphics
#include "GraphicManager/ModelManager/ModelManager.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

#include "Math/LerpShortAngle.h"

/// game
#include "Objects/Camera/GameCamera.h"



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
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Player");

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
	Movement();
	Rotation();
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

	vm->AddValue(groupName, "movementSpeed", movementSpeed_);
	vm->AddValue(groupName, "rotateSpeed", rotateSpeed_);

}

void Player::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	movementSpeed_ = vm->GetValue<float>(groupName, "movementSpeed");
	rotateSpeed_ = vm->GetValue<float>(groupName, "rotateSpeed");

}



void Player::LoadingBehaviors() {

}


/// ===================================================
/// member methods
/// ===================================================

void Player::Movement() {

	Vec2 inputLeftStick = Input::GetLeftStick();
	inputLeftStick = inputLeftStick.Normalize();
	velocity_ = {
		inputLeftStick.x * movementSpeed_ * Time::DeltaTime(),
		0.0f,
		inputLeftStick.y * movementSpeed_ * Time::DeltaTime(),
	};

	auto GetYawFromQuaternion = [](const Quaternion& q) {
		return std::atan2(
			2.0f * (q.y * q.w + q.x * q.z),
			1.0f - 2.0f * (q.x * q.x + q.y * q.y)
		);
	};

	Mat4 matCameraRotateY = Mat4::MakeRotateY(GetYawFromQuaternion(pGameCamera_->GetQuaternion()));
	velocity_ = Mat4::TransformNormal(velocity_, matCameraRotateY);

	pTransform_->position += velocity_;

}

void Player::Rotation() {
	if(velocity_ != Vec3(0, 0, 0)) {
		prevDirection_ = velocity_;
	}

	Vec3  dir = velocity_.Normalize();
	float nextRotateY = std::atan2(prevDirection_.x, prevDirection_.z);

	pTransform_->rotate.y = LerpShortAngle(pTransform_->rotate.y, nextRotateY, rotateSpeed_);
}

