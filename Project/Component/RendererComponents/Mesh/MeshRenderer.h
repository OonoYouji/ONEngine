#pragma once

/// engine
#include "../../IComponent.h"
#include "Engine/Graphics/Pipelines/Mesh/MeshRenderingPipeline.h"

/// ===================================================
/// mesh描画クラス
/// ===================================================
class MeshRenderer : public IRenderComponent {
public:

	MeshRenderer();
	~MeshRenderer();

	void PushBackRenderingData(RenderingPipelineCollection* _collection) override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	MeshRenderingPipeline::RenderingData renderingData_;

};

