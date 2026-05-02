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
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/EntityJsonConverter.h"

/// editor
#include "Engine/Editor/Clipboard/Clipboard.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"
#include "Engine/Editor/Manager/EditCommand.h"
#include "Engine/Editor/Math/ImGuiMath.h"


using namespace Editor;

/// ///////////////////////////////////////////////////
/// ゲームオブジェクトの作成コマンド
/// ///////////////////////////////////////////////////

CreateGameObjectCommand::CreateGameObjectCommand(ONEngine::ECSGroup* _ecs, const std::string& _name, ONEngine::GameEntity* _parentEntity)
	: entityName_(_name) {
	pEcsGroup_ = _ecs;
	parentGuid_ = ONEngine::Guid::kInvalid;
	if (_parentEntity) {
		parentGuid_ = _parentEntity->GetGuid();
	}
}

CreateGameObjectCommand::~CreateGameObjectCommand() {}

EDITOR_STATE CreateGameObjectCommand::Execute() {

	/// 生成するEntityのGuidを生成
	if (!generatedGuid_.CheckValid()) {
		generatedGuid_ = ONEngine::GenerateGuid();
	}

	generatedEntity_ = pEcsGroup_->GenerateEntity(generatedGuid_, false);

	EDITOR_STATE state = EDITOR_STATE_RUNNING;
	if (generatedEntity_) {
		generatedEntity_->SetName(entityName_);

		state = EDITOR_STATE_FINISH;

		/// 親子関係の設定
		if (parentGuid_.CheckValid()) {
			ONEngine::GameEntity* entity = pEcsGroup_->GetEntityFromGuid(parentGuid_);
			if (entity) {
				generatedEntity_->SetParent(entity);
			}
		}
	}

	return state;
}

EDITOR_STATE CreateGameObjectCommand::Undo() {
	if (parentGuid_.CheckValid()) {
		ONEngine::GameEntity* parentEntity = pEcsGroup_->GetEntityFromGuid(parentGuid_);
		if (parentEntity && generatedEntity_) {
			generatedEntity_->SetParent(nullptr);
		}
	}


	pEcsGroup_->RemoveEntity(generatedEntity_);

	return EDITOR_STATE_FINISH;
}


/// ///////////////////////////////////////////////////
/// オブジェクトの名前変更コマンド
/// ///////////////////////////////////////////////////
EntityRenameCommand::EntityRenameCommand(ONEngine::GameEntity* _entity, const std::string& _newName)
	: pEntity_(_entity) {
	oldName_ = pEntity_->GetName();
	newName_ = _newName;
}

EDITOR_STATE EntityRenameCommand::Execute() {

	if (pEntity_ == nullptr) {
		ONEngine::Console::Log("EntityRenameCommand : Entity is nullptr");
		return EDITOR_STATE_RUNNING;
	}

	pEntity_->SetName(newName_);

	return EDITOR_STATE_FINISH;
}

EDITOR_STATE EntityRenameCommand::Undo() {

	if (pEntity_) {
		pEntity_->SetName(oldName_);
	} else {
		ONEngine::Console::Log("EntityRenameCommand : Entity is nullptr");
	}

	return EDITOR_STATE::EDITOR_STATE_FINISH;
}


/// ///////////////////////////////////////////////////
/// シーンにあるオブジェクトから新しいクラスを作る
/// ///////////////////////////////////////////////////

CreateNewEntityClassCommand::CreateNewEntityClassCommand(ONEngine::GameEntity* _entity, const std::string& _outputFilePath)
	: pEntity_(_entity) {
	pEntity_ = _entity;
	sourceClassPath_ = "Engine/Editor/Commands/WorldEditorCommands/SourceEntity";
	sourceClassName_ = "SourceEntity";
	outputFilePath_ = _outputFilePath;
}

EDITOR_STATE CreateNewEntityClassCommand::Execute() {
	if (pEntity_->GetName().empty()) {
		ONEngine::Console::LogError("CreateNewEntityClassCommand : Entity name is empty");
		return EDITOR_STATE_FAILED;
	}

	CreateNewClassFile(sourceClassPath_ + ".h", outputFilePath_, pEntity_->GetName() + ".h");
	CreateNewClassFile(sourceClassPath_ + ".cpp", outputFilePath_, pEntity_->GetName() + ".cpp");

	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

EDITOR_STATE CreateNewEntityClassCommand::Undo() {
	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

EDITOR_STATE CreateNewEntityClassCommand::CreateNewClassFile(const std::string& _srcFilePath, const std::string& _outputFileName, const std::string& _newClassName) {

	// ファイルを読み込む
	std::ifstream inputFile(_srcFilePath);
	if (!inputFile) {
		ONEngine::Console::LogError("ファイルを開けません: " + _srcFilePath);
		return EDITOR_STATE_FAILED;
	}

	std::stringstream buffer;
	buffer << inputFile.rdbuf(); // ファイル全体を読み込む
	std::string content = buffer.str();

	inputFile.close();

	// 置き換える
	ONEngine::FileSystem::ReplaceAll(&content, sourceClassName_, pEntity_->GetName());

	// 新しいファイルに書き込む
	std::ofstream outputFile(_outputFileName + "/" + _newClassName);
	if (!outputFile) {
		ONEngine::Console::LogError("ファイルを書き込めません: " + _outputFileName);
		return EDITOR_STATE_FAILED;
	}

	outputFile << content;
	outputFile.close();

	return EDITOR_STATE_FINISH;
}


/// ///////////////////////////////////////////////////
/// エンティティを削除するコマンド
/// ///////////////////////////////////////////////////

DeleteEntityCommand::DeleteEntityCommand(ONEngine::ECSGroup* _ecs, ONEngine::GameEntity* _entity)
	: pEcsGroup_(_ecs), pEntity_(_entity) {
}

EDITOR_STATE DeleteEntityCommand::Execute() {
	if (!pEcsGroup_ || !pEntity_) {
		ONEngine::Console::LogError("DeleteEntityCommand : ECS or Entity is nullptr");
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
CreatePrefabCommand::CreatePrefabCommand(ONEngine::GameEntity* _entity)
	: pEntity_(_entity) {
	if (pEntity_ == nullptr) {
		ONEngine::Console::LogError("CreatePrefabCommand : Entity is nullptr");
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
	nlohmann::json entityJson = ONEngine::EntityJsonConverter::ToJson(pEntity_);

	/// 子の要素も入れる
	SerializeRecursive(pEntity_, entityJson);


	/// jsonが空ならログを残して終了
	if (entityJson.empty()) {
		ONEngine::Console::LogError("CreatePrefabCommand : EntityJson is empty");
		return EDITOR_STATE_FAILED;
	}


	/// ファイルに出力
	std::string prefabPath = "./Assets/Prefabs/" + prefabName_;
	std::ofstream outputFile(prefabPath);
	if (!outputFile.is_open()) {
		ONEngine::Console::LogError("CreatePrefabCommand : Failed to open prefab file: " + prefabPath);
		return EDITOR_STATE_FAILED;
	}

	outputFile << entityJson.dump(4);
	outputFile.close();
	ONEngine::Console::Log("Prefab created: " + prefabPath);

	return EDITOR_STATE_FINISH;
}

EDITOR_STATE CreatePrefabCommand::Undo() {
	return EDITOR_STATE_FINISH;
}

void CreatePrefabCommand::SerializeRecursive(ONEngine::GameEntity* _entity, nlohmann::json& _json) {
	/// 子の要素も入れる
	for (auto& child : _entity->GetChildren()) {
		/// クローンオブジェクトはスキップ
		if (child->GetId() < 0) {
			continue;
		}

		nlohmann::json childJson = ONEngine::EntityJsonConverter::ToJson(child);
		SerializeRecursive(child, childJson);
		_json["children"].push_back(childJson);
	}
}


/// ///////////////////////////////////////////////////
/// エンティティをコピーするコマンド
/// ///////////////////////////////////////////////////

CopyEntityCommand::CopyEntityCommand(ONEngine::GameEntity* _entity) : pEntity_(_entity) {}

EDITOR_STATE CopyEntityCommand::Execute() {
	/// jsonに変換
	entityJson_ = ONEngine::EntityJsonConverter::ToJson(pEntity_);
	EditCommand::SetClipboardData(entityJson_);

	/// チェック
	nlohmann::json* copiedEntity = EditCommand::GetClipboardData<nlohmann::json>();
	if (copiedEntity) {
		/// stringに変換してログに出す
		std::string jsonString = copiedEntity->dump(4);
		ONEngine::Console::Log("Copied Entity JSON:\n" + jsonString);
	}


	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

EDITOR_STATE CopyEntityCommand::Undo() {
	/// 特にやることなし
	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

PasteEntityCommand::PasteEntityCommand(ONEngine::ECSGroup* _ecs, ONEngine::GameEntity* _selectedEntity)
	: pEcsGroup_(_ecs), pSelectedEntity_(_selectedEntity) {
}

EDITOR_STATE PasteEntityCommand::Execute() {
	/// クリップボードからデータを取得
	nlohmann::json* copiedEntity = EditCommand::GetClipboardData<nlohmann::json>();
	if (!copiedEntity || copiedEntity->empty()) {
		ONEngine::Console::LogError("PasteEntityCommand : Clipboard is empty or invalid");
		return EDITOR_STATE_FAILED;
	}

	/// jsonからエンティティを生成
	std::string originalName = (*copiedEntity)["name"].get<std::string>();

	uint32_t count = pEcsGroup_->CountEntity(originalName);
	pastedEntity_ = pEcsGroup_->GenerateEntity(ONEngine::GenerateGuid(), ONEngine::DebugConfig::isDebugging);
	ONEngine::EntityJsonConverter::FromJson(*copiedEntity, pastedEntity_, pEcsGroup_->GetGroupName());
	if (!pastedEntity_) {
		ONEngine::Console::LogError("PasteEntityCommand : Failed to create entity from JSON");
		return EDITOR_STATE_FAILED;
	}

	/// 新しい名前を設定
	std::string newName = originalName;
	if (count > 0) {
		newName += "_" + std::to_string(count);
	}

	if (ONEngine::DebugConfig::isDebugging) {
		newName += "(Clone)";
	}

	pastedEntity_->SetName(newName);
	if (pSelectedEntity_) {
		pastedEntity_->SetParent(pSelectedEntity_);
	}

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


/// ///////////////////////////////////////////////////
/// エンティティの親子関係を変更するコマンド
/// ///////////////////////////////////////////////////

ChangeEntityParentCommand::ChangeEntityParentCommand(ONEngine::GameEntity* _entity, ONEngine::GameEntity* _newParent)
	: pEntity_(_entity), pNewParent_(_newParent) {
}

EDITOR_STATE ChangeEntityParentCommand::Execute() {
	if (!pEntity_) {
		ONEngine::Console::LogError("ChangeEntityParentCommand : Entity is nullptr");
		return EDITOR_STATE_FAILED;
	}
	/// 古い親を保存
	pOldParent_ = pEntity_->GetParent();
	/// 親を変更
	pEntity_->SetParent(pNewParent_);
	return EDITOR_STATE_FINISH;
}

EDITOR_STATE ChangeEntityParentCommand::Undo() {
	if (!pEntity_) {
		ONEngine::Console::LogError("ChangeEntityParentCommand : Entity is nullptr");
		return EDITOR_STATE_FAILED;
	}

	pEntity_->RemoveParent();

	/// 親を元に戻す
	if (pOldParent_) {
		pEntity_->SetParent(pOldParent_);
	}
	return EDITOR_STATE_FINISH;
}
