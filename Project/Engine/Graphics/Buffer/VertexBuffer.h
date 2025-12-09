#pragma once

/// std
#include <vector>

/// engine
#include "Engine/Core/DirectX12/Resource/DxResource.h"


/// ///////////////////////////////////////////////////
/// index buffer view
/// ///////////////////////////////////////////////////
template<typename T>
class VertexBuffer final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	VertexBuffer() = default;
	~VertexBuffer() = default;

	/// @brief Bufferの作成
	/// @param _vertexSize 頂点の数
	/// @param _dxDevice DxDeviceのポインタ
	void Create(size_t _vertexSize, class DxDevice* _dxDevice);


	/// @brief vertices_のメモリ確保
	/// @param _value サイズ
	void Reserve(size_t _value);

	/// @brief vertices_のリサイズ
	/// @param _value サイズ
	void Resize(size_t _value);


	/// @brief コマンドリストにバインドする
	/// @param _commandList 
	void BindForCommandList(ID3D12GraphicsCommandList* _commandList);


	/// @brief GPUにマッピングする
	void Map();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	DxResource resource_;
	std::vector<T> vertices_;
	D3D12_VERTEX_BUFFER_VIEW vbv_;
	T* mappingData_ = nullptr;

public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	const std::vector<T>& GetVertices() const;

	void SetVertex(size_t _index, const T& _vertex);
	void SetVertices(const std::vector<T>& _vertices);

};



template<typename T>
inline void VertexBuffer<T>::Create(size_t _vertexSize, DxDevice* _dxDevice) {
	size_t tSize = sizeof(T);

	Resize(_vertexSize);
	/// vertex buffer
	resource_.CreateResource(_dxDevice, tSize * vertices_.size());

	vbv_.BufferLocation = resource_.Get()->GetGPUVirtualAddress();
	vbv_.SizeInBytes = static_cast<UINT>(tSize * vertices_.size());
	vbv_.StrideInBytes = static_cast<UINT>(tSize);
}

template<typename T>
inline void VertexBuffer<T>::Reserve(size_t _value) {
	vertices_.reserve(_value);
}

template<typename T>
inline void VertexBuffer<T>::Resize(size_t _value) {
	vertices_.resize(_value);
}

template<typename T>
inline void VertexBuffer<T>::BindForCommandList(ID3D12GraphicsCommandList* _commandList) {
	_commandList->IASetVertexBuffers(0, 1, &vbv_);
}

template<typename T>
inline void VertexBuffer<T>::Map() {
	resource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappingData_));
	std::memcpy(mappingData_, vertices_.data(), sizeof(T) * vertices_.size());
}

template<typename T>
inline const std::vector<T>& VertexBuffer<T>::GetVertices() const {
	return vertices_;
}

template<typename T>
inline void VertexBuffer<T>::SetVertex(size_t _index, const T& _vertex) {
	if (mappingData_) {
		mappingData_[_index] = _vertex;
	}
	vertices_[_index] = _vertex;
}

template<typename T>
inline void VertexBuffer<T>::SetVertices(const std::vector<T>& _vertices) {
	vertices_ = _vertices;
}
