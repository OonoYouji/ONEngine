#include "Enemy.h"
#include "State/EnemyIdleState.h"
#include "State/IEnemyState.h"

#include <iostream>

#include "BehaviorTree/AttackBehaviorTree.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "Game/Objects/Player/Player.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

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
				workAttackVariables_[createObjectName_] = WorkAttackAction();

				currentEditAction_ = &workAttackVariables_[createObjectName_];
				currentEditActionName_ = const_cast<std::string*>(&workAttackVariables_.find(createObjectName_)->first);
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
			if(ImGui::BeginCombo("Attack Actions",currentEditActionName_->c_str())){
				for(auto& [key,value] : workAttackVariables_){
					bool isSelected = (currentEditActionName_ == &key);
					if(ImGui::Selectable(key.c_str(),isSelected)){
						currentEditActionName_ = const_cast<std::string*>(&key);
						currentEditAction_ = &value;
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
				ImGui::DragFloat("setupTime",&currentEditAction_->motionTimes_.startupTime_,0.1f,0.0f);
				ImGui::DragFloat("activeTime",&currentEditAction_->motionTimes_.activeTime_,0.1f,0.0f);
				ImGui::DragFloat("endLagTime",&currentEditAction_->motionTimes_.endLagTime_,0.1f,0.0f);
				ImGui::DragFloat("damage",&currentEditAction_->damage_,0.1f,0.0f);

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
				comboVariables_[createObjectName_];

				currentEditCombo_ = &comboVariables_[createObjectName_];
				currentEditComboName_ = const_cast<std::string*>(&comboVariables_.find(createObjectName_)->first);
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
				for(auto& [key,value] : comboVariables_){
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
					for(auto& [attackName,attack] : workAttackVariables_){
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
}

void Enemy::SetAnimationRender(const std::string& filePath){
	this->animationRender_->ChangeAnimation(filePath);
}

void Enemy::TransitionState(IEnemyState* next){
	currentState_.reset(next);
	currentState_->Initialize();
}

Player* Enemy::GetPlayer() const{ return player_; }

const WorkAttackAction& Enemy::GetWorkAttack(const std::string& attack) const{
	auto it = workAttackVariables_.find(attack);
	if(it != workAttackVariables_.end()){
		return it->second;  // 見つかった場合、その要素を返す
	} else{
		throw std::runtime_error("Attack action '" + attack + "' not found");  // 見つからなかった場合の例外処理
	}
}

const ComboAttacks& Enemy::GetComboAttacks(const std::string& comboName) const{
	auto it = comboVariables_.find(comboName);
	if(it != comboVariables_.end()){
		return it->second;  // 見つかった場合、その要素を返す
	} else{
		throw std::runtime_error("Attack action '" + comboName + "' not found");  // 見つからなかった場合の例外処理
	}
}
