#pragma once

/// std
#include <typeindex>
#include <string>
#include <unordered_map>

/// engine
#include "../IEditCommand.h"


namespace ONEngine {
/// 前方宣言
class GameEntity;
class ECSGroup;
class SceneManager;
class IComponent;
}



/// ///////////////////////////////////////////////
/// エンティティのデータ出力コマンド
/// ///////////////////////////////////////////////
namespace Editor {

class EntityDataOutputCommand : public IEditCommand {
public:
	EntityDataOutputCommand(ONEngine::GameEntity* _entity);
	~EntityDataOutputCommand() override = default;

	/// @brief コマンドの実行
	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	ONEngine::GameEntity* pEntity_ = nullptr;
	std::string outputFilePath_;
};

/// ///////////////////////////////////////////////
/// エンティティのデータ入力コマンド
/// ///////////////////////////////////////////////
class EntityDataInputCommand : public IEditCommand {
public:
	EntityDataInputCommand() = default;
	EntityDataInputCommand(ONEngine::GameEntity* _entity);
	~EntityDataInputCommand() override = default;

	/// @brief コマンドの実行
	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

	void SetEntity(ONEngine::GameEntity* _entity);

private:
	ONEngine::GameEntity* pEntity_ = nullptr;
	std::string inputFilePath_;
};


/// ///////////////////////////////////////////////
/// Componentの追加
/// ///////////////////////////////////////////////
class AddComponentCommand : public IEditCommand {
public:
	AddComponentCommand(ONEngine::GameEntity* _entity, const std::string& _componentName);
	~AddComponentCommand() override = default;
	/// @brief コマンドの実行
	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	ONEngine::GameEntity* pEntity_ = nullptr;
	std::string componentName_;
};


/// ///////////////////////////////////////////////
/// Componentの削除
/// ///////////////////////////////////////////////
class RemoveComponentCommand : public IEditCommand {
public:
	RemoveComponentCommand(ONEngine::GameEntity* _entity, const std::string& _componentName, std::unordered_map<size_t, ONEngine::IComponent*>::iterator* _resultItr);
	~RemoveComponentCommand() override = default;

	/// @brief コマンドの実行
	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	std::unordered_map<size_t, ONEngine::IComponent*>::iterator* pIterator_;
	ONEngine::GameEntity* pEntity_ = nullptr;
	std::string componentName_;
};


/// ///////////////////////////////////////////////
/// Scriptの再読み込み
/// ///////////////////////////////////////////////
class ReloadAllScriptsCommand : public IEditCommand {
public:
	ReloadAllScriptsCommand(ONEngine::ECSGroup* _ecs, ONEngine::SceneManager* _sceneManager);
	~ReloadAllScriptsCommand() override = default;
	/// @brief コマンドの実行
	EDITOR_STATE Execute() override;
	EDITOR_STATE Undo() override;

private:
	/// ================================================
	/// private : objects
	/// ================================================
	ONEngine::ECSGroup* pEcsGroup_ = nullptr;
	ONEngine::SceneManager* pSceneManager_ = nullptr;
};


} /// Editor