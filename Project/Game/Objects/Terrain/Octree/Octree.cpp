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

TerrainQuadTree::TerrainQuadTree(const AABB& _aabb)
	/*: boundary_(_aabb) */{
	boundary_.reset(new AABB(_aabb));
}

bool TerrainQuadTree::Insert(size_t _index, Mesh::VertexData* _vertex) {
	if (!boundary_->Contains(Vector3(_vertex->position.x, _vertex->position.y, _vertex->position.z))) {
		return false;
	}

	if (terrainVertices_.size() < CAPACITY) {
		terrainVertices_.push_back(std::make_pair(_index, _vertex));
		return true;
	}

	if (!divided_) {
		Subdivide();
	}

	for (auto& child : children_) {
		if (child->Insert(_index, _vertex)) return true;
	}

	return false; // どこにも入らなかった（めったに起きない）
}

void TerrainQuadTree::Subdivide() {
	Vector3 c = boundary_->center;
	Vector3 h = boundary_->halfSize * 0.5f;

	// インデックス: 0 = NW, 1 = NE, 2 = SW, 3 = SE（XZ方向）
	for (int i = 0; i < 4; ++i) {
		float offsetX = (i % 2 == 0) ? -h.x : h.x;
		float offsetZ = (i < 2) ? -h.z : h.z;

		Vector3 offset = { offsetX, 0.0f, offsetZ }; // Yは固定（高さの分割なし）

		// Y方向の中心とサイズはそのまま
		Vector3 childCenter = c + offset;
		Vector3 childHalfSize = { h.x, boundary_->halfSize.y, h.z }; // Yは分割しない

		children_[i] = std::make_unique<TerrainQuadTree>(AABB{ childCenter, childHalfSize });
	}

	divided_ = true;
}


void TerrainQuadTree::Query(const AABB& _range, std::vector<std::pair<size_t, Mesh::VertexData*>>& _vertices) const {
	if (!boundary_->Intersects(_range)) {
		return;
	}

	for (const auto& vertex : terrainVertices_) {
		const Vector4& v = vertex.second->position;
		if (_range.Contains(Vector3(v.x, v.y, v.z))) {
			_vertices.push_back(vertex);
		}
	}

	if (divided_) {
		for (const auto& child : children_) {
			child->Query(_range, _vertices);
		}
	}
}

void TerrainQuadTree::QuerySphere(const Vector3& _center, float _radius, std::vector<std::pair<size_t, Mesh::VertexData*>>* _result) {

	/// AABBの範囲外なら何もしない
	if (!CollisionCheck::CubeVsSphere(
		boundary_->center, boundary_->halfSize * 2.0f,
		_center, _radius)) {
		return;
	}

	/// AABBの範囲内にある頂点を取得
	for (const auto& vertex : terrainVertices_) {
		const Vector4& v = vertex.second->position;
		if (CollisionCheck::SphereVsSphere(
			_center, _radius,
			Vector3(v.x, v.y, v.z), 0.5f)) {

			_result->push_back(vertex);

			Gizmo::DrawWireSphere(
				Vector3(v.x, v.y, v.z), 0.5f,
				Color::kGreen
			);
		}
	}

	if (divided_) {
		for (const auto& child : children_) {
			child->QuerySphere(_center, _radius, _result);
		}
	}
}

void TerrainQuadTree::Draw(TerrainQuadTree* _octree, const Vector4& _color, size_t _depth) const {

	//if (_depth > 5) return; // 深さ制限

	if (!this || !boundary_) return;

	std::array<Color, 5> color{
		Color(1.0f, 0.0f, 0.0f, 1.0f),
		Color(0.0f, 1.0f, 0.0f, 1.0f),
		Color(0.0f, 0.0f, 1.0f, 1.0f),
		Color(1.0f, 1.0f, 0.0f, 1.0f),
		Color(1.0f, 1.0f, 1.0f, 1.0f)
	};

	//Gizmo::DrawWireCube(
	//	boundary_->center,
	//	boundary_->halfSize * 2.0f,
	//	color[_depth % color.size()]
	//);

	for (auto& child : _octree->children_) {
		child->Draw(child.get(), _color + Color(0.1f, 0.1f, 0.1f, 0.0f), _depth + 1);
	}
}
