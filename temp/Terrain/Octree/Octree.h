#pragma once

/// std
#include <memory>
#include <array>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Resource/ResourceData/Mesh.h"

#include "../TerrainVertex.h"

/// ////////////////////////////////////////////////////////////
/// AABB
/// ////////////////////////////////////////////////////////////
struct AABB final {
	Vector3 center;
	Vector3 halfSize;

	/// @brief _pointがAABBの範囲内にあるかどうかを判定する
	/// @param _point ワールド座標の点
	/// @return true: _pointがAABBの範囲内にある false: _pointがAABBの範囲外
	bool Contains(const Vector3& _point) const;

	/// @brief _rangeがAABBの範囲と交差しているかどうかを判定する
	/// @param _range 他のAABB
	/// @return true: _rangeがAABBの範囲と交差している false: _rangeがAABBの範囲外
	bool Intersects(const AABB& _range) const;

};


struct Rect3D {
	float x;
	float z;
	float width;
	float depth;

	bool Contains(const Vector3& _point) const {
		return (_point.x >= x && _point.x <= x + width
			&& _point.z >= z && _point.z <= z + depth);
	}

	bool Intersects(const Rect3D& _range) const {
		return (std::abs(x - _range.x) <= (width + _range.width)
			&& std::abs(z - _range.z) <= (depth + _range.depth));
	}

};


/// ////////////////////////////////////////////////////////////
/// QuadTree
/// ////////////////////////////////////////////////////////////
class TerrainQuadTree final {
public:

	enum {
		CAPACITY = 126, ///< ノードの最大数
	};

public:
	/// ====================================================
	/// public : methods
	/// ====================================================

	TerrainQuadTree(const AABB& _aabb);

	bool Insert(size_t _index, TerrainVertex* _vertex);

	void Subdivide();

	void Query(const AABB& _range, std::vector<std::pair<size_t, TerrainVertex*>>& _vertices) const;

	/// @brief terrainVertices_の中から、指定した範囲内にある頂点を取得する
	/// @param _center sphereの中心
	/// @param _radius sphereの半径
	/// @param _result 衝突している頂点のポインタを格納する配列
	void QuerySphere(const Vector3& _center, float _radius, std::vector<std::pair<size_t, TerrainVertex*>>* _result);

	/// @brief terrainVertices_の中から、指定したカプセルに衝突している頂点を取得する
	/// @param _start カプセルの始点
	/// @param _end カプセルの終点
	/// @param _radius カプセルの半径
	/// @param _result 衝突している頂点のポインタを格納する配列
	void QueryCapsule(const Vector3& _start, const Vector3& _end, float _radius, std::vector<std::pair<size_t, TerrainVertex*>>* _result);

	void Draw(TerrainQuadTree* _octree, const Vector4& _color, size_t _depth = 1) const;

private:
	/// ====================================================
	/// private : objects
	/// ====================================================

	std::unique_ptr<AABB> boundary_;        ///< 範囲
	bool divided_ = false; ///< 分割されているかどうか
	std::vector<std::pair<size_t, TerrainVertex*>> terrainVertices_;



	std::array<std::unique_ptr<TerrainQuadTree>, 4> children_; ///< 子ノード


};

