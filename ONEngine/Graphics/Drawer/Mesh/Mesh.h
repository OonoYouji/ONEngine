#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <cmath>
#include <vector>

#include <Vector4.h>
#include <Vector3.h>
#include <Vector2.h>


using namespace Microsoft::WRL;

/// ===================================================
/// メッシュ
/// ===================================================
class Mesh final {
public:

	/// ===================================================
	/// public : sub class
	/// ===================================================

	struct VertexData final {
		Vec4 position;
		Vec2 texcoord;
		bool operator==(const VertexData&) const = default;
	};

public:

	Mesh();
	~Mesh();


	/// ===================================================
	/// public : methods
	/// ===================================================


	/// <summary>
	/// 頂点データの追加
	/// </summary>
	/// <param name="vertex"> : Mesh::VertexData構造体</param>
	void AddVertex(const VertexData& vertex);

	/// <summary>
	/// インデックスデータの追加
	/// </summary>
	/// <param name="index"> : uint32_t</param>
	void AddIndex(uint32_t index);

	/// <summary>
	/// バッファの作成
	/// </summary>
	void Create();

	/// <summary>
	/// commandListにVBVとIBVを積む、描画フラグがtrueであればDrawCallを呼び出す
	/// </summary>
	/// <param name="commandList"> : コマンドリスト</param>
	/// <param name="isDrawCall"> : 描画フラグ</param>
	void Draw(ID3D12GraphicsCommandList* commandList, bool isDrawCall);

	/// <summary>
	/// DrawCallの呼び出し
	/// </summary>
	/// <param name="commandList"></param>
	void DrawCall(ID3D12GraphicsCommandList* commandList);

private:

	std::vector<VertexData> vertices_;
	std::vector<uint32_t> indices_;

	ComPtr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbv_;

	ComPtr<ID3D12Resource> indexBuffer_;
	D3D12_INDEX_BUFFER_VIEW ibv_;

};