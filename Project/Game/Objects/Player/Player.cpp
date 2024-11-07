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
	ImGui::DragFloat("MaxHP",&maxHP_,0.1f);
	ImGui::InputFloat("HP",&currentHP_);

	if(ImGui::Button("Heal Hp For Max")){
		currentHP_ = maxHP_;
	}

	if(ImGui::TreeNode("RootBehavior")){
		ImGui::DragFloat("Speed_InRootBehavior",&workRootBehavior_.speed_,0.1f);
		ImGui::DragFloat("RotateLerpSensitivity_InRootBehavior",&workRootBehavior_.rotateLerpSensitivity_,0.1f);

		ImGui::TreePop();
	}

	ImGui::Spacing();

	if(ImGui::TreeNode("AvoidanceBehavior")){
		ImGui::DragFloat("MoveDistance_InAvoidanceBehavior",&workAvoidanceBehavior_.moveDistance_,0.1f);
		ImGui::DragFloat("StartUpTime_InAvoidanceBehavior",&workAvoidanceBehavior_.motionTimes_.startupTime_,0.1f);
		ImGui::DragFloat("ActiveTime_InAvoidanceBehavior",&workAvoidanceBehavior_.motionTimes_.activeTime_,0.1f);
		ImGui::DragFloat("EndLagTime_InAvoidanceBehavior",&workAvoidanceBehavior_.motionTimes_.endLagTime_,0.1f);

		ImGui::TreePop();
	}
}

void Player::TransitionBehavior(std::unique_ptr<IPlayerBehavior> next){
	currentBehavior_ = std::move(next);
}
