#pragma once

/// std
#include <memory>

/// engine
#include "Engine/WindowManager/WindowManager.h"
#include "Engine/DirectX12/Manager/DxManager.h"
#include "IGameLoop/IGameLoop.h"
#include "GameFrameworkStartSetting.h"


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
	void Initialize(const GameFrameworkStartSetting& _startSetting);

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

	/// framework, scene manager, main loop etc...

	std::unique_ptr<DxManager>     dxManager_     = nullptr;
	std::unique_ptr<WindowManager> windowManager_ = nullptr;

	std::unique_ptr<IGameLoop>     gameLoop_;

};

