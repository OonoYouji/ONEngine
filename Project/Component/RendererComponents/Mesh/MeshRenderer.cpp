#include "MeshRenderer.h"


MeshRenderer::MeshRenderer() {
	std::vector<MeshRenderingPipeline::VertexData>& vertices = renderingData_.vertices;
	const size_t&& kVertexDataSize = sizeof(MeshRenderingPipeline::VertexData);

	vertices.push_back({ Vector4(0.0f, 0.5f, 0.0f, 1.0f) });
	vertices.push_back({ Vector4(0.5f, -0.5f, 0.0f, 1.0f) });
	vertices.push_back({ Vector4(-0.5f, -0.5f, 0.0f, 1.0f) });

	renderingData_.vertexBuffer.CreateResource(nullptr, kVertexDataSize * vertices.size());
	renderingData_.vertexBuffer.Get()->Map(0, nullptr, reinterpret_cast<void**>(&renderingData_.mappingData));

	renderingData_.vbv.BufferLocation = renderingData_.vertexBuffer.Get()->GetGPUVirtualAddress();
	renderingData_.vbv.SizeInBytes    = static_cast<UINT>(kVertexDataSize * vertices.size());
	renderingData_.vbv.StrideInBytes  = static_cast<UINT>(sizeof(MeshRenderingPipeline::VertexData));
}

MeshRenderer::~MeshRenderer() {}

void MeshRenderer::PushBackRenderingData(RenderingPipelineCollection* _collection) {
	MeshRenderingPipeline* renderer = _collection->GetRenderingPipeline<MeshRenderingPipeline>();
	renderer->PushBackRenderingData(&renderingData_);
}
