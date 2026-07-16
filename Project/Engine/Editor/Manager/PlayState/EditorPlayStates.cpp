#include "EditorPlayState.h"
#include "EditorPlayStates.h"

/// engine
#include "Engine/Scene/SceneManager.h"
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Editor/Math/ImGuiSelection.h"
#include "Engine/Script/MonoScriptEngine.h"

namespace Editor {

/// ========================================================
/// EditorPlayStateManager Implementation
/// ========================================================

EditorPlayStateManager& EditorPlayStateManager::GetInstance() {
    static EditorPlayStateManager instance;
    return instance;
}

void EditorPlayStateManager::Initialize(ONEngine::SceneManager* sceneManager) {
    pSceneManager_ = sceneManager;
    // 初期状態を Edit 状態に設定
    currentState_ = std::make_unique<EditModeState>();
    currentState_->Enter(this, pSceneManager_);
}

void EditorPlayStateManager::Update() {
    if (currentState_) {
        currentState_->Update(this, pSceneManager_);
    }
}

void EditorPlayStateManager::ChangeState(std::unique_ptr<IEditorPlayState> newState) {
    if (currentState_) {
        currentState_->Exit(this, pSceneManager_);
    }
    currentState_ = std::move(newState);
    if (currentState_) {
        currentState_->Enter(this, pSceneManager_);
    }
}

PlayStateType EditorPlayStateManager::GetCurrentStateType() const {
    return currentState_ ? currentState_->GetStateType() : PlayStateType::Edit;
}

bool EditorPlayStateManager::IsPlaying() const {
    return GetCurrentStateType() == PlayStateType::Play;
}


/// ========================================================
/// EditModeState Implementation
/// ========================================================

void EditModeState::Enter(EditorPlayStateManager* /*manager*/, ONEngine::SceneManager* sceneManager) {
    ONEngine::DebugConfig::isDebugging = false;
    if (sceneManager) {
        sceneManager->ReloadScene(true);
    }
    ImGuiSelection::SetSelectedObject(ONEngine::Guid::kInvalid, SelectionType::None);
    ONEngine::Console::Log("[EditorState] Entered Edit Mode.");
}

void EditModeState::Update(EditorPlayStateManager* /*manager*/, ONEngine::SceneManager* /*sceneManager*/) {}

void EditModeState::Exit(EditorPlayStateManager* /*manager*/, ONEngine::SceneManager* /*sceneManager*/) {}


/// ========================================================
/// PlayModeState Implementation
/// ========================================================

void PlayModeState::Enter(EditorPlayStateManager* /*manager*/, ONEngine::SceneManager* sceneManager) {
    ONEngine::DebugConfig::isDebugging = true;
    ONEngine::Console::ClearLogBuffer(ONEngine::LogCategory::Application);
    
    if (sceneManager) {
        sceneManager->SaveCurrentSceneTemporary();
    }

    // シーンロード前にC#スクリプトエンジンの状態を最新にする
    ONEngine::MonoScriptEngine::GetInstance().HotReload();

    if (sceneManager) {
        sceneManager->ReloadScene(true);
    }

    ImGuiSelection::SetSelectedObject(ONEngine::Guid::kInvalid, SelectionType::None);
    ONEngine::Console::Log("[EditorState] Entered Play Mode.");
}

void PlayModeState::Update(EditorPlayStateManager* /*manager*/, ONEngine::SceneManager* /*sceneManager*/) {}

void PlayModeState::Exit(EditorPlayStateManager* /*manager*/, ONEngine::SceneManager* /*sceneManager*/) {}


/// ========================================================
/// PauseModeState Implementation
/// ========================================================

void PauseModeState::Enter(EditorPlayStateManager* /*manager*/, ONEngine::SceneManager* /*sceneManager*/) {
    ONEngine::DebugConfig::isDebugging = false; // 一時停止中は更新を遮断するために isDebugging = false に同期する
    ONEngine::Console::Log("[EditorState] Entered Pause Mode.");
}

void PauseModeState::Update(EditorPlayStateManager* /*manager*/, ONEngine::SceneManager* /*sceneManager*/) {}

void PauseModeState::Exit(EditorPlayStateManager* /*manager*/, ONEngine::SceneManager* /*sceneManager*/) {}

} /// namespace Editor
