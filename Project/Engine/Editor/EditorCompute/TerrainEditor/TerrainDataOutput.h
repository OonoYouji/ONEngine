#pragma once

/// engine
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Asset/Assets/Texture/Texture.h"

#include "../Interface/IEditorCompute.h"
#include "TerrainInfo.h"


/// /////////////////////////////////////////////////
/// 地形のデータを出力する
/// /////////////////////////////////////////////////
namespace ONEngine {

class TerrainDataOutput : public IEditorCompute {

	enum {
		CBV_TERRAIN_SIZE,
		UAV_VERTICES,
		UAV_OUTPUT_VERTEX_TEXTURE,
		UAV_OUTPUT_SPLAT_BLEND_TEXTURE
	};

public:
	/// ==========================================
	/// public : methods
	/// ==========================================

	TerrainDataOutput();
	~TerrainDataOutput() override;

	void Initialize(class ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void Execute(class EntityComponentSystem* _ecs, class DxCommand* _dxCommand, class AssetCollection* _assetCollection) override;

private:
	/// ==========================================
	/// private : objects
	/// ==========================================

	class DxManager* pDxManager_;

	ConstantBuffer<TerrainSize> terrainSize_;
	Texture outputVertexTexture_;
	Texture outputSplatBlendTexture_;

};


} /// ONEngine
