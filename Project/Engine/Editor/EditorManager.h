#pragma once

/// std
#include <memory>
#include <type_traits>
#include <vector>
#include <deque>
#include <unordered_map>
#include <string>
#include <functional>
#include <any>
#include <type_traits>

/// editor
#include "Commands/Interface/IEditorCommand.h"

/// @brief コマンドの生成関数
using Creator = std::function<std::unique_ptr<IEditorCommand>(const std::vector<std::any>&)>;

/// @brief 
template <typename T>
concept IsEditorCommand = std::is_base_of_v<IEditorCommand, T>;

//template <typename... Args>
//using Creator = std::function<std::unique_ptr<IEditorCommand>(Args...)>;


/// /////////////////////////////////////////////////
/// エディタの管理クラス
/// /////////////////////////////////////////////////
class EditorManager final {
public:
	/// =========================================
	/// public : methods
	/// =========================================

	EditorManager(class EntityComponentSystem* _ecs);
	~EditorManager();

	void Initialize();


	/* ----- factory ----- */

	//template<typename T>
	//void RegisterCommand() requires IsEditorCommand<T>;

	//template<typename T, typename... Args>
	//std::unique_ptr<IEditorCommand> CreateCommand(Args&& ...args) requires IsEditorCommand<T>;

	template<typename T, typename... Args>
	std::unique_ptr<T> CloneCommand(Args&&... _args) requires IsEditorCommand<T>;


	/* ----- command ----- */

	template<typename T, typename... Args>
	void ExecuteCommand(Args&& ..._args) requires IsEditorCommand<T>;

	void Undo();
	void Redo();

private:
	/// ==========================================
	/// private : objects
	/// ==========================================

	/* ----- other class ----- */
	class EntityComponentSystem* pECS_ = nullptr; ///< エンティティコンポーネントシステムへのポインタ

	/* ----- factory ----- */
	//std::unordered_map<std::string, Creator> commandFactoryMap_; ///< コマンドのファクトリマップ


	/* ----- container ----- */

	std::unordered_map<std::string, std::unique_ptr<IEditorCommand>> prototypeCommands_; ///< コマンドのコレクション

	std::deque<std::unique_ptr<IEditorCommand>> commandStack_; ///< コマンドのスタック
	std::deque<std::unique_ptr<IEditorCommand>> redoStack_; ///< コマンドのリドゥスタック


	/* ----- temp object ----- */
	std::string className_;
};



//
//template<typename T>
//inline void EditorManager::RegisterCommand() requires IsEditorCommand<T> {
//	className_ = typeid(T).name();
//	commandFactoryMap_[className_] = [](auto&& ...args) -> std::unique_ptr<IEditorCommand> {
//		return std::make_unique<T>(std::forward<decltype(args)>(args)...);
//		};
//
//}
//
//template<typename T, typename... Args>
//inline std::unique_ptr<IEditorCommand> EditorManager::CreateCommand(Args&& ..._args) requires IsEditorCommand<T> {
//	className_ = typeid(T).name();
//	return commandFactoryMap_[className_](_args);
//}

template<typename T, typename ...Args>
inline std::unique_ptr<T> EditorManager::CloneCommand(Args&&... _args) requires IsEditorCommand<T> {
	className_ = typeid(T).name();
	auto it = prototypeCommands_.find(className_);
	if (it == prototypeCommands_.end()) {
		prototypeCommands_[className_] = std::make_unique<T>(_args...);
	}

	// コピーコンストラクタで T のインスタンスを複製
	const T* prototype = static_cast<T*>(prototypeCommands_.at(className_).get());
	return std::make_unique<T>(*prototype);
}

template<typename T, typename ...Args>
inline void EditorManager::ExecuteCommand(Args && ..._args) requires IsEditorCommand<T> {
	auto command = CloneCommand<T>(_args...);
	command->Execute();
	commandStack_.push_back(std::move(command));
}
