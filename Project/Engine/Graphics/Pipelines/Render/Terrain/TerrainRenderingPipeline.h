#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"

#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/VertexBuffer.h"
#include "Engine/Graphics/Buffer/IndexBuffer.h"
#include "Engine/Graphics/Buffer/Data/Material.h"
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
		ROOT_PARAM_MATERIAL,
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
	void Draw(class EntityComponentSystem* _ecs, const std::vector<IEntity*>& _entities, class Camera* _camera, DxCommand* _dxCommand) override;

private:
	/// ====================================
	/// private : objects
	/// ====================================

	class GraphicsResourceCollection* pResourceCollection_;
	Terrain* pTerrain_;

	ConstantBuffer<Matrix4x4> transformBuffer_;
	ConstantBuffer<Material> materialBuffer_;

	IndexBuffer indexBuffer_;
	VertexBuffer<TerrainVertex> vertexBuffer_;


};

