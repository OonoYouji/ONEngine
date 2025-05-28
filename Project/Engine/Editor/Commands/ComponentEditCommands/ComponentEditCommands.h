#pragma once

/// std
#include <typeindex>

/// engine
#include "../Interface/IEditorCommand.h"


class ComponentEditCommands : public IEditorCommand {
public:
	/// ===============================================
	/// public : methods
	/// ===============================================

	ComponentEditCommands() = default;
	~ComponentEditCommands() override = default;

	/// @brief コマンドの実行
	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;


private:
	/// ===============================================
	/// private : objects
	/// ===============================================
};



/// ///////////////////////////////////////////////
/// エンティティのデータ出力コマンド
/// ///////////////////////////////////////////////
class EntityDataOutputCommand : public IEditorCommand {
public:
	/// ================================================
	/// public : methods
	/// ================================================

	EntityDataOutputCommand(class IEntity* _entity);
	~EntityDataOutputCommand() override = default;

	/// @brief コマンドの実行
	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	/// ================================================
	/// private : objects
	/// ================================================

	class IEntity* pEntity_ = nullptr;
	//std::string outputData_ = "";

};