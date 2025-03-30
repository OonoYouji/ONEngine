#pragma once

///std
#include <array>
#include <string>

/// engine
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
	void Execute(class DxCommand* _dxCommand, class GraphicsResourceCollection* _resourceCollection) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::array<size_t, 4> textureIndices_;
};

