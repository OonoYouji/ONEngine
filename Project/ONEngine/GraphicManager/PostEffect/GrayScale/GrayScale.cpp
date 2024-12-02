#include "Grayscale.h"

/// std
#include <memory>

/// engine
#include "Core/ONEngine.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxBarrierCreator.h"

#include "../PostEffectPipeline/PostEffectPipelineManager.h"


Grayscale::Grayscale() {
	Initialize();
}

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

void Grayscale::Execution(Texture* _input, Texture* _output) {
	ID3D12GraphicsCommandList* pCommandList = ONEngine::GetDxCommon()->GetDxCommand()->GetList();
	auto srvDescriptorHeap = ONEngine::GetDxCommon()->GetSRVDescriptorHeap();

	computePipeline_->SetToCommnadList(pCommandList);

	/// srv resourceに変更
	if(_input->GetCurrentState() != D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) {
		ONE::DxBarrierCreator::CreateBarrier(_input->GetResource(), _input->GetCurrentState(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		_input->SetCurrentState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}

	
	/// uav resourceに変更
	if(_output->GetCurrentState() != D3D12_RESOURCE_STATE_UNORDERED_ACCESS) {
		ONE::DxBarrierCreator::CreateBarrier(_output->GetResource(), _output->GetCurrentState(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		_output->SetCurrentState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}



	D3D12_GPU_DESCRIPTOR_HANDLE inputGPUHandle  = srvDescriptorHeap->GetGPUDescriptorHandel(_input->GetDescriptorIndex(Texture::SRV));
	D3D12_GPU_DESCRIPTOR_HANDLE outputGPUHandle = srvDescriptorHeap->GetGPUDescriptorHandel(_output->GetDescriptorIndex(Texture::UAV));

	pCommandList->SetComputeRootDescriptorTable(0, inputGPUHandle);
	pCommandList->SetComputeRootDescriptorTable(1, outputGPUHandle);

	UINT threadGroupCountX = (1280 + 15) / 16;
	UINT threadGroupCountY = (720 + 15) / 16;

	pCommandList->Dispatch(threadGroupCountX, threadGroupCountY, 1);
}
