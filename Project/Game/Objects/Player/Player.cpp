#include "Player.h"

#include "Behavior/IPlayerBehavior.h"
#include "Behavior/PlayerAvoidanceBehavior.h"
#include "Behavior/PlayerRootBehavior.h"
#include "Behavior/PlayerWeakAttack.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "Input/Input.h"

#include "imgui.h"

Player::Player(GameCamera* _mainCamera) : pGameCamera_(_mainCamera) {
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

	workRootBehavior_.speed_ = 30.0f;
}

void Player::Update(){
	// ダメージを初期化
	damage_ = 0.0f;
	
	currentBehavior_->Update();
}

void Player::Debug(){
	ImGui::DragFloat("MaxHP",&maxHP_,0.1f);
	ImGui::InputFloat("HP",&currentHP_);
	ImGui::InputFloat("Power",&power_);

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

	ImGui::Spacing();

	if(ImGui::TreeNode("WeakAttack")){
		if(ImGui::TreeNode("Combo_1")){
			ImGui::DragFloat("StartUpTime_Combo_1",&workWeakAttackBehavior_[0].motionTimes_.startupTime_,0.1f);
			ImGui::DragFloat("ActiveTime_Combo_1",&workWeakAttackBehavior_[0].motionTimes_.activeTime_,0.1f);
			ImGui::DragFloat("EndLagTime_Combo_1",&workWeakAttackBehavior_[0].motionTimes_.endLagTime_,0.1f);
			ImGui::DragFloat("DamageFactor_Combo_1",&workWeakAttackBehavior_[0].damageFactor_,0.1f);
			ImGui::TreePop();
		}
		if(ImGui::TreeNode("Combo_2")){
			ImGui::DragFloat("StartUpTime_Combo_2",&workWeakAttackBehavior_[1].motionTimes_.startupTime_,0.1f);
			ImGui::DragFloat("ActiveTime_Combo_2",&workWeakAttackBehavior_[1].motionTimes_.activeTime_,0.1f);
			ImGui::DragFloat("EndLagTime_Combo_2",&workWeakAttackBehavior_[1].motionTimes_.endLagTime_,0.1f);
			ImGui::DragFloat("DamageFactor_Combo_2",&workWeakAttackBehavior_[1].damageFactor_,0.1f);
			ImGui::TreePop();
		}
		if(ImGui::TreeNode("Combo_3")){
			ImGui::DragFloat("StartUpTime_Combo_3",&workWeakAttackBehavior_[2].motionTimes_.startupTime_,0.1f);
			ImGui::DragFloat("ActiveTime_Combo_3",&workWeakAttackBehavior_[2].motionTimes_.activeTime_,0.1f);
			ImGui::DragFloat("EndLagTime_Combo_3",&workWeakAttackBehavior_[2].motionTimes_.endLagTime_,0.1f);
			ImGui::DragFloat("DamageFactor_Combo_3",&workWeakAttackBehavior_[2].damageFactor_,0.1f);
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}

void Player::TransitionBehavior(std::unique_ptr<IPlayerBehavior> next){
	currentBehavior_ = std::move(next);
}
