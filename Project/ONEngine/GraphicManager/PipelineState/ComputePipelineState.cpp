#include "ComputePipelineState.h"

/// engine
#include "LoggingManager/Logger.h"
#include "Debugger/Assertion.h"


ComputePipelineState::ComputePipelineState() {}

ComputePipelineState::~ComputePipelineState() {}



void ComputePipelineState::Create() {
	CreateRootSignature();
}

void ComputePipelineState::CreateRootSignature(ID3D12Device* _device) {
	HRESULT          result = S_FALSE;
	ComPtr<ID3DBlob> signatureBlob;
	ComPtr<ID3DBlob> errorBlob;

	D3D12_ROOT_SIGNATURE_DESC desc{};
	desc.Flags             = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; ///
	desc.pParameters       = rootParameters_.data();                                       /// RootParameter配列へのポインタ
	desc.NumParameters     = static_cast<UINT>(rootParameters_.size());                    /// RootParameterの配列の長さ
	desc.pStaticSamplers   = staticSamplers_.data();                                       /// StaticSampler配列へのポインタ
	desc.NumStaticSamplers = static_cast<UINT>(staticSamplers_.size());                    /// StaticSamplerの配列の長さ



	result = D3D12SerializeRootSignature(
		&desc, D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob, &errorBlob
	);
	
	if(FAILED(result)) {
		ONE::Logger::ConsolePrint(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		Assert(false, reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
	}



	result = _device->CreateRootSignature(
		0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_)
	);
	
	Assert(SUCCEEDED(result), "compute root signature creation failed.");
}



void ComputePipelineState::CreatePipelineState(ID3D12Device* _device) {
	D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
	desc.pRootSignature = rootSignature_.Get();	//- RootSignature

	///- Shaderの設定
	desc.CS = {
		shaderBlob->GetCS()->GetBufferPointer(),
		shaderBlob->GetCS()->GetBufferSize()
	};

	///- 生成
	HRESULT result = _device->CreateComputePipelineState(
		&desc, IID_PPV_ARGS(&pipelineState_)
	);

	Assert(SUCCEEDED(result), "compute pipeline state creation failed.");
}
