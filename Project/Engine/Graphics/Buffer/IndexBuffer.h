#pragma once

/// std
#include <vector>

/// engine
#include "Engine/Core/DirectX12/Resource/DxResource.h"


/// ///////////////////////////////////////////////////
/// index buffer view
/// ///////////////////////////////////////////////////
class IndexBuffer final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	IndexBuffer();
	~IndexBuffer();

	/// @brief Bufferを作成する
	/// @param _indicesSize Indexの数
	/// @param _dxDevice DxDeviceへのポインタ
	void Create(size_t _indicesSize, class DxDevice* _dxDevice);

	/// @brief Indices用のメモリを確保する
	/// @param _value サイズ
	void Reserve(size_t _value);

	/// @brief Indices用のサイズを変更する
	/// @param _value サイズ
	void Resize(size_t _value);


	/// @brief コマンドリストにバインドする
	/// @param _commandList バインド対象のコマンドリスト
	void BindForCommandList(ID3D12GraphicsCommandList* _commandList);


	/// @brief GPU用にマッピングする
	void Map();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	DxResource resource_;
	std::vector<uint32_t> indices_;
	D3D12_INDEX_BUFFER_VIEW ibv_;


public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	const std::vector<uint32_t>& GetIndices() const;

	void SetIndex(size_t _indexIndex, uint32_t _indexValue);
	void SetIndices(const std::vector<uint32_t>& _indices);

};

