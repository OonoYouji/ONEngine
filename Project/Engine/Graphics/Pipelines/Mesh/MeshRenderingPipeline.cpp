#include "MeshRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/Entity/Collection/EntityCollection.h"
#include "Engine/Component/Transform/Transform.h"
#include "Engine/Component/RendererComponents/Mesh/MeshRenderer.h"


MeshRenderingPipeline::MeshRenderingPipeline(GraphicsResourceCollection* _resourceCollection, EntityCollection* _entityCollection) 
	: resourceCollection_(_resourceCollection), entityCollection_(_entityCollection) {}

MeshRenderingPipeline::~MeshRenderingPipeline() {}

void MeshRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

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

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); ///< view projection: 0

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< material
		pipeline_->AddDescriptorRange(1, 16, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); ///< texture
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< transform
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);       ///< material  : 1
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1);       ///< texture   : 2
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 2);      ///< transform : 3

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);         ///< texture sampler

		pipeline_->Add32BitConstant(D3D12_SHADER_VISIBILITY_VERTEX, 1);        ///< instance id: 4


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

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());

	}


	{	/// buffer create

		transformBuffer_ = std::make_unique<StructuredBuffer<Matrix4x4>>();
		transformBuffer_->Create(static_cast<uint32_t>(kMaxRenderingMeshCount_), _dxManager->GetDxDevice(), _dxManager->GetDxDescriptorHeap(DescriptorHeapType_CBV_SRV_UAV));

		materialBuffer = std::make_unique<StructuredBuffer<Vector4>>();
		materialBuffer->Create(static_cast<uint32_t>(kMaxRenderingMeshCount_), _dxManager->GetDxDevice(), _dxManager->GetDxDescriptorHeap(DescriptorHeapType_CBV_SRV_UAV));


	}

}


void MeshRenderingPipeline::Draw(DxCommand* _dxCommand, EntityCollection* _entityCollection) {
	ID3D12GraphicsCommandList* commandList = _dxCommand->GetCommandList();
	Camera* camera = _entityCollection->GetCameras()[0]; ///< TODO: 仮のカメラ取得

	/// pre draw

	/// settings
	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	camera->GetViewProjectionBuffer()->BindForCommandList(commandList, 0);

	/// mesh と transform の対応付け
	std::unordered_map<std::string, std::list<MeshRenderer*>> rendererPerMesh;
	for (auto& entity : _entityCollection->GetEntities()) {
		MeshRenderer*&& meshRenderer = entity->GetComponent<MeshRenderer>();
		if (meshRenderer) {
			rendererPerMesh[meshRenderer->GetMeshPath()].push_back(meshRenderer);
		}
	}

	size_t transformIndex = 0; ///< transform buffer の index
	UINT   instanceIndex  = 0; ///< instance id
	for (auto& [meshPath, renderers] : rendererPerMesh) {

		/// modelの取得、なければ次へ
		const Model*&& model = resourceCollection_->GetModel(meshPath);
		if (!model) { 
			continue;
		}

		/// transform, material を mapping
		for (auto& renderer : renderers) {
			materialBuffer->SetMappedData(transformIndex, renderer->GetColor());
			transformBuffer_->SetMappedData(transformIndex, renderer->GetOwner()->GetTransform()->GetMatWorld());
			++transformIndex;
		}
		materialBuffer->BindToCommandList(1, commandList);
		transformBuffer_->BindToCommandList(3, commandList);

		/// mesh の描画
		for (auto& mesh : model->GetMeshes()) {
			/// vbv, ibvのセット
			commandList->IASetVertexBuffers(0, 1, &mesh->GetVBV());
			commandList->IASetIndexBuffer(&mesh->GetIBV());
			commandList->SetGraphicsRoot32BitConstant(4, instanceIndex, 0);

			/// buffer dataのセット、先頭の texture gpu handle をセットする
			auto& textures = resourceCollection_->GetTextures();
			commandList->SetGraphicsRootDescriptorTable(2, textures.begin()->second->GetGPUDescriptorHandle());

			/// 描画
			commandList->DrawIndexedInstanced(
				static_cast<UINT>(mesh->GetIndices().size()),
				static_cast<UINT>(renderers.size()), 
				0, 0, 0
			);
		}

		instanceIndex += static_cast<UINT>(renderers.size());
	}

	/// post draw
}

