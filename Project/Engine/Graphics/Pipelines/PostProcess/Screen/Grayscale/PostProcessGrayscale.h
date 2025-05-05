#pragma once

/// std
#include <array>

/// engine
#include "Engine/Graphics/Pipelines/Interface/IPostProcessPipeline.h"

/// ///////////////////////////////////////////////////
/// グレースケール処理
/// ///////////////////////////////////////////////////
class PostProcessGrayscale : public ScreenPostProcess {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;

	void Execute(
		class DxCommand* _dxCommand,
		class GraphicsResourceCollection* _resourceCollection,
		class EntityComponentSystem* _entityComponentSystem
	) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::array<size_t, 2> textureIndices_;
};

