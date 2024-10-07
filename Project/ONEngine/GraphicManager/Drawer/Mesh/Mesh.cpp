#include "Mesh.h"

#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"


Mesh::Mesh() {}
Mesh::~Mesh() {}


/// ===================================================
/// 頂点データの追加
/// ===================================================
void Mesh::AddVertex(const VertexData& vertex) {
	vertices_.push_back(vertex);
}


/// ===================================================
/// インデックスデータの追加
/// ===================================================
void Mesh::AddIndex(uint32_t index) {
	indices_.push_back(index);
}


/// ===================================================
/// バッファの作成
/// ===================================================
void Mesh::CreateBuffer() {

	vertexBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(VertexData) * vertices_.size());

	VertexData* vertexData = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices_.data(), sizeof(VertexData) * vertices_.size());

	vbv_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbv_.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * vertices_.size());
	vbv_.StrideInBytes = static_cast<UINT>(sizeof(VertexData));



	indexBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(uint32_t) * indices_.size());

	uint32_t* indexData = nullptr;
	indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	std::memcpy(indexData, indices_.data(), sizeof(uint32_t) * indices_.size());

	ibv_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	ibv_.Format = DXGI_FORMAT_R32_UINT;
	ibv_.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * indices_.size());

}



void Mesh::Draw(ID3D12GraphicsCommandList* commandList, bool isDrawCall) {
	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->IASetIndexBuffer(&ibv_);

	if(!isDrawCall) { return; }

	commandList->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);
}



void Mesh::DrawCall(ID3D12GraphicsCommandList* commandList) {
	commandList->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);
}
