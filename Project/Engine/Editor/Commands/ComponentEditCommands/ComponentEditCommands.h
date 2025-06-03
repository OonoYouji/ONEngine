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
	
	EntityDataInputCommand() = default;
	EntityDataInputCommand(class IEntity* _entity);
	~EntityDataInputCommand() override = default;
	
	/// @brief コマンドの実行
	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

	void SetEntity(IEntity* _entity);

private:
	/// ================================================
	/// private : objects
	/// ================================================
	class IEntity* pEntity_ = nullptr;
	std::string inputFilePath_;
	//nlohmann::json inputData_;
};


/// ///////////////////////////////////////////////
/// Componentの追加
/// ///////////////////////////////////////////////
class AddComponentCommand : public IEditorCommand {
public:
	/// ================================================
	/// public : methods
	/// ================================================
	
	AddComponentCommand(class IEntity* _entity, const std::string& _componentName);
	~AddComponentCommand() override = default;
	/// @brief コマンドの実行
	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	/// ================================================
	/// private : objects
	/// ================================================

	class IEntity* pEntity_ = nullptr;
	std::string componentName_;
	//std::type_index componentTypeIndex_; ///< コンポーネントの型情報

};
