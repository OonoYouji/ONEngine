#pragma once

/// directX
#include <d3d12.h>

/// std
#include <cstdint>
#include <vector>

/// engine
#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "Engine/Core/Utility/Math/Vector4.h"
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector2.h"


namespace ONEngine {

class DxDevice;

/// ///////////////////////////////////////////////////
/// graphics resource の mesh data
/// ///////////////////////////////////////////////////
/**
 * @class Mesh
 * @brief 3Dグラフィックス描画のための頂点バッファとインデックスバッファを管理するクラステンプレート
 * @tparam Vertex 頂点レイアウト用の構造体型
 */
template<typename Vertex>
class Mesh final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief デフォルトコンストラクタ
	 */
	Mesh() = default;

	/**
	 * @brief デストラクタ
	 */
	~Mesh() = default;

	/**
	 * @brief 頂点バッファおよびインデックスバッファをGPU上に作成し、データをマッピングします。
	 * @param _dxDevice DxDeviceオブジェクトへのポインタ
	 */
	void CreateBuffer(DxDevice* _dxDevice);

	/**
	 * @brief 頂点データをGPUバッファにマッピング・コピーします。
	 */
	void VertexBufferMapping();

	/**
	 * @brief インデックスデータをGPUバッファにマッピング・コピーします。
	 */
	void IndexBufferMapping();

	/**
	 * @brief CPU側の最新頂点データをGPUバッファへ再度コピー（転送）します。
	 */
	void MemcpyVertexData();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<Vertex>      vertices_;          ///< 頂点データのリスト
	DxResource               vertexBuffer_;      ///< 頂点バッファリソース
	D3D12_VERTEX_BUFFER_VIEW vbv_;               ///< 頂点バッファビュー
	Vertex*                  mappingVertexData_; ///< マッピングされた頂点データのCPU側アドレス

	std::vector<uint32_t>    indices_;           ///< 頂点インデックスのリスト
	DxResource               indexBuffer_;       ///< インデックスバッファリソース
	D3D12_INDEX_BUFFER_VIEW  ibv_;               ///< インデックスバッファビュー


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/**
	 * @brief 頂点データ配列を設定します。
	 * @param _vertices 設定する頂点データリスト
	 */
	void SetVertices(const std::vector<Vertex>& _vertices);

	/**
	 * @brief 頂点インデックス配列を設定します。
	 * @param _indices 設定するインデックスリスト
	 */
	void SetIndices(const std::vector<uint32_t>& _indices);


	/**
	 * @brief 頂点データ配列を取得します。
	 * @return 頂点データのリスト
	 */
	const std::vector<Vertex>& GetVertices() const;

	/**
	 * @brief 頂点インデックス配列を取得します。
	 * @return インデックスのリスト
	 */
	const std::vector<uint32_t>& GetIndices() const;

	/**
	 * @brief 頂点バッファビューを取得します。
	 * @return D3D12_VERTEX_BUFFER_VIEW の参照
	 */
	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const;

	/**
	 * @brief インデックスバッファビューを取得します。
	 * @return D3D12_INDEX_BUFFER_VIEW の参照
	 */
	const D3D12_INDEX_BUFFER_VIEW& GetIBV() const;

};



template<typename Vertex>
void Mesh<Vertex>::CreateBuffer(DxDevice* _dxDevice) {
	/// ----- vbv, ibv のBufferを生成、Mapする ----- ///

	const size_t kVertexDataSize = sizeof(Vertex);

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

template<typename Vertex>
void Mesh<Vertex>::VertexBufferMapping() {
	/// ----- 頂点データをGPUで使用するためにMapする ----- ///
	vertexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappingVertexData_));
	std::memcpy(mappingVertexData_, vertices_.data(), sizeof(Vertex) * vertices_.size());
}

template<typename Vertex>
void Mesh<Vertex>::IndexBufferMapping() {
	/// ----- インデックスデータをGPUで使用するためにMapする ----- ///
	uint32_t* mappingData = nullptr;
	indexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappingData));
	std::memcpy(mappingData, indices_.data(), sizeof(uint32_t) * indices_.size());
}

template<typename Vertex>
void Mesh<Vertex>::MemcpyVertexData() {
	/// ----- 頂点データを再度コピーする ※一度VertexBufferMapping()でMapしないと成功しない　----- ///
	if(mappingVertexData_) {
		std::memcpy(mappingVertexData_, vertices_.data(), sizeof(Vertex) * vertices_.size());
	}
}


template<typename Vertex>
void Mesh<Vertex>::SetVertices(const std::vector<Vertex>& _vertices) {
	vertices_ = _vertices;
}

template<typename Vertex>
void Mesh<Vertex>::SetIndices(const std::vector<uint32_t>& _indices) {
	indices_ = _indices;
}

template<typename Vertex>
const std::vector<Vertex>& Mesh<Vertex>::GetVertices() const {
	return vertices_;
}

template<typename Vertex>
const std::vector<uint32_t>& Mesh<Vertex>::GetIndices() const {
	return indices_;
}

template<typename Vertex>
const D3D12_VERTEX_BUFFER_VIEW& Mesh<Vertex>::GetVBV() const {
	return vbv_;
}

template<typename Vertex>
const D3D12_INDEX_BUFFER_VIEW& Mesh<Vertex>::GetIBV() const {
	return ibv_;
}


} /// ONEngine
