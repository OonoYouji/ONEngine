#pragma once

/// std
#include <string>

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

	CreateGameObjectCommand(class EntityComponentSystem* _ecs);
	~CreateGameObjectCommand();

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:

	class EntityComponentSystem* pECS_ = nullptr;
	class IEntity* generatedEntity_ = nullptr;
};


/// ///////////////////////////////////////////////////
/// シーンに配置してあるオブジェクトの名前をへんこうする 
/// ///////////////////////////////////////////////////
class EntityRenameCommand : public IEditorCommand {
public:
	EntityRenameCommand(class IEntity* _entity);
	~EntityRenameCommand() = default;

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	class IEntity* pEntity_;
	std::string oldName_ = "";
	std::string newName_ = "";
};


/// ///////////////////////////////////////////////////
/// シーンにあるオブジェクトから新しいクラスを作る
/// ///////////////////////////////////////////////////
class CreateNewEntityClassCommand : public IEditorCommand {
public:
	CreateNewEntityClassCommand(class IEntity* _entity, const std::string& _outputFilePath);
	~CreateNewEntityClassCommand() = default;

	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

	std::string ReplaceAll(const std::string& _str, const std::string& _from, const std::string& _to);

	EDITOR_STATE CreateNewClassFile(const std::string& _srcFilePath, const std::string& _outputFileName, const std::string& _newClassName);

private:
	class IEntity* pEntity_ = nullptr;

	std::string sourceClassPath_;
	std::string sourceClassName_;
	std::string outputFilePath_;

};