#pragma once

/// std
#include <array>

/// engine
#include "Engine/Graphics/Pipelines/Interface/IPostProcessPipeline.h"

/// ///////////////////////////////////////////////////
/// ラジアルブラー処理
/// ///////////////////////////////////////////////////
class PostProcessRadialBlur : public IPostProcessPipeline {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	PostProcessRadialBlur();
	~PostProcessRadialBlur() override;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;

	void Execute(
		const std::string& _textureName,
		class DxCommand* _dxCommand,
		class GraphicsResourceCollection* _resourceCollection,
		class EntityComponentSystem* _entityComponentSystem
	) override;


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::array<size_t, 3> textureIndices_; ///< テクスチャのインデックス
};

