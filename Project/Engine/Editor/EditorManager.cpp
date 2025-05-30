#include "EditorManager.h"

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Editor/Commands/WorldEditorCommands/WorldEditorCommands.h"

class LogCommand : public IEditorCommand {
public:
	LogCommand() {}
	~LogCommand() {}

	EDITOR_STATE Execute() override {
		Console::Log("LogCommand");
		return EDITOR_STATE_FINISH;
	}

	EDITOR_STATE Undo() override {
		Console::Log("Undo LogCommand");
		return EDITOR_STATE_FINISH;
	}
};



EditorManager::EditorManager(EntityComponentSystem* _ecs)
	: pECS_(_ecs) {}
EditorManager::~EditorManager() {}

void EditorManager::Initialize() {
	runningCommand_ = nullptr;
}

void EditorManager::Update() {

	if (runningCommand_) {
		EDITOR_STATE state = runningCommand_->Execute();
		if (state != EDITOR_STATE_RUNNING) {
			runningCommand_ = nullptr;
		} else {
			Console::Log("editor command is running");
		}

	} else {
#ifdef _DEBUG
		// undo, redo を行う
		if (Input::PressKey(DIK_LCONTROL) && Input::TriggerKey(DIK_Z)) {
			Undo();
		}

		if (Input::PressKey(DIK_LCONTROL) && Input::TriggerKey(DIK_Y)) {
			Redo();
		}
#endif // _DEBUG
	}

}



void EditorManager::Undo() {
	if (commandStack_.empty()) {
		return;
	}
	std::unique_ptr<IEditorCommand> command = std::move(commandStack_.back());
	command->Undo();
	redoStack_.push_back(std::move(command));
	commandStack_.pop_back();
}

void EditorManager::Redo() {
	if (redoStack_.empty()) {
		return;
	}

	/// stackから実行する
	std::unique_ptr<IEditorCommand> command = std::move(redoStack_.front());
	command->Execute();
	redoStack_.pop_front();

	/// command stackに戻す
	commandStack_.push_back(std::move(command));

}
