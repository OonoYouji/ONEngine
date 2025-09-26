#include "RenderingFramework.h"

/// engine
#include "Engine/Core/DirectX12/Device/DxDevice.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/ImGui/ImGuiManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"

RenderingFramework::RenderingFramework() {}
RenderingFramework::~RenderingFramework() {}

void RenderingFramework::Initialize(DxManager* _dxManager, WindowManager* _windowManager, EntityComponentSystem* _pEntityComponentSystem) {

	/// shader compilerの初期化
	shaderCompiler_ = std::make_unique<ShaderCompiler>();
	shaderCompiler_->Initialize();

	pDxManager_ = _dxManager;
	pWindowManager_ = _windowManager;
	pEntityComponentSystem_ = _pEntityComponentSystem;

	resourceCollection_ = std::make_unique<GraphicsResourceCollection>();
	renderingPipelineCollection_ = std::make_unique<RenderingPipelineCollection>(shaderCompiler_.get(), pDxManager_, pEntityComponentSystem_, resourceCollection_.get());

	renderingPipelineCollection_->Initialize();
	resourceCollection_->Initialize(pDxManager_);


	const size_t kRenderTexCount = 3;
	const std::array<std::string, kRenderTexCount> renderTexNames = {
		"scene", "debug", "prefab"
	};
	renderTextures_.resize(kRenderTexCount);

	for (size_t i = 0; i < kRenderTexCount; i++) {
		renderTextures_[i] = std::make_unique<SceneRenderTexture>();
		renderTextures_[i]->Initialize(
			renderTexNames[i], Vector4(0.1f, 0.25f, 0.5f, 1.0f),
			pDxManager_, resourceCollection_.get()
		);
	}


	std::unique_ptr<UAVTexture> uavTexture = std::make_unique<UAVTexture>();
	uavTexture->Initialize("postProcessResult", pDxManager_, resourceCollection_.get());


#ifdef DEBUG_MODE
#else
	copyImagePipeline_ = std::make_unique<CopyImageRenderingPipeline>(resourceCollection_.get());
	copyImagePipeline_->Initialize(shaderCompiler_.get(), pDxManager_);
	releaseBuildSubWindow_ = pWindowManager_->GenerateWindow(L"test", Vector2(1280.0f, 720.0f), WindowManager::WindowType::Sub);
	pWindowManager_->HideGameWindow(releaseBuildSubWindow_);
#endif // DEBUG_MODE

}

void RenderingFramework::Draw() {

	PreDraw();

#ifdef DEBUG_MODE /// imguiの描画
	pImGuiManager_->GetDebugGameWindow()->PreDraw();


	if (DebugConfig::selectedMode_ == DebugConfig::SELECTED_MODE_EDITOR) {
		DrawPrefab();
	} else {
		DrawDebug();
		DrawScene();
	}

	pImGuiManager_->GetDebugGameWindow()->PostDraw();

	pWindowManager_->MainWindowPreDraw();
	pImGuiManager_->Draw();
	pWindowManager_->MainWindowPostDraw();

#else
	releaseBuildSubWindow_->PreDraw();
	DrawScene();
	releaseBuildSubWindow_->PostDraw();

	pWindowManager_->MainWindowPreDraw();
	ECSGroup* currentGroup = pEntityComponentSystem_->GetCurrentGroup();
	copyImagePipeline_->Draw(currentGroup, {}, currentGroup->GetMainCamera2D(), pDxManager_->GetDxCommand());
	pWindowManager_->MainWindowPostDraw();
#endif // DEBUG_MODE

	DxCommandExeAndReset();
}

void RenderingFramework::PreDraw() {
	CameraComponent* camera = pEntityComponentSystem_->GetECSGroup("Debug")->GetMainCamera();
	renderingPipelineCollection_->PreDrawEntities(camera, pEntityComponentSystem_->GetECSGroup("Debug")->GetMainCamera2D());
}

void RenderingFramework::DrawScene() {
	CameraComponent* camera = pEntityComponentSystem_->GetCurrentGroup()->GetMainCamera();
	if (!camera) {
		Console::Log("[error] RenderingFramework::DrawScene: Main Camera is null");
		return;
	}

	SceneRenderTexture* renderTex = renderTextures_[RENDER_TEXTURE_SCENE].get();

	renderTex->CreateBarrierRenderTarget(pDxManager_->GetDxCommand());
	renderTex->SetRenderTarget(pDxManager_->GetDxCommand(), pDxManager_->GetDxDSVHeap());
	renderingPipelineCollection_->DrawEntities(camera, pEntityComponentSystem_->GetCurrentGroup()->GetMainCamera2D());
	renderTex->CreateBarrierPixelShaderResource(pDxManager_->GetDxCommand());

	renderingPipelineCollection_->ExecutePostProcess(renderTex->GetName());
}

void RenderingFramework::DrawDebug() {
	CameraComponent* camera = pEntityComponentSystem_->GetECSGroup("Debug")->GetMainCamera();
	SceneRenderTexture* renderTex = renderTextures_[RENDER_TEXTURE_DEBUG].get();

	renderTex->CreateBarrierRenderTarget(pDxManager_->GetDxCommand());
	renderTex->SetRenderTarget(pDxManager_->GetDxCommand(), pDxManager_->GetDxDSVHeap());
	renderingPipelineCollection_->DrawEntities(camera, pEntityComponentSystem_->GetCurrentGroup()->GetMainCamera2D());
	renderTex->CreateBarrierPixelShaderResource(pDxManager_->GetDxCommand());

	renderingPipelineCollection_->ExecutePostProcess(renderTex->GetName());
}

void RenderingFramework::DrawPrefab() {
	CameraComponent* camera = pEntityComponentSystem_->GetECSGroup("Debug")->GetMainCamera();

	SceneRenderTexture* renderTex = renderTextures_[RENDER_TEXTURE_PREFAB].get();

	renderTex->CreateBarrierRenderTarget(pDxManager_->GetDxCommand());
	renderTex->SetRenderTarget(pDxManager_->GetDxCommand(), pDxManager_->GetDxDSVHeap());
	renderingPipelineCollection_->DrawSelectedPrefab(camera, pEntityComponentSystem_->GetECSGroup("Debug")->GetMainCamera2D());
	renderTex->CreateBarrierPixelShaderResource(pDxManager_->GetDxCommand());

	renderingPipelineCollection_->ExecutePostProcess(renderTex->GetName());
}

void RenderingFramework::HeapBindToCommandList() {
	pDxManager_->GetDxSRVHeap()->BindToCommandList(
		pDxManager_->GetDxCommand()->GetCommandList()
	);
}

void RenderingFramework::DxCommandExeAndReset() {

	/// commandの実行
	pDxManager_->GetDxCommand()->CommandExecute();
	pWindowManager_->PresentAll();
	pDxManager_->GetDxCommand()->CommandReset();
}

GraphicsResourceCollection* RenderingFramework::GetResourceCollection() const {
	return resourceCollection_.get();
}

#ifdef DEBUG_MODE
void RenderingFramework::SetImGuiManager(ImGuiManager* _imGuiManager) {
	pImGuiManager_ = _imGuiManager;
}
#endif // DEBUG_MODE

ShaderCompiler* RenderingFramework::GetShaderCompiler() const {
	return shaderCompiler_.get();
}

