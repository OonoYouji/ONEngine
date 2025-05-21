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
	//RegisterCommand<CreateGameObjectCommand>();

	//std::unique_ptr<CreateGameObjectCommand> command = CloneCommand<CreateGameObjectCommand>(pECS_);
	//ExecuteCommand<LogCommand>();
	//Undo();
	//Redo();

	//CreateCommand<CreateGameObjectCommand>(pECS_);
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
	std::unique_ptr<IEditorCommand> command = std::move(redoStack_.front());
	command->Execute();
	redoStack_.pop_front();
}
