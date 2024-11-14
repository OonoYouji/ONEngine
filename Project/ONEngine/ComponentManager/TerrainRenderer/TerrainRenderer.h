#pragma once

/// std
#include <memory>

/// graphics
#include "GraphicManager/PipelineState/PipelineState.h"

/// base class
#include "ComponentManager/Base/BaseComponent.h"

class TerrainRenderer : public BaseComponent {
public:

	TerrainRenderer();
	~TerrainRenderer();

	void Initialize() override;
	void Update()     override;
	void Draw()       override;
	void Debug()      override;

private:

	/*##########################################################
		TODO : COMMENT
		pipelineを作る、地形を描画
	##########################################################*/

	std::unique_ptr<PipelineState> pipelineState_;
	PipelineState::Shader          shader_;

	D3D12_VERTEX_BUFFER_VIEW vbv_;
	D3D12_INDEX_BUFFER_VIEW  ibv_;



};