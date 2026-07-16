#pragma once

#include "EditorPlayState.h"

namespace Editor {

/**
 * @class EditModeState
 * @brief 編集モード（停止中）のステートクラス
 */
class EditModeState : public IEditorPlayState {
public:
    void Enter(EditorPlayStateManager* manager, ONEngine::SceneManager* sceneManager) override;
    void Update(EditorPlayStateManager* manager, ONEngine::SceneManager* sceneManager) override;
    void Exit(EditorPlayStateManager* manager, ONEngine::SceneManager* sceneManager) override;
    PlayStateType GetStateType() const override { return PlayStateType::Edit; }
};

/**
 * @class PlayModeState
 * @brief ゲーム実行モード（再生中）のステートクラス
 */
class PlayModeState : public IEditorPlayState {
public:
    void Enter(EditorPlayStateManager* manager, ONEngine::SceneManager* sceneManager) override;
    void Update(EditorPlayStateManager* manager, ONEngine::SceneManager* sceneManager) override;
    void Exit(EditorPlayStateManager* manager, ONEngine::SceneManager* sceneManager) override;
    PlayStateType GetStateType() const override { return PlayStateType::Play; }
};

/**
 * @class PauseModeState
 * @brief ゲーム一時停止モードのステートクラス
 */
class PauseModeState : public IEditorPlayState {
public:
    void Enter(EditorPlayStateManager* manager, ONEngine::SceneManager* sceneManager) override;
    void Update(EditorPlayStateManager* manager, ONEngine::SceneManager* sceneManager) override;
    void Exit(EditorPlayStateManager* manager, ONEngine::SceneManager* sceneManager) override;
    PlayStateType GetStateType() const override { return PlayStateType::Pause; }
};

} /// namespace Editor
