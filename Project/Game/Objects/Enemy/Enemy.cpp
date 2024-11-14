#include "Enemy.h"
#include "State/IEnemyState.h"
#include "State/EnemyIdleState.h"

#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "Game/Objects/Player/Player.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


Enemy::Enemy(Player* player):BaseGameObject(),player_(player),maxHp_(0.0f),maxStamina_(0.0f){
	CreateTag(this);
}

Enemy::~Enemy(){}

void Enemy::Initialize(){
	animationRender_ = AddComponent<AnimationRenderer>("Kari_Boss_Wait");

	hp_ = maxHp_;
	stamina_ = maxStamina_;

	currentState_.reset(new EnemyIdleState(this));
	currentState_->Initialize();
}

void Enemy::Update(){
	currentState_->Update();
}

void Enemy::Debug(){
	ImGui::InputText("CurrentAction :",const_cast<char*>(currentAction_.c_str()),currentAction_.size());

	float weakAttackPoint = GetWeakAttackPoint();
	ImGui::InputFloat("WeakAttackPoint",&weakAttackPoint);
	float strongAttackPoint = GetStrongAttackPoint();
	ImGui::InputFloat("WeakAttackPoint",&strongAttackPoint);
	float shortIdlePoint = GetShortIdlePoint();
	ImGui::InputFloat("WeakAttackPoint",&shortIdlePoint);
	float longIdlePoint = GetLongIdlePoint();
	ImGui::InputFloat("WeakAttackPoint",&longIdlePoint);

	if(ImGui::TreeNode("Status")){
		ImGui::DragFloat("MaxHP",const_cast<float*>(&maxHp_),0.1f);
		ImGui::DragFloat("MaxStamina",const_cast<float*>(&maxStamina_),0.1f);
		ImGui::DragFloat("Speed",&speed_,0.1f);

		ImGui::Spacing();

		ImGui::InputFloat("Current HP",&hp_);
		ImGui::InputFloat("Current Stamina",&stamina_);

		if(ImGui::Button("SetMax Status")){
			hp_ = maxHp_;
			stamina_ = maxStamina_;
		}
		ImGui::TreePop();
	}

	if(ImGui::TreeNode("Idle Action")){
		if(ImGui::TreeNode("ShortIdle")){
			ImGui::DragFloat("ShortIdle_hpWeight",&workShortIdle_.hpWeight_,0.1f);
			ImGui::DragFloat("ShortIdle_staminaWeight",&workShortIdle_.staminaWeight_,0.1f);
			ImGui::DragFloat("ShortIdle_healingStamina",&workShortIdle_.healingStamina_,0.1f);
			ImGui::TreePop();
		}

		ImGui::Spacing();

		if(ImGui::TreeNode("LongIdle")){
			ImGui::DragFloat("LongIdle_hpWeight",&workLongIdle_.hpWeight_,0.1f);
			ImGui::DragFloat("LongIdle_staminaWeight",&workLongIdle_.staminaWeight_,0.1f);
			ImGui::DragFloat("ShortIdle_healingStamina",&workLongIdle_.healingStamina_,0.1f);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

	if(ImGui::TreeNode("Attack Action")){
		if(ImGui::TreeNode("WeakAttack")){
			ImGui::DragFloat("WeakAttack_hpWeight",&workWeakAttack_.hpWeight_,0.1f);
			ImGui::DragFloat("WeakAttack_staminaWeight",&workWeakAttack_.staminaWeight_,0.1f);
			ImGui::DragFloat("WeakAttack_activationDistance",&workWeakAttack_.activationDistance_,0.1f);
			ImGui::DragFloat("WeakAttack_staminaConsumed",&workWeakAttack_.staminaConsumed_,0.1f);
			ImGui::DragFloat("WeakAttack_damage",&workWeakAttack_.damage_,0.1f);
			ImGui::TreePop();
		}

		ImGui::Spacing();

		if(ImGui::TreeNode("StrongAttack")){
			ImGui::DragFloat("StrongAttack_hpWeight",&workStrongAttack_.hpWeight_,0.1f);
			ImGui::DragFloat("StrongAttack_staminaWeight",&workStrongAttack_.staminaWeight_,0.1f);
			ImGui::DragFloat("StrongAttack_activationDistance",&workStrongAttack_.activationDistance_,0.1f);
			ImGui::DragFloat("StrongAttack_staminaConsumed",&workStrongAttack_.staminaConsumed_,0.1f);
			ImGui::DragFloat("StrongAttack_damage",&workStrongAttack_.damage_,0.1f);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

void Enemy::SetAnimationRender(const std::string& filePath){
	animationRender_ = AddComponent<AnimationRenderer>(filePath);
}

void Enemy::TransitionState(IEnemyState* next){
	currentState_.reset(next);
	currentState_->Initialize();
}

Player* Enemy::GetPlayer() const{ return player_; }

float Enemy::GetShortIdlePoint() const{
	return ((hp_ / maxHp_) * workShortIdle_.hpWeight_) + ((stamina_ / maxStamina_) * workShortIdle_.staminaWeight_);
}

float Enemy::GetLongIdlePoint() const{
	return ((hp_ / maxHp_) * workLongIdle_.hpWeight_) + ((stamina_ / maxStamina_) * workLongIdle_.staminaWeight_);
}

float Enemy::GetWeakAttackPoint() const{
	return ((hp_ / maxHp_) * workWeakAttack_.hpWeight_) +
		((stamina_ / maxStamina_) * workWeakAttack_.staminaWeight_);
}

float Enemy::GetStrongAttackPoint() const{
	return ((hp_ / maxHp_) * workStrongAttack_.hpWeight_) +
		((stamina_ / maxStamina_) * workStrongAttack_.staminaWeight_);
}