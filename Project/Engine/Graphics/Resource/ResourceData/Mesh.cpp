#include "Mesh.h"

/// engine
#include "Engine/Core/DirectX12/Device/DxDevice.h"


Mesh::Mesh() {}
Mesh::~Mesh() {}

void Mesh::CreateBuffer(DxDevice* _dxDevice) {

	const size_t kVertexDataSize = sizeof(VertexData);

	/// vertex buffer
	vertexBuffer_.CreateResource(_dxDevice, kVertexDataSize * vertices_.size());

	vbv_.BufferLocation = vertexBuffer_.Get()->GetGPUVirtualAddress();
	vbv_.SizeInBytes    = static_cast<UINT>(kVertexDataSize * vertices_.size());
	vbv_.StrideInBytes  = static_cast<UINT>(kVertexDataSize);

	/// index buffer
	indexBuffer_.CreateResource(_dxDevice, sizeof(uint32_t) * indices_.size());

	ibv_.BufferLocation = indexBuffer_.Get()->GetGPUVirtualAddress();
	ibv_.SizeInBytes    = static_cast<UINT>(sizeof(uint32_t) * indices_.size());
	ibv_.Format         = DXGI_FORMAT_R32_UINT;

	/// mapping
	VertexBufferMapping();
	IndexBufferMapping();

}

void Mesh::VertexBufferMapping() {
	VertexData* mappingData = nullptr;
	vertexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappingData));
	std::memcpy(mappingData, vertices_.data(), sizeof(VertexData) * vertices_.size());
}

void Mesh::IndexBufferMapping() {
	uint32_t* mappingData = nullptr;
	indexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappingData));
	std::memcpy(mappingData, indices_.data(), sizeof(uint32_t) * indices_.size());
}


void Mesh::SetVertices(const std::vector<VertexData>& _vertices) {
	vertices_ = _vertices;
}

void Mesh::SetIndices(const std::vector<uint32_t>& _indices) {
	indices_ = _indices;
}
