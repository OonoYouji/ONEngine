#define NOMINMAX
#include "Player.h"

/// std
#include <numbers>

/// externals
#include <imgui.h>

/// engine
#include "VariableManager/VariableManager.h"
#include "Input/Input.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "FrameManager/Time.h"

/// component
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"
#include "ComponentManager/Collider/SphereCollider.h"

/// this behavior
#include "Behavior/IPlayerBehavior.h"
#include "Behavior/PlayerAvoidanceBehavior.h"
#include "Behavior/PlayerRootBehavior.h"
#include "Behavior/PlayerWeakAttack.h"

/// objects
#include "Collision/PlayerAttackCollider/PlayerAttackCollider.h"
#include "Objects/Enemy/Enemy.h"
#include "Objects/EntityShadow/EntityShadow.h"
#include "Objects/TrackingCamera/TrackingCamera.h"
#include "Objects/Camera/GameCamera.h"


Player::Player() {
	CreateTag(this);
}

Player::~Player() {}

void Player::Initialize() {

	//sphereCollider_ = AddComponent<SphereCollider>(ModelManager::Load("Sphere"));

	bodyAnimationRenderer_   = AddComponent<AnimationRenderer>("Player_Wait");
	weaponAnimationRenderer_ = AddComponent<AnimationRenderer>("Player_Wait");

	SetAnimationFlags(0);

	/// ボディの先読み
	const std::array<std::string, 3> bodyModelAnimationFilePaths{
		"Player_WeakAttack1_1_P", "Player_WeakAttack1_2_P", "Player_WeakAttack1_3_P"
	};
	/// 武器の先読み
	const std::array<std::string, 3> weaponModelAnimationFilePaths{
		"Player_WeakAttack1_1_W", "Player_WeakAttack1_2_W", "Player_WeakAttack1_3_W"
	};

	for(size_t i = 0; i < 3; ++i) {
		SetAnimationModel(
			bodyModelAnimationFilePaths[i],
			weaponModelAnimationFilePaths[i]
		);
	}


	/// 攻撃のモーションを先に読み込んでおく
	const std::array<std::string, 3> weakAnimationFilePaths{
		"KariPlayer_WeakAttack", "KariPlayer_WeakAttack1", "KariPlayer_WeakAttack2"
	};

	for(auto& filePath : weakAnimationFilePaths) {
		SetAnimationModel(filePath);
	}


	SetAnimationModel("Player_Wait"); /// 元のアニメーションに変更
	SetIsActiveWeapon(false);



	currentBehavior_.reset(new PlayerRootBehavior(this));

	/// hp parameters
	maxHP_ = 100.0f;
	currentHP_ = maxHP_;

	weaponSpawnMaxTime_ = 0.5f;
	workRootBehavior_.speed_ = 30.0f;
	//振り向き
	workRootBehavior_.rotateLerpSensitivity_ = 0.1f;

	//回避
	workAvoidanceBehavior_.moveDistance_ = 10;
	workAvoidanceBehavior_.motionTimes_.activeTime_ = 0.8f;


	/// ---------------------------------------------------
	/// 攻撃のアクティブタイムをアニメーションで初期化
	/// ---------------------------------------------------

	for(size_t i = 0; i < workWeakAttackBehavior_.size(); ++i) {
		WorkWeakAttackBehavior& wwab = workWeakAttackBehavior_[i];
		wwab.motionTimes_.activeTime_ = bodyAnimationRenderer_->GetDuration(weakAnimationFilePaths[i]);
	}


	/// ---------------------------------------------------
	/// コライダーの処理化
	/// ---------------------------------------------------

	attackCollider_ = new PlayerAttackCollider(this, pTrackingCamera_->GetGameCamera());
	attackCollider_->Initialize();
	attackCollider_->isActive = false;

	entityShadow_ = new EntityShadow();
	entityShadow_->Initialize();
	entityShadow_->SetParent(pTransform_);

	/// varialbe managerに値を追加する
	AddVariables();
	LoadVariables();
	ApplyVariables();


}

void Player::Update() {
	ApplyVariables();

	// ダメージを初期化
	//damage_ = 0.0f;

	currentBehavior_->Update();
	SpawnWeapon();

	pTransform_->Update();
	PushBack();
}

#pragma region Debug
void Player::Debug() {



	ImGui::DragFloat("MaxHP", &maxHP_, 0.1f);
	ImGui::InputFloat("HP", &currentHP_);

	if(ImGui::Button("Heal Hp For Max")) {
		currentHP_ = maxHP_;
	}

	if(ImGui::TreeNode("RootBehavior")) {
		ImGui::DragFloat("Speed_InRootBehavior", &workRootBehavior_.speed_, 0.1f);
		ImGui::DragFloat("RotateLerpSensitivity_InRootBehavior", &workRootBehavior_.rotateLerpSensitivity_, 0.1f);

		ImGui::TreePop();
	}

	ImGui::Spacing();


	VariableManager* vm = VariableManager::GetInstance();
	

	if(ImGui::TreeNode("AvoidanceBehavior")) {

		if(ImGui::Button("save file")) {
			vm->SaveSpecificGroupsToJson("./Resources/Parameters/Objects", "WorkAvoidanceBehavior");
		}
		vm->DebuggingSpecificGroup("WorkAvoidanceBehavior");

		ImGui::TreePop();
	}

	ImGui::Spacing();



	if(ImGui::TreeNode("WeakAttack")) {

		/// weak attack
		for(size_t i = 0; i < 3; ++i) {
			std::string nodeLabel = "WorkWeakAttackBehavior" + std::to_string(i + 1);
			if(ImGui::TreeNode(nodeLabel.c_str())) {

				if(ImGui::Button("save file")) {
					vm->SaveSpecificGroupsToJson("./Resources/Parameters/Objects", nodeLabel);
				}

				ImGui::Spacing();
				vm->DebuggingSpecificGroup(nodeLabel);
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	ImGui::Spacing();


	/// strong attack behavior
	if(ImGui::TreeNode(strongAttackBehavior_.name_.c_str())) {

		if(ImGui::Button("save file")) {
			vm->SaveSpecificGroupsToJson("./Resources/Parameters/Objects", strongAttackBehavior_.name_);
		}

		ImGui::Spacing();
		vm->DebuggingSpecificGroup(strongAttackBehavior_.name_);
		ImGui::TreePop();
	}



	/// animation
	if(ImGui::TreeNode("animation")) {

		for(auto& joint : bodyAnimationRenderer_->GetSkeleton()->joints) {
			std::string label = std::format("time##{:p}", reinterpret_cast<void*>(&joint));
			label += joint.name;
			ImGui::DragFloat(label.c_str(), &joint.animationTime, 0.0f);
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
	vm->AddValue(groupName, "startPosY", startPosY_);
	vm->AddValue(groupName, "endPosY",   endPosY_);
	vm->AddValue(groupName, "weaponSpawnMaxTime", weaponSpawnMaxTime_);


	{	/// avoidance behavior
		const std::string& name = "WorkAvoidanceBehavior";
		vm->AddValue(name, "moveDistance", workAvoidanceBehavior_.moveDistance_);
		vm->AddValue(name, "startupTime",  workAvoidanceBehavior_.motionTimes_.startupTime_);
		vm->AddValue(name, "activeTime",   workAvoidanceBehavior_.motionTimes_.activeTime_);
		vm->AddValue(name, "endLagTime",   workAvoidanceBehavior_.motionTimes_.endLagTime_);
	}

	for(size_t i = 0; i < 3; ++i){	/// workWeakAttackBehavior_ 値のio

		const std::string& name = "WorkWeakAttackBehavior" + std::to_string(i + 1);

		vm->AddValue(name, "startupTime",  workWeakAttackBehavior_[i].motionTimes_.startupTime_);
		vm->AddValue(name, "activeTime",   workWeakAttackBehavior_[i].motionTimes_.activeTime_);
		vm->AddValue(name, "endLagTime",   workWeakAttackBehavior_[i].motionTimes_.endLagTime_);
		vm->AddValue(name, "damageFactor", workWeakAttackBehavior_[i].damageFactor_);
	}



	{	/// strong behavior 値のio
		const std::string& groupName = strongAttackBehavior_.name_;

		vm->AddValue(groupName, "damage0", strongAttackBehavior_.damages_[0]);
		vm->AddValue(groupName, "damage1", strongAttackBehavior_.damages_[1]);
		vm->AddValue(groupName, "damage2", strongAttackBehavior_.damages_[2]);
	}


}

void Player::LoadVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", groupName);

	{	/// avoidance behavior 値のio
		vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", "WorkAvoidanceBehavior");
	}

	for(size_t i = 0; i < 3; ++i) {	/// workWeakAttackBehavior_ 値のio
		const std::string& name = "WorkWeakAttackBehavior" + std::to_string(i + 1);
		vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", name);
	}

	{	/// strong behavior 値のio
		const std::string& groupName = strongAttackBehavior_.name_;
		vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", groupName);
	}
}

void Player::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	/// work root behavior data
	workRootBehavior_.speed_                 = vm->GetValue<float>(groupName, "speed");
	workRootBehavior_.rotateLerpSensitivity_ = vm->GetValue<float>(groupName, "rotateLerpSensitivity");
	startPosY_          = vm->GetValue<float>(groupName, "startPosY");
	endPosY_            = vm->GetValue<float>(groupName, "endPosY");
	weaponSpawnMaxTime_ = vm->GetValue<float>(groupName, "weaponSpawnMaxTime");

	{	/// avoidance behavior
		const std::string& name = "WorkAvoidanceBehavior";
		workAvoidanceBehavior_.moveDistance_             = vm->GetValue<float>(name, "moveDistance");
		workAvoidanceBehavior_.motionTimes_.startupTime_ = vm->GetValue<float>(name, "startupTime");
		workAvoidanceBehavior_.motionTimes_.activeTime_  = vm->GetValue<float>(name, "activeTime");
		workAvoidanceBehavior_.motionTimes_.endLagTime_  = vm->GetValue<float>(name, "endLagTime");
	}


	for(size_t i = 0; i < 3; ++i) {	/// workWeakAttackBehavior_ 値のio

		const std::string& name = "WorkWeakAttackBehavior" + std::to_string(i + 1);

		workWeakAttackBehavior_[i].motionTimes_.startupTime_ = vm->GetValue<float>(name, "startupTime");
		workWeakAttackBehavior_[i].motionTimes_.activeTime_  = vm->GetValue<float>(name, "activeTime");
		workWeakAttackBehavior_[i].motionTimes_.endLagTime_  = vm->GetValue<float>(name, "endLagTime");
		workWeakAttackBehavior_[i].damageFactor_             = vm->GetValue<float>(name, "damageFactor");
	}


	{	/// strong behavior 値のio
		const std::string& name = strongAttackBehavior_.name_;

		strongAttackBehavior_.damages_[0] = vm->GetValue<float>(name, "damage0");
		strongAttackBehavior_.damages_[1] = vm->GetValue<float>(name, "damage1");
		strongAttackBehavior_.damages_[2] = vm->GetValue<float>(name, "damage2");
	}


}

#pragma endregion


void Player::PushBack() {
	float radius = pEnemy_->GetColliderRadius() + colliderRadius_;
	Vec3 diff    = pEnemy_->GetPosition() - GetPosition();

	/// 当たっている
	if(diff.Len() < radius) {
		Vec3 pushBackDirection = -diff.Normalize();

		pTransform_->position += pushBackDirection * (radius - diff.Len());
		pTransform_->position.y = 0.0f;
	}
}

void Player::SpawnWeapon() {
	if(weaponSpawnTime_ == 0.0f) {
		return;
	}

	weaponSpawnTime_ -= Time::DeltaTime();
	weaponSpawnTime_ = std::max(weaponSpawnTime_, 0.0f);

	float lerpT = weaponSpawnTime_ / weaponSpawnMaxTime_;

	std::vector<Material>& materials = weaponAnimationRenderer_->GetMaterials();
	for(auto& mate : materials) {
		float posX = mate.GetPosition().x;
		mate.SetPosition(
			{ posX , std::lerp(endPosY_, startPosY_, lerpT) }
		);

		mate.UpdateMatrix();
	}
}




void Player::TransitionBehavior(std::unique_ptr<IPlayerBehavior> next) {
	currentBehavior_ = std::move(next);
}

void Player::SetEnemy(Enemy* _enemy) {
	pEnemy_ = _enemy;
}

void Player::SetTrackingCamera(TrackingCamera* _trackingCamera) {
	pTrackingCamera_ = _trackingCamera;
}

void Player::SetAnimationModel(const std::string& _filePath) {
	bodyAnimationRenderer_->ChangeAnimation(_filePath);
}

void Player::SetAnimationModel(const std::string& _bodyModelFilePath, const std::string& _weaponModelFilePath) {
	bodyAnimationRenderer_->ChangeAnimation(_bodyModelFilePath);
	weaponAnimationRenderer_->ChangeAnimation(_weaponModelFilePath);
}

void Player::SetAnimationTotalTime(float _totalTime) {
	bodyAnimationRenderer_->SetTotalTime(_totalTime, bodyAnimationRenderer_->GetCurrentNodeAnimationKey());
	weaponAnimationRenderer_->SetTotalTime(_totalTime, weaponAnimationRenderer_->GetCurrentNodeAnimationKey());
}

void Player::ResetAnimationTotal() {
	bodyAnimationRenderer_->SetTotalTime(
		bodyAnimationRenderer_->GetDuration(bodyAnimationRenderer_->GetCurrentNodeAnimationKey()), 
		bodyAnimationRenderer_->GetCurrentNodeAnimationKey()
	);

	weaponAnimationRenderer_->SetTotalTime(
		weaponAnimationRenderer_->GetDuration(weaponAnimationRenderer_->GetCurrentNodeAnimationKey()),
		weaponAnimationRenderer_->GetCurrentNodeAnimationKey()
	);
}

void Player::SetAnimationFlags(int _flags, bool _isResetTime) {
	bodyAnimationRenderer_->SetAnimationFlags(_flags);
	weaponAnimationRenderer_->SetAnimationFlags(_flags);

	if(_isResetTime) {
		bodyAnimationRenderer_->Restart();
		weaponAnimationRenderer_->Restart();
	}
}

float Player::GetAnimationDuration() {
	return bodyAnimationRenderer_->GetDuration(bodyAnimationRenderer_->GetCurrentNodeAnimationKey());
}

void Player::SetIsActiveWeapon(bool _isActive) {
	weaponAnimationRenderer_->isActive = _isActive;
	if(_isActive) {
		//weaponSpawnTime_ = weaponSpawnMaxTime_;
	}
}

void Player::SetAttackMode(int _mode) {
	attackCollider_->SetMode(_mode);
}
