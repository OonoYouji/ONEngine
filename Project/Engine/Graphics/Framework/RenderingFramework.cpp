#include "RenderingFramework.h"

/// engine
#include "Engine/DirectX12/Device/DxDevice.h"


RenderingFramework::RenderingFramework() {}
RenderingFramework::~RenderingFramework() {}

void RenderingFramework::Initialize(DxDevice* _dxDevice) {

	/// shader compilerの初期化
	shaderCompiler_ = std::make_unique<ShaderCompiler>();
	shaderCompiler_->Initialize();

	dxDevice_ = _dxDevice;

}

