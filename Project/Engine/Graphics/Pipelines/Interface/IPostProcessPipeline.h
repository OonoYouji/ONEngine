#pragma once

/// std
#include <memory>

/// engine
#include "Engine/Graphics/Shader/ComputePipeline.h"
#include "Engine/Graphics/Shader/ShaderCompiler.h"

/// ///////////////////////////////////////////////////
/// PostProcessのinterfaceクラス
/// ///////////////////////////////////////////////////
class IPostProcessPipeline {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	virtual ~IPostProcessPipeline() = default;

	/// @brief pipelineの初期化を行う
	/// @param _shaderCompiler ShaderCompilerへのポインタ
	/// @param _dxManager DxManagerへのポインタ
	virtual void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) = 0;

	/// @brief post processの実行
	virtual void Execute(class DxCommand* _dxCommand) = 0;

protected:
	/// ===================================================
	/// protected : objects
	/// ===================================================

	std::unique_ptr<ComputePipeline> pipeline_;

};

