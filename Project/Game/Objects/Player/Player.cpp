#include "Player.h"

#include "Behavior/IPlayerBehavior.h"
#include "Behavior/PlayerRootBehavior.h"

#include "ComponentManager/MeshRenderer/MeshRenderer.h"

#include "imgui.h"

Player::Player(){
	CreateTag(this);
}

Player::~Player(){}

void Player::Initialize(){
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Sphere");

	currentBehavior_.reset(new PlayerRootBehavior(this));


	/// hp parameters
	maxHP_     = 100.0f;
	currentHP_ = maxHP_;
}

void Player::Update(){
	currentBehavior_->Update();
}

void Player::Debug(){
	ImGui::DragFloat("Speed", &speed_,     0.1f, 0.1f);
	ImGui::DragFloat("HP",    &currentHP_, 0.1f, 0.0f, 100.0f);
}
