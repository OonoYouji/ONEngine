#include "Grayscale.h"

#include "Core/ONEngine.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"


Grayscale::Grayscale() {}

Grayscale::~Grayscale() {}

void Grayscale::Initialize() {

	/// compile shader
	shaderBlob_.Compile(
		L"./Resources/Shaders/PostEffects/Grayscale.CS.hlsl",
		L"cs_6_6", ShaderBlob::CS
	);


	/// create pipeline
	computePipeline_.reset(new ComputePipelineState());
	computePipeline_->SetShaderBlob(&shaderBlob_);

	computePipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	computePipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV);

	computePipeline_->AddDescriptorTable(0);
	computePipeline_->AddDescriptorTable(1);

	computePipeline_->Create();

}

void Grayscale::Execution(ID3D12Resource* _input, ID3D12Resource* _output) {
	ID3D12GraphicsCommandList* pCommandList = ONEngine::GetDxCommon()->GetDxCommand()->GetList();

	computePipeline_->SetToCommnadList(pCommandList);
}
