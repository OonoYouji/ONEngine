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

	/// @brief rendering pipelineに描画データを追加
	/// @param _collection pipelineのコレクションクラスへのポインタ
	void PushBackRenderingData(class RenderingPipelineCollection* _collection) override;

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

