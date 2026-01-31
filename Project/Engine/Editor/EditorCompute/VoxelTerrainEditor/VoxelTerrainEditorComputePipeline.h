#pragma once

#include <vector>

/// engine
#include "../Interface/IEditorCompute.h"
#include "Engine/Asset/Assets/Texture/Texture.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"

namespace ONEngine {
class VoxelTerrain;
}

/// /////////////////////////////////////////////////
/// ボクセル地形を編集するためのパイプラインを起動するクラス
/// /////////////////////////////////////////////////
namespace Editor {

class VoxelTerrainEditorComputePipeline : public IEditorCompute {

	enum ROOT_PARAM {
		CBV_TERRAIN_INFO,
		CBV_VIEW_PROJECTION,
		CBV_CAMERA,
		CBV_INPUT_INFO,
		CBV_EDITOR_INFO,
		C32BIT_CHUNK_ID,
		UAV_MOUSE_POS,
		SRV_CHUNKS,
		SRV_WORLD_TEXTURE,
		UAV_VOXEL_TEXTURES
	};

	enum CALC_MOUSE_POS_ROOT_PARAM {
		C_MOUSE_POS_CBV_INPUT_INFO,
		C_MOUSE_POS_UAV_MOUSE_POS,
		C_MOUSE_POS_SRV_WORLD_TEXTURE
	};

public:
	/// =========================================
	/// public : methods
	/// =========================================

	VoxelTerrainEditorComputePipeline();
	~VoxelTerrainEditorComputePipeline() override;

	void Initialize(ONEngine::ShaderCompiler* _shaderCompiler, ONEngine::DxManager* _dxm) override;
	void Execute(ONEngine::EntityComponentSystem* _ecs, ONEngine::DxCommand* _dxCommand, ONEngine::AssetCollection* _assetCollection) override;

	void ExecuteCalculateMouseWorldPos(ONEngine::DxCommand* dxCommand, ONEngine::AssetCollection* assetCollection);
	
	std::vector<int> GetEditedChunkIDs(ONEngine::VoxelTerrain* vt);

private:
	/// =========================================
	/// private : objects
	/// =========================================

	ONEngine::DxManager* pDxManager_ = nullptr;

	std::unique_ptr<ONEngine::ComputePipeline> calculationMouseWorldPosPipeline_ = nullptr;

	ONEngine::StructuredBuffer<ONEngine::Vector4> uavMousePosBuffer_;
	ONEngine::ConstantBuffer<ONEngine::Vector2> cBufferMouseUV_;
	ONEngine::Vector4 mouseWorldPos_ = ONEngine::Vector4::Zero;
};

} /// Editor
