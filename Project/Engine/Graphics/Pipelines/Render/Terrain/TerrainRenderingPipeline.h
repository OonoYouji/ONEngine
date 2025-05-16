#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Core/Utility/Utility.h"

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
	};



public:
	/// ====================================
	/// public : methods
	/// ====================================

	TerrainRenderingPipeline(class GraphicsResourceCollection* _resourceCollection);
	~TerrainRenderingPipeline();

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;
	void Draw(DxCommand* _dxCommand, class EntityComponentSystem* _pEntityComponentSystem, class Camera* _camera) override;

private:
	/// ====================================
	/// private : objects
	/// ====================================

	class GraphicsResourceCollection* pResourceCollection_;

	class Terrain* pTerrain_ = nullptr;

	ConstantBuffer<Matrix4x4> transformBuffer_;




};

