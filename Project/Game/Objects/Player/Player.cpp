#include "Player.h"

/// std
#include <numbers>

/// externals
#include <imgui.h>

/// engine
#include "VariableManager/VariableManager.h"
#include "Input/Input.h"

/// component
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"

/// this behavior
#include "Behavior/IPlayerBehavior.h"
#include "Behavior/PlayerAvoidanceBehavior.h"
#include "Behavior/PlayerRootBehavior.h"
#include "Behavior/PlayerWeakAttack.h"



Player::Player(GameCamera* _mainCamera) : pGameCamera_(_mainCamera) {
	CreateTag(this);
}

Player::~Player() {}

void Player::Initialize() {

	animationRenderer_ = AddComponent<AnimationRenderer>("KariPlayer_Wait");

	currentBehavior_.reset(new PlayerRootBehavior(this));

	/// hp parameters
	maxHP_ = 100.0f;
	currentHP_ = maxHP_;

	workRootBehavior_.speed_ = 30.0f;
	//振り向き
	workRootBehavior_.rotateLerpSensitivity_ = 0.1f;

	//回避
	workAvoidanceBehavior_.moveDistance_ = 10;
	workAvoidanceBehavior_.motionTimes_.activeTime_ = 0.8f;



	/// varialbe managerに値を追加する
	AddVariables();
	LoadVariables();
	ApplyVariables();


}

void Player::Update() {
	ApplyVariables();

	// ダメージを初期化
	damage_ = 0.0f;

	currentBehavior_->Update();

}

void Player::Debug() {

	ImGui::DragFloat("MaxHP", &maxHP_, 0.1f);
	ImGui::InputFloat("HP", &currentHP_);
	ImGui::InputFloat("Power", &power_);

	if(ImGui::Button("Heal Hp For Max")) {
		currentHP_ = maxHP_;
	}

	if(ImGui::TreeNode("RootBehavior")) {
		ImGui::DragFloat("Speed_InRootBehavior", &workRootBehavior_.speed_, 0.1f);
		ImGui::DragFloat("RotateLerpSensitivity_InRootBehavior", &workRootBehavior_.rotateLerpSensitivity_, 0.1f);

		ImGui::TreePop();
	}

	ImGui::Spacing();

	if(ImGui::TreeNode("AvoidanceBehavior")) {
		ImGui::DragFloat("MoveDistance_InAvoidanceBehavior", &workAvoidanceBehavior_.moveDistance_, 0.1f);
		ImGui::DragFloat("StartUpTime_InAvoidanceBehavior", &workAvoidanceBehavior_.motionTimes_.startupTime_, 0.1f);
		ImGui::DragFloat("ActiveTime_InAvoidanceBehavior", &workAvoidanceBehavior_.motionTimes_.activeTime_, 0.1f);
		ImGui::DragFloat("EndLagTime_InAvoidanceBehavior", &workAvoidanceBehavior_.motionTimes_.endLagTime_, 0.1f);

		ImGui::TreePop();
	}

	ImGui::Spacing();

	if(ImGui::TreeNode("WeakAttack")) {
		if(ImGui::TreeNode("Combo_1")) {
			ImGui::DragFloat("StartUpTime_Combo_1", &workWeakAttackBehavior_[0].motionTimes_.startupTime_, 0.1f);
			ImGui::DragFloat("ActiveTime_Combo_1", &workWeakAttackBehavior_[0].motionTimes_.activeTime_, 0.1f);
			ImGui::DragFloat("EndLagTime_Combo_1", &workWeakAttackBehavior_[0].motionTimes_.endLagTime_, 0.1f);
			ImGui::DragFloat("DamageFactor_Combo_1", &workWeakAttackBehavior_[0].damageFactor_, 0.1f);
			ImGui::TreePop();
		}

		if(ImGui::TreeNode("Combo_2")) {
			ImGui::DragFloat("StartUpTime_Combo_2", &workWeakAttackBehavior_[1].motionTimes_.startupTime_, 0.1f);
			ImGui::DragFloat("ActiveTime_Combo_2", &workWeakAttackBehavior_[1].motionTimes_.activeTime_, 0.1f);
			ImGui::DragFloat("EndLagTime_Combo_2", &workWeakAttackBehavior_[1].motionTimes_.endLagTime_, 0.1f);
			ImGui::DragFloat("DamageFactor_Combo_2", &workWeakAttackBehavior_[1].damageFactor_, 0.1f);
			ImGui::TreePop();
		}
		if(ImGui::TreeNode("Combo_3")) {
			ImGui::DragFloat("StartUpTime_Combo_3", &workWeakAttackBehavior_[2].motionTimes_.startupTime_, 0.1f);
			ImGui::DragFloat("ActiveTime_Combo_3", &workWeakAttackBehavior_[2].motionTimes_.activeTime_, 0.1f);
			ImGui::DragFloat("EndLagTime_Combo_3", &workWeakAttackBehavior_[2].motionTimes_.endLagTime_, 0.1f);
			ImGui::DragFloat("DamageFactor_Combo_3", &workWeakAttackBehavior_[2].damageFactor_, 0.1f);
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}

void Player::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	/// work root behavior data
	vm->AddValue(groupName, "speed",                 workRootBehavior_.speed_);
	vm->AddValue(groupName, "rotateLerpSensitivity", workRootBehavior_.rotateLerpSensitivity_);

	/// avoidance behavior
	vm->AddValue(groupName, "moveDistance",    workAvoidanceBehavior_.moveDistance_);
	vm->AddValue(groupName, "workStartupTime", workAvoidanceBehavior_.motionTimes_.startupTime_);
	vm->AddValue(groupName, "workActiveTime",  workAvoidanceBehavior_.motionTimes_.activeTime_);
	vm->AddValue(groupName, "workEndLagTime",  workAvoidanceBehavior_.motionTimes_.endLagTime_);
}

void Player::LoadVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", groupName);
}

void Player::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	/// work root behavior data
	workRootBehavior_.speed_                 = vm->GetValue<float>(groupName, "speed");
	workRootBehavior_.rotateLerpSensitivity_ = vm->GetValue<float>(groupName, "rotateLerpSensitivity");

	/// avoidance behavior
	workAvoidanceBehavior_.moveDistance_             = vm->GetValue<float>(groupName, "moveDistance");
	workAvoidanceBehavior_.motionTimes_.startupTime_ = vm->GetValue<float>(groupName, "workStartupTime");
	workAvoidanceBehavior_.motionTimes_.activeTime_  = vm->GetValue<float>(groupName, "workActiveTime");
	workAvoidanceBehavior_.motionTimes_.endLagTime_  = vm->GetValue<float>(groupName, "workEndLagTime");

}

void Player::TransitionBehavior(std::unique_ptr<IPlayerBehavior> next) {
	currentBehavior_ = std::move(next);
}



void Player::SetAnimationModel(const std::string& _filePath) {
	animationRenderer_->ChangeAnimation(_filePath);
}
