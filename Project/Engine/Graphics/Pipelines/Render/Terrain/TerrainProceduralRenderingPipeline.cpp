#include "TerrainProceduralRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/Asset/Collection/GraphicsResourceCollection.h"

TerrainProceduralRenderingPipeline::TerrainProceduralRenderingPipeline(GraphicsResourceCollection* _resourceCollection)
	: pGrc_(_resourceCollection) {}
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
		computePipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// SRV_SPLAT_BLEND_TEXTURE

		computePipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); /// UAV_INSTANCE_DATA
		computePipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); /// SRV_VERTEX_TEXTURE
		computePipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2); /// SRV_SPLAT_BLEND_TEXTURE

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
		pipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
		pipeline_->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

		/// Buffer
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// GP_CBV_VIEW_PROJECTION
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0); /// GP_CBV_TEXTURE_ID

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// GP_SRV_INSNTANCE_DATA
		pipeline_->AddDescriptorRange(0, MAX_TEXTURE_COUNT, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// GP_SRV_TEXTURES
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// GP_SRV_INSNTANCE_DATA
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1); /// GP_SRV_TEXTURES

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0); /// StaticSampler for textures

		/// setting
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		pipeline_->SetDepthStencilDesc(depthStencilDesc);

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());
	}


	{	/// buffer create
		instanceDataAppendBuffer_.CreateAppendBuffer(static_cast<uint32_t>(std::pow(2, 24)), _dxManager->GetDxDevice(), _dxManager->GetDxCommand(), _dxManager->GetDxSRVHeap());
		textureIdBuffer_.Create(_dxManager->GetDxDevice());
		instanceCount_ = 0;
	}
}

void TerrainProceduralRenderingPipeline::PreDraw(ECSGroup*, CameraComponent*, DxCommand* _dxCommand) {
	auto cmdList = _dxCommand->GetCommandList();

	computePipeline_->SetPipelineStateForCommandList(_dxCommand);
	instanceDataAppendBuffer_.AppendBindForComputeCommandList(cmdList, CP_INSNTANCE_DATA); // UAV_INSTANCE_DATA

	const Texture* vertexTexture = pGrc_->GetTexture("./Packages/Textures/Terrain/TerrainVertex.png");
	const Texture* splatBlendTexture = pGrc_->GetTexture("./Packages/Textures/Terrain/TerrainSplatBlend.png");
	cmdList->SetComputeRootDescriptorTable(CP_SRV_VERTEX_TEXTURE, vertexTexture->GetSRVGPUHandle());
	cmdList->SetComputeRootDescriptorTable(CP_SRV_SPLAT_BLEND_TEXTURE, splatBlendTexture->GetSRVGPUHandle());

	const uint32_t threadGroupSize = 32;
	cmdList->Dispatch(
		(2000 + threadGroupSize - 1) / threadGroupSize,
		(2000 + threadGroupSize - 1) / threadGroupSize,
		1
	);

	D3D12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(instanceDataAppendBuffer_.GetResource().Get());
	cmdList->ResourceBarrier(1, &uavBarrier);
	instanceDataAppendBuffer_.GetCounterResource().CreateBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, _dxCommand);

	_dxCommand->CommandExecute();
	_dxCommand->CommandReset();
	_dxCommand->WaitForGpuComplete();

	instanceCount_ = instanceDataAppendBuffer_.ReadCounter(_dxCommand);
	pDxManager_->HeapBindToCommandList();
	instanceDataAppendBuffer_.ResetCounter(_dxCommand); // カウンターをリセット
}

void TerrainProceduralRenderingPipeline::Draw(ECSGroup* _ecs, const std::vector<GameEntity*>&, CameraComponent* _camera, DxCommand* _dxCommand) {

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
	if (!pTerrain_ || !pTerrain_->GetIsCreated() || !pTerrain_->enable) {
		return;
	}


	/// Procedural Rendering でなければ return
	if (!pTerrain_->GetIsRenderingProcedural()) {
		return;
	}



	/* ----- pipeline の設定 & 起動 ----- */

	auto cmdList = _dxCommand->GetCommandList();

	/// -----------------------------------------------
	/// 必用なリソースの取得
	/// -----------------------------------------------

	/// model
	const Model* model = pGrc_->GetModel("./Packages/Models/BackgroundObjects/Tree3.obj");

	/// textures
	auto& textures = pGrc_->GetTextures();


	/// -----------------------------------------------
	/// pipelineの設定
	/// -----------------------------------------------

	/// pipelineの設定
	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	/// 形状の設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	/* ----- bufferの設定 ----- */

	/// vertex: camera
	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, GP_CBV_VIEW_PROJECTION);
	/// vertex: instance data
	instanceDataAppendBuffer_.SRVBindForGraphicsCommandList(cmdList, GP_SRV_INSNTANCE_DATA); // GP_SRV_INSNTANCE_DATA

	/// pixel: texture id
	uint32_t texId = static_cast<uint32_t>(pGrc_->GetTextureIndex("./Packages/Textures/white.png"));
	textureIdBuffer_.SetMappedData({ texId });
	textureIdBuffer_.BindForGraphicsCommandList(cmdList, GP_CBV_TEXTURE_ID);
	/// pixel: テクスチャをバインド
	cmdList->SetGraphicsRootDescriptorTable(GP_SRV_TEXTURES, (*textures.begin()).GetSRVGPUHandle());

	for (auto& mesh : model->GetMeshes()) {

		/// vbv, ibvの設定
		cmdList->IASetVertexBuffers(0, 1, &mesh->GetVBV());
		cmdList->IASetIndexBuffer(&mesh->GetIBV());

		// 描画実行
		cmdList->DrawIndexedInstanced(static_cast<UINT>(mesh->GetIndices().size()), instanceCount_, 0, 0, 0);
	}

}
