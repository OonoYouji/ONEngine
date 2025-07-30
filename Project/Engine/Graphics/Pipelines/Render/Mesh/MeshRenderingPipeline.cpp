#include "MeshRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/Entities/Camera/Camera.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Transform/Transform.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/CustomMeshRenderer.h"


MeshRenderingPipeline::MeshRenderingPipeline(GraphicsResourceCollection* _resourceCollection)
	: resourceCollection_(_resourceCollection) {}

MeshRenderingPipeline::~MeshRenderingPipeline() {}

void MeshRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

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
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);

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


		pipeline_->SetBlendDesc(BlendMode::Normal());

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline_->SetDepthStencilDesc(depthStencilDesc);


		pipeline_->CreatePipeline(_dxManager->GetDxDevice());

	}


	{	/// buffer create

		transformBuffer_ = std::make_unique<StructuredBuffer<Matrix4x4>>();
		transformBuffer_->Create(static_cast<uint32_t>(kMaxRenderingMeshCount_), _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());

		materialBuffer = std::make_unique<StructuredBuffer<Material>>();
		materialBuffer->Create(static_cast<uint32_t>(kMaxRenderingMeshCount_), _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());

		textureIdBuffer_ = std::make_unique<StructuredBuffer<uint32_t>>();
		textureIdBuffer_->Create(static_cast<uint32_t>(kMaxRenderingMeshCount_), _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());

	}

}

void MeshRenderingPipeline::Draw(const std::vector<IEntity*>& _entities, Camera* _camera, DxCommand* _dxCommand) {

	/// mesh と transform の対応付け
	std::unordered_map<std::string, std::list<MeshRenderer*>> rendererPerMesh;
	std::list<CustomMeshRenderer*> customRenderers;
	for (auto& entity : _entities) {
		MeshRenderer*&& meshRenderer = entity->GetComponent<MeshRenderer>();

		if (meshRenderer && meshRenderer->enable) {

			/// meshが読み込まれていなければ、デフォルトのメッシュを使用
			if (!resourceCollection_->GetModel(meshRenderer->GetMeshPath())) {
				Console::Log("Mesh not found: " + meshRenderer->GetMeshPath());
				rendererPerMesh["./Assets/Models/primitive/cube.obj"].push_back(meshRenderer);
				continue; ///< meshが読み込まれていなければスキップ
			}

			rendererPerMesh[meshRenderer->GetMeshPath()].push_back(meshRenderer);
		}

		CustomMeshRenderer*&& customMeshRenderer = entity->GetComponent<CustomMeshRenderer>();
		if (customMeshRenderer && customMeshRenderer->enable) {
			customRenderers.push_back(customMeshRenderer);
		}
	}

	///< 描画対象がなければ 早期リターン
	if (rendererPerMesh.empty() && customRenderers.empty()) {
		return;
	}


	ID3D12GraphicsCommandList* commandList = _dxCommand->GetCommandList();

	/// settings
	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_camera->GetViewProjectionBuffer()->BindForGraphicsCommandList(commandList, 0);

	/// buffer dataのセット、先頭の texture gpu handle をセットする
	auto& textures = resourceCollection_->GetTextures();
	commandList->SetGraphicsRootDescriptorTable(3, (*textures.begin())->GetSRVGPUHandle());

	transformIndex_ = 0;
	instanceIndex_ = 0;
	/// alphaを考慮してカスタムメッシュを先に描画
	RenderingMesh(commandList, &customRenderers, textures); 
	RenderingMesh(commandList, &rendererPerMesh, textures);
}

void MeshRenderingPipeline::RenderingMesh(ID3D12GraphicsCommandList* _commandList, std::unordered_map<std::string, std::list<MeshRenderer*>>* _meshRendererPerMesh, const std::vector<std::unique_ptr<Texture>>& _pTexture) {

	for (auto& [meshPath, renderers] : (*_meshRendererPerMesh)) {

		/// modelの取得、なければ次へ
		const Model*&& model = resourceCollection_->GetModel(meshPath);
		if (!model) {
			continue;
		}

		/// transform, material を mapping
		for (auto& renderer : renderers) {

			/// materialのセット
			renderer->SetMaterialEntityId();
			materialBuffer->SetMappedData(
				transformIndex_,
				renderer->GetMaterial()
			);

			/// texture id のセット
			size_t textureIndex = resourceCollection_->GetTextureIndex(renderer->GetTexturePath());
			textureIdBuffer_->SetMappedData(
				transformIndex_,
				_pTexture[textureIndex]->GetSRVDescriptorIndex()
			);

			/// transform のセット
			transformBuffer_->SetMappedData(
				transformIndex_,
				renderer->GetOwner()->GetTransform()->GetMatWorld()
			);


			++transformIndex_;
		}

		/// 上でセットしたデータをバインド
		materialBuffer->BindToCommandList(1, _commandList);
		textureIdBuffer_->BindToCommandList(2, _commandList);
		transformBuffer_->BindToCommandList(4, _commandList);

		/// 現在のinstance idをセット
		_commandList->SetGraphicsRoot32BitConstant(5, instanceIndex_, 0);

		/// mesh の描画
		for (auto& mesh : model->GetMeshes()) {
			/// vbv, ibvのセット
			_commandList->IASetVertexBuffers(0, 1, &mesh->GetVBV());
			_commandList->IASetIndexBuffer(&mesh->GetIBV());

			/// 描画
			_commandList->DrawIndexedInstanced(
				static_cast<UINT>(mesh->GetIndices().size()),
				static_cast<UINT>(renderers.size()),
				0, 0, 0
			);
		}

		instanceIndex_ += static_cast<UINT>(renderers.size());
	}
}


void MeshRenderingPipeline::RenderingMesh(ID3D12GraphicsCommandList* _commandList, std::list<CustomMeshRenderer*>* _pCustomRenderers, const std::vector<std::unique_ptr<Texture>>& _pTexture) {
	for (auto& renderer : (*_pCustomRenderers)) {

		/// modelの取得、なければ次へ
		const Mesh* mesh = renderer->GetMesh();
		if (!mesh) {
			continue;
		}

		/// materialのセット
		materialBuffer->SetMappedData(
			transformIndex_,
			renderer->GetMaterial()
		);

		/// texture id のセット
		size_t textureIndex = resourceCollection_->GetTextureIndex(renderer->GetTexturePath());
		textureIdBuffer_->SetMappedData(
			transformIndex_,
			_pTexture[textureIndex]->GetSRVDescriptorIndex()
		);

		/// transform のセット
		transformBuffer_->SetMappedData(
			transformIndex_,
			renderer->GetOwner()->GetTransform()->GetMatWorld()
		);


		++transformIndex_;

		/// 上でセットしたデータをバインド
		materialBuffer->BindToCommandList(1, _commandList);
		textureIdBuffer_->BindToCommandList(2, _commandList);
		transformBuffer_->BindToCommandList(4, _commandList);

		/// 現在のinstance idをセット
		_commandList->SetGraphicsRoot32BitConstant(5, instanceIndex_, 0);

		/// vbv, ibvのセット
		_commandList->IASetVertexBuffers(0, 1, &mesh->GetVBV());
		_commandList->IASetIndexBuffer(&mesh->GetIBV());

		/// 描画
		_commandList->DrawIndexedInstanced(
			static_cast<UINT>(mesh->GetIndices().size()),
			1, 0, 0, 0
		);

		++instanceIndex_;
	}
}

