#include "IndexBuffer.h"

IndexBuffer::IndexBuffer() {}
IndexBuffer::~IndexBuffer() {}

void IndexBuffer::Create(size_t _indicesSize, DxDevice* _dxDevice) {
	/// ----- IndexBufferの作成 ----- ///

	Resize(_indicesSize);
	resource_.CreateResource(_dxDevice, sizeof(uint32_t) * indices_.size());

	ibv_.BufferLocation = resource_.Get()->GetGPUVirtualAddress();
	ibv_.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * indices_.size());
	ibv_.Format = DXGI_FORMAT_R32_UINT;
}

void IndexBuffer::Reserve(size_t _value) {
	indices_.reserve(_value);
}

void IndexBuffer::Resize(size_t _value) {
	indices_.resize(_value);
}

void IndexBuffer::BindForCommandList(ID3D12GraphicsCommandList* _commandList) {
	_commandList->IASetIndexBuffer(&ibv_);
}

void IndexBuffer::Map() {
	/// ----- インデックスデータをGPU用にマッピング ----- ///
	uint32_t* map = nullptr;
	resource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&map));
	std::memcpy(map, indices_.data(), sizeof(uint32_t) * indices_.size());
}

const std::vector<uint32_t>& IndexBuffer::GetIndices() const {
	return indices_;
}

void IndexBuffer::SetIndex(size_t _indexIndex, uint32_t _indexValue) {
	indices_[_indexIndex] = _indexValue;
}

void IndexBuffer::SetIndices(const std::vector<uint32_t>& _indices) {
	indices_ = _indices;
}
