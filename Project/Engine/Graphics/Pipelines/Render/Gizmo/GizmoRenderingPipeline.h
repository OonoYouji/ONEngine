#pragma once

/// std
#include <unordered_map>
#include <list>

/// engine
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "Engine/Core/Utility/Utility.h"

/// ///////////////////////////////////////////////////
/// gizmoの表示pipeline
/// ///////////////////////////////////////////////////
class GizmoRenderingPipeline : public IRenderingPipeline {
private:

	struct VertexData {
		Vector4 position;
		Vector4 color;
	};

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

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;
	void Draw(const std::vector<IEntity*>& _entities, class Camera* _camera, DxCommand* _dxCommand) override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	std::vector<VertexData> GetSphereVertices(const Vector3& _center, float _radius, const Vector4& _color, size_t _segment = 24);
	std::vector<VertexData> GetCubeVertices(const Vector3& _center, const Vector3& _size, const Vector4& _color);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// solid, wire frame の描画pipeline
	std::unordered_map<size_t, std::unique_ptr<GraphicsPipeline>> pipelines_;

	size_t maxVertexNum_; ///< 最大ライン数

	DxResource vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbv_;
	VertexData* mappingData_;
	std::vector<VertexData> vertices_;

};

