#pragma once

/// std
#include <memory>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Window/WindowManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Graphics/Framework/RenderingFramework.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Script/MonoScriptEngine.h"
#include "GameFrameworkConfig.h"

/// editor
#include "Engine/Editor/Manager/EditorManager.h"
#include "Engine/Editor/Manager/ImGuiManager.h"

/// ///////////////////////////////////////////////////
/// game framework class
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class GameFramework
 * @brief ゲームエンジン全体の初期化、メインループ実行、各基盤モジュール（DirectX, Window, ECS, Script, Rendering, Editor）の制御を行う中心クラス
 */
class GameFramework final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	GameFramework();

	/**
	 * @brief デストラクタ。各種基盤モジュールの終了処理を行います。
	 */
	~GameFramework();

	/**
	 * @brief エンジン全体および各モジュールの初期化を実行します。
	 * @param _startSetting 開始時のウィンドウサイズやタイトル等の設定パラメータ
	 */
	void Initialize(const GameFrameworkConfig& _startSetting);

	/**
	 * @brief ゲームのメインループを実行します。ウィンドウが閉じられるまで処理を繰り返します。
	 */
	void Run();

	/**
	 * @brief デバッグ用の起動時シーン（JSON定義）をロードします。
	 */
	void LoadDebugScene();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<DxManager> dxManager_;                         ///< DirectX12コアシステム管理
	std::unique_ptr<WindowManager> windowManager_;                 ///< ウィンドウシステム管理
	std::unique_ptr<SceneManager> sceneManager_;                   ///< シーン遷移管理
	std::unique_ptr<EntityComponentSystem> entityComponentSystem_; ///< ECSによるゲームオブジェクト・システム管理
	std::unique_ptr<RenderingFramework> renderingFramework_;       ///< 描画パイプライン管理

	std::unique_ptr<Editor::ImGuiManager> imGuiManager_;           ///< ImGui表示管理
	std::unique_ptr<Editor::EditorManager> editorManager_;         ///< エディタウィンドウ群管理
};

} // namespace ONEngine
