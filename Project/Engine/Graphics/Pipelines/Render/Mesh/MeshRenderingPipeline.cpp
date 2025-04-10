#include "MeshRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/Entity/Collection/EntityCollection.h"
#include "Engine/Component/ComputeComponents/Transform/Transform.h"
#include "Engine/Component/RendererComponents/Mesh/MeshRenderer.h"


MeshRenderingPipeline::MeshRenderingPipeline(GraphicsResourceCollection* _resourceCollection) 
	: resourceCollection_(_resourceCollection) {}

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
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< textureId
		pipeline_->AddDescriptorRange(2, 32, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); ///< texture
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< transform
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);       ///< material  : 1
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1);       ///< textureId : 2
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 2);       ///< texture   : 3
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 3);      ///< transform : 4

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);         ///< texture sampler

		pipeline_->Add32BitConstant(D3D12_SHADER_VISIBILITY_VERTEX, 1);        ///< instance id: 5


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

		pipeline_->SetRTVNum(4); /// 色、ワールド座標、法線、フラグ
		pipeline_->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, 1);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, 2);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 3);

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());

	}


	{	/// buffer create

		transformBuffer_ = std::make_unique<StructuredBuffer<Matrix4x4>>();
		transformBuffer_->Create(static_cast<uint32_t>(kMaxRenderingMeshCount_), _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());

		materialBuffer = std::make_unique<StructuredBuffer<Vector4>>();
		materialBuffer->Create(static_cast<uint32_t>(kMaxRenderingMeshCount_), _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());

		textureIdBuffer_ = std::make_unique<StructuredBuffer<uint32_t>>();
		textureIdBuffer_->Create(static_cast<uint32_t>(kMaxRenderingMeshCount_), _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());

	}

}


void MeshRenderingPipeline::Draw(DxCommand* _dxCommand, EntityCollection* _entityCollection) {

	/// mesh と transform の対応付け
	std::unordered_map<std::string, std::list<MeshRenderer*>> rendererPerMesh;
	for (auto& entity : _entityCollection->GetEntities()) {
		MeshRenderer*&& meshRenderer = entity->GetComponent<MeshRenderer>();
		if (meshRenderer) {
			rendererPerMesh[meshRenderer->GetMeshPath()].push_back(meshRenderer);
		}
	}

	///< 描画対象がなければ 早期リターン
	if (rendererPerMesh.empty()) {
		return;
	}


	ID3D12GraphicsCommandList* commandList = _dxCommand->GetCommandList();
	Camera* camera = _entityCollection->GetMainCamera();
	if (!camera) { ///< カメラがない場合は描画しない
		return;
	}

	/// settings
	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	camera->GetViewProjectionBuffer()->BindForGraphicsCommandList(commandList, 0);

	/// buffer dataのセット、先頭の texture gpu handle をセットする
	auto& textures = resourceCollection_->GetTextures();
	commandList->SetGraphicsRootDescriptorTable(3, (*textures.begin())->GetSRVGPUHandle());


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

			/// materialのセット
			materialBuffer->SetMappedData(
				transformIndex, 
				renderer->GetColor()
			);

			/// texture id のセット
			size_t textureIndex = resourceCollection_->GetTextureIndex(renderer->GetTexturePath());
			textureIdBuffer_->SetMappedData(
				transformIndex,
				textures[textureIndex]->GetSRVDescriptorIndex()
			);

			/// transform のセット
			transformBuffer_->SetMappedData(
				transformIndex, 
				renderer->GetOwner()->GetTransform()->GetMatWorld()
			);


			++transformIndex;
		}

		/// 上でセットしたデータをバインド
		materialBuffer->BindToCommandList(1, commandList);
		textureIdBuffer_->BindToCommandList(2, commandList);
		transformBuffer_->BindToCommandList(4, commandList);

		/// 現在のinstance idをセット
		commandList->SetGraphicsRoot32BitConstant(5, instanceIndex, 0);

		/// mesh の描画
		for (auto& mesh : model->GetMeshes()) {
			/// vbv, ibvのセット
			commandList->IASetVertexBuffers(0, 1, &mesh->GetVBV());
			commandList->IASetIndexBuffer(&mesh->GetIBV());

			/// 描画
			commandList->DrawIndexedInstanced(
				static_cast<UINT>(mesh->GetIndices().size()),
				static_cast<UINT>(renderers.size()), 
				0, 0, 0
			);
		}

		instanceIndex += static_cast<UINT>(renderers.size());
	}

}

