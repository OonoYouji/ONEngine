#pragma once

/// std
#include <memory>

/// engine
#include "Engine/Window/WindowManager.h"
#include "Engine/DirectX12/Manager/DxManager.h"
#include "Engine/Scene/SceneManager.h"
#include "GameFrameworkConfig.h"

#include "Engine/Graphics/Framework/RenderingFramework.h"
#include "Engine/Graphics/Renderer/Primitive/Line2DRenderer.h"


/// ===================================================
/// game framework class
/// ===================================================
class GameFramework final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	GameFramework();
	~GameFramework();
	
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(const GameFrameworkConfig& _startSetting);

	/// <summary>
	/// ゲームのメインループを回す
	/// </summary>
	void Run();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<DxManager>     dxManager_     = nullptr;
	std::unique_ptr<WindowManager> windowManager_ = nullptr;
	std::unique_ptr<SceneManager>  sceneManager_  = nullptr;

	std::unique_ptr<RenderingFramework> renderingFramework_;


public:

	DxManager* GetDxManager() const { return dxManager_.get(); }

};

