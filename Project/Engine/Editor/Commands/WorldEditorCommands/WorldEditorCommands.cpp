#include "WorldEditorCommands.h"

/// std
#include <iostream>
#include <fstream>
#include <sstream>
#include <numbers>

/// engine
#include "Engine/Asset/Guid/Guid.h"
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/ImGui/Math/ImGuiMath.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/EntityJsonConverter.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"
#include "Engine/Editor/Clipboard.h"
#include "Engine/Editor/EditCommand.h"

/// ///////////////////////////////////////////////////
/// ゲームオブジェクトの作成コマンド
/// ///////////////////////////////////////////////////

CreateGameObjectCommand::CreateGameObjectCommand(ECSGroup* _ecs) {
	pEcsGroup_ = _ecs;
}

CreateGameObjectCommand::~CreateGameObjectCommand() {}

EDITOR_STATE CreateGameObjectCommand::Execute() {
	generatedEntity_ = pEcsGroup_->GenerateEntity(GenerateGuid(), false);

	EDITOR_STATE state = EDITOR_STATE_RUNNING;
	if (generatedEntity_) {
		state = EDITOR_STATE_FINISH;
	}

	return state;
}

EDITOR_STATE CreateGameObjectCommand::Undo() {
	pEcsGroup_->RemoveEntity(generatedEntity_);

	return EDITOR_STATE_FINISH;
}


/// ///////////////////////////////////////////////////
/// オブジェクトの名前変更コマンド
/// ///////////////////////////////////////////////////
EntityRenameCommand::EntityRenameCommand(GameEntity* _entity, const std::string& _newName)
	: pEntity_(_entity) {
	oldName_ = pEntity_->GetName();
	newName_ = _newName;
}

EDITOR_STATE EntityRenameCommand::Execute() {

	if (pEntity_ == nullptr) {
		Console::Log("EntityRenameCommand : Entity is nullptr");
		return EDITOR_STATE_RUNNING;
	}

	pEntity_->SetName(newName_);

	return EDITOR_STATE_FINISH;
}

EDITOR_STATE EntityRenameCommand::Undo() {

	if (pEntity_) {
		pEntity_->SetName(oldName_);
	} else {
		Console::Log("EntityRenameCommand : Entity is nullptr");
	}

	return EDITOR_STATE::EDITOR_STATE_FINISH;
}


/// ///////////////////////////////////////////////////
/// シーンにあるオブジェクトから新しいクラスを作る
/// ///////////////////////////////////////////////////

CreateNewEntityClassCommand::CreateNewEntityClassCommand(GameEntity* _entity, const std::string& _outputFilePath)
	: pEntity_(_entity) {
	pEntity_ = _entity;
	sourceClassPath_ = "Engine/Editor/Commands/WorldEditorCommands/SourceEntity";
	sourceClassName_ = "SourceEntity";
	outputFilePath_ = _outputFilePath;
}

EDITOR_STATE CreateNewEntityClassCommand::Execute() {
	if (pEntity_->GetName().empty()) {
		Console::Log("CreateNewEntityClassCommand : Entity name is empty");
		return EDITOR_STATE_FAILED;
	}

	CreateNewClassFile(sourceClassPath_ + ".h", outputFilePath_, pEntity_->GetName() + ".h");
	CreateNewClassFile(sourceClassPath_ + ".cpp", outputFilePath_, pEntity_->GetName() + ".cpp");

	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

EDITOR_STATE CreateNewEntityClassCommand::Undo() {
	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

std::string CreateNewEntityClassCommand::ReplaceAll(const std::string& _str, const std::string& _from, const std::string& _to) {
	std::string result = _str;
	size_t start_pos = 0;
	while ((start_pos = result.find(_from, start_pos)) != std::string::npos) {
		result.replace(start_pos, _from.length(), _to);
		start_pos += _to.length(); // `to` の長さ分進める
	}
	return result;;
}

EDITOR_STATE CreateNewEntityClassCommand::CreateNewClassFile(const std::string& _srcFilePath, const std::string& _outputFileName, const std::string& _newClassName) {

	// ファイルを読み込む
	std::ifstream inputFile(_srcFilePath);
	if (!inputFile) {
		Console::Log("ファイルを開けません: " + _srcFilePath);
		return EDITOR_STATE_FAILED;
	}

	std::stringstream buffer;
	buffer << inputFile.rdbuf(); // ファイル全体を読み込む
	std::string content = buffer.str();

	inputFile.close();

	// 置き換える
	content = ReplaceAll(content, sourceClassName_, pEntity_->GetName());

	// 新しいファイルに書き込む
	std::ofstream outputFile(_outputFileName + "/" + _newClassName);
	if (!outputFile) {
		Console::Log("ファイルを書き込めません: " + _outputFileName);
		return EDITOR_STATE_FAILED;
	}

	outputFile << content;
	outputFile.close();

	return EDITOR_STATE_FINISH;
}


/// ///////////////////////////////////////////////////
/// エンティティを削除するコマンド
/// ///////////////////////////////////////////////////

DeleteEntityCommand::DeleteEntityCommand(ECSGroup* _ecs, GameEntity* _entity)
	: pEcsGroup_(_ecs), pEntity_(_entity) {
}

EDITOR_STATE DeleteEntityCommand::Execute() {
	if (!pEcsGroup_ || !pEntity_) {
		Console::Log("DeleteEntityCommand : ECS or Entity is nullptr");
		return EDITOR_STATE_FAILED;
	}

	pEcsGroup_->RemoveEntity(pEntity_);

	return EDITOR_STATE_FINISH;
}

EDITOR_STATE DeleteEntityCommand::Undo() {
	return EDITOR_STATE_FINISH;
}


/// ///////////////////////////////////////////////////
/// プレハブを作成するコマンド
/// ///////////////////////////////////////////////////
CreatePrefabCommand::CreatePrefabCommand(GameEntity* _entity)
	: pEntity_(_entity) {
	if (pEntity_ == nullptr) {
		Console::Log("CreatePrefabCommand : Entity is nullptr");
		return;
	}

	/// プレハブのパスを設定
	prefabName_ = pEntity_->GetName() + ".prefab";
}

EDITOR_STATE CreatePrefabCommand::Execute() {

	/// ディレクトリがあるのかチェック
	if (!std::filesystem::exists(prefabPath_)) {
		std::filesystem::create_directories(prefabPath_);
	}


	/// jsonに変換
	nlohmann::json entityJson = EntityJsonConverter::ToJson(pEntity_);

	/// jsonが空ならログを残して終了
	if (entityJson.empty()) {
		Console::Log("CreatePrefabCommand : EntityJson is empty");
		return EDITOR_STATE_FAILED;
	}


	/// ファイルに出力
	std::string prefabPath = "./Assets/Prefabs/" + prefabName_;
	std::ofstream outputFile(prefabPath);
	if (!outputFile.is_open()) {
		Console::Log("CreatePrefabCommand : Failed to open prefab file: " + prefabPath);
		return EDITOR_STATE_FAILED;
	}

	outputFile << entityJson.dump(4);
	outputFile.close();
	Console::Log("Prefab created: " + prefabPath);

	return EDITOR_STATE_FINISH;
}

EDITOR_STATE CreatePrefabCommand::Undo() {
	return EDITOR_STATE_FINISH;
}

CopyEntityCommand::CopyEntityCommand(GameEntity* _entity) : pEntity_(_entity) {}

EDITOR_STATE CopyEntityCommand::Execute() {
	/// jsonに変換
	entityJson_ = EntityJsonConverter::ToJson(pEntity_);
	EditCommand::SetClipboardData(entityJson_);

	/// チェック
	nlohmann::json* copiedEntity = EditCommand::GetClipboardData<nlohmann::json>();
	if (copiedEntity) {
		/// stringに変換してログに出す
		std::string jsonString = copiedEntity->dump(4);
		Console::Log("Copied Entity JSON:\n" + jsonString);
	}


	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

EDITOR_STATE CopyEntityCommand::Undo() {
	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

PasteEntityCommand::PasteEntityCommand(ECSGroup* _ecs) : pEcsGroup_(_ecs) {}

EDITOR_STATE PasteEntityCommand::Execute() {
	/// クリップボードからデータを取得
	nlohmann::json* copiedEntity = EditCommand::GetClipboardData<nlohmann::json>();
	if (!copiedEntity || copiedEntity->empty()) {
		Console::Log("PasteEntityCommand : Clipboard is empty or invalid");
		return EDITOR_STATE_FAILED;
	}

	/// jsonからエンティティを生成
	std::string originalName = (*copiedEntity)["name"].get<std::string>();

	uint32_t count = pEcsGroup_->CountEntity(originalName);
	pastedEntity_ = pEcsGroup_->GenerateEntity(GenerateGuid(), DebugConfig::isDebugging);
	EntityJsonConverter::FromJson(*copiedEntity, pastedEntity_, pEcsGroup_->GetGroupName());
	if (!pastedEntity_) {
		Console::Log("PasteEntityCommand : Failed to create entity from JSON");
		return EDITOR_STATE_FAILED;
	}

	/// 新しい名前を設定
	std::string newName = originalName;
	if (count > 0) {
		newName += "_" + std::to_string(count);
	}

	if (DebugConfig::isDebugging) {
		newName += "(Clone)";
	}

	pastedEntity_->SetName(newName);

	return EDITOR_STATE_FINISH;
}

EDITOR_STATE PasteEntityCommand::Undo() {
	if (pastedEntity_) {
		pEcsGroup_->RemoveEntity(pastedEntity_);
		pastedEntity_ = nullptr;
		return EDITOR_STATE_FINISH;
	}
	return EDITOR_STATE_FAILED;
}
