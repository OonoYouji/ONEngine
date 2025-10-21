#pragma once

/// std
#include <memory>
#include <array>

/// engine
#include "../../../Interface/IPostProcessPipeline.h"

/// ///////////////////////////////////////////////////
/// オブジェクト毎のガウスブラー処理
/// ///////////////////////////////////////////////////
class PostProcessGaussianBlurPerObject : public PerObjectPostProcess {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	PostProcessGaussianBlurPerObject() = default;
	~PostProcessGaussianBlurPerObject() override = default;

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
	
	std::array<size_t, 2> textureIndices_; ///< テクスチャのインデックス

};

