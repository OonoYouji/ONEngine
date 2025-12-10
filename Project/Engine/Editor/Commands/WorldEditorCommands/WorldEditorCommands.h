#pragma once

/// std
#include <string>

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "../IEditorCommand.h"
#include "Engine/Asset/Guid/Guid.h"


/// ///////////////////////////////////////////////////
/// ゲームオブジェクトの作成コマンド
/// ///////////////////////////////////////////////////
namespace ONEngine {

class CreateGameObjectCommand : public IEditorCommand {
public:
	CreateGameObjectCommand(class ECSGroup* _ecs, const std::string& _name = "NewEntity", class GameEntity* _parentEntity = nullptr);
	~CreateGameObjectCommand();

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	class ECSGroup* pEcsGroup_ = nullptr;
	class GameEntity* generatedEntity_ = nullptr;
	Guid generatedGuid_;
	Guid parentGuid_;
	const std::string entityName_;
};


/// ///////////////////////////////////////////////////
/// シーンに配置してあるオブジェクトの名前をへんこうする 
/// ///////////////////////////////////////////////////
class EntityRenameCommand : public IEditorCommand {
public:
	EntityRenameCommand(class GameEntity* _entity, const std::string& _newName);
	~EntityRenameCommand() = default;

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	class GameEntity* pEntity_;
	std::string oldName_ = "";
	std::string newName_ = "";
};


/// ///////////////////////////////////////////////////
/// シーンにあるオブジェクトから新しいクラスを作る
/// ///////////////////////////////////////////////////
class CreateNewEntityClassCommand : public IEditorCommand {
public:
	CreateNewEntityClassCommand(class GameEntity* _entity, const std::string& _outputFilePath);
	~CreateNewEntityClassCommand() = default;

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

	EDITOR_STATE CreateNewClassFile(const std::string& _srcFilePath, const std::string& _outputFileName, const std::string& _newClassName);

private:
	class GameEntity* pEntity_ = nullptr;

	std::string sourceClassPath_;
	std::string sourceClassName_;
	std::string outputFilePath_;
};


/// ///////////////////////////////////////////////////
/// プレハブを作成するコマンド
/// ///////////////////////////////////////////////////
class CreatePrefabCommand : public IEditorCommand {
public:
	CreatePrefabCommand(class GameEntity* _entity);
	~CreatePrefabCommand() = default;

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

	/// @brief 再帰的にエンティティをシリアライズする
	void SerializeRecursive(class GameEntity* _entity, nlohmann::json& _json);

private:
	class GameEntity* pEntity_ = nullptr;
	std::string prefabPath_ = "./Assets/Prefabs/";
	std::string prefabName_ = "NewPrefab.json";
};


/// ///////////////////////////////////////////////////
/// エンティティを削除するコマンド
/// ///////////////////////////////////////////////////
class DeleteEntityCommand : public IEditorCommand {
public:
	DeleteEntityCommand(class ECSGroup* _ecs, class GameEntity* _entity);
	~DeleteEntityCommand() = default;

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	class ECSGroup* pEcsGroup_;
	class GameEntity* pEntity_;
};


/// ///////////////////////////////////////////////////
/// エンティティをコピーするコマンド
/// ///////////////////////////////////////////////////
class CopyEntityCommand : public IEditorCommand {
public:
	CopyEntityCommand(class GameEntity* _entity);
	~CopyEntityCommand() = default;

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;
private:
	class GameEntity* pEntity_;
	nlohmann::json entityJson_;
};


/// ///////////////////////////////////////////////////
/// エンティティをペーストするコマンド
/// ///////////////////////////////////////////////////
class PasteEntityCommand : public IEditorCommand {
public:
	PasteEntityCommand(class ECSGroup* _ecs, class GameEntity* _selectedEntity);
	~PasteEntityCommand() = default;

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	class ECSGroup* pEcsGroup_;
	class GameEntity* pSelectedEntity_ = nullptr;
	class GameEntity* pastedEntity_ = nullptr;
};

/// ///////////////////////////////////////////////////
/// エンティティの親子付けを変更するコマンド
/// ///////////////////////////////////////////////////
class ChangeEntityParentCommand : public IEditorCommand {
public:
	ChangeEntityParentCommand(class GameEntity* _entity, class GameEntity* _newParent);
	~ChangeEntityParentCommand() = default;
	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;
private:
	class GameEntity* pEntity_ = nullptr;
	class GameEntity* pNewParent_ = nullptr;
	class GameEntity* pOldParent_ = nullptr;
};

} /// ONEngine
