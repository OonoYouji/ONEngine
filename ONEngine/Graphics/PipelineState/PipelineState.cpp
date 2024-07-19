#include <PipelineState.h>

#include <cassert>

#include <Logger.h>
#include <DxCommon.h>
#include <DxCommand.h>
#include <DxShaderCompiler.h>


/// ===================================================
/// static objects initialize
/// ===================================================
const std::wstring PipelineState::kDirectoryPath_ = L"./Resources/Shaders/";


PipelineState::PipelineState() {}
PipelineState::~PipelineState() {}


/// ===================================================
/// Shaderの初期化
/// ===================================================
void PipelineState::ShaderCompile(const std::wstring& vsFilePath, const wchar_t* vsProfile, const std::wstring& psFilePath, const wchar_t* psProfile) {
	ONE::DxCommon* dxCommon = ONE::DxCommon::GetInstance();

	shader_.vs = dxCommon->GetShaderCompiler()->CompileShader(kDirectoryPath_ + vsFilePath, vsProfile);
	shader_.ps = dxCommon->GetShaderCompiler()->CompileShader(kDirectoryPath_ + psFilePath, psProfile);

}


/// ===================================================
/// rootSignatureとpipelineStateの初期化
/// ===================================================
void PipelineState::Initialize() {

	CreateRootSignature(ONE::DxCommon::GetInstance()->GetDevice());
	CreatePipelineState(ONE::DxCommon::GetInstance()->GetDevice());

}


/// ===================================================
/// InputElementの追加
/// ===================================================
void PipelineState::AddInputElement(const std::string& semanticName, uint32_t semanticIndex, DXGI_FORMAT format) {
	semanticNames_.push_back(semanticName);

	D3D12_INPUT_ELEMENT_DESC desc{};
	desc.SemanticName = semanticNames_.rbegin()->c_str();
	desc.SemanticIndex = semanticIndex;
	desc.Format = format;
	desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElements_.push_back(desc);
}


/// ===================================================
/// FillModeの設定
/// ===================================================
void PipelineState::SetFillMode(FillMode fillMode) {
	if(fillMode == kSolid) {
		rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
	} else {
		rasterizerDesc_.FillMode = D3D12_FILL_MODE_WIREFRAME;
	}
}


/// ===================================================
/// TopologyTypeの設定
/// ===================================================
void PipelineState::SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type) {
	topology_ = type;
}


/// ===================================================
/// PipelineStateをCommandListにセット
/// ===================================================
void PipelineState::SetPipelineState() {
	ID3D12GraphicsCommandList* commandList = ONE::DxCommon::GetInstance()->GetCommand()->GetList();
	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(pipelineState_.Get());
}


/// ===================================================
/// RootSignatureの生成
/// ===================================================
void PipelineState::CreateRootSignature(ID3D12Device* device) {
	HRESULT hr = S_FALSE;
	ComPtr<ID3DBlob> signatureBlob;
	ComPtr<ID3DBlob> errorBlob;

	D3D12_ROOT_SIGNATURE_DESC desc{};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//desc.pParameters = rootParameters_.data();							//- RootParameter配列へのポインタ
	//desc.NumParameters = static_cast<UINT>(rootParameters_.size());		//- RootParameterの配列の長さ
	//desc.pStaticSamplers = staticSamplers_.data();						//- StaticSampler配列へのポインタ
	//desc.NumStaticSamplers = static_cast<UINT>(staticSamplers_.size());	//- StaticSamplerの配列の長さ

	///- シリアライズしてバイナリ
	hr = D3D12SerializeRootSignature(
		&desc, D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob, &errorBlob);
	if(FAILED(hr)) {
		ONE::Logger::ConsolePrint(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	///- バイナリを元に生成
	hr = device->CreateRootSignature(
		0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_)
	);
	assert(SUCCEEDED(hr));
}



/// ===================================================
/// PipelineStateの生成
/// ===================================================
void PipelineState::CreatePipelineState(ID3D12Device* device) {

	/// ---------------------------------------------------
	/// input layout 
	/// ---------------------------------------------------
	for(uint32_t index = 0; index < inputElements_.size(); ++index) {
		inputElements_[index].SemanticName = semanticNames_[index].c_str();
	}
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
	inputLayoutDesc.pInputElementDescs = inputElements_.data();
	inputLayoutDesc.NumElements = static_cast<UINT>(inputElements_.size());


	/// ---------------------------------------------------
	/// resterizer desc
	/// ---------------------------------------------------
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;


	/// ---------------------------------------------------
	/// blend state
	/// ---------------------------------------------------
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;


	/// ---------------------------------------------------
	/// pipeline state desc
	/// ---------------------------------------------------
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
	desc.pRootSignature = rootSignature_.Get();	//- RootSignature
	desc.InputLayout = inputLayoutDesc;			//- InputLayout

	///- Shaderの設定
	desc.VS = {
		shader_.vs->GetBufferPointer(),
		shader_.vs->GetBufferSize()
	};
	desc.PS = {
		shader_.ps->GetBufferPointer(),
		shader_.ps->GetBufferSize()
	};


	desc.BlendState = blendDesc;			//- BlendState
	desc.RasterizerState = rasterizerDesc_;  //- RasterizerState

	desc.NumRenderTargets = 1;								//- 
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//- 
	desc.PrimitiveTopologyType = topology_;				//- 使用する形状Type
	desc.SampleDesc.Count = 1;								//- 
	desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;			//- 
	//desc.DepthStencilState = depthStencilDesc;				//- 
	desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;			//- 

	///- 生成
	HRESULT result = device->CreateGraphicsPipelineState(
		&desc, IID_PPV_ARGS(&pipelineState_)
	);
	assert(SUCCEEDED(result));

}
