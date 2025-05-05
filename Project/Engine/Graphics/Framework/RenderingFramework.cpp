#include "RenderingFramework.h"

/// engine
#include "Engine/Core/DirectX12/Device/DxDevice.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/ImGui/ImGuiManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

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

	{	/// game render textures
		renderTextures_.resize(4);
		for (auto& renderTexture : renderTextures_) {
			renderTexture = std::make_unique<RenderTexture>();
		}

		renderTextures_[0]->Initialize(DXGI_FORMAT_R8G8B8A8_UNORM, Vector4(0.1f, 0.25f, 0.5f, 1.0f), "scene", dxManager_, resourceCollection_.get());
		renderTextures_[1]->Initialize(DXGI_FORMAT_R16G16B16A16_FLOAT, Vector4(0.1f, 0.25f, 0.5f, 1.0f), "worldPosition", dxManager_, resourceCollection_.get());
		renderTextures_[2]->Initialize(DXGI_FORMAT_R16G16B16A16_FLOAT, Vector4(0.1f, 0.25f, 0.5f, 1.0f), "normal", dxManager_, resourceCollection_.get());
		renderTextures_[3]->Initialize(DXGI_FORMAT_R8G8B8A8_UNORM, {}, "flags", dxManager_, resourceCollection_.get());
	}

	{	/// debug render texture
		debugRenderTextures_.resize(4);
		for (auto& renderTexture : debugRenderTextures_) {
			renderTexture = std::make_unique<RenderTexture>();
		}

		debugRenderTextures_[0]->Initialize(DXGI_FORMAT_R8G8B8A8_UNORM, Vector4(0.1f, 0.25f, 0.5f, 1.0f), "debugScene", dxManager_, resourceCollection_.get());
		debugRenderTextures_[1]->Initialize(DXGI_FORMAT_R16G16B16A16_FLOAT, Vector4(0.1f, 0.25f, 0.5f, 1.0f), "debugWorldPosition", dxManager_, resourceCollection_.get());
		debugRenderTextures_[2]->Initialize(DXGI_FORMAT_R16G16B16A16_FLOAT, Vector4(0.1f, 0.25f, 0.5f, 1.0f), "debugNormalize", dxManager_, resourceCollection_.get());
		debugRenderTextures_[3]->Initialize(DXGI_FORMAT_R8G8B8A8_UNORM, {}, "debugFlags", dxManager_, resourceCollection_.get());
	}

	std::unique_ptr<UAVTexture> uavTexture = std::make_unique<UAVTexture>();
	uavTexture->Initialize("postProcessResult", dxManager_, resourceCollection_.get());


#ifdef _DEBUG
#else
	copyImagePipeline_ = std::make_unique<CopyImageRenderingPipeline>(resourceCollection_.get());
	copyImagePipeline_->Initialize(shaderCompiler_.get(), dxManager_);
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

	{	/// Debug Camera Rendering
		for (auto& renderTexture : debugRenderTextures_) {
			renderTexture->CreateBarrierRenderTarget(dxManager_->GetDxCommand());
		}

		debugRenderTextures_[0]->SetRenderTarget(
			dxManager_->GetDxCommand(), dxManager_->GetDxDSVHeap(),
			debugRenderTextures_
		);

		renderingPipelineCollection_->DrawEntities(pEntityComponentSystem_->GetDebugCamera(), pEntityComponentSystem_->GetMainCamera2D());

		for (auto& renderTexture : debugRenderTextures_) {
			renderTexture->CreateBarrierPixelShaderResource(dxManager_->GetDxCommand());
		}
	}

	{	/// Game Camera Rendering
		for (auto& renderTexture : renderTextures_) {
			renderTexture->CreateBarrierRenderTarget(dxManager_->GetDxCommand());
		}

		renderTextures_[0]->SetRenderTarget(
			dxManager_->GetDxCommand(), dxManager_->GetDxDSVHeap(),
			renderTextures_
		);

		renderingPipelineCollection_->DrawEntities(pEntityComponentSystem_->GetMainCamera(), pEntityComponentSystem_->GetMainCamera2D());

		for (auto& renderTexture : renderTextures_) {
			renderTexture->CreateBarrierPixelShaderResource(dxManager_->GetDxCommand());
		}
	}

	imGuiManager_->GetDebugGameWindow()->PostDraw();

	/// post processの実行
	renderingPipelineCollection_->ExecutePostProcess();

	windowManager_->MainWindowPreDraw();
	imGuiManager_->Draw();
	windowManager_->MainWindowPostDraw();

#else
	for (auto& renderTexture : renderTextures_) {
		renderTexture->CreateBarrierRenderTarget(dxManager_->GetDxCommand());
	}

	renderTextures_[0]->SetRenderTarget(
		dxManager_->GetDxCommand(), dxManager_->GetDxDSVHeap(),
		renderTextures_
	);
	renderingPipelineCollection_->DrawEntities(pEntityComponentSystem_->GetMainCamera(), pEntityComponentSystem_->GetMainCamera2D());

	for (auto& renderTexture : renderTextures_) {
		renderTexture->CreateBarrierPixelShaderResource(dxManager_->GetDxCommand());
	}

	/// post processの実行
	renderingPipelineCollection_->ExecutePostProcess();

	windowManager_->MainWindowPreDraw();
	copyImagePipeline_->Draw(dxManager_->GetDxCommand(), pEntityComponentSystem_, pEntityComponentSystem_->GetMainCamera2D());
	windowManager_->MainWindowPostDraw();

#endif // _DEBUG


	/// commandの実行
	dxManager_->GetDxCommand()->CommandExecute();
	windowManager_->PresentAll();
	dxManager_->GetDxCommand()->CommandReset();
}

