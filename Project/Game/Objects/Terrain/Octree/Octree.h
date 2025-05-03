#pragma once

/// std
#include <memory>
#include <array>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Resource/ResourceData/Mesh.h"

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


/// ////////////////////////////////////////////////////////////
/// QuadTree
/// ////////////////////////////////////////////////////////////
class TerrainOctree final {
public:

	enum {
		CAPACITY = 100, ///< ノードの最大数
	};

public:
	/// ====================================================
	/// public : methods
	/// ====================================================

	TerrainOctree(const AABB& _aabb);

	bool Insert(Mesh::VertexData* _vertex);

	void Subdivide();

	void Query(const AABB& _range, std::vector<Mesh::VertexData*>& _vertices) const;

	/// @brief terrainVertices_の中から、指定した範囲内にある頂点を取得する
	/// @param _center sphereの中心
	/// @param _radius sphereの半径
	/// @param _result 衝突している頂点のポインタを格納する配列
	void QuerySphere(const Vector3& _center, float _radius, std::vector<Mesh::VertexData*>* _result);

private:
	/// ====================================================
	/// private : objects
	/// ====================================================

	AABB boundary_;        ///< 範囲
	bool divided_ = false; ///< 分割されているかどうか
	std::vector<Mesh::VertexData*> terrainVertices_;

	std::array<std::unique_ptr<TerrainOctree>, 8> children_; ///< 子ノード


};

