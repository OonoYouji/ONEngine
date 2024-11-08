#pragma once

/// std
#include <memory>

/// engine
#include "GraphicManager/PipelineState/PipelineState.h"

#include "NumberRenderer.h"


class NumberRendererCommon final {
	NumberRendererCommon()  = default;
	~NumberRendererCommon() = default;
public:

	static NumberRendererCommon* GetInstance() {
		static NumberRendererCommon instance;
		return &instance;
	}


	void Initialize();
	void Finalize();

	void PreDraw();
	void PostDraw();

	void AddActive(NumberRenderer* _numberRenderer);

private:

	std::unique_ptr<PipelineState>        pipelineState_ = nullptr;
	PipelineState::Shader                 shader_        = {};

	/// vertex buffer
	std::vector<NumberRendererVertexData> vertices_;
	ComPtr<ID3D12Resource>                vertexBuffer_  = nullptr;
	D3D12_VERTEX_BUFFER_VIEW              vbv_           = {};

	/// index buffer
	std::vector<uint32_t>                 indices_;
	ComPtr<ID3D12Resource>                indexBuffer_   = nullptr;
	D3D12_INDEX_BUFFER_VIEW               ibv_           = {};


	std::list<NumberRenderer*>            activeList_    = {};

};