#pragma once

/// directx
#include <d3d12.h>

/// engine
#include "Engine/DirectX12/ComPtr/ComPtr.h"



/// ===================================================
/// グラフィクス用	pipeline
/// ===================================================
class GraphicsPipeline {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	GraphicsPipeline();
	~GraphicsPipeline();


	void CreatePipeline();



private:

	/// ===================================================
	/// private : methods
	/// ===================================================

	void CreateRootSignature();
	void CreatePipelineStateObject();
	void CreateShader();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<ID3D12RootSignature> rootSignature_;
	ComPtr<ID3D12PipelineState> pipelineState_;

};

