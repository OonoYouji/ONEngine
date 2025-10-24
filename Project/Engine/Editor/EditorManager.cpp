#include "EditorManager.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Utility.h"

#include "EditCommand.h"
#include "Commands/WorldEditorCommands/WorldEditorCommands.h"

/// editor compute
#include "EditorCompute/TerrainEditor/TerrainDataOutput.h"
#include "EditorCompute/TerrainEditor/TerrainVertexCreator.h"
#include "EditorCompute/TerrainEditor/TerrainVertexEditorCompute.h"
#include "EditorCompute/TerrainEditor/RiverTerrainAbjustPipeline.h"
#include "EditorCompute/River/RiverMeshGeneratePipeline.h"
#include "EditorCompute/Grass/GrassArrangementPipeline.h"

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
	: pEcs_(_ecs) {}
EditorManager::~EditorManager() = default;

void EditorManager::Initialize(DxManager* _dxm, ShaderCompiler* _sc) {
	pDxManager_ = _dxm;
	runningCommand_ = nullptr;

	/// EditCommandへEditorManagerのポインタを渡す
	EditCommand::pEditorManager_ = this;

	/// editor compute の登録
	AddEditorCompute(_dxm, _sc, std::make_unique<TerrainDataOutput>());
	AddEditorCompute(_dxm, _sc, std::make_unique<TerrainVertexCreator>());
	AddEditorCompute(_dxm, _sc, std::make_unique<TerrainVertexEditorCompute>());
	AddEditorCompute(_dxm, _sc, std::make_unique<RiverMeshGeneratePipeline>());
	AddEditorCompute(_dxm, _sc, std::make_unique<RiverTerrainAbjustPipeline>());
	AddEditorCompute(_dxm, _sc, std::make_unique<GrassArrangementPipeline>());
}

void EditorManager::Update(AssetCollection* _grc) {

	/// エディタのコマンドを実行する
	for (auto& compute : editorComputes_) {
		compute->Execute(pEcs_, pDxManager_->GetDxCommand(), _grc);
	}

	if (runningCommand_) {
		EDITOR_STATE state = runningCommand_->Execute();
		if (state != EDITOR_STATE_RUNNING) {
			runningCommand_ = nullptr;
		} else {
			Console::Log("editor command is running");
		}

	} else {
#ifdef DEBUG_MODE
		// undo, redo を行う
		if (Input::PressKey(DIK_LCONTROL) && Input::TriggerKey(DIK_Z)) {
			Undo();
		}

		if (Input::PressKey(DIK_LCONTROL) && Input::TriggerKey(DIK_Y)) {
			Redo();
		}
#endif // DEBUG_MODE
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

void EditorManager::AddEditorCompute(DxManager* _dxm, ShaderCompiler* _shaderCompiler, std::unique_ptr<IEditorCompute> _compute) {
	_compute->Initialize(_shaderCompiler, _dxm);

	editorComputes_.push_back(std::move(_compute));
}
