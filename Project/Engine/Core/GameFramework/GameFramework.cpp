#include "GameFramework.h"

/// engine
#include "Engine/Core/Utility/Input/Input.h"
#include "Engine/Core/Utility/Time/Time.h"


GameFramework::GameFramework() {}
GameFramework::~GameFramework() {
	/// gpuの処理が終わるまで待つ
	dxManager_->GetDxCommand()->WaitForGpuComplete();

	Time::Finalize();
	Input::Finalize();

	/// engineの終了処理
	windowManager_->Finalize();
	dxManager_->Finalize();
}

void GameFramework::Initialize(const GameFrameworkConfig& _startSetting) {

	/// 各クラスのインスタンスを生成する
	dxManager_ = std::make_unique<DxManager>();
	windowManager_ = std::make_unique<WindowManager>(dxManager_.get());
	renderingFramework_ = std::make_unique<RenderingFramework>();
	entityComponentSystem_ = std::make_unique<EntityComponentSystem>(dxManager_.get());
	sceneManager_ = std::make_unique<SceneManager>(entityComponentSystem_.get());

#ifdef _DEBUG
	imGuiManager_ = std::make_unique<ImGuiManager>(dxManager_.get(), windowManager_.get(), entityComponentSystem_.get());
#endif // _DEBUG


	/// 各クラスの初期化を行う

	/// directX12の初期化
	dxManager_->Initialize();
	/// windowの初期化
	windowManager_->Initialize();
	/// main windowの生成
#ifdef _DEBUG
	windowManager_->GenerateWindow(_startSetting.windowName + L" : debug mode", Vector2(1280.0f, 720.0f), WindowManager::WindowType::Main);
#else
	windowManager_->GenerateWindow(_startSetting.windowName, _startSetting.windowSize, WindowManager::WindowType::Main);
#endif // _DEBUG


	/// input systemの初期化
	Input::Initialize(windowManager_.get());
	/// rendering frameworkの初期化
	renderingFramework_->Initialize(dxManager_.get(), windowManager_.get(), entityComponentSystem_.get());
	/// entity collectionの初期化
	entityComponentSystem_->Initialize();
	/// scene managerの初期化
	sceneManager_->Initialize(renderingFramework_->GetResourceCollection());


#ifdef _DEBUG
	imGuiManager_->Initialize(renderingFramework_->GetResourceCollection());
	imGuiManager_->SetImGuiWindow(windowManager_->GetMainWindow());
	renderingFramework_->SetImGuiManager(imGuiManager_.get());
#endif // _DEBUG


	/// timeの初期化
	Time::Initialize();
}

void GameFramework::Run() {

	/// game loopが終了するまで回す
	while (true) {

		/// 更新処理
		Input::Update();
		Time::Update();

		windowManager_->Update();
#ifdef _DEBUG
		imGuiManager_->Update();

		///!< ゲームデバッグモードの場合は更新処理を行う
		//if (imGuiManager_->GetIsGameDebug()) {
		sceneManager_->Update();
		entityComponentSystem_->Update();
		//}
#else
		sceneManager_->Update();
		entityComponentSystem_->Update();
#endif // _DEBUG

		/// 描画処理
		renderingFramework_->Draw();

		/// 破棄されたら終了
		if (windowManager_->GetMainWindow()->GetProcessMessage()) {
			break;
		}
	}

}
