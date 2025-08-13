#pragma once

/// engine
#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "../../Interface/IRenderingPipeline.h"
#include "../Gizmo/GizmoPrimitiveVertices.h"

/// ///////////////////////////////////////////////////
/// SkinMeshの骨を描画するレンダリングパイプライン
/// ///////////////////////////////////////////////////
class SkinMeshSkeletonRenderingPipeline : public IRenderingPipeline{
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	SkinMeshSkeletonRenderingPipeline();
	~SkinMeshSkeletonRenderingPipeline() override = default;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;
	void Draw(class EntityComponentSystem* _ecs, const std::vector<class GameEntity*>& _entities, class CameraComponent* _camera, DxCommand* _dxCommand) override;


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	size_t maxVertexNum_; ///< 最大ライン数

	DxResource vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbv_;
	GizmoPrimitive::VertexData* mappingData_;
	std::vector<GizmoPrimitive::VertexData> vertices_;

};

