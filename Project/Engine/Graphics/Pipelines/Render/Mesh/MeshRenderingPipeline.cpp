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
#include "Engine/Core/DirectX12/GPUTimeStamp/GPUTimeStamp.h"


MeshRenderingPipeline::MeshRenderingPipeline(Asset::AssetCollection* _assetCollection)
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
		pipeline_->AddDescriptorRange(2, Asset::MAX_TEXTURE_COUNT, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); ///< texture
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

		// --- Telegraph専用パイプラインの生成 ---
		telegraphPipeline_ = std::make_unique<GraphicsPipeline>();
		telegraphPipeline_->SetShader(&shader);
		telegraphPipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		telegraphPipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
		telegraphPipeline_->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
		telegraphPipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		telegraphPipeline_->SetCullMode(D3D12_CULL_MODE_BACK);
		telegraphPipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		telegraphPipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);
		telegraphPipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		telegraphPipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		telegraphPipeline_->AddDescriptorRange(2, Asset::MAX_TEXTURE_COUNT, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		telegraphPipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		telegraphPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);
		telegraphPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1);
		telegraphPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 2);
		telegraphPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 3);
		telegraphPipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);
		telegraphPipeline_->Add32BitConstant(D3D12_SHADER_VISIBILITY_VERTEX, 1);
		
		telegraphPipeline_->SetBlendDesc(BlendMode::Normal());
		telegraphPipeline_->SetDepthStencilDesc(TelegraphDepthStencilDesc()); // Z-Test Always, Z-Write Off
		
		telegraphPipeline_->CreatePipeline(_dxm->GetDxDevice());
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

	GPUTimeStamp::GetInstance().BeginTimeStamp(GPUTimeStampID::MeshRendering);

	/// レイヤーごとに分類
	std::unordered_map<RenderQueue, std::unordered_map<std::string, std::list<MeshRenderer*>>> queueMap;
	for (const auto& meshRenderer : meshRendererArray->GetUsedComponents()) {
		if(!CheckComponentEnable(meshRenderer)) {
			continue;
		}

		RenderQueue queue = meshRenderer->GetRenderQueue();
		std::string meshPath = meshRenderer->GetMeshPath();

		/// meshが読み込まれていなければ、デフォルトのメッシュを使用
		if (!pAssetCollection_->GetModel(meshPath)) {
			meshPath = "./Assets/Models/primitive/cube.obj";
		}
		queueMap[queue][meshPath].push_back(meshRenderer);
	}


	auto cmdList = _dxCommand->GetCommandList();

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	auto& textures = pAssetCollection_->GetTextures();

	transformIndex_ = 0;
	instanceIndex_ = 0;

	/// 1. Backgroundの描画
	if (queueMap.count(RenderQueue::Background)) {
		pipeline_->SetPipelineStateForCommandList(_dxCommand);
		_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, 0);
		cmdList->SetGraphicsRootDescriptorTable(3, (*textures.begin()).GetSRVGPUHandle());
		Drawing(cmdList, queueMap[RenderQueue::Background], textures);
	}

	/// 2. Telegraphの描画 (専用のZ-Test無視パイプライン)
	if (queueMap.count(RenderQueue::Telegraph)) {
		telegraphPipeline_->SetPipelineStateForCommandList(_dxCommand);
		_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, 0);
		cmdList->SetGraphicsRootDescriptorTable(3, (*textures.begin()).GetSRVGPUHandle());
		Drawing(cmdList, queueMap[RenderQueue::Telegraph], textures);
	}

	/// 3. Defaultの描画 (通常のパイプライン)
	if (queueMap.count(RenderQueue::Default)) {
		pipeline_->SetPipelineStateForCommandList(_dxCommand);
		_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, 0);
		cmdList->SetGraphicsRootDescriptorTable(3, (*textures.begin()).GetSRVGPUHandle());
		Drawing(cmdList, queueMap[RenderQueue::Default], textures);
	}


	GPUTimeStamp::GetInstance().EndTimeStamp(GPUTimeStampID::MeshRendering);
}

void MeshRenderingPipeline::Drawing(ID3D12GraphicsCommandList* _cmdList, std::unordered_map<std::string, std::list<MeshRenderer*>>& _pathMeshMap, const std::vector<Asset::Texture>& _textures) {
	for (auto& [meshPath, renderers] : _pathMeshMap) {

		/// modelの取得、なければ次へ
		const Asset::Model*&& model = pAssetCollection_->GetModel(meshPath);
		if (!model) {
			continue;
		}

		/// transform, material を mapping
		for (auto& renderer : renderers) {
			/// TextureのIdをGuidからセット
			renderer->SetupRenderData(pAssetCollection_);

			uint32_t id = renderer->GetGpuMaterial().baseTextureId;
			if(id < 0 || id >= _textures.size()) {
				continue;
			}

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

