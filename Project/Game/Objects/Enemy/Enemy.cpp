#include "Enemy.h"

/// std
#include <iostream>
#include <numbers>

/// enemy behavior
#include "BehaviorWorker/EnemyBehaviorWorkers.h"
#include "EnemyBehaviorTree/EnemyAttackBehaviors/EnemyLongRangeAttack.h"
#include "EnemyBehaviorTree/EnemyAttackBehaviors/EnemyRangedAttack.h"
#include "EnemyBehaviorTree/EnemyAttackBehaviors/EnemyStrongAttack.h"
#include "EnemyBehaviorTree/EnemyAttackBehaviors/EnemyTackleAttack.h"
#include "EnemyBehaviorTree/EnemyAttackBehaviors/EnemyWeakAttack.h"
#include "EnemyBehaviorTree/EnemyAttackBehaviors/EnemyWeakAttack2.h"
#include "EnemyBehaviorTree/EnemyBasicActions.h"
#include "EnemyBehaviorTree/EnemyIdleBehaviors/EnemyAwakening.h"
#include "EnemyBehaviorTree/EnemyIdleBehaviors/EnemyIdle.h"
#include "EnemyBehaviorTree/EnemyMoveBehaviors/EnemyChase.h"
#include "EnemyBehaviorTree/EnemyMoveBehaviors/EnemyJumpAway.h"
#include "EnemyCollider/EnemyAttackCollider.h"
#include "EnemyEffect/EnemyEffect.h"

#include "../ONEngine/ComponentManager/Collider/SphereCollider.h"
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"
#include "ComponentManager/AudioSource/AudioSource.h"
#include "GraphicManager/ModelManager/ModelManager.h"

#include "Math/Random.h"
#include "MyFileSystem/MyFileSystem.h"
#include "VariableManager/VariableManager.h"

/// objects
#include "Objects/EntityShadow/EntityShadow.h"
#include "Objects/Player/Player.h"
#include "Objects/GameManagerObject/GameManagerObject.h"

#ifdef _DEBUG
/// externals
#include "imgui.h"

bool InputText(const char* label,std::string* str,ImGuiInputTextFlags flags = 0,ImGuiInputTextCallback callback = nullptr,void* user_data = nullptr){
	// バッファを作成
	char buffer[256];
	strncpy_s(buffer,sizeof(buffer),str->c_str(),_TRUNCATE); // 安全なコピー関数を使用
	buffer[sizeof(buffer) - 1] = '\0'; // 念のための終端文字

	// ImGuiの処理
	bool changed = ImGui::InputText(label,buffer,sizeof(buffer),flags,callback,user_data);

	// 入力が変更された場合、string を更新
	if(changed){
		*str = std::string(buffer);
	}

	return changed;
}

#endif // _DEBUG

Enemy::Enemy(Player* player,EnemyAttackCollider* collider):BaseGameObject(),player_(player),maxHp_(0.0f){
	attackCollider_ = collider;
	CreateTag(this);
}

Enemy::~Enemy(){}

void Enemy::Initialize(){

	hitCollider_ = AddComponent<SphereCollider>(ModelManager::Load("Sphere"));
	hitCollider_->SetRadius(colliderRadius_);

	effect1_ = new EnemyEffect();
	effect1_->SetParent(pTransform_);
	// 先に使うモーションを 読み込んでおく
	effect1_->SetEffectAnimationRender("Effect14");
	effect1_->SetEffectAnimationRender("Effect5");
	effect1_->SetEffectAnimationRender("Effect11");
	effect1_->SetIsActive(false);

	effect2_ = new EnemyEffect();
	effect2_->SetParent(pTransform_);
	effect2_->SetEffectAnimationRender("Effect5");
	effect2_->SetIsActive(false);

	passiveEffect_ = new EnemyEffect();

	passiveEffect_->SetEffectAnimationRender("Effect13");
	passiveEffect_->SetIsActive(false);

	damageEffects_.resize(3);
	for(auto& effect : damageEffects_){
		effect =  new EnemyEffect();
		effect->SetEffectAnimationRender("Effect15");
		effect->SetEffectAnimationRender("Effect16");
		effect->SetIsActive(false);
	}

	bodyAnimationRenderer_ 		 = AddComponent<AnimationRenderer>("Boss_Wait");
	weaponAnimationRenderer_  	 = AddComponent<AnimationRenderer>("Boss_Wait");
	subWeaponAnimationRenderer_  = AddComponent<AnimationRenderer>("Boss_Wait");
	effectAnimationRenderer_	 = AddComponent<AnimationRenderer>("Boss_Wait");
	// あにめーしょん を ロード
	LoadAllAnimation();

	se_ = AddComponent<AudioSource>();
	se_->volume = 0.1f;

	weaponAnimationRenderer_->isActive = false;
	subWeaponAnimationRenderer_->isActive = false;
	effectAnimationRenderer_->isActive = false;

	entityShadow_ = new EntityShadow();
	entityShadow_->Initialize();
	entityShadow_->SetParent(pTransform_);
	entityShadow_->SetScale(Vec3::kOne * 1.5f);

	LoadStatus();
	LoadAllAction();
	LoadCombos();

	hp_ = maxHp_;

	currentHpState_ = HpState::HP_HIGHTE;


	/// 反対を向いているのを正面にする
	pTransform_->rotate.y = std::numbers::pi_v<float>;

	///=====================================
	/// RangeType ごと に 仕分け
	///=====================================
	{
		comboByRangeTypeByHpState_[static_cast<int32_t>(currentHpState_)][EnemyAttackRangeType::SHORT_RANGE].clear();
		comboByRangeTypeByHpState_[static_cast<int32_t>(currentHpState_)][EnemyAttackRangeType::MIDDLE_RANGE].clear();
		comboByRangeTypeByHpState_[static_cast<int32_t>(currentHpState_)][EnemyAttackRangeType::LONG_RANGE].clear();

		for(int32_t i = 0; i < static_cast<int32_t>(HpState::COUNT); i++){
			for(const auto& [comboName,combo] : editComboVariables_[i]){
				comboByRangeTypeByHpState_[i][combo.rangeType_].push_back(comboName);
			}
		}
	}

	// 最初の行動を設定
	DecideNextNode();

}

void Enemy::Update(){
	if(rootNode_){
		EnemyBehaviorTree::Status status = rootNode_->tick();

		if(status != EnemyBehaviorTree::Status::RUNNING){
			DecideNextNode();
		}
	}

	for(auto& damageEffect : damageEffects_){
		// 再生後 の エフェクトは消す
		if(damageEffect->GetAnimationRenderer()->GetPlayOnced()){
			damageEffect->SetIsActive(false);
		}
	}

	if(currentHpState_ != HpState::HP_LOW){
		hp_ = (std::max)(hp_,thresholdByHpState_[static_cast<int32_t>(HpState::HP_LOW)]);
	}

	float playerLength = pTransform_->position.Len();
	float lengthMax = player_->GetStageRange() - colliderRadius_;
	float lengthMin = -player_->GetStageRange() + colliderRadius_;
	float clampedEnemyPosLength = (std::clamp)(playerLength,
											   lengthMin,
											   lengthMax);

	preOutOfStage_ = outOfStage_;
	outOfStage_ = false;

	if(playerLength > lengthMax
	   || playerLength < lengthMin){
		outOfStage_ = true;
	}

	passiveEffect_->SetPosition(pTransform_->position);

	pTransform_->position = Vec3::Normalize(pTransform_->position) * clampedEnemyPosLength;
}

void Enemy::Debug(){
#ifdef _DEBUG
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

		ImGui::Spacing();

		ImGui::InputFloat("Current HP",&hp_);

		if(ImGui::Button("SetMax Status")){
			hp_ = maxHp_;
		}

		ImGui::Text("Spawn Weapon");
		ImGui::SliderFloat("spawnWeaponUvStartY",&spawnWeaponUvStartPosY_,0.0f,1.0f);
		ImGui::SliderFloat("spawnWeaponUvEndY",&spawnWeaponUvEndPosY_,0.0f,1.0f);

		ImGui::Text("Spawn SubWeapon");
		ImGui::SliderFloat("spawnSubWeaponUvStartY",&spawnSubWeaponUvStartPosY_,0.0f,1.0f);
		ImGui::SliderFloat("spawnSubWeaponUvEndY",&spawnSubWeaponUvEndPosY_,0.0f,1.0f);


		ImGui::TreePop();
	}

	ImGui::Spacing();

	if(ImGui::Button("Play Awakening Motion")){
		rootNode_ = std::make_unique<EnemyBehaviorTree::EnemyAwakening>(this);
	}
	if(ImGui::Checkbox("actionIsActive",&actionIsActive_)){
		DecideNextNode();
	}
	ImGui::Text("%s",dointCombo_.c_str());

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
			InputText("New ActionName",&createObjectName_);
			if(ImGui::Button("Create")){
				workEnemyActionVariables_[createObjectName_] = std::make_unique<WorkIdleAction>();

				currentEditAction_ = workEnemyActionVariables_[createObjectName_].get();
				currentEditActionName_ = const_cast<AttackActionName*>(&workEnemyActionVariables_.find(createObjectName_)->first);
				actionNameBeforeNameChange_ = *currentEditActionName_;
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
			if(!currentEditActionName_){
				currentEditActionName_ = const_cast<std::string*>(&workEnemyActionVariables_.begin()->first);
				actionNameBeforeNameChange_ = *currentEditActionName_;
				currentEditAction_ = workEnemyActionVariables_[*currentEditActionName_].get();
			}
			if(ImGui::BeginCombo("Actions",currentEditActionName_->c_str())){
				for(auto& [key,value] : workEnemyActionVariables_){
					bool isSelected = (currentEditActionName_ == &key);
					if(ImGui::Selectable(key.c_str(),isSelected)){
						currentEditActionName_ = const_cast<AttackActionName*>(&key);
						actionNameBeforeNameChange_ = *currentEditActionName_;
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
				if(InputText("Name",&actionNameBeforeNameChange_,ImGuiInputTextFlags_EnterReturnsTrue)){
					*currentEditActionName_ = actionNameBeforeNameChange_;
					actionNameBeforeNameChange_ = *currentEditActionName_;
				}
				ImGui::Spacing();

				// Type を 変更
				if(ImGui::BeginCombo("Action Type",actionTypeWord[currentEditAction_->type_].c_str())){
					for(auto& [key,value] : actionTypeWord){
						bool isSelected = (currentEditAction_->type_ == key);
						if(ImGui::Selectable(value.c_str(),isSelected)){
							workEnemyActionVariables_[*currentEditActionName_] = std::move(CreateWorker(key));
							workEnemyActionVariables_[*currentEditActionName_]->type_ = key;
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
	/// Combo
	///===============================================
	if(ImGui::TreeNode("Combo")){
		if(ImGui::BeginCombo("HpState",wordByHpState[static_cast<int32_t>(currentEditHpState_)].c_str())){
			for(int32_t i = 0; i < static_cast<int32_t>(HpState::COUNT); i++){
				bool isSelected = (i == static_cast<int32_t>(currentEditHpState_));
				if(ImGui::Selectable(wordByHpState[i].c_str(),isSelected)){
					currentEditHpState_ = static_cast<HpState>(i);

					auto& combo = editComboVariables_[i];
					if(!combo.empty()){
						currentEditCombo_ = &combo.begin()->second;
						currentEditComboName_ = const_cast<std::string*>(&combo.begin()->first);
					}
				}
				if(isSelected){
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		// 新しい Combo の 作成
		if(!isComboCreateWindowPop_){
			if(ImGui::Button("Create Combo")){
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

			ImGui::Begin("New Combo");
			InputText("New Combo Name",&createObjectName_);
			if(ImGui::Button("Create")){
				int32_t hpState = static_cast<int32_t>(currentEditHpState_);
				editComboVariables_[hpState][createObjectName_] = ComboAttacks();
				currentEditCombo_ = &editComboVariables_[hpState][createObjectName_];
				currentEditComboName_ = const_cast<std::string*>(&editComboVariables_[hpState].find(createObjectName_)->first);
				comboNameBeforeNameChange_ = *currentEditComboName_;
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
			if(!currentEditComboName_){
				currentEditComboName_ = const_cast<std::string*>(&editComboVariables_[static_cast<int32_t>(currentEditHpState_)].begin()->first);
				comboNameBeforeNameChange_ = *currentEditComboName_;
				currentEditCombo_ = &editComboVariables_[static_cast<int32_t>(currentEditHpState_)][*currentEditComboName_];
			} else{
				if(ImGui::BeginCombo("Combos",currentEditComboName_->c_str())){
					int32_t currentEditComboHpIndex = static_cast<int32_t>(currentEditHpState_);
					for(auto& [key,value] : editComboVariables_[currentEditComboHpIndex]){
						bool isSelected = (currentEditComboName_ == &key);
						if(ImGui::Selectable(key.c_str(),isSelected)){
							currentEditComboName_ = const_cast<std::string*>(&key);
							comboNameBeforeNameChange_ = *currentEditComboName_;
							currentEditCombo_ = &value;
						}
						if(isSelected){
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}
		} else{
			currentEditComboName_ = nullptr;
			currentEditCombo_ = nullptr;
		}

		ImGui::Spacing();

		// 調整
		if(currentEditCombo_){
			if(ImGui::TreeNode(currentEditComboName_->c_str())){
				if(ImGui::BeginCombo("Swap HpState",wordByHpState[static_cast<int32_t>(currentEditHpState_)].c_str())){
					for(int32_t i = 0; i < static_cast<int32_t>(HpState::COUNT); i++){
						bool isSelected = (i == static_cast<int32_t>(currentEditHpState_));
						if(ImGui::Selectable(wordByHpState[i].c_str(),isSelected)){
							auto& currentMap = editComboVariables_[static_cast<int32_t>(currentEditHpState_)];
							auto& targetMap = editComboVariables_[i];

							auto it = currentMap.find(*currentEditComboName_);
							if(it != currentMap.end()){
								targetMap[*currentEditComboName_] = std::move(it->second);
								currentMap.erase(it);

								currentEditCombo_ = &targetMap[*currentEditComboName_];
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

				if(InputText("Name",&comboNameBeforeNameChange_,ImGuiInputTextFlags_EnterReturnsTrue)){
					*currentEditComboName_ = comboNameBeforeNameChange_;
					comboNameBeforeNameChange_ = *currentEditComboName_;
				}

				if(ImGui::BeginChild("Action List",ImVec2(230,270),true)){
					for(auto& [attackName,attack] : workEnemyActionVariables_){
						if(ImGui::Button(attackName.c_str())){
							currentEditCombo_->comboAttacks_.push_back(
								ComboAttack(attackName,static_cast<int32_t>(currentEditCombo_->comboAttacks_.size())));
						}
					}
				}
				ImGui::EndChild();
				ImGui::SameLine();
				if(ImGui::BeginChild(("currentCombo::" + *currentEditComboName_).c_str(),ImVec2(230,270),true)){
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
				if(ImGui::Button("DeleteBackAction")){
					if(!currentEditCombo_->comboAttacks_.empty()){
						currentEditCombo_->comboAttacks_.pop_back();
					}
				}

				if(ImGui::SmallButton("Play")){
					rootNode_ = std::make_unique<EnemyBehaviorTree::AttackCombo>(this,*currentEditComboName_);
				}
				ImGui::SameLine();
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
		comboByRangeTypeByHpState_[static_cast<int32_t>(currentEditHpState_)][EnemyAttackRangeType::SHORT_RANGE].clear();
		comboByRangeTypeByHpState_[static_cast<int32_t>(currentEditHpState_)][EnemyAttackRangeType::MIDDLE_RANGE].clear();
		comboByRangeTypeByHpState_[static_cast<int32_t>(currentEditHpState_)][EnemyAttackRangeType::LONG_RANGE].clear();

		for(int32_t i = 0; i < static_cast<int32_t>(HpState::COUNT); i++){
			for(const auto& [comboName,combo] : editComboVariables_[i]){
				comboByRangeTypeByHpState_[i][combo.rangeType_].push_back(comboName);
			}
		}
	}
#endif // _DEBUG
}

void Enemy::SaveStatus(){
	VariableManager* variableManager = VariableManager::GetInstance();
	{
		variableManager->SetValue("Enemy_Status","HP",maxHp_);
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

	{
		variableManager->SetValue<float>("Enemy_Status","spawnWeaponUvStartPosY",spawnWeaponUvStartPosY_);
		variableManager->SetValue<float>("Enemy_Status","spawnWeaponUvEndPosY",spawnWeaponUvEndPosY_);

		variableManager->SetValue<float>("Enemy_Status","spawnSubWeaponUvStartPosY",spawnSubWeaponUvStartPosY_);
		variableManager->SetValue<float>("Enemy_Status","spawnSubWeaponUvEndPosY",spawnSubWeaponUvEndPosY_);
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
	distanceByRangeTypes_[EnemyAttackRangeType::SHORT_RANGE]  = variableManager->GetValue<float>("Enemy_Status","SHORT_RANGE");
	distanceByRangeTypes_[EnemyAttackRangeType::MIDDLE_RANGE] = variableManager->GetValue<float>("Enemy_Status","MIDDLE_RANGE");
	distanceByRangeTypes_[EnemyAttackRangeType::LONG_RANGE]   = variableManager->GetValue<float>("Enemy_Status","LONG_RANGE");

	{
		int32_t hpHight = static_cast<int32_t>(HpState::HP_HIGHTE);
		int32_t hpNormal = static_cast<int32_t>(HpState::HP_NORMAL);
		int32_t hpLow = static_cast<int32_t>(HpState::HP_LOW);
		thresholdByHpState_[hpHight]  = variableManager->GetValue<float>("Enemy_Status",wordByHpState[hpHight]);
		thresholdByHpState_[hpNormal] = variableManager->GetValue<float>("Enemy_Status",wordByHpState[hpNormal]);
		thresholdByHpState_[hpLow] 	  = variableManager->GetValue<float>("Enemy_Status",wordByHpState[hpLow]);
	}

	{
		spawnSubWeaponUvStartPosY_ = variableManager->GetValue<float>("Enemy_Status","spawnSubWeaponUvStartPosY");
		spawnWeaponUvEndPosY_ 	   = variableManager->GetValue<float>("Enemy_Status","spawnWeaponUvEndPosY");
		spawnWeaponUvStartPosY_    = variableManager->GetValue<float>("Enemy_Status","spawnWeaponUvStartPosY");
		spawnSubWeaponUvEndPosY_   = variableManager->GetValue<float>("Enemy_Status","spawnSubWeaponUvEndPosY");
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

void Enemy::LoadAllAnimation(){
	bodyAnimationRenderer_->ChangeAnimation("Boss_Wait");
	bodyAnimationRenderer_->ChangeAnimation("Boss_Awakening");
	weaponAnimationRenderer_->ChangeAnimation("Effect1");
	subWeaponAnimationRenderer_->ChangeAnimation("Effect2");
	effectAnimationRenderer_->ChangeAnimation("Effect3");

	bodyAnimationRenderer_->ChangeAnimation("Boss_Jump_1");
	bodyAnimationRenderer_->ChangeAnimation("Boss_Jump_2");
	bodyAnimationRenderer_->ChangeAnimation("Boss_Jump_3");

	bodyAnimationRenderer_->ChangeAnimation("Boss_LongRangeAttack_1_B");
	bodyAnimationRenderer_->ChangeAnimation("Boss_LongRangeAttack_2_B");
	bodyAnimationRenderer_->ChangeAnimation("Boss_LongRangeAttack_3_B");

	weaponAnimationRenderer_->ChangeAnimation("Boss_LongRangeAttack_1_W1");
	weaponAnimationRenderer_->ChangeAnimation("Boss_LongRangeAttack_2_W1");
	weaponAnimationRenderer_->ChangeAnimation("Boss_LongRangeAttack_3_W1");

	subWeaponAnimationRenderer_->ChangeAnimation("Boss_LongRangeAttack_1_W2");
	subWeaponAnimationRenderer_->ChangeAnimation("Boss_LongRangeAttack_2_W2");
	subWeaponAnimationRenderer_->ChangeAnimation("Boss_LongRangeAttack_3_W2");

	bodyAnimationRenderer_->ChangeAnimation("Boss_LongRangeAttack_half_1_B");
	bodyAnimationRenderer_->ChangeAnimation("Boss_LongRangeAttack_half_2_B");
	bodyAnimationRenderer_->ChangeAnimation("Boss_LongRangeAttack_half_3_B");

	weaponAnimationRenderer_->ChangeAnimation("Boss_LongRangeAttack_half_1_W1");
	weaponAnimationRenderer_->ChangeAnimation("Boss_LongRangeAttack_half_2_W1");
	weaponAnimationRenderer_->ChangeAnimation("Boss_LongRangeAttack_half_3_W1");

	subWeaponAnimationRenderer_->ChangeAnimation("Boss_LongRangeAttack_half_1_W2");
	subWeaponAnimationRenderer_->ChangeAnimation("Boss_LongRangeAttack_half_2_W2");
	subWeaponAnimationRenderer_->ChangeAnimation("Boss_LongRangeAttack_half_3_W2");

	bodyAnimationRenderer_->ChangeAnimation("Boss_RangedAttack_1_B");
	bodyAnimationRenderer_->ChangeAnimation("Boss_RangedAttack_2_B");
	bodyAnimationRenderer_->ChangeAnimation("Boss_RangedAttack_3_B");

	weaponAnimationRenderer_->ChangeAnimation("Boss_RangedAttack_1_W");
	weaponAnimationRenderer_->ChangeAnimation("Boss_RangedAttack_2_W");
	weaponAnimationRenderer_->ChangeAnimation("Boss_RangedAttack_3_W");

	bodyAnimationRenderer_->ChangeAnimation("Boss_RushAttack_1_B");
	bodyAnimationRenderer_->ChangeAnimation("Boss_RushAttack_2_B");
	bodyAnimationRenderer_->ChangeAnimation("Boss_RushAttack_3_B");

	weaponAnimationRenderer_->ChangeAnimation("Boss_RushAttack_1_W");
	weaponAnimationRenderer_->ChangeAnimation("Boss_RushAttack_2_W");
	weaponAnimationRenderer_->ChangeAnimation("Boss_RushAttack_3_W");

	bodyAnimationRenderer_->ChangeAnimation("Boss_StrongAttack_1_1_B");
	bodyAnimationRenderer_->ChangeAnimation("Boss_StrongAttack_1_2_B");
	bodyAnimationRenderer_->ChangeAnimation("Boss_StrongAttack_1_3_B");

	weaponAnimationRenderer_->ChangeAnimation("Boss_StrongAttack_1_1_W");
	weaponAnimationRenderer_->ChangeAnimation("Boss_StrongAttack_1_2_W");
	weaponAnimationRenderer_->ChangeAnimation("Boss_StrongAttack_1_3_W");

	/// effect 
	effectAnimationRenderer_->ChangeAnimation("Effect5");

}

EnemyEffect* Enemy::GetEnemy1Effect(){
	return effect1_;
}

EnemyEffect* Enemy::GetEnemy2Effect(){
	return effect2_;
}

void Enemy::StartPassiveEffect(){
	passiveEffect_->SetIsActive(true);

	passiveEffect_->SetEffectAnimationRender("Effect13");
	passiveEffect_->SetEffectAnimationFlags(0,true);
}

Vector3 Enemy::GetCollisionOffset(ActionTypes type){
	return attackCollider_->GetOffset(type);
}

void Enemy::SetAnimationRender(const std::string& filePath){
	this->bodyAnimationRenderer_->ChangeAnimation(filePath);
	this->weaponAnimationRenderer_->isActive = false;
	this->subWeaponAnimationRenderer_->isActive = false;
	this->effectAnimationRenderer_->isActive = false;
}

void Enemy::SetAnimationRender(const std::string& filePath,const std::string& weaponFilePath){
	this->bodyAnimationRenderer_->ChangeAnimation(filePath);
	this->weaponAnimationRenderer_->isActive = true;
	this->weaponAnimationRenderer_->ChangeAnimation(weaponFilePath);
	this->subWeaponAnimationRenderer_->isActive = false;
	this->effectAnimationRenderer_->isActive = false;
}

void Enemy::SetAnimationRender(const std::string& filePath,
							   const std::string& weaponFilePath,
							   const std::string& subWeapon){
	this->bodyAnimationRenderer_->ChangeAnimation(filePath);

	this->weaponAnimationRenderer_->isActive = true;
	this->weaponAnimationRenderer_->ChangeAnimation(weaponFilePath);

	this->subWeaponAnimationRenderer_->isActive = true;
	this->subWeaponAnimationRenderer_->ChangeAnimation(subWeapon);

	this->effectAnimationRenderer_->isActive = false;
}

void Enemy::SetAnimationRender(const std::string& filePath,
							   const std::string& weaponFilePath,
							   const std::string& subWeapon,
							   const std::string& effect){
	this->bodyAnimationRenderer_->ChangeAnimation(filePath);

	this->weaponAnimationRenderer_->isActive = true;
	this->weaponAnimationRenderer_->ChangeAnimation(weaponFilePath);

	this->subWeaponAnimationRenderer_->isActive = true;
	this->subWeaponAnimationRenderer_->ChangeAnimation(subWeapon);

	this->effectAnimationRenderer_->isActive = true;
	this->effectAnimationRenderer_->ChangeAnimation(effect);
}

void Enemy::PlaySE(const std::string& se){
	se_->PlayOneShot(se,0.4f);
}

void Enemy::SpawnDamageEffect(float damage){
	std::string damageEffectName;
	if(damage > 500.0f){ // 大ダメージ
		damageEffectName = "Effect15";
	} else{
		damageEffectName = "Effect16";
	}

	for(auto& damageEffect : damageEffects_){
		if(!damageEffect->GetIsActiveAnimation()){
			damageEffect->SetIsActive(true);
			damageEffect->SetEffectAnimationRender(damageEffectName);
			damageEffect->SetEffectAnimationFlags(1,true);

			damageEffect->ResetAnimationTotal();

			damageEffect->SetPosition(GetPosition());
		}
	}
}

void Enemy::SpawnWeapon(float t){
	std::vector<Material>& materials = weaponAnimationRenderer_->GetMaterials();
	for(auto& mate : materials){
		float posX = mate.GetPosition().x;
		mate.SetPosition(
			{posX,std::lerp(spawnWeaponUvStartPosY_,spawnWeaponUvEndPosY_,t)}
		);

		mate.UpdateMatrix();
	}
}

void Enemy::SpawnSubWeapon(float t){
	std::vector<Material>& materials = subWeaponAnimationRenderer_->GetMaterials();
	for(auto& mate : materials){
		float posX = mate.GetPosition().x;
		mate.SetPosition(
			{posX,std::lerp(spawnSubWeaponUvStartPosY_,spawnSubWeaponUvEndPosY_,t)}
		);

		mate.UpdateMatrix();
	}
}

void Enemy::SetAnimationTotalTime(float _totalTime){
	bodyAnimationRenderer_->SetTotalTime(_totalTime,
										 bodyAnimationRenderer_->GetCurrentNodeAnimationKey());
	weaponAnimationRenderer_->SetTotalTime(_totalTime,
										   weaponAnimationRenderer_->GetCurrentNodeAnimationKey());
	subWeaponAnimationRenderer_->SetTotalTime(_totalTime,
											  subWeaponAnimationRenderer_->GetCurrentNodeAnimationKey());
}

void Enemy::SetEffectAnimationTotalTime(float _totalTime){
	effectAnimationRenderer_->SetTotalTime(_totalTime,
										   effectAnimationRenderer_->GetCurrentNodeAnimationKey());
}

float Enemy::GetBodyAnimationTotalTime() const{
	return bodyAnimationRenderer_->GetDuration(bodyAnimationRenderer_->GetCurrentNodeAnimationKey());
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

	subWeaponAnimationRenderer_->SetTotalTime(
		subWeaponAnimationRenderer_->GetDuration(subWeaponAnimationRenderer_->GetCurrentNodeAnimationKey()),
		subWeaponAnimationRenderer_->GetCurrentNodeAnimationKey());
}

void Enemy::ResetEffectAnimationTotal(){
	effectAnimationRenderer_->SetTotalTime(
		effectAnimationRenderer_->GetDuration(effectAnimationRenderer_->GetCurrentNodeAnimationKey()),
		effectAnimationRenderer_->GetCurrentNodeAnimationKey());
}

void Enemy::SetAnimationFlags(int _flags,bool _isResetTime){
	bodyAnimationRenderer_->SetAnimationFlags(_flags);
	weaponAnimationRenderer_->SetAnimationFlags(_flags);
	subWeaponAnimationRenderer_->SetAnimationFlags(_flags);
	effectAnimationRenderer_->SetAnimationFlags(_flags);
	if(_isResetTime){
		bodyAnimationRenderer_->Restart();
		weaponAnimationRenderer_->Restart();
		subWeaponAnimationRenderer_->Restart();
		effectAnimationRenderer_->Restart();
	}
}

void Enemy::SetEffectAnimationFlags(int _flags,bool _isResetTime){
	effectAnimationRenderer_->SetAnimationFlags(_flags);
	if(_isResetTime){
		effectAnimationRenderer_->Restart();
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
		case ActionTypes::WEAK_ATTACK_2:
			result->addChild(std::make_unique<EnemyBehaviorTree::WeakAttack2>(this,reinterpret_cast<WorkWeakAttack2Action*>(worker)));
			break;
		case ActionTypes::STRONG_ATTACK:
			result->addChild(std::make_unique<EnemyBehaviorTree::StrongAttack>(this,reinterpret_cast<WorkStrongAttackAction*>(worker)));
			break;
		case ActionTypes::TACKLE_ATTACK:
			result->addChild(std::make_unique<EnemyBehaviorTree::TackleAttack>(this,reinterpret_cast<WorkTackleAttackAction*>(worker)));
			break;
		case ActionTypes::RANGED_ATTACK:
			result->addChild(std::make_unique<EnemyBehaviorTree::RangedAttack>(this,reinterpret_cast<WorkRangedAttackAction*>(worker)));
			break;
		case ActionTypes::LONGRANGE_ATTACK:
			result->addChild(std::make_unique<EnemyBehaviorTree::LongRangeAttack>(this,reinterpret_cast<WorkLongRangeAttackAction*>(worker)));
			break;
		case ActionTypes::JUMP_AWAY:
			result->addChild(std::make_unique<EnemyBehaviorTree::JumpAway>(this,reinterpret_cast<WorkJumpAwayAction*>(worker)));
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
		case ActionTypes::RANGED_ATTACK:
			result = std::make_unique<WorkRangedAttackAction>();
			break;
		case ActionTypes::LONGRANGE_ATTACK:
			result = std::make_unique<WorkLongRangeAttackAction>();
			break;
		case ActionTypes::WEAK_ATTACK_2:
			result = std::make_unique<WorkWeakAttack2Action>();
			break;
		case ActionTypes::JUMP_AWAY:
			result = std::make_unique<WorkJumpAwayAction>();
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

	preHpState_ = currentHpState_;
	// 現在の HP State を 知る

	if(hp_ > thresholdByHpState_[static_cast<int32_t>(HpState::HP_NORMAL)]){
		currentHpState_ = HpState::HP_HIGHTE;
	} else if(hp_ > thresholdByHpState_[static_cast<int32_t>(HpState::HP_LOW)]){
		currentHpState_ = HpState::HP_NORMAL;
	} else{
		currentHpState_ = HpState::HP_LOW;
	}

	// HP が 低くなったら 覚醒モーション
	if(currentHpState_ == HpState::HP_LOW
	   && preHpState_ != HpState::HP_LOW){
		rootNode_ = std::make_unique<EnemyBehaviorTree::EnemyAwakening>(this);
		return;
	}

	if(lengthEnemy2Player <= distanceByRangeTypes_[EnemyAttackRangeType::SHORT_RANGE]){
		// ShortRange 以下なら
		const std::deque<std::string>& comboNameList = this->GetComboList(currentHpState_,EnemyAttackRangeType::SHORT_RANGE);
		comboName = comboNameList[Random::Int(0,static_cast<int>(comboNameList.size() - 1))];

	} else if(lengthEnemy2Player > distanceByRangeTypes_[EnemyAttackRangeType::MIDDLE_RANGE]){
		// MiddleRange より 大きいなら
		const std::deque<std::string>& comboNameList = this->GetComboList(currentHpState_,EnemyAttackRangeType::LONG_RANGE);
		comboName = comboNameList[Random::Int(0,static_cast<int>(comboNameList.size() - 1))];

	} else{
		const std::deque<std::string>& comboNameList = this->GetComboList(currentHpState_,EnemyAttackRangeType::MIDDLE_RANGE);
		comboName = comboNameList[Random::Int(0,static_cast<int>(comboNameList.size() - 1))];
	}

	rootNode_ = std::make_unique<EnemyBehaviorTree::AttackCombo>(this,comboName);
	//dointCombo_ = comboName;

	if(comboName == preComboName_){
		DecideNextNode();
		return;
	}

	preComboName_ = comboName;

	if(!rootNode_){
		assert(0);
	}
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

void Enemy::ActivateAttackCollider(ActionTypes offset){
	attackCollider_->Activate(offset);
}

void Enemy::TerminateAttackCollider(){
	attackCollider_->Terminate();
}