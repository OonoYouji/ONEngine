#pragma once

/// std
#include <memory>

namespace ONEngine {
class SceneManager;
}

namespace Editor {

/**
 * @enum PlayStateType
 * @brief エディタの動作モード（編集、実行、一時停止）を定義する列挙型
 */
enum class PlayStateType {
    Edit,  ///< 編集モード（停止中）
    Play,  ///< ゲーム実行モード（アクティブ）
    Pause  ///< ゲーム一時停止モード（時間は停止、現在のシーン状態を維持）
};

class EditorPlayStateManager;

/**
 * @class IEditorPlayState
 * @brief エディタの動作モードごとの振る舞いをカプセル化するためのステートパターンインターフェース
 */
class IEditorPlayState {
public:
    /**
     * @brief デストラクタ
     */
    virtual ~IEditorPlayState() = default;

    /**
     * @brief この状態に入った際の初期化処理を行います。
     * @param manager 状態管理を行うコンテキストへのポインタ
     * @param sceneManager シーン管理システムへのポインタ
     */
    virtual void Enter(EditorPlayStateManager* manager, ONEngine::SceneManager* sceneManager) = 0;

    /**
     * @brief この状態における毎フレームの更新処理を行います。
     * @param manager 状態管理を行うコンテキストへのポインタ
     * @param sceneManager シーン管理システムへのポインタ
     */
    virtual void Update(EditorPlayStateManager* manager, ONEngine::SceneManager* sceneManager) = 0;

    /**
     * @brief この状態から抜ける際のクリーンアップ処理を行います。
     * @param manager 状態管理を行うコンテキストへのポインタ
     * @param sceneManager シーン管理システムへのポインタ
     */
    virtual void Exit(EditorPlayStateManager* manager, ONEngine::SceneManager* sceneManager) = 0;

    /**
     * @brief 現在の状態の種類を取得します。
     * @return プレイ状態の列挙型値
     */
    virtual PlayStateType GetStateType() const = 0;
};


/**
 * @class EditorPlayStateManager
 * @brief エディタの動作モードの遷移と更新呼び出しを管理するコンテキストクラス（シングルトン）
 */
class EditorPlayStateManager {
public:
    /**
     * @brief シングルトンインスタンスを取得します。
     * @return マネージャーの参照
     */
    static EditorPlayStateManager& GetInstance();

    /**
     * @brief 初期化処理を行います。デフォルトで編集モードに設定します。
     * @param sceneManager 使用するシーンマネージャーへのポインタ
     */
    void Initialize(ONEngine::SceneManager* sceneManager);

    /**
     * @brief 現在のステートのUpdate処理を毎フレーム実行します。
     */
    void Update();

    /**
     * @brief 新しいステートへ遷移させます。旧ステートのExitと新ステートのEnterを実行します。
     * @param newState 遷移先の新しいステートインスタンス
     */
    void ChangeState(std::unique_ptr<IEditorPlayState> newState);

    /**
     * @brief 現在のアクティブなステートの種類を取得します。
     * @return 現在の状態の種類
     */
    PlayStateType GetCurrentStateType() const;

    /**
     * @brief ゲームが実行（プレイ）中であるかどうかを判定します。
     * @return 実行中の場合はtrue、そうでない（Edit/Pause）場合はfalse
     */
    bool IsPlaying() const;

private:
    EditorPlayStateManager() = default;
    ~EditorPlayStateManager() = default;
    EditorPlayStateManager(const EditorPlayStateManager&) = delete;
    EditorPlayStateManager& operator=(const EditorPlayStateManager&) = delete;

    ONEngine::SceneManager* pSceneManager_ = nullptr;
    std::unique_ptr<IEditorPlayState> currentState_;
};

} /// namespace Editor
