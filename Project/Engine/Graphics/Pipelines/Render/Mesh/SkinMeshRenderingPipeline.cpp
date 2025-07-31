#include "SkinMeshRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/DirectX12/Command/DxCommand.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/RendererComponents/SkinMesh/SkinMeshRenderer.h"
#include "Engine/ECS/Entity/Entities/Camera/Camera.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"


SkinMeshRenderingPipeline::SkinMeshRenderingPipeline(GraphicsResourceCollection* _graphicsResourceCollection)
	: pGraphicsResourceCollection_(_graphicsResourceCollection) {}

void SkinMeshRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	{
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Render/Mesh/SkinMesh.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"./Packages/Shader/Render/Mesh/SkinMesh.ps.hlsl", L"ps_6_0", Shader::Type::ps);


		pipeline_ = std::make_unique<GraphicsPipeline>();

		pipeline_->SetShader(&shader);

		/*
			struct VSInput {
				float4 position : SV_POSITION;
				float3 normal : NORMAL;
				float2 uv : TEXCOORD0;
				float4 weight : WEIGHT0;
				int4 index : INDEX0;
			};
		*/

		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
		pipeline_->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
		pipeline_->AddInputElement("WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1);
		pipeline_->AddInputElement("INDEX", 0, DXGI_FORMAT_R32G32B32A32_SINT, 1);


		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// 0: ViewProjection
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1); /// 1: Transform
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);  /// 2: Material
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 1);  /// 3: TextureId

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// WellForGPU
		pipeline_->AddDescriptorRange(0, 32, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// Texture

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// 4: WellForGPU
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1);  /// 5: Texture

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);



		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_BACK);
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline_->SetDepthStencilDesc(depthStencilDesc);

		pipeline_->SetBlendDesc(BlendMode::Normal());

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());
	}



	{
		/// Buffer

		transformBuffer_ = std::make_unique<ConstantBuffer<Matrix4x4>>();
		materialBuffer_ = std::make_unique<ConstantBuffer<Vector4>>();
		textureIdBuffer_ = std::make_unique<ConstantBuffer<uint32_t>>();

		transformBuffer_->Create(_dxManager->GetDxDevice());
		materialBuffer_->Create(_dxManager->GetDxDevice());
		textureIdBuffer_->Create(_dxManager->GetDxDevice());

	}


}

void SkinMeshRenderingPipeline::Draw(class EntityComponentSystem* _ecs, const std::vector<IEntity*>& _entities, Camera* _camera, DxCommand* _dxCommand) {

	std::vector<SkinMeshRenderer*> skinMeshRenderers;
	for (auto& entity : _entities) {
		SkinMeshRenderer* skinMesh = entity->GetComponent<SkinMeshRenderer>();
		if (skinMesh && skinMesh->enable) {
			skinMeshRenderers.push_back(skinMesh);
		}
	}

	if (skinMeshRenderers.empty()) {
		return; ///< 描画するスキンメッシュがない場合は何もしない
	}


	ID3D12GraphicsCommandList* commandList = _dxCommand->GetCommandList();

	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// ViewProjection Bind
	_camera->GetViewProjectionBuffer()->BindForGraphicsCommandList(commandList, ViewProjectionCBV);
	/// Textures Bind
	auto& textures = pGraphicsResourceCollection_->GetTextures();
	commandList->SetGraphicsRootDescriptorTable(TextureSRV, (*textures.begin())->GetSRVGPUHandle()); ///< Texture


	/// インスタンスごとの設定
	for (auto& comp : skinMeshRenderers) {
		if (!comp || !comp->enable) {
			continue; ///< 無効なコンポーネントはスキップ
		}

		if (!comp->skinCluster_) {
			continue; ///< スキンクラスターが存在しない場合はスキップ
		}

		IEntity* entity = comp->GetOwner();
		if (!entity) {
			continue; /// エンティティが無効な場合はスキップ
		}

		/// Transform Bind
		transformBuffer_->SetMappedData(entity->GetTransform()->GetMatWorld());

		/// Material Bind
		materialBuffer_->SetMappedData(comp->GetColor());

		/// TextureId Bind
		size_t textureIndex = pGraphicsResourceCollection_->GetTextureIndex(comp->GetTexturePath());
		textureIdBuffer_->SetMappedData(textures[textureIndex]->GetSRVDescriptorIndex());

		transformBuffer_->BindForGraphicsCommandList(commandList, TransformCBV);
		materialBuffer_->BindForGraphicsCommandList(commandList, MaterialCBV);
		commandList->SetGraphicsRootDescriptorTable(WellForGPUSRV, comp->skinCluster_->paletteSRVHandle.second);
		textureIdBuffer_->BindForGraphicsCommandList(commandList, TextureIdCBV);


		/// mesh の描画
		Model* model = pGraphicsResourceCollection_->GetModel(comp->GetMeshPath());
		for (auto& mesh : model->GetMeshes()) {
			/// vbv, ibvのセット
			D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
				mesh->GetVBV(), comp->skinCluster_->vbv
			};
			commandList->IASetVertexBuffers(0, 2, vbvs);
			commandList->IASetIndexBuffer(&mesh->GetIBV());

			/// 描画
			commandList->DrawIndexedInstanced(
				static_cast<UINT>(mesh->GetIndices().size()),
				1, 0, 0, 0
			);
		}


	}

}
