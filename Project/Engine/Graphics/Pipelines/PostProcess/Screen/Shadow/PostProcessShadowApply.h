#pragma once

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Pipelines/Interface/IPostProcessPipeline.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"

/// ///////////////////////////////////////////////////
/// 影を適用するためのポストプロセス
/// ///////////////////////////////////////////////////
class PostProcessShadowApply : public ScreenPostProcess {

	enum ROOT_PARAM {
		CBV_VIEW_PROJECTION,
		CBV_SHADOW_PARAM,
		SRV_SCENE_COLOR,
		SRV_SCENE_DEPTH,
		SRV_SHADOW_MAP,
		SRV_WORLD_POSITION,
		UAV_OUTPUT_COLOR,
	};

	struct ShadowParameter {
		Vector2 screenSize;
		Vector2 texelSizeShadow;
		float shadowBias;
		float shadowDarkness;
		int pcfRadius;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/// @brief pipelineの初期化を行う
	/// @param _shaderCompiler ShaderCompilerへのポインタ
	/// @param _dxm DxManagerへのポインタ
	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	
	/// @brief post processの実行
	void Execute(
		const std::string& _textureName,
		class DxCommand* _dxCommand,
		class AssetCollection* _assetCollection,
		class EntityComponentSystem* _ecs
	) override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================
	
	ConstantBuffer<ShadowParameter> shadowParamBuffer_;

};

