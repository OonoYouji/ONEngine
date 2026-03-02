#include "PostProcessVoxelTerrainBrush.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/ECS/Component/Components/ComputeComponents/VoxelTerrain/VoxelTerrain.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"


namespace ONEngine {

PostProcessVoxelTerrainBrush::PostProcessVoxelTerrainBrush() = default;
PostProcessVoxelTerrainBrush::~PostProcessVoxelTerrainBrush() = default;


void PostProcessVoxelTerrainBrush::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	{	/// shader compile

		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"Packages/Shader/PostProcess/PerObject/VoxelTerrainBrush/VoxelTerrainBrush.cs.hlsl", L"cs_6_6", Shader::Type::cs);

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

		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}

	{	/// buffer
		brushBuffer_.Create(_dxm->GetDxDevice());
	}

}

void PostProcessVoxelTerrainBrush::Execute(
	const std::string& _textureName, DxCommand* _dxCommand,
	AssetCollection* _assetCollection, EntityComponentSystem* _ecs) {

	/// TerrainComponentの有無チェック
	ComponentArray<VoxelTerrain>* voxelTerrainArray = _ecs->GetCurrentGroup()->GetComponentArray<VoxelTerrain>();
	/// 両方とも存在しない、もしくは使用中のコンポーネントが無い場合は処理しない
	if(!CheckComponentArrayEnable(voxelTerrainArray)) {
		return;
	}

	/// 地形が編集モード中なのかチェック
	VoxelTerrain* vt = nullptr;
	for(const auto& terrain : voxelTerrainArray->GetUsedComponents()) {
		if(CheckComponentEnable(terrain)) {
			vt = terrain;
			break;
		}
	}

	/// 編集モードでなければ処理しない
	if(!CheckComponentEnable(vt)) {
		return;
	}

	/// brush data
	const Vector2 mousePos = Input::GetImGuiImageMousePosNormalized("Scene");
	/// 範囲外なら処理しない
	if(!Math::Inside(mousePos, Vector2::Zero, Vector2::HD)) {
		return;
	}

	float brushRadius = vt->GetBrushRadius();

	float blinkSpeed = 2.0f;
	float minAlpha = 0.2f;
	float maxAlpha = 0.8f;

	float phase = std::fmod(Time::GetTime() * blinkSpeed, 2.0f);
	float t = (phase < 1.0f) ? phase : 2.0f - phase;

	float easedT = ONEngine::Ease::InOut::Sine(t);

	Vector4 defaultColor = { 0.5725f, 0.7412f, 0.9451f, 1.0f };
	defaultColor.w = minAlpha + (maxAlpha - minAlpha) * easedT; /// ここに計算したアルファ値を代入

	brushBuffer_.SetMappedData(
		Brush{ defaultColor, mousePos, brushRadius }
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
		Math::DivideAndRoundUp(static_cast<uint32_t>(EngineConfig::kWindowSize.x), 16),
		Math::DivideAndRoundUp(static_cast<uint32_t>(EngineConfig::kWindowSize.y), 16),
		1
	);

	/// 大本のsceneテクスチャに結果をコピー
	CopyResource(
		textures[textureIndices_[3]].GetDxResource().Get(),
		textures[textureIndices_[0]].GetDxResource().Get(),
		cmdList
	);

}

} /// namespace ONEngine