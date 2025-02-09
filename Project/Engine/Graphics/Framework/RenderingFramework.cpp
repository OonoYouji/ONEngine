#include "RenderingFramework.h"

/// engine
#include "Engine/DirectX12/Device/DxDevice.h"
#include "Engine/DirectX12/Manager/DxManager.h"
#include "Engine/Entity/Collection/EntityCollection.h"

/// shader
#include "Engine/Graphics/Renderers/Primitive/Line2DRenderer.h"
#include "Engine/Graphics/Renderers/Mesh/MeshRenderer.h"


RenderingFramework::RenderingFramework(EntityCollection* _entityCollection)
	: entityCollection_(_entityCollection) {}

RenderingFramework::~RenderingFramework() {}

void RenderingFramework::Initialize(DxManager* _dxManager, WindowManager* _windowManager) {

	/// shader compilerの初期化
	shaderCompiler_ = std::make_unique<ShaderCompiler>();
	shaderCompiler_->Initialize();

	dxManager_     = _dxManager;
	windowManager_ = _windowManager;


	/// レンダラーの生成
	GenerateRenderer<Line2DRenderer>();
	GenerateRenderer<MeshRenderer>();
}

void RenderingFramework::Draw() {

	/// descriptor heapの設定
	dxManager_->GetDxDescriptorHeap(DescriptorHeapType_CBV_SRV_UAV)->BindToCommandList(
		dxManager_->GetDxCommand()->GetCommandList()
	);

	/// 描画処理
	windowManager_->PreDraw();


	/// shaderのpre draw
	for (std::unique_ptr<IRenderer>& renderer : renderers_) {
		renderer->PreDraw(dxManager_->GetDxCommand());
	}

	/// 描画データを積む
	entityCollection_->Draw();

	/// shaderのpost draw
	for (std::unique_ptr<IRenderer>& renderer : renderers_) {
		renderer->PostDraw(dxManager_->GetDxCommand());
	}



	windowManager_->PostDraw();


	/// commandの実行
	dxManager_->GetDxCommand()->CommandExecute();
	windowManager_->Present();
	dxManager_->GetDxCommand()->CommandReset();
}

