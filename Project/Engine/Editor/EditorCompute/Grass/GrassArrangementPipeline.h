#pragma once

/// engine
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"

/// interface
#include "../Interface/IEditorCompute.h"


/// ///////////////////////////////////////////////////// 
/// 草の配置を行うシェーダーの起動を行うクラス
/// ///////////////////////////////////////////////////// 
class GrassArrangementPipeline : public IEditorCompute {
public:

	enum ROOT_PARAM {
		CBV_USED_TEXTURED_IDS,
		UAV_BLADE_INSTANCES,
		SRV_TEXTURES,
	};

	struct UsedTextureIDs {
		uint32_t grassArrangementTexId;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	GrassArrangementPipeline();
	~GrassArrangementPipeline();

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;
	void Execute(class EntityComponentSystem* _ecs, class DxCommand* _dxCommand, class GraphicsResourceCollection* _resourceCollection) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	ConstantBuffer<UsedTextureIDs> usedTexIdBuffer_;

};