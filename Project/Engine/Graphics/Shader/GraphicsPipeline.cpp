#include "GraphicsPipeline.h"

GraphicsPipeline::GraphicsPipeline() {}
GraphicsPipeline::~GraphicsPipeline() {}

void GraphicsPipeline::CreatePipeline() {
	/// root signatureとpipeline state objectを生成する
	CreateRootSignature();
	CreatePipelineStateObject();
}


void GraphicsPipeline::SetShadaer(Shader* _shader) {
	pShader_ = _shader;
}

void GraphicsPipeline::AddInputElement(const std::string& _semanticName, uint32_t _semanticIndex, DXGI_FORMAT _format) {
	D3D12_INPUT_ELEMENT_DESC element = {};
	element.SemanticName      = _semanticName.c_str();
	element.SemanticIndex     = _semanticIndex;
	element.Format            = _format;
	element.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElements_.push_back(element);
	semanticNames_.push_back(_semanticName);
}

void GraphicsPipeline::AddCBV(D3D12_SHADER_VISIBILITY _shaderVisibilty, uint32_t _shaderRegister) {
	D3D12_ROOT_PARAMETER parameter{};
	parameter.ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
	parameter.ShaderVisibility          = _shaderVisibilty;
	parameter.Descriptor.ShaderRegister = _shaderRegister;

	rootParameters_.push_back(parameter);
}


void GraphicsPipeline::CreateRootSignature() {}

void GraphicsPipeline::CreatePipelineStateObject() {}
