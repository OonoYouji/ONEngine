#pragma once

/// engine
#include "../Interface/IRenderingPipeline.h"

/// ===================================================d
/// sprite描画のパイプライン
/// ===================================================d
class SpriteRenderingPipeline final : public IRenderingPipeline {
public:
	
	/// ===================================================
	/// public : methods
	/// ===================================================

	SpriteRenderingPipeline(class GraphicsResourceCollection* _resourceCollection);
	~SpriteRenderingPipeline();


	/// @brief 初期化処理
	/// @param _shaderCompiler ShaderCompilerへのポインタ
	/// @param _dxManager DxManagerへのポインタ
	void Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) override;

	/// @brief 描画処理
	/// @param _dxCommand DxCommandへのポインタ
	/// @param _entityCollection EntityCollectionへのポインタ
	void Draw(DxCommand* _dxCommand, EntityCollection* _entityCollection) override;


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class GraphicsResourceCollection* resourceCollection_ = nullptr;

};

