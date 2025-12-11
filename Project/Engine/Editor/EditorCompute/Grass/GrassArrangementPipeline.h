#pragma once

/// engine
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"

/// interface
#include "../Interface/IEditorCompute.h"


namespace Editor {

/// ///////////////////////////////////////////////////// 
/// 草の配置を行うシェーダーの起動を行うクラス
/// ///////////////////////////////////////////////////// 
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

	void Initialize(ONEngine::ShaderCompiler* _shaderCompiler, ONEngine::DxManager* _dxm) override;
	void Execute(ONEngine::EntityComponentSystem* _ecs, ONEngine::DxCommand* _dxCommand, ONEngine::AssetCollection* _assetCollection) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	ONEngine::ConstantBuffer<UsedTextureIDs> usedTexIdBuffer_;

};

} /// Editor
