#include "Mesh.h"

/// engine
#include "Engine/Core/DirectX12/Device/DxDevice.h"


Mesh::Mesh() = default;
Mesh::~Mesh() = default;

void Mesh::CreateBuffer(DxDevice* _dxDevice) {
	/// ----- vbv, ibv のBufferを生成、Mapする ----- ///

	const size_t kVertexDataSize = sizeof(VertexData);

	/// vertex buffer
	vertexBuffer_.CreateResource(_dxDevice, kVertexDataSize * vertices_.size());
	vbv_.BufferLocation = vertexBuffer_.Get()->GetGPUVirtualAddress();
	vbv_.SizeInBytes = static_cast<UINT>(kVertexDataSize * vertices_.size());
	vbv_.StrideInBytes = static_cast<UINT>(kVertexDataSize);

	/// index buffer
	indexBuffer_.CreateResource(_dxDevice, sizeof(uint32_t) * indices_.size());
	ibv_.BufferLocation = indexBuffer_.Get()->GetGPUVirtualAddress();
	ibv_.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * indices_.size());
	ibv_.Format = DXGI_FORMAT_R32_UINT;


	/// mapping
	VertexBufferMapping();
	IndexBufferMapping();
}

void Mesh::VertexBufferMapping() {
	/// ----- 頂点データをGPUで使用するためにMapする ----- ///
	vertexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappingVertexData_));
	std::memcpy(mappingVertexData_, vertices_.data(), sizeof(VertexData) * vertices_.size());
}

void Mesh::IndexBufferMapping() {
	/// ----- インデックスデータをGPUで使用するためにMapする ----- ///
	uint32_t* mappingData = nullptr;
	indexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappingData));
	std::memcpy(mappingData, indices_.data(), sizeof(uint32_t) * indices_.size());
}

void Mesh::MemcpyVertexData() {
	/// ----- 頂点データを再度コピーする ※一度VertexBufferMapping()でMapしないと成功しない　----- ///
	if (mappingVertexData_) {
		std::memcpy(mappingVertexData_, vertices_.data(), sizeof(VertexData) * vertices_.size());
	}
}


void Mesh::SetVertices(const std::vector<VertexData>& _vertices) {
	vertices_ = _vertices;
}

void Mesh::SetIndices(const std::vector<uint32_t>& _indices) {
	indices_ = _indices;
}

const std::vector<Mesh::VertexData>& Mesh::GetVertices() const {
	return vertices_;
}

const std::vector<uint32_t>& Mesh::GetIndices() const {
	return indices_;
}

const D3D12_VERTEX_BUFFER_VIEW& Mesh::GetVBV() const {
	return vbv_;
}

const D3D12_INDEX_BUFFER_VIEW& Mesh::GetIBV() const {
	return ibv_;
}