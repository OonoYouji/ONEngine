#include "MeshRenderingPipeline.h"

/// engine
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/Entity/Collection/EntityCollection.h"
#include "Engine/Component/Transform/Transform.h"
#include "Engine/Component/RendererComponents/Mesh/MeshRenderer.h"


MeshRenderingPipeline::MeshRenderingPipeline(GraphicsResourceCollection* _resourceCollection, EntityCollection* _entityCollection) 
	: resourceCollection_(_resourceCollection), entityCollection_(_entityCollection) {}

MeshRenderingPipeline::~MeshRenderingPipeline() {}

void MeshRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxDevice* _dxDevice) {

	{	/// pipeline create

		/// shader compile
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"Assets/Shader/Mesh/Mesh.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"Assets/Shader/Mesh/Mesh.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
		pipeline_->AddInputElement("NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT);

		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_BACK);

		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); ///< transform
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1); ///< view projection

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); ///< texture
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);      ///< texture
		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);        ///< texture sampler


		D3D12_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].BlendEnable           = TRUE;
		blendDesc.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		pipeline_->SetBlendDesc(blendDesc);

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable                    = TRUE;
		depthStencilDesc.DepthWriteMask                 = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc                      = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline_->SetDepthStencilDesc(depthStencilDesc);

		pipeline_->CreatePipeline(_dxDevice);

	}


	{	/// constant buffer create

		transformBuffer_ = std::make_unique<ConstantBuffer<Matrix4x4>>();
		transformBuffer_->Create(_dxDevice);

	}

}


void MeshRenderingPipeline::Draw(DxCommand* _dxCommand, EntityCollection* _entityCollection) {
	ID3D12GraphicsCommandList* commandList = _dxCommand->GetCommandList();
	Camera* camera = _entityCollection->GetCameras()[0]; ///< TODO: 仮のカメラ取得

	/// pre draw

	/// settings
	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	camera->GetViewProjectionBuffer()->BindForCommandList(commandList, 1);


	/// draw
	for (auto& entity : _entityCollection->GetEntities()) {
		MeshRenderer*&& meshRenderer = entity->GetComponent<MeshRenderer>();

		if (!meshRenderer) { 
			continue;
		}

		const Model*&& model     = resourceCollection_->GetModel(meshRenderer->GetMeshPath());
		Transform*&&   transform = entity->GetTransform();

		if (!model) { ///< meshがない場合は描画しない
			continue;
		}


		for (auto& mesh : model->GetMeshes()) {

			/// vbv, ibvのセット
			commandList->IASetVertexBuffers(0, 1, &mesh->GetVBV());
			commandList->IASetIndexBuffer(&mesh->GetIBV());

			/// buffer dataのセット
			transformBuffer_->SetMappingData(transform->GetMatWorld());
			transformBuffer_->BindForCommandList(commandList, 0);

			const Texture* texture = resourceCollection_->GetTexture("Assets/Textures/uvChecker.png");
			commandList->SetGraphicsRootDescriptorTable(2, texture->GetGPUDescriptorHandle());

			/// 描画
			commandList->DrawIndexedInstanced(static_cast<UINT>(mesh->GetIndices().size()), 1, 0, 0, 0);

		}
	}


	/// post draw


}

