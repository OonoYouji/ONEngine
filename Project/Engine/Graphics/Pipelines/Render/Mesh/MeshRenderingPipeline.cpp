#include "MeshRenderingPipeline.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Transform/Transform.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/CustomMeshRenderer.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"


MeshRenderingPipeline::MeshRenderingPipeline(AssetCollection* _assetCollection)
	: pAssetCollection_(_assetCollection) {
}

MeshRenderingPipeline::~MeshRenderingPipeline() {}

void MeshRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	{	/// pipeline create

		/// shader compile
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Render/Mesh/Mesh.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"./Packages/Shader/Render/Mesh/Mesh.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
		pipeline_->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_BACK);

		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); ///< view projection: 0

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< material
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< textureId
		pipeline_->AddDescriptorRange(2, MAX_TEXTURE_COUNT, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); ///< texture
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< transform
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);       ///< material  : 1
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1);       ///< textureId : 2
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 2);       ///< texture   : 3
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 3);      ///< transform : 4

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);         ///< texture sampler

		pipeline_->Add32BitConstant(D3D12_SHADER_VISIBILITY_VERTEX, 1);        ///< instance id: 5


		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetDepthStencilDesc(DefaultDepthStencilDesc());


		pipeline_->CreatePipeline(_dxm->GetDxDevice());

	}


	{	/// buffer create

		transformBuffer_.Create(static_cast<uint32_t>(kMaxRenderingMeshCount_), _dxm->GetDxDevice(), _dxm->GetDxSRVHeap());
		materialBuffer_.Create(static_cast<uint32_t>(kMaxRenderingMeshCount_), _dxm->GetDxDevice(), _dxm->GetDxSRVHeap());
		textureIdBuffer_.Create(static_cast<uint32_t>(kMaxRenderingMeshCount_), _dxm->GetDxDevice(), _dxm->GetDxSRVHeap());

	}

}

void MeshRenderingPipeline::Draw(class ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {

	/// MeshRendererの取得＆存在チェック
	ComponentArray<MeshRenderer>* meshRendererArray = _ecs->GetComponentArray<MeshRenderer>();
	if (!meshRendererArray || meshRendererArray->GetUsedComponents().empty()) {
		return;
	}

	/// mesh path ごとに mesh renderer を分類
	std::unordered_map<std::string, std::list<MeshRenderer*>> pathMeshMap;
	for (const auto& meshRenderer : meshRendererArray->GetUsedComponents()) {
		if(!CheckComponentEnable(meshRenderer)) {
			continue;
		}

		/// meshが読み込まれていなければ、デフォルトのメッシュを使用
		if (!pAssetCollection_->GetModel(meshRenderer->GetMeshPath())) {
			Console::Log("Mesh not found: " + meshRenderer->GetMeshPath());
			pathMeshMap["./Assets/Models/primitive/cube.obj"].push_back(meshRenderer);
			continue;
		}

		pathMeshMap[meshRenderer->GetMeshPath()].push_back(meshRenderer);
	}


	auto cmdList = _dxCommand->GetCommandList();

	/// ----- CommandListに必要な設定を行う ----- ///

	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// Bufferのバインド
	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, 0);
	auto& textures = pAssetCollection_->GetTextures();
	cmdList->SetGraphicsRootDescriptorTable(3, (*textures.begin()).GetSRVGPUHandle());

	transformIndex_ = 0;
	instanceIndex_ = 0;

	RenderingMesh(cmdList, &pathMeshMap, textures);
}

void MeshRenderingPipeline::RenderingMesh(ID3D12GraphicsCommandList* _cmdList, std::unordered_map<std::string, std::list<MeshRenderer*>>* _meshRendererPerMesh, const std::vector<Texture>& _textures) {
	for (auto& [meshPath, renderers] : (*_meshRendererPerMesh)) {

		/// modelの取得、なければ次へ
		const Model*&& model = pAssetCollection_->GetModel(meshPath);
		if (!model) {
			continue;
		}

		/// transform, material を mapping
		for (auto& renderer : renderers) {

			/// TextureのIdをGuidからセット
			renderer->SetupRenderData(pAssetCollection_);

			materialBuffer_.SetMappedData(
				transformIndex_,
				renderer->GetGpuMaterial()
			);

			textureIdBuffer_.SetMappedData(
				transformIndex_,
				_textures[renderer->GetGpuMaterial().baseTextureId].GetSRVDescriptorIndex()
			);

			/// transform のセット
			transformBuffer_.SetMappedData(
				transformIndex_,
				renderer->GetOwner()->GetTransform()->GetMatWorld()
			);


			++transformIndex_;
		}

		/// 上でセットしたデータをバインド
		materialBuffer_.SRVBindForGraphicsCommandList(_cmdList, 1);
		textureIdBuffer_.SRVBindForGraphicsCommandList(_cmdList, 2);
		transformBuffer_.SRVBindForGraphicsCommandList(_cmdList, 4);

		/// 現在のinstance idをセット
		_cmdList->SetGraphicsRoot32BitConstant(5, instanceIndex_, 0);

		/// mesh の描画
		for (auto& mesh : model->GetMeshes()) {
			/// vbv, ibvのセット
			_cmdList->IASetVertexBuffers(0, 1, &mesh->GetVBV());
			_cmdList->IASetIndexBuffer(&mesh->GetIBV());

			/// 描画
			_cmdList->DrawIndexedInstanced(
				static_cast<UINT>(mesh->GetIndices().size()),
				static_cast<UINT>(renderers.size()),
				0, 0, 0
			);
		}

		instanceIndex_ += static_cast<UINT>(renderers.size());
	}
}
