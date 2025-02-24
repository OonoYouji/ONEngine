#pragma once

/// std
#include <vector>

/// engine
#include "../Interface/IRenderingPipeline.h"
#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "Engine/Core/Utility/Math/Vector4.h"
#include "Engine/Core/Utility/Math/Vector2.h"

/// ===================================================d
/// sprite描画のパイプライン
/// ===================================================d
class SpriteRenderingPipeline final : public IRenderingPipeline {
public:

	/// ===================================================
	/// public : sub class
	/// ===================================================

	/// @brief 頂点データ
	struct VertexData {
		Vector4 position;
		Vector2 uv;
	};


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

	std::vector<VertexData>  vertices_;
	DxResource               vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbv_;

	std::vector<uint32_t>    indices_;
	DxResource               indexBuffer_;
	D3D12_INDEX_BUFFER_VIEW  ibv_;

};

