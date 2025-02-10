#pragma once

/// engine
#include "../../IComponent.h"
#include "Engine/Graphics/Pipelines/Mesh/MeshRenderingPipeline.h"

/// ===================================================
/// mesh描画クラス
/// ===================================================
class MeshRenderer : public IRenderComponent {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	MeshRenderer();
	~MeshRenderer();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	MeshRenderingPipeline::RenderingData renderingData_;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	void SetMeshId(size_t _meshId) {
		renderingData_.renderMeshId = _meshId;
	}

};

