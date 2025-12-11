#pragma once

/// engine
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/TerrainVertex.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"

/// editor
#include "../Interface/IEditorCompute.h"

/// /////////////////////////////////////////////////
/// 地形の頂点を編集するためのコンピュートシェーダー
/// /////////////////////////////////////////////////
namespace Editor {

class TerrainVertexEditorCompute : public IEditorCompute {

	struct TerrainInfo {
		int entityId;
	};

	struct InputInfo {
		ONEngine::Vector2 mousePosition; ///< マウスの位置
		float brushSize;                 ///< ブラシのサイズ
		float brushStrength;             ///< ブラシの強さ
		int pressKey;
		int editMode;
		int editTextureIndex;
	};

	enum ROOT_PARAM {
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

	void Initialize(ONEngine::ShaderCompiler* _shaderCompiler, ONEngine::DxManager* _dxm) override;
	void Execute(
		ONEngine::EntityComponentSystem* _ecs,
		ONEngine::DxCommand* _dxCommand,
		ONEngine::AssetCollection* _assetCollection
	) override;

private:
	/// =========================================
	/// private : objects
	/// =========================================

	ONEngine::ConstantBuffer<TerrainInfo> terrainInfo_;
	ONEngine::ConstantBuffer<InputInfo> inputInfo_;
};

} // namespace Editor
