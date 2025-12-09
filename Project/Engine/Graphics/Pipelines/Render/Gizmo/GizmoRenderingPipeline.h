#pragma once

/// std
#include <unordered_map>
#include <list>

/// engine
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "GizmoPrimitiveVertices.h"

/// ///////////////////////////////////////////////////
/// gizmoの表示pipeline
/// ///////////////////////////////////////////////////
class GizmoRenderingPipeline : public IRenderingPipeline {
private:

	enum {
		Solid,
		Wire,
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	GizmoRenderingPipeline();
	~GizmoRenderingPipeline() = default;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void Draw(class ECSGroup* _ecs, class CameraComponent* _camera, DxCommand* _dxCommand) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// solid, wire frame の描画pipeline
	std::unordered_map<size_t, std::unique_ptr<GraphicsPipeline>> pipelines_;

	size_t maxVertexNum_; ///< 最大ライン数

	DxResource vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbv_;
	GizmoPrimitive::VertexData* mappingData_;
	std::vector<GizmoPrimitive::VertexData> vertices_;

};

