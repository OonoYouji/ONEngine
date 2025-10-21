#pragma once

/// std
#include <array>

/// engine
#include "Engine/Graphics/Pipelines/Interface/IPostProcessPipeline.h"

/// ///////////////////////////////////////////////////
/// オブジェクト単位でのグレースケール処理
/// ///////////////////////////////////////////////////
class PostProcessGrayscalePerObject : public PerObjectPostProcess {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;

	void Execute(
		const std::string& _textureName,
		class DxCommand* _dxCommand,
		class AssetCollection* _resourceCollection,
		class EntityComponentSystem* _entityComponentSystem
	) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::array<size_t, 3> textureIndices_;
};

