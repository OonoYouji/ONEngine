#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Graphics/Shader/ComputePipeline.h"

namespace ONEngine {

class AssetCollection;

class VoxelTerrainVertexCreatePipeline : public IRenderingPipeline {

	enum ROOT_PARAM {
		CBV_VOXEL_TERRAIN_INFO,
		CBV_MARCHING_CUBE,
		BIT32_CHUNK_INDEX,
		SRV_CHUNKS,
		APPEND_OUT_VERTICES,
		SRV_VOXEL_TEXTURES,
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	VoxelTerrainVertexCreatePipeline(AssetCollection* _ac);
	~VoxelTerrainVertexCreatePipeline();

	/// --------------- override methods --------------- ///
	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void Draw(class ECSGroup* _ecs, class CameraComponent* _camera, DxCommand* _dxCommand) override;

private:

	std::unique_ptr<ComputePipeline> computePipeline_;
	AssetCollection* pAssetCollection_;


};

} /// namespace ONEngine