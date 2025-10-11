#include "GrassArrangementPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Grass/GrassField.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"

GrassArrangementPipeline::GrassArrangementPipeline() = default;
GrassArrangementPipeline::~GrassArrangementPipeline() = default;

void GrassArrangementPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	{	/// shader
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Editor/GrassArrangement.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		/// pipeline
		pipeline_ = std::make_unique<ComputePipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); // CBV_PARAMS

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); // UAV_VERTICES
		pipeline_->AddDescriptorRange(0, MAX_TEXTURE_COUNT, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SRV_CONTROL_POINTS

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); // ROOT_PARAM_CBV_PARAMS
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); // ROOT_PARAM_UAV_VERTICES

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0); // s0

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());
	}

	{	/// buffer
		usedTexIdBuffer_.Create(_dxManager->GetDxDevice());

	}

}

void GrassArrangementPipeline::Execute(EntityComponentSystem* _ecs, DxCommand* _dxCommand, GraphicsResourceCollection* _grc) {

	/// ==================================================
	/// 早期リターン条件のチェック
	/// ==================================================
	ComponentArray<GrassField>* grassArray = _ecs->GetCurrentGroup()->GetComponentArray<GrassField>();
	if (!grassArray || grassArray->GetUsedComponents().empty()) {
		return;
	}

	/// Pipelineの設定
	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	/// bufferの設定
	auto cmdList = _dxCommand->GetCommandList();

	uint32_t grassArrangementTexId = _grc->GetTextureIndex("./Packages/Textures/Terrain/GrassArrangement.png");
	uint32_t terrainVertexTexId = _grc->GetTextureIndex("./Packages/Textures/Terrain/TerrainVertex.png");
	usedTexIdBuffer_.SetMappedData(UsedTextureIDs{ grassArrangementTexId, terrainVertexTexId });
	usedTexIdBuffer_.BindForComputeCommandList(cmdList, CBV_USED_TEXTURED_IDS);

	const auto& textures = _grc->GetTextures();
	cmdList->SetComputeRootDescriptorTable(SRV_TEXTURES, textures[0].GetSRVHandle().gpuHandle);

	for (auto& grass : grassArray->GetUsedComponents()) {
		if (!grass || !grass->enable) {
			continue;
		}

		if (!grass->GetIsCreated() || grass->isArranged_) {
			continue;
		}

		/// 配置済みにフラグを変える
		grass->isArranged_ = true;

		/// Bufferの設定
		grass->GetRwGrassInstanceBuffer().UAVBindForComputeCommandList(cmdList, UAV_BLADE_INSTANCES);


		/// Dispatch数を計算してシェーダーを起動
		//UINT instanceCount = static_cast<UINT>(std::pow(2, 16) - 1);
		//UINT threadGroupSize = 32;
		//UINT dispatchSize = ((instanceCount + threadGroupSize - 1) / threadGroupSize) - 1;
		//cmdList->Dispatch(
		//	dispatchSize, 
		//	dispatchSize, 
		//	1
		//);

		UINT terrainResolution = 1000; // 地形の解像度
		UINT threadGroupSize = 32;

		UINT dispatchSizeX = (terrainResolution + threadGroupSize - 1) / threadGroupSize;
		UINT dispatchSizeY = (terrainResolution + threadGroupSize - 1) / threadGroupSize;

		cmdList->Dispatch(dispatchSizeX, dispatchSizeY, 1);
	}

}

