#include "GameFramework.h"

/// engine
#include "Engine/Core/Utility/Input/Input.h"


GameFramework::GameFramework() {}
GameFramework::~GameFramework() {
	/// gpuの処理が終わるまで待つ
	dxManager_->GetDxCommand()->WaitForGpuComplete();

	Input::Finalize();

	/// engineの終了処理
	windowManager_->Finalize();
	dxManager_->Finalize();
}

void GameFramework::Initialize(const GameFrameworkConfig& _startSetting) {

	/// 各クラスのインスタンスを生成する
	dxManager_          = std::make_unique<DxManager>();
	windowManager_      = std::make_unique<WindowManager>(dxManager_.get());
	renderingFramework_ = std::make_unique<RenderingFramework>();
	entityCollection_   = std::make_unique<EntityCollection>(dxManager_.get());
	sceneManager_       = std::make_unique<SceneManager>(entityCollection_.get());

#ifdef _DEBUG
	imGuiManager_       = std::make_unique<ImGuiManager>(dxManager_.get(), windowManager_.get());
#endif // _DEBUG


	/// 各クラスの初期化を行う

	/// directX12の初期化
	dxManager_->Initialize();
	/// windowの初期化
	windowManager_->Initialize();
	/// main windowの生成
	windowManager_->GenerateWindow(_startSetting.windowName, _startSetting.windowSize, WindowManager::WindowType::Main);
	/// input systemの初期化
	Input::Initialize(windowManager_.get());
	/// rendering frameworkの初期化
	renderingFramework_->Initialize(dxManager_.get(), windowManager_.get(), entityCollection_.get());
	/// entity collectionの初期化
	entityCollection_->Initialize();
	/// scene managerの初期化
	sceneManager_->Initialize(renderingFramework_->GetResourceCollection());


#ifdef _DEBUG
	imGuiManager_->Initialize();
	renderingFramework_->SetImGuiManager(imGuiManager_.get());
#endif // _DEBUG
}

void GameFramework::Run() {

	/// game loopが終了するまで回す
	while(true) {

		/// 更新処理
		Input::Update();

		windowManager_->Update();
#ifdef _DEBUG
		imGuiManager_->Update();
#endif // _DEBUG

		sceneManager_->Update();
		entityCollection_->Update();


		/// 描画処理
		renderingFramework_->Draw();

		/// 破棄されたら終了
		if(windowManager_->GetMainWindow()->GetProcessMessage()) {
			break;
		}
	}

}

