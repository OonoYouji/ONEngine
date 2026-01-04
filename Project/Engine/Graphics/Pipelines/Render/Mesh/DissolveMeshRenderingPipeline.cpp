#include "DissolveMeshRenderingPipeline.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/DissolveMeshRenderer.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"



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

	{	/// buffers
		sbufTransforms_.Create(kMaxRenderingMeshCount_, _dxm->GetDxDevice(), _dxm->GetDxSRVHeap());
		sbufMaterials_.Create(kMaxRenderingMeshCount_, _dxm->GetDxDevice(), _dxm->GetDxSRVHeap());
		sbufTextureIds_.Create(kMaxRenderingMeshCount_, _dxm->GetDxDevice(), _dxm->GetDxSRVHeap());
		sbufDissolveParams_.Create(kMaxRenderingMeshCount_, _dxm->GetDxDevice(), _dxm->GetDxSRVHeap());
	}

}

void DissolveMeshRenderingPipeline::Draw(ECSGroup* _ecsGroup, CameraComponent* _camera, DxCommand* _dxCommand) {

	ComponentArray<DissolveMeshRenderer>* dmrArray = _ecsGroup->GetComponentArray<DissolveMeshRenderer>();
	if(!CheckComponentArrayEnable(dmrArray)) {
		return;
	}

	using DMRList = std::list<DissolveMeshRenderer*>;
	std::unordered_map<Guid, DMRList> meshCompMap;
	for(auto& dmr : dmrArray->GetUsedComponents()) {
		if(!CheckComponentEnable(dmr)) {
			continue;
		}

		/// 無効なメッシュガイドの場合はスキップ
		if(dmr->GetMeshGuid() == Guid::kInvalid) {
			continue;
		}

		meshCompMap[dmr->GetMeshGuid()].emplace_back(dmr);
	}



	auto cmdList = _dxCommand->GetCommandList();
	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, CBV_VIEW_PROJECTION);

	const Texture& frontTexture = pAssetCollection_->GetTextures().front();
	cmdList->SetGraphicsRootDescriptorTable(SRV_TEXTURE, frontTexture.GetSRVGPUHandle());

	uint32_t transformIndex = 0;
	uint32_t instanceIndex_ = 0;

	for(const auto& [meshGuid, dmrList] : meshCompMap) {
		const Model* model = pAssetCollection_->GetAsset<Model>(meshGuid);
		if(!model) {
			continue;
		}

		for(const auto& dmr : dmrList) {

			const GPUMaterial& gpuMat = dmr->GetGPUMaterial(pAssetCollection_);
			sbufMaterials_.SetMappedData(transformIndex, gpuMat);
			sbufTextureIds_.SetMappedData(transformIndex, gpuMat.baseTextureId);
			sbufTransforms_.SetMappedData(
				transformIndex, dmr->GetOwner()->GetTransform()->GetMatWorld()
			);
			sbufDissolveParams_.SetMappedData(
				transformIndex,
				GPUDissolveParams{
					dmr->GetDissolveTextureId(pAssetCollection_),
					dmr->GetDissolveCompare(),
					dmr->GetDissolveThreshold()
				}
			);

			++transformIndex;
		}

		sbufMaterials_.SRVBindForGraphicsCommandList(cmdList, SRV_MATERIAL);
		sbufTextureIds_.SRVBindForGraphicsCommandList(cmdList, SRV_TEXTURE_ID);
		sbufTransforms_.SRVBindForGraphicsCommandList(cmdList, SRV_TRANSFORM);
		sbufDissolveParams_.SRVBindForGraphicsCommandList(cmdList, SRV_DISSOLVE_PARAMS);

		cmdList->SetGraphicsRoot32BitConstant(CBV_INSTANCE_OFFSET, instanceIndex_, 0);

		for(const auto& mesh : model->GetMeshes()) {
			cmdList->IASetVertexBuffers(0, 1, &mesh->GetVBV());
			cmdList->IASetIndexBuffer(&mesh->GetIBV());

			cmdList->DrawIndexedInstanced(
				static_cast<UINT>(mesh->GetIndices().size()),
				static_cast<UINT>(dmrList.size()),
				0, 0, 0
			);
		}

	}

}
