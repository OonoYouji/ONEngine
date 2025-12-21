#pragma once

/// engine
#include "../Interface/IEditorCompute.h"
#include "Engine/Asset/Assets/Texture/Texture.h"

/// /////////////////////////////////////////////////
/// ボクセル地形を編集するためのパイプラインを起動するクラス
/// /////////////////////////////////////////////////
namespace Editor {

class VoxelTerrainEditorComputePipeline : public IEditorCompute {

	enum ROOT_PARAM {
		CBV_TERRAIN_INFO,
		CBV_VIEW_PROJECTION,
		CBV_CAMERA,
		CBV_INPUT_INFO,
		CBV_EDITOR_INFO,
		SRV_CHUNKS,
		SRV_WORLD_TEXTURE,
		UAV_VOXEL_TEXTURES
	};

public:
	/// =========================================
	/// public : methods
	/// =========================================

	VoxelTerrainEditorComputePipeline();
	~VoxelTerrainEditorComputePipeline() override;

	void Initialize(ONEngine::ShaderCompiler* _shaderCompiler, ONEngine::DxManager* _dxm) override;
	void Execute(ONEngine::EntityComponentSystem* _ecs, ONEngine::DxCommand* _dxCommand, ONEngine::AssetCollection* _assetCollection) override;

	void EditStart();
	void EditEnd();

private:
	/// =========================================
	/// private : objects
	/// =========================================

	ONEngine::DxManager* pDxManager_ = nullptr;

	uint32_t editCount_;
	uint32_t maxEditCount_;

	/// 地形が編集される毎に地形テクスチャのコピーを保存しておくためのテクスチャ
	/// 複数回のUndo/Redoに対応するためこのテクスチャは地形のn倍の大きさで用意しておく
	/// n = maxEditCount_
	ONEngine::Texture editedTexture_;

};

} /// Editor
