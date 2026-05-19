#pragma once

/// engine
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Asset/Assets/Texture/Texture.h"

#include "../Interface/IEditorCompute.h"
#include "TerrainInfo.h"


namespace Editor {

/// /////////////////////////////////////////////////
/// 地形のデータを出力する
/// /////////////////////////////////////////////////
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

	void Initialize(ONEngine::ShaderCompiler* _shaderCompiler, ONEngine::DxManager* _dxm) override;
	void Execute(ONEngine::EntityComponentSystem* _ecs, ONEngine::DxCommand* _dxCommand, ONEngine::Asset::AssetCollection* _assetCollection) override;

private:
	/// ==========================================
	/// private : objects
	/// ==========================================

	ONEngine::DxManager* pDxManager_;

	ONEngine::ConstantBuffer<TerrainSize> terrainSize_;
	ONEngine::Asset::Texture outputSplatBlendTexture_;
	ONEngine::Asset::Texture outputVertexTexture_;

};

} /// Editor
