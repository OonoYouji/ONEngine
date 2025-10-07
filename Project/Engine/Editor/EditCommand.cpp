#include "EditCommand.h"

EditCommand::EditCommand() = default;
EditCommand::~EditCommand() = default;

/// 変数の初期化
EditorManager* EditCommand::pEditorManager_ = nullptr;

void EditCommand::Redo() {
	pEditorManager_->Redo();
}

void EditCommand::Undo() {
	pEditorManager_->Undo();
}