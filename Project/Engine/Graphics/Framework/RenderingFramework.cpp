#include "RenderingFramework.h"

/// engine
#include "Engine/Core/DirectX12/Device/DxDevice.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/ImGui/ImGuiManager.h"
#include "Engine/Entity/Collection/EntityCollection.h"

/// shader
#include "Engine/Graphics/Pipelines/Primitive/Line2DRenderingPipeline.h"
#include "Engine/Graphics/Pipelines/Mesh/MeshRenderingPipeline.h"


RenderingFramework::RenderingFramework() {}
RenderingFramework::~RenderingFramework() {}

void RenderingFramework::Initialize(DxManager* _dxManager, WindowManager* _windowManager, EntityCollection* _entityCollection) {

	/// shader compilerの初期化
	shaderCompiler_ = std::make_unique<ShaderCompiler>();
	shaderCompiler_->Initialize();

	dxManager_        = _dxManager;
	windowManager_    = _windowManager;
	entityCollection_ = _entityCollection;

	resourceCollection_          = std::make_unique<GraphicsResourceCollection>();
	renderingPipelineCollection_ = std::make_unique<RenderingPipelineCollection>(shaderCompiler_.get(), dxManager_, entityCollection_, resourceCollection_.get());

	renderingPipelineCollection_->Initialize();
	resourceCollection_->Initialize(dxManager_);

}

void RenderingFramework::Draw() {

	/// descriptor heapの設定
	dxManager_->GetDxSRVHeap()->BindToCommandList(
		dxManager_->GetDxCommand()->GetCommandList()
	);

	/// 描画処理
	windowManager_->PreDraw();

#ifdef _DEBUG /// imguiの描画
	imGuiManager_->Draw();
	if (imGuiManager_->GetIsGameDebug()) {
		renderingPipelineCollection_->DrawEntities();
	}
#else
	renderingPipelineCollection_->DrawEntities();
#endif // _DEBUG

	windowManager_->PostDraw();

	/// commandの実行
	dxManager_->GetDxCommand()->CommandExecute();
	windowManager_->Present();
	dxManager_->GetDxCommand()->CommandReset();
}

