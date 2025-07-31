#pragma once

/// engine
#include "../Interface/IEditorCompute.h"

/// //////////////////////////////////////////
/// 地形の頂点を編集するためのコンピュートシェーダー
/// //////////////////////////////////////////
class TerrainVertexEditorCompute : public IEditorCompute {
public:
	/// =========================================
	/// public : methods
	/// =========================================

	TerrainVertexEditorCompute();
	~TerrainVertexEditorCompute() override;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;
	void Draw(class DxCommand* _dxCommand, class GraphicsResourceCollection* _resourceCollection) override;

private:
	/// =========================================
	/// private : objects
	/// =========================================


};

