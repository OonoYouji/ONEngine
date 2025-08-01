#include "RenderingFramework.h"

/// engine
#include "Engine/Core/DirectX12/Device/DxDevice.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/ImGui/ImGuiManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Core/Config/EngineConfig.h"

RenderingFramework::RenderingFramework() {}
RenderingFramework::~RenderingFramework() {}

void RenderingFramework::Initialize(DxManager* _dxManager, WindowManager* _windowManager, EntityComponentSystem* _pEntityComponentSystem) {

	/// shader compilerの初期化
	shaderCompiler_ = std::make_unique<ShaderCompiler>();
	shaderCompiler_->Initialize();

	dxManager_ = _dxManager;
	windowManager_ = _windowManager;
	pEntityComponentSystem_ = _pEntityComponentSystem;

	resourceCollection_ = std::make_unique<GraphicsResourceCollection>();
	renderingPipelineCollection_ = std::make_unique<RenderingPipelineCollection>(shaderCompiler_.get(), dxManager_, pEntityComponentSystem_, resourceCollection_.get());

	renderingPipelineCollection_->Initialize();
	resourceCollection_->Initialize(dxManager_);


	const size_t kRenderTexCount = 3;
	const std::array<std::string, kRenderTexCount> renderTexNames = {
		"scene", "debug", "prefab"
	};
	renderTextures_.resize(kRenderTexCount);

	for (size_t i = 0; i < kRenderTexCount; i++) {
		renderTextures_[i] = std::make_unique<SceneRenderTexture>();
		renderTextures_[i]->Initialize(
			renderTexNames[i], Vector4(0.1f, 0.25f, 0.5f, 1.0f),
			dxManager_, resourceCollection_.get()
		);
	}


	std::unique_ptr<UAVTexture> uavTexture = std::make_unique<UAVTexture>();
	uavTexture->Initialize("postProcessResult", dxManager_, resourceCollection_.get());


#ifdef _DEBUG
#else
	copyImagePipeline_ = std::make_unique<CopyImageRenderingPipeline>(resourceCollection_.get());
	copyImagePipeline_->Initialize(shaderCompiler_.get(), dxManager_);
	releaseBuildSubWindow_ = windowManager_->GenerateWindow(L"test", Vector2(1280.0f, 720.0f), WindowManager::WindowType::Sub);
	windowManager_->HideGameWindow(releaseBuildSubWindow_);
#endif // _DEBUG

}

void RenderingFramework::Draw() {

	/// descriptor heapの設定
	dxManager_->GetDxSRVHeap()->BindToCommandList(
		dxManager_->GetDxCommand()->GetCommandList()
	);

	/// 描画処理
#ifdef _DEBUG /// imguiの描画
	imGuiManager_->GetDebugGameWindow()->PreDraw();


	if (DebugConfig::selectedMode_ == DebugConfig::SELECTED_MODE_EDITOR) {

		DrawPrefab();
	} else {

		DrawDebug();
		DrawScene();
	}

	imGuiManager_->GetDebugGameWindow()->PostDraw();

	windowManager_->MainWindowPreDraw();
	imGuiManager_->Draw();
	windowManager_->MainWindowPostDraw();

#else
	releaseBuildSubWindow_->PreDraw();
	DrawScene();
	releaseBuildSubWindow_->PostDraw();

	windowManager_->MainWindowPreDraw();
	DrawDebug();
	DrawScene();
	windowManager_->MainWindowPostDraw();

#endif // _DEBUG


	/// commandの実行
	dxManager_->GetDxCommand()->CommandExecute();
	windowManager_->PresentAll();
	dxManager_->GetDxCommand()->CommandReset();
}

void RenderingFramework::DrawScene() {
	Camera* camera = pEntityComponentSystem_->GetMainCamera();
	if (!camera) {
		Console::Log("[error] RenderingFramework::DrawScene: Main Camera is null");
		return;
	}

	SceneRenderTexture* renderTex = renderTextures_[RENDER_TEXTURE_SCENE].get();

	renderTex->CreateBarrierRenderTarget(dxManager_->GetDxCommand());
	renderTex->SetRenderTarget(dxManager_->GetDxCommand(), dxManager_->GetDxDSVHeap());
	renderingPipelineCollection_->DrawEntities(camera, pEntityComponentSystem_->GetMainCamera2D());
	renderTex->CreateBarrierPixelShaderResource(dxManager_->GetDxCommand());

	renderingPipelineCollection_->ExecutePostProcess(renderTex->GetName());
}

void RenderingFramework::DrawDebug() {
	Camera* camera = pEntityComponentSystem_->GetDebugCamera();
	if (!camera) {
		Console::Log("[error] RenderingFramework::DrawDebug: Debug Camera is null");
		return;
	}

	SceneRenderTexture* renderTex = renderTextures_[RENDER_TEXTURE_DEBUG].get();

	renderTex->CreateBarrierRenderTarget(dxManager_->GetDxCommand());
	renderTex->SetRenderTarget(dxManager_->GetDxCommand(), dxManager_->GetDxDSVHeap());
	renderingPipelineCollection_->DrawEntities(camera, pEntityComponentSystem_->GetMainCamera2D());
	renderTex->CreateBarrierPixelShaderResource(dxManager_->GetDxCommand());

	renderingPipelineCollection_->ExecutePostProcess(renderTex->GetName());
}

void RenderingFramework::DrawPrefab() {
	Camera* camera = pEntityComponentSystem_->GetDebugCamera();

	SceneRenderTexture* renderTex = renderTextures_[RENDER_TEXTURE_PREFAB].get();

	renderTex->CreateBarrierRenderTarget(dxManager_->GetDxCommand());
	renderTex->SetRenderTarget(dxManager_->GetDxCommand(), dxManager_->GetDxDSVHeap());
	renderingPipelineCollection_->DrawSelectedPrefab(camera, pEntityComponentSystem_->GetMainCamera2D());
	renderTex->CreateBarrierPixelShaderResource(dxManager_->GetDxCommand());

	renderingPipelineCollection_->ExecutePostProcess(renderTex->GetName());
}

