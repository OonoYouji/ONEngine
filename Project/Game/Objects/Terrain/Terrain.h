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
#include "TerrainVertex.h"

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

	enum SPLAT_TEX {
		GRASS,
		DIRT,
		ROCK,
		SNOW,
		SPLAT_TEX_COUNT
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

	void ClearEditVertices();

	void InputVertices();


private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	void CalculateMeshlet(); ///< 頂点の計算

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/* ----- terrain ----- */
	std::vector<TerrainVertex> vertices_; ///< 頂点データ
	std::vector<uint32_t> indices_; ///< インデックスデータ
	std::vector<Meshlet> meshlets_;

	std::span<std::span<TerrainVertex>> vertexSpan_; ///< 頂点データのスパン

	Vector2 terrainSize_ = Vector2(1000.0f, 1000.0f); ///< 地形のサイズ


	/* ----- Octree ----- */

	std::unique_ptr<TerrainQuadTree> octree_; ///< Octreeのポインタ


	/* ----- edit ----- */
	std::vector<std::pair<size_t, TerrainVertex*>> editVertices_;



	/* ----- splatting ----- */

	std::array<std::string, SPLAT_TEX_COUNT> splattingTexPaths_;


public:
	/// ===================================================
	/// public : accessor
	/// ====================================================

	/// @brief 頂点を二次元配列化したものを取得する
	/// @return 二次元配列にした頂点データのスパン
	const std::span<std::span<TerrainVertex>>& GetVertexSpan() const;

	const std::vector<TerrainVertex>& GetVertices() const;
	std::vector<TerrainVertex>& GetVertices();
	const std::vector<uint32_t>& GetIndices() const;

	const std::vector<Meshlet>& GetMeshlets() const;

	const std::vector<std::pair<size_t, TerrainVertex*>>& GetEditVertices();

	const std::array<std::string, SPLAT_TEX_COUNT>& GetSplatTexPaths() const;

	TerrainQuadTree* GetQuadTree();
};

