#include "SkinMeshRenderingPipeline.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/DirectX12/Command/DxCommand.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/RendererComponents/SkinMesh/SkinMeshRenderer.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/Asset/Collection/AssetCollection.h"


SkinMeshRenderingPipeline::SkinMeshRenderingPipeline(Asset::AssetCollection* _assetCollection)
	: pAssetCollection_(_assetCollection) {
}

void SkinMeshRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	{
		/// pipeline create

		/// shader compile
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Render/Mesh/SkinMesh.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"./Packages/Shader/Render/Mesh/SkinMesh.ps.hlsl", L"ps_6_0", Shader::Type::ps);


		pipeline_ = std::make_unique<GraphicsPipeline>();

		pipeline_->SetShader(&shader);

		/*	SkinMeshの頂点構造体
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
		pipeline_->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1);
		pipeline_->AddInputElement("INDEX", 0, DXGI_FORMAT_R32G32B32A32_SINT, 1);


		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0);       /// 0: ViewProjection (b0)
		pipeline_->Add32BitConstant(D3D12_SHADER_VISIBILITY_ALL, 1, 1); /// 1: InstanceIndex (b1)

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);    /// Range 0: InstanceData (t0)
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);    /// Range 1: WellForGPU (t1)
		pipeline_->AddDescriptorRange(2, 2048, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// Range 2: Texture (t2)

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0);    /// 2: InstanceData
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 1); /// 3: WellForGPU
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 2);  /// 4: Texture

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);



		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		pipeline_->SetDepthStencilDesc(DefaultDepthStencilDesc());

		pipeline_->SetBlendDesc(BlendMode::Normal());

		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}



	{
		/// Buffer
		instanceDataBuffer_.Create(static_cast<uint32_t>(kMaxInstances), _dxm->GetDxDevice(), _dxm->GetDxSRVHeap());
		instanceDataCPU_.resize(kMaxInstances);
	}


}

void SkinMeshRenderingPipeline::Draw(class ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {

	ComponentArray<SkinMeshRenderer>* skinMeshArray = _ecs->GetComponentArray<SkinMeshRenderer>();
	if (!skinMeshArray || skinMeshArray->GetUsedComponents().empty()) {
		return;
	}


	ID3D12GraphicsCommandList* cmdList = _dxCommand->GetCommandList();
	auto& textures = pAssetCollection_->GetTextures();

	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// 1. インスタンスデータの集約と転送
	size_t instanceCount = 0;
	std::vector<SkinMeshRenderer*> activeRenderers;

	for (auto& smRenderer : skinMeshArray->GetUsedComponents()) {
		if (instanceCount >= kMaxInstances) break;

		if (!smRenderer || !smRenderer->enable || !smRenderer->skinCluster_ || !smRenderer->GetOwner()->active) {
			continue;
		}

		/// StructuredBufferに書き込み
		SkinMeshInstanceData data;
		data.matWorld = smRenderer->GetOwner()->GetTransform()->matWorld;
		data.material = GPUMaterial{
			.uvTransform = { .scale = Vector2::One },
			.baseColor = smRenderer->GetColor(),
			.postEffectFlags = 1,
			.entityId = smRenderer->GetOwner()->GetId(),
			.baseTextureId = (int32_t)textures[pAssetCollection_->GetTextureIndex(smRenderer->GetTexturePath())].GetSRVDescriptorIndex()
		};

		instanceDataBuffer_.SetMappedData(instanceCount, data);

		activeRenderers.push_back(smRenderer);
		instanceCount++;
	}

	if (instanceCount == 0) return;


	/// 2. 描画
	/// ViewProjection Bind
	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, ViewProjectionCBV);
	
	/// InstanceData Buffer Bind
	instanceDataBuffer_.SRVBindForGraphicsCommandList(cmdList, InstanceDataSRV);

	/// Texture Table Bind
	cmdList->SetGraphicsRootDescriptorTable(TextureSRV, (*textures.begin()).GetSRVGPUHandle());


	for (size_t i = 0; i < activeRenderers.size(); i++) {
		SkinMeshRenderer* smRenderer = activeRenderers[i];

		/// 現在のインスタンスインデックスを渡す
		uint32_t index = static_cast<uint32_t>(i);
		cmdList->SetGraphicsRoot32BitConstants(InstanceIndexCBV, 1, &index, 0);

		/// モデルごとのボーンパレットをバインド
		cmdList->SetGraphicsRootDescriptorTable(WellForGPUSRV, smRenderer->skinCluster_->paletteSRVHandle.second);

		/// mesh の描画
		Asset::Model* model = pAssetCollection_->GetModel(smRenderer->GetMeshPath());
		for (auto& mesh : model->GetMeshes()) {
			/// vbv, ibvのセット
			D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
				mesh->GetVBV(), smRenderer->skinCluster_->vbv
			};
			cmdList->IASetVertexBuffers(0, 2, vbvs);
			cmdList->IASetIndexBuffer(&mesh->GetIBV());

			/// 描画
			cmdList->DrawIndexedInstanced(
				static_cast<UINT>(mesh->GetIndices().size()),
				1, 0, 0, 0
			);
		}
	}

}
