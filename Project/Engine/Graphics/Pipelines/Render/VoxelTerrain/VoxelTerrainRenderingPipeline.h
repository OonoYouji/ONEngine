#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"

#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"
#include "Engine/Graphics/Buffer/VertexBuffer.h"
#include "Engine/Graphics/Buffer/IndexBuffer.h"
#include "Engine/Graphics/Buffer/Data/GPUMaterial.h"
#include <d3d12.h>

/// /////////////////////////////////////////////////
/// Terrainの描画pipeline
/// /////////////////////////////////////////////////
namespace ONEngine {

class VoxelTerrainRenderingPipeline : public IRenderingPipeline {


	enum ROOT_PARAM {
		CBV_VOXEL_TERRAIN_INFO,
		CBV_VIEW_PROJECTION,
		CBV_CAMERA_POSITION,
		CBV_MATERIAL,
		SRV_CHUNK_ARRAY,
		SRV_VOXEL_TERRAIN_TEXTURE3D,
		//SRV_TEXTURES,
	};

	struct CommandInfo {
		Vector3Int dispatchSize;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	VoxelTerrainRenderingPipeline(class AssetCollection* _assetCollection);
	~VoxelTerrainRenderingPipeline();

	/// --------------- override methods --------------- ///
	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void Draw(class ECSGroup* _ecs, class CameraComponent* _camera, DxCommand* _dxCommand) override;

	void DrawCubic(class VoxelTerrain* vt, CameraComponent* camera, DxCommand* dxCommand);

	void CreatePipeline(GraphicsPipeline* _pipeline, Shader& _shader, DxManager* _dxm, D3D12_FILL_MODE _fillMode, D3D12_BLEND_DESC _blendMode);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// --------------- other class pointers --------------- ///
	class AssetCollection* pAssetCollection_;
	class DxManager* pDxManager_;

	std::unique_ptr<GraphicsPipeline> wireframeSubtractBlendPipeline_;
	std::unique_ptr<GraphicsPipeline> wireframePipeline_;
	std::unique_ptr<GraphicsPipeline> cubicPipeline_;

};


} /// ONEngine
