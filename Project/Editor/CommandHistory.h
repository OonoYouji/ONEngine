#pragma once
#include <memory>
#include <vector>
#include <stack>
#include <string>
#include "Engine/Common/Console.h"

namespace Engine::Editor {

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void Execute() = 0;
    virtual void Undo() = 0;
    virtual std::string GetName() const = 0;
};

class CommandHistory {
public:
    static CommandHistory& GetInstance() {
        static CommandHistory instance;
        return instance;
    }

    void Execute(std::shared_ptr<ICommand> command) {
        Engine::Console::Log(std::format("[Undo/Redo] Executing: {}", command->GetName()));
        command->Execute();
        undoStack_.push(command);
        
        while (!redoStack_.empty()) {
            redoStack_.pop();
        }

        if (undoStack_.size() > 100) {
            // 古い履歴の削除ロジック（省略）
        }
    }

    void Undo() {
        if (undoStack_.empty()) {
            Engine::Console::LogWarning("[Undo/Redo] Nothing to Undo");
            return;
        }

        auto command = undoStack_.top();
        undoStack_.pop();
        Engine::Console::Log(std::format("[Undo/Redo] Undoing: {}", command->GetName()));
        command->Undo();
        redoStack_.push(command);
    }

    void Redo() {
        if (redoStack_.empty()) {
            Engine::Console::LogWarning("[Undo/Redo] Nothing to Redo");
            return;
        }

        auto command = redoStack_.top();
        redoStack_.pop();
        Engine::Console::Log(std::format("[Undo/Redo] Redoing: {}", command->GetName()));
        command->Execute();
        undoStack_.push(command);
    }

    bool CanUndo() const { return !undoStack_.empty(); }
    bool CanRedo() const { return !redoStack_.empty(); }

    void Clear() {
        while (!undoStack_.empty()) undoStack_.pop();
        while (!redoStack_.empty()) redoStack_.pop();
    }

private:
    CommandHistory() = default;

    std::stack<std::shared_ptr<ICommand>> undoStack_;
    std::stack<std::shared_ptr<ICommand>> redoStack_;
};

} // namespace Engine::Editor
