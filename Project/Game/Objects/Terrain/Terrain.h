#pragma once

/// std
#include <vector>
#include <span>

/// externals
#include <meshoptimizer/src/meshoptimizer.h>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Core/Utility/Math/Vector4.h"
#include "Engine/Graphics/Resource/ResourceData/Mesh.h"

#include "Chunk/TerrainChunk.h"
#include "Octree/Octree.h"

/// ///////////////////////////////////////////////////
/// 地形のオブジェクトクラス
/// ///////////////////////////////////////////////////
class Terrain : public IEntity {
	friend class TerrainEditor;
public:

	struct Triangle {
		uint32_t i0 : 10;
		uint32_t i1 : 10;
		uint32_t i2 : 10;
		uint32_t padding : 2; ///< パディング
	};

	struct Meshlet {
		meshopt_Meshlet meshlet;
		std::vector<Triangle> triangles;
		/*Vec3 boundingSphereCenter;
		float boundingSphereRadius;*/
	};


public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Terrain();
	~Terrain();

	void Initialize() override;
	void Update()     override;

	bool Collision(Transform* _transform, class ToTerrainCollider* _toTerrainCollider); ///< 当たり判定を取る

private:
	/// ===================================================
	/// private : methods
	/// ===================================================
	
	void InputVertices();

	void CalculateMeshlet(); ///< 頂点の計算

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/* ----- terrain ----- */
	std::vector<Mesh::VertexData> vertices_; ///< 頂点データ
	std::vector<uint32_t> indices_; ///< インデックスデータ
	std::vector<Meshlet> meshlets_;

	std::span<std::span<Mesh::VertexData>> vertexSpan_; ///< 頂点データのスパン

	Vector2 terrainSize_ = Vector2(1000.0f, 1000.0f); ///< 地形のサイズ


	/* ----- Octree ----- */

	std::unique_ptr<TerrainQuadTree> octree_; ///< Octreeのポインタ


	/* ----- edit ----- */
	std::vector<std::pair<size_t, Mesh::VertexData*>> editVertices_;

public:
	/// ===================================================
	/// public : accessor
	/// ====================================================

	/// @brief 頂点を二次元配列化したものを取得する
	/// @return 二次元配列にした頂点データのスパン
	const std::span<std::span<Mesh::VertexData>>& GetVertexSpan() const { return vertexSpan_; }

	const std::vector<Mesh::VertexData>& GetVertices() const { return vertices_; } ///< 頂点データ

	std::vector<Mesh::VertexData>& GetVertices() { return vertices_; } ///< 頂点データ

	const std::vector<uint32_t>& GetIndices() const { return indices_; } ///< インデックスデータ

	const std::vector<Meshlet>& GetMeshlets() const { return meshlets_; } ///< メッシュレットデータ

	const std::vector<std::pair<size_t, Mesh::VertexData*>>& GetEditVertices();


	TerrainQuadTree* GetOctree() { return octree_.get(); }
};

