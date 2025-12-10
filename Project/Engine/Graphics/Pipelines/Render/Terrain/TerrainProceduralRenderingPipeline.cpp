#include "TerrainProceduralRenderingPipeline.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/Asset/Collection/AssetCollection.h"

TerrainProceduralRenderingPipeline::TerrainProceduralRenderingPipeline(AssetCollection* _assetCollection)
	: pAssetCollection_(_assetCollection) {
}
TerrainProceduralRenderingPipeline::~TerrainProceduralRenderingPipeline() {}

void TerrainProceduralRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	pDxManager_ = _dxm;

	{	/// compute pipeline

		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Editor/Procedural.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		arrangementPipeline_ = std::make_unique<ComputePipeline>();
		arrangementPipeline_->SetShader(&shader);

		/// Buffer
		arrangementPipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); /// CBV_DATA_BUFFER
		arrangementPipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); /// UAV_INSTANCE_DATA
		arrangementPipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// SRV_VERTEX_TEXTURE
		arrangementPipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// SRV_SPLAT_BLEND_TEXTURE
		arrangementPipeline_->AddDescriptorRange(2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// SRV_TREE_ARRANGEMENT_TEXTURE

		arrangementPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); /// UAV_INSTANCE_DATA
		arrangementPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); /// SRV_VERTEX_TEXTURE
		arrangementPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2); /// SRV_SPLAT_BLEND_TEXTURE
		arrangementPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 3); /// SRV_TREE_ARRANGEMENT_TEXTURE

		arrangementPipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0);

		arrangementPipeline_->CreatePipeline(_dxm->GetDxDevice());
	}

	{	/// カリング用パイプライン
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Render/Terrain/TerrainProcedural.cs.hlsl", L"cs_6_6", Shader::Type::cs);
		cullingPipeline_ = std::make_unique<ComputePipeline>();
		cullingPipeline_->SetShader(&shader);
		/// Buffer
		cullingPipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); /// CALL_CBV_VIEW_PROJECTION
		cullingPipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 1); /// CALL_CBV_MAX_INSTANCE_COUNT
		cullingPipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// CALL_SRV_INSNTANCE_DATA
		cullingPipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); /// CALL_UAV_RENDERING_INSTANCE
		cullingPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); /// CALL_SRV_INSNTANCE_DATA
		cullingPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); /// CALL_UAV_RENDERING_INSTANCE
		cullingPipeline_->CreatePipeline(_dxm->GetDxDevice());
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
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// GP_SRV_INSNTANCE_DATA
		pipeline_->AddDescriptorRange(0, MAX_TEXTURE_COUNT, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// GP_SRV_TEXTURES
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// GP_SRV_INSNTANCE_DATA
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 1); /// GP_SRV_INSNTANCE_DATA
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 2); /// GP_SRV_TEXTURES

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0); /// StaticSampler for textures

		/// setting
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);

		pipeline_->SetDepthStencilDesc(DefaultDepthStencilDesc());

		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}


	{	/// buffer create
		uint32_t maxInstanceCount = static_cast<uint32_t>(std::pow(2, 24));
		instanceDataAppendBuffer_.CreateAppendBuffer(maxInstanceCount, _dxm->GetDxDevice(), _dxm->GetDxCommand(), _dxm->GetDxSRVHeap());
		renderingInstanceAppendBuffer_.CreateAppendBuffer(maxInstanceCount, _dxm->GetDxDevice(), _dxm->GetDxCommand(), _dxm->GetDxSRVHeap());

		textureIdBuffer_.Create(_dxm->GetDxDevice());
		instanceCount_ = 0;
		drawInstanceCount_ = 0;

		dataBuffer_.Create(_dxm->GetDxDevice());
		dataBuffer_.SetMappedData(2000);

		maxInstanceCountBuffer_.Create(_dxm->GetDxDevice());
		maxInstanceCountBuffer_.SetMappedData(drawInstanceCount_);
	}
}

void TerrainProceduralRenderingPipeline::PreDraw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {
	/// 配列の取得 & 存在チェック
	ComponentArray<Terrain>* terrainArray = _ecs->GetComponentArray<Terrain>();
	if (!terrainArray || terrainArray->GetUsedComponents().empty()) {
		return;
	}

	/// 一旦先頭にあるTerrainのみ描画する
	Terrain* terrain = nullptr;
	for (auto& terrainComp : terrainArray->GetUsedComponents()) {
		if (terrainComp->GetOwner()) {
			terrain = terrainComp;
			break; // 先頭のTerrainのみを使用
		}
	}

	/// 見つからない場合、地形が生成されていない場合、無効な場合は return
	if (!terrain || !terrain->GetIsCreated() || !terrain->enable) {
		return;
	}


	/// プロシージャル植生をレンダリングするかチェック
	if (!terrain->GetIsRenderingProcedural()) {
		return;
	}



	auto cmdList = _dxCommand->GetCommandList();

	pDxManager_->HeapBindToCommandList();

	{	/// 配置Shaderの起動
		arrangementPipeline_->SetPipelineStateForCommandList(_dxCommand);
		dataBuffer_.BindForComputeCommandList(cmdList, ARR_DATA);
		instanceDataAppendBuffer_.AppendBindForComputeCommandList(cmdList, ARR_INSNTANCE_DATA); // UAV_INSTANCE_DATA

		/// 使用するテクスチャを取得、バインドする
		const Texture* vertexTexture = pAssetCollection_->GetTexture("./Packages/Textures/Terrain/TerrainVertex.png");
		const Texture* splatBlendTexture = pAssetCollection_->GetTexture("./Packages/Textures/Terrain/TerrainSplatBlend.png");
		const Texture* arrangementTexture = pAssetCollection_->GetTexture("./Packages/Textures/Terrain/TreeArrangement.png");
		cmdList->SetComputeRootDescriptorTable(ARR_SRV_VERTEX_TEXTURE, vertexTexture->GetSRVGPUHandle());
		cmdList->SetComputeRootDescriptorTable(ARR_SRV_SPLAT_BLEND_TEXTURE, splatBlendTexture->GetSRVGPUHandle());
		cmdList->SetComputeRootDescriptorTable(ARR_SRV_TREE_ARRANGEMENT_TEXTURE, arrangementTexture->GetSRVGPUHandle());

		/// numthreadsに合わせてDispatchする
		const uint32_t size = static_cast<uint32_t>(dataBuffer_.GetMappingData());
		const uint32_t threadGroupSize = 32;
		cmdList->Dispatch(
			Mathf::DivideAndRoundUp(size, threadGroupSize),
			Mathf::DivideAndRoundUp(size, threadGroupSize),
			1
		);

		D3D12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(instanceDataAppendBuffer_.GetResource().Get());
		cmdList->ResourceBarrier(1, &uavBarrier);
		instanceDataAppendBuffer_.GetCounterResource().CreateBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, _dxCommand);
	}


	_dxCommand->CommandExecuteAndWait();
	_dxCommand->CommandReset();
	_dxCommand->WaitForGpuComplete();

	instanceCount_ = instanceDataAppendBuffer_.ReadCounter(_dxCommand);
	pDxManager_->HeapBindToCommandList();
	instanceDataAppendBuffer_.ResetCounter(_dxCommand); // カウンターをリセット

	/// 以降の処理はこれが0だと処理できないのでreturn
	if (instanceCount_ == 0) {
		drawInstanceCount_ = 0;
		return;
	}


	{	/// カリングShaderの起動
		maxInstanceCountBuffer_.SetMappedData(instanceCount_);


		cullingPipeline_->SetPipelineStateForCommandList(_dxCommand);
		_camera->GetViewProjectionBuffer().BindForComputeCommandList(cmdList, CULL_CBV_VIEW_PROJECTION);
		maxInstanceCountBuffer_.BindForComputeCommandList(cmdList, CULL_CBV_MAX_INSTANCE_COUNT); // CULL_CBV_MAX_INSTANCE_COUNT
		instanceDataAppendBuffer_.SRVBindForComputeCommandList(cmdList, CULL_SRV_INSNTANCE_DATA); // SRV_INSTANCE_DATA
		renderingInstanceAppendBuffer_.AppendBindForComputeCommandList(cmdList, CULL_UAV_RENDERING_INSTANCE); // UAV_RENDERING_INSTANCE
		/// numthreadsに合わせてDispatchする
		const uint32_t threadGroupSize = 32;
		cmdList->Dispatch(
			Mathf::DivideAndRoundUp(instanceCount_, threadGroupSize),
			1, 1
		);
		D3D12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(renderingInstanceAppendBuffer_.GetResource().Get());
		cmdList->ResourceBarrier(1, &uavBarrier);
		renderingInstanceAppendBuffer_.GetCounterResource().CreateBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, _dxCommand);
	}

	_dxCommand->CommandExecuteAndWait();
	_dxCommand->CommandReset();
	_dxCommand->WaitForGpuComplete();

	drawInstanceCount_ = renderingInstanceAppendBuffer_.ReadCounter(_dxCommand);
	pDxManager_->HeapBindToCommandList();
	renderingInstanceAppendBuffer_.ResetCounter(_dxCommand); // カウンターをリセット

}

void TerrainProceduralRenderingPipeline::Draw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {

	/// 配列の取得 & 存在チェック
	ComponentArray<Terrain>* terrainArray = _ecs->GetComponentArray<Terrain>();
	if (!terrainArray || terrainArray->GetUsedComponents().empty()) {
		return;
	}

	/// 一旦先頭にあるTerrainのみ描画する
	Terrain* terrain = nullptr;
	for (auto& terrainComp : terrainArray->GetUsedComponents()) {
		if (terrainComp->GetOwner()) {
			terrain = terrainComp;
			break; // 先頭のTerrainのみを使用
		}
	}

	/// 見つからない場合、地形が生成されていない場合、無効な場合は return
	if (!terrain || !terrain->GetIsCreated() || !terrain->enable) {
		return;
	}


	/// プロシージャル植生をレンダリングするかチェック
	if (!terrain->GetIsRenderingProcedural()) {
		return;
	}

	if (drawInstanceCount_ == 0) {
		return;
	}


	/// ---- - pipeline の設定 & 起動 ----- ///

	auto cmdList = _dxCommand->GetCommandList();

	/// -----------------------------------------------
	/// 必用なリソースの取得
	/// -----------------------------------------------

	/// model
	const Model* model = pAssetCollection_->GetModel("./Packages/Models/BackgroundObjects/Tree3.obj");

	/// textures
	const auto& textures = pAssetCollection_->GetTextures();


	/// -----------------------------------------------
	/// pipelineの設定
	/// -----------------------------------------------

	/// pipelineの設定
	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	/// 形状の設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	/// ----- bufferの設定 ----- ///

	/// vertex: camera
	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, GP_CBV_VIEW_PROJECTION);
	/// vertex: instance data
	instanceDataAppendBuffer_.SRVBindForGraphicsCommandList(cmdList, GP_SRV_INSNTANCE_DATA); // GP_SRV_INSNTANCE_DATA
	renderingInstanceAppendBuffer_.SRVBindForGraphicsCommandList(cmdList, GP_SRV_RENDERING_INSTANCE); // GP_SRV_INSNTANCE_DATA

	/// pixel: texture id
	uint32_t texId = static_cast<uint32_t>(pAssetCollection_->GetTextureIndex("./Packages/Textures/white.png"));
	textureIdBuffer_.SetMappedData({ texId });
	textureIdBuffer_.BindForGraphicsCommandList(cmdList, GP_CBV_TEXTURE_ID);
	/// pixel: テクスチャをバインド
	cmdList->SetGraphicsRootDescriptorTable(GP_SRV_TEXTURES, (*textures.begin()).GetSRVGPUHandle());

	for (auto& mesh : model->GetMeshes()) {

		/// vbv, ibvの設定
		cmdList->IASetVertexBuffers(0, 1, &mesh->GetVBV());
		cmdList->IASetIndexBuffer(&mesh->GetIBV());

		// 描画実行
		cmdList->DrawIndexedInstanced(static_cast<UINT>(mesh->GetIndices().size()), drawInstanceCount_, 0, 0, 0);
	}

}
