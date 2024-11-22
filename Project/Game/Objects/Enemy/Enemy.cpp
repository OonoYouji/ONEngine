#include "Enemy.h"

#include <iostream>

#include "BehaviorWorker/EnemyBehaviorWorkers.h"
#include "EnemyBehaviorTree/EnemyAttackBehaviors/EnemyStrongAttack.h"
#include "EnemyBehaviorTree/EnemyAttackBehaviors/EnemyTackleAttack.h"
#include "EnemyBehaviorTree/EnemyAttackBehaviors/EnemyWeakAttack.h"
#include "EnemyBehaviorTree/EnemyAttackBehaviors/EnemyWeakAttack.h"
#include "EnemyBehaviorTree/EnemyBasicActions.h"
#include "EnemyBehaviorTree/EnemyIdleBehaviors/EnemyIdle.h"
#include "EnemyBehaviorTree/EnemyMoveBehaviors/EnemyChase.h"
#include "EnemyCollider/EnemyAttackCollider.h"

#include "../ONEngine/ComponentManager/Collider/SphereCollider.h"
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"
#include "GraphicManager/ModelManager/ModelManager.h"

#include "Math/Random.h"
#include "MyFileSystem/MyFileSystem.h"
#include "VariableManager/VariableManager.h"

#include "Game/Objects/Player/Player.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

Enemy::Enemy(Player* player,EnemyAttackCollider* collider):BaseGameObject(),player_(player),maxHp_(0.0f){
	attackCollider_ = collider;
	CreateTag(this);
}

Enemy::~Enemy(){}

void Enemy::Initialize(){
	bodyAnimationRenderer_ = AddComponent<AnimationRenderer>("Boss_Wait");
	weaponAnimationRenderer_ =  AddComponent<AnimationRenderer>("Boss_Wait");
	hitCollider_ = AddComponent<SphereCollider>(ModelManager::Load("Sphere"));
	hitCollider_->SetRadius(colliderRadius_);
	// 最初の行動を設定
	//DecideNextNode();

	LoadStatus();
	LoadAllAction();
	LoadCombos();

	hp_ = maxHp_;
}

void Enemy::Update(){
	if(rootNode_){
		EnemyBehaviorTree::Status status = rootNode_->tick();
		if(status == EnemyBehaviorTree::Status::SUCCESS){
			//DecideNextNode();
			rootNode_ = nullptr;
		} else if(status == EnemyBehaviorTree::Status::FAILURE){
			rootNode_ = nullptr;
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
							workEnemyActionVariables_[currentEditActionName_] = std::move(CreateWorker(key));
							workEnemyActionVariables_[currentEditActionName_]->type_ = key;
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
			if(ImGui::BeginCombo("Current Editing Combo Space",wordByHpState[static_cast<int32_t>(currentEditHpState_)].c_str())){
				for(int32_t i = 0; i < static_cast<int32_t>(HpState::COUNT); i++){
					bool isSelected = (i == static_cast<int32_t>(currentEditHpState_));
					if(ImGui::Selectable(wordByHpState[static_cast<int32_t>(i)].c_str(),isSelected)){
						currentEditHpState_ = static_cast<HpState>(i);
					}
					if(isSelected){
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			};

			ImGui::Begin("New Attack Combo");
			ImGui::InputText("New Combo Name",&createObjectName_[0],sizeof(char) * 64);
			if(ImGui::Button("Create")){
				editComboVariables_[0][createObjectName_] = ComboAttacks();
				currentEditCombo_ = &editComboVariables_[0][createObjectName_];
				currentEditComboName_ = editComboVariables_[0].find(createObjectName_)->first;
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
		if(!editComboVariables_[static_cast<int32_t>(currentEditHpState_)].empty()){
			if(currentEditComboName_ == ""){
				currentEditActionName_ = editComboVariables_[static_cast<int32_t>(currentEditHpState_)].begin()->first;
				currentEditCombo_ = &editComboVariables_[static_cast<int32_t>(currentEditHpState_)][currentEditActionName_];
			}
			if(ImGui::BeginCombo("Combos",currentEditComboName_.c_str())){
				int32_t currentEditComboHpIndex = static_cast<int32_t>(currentEditHpState_);
				for(auto& [key,value] : editComboVariables_[currentEditComboHpIndex]){
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
		} else{
			currentEditActionName_ = "";
			currentEditCombo_ = nullptr;
		}

		ImGui::Spacing();

		// 調整
		if(currentEditCombo_){
			if(ImGui::TreeNode(currentEditComboName_.c_str())){
				if(ImGui::BeginCombo("Swap HpState",wordByHpState[static_cast<int32_t>(currentEditHpState_)].c_str())){
					for(int32_t i = 0; i < static_cast<int32_t>(HpState::COUNT); i++){
						bool isSelected = (i == static_cast<int32_t>(currentEditHpState_));
						if(ImGui::Selectable(wordByHpState[i].c_str(),isSelected)){
							auto& currentMap = editComboVariables_[static_cast<int32_t>(currentEditHpState_)];
							auto& targetMap = editComboVariables_[i];

							auto it = currentMap.find(currentEditComboName_);
							if(it != currentMap.end()){
								targetMap[currentEditComboName_] = std::move(it->second);
								currentMap.erase(it);

								currentEditCombo_ = &targetMap[currentEditComboName_];
							}
						}
						if(isSelected){
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				if(ImGui::BeginCombo("RangeType",rangeTypes[currentEditCombo_->rangeType_].c_str())){
					for(const auto& [type,name] : rangeTypes){
						bool isSelected = (currentEditCombo_->rangeType_ == type);
						if(ImGui::Selectable(name.c_str(),isSelected)){
							currentEditCombo_->rangeType_ = type;
						}
						if(isSelected){
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				if(ImGui::BeginChild("Action List",ImVec2(130,270),true)){
					for(auto& [attackName,attack] : workEnemyActionVariables_){
						if(ImGui::Button(attackName.c_str())){
							currentEditCombo_->comboAttacks_.push_back(
								ComboAttack(attackName,static_cast<int32_t>(currentEditCombo_->comboAttacks_.size())));
						}
					}
				}
				ImGui::EndChild();
				ImGui::SameLine();
				if(ImGui::BeginChild(("currentCombo::" + currentEditComboName_).c_str(),ImVec2(130,270),true)){
					int index = 0;
					for(auto it = currentEditCombo_->comboAttacks_.begin();
						it != currentEditCombo_->comboAttacks_.end(); ++it,++index){

						ImGui::PushID(index);
						ImGui::Text("%s",it->attackName_.c_str());

						if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)){
							int64_t src_index = std::distance(currentEditCombo_->comboAttacks_.begin(),it);
							ImGui::SetDragDropPayload("LIST_ITEM",&src_index,sizeof(int64_t));
							ImGui::Text("Moving %s",it->attackName_.c_str());
							ImGui::EndDragDropSource();
						}

						if(ImGui::BeginDragDropTarget()){
							if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("LIST_ITEM")){
								int src_index = *(int*)payload->Data;
								auto src_it = std::next(currentEditCombo_->comboAttacks_.begin(),src_index);
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
				}
				ImGui::EndChild();

				if(ImGui::SmallButton("Play")){
					rootNode_ = std::make_unique<EnemyBehaviorTree::AttackCombo>(this,currentEditComboName_);
				}
				if(ImGui::SmallButton("Stop")){
					rootNode_ = nullptr;
				}
				ImGui::TreePop(); // 必ず呼ぶ
			}
		}

		ImGui::TreePop();
	}

	///===============================================
	/// RangeType ごとの 許容距離
	///===============================================
	if(ImGui::TreeNode("distance By RangeTypes")){
		ImGui::Text("distance < SHORT_RANGE  = SHORT_RANGE\ndistance > MIDDLE_RANGE  = LONG_RANGE\n else = MIDDLE_RANGE");
		ImGui::DragFloat("ShortRange",&distanceByRangeTypes_[EnemyAttackRangeType::SHORT_RANGE],0.1f);
		ImGui::DragFloat("MiddleRange",&distanceByRangeTypes_[EnemyAttackRangeType::MIDDLE_RANGE],0.1f);
		ImGui::DragFloat("LongRange",&distanceByRangeTypes_[EnemyAttackRangeType::LONG_RANGE],0.1f);
		ImGui::TreePop();
	}

	///===============================================
	/// 各HP State になるための値
	///===============================================
	if(ImGui::TreeNode("threshold By HpState")){
		int32_t hpStateCount = static_cast<int32_t>(HpState::COUNT);
		ImGui::Text("hp > HP_NORMAL  = HP_HIGHTE\n hp > HP_LOW  = HP_NORMAL\n else = HP_LOW");

		int32_t hpHight = static_cast<int32_t>(HpState::HP_HIGHTE);
		int32_t hpNormal = static_cast<int32_t>(HpState::HP_NORMAL);
		int32_t hpLow = static_cast<int32_t>(HpState::HP_LOW);

		ImGui::SliderFloat(wordByHpState[hpHight].c_str(),&thresholdByHpState_[hpHight],maxHp_,maxHp_);
		ImGui::SliderFloat(wordByHpState[hpNormal].c_str(),&thresholdByHpState_[hpNormal],thresholdByHpState_[hpLow],thresholdByHpState_[hpHight]);
		ImGui::SliderFloat(wordByHpState[hpLow].c_str(),&thresholdByHpState_[hpLow],0.0f,thresholdByHpState_[hpNormal]);
		ImGui::TreePop();
	}

	///=====================================
	/// RangeType ごと に 仕分け
	///=====================================
	{
		comboByRangeTypeByHpState_[static_cast<int32_t>(currentHPState_)][EnemyAttackRangeType::SHORT_RANGE].clear();
		comboByRangeTypeByHpState_[static_cast<int32_t>(currentHPState_)][EnemyAttackRangeType::MIDDLE_RANGE].clear();
		comboByRangeTypeByHpState_[static_cast<int32_t>(currentHPState_)][EnemyAttackRangeType::LONG_RANGE].clear();

		for(int32_t i = 0; i < static_cast<int32_t>(HpState::COUNT); i++){
			for(const auto& [comboName,combo] : editComboVariables_[i]){
				comboByRangeTypeByHpState_[i][combo.rangeType_].push_back(comboName);
			}
		}
	}
}

void Enemy::SaveStatus(){
	VariableManager* variableManager = VariableManager::GetInstance();
	{
		variableManager->SetValue("Enemy_Status","HP",maxHp_);
		variableManager->SetValue("Enemy_Status","speed",speed_);
	}

	{
		variableManager->SetValue<float>("Enemy_Status",rangeTypes[EnemyAttackRangeType::SHORT_RANGE],distanceByRangeTypes_[EnemyAttackRangeType::SHORT_RANGE]);
		variableManager->SetValue<float>("Enemy_Status",rangeTypes[EnemyAttackRangeType::MIDDLE_RANGE],distanceByRangeTypes_[EnemyAttackRangeType::MIDDLE_RANGE]);
		variableManager->SetValue<float>("Enemy_Status",rangeTypes[EnemyAttackRangeType::LONG_RANGE],distanceByRangeTypes_[EnemyAttackRangeType::LONG_RANGE]);
	}

	{
		int32_t hpHight = static_cast<int32_t>(HpState::HP_HIGHTE);
		int32_t hpNormal = static_cast<int32_t>(HpState::HP_NORMAL);
		int32_t hpLow = static_cast<int32_t>(HpState::HP_LOW);
		variableManager->SetValue<float>("Enemy_Status",wordByHpState[hpHight],thresholdByHpState_[hpHight]);
		variableManager->SetValue<float>("Enemy_Status",wordByHpState[hpNormal],thresholdByHpState_[hpNormal]);
		variableManager->SetValue<float>("Enemy_Status",wordByHpState[hpLow],thresholdByHpState_[hpLow]);
	}

	variableManager->SaveSpecificGroupsToJson(enemyJsonDirectory,"Enemy_Status");
}

void Enemy::SaveCombos(){
	VariableManager* variableManager = VariableManager::GetInstance();

	int32_t allComboNum = 0;
	for(auto& combo : editComboVariables_){
		allComboNum += static_cast<int32_t>(combo.size());
	}

	variableManager->SetValue("Enemy_Combos","CombosSize",allComboNum);
	int32_t index = 0;
	for(int32_t hpState = 0; hpState < static_cast<int32_t>(HpState::COUNT); hpState++){

		for(auto& [name,combo] : editComboVariables_[hpState]){
			variableManager->SetValue("Enemy_Combos","Index_" + std::to_string(index),name);

			SaveCombo(name,hpState);
			++index;
		}
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

void Enemy::SaveCombo(const std::string& comboName,int32_t hpState){
	VariableManager* variableManager = VariableManager::GetInstance();

	auto& currentCombo = editComboVariables_[hpState][comboName];
	variableManager->SetValue<int>(comboName,"HpState",static_cast<int>(hpState));
	variableManager->SetValue<int>(comboName,"RangeType",static_cast<int>(currentCombo.rangeType_));
	variableManager->SetValue(comboName,"ComboSize",static_cast<int>(currentCombo.comboAttacks_.size()));
	int32_t index = 0;
	for(auto& [name,combo] : currentCombo.comboAttacks_){
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

	{
		int32_t hpHight = static_cast<int32_t>(HpState::HP_HIGHTE);
		int32_t hpNormal = static_cast<int32_t>(HpState::HP_NORMAL);
		int32_t hpLow = static_cast<int32_t>(HpState::HP_LOW);
		thresholdByHpState_[hpHight] = variableManager->GetValue<float>("Enemy_Status",wordByHpState[hpHight]);
		thresholdByHpState_[hpNormal] = variableManager->GetValue<float>("Enemy_Status",wordByHpState[hpNormal]);
		thresholdByHpState_[hpLow] = variableManager->GetValue<float>("Enemy_Status",wordByHpState[hpLow]);
	}
}

void Enemy::LoadCombos(){
	VariableManager* variableManager = VariableManager::GetInstance();

	variableManager->LoadSpecificGroupsToJson(enemyComboDirectory,"Enemy_Combos");
	int32_t combosSize = variableManager->GetValue<int>("Enemy_Combos","CombosSize");

	for(size_t i = 0; i < combosSize; i++){
		std::string comboName = variableManager->GetValue<std::string>("Enemy_Combos","Index_" + std::to_string(i));
		variableManager->LoadSpecificGroupsToJson(enemyComboDirectory,comboName);
		int hpState = variableManager->GetValue<int>(comboName,"HpState");
		editComboVariables_[hpState][comboName] = {};
		editComboVariables_[hpState][comboName].rangeType_ = static_cast<EnemyAttackRangeType>(variableManager->GetValue<int>(comboName,"RangeType"));
		LoadCombo(comboName,variableManager->GetValue<int>(comboName,"ComboSize"),hpState);
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

void Enemy::LoadCombo(const std::string& comboName,int32_t size,int32_t hpState){
	VariableManager* variableManager = VariableManager::GetInstance();

	for(int32_t i = 0; i < size; i++){
		editComboVariables_[hpState][comboName].comboAttacks_.push_back({variableManager->GetValue<std::string>(comboName,"Index_" + std::to_string(i)),i});
	}
}

void Enemy::SetAnimationRender(const std::string& filePath){
	this->bodyAnimationRenderer_->ChangeAnimation(filePath);
	this->weaponAnimationRenderer_->isActive = false;
}

void Enemy::SetAnimationRender(const std::string& filePath,const std::string& weaponFilePath){
	this->bodyAnimationRenderer_->ChangeAnimation(filePath);
	this->weaponAnimationRenderer_->isActive = true;
	this->weaponAnimationRenderer_->ChangeAnimation(weaponFilePath);
}

void Enemy::SetAnimationTotalTime(float _totalTime){
	bodyAnimationRenderer_->SetTotalTime(_totalTime,bodyAnimationRenderer_->GetCurrentNodeAnimationKey());
	weaponAnimationRenderer_->SetTotalTime(_totalTime,weaponAnimationRenderer_->GetCurrentNodeAnimationKey());
}

void Enemy::ResetAnimationTotal(){
	bodyAnimationRenderer_->SetTotalTime(
		bodyAnimationRenderer_->GetDuration(bodyAnimationRenderer_->GetCurrentNodeAnimationKey()),
		bodyAnimationRenderer_->GetCurrentNodeAnimationKey()
	);

	weaponAnimationRenderer_->SetTotalTime(
		weaponAnimationRenderer_->GetDuration(weaponAnimationRenderer_->GetCurrentNodeAnimationKey()),
		weaponAnimationRenderer_->GetCurrentNodeAnimationKey()
	);
}

void Enemy::SetAnimationFlags(int _flags,bool _isResetTime){
	bodyAnimationRenderer_->SetAnimationFlags(_flags);
	weaponAnimationRenderer_->SetAnimationFlags(_flags);

	if(_isResetTime){
		bodyAnimationRenderer_->Restart();
		weaponAnimationRenderer_->Restart();
	}
}

Player* Enemy::GetPlayer() const{ return player_; }

std::unique_ptr<EnemyBehaviorTree::Sequence> Enemy::CreateAction(const std::string& actionName){
	WorkEnemyAction* worker = workEnemyActionVariables_[actionName].get();

	std::unique_ptr<EnemyBehaviorTree::Sequence> result = nullptr;

	result = std::make_unique<EnemyBehaviorTree::Sequence>(this);

	switch(worker->type_){
		case ActionTypes::IDLE:
			result->addChild(std::make_unique<EnemyBehaviorTree::Idle>(this,reinterpret_cast<WorkIdleAction*>(worker)));
			break;
		case ActionTypes::CHASE:
			result->addChild(std::make_unique<EnemyBehaviorTree::Chase>(this,reinterpret_cast<WorkChaseAction*>(worker)));
			break;
		case ActionTypes::WEAK_ATTACK:
			result->addChild(std::make_unique<EnemyBehaviorTree::WeakAttack>(this,reinterpret_cast<WorkWeakAttackAction*>(worker)));
			break;
		case ActionTypes::STRONG_ATTACK:
			result->addChild(std::make_unique<EnemyBehaviorTree::StrongAttack>(this,reinterpret_cast<WorkStrongAttackAction*>(worker)));
			break;
		case ActionTypes::TACKLE_ATTACK:
			result->addChild(std::make_unique<EnemyBehaviorTree::TackleAttack>(this,reinterpret_cast<WorkTackleAttackAction*>(worker)));
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
		case ActionTypes::TACKLE_ATTACK:
			result = std::make_unique<WorkTackleAttackAction>();
			break;
		case ActionTypes::CHASE:
			result = std::make_unique<WorkChaseAction>();
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

	// 現在の HP State を 知る
	if(hp_ > thresholdByHpState_[static_cast<int32_t>(HpState::HP_NORMAL)]){
		currentHPState_ = HpState::HP_HIGHTE;
	} else if(hp_ > thresholdByHpState_[static_cast<int32_t>(HpState::HP_LOW)]){
		currentHPState_ = HpState::HP_NORMAL;
	} else{
		currentHPState_ = HpState::HP_LOW;
	}

	if(lengthEnemy2Player <= distanceByRangeTypes_[EnemyAttackRangeType::SHORT_RANGE]){
		// ShortRange 以下なら
		const std::deque<std::string>& comboNameList = this->GetComboList(currentHPState_,EnemyAttackRangeType::SHORT_RANGE);
		comboName = comboNameList[Random::Int(0,static_cast<int>(comboNameList.size() - 1))];

	} else if(lengthEnemy2Player > distanceByRangeTypes_[EnemyAttackRangeType::MIDDLE_RANGE]){
		// MiddleRange より 大きいなら
		const std::deque<std::string>& comboNameList = this->GetComboList(currentHPState_,EnemyAttackRangeType::LONG_RANGE);
		comboName = comboNameList[Random::Int(0,static_cast<int>(comboNameList.size() - 1))];

	} else{
		const std::deque<std::string>& comboNameList = this->GetComboList(currentHPState_,EnemyAttackRangeType::MIDDLE_RANGE);
		comboName = comboNameList[Random::Int(0,static_cast<int>(comboNameList.size() - 1))];
	}
	rootNode_ = std::make_unique<EnemyBehaviorTree::AttackCombo>(this,comboName);
}

const ComboAttacks& Enemy::GetComboAttacks(int32_t hpCombo,const std::string& comboName) const{
	auto it = editComboVariables_[hpCombo].find(comboName);
	if(it != editComboVariables_[hpCombo].end()){
		return it->second;  // 見つかった場合、その要素を返す
	} else{
		throw std::runtime_error("Attack action '" + comboName + "' not found");  // 見つからなかった場合の例外処理
	}
}

const std::deque<std::string>& Enemy::GetComboList(HpState state,EnemyAttackRangeType rangeType) const{
	auto it = comboByRangeTypeByHpState_[static_cast<int32_t>(state)].find(rangeType);
	if(it != comboByRangeTypeByHpState_[static_cast<int32_t>(state)].end()){
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

void Enemy::ActivateAttackCollider(ActionTypes offset,float radius){
	attackCollider_->Activate(offset,radius);
}

void Enemy::TerminateAttackCollider(){
	attackCollider_->Terminate();
}