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


		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// 0: ViewProjection
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1); /// 1: Transform
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);  /// 2: Material
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 1);  /// 3: TextureId

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);    /// Range 0: WellForGPU (t0)
		pipeline_->AddDescriptorRange(0, 2048, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// Range 1: Texture (t0)

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// 4: WellForGPU
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1);  /// 5: Texture

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);



		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_BACK);
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		pipeline_->SetDepthStencilDesc(DefaultDepthStencilDesc());

		pipeline_->SetBlendDesc(BlendMode::Normal());

		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}



	{
		/// Buffer
		for (size_t i = 0; i < kMaxInstances; i++) {
			transformBuffers_.push_back(std::make_unique<ConstantBuffer<Matrix4x4>>());
			materialBuffers_.push_back(std::make_unique<ConstantBuffer<GPUMaterial>>());
			textureIdBuffers_.push_back(std::make_unique<ConstantBuffer<uint32_t>>());

			transformBuffers_.back()->Create(_dxm->GetDxDevice());
			materialBuffers_.back()->Create(_dxm->GetDxDevice());
			textureIdBuffers_.back()->Create(_dxm->GetDxDevice());
		}
	}


}

void SkinMeshRenderingPipeline::Draw(class ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {

	ComponentArray<SkinMeshRenderer>* skinMeshArray = _ecs->GetComponentArray<SkinMeshRenderer>();
	if (!skinMeshArray || skinMeshArray->GetUsedComponents().empty()) {
		return;
	}


	ID3D12GraphicsCommandList* cmdList = _dxCommand->GetCommandList();

	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto& textures = pAssetCollection_->GetTextures();

	/// インスタンスごとの設定
	size_t instanceIndex = 0;
	for (auto& smRenderer : skinMeshArray->GetUsedComponents()) {
		if (instanceIndex >= kMaxInstances) {
			break; ///< 最大描画数を超えた場合は終了
		}

		if (!smRenderer || !smRenderer->enable) {
			continue; ///< 無効なコンポーネントはスキップ
		}

		if (!smRenderer->skinCluster_) {
			continue; ///< スキンクラスターが存在しない場合はスキップ
		}

		GameEntity* entity = smRenderer->GetOwner();
		if (!entity || !entity->active) {
			continue; /// エンティティが無効な場合はスキップ
		}

		/// ViewProjection Bind (念のためループ内でバインド)
		_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, ViewProjectionCBV);

		/// Transform Bind
		transformBuffers_[instanceIndex]->SetMappedData(entity->GetTransform()->GetMatWorld());

		/// Material Bind
		materialBuffers_[instanceIndex]->SetMappedData(
			GPUMaterial{
				.baseColor = smRenderer->GetColor(),
				.postEffectFlags = 1,
				.entityId = smRenderer->GetOwner()->GetId()
			}
		);

		/// TextureId Bind
		size_t textureIndex = pAssetCollection_->GetTextureIndex(smRenderer->GetTexturePath());
		textureIdBuffers_[instanceIndex]->SetMappedData(textures[textureIndex].GetSRVDescriptorIndex());

		transformBuffers_[instanceIndex]->BindForGraphicsCommandList(cmdList, TransformCBV);
		materialBuffers_[instanceIndex]->BindForGraphicsCommandList(cmdList, MaterialCBV);
		cmdList->SetGraphicsRootDescriptorTable(WellForGPUSRV, smRenderer->skinCluster_->paletteSRVHandle.second);
		textureIdBuffers_[instanceIndex]->BindForGraphicsCommandList(cmdList, TextureIdCBV);
		cmdList->SetGraphicsRootDescriptorTable(TextureSRV, (*textures.begin()).GetSRVGPUHandle()); ///< Texture (ループ内でバインド)


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

		instanceIndex++;
	}

}
