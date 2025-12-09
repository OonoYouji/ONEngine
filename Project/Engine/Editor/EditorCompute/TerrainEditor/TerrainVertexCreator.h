#pragma once

/// engine
#include "../Interface/IEditorCompute.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "TerrainInfo.h"

/// /////////////////////////////////////////////////
/// 地形の頂点を生成するためのコンピュートシェーダー
/// /////////////////////////////////////////////////
class TerrainVertexCreator : public IEditorCompute {

	enum {
		CBV_TERRAIN_SIZE,
		UAV_VERTICES,
		UAV_INDICES,
		SRV_VERTEX_TEXTURE,
		SRV_SPLAT_BLEND_TEXTURE,
	};

public:
	/// =========================================
	/// public : methods
	/// =========================================

	TerrainVertexCreator();
	~TerrainVertexCreator() override;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void Execute(class EntityComponentSystem* _ecs, class DxCommand* _dxCommand, class AssetCollection* _assetCollection) override;

private:
	/// =========================================
	/// private : objects
	/// =========================================
	
	class DxManager* pDxManager_;
	ConstantBuffer<TerrainSize> terrainSize_;

};

