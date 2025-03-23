#include "RenderingFramework.h"

/// engine
#include "Engine/Core/DirectX12/Device/DxDevice.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/ImGui/ImGuiManager.h"
#include "Engine/Entity/Collection/EntityCollection.h"

RenderingFramework::RenderingFramework() {}
RenderingFramework::~RenderingFramework() {}

void RenderingFramework::Initialize(DxManager* _dxManager, WindowManager* _windowManager, EntityCollection* _entityCollection) {

	/// shader compilerの初期化
	shaderCompiler_ = std::make_unique<ShaderCompiler>();
	shaderCompiler_->Initialize();

	dxManager_ = _dxManager;
	windowManager_ = _windowManager;
	entityCollection_ = _entityCollection;

	resourceCollection_ = std::make_unique<GraphicsResourceCollection>();
	renderingPipelineCollection_ = std::make_unique<RenderingPipelineCollection>(shaderCompiler_.get(), dxManager_, entityCollection_, resourceCollection_.get());

	renderTextures_.resize(3);
	for (auto& renderTexture : renderTextures_) {
		renderTexture = std::make_unique<RenderTexture>();
	}


	renderingPipelineCollection_->Initialize();
	resourceCollection_->Initialize(dxManager_);

	renderTextures_[0]->Initialize(DXGI_FORMAT_R8G8B8A8_UNORM, Vector4(0.1f, 0.25f, 0.5f, 1.0f), dxManager_, resourceCollection_.get());
	renderTextures_[1]->Initialize(DXGI_FORMAT_R16G16B16A16_FLOAT, Vector4(0.1f, 0.25f, 0.5f, 1.0f), dxManager_, resourceCollection_.get());
	renderTextures_[2]->Initialize(DXGI_FORMAT_R16G16B16A16_FLOAT, Vector4(0.1f, 0.25f, 0.5f, 1.0f), dxManager_, resourceCollection_.get());

}

void RenderingFramework::Draw() {

	/// descriptor heapの設定
	dxManager_->GetDxSRVHeap()->BindToCommandList(
		dxManager_->GetDxCommand()->GetCommandList()
	);

	/// 描画処理
#ifdef _DEBUG /// imguiの描画

	//if (imGuiManager_->GetIsGameDebug()) {
	imGuiManager_->GetDebugGameWindow()->PreDraw();


	/* 
	* [TODO: このCreateBarrierRenderTargetで発生しているエラーを解決する]
	* 配列のindex0は問題なく動作するが、index1以降でエラーが発生する
	*/

	for (auto& renderTexture : renderTextures_) {
		renderTexture->CreateBarrierRenderTarget(dxManager_->GetDxCommand());
	}

	//rendererTextures_[0]->SetRenderTarget(
	//	dxManager_->GetDxCommand(), dxManager_->GetDxDSVHeap(),
	//	rendererTextures_
	//);
	renderTextures_[0]->SetRenderTarget(
		dxManager_->GetDxCommand(), dxManager_->GetDxDSVHeap()
	);

	renderingPipelineCollection_->DrawEntities();

	for (auto& renderTexture : renderTextures_) {
		renderTexture->CreateBarrierPixelShaderResource(dxManager_->GetDxCommand());
	}

	imGuiManager_->GetDebugGameWindow()->PostDraw();
	//}

	windowManager_->MainWindowPreDraw();
	imGuiManager_->Draw();
	windowManager_->MainWindowPostDraw();


#else

	windowManager_->MainWindowPreDraw();
	renderingPipelineCollection_->DrawEntities();
	windowManager_->MainWindowPostDraw();

#endif // _DEBUG

	/// post processの実行
	renderingPipelineCollection_->ExecutePostProcess();

	/// commandの実行
	dxManager_->GetDxCommand()->CommandExecute();
	windowManager_->PresentAll();
	dxManager_->GetDxCommand()->CommandReset();
}

