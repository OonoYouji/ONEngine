#include "DissolveMeshRenderingPipeline.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"



using namespace ONEngine;

DissolveMeshRenderingPipeline::DissolveMeshRenderingPipeline(AssetCollection* _ac)
	: pAssetCollection_(_ac) {
}

void DissolveMeshRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	{
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Render/Mesh/DissolveMesh.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"./Packages/Shader/Render/Mesh/DissolveMesh.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);


		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
		pipeline_->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);


		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_BACK);
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetDepthStencilDesc(DefaultDepthStencilDesc());


		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// view projection: 0

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  /// transform
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  /// material
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  /// dissolve params
		pipeline_->AddDescriptorRange(2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  /// textureId
		pipeline_->AddDescriptorRange(3, MAX_TEXTURE_COUNT, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// texture

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 0);      /// transform 
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1);       /// material
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 2);       /// dissolve params
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 3);       /// textureId
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 4);       /// texture

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);         /// texture sampler

		pipeline_->Add32BitConstant(D3D12_SHADER_VISIBILITY_VERTEX, 1);        /// instance id: 6


		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}

}

void DissolveMeshRenderingPipeline::Draw(ECSGroup* _ecsGroup, CameraComponent* _camera, DxCommand* _dxCommand) {
}
