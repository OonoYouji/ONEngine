#include "ComponentEditCommands.h"


/// std
#include <fstream>

/// external
#include <nlohmann/json.hpp>
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/EntityJsonConverter.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Script/MonoScriptEngine.h"
#include "ComponentJsonConverter.h"

using namespace ONEngine;
using namespace Editor;

/// ////////////////////////////////////////////////
/// エンティティのデータ出力コマンド
/// ////////////////////////////////////////////////

EntityDataOutputCommand::EntityDataOutputCommand(GameEntity* _entity) {
	pEntity_ = _entity;
	outputFilePath_ = "Assets/Entities/" + pEntity_->GetName() + ".entity";
}

EDITOR_STATE EntityDataOutputCommand::Execute() {
	if (!pEntity_) return EDITOR_STATE_FAILED;

	nlohmann::json entityJson = EntityJsonConverter::ToJson(pEntity_);

	std::filesystem::path path(outputFilePath_);
	std::filesystem::create_directories(path.parent_path());

	std::ofstream ofs(outputFilePath_);
	if (!ofs) {
		Console::LogError("ファイルを開けませんでした: " + outputFilePath_);
		return EDITOR_STATE::EDITOR_STATE_FAILED;
	}

	ofs << entityJson.dump(4);

	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

EDITOR_STATE EntityDataOutputCommand::Undo() {
	return EDITOR_STATE::EDITOR_STATE_FINISH;
}


/// ///////////////////////////////////////////////
/// エンティティのデータ入力コマンド
/// ///////////////////////////////////////////////

EntityDataInputCommand::EntityDataInputCommand(GameEntity* _entity) : pEntity_(_entity) {
	inputFilePath_ = "Assets/Entities/" + pEntity_->GetName() + ".entity";
}

EDITOR_STATE EntityDataInputCommand::Execute() {
	if (!pEntity_) return EDITOR_STATE_FAILED;

	/// fileを開く
	std::ifstream ifs(inputFilePath_);
	if (!ifs) {
		Console::LogError("ファイルを開けませんでした: " + inputFilePath_);
		return EDITOR_STATE::EDITOR_STATE_FAILED;
	}

	/// jsonを読み込む
	nlohmann::json entityJson;
	ifs >> entityJson;
	ifs.close();

	/// エンティティの構成を復元
	EntityJsonConverter::FromJson(entityJson, pEntity_, pEntity_->GetECSGroup()->GetGroupName());

	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

EDITOR_STATE EntityDataInputCommand::Undo() {
	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

void EntityDataInputCommand::SetEntity(GameEntity* _entity) {
	pEntity_ = _entity;
	inputFilePath_ = "Assets/Jsons/" + pEntity_->GetName() + "Components.json";
}


/// ///////////////////////////////////////////////
/// Componentの追加
/// ///////////////////////////////////////////////

AddComponentCommand::AddComponentCommand(GameEntity* _entity, const std::string& _componentName) {
	pEntity_ = _entity;
	componentName_ = _componentName;
}

EDITOR_STATE AddComponentCommand::Execute() {
	if (!pEntity_) {
		Console::Log("AddComponentCommand: Entity is nullptr");
		return EDITOR_STATE_FAILED;
	}

	IComponent* component = pEntity_->AddComponent(componentName_);
	if (!component) {
		Console::Log("AddComponentCommand: コンポーネントの追加に失敗しました: " + componentName_);
		return EDITOR_STATE_FAILED;
	}

	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

EDITOR_STATE AddComponentCommand::Undo() {

	return EDITOR_STATE::EDITOR_STATE_FINISH;
}


/// ///////////////////////////////////////////////
/// Componentの削除
/// ///////////////////////////////////////////////

RemoveComponentCommand::RemoveComponentCommand(GameEntity* _entity, const std::string& _componentName, std::unordered_map<size_t, IComponent*>::iterator* _resultItr)
	: pEntity_(_entity), componentName_(_componentName), pIterator_(_resultItr) {}


EDITOR_STATE RemoveComponentCommand::Execute() {

	if (!pEntity_) {
		Console::Log("[error] RemoveComponentCommand: Entity is nullptr");
		return EDITOR_STATE_FAILED;
	}

	if (!pEntity_->GetComponent(componentName_)) {
		Console::Log("[error] RemoveComponentCommand: コンポーネントが見つかりません: " + componentName_);
		return EDITOR_STATE_FAILED;
	}


	if (pIterator_) {
		*pIterator_ = pEntity_->GetComponents().find(GetComponentHash(componentName_));
		(*pIterator_)++;
	}

	/// 削除
	pEntity_->RemoveComponent(componentName_);

	return EDITOR_STATE_FINISH;
}

EDITOR_STATE RemoveComponentCommand::Undo() {
	return EDITOR_STATE_FINISH;
}



/// ////////////////////////////////////////////////
/// ReloadAllScriptsCommand
/// ////////////////////////////////////////////////

ReloadAllScriptsCommand::ReloadAllScriptsCommand(ECSGroup* _ecs, SceneManager* _sceneManager)
	: pEcsGroup_(_ecs), pSceneManager_(_sceneManager) {}

EDITOR_STATE ReloadAllScriptsCommand::Execute() {

	/// シーンを読み直す
	pSceneManager_->SetNextScene(pSceneManager_->GetCurrentSceneName());
	MonoScriptEngine::GetInstance().HotReload();

	return EDITOR_STATE_FINISH;
}

EDITOR_STATE ReloadAllScriptsCommand::Undo() {
	return EDITOR_STATE_FINISH;
}

