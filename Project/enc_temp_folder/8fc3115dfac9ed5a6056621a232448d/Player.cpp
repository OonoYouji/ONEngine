#include "Player.h"

/// engine
#include "VariableManager/VariableManager.h"
#include "Input/Input.h"
#include "FrameManager/Time.h"

#include "ComponentManager/MeshRenderer/MeshRenderer.h"





Player::Player() {
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




	/// ===================================================
	/// json variable io
	/// ===================================================

	AddVariables();
	ApplyVariables();

}

void Player::Update() {
	ApplyVariables();
	Movement();
}


/// ===================================================
/// variable io
/// ===================================================

void Player::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "movementSpeed", movementSpeed_);

}

void Player::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	movementSpeed_ = vm->GetValue<float>(groupName, "movementSpeed");

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


	pTransform_->position += velocity_;

}

