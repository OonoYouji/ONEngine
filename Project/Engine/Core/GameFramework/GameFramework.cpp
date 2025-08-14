#include "GameFramework.h"

/// engine
#include "Engine/Core/Utility/Input/Input.h"
#include "Engine/Core/Utility/Time/Time.h"
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"


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
	entityComponentSystem_ = std::make_unique<EntityComponentSystem>(dxManager_.get());
	renderingFramework_ = std::make_unique<RenderingFramework>();
	sceneManager_ = std::make_unique<SceneManager>(entityComponentSystem_.get());
	monoScriptEngine_ = std::make_unique<MonoScriptEngine>();

	editorManager_ = std::make_unique<EditorManager>(entityComponentSystem_.get());
	imGuiManager_ = std::make_unique<ImGuiManager>(dxManager_.get(), windowManager_.get(), entityComponentSystem_.get(), editorManager_.get(), sceneManager_.get());

	/// ポインタを保持
	SetMonoScriptEnginePtr(monoScriptEngine_.get());
	SetEntityComponentSystemPtr(entityComponentSystem_.get());


	/// 各クラスの初期化を行う

	/// directX12の初期化
	dxManager_->Initialize();
	/// windowの初期化
	windowManager_->Initialize();
	/// main windowの生成
#ifdef DEBUG_MODE
	UINT style = WS_OVERLAPPEDWINDOW;
	style &= ~WS_THICKFRAME;
	windowManager_->GenerateWindow(_startSetting.windowName + L" : debug mode", DebugConfig::kDebugWindowSize, WindowManager::WindowType::Main, style);
#else
	windowManager_->GenerateWindow(_startSetting.windowName, _startSetting.windowSize, WindowManager::WindowType::Main);
#endif // DEBUG_MODE

	monoScriptEngine_->Initialize();

	/// input systemの初期化
	Input::Initialize(windowManager_.get(), imGuiManager_.get());
	renderingFramework_->Initialize(dxManager_.get(), windowManager_.get(), entityComponentSystem_.get());
	entityComponentSystem_->Initialize(renderingFramework_->GetResourceCollection());


	/// timeの初期化
	Time::Initialize();

	/// scene managerの初期化
	sceneManager_->Initialize(renderingFramework_->GetResourceCollection());


#ifdef DEBUG_MODE
	imGuiManager_->Initialize(renderingFramework_->GetResourceCollection());
	imGuiManager_->SetImGuiWindow(windowManager_->GetMainWindow());
	renderingFramework_->SetImGuiManager(imGuiManager_.get());
	editorManager_->Initialize(dxManager_.get(), renderingFramework_->GetShaderCompiler());
#endif // DEBUG_MODE


}

void GameFramework::Run() {

	/// game loopが終了するまで回す
	while (true) {

		/// 更新処理
		Input::Update();
		Time::Update();

		renderingFramework_->HeapBindToCommandList();
		windowManager_->Update();
#ifdef DEBUG_MODE
		editorManager_->Update(renderingFramework_->GetResourceCollection());
		imGuiManager_->Update();
		entityComponentSystem_->DebuggingUpdate();

		///!< ゲームデバッグモードの場合は更新処理を行う
		if (DebugConfig::isDebugging) {
			sceneManager_->Update();
			entityComponentSystem_->Update();
		}

		entityComponentSystem_->OutsideOfRuntimeUpdateSystems();
#else
		sceneManager_->Update();
		entityComponentSystem_->Update();
		entityComponentSystem_->OutsideOfRuntimeUpdateSystems();
#endif // DEBUG_MODE

		/// 描画処理
		renderingFramework_->Draw();

		/// 破棄されたら終了
		if (windowManager_->GetMainWindow()->GetProcessMessage()) {
			break;
		}
	}

}
