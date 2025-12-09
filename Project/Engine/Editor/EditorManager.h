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

/// editor
#include "Engine/Core/Utility/Utility.h"

#include "Clipboard.h"
#include "Commands/Interface/IEditorCommand.h"
#include "EditorCompute/Interface/IEditorCompute.h"

/// @brief コマンドの生成関数
using Creator = std::function<std::unique_ptr<IEditorCommand>(const std::vector<std::any>&)>;

/// @brief 
template <typename T>
concept IsEditorCommand = std::is_base_of_v<IEditorCommand, T>;


/// /////////////////////////////////////////////////
/// エディタの管理クラス
/// /////////////////////////////////////////////////
class EditorManager final {
	friend class EditCommand;
public:
	/// =========================================
	/// public : methods
	/// =========================================

	EditorManager(class EntityComponentSystem* _ecs);
	~EditorManager();

	void Initialize(class DxManager* _dxm, class ShaderCompiler* _shaderCompiler);

	void Update(class AssetCollection* _assetCollection);


	/// ----- factory ----- ///

	template<IsEditorCommand T, typename... Args>
	std::unique_ptr<T> CloneCommand(Args&&... _args);


	/// ----- command ----- ///

	template<IsEditorCommand T, typename... Args>
	void ExecuteCommand(Args&& ..._args);

	void Undo();
	void Redo();

	/// ----- editor compute ----- ///

	void AddEditorCompute(class DxManager* _dxm, class ShaderCompiler* _shaderCompiler, std::unique_ptr<IEditorCompute> _compute);


private:
	/// ==========================================
	/// private : objects
	/// ==========================================

	/// ----- other class ----- ///
	class EntityComponentSystem* pEcs_;
	class DxManager* pDxManager_;

	/// ----- clipboard ----- ///
	Clipboard clipboard_;

	/// ----- container -----///
	std::unordered_map<std::string, std::unique_ptr<IEditorCommand>> prototypeCommands_; ///< コマンドのコレクション

	IEditorCommand* runningCommand_; ///< 現在実行中のコマンド
	std::deque<std::unique_ptr<IEditorCommand>> commandStack_; ///< コマンドのスタック
	std::deque<std::unique_ptr<IEditorCommand>> redoStack_; ///< コマンドのリドゥスタック


	/// ----- editor compute ----- ///
	std::vector<std::unique_ptr<IEditorCompute>> editorComputes_;


	/// ----- temp object ----- ///
	std::string className_;
};


template<IsEditorCommand T, typename ...Args>
inline std::unique_ptr<T> EditorManager::CloneCommand(Args&&... _args) {
	className_ = typeid(T).name();
	auto it = prototypeCommands_.find(className_);
	if (it == prototypeCommands_.end()) {
		prototypeCommands_[className_] = std::make_unique<T>(_args...);
	}

	// コピーコンストラクタで T のインスタンスを複製
	const T* prototype = static_cast<T*>(prototypeCommands_.at(className_).get());
	return std::make_unique<T>(*prototype);
}

template<IsEditorCommand T, typename ...Args>
inline void EditorManager::ExecuteCommand(Args && ..._args) {
	std::unique_ptr<T> command = std::make_unique<T>(_args...);
	EDITOR_STATE state = command->Execute();
	if (state == EDITOR_STATE_RUNNING) {
		runningCommand_ = command.get();
	}

	commandStack_.push_back(std::move(command));
	if (state == EDITOR_STATE_FINISH) {
		Console::Log("Command Executed: " + std::string(typeid(T).name()));
	} else {
		Console::Log("Command Failed: " + std::string(typeid(T).name()));
	}

	/// redoスタックにコマンドがあればクリアする
	if (redoStack_.size() > 0) {
		redoStack_.clear();
	}
}
