#include "GameFramework.h"

GameFramework::GameFramework() {}
GameFramework::~GameFramework() {
	/// gpuの処理が終わるまで待つ
	dxManager_->GetDxCommand()->WaitForGpuComplete();

	/// engineの終了処理
	windowManager_->Finalize();
	dxManager_->Finalize();
}

void GameFramework::Initialize(const GameFrameworkConfig& _startSetting) {

	/// directX12の初期化
	dxManager_ = std::make_unique<DxManager>();
	dxManager_->Initialize();
	

	/// windowの初期化
	windowManager_ = std::make_unique<WindowManager>(dxManager_.get());
	windowManager_->Initialize();
	
	/// main windowの生成
	windowManager_->GenerateWindow(_startSetting.windowName, _startSetting.windowSize, WindowManager::WindowType::Main);

	/// entity collectionの初期化
	entityCollection_ = std::make_unique<EntityCollection>(dxManager_.get());
	entityCollection_->Initialize();
	
	/// scene managerの初期化
	sceneManager_ = std::make_unique<SceneManager>(entityCollection_.get());
	sceneManager_->Initialize();
	
	renderingFramework_ = std::make_unique<RenderingFramework>();
	renderingFramework_->Initialize(dxManager_.get(), windowManager_.get());
	renderingFramework_->GenerateRenderer<Line2DRenderer>();

}

void GameFramework::Run() {

	/// game loopが終了するまで回す
	while(true) {

		/// 更新処理
		windowManager_->Update();
		sceneManager_->Update();
		entityCollection_->Update();

		/// 描画処理
		entityCollection_->Draw();
		renderingFramework_->Draw();

		/// 破棄されたら終了
		if(windowManager_->GetMainWindow()->GetProcessMessage()) {
			break;
		}
	}

}

