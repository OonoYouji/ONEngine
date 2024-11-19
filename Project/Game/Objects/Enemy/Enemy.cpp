#include "Enemy.h"

#include <iostream>

#include "BehaviorTree/EnemyActions.h"
#include "BehaviorWorker/EnemyBehaviorWorlers.h"
#include "State/EnemyIdleState.h"
#include "State/IEnemyState.h"

#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "Game/Objects/Player/Player.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


std::unordered_map<EnemyAttackRangeType,float> distanceByRangeTypes = {
	{EnemyAttackRangeType::SHORT_RANGE,10.0f},
	{EnemyAttackRangeType::MIDDLE_RANGE,20.0f},
	{EnemyAttackRangeType::LONG_RANGE,1000.0f},
};
// EnemyAttackRangeTypeと文字列の対応付け
std::unordered_map<EnemyAttackRangeType,std::string> rangeTypes = {
	{EnemyAttackRangeType::SHORT_RANGE,"SHORT_RANGE"},
	{EnemyAttackRangeType::MIDDLE_RANGE,"MIDDLE_RANGE"},
	{EnemyAttackRangeType::LONG_RANGE,"LONG_RANGE"},
};

Enemy::Enemy(Player* player):BaseGameObject(),player_(player),maxHp_(0.0f){
	CreateTag(this);
}

Enemy::~Enemy(){}

void Enemy::Initialize(){
	animationRender_ = AddComponent<AnimationRenderer>("Kari_Boss_Wait");

	hp_ = maxHp_;

	currentState_.reset(new EnemyIdleState(this));
	currentState_->Initialize();
}

void Enemy::Update(){
	currentState_->Update();
}

void Enemy::Debug(){
	ImGui::InputText("CurrentAction :",const_cast<char*>(currentAction_.c_str()),currentAction_.size());

	///===============================================
	/// ステータスの表示
	///===============================================
	if(ImGui::TreeNode("Status")){
		ImGui::DragFloat("MaxHP",const_cast<float*>(&maxHp_),0.1f);
		ImGui::DragFloat("Speed",&speed_,0.1f);

		ImGui::Spacing();

		ImGui::InputFloat("Current HP",&hp_);

		if(ImGui::Button("SetMax Status")){
			hp_ = maxHp_;
		}
		ImGui::TreePop();
	}

	ImGui::Spacing();

	///===============================================
	/// AttackActions
	///===============================================
	if(ImGui::TreeNode("AttackActions")){
		// 新しい アクションを 生成する時に window を popupする
		if(!isCreateWindowPop_){
			if(ImGui::Button("Create Attack Action")){
				isCreateWindowPop_ = true;
			}
		} else{
			ImGui::Begin("New Attack Action");
			ImGui::InputText("New ActionName",&createObjectName_[0],sizeof(char) * 64);
			if(ImGui::Button("Create")){
				workEnemyActionVariables_[createObjectName_] = std::make_unique<WorkIdleAction>();

				currentEditAction_ = workEnemyActionVariables_[createObjectName_].get();
				currentEditActionName_ = const_cast<std::string*>(&workEnemyActionVariables_.find(createObjectName_)->first);
				isCreateWindowPop_ = false;
				createObjectName_ = "NULL";
			}
			ImGui::SameLine();
			if(ImGui::Button("Cancel")){
				isCreateWindowPop_ = false;
				createObjectName_ = "NULL";
			}
			ImGui::End();
		}

		ImGui::Spacing();

		// 調整できるオブジェクトが存在すれば オブジェクトを 一覧表示
		if(currentEditActionName_){
			if(ImGui::BeginCombo("Actions",currentEditActionName_->c_str())){
				for(auto& [key,value] : workEnemyActionVariables_){
					bool isSelected = (currentEditActionName_ == &key);
					if(ImGui::Selectable(key.c_str(),isSelected)){
						currentEditActionName_ = const_cast<std::string*>(&key);
						currentEditAction_ = value.get();
					}
					if(isSelected){
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}

		ImGui::Spacing();

		// 調整できるように 
		if(currentEditActionName_){
			if(ImGui::TreeNode(("currentCombo::" + *currentEditActionName_).c_str())){
				ImGui::Spacing();

				// Type を 変更
				if(ImGui::BeginCombo("Attack Type",actionTypeWord[currentEditAction_->type_].c_str())){
					for(auto& [key,value] : actionTypeWord){
						bool isSelected = (currentEditAction_->type_ == key);
						if(ImGui::Selectable(value.c_str(),isSelected)){
							workEnemyActionVariables_[*currentEditActionName_] = std::move(CreateWorker(currentEditAction_->type_));
							currentEditAction_ = workEnemyActionVariables_[*currentEditActionName_].get();
						}
						if(isSelected){
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				currentEditAction_->Debug();

				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	///===============================================
	/// AttackCombo
	///===============================================
	if(ImGui::TreeNode("AttackCombo")){
		// 新しい Combo の 作成
		if(!isCreateWindowPop_){
			if(ImGui::Button("Create Attack Combo")){
				isCreateWindowPop_ = true;
			}
		} else{
			ImGui::Begin("New Attack Combo");
			ImGui::InputText("New Combo Name",&createObjectName_[0],sizeof(char) * 64);
			if(ImGui::Button("Create")){
				editComboVariables_[createObjectName_] = ComboAttacks();
				currentEditCombo_ = &editComboVariables_[createObjectName_];
				currentEditComboName_ = const_cast<std::string*>(&editComboVariables_.find(createObjectName_)->first);
				isCreateWindowPop_ = false;
				createObjectName_ = "NULL";
			}
			ImGui::SameLine();
			if(ImGui::Button("Cancel")){
				isCreateWindowPop_ = false;
				createObjectName_ = "NULL";
			}
			ImGui::End();
		}

		ImGui::Spacing();

		// 調整できるオブジェクトが存在すれば オブジェクトを 一覧表示
		if(currentEditComboName_){
			if(ImGui::BeginCombo("Combos",currentEditComboName_->c_str())){
				for(auto& [key,value] : editComboVariables_){
					bool isSelected = (currentEditComboName_ == &key);
					if(ImGui::Selectable(key.c_str(),isSelected)){
						currentEditComboName_ = const_cast<std::string*>(&key);
						currentEditCombo_ = &value;
					}
					if(isSelected){
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}

		ImGui::Spacing();

		// 調整
		if(currentEditComboName_){
			if(ImGui::TreeNode(currentEditComboName_->c_str())){
				if(ImGui::TreeNode("AttackActions")){
					for(auto& [attackName,attack] : workEnemyActionVariables_){
						if(ImGui::Button(attackName.c_str())){
							currentEditCombo_->comboAttacks_.push_back(ComboAttack(attackName,static_cast<int32_t>(currentEditCombo_->comboAttacks_.size())));
						}
					}
					ImGui::TreePop();
				}

				if(ImGui::BeginCombo("RangeType",rangeTypes[currentEditCombo_->rangeType_].c_str())){
					for(const auto& [type,name] : rangeTypes){
						bool isSelected = (static_cast<BYTE>(currentEditCombo_->rangeType_) == static_cast<BYTE>(type));
						if(ImGui::Selectable(name.c_str(),isSelected)){
							// 選択された型を更新
							currentEditCombo_->rangeType_ = type;
						}
						if(isSelected){
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				if(ImGui::TreeNode(("currentCombo::" + *currentEditComboName_).c_str())){
					int index = 0;
					for(auto it = currentEditCombo_->comboAttacks_.begin(); it != currentEditCombo_->comboAttacks_.end(); ++it,++index){
						ImGui::PushID(index);
						ImGui::Text("%s",it->attackName_.c_str());

						if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)){
							int64_t src_index = std::distance(currentEditCombo_->comboAttacks_.begin(),it);
							ImGui::SetDragDropPayload("LIST_ITEM",&src_index,sizeof(int64_t));  // インデックスを渡す
							ImGui::Text("Moving %s",it->attackName_.c_str());
							ImGui::EndDragDropSource();
						}

						if(ImGui::BeginDragDropTarget()){
							if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("LIST_ITEM")){
								int src_index = *(int*)payload->Data;  // インデックスを取得
								auto src_it = std::next(currentEditCombo_->comboAttacks_.begin(),src_index);  // イテレータに変換
								if(src_it != it){
									ComboAttack temp = *src_it;
									currentEditCombo_->comboAttacks_.erase(src_it);
									currentEditCombo_->comboAttacks_.insert(it,temp);
								}
							}
							ImGui::EndDragDropTarget();
						}
						ImGui::PopID();
					}
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	if(ImGui::TreeNode("Action Pattern")){
		ImGui::TreePop();
	}

	///=====================================
	/// RangeType ごと に 仕分け
	///=====================================
	{
		comboByRangeType_[EnemyAttackRangeType::SHORT_RANGE].clear();
		comboByRangeType_[EnemyAttackRangeType::MIDDLE_RANGE].clear();
		comboByRangeType_[EnemyAttackRangeType::LONG_RANGE].clear();

		for(const auto& [comboName,combo] : editComboVariables_){
			comboByRangeType_[combo.rangeType_].push_back(comboName);
		}
	}
}

void Enemy::SetAnimationRender(const std::string& filePath){
	this->animationRender_->ChangeAnimation(filePath);
}

void Enemy::TransitionState(IEnemyState* next){
	currentState_.reset(next);
	currentState_->Initialize();
}

Player* Enemy::GetPlayer() const{ return player_; }

std::unique_ptr<EnemyBehaviorTree::Sequence> Enemy::CreateAction(const std::string& actionName){
	WorkEnemyAction* worker = workEnemyActionVariables_[actionName].get();

	std::unique_ptr<EnemyBehaviorTree::Sequence> result = nullptr;

	result = std::make_unique<EnemyBehaviorTree::Sequence>(this);
	result->addChild(std::make_unique<EnemyBehaviorTree::TransitionAnimation>(this,worker->animationName_));

	switch(worker->types_){
		case ActionTypes::WEAK_ATTACK:
			result->addChild(std::make_unique<EnemyBehaviorTree::WeakAttack>(this,reinterpret_cast<WorkWeakAttackAction*>(worker)));
			break;
		case ActionTypes::STRONG_ATTACK:
			result->addChild(std::make_unique<EnemyBehaviorTree::StrongAttack>(this,reinterpret_cast<WorkStrongAttackAction*>(worker)));
			break;
		case ActionTypes::IDLE:
			result->addChild(std::make_unique<EnemyBehaviorTree::IdleAction>(this,reinterpret_cast<WorkIdleAction*>(worker)));
			break;
		default:
			// 該当 する Typeが なければ reset
			result.reset();
			break;
	}
	return result;
}

std::unique_ptr<WorkEnemyAction> Enemy::CreateWorker(ActionTypes type){
	std::unique_ptr<WorkEnemyAction> result = nullptr;
	switch(type){
		case ActionTypes::WEAK_ATTACK:
			result = std::make_unique<WorkWeakAttackAction>();
			break;
		case ActionTypes::STRONG_ATTACK:
			result = std::make_unique<WorkStrongAttackAction>();
			break;
		case ActionTypes::IDLE:
			result = std::make_unique<WorkIdleAction>();
			break;
		default:
			break;
	}

	return result;
}

const ComboAttacks& Enemy::GetComboAttacks(const std::string& comboName) const{
	auto it = editComboVariables_.find(comboName);
	if(it != editComboVariables_.end()){
		return it->second;  // 見つかった場合、その要素を返す
	} else{
		throw std::runtime_error("Attack action '" + comboName + "' not found");  // 見つからなかった場合の例外処理
	}
}

const std::list<std::string>& Enemy::GetComboList(EnemyAttackRangeType rangeType) const{
	auto it = comboByRangeType_.find(rangeType);
	if(it != comboByRangeType_.end()){
		return it->second;  // 見つかった場合、その要素を返す
	} else{
		throw std::runtime_error("NotFound the RangeType");  // 見つからなかった場合の例外処理
	}
}
