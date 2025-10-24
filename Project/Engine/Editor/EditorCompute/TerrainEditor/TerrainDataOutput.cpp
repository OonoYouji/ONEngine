#include "TerrainDataOutput.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Utility.h"

#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"

TerrainDataOutput::TerrainDataOutput() {}
TerrainDataOutput::~TerrainDataOutput() {}

void TerrainDataOutput::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	pDxManager_ = _dxm;

	{	/// shader
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Editor/TerrainDataOutput.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		pipeline_ = std::make_unique<ComputePipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0);	/// CBV_TERRAIN_SIZE
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV);	/// UAV_VERTICES
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV);	/// UAV_OUTPUT_TEXTURE
		pipeline_->AddDescriptorRange(2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV);	/// UAV_OUTPUT_SPLAT_BLEND_TEXTURE
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0);	/// UAV_VERTICES
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1);	/// UAV_OUTPUT_TEXTURE
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2);	/// UAV_OUTPUT_SPLAT_BLEND_TEXTURE

		pipeline_->CreatePipeline(_dxm->GetDxDevice());

	}

	{	/// buffer
		terrainSize_.Create(_dxm->GetDxDevice());
		outputVertexTexture_.CreateUAVTexture(1000, 1000, _dxm->GetDxDevice(), _dxm->GetDxSRVHeap(), DXGI_FORMAT_R16G16B16A16_UNORM);
		outputSplatBlendTexture_.CreateUAVTexture(1000, 1000, _dxm->GetDxDevice(), _dxm->GetDxSRVHeap());
	}


}

void TerrainDataOutput::Execute(EntityComponentSystem* _ecs, DxCommand* _dxCommand, AssetCollection* /*_assetCollection*/) {
	/// 出力をするときしか処理しない
	if (!(Input::PressKey(DIK_LCONTROL) && Input::TriggerKey(DIK_O))) {
		return;
	}

	/// 地形の component があるのかチェック
	ComponentArray<Terrain>* terrainArray = _ecs->GetCurrentGroup()->GetComponentArray<Terrain>();
	if (!terrainArray) {
		return;
	}

	Terrain* pTerrain = nullptr;
	for (auto& terrain : terrainArray->GetUsedComponents()) {
		if (!terrain) {
			continue;
		}

		pTerrain = terrain;
	}

	if (!pTerrain || !pTerrain->GetIsCreated()) {
		return;
	}


	/// bufferに値を設定
	uint32_t width = static_cast<uint32_t>(pTerrain->GetSize().x);
	uint32_t height = static_cast<uint32_t>(pTerrain->GetSize().y);
	terrainSize_.SetMappedData({ width, height });

	/// pipelineの設定&実行
	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	auto cmdList = _dxCommand->GetCommandList();

	terrainSize_.BindForComputeCommandList(cmdList, CBV_TERRAIN_SIZE);
	pTerrain->GetRwVertices().UAVBindForComputeCommandList(cmdList, UAV_VERTICES);
	cmdList->SetComputeRootDescriptorTable(UAV_OUTPUT_VERTEX_TEXTURE, outputVertexTexture_.GetUAVGPUHandle());
	cmdList->SetComputeRootDescriptorTable(UAV_OUTPUT_SPLAT_BLEND_TEXTURE, outputSplatBlendTexture_.GetUAVGPUHandle());

	const size_t threadGroupSize = 16;
	cmdList->Dispatch(
		(width + threadGroupSize - 1) / threadGroupSize,
		(height + threadGroupSize - 1) / threadGroupSize,
		1
	);

	outputVertexTexture_.OutputTexture(L"./Packages/Textures/Terrain/TerrainVertex.png", pDxManager_->GetDxDevice(), _dxCommand);
	outputSplatBlendTexture_.OutputTexture(L"./Packages/Textures/Terrain/TerrainSplatBlend.png", pDxManager_->GetDxDevice(), _dxCommand);
	pDxManager_->GetDxSRVHeap()->BindToCommandList(
		pDxManager_->GetDxCommand()->GetCommandList()
	);
}
