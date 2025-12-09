#pragma once

/// engine
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"

/// interface
#include "../Interface/IEditorCompute.h"


/// ///////////////////////////////////////////////////// 
/// 草の配置を行うシェーダーの起動を行うクラス
/// ///////////////////////////////////////////////////// 
namespace ONEngine {

class GrassArrangementPipeline : public IEditorCompute {
public:

	enum ROOT_PARAM {
		CBV_USED_TEXTURED_IDS,
		C32BIT_CONSTANTS,
		UAV_BLADE_INSTANCES,
		SRV_TEXTURES,
	};

	struct UsedTextureIDs {
		uint32_t grassArrangementTexId;
		uint32_t terrainVertexTexId;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	GrassArrangementPipeline();
	~GrassArrangementPipeline();

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void Execute(class EntityComponentSystem* _ecs, class DxCommand* _dxCommand, class AssetCollection* _assetCollection) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	ConstantBuffer<UsedTextureIDs> usedTexIdBuffer_;

};

} /// ONEngine
