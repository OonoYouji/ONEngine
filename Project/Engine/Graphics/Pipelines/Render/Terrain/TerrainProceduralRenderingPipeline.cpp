#include "TerrainProceduralRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"

TerrainProceduralRenderingPipeline::TerrainProceduralRenderingPipeline(GraphicsResourceCollection* _resourceCollection)
	: pResourceCollection_(_resourceCollection) {}
TerrainProceduralRenderingPipeline::~TerrainProceduralRenderingPipeline() {}

void TerrainProceduralRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	pDxManager_ = _dxManager;

	{	/// compute pipeline

		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Editor/Procedural.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		computePipeline_ = std::make_unique<ComputePipeline>();
		computePipeline_->SetShader(&shader);

		/// Buffer
		computePipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); /// UAV_INSTANCE_DATA
		computePipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// SRV_VERTEX_TEXTURE

		computePipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); /// UAV_INSTANCE_DATA
		computePipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); /// SRV_VERTEX_TEXTURE

		computePipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0);

		computePipeline_->CreatePipeline(_dxManager->GetDxDevice());
	}



	{
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Render/Terrain/TerrainProcedural.ps.hlsl", L"ps_6_6", Shader::Type::ps);
		shader.CompileShader(L"./Packages/Shader/Render/Terrain/TerrainProcedural.vs.hlsl", L"vs_6_6", Shader::Type::vs);


		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		/// InputLayout
		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
		pipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

		/// Buffer
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// GP_CBV_VIEW_PROJECTION

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// GP_SRV_INSNTANCE_DATA
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// GP_SRV_INSNTANCE_DATA


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

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());
	}


	{	/// buffer create

		instanceDataBuffer_.Create(100, _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());
		instanceDataBuffer_.CreateAppendBuffer(100, _dxManager->GetDxDevice(), _dxManager->GetDxCommand(), _dxManager->GetDxSRVHeap());

	}
}

void TerrainProceduralRenderingPipeline::Draw(EntityComponentSystem* _ecs, const std::vector<IEntity*>& _entities, CameraComponent* _camera, DxCommand* _dxCommand) {

	ComponentArray<Terrain>* terrainArray = _ecs->GetComponentArray<Terrain>();
	if (!terrainArray) {
		return;
	}

	/// 一旦先頭にあるTerrainのみ描画する
	Terrain* pTerrain_ = nullptr;
	for (auto& terrain : terrainArray->GetUsedComponents()) {
		if (terrain->GetOwner()) {
			pTerrain_ = terrain;
			break; // 先頭のTerrainのみを使用
		}
	}

	/// 見つかんなかったら return
	if (!pTerrain_ || !pTerrain_->GetIsCreated()) {
		return;
	}


	auto cmdList = _dxCommand->GetCommandList();

	static bool isFirst = true;
	if (isFirst) {	/// compute shaderを起動する
		isFirst = false;
		computePipeline_->SetPipelineStateForCommandList(_dxCommand);

		instanceDataBuffer_.AppendBindForComputeCommandList(CP_INSNTANCE_DATA, cmdList); // UAV_INSTANCE_DATA

		const Texture* vertexTexture = pResourceCollection_->GetTexture("./Packages/Textures/Terrain/TerrainVertex.png");
		cmdList->SetComputeRootDescriptorTable(CP_SRV_VERTEX_TEXTURE, vertexTexture->GetSRVGPUHandle());

		const uint32_t threadGroupSize = 16;
		cmdList->Dispatch(
			(1000 + threadGroupSize - 1) / threadGroupSize,
			(1000 + threadGroupSize - 1) / threadGroupSize,
			1
		);
	}


	{	/// 描画する


		// インスタンス数をGPUからCPUにコピー（例：ID3D12Resourceにカウンタをコピー）
		uint32_t instanceCount = instanceDataBuffer_.ReadCounter(_dxCommand);
		pDxManager_->HeapBindToCommandList();

		/// 一旦適当なモデルを使用する
		const Model* model = pResourceCollection_->GetModel("./Packages/Models/primitive/cube.obj");

		const D3D12_VERTEX_BUFFER_VIEW& vbv = model->GetMeshes().front()->GetVBV();
		const D3D12_INDEX_BUFFER_VIEW& ibv = model->GetMeshes().front()->GetIBV();

		pipeline_->SetPipelineStateForCommandList(_dxCommand);
		// 描画セットアップ
		cmdList->IASetVertexBuffers(0, 1, &vbv);
		cmdList->IASetIndexBuffer(&ibv);
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// SRVとしてインスタンス行列バッファをルートシグネチャにセット
		_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, GP_CBV_VIEW_PROJECTION);
		instanceDataBuffer_.SRVBindForGraphicsCommandList(GP_SRV_INSNTANCE_DATA, cmdList); // GP_SRV_INSNTANCE_DATA

		// 描画実行
		cmdList->DrawIndexedInstanced(static_cast<UINT>(model->GetMeshes().front()->GetIndices().size()), instanceCount, 0, 0, 0);
	}

}
