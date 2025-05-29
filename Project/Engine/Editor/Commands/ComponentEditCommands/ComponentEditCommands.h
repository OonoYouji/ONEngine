#pragma once

/// std
#include <typeindex>
#include <string>

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
	std::string outputFilePath_;

};

/// ///////////////////////////////////////////////
/// エンティティのデータ入力コマンド
/// ///////////////////////////////////////////////
class EntityDataInputCommand : public IEditorCommand {
public:
	/// ================================================
	/// public : methods
	/// ================================================
	
	EntityDataInputCommand(class IEntity* _entity, const std::string& _filePath);
	~EntityDataInputCommand() override = default;
	
	/// @brief コマンドの実行
	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	/// ================================================
	/// private : objects
	/// ================================================
	class IEntity* pEntity_ = nullptr;
	std::string inputFilePath_;
	//nlohmann::json inputData_;
};