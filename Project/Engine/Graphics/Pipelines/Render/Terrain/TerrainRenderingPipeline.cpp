#include "TerrainRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"


TerrainRenderingPipeline::TerrainRenderingPipeline(GraphicsResourceCollection* _resourceCollection)
	: pResourceCollection_(_resourceCollection) {}
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


		/// buffer

		/// vs
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// ROOT_PARAM_VIEW_PROJECTION
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1); /// ROOT_PARAM_TRANSFORM

		/// ps
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0); /// ROOT_PARAM_MATERIAL
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// grass
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// dirt
		pipeline_->AddDescriptorRange(2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// rock
		pipeline_->AddDescriptorRange(3, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// snow

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0); /// textures
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1); /// textures
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 2); /// textures
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 3); /// textures

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);



		/// setting
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline_->SetDepthStencilDesc(depthStencilDesc);

		/// pipeline create
		pipeline_->CreatePipeline(_dxManager->GetDxDevice());
	}


	{ /// buffer

		transformBuffer_.Create(_dxManager->GetDxDevice());
		materialBuffer_.Create(_dxManager->GetDxDevice());
		vertexBuffer_.Create(1000 * 1000, _dxManager->GetDxDevice());
		indexBuffer_.Create(999 * 1000 * 6, _dxManager->GetDxDevice());
	}

	pTerrain_ = nullptr;

}

void TerrainRenderingPipeline::Draw(class EntityComponentSystem* _ecs, const std::vector<IEntity*>& _entities, CameraComponent* _camera, DxCommand* _dxCommand) {

	/// 地形を取得
	Terrain* prevTerrain_ = pTerrain_;
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

	/// prevと違うterrainならmapする
	if (prevTerrain_ != pTerrain_) {
		indexBuffer_.SetIndices(pTerrain_->GetIndices());
		vertexBuffer_.SetVertices(pTerrain_->GetVertices());

		indexBuffer_.Map();
		vertexBuffer_.Map();
	}


	/// value setting
	const Matrix4x4& matWorld = pTerrain_->GetOwner()->GetTransform()->GetMatWorld();
	transformBuffer_.SetMappedData(matWorld);

	/// 編集した頂点を更新する
	if (!pTerrain_->GetEditVertices().empty()) {
		for (auto& editV : pTerrain_->GetEditVertices()) {
			vertexBuffer_.SetVertex(editV.first, *editV.second);
		}
		//pTerrain_->ClearEditVertices();
	}


	/// bufferの値を更新
	transformBuffer_.SetMappedData(matWorld);
	materialBuffer_.SetMappedData(
		Material(Vector4::kWhite, 1, pTerrain_->GetOwner()->GetId())
	);


	/// 描画する
	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	auto command = _dxCommand->GetCommandList();

	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(command, ROOT_PARAM_VIEW_PROJECTION);
	transformBuffer_.BindForGraphicsCommandList(command, ROOT_PARAM_TRANSFORM);
	materialBuffer_.BindForGraphicsCommandList(command, ROOT_PARAM_MATERIAL);

	/// texs
	const auto& textures = pResourceCollection_->GetTextures();

	for (uint32_t i = 0; i < pTerrain_->GetSplatTexPaths().size(); i++) {
		const std::string& path = pTerrain_->GetSplatTexPaths()[i];
		size_t index = pResourceCollection_->GetTextureIndex(path);
		command->SetGraphicsRootDescriptorTable(
			static_cast<UINT>(ROOT_PARAM_TEX_GRASS + i),
			textures[index]->GetSRVGPUHandle()
		);
	}



	/// vbv ibv setting
	vertexBuffer_.BindForCommandList(command);
	indexBuffer_.BindForCommandList(command);

	command->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	command->DrawIndexedInstanced(
		static_cast<UINT>(indexBuffer_.GetIndices().size()),
		1, 0, 0, 0
	);


}



