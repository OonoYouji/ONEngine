#pragma once

/// std
#include <array>

/// engine
#include "Engine/Graphics/Pipelines/Interface/IPostProcessPipeline.h"

/// ///////////////////////////////////////////////////
/// オブジェクト単位でのグレースケール処理
/// ///////////////////////////////////////////////////
namespace ONEngine {

class PostProcessGrayscalePerObject : public PerObjectPostProcess {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	void Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) override;

	void Execute(
		const std::string& _textureName,
		DxCommand* _dxCommand,
		Asset::AssetCollection* _assetCollection,
		EntityComponentSystem* _entityComponentSystem
	) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::array<size_t, 3> textureIndices_;
};


} /// ONEngine
