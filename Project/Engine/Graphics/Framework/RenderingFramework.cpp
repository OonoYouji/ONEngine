#include "RenderingFramework.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/DirectX12/GPUTimeStamp/GPUTimeStamp.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/ComputeComponents/ShadowCaster/ShadowCaster.h"
#include "Engine/Core/Utility/Tools/Log.h"
#include "RenderInfo.h"

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

	assetCollection_ = std::make_unique<Asset::AssetCollection>();
	renderingPipelineCollection_ = std::make_unique<RenderingPipelineCollection>(shaderCompiler_.get(), pDxManager_, pEntityComponentSystem_, assetCollection_.get());

	renderingPipelineCollection_->Initialize();
	assetCollection_->Initialize(pDxManager_);


	/// ----- RenderTextureの初期化 ----- ///
	renderTextures_.resize(RenderInfo::kRenderTextureCount);

	const Vector4 clearColor = Vector4(0.1f, 0.25f, 0.5f, 1.0f);
	for(size_t i = 0; i < RenderInfo::kRenderTextureCount; i++) {
		renderTextures_[i] = std::make_unique<SceneRenderTexture>();
		renderTextures_[i]->Initialize(
			RenderInfo::kRenderTargetDir + RenderInfo::kRenderTargetNames[i],
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
	HeapBindToCommandList();
	PreDraw(pEntityComponentSystem_->GetCurrentGroup());

	GPUTimeStamp::GetInstance().BeginTimeStamp(GPUTimeStampID::RenderingTotal);

#ifdef DEBUG_MODE /// imguiの描画
	/// ----- debug build の描画 ----- ///

	/// ImGuiの描画前処理
	pImGuiManager_->GetDebugGameWindow()->PreDraw();

	/// 選択しているWindow次第で描画を切り替え
	switch(DebugConfig::selectedMode_) {
	case static_cast<int>(DebugConfig::SelectedTab::Prefab):
		/// Prefabモード時の描画
		DrawPrefab();
		break;
	case static_cast<int>(DebugConfig::SelectedTab::Develop):
		DrawShadowMap();

		/// GameSceneを表示するのか
		if(DebugConfig::isShowGameScene) {
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

	GPUTimeStamp::GetInstance().EndTimeStamp(GPUTimeStampID::RenderingTotal);

	// GPUの結果を一括取得（計測の効率化）
	GPUTimeStamp::GetInstance().FetchResults();

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

	GPUTimeStamp::GetInstance().EndTimeStamp(GPUTimeStampID::RenderingTotal);

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
	
	// 実行チェックログ
	static int drawSceneLogCount = 0;
	if (drawSceneLogCount < 10) {
		std::string reason = "";
		if (!camera) reason = "Camera is null";
		else if (!camera->enable) reason = "Camera is disabled";
		else if (!camera->IsMakeViewProjection()) reason = "Projection failed";
		
		if (reason != "") {
			ONEngine::Console::Log("[RenderingFramework] DrawScene early return: " + reason, ONEngine::LogCategory::Engine);
		} else {
			ONEngine::Console::Log("[RenderingFramework] DrawScene started successfully", ONEngine::LogCategory::Engine);
		}
		drawSceneLogCount++;
	}

	if(!camera || !camera->enable || !camera->IsMakeViewProjection()) {
		return;
	}

	GPUTimeStamp::GetInstance().BeginTimeStamp(GPUTimeStampID::MainScene);

	SceneRenderTexture* renderTex = renderTextures_[RENDER_TEXTURE_SCENE].get();

	renderTex->CreateBarrierRenderTarget(pDxManager_->GetDxCommand());
	renderTex->SetRenderTarget(pDxManager_->GetDxCommand(), pDxManager_->GetDxDSVHeap());
	/// 3D描画
	renderingPipelineCollection_->DrawEntities(camera, nullptr);
	renderTex->CreateBarrierPixelShaderResource(pDxManager_->GetDxCommand());

	GPUTimeStamp::GetInstance().EndTimeStamp(GPUTimeStampID::MainScene);

	/// ポストエフェクト
	GPUTimeStamp::GetInstance().BeginTimeStamp(GPUTimeStampID::PostProcess);
	renderingPipelineCollection_->ExecutePostProcess(renderTex->GetName());
	GPUTimeStamp::GetInstance().EndTimeStamp(GPUTimeStampID::PostProcess);

	renderTex->CreateBarrierRenderTarget(pDxManager_->GetDxCommand());
	renderTex->SetRenderTarget(pDxManager_->GetDxCommand(), pDxManager_->GetDxDSVHeap(), false); // Clear=false
	
	// ビューポートを再設定 (ポストプロセスでリセットされている可能性があるため)
	D3D12_VIEWPORT viewport = { 0.0f, 0.0f, EngineConfig::kWindowSize.x, EngineConfig::kWindowSize.y, 0.0f, 1.0f };
	D3D12_RECT scissor = { 0, 0, (LONG)EngineConfig::kWindowSize.x, (LONG)EngineConfig::kWindowSize.y };
	pDxManager_->GetDxCommand()->GetCommandList()->RSSetViewports(1, &viewport);
	pDxManager_->GetDxCommand()->GetCommandList()->RSSetScissorRects(1, &scissor);

	/// パーティクル描画 (ポストエフェクト後)
	renderingPipelineCollection_->DrawParticles(camera);

	/// 2Dオーバーレイ描画
	renderingPipelineCollection_->DrawEntities2D(pEntityComponentSystem_->GetCurrentGroup()->GetMainCamera2D());

	/// Gizmo描画 (最後)
	renderingPipelineCollection_->DrawGizmos(camera);

	renderTex->CreateBarrierPixelShaderResource(pDxManager_->GetDxCommand());
}

void RenderingFramework::DrawDebug() {
	CameraComponent* camera = pEntityComponentSystem_->GetECSGroup("Debug")->GetMainCamera();

	// 実行チェックログ
	static int drawDebugLogCount = 0;
	if (drawDebugLogCount < 10) {
		std::string reason = "";
		if (!camera) reason = "Debug Camera is null";
		else if (!camera->enable) reason = "Debug Camera is disabled";
		else if (!camera->IsMakeViewProjection()) reason = "Debug Projection failed";

		if (reason != "") {
			ONEngine::Console::Log("[RenderingFramework] DrawDebug early return: " + reason, ONEngine::LogCategory::Engine);
		} else {
			ONEngine::Console::Log("[RenderingFramework] DrawDebug started successfully", ONEngine::LogCategory::Engine);
		}
		drawDebugLogCount++;
	}

	if (!camera || !camera->enable || !camera->IsMakeViewProjection()) {
		return;
	}

	GPUTimeStamp::GetInstance().BeginTimeStamp(GPUTimeStampID::DebugDraw);

	SceneRenderTexture* renderTex = renderTextures_[RENDER_TEXTURE_DEBUG].get();

	renderTex->CreateBarrierRenderTarget(pDxManager_->GetDxCommand());
	renderTex->SetRenderTarget(pDxManager_->GetDxCommand(), pDxManager_->GetDxDSVHeap());
	/// 3D描画
	renderingPipelineCollection_->DrawEntities(camera, nullptr);
	renderTex->CreateBarrierPixelShaderResource(pDxManager_->GetDxCommand());

	renderingPipelineCollection_->ExecutePostProcess(renderTex->GetName());

	renderTex->CreateBarrierRenderTarget(pDxManager_->GetDxCommand());
	renderTex->SetRenderTarget(pDxManager_->GetDxCommand(), pDxManager_->GetDxDSVHeap(), false); // Clear=false

	// ビューポートを再設定
	D3D12_VIEWPORT viewport = { 0.0f, 0.0f, EngineConfig::kWindowSize.x, EngineConfig::kWindowSize.y, 0.0f, 1.0f };
	D3D12_RECT scissor = { 0, 0, (LONG)EngineConfig::kWindowSize.x, (LONG)EngineConfig::kWindowSize.y };
	pDxManager_->GetDxCommand()->GetCommandList()->RSSetViewports(1, &viewport);
	pDxManager_->GetDxCommand()->GetCommandList()->RSSetScissorRects(1, &scissor);

	/// パーティクル描画 (ポストエフェクト後)
	renderingPipelineCollection_->DrawParticles(camera);

	// デバッグ用の2D描画
	renderingPipelineCollection_->DrawEntities2D(pEntityComponentSystem_->GetECSGroup("Debug")->GetMainCamera2D());

	// ★重要: Debug表示中でも GameScene の UI (HPバー等) を表示したい場合があるため、GameScene の 2D も重ねる
	if (pEntityComponentSystem_->GetECSGroup("GameScene")) {
		renderingPipelineCollection_->DrawEntities2D(pEntityComponentSystem_->GetECSGroup("GameScene")->GetMainCamera2D(), "GameScene");
	}

	/// Gizmo描画 (最後)
	renderingPipelineCollection_->DrawGizmos(camera);

	renderTex->CreateBarrierPixelShaderResource(pDxManager_->GetDxCommand());

	GPUTimeStamp::GetInstance().EndTimeStamp(GPUTimeStampID::DebugDraw);
}

void RenderingFramework::DrawPrefab() {
	CameraComponent* camera = pEntityComponentSystem_->GetECSGroup("Debug")->GetMainCamera();

	// 実行チェックログ
	static int drawPrefabLogCount = 0;
	if (drawPrefabLogCount < 10) {
		std::string reason = "";
		if (!camera) reason = "Prefab Camera is null";
		else if (!camera->enable) reason = "Prefab Camera is disabled";
		else if (!camera->IsMakeViewProjection()) reason = "Prefab Projection failed";

		if (reason != "") {
			ONEngine::Console::Log("[RenderingFramework] DrawPrefab early return: " + reason, ONEngine::LogCategory::Engine);
		} else {
			ONEngine::Console::Log("[RenderingFramework] DrawPrefab started successfully", ONEngine::LogCategory::Engine);
		}
		drawPrefabLogCount++;
	}

	if (!camera || !camera->enable || !camera->IsMakeViewProjection()) {
		return;
	}

	GPUTimeStamp::GetInstance().BeginTimeStamp(GPUTimeStampID::PrefabDraw);

	SceneRenderTexture* renderTex = renderTextures_[RENDER_TEXTURE_PREFAB].get();

	renderTex->CreateBarrierRenderTarget(pDxManager_->GetDxCommand());
	renderTex->SetRenderTarget(pDxManager_->GetDxCommand(), pDxManager_->GetDxDSVHeap());
	/// 3D描画
	renderingPipelineCollection_->DrawSelectedPrefab(camera, nullptr);
	renderTex->CreateBarrierPixelShaderResource(pDxManager_->GetDxCommand());

	renderingPipelineCollection_->ExecutePostProcess(renderTex->GetName());

	renderTex->CreateBarrierRenderTarget(pDxManager_->GetDxCommand());
	renderTex->SetRenderTarget(pDxManager_->GetDxCommand(), pDxManager_->GetDxDSVHeap(), false); // Clear=false

	// ビューポートを再設定
	D3D12_VIEWPORT viewport = { 0.0f, 0.0f, EngineConfig::kWindowSize.x, EngineConfig::kWindowSize.y, 0.0f, 1.0f };
	D3D12_RECT scissor = { 0, 0, (LONG)EngineConfig::kWindowSize.x, (LONG)EngineConfig::kWindowSize.y };
	pDxManager_->GetDxCommand()->GetCommandList()->RSSetViewports(1, &viewport);
	pDxManager_->GetDxCommand()->GetCommandList()->RSSetScissorRects(1, &scissor);

	/// パーティクル描画 (ポストエフェクト後)
	renderingPipelineCollection_->DrawParticles(camera);

	renderingPipelineCollection_->DrawSelectedPrefab2D(pEntityComponentSystem_->GetECSGroup("Debug")->GetMainCamera2D());

	/// Gizmo描画 (最後)
	renderingPipelineCollection_->DrawGizmos(camera);

	renderTex->CreateBarrierPixelShaderResource(pDxManager_->GetDxCommand());

	GPUTimeStamp::GetInstance().EndTimeStamp(GPUTimeStampID::PrefabDraw);
}

void RenderingFramework::DrawShadowMap() {
	ECSGroup* currentGroup = pEntityComponentSystem_->GetCurrentGroup();

	/// ShadowCaster ComponentArrayの取得&確認
	ComponentArray<ShadowCaster>* shadowCasterArray = currentGroup->GetComponentArray<ShadowCaster>();
	if(!shadowCasterArray || shadowCasterArray->GetUsedComponents().empty()) {
		// Console::LogError("RenderingFramework::DrawShadowMap: ShadowCaster ComponentArray is null");
		return;
	}

	/// ShadowCasterの取得&確認
	ShadowCaster* shadowCaster = shadowCasterArray->GetUsedComponents().front();
	if(!shadowCaster) {
		Console::LogError("RenderingFramework::DrawShadowMap: ShadowCaster is null");
		return;
	}

	/// 投影用のカメラの取得&確認
	CameraComponent* shadowCamera = shadowCaster->GetShadowCasterCamera();
	if(!shadowCamera) {
		Console::LogError("RenderingFramework::DrawShadowMap: ShadowCaster Camera is null");
		return;
	}

	GPUTimeStamp::GetInstance().BeginTimeStamp(GPUTimeStampID::ShadowMap);

	SceneRenderTexture* renderTex = renderTextures_[RENDER_TEXTURE_SHADOW_MAP].get();
	renderTex->CreateBarrierRenderTarget(pDxManager_->GetDxCommand());
	renderTex->SetRenderTarget(pDxManager_->GetDxCommand(), pDxManager_->GetDxDSVHeap());
	renderingPipelineCollection_->DrawEntities(shadowCamera, nullptr);
	renderTex->CreateBarrierPixelShaderResource(pDxManager_->GetDxCommand());

	GPUTimeStamp::GetInstance().EndTimeStamp(GPUTimeStampID::ShadowMap);
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

Asset::AssetCollection* RenderingFramework::GetAssetCollection() const {
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

