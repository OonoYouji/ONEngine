#pragma once

/// std
#include <typeindex>
#include <string>
#include <unordered_map>

/// engine
#include "../Interface/IEditorCommand.h"


/// ///////////////////////////////////////////////
/// エンティティのデータ出力コマンド
/// ///////////////////////////////////////////////
class EntityDataOutputCommand : public IEditorCommand {
public:
	EntityDataOutputCommand(class GameEntity* _entity);
	~EntityDataOutputCommand() override = default;

	/// @brief コマンドの実行
	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	class GameEntity* pEntity_ = nullptr;
	std::string outputFilePath_;
};

/// ///////////////////////////////////////////////
/// エンティティのデータ入力コマンド
/// ///////////////////////////////////////////////
class EntityDataInputCommand : public IEditorCommand {
public:
	EntityDataInputCommand() = default;
	EntityDataInputCommand(class GameEntity* _entity);
	~EntityDataInputCommand() override = default;
	
	/// @brief コマンドの実行
	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

	void SetEntity(GameEntity* _entity);

private:
	class GameEntity* pEntity_ = nullptr;
	std::string inputFilePath_;
};


/// ///////////////////////////////////////////////
/// Componentの追加
/// ///////////////////////////////////////////////
class AddComponentCommand : public IEditorCommand {
public:
	AddComponentCommand(class GameEntity* _entity, const std::string& _componentName);
	~AddComponentCommand() override = default;
	/// @brief コマンドの実行
	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	class GameEntity* pEntity_ = nullptr;
	std::string componentName_;
};


/// ///////////////////////////////////////////////
/// Componentの削除
/// ///////////////////////////////////////////////
class RemoveComponentCommand : public IEditorCommand {
public:
	RemoveComponentCommand(class GameEntity* _entity, const std::string& _componentName, std::unordered_map<size_t, class IComponent*>::iterator* _resultItr);
	~RemoveComponentCommand() override = default;

	/// @brief コマンドの実行
	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	std::unordered_map<size_t, class IComponent*>::iterator* pIterator_;
	class GameEntity* pEntity_ = nullptr;
	std::string componentName_;
};


/// ///////////////////////////////////////////////
/// Scriptの再読み込み
/// ///////////////////////////////////////////////
class ReloadAllScriptsCommand : public IEditorCommand {
public:
	ReloadAllScriptsCommand(class ECSGroup* _ecs, class SceneManager* _sceneManager);
	~ReloadAllScriptsCommand() override = default;
	/// @brief コマンドの実行
	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	/// ================================================
	/// private : objects
	/// ================================================
	class ECSGroup* pEcsGroup_ = nullptr;
	class SceneManager* pSceneManager_ = nullptr;
};