#include "PostProcessTerrainBrush.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Input/Input.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

PostProcessTerrainBrush::PostProcessTerrainBrush() = default;
PostProcessTerrainBrush::~PostProcessTerrainBrush() = default;


void PostProcessTerrainBrush::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	{	/// shader compile

		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"Packages/Shader/PostProcess/PerObject/TerrainBrush/TerrainBrush.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		pipeline_ = std::make_unique<ComputePipeline>();
		pipeline_->SetShader(&shader);

		/// BRUSH
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0);

		/// SRV, UAV
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipeline_->AddDescriptorRange(2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV);

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 3);

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0);

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());
	}

	{	/// buffer
		brushBuffer_.Create(_dxManager->GetDxDevice());
	}

}

void PostProcessTerrainBrush::Execute(
	const std::string& _textureName, DxCommand* _dxCommand,
	AssetCollection* _assetCollection, EntityComponentSystem* _entityComponentSystem) {

	/// brush data
	const Vector2 mousePos = Input::GetImGuiImageMousePosNormalized("Scene");
	/// 範囲外なら処理しない
	if (mousePos.x < 0.0f || mousePos.x > 1280.0f ||
		mousePos.y < 0.0f || mousePos.y > 720.0f) {
		return;
	}

	/// Terrainの取得
	Terrain* terrain = nullptr;
	ComponentArray<Terrain>* terrainArray = _entityComponentSystem->GetCurrentGroup()->GetComponentArray<Terrain>();
	if (terrainArray) {
		for (auto& t : terrainArray->GetUsedComponents()) {
			if (!t) {
				continue;
			}
			terrain = t;
			break;
		}
	}

	if (!terrain) {
		return;
	}

	brushBuffer_.SetMappedData(
		Brush{ mousePos, terrain->GetEditorInfo().brushRadius }
	);

	/// texture index
	auto& textures = _assetCollection->GetTextures();
	textureIndices_[0] = _assetCollection->GetTextureIndex(_textureName + "Scene");
	textureIndices_[1] = _assetCollection->GetTextureIndex(_textureName + "WorldPosition");
	textureIndices_[2] = _assetCollection->GetTextureIndex(_textureName + "Flags");
	textureIndices_[3] = _assetCollection->GetTextureIndex("postProcessResult");

	auto cmdList = _dxCommand->GetCommandList();

	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	brushBuffer_.BindForComputeCommandList(_dxCommand->GetCommandList(), CBV_BRUSH);
	cmdList->SetComputeRootDescriptorTable(SRV_COLOR, textures[textureIndices_[0]].GetSRVGPUHandle());
	cmdList->SetComputeRootDescriptorTable(SRV_POSITION, textures[textureIndices_[1]].GetSRVGPUHandle());
	cmdList->SetComputeRootDescriptorTable(SRV_FLAGS, textures[textureIndices_[2]].GetSRVGPUHandle());
	cmdList->SetComputeRootDescriptorTable(UAV_RESULT, textures[textureIndices_[3]].GetUAVGPUHandle());


	cmdList->Dispatch(
		static_cast<UINT>(EngineConfig::kWindowSize.x) / 16,
		static_cast<UINT>(EngineConfig::kWindowSize.y) / 16,
		1
	);

	/// 大本のsceneテクスチャに結果をコピー
	CopyResource(
		textures[textureIndices_[3]].GetDxResource().Get(),
		textures[textureIndices_[0]].GetDxResource().Get(),
		cmdList
	);

}