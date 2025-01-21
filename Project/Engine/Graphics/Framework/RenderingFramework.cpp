#include "RenderingFramework.h"

/// engine
#include "Engine/DirectX12/Device/DxDevice.h"
#include "Engine/DirectX12/Manager/DxManager.h"


RenderingFramework::RenderingFramework() {}
RenderingFramework::~RenderingFramework() {}

void RenderingFramework::Initialize(DxManager* _dxManager) {

	/// shader compilerの初期化
	shaderCompiler_ = std::make_unique<ShaderCompiler>();
	shaderCompiler_->Initialize();

	dxManager_ = _dxManager;

}

void RenderingFramework::Draw() {

	for (std::unique_ptr<IRenderer>& renderer : renderers_) {
		renderer->PreDraw(dxManager_->GetDxCommand());
		renderer->DrawCall();
		renderer->PostDraw(dxManager_->GetDxCommand());
	}

}

