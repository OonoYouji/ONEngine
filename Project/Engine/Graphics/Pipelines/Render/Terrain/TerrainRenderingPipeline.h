#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"

#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/VertexBuffer.h"
#include "Engine/Graphics/Buffer/IndexBuffer.h"
#include "Engine/Graphics/Buffer/Data/GPUMaterial.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"

/// ////////////////////////////////////
/// Terrainの描画pipeline
/// ////////////////////////////////////
class TerrainRenderingPipeline : public IRenderingPipeline {
public:
	/// =====================================
	/// public : sub class
	/// =====================================

	enum ROOT_PARAM {
		ROOT_PARAM_VIEW_PROJECTION,
		ROOT_PARAM_TRANSFORM,
		ROOT_PARAM_TEX_GRASS,
		ROOT_PARAM_TEX_DIRT,
		ROOT_PARAM_TEX_ROCK,
		ROOT_PARAM_TEX_SNOW,
		ROOT_PARAM_MATERIAL,
	};



public:
	/// ====================================
	/// public : methods
	/// ====================================

	TerrainRenderingPipeline(class AssetCollection* _resourceCollection);
	~TerrainRenderingPipeline();

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;
	void Draw(class ECSGroup* _ecs, const std::vector<GameEntity*>& _entities, class CameraComponent* _camera, DxCommand* _dxCommand) override;

private:
	/// ====================================
	/// private : objects
	/// ====================================

	class AssetCollection* pAssetCollection_;
	Terrain* pTerrain_;

	ConstantBuffer<Matrix4x4> transformBuffer_;
	StructuredBuffer<GPUMaterial> materialBuffer_;

};

