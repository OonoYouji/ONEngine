#pragma once

/// engine
#include "../Interface/IEditorCompute.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "TerrainInfo.h"

namespace Editor {

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

	void Initialize(ONEngine::ShaderCompiler* _shaderCompiler, ONEngine::DxManager* _dxm) override;
	void Execute(ONEngine::EntityComponentSystem* _ecs, ONEngine::DxCommand* _dxCommand, ONEngine::AssetCollection* _assetCollection) override;

private:
	/// =========================================
	/// private : objects
	/// =========================================
	
	ONEngine::DxManager* pDxManager_;
	ONEngine::ConstantBuffer<TerrainSize> terrainSize_;

};


} /// Editor
