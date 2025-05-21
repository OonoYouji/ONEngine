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
//class CreateNewEntityClassCommand : public IEditorCommand {
//public:
//
//
//
//private:
//};