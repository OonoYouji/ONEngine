#pragma once

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Pipelines/Interface/IPostProcessPipeline.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/ECS/Component/Components/ComputeComponents/ShadowCaster/ShadowCaster.h"

/// ///////////////////////////////////////////////////
/// 影を適用するためのポストプロセス
/// ///////////////////////////////////////////////////
namespace ONEngine {

class PostProcessShadowApply : public ScreenPostProcess {

	enum ROOT_PARAM {
		CBV_VIEW_PROJECTION,
		CBV_SHADOW_PARAM,
		SRV_SCENE_COLOR,
		SRV_SHADOW_MAP,
		SRV_WORLD_POSITION,
		SRV_FLAGS,
		UAV_OUTPUT_COLOR,
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/// @brief pipelineの初期化を行う
	/// @param _shaderCompiler ShaderCompilerへのポインタ
	/// @param _dxm DxManagerへのポインタ
	void Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) override;
	
	/// @brief post processの実行
	void Execute(
		const std::string& _textureName,
		DxCommand* _dxCommand,
		Asset::AssetCollection* _assetCollection,
		EntityComponentSystem* _ecs
	) override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================
	
	DxManager* pDxManager_ = nullptr;

	ConstantBuffer<ShadowParameter> shadowParamBuffer_;

};


} /// ONEngine
