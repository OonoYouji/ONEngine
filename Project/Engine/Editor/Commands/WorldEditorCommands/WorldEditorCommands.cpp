#include "WorldEditorCommands.h"

/// std
#include <iostream>
#include <fstream>
#include <sstream>
#include <numbers>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/ImGui/Math/ImGuiMath.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/Entities/EmptyEntity/EmptyEntity.h"
#include "Engine/ECS/Entity/Entities/Camera/Camera.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"

/// ///////////////////////////////////////////////////
/// ゲームオブジェクトの作成コマンド
/// ///////////////////////////////////////////////////

CreateGameObjectCommand::CreateGameObjectCommand(EntityComponentSystem* _ecs) {
	pECS_ = _ecs;
}

CreateGameObjectCommand::~CreateGameObjectCommand() {}

EDITOR_STATE CreateGameObjectCommand::Execute() {
	generatedEntity_ = pECS_->GenerateEntity<EmptyEntity>();

	EDITOR_STATE state = EDITOR_STATE_RUNNING;
	if (generatedEntity_) {
		state = EDITOR_STATE_FINISH;
	}

	return state;
}

EDITOR_STATE CreateGameObjectCommand::Undo() {
	pECS_->RemoveEntity(generatedEntity_);

	return EDITOR_STATE_FINISH;
}


/// ///////////////////////////////////////////////////
/// オブジェクトの名前変更コマンド
/// ///////////////////////////////////////////////////
EntityRenameCommand::EntityRenameCommand(IEntity* _entity, const std::string& _newName)
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

CreateNewEntityClassCommand::CreateNewEntityClassCommand(IEntity* _entity, const std::string& _outputFilePath)
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

DeleteEntityCommand::DeleteEntityCommand(EntityComponentSystem* _ecs, IEntity* _entity)
	: pECS_(_ecs), pEntity_(_entity) {}

EDITOR_STATE DeleteEntityCommand::Execute() {
	if (!pECS_ || !pEntity_) {
		Console::Log("DeleteEntityCommand : ECS or Entity is nullptr");
		return EDITOR_STATE_FAILED;
	}

	pECS_->RemoveEntity(pEntity_);

	return EDITOR_STATE_FINISH;
}

EDITOR_STATE DeleteEntityCommand::Undo() {
	return EDITOR_STATE_FINISH;
}


/// ///////////////////////////////////////////////////
/// シーンをロードするコマンド
/// ///////////////////////////////////////////////////
LoadSceneCommand::LoadSceneCommand(EntityComponentSystem* _ecs, const std::string& _scenePath)
	: pECS_(_ecs), scenePath_(_scenePath) {

}

EDITOR_STATE LoadSceneCommand::Execute() {
	LoadScene(scenePath_);
	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

EDITOR_STATE LoadSceneCommand::Undo() {
	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

void LoadSceneCommand::LoadScene(const std::string& _scenePath) {

	/// ファイル名が空ならば終了
	if (_scenePath.empty()) {
		Console::Log("Level name is empty.");
		return;
	}

	/// ファイル名が存在しないならば終了
	if (!std::filesystem::exists(_scenePath)) {
		Console::Log("Level file does not exist: " + _scenePath);
		return;
	}

	/// fileを開く
	std::ifstream file(_scenePath);
	if (!file.is_open()) {
		Console::Log("Failed to open level file: " + _scenePath);
		return;
	}


	/// jsonを読み込む
	nlohmann::json jsonData;
	file >> jsonData;

	for (const auto& objectData : jsonData["objects"]) {
		LoadEntity(objectData);
	}

}

IEntity* LoadSceneCommand::LoadEntity(const nlohmann::json& _entityData) {
	/// エンティティの名前を取得
	std::string objectName = _entityData["name"].get<std::string>();
	/// エンティティのタイプを取得
	std::string objectType = _entityData["type"].get<std::string>();

	if (!_entityData.contains("transform") || _entityData["transform"].is_null()) {
		Console::Log("transform not found");
		return nullptr;
	}

	nlohmann::json transformData = _entityData["transform"];
	Vec3 position = transformData["translation"].get<Vec3>();
	Vec3 rotate = transformData["rotation"].get<Vec3>();
	Vec3 scale = transformData["scale"].get<Vec3>();

	/// 座標軸を変換する
	float posY = position.z;
	position.z = position.y;
	position.y = posY;

	/// 回転を変換する
	// Blenderではx=0が真下を向くようになっているので自作エンジン側の0=正面に合わせて変換する
	rotate = -rotate;
	rotate.x += 90.0f * std::numbers::pi_v<float> / 180.0f;
	float rotateY = rotate.z;
	rotate.z = rotate.y;
	rotate.y = rotateY;

	/// スケールを変換する
	float scaleY = scale.z;
	scale.z = scale.y;
	scale.y = scaleY;

	IEntity* entity = nullptr;
	/// エンティティを生成&SRTを設定
	if (objectType == "CAMERA") {
		entity = pECS_->GenerateCamera();
		pECS_->SetMainCamera(static_cast<Camera*>(entity));
	} else {
		entity = pECS_->GenerateEntity<EmptyEntity>();
	}

	if (!entity) {
		Console::Log("Failed to create entity for object: " + objectName);
		return nullptr;
	}

	entity->SetPosition(position);
	entity->SetRotate(rotate);
	entity->SetScale(scale);
	entity->UpdateTransform();
	entity->SetName(objectName);

	if (_entityData.contains("children")) {
		for (auto& childData : _entityData["children"]) {
			/// 子エンティティを再帰的に読み込む
			if (IEntity* child = LoadEntity(childData)) {
				child->SetParent(entity);
			}
		}
	}

	if (objectType == "MESH") {
		MeshRenderer* meshRenderer = entity->AddComponent<MeshRenderer>();
		meshRenderer->SetMeshPath(_entityData["mesh"].get<std::string>());
	} else if (objectType == "LIGHT") {
		entity->AddComponent<DirectionalLight>();
	} else {
		Console::Log("Unknown object type: " + objectType);
		return nullptr;
	}

	return entity;
}
