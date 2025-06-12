#pragma once

/// std
#include <memory>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Window/WindowManager.h"
#include "Engine/Core/ImGui/ImGuiManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Graphics/Framework/RenderingFramework.h"
#include "Engine/Editor/EditorManager.h"
#include "Engine/Script/MonoScriptEngine.h"
#include "GameFrameworkConfig.h"



/// ///////////////////////////////////////////////////
/// game framework class
/// ///////////////////////////////////////////////////
class GameFramework final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	GameFramework();
	~GameFramework();
	
	/// @brief 初期化処理
	/// @param _startSetting 開始時の設定
	void Initialize(const GameFrameworkConfig& _startSetting);

	/// @brief ゲームのメインループ
	void Run();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<DxManager>             dxManager_             = nullptr;
	std::unique_ptr<WindowManager>         windowManager_         = nullptr;
	std::unique_ptr<SceneManager>          sceneManager_          = nullptr;
	std::unique_ptr<EntityComponentSystem> entityComponentSystem_ = nullptr;

	std::unique_ptr<ImGuiManager>          imGuiManager_     = nullptr;
	std::unique_ptr<EditorManager>         editorManager_ = nullptr;

	std::unique_ptr<RenderingFramework>    renderingFramework_;

	std::unique_ptr<MonoScriptEngine>      monoScriptEngine_ = nullptr;

};

