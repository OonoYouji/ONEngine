#pragma once

/// std
#include <string>

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "Engine/Asset/Guid/Guid.h"

/// editor
#include "../IEditCommand.h"

namespace ONEngine {
class GameEntity;
class ECSGroup;
}


/// ///////////////////////////////////////////////////
/// ゲームオブジェクトの作成コマンド
/// ///////////////////////////////////////////////////
namespace Editor {

class CreateGameObjectCommand : public IEditCommand {
public:
	CreateGameObjectCommand(ONEngine::ECSGroup* _ecs, const std::string& _name = "NewEntity", ONEngine::GameEntity* _parentEntity = nullptr);
	~CreateGameObjectCommand();

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	ONEngine::ECSGroup* pEcsGroup_ = nullptr;
	ONEngine::GameEntity* generatedEntity_ = nullptr;
	ONEngine::Guid generatedGuid_;
	ONEngine::Guid parentGuid_;
	const std::string entityName_;
};


/// ///////////////////////////////////////////////////
/// シーンに配置してあるオブジェクトの名前をへんこうする 
/// ///////////////////////////////////////////////////
class EntityRenameCommand : public IEditCommand {
public:
	EntityRenameCommand(ONEngine::GameEntity* _entity, const std::string& _newName);
	~EntityRenameCommand() = default;

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	ONEngine::GameEntity* pEntity_;
	std::string oldName_ = "";
	std::string newName_ = "";
};


/// ///////////////////////////////////////////////////
/// シーンにあるオブジェクトから新しいクラスを作る
/// ///////////////////////////////////////////////////
class CreateNewEntityClassCommand : public IEditCommand {
public:
	CreateNewEntityClassCommand(ONEngine::GameEntity* _entity, const std::string& _outputFilePath);
	~CreateNewEntityClassCommand() = default;

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

	EDITOR_STATE CreateNewClassFile(const std::string& _srcFilePath, const std::string& _outputFileName, const std::string& _newClassName);

private:
	ONEngine::GameEntity* pEntity_ = nullptr;

	std::string sourceClassPath_;
	std::string sourceClassName_;
	std::string outputFilePath_;
};


/// ///////////////////////////////////////////////////
/// プレハブを作成するコマンド
/// ///////////////////////////////////////////////////
class CreatePrefabCommand : public IEditCommand {
public:
	CreatePrefabCommand(ONEngine::GameEntity* _entity);
	~CreatePrefabCommand() = default;

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

	/// @brief 再帰的にエンティティをシリアライズする
	void SerializeRecursive(ONEngine::GameEntity* _entity, nlohmann::json& _json);

private:
	ONEngine::GameEntity* pEntity_ = nullptr;
	std::string prefabPath_ = "./Assets/Prefabs/";
	std::string prefabName_ = "NewPrefab.json";
};


/// ///////////////////////////////////////////////////
/// エンティティを削除するコマンド
/// ///////////////////////////////////////////////////
class DeleteEntityCommand : public IEditCommand {
public:
	DeleteEntityCommand(ONEngine::ECSGroup* _ecs, ONEngine::GameEntity* _entity);
	~DeleteEntityCommand() = default;

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	ONEngine::ECSGroup* pEcsGroup_;
	ONEngine::GameEntity* pEntity_;
};


/// ///////////////////////////////////////////////////
/// エンティティをコピーするコマンド
/// ///////////////////////////////////////////////////
class CopyEntityCommand : public IEditCommand {
public:
	CopyEntityCommand(ONEngine::GameEntity* _entity);
	~CopyEntityCommand() = default;

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;
private:
	ONEngine::GameEntity* pEntity_;
	nlohmann::json entityJson_;
};


/// ///////////////////////////////////////////////////
/// エンティティをペーストするコマンド
/// ///////////////////////////////////////////////////
class PasteEntityCommand : public IEditCommand {
public:
	PasteEntityCommand(ONEngine::ECSGroup* _ecs, ONEngine::GameEntity* _selectedEntity);
	~PasteEntityCommand() = default;

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	ONEngine::ECSGroup* pEcsGroup_;
	ONEngine::GameEntity* pSelectedEntity_ = nullptr;
	ONEngine::GameEntity* pastedEntity_ = nullptr;
};

/// ///////////////////////////////////////////////////
/// エンティティの親子付けを変更するコマンド
/// ///////////////////////////////////////////////////
class ChangeEntityParentCommand : public IEditCommand {
public:
	ChangeEntityParentCommand(ONEngine::GameEntity* _entity, ONEngine::GameEntity* _newParent);
	~ChangeEntityParentCommand() = default;
	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;
private:
	ONEngine::GameEntity* pEntity_ = nullptr;
	ONEngine::GameEntity* pNewParent_ = nullptr;
	ONEngine::GameEntity* pOldParent_ = nullptr;
};

} /// Editor
