#pragma once

/// std
#include <vector>

/// engine
#include "Engine/Core/DirectX12/Resource/DxResource.h"


/// ///////////////////////////////////////////////////
/// index buffer view
/// ///////////////////////////////////////////////////
namespace ONEngine {

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
	void Create(size_t _vertexSize, class DxDevice* _dxDevice, DxCommand* _dxCommand);


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

	void CopyFromAppendBuffer(ID3D12GraphicsCommandList* _cmdList, ID3D12Resource* _appendBuffer, uint32_t _vertexCount);
	void CopyFromUAVBuffer(ID3D12GraphicsCommandList* _cmdList, ID3D12Resource* _uavBuffer, uint32_t _vertexCount);

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
inline void VertexBuffer<T>::Create(size_t _vertexSize, DxDevice* _dxDevice, DxCommand* _dxCommand) {
	size_t tSize = sizeof(T);

	Resize(_vertexSize);
	/// vertex buffer
	resource_.CreateDefaultHeap(_dxDevice, _dxCommand, tSize * vertices_.size(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

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
inline void VertexBuffer<T>::CopyFromAppendBuffer(ID3D12GraphicsCommandList* _cmdList, ID3D12Resource* _appendBuffer, uint32_t _vertexCount) {
	// リソースサイズチェック
	UINT64 copySize = static_cast<UINT64>(_vertexCount * sizeof(T));
	Assert(copySize <= vbv_.SizeInBytes, "VertexBuffer size is too small for AppendBuffer copy");

	// バリア: VBV -> COPY_DEST
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		resource_.Get(),
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	_cmdList->ResourceBarrier(1, &barrier);

	// Copy
	_cmdList->CopyBufferRegion(
		resource_.Get(), 0,       // Destination
		_appendBuffer, 0,          // Source
		copySize
	);

	// バリア戻す: COPY_DEST -> VBV
	std::swap(barrier.Transition.StateBefore, barrier.Transition.StateAfter);
	_cmdList->ResourceBarrier(1, &barrier);

	// VBV の SizeInBytes をコピー頂点数に更新
	vbv_.SizeInBytes = static_cast<UINT>(copySize);
}

template<typename T>
inline void VertexBuffer<T>::CopyFromUAVBuffer(ID3D12GraphicsCommandList* _cmdList, ID3D12Resource* _uavBuffer, uint32_t _vertexCount) {
	// リソースサイズチェック
	UINT64 copySize = static_cast<UINT64>(_vertexCount * sizeof(T));
	Assert(copySize <= vbv_.SizeInBytes, "VertexBuffer size is too small for UAVBuffer copy");

	// バリア: UAV -> COPY_SOURCE, VBV -> COPY_DEST
	D3D12_RESOURCE_BARRIER barriers[2] = {
		CD3DX12_RESOURCE_BARRIER::Transition(
			_uavBuffer,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_COPY_SOURCE
		),
			CD3DX12_RESOURCE_BARRIER::Transition(
				resource_.Get(),
				D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
				D3D12_RESOURCE_STATE_COPY_DEST
			)
	};
	_cmdList->ResourceBarrier(2, barriers);

	// Copy
	_cmdList->CopyBufferRegion(
		resource_.Get(), 0,       // Destination
		_uavBuffer, 0,            // Source
		copySize
	);

	// バリア戻す: COPY_SOURCE -> UAV, COPY_DEST -> VBV
	std::swap(barriers[0].Transition.StateBefore, barriers[0].Transition.StateAfter);
	std::swap(barriers[1].Transition.StateBefore, barriers[1].Transition.StateAfter);
	_cmdList->ResourceBarrier(2, barriers);

	// VBV の SizeInBytes をコピー頂点数に更新
	vbv_.SizeInBytes = static_cast<UINT>(copySize);
}

template<typename T>
inline const std::vector<T>& VertexBuffer<T>::GetVertices() const {
	return vertices_;
}

template<typename T>
inline void VertexBuffer<T>::SetVertex(size_t _index, const T& _vertex) {
	if(mappingData_) {
		mappingData_[_index] = _vertex;
	}
	vertices_[_index] = _vertex;
}

template<typename T>
inline void VertexBuffer<T>::SetVertices(const std::vector<T>& _vertices) {
	vertices_ = _vertices;
}

} /// ONEngine
