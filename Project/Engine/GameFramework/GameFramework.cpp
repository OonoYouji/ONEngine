#include "GameFramework.h"

GameFramework::GameFramework() {}
GameFramework::~GameFramework() {}

void GameFramework::Initialize(const GameFrameworkConfig& _startSetting) {

	/// directX12の初期化
	dxManager_     = std::make_unique<DxManager>();
	dxManager_->Initialize();
	

	/// windowの初期化
	windowManager_ = std::make_unique<WindowManager>(dxManager_.get());
	windowManager_->Initialize();

	/// main windowの生成
	windowManager_->GenerateWindow(_startSetting.windowName, _startSetting.windowSize, WindowManager::WindowType::Main);


	/// game loopの初期化
	gameLoop_.reset(_startSetting.gameLoop);
	gameLoop_->Initialize();

}

void GameFramework::Run() {

	/// game loopが終了するまで回す
	while(!gameLoop_->GetIsEnd()) {

		/// 更新処理
		windowManager_->Update();
		gameLoop_->Update();


		/// 描画処理
		dxManager_->GetDxDescriptorHeap(DescriptorHeapType_CBV_SRV_UAV)->BindToCommandList(
			dxManager_->GetDxCommand()->GetCommandList()
		);

		windowManager_->PreDraw();
		gameLoop_->Draw();
		windowManager_->PostDraw();


		/// commandの実行
		dxManager_->GetDxCommand()->CommandExecute();
		windowManager_->Present();
		dxManager_->GetDxCommand()->CommandReset();


		/// 破棄されたら終了
		if(windowManager_->GetMainWindow()->GetProcessMessage()) {
			break;
		}
	}

}

void GameFramework::Finalize() {

	/// gpuの処理が終わるまで待つ
	dxManager_->GetDxCommand()->WaitForGpuComplete();

	/// main loopの終了処理
	gameLoop_->Finalize();

	/// engineの終了処理
	windowManager_->Finalize();
	dxManager_->Finalize();

}
