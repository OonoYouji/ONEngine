#pragma once

#include "Engine/Graphics/Pipelines/Interface/IPostProcessPipeline.h"

/// ///////////////////////////////////////////////////
/// PostProcessLighting
/// ///////////////////////////////////////////////////
class PostProcessLighting : public IPostProcessPipeline {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	PostProcessLighting();
	~PostProcessLighting();
	
	/// @brief pipelineの初期化を行う
	/// @param _shaderCompiler ShaderCompilerへのポインタ
	/// @param _dxManager DxManagerへのポインタ
	void Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) override;
	
	/// @brief post processの実行
	void Execute(class DxCommand* _dxCommand) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

};

