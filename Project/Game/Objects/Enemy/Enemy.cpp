#include "Enemy.h"

#include <iostream>

#include "BehaviorTree/EnemyActions.h"
#include "BehaviorWorker/EnemyBehaviorWorkers.h"

#include "../ONEngine/ComponentManager/Collider/SphereCollider.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "Game/Objects/Player/Player.h"
#include "Math/Random.h"
#include "MyFileSystem/MyFileSystem.h"
#include "VariableManager/VariableManager.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

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

	// 最初の行動を設定
	//DecideNextNode();

	LoadStatus();
	LoadAllAction();
	LoadCombos();
}

void Enemy::Update(){
	if(rootNode_){
		if(rootNode_->tick() == EnemyBehaviorTree::Status::SUCCESS){
			//DecideNextNode();
		}
	}
}

void Enemy::Debug(){
	ImGui::InputText("CurrentAction :",const_cast<char*>(currentAction_.c_str()),currentAction_.size());

	if(ImGui::Button("Save")){
		SaveStatus();
		SaveAllAction();
		SaveCombos();
	}

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

	if(ImGui::Button("ReloadResourceFileList")){
		WorkEnemyAction::animationList =  MyFileSystem::SearchFile("./Resources/Models","gltf");
	}

	///===============================================
	/// AttackActions
	///===============================================
	if(ImGui::TreeNode("Actions")){
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
				currentEditActionName_ = workEnemyActionVariables_.find(createObjectName_)->first;
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
		if(!workEnemyActionVariables_.empty()){
			if(currentEditActionName_ == ""){
				currentEditActionName_ = workEnemyActionVariables_.begin()->first;
				currentEditAction_ = workEnemyActionVariables_[currentEditActionName_].get();
			}
			if(ImGui::BeginCombo("Actions",currentEditActionName_.c_str())){
				for(auto& [key,value] : workEnemyActionVariables_){
					bool isSelected = (currentEditActionName_ == key);
					if(ImGui::Selectable(key.c_str(),isSelected)){
						currentEditActionName_ = key;
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
		if(currentEditActionName_ != ""){
			if(ImGui::TreeNode(("currentCombo::" + currentEditActionName_).c_str())){
				ImGui::Spacing();

				// Type を 変更
				if(ImGui::BeginCombo("Action Type",actionTypeWord[currentEditAction_->type_].c_str())){
					for(auto& [key,value] : actionTypeWord){
						bool isSelected = (currentEditAction_->type_ == key);
						if(ImGui::Selectable(value.c_str(),isSelected)){
							std::string animation = workEnemyActionVariables_[currentEditActionName_]->animationName_;
							workEnemyActionVariables_[currentEditActionName_] = std::move(CreateWorker(key));
							workEnemyActionVariables_[currentEditActionName_]->type_ = key;
							workEnemyActionVariables_[currentEditActionName_]->animationName_ = animation;
							currentEditAction_ = workEnemyActionVariables_[currentEditActionName_].get();
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
		if(!isComboCreateWindowPop_){
			if(ImGui::Button("Create Attack Combo")){
				isComboCreateWindowPop_ = true;
			}
		} else{
			ImGui::Begin("New Attack Combo");
			ImGui::InputText("New Combo Name",&createObjectName_[0],sizeof(char) * 64);
			if(ImGui::Button("Create")){
				editComboVariables_[createObjectName_] = ComboAttacks();
				currentEditCombo_ = &editComboVariables_[createObjectName_];
				currentEditComboName_ = editComboVariables_.find(createObjectName_)->first;
				isComboCreateWindowPop_ = false;
				createObjectName_ = "NULL";
			}
			ImGui::SameLine();
			if(ImGui::Button("Cancel")){
				isComboCreateWindowPop_ = false;
				createObjectName_ = "NULL";
			}
			ImGui::End();
		}

		ImGui::Spacing();

		// 調整できるオブジェクトが存在すれば オブジェクトを 一覧表示
		if(!editComboVariables_.empty()){
			if(currentEditComboName_ == ""){
				currentEditActionName_ = editComboVariables_.begin()->first;
				currentEditCombo_ = &editComboVariables_[currentEditActionName_];
			}
			if(ImGui::BeginCombo("Combos",currentEditComboName_.c_str())){
				for(auto& [key,value] : editComboVariables_){
					bool isSelected = (currentEditComboName_ == key);
					if(ImGui::Selectable(key.c_str(),isSelected)){
						currentEditComboName_ = key;
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
		if(currentEditCombo_){
			if(ImGui::TreeNode(currentEditComboName_.c_str())){
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
						bool isSelected = (currentEditCombo_->rangeType_ == type);
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

				if(ImGui::Button("Play CurrentCombo")){
					rootNode_ = std::make_unique<EnemyBehaviorTree::AttackCombo>(this,currentEditComboName_);
				}
				if(ImGui::Button("Stop")){
					rootNode_ = nullptr;
				}
				if(ImGui::TreeNode(("currentCombo::" + currentEditComboName_).c_str())){
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

	///===============================================
	/// RangeType ごとの 許容距離
	///===============================================
	if(ImGui::TreeNode("distanceByRangeTypes")){
		ImGui::DragFloat("ShortRange",&distanceByRangeTypes_[EnemyAttackRangeType::SHORT_RANGE],0.1f);
		ImGui::DragFloat("MiddleRange",&distanceByRangeTypes_[EnemyAttackRangeType::MIDDLE_RANGE],0.1f);
		ImGui::DragFloat("LongRange",&distanceByRangeTypes_[EnemyAttackRangeType::LONG_RANGE],0.1f);
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

void Enemy::SaveStatus(){
	VariableManager* variableManager = VariableManager::GetInstance();
	variableManager->SetValue("Enemy_Status","HP",maxHp_);
	variableManager->SetValue("Enemy_Status","speed",speed_);

	variableManager->SetValue<float>("Enemy_Status",rangeTypes[EnemyAttackRangeType::SHORT_RANGE],distanceByRangeTypes_[EnemyAttackRangeType::SHORT_RANGE]);
	variableManager->SetValue<float>("Enemy_Status",rangeTypes[EnemyAttackRangeType::MIDDLE_RANGE],distanceByRangeTypes_[EnemyAttackRangeType::MIDDLE_RANGE]);
	variableManager->SetValue<float>("Enemy_Status",rangeTypes[EnemyAttackRangeType::LONG_RANGE],distanceByRangeTypes_[EnemyAttackRangeType::LONG_RANGE]);

	variableManager->SaveSpecificGroupsToJson(enemyJsonDirectory,"Enemy_Status");
}

void Enemy::SaveCombos(){
	VariableManager* variableManager = VariableManager::GetInstance();

	variableManager->SetValue("Enemy_Combos","CombosSize",static_cast<int>(editComboVariables_.size()));
	int32_t index = 0;
	for(auto& [name,combo] : editComboVariables_){
		variableManager->SetValue("Enemy_Combos","Index_" + std::to_string(index),name);

		SaveCombo(name);
		++index;
	}

	variableManager->SaveSpecificGroupsToJson(enemyComboDirectory,"Enemy_Combos");
}

void Enemy::SaveAllAction(){
	VariableManager* variableManager = VariableManager::GetInstance();

	variableManager->SetValue("Enemy_Actions","ActionSize",static_cast<int>(workEnemyActionVariables_.size()));

	int32_t index = 0;
	for(auto& [name,worker] : workEnemyActionVariables_){
		variableManager->SetValue("Enemy_Actions","Index_" + std::to_string(index),name);

		worker->Save(name);
		variableManager->SaveSpecificGroupsToJson(enemyActionDirectory,name);
		++index;
	}
	variableManager->SaveSpecificGroupsToJson(enemyActionDirectory,"Enemy_Actions");
}

void Enemy::SaveCombo(const std::string& comboName){
	VariableManager* variableManager = VariableManager::GetInstance();

	variableManager->SetValue<int>(comboName,"RangeType",static_cast<int>(editComboVariables_[comboName].rangeType_));
	variableManager->SetValue(comboName,"ComboSize",static_cast<int>(editComboVariables_[comboName].comboAttacks_.size()));
	int32_t index = 0;
	for(auto& [name,combo] : editComboVariables_[comboName].comboAttacks_){
		variableManager->SetValue(comboName,"Index_" + std::to_string(index),name);
		++index;
	}

	variableManager->SaveSpecificGroupsToJson(enemyComboDirectory,comboName);
}

void Enemy::LoadStatus(){
	VariableManager* variableManager = VariableManager::GetInstance();
	variableManager->LoadSpecificGroupsToJson(enemyJsonDirectory,"Enemy_Status");
	maxHp_ = variableManager->GetValue<float>("Enemy_Status","HP");
	speed_ = variableManager->GetValue<float>("Enemy_Status","speed");
	distanceByRangeTypes_[EnemyAttackRangeType::SHORT_RANGE] = variableManager->GetValue<float>("Enemy_Status","SHORT_RANGE");
	distanceByRangeTypes_[EnemyAttackRangeType::MIDDLE_RANGE] = variableManager->GetValue<float>("Enemy_Status","MIDDLE_RANGE");
	distanceByRangeTypes_[EnemyAttackRangeType::LONG_RANGE] = variableManager->GetValue<float>("Enemy_Status","LONG_RANGE");
}

void Enemy::LoadCombos(){
	VariableManager* variableManager = VariableManager::GetInstance();

	variableManager->LoadSpecificGroupsToJson(enemyComboDirectory,"Enemy_Combos");
	int32_t combosSize = variableManager->GetValue<int>("Enemy_Combos","CombosSize");

	for(size_t i = 0; i < combosSize; i++){
		std::string comboName = variableManager->GetValue<std::string>("Enemy_Combos","Index_" + std::to_string(i));
		variableManager->LoadSpecificGroupsToJson(enemyComboDirectory,comboName);
		editComboVariables_[comboName] = {};
		editComboVariables_[comboName].rangeType_ = static_cast<EnemyAttackRangeType>(variableManager->GetValue<int>(comboName,"RangeType"));
		LoadCombo(comboName,variableManager->GetValue<int>(comboName,"ComboSize"));
	}
}

void Enemy::LoadAllAction(){
	VariableManager* variableManager = VariableManager::GetInstance();

	variableManager->LoadSpecificGroupsToJson(enemyActionDirectory,"Enemy_Actions");
	int32_t actionSize =  variableManager->GetValue<int>("Enemy_Actions","ActionSize");
	for(size_t i = 0; i < actionSize; ++i){
		std::string actionName = variableManager->GetValue<std::string>("Enemy_Actions","Index_" + std::to_string(i));
		variableManager->LoadSpecificGroupsToJson(enemyActionDirectory,actionName);
		workEnemyActionVariables_[actionName] = std::move(CreateWorker(static_cast<ActionTypes>(variableManager->GetValue<int>(actionName,"Type"))));
		workEnemyActionVariables_[actionName]->Load(actionName);
	}
}

void Enemy::LoadCombo(const std::string& comboName,int32_t size){
	VariableManager* variableManager = VariableManager::GetInstance();

	for(int32_t i = 0; i < size; i++){
		editComboVariables_[comboName].comboAttacks_.push_back({variableManager->GetValue<std::string>(comboName,"Index_" + std::to_string(i)),i});
	}
}

void Enemy::SetAnimationRender(const std::string& filePath){
	this->animationRender_->ChangeAnimation(filePath);
}

Player* Enemy::GetPlayer() const{ return player_; }

std::unique_ptr<EnemyBehaviorTree::Sequence> Enemy::CreateAction(const std::string& actionName){
	WorkEnemyAction* worker = workEnemyActionVariables_[actionName].get();

	std::unique_ptr<EnemyBehaviorTree::Sequence> result = nullptr;

	result = std::make_unique<EnemyBehaviorTree::Sequence>(this);
	result->addChild(std::make_unique<EnemyBehaviorTree::TransitionAnimation>(this,worker->animationName_));

	switch(worker->type_){
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

/// <summary>
/// 次の行動を決める
/// </summary>
void Enemy::DecideNextNode(){
	float lengthEnemy2Player = (player_->GetPosition() - this->GetPosition()).Len();

	std::string comboName = "";

	if(lengthEnemy2Player <= distanceByRangeTypes_[EnemyAttackRangeType::SHORT_RANGE]){
		// ShortRange 以下なら
		const std::deque<std::string>& comboNameList = this->GetComboList(EnemyAttackRangeType::SHORT_RANGE);
		comboName = comboNameList[Random::Int(0,static_cast<int>(comboNameList.size() - 1))];

	} else if(lengthEnemy2Player > distanceByRangeTypes_[EnemyAttackRangeType::MIDDLE_RANGE]){
		// MiddleRange より 大きいなら
		const std::deque<std::string>& comboNameList = this->GetComboList(EnemyAttackRangeType::LONG_RANGE);
		comboName = comboNameList[Random::Int(0,static_cast<int>(comboNameList.size() - 1))];

	} else{
		const std::deque<std::string>& comboNameList = this->GetComboList(EnemyAttackRangeType::MIDDLE_RANGE);
		comboName = comboNameList[Random::Int(0,static_cast<int>(comboNameList.size() - 1))];
	}
	rootNode_ = std::make_unique<EnemyBehaviorTree::AttackCombo>(this,comboName);
}

const ComboAttacks& Enemy::GetComboAttacks(const std::string& comboName) const{
	auto it = editComboVariables_.find(comboName);
	if(it != editComboVariables_.end()){
		return it->second;  // 見つかった場合、その要素を返す
	} else{
		throw std::runtime_error("Attack action '" + comboName + "' not found");  // 見つからなかった場合の例外処理
	}
}

const std::deque<std::string>& Enemy::GetComboList(EnemyAttackRangeType rangeType) const{
	auto it = comboByRangeType_.find(rangeType);
	if(it != comboByRangeType_.end()){
		return it->second;  // 見つかった場合、その要素を返す
	} else{
		throw std::runtime_error("NotFound the RangeType");  // 見つからなかった場合の例外処理
	}
}

float Enemy::GetDistanceByRangeTypes(EnemyAttackRangeType rangeType) const{
	auto it = distanceByRangeTypes_.find(rangeType);
	if(it != distanceByRangeTypes_.end()){
		return it->second;  // 見つかった場合、その要素を返す
	} else{
		throw std::runtime_error("NotFound the RangeType");  // 見つからなかった場合の例外処理
	}
}