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
		pipeline_->Add32BitConstant(D3D12_SHADER_VISIBILITY_ALL, 1, 2); // ROOT_PARAM_START_INDEX

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

		if (!grass->GetIsCreated() /*|| grass->isArranged_*/) {
			continue;
		}

		/// 配置済みにフラグを変える
		grass->isArranged_ = true;

		/// Bufferの設定
		grass->GetRwGrassInstanceBuffer().UAVBindForComputeCommandList(
			cmdList, UAV_BLADE_INSTANCES
		);

		UINT maxInstanceNum = grass->GetMaxGrassCount();
		UINT threadGroupSize = 32;
		UINT maxDispatchCount = 65535; // DirectX 12の1回のDispatchで処理可能な最大スレッド数

		// Dispatchを分割して実行
		for (UINT startIndex = 0; startIndex < maxInstanceNum; startIndex += maxDispatchCount * threadGroupSize) {
			UINT remainingInstances = maxInstanceNum - startIndex;
			UINT currentInstanceCount = (std::min)(remainingInstances, maxDispatchCount * threadGroupSize);

			UINT dispatchSizeX = (currentInstanceCount + threadGroupSize - 1) / threadGroupSize;

			// startIndexとcurrentInstanceCountをシェーダーに渡す
			UINT constants[2] = { startIndex, currentInstanceCount };
			cmdList->SetComputeRoot32BitConstants(C32BIT_CONSTANTS, 2, constants, 0);

			// Dispatchを実行
			cmdList->Dispatch(dispatchSizeX, 1, 1);
		}

	}

}

