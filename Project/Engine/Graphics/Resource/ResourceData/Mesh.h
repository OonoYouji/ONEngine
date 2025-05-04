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


/// ===================================================
/// graphics resource の mesh data
/// ===================================================
class Mesh final {
public:

	/// @brief 頂点データ
	struct VertexData final {
		Vec4 position;
		Vec2 uv;
		Vec3 normal;
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Mesh();
	~Mesh();
	/*Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&&) = delete;*/

	/// @brief vertex buffer, index bufferの作成
	/// @param _dxDevice DxDeviceクラスへのポインタ
	void CreateBuffer(class DxDevice* _dxDevice);

	/// @brief vertex bufferをマッピング
	void VertexBufferMapping();

	/// @brief index bufferをマッピング
	void IndexBufferMapping();

	void MemcpyVertexData();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<VertexData>  vertices_;
	DxResource               vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbv_;
	VertexData* mappingVertexData_ = nullptr;

	std::vector<uint32_t>    indices_;
	DxResource               indexBuffer_;
	D3D12_INDEX_BUFFER_VIEW  ibv_;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief vertices_をセット
	/// @param _vertices 頂点データ配列
	void SetVertices(const std::vector<VertexData>& _vertices);

	/// @brief 頂点のインデックスをセット
	/// @param _indices 頂点インデックス配列
	void SetIndices(const std::vector<uint32_t>& _indices);



	/// @brief 頂点データを取得
	/// @return 頂点データの配列
	const std::vector<VertexData>& GetVertices() const {
		return vertices_;
	}

	/// @brief 頂点インデックスを取得
	/// @return 頂点インデックスの配列
	const std::vector<uint32_t>& GetIndices() const {
		return indices_;
	}

	/// @brief vertex buffer viewを取得
	/// @return vbv date
	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const {
		return vbv_;
	}

	/// @brief index buffer viewを取得
	/// @return ibv date
	const D3D12_INDEX_BUFFER_VIEW& GetIBV() const {
		return ibv_;
	}

};