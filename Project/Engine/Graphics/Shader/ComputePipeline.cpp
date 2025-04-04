#include "ComputePipeline.h"
#include <iostream>

/// engine
#include "Engine/Core/DirectX12/Device/DxDevice.h"
#include "Engine/Core/DirectX12/Command/DxCommand.h"
#include "Engine/Core/Utility/DebugTools/Assert.h"
#include "Engine/Core/Utility/DebugTools/Log.h"

ComputePipeline::ComputePipeline() {}
ComputePipeline::~ComputePipeline() {}

void ComputePipeline::CreatePipeline(DxDevice* _dxDevice) {

	std::string message = std::to_string(rootParameters_.size());
	Log("root parameter size : " + message);

	CreateRootSignature(_dxDevice);
	CreatePipelineStateObject(_dxDevice);

}



void ComputePipeline::SetShader(Shader* _shader) {
	shader_ = _shader;
}

void ComputePipeline::AddCBV(D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _shaderRegister) {
	D3D12_ROOT_PARAMETER parameter{};
	parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	parameter.ShaderVisibility = _shaderVisibility;
	parameter.Descriptor.ShaderRegister = _shaderRegister;

	rootParameters_.push_back(parameter);
}

void ComputePipeline::Add32BitConstant(D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _shaderRegister) {
	D3D12_ROOT_PARAMETER parameter{};
	parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	parameter.ShaderVisibility = _shaderVisibility;
	parameter.Descriptor.ShaderRegister = _shaderRegister;
	parameter.Constants.Num32BitValues = 1;

	rootParameters_.push_back(parameter);
}

void ComputePipeline::AddDescriptorRange(uint32_t _baseShaderRegister, uint32_t _numDescriptor, D3D12_DESCRIPTOR_RANGE_TYPE  _rangeType) {
	D3D12_DESCRIPTOR_RANGE range{};
	range.BaseShaderRegister = _baseShaderRegister;
	range.NumDescriptors = _numDescriptor;
	range.RangeType = _rangeType;
	range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	descriptorRanges_.push_back(range);
}

void ComputePipeline::AddDescriptorTable(D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _descriptorIndex) {
	Assert(descriptorRanges_.size() > _descriptorIndex, "out of range...");

	D3D12_ROOT_PARAMETER parameter{};
	parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter.ShaderVisibility = _shaderVisibility;
	parameter.DescriptorTable.pDescriptorRanges = &descriptorRanges_[_descriptorIndex];
	parameter.DescriptorTable.NumDescriptorRanges = 1;

	rootParameters_.push_back(parameter);
}

void ComputePipeline::AddStaticSampler(D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _shaderRegister) {
	D3D12_STATIC_SAMPLER_DESC sampler{};
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; /// バイリニアフィルタ
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; /// 0~1の範囲外をリピート
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;     /// 比較しない
	sampler.MaxLOD = D3D12_FLOAT32_MAX;               /// ありったけのMipMapを使う
	sampler.ShaderRegister = _shaderRegister;                 /// 使用するRegister番号
	sampler.ShaderVisibility = _shaderVisibility;

	staticSamplers_.push_back(sampler);
}

void ComputePipeline::SetFillMode(D3D12_FILL_MODE _fillMode) {
	rasterizerDesc_.FillMode = _fillMode;
}

void ComputePipeline::SetCullMode(D3D12_CULL_MODE _cullMode) {
	rasterizerDesc_.CullMode = _cullMode;
}

void ComputePipeline::SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE _topologyType) {
	primitiveTopologyType_ = _topologyType;
}

void ComputePipeline::SetPipelineStateForCommandList(DxCommand* _dxCommand) {
	_dxCommand->GetCommandList()->SetPipelineState(pipelineState_.Get());
	_dxCommand->GetCommandList()->SetComputeRootSignature(rootSignature_.Get());
}



void ComputePipeline::CreateRootSignature(DxDevice* _dxDevice) {
	HRESULT hr = S_FALSE;
	ComPtr<ID3DBlob> signatureBlob;
	ComPtr<ID3DBlob> errorBlob;

	D3D12_ROOT_SIGNATURE_DESC desc{};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	desc.pParameters = rootParameters_.data();					/// RootParameter配列へのポインタ
	desc.NumParameters = static_cast<UINT>(rootParameters_.size());	/// RootParameterの配列の長さ
	desc.pStaticSamplers = staticSamplers_.data();					/// StaticSampler配列へのポインタ
	desc.NumStaticSamplers = static_cast<UINT>(staticSamplers_.size());	/// StaticSamplerの配列の長さ

	/// シリアライズしてバイナリ
	hr = D3D12SerializeRootSignature(
		&desc, D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob, &errorBlob
	);

	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		Assert(false, "error...");
	}

	/// バイナリを元に生成
	hr = _dxDevice->GetDevice()->CreateRootSignature(
		0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_)
	);

	Assert(SUCCEEDED(hr), "error...");
}

void ComputePipeline::CreatePipelineStateObject(DxDevice* _dxDevice) {
	/// pipeline state desc
	D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
	desc.pRootSignature = rootSignature_.Get();

	/// shader setting
	desc.CS = {
		shader_->GetCS()->GetBufferPointer(),
		shader_->GetCS()->GetBufferSize()
	};

	/// pipeline state objectの生成
	HRESULT result = _dxDevice->GetDevice()->CreateComputePipelineState(
		&desc, IID_PPV_ARGS(&pipelineState_)
	);

	Assert(SUCCEEDED(result), "error...");
}
