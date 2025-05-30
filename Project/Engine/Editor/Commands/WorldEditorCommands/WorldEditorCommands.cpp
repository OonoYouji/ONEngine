#include "WorldEditorCommands.h"

/// std
#include <iostream>
#include <fstream>
#include <sstream>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/EmptyEntity/EmptyEntity.h"
#include "Engine/Core/ImGui/Math/ImGuiMath.h"

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
