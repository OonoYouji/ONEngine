#include "TerrainRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"


TerrainRenderingPipeline::TerrainRenderingPipeline(AssetCollection* _assetCollection)
	: pAssetCollection_(_assetCollection) {}
TerrainRenderingPipeline::~TerrainRenderingPipeline() {}



void TerrainRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	{	/// shader

		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"Packages/Shader/Render/Terrain/Terrain.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"Packages/Shader/Render/Terrain/Terrain.ps.hlsl", L"ps_6_0", Shader::Type::ps);


		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		/// input element
		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
		pipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
		pipeline_->AddInputElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("INDEX", 0, DXGI_FORMAT_R32_SINT);


		/// buffer

		/// vs
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// ROOT_PARAM_VIEW_PROJECTION
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1); /// ROOT_PARAM_TRANSFORM

		/// ps
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// grass
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// dirt
		pipeline_->AddDescriptorRange(2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// rock
		pipeline_->AddDescriptorRange(3, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// snow
		pipeline_->AddDescriptorRange(4, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// ROOT_PARAM_MATERIAL

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0); /// textures
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1); /// textures
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 2); /// textures
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 3); /// textures
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 4); /// ROOT_PARAM_MATERIAL

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);



		/// setting
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_BACK);

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline_->SetDepthStencilDesc(depthStencilDesc);

		/// pipeline create
		pipeline_->CreatePipeline(_dxManager->GetDxDevice());
	}


	{	/// buffer
		transformBuffer_.Create(_dxManager->GetDxDevice());
		materialBuffer_.Create(1, _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());
	}

	pTerrain_ = nullptr;
}

void TerrainRenderingPipeline::Draw(class ECSGroup* _ecs, const std::vector<GameEntity*>&, CameraComponent* _camera, DxCommand* _dxCommand) {


	/// 地形を取得
	pTerrain_ = nullptr;
	ComponentArray<Terrain>* terrainArray = _ecs->GetComponentArray<Terrain>();
	if (!terrainArray) {
		return;
	}

	/// 一旦先頭にあるTerrainのみ描画する
	for (auto& terrain : terrainArray->GetUsedComponents()) {
		if (terrain->GetOwner()) {
			pTerrain_ = terrain;
		}
	}

	/// 見つかんなかったらreturn
	if (pTerrain_ == nullptr) {
		return;
	}

	/// terrainが生成されていないならreturn
	if (!pTerrain_->GetIsCreated()) {
		Console::LogInfo("TerrainRenderingPipeline::Draw: Terrain is not created");
		return;
	}

	if (!pTerrain_->enable) {
		Console::LogInfo("TerrainRenderingPipeline::Draw: Terrain is disabled");
		return;
	}

	/// value setting
	const Matrix4x4& matWorld = pTerrain_->GetOwner()->GetTransform()->GetMatWorld();
	transformBuffer_.SetMappedData(matWorld);

	GameEntity* entity = pTerrain_->GetOwner();
	/// bufferの値を更新
	transformBuffer_.SetMappedData(matWorld);
	materialBuffer_.SetMappedData( 
		0, 
		GPUMaterial{
			.uvTransform = UVTransform{ Vector2(0, 0), Vector2(100, 100), 0.0f },
			.baseColor = Vector4(1, 2, 3, 4),
			.postEffectFlags = 1,
			.entityId = entity->GetId(),
			.baseTextureId = 0,
			.normalTextureId = 0,
		}
	);


	/// 描画する
	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	auto cmdList = _dxCommand->GetCommandList();

	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, ROOT_PARAM_VIEW_PROJECTION);
	transformBuffer_.BindForGraphicsCommandList(cmdList, ROOT_PARAM_TRANSFORM);
	materialBuffer_.SRVBindForGraphicsCommandList(cmdList, ROOT_PARAM_MATERIAL);

	/// texs
	const auto& textures = pAssetCollection_->GetTextures();
	for (uint32_t i = 0; i < pTerrain_->GetSplatTexPaths().size(); i++) {
		const std::string& path = pTerrain_->GetSplatTexPaths()[i];
		size_t index = pAssetCollection_->GetTextureIndex(path);
		cmdList->SetGraphicsRootDescriptorTable(
			static_cast<UINT>(ROOT_PARAM_TEX_GRASS + i),
			textures[index].GetSRVGPUHandle()
		);
	}


	/// vbvとibvのリソースバリアーを変える
	DxResource& vertexBuffer = pTerrain_->GetRwVertices().GetResource();
	vertexBuffer.CreateBarrier(
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		_dxCommand
	);

	/// vbv setting
	D3D12_VERTEX_BUFFER_VIEW vbv = {};
	vbv.BufferLocation = vertexBuffer.Get()->GetGPUVirtualAddress();
	vbv.StrideInBytes = sizeof(TerrainVertex);
	vbv.SizeInBytes = sizeof(TerrainVertex) * pTerrain_->GetMaxVertexNum();
	cmdList->IASetVertexBuffers(0, 1, &vbv);


	DxResource& indexBuffer = pTerrain_->GetRwIndices().GetResource();
	indexBuffer.CreateBarrier(
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_INDEX_BUFFER,
		_dxCommand
	);

	/// ibv setting
	D3D12_INDEX_BUFFER_VIEW ibv = {};
	ibv.BufferLocation = indexBuffer.Get()->GetGPUVirtualAddress();
	ibv.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * pTerrain_->GetMaxIndexNum());
	ibv.Format = DXGI_FORMAT_R32_UINT;
	cmdList->IASetIndexBuffer(&ibv);

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Console::LogInfo("TerrainRenderingPipeline::Draw");
	cmdList->DrawIndexedInstanced(
		static_cast<UINT>(pTerrain_->GetMaxIndexNum()),
		1, 0, 0, 0
	);


	/// 元の状態に戻す
	vertexBuffer.CreateBarrier(
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		_dxCommand
	);
	/// 元の状態に戻す
	indexBuffer.CreateBarrier(
		D3D12_RESOURCE_STATE_INDEX_BUFFER,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		_dxCommand
	);
}



