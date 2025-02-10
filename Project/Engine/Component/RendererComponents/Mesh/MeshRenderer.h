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

	size_t meshId_;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	void SetMeshId(size_t _meshId) {
		meshId_ = _meshId;
	}


	size_t GetMeshId() const {
		return meshId_;
	}

};

