#include "RenderingFramework.h"

/// engine
#include "Engine/Core/DirectX12/Device/DxDevice.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Entity/Collection/EntityCollection.h"

/// shader
#include "Engine/Graphics/Pipelines/Primitive/Line2DRenderingPipeline.h"
#include "Engine/Graphics/Pipelines/Mesh/MeshRenderingPipeline.h"


RenderingFramework::RenderingFramework(EntityCollection* _entityCollection)
	: entityCollection_(_entityCollection) {}

RenderingFramework::~RenderingFramework() {}

void RenderingFramework::Initialize(DxManager* _dxManager, WindowManager* _windowManager) {

	/// shader compilerの初期化
	shaderCompiler_ = std::make_unique<ShaderCompiler>();
	shaderCompiler_->Initialize();

	dxManager_     = _dxManager;
	windowManager_ = _windowManager;

	renderingPipelineCollection_ = std::make_unique<RenderingPipelineCollection>(shaderCompiler_.get(), dxManager_, entityCollection_);
	renderingPipelineCollection_->Initialize();

}

void RenderingFramework::Draw() {

	/// descriptor heapの設定
	dxManager_->GetDxDescriptorHeap(DescriptorHeapType_CBV_SRV_UAV)->BindToCommandList(
		dxManager_->GetDxCommand()->GetCommandList()
	);

	/// 描画処理
	windowManager_->PreDraw();

	renderingPipelineCollection_->DrawEntities();

	windowManager_->PostDraw();


	/// commandの実行
	dxManager_->GetDxCommand()->CommandExecute();
	windowManager_->Present();
	dxManager_->GetDxCommand()->CommandReset();
}

