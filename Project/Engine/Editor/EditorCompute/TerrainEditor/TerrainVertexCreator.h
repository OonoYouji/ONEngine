#pragma once

/// engine
#include "../Interface/IEditorCompute.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "TerrainInfo.h"

/// //////////////////////////////////////////
/// 地形の頂点を生成するためのコンピュートシェーダー
/// //////////////////////////////////////////
class TerrainVertexCreator : public IEditorCompute {

	enum {
		CBV_TERRAIN_SIZE,
		UAV_VERTICES,
		UAV_INDICES,
	};

public:
	/// =========================================
	/// public : methods
	/// =========================================

	TerrainVertexCreator();
	~TerrainVertexCreator() override;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;
	void Execute(class EntityComponentSystem* _ecs, class DxCommand* _dxCommand, class GraphicsResourceCollection* _resourceCollection) override;

private:
	/// =========================================
	/// private : objects
	/// =========================================
	
	class DxManager* pDxManager_;
	ConstantBuffer<TerrainSize> terrainSize_;

};

