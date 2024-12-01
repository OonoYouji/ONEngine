#include "ComputePipelineState.h"

/// engine
#include "Core/ONEngine.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "LoggingManager/Logger.h"
#include "Debugger/Assertion.h"


ComputePipelineState::ComputePipelineState() {}

ComputePipelineState::~ComputePipelineState() {}



void ComputePipelineState::Create() {
	ID3D12Device* device = ONEngine::GetDxCommon()->GetDevice();
	CreateRootSignature(device);
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

	/// Shaderの設定
	desc.CS = {
		shaderBlob_->GetBlob(ShaderBlob::CS)->GetBufferPointer(),
		shaderBlob_->GetBlob(ShaderBlob::CS)->GetBufferSize()
	};

	/// 生成
	HRESULT result = _device->CreateComputePipelineState(
		&desc, IID_PPV_ARGS(&pipelineState_)
	);

	Assert(SUCCEEDED(result), "compute pipeline state creation failed.");
}

void ComputePipelineState::SetShaderBlob(ShaderBlob* _shaderBlob) {
	shaderBlob_ = _shaderBlob;
}



/// ===================================================
/// ConstantBufferViewを設定
/// ===================================================
void ComputePipelineState::AddCBV(D3D12_SHADER_VISIBILITY shaderVisibilty, uint32_t shaderRegister) {
	D3D12_ROOT_PARAMETER rootParameter{};
	rootParameter.ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter.ShaderVisibility          = shaderVisibilty;
	rootParameter.Descriptor.ShaderRegister = shaderRegister;
	rootParameters_.push_back(rootParameter);
}


/// ===================================================
/// DescriptorRangeの追加
/// ===================================================
void ComputePipelineState::AddDescriptorRange(uint32_t baseShaderRegister, uint32_t numDescriptor, D3D12_DESCRIPTOR_RANGE_TYPE rangeType) {
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.BaseShaderRegister                = baseShaderRegister;                   /// 開始する番号
	descriptorRange.NumDescriptors                    = numDescriptor;                        /// 使用するTextureの数
	descriptorRange.RangeType                         = rangeType;                            /// 使用するRangeType
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; /// 
	descriptorRanges_.push_back(descriptorRange);
}


/// ===================================================
/// DescriptorTableの追加
/// ===================================================
void ComputePipelineState::AddDescriptorTable(D3D12_SHADER_VISIBILITY shaderVisibilty, uint32_t descriptorIndex) {
	assert(descriptorIndex <= descriptorRanges_.size());
	D3D12_ROOT_PARAMETER rootParameter{};
	rootParameter.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;        /// 使用するRegisterの種類
	rootParameter.ShaderVisibility                    = shaderVisibilty;                                   /// 使用するShader
	rootParameter.DescriptorTable.pDescriptorRanges   = &descriptorRanges_[descriptorIndex];               /// Tableの中身の配列
	rootParameter.DescriptorTable.NumDescriptorRanges = descriptorRanges_[descriptorIndex].NumDescriptors; /// Tableで使用する数
	rootParameters_.push_back(rootParameter);
}


/// ===================================================
/// Samplerの追加
/// ===================================================
void ComputePipelineState::AddStaticSampler(D3D12_SHADER_VISIBILITY shaderVisibility, uint32_t shaderRegister) {
	D3D12_STATIC_SAMPLER_DESC staticSampler{};
	staticSampler.Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR; /// バイリニアフィルタ
	staticSampler.AddressU         = D3D12_TEXTURE_ADDRESS_MODE_WRAP; /// 0~1の範囲外をリピート
	staticSampler.AddressV         = D3D12_TEXTURE_ADDRESS_MODE_WRAP; 
	staticSampler.AddressW         = D3D12_TEXTURE_ADDRESS_MODE_WRAP; 
	staticSampler.ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;     /// 比較しない
	staticSampler.MaxLOD           = D3D12_FLOAT32_MAX;               /// ありったけのMipMapを使う
	staticSampler.ShaderRegister   = shaderRegister;                  /// 使用するRegister番号
	staticSampler.ShaderVisibility = shaderVisibility;                /// 使用するShader
	staticSamplers_.push_back(staticSampler);
}
