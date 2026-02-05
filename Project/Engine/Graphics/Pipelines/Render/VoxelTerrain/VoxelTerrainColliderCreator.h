#pragma once

/// std
#include <memory>

/// engine
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Graphics/Shader/ComputePipeline.h"
#include <Engine/Core/DirectX12/Command/DxCommand.h>

namespace ONEngine {

/// ///////////////////////////////////////////////////
/// ボクセル地形のコライダー生成用のシステム
/// ///////////////////////////////////////////////////
class VoxelTerrainColliderCreator : public IRenderingPipeline {

	enum ROOT_PARAM {
		CBV_VOXEL_TERRAIN_INFO,
		CBV_GENERATE_PARAMS,
		CBV_CHUNK_ID_C32BIT,
		UAV_VERTICES,
		SRV_CHUNKS,
		SRV_CHUNK_TEXTURE3D,
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	VoxelTerrainColliderCreator();
	~VoxelTerrainColliderCreator() = default;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void Draw(class ECSGroup* _ecs, class CameraComponent* _camera, DxCommand* _dxCommand) override;


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<ComputePipeline> computePipeline_;

};


} /// namespace ONEngine