#include "EditorCommandCollection.h"

/// ///////////////////////////////////////////////////
/// コマンドのコレクション
/// ///////////////////////////////////////////////////

EditorCommandCollection::EditorCommandCollection() {}
EditorCommandCollection::~EditorCommandCollection() {}

void EditorCommandCollection::Initialize() {}


/// ///////////////////////////////////////////////////
/// コマンドのスタッカー
/// ///////////////////////////////////////////////////

CommandStacker::CommandStacker() {}
CommandStacker::~CommandStacker() {}

void CommandStacker::PushCommand(IEditorCommand* _command) {
	commandStack_.push_back(_command);
}

void CommandStacker::Undo() {
	if (commandStack_.empty()) {
		return;
	}

	IEditorCommand* command = commandStack_.back();
	redoStack_.push_back(command);
	command->Undo();
	commandStack_.pop_back();
}

void CommandStacker::Redo() {
	if (redoStack_.empty()) {
		return;
	}

	IEditorCommand* command = redoStack_.front();
	command->Execute();
	redoStack_.pop_front();
}
