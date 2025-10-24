#pragma once

/// engine
#include "../Interface/IEditorCompute.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/TerrainVertex.h"

/// /////////////////////////////////////////////////
/// 地形の頂点を編集するためのコンピュートシェーダー
/// /////////////////////////////////////////////////
class TerrainVertexEditorCompute : public IEditorCompute {

	struct TerrainInfo {
		int entityId;
	};

	struct InputInfo {
		Vector2 mousePosition; ///< マウスの位置
		float brushSize;       ///< ブラシのサイズ
		float brushStrength;   ///< ブラシの強さ
		int pressKey;
		int editMode;
		int editTextureIndex;
	};

	enum RootParameter {
		CBV_TERRAIN_INFO,
		CBV_INPUT_INFO,
		UAV_VERTICES,
		SRV_POSITION_TEXTURE,
		SRV_FLAG_TEXTURE,
	};

public:
	/// =========================================
	/// public : methods
	/// =========================================

	TerrainVertexEditorCompute();
	~TerrainVertexEditorCompute() override;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void Execute(class EntityComponentSystem* _ecs, class DxCommand* _dxCommand, class AssetCollection* _assetCollection) override;

private:
	/// =========================================
	/// private : objects
	/// =========================================

	ConstantBuffer<TerrainInfo> terrainInfo_;
	ConstantBuffer<InputInfo> inputInfo_;

};

