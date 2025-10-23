#pragma once

/// std
#include <string>

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "../Interface/IEditorCommand.h"


/// ///////////////////////////////////////////////////
/// ゲームオブジェクトの作成コマンド
/// ///////////////////////////////////////////////////
class CreateGameObjectCommand : public IEditorCommand {
public:
	/// =========================================
	/// public : methods
	/// =========================================

	CreateGameObjectCommand(class ECSGroup* _ecs);
	~CreateGameObjectCommand();

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:

	class ECSGroup* pECSGroup_ = nullptr;
	class GameEntity* generatedEntity_ = nullptr;
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

	std::string ReplaceAll(const std::string& _str, const std::string& _from, const std::string& _to);

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
	/// =========================================
	/// public : methods
	/// =========================================

	DeleteEntityCommand(class ECSGroup* _ecs, class GameEntity* _entity);
	~DeleteEntityCommand() = default;

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	/// =========================================
	/// private : objects
	/// =========================================

	class ECSGroup* pECSGroup_;
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
	PasteEntityCommand(class ECSGroup* _ecs);
	~PasteEntityCommand() = default;
	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;
private:
	class ECSGroup* pECSGroup_;
	class GameEntity* pastedEntity_ = nullptr;
};
