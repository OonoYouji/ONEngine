#pragma once

/// std
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <deque>

/// engine
#include "../Interface/IEditorCommand.h"

/// ///////////////////////////////////////////////////
/// エディタのコマンドをまとめるクラス
/// ///////////////////////////////////////////////////
class EditorCommandCollection {
public:
	/// =========================================
	/// public : methods
	/// =========================================

	EditorCommandCollection();
	~EditorCommandCollection();

	void Initialize();


private:
	/// =========================================
	/// private : objects
	/// =========================================

	std::unordered_map<size_t, std::string> refIndexMap_; /// commands
	std::vector<std::unique_ptr<IEditorCommand>> commands_;

};



/// ///////////////////////////////////////////////////
/// コマンドのスタックを管理するクラス
/// ///////////////////////////////////////////////////
class CommandStacker {
public:
	/// =========================================
	/// public : methods
	/// =========================================

	CommandStacker();
	~CommandStacker();

	void PushCommand(IEditorCommand* _command);

	void Undo();
	void Redo();

private:
	/// =========================================
	/// private : objects
	/// =========================================

	std::vector<IEditorCommand*> commandStack_;
	std::deque<IEditorCommand*> redoStack_;


};