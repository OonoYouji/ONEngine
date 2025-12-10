#pragma once

/// std
#include <list>

/// engine
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Core/Utility/Math/Vector4.h"
#include "Engine/Core/DirectX12/Resource/DxResource.h"


/// /////////////////////////////////////////////////
/// 3Dライン描画クラス
/// /////////////////////////////////////////////////
namespace ONEngine {

class Line3DRenderingPipeline : public IRenderingPipeline {
public:

	struct VertexData {
		Vector4 position;
		Vector4 color;
	};

	struct RenderingData {
		std::vector<VertexData> vertices;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Line3DRenderingPipeline();
	~Line3DRenderingPipeline();

	/// @brief 初期化処理
	/// @param _shaderCompiler shaderのコンパイラー
	/// @param _dxDevice DxDeviceへのポインタ
	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;

	/// @brief 描画処理
	/// @param _dxCommand DxCommandへのポインタ
	/// @param _entityCollection EntityCollectionへのポインタ
	void Draw(class ECSGroup* _ecs, class CameraComponent* _camera, DxCommand* _dxCommand) override;


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	const size_t kMaxVertexNum_ = 1024; ///< 最大ライン数

	DxResource                  vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW    vbv_;
	VertexData* mappingData_;
	std::vector<VertexData>     vertices_;

	std::list<RenderingData*>   renderingDataList_;
};


} /// ONEngine
