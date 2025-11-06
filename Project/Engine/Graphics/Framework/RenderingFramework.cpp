#include "RenderingFramework.h"

/// engine
#include "Engine/Core/DirectX12/Device/DxDevice.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/ImGui/ImGuiManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/ComputeComponents/ShadowCaster/ShadowCaster.h"

RenderingFramework::RenderingFramework() {}
RenderingFramework::~RenderingFramework() {}

void RenderingFramework::Initialize(DxManager* _dxm, WindowManager* _windowManager, EntityComponentSystem* _pEntityComponentSystem) {

	/// shader compilerの初期化
	shaderCompiler_ = std::make_unique<ShaderCompiler>();
	shaderCompiler_->Initialize();

	pDxManager_ = _dxm;
	pWindowManager_ = _windowManager;
	pEntityComponentSystem_ = _pEntityComponentSystem;

	assetCollection_ = std::make_unique<AssetCollection>();
	renderingPipelineCollection_ = std::make_unique<RenderingPipelineCollection>(shaderCompiler_.get(), pDxManager_, pEntityComponentSystem_, assetCollection_.get());

	renderingPipelineCollection_->Initialize();
	assetCollection_->Initialize(pDxManager_);


	const size_t kRenderTexCount = 4;
	const std::array<std::string, kRenderTexCount> renderTexNames = {
		"scene", "debug", "prefab", "shadowMap"
	};
	renderTextures_.resize(kRenderTexCount);

	for (size_t i = 0; i < kRenderTexCount; i++) {
		renderTextures_[i] = std::make_unique<SceneRenderTexture>();
		renderTextures_[i]->Initialize(
			"./Assets/Scene/RenderTexture/" + renderTexNames[i], Vector4(0.1f, 0.25f, 0.5f, 1.0f),
			pDxManager_, assetCollection_.get()
		);
	}


	std::unique_ptr<UAVTexture> uavTexture = std::make_unique<UAVTexture>();
	uavTexture->Initialize("postProcessResult", pDxManager_, assetCollection_.get());


#ifdef DEBUG_MODE
#else
	copyImagePipeline_ = std::make_unique<CopyImageRenderingPipeline>(assetCollection_.get());
	copyImagePipeline_->Initialize(shaderCompiler_.get(), pDxManager_);
	releaseBuildSubWindow_ = pWindowManager_->GenerateWindow(L"test", Vector2(1280.0f, 720.0f), WindowManager::WindowType::Sub);
	pWindowManager_->HideGameWindow(releaseBuildSubWindow_);
#endif // DEBUG_MODE

}

void RenderingFramework::Draw() {
	/// ----- 描画全体の処理 ----- ///

	/// 事前処理
	PreDraw();

#ifdef DEBUG_MODE /// imguiの描画
	/// ----- debug build の描画 ----- ///

	/// ImGuiの描画前処理
	pImGuiManager_->GetDebugGameWindow()->PreDraw();

	/// 選択しているWindow次第で描画を切り替え
	switch (DebugConfig::selectedMode_) {
	case DebugConfig::SELECTED_MODE_EDITOR:
		/// Prefabモード時の描画
		DrawPrefab();
		break;
	case DebugConfig::SELECTED_MODE_GAME:
		/// Gameモード時の描画
		DrawDebug();
		DrawScene();
		DrawShadowMap();
		break;
	}


	/// ImGuiの描画後処理
	pImGuiManager_->GetDebugGameWindow()->PostDraw();


	/// メインウィンドウにImGuiを描画
	pWindowManager_->MainWindowPreDraw();
	pImGuiManager_->Draw();
	pWindowManager_->MainWindowPostDraw();

#else
	/// ----- release build の描画 ----- ///
	releaseBuildSubWindow_->PreDraw();
	DrawScene();
	releaseBuildSubWindow_->PostDraw();

	pWindowManager_->MainWindowPreDraw();
	ECSGroup* currentGroup = pEntityComponentSystem_->GetCurrentGroup();
	copyImagePipeline_->Draw(currentGroup, currentGroup->GetMainCamera2D(), pDxManager_->GetDxCommand());
	pWindowManager_->MainWindowPostDraw();
#endif // DEBUG_MODE

	DxCommandExeAndReset();
}

void RenderingFramework::PreDraw() {
	CameraComponent* camera = pEntityComponentSystem_->GetECSGroup("Debug")->GetMainCamera();
	CameraComponent* camera2d = pEntityComponentSystem_->GetECSGroup("Debug")->GetMainCamera2D();
	renderingPipelineCollection_->PreDrawEntities(camera, camera2d);
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

void RenderingFramework::DrawShadowMap() {
	ECSGroup* currentGroup = pEntityComponentSystem_->GetCurrentGroup();

	/// ShadowCaster ComponentArrayの取得&確認
	ComponentArray<ShadowCaster>* shadowCasterArray = currentGroup->GetComponentArray<ShadowCaster>();
	if (!shadowCasterArray || shadowCasterArray->GetUsedComponents().empty()) {
		Console::LogError("RenderingFramework::DrawShadowMap: ShadowCaster ComponentArray is null");
		return;
	}

	/// ShadowCasterの取得&確認
	ShadowCaster* shadowCaster = shadowCasterArray->GetUsedComponents().front();
	if (!shadowCaster) {
		Console::LogError("RenderingFramework::DrawShadowMap: ShadowCaster is null");
		return;
	}

	/// 投影用のカメラの取得&確認
	CameraComponent* shadowCamera = shadowCaster->GetShadowCasterCamera();
	if (!shadowCamera) {
		Console::LogError("RenderingFramework::DrawShadowMap: ShadowCaster Camera is null");
		return;
	}


	SceneRenderTexture* renderTex = renderTextures_[RENDER_TEXTURE_SHADOW_MAP].get();
	renderTex->CreateBarrierRenderTarget(pDxManager_->GetDxCommand());
	renderTex->SetRenderTarget(pDxManager_->GetDxCommand(), pDxManager_->GetDxDSVHeap());
	renderingPipelineCollection_->DrawEntities(shadowCamera, nullptr);
	renderTex->CreateBarrierPixelShaderResource(pDxManager_->GetDxCommand());
}

void RenderingFramework::ResetCommand() {
	pDxManager_->GetDxCommand()->WaitForGpuComplete();
	pDxManager_->GetDxCommand()->CommandReset();
}

void RenderingFramework::HeapBindToCommandList() {
	pDxManager_->GetDxSRVHeap()->BindToCommandList(
		pDxManager_->GetDxCommand()->GetCommandList()
	);
}

void RenderingFramework::DxCommandExeAndReset() {

	/// commandの実行
	pDxManager_->GetDxCommand()->CommandExecuteAndWait();
	pWindowManager_->PresentAll();
	pDxManager_->GetDxCommand()->CommandReset();
}

AssetCollection* RenderingFramework::GetAssetCollection() const {
	return assetCollection_.get();
}

#ifdef DEBUG_MODE
void RenderingFramework::SetImGuiManager(ImGuiManager* _imGuiManager) {
	pImGuiManager_ = _imGuiManager;
}
#endif // DEBUG_MODE

ShaderCompiler* RenderingFramework::GetShaderCompiler() const {
	return shaderCompiler_.get();
}

