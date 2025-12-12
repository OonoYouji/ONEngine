#include "RenderingFramework.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/DirectX12/Device/DxDevice.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/ComputeComponents/ShadowCaster/ShadowCaster.h"

/// editor
#include "Engine/Editor/Manager/ImGuiManager.h"

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


	/// ----- RenderTextureの初期化 ----- ///
	const size_t kRenderTexCount = 4;
	const std::array<std::string, kRenderTexCount> renderTexNames = {
		"scene", "debug", "prefab", "shadowMap"
	};
	renderTextures_.resize(kRenderTexCount);

	const Vector4 clearColor = Vector4(0.1f, 0.25f, 0.5f, 1.0f);
	for (size_t i = 0; i < kRenderTexCount; i++) {
		renderTextures_[i] = std::make_unique<SceneRenderTexture>();
		renderTextures_[i]->Initialize(
			"./Assets/Scene/RenderTexture/" + renderTexNames[i], 
			clearColor, EngineConfig::kWindowSize,
			pDxManager_, assetCollection_.get()
		);
	}


	/// PostProcess用UAVTextureの初期化
	std::unique_ptr<UAVTexture> uavTexture = std::make_unique<UAVTexture>();
	uavTexture->Initialize("postProcessResult", pDxManager_, assetCollection_.get());


#ifdef DEBUG_MODE
#else
	copyImagePipeline_ = std::make_unique<CopyImageRenderingPipeline>(assetCollection_.get());
	copyImagePipeline_->Initialize(shaderCompiler_.get(), pDxManager_);
	releaseBuildSubWindow_ = pWindowManager_->GenerateWindow(L"test", Vector2::HD, WindowManager::WindowType::Sub);
	pWindowManager_->HideGameWindow(releaseBuildSubWindow_);
#endif // DEBUG_MODE

}

void RenderingFramework::Draw() {
	/// ----- 描画全体の処理 ----- ///
	PreDraw(pEntityComponentSystem_->GetCurrentGroup());

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
		DrawShadowMap();
		
		/// GameSceneを表示するのか
		if (DebugConfig::isShowGameScene) {
			DrawScene();
		}

		/// DebugSceneを表示するのか
		if(DebugConfig::isShowDebugScene) {
			DrawDebug();
		}
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
	DrawShadowMap();
	DrawScene();
	releaseBuildSubWindow_->PostDraw();

	pWindowManager_->MainWindowPreDraw();
	ECSGroup* currentGroup = pEntityComponentSystem_->GetCurrentGroup();
	copyImagePipeline_->Draw(currentGroup, currentGroup->GetMainCamera2D(), pDxManager_->GetDxCommand());
	pWindowManager_->MainWindowPostDraw();
#endif // DEBUG_MODE

	DxCommandExeAndReset();
}

void RenderingFramework::PreDraw(ECSGroup* _ecsGroup) {
	CameraComponent* camera = _ecsGroup->GetMainCamera();
	CameraComponent* camera2d = _ecsGroup->GetMainCamera2D();
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
	renderingPipelineCollection_->DrawSelectedPrefab(camera, pEntityComponentSystem_->GetCurrentGroup()->GetMainCamera2D());
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
void RenderingFramework::SetImGuiManager(Editor::ImGuiManager* _imGuiManager) {
	pImGuiManager_ = _imGuiManager;
}
#endif // DEBUG_MODE

ShaderCompiler* RenderingFramework::GetShaderCompiler() const {
	return shaderCompiler_.get();
}

