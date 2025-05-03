#include "Octree.h"

/// engine
#include "Engine/ECS/Component/Component.h"


bool AABB::Contains(const Vector3& _point) const {
	return (_point.x >= center.x - halfSize.x && _point.x <= center.x + halfSize.x
		&& _point.y >= center.y - halfSize.y && _point.y <= center.y + halfSize.y
		&& _point.z >= center.z - halfSize.z && _point.z <= center.z + halfSize.z);
}

bool AABB::Intersects(const AABB& _range) const {
	return (std::abs(center.x - _range.center.x) <= (halfSize.x + _range.halfSize.x)
		&& std::abs(center.y - _range.center.y) <= (halfSize.y + _range.halfSize.y)
		&& std::abs(center.z - _range.center.z) <= (halfSize.z + _range.halfSize.z));
}

TerrainOctree::TerrainOctree(const AABB& _aabb)
	: boundary_(_aabb) {

}

bool TerrainOctree::Insert(Mesh::VertexData* _vertex) {
	if (!boundary_.Contains(Vector3(_vertex->position.x, _vertex->position.y, _vertex->position.z))) {
		return false;
	}

	if (terrainVertices_.size() < CAPACITY) {
		terrainVertices_.push_back(_vertex);
		return true;
	}

	if (!divided_) {
		Subdivide();
	}

	for (auto& child : children_) {
		if (child->Insert(_vertex)) return true;
	}

	return false; // どこにも入らなかった（めったに起きない）
}

void TerrainOctree::Subdivide() {
	Vector3 c = boundary_.center;
	Vector3 h = boundary_.halfSize * 0.5f;

	for (int i = 0; i < 8; ++i) {
		Vector3 offset = {
			(i & 1 ? h.x : -h.x),
			(i & 2 ? h.y : -h.y),
			(i & 4 ? h.z : -h.z)
		};
		children_[i] = std::make_unique<TerrainOctree>(AABB{ c + offset, h });
	}

	divided_ = true;
}

void TerrainOctree::Query(const AABB& _range, std::vector<Mesh::VertexData*>& _vertices) const {
	if (!boundary_.Intersects(_range)) {
		return;
	}

	for (const auto& vertex : terrainVertices_) {
		if (_range.Contains(Vector3(vertex->position.x, vertex->position.y, vertex->position.z))) {
			_vertices.push_back(vertex);
		}
	}

	if (divided_) {
		for (const auto& child : children_) {
			child->Query(_range, _vertices);
		}
	}
}

void TerrainOctree::QuerySphere(const Vector3& _center, float _radius, std::vector<Mesh::VertexData*>* _result) {

	/// AABBの範囲外なら何もしない
	if (!CollisionCheck::CubeVsSphere(
		boundary_.center, boundary_.halfSize * 2.0f,
		_center, _radius)) {
		return;
	}

	/// AABBの範囲内にある頂点を取得
	for (const auto& vertex : terrainVertices_) {
		if (CollisionCheck::SphereVsSphere(
			_center, _radius,
			Vector3(vertex->position.x, vertex->position.y, vertex->position.z), 0.5f)) {
			_result->push_back(vertex);
		}
	}

	if (divided_) {
		for (const auto& child : children_) {
			child->QuerySphere(_center, _radius, _result);
		}
	}
}
