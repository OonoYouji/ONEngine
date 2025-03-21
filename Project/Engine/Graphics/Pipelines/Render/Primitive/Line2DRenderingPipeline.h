#pragma once

/// std
#include <list>

/// engine
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Core/Utility/Math/Vector4.h"
#include "Engine/Core/DirectX12/Resource/DxResource.h"


/// ===================================================
/// 2Dライン描画クラス
/// ===================================================
class Line2DRenderingPipeline : public IRenderingPipeline {
public:

	/// ===================================================
	/// public : nest class
	/// ===================================================

	struct VertexData {
		Vec4 position;
		Vec4 color;
	};

	struct RenderingData final {
		std::vector<VertexData> vertices;
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Line2DRenderingPipeline();
	~Line2DRenderingPipeline();

	/// @brief 初期化関数
	/// @param _shaderCompiler シェーダーコンパイラーへのポインタ
	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;

	/// @brief 描画処理を行う
	/// @param _dxCommand DxCommandへのポインタ
	/// @param _entityCollection EntityCollectionへのポインタ
	void Draw(DxCommand* _dxCommand, EntityCollection* _entityCollection) override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	const size_t kMaxVertexNum_ = 1024; ///< 最大ライン数

	DxResource                  vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW    vbv_;
	VertexData*                 mappingData_  = nullptr;
	std::vector<VertexData>     vertices_;

	std::list<RenderingData*>   renderingDataList_;	

};

