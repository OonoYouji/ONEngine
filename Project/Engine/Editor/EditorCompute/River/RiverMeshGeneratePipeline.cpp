#include "RiverMeshGeneratePipeline.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"

RiverMeshGeneratePipeline::RiverMeshGeneratePipeline() = default;
RiverMeshGeneratePipeline::~RiverMeshGeneratePipeline() = default;

void RiverMeshGeneratePipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	pDxManager_ = _dxm;

	{	/// shader
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Editor/RiverMeshGenerator.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		pipeline_ = std::make_unique<ComputePipeline>();
		pipeline_->SetShader(&shader);

		/// buffer
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0);

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// SRV_CONTROL_POINTS
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); /// UAV_VERTICES
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); /// UAV_INDICES

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); /// SRV_CONTROL_POINTS
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); /// UAV_VERTICES
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2); /// UAV_INDICES

		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}

}

void RiverMeshGeneratePipeline::Execute(EntityComponentSystem* _ecs, DxCommand* _dxCommand, AssetCollection* /*_assetCollection*/) {
	/// --------------------------------------------------------------------
	/// 早期リターンチェック
	/// --------------------------------------------------------------------

	ECSGroup* ecsGroup = _ecs->GetCurrentGroup();
	if (!ecsGroup) {
		Console::LogError("RiverMeshGeneratePipeline::Execute: ECSGroup is null");
		return;
	}

	ComponentArray<Terrain>* terrainArray = ecsGroup->GetComponentArray<Terrain>();
	if (!terrainArray || terrainArray->GetUsedComponents().empty()) {
		Console::LogError("RiverMeshGeneratePipeline::Execute: Terrain component array is null");
		return;
	}

	Terrain* terrain = terrainArray->GetUsedComponents().front();
	if (!terrain) {
		Console::LogError("RiverMeshGeneratePipeline::Execute: Terrain component is null");
		return;
	}


	/// --------------------------------------------------------------------
	/// 川のメッシュ生成
	/// --------------------------------------------------------------------

	/// 川の取得
	River* river = terrain->GetRiver();
	if (!river->GetIsGenerateMeshRequest()) {
		return;
	}

	/// バッファは生成時に毎回作る
	river->CreateBuffers(pDxManager_->GetDxDevice(), pDxManager_->GetDxSRVHeap(), _dxCommand);
	river->SetBufferData();


	auto cmdList = _dxCommand->GetCommandList();

	/// bufferの設定
	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	river->GetParamBuffer().BindForComputeCommandList(cmdList, CBV_PARAMS);
	river->GetControlPointBuffer().SRVBindForComputeCommandList(cmdList, SRV_CONTROL_POINTS);
	river->GetRwVertices().UAVBindForComputeCommandList(cmdList, UAV_VERTICES);
	river->GetRwIndices().UAVBindForComputeCommandList(cmdList, UAV_INDICES);

	/// 起動
	const UINT totalVertices = river->GetSamplePerSegment() * (river->GetNumControlPoint() - 3) * 2;
	const UINT threadsPerGroup = 16;
	cmdList->Dispatch(
		Mathf::DivideAndRoundUp(totalVertices, threadsPerGroup),
		1, 1
	);
}

